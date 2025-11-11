// SPDX-License-Identifier: GPL-2.0+
/*
 *  I2C adapter driver for LRW
 *
 *  Copyright (c) 2025, LRW CORPORATION. All rights reserved.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/export.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/reset.h>
#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/regmap.h>
#include <linux/swab.h>
#include <linux/types.h>
#include <linux/units.h>
#include "i2c-lrw-core.h"

static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] = "slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] = "data not acknowledged",
	[ABRT_GCALL_NOACK] = "no acknowledgment for a general call",
	[ABRT_GCALL_READ] = "read after general call",
	[ABRT_SBYTE_ACKDET] = "start byte acknowledged",
	[ABRT_SBYTE_NORSTRT] =
		"trying to send start byte when restart is disabled",
	[ABRT_10B_RD_NORSTRT] =
		"trying to read when restart is disabled (10bit mode)",
	[ABRT_MASTER_DIS] = "trying to use disabled adapter",
	[ARB_LOST] = "lost arbitration",
	[ABRT_SLAVE_FLUSH_TXFIFO] =
		"read command so flush old data in the TX FIFO",
	[ABRT_SLAVE_ARBLOST] =
		"slave lost the bus while transmitting data to a remote master",
	[ABRT_SLAVE_RD_INTX] = "incorrect slave-transmitter mode configuration",
	[ABRT_SDA_STUCK_AT_LOW] = "SDA line stuck at low timeout",
};

static int lrw_reg_read(void *context, unsigned int reg, unsigned int *val)
{
	struct i2c_lrw_dev *dev = context;

	*val = readl_relaxed(dev->base + reg);

	return 0;
}

static int lrw_reg_write(void *context, unsigned int reg, unsigned int val)
{
	struct i2c_lrw_dev *dev = context;

	writel_relaxed(val, dev->base + reg);

	return 0;
}

static int i2c_lrw_acquire_lock(struct i2c_lrw_dev *dev)
{
	int ret;

	if (!dev->acquire_lock)
		return 0;

	ret = dev->acquire_lock();
	if (!ret)
		return 0;

	dev_err(dev->dev, "couldn't acquire bus ownership\n");

	return ret;
}

static void i2c_lrw_release_lock(struct i2c_lrw_dev *dev)
{
	if (dev->release_lock)
		dev->release_lock();
}

static int i2c_lrw_init_regmap(struct i2c_lrw_dev *dev)
{
	struct regmap_config map_cfg = {
		.reg_bits = 32,
		.val_bits = 32,
		.reg_stride = 4,
		.disable_locking = true,
		.reg_read = lrw_reg_read,
		.reg_write = lrw_reg_write,
		.max_register = LRW_SMBUS_INTR_STAT,
	};

	if (dev->map)
		return 0;

	dev->map = devm_regmap_init(dev->dev, NULL, dev, &map_cfg);
	if (IS_ERR(dev->map)) {
		dev_err(dev->dev, "Failed to init the registers map\n");
		return PTR_ERR(dev->map);
	}

	return 0;
}

static const u32 supported_speeds[] = {
	I2C_MAX_HIGH_SPEED_MODE_FREQ,
	I2C_MAX_FAST_MODE_PLUS_FREQ,
	I2C_MAX_FAST_MODE_FREQ,
	I2C_MAX_STANDARD_MODE_FREQ,
};

int i2c_lrw_validate_speed(struct i2c_lrw_dev *dev)
{
	struct i2c_timings *t = &dev->timings;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(supported_speeds); i++) {
		if (t->bus_freq_hz == supported_speeds[i])
			return 0;
	}

	dev_err(dev->dev,
		"%d Hz is unsupported, only 100kHz, 400kHz, 1MHz and 3.4MHz are supported\n",
		t->bus_freq_hz);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(i2c_lrw_validate_speed);

#ifdef CONFIG_ACPI

#include <linux/dmi.h>

static void i2c_lrw_acpi_params(struct device *device, char method[], u16 *hcnt,
				u16 *lcnt, u32 *sda_hold, u32 *sda_stuck_timout)
{
	struct acpi_buffer buf = { ACPI_ALLOCATE_BUFFER };
	acpi_handle handle = ACPI_HANDLE(device);
	union acpi_object *obj;

	if (ACPI_FAILURE(acpi_evaluate_object(handle, method, NULL, &buf)))
		return;

	obj = (union acpi_object *)buf.pointer;
	if (obj->type == ACPI_TYPE_PACKAGE && obj->package.count == 4) {
		const union acpi_object *objs = obj->package.elements;

		*hcnt = (u16)objs[0].integer.value;
		*lcnt = (u16)objs[1].integer.value;
		*sda_hold = (u32)objs[2].integer.value;
		*sda_stuck_timout = (u32)objs[3].integer.value;
	}
	kfree(buf.pointer);
}

int i2c_lrw_acpi_configure(struct device *device)
{
	struct i2c_lrw_dev *dev = dev_get_drvdata(device);
	struct i2c_timings *t = &dev->timings;
	u32 hs_ht = 0, fs_ht = 0;
	u32 sda_stuck_at_low_timeout;

	i2c_lrw_acpi_params(device, "HSCN", &dev->hs_hcnt, &dev->hs_lcnt,
				&hs_ht, &sda_stuck_at_low_timeout);
	i2c_lrw_acpi_params(device, "FMCN", &dev->fs_hcnt, &dev->fs_lcnt,
				&fs_ht, &sda_stuck_at_low_timeout);
	dev->sda_stuck_at_low_timeout = sda_stuck_at_low_timeout;

	switch (t->bus_freq_hz) {
	case I2C_MAX_STANDARD_MODE_FREQ:
	case I2C_MAX_FAST_MODE_FREQ:
	case I2C_MAX_FAST_MODE_PLUS_FREQ:
		dev->sda_hold_time = fs_ht;
		break;
	case I2C_MAX_HIGH_SPEED_MODE_FREQ:
		dev->sda_hold_time = hs_ht;
		break;
	default:
		dev_err(dev->dev,
			"%d Hz is unsupported, only 100kHz, 400kHz, 1MHz and 3.4MHz are supported\n",
			t->bus_freq_hz);
		break;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(i2c_lrw_acpi_configure);

static u32 i2c_lrw_acpi_round_bus_speed(struct device *device)
{
	u32 acpi_speed;
	int i;

	acpi_speed = i2c_acpi_find_bus_speed(device);

	for (i = 0; i < ARRAY_SIZE(supported_speeds); i++) {
		if (acpi_speed >= supported_speeds[i])
			return supported_speeds[i];
	}

	return 0;
}

#else /* CONFIG_ACPI */

