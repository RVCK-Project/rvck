// SPDX-License-Identifier: GPL-2.0
/*
 * lanxin i2c driver
 *
 */


#include <linux/clk.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/units.h>

#define LX_IC_DEFAULT_FUNCTIONALITY		(I2C_FUNC_I2C | \
						 I2C_FUNC_SMBUS_BYTE | \
						 I2C_FUNC_SMBUS_BYTE_DATA | \
						 I2C_FUNC_SMBUS_WORD_DATA | \
						 I2C_FUNC_SMBUS_BLOCK_DATA | \
						 I2C_FUNC_SMBUS_I2C_BLOCK)

/*
 * Registers offset
 */
#define LX_IC_ENABLE				0x04	/* IC_DFLT_OPERATION_REG_OFFSET + 0x4 */
#define LX_IC_ENABLE_ABORT			BIT(1)

#define LX_IC_CTRL				0x24	/* IC_DFLT_I2C_REG_OFFSET + 0x4 */
#define LX_IC_CTRL_MASTER			BIT(0)
#define LX_IC_CTRL_SPEED_STD			(1 << 4)
#define LX_IC_CTRL_SPEED_FAST			(2 << 4)
#define LX_IC_CTRL_SPEED_HIGH			(3 << 4)
#define LX_IC_CTRL_SPEED_MASK			GENMASK(5, 4)
#define LX_IC_CTRL_10BITADDR_SLAVE		BIT(8)
#define LX_IC_CTRL_10BITADDR_MASTER		BIT(9)
#define LX_IC_CTRL_STOP_DET_IFADDRESSED		BIT(10)
#define LX_IC_CTRL_TX_EMPTY_CTRL		BIT(11)
#define LX_IC_CTRL_RX_FIFO_FULL_HLD_CTRL	BIT(12)
#define LX_IC_CTRL_BUS_CLEAR_CTRL		BIT(14)

#define LX_IC_TAR				0x28	/* IC_DFLT_I2C_REG_OFFSET + 0x8 */
#define LX_IC_TAR_10BITADDR_MASTER		BIT(12)

#define LX_IC_DAR				0x2c	/* IC_DFLT_I2C_REG_OFFSET + 0xc */
#define LX_IC_SCL_HCNT				0x44	/* IC_DFLT_I2C_REG_OFFSET + 0x24 */
#define LX_IC_SCL_LCNT				0x48	/* IC_DFLT_I2C_REG_OFFSET + 0x28 */
#define LX_IC_HS_SCL_HCNT			0x4c	/* IC_DFLT_I2C_REG_OFFSET + 0x2c */
#define LX_IC_HS_SCL_LCNT			0x50	/* IC_DFLT_I2C_REG_OFFSET + 0x30 */

#define LX_IC_SDA_HOLD				0x54	/* IC_DFLT_I2C_REG_OFFSET + 0x34 */
#define LX_IC_SDA_HOLD_RX_SHIFT			16
#define LX_IC_SDA_HOLD_RX_MASK			GENMASK(23, 16)

#define LX_IC_DATA_CMD				0x78	/* IC_DFLT_I2C_REG_OFFSET + 0x58 */
#define LX_IC_DATA_CMD_DAT			GENMASK(7, 0)
#define LX_IC_DATA_CMD_FIRST_DATA_BYTE		BIT(11)

#define LX_IC_RX_TL				0x7c	/* IC_DFLT_I2C_REG_OFFSET + 0x5c */
#define LX_IC_TX_TL				0x80	/* IC_DFLT_I2C_REG_OFFSET + 0x60 */

#define LX_IC_INTR_STAT				0x94	/* IC_DFLT_I2C_REG_OFFSET + 0x74 */
#define LX_IC_INTR_RX_UNDER			BIT(0)
#define LX_IC_INTR_RX_OVER			BIT(1)
#define LX_IC_INTR_RX_FULL			BIT(2)
#define LX_IC_INTR_TX_OVER			BIT(3)
#define LX_IC_INTR_TX_EMPTY			BIT(4)
#define LX_IC_INTR_RD_REQ			BIT(5)
#define LX_IC_INTR_TX_ABRT			BIT(6)
#define LX_IC_INTR_RX_DONE			BIT(7)
#define LX_IC_INTR_ACTIVITY			BIT(8)
#define LX_IC_INTR_STOP_DET			BIT(9)
#define LX_IC_INTR_START_DET			BIT(10)
#define LX_IC_INTR_GEN_CALL			BIT(11)
#define LX_IC_INTR_RESTART_DET			BIT(12)
#define LX_IC_INTR_MST_ON_HOLD			BIT(13)
#define LX_IC_INTR_DEFAULT_MASK			(LX_IC_INTR_RX_FULL | \
						 LX_IC_INTR_TX_ABRT | \
						 LX_IC_INTR_STOP_DET)
#define LX_IC_INTR_MASTER_MASK			(LX_IC_INTR_DEFAULT_MASK | \
						 LX_IC_INTR_TX_EMPTY)
#define LX_IC_INTR_SLAVE_MASK			(LX_IC_INTR_DEFAULT_MASK | \
						 LX_IC_INTR_RX_UNDER | \
						 LX_IC_INTR_RD_REQ)

#define LX_IC_INTR_MASK				0x98	/* IC_DFLT_I2C_REG_OFFSET + 0x78 */
#define LX_IC_INTR_RAW_STAT			0x9c	/* IC_DFLT_I2C_REG_OFFSET + 0x7c */

#define LX_IC_INTR_CLR				0xa0	/* IC_DFLT_I2C_REG_OFFSET + 0x80 */
#define LX_IC_INTR_CLR_ALL			BIT(0)
#define LX_IC_INTR_CLR_RX_UNDER			BIT(1)
#define LX_IC_INTR_CLR_RX_OVER			BIT(2)
#define LX_IC_INTR_CLR_TX_OVER			BIT(3)
#define LX_IC_INTR_CLR_RD_REQ			BIT(4)
#define LX_IC_INTR_CLR_TX_TRMNT			BIT(5)
#define LX_IC_INTR_CLR_RX_DONE			BIT(6)
#define LX_IC_INTR_CLR_ACTIVITY			BIT(7)
#define LX_IC_INTR_CLR_STOP_DET			BIT(8)
#define LX_IC_INTR_CLR_START_DET		BIT(9)
#define LX_IC_INTR_CLR_GEN_CALL			BIT(10)
#define LX_IC_INTR_CLR_RESTART_DET		BIT(11)
#define LX_IC_INTR_CLR_STUCK_DET		BIT(12)
#define LX_IC_INTR_CLR_WR_REQ			BIT(13)
#define LX_IC_INTR_CLR_TGT_ADDR_TAG		BIT(14)

#define LX_IC_ENABLE_STATUS			0xa4	/* IC_DFLT_I2C_REG_OFFSET + 0x84 */
#define LX_IC_TX_ABRT_SOURCE			0xa8	/* IC_DFLT_I2C_REG_OFFSET + 0x88 */

