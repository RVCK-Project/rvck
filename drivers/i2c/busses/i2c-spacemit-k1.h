/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Spacemit i2c driver header file
 *
 * Copyright (c) 2023, spacemit Corporation.
 *
 */

#ifndef I2C_SPACEMIT_K1X_H
#define I2C_SPACEMIT_K1X_H
#include <linux/bitops.h>
#include <linux/i2c.h>
#include <linux/reset.h>
#include <linux/i2c-dev.h>
#include <linux/pm_qos.h>

/* spacemit i2c registers */
enum {
	REG_CR		= 0x0,		/* Control Register */
	REG_SR		= 0x4,		/* Status Register */
	REG_SAR		= 0x8,		/* Slave Address Register */
	REG_DBR		= 0xc,		/* Data Buffer Register */
	REG_LCR		= 0x10,		/* Load Count Register */
	REG_WCR		= 0x14,		/* Wait Count Register */
	REG_RST_CYC	= 0x18,		/* Bus reset cycle counter */
	REG_BMR		= 0x1c,		/* Bus monitor register */
	REG_WFIFO	= 0x20,		/* Write FIFO Register */
	REG_WFIFO_WPTR	= 0x24,		/* Write FIFO Write Pointer Register */
	REG_WFIFO_RPTR	= 0x28,		/* Write FIFO Read Pointer Register */
	REG_RFIFO	= 0x2c,		/* Read FIFO Register */
	REG_RFIFO_WPTR	= 0x30,		/* Read FIFO Write Pointer Register */
	REG_RFIFO_RPTR	= 0x34,		/* Read FIFO Read Pointer Register */
};

/* register REG_CR fields */
enum {
	CR_START	= BIT(0),	/* start bit */
	CR_STOP		= BIT(1),	/* stop bit */
	CR_ACKNAK	= BIT(2),	/* send ACK(0) or NAK(1) */
	CR_TB		= BIT(3),	/* transfer byte bit */
	CR_TXBEGIN	= BIT(4),	/* transaction begin */
	CR_FIFOEN	= BIT(5),	/* enable FIFO mode */
	CR_GPIOEN	= BIT(6),	/* enable GPIO mode for SCL in HS */
	CR_DMAEN	= BIT(7),	/* enable DMA for TX and RX FIFOs */
	CR_MODE_FAST	= BIT(8),	/* bus mode (master operation) */
	CR_MODE_HIGH	= BIT(9),	/* bus mode (master operation) */
	CR_UR		= BIT(10),	/* unit reset */
	CR_RSTREQ	= BIT(11),	/* i2c bus reset request */
	CR_MA		= BIT(12),	/* master abort */
	CR_SCLE		= BIT(13),	/* master clock enable */
	CR_IUE		= BIT(14),	/* unit enable */
	CR_HS_STRETCH	= BIT(16),	/* I2C hs stretch */
	CR_ALDIE	= BIT(18),	/* enable arbitration interrupt */
	CR_DTEIE	= BIT(19),	/* enable tx interrupts */
	CR_DRFIE	= BIT(20),	/* enable rx interrupts */
	CR_GCD		= BIT(21),	/* general call disable */
	CR_BEIE		= BIT(22),	/* enable bus error ints */
	CR_SADIE	= BIT(23),	/* slave address detected int enable */
	CR_SSDIE	= BIT(24),	/* slave STOP detected int enable */
	CR_MSDIE	= BIT(25),	/* master STOP detected int enable */
	CR_MSDE		= BIT(26),	/* master STOP detected enable */
	CR_TXDONEIE	= BIT(27),	/* transaction done int enable */
	CR_TXEIE	= BIT(28),	/* transmit FIFO empty int enable */
	CR_RXHFIE	= BIT(29),	/* receive FIFO half-full int enable */
	CR_RXFIE	= BIT(30),	/* receive FIFO full int enable */
	CR_RXOVIE	= BIT(31),	/* receive FIFO overrun int enable */
};

/* register REG_SR fields */
enum {
	SR_RWM		= BIT(13),	/* read/write mode */
	SR_ACKNAK	= BIT(14),	/* ACK/NACK status */
	SR_UB		= BIT(15),	/* unit busy */
	SR_IBB		= BIT(16),	/* i2c bus busy */
	SR_EBB		= BIT(17),	/* early bus busy */
	SR_ALD		= BIT(18),	/* arbitration loss detected */
	SR_ITE		= BIT(19),	/* tx buffer empty */
	SR_IRF		= BIT(20),	/* rx buffer full */
	SR_GCAD		= BIT(21),	/* general call address detected */
	SR_BED		= BIT(22),	/* bus error no ACK/NAK */
	SR_SAD		= BIT(23),	/* slave address detected */
	SR_SSD		= BIT(24),	/* slave stop detected */
	SR_MSD		= BIT(26),	/* master stop detected */
	SR_TXDONE	= BIT(27),	/* transaction done */
	SR_TXE		= BIT(28),	/* tx FIFO empty */
	SR_RXHF		= BIT(29),	/* rx FIFO half-full */
	SR_RXF		= BIT(30),	/* rx FIFO full */
	SR_RXOV		= BIT(31),	/* RX FIFO overrun */
};