static inline u32 i2c_lrw_acpi_round_bus_speed(struct device *device)
{
	return 0;
}

int i2c_lrw_dt_configure(struct device *device)
{
	struct i2c_lrw_dev *dev = dev_get_drvdata(device);
	struct device_node *node = device->of_node;
	u32 raw_property;

	if (node) {
		of_property_read_u32(node, "hs_hcnt", &raw_property);
		dev->hs_hcnt = (u16)raw_property;
		of_property_read_u32(node, "hs_lcnt", &raw_property);
		dev->hs_lcnt = (u16)raw_property;
		of_property_read_u32(node, "fs_hcnt", &raw_property);
		dev->fs_hcnt = (u16)raw_property;
		of_property_read_u32(node, "fs_lcnt", &raw_property);
		dev->fs_lcnt = (u16)raw_property;
		of_property_read_u32(node, "sda_hold_time",
					&dev->sda_hold_time);
		of_property_read_u32(node, "sda_stuck_at_low_timeout",
					&dev->sda_stuck_at_low_timeout);
	}
	dev_dbg(dev->dev, "dt dev param = %x, %x, %x, %x, %x, %x\n",
		dev->hs_hcnt, dev->hs_lcnt, dev->fs_hcnt, dev->fs_lcnt,
		dev->sda_hold_time, dev->sda_stuck_at_low_timeout);
	return 0;
}
EXPORT_SYMBOL_GPL(i2c_lrw_dt_configure);

#endif /* CONFIG_ACPI */

void i2c_lrw_adjust_bus_speed(struct i2c_lrw_dev *dev)
{
	u32 acpi_speed = i2c_lrw_acpi_round_bus_speed(dev->dev);
	struct i2c_timings *t = &dev->timings;

	if (acpi_speed && t->bus_freq_hz)
		t->bus_freq_hz = min(t->bus_freq_hz, acpi_speed);
	else if (acpi_speed || t->bus_freq_hz)
		t->bus_freq_hz = max(t->bus_freq_hz, acpi_speed);
	else
		t->bus_freq_hz = I2C_MAX_FAST_MODE_FREQ;
}
EXPORT_SYMBOL_GPL(i2c_lrw_adjust_bus_speed);