#define LX_IC_STATUS				0xac	/* IC_DFLT_I2C_REG_OFFSET + 0x8c */
#define LX_IC_STATUS_ACTIVITY			BIT(0)
#define LX_IC_STATUS_TFE			BIT(2)
#define LX_IC_STATUS_RFNE			BIT(3)
#define LX_IC_STATUS_MASTER_ACTIVITY		BIT(5)
#define LX_IC_STATUS_SLAVE_ACTIVITY		BIT(6)

#define LX_IC_TXFLR				0xb0	/* IC_DFLT_I2C_REG_OFFSET + 0x90 */
#define LX_IC_RXFLR				0xb4	/* IC_DFLT_I2C_REG_OFFSET + 0x94 */
#define LX_IC_COMP_VERSION			0xc4	/* IC_DFLT_I2C_REG_OFFSET + 0xa4 */
#define LX_IC_SDA_HOLD_MIN_VERS			0x3130312A
#define LX_IC_COMP_TYPE				0xc8	/* IC_DFLT_I2C_REG_OFFSET + 0xa8 */
#define LX_IC_COMP_TYPE_VALUE			0x44570140


#define LX_IC_ERR_TX_ABRT			0x1

/*
 * Sofware status flags
 */
#define STATUS_ACTIVE				BIT(0)
#define STATUS_WRITE_IN_PROGRESS		BIT(1)
#define STATUS_READ_IN_PROGRESS			BIT(2)
#define STATUS_MASK				GENMASK(2, 0)

/*
 * operation modes
 */
#define LX_IC_MASTER				0
#define LX_IC_SLAVE				1

/*
 * Hardware abort codes from the LX_IC_TX_ABRT_SOURCE register
 *
 * Only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK			0
#define ABRT_10ADDR1_NOACK			1
#define ABRT_10ADDR2_NOACK			2
#define ABRT_TXDATA_NOACK			3
#define ABRT_GCALL_NOACK			4
#define ABRT_GCALL_READ				5
#define ABRT_SBYTE_ACKDET			7
#define ABRT_SBYTE_NORSTRT			9
#define ABRT_10B_RD_NORSTRT			10
#define ABRT_RSVD_10TO8				GENMASK(10, 8)
#define ABRT_MASTER_DIS				11
#define ARB_LOST				12
#define ABRT_SLAVE_FLUSH_TXFIFO			13
#define ABRT_SLAVE_ARBLOST			14
#define ABRT_SLAVE_RD_INTX			15
#define LX_IC_TX_ABRT_7B_ADDR_NOACK		BIT(ABRT_7B_ADDR_NOACK)
#define LX_IC_TX_ABRT_10ADDR1_NOACK		BIT(ABRT_10ADDR1_NOACK)
#define LX_IC_TX_ABRT_10ADDR2_NOACK		BIT(ABRT_10ADDR2_NOACK)
#define LX_IC_TX_ABRT_TXDATA_NOACK		BIT(ABRT_TXDATA_NOACK)
#define LX_IC_TX_ABRT_GCALL_NOACK		BIT(ABRT_GCALL_NOACK)
#define LX_IC_TX_ABRT_GCALL_READ		BIT(ABRT_GCALL_READ)
#define LX_IC_TX_ABRT_SBYTE_ACKDET		BIT(ABRT_SBYTE_ACKDET)
#define LX_IC_TX_ABRT_MASTER_DIS		BIT(ABRT_MASTER_DIS)
#define LX_IC_TX_ARB_LOST			BIT(ARB_LOST)
#define LX_IC_RX_ABRT_SLAVE_FLUSH_TXFIFO	BIT(ABRT_SLAVE_FLUSH_TXFIFO)
#define LX_IC_RX_ABRT_SLAVE_ARBLOST		BIT(ABRT_SLAVE_ARBLOST)
#define LX_IC_RX_ABRT_SLAVE_RD_INTX		BIT(ABRT_SLAVE_RD_INTX)
#define LX_IC_TX_FLUSH_CNT			GENMASK(31, 23)
#define LX_IC_TX_ABRT_NOACK			(LX_IC_TX_ABRT_7B_ADDR_NOACK | \
						 LX_IC_TX_ABRT_10ADDR1_NOACK | \
						 LX_IC_TX_ABRT_10ADDR2_NOACK | \
						 LX_IC_TX_ABRT_TXDATA_NOACK | \
						 LX_IC_TX_ABRT_GCALL_NOACK)

#define ACCESS_INTR_MASK			BIT(0)
#define ACCESS_NO_IRQ_SUSPEND			BIT(1)
#define ARBITRATION_SEMAPHORE			BIT(2)

#define MODEL_MASK				GENMASK(11, 8)

#define LX_TX_FIFO_DEPTH			8
#define LX_RX_FIFO_DEPTH			8

struct clk;
struct device;
struct reset_control;

/**
 * struct lx_i2c_dev - private data
 * @dev: driver model device node
 * @map: IO registers map
 * @sysmap: System controller registers map
 * @base: IO registers pointer
 * @ext: Extended IO registers pointer
 * @cmd_complete: tx completion indicator
 * @clk: input reference clock
 * @pclk: clock required to access the registers
 * @rst: optional reset for the controller
 * @slave: represent an I2C slave device
 * @get_clk_rate_khz: callback to retrieve IP specific bus speed
 * @cmd_err: run time hadware error code
 * @msgs: points to an array of messages currently being transferred
 * @msgs_num: the number of elements in msgs
 * @msg_write_idx: the element index of the current tx message in the msgs array
 * @tx_buf_len: the length of the current tx buffer
 * @tx_buf: the current tx buffer
 * @msg_read_idx: the element index of the current rx message in the msgs array
 * @rx_buf_len: the length of the current rx buffer
 * @rx_buf: the current rx buffer
 * @msg_err: error status of the current transfer
 * @status: i2c master status, one of STATUS_*
 * @abort_source: copy of the TX_ABRT_SOURCE register
 * @irq: interrupt number for the i2c master
 * @flags: platform specific flags like type of IO accessors or model
 * @adapter: i2c subsystem adapter node
 * @functionality: I2C_FUNC_* ORed bits to reflect what controller does support
 * @master_cfg: configuration for the master device
 * @slave_cfg: configuration for the slave device
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 * @rx_outstanding: current master-rx elements in tx fifo
 * @timings: bus clock frequency, SDA hold and other timings
 * @sda_hold_time: SDA hold value
 * @ss_hcnt: standard speed HCNT value
 * @ss_lcnt: standard speed LCNT value
 * @hs_hcnt: high speed HCNT value
 * @hs_lcnt: high speed LCNT value
 * @acquire_lock: function to acquire a hardware lock on the bus
 * @release_lock: function to release a hardware lock on the bus
 * @semaphore_idx: Index of table with semaphore type attached to the bus. It's
 *	-1 if there is no semaphore.
 * @disable: function to disable the controller
 * @init: function to initialize the I2C hardware
 * @set_sda_hold_time: callback to retrieve IP specific SDA hold timing
 * @mode: operation mode - LX_IC_MASTER or LX_IC_SLAVE
 * @rinfo: I²C GPIO recovery information
 */
