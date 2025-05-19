/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for Spacemit k1 spi controller
 *
 * Copyright (c) 2023, spacemit Corporation.
 *
 */

#ifndef _SPI_SPACEMIT_K1_H
#define _SPI_SPACEMIT_K1_H

#include <linux/atomic.h>
#include <linux/dmaengine.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/sizes.h>
#include <linux/spi/spi.h>
#include <linux/pm_qos.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/reset.h>

/* Spacemit k1 SPI Registers */
#define TOP_CTRL		0x00	/* SPI Top Control Register */
#define FIFO_CTRL		0x04	/* SPI FIFO Control Register */
#define INT_EN			0x08	/* SPI Interrupt Enable Register */
#define TO			0x0C	/* SPI Time Out Register */
#define DATAR			0x10	/* SPI Data Register */
#define STATUS			0x14	/* SPI Stauts Register */
/* SPI Programmable Serial Protocal Control Register */
#define PSP_CTRL		0x18
#define NET_WORK_CTRL		0x1C	/* SPI NET Work Control Register */
#define NET_WORK_STATUS		0x20	/* SPI Net Work Status Register */
#define RWOT_CTRL		0x24	/* SPI RWOT Control Register */
#define RWOT_CCM		0x28	/* SPI RWOT Counter Cycles Match Register */
/* SPI RWOT Counter Value Write for Read Request Register */
#define RWOT_CVWRn		0x2C

/* 0x00 TOP_CTRL */
#define TOP_TTELP		(1 << 18)
#define TOP_TTE			(1 << 17)
#define TOP_SCFR		(1 << 16)
#define TOP_IFS			(1 << 15)
#define TOP_HOLD_FRAME_LOW	(1 << 14)
#define TOP_TRAIL		(1 << 13)
#define TOP_LBM			(1 << 12)
#define TOP_SPH			(1 << 11)
#define TOP_SPO			(1 << 10)
#define TOP_DSS(x)		((x - 1) << 5)
#define TOP_DSS_MASK		(0x1F << 5)
#define TOP_SFRMDIR		(1 << 4)
#define TOP_SCLKDIR		(1 << 3)
#define TOP_FRF_MASK		(0x3 << 1)
/* Motorola's Serial Peripheral Interface (SPI) */
#define TOP_FRF_Motorola	(0x0 << 1)
/* Texas Instruments' Synchronous Serial Protocol (SSP) */
#define TOP_FRF_TI		(0x1 << 1)
#define TOP_FRF_National	(0x2 << 1)	/* National Microwire */
#define TOP_FRF_PSP		(0x3 << 1)	/* Programmable Serial Protocol(PSP) */
#define TOP_SSE			(1 << 0)

/* 0x04 FIFO_CTRL */
#define FIFO_STRF			(1 << 19)
#define FIFO_EFWR			(1 << 18)
#define FIFO_RXFIFO_AUTO_FULL_CTRL	(1 << 17)
#define FIFO_FPCKE			(1 << 16)
#define FIFO_TXFIFO_WR_ENDIAN_MASK	(0x3 << 14)
#define FIFO_RXFIFO_RD_ENDIAN_MASK	(0x3 << 12)
#define FIFO_WR_ENDIAN_16BITS		(1 << 14)	/* Swap first 16 bits and last 16 bits */
#define FIFO_WR_ENDIAN_8BITS		(2 << 14)	/* Swap all 4 bytes */
#define FIFO_RD_ENDIAN_16BITS		(1 << 12)	/* Swap first 16 bits and last 16 bits */
#define FIFO_RD_ENDIAN_8BITS		(2 << 12)	/* Swap all 4 bytes */
#define FIFO_RSRE			(1 << 11)
#define FIFO_TSRE			(1 << 10)

/* 0x08 INT_EN */
#define INT_EN_EBCEI		(1 << 6)
#define INT_EN_TIM		(1 << 5)
#define INT_EN_RIM		(1 << 4)
#define INT_EN_TIE		(1 << 3)
#define INT_EN_RIE		(1 << 2)
#define INT_EN_TINTE		(1 << 1)
#define INT_EN_PINTE		(1 << 0)

/* 0x0C TO */
#define TIMEOUT(x)	((x) << 0)

/* 0x10 DATAR */
#define DATA(x)		((x) << 0)

/* 0x14 STATUS */
#define STATUS_OSS		(1 << 23)
#define STATUS_TX_OSS		(1 << 22)
#define STATUS_BCE		(1 << 21)
#define STATUS_ROR		(1 << 20)
#define STATUS_RNE		(1 << 14)
#define STATUS_RFS		(1 << 13)
#define STATUS_TUR		(1 << 12)
#define STATUS_TNF		(1 << 6)
#define STATUS_TFS		(1 << 5)
#define STATUS_EOC		(1 << 4)
#define STATUS_TINT		(1 << 3)
#define STATUS_PINT		(1 << 2)
#define STATUS_CSS		(1 << 1)
#define STATUS_BSY		(1 << 0)

/* 0x18 PSP_CTRL */
#define PSP_EDMYSTOP(x)		((x) << 27)
#define PSP_EMYSTOP(x)		((x) << 25)
#define PSP_EDMYSTRT(x)		((x) << 23)
#define PSP_DMYSTRT(x)		((x) << 21)
#define PSP_STRTDLY(x)		((x) << 18)
#define PSP_SFRMWDTH(x)		((x) << 12)
#define PSP_SFRMDLY(x)		((x) << 5)
#define PSP_SFRMP		(1 << 4)
#define PSP_FSRT		(1 << 3)
#define PSP_ETDS		(1 << 2)
#define PSP_SCMODE(x)		((x) << 0)