static u32 i2c_lrw_scl_hcnt(u32 ic_clk, u32 tSYMBOL, u32 tf, int cond,
				int offset)
{
	if (cond)
		return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * tSYMBOL, 1000000) -
			5 + offset;
	else
		return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * (tSYMBOL + tf),
			1000000) - 3 + offset;
}

static u32 i2c_lrw_scl_lcnt(u32 ic_clk, u32 tLOW, u32 tf, int offset)
{
	return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * (tLOW + tf), 1000000) - 1 +
		offset;
}

static int i2c_lrw_set_sda_hold(struct i2c_lrw_dev *dev)
{
	unsigned int reg;
	int ret;

	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		return ret;

	ret = regmap_read(dev->map, LRW_IC_VERSION, &reg);
	if (ret)
		goto err_release_lock;

	if (reg >= LRW_IC_SDA_HOLD_MIN_VERS) {
		if (!dev->sda_hold_time) {
			ret = regmap_read(dev->map, LRW_IC_SDA_HOLD,
					&dev->sda_hold_time);
			if (ret)
				goto err_release_lock;
		}

		if (!(dev->sda_hold_time & LRW_IC_SDA_HOLD_RX_MASK))
			dev->sda_hold_time |= 1 << LRW_IC_SDA_HOLD_RX_SHIFT;

		dev_dbg(dev->dev, "SDA Hold Time TX:RX = %d:%d\n",
			dev->sda_hold_time & ~(u32)LRW_IC_SDA_HOLD_RX_MASK,
			dev->sda_hold_time >> LRW_IC_SDA_HOLD_RX_SHIFT);
	} else if (dev->set_sda_hold_time) {
		dev->set_sda_hold_time(dev);
	} else if (dev->sda_hold_time) {
		dev_warn(dev->dev,
			 "Hardware too old to adjust SDA hold time.\n");
		dev->sda_hold_time = 0;
	}

err_release_lock:
	i2c_lrw_release_lock(dev);

	return ret;
}

void __i2c_lrw_disable(struct i2c_lrw_dev *dev)
{
	unsigned int raw_intr_stats;
	unsigned int enable;
	int timeout = 100;
	bool abort_needed;
	unsigned int status;
	int ret;

	regmap_read(dev->map, LRW_IC_CLR_RAW_INTR, &raw_intr_stats);
	regmap_read(dev->map, LRW_IC_ENABLE, &enable);

	abort_needed = raw_intr_stats & LRW_IC_INTR_MST_ON_HOLD;
	if (abort_needed) {
		regmap_write(dev->map, LRW_IC_ENABLE,
				enable | LRW_IC_ENABLE_ABORT);
		ret = regmap_read_poll_timeout(dev->map, LRW_IC_ENABLE, enable,
						!(enable & LRW_IC_ENABLE_ABORT),
						50, 200);
		if (ret)
			dev_err(dev->dev,
				"timeout while trying to abort current transfer\n");
	}

	do {
		__i2c_lrw_disable_nowait(dev);
		regmap_read(dev->map, LRW_IC_STATUS, &status);
		if ((status & 1) == 0)
			return;

		usleep_range(25, 250);
	} while (timeout--);

	dev_warn(dev->dev, "timeout in disabling adapter\n");
}

int i2c_lrw_bus_recover(struct i2c_adapter *adap)
{
	struct i2c_lrw_dev *dev = i2c_get_adapdata(adap);
	int timeout = 1000;
	u32 enabled;

	if (dev->abort_source & IC_ABRT_SDA_STUCK_AT_LOW_POS) {
		regmap_update_bits(dev->map, LRW_IC_ENABLE,
				IC_SDA_STUCK_RECOVERY_ENABLE_POS,
				IC_SDA_STUCK_RECOVERY_ENABLE_POS);
		do {
			regmap_read(dev->map, LRW_IC_ENABLE, &enabled);
			if ((enabled & IC_SDA_STUCK_RECOVERY_ENABLE_POS) ==
				IC_SDA_STUCK_AT_LOW_RECOVERIED)
				break;
			udelay(10);
		} while (--timeout);

		if (!timeout) {
			dev_err(dev->dev, "I2C bus recovery timeout\n");
			return -ETIMEDOUT;
		}

		dev_err(dev->dev, "I2C hardware recovery complete\n");
	}
	return 0;
}
EXPORT_SYMBOL_GPL(i2c_lrw_bus_recover);