struct lx_i2c_dev {
	struct device		*dev;
	struct regmap		*map;
	struct regmap		*sysmap;
	void __iomem		*base;
	void __iomem		*ext;
	struct completion	cmd_complete;
	struct clk		*clk;
	struct clk		*pclk;
	struct reset_control	*rst;
	struct i2c_client	*slave;
	u32			(*get_clk_rate_khz) (struct lx_i2c_dev *dev);
	int			cmd_err;
	struct i2c_msg		*msgs;
	int			msgs_num;
	int			msg_write_idx;
	u32			tx_buf_len;
	u8			*tx_buf;
	int			msg_read_idx;
	u32			rx_buf_len;
	u8			*rx_buf;
	int			msg_err;
	unsigned int		status;
	unsigned int		abort_source;
	int			irq;
	u32			flags;
	struct i2c_adapter	adapter;
	u32			functionality;
	u32			master_cfg;
	u32			slave_cfg;
	unsigned int		tx_fifo_depth;
	unsigned int		rx_fifo_depth;
	int			rx_outstanding;
	struct i2c_timings	timings;
	u32			sda_hold_time;
	u16			ss_hcnt;
	u16			ss_lcnt;
	u16			hs_hcnt;
	u16			hs_lcnt;
	int			(*acquire_lock)(void);
	void			(*release_lock)(void);
	int			semaphore_idx;
	void			(*disable)(struct lx_i2c_dev *dev);
	int			(*init)(struct lx_i2c_dev *dev);
	int			(*set_sda_hold_time)(struct lx_i2c_dev *dev);
	int			mode;
	struct i2c_bus_recovery_info rinfo;
};


struct i2c_lx_semaphore_callbacks {
	int	(*probe)(struct lx_i2c_dev *dev);
	void	(*remove)(struct lx_i2c_dev *dev);
};


static char *abort_sources[] = {
	[ABRT_7B_ADDR_NOACK] =
		"slave address not acknowledged (7bit mode)",
	[ABRT_10ADDR1_NOACK] =
		"first address byte not acknowledged (10bit mode)",
	[ABRT_10ADDR2_NOACK] =
		"second address byte not acknowledged (10bit mode)",
	[ABRT_TXDATA_NOACK] =
		"data not acknowledged",
	[ABRT_GCALL_NOACK] =
		"no acknowledgment for a general call",
	[ABRT_GCALL_READ] =
		"read after general call",
	[ABRT_SBYTE_ACKDET] =
		"start byte acknowledged",
	[ABRT_MASTER_DIS] =
		"trying to use disabled adapter",
	[ARB_LOST] =
		"lost arbitration",
	[ABRT_SLAVE_FLUSH_TXFIFO] =
		"read command so flush old data in the TX FIFO",
	[ABRT_SLAVE_ARBLOST] =
		"slave lost the bus while transmitting data to a remote master",
	[ABRT_SLAVE_RD_INTX] =
		"slave device trying to transmit to remote controller in read mode",
};

static int lx_reg_read(void *context, unsigned int reg, unsigned int *val)
{
	struct lx_i2c_dev *dev = context;

	*val = readl(dev->base + reg);

	return 0;
}

static int lx_reg_write(void *context, unsigned int reg, unsigned int val)
{
	struct lx_i2c_dev *dev = context;

	writel(val, dev->base + reg);

	return 0;
}

static int lx_reg_read_swab(void *context, unsigned int reg, unsigned int *val)
{
	struct lx_i2c_dev *dev = context;

	*val = swab32(readl(dev->base + reg));

	return 0;
}

static int lx_reg_write_swab(void *context, unsigned int reg, unsigned int val)
{
	struct lx_i2c_dev *dev = context;

	writel(swab32(val), dev->base + reg);

	return 0;
}

static int lx_reg_read_word(void *context, unsigned int reg, unsigned int *val)
{
	struct lx_i2c_dev *dev = context;

	*val = readw(dev->base + reg) |
		(readw(dev->base + reg + 2) << 16);

	return 0;
}

static int lx_reg_write_word(void *context, unsigned int reg, unsigned int val)
{
	struct lx_i2c_dev *dev = context;

	writew(val, dev->base + reg);
	writew(val >> 16, dev->base + reg + 2);

	return 0;
}

static int i2c_lx_acquire_lock(struct lx_i2c_dev *dev)
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

static void i2c_lx_release_lock(struct lx_i2c_dev *dev)
{
	if (dev->release_lock)
		dev->release_lock();
}

/**
 * i2c_lx_init_regmap() - Initialize registers map
 * @dev: device private data
 *
 * Autodetects needed register access mode and creates the regmap with
 * corresponding read/write callbacks. This must be called before doing any
 * other register access.
 */
static int i2c_lx_init_regmap(struct lx_i2c_dev *dev)
{
	struct regmap_config map_cfg = {
		.reg_bits = 32,
		.val_bits = 32,
		.reg_stride = 4,
		.disable_locking = true,
		.reg_read = lx_reg_read,
		.reg_write = lx_reg_write,
		.max_register = LX_IC_COMP_TYPE,
	};
	u32 reg;
	int ret;

	/*
	 * Skip detecting the registers map configuration if the regmap has
	 * already been provided by a higher code.
	 */
	if (dev->map)
		return 0;

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return ret;

	reg = readl(dev->base + LX_IC_COMP_TYPE);
	i2c_lx_release_lock(dev);

	if (reg == swab32(LX_IC_COMP_TYPE_VALUE)) {
		map_cfg.reg_read = lx_reg_read_swab;
		map_cfg.reg_write = lx_reg_write_swab;
	} else if (reg == (LX_IC_COMP_TYPE_VALUE & 0x0000ffff)) {
		map_cfg.reg_read = lx_reg_read_word;
		map_cfg.reg_write = lx_reg_write_word;
	} else if (reg != LX_IC_COMP_TYPE_VALUE) {
		dev_err(dev->dev,
			"Unknown Synopsys component type: 0x%08x\n", reg);
		return -ENODEV;
	}

	/*
	 * Note we'll check the return value of the regmap IO accessors only
	 * at the probe stage. The rest of the code won't do this because
	 * basically we have MMIO-based regmap so non of the read/write methods
	 * can fail.
	 */
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

static int i2c_lx_validate_speed(struct lx_i2c_dev *dev)
{
	struct i2c_timings *t = &dev->timings;
	unsigned int i;

	/*
	 * Only standard mode at 100kHz, fast mode at 400kHz,
	 * fast mode plus at 1MHz and high speed mode at 3.4MHz are supported.
	 */
	for (i = 0; i < ARRAY_SIZE(supported_speeds); i++) {
		if (t->bus_freq_hz == supported_speeds[i])
			return 0;
	}

	dev_err(dev->dev,
		"%d Hz is unsupported, only 100kHz, 400kHz, 1MHz and 3.4MHz are supported\n",
		t->bus_freq_hz);

	return -EINVAL;
}

static u32 i2c_lx_acpi_round_bus_speed(struct device *device)
{
	u32 acpi_speed;
	int i;

	acpi_speed = i2c_acpi_find_bus_speed(device);
	/*
	 * Some DSTDs use a non standard speed, round down to the lowest
	 * standard speed.
	 */
	for (i = 0; i < ARRAY_SIZE(supported_speeds); i++) {
		if (acpi_speed >= supported_speeds[i])
			return supported_speeds[i];
	}

	return 0;
}

static void i2c_lx_adjust_bus_speed(struct lx_i2c_dev *dev)
{
	u32 acpi_speed = i2c_lx_acpi_round_bus_speed(dev->dev);
	struct i2c_timings *t = &dev->timings;

	/*
	 * Find bus speed from the "clock-frequency" device property, ACPI
	 * or by using fast mode if neither is set.
	 */
	if (acpi_speed && t->bus_freq_hz)
		t->bus_freq_hz = min(t->bus_freq_hz, acpi_speed);
	else if (acpi_speed || t->bus_freq_hz)
		t->bus_freq_hz = max(t->bus_freq_hz, acpi_speed);
	else
		t->bus_freq_hz = I2C_MAX_FAST_MODE_FREQ;
}

static u32 i2c_lx_read_scl_reg(struct lx_i2c_dev *dev, u32 reg)
{
	u32 val;
	int ret;

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return 0;

	ret = regmap_read(dev->map, reg, &val);
	i2c_lx_release_lock(dev);

	return ret ? 0 : val;
}

static u32 i2c_lx_scl_hcnt(struct lx_i2c_dev *dev, unsigned int reg, u32 ic_clk,
		    u32 tSYMBOL, u32 tf, int cond, int offset)
{
	if (!ic_clk)
		return i2c_lx_read_scl_reg(dev, reg);

	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond)
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * tSYMBOL, MICRO) -
		       8 + offset;
	else
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportinal to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_lx_scl_lcnt().
		 */
		return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * (tSYMBOL + tf), MICRO) -
		       3 + offset;
}