/* 0x1C NET_WORK_CTRL */
#define RTSA(x)			((x) << 12)
#define RTSA_MASK		(0xFF << 12)
#define TTSA(x)			((x) << 4)
#define TTSA_MASK		(0xFF << 4)
#define NET_FRDC(x)		((x) << 1)
#define NET_WORK_MODE		(1 << 0)

/* 0x20 NET_WORK_STATUS */
#define NET_SATUS_NMBSY		(1 << 3)
#define NET_STATUS_TSS(x)	((x) << 0)

/* 0x24 RWOT_CTRL */
#define RWOT_MASK_RWOT_LAST_SAMPLE	(1 << 4)
#define RWOT_CLR_RWOT_CYCLE		(1 << 3)
#define RWOT_SET_RWOT_CYCLE		(1 << 2)
#define RWOT_CYCLE_RWOT_EN		(1 << 1)
#define RWOT_RWOT			(1 << 0)

struct spi_driver_data {
	/* Driver model hookup */
	struct platform_device *pdev;

	/* SPI framework hookup */
	struct spi_master *master;

	/* k1 hookup */
	struct k1_spi_master *master_info;

	/* spi register addresses */
	void __iomem *ioaddr;
	u32 ssdr_physical;

	/* spi masks*/
	u32 dma_fifo_ctrl;
	u32 dma_top_ctrl;
	u32 int_cr;
	u32 clear_sr;
	u32 mask_sr;

	/* Message Transfer pump */
	struct work_struct pump_transfers;

	/* DMA engine support */
	struct dma_chan *rx_chan;
	struct dma_chan *tx_chan;
	struct sg_table rx_sgt;
	struct sg_table tx_sgt;
	int rx_nents;
	int tx_nents;
	void *dummy;
	atomic_t dma_running;

	/* Current message transfer state info */
	struct spi_message *cur_msg;
	struct spi_transfer *cur_transfer;
	struct chip_data *cur_chip;
	struct completion   cur_msg_completion;
	size_t len;
	void *tx;
	void *tx_end;
	void *rx;
	void *rx_end;
	int dma_mapped;
	dma_addr_t rx_dma;
	dma_addr_t tx_dma;
	size_t rx_map_len;
	size_t tx_map_len;
	u8 n_bytes;
	int (*write)(struct spi_driver_data *drv_data);
	int (*read)(struct spi_driver_data *drv_data);
	irqreturn_t (*transfer_handler)(struct spi_driver_data *drv_data);
	void (*cs_control)(u32 command);
	struct freq_qos_request qos_idle;
	int qos_idle_value;
	struct clk      *clk;
	struct reset_control *reset;
	int irq;
	unsigned int spi_enhancement;
	unsigned char slave_mode;
	struct timer_list slave_rx_timer;
};

struct chip_data {
	u32 top_ctrl;
	u32 fifo_ctrl;
	u32 timeout;
	u8 n_bytes;
	u32 dma_burst_size;
	u32 threshold;
	u32 dma_threshold;
	u8 enable_dma;
	union {
		int gpio_cs;
		unsigned int frm;
	};
	int gpio_cs_inverted;
	int (*write)(struct spi_driver_data *drv_data);
	int (*read)(struct spi_driver_data *drv_data);
	void (*cs_control)(u32 command);
};

static inline u32 k1_spi_read(const struct spi_driver_data *drv_data, unsigned int reg)
{
	return __raw_readl(drv_data->ioaddr + reg);
}

static  inline void k1_spi_write(const struct spi_driver_data *drv_data, unsigned int reg, u32 val)
{
	__raw_writel(val, drv_data->ioaddr + reg);
}

#define START_STATE ((void *)0)
#define RUNNING_STATE ((void *)1)
#define DONE_STATE ((void *)2)
#define ERROR_STATE ((void *)-1)

#define IS_DMA_ALIGNED(x)	IS_ALIGNED((unsigned long)(x), DMA_ALIGNMENT)
#define DMA_ALIGNMENT           64

extern int k1_spi_flush(struct spi_driver_data *drv_data);
extern void *k1_spi_next_transfer(struct spi_driver_data *drv_data);

/*
 * Select the right DMA implementation.
 */
#define MAX_DMA_LEN		SZ_512K
#define DEFAULT_DMA_FIFO_CTRL	(FIFO_TSRE | FIFO_RSRE)
#define DEFAULT_DMA_TOP_CTRL	(TOP_TRAIL)

#define RX_THRESH_DFLT		9
#define TX_THRESH_DFLT		8
/* 0x14  */
#define STATUS_TFL_MASK		(0x1f << 7)	/* Transmit FIFO Level mask */
#define STATUS_RFL_MASK		(0x1f << 15)	/* Receive FIFO Level mask */
/* 0x4 */
#define FIFO_TFT		(0x0000001F)	/* Transmit FIFO Threshold (mask) */
#define FIFO_TxTresh(x)		(((x) - 1) << 0)	/* level [1..32] */
#define FIFO_RFT		(0x000003E0)	/* Receive FIFO Threshold (mask) */
#define FIFO_RxTresh(x)	(((x) - 1) << 5)	/* level [1..32] */

#define K1_SPI_CS_ASSERT	(0x01)
#define K1_SPI_CS_DEASSERT	(0x02)

struct dma_chan;

/* device.platform_data for spi controller devices */
struct k1_spi_master {
	u16 num_chipselect;
	u8 enable_dma;

	/* DMA engine specific config */
	bool (*dma_filter)(struct dma_chan *chan, void *param);
	void *tx_param;
	void *rx_param;
};

#endif /* _SPI_SPACEMIT_K1_H */