static u32 i2c_lrw_clk_rate(struct i2c_lrw_dev *dev)
{
	if (WARN_ON_ONCE(!dev->get_clk_rate_khz))
		return 0;
	return dev->get_clk_rate_khz(dev);
}

int i2c_lrw_prepare_clk(struct i2c_lrw_dev *dev, bool prepare)
{
	int ret;

	if (prepare) {
		ret = clk_prepare_enable(dev->pclk);
		if (ret)
			return ret;

		ret = clk_prepare_enable(dev->clk);
		if (ret)
			clk_disable_unprepare(dev->pclk);

		return ret;
	}

	clk_disable_unprepare(dev->clk);
	clk_disable_unprepare(dev->pclk);

	return 0;
}
EXPORT_SYMBOL_GPL(i2c_lrw_prepare_clk);

static int i2c_lrw_wait_bus_not_busy(struct i2c_lrw_dev *dev)
{
	u32 status;
	int ret;

	ret = regmap_read_poll_timeout(dev->map, LRW_IC_STATUS, status,
					!(status & LRW_IC_STATUS_ACTIVITY), 1100,
					20000);
	if (ret) {
		dev_warn(dev->dev, "timeout waiting for bus ready\n");

		i2c_recover_bus(&dev->adapter);

		regmap_read(dev->map, LRW_IC_STATUS, &status);
		if (!(status & LRW_IC_STATUS_ACTIVITY))
			ret = 0;
	}

	return ret;
}

static int i2c_lrw_handle_tx_abort(struct i2c_lrw_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;

	if (abort_source & LRW_IC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
			dev_dbg(dev->dev, "%s: %s\n", __func__,
				abort_sources[i]);
		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		dev_err(dev->dev, "%s: %s\n", __func__, abort_sources[i]);

	if (abort_source & LRW_IC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & LRW_IC_TX_ABRT_GCALL_READ)
		return -EINVAL;
	else if (abort_source & LRW_IC_TX_ABRT_SDA_STUCK_AT_LOW)
		return i2c_lrw_bus_recover(&dev->adapter);
	else
		return -EIO;
}

static int i2c_lrw_set_fifo_size(struct i2c_lrw_dev *dev)
{
	u32 tx_fifo_depth, rx_fifo_depth;

	tx_fifo_depth = I2C_FIFO_DEPTH;
	rx_fifo_depth = I2C_FIFO_DEPTH;
	if (!dev->tx_fifo_depth) {
		dev->tx_fifo_depth = tx_fifo_depth;
		dev->rx_fifo_depth = rx_fifo_depth;
	} else if (tx_fifo_depth >= 2) {
		dev->tx_fifo_depth =
			min_t(u32, dev->tx_fifo_depth, tx_fifo_depth);
		dev->rx_fifo_depth =
			min_t(u32, dev->rx_fifo_depth, rx_fifo_depth);
	}

	return 0;
}

static u32 i2c_lrw_func(struct i2c_adapter *adap)
{
	struct i2c_lrw_dev *dev = i2c_get_adapdata(adap);

	return dev->functionality;
}

static void i2c_lrw_disable(struct i2c_lrw_dev *dev)
{
	int ret;

	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		return;

	__i2c_lrw_disable(dev);

	regmap_write(dev->map, LRW_IC_INTR_MASK, 0);
	regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR, LRW_IC_CLR_INTR,
			LRW_IC_CLR_INTR);

	i2c_lrw_release_lock(dev);
}

static void i2c_lrw_configure_fifo_master(struct i2c_lrw_dev *dev)
{
	regmap_write(dev->map, LRW_IC_TX_TL, dev->tx_fifo_depth / 2);
	regmap_write(dev->map, LRW_IC_RX_TL, 0);

	regmap_write(dev->map, LRW_IC_CON, dev->master_cfg);
}