static u32 i2c_lx_scl_lcnt(struct lx_i2c_dev *dev, unsigned int reg, u32 ic_clk,
		    u32 tLOW, u32 tf, int offset)
{
	if (!ic_clk)
		return i2c_lx_read_scl_reg(dev, reg);

	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return DIV_ROUND_CLOSEST_ULL((u64)ic_clk * (tLOW + tf), MICRO) -
	       1 + offset;
}

static int i2c_lx_set_sda_hold(struct lx_i2c_dev *dev)
{
	unsigned int reg;
	int ret;

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return ret;

	/* Configure SDA Hold Time if required */
	ret = regmap_read(dev->map, LX_IC_COMP_VERSION, &reg);
	if (ret)
		goto err_release_lock;

	if (reg >= LX_IC_SDA_HOLD_MIN_VERS) {
		if (!dev->sda_hold_time) {
			/* Keep previous hold time setting if no one set it */
			ret = regmap_read(dev->map, LX_IC_SDA_HOLD,
					  &dev->sda_hold_time);
			if (ret)
				goto err_release_lock;
		}

		/*
		 * Workaround for avoiding TX arbitration lost in case I2C
		 * slave pulls SDA down "too quickly" after falling edge of
		 * SCL by enabling non-zero SDA RX hold. Specification says it
		 * extends incoming SDA low to high transition while SCL is
		 * high but it appears to help also above issue.
		 */
		if (!(dev->sda_hold_time & LX_IC_SDA_HOLD_RX_MASK))
			dev->sda_hold_time |= 1 << LX_IC_SDA_HOLD_RX_SHIFT;

		dev_dbg(dev->dev, "SDA Hold Time TX:RX = %d:%d\n",
			dev->sda_hold_time & ~(u32)LX_IC_SDA_HOLD_RX_MASK,
			dev->sda_hold_time >> LX_IC_SDA_HOLD_RX_SHIFT);
	} else if (dev->set_sda_hold_time) {
		dev->set_sda_hold_time(dev);
	} else if (dev->sda_hold_time) {
		dev_warn(dev->dev,
			"Hardware too old to adjust SDA hold time.\n");
		dev->sda_hold_time = 0;
	}

err_release_lock:
	i2c_lx_release_lock(dev);

	return ret;
}


static inline void __i2c_lx_enable(struct lx_i2c_dev *dev)
{
	dev->status |= STATUS_ACTIVE;
	regmap_write(dev->map, LX_IC_ENABLE, 1);
}

static inline void __i2c_lx_disable_nowait(struct lx_i2c_dev *dev)
{
	regmap_write(dev->map, LX_IC_ENABLE, 0);
	dev->status &= ~STATUS_ACTIVE;
}

static void __i2c_lx_disable(struct lx_i2c_dev *dev)
{
	unsigned int raw_intr_stats;
	unsigned int enable;
	int timeout = 100;
	bool abort_needed;
	unsigned int status;
	int ret;

	regmap_read(dev->map, LX_IC_INTR_RAW_STAT, &raw_intr_stats);
	regmap_read(dev->map, LX_IC_ENABLE, &enable);

	abort_needed = raw_intr_stats & LX_IC_INTR_MST_ON_HOLD;
	if (abort_needed) {
		regmap_write(dev->map, LX_IC_ENABLE, enable | LX_IC_ENABLE_ABORT);
		ret = regmap_read_poll_timeout(dev->map, LX_IC_ENABLE, enable,
					       !(enable & LX_IC_ENABLE_ABORT), 10,
					       100);
		if (ret)
			dev_err(dev->dev, "timeout while trying to abort current transfer\n");
	}

	do {
		__i2c_lx_disable_nowait(dev);
		/*
		 * The enable status register may be unimplemented, but
		 * in that case this test reads zero and exits the loop.
		 */
		regmap_read(dev->map, LX_IC_ENABLE_STATUS, &status);
		if ((status & 1) == 0)
			return;

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		usleep_range(25, 250);
	} while (timeout--);

	dev_warn(dev->dev, "timeout in disabling adapter\n");
}

static u32 i2c_lx_clk_rate(struct lx_i2c_dev *dev)
{
	/*
	 * Clock is not necessary if we got LCNT/HCNT values directly from
	 * the platform code.
	 */
	if (!dev->get_clk_rate_khz) {
		dev_dbg_once(dev->dev, "Callback get_clk_rate_khz() is not defined\n");
		return 0;
	}
	return dev->get_clk_rate_khz(dev);
}

