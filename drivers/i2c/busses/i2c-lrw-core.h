/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  I2C adapter driver for LRW
 *
 *  Copyright (c) 2025, LRW CORPORATION. All rights reserved.
 */

#include <linux/bits.h>
#include <linux/compiler_types.h>
#include <linux/completion.h>
#include <linux/dev_printk.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/types.h>

#define LRW_IC_DEFAULT_FUNCTIONALITY							\
	(I2C_FUNC_I2C | I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | \
	I2C_FUNC_SMBUS_WORD_DATA | I2C_FUNC_SMBUS_BLOCK_DATA |			\
	I2C_FUNC_SMBUS_I2C_BLOCK)

#define I2C_TXFLR_MASK 0xFF
#define I2C_RXFLR_MASK 0xFF
#define I2C_RXFLR_SHIFT 16
#define I2C_FIFO_DEPTH 32

#define LRW_IC_CON_MASTER BIT(0)
#define LRW_IC_CON_SPEED_STD_FAST (0 << 1)
#define LRW_IC_CON_SPEED_HIGH (1 << 1)
#define LRW_IC_CON_SPEED_MASK BIT(1)
#define LRW_IC_CON_10BITADDR_SLAVE BIT(2)
#define LRW_IC_CON_10BITADDR_MASTER BIT(3)
#define LRW_IC_CON_RESTART_EN BIT(4)
#define LRW_IC_CON_STOP_DET_IFADDRESSED BIT(5)
#define LRW_IC_CON_TX_EMPTY_CTRL BIT(6)
#define LRW_IC_CON_RX_FIFO_FULL_HLD_CTRL BIT(7)
#define IC_CON_BUS_CLEAR_CTRL_POS BIT(8)

#define LRW_IC_DATA_CMD_DAT GENMASK(7, 0)

/*
 * Registers offset
 */
#define LRW_IC_VERSION 0x0
#define LRW_IC_CON 0x04
#define LRW_IC_ENABLE 0x08
#define LRW_IC_DATA_CMD 0x0c

#define LRW_IC_FS_SCL_HCNT 0x10
#define LRW_IC_FS_SCL_LCNT 0x14
#define LRW_IC_FS_SPKLEN 0x18
#define LRW_IC_HS_SCL_HCNT 0x1c
#define LRW_IC_HS_SCL_LCNT 0x20
#define LRW_IC_HS_SPKLEN 0x24
#define LRW_IC_HS_CTRCODE 0x28
#define LRW_IC_TAR 0x2c
#define LRW_IC_SAR1 0x30
#define LRW_IC_SAR2 0x34
#define LRW_IC_SAR3 0x38
#define LRW_IC_SAR4 0x3c
#define LRW_IC_RX_TL 0x40
#define LRW_IC_TX_TL 0x44
#define LRW_IC_FIFO_LEVEL 0x48
#define LRW_IC_TX_ABRT_SOURCE 0x4c
#define LRW_IC_CLR_RAW_INTR 0x50
#define LRW_IC_INTR_MASK 0x54
#define LRW_IC_INTR_STAT 0x58
#define LRW_IC_DMA_CTRL 0x5c
#define LRW_IC_SDA_SETUP 0x60
#define LRW_IC_SDA_HOLD 0x64
#define LRW_IC_BUS_FREE 0x68

#define LRW_IC_START_HOLD 0x6c
#define LRW_IC_RESTART_HOLD_HS 0x70
#define LRW_IC_RESTART_SETUP 0x74
#define LRW_IC_STOP_SETUP 0x78

#define LRW_IC_SCL_STUCK_AT_LOW_TIMEOUT_MIN 0x7c
#define LRW_IC_SCL_STUCK_AT_LOW_TIMEOUT_MAX 0x80
#define LRW_IC_SDA_STUCK_AT_LOW_TIMEOUT 0x84
#define LRW_IC_REG_TIMEOUT_RST 0x88
#define LRW_IC_STATUS 0x8c
#define LRW_IC_DEBUG 0x90

#define LRW_SMBUS_CLR_RAW_INTR 0xa4
#define LRW_SMBUS_INTR_MASK 0xa8
#define LRW_SMBUS_INTR_STAT 0xac

#define LRW_IC_CLR_INTR BIT(31)

#define LRW_IC_SDA_HOLD_MIN_VERS 0x1C240615
#define LRW_IC_INTR_RX_OVER BIT(0)
#define LRW_IC_INTR_RX_FULL BIT(1)
#define LRW_IC_INTR_TX_EMPTY BIT(2)
#define LRW_IC_INTR_RD_REQ BIT(3)
#define LRW_IC_INTR_TX_ABRT BIT(4)
#define LRW_IC_INTR_RX_DONE BIT(5)
#define LRW_IC_INTR_ACTIVITY BIT(6)
#define LRW_IC_INTR_STOP_DET BIT(7)
#define LRW_IC_INTR_START_DET BIT(8)
#define LRW_IC_INTR_GEN_CALL BIT(9)
#define LRW_IC_INTR_RESTART_DET BIT(10)
#define LRW_IC_INTR_MST_ON_HOLD BIT(11)