static int i2c_lrw_set_timings_master(struct i2c_lrw_dev *dev)
{
	u32 sda_falling_time, scl_falling_time;
	struct i2c_timings *t = &dev->timings;
	const char *fp_str = "";
	u32 ic_clk;
	int ret;

	sda_falling_time = t->sda_fall_ns ?: 300;
	scl_falling_time = t->scl_fall_ns ?: 300;

	if ((dev->master_cfg & LRW_IC_CON_SPEED_MASK) ==
		LRW_IC_CON_SPEED_HIGH) {
		if (!dev->hs_hcnt || !dev->hs_lcnt) {
			ic_clk = i2c_lrw_clk_rate(dev);
			dev->hs_hcnt = i2c_lrw_scl_hcnt(ic_clk, 160,
							sda_falling_time, 0, 0);
			dev->hs_lcnt = i2c_lrw_scl_lcnt(ic_clk, 320,
							scl_falling_time, 0);
		} else {
			dev_err(dev->dev, "High Speed not supported!\n");
			t->bus_freq_hz = I2C_MAX_FAST_MODE_FREQ;
			dev->master_cfg &= ~LRW_IC_CON_SPEED_MASK;
			dev->master_cfg |= LRW_IC_CON_SPEED_STD_FAST;
			dev->hs_hcnt = 0;
			dev->hs_lcnt = 0;
		}
		dev_dbg(dev->dev, "High Speed Mode HCNT:LCNT = %d:%d\n",
			dev->hs_hcnt, dev->hs_lcnt);
	}

	if (!dev->fs_hcnt || !dev->fs_lcnt) {
		ic_clk = i2c_lrw_clk_rate(dev);
		switch (t->bus_freq_hz) {
		case I2C_MAX_STANDARD_MODE_FREQ: /* 100kHz */
			dev->fs_hcnt = i2c_lrw_scl_hcnt(ic_clk, 4000,
							sda_falling_time, 0, 0);
			dev->fs_lcnt = i2c_lrw_scl_lcnt(ic_clk, 4700,
							scl_falling_time, 0);
			break;
		case I2C_MAX_FAST_MODE_FREQ: /* 400kHz */
			dev->fs_hcnt = i2c_lrw_scl_hcnt(ic_clk, 600,
							sda_falling_time, 0, 0);
			dev->fs_lcnt = i2c_lrw_scl_lcnt(ic_clk, 1300,
							scl_falling_time, 0);
			break;

		case I2C_MAX_FAST_MODE_PLUS_FREQ: /* 1MHz */
			dev->fs_hcnt = i2c_lrw_scl_hcnt(ic_clk, 260,
							sda_falling_time, 0, 0);
			dev->fs_lcnt = i2c_lrw_scl_lcnt(ic_clk, 500,
							scl_falling_time, 0);
			fp_str = " Plus";
			break;
		default:
			break;
		}
		dev_dbg(dev->dev, "Fast Mode%s HCNT:LCNT = %d:%d\n", fp_str,
			dev->fs_hcnt, dev->fs_lcnt);
	}

	ret = i2c_lrw_set_sda_hold(dev);
	if (ret)
		return ret;

	dev_dbg(dev->dev, "Bus speed: %s\n",
		i2c_freq_mode_string(t->bus_freq_hz));
	return 0;
}

static int i2c_lrw_init_master(struct i2c_lrw_dev *dev)
{
	int ret;

	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		return ret;

	__i2c_lrw_disable(dev);

	regmap_write(dev->map, LRW_IC_FS_SCL_HCNT, dev->fs_hcnt);
	regmap_write(dev->map, LRW_IC_FS_SCL_LCNT, dev->fs_lcnt);
	regmap_write(dev->map, LRW_IC_SDA_STUCK_AT_LOW_TIMEOUT,
			dev->sda_stuck_at_low_timeout);
	if (dev->hs_hcnt && dev->hs_lcnt) {
		regmap_write(dev->map, LRW_IC_HS_SCL_HCNT, dev->hs_hcnt);
		regmap_write(dev->map, LRW_IC_HS_SCL_LCNT, dev->hs_lcnt);
	}

	if (dev->sda_hold_time)
		regmap_write(dev->map, LRW_IC_SDA_HOLD, dev->sda_hold_time);

	i2c_lrw_configure_fifo_master(dev);
	i2c_lrw_release_lock(dev);

	return 0;
}

static void i2c_lrw_xfer_init(struct i2c_lrw_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 ic_con = 0, ic_tar = 0;

	__i2c_lrw_disable(dev);

	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN) {
		ic_con = LRW_IC_CON_10BITADDR_MASTER;
		ic_tar = LRW_IC_TAR_10BITADDR_MASTER;
	}

	regmap_update_bits(dev->map, LRW_IC_CON, LRW_IC_CON_10BITADDR_MASTER,
			ic_con);
	regmap_write(dev->map, LRW_IC_TAR,
			msgs[dev->msg_write_idx].addr | ic_tar);

	regmap_write(dev->map, LRW_IC_INTR_MASK, 0);

	__i2c_lrw_enable(dev);

	regmap_update_bits(dev->map, LRW_IC_CLR_RAW_INTR, LRW_IC_CLR_INTR,
			LRW_IC_CLR_INTR);
	regmap_write(dev->map, LRW_IC_INTR_MASK, LRW_IC_INTR_MASTER_MASK);
}