static int i2c_lx_prepare_clk(struct lx_i2c_dev *dev, bool prepare)
{
	int ret;

	if (prepare) {
		/* Optional interface clock */
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

/*
 * Waiting for bus not busy
 */
static int i2c_lx_wait_bus_not_busy(struct lx_i2c_dev *dev)
{
	unsigned int status;
	int ret;

	ret = regmap_read_poll_timeout(dev->map, LX_IC_STATUS, status,
				       !(status & LX_IC_STATUS_ACTIVITY),
				       1100, 20000);
	if (ret) {
		dev_warn(dev->dev, "timeout waiting for bus ready\n");

		i2c_recover_bus(&dev->adapter);

		regmap_read(dev->map, LX_IC_STATUS, &status);
		if (!(status & LX_IC_STATUS_ACTIVITY))
			ret = 0;
	}

	return ret;
}

static int i2c_lx_handle_tx_abort(struct lx_i2c_dev *dev)
{
	unsigned long abort_source = dev->abort_source;
	int i;

	if (abort_source & LX_IC_TX_ABRT_NOACK) {
		for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
			dev_dbg(dev->dev,
				"%s: %s\n", __func__, abort_sources[i]);
		return -EREMOTEIO;
	}

	for_each_set_bit(i, &abort_source, ARRAY_SIZE(abort_sources))
		dev_err(dev->dev, "%s: %s\n", __func__, abort_sources[i]);

	if (abort_source & LX_IC_TX_ARB_LOST)
		return -EAGAIN;
	else if (abort_source & LX_IC_TX_ABRT_GCALL_READ)
		return -EINVAL; /* wrong msgs[] data */
	else
		return -EIO;
}

static int i2c_lx_set_fifo_size(struct lx_i2c_dev *dev)
{
	dev->tx_fifo_depth = LX_TX_FIFO_DEPTH;
	dev->rx_fifo_depth = LX_RX_FIFO_DEPTH;

	return 0;
}

static u32 i2c_lx_func(struct i2c_adapter *adap)
{
	struct lx_i2c_dev *dev = i2c_get_adapdata(adap);

	return dev->functionality;
}

static void i2c_lx_disable(struct lx_i2c_dev *dev)
{
	int ret;

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return;

	/* Disable controller */
	__i2c_lx_disable(dev);

	/* Disable all interrupts */
	regmap_write(dev->map, LX_IC_INTR_MASK, 0);
	regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_ALL);
	i2c_lx_release_lock(dev);
}


static void i2c_lx_configure_fifo_master(struct lx_i2c_dev *dev)
{
	/* Configure Tx/Rx FIFO threshold levels */
	regmap_write(dev->map, LX_IC_TX_TL, dev->tx_fifo_depth / 2);
	regmap_write(dev->map, LX_IC_RX_TL, 0);

	/* Configure the I2C master */
	regmap_write(dev->map, LX_IC_CTRL, dev->master_cfg);
}

static int i2c_lx_set_timings_master(struct lx_i2c_dev *dev)
{
	u32 sda_falling_time, scl_falling_time;
	struct i2c_timings *t = &dev->timings;
	u32 ic_clk;
	int ret;
	/* Set standard and fast speed dividers for high/low periods */
	sda_falling_time = t->sda_fall_ns ?: 300; /* ns */
	scl_falling_time = t->scl_fall_ns ?: 300; /* ns */

	/* Calculate SCL timing parameters for standard mode if not set */
	if (!dev->ss_hcnt || !dev->ss_lcnt) {
		ic_clk = i2c_lx_clk_rate(dev);
		dev->ss_hcnt =
			i2c_lx_scl_hcnt(dev,
					LX_IC_SCL_HCNT,
					ic_clk,
					4000,	/* tHD;STA = tHIGH = 4.0 us */
					sda_falling_time,
					0,	/* 0: DW default, 1: Ideal */
					0);	/* No offset */
		dev->ss_lcnt =
			i2c_lx_scl_lcnt(dev,
					LX_IC_SCL_LCNT,
					ic_clk,
					4700,	/* tLOW = 4.7 us */
					scl_falling_time,
					0);	/* No offset */
	}
	dev_dbg(dev->dev, "Standard Mode HCNT:LCNT = %d:%d\n",
		dev->ss_hcnt, dev->ss_lcnt);

	/* Check is high speed possible and fall back to fast mode if not */
	if ((dev->master_cfg & LX_IC_CTRL_SPEED_MASK) ==
		LX_IC_CTRL_SPEED_HIGH) {
		if (!dev->hs_hcnt || !dev->hs_lcnt) {
			ic_clk = i2c_lx_clk_rate(dev);
			dev->hs_hcnt =
				i2c_lx_scl_hcnt(dev,
						LX_IC_HS_SCL_HCNT,
						ic_clk,
						160,	/* tHIGH = 160 ns */
						sda_falling_time,
						0,	/* DW default */
						0);	/* No offset */
			dev->hs_lcnt =
				i2c_lx_scl_lcnt(dev,
						LX_IC_HS_SCL_LCNT,
						ic_clk,
						320,	/* tLOW = 320 ns */
						scl_falling_time,
						0);	/* No offset */
		}
		dev_dbg(dev->dev, "High Speed Mode HCNT:LCNT = %d:%d\n",
			dev->hs_hcnt, dev->hs_lcnt);
	}

	ret = i2c_lx_set_sda_hold(dev);
	if (ret)
		return ret;

	dev_dbg(dev->dev, "Bus speed: %s\n", i2c_freq_mode_string(t->bus_freq_hz));
	return 0;
}

/**
 * i2c_lx_init_master() - Initialize the lanxin I2C master hardware
 * @dev: device private data
 *
 * This functions configures and enables the I2C master.
 * This function is called during I2C init function, and in case of timeout at
 * run time.
 */
static int i2c_lx_init_master(struct lx_i2c_dev *dev)
{
	int ret;

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return ret;

	/* Disable the adapter */
	__i2c_lx_disable(dev);

	/* Write standard speed timing parameters */
	regmap_write(dev->map, LX_IC_SCL_HCNT, dev->ss_hcnt);
	regmap_write(dev->map, LX_IC_SCL_LCNT, dev->ss_lcnt);
	/* Write high speed timing parameters if supported */
	if (dev->hs_hcnt && dev->hs_lcnt) {
		regmap_write(dev->map, LX_IC_HS_SCL_HCNT, dev->hs_hcnt);
		regmap_write(dev->map, LX_IC_HS_SCL_LCNT, dev->hs_lcnt);
	}

	/* Write SDA hold time if supported */
	if (dev->sda_hold_time)
		regmap_write(dev->map, LX_IC_SDA_HOLD, dev->sda_hold_time);

	i2c_lx_configure_fifo_master(dev);
	i2c_lx_release_lock(dev);

	return 0;
}

static void i2c_lx_xfer_init(struct lx_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 ic_con = 0, ic_tar = 0;

	/* Disable the adapter */
	__i2c_lx_disable(dev);

	/* If the slave address is ten bit address, enable 10BITADDR */
	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN) {
		ic_con = LX_IC_CTRL_10BITADDR_MASTER;
		/*
		 * If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers.
		 */
		ic_tar = LX_IC_TAR_10BITADDR_MASTER;
	}

	regmap_update_bits(dev->map, LX_IC_CTRL, LX_IC_CTRL_10BITADDR_MASTER,
			   ic_con);

	/*
	 * Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable.
	 */
	regmap_write(dev->map, LX_IC_TAR,
		     msgs[dev->msg_write_idx].addr | ic_tar);

	/* Enforce disabled interrupts (due to HW issues) */
	regmap_write(dev->map, LX_IC_INTR_MASK, 0);

	/* Enable the adapter */
	__i2c_lx_enable(dev);


	/* Clear and enable interrupts */
	regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_ALL);
	regmap_write(dev->map, LX_IC_INTR_MASK, LX_IC_INTR_MASTER_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_lx_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void
i2c_lx_xfer_msg(struct lx_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 intr_mask;
	int tx_limit, rx_limit;
	u32 addr = msgs[dev->msg_write_idx].addr;
	u32 buf_len = dev->tx_buf_len;
	u8 *buf = dev->tx_buf;
	bool need_restart = false;
	unsigned int flr;

	intr_mask = LX_IC_INTR_MASTER_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		u32 flags = msgs[dev->msg_write_idx].flags;

		/*
		 * If target address has changed, we need to
		 * reprogram the target address in the I2C
		 * adapter when we are done with this transfer.
		 */
		if (msgs[dev->msg_write_idx].addr != addr) {
			dev_err(dev->dev,
				"%s: invalid target address\n", __func__);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages.
			 */

			if (dev->msg_write_idx > 0)
				need_restart = true;
		}

		regmap_read(dev->map, LX_IC_TXFLR, &flr);
		tx_limit = dev->tx_fifo_depth - flr;

		regmap_read(dev->map, LX_IC_RXFLR, &flr);
		rx_limit = dev->rx_fifo_depth - flr;

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			u32 cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */

			/*
			 * i2c-core always sets the buffer length of
			 * I2C_FUNC_SMBUS_BLOCK_DATA to 1. The length will
			 * be adjusted when receiving the first byte.
			 * Thus we can't stop the transaction here.
			 */
			if (dev->msg_write_idx == dev->msgs_num - 1 &&
			    buf_len == 1 && !(flags & I2C_M_RECV_LEN))
				cmd |= BIT(9);

			if (need_restart) {
				cmd |= BIT(10);
				need_restart = false;
			}

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {

				/* Avoid rx buffer overrun */
				if (dev->rx_outstanding >= dev->rx_fifo_depth)
					break;

				regmap_write(dev->map, LX_IC_DATA_CMD,
					     cmd | 0x100);
				rx_limit--;
				dev->rx_outstanding++;
			} else {
				regmap_write(dev->map, LX_IC_DATA_CMD,
					     cmd | *buf++);
			}
			tx_limit--; buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		/*
		 * Because we don't know the buffer length in the
		 * I2C_FUNC_SMBUS_BLOCK_DATA case, we can't stop the
		 * transaction here. Also disable the TX_EMPTY IRQ
		 * while waiting for the data length byte to avoid the
		 * bogus interrupts flood.
		 */
		if (flags & I2C_M_RECV_LEN) {
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			intr_mask &= ~LX_IC_INTR_TX_EMPTY;
			break;
		} else if (buf_len > 0) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dev->msg_write_idx == dev->msgs_num)
		intr_mask &= ~LX_IC_INTR_TX_EMPTY;

	if (dev->msg_err)
		intr_mask = 0;

	regmap_write(dev->map,  LX_IC_INTR_MASK, intr_mask);
}