#define LRW_IC_INTR_DEFAULT_MASK \
	(LRW_IC_INTR_RX_FULL | LRW_IC_INTR_TX_ABRT | LRW_IC_INTR_STOP_DET)
#define LRW_IC_INTR_MASTER_MASK \
	(LRW_IC_INTR_DEFAULT_MASK | LRW_IC_INTR_TX_EMPTY)
#define LRW_IC_INTR_SLAVE_MASK \
	(LRW_IC_INTR_DEFAULT_MASK | LRW_IC_INTR_RX_DONE | LRW_IC_INTR_RD_REQ)

#define LRW_IC_ENABLE_ABORT BIT(1)

#define LRW_IC_STATUS_ACTIVITY 0x1
#define LRW_IC_STATUS_TFE BIT(2)
#define LRW_IC_STATUS_MASTER_ACTIVITY BIT(5)
#define LRW_IC_STATUS_SLAVE_ACTIVITY BIT(6)

#define LRW_IC_SDA_HOLD_RX_SHIFT 16
#define LRW_IC_SDA_HOLD_RX_MASK GENMASK(23, LRW_IC_SDA_HOLD_RX_SHIFT)

#define LRW_IC_ERR_TX_ABRT 0x1
#define I2C_SCL_TIMEOUT_ERROR 0x2

#define LRW_IC_TAR_10BITADDR_MASTER BIT(12)

#define IC_INTR_SCL_STUCK_AT_LOW_MIN BIT(12)
/*IC_TX_ABRT_SOURCE*/
#define IC_ABRT_SDA_STUCK_AT_LOW_POS BIT(17)
/* IC_ENABLE */
#define IC_SDA_STUCK_RECOVERY_ENABLE_POS BIT(3)
#define IC_SDA_STUCK_AT_LOW_RECOVERIED 0x0

/*
 * Sofware status flags
 */
#define STATUS_ACTIVE BIT(0)
#define STATUS_WRITE_IN_PROGRESS BIT(1)
#define STATUS_READ_IN_PROGRESS BIT(2)
#define STATUS_MASK GENMASK(2, 0)

/*
 * operation modes
 */
#define LRW_IC_MASTER 0
#define LRW_IC_SLAVE 1

/*
 * Hardware abort codes from the LRW_IC_TX_ABRT_SOURCE register
 *
 * Only expected abort codes are listed here
 * refer to the datasheet for the full list
 */
#define ABRT_7B_ADDR_NOACK 0
#define ABRT_10ADDR1_NOACK 1
#define ABRT_10ADDR2_NOACK 2
#define ABRT_TXDATA_NOACK 3
#define ABRT_GCALL_NOACK 4
#define ABRT_GCALL_READ 5
#define ABRT_HS_ACK_DET 6
#define ABRT_SBYTE_ACKDET 7
#define ABRT_HS_NORSTRT 8
#define ABRT_SBYTE_NORSTRT 9
#define ABRT_10B_RD_NORSTRT 10
#define ABRT_MASTER_DIS 11
#define ARB_LOST 12
#define ABRT_SLAVE_FLUSH_TXFIFO 13
#define ABRT_SLAVE_ARBLOST 14
#define ABRT_SLAVE_RD_INTX 15
#define ABRT_USER_ABRT 16
#define ABRT_SDA_STUCK_AT_LOW 17

#define LRW_IC_TX_ABRT_7B_ADDR_NOACK BIT(ABRT_7B_ADDR_NOACK)
#define LRW_IC_TX_ABRT_10ADDR1_NOACK BIT(ABRT_10ADDR1_NOACK)
#define LRW_IC_TX_ABRT_10ADDR2_NOACK BIT(ABRT_10ADDR2_NOACK)
#define LRW_IC_TX_ABRT_TXDATA_NOACK BIT(ABRT_TXDATA_NOACK)
#define LRW_IC_TX_ABRT_GCALL_NOACK BIT(ABRT_GCALL_NOACK)
#define LRW_IC_TX_ABRT_GCALL_READ BIT(ABRT_GCALL_READ)
#define LRW_IC_TX_ABRT_SBYTE_ACKDET BIT(ABRT_SBYTE_ACKDET)
#define LRW_IC_TX_ABRT_SBYTE_NORSTRT BIT(ABRT_SBYTE_NORSTRT)
#define LRW_IC_TX_ABRT_10B_RD_NORSTRT BIT(ABRT_10B_RD_NORSTRT)
#define LRW_IC_TX_ABRT_MASTER_DIS BIT(ABRT_MASTER_DIS)
#define LRW_IC_TX_ARB_LOST BIT(ARB_LOST)
#define LRW_IC_TX_ABRT_SDA_STUCK_AT_LOW BIT(ABRT_SDA_STUCK_AT_LOW)
#define LRW_IC_RX_ABRT_SLAVE_RD_INTX BIT(ABRT_SLAVE_RD_INTX)
#define LRW_IC_RX_ABRT_SLAVE_ARBLOST BIT(ABRT_SLAVE_ARBLOST)
#define LRW_IC_RX_ABRT_SLAVE_FLUSH_TXFIFO BIT(ABRT_SLAVE_FLUSH_TXFIFO)