static void i2c_lrw_xfer_msg(struct i2c_lrw_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 intr_mask;
	u32 fifo_level;
	int tx_limit, rx_limit;
	u32 addr = msgs[dev->msg_write_idx].addr;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;
	bool need_restart = false;
	unsigned int flr;

	intr_mask = LRW_IC_INTR_MASTER_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		u32 flags = msgs[dev->msg_write_idx].flags;

		if (msgs[dev->msg_write_idx].addr != addr) {
			dev_err(dev->dev, "%s: invalid target address\n",
				__func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;

			if ((dev->master_cfg & LRW_IC_CON_RESTART_EN) &&
				(dev->msg_write_idx > 0))
				need_restart = true;
		}

		regmap_read(dev->map, LRW_IC_FIFO_LEVEL, &fifo_level);
		flr = fifo_level & I2C_TXFLR_MASK;
		tx_limit = dev->tx_fifo_depth - flr;

		flr = (fifo_level >> I2C_RXFLR_SHIFT) & I2C_RXFLR_MASK;
		rx_limit = dev->rx_fifo_depth - flr;

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			if (dev->msg_write_idx == dev->msgs_num - 1 &&
				buf_len == 1 && !(flags & I2C_M_RECV_LEN))
				cmd |= BIT(9);

			if (need_restart) {
				cmd |= BIT(10);
				need_restart = false;
			}

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
				if (dev->rx_outstanding >= dev->rx_fifo_depth)
					break;

				regmap_write(dev->map, LRW_IC_DATA_CMD,
						cmd | 0x100);
				rx_limit--;
				dev->rx_outstanding++;
			} else {
				regmap_write(dev->map, LRW_IC_DATA_CMD,
						cmd | *buf++);
			}
			tx_limit--;
			buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		if (flags & I2C_M_RECV_LEN) {
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			intr_mask &= ~LRW_IC_INTR_TX_EMPTY;
			break;
		}

		if (buf_len > 0) {
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		}

		dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	if (dev->msg_write_idx == dev->msgs_num)
		intr_mask &= ~LRW_IC_INTR_TX_EMPTY;

	if (dev->msg_err)
		intr_mask = 0;

	regmap_write(dev->map, LRW_IC_INTR_MASK, intr_mask);
}

static u8 i2c_lrw_recv_len(struct i2c_lrw_dev *dev, u8 len)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 flags = msgs[dev->msg_read_idx].flags;

	len += (flags & I2C_CLIENT_PEC) ? 2 : 1;
	dev->tx_buf_len = len - min_t(u8, len, dev->rx_outstanding);
	msgs[dev->msg_read_idx].len = len;
	msgs[dev->msg_read_idx].flags &= ~I2C_M_RECV_LEN;

	regmap_update_bits(dev->map, LRW_IC_INTR_MASK, LRW_IC_INTR_TX_EMPTY,
			LRW_IC_INTR_TX_EMPTY);

	return len;
}

static void i2c_lrw_read(struct i2c_lrw_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 fifo_level;
	unsigned int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		unsigned int tmp;
		u32 len;
		u8 *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD))
			continue;

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		regmap_read(dev->map, LRW_IC_FIFO_LEVEL, &fifo_level);
		rx_valid = (fifo_level >> I2C_RXFLR_SHIFT) & I2C_RXFLR_MASK;

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			u32 flags = msgs[dev->msg_read_idx].flags;

			regmap_read(dev->map, LRW_IC_DATA_CMD, &tmp);
			tmp &= LRW_IC_DATA_CMD_DAT;
			if (flags & I2C_M_RECV_LEN) {
				if (!tmp || tmp > I2C_SMBUS_BLOCK_MAX)
					tmp = 1;

				len = i2c_lrw_recv_len(dev, tmp);
			}
			*buf++ = tmp;
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		}
		dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int i2c_lrw_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[],
			int num)
{
	struct i2c_lrw_dev *dev = i2c_get_adapdata(adap);
	int ret;

	reinit_completion(&dev->cmd_complete);
	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = 0;
	dev->abort_source = 0;
	dev->rx_outstanding = 0;
	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		goto done_nolock;
	ret = i2c_lrw_wait_bus_not_busy(dev);
	if (ret < 0)
		goto done;
	i2c_lrw_xfer_init(dev);

	if (!wait_for_completion_timeout(&dev->cmd_complete, adap->timeout)) {
		dev_err(dev->dev, "controller timed out\n");
		i2c_lrw_init_master(dev);
		ret = -ETIMEDOUT;
		goto done;
	}

	dev->status &= ~STATUS_ACTIVE;

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto disable_controller;
	}

	if (likely(!dev->cmd_err && !dev->status)) {
		ret = num;
		goto disable_controller;
	}

	if (dev->cmd_err == LRW_IC_ERR_TX_ABRT) {
		ret = i2c_lrw_handle_tx_abort(dev);
		goto disable_controller;
	}

	if (dev->status) {
		dev_err(dev->dev,
			"transfer terminated early - interrupt latency too high?\n");
		ret = -EIO;
		goto disable_controller;
	}

	ret = -EIO;