static u8
i2c_lx_recv_len(struct lx_i2c_dev *dev, u8 len)
{
	struct i2c_msg *msgs = dev->msgs;
	u32 flags = msgs[dev->msg_read_idx].flags;

	/*
	 * Adjust the buffer length and mask the flag
	 * after receiving the first byte.
	 */
	len += (flags & I2C_CLIENT_PEC) ? 2 : 1;
	dev->tx_buf_len = len - min_t(u8, len, dev->rx_outstanding);
	msgs[dev->msg_read_idx].len = len;
	msgs[dev->msg_read_idx].flags &= ~I2C_M_RECV_LEN;

	/*
	 * Received buffer length, re-enable TX_EMPTY interrupt
	 * to resume the SMBUS transaction.
	 */
	regmap_update_bits(dev->map, LX_IC_INTR_MASK, LX_IC_INTR_TX_EMPTY,
			   LX_IC_INTR_TX_EMPTY);

	return len;
}

static void
i2c_lx_read(struct lx_i2c_dev *dev)
{
	struct i2c_msg *msgs = dev->msgs;
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

		regmap_read(dev->map, LX_IC_RXFLR, &rx_valid);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			u32 flags = msgs[dev->msg_read_idx].flags;

			regmap_read(dev->map, LX_IC_DATA_CMD, &tmp);
			tmp &= LX_IC_DATA_CMD_DAT;
			/* Ensure length byte is a valid value */
			if (flags & I2C_M_RECV_LEN) {
				/*
				 * if IC_EMPTYFIFO_HOLD_MASTER_EN is set, which cannot be
				 * detected from the registers, the controller can be
				 * disabled if the STOP bit is set. But it is only set
				 * after receiving block data response length in
				 * I2C_FUNC_SMBUS_BLOCK_DATA case. That needs to read
				 * another byte with STOP bit set when the block data
				 * response length is invalid to complete the transaction.
				 */
				if (!tmp || tmp > I2C_SMBUS_BLOCK_MAX)
					tmp = 1;

				len = i2c_lx_recv_len(dev, tmp);
			}
			*buf++ = tmp;
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else
			dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

/*
 * Prepare controller for a transaction and call i2c_lx_xfer_msg.
 */
static int
i2c_lx_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct lx_i2c_dev *dev = i2c_get_adapdata(adap);
	int ret;

	dev_dbg(dev->dev, "%s: msgs: %d\n", __func__, num);

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

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		goto done_nolock;

	ret = i2c_lx_wait_bus_not_busy(dev);
	if (ret < 0)
		goto done;

	/* Start the transfers */
	i2c_lx_xfer_init(dev);

	/* Wait for tx to complete */
	if (!wait_for_completion_timeout(&dev->cmd_complete, adap->timeout)) {
		dev_err(dev->dev, "controller timed out\n");
		/* i2c_lx_init implicitly disables the adapter */
		i2c_recover_bus(&dev->adapter);
		i2c_lx_init_master(dev);
		ret = -ETIMEDOUT;
		goto done;
	}

	/*
	 * We must disable the adapter before returning and signaling the end
	 * of the current transfer. Otherwise the hardware might continue
	 * generating interrupts which in turn causes a race condition with
	 * the following transfer.  Needs some more investigation if the
	 * additional interrupts are a hardware bug or this driver doesn't
	 * handle them correctly yet.
	 */
	__i2c_lx_disable_nowait(dev);

	if (dev->msg_err) {
		ret = dev->msg_err;
		goto done;
	}

	/* No error */
	if (likely(!dev->cmd_err && !dev->status)) {
		ret = num;
		goto done;
	}

	/* We have an error */
	if (dev->cmd_err == LX_IC_ERR_TX_ABRT) {
		ret = i2c_lx_handle_tx_abort(dev);
		goto done;
	}

	if (dev->status)
		dev_err(dev->dev,
			"transfer terminated early - interrupt latency too high?\n");

	ret = -EIO;

done:
	i2c_lx_release_lock(dev);

done_nolock:

	return ret;
}

static const struct i2c_algorithm i2c_lx_algo = {
	.master_xfer = i2c_lx_xfer,
	.functionality = i2c_lx_func,
};