/* register REG_LCR fields */
enum {
	LCR_SLV		= 0x000001FF,	/* SLV: bit[8:0] */
	LCR_FLV		= 0x0003FE00,	/* FLV: bit[17:9] */
	LCR_HLVH	= 0x07FC0000,	/* HLVH: bit[26:18] */
	LCR_HLVL	= 0xF8000000,	/* HLVL: bit[31:27] */
};

/* register REG_WCR fields */
enum {
	WCR_COUNT	= 0x0000001F,	/* COUNT: bit[4:0] */
	WCR_COUNT1	= 0x000003E0,	/* HS_COUNT1: bit[9:5] */
	WCR_COUNT2	= 0x00007C00,	/* HS_COUNT2: bit[14:10] */
};

/* register REG_BMR fields */
enum {
	BMR_SDA		= BIT(0),	/* SDA line level */
	BMR_SCL		= BIT(1),	/* SCL line level */
};

/* register REG_WFIFO fields */
enum {
	WFIFO_DATA_MSK		= 0x000000FF,	/* data: bit[7:0] */
	WFIFO_CTRL_MSK		= 0x000003E0,	/* control: bit[11:8] */
	WFIFO_CTRL_START	= BIT(8),	/* start bit */
	WFIFO_CTRL_STOP		= BIT(9),	/* stop bit */
	WFIFO_CTRL_ACKNAK	= BIT(10),	/* send ACK(0) or NAK(1) */
	WFIFO_CTRL_TB		= BIT(11),	/* transfer byte bit */
};

/* status register init value */
enum {
	SPACEMIT_I2C_INT_STATUS_MASK	= 0xfffc0000,	/* SR bits[31:18] */
	SPACEMIT_I2C_INT_CTRL_MASK	= (CR_ALDIE | CR_DTEIE | CR_DRFIE |
				CR_BEIE | CR_TXDONEIE | CR_TXEIE |
				CR_RXHFIE | CR_RXFIE | CR_RXOVIE |
				CR_MSDIE),
};

/* i2c transfer mode */
enum spacemit_i2c_xfer_mode {
	SPACEMIT_I2C_MODE_INTERRUPT,
	SPACEMIT_I2C_MODE_PIO,
	SPACEMIT_I2C_MODE_INVALID,
};

/* i2c transfer phase during transaction */
enum spacemit_i2c_xfer_phase {
	SPACEMIT_I2C_XFER_MASTER_CODE,
	SPACEMIT_I2C_XFER_SLAVE_ADDR,
	SPACEMIT_I2C_XFER_BODY,
	SPACEMIT_I2C_XFER_IDLE,
};

/* i2c controller FIFO depth */
#define SPACEMIT_I2C_RX_FIFO_DEPTH		(8)
#define SPACEMIT_I2C_TX_FIFO_DEPTH		(8)

/* i2c bus recover timeout: us */
#define SPACEMIT_I2C_BUS_RECOVER_TIMEOUT	(100000)

/* i2c bus active timeout: us */
#define SPACEMIT_I2C_BUS_ACTIVE_TIMEOUT		(100000)

#define SPACEMIT_I2C_APB_CLOCK_26M		(26000000)
#define SPACEMIT_I2C_APB_CLOCK_52M		(52000000)

/* i2c-spacemit driver's main struct */
struct spacemit_i2c_dev {
	struct device		*dev;
	struct i2c_adapter	adapt;
	struct i2c_msg		*msgs;
	int			num;
	struct resource		resrc;
	struct mutex		mtx;
	spinlock_t		fifo_lock;
	int			drv_retries;

	/* virtual base address mapped for register */
	void __iomem		*mapbase;

	struct reset_control    *resets;
	struct clk		*clk;
	int			irq;
	int			clk_freq_in;
	int			clk_freq_out;
	bool			clk_always_on;

	/* i2c speed mode selection */
	bool			fast_mode;
	bool			high_mode;

	/* master code for high-speed mode */
	u8			master_code;
	u32			clk_rate;
	u32			i2c_lcr;
	u32			i2c_wcr;

	bool			shutdown;

	struct pinctrl		*pinctrl;
	struct pinctrl_state	*pin_i2c_ap;
	struct pinctrl_state	*pin_i2c_cp;
	struct pinctrl_state	*pin_gpio;

	/* slave address with read/write flag */
	u32			slave_addr_rw;

	struct i2c_msg		*cur_msg;
	int			msg_idx;
	u8			*msg_buf;
	bool			is_rx;
	size_t			rx_cnt;
	size_t			tx_cnt;
	bool			is_xfer_start;
	int			rx_total;
	bool			smbus_rcv_len;

	struct completion	complete;
	u32			timeout;
	enum spacemit_i2c_xfer_mode	xfer_mode;
	enum spacemit_i2c_xfer_phase	phase;
	u32			i2c_ctrl_reg_value;
	u32			i2c_status;
	u32			i2c_err;

	/* hwlock address */
	void __iomem            *hwlock_addr;

	/*  apb clock */
	u32			apb_clock;
};

#endif /* I2C_SPACEMIT_K1X_H */