disable_controller:
	__i2c_lrw_disable_nowait(dev);
done:
	i2c_lrw_release_lock(dev);

done_nolock:
	return ret;
}

static const struct i2c_algorithm i2c_lrw_algo = {
	.master_xfer = i2c_lrw_xfer,
	.functionality = i2c_lrw_func,
};

static const struct i2c_adapter_quirks i2c_lrw_quirks = {
	.flags = I2C_AQ_NO_ZERO_LEN,
};

static u32 i2c_lrw_read_clear_intrbits(struct i2c_lrw_dev *dev)
{
	u32 stat;

	regmap_read(dev->map, LRW_IC_INTR_STAT, &stat);

	if (stat & LRW_IC_INTR_RX_OVER)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_RX_OVER, LRW_IC_INTR_RX_OVER);
	if (stat & LRW_IC_INTR_RD_REQ)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_RD_REQ, LRW_IC_INTR_RD_REQ);
	if (stat & LRW_IC_INTR_TX_ABRT) {
		regmap_read(dev->map, LRW_IC_TX_ABRT_SOURCE,
				&dev->abort_source);
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_TX_ABRT, LRW_IC_INTR_TX_ABRT);
	}
	if (stat & LRW_IC_INTR_RX_DONE)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_RX_DONE, LRW_IC_INTR_RX_DONE);
	if (stat & LRW_IC_INTR_ACTIVITY)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_ACTIVITY, LRW_IC_INTR_ACTIVITY);
	if (stat & LRW_IC_INTR_STOP_DET)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_STOP_DET, LRW_IC_INTR_STOP_DET);
	if (stat & LRW_IC_INTR_START_DET)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_START_DET,
				LRW_IC_INTR_START_DET);
	if (stat & LRW_IC_INTR_GEN_CALL)
		regmap_write_bits(dev->map, LRW_IC_CLR_RAW_INTR,
				LRW_IC_INTR_GEN_CALL, LRW_IC_INTR_GEN_CALL);

	return stat;
}

static irqreturn_t i2c_lrw_isr(int this_irq, void *dev_id)
{
	struct i2c_lrw_dev *dev = dev_id;
	u32 stat, enabled;

	regmap_read(dev->map, LRW_IC_ENABLE, &enabled);
	regmap_read(dev->map, LRW_IC_CLR_RAW_INTR, &stat);
	if (!enabled || !(stat & ~LRW_IC_INTR_ACTIVITY))
		return IRQ_NONE;

	stat = i2c_lrw_read_clear_intrbits(dev);

	if (!(dev->status & STATUS_ACTIVE)) {
		regmap_write(dev->map, LRW_IC_INTR_MASK, 0);
		return IRQ_HANDLED;
	}

	if (stat & LRW_IC_INTR_TX_ABRT) {
		dev->cmd_err |= LRW_IC_ERR_TX_ABRT;
		dev->status &= ~STATUS_MASK;
		dev->rx_outstanding = 0;

		dev_dbg(dev->dev, "abrt intr:0x%x, abort:0x%x\n", stat,
			 dev->abort_source);
		regmap_write(dev->map, LRW_IC_INTR_MASK, 0);
		goto tx_aborted;
	}

	if (stat & LRW_IC_INTR_RX_FULL)
		i2c_lrw_read(dev);

	if (stat & LRW_IC_INTR_TX_EMPTY)
		i2c_lrw_xfer_msg(dev);

tx_aborted:
	if (((stat & (LRW_IC_INTR_TX_ABRT | LRW_IC_INTR_STOP_DET)) ||
		dev->msg_err) &&
		(dev->rx_outstanding == 0)) {
		complete(&dev->cmd_complete);
	} else {
		regmap_read(dev->map, LRW_IC_INTR_MASK, &stat);
		regmap_write(dev->map, LRW_IC_INTR_MASK, 0);
		regmap_write(dev->map, LRW_IC_INTR_MASK, stat);
	}

	return IRQ_HANDLED;
}