static u32 i2c_lx_read_clear_intrbits(struct lx_i2c_dev *dev)
{
	unsigned int stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * The unmasked raw version of interrupt status bits is available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = readl(IC_INTR_STAT);
	 * equals to,
	 *   stat = readl(IC_RAW_INTR_STAT) & readl(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	regmap_read(dev->map, LX_IC_INTR_STAT, &stat);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * readl(IC_INTR_STAT) to readl(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */

	if (stat & LX_IC_INTR_RX_UNDER)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_RX_UNDER);
	if (stat & LX_IC_INTR_RX_OVER)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_RX_OVER);
	if (stat & LX_IC_INTR_TX_OVER)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_TX_OVER);
	if (stat & LX_IC_INTR_RD_REQ)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_RD_REQ);
	if (stat & LX_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		regmap_read(dev->map, LX_IC_TX_ABRT_SOURCE, &dev->abort_source);
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_TX_TRMNT);
	}
	if (stat & LX_IC_INTR_RX_DONE)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_RX_DONE);
	if (stat & LX_IC_INTR_ACTIVITY)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_ACTIVITY);
	if ((stat & LX_IC_INTR_STOP_DET) &&
	    ((dev->rx_outstanding == 0) || (stat & LX_IC_INTR_RX_FULL))) {
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_STOP_DET);
	}
	if (stat & LX_IC_INTR_START_DET)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_START_DET);
	if (stat & LX_IC_INTR_GEN_CALL)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_GEN_CALL);
	if (stat & LX_IC_INTR_RESTART_DET)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_RESTART_DET);
	if (stat & LX_IC_INTR_MST_ON_HOLD)
		regmap_write(dev->map, LX_IC_INTR_CLR, LX_IC_INTR_CLR_STUCK_DET);

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C master interrupt
 * occurs.
 */
static irqreturn_t i2c_lx_isr(int this_irq, void *dev_id)
{
	struct lx_i2c_dev *dev = dev_id;
	unsigned int stat, enabled;

	regmap_read(dev->map, LX_IC_ENABLE, &enabled);
	regmap_read(dev->map, LX_IC_INTR_RAW_STAT, &stat);
	if (!enabled || !(stat & ~LX_IC_INTR_ACTIVITY))
		return IRQ_NONE;
	dev_dbg(dev->dev, "enabled=%#x stat=%#x\n", enabled, stat);

	stat = i2c_lx_read_clear_intrbits(dev);

	if (!(dev->status & STATUS_ACTIVE)) {
		/*
		 * Unexpected interrupt in driver point of view. State
		 * variables are either unset or stale so acknowledge and
		 * disable interrupts for suppressing further interrupts if
		 * interrupt really came from this HW (E.g. firmware has left
		 * the HW active).
		 */
		regmap_write(dev->map, LX_IC_INTR_MASK, 0);
		return IRQ_HANDLED;
	}

	if (stat & LX_IC_INTR_TX_ABRT) {
		dev->cmd_err |= LX_IC_ERR_TX_ABRT;
		dev->status &= ~STATUS_MASK;
		dev->rx_outstanding = 0;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed. Make sure to skip them.
		 */
		regmap_write(dev->map, LX_IC_INTR_MASK, 0);
		goto tx_aborted;
	}

	if (stat & LX_IC_INTR_RX_FULL)
		i2c_lx_read(dev);

	if (stat & LX_IC_INTR_TX_EMPTY)
		i2c_lx_xfer_msg(dev);

	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_lx_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:
	if (((stat & (LX_IC_INTR_TX_ABRT | LX_IC_INTR_STOP_DET)) || dev->msg_err) &&
	     (dev->rx_outstanding == 0))
		complete(&dev->cmd_complete);
	else if (unlikely(dev->flags & ACCESS_INTR_MASK)) {
		/* Workaround to trigger pending interrupt */
		regmap_read(dev->map, LX_IC_INTR_MASK, &stat);
		regmap_write(dev->map, LX_IC_INTR_MASK, 0);
		regmap_write(dev->map, LX_IC_INTR_MASK, stat);
	}

	return IRQ_HANDLED;
}

static void i2c_lx_configure_master(struct lx_i2c_dev *dev)
{
	struct i2c_timings *t = &dev->timings;

	dev->functionality = I2C_FUNC_10BIT_ADDR | LX_IC_DEFAULT_FUNCTIONALITY;

	dev->master_cfg = LX_IC_CTRL_MASTER;

	dev->mode = LX_IC_MASTER;

	switch (t->bus_freq_hz) {
	case I2C_MAX_STANDARD_MODE_FREQ:
		dev->master_cfg |= LX_IC_CTRL_SPEED_STD;
		break;
	case I2C_MAX_HIGH_SPEED_MODE_FREQ:
		dev->master_cfg |= LX_IC_CTRL_SPEED_HIGH;
		break;
	default:
		dev->master_cfg |= LX_IC_CTRL_SPEED_FAST;
	}
}

static void i2c_lx_prepare_recovery(struct i2c_adapter *adap)
{
	struct lx_i2c_dev *dev = i2c_get_adapdata(adap);

	i2c_lx_disable(dev);
	reset_control_assert(dev->rst);
	i2c_lx_prepare_clk(dev, false);
}

static void i2c_lx_unprepare_recovery(struct i2c_adapter *adap)
{
	struct lx_i2c_dev *dev = i2c_get_adapdata(adap);

	i2c_lx_prepare_clk(dev, true);
	reset_control_deassert(dev->rst);
	i2c_lx_init_master(dev);
}

static int i2c_lx_init_recovery_info(struct lx_i2c_dev *dev)
{
	struct i2c_bus_recovery_info *rinfo = &dev->rinfo;
	struct i2c_adapter *adap = &dev->adapter;
	struct gpio_desc *gpio;

	gpio = devm_gpiod_get_optional(dev->dev, "scl", GPIOD_OUT_HIGH);
	if (IS_ERR_OR_NULL(gpio))
		return PTR_ERR_OR_ZERO(gpio);

	rinfo->scl_gpiod = gpio;

	gpio = devm_gpiod_get_optional(dev->dev, "sda", GPIOD_IN);
	if (IS_ERR(gpio))
		return PTR_ERR(gpio);
	rinfo->sda_gpiod = gpio;

	rinfo->pinctrl = devm_pinctrl_get(dev->dev);
	if (IS_ERR(rinfo->pinctrl)) {
		if (PTR_ERR(rinfo->pinctrl) == -EPROBE_DEFER)
			return PTR_ERR(rinfo->pinctrl);

		rinfo->pinctrl = NULL;
		dev_err(dev->dev, "getting pinctrl info failed: bus recovery might not work\n");
	} else if (!rinfo->pinctrl) {
		dev_dbg(dev->dev, "pinctrl is disabled, bus recovery might not work\n");
	}

	rinfo->recover_bus = i2c_generic_scl_recovery;
	rinfo->prepare_recovery = i2c_lx_prepare_recovery;
	rinfo->unprepare_recovery = i2c_lx_unprepare_recovery;
	adap->bus_recovery_info = rinfo;

	dev_info(dev->dev, "running with gpio recovery mode! scl%s",
		 rinfo->sda_gpiod ? ",sda" : "");

	return 0;
}