#define LRW_IC_TX_ABRT_NOACK								\
	(LRW_IC_TX_ABRT_7B_ADDR_NOACK | LRW_IC_TX_ABRT_10ADDR1_NOACK | \
	LRW_IC_TX_ABRT_10ADDR2_NOACK | LRW_IC_TX_ABRT_TXDATA_NOACK | \
	LRW_IC_TX_ABRT_GCALL_NOACK)

struct clk;
struct device;
struct reset_control;

/**
 * struct lrw_i2c_dev - private i2c-lrw data
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
 * @tx_fifo_depth: depth of the hardware tx fifo
 * @rx_fifo_depth: depth of the hardware rx fifo
 * @rx_outstanding: current master-rx elements in tx fifo
 * @timings: bus clock frequency, SDA hold and other timings
 * @sda_hold_time: SDA hold value
 * @fs_hcnt: fast speed HCNT value
 * @fs_lcnt: fast speed LCNT value
 * @hs_hcnt: high speed HCNT value
 * @hs_lcnt: high speed LCNT value
 * @acquire_lock: function to acquire a hardware lock on the bus
 * @release_lock: function to release a hardware lock on the bus
 * @semaphore_idx: Index of table with semaphore type attached to the bus. It's
 *	-1 if there is no semaphore.
 * @shared_with_punit: true if this bus is shared with the SoCs PUNIT
 * @disable: function to disable the controller
 * @init: function to initialize the I2C hardware
 * @set_sda_hold_time: callback to retrieve IP specific SDA hold timing
 * @rinfo: I²C GPIO recovery information
 *
 * HCNT and LCNT parameters can be used if the platform knows more accurate
 * values than the one computed based only on the input clock frequency.
 * Leave them to be %0 if not used.
 */
struct i2c_lrw_dev {
	struct device *dev;
	struct regmap *map;
	struct regmap *sysmap;
	void __iomem *base;
	void __iomem *ext;
	struct completion cmd_complete;
	struct clk *clk;
	struct clk *pclk;
	struct reset_control *rst;
	struct i2c_client *slave;
	u32 (*get_clk_rate_khz)(struct i2c_lrw_dev *dev);
	int cmd_err;
	struct i2c_msg *msgs;
	int msgs_num;
	int msg_write_idx;
	u32 tx_buf_len;
	u8 *tx_buf;
	int msg_read_idx;
	u32 rx_buf_len;
	u8 *rx_buf;
	int msg_err;
	unsigned int status;
	unsigned int abort_source;
	int irq;
	u32 flags;
	struct i2c_adapter adapter;
	u32 functionality;
	u32 master_cfg;
	unsigned int tx_fifo_depth;
	unsigned int rx_fifo_depth;
	int rx_outstanding;
	struct i2c_timings timings;
	u32 sda_hold_time;
	u32 sda_stuck_at_low_timeout;
	u16 fs_hcnt;
	u16 fs_lcnt;
	u16 hs_hcnt;
	u16 hs_lcnt;
	int (*acquire_lock)(void);
	void (*release_lock)(void);
	int semaphore_idx;
	void (*disable)(struct i2c_lrw_dev *dev);
	int (*init)(struct i2c_lrw_dev *dev);
	int (*set_sda_hold_time)(struct i2c_lrw_dev *dev);
	struct i2c_bus_recovery_info rinfo;
};

#define MODEL_MASK GENMASK(11, 8)

int i2c_lrw_prepare_clk(struct i2c_lrw_dev *dev, bool prepare);

static inline void __i2c_lrw_enable(struct i2c_lrw_dev *dev)
{
	dev->status |= STATUS_ACTIVE;
	regmap_write(dev->map, LRW_IC_ENABLE, 1);
}

static inline void __i2c_lrw_disable_nowait(struct i2c_lrw_dev *dev)
{
	regmap_write(dev->map, LRW_IC_ENABLE, 0);
	dev->status &= ~STATUS_ACTIVE;
}

void __i2c_lrw_disable(struct i2c_lrw_dev *dev);

extern void i2c_lrw_configure_master(struct i2c_lrw_dev *dev);
extern int i2c_lrw_probe_master(struct i2c_lrw_dev *dev);

static inline int i2c_lrw_probe(struct i2c_lrw_dev *dev)
{
	return i2c_lrw_probe_master(dev);
}

static inline void i2c_lrw_configure(struct i2c_lrw_dev *dev)
{
	i2c_lrw_configure_master(dev);
}

static inline int i2c_lrw_probe_lock_support(struct i2c_lrw_dev *dev)
{
	return 0;
}

int i2c_lrw_validate_speed(struct i2c_lrw_dev *dev);
void i2c_lrw_adjust_bus_speed(struct i2c_lrw_dev *dev);

#if IS_ENABLED(CONFIG_ACPI)
int i2c_lrw_acpi_configure(struct device *device);
static inline int i2c_lrw_dt_configure(struct device *device)
{
	return -ENODEV;
}
#else
static inline int i2c_lrw_acpi_configure(struct device *device)
{
	return -ENODEV;
}
int i2c_lrw_dt_configure(struct device *device);
#endif