void i2c_lrw_configure_master(struct i2c_lrw_dev *dev)
{
	struct i2c_timings *t = &dev->timings;

	dev->functionality = I2C_FUNC_10BIT_ADDR | LRW_IC_DEFAULT_FUNCTIONALITY;

	dev->master_cfg = LRW_IC_CON_MASTER | LRW_IC_CON_RESTART_EN |
			IC_CON_BUS_CLEAR_CTRL_POS;

	switch (t->bus_freq_hz) {
	case I2C_MAX_STANDARD_MODE_FREQ:
	case I2C_MAX_FAST_MODE_FREQ:
	case I2C_MAX_FAST_MODE_PLUS_FREQ:
		dev->master_cfg |= LRW_IC_CON_SPEED_STD_FAST;
		break;
	case I2C_MAX_HIGH_SPEED_MODE_FREQ:
		dev->master_cfg |= LRW_IC_CON_SPEED_HIGH;
		break;
	default:
		dev_warn(dev->dev,
			 "dev bus_freq_hz outlined in the device datasheet?\n");
	}
}
EXPORT_SYMBOL_GPL(i2c_lrw_configure_master);

static int i2c_lrw_init_recovery_info(struct i2c_lrw_dev *dev)
{
	struct i2c_bus_recovery_info *rinfo = &dev->rinfo;
	struct i2c_adapter *adap = &dev->adapter;

	rinfo->recover_bus = i2c_lrw_bus_recover;
	adap->bus_recovery_info = rinfo;

	return 0;
}

int i2c_lrw_probe_master(struct i2c_lrw_dev *dev)
{
	struct i2c_adapter *adap = &dev->adapter;
	unsigned long irq_flags;
	unsigned int ic_con;
	int ret;

	init_completion(&dev->cmd_complete);

	dev->init = i2c_lrw_init_master;
	dev->disable = i2c_lrw_disable;

	ret = i2c_lrw_init_regmap(dev);
	if (ret)
		return ret;

	ret = i2c_lrw_set_timings_master(dev);
	if (ret)
		return ret;

	ret = i2c_lrw_set_fifo_size(dev);
	if (ret)
		return ret;

	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		return ret;

	ret = regmap_read(dev->map, LRW_IC_CON, &ic_con);
	i2c_lrw_release_lock(dev);
	if (ret)
		return ret;

	if (ic_con & IC_CON_BUS_CLEAR_CTRL_POS)
		dev->master_cfg |= IC_CON_BUS_CLEAR_CTRL_POS;

	ret = dev->init(dev);
	if (ret)
		return ret;

	snprintf(adap->name, sizeof(adap->name), "LRW I2C adapter");
	adap->retries = 3;
	adap->algo = &i2c_lrw_algo;
	adap->quirks = &i2c_lrw_quirks;
	adap->dev.parent = dev->dev;
	i2c_set_adapdata(adap, dev);

	irq_flags = IRQF_SHARED | IRQF_COND_SUSPEND;

	ret = i2c_lrw_acquire_lock(dev);
	if (ret)
		return ret;

	regmap_write(dev->map, LRW_IC_INTR_MASK, 0);
	regmap_write(dev->map, LRW_SMBUS_INTR_MASK, 0);

	i2c_lrw_release_lock(dev);

	ret = devm_request_irq(dev->dev, dev->irq, i2c_lrw_isr, irq_flags,
				dev_name(dev->dev), dev);
	if (ret) {
		dev_err(dev->dev, "failure requesting irq %i: %d\n", dev->irq,
			ret);
		return ret;
	}

	ret = i2c_lrw_init_recovery_info(dev);
	if (ret)
		return ret;

	ret = i2c_add_numbered_adapter(adap);
	if (ret)
		dev_err(dev->dev, "failure adding adapter: %d\n", ret);

	return ret;
}
EXPORT_SYMBOL_GPL(i2c_lrw_probe_master);

MODULE_DESCRIPTION("LRW I2C bus master adapter");
MODULE_LICENSE("GPL");