static int i2c_lx_probe_master(struct lx_i2c_dev *dev)
{
	struct i2c_adapter *adap = &dev->adapter;
	unsigned long irq_flags;
	unsigned int ic_con;
	int ret;

	init_completion(&dev->cmd_complete);

	dev->init = i2c_lx_init_master;
	dev->disable = i2c_lx_disable;

	ret = i2c_lx_init_regmap(dev);
	if (ret)
		return ret;

	ret = i2c_lx_set_timings_master(dev);
	if (ret)
		return ret;

	ret = i2c_lx_set_fifo_size(dev);
	if (ret)
		return ret;

	/* Lock the bus for accessing LX_IC_CTRL */
	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return ret;

	/*
	 * On AMD platforms BIOS advertises the bus clear feature
	 * and enables the SCL/SDA stuck low. SMU FW does the
	 * bus recovery process. Driver should not ignore this BIOS
	 * advertisement of bus clear feature.
	 */
	ret = regmap_read(dev->map, LX_IC_CTRL, &ic_con);
	i2c_lx_release_lock(dev);
	if (ret)
		return ret;

	if (ic_con & LX_IC_CTRL_BUS_CLEAR_CTRL)
		dev->master_cfg |= LX_IC_CTRL_BUS_CLEAR_CTRL;

	ret = dev->init(dev);
	if (ret)
		return ret;

	snprintf(adap->name, sizeof(adap->name),
		 "Lanxin I2C adapter");
	adap->retries = 3;
	adap->algo = &i2c_lx_algo;
	adap->dev.parent = dev->dev;
	i2c_set_adapdata(adap, dev);

	if (dev->flags & ACCESS_NO_IRQ_SUSPEND) {
		irq_flags = IRQF_NO_SUSPEND;
	} else {
		irq_flags = IRQF_SHARED | IRQF_COND_SUSPEND;
	}

	ret = i2c_lx_acquire_lock(dev);
	if (ret)
		return ret;

	regmap_write(dev->map, LX_IC_INTR_MASK, 0);
	i2c_lx_release_lock(dev);

	ret = devm_request_irq(dev->dev, dev->irq, i2c_lx_isr, irq_flags,
			       dev_name(dev->dev), dev);
	if (ret) {
		dev_err(dev->dev, "failure requesting irq %i: %d\n",
			dev->irq, ret);
		return ret;
	}

	ret = i2c_lx_init_recovery_info(dev);
	if (ret)
		return ret;

	ret = i2c_add_numbered_adapter(adap);
	if (ret)
		dev_err(dev->dev, "failure adding adapter: %d\n", ret);

	return ret;
}

static u32 i2c_lx_get_clk_rate_khz(struct lx_i2c_dev *dev)
{
	return clk_get_rate(dev->clk) / KILO;
}

static const struct acpi_device_id lx_i2c_acpi_match[] = {
	{ "LANX0001", 0 },
	{ }
};
MODULE_DEVICE_TABLE(acpi, lx_i2c_acpi_match);

static int lx_i2c_plat_request_regs(struct lx_i2c_dev *dev)
{
	struct platform_device *pdev = to_platform_device(dev->dev);
	int ret;

	dev->base = devm_platform_ioremap_resource(pdev, 0);
	ret = PTR_ERR_OR_ZERO(dev->base);

	return ret;
}

static int lx_i2c_plat_probe(struct platform_device *pdev)
{
	struct i2c_adapter *adap;
	struct lx_i2c_dev *dev;
	struct i2c_timings *t;
	int irq, ret;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	dev = devm_kzalloc(&pdev->dev, sizeof(struct lx_i2c_dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->flags = (uintptr_t)device_get_match_data(&pdev->dev);

	dev->dev = &pdev->dev;
	dev->irq = irq;
	platform_set_drvdata(pdev, dev);

	ret = lx_i2c_plat_request_regs(dev);
	if (ret)
		return ret;

	dev->rst = devm_reset_control_get_optional_exclusive(&pdev->dev, NULL);
	if (IS_ERR(dev->rst))
		return PTR_ERR(dev->rst);

	reset_control_deassert(dev->rst);

	t = &dev->timings;
	i2c_parse_fw_timings(&pdev->dev, t, false);

	i2c_lx_adjust_bus_speed(dev);

	ret = i2c_lx_validate_speed(dev);
	if (ret)
		goto exit_reset;

	i2c_lx_configure_master(dev);

	/* Optional interface clock */
	dev->pclk = devm_clk_get_optional(&pdev->dev, "pclk");
	if (IS_ERR(dev->pclk)) {
		ret = PTR_ERR(dev->pclk);
		goto exit_reset;
	}

	dev->clk = devm_clk_get_optional(&pdev->dev, NULL);
	if (IS_ERR(dev->clk)) {
		ret = PTR_ERR(dev->clk);
		goto exit_reset;
	}

	ret = i2c_lx_prepare_clk(dev, true);
	if (ret)
		goto exit_reset;

	if (dev->clk) {
		u64 clk_khz;

		dev->get_clk_rate_khz = i2c_lx_get_clk_rate_khz;
		clk_khz = dev->get_clk_rate_khz(dev);

		if (!dev->sda_hold_time && t->sda_hold_ns)
			dev->sda_hold_time =
				DIV_S64_ROUND_CLOSEST(clk_khz * t->sda_hold_ns, MICRO);
	}

	adap = &dev->adapter;
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_DEPRECATED;
	ACPI_COMPANION_SET(&adap->dev, ACPI_COMPANION(&pdev->dev));
	adap->dev.of_node = pdev->dev.of_node;
	adap->nr = -1;

	if (dev->flags & ACCESS_NO_IRQ_SUSPEND) {
		dev_pm_set_driver_flags(&pdev->dev,
					DPM_FLAG_SMART_PREPARE);
	} else {
		dev_pm_set_driver_flags(&pdev->dev,
					DPM_FLAG_SMART_PREPARE |
					DPM_FLAG_SMART_SUSPEND);
	}

	device_enable_async_suspend(&pdev->dev);

	ret = i2c_lx_probe_master(dev);
	if (ret)
		goto exit_reset;

	return ret;

exit_reset:
	reset_control_assert(dev->rst);
	return ret;
}

static void lx_i2c_plat_remove(struct platform_device *pdev)
{
	struct lx_i2c_dev *dev = platform_get_drvdata(pdev);

	i2c_del_adapter(&dev->adapter);

	dev->disable(dev);

	reset_control_assert(dev->rst);
}

static struct platform_driver lx_i2c_driver = {
	.probe = lx_i2c_plat_probe,
	.remove_new = lx_i2c_plat_remove,
	.driver		= {
		.name	= "i2c_lanxin",
		.acpi_match_table = ACPI_PTR(lx_i2c_acpi_match),
	},
};

static int __init lx_i2c_init_driver(void)
{
	return platform_driver_register(&lx_i2c_driver);
}
subsys_initcall(lx_i2c_init_driver);

static void __exit lx_i2c_exit_driver(void)
{
	platform_driver_unregister(&lx_i2c_driver);
}
module_exit(lx_i2c_exit_driver);

MODULE_DESCRIPTION("Lanxin I2C Controller Driver");
MODULE_LICENSE("GPL");
