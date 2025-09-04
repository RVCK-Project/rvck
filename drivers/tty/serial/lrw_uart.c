// SPDX-License-Identifier: GPL-2.0+
/*
 *  Serial Port driver for LRW
 *
 *  Copyright (c) 2025, LRW CORPORATION. All rights reserved.
 */

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/platform_device.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/pinctrl/consumer.h>
#include <linux/sizes.h>
#include <linux/io.h>
#include <linux/acpi.h>

#define UART_NR			14

#define ISR_PASS_LIMIT		256

#define LRW_UART_NAME		"lrw-uart"

#define LRW_UART_TTY_PREFIX	"ttyLRW"

/* LRW_UART_TX_FIFO_DEPTH: depth of the TX FIFO (in bytes) */
#define LRW_UART_TX_FIFO_DEPTH	16

/* LRW_UART_RX_FIFO_DEPTH: depth of the RX FIFO (in bytes) */
#define LRW_UART_RX_FIFO_DEPTH	16

/* LRW UART register offsets */
#define UARTDR			0x00	/* Data register */
#define UARTRSR			0x04	/* Receive status register */
#define UARTECR			0x04	/* Error clear register */
#define UARTSC			0x08	/* Special character register */
#define UARTMDR			0x0C	/* RS485 Muti-drop register */
#define UARTTAT			0x10	/* RS485 turn-around time register */
#define UARTFCR			0x14	/* FIFO control register */
#define UARTFR			0x18	/* Flag register */
#define UARTIND			0x1C	/* Integer baud rate register */
#define UARTFD			0x20	/* Fractional baud rate register */
#define UARTBSR			0x24	/* Baud sample rate register */
#define	UARTFRCR		0x28	/* Frame control register */
#define UARTMCFG		0x2C	/* config register */
#define	UARTMCR			0x30	/* Modem control register */
#define	UARTIRCR		0x34	/* IrDA mode control register */
#define UARTIMSC		0x38	/* Interrupt mask set/clear register */
#define UARTRIS			0x3C	/* Raw interrupt status register */
#define UARTMIS			0x40	/* Masked interrupt states register */
#define UARTICR			0x44	/* Interrupt clear register */
#define UARTFCCR		0x48	/* Flow control register */
#define UARTRVS			0x58	/* Version register */

#define UARTDR_OE		BIT(11)
#define UARTDR_BE		BIT(10)
#define UARTDR_PE		BIT(9)
#define UARTDR_FE		BIT(8)

#define UARTRSR_OE		BIT(3)
#define UARTRSR_BE		BIT(2)
#define UARTRSR_PE		BIT(1)
#define UARTRSR_FE		BIT(0)

#define UARTFCR_RXFTRS		GENMASK(7, 5)
#define UARTFCR_RXFTRS_RX1_8	FIELD_PREP_CONST(UARTFCR_RXFTRS, 0)
#define UARTFCR_RXFTRS_RX2_8	FIELD_PREP_CONST(UARTFCR_RXFTRS, 1)
#define UARTFCR_RXFTRS_RX4_8	FIELD_PREP_CONST(UARTFCR_RXFTRS, 2)
#define UARTFCR_RXFTRS_RX6_8	FIELD_PREP_CONST(UARTFCR_RXFTRS, 3)
#define UARTFCR_RXFTRS_RX7_8	FIELD_PREP_CONST(UARTFCR_RXFTRS, 4)
#define UARTFCR_TXFTRS		GENMASK(4, 2)
#define UARTFCR_TXFTRS_TX1_8	FIELD_PREP_CONST(UARTFCR_TXFTRS, 0)
#define UARTFCR_TXFTRS_TX2_8	FIELD_PREP_CONST(UARTFCR_TXFTRS, 1)
#define UARTFCR_TXFTRS_TX4_8	FIELD_PREP_CONST(UARTFCR_TXFTRS, 2)
#define UARTFCR_TXFTRS_TX6_8	FIELD_PREP_CONST(UARTFCR_TXFTRS, 3)
#define UARTFCR_TXFTRS_TX7_8	FIELD_PREP_CONST(UARTFCR_TXFTRS, 4)
#define UARTFCR_FEN		BIT(0)

#define UARTFR_RI		BIT(8)
#define UARTFR_TXFE		BIT(7)
#define UARTFR_RXFF		BIT(6)
#define UARTFR_TXFF		(1 << 5)	/* used in ASM */
#define UARTFR_RXFE		BIT(4)
#define UARTFR_BUSY		(1 << 3)	/* used in ASM */
#define UARTFR_DCD		BIT(2)
#define UARTFR_DSR		BIT(1)
#define UARTFR_CTS		BIT(0)
#define UARTFR_TMSK		(UARTFR_TXFF + UARTFR_BUSY)

#define UARTFRCR_STP2		BIT(5)
#define UARTFRCR_SPS		BIT(4)
#define UARTFRCR_EOP		BIT(3)
#define UARTFRCR_PEN		BIT(2)
#define UARTFRCR_WLEN_8		0x3
#define UARTFRCR_WLEN_7		0x2
#define UARTFRCR_WLEN_6		0x1
#define UARTFRCR_WLEN_5		0x0

#define UARTMCFG_LBE		BIT(7)	/* loopback enable */
#define UARTMCFG_RXE		BIT(3)	/* receive enable */
#define UARTMCFG_TXE		BIT(2)	/* transmit enable */
#define UARTMCFG_BRK		BIT(1)	/* send break */
#define UARTMCFG_UARTEN		BIT(0)	/* UART enable */

#define UARTMCR_OUT2		BIT(3)	/* OUT2 */
#define UARTMCR_OUT1		BIT(2)	/* OUT1 */
#define UARTMCR_RTS		BIT(1)	/* RTS */
#define UARTMCR_DTR		BIT(0)	/* DTR */

#define UARTIMSC_OEIM		BIT(10)	/* overrun error interrupt mask */
#define UARTIMSC_BEIM		BIT(9)	/* break error interrupt mask */
#define UARTIMSC_PEIM		BIT(8)	/* parity error interrupt mask */
#define UARTIMSC_FEIM		BIT(7)	/* framing error interrupt mask */
#define UARTIMSC_RTIM		BIT(6)	/* receive timeout interrupt mask */
#define UARTIMSC_TXIM		BIT(5)	/* transmit interrupt mask */
#define UARTIMSC_RXIM		BIT(4)	/* receive interrupt mask */
#define UARTIMSC_DSRMIM		BIT(3)	/* DSR interrupt mask */
#define UARTIMSC_DCDMIM		BIT(2)	/* DCD interrupt mask */
#define UARTIMSC_CTSMIM		BIT(1)	/* CTS interrupt mask */
#define UARTIMSC_RIMIM		BIT(0)	/* RI interrupt mask */

#define UARTICR_OEIC		BIT(10)	/* overrun error interrupt clear */
#define UARTICR_BEIC		BIT(9)	/* break error interrupt clear */
#define UARTICR_PEIC		BIT(8)	/* parity error interrupt clear */
#define UARTICR_FEIC		BIT(7)	/* framing error interrupt clear */
#define UARTICR_RTIC		BIT(6)	/* receive timeout interrupt clear */
#define UARTICR_TXIC		BIT(5)	/* transmit interrupt clear */
#define UARTICR_RXIC		BIT(4)	/* receive interrupt clear */
#define UARTICR_DSRMIC		BIT(3)	/* DSR interrupt clear */
#define UARTICR_DCDMIC		BIT(2)	/* DCD interrupt clear */
#define UARTICR_CTSMIC		BIT(1)	/* CTS interrupt clear */
#define UARTICR_RIMIC		BIT(0)	/* RI interrupt clear */

#define UARTFCCR_CTSEN		BIT(5)	/* CTS hardware flow control */
#define UARTFCCR_RTSEN		BIT(4)	/* RTS hardware flow control */
#define UARTFCCR_DMAONERR	BIT(2)	/* disable dma on error */
#define UARTFCCR_TXDMAE		BIT(1)	/* enable transmit dma */
#define UARTFCCR_RXDMAE		BIT(0)	/* enable receive dma */

#define UARTRSR_ANY		(UARTRSR_OE | UARTRSR_BE | UARTRSR_PE | UARTRSR_FE)
#define UARTFR_MODEM_ANY	(UARTFR_DCD | UARTFR_DSR | UARTFR_CTS)

#define UART_DR_ERROR		(UARTDR_OE | UARTDR_BE | UARTDR_PE | UARTDR_FE)
#define UART_DUMMY_DR_RX	BIT(16)

enum {
	REG_DR,
	REG_FCR,
	REG_FR,
	REG_IND,
	REG_FD,
	REG_BSR,
	REG_FRCR,
	REG_MCFG,
	REG_MCR,
	REG_IMSC,
	REG_RIS,
	REG_MIS,
	REG_ICR,
	REG_FCCR,

	/* The size of the array - must be last */
	REG_ARRAY_SIZE,
};

static u16 lrw_uart_std_offsets[REG_ARRAY_SIZE] = {
	[REG_DR] = UARTDR,
	[REG_FCR] = UARTFCR,
	[REG_FR] = UARTFR,
	[REG_IND] = UARTIND,
	[REG_FD] = UARTFD,
	[REG_BSR] = UARTBSR,
	[REG_FRCR] = UARTFRCR,
	[REG_MCFG] = UARTMCFG,
	[REG_MCR] = UARTMCR,
	[REG_IMSC] = UARTIMSC,
	[REG_RIS] = UARTRIS,
	[REG_MIS] = UARTMIS,
	[REG_ICR] = UARTICR,
	[REG_FCCR] = UARTFCCR,
};

/* There is by now at least one vendor with differing details, so handle it */
struct vendor_data {
	const u16		*reg_offset;
	unsigned int		fcr;
	unsigned int		fr_busy;
	unsigned int		fr_dsr;
	unsigned int		fr_cts;
	unsigned int		fr_ri;
	unsigned int		inv_fr;
	bool			access_32b;
	bool			oversampling;
	bool			dma_threshold;
	bool			cts_event_workaround;
	bool			always_enabled;
	bool			fixed_options;
};

static struct vendor_data vendor_lrw = {
	.reg_offset		= lrw_uart_std_offsets,
	.fcr			= UARTFCR_RXFTRS_RX4_8 | UARTFCR_TXFTRS_TX4_8 | UARTFCR_FEN,
	.fr_busy		= UARTFR_BUSY,
	.fr_dsr			= UARTFR_DSR,
	.fr_cts			= UARTFR_CTS,
	.fr_ri			= UARTFR_RI,
	.access_32b		= true,
	.oversampling		= false,
	.dma_threshold		= false,
	.cts_event_workaround	= false,
	.always_enabled		= true,
	.fixed_options		= true,
};

/* Deals with DMA transactions */

struct lrw_uart_dmabuf {
	dma_addr_t		dma;
	size_t			len;
	char			*buf;
};

struct lrw_uart_dmarx_data {
	struct dma_chan		*chan;
	struct completion	complete;
	bool			use_buf_b;
	struct lrw_uart_dmabuf	dbuf_a;
	struct lrw_uart_dmabuf	dbuf_b;
	dma_cookie_t		cookie;
	bool			running;
	struct timer_list	timer;
	unsigned int last_residue;
	unsigned long last_jiffies;
	bool auto_poll_rate;
	unsigned int poll_rate;
	unsigned int poll_timeout;
};

struct lrw_uart_dmatx_data {
	struct dma_chan		*chan;
	dma_addr_t		dma;
	size_t			len;
	char			*buf;
	bool			queued;
};

struct lrw_uart_data {
	bool (*dma_filter)(struct dma_chan *chan, void *filter_param);
	void *dma_rx_param;
	void *dma_tx_param;
	bool dma_rx_poll_enable;
	unsigned int dma_rx_poll_rate;
	unsigned int dma_rx_poll_timeout;
	void (*init)(void);
	void (*exit)(void);
};

/*
 * We wrap our port structure around the generic uart_port.
 */
struct lrw_uart_port {
	struct uart_port	port;
	const u16		*reg_offset;
	struct clk		*clk;
	const struct vendor_data *vendor;
	unsigned int		im;		/* interrupt mask */
	unsigned int		old_status;
	unsigned int		fifosize;	/* vendor-specific */
	unsigned int		fixed_baud;	/* vendor-set fixed baud rate */
	char			type[12];
	bool			rs485_tx_started;
	unsigned int		rs485_tx_drain_interval; /* usecs */
#ifdef CONFIG_DMA_ENGINE
	/* DMA stuff */
	unsigned int		dmacr;		/* dma control reg */
	bool			using_tx_dma;
	bool			using_rx_dma;
	struct lrw_uart_dmarx_data dmarx;
	struct lrw_uart_dmatx_data dmatx;
	bool			dma_probed;
#endif
};

static unsigned int lrw_uart_tx_empty(struct uart_port *port);

static unsigned int lrw_uart_reg_to_offset(const struct lrw_uart_port *sup,
	unsigned int reg)
{
	return sup->reg_offset[reg];
}

static unsigned int lrw_uart_read(const struct lrw_uart_port *sup,
	unsigned int reg)
{
	void __iomem *addr = sup->port.membase + lrw_uart_reg_to_offset(sup, reg);

	return (sup->port.iotype == UPIO_MEM32) ?
		readl_relaxed(addr) : readw_relaxed(addr);
}

static void lrw_uart_write(unsigned int val, const struct lrw_uart_port *sup,
	unsigned int reg)
{
	void __iomem *addr = sup->port.membase + lrw_uart_reg_to_offset(sup, reg);

	if (sup->port.iotype == UPIO_MEM32)
		writel_relaxed(val, addr);
	else
		writew_relaxed(val, addr);
}

/*
 * Reads up to 256 characters from the FIFO or until it's empty and
 * inserts them into the TTY layer. Returns the number of characters
 * read from the FIFO.
 */
static int lrw_uart_fifo_to_tty(struct lrw_uart_port *sup)
{
	unsigned int ch, fifotaken;
	int sysrq;
	u16 status;
	u8 flag;

	for (fifotaken = 0; fifotaken != 256; fifotaken++) {
		status = lrw_uart_read(sup, REG_FR);
		if (status & UARTFR_RXFE)
			break;

		/* Take chars from the FIFO and update status */
		ch = lrw_uart_read(sup, REG_DR) | UART_DUMMY_DR_RX;
		flag = TTY_NORMAL;
		sup->port.icount.rx++;

		if (unlikely(ch & UART_DR_ERROR)) {
			if (ch & UARTDR_BE) {
				ch &= ~(UARTDR_FE | UARTDR_PE);
				sup->port.icount.brk++;
				if (uart_handle_break(&sup->port))
					continue;
			} else if (ch & UARTDR_PE) {
				sup->port.icount.parity++;
			} else if (ch & UARTDR_FE) {
				sup->port.icount.frame++;
			}
			if (ch & UARTDR_OE)
				sup->port.icount.overrun++;

			ch &= sup->port.read_status_mask;

			if (ch & UARTDR_BE)
				flag = TTY_BREAK;
			else if (ch & UARTDR_PE)
				flag = TTY_PARITY;
			else if (ch & UARTDR_FE)
				flag = TTY_FRAME;
		}

		uart_port_unlock(&sup->port);
		sysrq = uart_prepare_sysrq_char(&sup->port, ch & 255);
		uart_port_lock(&sup->port);

		if (!sysrq)
			uart_insert_char(&sup->port, ch, UARTDR_OE, ch, flag);
	}

	return fifotaken;
}

/*
 * All the DMA operation mode stuff goes inside this ifdef.
 * This assumes that you have a generic DMA device interface,
 * no custom DMA interfaces are supported.
 */
#ifdef CONFIG_DMA_ENGINE

#define LRW_UART_DMA_BUFFER_SIZE PAGE_SIZE

static int lrw_uart_dmabuf_init(struct dma_chan *chan, struct lrw_uart_dmabuf *db,
	enum dma_data_direction dir)
{
	db->buf = dma_alloc_coherent(chan->device->dev, LRW_UART_DMA_BUFFER_SIZE,
				     &db->dma, GFP_KERNEL);
	if (!db->buf)
		return -ENOMEM;
	db->len = LRW_UART_DMA_BUFFER_SIZE;

	return 0;
}

static void lrw_uart_dmabuf_free(struct dma_chan *chan, struct lrw_uart_dmabuf *db,
	enum dma_data_direction dir)
{
	if (db->buf) {
		dma_free_coherent(chan->device->dev,
				  LRW_UART_DMA_BUFFER_SIZE, db->buf, db->dma);
	}
}

static void lrw_uart_dma_probe(struct lrw_uart_port *sup)
{
	/* DMA is the sole user of the platform data right now */
	struct lrw_uart_data *plat = dev_get_platdata(sup->port.dev);
	struct device *dev = sup->port.dev;
	struct dma_slave_config tx_conf = {
		.dst_addr = sup->port.mapbase +
				 lrw_uart_reg_to_offset(sup, REG_DR),
		.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE,
		.direction = DMA_MEM_TO_DEV,
		.dst_maxburst = sup->fifosize >> 1,
		.device_fc = false,
	};
	struct dma_chan *chan;
	dma_cap_mask_t mask;

	sup->dma_probed = true;
	chan = dma_request_chan(dev, "tx");
	if (IS_ERR(chan)) {
		if (PTR_ERR(chan) == -EPROBE_DEFER) {
			sup->dma_probed = false;
			return;
		}

		/* We need platform data */
		if (!plat || !plat->dma_filter) {
			dev_dbg(sup->port.dev, "no DMA platform data\n");
			return;
		}

		/* Try to acquire a generic DMA engine slave TX channel */
		dma_cap_zero(mask);
		dma_cap_set(DMA_SLAVE, mask);

		chan = dma_request_channel(mask, plat->dma_filter,
					   plat->dma_tx_param);
		if (!chan) {
			dev_err(sup->port.dev, "no TX DMA channel!\n");
			return;
		}
	}

	dmaengine_slave_config(chan, &tx_conf);
	sup->dmatx.chan = chan;

	dev_info(sup->port.dev, "DMA channel TX %s\n",
		 dma_chan_name(sup->dmatx.chan));

	/* Optionally make use of an RX channel as well */
	chan = dma_request_slave_channel(dev, "rx");

	if (IS_ERR(chan) && plat && plat->dma_rx_param) {
		chan = dma_request_channel(mask, plat->dma_filter, plat->dma_rx_param);

		if (!chan) {
			dev_err(sup->port.dev, "no RX DMA channel!\n");
			return;
		}
	}

	if (!IS_ERR(chan)) {
		struct dma_slave_config rx_conf = {
			.src_addr = sup->port.mapbase +
				lrw_uart_reg_to_offset(sup, REG_DR),
			.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE,
			.direction = DMA_DEV_TO_MEM,
			.src_maxburst = sup->fifosize >> 2,
			.device_fc = false,
		};
		struct dma_slave_caps caps;

		/*
		 * Some DMA controllers provide information on their capabilities.
		 * If the controller does, check for suitable residue processing
		 * otherwise assime all is well.
		 */
		if (dma_get_slave_caps(chan, &caps) == 0) {
			if (caps.residue_granularity ==
					DMA_RESIDUE_GRANULARITY_DESCRIPTOR) {
				dma_release_channel(chan);
				dev_info(sup->port.dev,
					 "RX DMA disabled - no residue processing\n");
				return;
			}
		}
		dmaengine_slave_config(chan, &rx_conf);
		sup->dmarx.chan = chan;

		sup->dmarx.auto_poll_rate = false;
		if (plat && plat->dma_rx_poll_enable) {
			/* Set poll rate if specified. */
			if (plat->dma_rx_poll_rate) {
				sup->dmarx.auto_poll_rate = false;
				sup->dmarx.poll_rate = plat->dma_rx_poll_rate;
			} else {
				/*
				 * 100 ms defaults to poll rate if not
				 * specified. This will be adjusted with
				 * the baud rate at set_termios.
				 */
				sup->dmarx.auto_poll_rate = true;
				sup->dmarx.poll_rate =  100;
			}
			/* 3 secs defaults poll_timeout if not specified. */
			if (plat->dma_rx_poll_timeout)
				sup->dmarx.poll_timeout =
					plat->dma_rx_poll_timeout;
			else
				sup->dmarx.poll_timeout = 3000;
		} else if (!plat && dev->of_node) {
			sup->dmarx.auto_poll_rate =
					of_property_read_bool(dev->of_node, "auto-poll");
			if (sup->dmarx.auto_poll_rate) {
				u32 x;

				if (of_property_read_u32(dev->of_node, "poll-rate-ms", &x) == 0)
					sup->dmarx.poll_rate = x;
				else
					sup->dmarx.poll_rate = 100;
				if (of_property_read_u32(dev->of_node, "poll-timeout-ms", &x) == 0)
					sup->dmarx.poll_timeout = x;
				else
					sup->dmarx.poll_timeout = 3000;
			}
		}
		dev_info(sup->port.dev, "DMA channel RX %s\n",
			 dma_chan_name(sup->dmarx.chan));
	}
}

static void lrw_uart_dma_remove(struct lrw_uart_port *sup)
{
	if (sup->dmatx.chan)
		dma_release_channel(sup->dmatx.chan);
	if (sup->dmarx.chan)
		dma_release_channel(sup->dmarx.chan);
}

/* Forward declare these for the refill routine */
static int lrw_uart_dma_tx_refill(struct lrw_uart_port *sup);
static void lrw_uart_start_tx_pio(struct lrw_uart_port *sup);

/*
 * The current DMA TX buffer has been sent.
 * Try to queue up another DMA buffer.
 */
static void lrw_uart_dma_tx_callback(void *data)
{
	struct lrw_uart_port *sup = data;
	struct lrw_uart_dmatx_data *dmatx = &sup->dmatx;
	unsigned long flags;
	u16 dmacr;

	uart_port_lock_irqsave(&sup->port, &flags);
	if (sup->dmatx.queued)
		dma_unmap_single(dmatx->chan->device->dev, dmatx->dma,
				 dmatx->len, DMA_TO_DEVICE);

	dmacr = sup->dmacr;
	sup->dmacr = dmacr & ~UARTFCCR_TXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);

	/*
	 * If TX DMA was disabled, it means that we've stopped the DMA for
	 * some reason (eg, XOFF received, or we want to send an X-char.)
	 *
	 * Note: we need to be careful here of a potential race between DMA
	 * and the rest of the driver - if the driver disables TX DMA while
	 * a TX buffer completing, we must update the tx queued status to
	 * get further refills (hence we check dmacr).
	 */
	if (!(dmacr & UARTFCCR_TXDMAE) || uart_tx_stopped(&sup->port) ||
	    uart_circ_empty(&sup->port.state->xmit)) {
		sup->dmatx.queued = false;
		uart_port_unlock_irqrestore(&sup->port, flags);
		return;
	}

	if (lrw_uart_dma_tx_refill(sup) <= 0)
		/*
		 * We didn't queue a DMA buffer for some reason, but we
		 * have data pending to be sent.  Re-enable the TX IRQ.
		 */
		lrw_uart_start_tx_pio(sup);

	uart_port_unlock_irqrestore(&sup->port, flags);
}

/*
 * Try to refill the TX DMA buffer.
 * Locking: called with port lock held and IRQs disabled.
 * Returns:
 *   1 if we queued up a TX DMA buffer.
 *   0 if we didn't want to handle this by DMA
 *  <0 on error
 */
static int lrw_uart_dma_tx_refill(struct lrw_uart_port *sup)
{
	struct lrw_uart_dmatx_data *dmatx = &sup->dmatx;
	struct dma_chan *chan = dmatx->chan;
	struct dma_device *dma_dev = chan->device;
	struct dma_async_tx_descriptor *desc;
	struct circ_buf *xmit = &sup->port.state->xmit;
	unsigned int count;

	/*
	 * Try to avoid the overhead involved in using DMA if the
	 * transaction fits in the first half of the FIFO, by using
	 * the standard interrupt handling.  This ensures that we
	 * issue a uart_write_wakeup() at the appropriate time.
	 */
	count = uart_circ_chars_pending(xmit);
	if (count < (sup->fifosize >> 1)) {
		sup->dmatx.queued = false;
		return 0;
	}

	/*
	 * Bodge: don't send the last character by DMA, as this
	 * will prevent XON from notifying us to restart DMA.
	 */
	count -= 1;

	/* Else proceed to copy the TX chars to the DMA buffer and fire DMA */
	if (count > LRW_UART_DMA_BUFFER_SIZE)
		count = LRW_UART_DMA_BUFFER_SIZE;

	if (xmit->tail < xmit->head)
		memcpy(&dmatx->buf[0], &xmit->buf[xmit->tail], count);
	else {
		size_t first = UART_XMIT_SIZE - xmit->tail;
		size_t second;

		if (first > count)
			first = count;
		second = count - first;

		memcpy(&dmatx->buf[0], &xmit->buf[xmit->tail], first);
		if (second)
			memcpy(&dmatx->buf[first], &xmit->buf[0], second);
	}

	dmatx->len = count;
	dmatx->dma = dma_map_single(dma_dev->dev, dmatx->buf, count,
				    DMA_TO_DEVICE);
	if (dmatx->dma == DMA_MAPPING_ERROR) {
		sup->dmatx.queued = false;
		dev_dbg(sup->port.dev, "unable to map TX DMA\n");
		return -EBUSY;
	}

	desc = dmaengine_prep_slave_single(chan, dmatx->dma, dmatx->len, DMA_MEM_TO_DEV,
					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dma_unmap_single(dma_dev->dev, dmatx->dma, dmatx->len, DMA_TO_DEVICE);
		sup->dmatx.queued = false;
		/*
		 * If DMA cannot be used right now, we complete this
		 * transaction via IRQ and let the TTY layer retry.
		 */
		dev_dbg(sup->port.dev, "TX DMA busy\n");
		return -EBUSY;
	}

	/* Some data to go along to the callback */
	desc->callback = lrw_uart_dma_tx_callback;
	desc->callback_param = sup;

	/* All errors should happen at prepare time */
	dmaengine_submit(desc);

	/* Fire the DMA transaction */
	dma_dev->device_issue_pending(chan);

	sup->dmacr |= UARTFCCR_TXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	sup->dmatx.queued = true;

	/*
	 * Now we know that DMA will fire, so advance the ring buffer
	 * with the stuff we just dispatched.
	 */
	uart_xmit_advance(&sup->port, count);

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&sup->port);

	return 1;
}

/*
 * We received a transmit interrupt without a pending X-char but with
 * pending characters.
 * Locking: called with port lock held and IRQs disabled.
 * Returns:
 *   false if we want to use PIO to transmit
 *   true if we queued a DMA buffer
 */
static bool lrw_uart_dma_tx_irq(struct lrw_uart_port *sup)
{
	if (!sup->using_tx_dma)
		return false;

	/*
	 * If we already have a TX buffer queued, but received a
	 * TX interrupt, it will be because we've just sent an X-char.
	 * Ensure the TX DMA is enabled and the TX IRQ is disabled.
	 */
	if (sup->dmatx.queued) {
		sup->dmacr |= UARTFCCR_TXDMAE;
		lrw_uart_write(sup->dmacr, sup, REG_FCCR);
		sup->im &= ~UARTIMSC_TXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
		return true;
	}

	/*
	 * We don't have a TX buffer queued, so try to queue one.
	 * If we successfully queued a buffer, mask the TX IRQ.
	 */
	if (lrw_uart_dma_tx_refill(sup) > 0) {
		sup->im &= ~UARTIMSC_TXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
		return true;
	}
	return false;
}

/*
 * Stop the DMA transmit (eg, due to received XOFF).
 * Locking: called with port lock held and IRQs disabled.
 */
static inline void lrw_uart_dma_tx_stop(struct lrw_uart_port *sup)
{
	if (sup->dmatx.queued) {
		sup->dmacr &= ~UARTFCCR_TXDMAE;
		lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	}
}

/*
 * Try to start a DMA transmit, or in the case of an XON/OFF
 * character queued for send, try to get that character out ASAP.
 * Locking: called with port lock held and IRQs disabled.
 * Returns:
 *   false if we want the TX IRQ to be enabled
 *   true if we have a buffer queued
 */
static inline bool lrw_uart_dma_tx_start(struct lrw_uart_port *sup)
{
	u16 dmacr;

	if (!sup->using_tx_dma)
		return false;

	if (!sup->port.x_char) {
		/* no X-char, try to push chars out in DMA mode */
		bool ret = true;

		if (!sup->dmatx.queued) {
			if (lrw_uart_dma_tx_refill(sup) > 0) {
				sup->im &= ~UARTIMSC_TXIM;
				lrw_uart_write(sup->im, sup, REG_IMSC);
			} else {
				ret = false;
			}
		} else if (!(sup->dmacr & UARTFCCR_TXDMAE)) {
			sup->dmacr |= UARTFCCR_TXDMAE;
			lrw_uart_write(sup->dmacr, sup, REG_FCCR);
		}
		return ret;
	}

	/*
	 * We have an X-char to send.  Disable DMA to prevent it loading
	 * the TX fifo, and then see if we can stuff it into the FIFO.
	 */
	dmacr = sup->dmacr;
	sup->dmacr &= ~UARTFCCR_TXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);

	if (lrw_uart_read(sup, REG_FR) & UARTFR_TXFF) {
		/*
		 * No space in the FIFO, so enable the transmit interrupt
		 * so we know when there is space.  Note that once we've
		 * loaded the character, we should just re-enable DMA.
		 */
		return false;
	}

	lrw_uart_write(sup->port.x_char, sup, REG_DR);
	sup->port.icount.tx++;
	sup->port.x_char = 0;

	/* Success - restore the DMA state */
	sup->dmacr = dmacr;
	lrw_uart_write(dmacr, sup, REG_FCCR);

	return true;
}

/*
 * Flush the transmit buffer.
 * Locking: called with port lock held and IRQs disabled.
 */
static void lrw_uart_dma_flush_buffer(struct uart_port *port)
__releases(&sup->port.lock)
__acquires(&sup->port.lock)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	if (!sup->using_tx_dma)
		return;

	dmaengine_terminate_async(sup->dmatx.chan);

	if (sup->dmatx.queued) {
		dma_unmap_single(sup->dmatx.chan->device->dev, sup->dmatx.dma,
				 sup->dmatx.len, DMA_TO_DEVICE);
		sup->dmatx.queued = false;
		sup->dmacr &= ~UARTFCCR_TXDMAE;
		lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	}
}

static void lrw_uart_dma_rx_callback(void *data);

static int lrw_uart_dma_rx_trigger_dma(struct lrw_uart_port *sup)
{
	struct dma_chan *rxchan = sup->dmarx.chan;
	struct lrw_uart_dmarx_data *dmarx = &sup->dmarx;
	struct dma_async_tx_descriptor *desc;
	struct lrw_uart_dmabuf *dbuf;

	if (!rxchan)
		return -EIO;

	/* Start the RX DMA job */
	dbuf = sup->dmarx.use_buf_b ?
		&sup->dmarx.dbuf_b : &sup->dmarx.dbuf_a;
	desc = dmaengine_prep_slave_single(rxchan, dbuf->dma, dbuf->len,
					   DMA_DEV_TO_MEM,
					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	/*
	 * If the DMA engine is busy and cannot prepare a
	 * channel, no big deal, the driver will fall back
	 * to interrupt mode as a result of this error code.
	 */
	if (!desc) {
		sup->dmarx.running = false;
		dmaengine_terminate_all(rxchan);
		return -EBUSY;
	}

	/* Some data to go along to the callback */
	desc->callback = lrw_uart_dma_rx_callback;
	desc->callback_param = sup;
	dmarx->cookie = dmaengine_submit(desc);
	dma_async_issue_pending(rxchan);

	sup->dmacr |= UARTFCCR_RXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	sup->dmarx.running = true;

	sup->im &= ~UARTIMSC_RXIM;
	lrw_uart_write(sup->im, sup, REG_IMSC);

	return 0;
}

/*
 * This is called when either the DMA job is complete, or
 * the FIFO timeout interrupt occurred. This must be called
 * with the port spinlock sup->port.lock held.
 */
static void lrw_uart_dma_rx_chars(struct lrw_uart_port *sup,
				  u32 pending, bool use_buf_b,
				  bool readfifo)
{
	struct tty_port *port = &sup->port.state->port;
	struct lrw_uart_dmabuf *dbuf = use_buf_b ?
		&sup->dmarx.dbuf_b : &sup->dmarx.dbuf_a;
	int dma_count = 0;
	u32 fifotaken = 0; /* only used for vdbg() */

	struct lrw_uart_dmarx_data *dmarx = &sup->dmarx;
	int dmataken = 0;

	if (sup->dmarx.poll_rate) {
		/* The data can be taken by polling */
		dmataken = dbuf->len - dmarx->last_residue;
		/* Recalculate the pending size */
		if (pending >= dmataken)
			pending -= dmataken;
	}

	/* Pick the remain data from the DMA */
	if (pending) {
		/*
		 * First take all chars in the DMA pipe, then look in the FIFO.
		 * Note that tty_insert_flip_buf() tries to take as many chars
		 * as it can.
		 */
		dma_count = tty_insert_flip_string(port, dbuf->buf + dmataken, pending);

		sup->port.icount.rx += dma_count;
		if (dma_count < pending)
			dev_warn(sup->port.dev,
				 "couldn't insert all characters (TTY is full?)\n");
	}

	/* Reset the last_residue for Rx DMA poll */
	if (sup->dmarx.poll_rate)
		dmarx->last_residue = dbuf->len;

	/*
	 * Only continue with trying to read the FIFO if all DMA chars have
	 * been taken first.
	 */
	if (dma_count == pending && readfifo) {
		/* Clear any error flags */
		lrw_uart_write(UARTICR_OEIC | UARTICR_BEIC | UARTICR_PEIC |
			      UARTICR_FEIC, sup, REG_ICR);

		/*
		 * If we read all the DMA'd characters, and we had an
		 * incomplete buffer, that could be due to an rx error, or
		 * maybe we just timed out. Read any pending chars and check
		 * the error status.
		 *
		 * Error conditions will only occur in the FIFO, these will
		 * trigger an immediate interrupt and stop the DMA job, so we
		 * will always find the error in the FIFO, never in the DMA
		 * buffer.
		 */
		fifotaken = lrw_uart_fifo_to_tty(sup);
	}

	dev_vdbg(sup->port.dev,
		 "Took %d chars from DMA buffer and %d chars from the FIFO\n",
		 dma_count, fifotaken);
	tty_flip_buffer_push(port);
}

static void lrw_uart_dma_rx_irq(struct lrw_uart_port *sup)
{
	struct lrw_uart_dmarx_data *dmarx = &sup->dmarx;
	struct dma_chan *rxchan = dmarx->chan;
	struct lrw_uart_dmabuf *dbuf = dmarx->use_buf_b ?
		&dmarx->dbuf_b : &dmarx->dbuf_a;
	size_t pending;
	struct dma_tx_state state;
	enum dma_status dmastat;

	/*
	 * Pause the transfer so we can trust the current counter,
	 * do this before we pause the LRW UART block, else we may
	 * overflow the FIFO.
	 */
	if (dmaengine_pause(rxchan))
		dev_err(sup->port.dev, "unable to pause DMA transfer\n");
	dmastat = rxchan->device->device_tx_status(rxchan,
						   dmarx->cookie, &state);
	if (dmastat != DMA_PAUSED)
		dev_err(sup->port.dev, "unable to pause DMA transfer\n");

	/* Disable RX DMA - incoming data will wait in the FIFO */
	sup->dmacr &= ~UARTFCCR_RXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	sup->dmarx.running = false;

	pending = dbuf->len - state.residue;
	if (WARN_ON_ONCE(pending > LRW_UART_DMA_BUFFER_SIZE))
		pending = LRW_UART_DMA_BUFFER_SIZE;

	/* Then we terminate the transfer - we now know our residue */
	dmaengine_terminate_all(rxchan);

	/*
	 * This will take the chars we have so far and insert
	 * into the framework.
	 */
	lrw_uart_dma_rx_chars(sup, pending, dmarx->use_buf_b, true);

	/* Switch buffer & re-trigger DMA job */
	dmarx->use_buf_b = !dmarx->use_buf_b;
	if (lrw_uart_dma_rx_trigger_dma(sup)) {
		dev_dbg(sup->port.dev,
			"could not retrigger RX DMA job fall back to interrupt mode\n");
		sup->im |= UARTIMSC_RXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
	}
}

static void lrw_uart_dma_rx_callback(void *data)
{
	struct lrw_uart_port *sup = data;
	struct lrw_uart_dmarx_data *dmarx = &sup->dmarx;
	struct dma_chan *rxchan = dmarx->chan;
	bool lastbuf = dmarx->use_buf_b;
	struct lrw_uart_dmabuf *dbuf = dmarx->use_buf_b ?
		&dmarx->dbuf_b : &dmarx->dbuf_a;
	size_t pending;
	struct dma_tx_state state;
	int ret;

	/*
	 * This completion interrupt occurs typically when the
	 * RX buffer is totally stuffed but no timeout has yet
	 * occurred. When that happens, we just want the RX
	 * routine to flush out the secondary DMA buffer while
	 * we immediately trigger the next DMA job.
	 */
	uart_port_lock_irq(&sup->port);
	/*
	 * Rx data can be taken by the UART interrupts during
	 * the DMA irq handler. So we check the residue here.
	 */
	rxchan->device->device_tx_status(rxchan, dmarx->cookie, &state);
	pending = dbuf->len - state.residue;
	if (WARN_ON_ONCE(pending > LRW_UART_DMA_BUFFER_SIZE))
		pending = LRW_UART_DMA_BUFFER_SIZE;

	/* Then we terminate the transfer - we now know our residue */
	dmaengine_terminate_all(rxchan);

	sup->dmarx.running = false;
	dmarx->use_buf_b = !lastbuf;
	ret = lrw_uart_dma_rx_trigger_dma(sup);

	lrw_uart_dma_rx_chars(sup, pending, lastbuf, false);
	uart_unlock_and_check_sysrq(&sup->port);
	/*
	 * Do this check after we picked the DMA chars so we don't
	 * get some IRQ immediately from RX.
	 */
	if (ret) {
		dev_dbg(sup->port.dev,
			"could not retrigger RX DMA job fall back to interrupt mode\n");
		sup->im |= UARTIMSC_RXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
	}
}

/*
 * Stop accepting received characters, when we're shutting down or
 * suspending this port.
 * Locking: called with port lock held and IRQs disabled.
 */
static inline void lrw_uart_dma_rx_stop(struct lrw_uart_port *sup)
{
	if (!sup->using_rx_dma)
		return;

	/* FIXME.  Just disable the DMA enable */
	sup->dmacr &= ~UARTFCCR_RXDMAE;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);
}

/*
 * Timer handler for Rx DMA polling.
 * Every polling, It checks the residue in the dma buffer and transfer
 * data to the tty. Also, last_residue is updated for the next polling.
 */
static void lrw_uart_dma_rx_poll(struct timer_list *t)
{
	struct lrw_uart_port *sup = from_timer(sup, t, dmarx.timer);
	struct tty_port *port = &sup->port.state->port;
	struct lrw_uart_dmarx_data *dmarx = &sup->dmarx;
	struct dma_chan *rxchan = sup->dmarx.chan;
	unsigned long flags;
	unsigned int dmataken = 0;
	unsigned int size = 0;
	struct lrw_uart_dmabuf *dbuf;
	int dma_count;
	struct dma_tx_state state;

	dbuf = dmarx->use_buf_b ? &sup->dmarx.dbuf_b : &sup->dmarx.dbuf_a;
	rxchan->device->device_tx_status(rxchan, dmarx->cookie, &state);
	if (likely(state.residue < dmarx->last_residue)) {
		dmataken = dbuf->len - dmarx->last_residue;
		size = dmarx->last_residue - state.residue;
		dma_count = tty_insert_flip_string(port, dbuf->buf + dmataken,
						   size);
		if (dma_count == size)
			dmarx->last_residue =  state.residue;
		dmarx->last_jiffies = jiffies;
	}
	tty_flip_buffer_push(port);

	/*
	 * If no data is received in poll_timeout, the driver will fall back
	 * to interrupt mode. We will retrigger DMA at the first interrupt.
	 */
	if (jiffies_to_msecs(jiffies - dmarx->last_jiffies)
			> sup->dmarx.poll_timeout) {
		uart_port_lock_irqsave(&sup->port, &flags);
		lrw_uart_dma_rx_stop(sup);
		sup->im |= UARTIMSC_RXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
		uart_port_unlock_irqrestore(&sup->port, flags);

		sup->dmarx.running = false;
		dmaengine_terminate_all(rxchan);
		del_timer(&sup->dmarx.timer);
	} else {
		mod_timer(&sup->dmarx.timer,
			  jiffies + msecs_to_jiffies(sup->dmarx.poll_rate));
	}
}

static void lrw_uart_dma_startup(struct lrw_uart_port *sup)
{
	int ret;

	if (!sup->dma_probed)
		lrw_uart_dma_probe(sup);

	if (!sup->dmatx.chan)
		return;

	sup->dmatx.buf = kmalloc(LRW_UART_DMA_BUFFER_SIZE, GFP_KERNEL | __GFP_DMA);
	if (!sup->dmatx.buf) {
		sup->port.fifosize = sup->fifosize;
		return;
	}

	sup->dmatx.len = LRW_UART_DMA_BUFFER_SIZE;

	/* The DMA buffer is now the FIFO the TTY subsystem can use */
	sup->port.fifosize = LRW_UART_DMA_BUFFER_SIZE;
	sup->using_tx_dma = true;

	if (!sup->dmarx.chan)
		goto skip_rx;

	/* Allocate and map DMA RX buffers */
	ret = lrw_uart_dmabuf_init(sup->dmarx.chan, &sup->dmarx.dbuf_a,
				  DMA_FROM_DEVICE);
	if (ret) {
		dev_err(sup->port.dev, "failed to init DMA %s: %d\n",
			"RX buffer A", ret);
		goto skip_rx;
	}

	ret = lrw_uart_dmabuf_init(sup->dmarx.chan, &sup->dmarx.dbuf_b,
				  DMA_FROM_DEVICE);
	if (ret) {
		dev_err(sup->port.dev, "failed to init DMA %s: %d\n",
			"RX buffer B", ret);
		lrw_uart_dmabuf_free(sup->dmarx.chan, &sup->dmarx.dbuf_a,
				    DMA_FROM_DEVICE);
		goto skip_rx;
	}

	sup->using_rx_dma = true;

skip_rx:
	/* Turn on DMA error (RX/TX will be enabled on demand) */
	sup->dmacr |= UARTFCCR_DMAONERR;
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);

	if (sup->using_rx_dma) {
		if (lrw_uart_dma_rx_trigger_dma(sup))
			dev_dbg(sup->port.dev,
				"could not trigger initial RX DMA job, fall back to interrupt mode\n");
		if (sup->dmarx.poll_rate) {
			timer_setup(&sup->dmarx.timer, lrw_uart_dma_rx_poll, 0);
			mod_timer(&sup->dmarx.timer,
				  jiffies + msecs_to_jiffies(sup->dmarx.poll_rate));
			sup->dmarx.last_residue = LRW_UART_DMA_BUFFER_SIZE;
			sup->dmarx.last_jiffies = jiffies;
		}
	}
}

static void lrw_uart_dma_shutdown(struct lrw_uart_port *sup)
{
	if (!(sup->using_tx_dma || sup->using_rx_dma))
		return;

	/* Disable RX and TX DMA */
	while (lrw_uart_read(sup, REG_FR) & sup->vendor->fr_busy)
		cpu_relax();

	uart_port_lock_irq(&sup->port);
	sup->dmacr &= ~(UARTFCCR_DMAONERR | UARTFCCR_RXDMAE | UARTFCCR_TXDMAE);
	lrw_uart_write(sup->dmacr, sup, REG_FCCR);
	uart_port_unlock_irq(&sup->port);

	if (sup->using_tx_dma) {
		/* In theory, this should already be done by lrw_uart_dma_flush_buffer */
		dmaengine_terminate_all(sup->dmatx.chan);
		if (sup->dmatx.queued) {
			dma_unmap_single(sup->dmatx.chan->device->dev,
					 sup->dmatx.dma, sup->dmatx.len,
					 DMA_TO_DEVICE);
			sup->dmatx.queued = false;
		}

		kfree(sup->dmatx.buf);
		sup->using_tx_dma = false;
	}

	if (sup->using_rx_dma) {
		dmaengine_terminate_all(sup->dmarx.chan);
		/* Clean up the RX DMA */
		lrw_uart_dmabuf_free(sup->dmarx.chan, &sup->dmarx.dbuf_a, DMA_FROM_DEVICE);
		lrw_uart_dmabuf_free(sup->dmarx.chan, &sup->dmarx.dbuf_b, DMA_FROM_DEVICE);
		if (sup->dmarx.poll_rate)
			del_timer_sync(&sup->dmarx.timer);
		sup->using_rx_dma = false;
	}
}

static inline bool lrw_uart_dma_rx_available(struct lrw_uart_port *sup)
{
	return sup->using_rx_dma;
}

static inline bool lrw_uart_dma_rx_running(struct lrw_uart_port *sup)
{
	return sup->using_rx_dma && sup->dmarx.running;
}

#else
/* Blank functions if the DMA engine is not available */
static inline void lrw_uart_dma_remove(struct lrw_uart_port *sup)
{
}

static inline void lrw_uart_dma_startup(struct lrw_uart_port *sup)
{
}

static inline void lrw_uart_dma_shutdown(struct lrw_uart_port *sup)
{
}

static inline bool lrw_uart_dma_tx_irq(struct lrw_uart_port *sup)
{
	return false;
}

static inline void lrw_uart_dma_tx_stop(struct lrw_uart_port *sup)
{
}

static inline bool lrw_uart_dma_tx_start(struct lrw_uart_port *sup)
{
	return false;
}

static inline void lrw_uart_dma_rx_irq(struct lrw_uart_port *sup)
{
}

static inline void lrw_uart_dma_rx_stop(struct lrw_uart_port *sup)
{
}

static inline int lrw_uart_dma_rx_trigger_dma(struct lrw_uart_port *sup)
{
	return -EIO;
}

static inline bool lrw_uart_dma_rx_available(struct lrw_uart_port *sup)
{
	return false;
}

static inline bool lrw_uart_dma_rx_running(struct lrw_uart_port *sup)
{
	return false;
}

#define lrw_uart_dma_flush_buffer	NULL
#endif

static void lrw_uart_rs485_tx_stop(struct lrw_uart_port *sup)
{
	/*
	 * To be on the safe side only time out after twice as many iterations
	 * as fifo size.
	 */
	const int MAX_TX_DRAIN_ITERS = sup->port.fifosize * 2;
	struct uart_port *port = &sup->port;
	int i = 0;
	u32 mcr, mcfg;

	/* Wait until hardware tx queue is empty */
	while (!lrw_uart_tx_empty(port)) {
		if (i > MAX_TX_DRAIN_ITERS) {
			dev_warn(port->dev,
				 "timeout while draining hardware tx queue\n");
			break;
		}

		udelay(sup->rs485_tx_drain_interval);
		i++;
	}

	if (port->rs485.delay_rts_after_send)
		mdelay(port->rs485.delay_rts_after_send);

	mcr = lrw_uart_read(sup, REG_MCR);

	if (port->rs485.flags & SER_RS485_RTS_AFTER_SEND)
		mcr &= ~UARTMCR_RTS;
	else
		mcr |= UARTMCR_RTS;

	lrw_uart_write(mcr, sup, REG_MCR);

	/* Disable the transmitter and reenable the transceiver */
	mcfg = lrw_uart_read(sup, REG_MCFG);
	mcfg &= ~UARTMCFG_TXE;
	mcfg |= UARTMCFG_RXE;
	lrw_uart_write(mcfg, sup, REG_MCFG);

	sup->rs485_tx_started = false;
}

static void lrw_uart_stop_tx(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	sup->im &= ~UARTIMSC_TXIM;
	lrw_uart_write(sup->im, sup, REG_IMSC);
	lrw_uart_dma_tx_stop(sup);

	if ((port->rs485.flags & SER_RS485_ENABLED) && sup->rs485_tx_started)
		lrw_uart_rs485_tx_stop(sup);
}

static bool lrw_uart_tx_chars(struct lrw_uart_port *sup, bool from_irq);

/* Start TX with programmed I/O only (no DMA) */
static void lrw_uart_start_tx_pio(struct lrw_uart_port *sup)
{
	if (lrw_uart_tx_chars(sup, false)) {
		sup->im |= UARTIMSC_TXIM;
		lrw_uart_write(sup->im, sup, REG_IMSC);
	}
}

static void lrw_uart_rs485_tx_start(struct lrw_uart_port *sup)
{
	struct uart_port *port = &sup->port;
	u32 mcr, mcfg;

	/* Enable transmitter */
	mcfg = lrw_uart_read(sup, REG_MCFG);
	mcfg |= UARTMCFG_TXE;

	/* Disable receiver if half-duplex */
	if (!(port->rs485.flags & SER_RS485_RX_DURING_TX))
		mcfg &= ~UARTMCFG_RXE;

	lrw_uart_write(mcfg, sup, REG_MCFG);

	mcr = lrw_uart_read(sup, REG_MCR);
	if (port->rs485.flags & SER_RS485_RTS_ON_SEND)
		mcr &= ~UARTMCR_RTS;
	else
		mcr |= UARTMCR_RTS;

	lrw_uart_write(mcr, sup, REG_MCR);

	if (port->rs485.delay_rts_before_send)
		mdelay(port->rs485.delay_rts_before_send);

	sup->rs485_tx_started = true;
}

static void lrw_uart_start_tx(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	if ((sup->port.rs485.flags & SER_RS485_ENABLED) &&
	    !sup->rs485_tx_started)
		lrw_uart_rs485_tx_start(sup);

	if (!lrw_uart_dma_tx_start(sup))
		lrw_uart_start_tx_pio(sup);
}

static void lrw_uart_stop_rx(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	sup->im &= ~(UARTIMSC_RXIM | UARTIMSC_RTIM | UARTIMSC_FEIM |
		     UARTIMSC_PEIM | UARTIMSC_BEIM | UARTIMSC_OEIM);
	lrw_uart_write(sup->im, sup, REG_IMSC);

	lrw_uart_dma_rx_stop(sup);
}

static void lrw_uart_throttle_rx(struct uart_port *port)
{
	unsigned long flags;

	uart_port_lock_irqsave(port, &flags);
	lrw_uart_stop_rx(port);
	uart_port_unlock_irqrestore(port, flags);
}

static void lrw_uart_enable_ms(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	sup->im |= UARTIMSC_RIMIM | UARTIMSC_CTSMIM | UARTIMSC_DCDMIM | UARTIMSC_DSRMIM;
	lrw_uart_write(sup->im, sup, REG_IMSC);
}

static void lrw_uart_rx_chars(struct lrw_uart_port *sup)
__releases(&sup->port.lock)
__acquires(&sup->port.lock)
{
	lrw_uart_fifo_to_tty(sup);

	uart_port_unlock(&sup->port);
	tty_flip_buffer_push(&sup->port.state->port);
	/*
	 * If we were temporarily out of DMA mode for a while,
	 * attempt to switch back to DMA mode again.
	 */
	if (lrw_uart_dma_rx_available(sup)) {
		if (lrw_uart_dma_rx_trigger_dma(sup)) {
			dev_dbg(sup->port.dev,
				"could not trigger RX DMA job fall back to interrupt mode again\n");
			sup->im |= UARTIMSC_RXIM;
			lrw_uart_write(sup->im, sup, REG_IMSC);
		} else {
#ifdef CONFIG_DMA_ENGINE
			/* Start Rx DMA poll */
			if (sup->dmarx.poll_rate) {
				sup->dmarx.last_jiffies = jiffies;
				sup->dmarx.last_residue	= LRW_UART_DMA_BUFFER_SIZE;
				mod_timer(&sup->dmarx.timer,
					  jiffies + msecs_to_jiffies(sup->dmarx.poll_rate));
			}
#endif
		}
	}
	uart_port_lock(&sup->port);
}

static bool lrw_uart_tx_char(struct lrw_uart_port *sup, unsigned char c,
			    bool from_irq)
{
	if (unlikely(!from_irq) &&
	    lrw_uart_read(sup, REG_FR) & UARTFR_TXFF)
		return false; /* unable to transmit character */

	lrw_uart_write(c, sup, REG_DR);
	sup->port.icount.tx++;

	return true;
}

/* Returns true if tx interrupts have to be (kept) enabled  */
static bool lrw_uart_tx_chars(struct lrw_uart_port *sup, bool from_irq)
{
	struct circ_buf *xmit = &sup->port.state->xmit;
	int count = sup->fifosize >> 1;

	if (sup->port.x_char) {
		if (!lrw_uart_tx_char(sup, sup->port.x_char, from_irq))
			return true;
		sup->port.x_char = 0;
		--count;
	}

	if (uart_circ_empty(xmit) || uart_tx_stopped(&sup->port)) {
		lrw_uart_stop_tx(&sup->port);
		return false;
	}

	/* If we are using DMA mode, try to send some characters. */
	if (lrw_uart_dma_tx_irq(sup))
		return true;

	do {
		if (likely(from_irq) && count-- == 0)
			break;

		if (!lrw_uart_tx_char(sup, xmit->buf[xmit->tail], from_irq))
			break;

		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
	} while (!uart_circ_empty(xmit));

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&sup->port);

	if (uart_circ_empty(xmit)) {
		lrw_uart_stop_tx(&sup->port);
		return false;
	}
	return true;
}

static void lrw_uart_modem_status(struct lrw_uart_port *sup)
{
	unsigned int status, delta;

	status = lrw_uart_read(sup, REG_FR) & UARTFR_MODEM_ANY;

	delta = status ^ sup->old_status;
	sup->old_status = status;

	if (!delta)
		return;

	if (delta & UARTFR_DCD)
		uart_handle_dcd_change(&sup->port, status & UARTFR_DCD);

	if (delta & sup->vendor->fr_dsr)
		sup->port.icount.dsr++;

	if (delta & sup->vendor->fr_cts)
		uart_handle_cts_change(&sup->port,
				       status & sup->vendor->fr_cts);

	wake_up_interruptible(&sup->port.state->port.delta_msr_wait);
}

static void check_apply_cts_event_workaround(struct lrw_uart_port *sup)
{
	if (!sup->vendor->cts_event_workaround)
		return;

	/* workaround to make sure that all bits are unlocked.. */
	lrw_uart_write(0x00, sup, REG_ICR);

	/*
	 * WA: introduce 26ns(1 uart clk) delay before W1C;
	 * single apb access will incur 2 pclk(133.12Mhz) delay,
	 * so add 2 dummy reads
	 */
	lrw_uart_read(sup, REG_ICR);
	lrw_uart_read(sup, REG_ICR);
}

static irqreturn_t lrw_uart_int(int irq, void *dev_id)
{
	struct lrw_uart_port *sup = dev_id;
	unsigned long flags;
	unsigned int status, pass_counter = ISR_PASS_LIMIT;
	int handled = 0;

	uart_port_lock_irqsave(&sup->port, &flags);
	status = lrw_uart_read(sup, REG_RIS) & sup->im;
	if (status) {
		do {
			check_apply_cts_event_workaround(sup);

			lrw_uart_write(status & ~(UARTICR_TXIC | UARTICR_RTIC | UARTICR_RXIC),
				      sup, REG_ICR);

			if (status & (UARTICR_RTIC | UARTICR_RXIC)) {
				if (lrw_uart_dma_rx_running(sup))
					lrw_uart_dma_rx_irq(sup);
				else
					lrw_uart_rx_chars(sup);
			}
			if (status & (UARTICR_DSRMIC | UARTICR_DCDMIC |
				      UARTICR_CTSMIC | UARTICR_RIMIC))
				lrw_uart_modem_status(sup);
			if (status & UARTICR_TXIC)
				lrw_uart_tx_chars(sup, true);

			if (pass_counter-- == 0)
				break;

			status = lrw_uart_read(sup, REG_RIS) & sup->im;
		} while (status != 0);
		handled = 1;
	}

	uart_port_unlock_irqrestore(&sup->port, flags);

	return IRQ_RETVAL(handled);
}

static unsigned int lrw_uart_tx_empty(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	/* Allow feature register bits to be inverted to work around errata */
	unsigned int status = lrw_uart_read(sup, REG_FR) ^ sup->vendor->inv_fr;

	return status & (sup->vendor->fr_busy | UARTFR_TXFF) ?
							0 : TIOCSER_TEMT;
}

static void lrw_uart_maybe_set_bit(bool cond, unsigned int *ptr, unsigned int mask)
{
	if (cond)
		*ptr |= mask;
}

static unsigned int lrw_uart_get_mctrl(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned int result = 0;
	unsigned int status = lrw_uart_read(sup, REG_FR);

	lrw_uart_maybe_set_bit(status & UARTFR_DCD, &result, TIOCM_CAR);
	lrw_uart_maybe_set_bit(status & sup->vendor->fr_dsr, &result, TIOCM_DSR);
	lrw_uart_maybe_set_bit(status & sup->vendor->fr_cts, &result, TIOCM_CTS);
	lrw_uart_maybe_set_bit(status & sup->vendor->fr_ri, &result, TIOCM_RNG);

	return result;
}

static void lrw_uart_assign_bit(bool cond, unsigned int *ptr, unsigned int mask)
{
	if (cond)
		*ptr |= mask;
	else
		*ptr &= ~mask;
}

static void lrw_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned int mcr;
	unsigned int mcfg;
	unsigned int fccr;

	mcr = lrw_uart_read(sup, REG_MCR);
	mcfg = lrw_uart_read(sup, REG_MCFG);
	fccr = lrw_uart_read(sup, REG_FCCR);

	lrw_uart_assign_bit(mctrl & TIOCM_RTS, &mcr, UARTMCR_RTS);
	lrw_uart_assign_bit(mctrl & TIOCM_DTR, &mcr, UARTMCR_DTR);
	lrw_uart_assign_bit(mctrl & TIOCM_OUT1, &mcr, UARTMCR_OUT1);
	lrw_uart_assign_bit(mctrl & TIOCM_OUT2, &mcr, UARTMCR_OUT2);
	lrw_uart_assign_bit(mctrl & TIOCM_LOOP, &mcfg, UARTMCFG_LBE);

	if (port->status & UPSTAT_AUTORTS) {
		/* We need to disable auto-RTS if we want to turn RTS off */
		lrw_uart_assign_bit(mctrl & TIOCM_RTS, &fccr, UARTFCCR_RTSEN);
	}

	lrw_uart_write(mcr, sup, REG_MCR);
	lrw_uart_write(mcfg, sup, REG_MCFG);
	lrw_uart_write(fccr, sup, REG_FCCR);
}

static void lrw_uart_break_ctl(struct uart_port *port, int break_state)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned long flags;
	unsigned int mcfg;

	uart_port_lock_irqsave(&sup->port, &flags);
	mcfg = lrw_uart_read(sup, REG_MCFG);
	if (break_state == -1)
		mcfg |= UARTMCFG_BRK;
	else
		mcfg &= ~UARTMCFG_BRK;
	lrw_uart_write(mcfg, sup, REG_MCFG);
	uart_port_unlock_irqrestore(&sup->port, flags);
}

#ifdef CONFIG_CONSOLE_POLL

static void lrw_uart_quiesce_irqs(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	lrw_uart_write(lrw_uart_read(sup, REG_MIS), sup, REG_ICR);
	/*
	 * There is no way to clear TXIM as this is "ready to transmit IRQ", so
	 * we simply mask it. start_tx() will unmask it.
	 *
	 * Note we can race with start_tx(), and if the race happens, the
	 * polling user might get another interrupt just after we clear it.
	 * But it should be OK and can happen even w/o the race, e.g.
	 * controller immediately got some new data and raised the IRQ.
	 *
	 * And whoever uses polling routines assumes that it manages the device
	 * (including tx queue), so we're also fine with start_tx()'s caller
	 * side.
	 */
	lrw_uart_write(lrw_uart_read(sup, REG_IMSC) & ~UARTIMSC_TXIM, sup,
		    REG_IMSC);
}

static int lrw_uart_get_poll_char(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned int status;

	/*
	 * The caller might need IRQs lowered, e.g. if used with KDB NMI
	 * debugger.
	 */
	lrw_uart_quiesce_irqs(port);

	status = lrw_uart_read(sup, REG_FR);
	if (status & UARTFR_RXFE)
		return NO_POLL_CHAR;

	return lrw_uart_read(sup, REG_DR);
}

static void lrw_uart_put_poll_char(struct uart_port *port, unsigned char ch)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	while (lrw_uart_read(sup, REG_FR) & UARTFR_TXFF)
		cpu_relax();

	lrw_uart_write(ch, sup, REG_DR);
}

#endif /* CONFIG_CONSOLE_POLL */

static int lrw_uart_hwinit(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	int retval;
	unsigned int clk;

	/* Optionaly enable pins to be muxed in and configured */
	pinctrl_pm_select_default_state(port->dev);

	/*
	 * Try to enable the clock producer.
	 */
	retval = clk_prepare_enable(sup->clk);
	if (retval)
		return retval;

	if (has_acpi_companion(sup->port.dev)) {
		device_property_read_u32(sup->port.dev, "clock-frequency", &clk);
		sup->port.uartclk = clk;
	} else {
		sup->port.uartclk = clk_get_rate(sup->clk);
	}

	/* Clear pending error and receive interrupts */
	lrw_uart_write(UARTICR_OEIC | UARTICR_BEIC | UARTICR_PEIC |
		      UARTICR_FEIC | UARTICR_RTIC | UARTICR_RXIC,
		      sup, REG_ICR);

	/*
	 * Save interrupts enable mask, and enable RX interrupts in case if
	 * the interrupt is used for NMI entry.
	 */
	sup->im = lrw_uart_read(sup, REG_IMSC);
	lrw_uart_write(UARTIMSC_RTIM | UARTIMSC_RXIM, sup, REG_IMSC);

	if (dev_get_platdata(sup->port.dev)) {
		struct lrw_uart_data *plat;

		plat = dev_get_platdata(sup->port.dev);
		if (plat->init)
			plat->init();
	}
	return 0;
}

static int lrw_uart_allocate_irq(struct lrw_uart_port *sup)
{
	lrw_uart_write(sup->im, sup, REG_IMSC);

	return request_irq(sup->port.irq, lrw_uart_int, IRQF_SHARED, "lrw-uart", sup);
}

/*
 * Enable interrupts, only timeouts when using DMA
 * if initial RX DMA job failed, start in interrupt mode
 * as well.
 */
static void lrw_uart_enable_interrupts(struct lrw_uart_port *sup)
{
	unsigned long flags;
	unsigned int i;

	uart_port_lock_irqsave(&sup->port, &flags);

	/* Clear out any spuriously appearing RX interrupts */
	lrw_uart_write(UARTICR_RTIC | UARTICR_RXIC, sup, REG_ICR);

	/*
	 * RXIS is asserted only when the RX FIFO transitions from below
	 * to above the trigger threshold.  If the RX FIFO is already
	 * full to the threshold this can't happen and RXIS will now be
	 * stuck off.  Drain the RX FIFO explicitly to fix this:
	 */
	for (i = 0; i < sup->fifosize * 2; ++i) {
		if (lrw_uart_read(sup, REG_FR) & UARTFR_RXFE)
			break;

		lrw_uart_read(sup, REG_DR);
	}

	sup->im = UARTIMSC_RTIM;
	if (!lrw_uart_dma_rx_running(sup))
		sup->im |= UARTIMSC_RXIM;
	lrw_uart_write(sup->im, sup, REG_IMSC);
	uart_port_unlock_irqrestore(&sup->port, flags);
}

static void lrw_uart_unthrottle_rx(struct uart_port *port)
{
	struct lrw_uart_port *sup = container_of(port, struct lrw_uart_port, port);
	unsigned long flags;

	uart_port_lock_irqsave(&sup->port, &flags);

	sup->im = UARTIMSC_RTIM;
	if (!lrw_uart_dma_rx_running(sup))
		sup->im |= UARTIMSC_RXIM;

	lrw_uart_write(sup->im, sup, REG_IMSC);

	uart_port_unlock_irqrestore(&sup->port, flags);
}

static int lrw_uart_startup(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned int mcr;
	unsigned int mcfg;
	int retval;

	retval = lrw_uart_hwinit(port);
	if (retval)
		goto clk_dis;

	retval = lrw_uart_allocate_irq(sup);
	if (retval)
		goto clk_dis;

	lrw_uart_write(sup->vendor->fcr, sup, REG_FCR);

	uart_port_lock_irq(&sup->port);

	mcr = lrw_uart_read(sup, REG_MCR);
	mcr &= UARTMCR_RTS | UARTMCR_DTR;

	lrw_uart_write(mcr, sup, REG_MCR);

	mcfg = lrw_uart_read(sup, REG_MCFG);

	mcfg |= UARTMCFG_UARTEN | UARTMCFG_RXE;

	if (!(port->rs485.flags & SER_RS485_ENABLED))
		mcfg |= UARTMCFG_TXE;

	lrw_uart_write(mcfg, sup, REG_MCFG);

	uart_port_unlock_irq(&sup->port);

	/*
	 * initialise the old status of the modem signals
	 */
	sup->old_status = lrw_uart_read(sup, REG_FR) & UARTFR_MODEM_ANY;

	/* Startup DMA */
	lrw_uart_dma_startup(sup);

	lrw_uart_enable_interrupts(sup);

	return 0;

 clk_dis:
	clk_disable_unprepare(sup->clk);
	return retval;
}

static void lrw_uart_shutdown_channel(struct lrw_uart_port *sup,
	unsigned int mcfg, unsigned int fcr)
{
	unsigned long val;

	val = lrw_uart_read(sup, mcfg);
	val &= ~(UARTMCFG_BRK);
	lrw_uart_write(val, sup, mcfg);

	val = lrw_uart_read(sup, fcr);
	val &= ~(UARTFCR_FEN);
	lrw_uart_write(val, sup, fcr);
}

/*
 * disable the port. It should not disable RTS and DTR.
 * Also RTS and DTR state should be preserved to restore
 * it during startup().
 */
static void lrw_uart_disable_uart(struct lrw_uart_port *sup)
{
	unsigned int mcr;
	unsigned int mcfg;

	sup->port.status &= ~(UPSTAT_AUTOCTS | UPSTAT_AUTORTS);
	uart_port_lock_irq(&sup->port);
	mcr = lrw_uart_read(sup, REG_MCR);
	mcr &= UARTMCR_RTS | UARTMCR_DTR;
	lrw_uart_write(mcr, sup, REG_MCR);

	mcfg = lrw_uart_read(sup, REG_MCFG);
	mcfg |= UARTMCFG_UARTEN | UARTMCFG_TXE;
	lrw_uart_write(mcfg, sup, REG_MCFG);
	uart_port_unlock_irq(&sup->port);

	/*
	 * disable break condition and fifos
	 */
	lrw_uart_shutdown_channel(sup, REG_MCFG, REG_FCR);
}

static void lrw_uart_disable_interrupts(struct lrw_uart_port *sup)
{
	uart_port_lock_irq(&sup->port);

	/* mask all interrupts and clear all pending ones */
	sup->im = 0;
	lrw_uart_write(sup->im, sup, REG_IMSC);
	lrw_uart_write(0xffff, sup, REG_ICR);

	uart_port_unlock_irq(&sup->port);
}

static void lrw_uart_shutdown(struct uart_port *port)
{
	struct lrw_uart_port *sup =
		container_of(port, struct lrw_uart_port, port);

	lrw_uart_disable_interrupts(sup);

	lrw_uart_dma_shutdown(sup);

	if ((port->rs485.flags & SER_RS485_ENABLED) && sup->rs485_tx_started)
		lrw_uart_rs485_tx_stop(sup);

	free_irq(sup->port.irq, sup);

	lrw_uart_disable_uart(sup);

	/*
	 * Shut down the clock producer
	 */
	clk_disable_unprepare(sup->clk);
	/* Optionally let pins go into sleep states */
	pinctrl_pm_select_sleep_state(port->dev);

	if (dev_get_platdata(sup->port.dev)) {
		struct lrw_uart_data *plat;

		plat = dev_get_platdata(sup->port.dev);
		if (plat->exit)
			plat->exit();
	}

	if (sup->port.ops->flush_buffer)
		sup->port.ops->flush_buffer(port);
}

static void
lrw_uart_setup_status_masks(struct uart_port *port, struct ktermios *termios)
{
	port->read_status_mask = UARTDR_OE | 255;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= UARTDR_FE | UARTDR_PE;
	if (termios->c_iflag & (IGNBRK | BRKINT | PARMRK))
		port->read_status_mask |= UARTDR_BE;

	/*
	 * Characters to ignore
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= UARTDR_FE | UARTDR_PE;
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= UARTDR_BE;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= UARTDR_OE;
	}

	/*
	 * Ignore all characters if CREAD is not set.
	 */
	if ((termios->c_cflag & CREAD) == 0)
		port->ignore_status_mask |= UART_DUMMY_DR_RX;
}

static void
lrw_uart_set_termios(struct uart_port *port, struct ktermios *termios,
		     const struct ktermios *old)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	unsigned int frcr;
	unsigned int mcr, fccr;
	unsigned int mcfg;
	unsigned long flags;
	unsigned int baud, quot, clkdiv;
	unsigned int bits;
	unsigned int clk;

	if (sup->vendor->oversampling)
		clkdiv = 8;
	else
		clkdiv = 16;

	if (has_acpi_companion(sup->port.dev)) {
		device_property_read_u32(sup->port.dev, "clock-frequency", &clk);
		sup->port.uartclk = clk;
	}

	/*
	 * Ask the core to calculate the divisor for us.
	 */
	baud = uart_get_baud_rate(port, termios, old, 0,
				  port->uartclk / clkdiv);

#ifdef CONFIG_DMA_ENGINE
	/*
	 * Adjust RX DMA polling rate with baud rate if not specified.
	 */
	if (sup->dmarx.auto_poll_rate)
		sup->dmarx.poll_rate = DIV_ROUND_UP(10000000, baud);
#endif

	if (baud > port->uartclk / 16)
		quot = DIV_ROUND_CLOSEST(port->uartclk * 8, baud);
	else
		quot = DIV_ROUND_CLOSEST(port->uartclk * 4, baud);

	switch (termios->c_cflag & CSIZE) {
	case CS5:
		frcr = UARTFRCR_WLEN_5;
		break;
	case CS6:
		frcr = UARTFRCR_WLEN_6;
		break;
	case CS7:
		frcr = UARTFRCR_WLEN_7;
		break;
	default: // CS8
		frcr = UARTFRCR_WLEN_8;
		break;
	}

	if (termios->c_cflag & CSTOPB)
		frcr |= UARTFRCR_STP2;
	if (termios->c_cflag & PARENB) {
		frcr |= UARTFRCR_PEN;
		if (!(termios->c_cflag & PARODD))
			frcr |= UARTFRCR_EOP;
		if (termios->c_cflag & CMSPAR)
			frcr |= UARTFRCR_SPS;
	}


	bits = tty_get_frame_size(termios->c_cflag);

	uart_port_lock_irqsave(port, &flags);

	/*
	 * Update the per-port timeout.
	 */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * Calculate the approximated time it takes to transmit one character
	 * with the given baud rate. We use this as the poll interval when we
	 * wait for the tx queue to empty.
	 */
	sup->rs485_tx_drain_interval = DIV_ROUND_UP(bits * 1000 * 1000, baud);

	lrw_uart_setup_status_masks(port, termios);

	if (UART_ENABLE_MS(port, termios->c_cflag))
		lrw_uart_enable_ms(port);

	if (port->rs485.flags & SER_RS485_ENABLED)
		termios->c_cflag &= ~CRTSCTS;

	mcr = lrw_uart_read(sup, REG_MCR);
	mcfg = lrw_uart_read(sup, REG_MCFG);
	fccr = lrw_uart_read(sup, REG_FCCR);

	if (termios->c_cflag & CRTSCTS) {
		if (mcr & UARTMCR_RTS)
			fccr |= UARTFCCR_RTSEN;

		fccr |= UARTFCCR_CTSEN;
		port->status |= UPSTAT_AUTOCTS | UPSTAT_AUTORTS;
	} else {
		fccr &= ~(UARTFCCR_CTSEN | UARTFCCR_RTSEN);
		port->status &= ~(UPSTAT_AUTOCTS | UPSTAT_AUTORTS);
	}

	/* Set baud rate */
	lrw_uart_write(quot & 0x3f, sup, REG_FD);
	lrw_uart_write(quot >> 6, sup, REG_IND);

	/*
	 * ----------v----------v----------v----------v-----
	 * NOTE: REG_FRCR MUST BE WRITTEN AFTER REG_FD & REG_IND.
	 * ----------^----------^----------^----------^-----
	 */
	lrw_uart_write(frcr, sup, REG_FRCR);

	lrw_uart_write(fccr, sup, REG_FCCR);

	/*
	 * Receive was disabled by lrw_uart_disable_uart during shutdown.
	 * Need to reenable receive if you need to use a tty_driver
	 * returns from tty_find_polling_driver() after a port shutdown.
	 */
	mcfg |= UARTMCFG_RXE | UARTMCFG_UARTEN;
	lrw_uart_write(mcfg, sup, REG_MCFG);

	uart_port_unlock_irqrestore(port, flags);
}

static const char *lrw_uart_type(struct uart_port *port)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);
	return sup->port.type == PORT_LRW ? sup->type : NULL;
}

/*
 * Configure/autoconfigure the port.
 */
static void lrw_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_LRW;
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int lrw_uart_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	int ret = 0;

	if (ser->type != PORT_UNKNOWN && ser->type != PORT_LRW)
		ret = -EINVAL;
	if (ser->irq < 0 || ser->irq >= nr_irqs)
		ret = -EINVAL;
	if (ser->baud_base < 9600)
		ret = -EINVAL;
	if (port->mapbase != (unsigned long)ser->iomem_base)
		ret = -EINVAL;
	return ret;
}

static int lrw_uart_rs485_config(struct uart_port *port, struct ktermios *termios,
			      struct serial_rs485 *rs485)
{
	struct lrw_uart_port *sup =
		container_of(port, struct lrw_uart_port, port);

	if (port->rs485.flags & SER_RS485_ENABLED)
		lrw_uart_rs485_tx_stop(sup);

	/* Make sure auto RTS is disabled */
	if (rs485->flags & SER_RS485_ENABLED) {
		u32 fccr = lrw_uart_read(sup, REG_FCCR);

		fccr &= ~UARTFCCR_RTSEN;
		lrw_uart_write(fccr, sup, REG_FCCR);
		port->status &= ~UPSTAT_AUTORTS;
	}

	return 0;
}

static const struct uart_ops lrw_uart_pops = {
	.tx_empty	= lrw_uart_tx_empty,
	.set_mctrl	= lrw_uart_set_mctrl,
	.get_mctrl	= lrw_uart_get_mctrl,
	.stop_tx	= lrw_uart_stop_tx,
	.start_tx	= lrw_uart_start_tx,
	.stop_rx	= lrw_uart_stop_rx,
	.throttle	= lrw_uart_throttle_rx,
	.unthrottle	= lrw_uart_unthrottle_rx,
	.enable_ms	= lrw_uart_enable_ms,
	.break_ctl	= lrw_uart_break_ctl,
	.startup	= lrw_uart_startup,
	.shutdown	= lrw_uart_shutdown,
	.flush_buffer	= lrw_uart_dma_flush_buffer,
	.set_termios	= lrw_uart_set_termios,
	.type		= lrw_uart_type,
	.config_port	= lrw_uart_config_port,
	.verify_port	= lrw_uart_verify_port,
#ifdef CONFIG_CONSOLE_POLL
	.poll_init     = lrw_uart_hwinit,
	.poll_get_char = lrw_uart_get_poll_char,
	.poll_put_char = lrw_uart_put_poll_char,
#endif
};

static struct lrw_uart_port *lrw_uart_console_ports[UART_NR];

#ifdef CONFIG_SERIAL_LRW_UART_CONSOLE

static void lrw_uart_console_putchar(struct uart_port *port, unsigned char ch)
{
	struct lrw_uart_port *sup =
	    container_of(port, struct lrw_uart_port, port);

	while (lrw_uart_read(sup, REG_FR) & UARTFR_TXFF)
		cpu_relax();
	lrw_uart_write(ch, sup, REG_DR);
}

static void
lrw_uart_console_write(struct console *co, const char *s, unsigned int count)
{
	struct lrw_uart_port *sup = lrw_uart_console_ports[co->index];
	unsigned int old_fccr = 0, new_fccr;
	unsigned int old_mcfg = 0, new_mcfg;
	unsigned long flags;
	int locked = 1;

	clk_enable(sup->clk);

	local_irq_save(flags);
	if (sup->port.sysrq)
		locked = 0;
	else if (oops_in_progress)
		locked = uart_port_trylock(&sup->port);
	else
		uart_port_lock(&sup->port);

	/*
	 *	First save the FCCR then disable the interrupts
	 */
	if (!sup->vendor->always_enabled) {
		old_fccr = lrw_uart_read(sup, REG_FCCR);
		new_fccr = old_fccr & ~UARTFCCR_CTSEN;
		lrw_uart_write(new_fccr, sup, REG_FCCR);

		old_mcfg = lrw_uart_read(sup, REG_MCFG);
		new_mcfg |= UARTMCFG_UARTEN | UARTMCFG_TXE;
		lrw_uart_write(new_mcfg, sup, REG_MCFG);
	}

	uart_console_write(&sup->port, s, count, lrw_uart_console_putchar);

	/*
	 *	Finally, wait for transmitter to become empty and restore the
	 *	TCR. Allow feature register bits to be inverted to work around
	 *	errata.
	 */
	while ((lrw_uart_read(sup, REG_FR) ^ sup->vendor->inv_fr)
						& sup->vendor->fr_busy)
		cpu_relax();
	if (!sup->vendor->always_enabled) {
		lrw_uart_write(old_fccr, sup, REG_FCCR);
		lrw_uart_write(old_mcfg, sup, REG_MCFG);
	}

	if (locked)
		uart_port_unlock(&sup->port);
	local_irq_restore(flags);

	clk_disable(sup->clk);
}

static void lrw_uart_console_get_options(struct lrw_uart_port *sup, int *baud,
					 int *parity, int *bits)
{
	unsigned int frcr, ind, fd;

	if (!(lrw_uart_read(sup, REG_MCFG) & UARTMCFG_UARTEN))
		return;

	frcr = lrw_uart_read(sup, REG_FRCR);

	*parity = 'n';
	if (frcr & UARTFRCR_PEN) {
		if (frcr & UARTFRCR_EOP)
			*parity = 'e';
		else
			*parity = 'o';
	}

	if ((frcr & 0x3) == UARTFRCR_WLEN_7)
		*bits = 7;
	else
		*bits = 8;

	ind = lrw_uart_read(sup, REG_IND);
	fd = lrw_uart_read(sup, REG_FD);

	*baud = sup->port.uartclk * 4 / (64 * ind + fd);
}

static int lrw_uart_console_setup(struct console *co, char *options)
{
	struct lrw_uart_port *sup;
	int baud = 38400;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';
	int ret;
	unsigned int clk;

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index >= UART_NR)
		co->index = 0;
	sup = lrw_uart_console_ports[co->index];
	if (!sup)
		return -ENODEV;

	/* Allow pins to be muxed in and configured */
	pinctrl_pm_select_default_state(sup->port.dev);

	ret = clk_prepare(sup->clk);
	if (ret)
		return ret;

	if (dev_get_platdata(sup->port.dev)) {
		struct lrw_uart_data *plat;

		plat = dev_get_platdata(sup->port.dev);
		if (plat->init)
			plat->init();
	}

	if (has_acpi_companion(sup->port.dev)) {
		device_property_read_u32(sup->port.dev, "clock-frequency", &clk);
		sup->port.uartclk = clk;
	} else {
		sup->port.uartclk = clk_get_rate(sup->clk);
	}

	if (sup->vendor->fixed_options) {
		baud = sup->fixed_baud;
	} else {
		if (options)
			uart_parse_options(options,
					   &baud, &parity, &bits, &flow);
		else
			lrw_uart_console_get_options(sup, &baud, &parity, &bits);
	}

	return uart_set_options(&sup->port, co, baud, parity, bits, flow);
}

/**
 *	lrw_uart_console_match - non-standard console matching
 *	@co:	  registering console
 *	@name:	  name from console command line
 *	@idx:	  index from console command line
 *	@options: ptr to option string from console command line
 *
 *	Only attempts to match console command lines of the form:
 *	    console=lrw_uart,mmio|mmio32,<addr>[,<options>]
 *	    console=lrw_uart,0x<addr>[,<options>]
 *	This form is used to register an initial earlycon boot console and
 *	replace it with the lrw_uart_console at lrw_uart driver init.
 *
 *	Performs console setup for a match (as required by interface)
 *	If no <options> are specified, then assume the h/w is already setup.
 *
 *	Returns 0 if console matches; otherwise non-zero to use default matching
 */
static int lrw_uart_console_match(struct console *co, char *name, int idx,
				  char *options)
{
	unsigned char iotype;
	resource_size_t addr;
	int i;

	if (strcmp(name, "lrw_uart") != 0)
		return -ENODEV;

	if (uart_parse_earlycon(options, &iotype, &addr, &options))
		return -ENODEV;

	if (iotype != UPIO_MEM && iotype != UPIO_MEM32)
		return -ENODEV;

	/* try to match the port specified on the command line */
	for (i = 0; i < ARRAY_SIZE(lrw_uart_console_ports); i++) {
		struct uart_port *port;

		if (!lrw_uart_console_ports[i])
			continue;

		port = &lrw_uart_console_ports[i]->port;

		if (port->mapbase != addr)
			continue;

		co->index = i;
		port->cons = co;
		return lrw_uart_console_setup(co, options);
	}

	return -ENODEV;
}

static struct uart_driver lrw_uart_driver;

static struct console lrw_uart_console = {
	.name		= LRW_UART_TTY_PREFIX,
	.write		= lrw_uart_console_write,
	.device		= uart_console_device,
	.setup		= lrw_uart_console_setup,
	.match		= lrw_uart_console_match,
	.flags		= CON_PRINTBUFFER | CON_ANYTIME,
	.index		= -1,
	.data		= &lrw_uart_driver,
};

#define LRW_UART_CONSOLE	(&lrw_uart_console)

static void lrw_uart_putc(struct uart_port *port, unsigned char c)
{
	while (readl(port->membase + UARTFR) & UARTFR_TXFF)
		cpu_relax();
	if (port->iotype == UPIO_MEM32)
		writel(c, port->membase + UARTDR);
	else
		writeb(c, port->membase + UARTDR);
	while (readl(port->membase + UARTFR) & UARTFR_BUSY)
		cpu_relax();
}

static void lrw_uart_early_write(struct console *con, const char *s, unsigned int n)
{
	struct earlycon_device *dev = con->data;

	uart_console_write(&dev->port, s, n, lrw_uart_putc);
}

#ifdef CONFIG_CONSOLE_POLL
static int lrw_uart_getc(struct uart_port *port)
{
	if (readl(port->membase + UARTFR) & UARTFR_RXFE)
		return NO_POLL_CHAR;

	if (port->iotype == UPIO_MEM32)
		return readl(port->membase + UARTDR);
	else
		return readb(port->membase + UARTDR);
}

static int lrw_uart_early_read(struct console *con, char *s, unsigned int n)
{
	struct earlycon_device *dev = con->data;
	int ch, num_read = 0;

	while (num_read < n) {
		ch = lrw_uart_getc(&dev->port);
		if (ch == NO_POLL_CHAR)
			break;

		s[num_read++] = ch;
	}

	return num_read;
}
#else
#define lrw_uart_early_read NULL
#endif

/*
 * On non-ACPI systems, earlycon is enabled by specifying
 * "earlycon=lrw_uart,<address>" on the kernel command line.
 *
 * On ACPI ARM64 systems, an "early" console is enabled via the SPCR table,
 * by specifying only "earlycon" on the command line.  Because it requires
 * SPCR, the console starts after ACPI is parsed, which is later than a
 * traditional early console.
 *
 * To get the traditional early console that starts before ACPI is parsed,
 * specify the full "earlycon=lrw_uart,<address>" option.
 */
static int __init lrw_uart_early_console_setup(struct earlycon_device *device,
					      const char *opt)
{
	if (!device->port.membase)
		return -ENODEV;

	device->con->write = lrw_uart_early_write;
	device->con->read = lrw_uart_early_read;

	return 0;
}

OF_EARLYCON_DECLARE(lrw_uart, "lrw-uart", lrw_uart_early_console_setup);

#else
#define LRW_UART_CONSOLE	NULL
#endif

static struct uart_driver lrw_uart_driver = {
	.owner			= THIS_MODULE,
	.driver_name		= LRW_UART_NAME,
	.dev_name		= LRW_UART_TTY_PREFIX,
	.nr			= UART_NR,
	.cons			= LRW_UART_CONSOLE,
};

static int lrw_uart_probe_dt_alias(int index, struct device *dev)
{
	struct device_node *np;
	static bool seen_dev_with_alias;
	static bool seen_dev_without_alias;
	int ret = index;

	if (!IS_ENABLED(CONFIG_OF))
		return ret;

	np = dev->of_node;
	if (!np)
		return ret;

	ret = of_alias_get_id(np, "serial");
	if (ret < 0) {
		seen_dev_without_alias = true;
		ret = index;
	} else {
		seen_dev_with_alias = true;
		if (ret >= ARRAY_SIZE(lrw_uart_console_ports) || lrw_uart_console_ports[ret]) {
			dev_warn(dev, "requested serial port %d  not available.\n", ret);
			ret = index;
		}
	}

	if (seen_dev_with_alias && seen_dev_without_alias)
		dev_warn(dev, "aliased and non-aliased serial devices found in device tree. Serial port enumeration may be unpredictable.\n");

	return ret;
}

/* unregisters the driver also if no more ports are left */
static void lrw_uart_unregister_port(struct lrw_uart_port *sup)
{
	int i;
	bool busy = false;

	for (i = 0; i < ARRAY_SIZE(lrw_uart_console_ports); i++) {
		if (lrw_uart_console_ports[i] == sup)
			lrw_uart_console_ports[i] = NULL;
		else if (lrw_uart_console_ports[i])
			busy = true;
	}
	lrw_uart_dma_remove(sup);
	if (!busy)
		uart_unregister_driver(&lrw_uart_driver);
}

static int lrw_uart_find_free_port(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(lrw_uart_console_ports); i++)
		if (!lrw_uart_console_ports[i])
			return i;

	return -EBUSY;
}

static int lrw_uart_setup_port(struct device *dev, struct lrw_uart_port *sup,
			      struct resource *mmiobase, int index)
{
	void __iomem *base;
	int ret;

	base = devm_ioremap_resource(dev, mmiobase);
	if (IS_ERR(base))
		return PTR_ERR(base);

	index = lrw_uart_probe_dt_alias(index, dev);

	sup->port.dev = dev;
	sup->port.mapbase = mmiobase->start;
	sup->port.membase = base;
	sup->port.fifosize = sup->fifosize;
	sup->port.has_sysrq = IS_ENABLED(CONFIG_SERIAL_LRW_UART_CONSOLE);
	sup->port.flags = UPF_BOOT_AUTOCONF;
	sup->port.line = index;

	ret = uart_get_rs485_mode(&sup->port);
	if (ret)
		return ret;

	lrw_uart_console_ports[index] = sup;

	return 0;
}

static int lrw_uart_register_port(struct lrw_uart_port *sup)
{
	int ret, i;

	/* Ensure interrupts from this UART are masked and cleared */
	lrw_uart_write(0, sup, REG_IMSC);
	lrw_uart_write(0xffff, sup, REG_ICR);

	if (!lrw_uart_driver.state) {
		ret = uart_register_driver(&lrw_uart_driver);
		if (ret < 0) {
			dev_err(sup->port.dev,
				"Failed to register LRW UART driver\n");
			for (i = 0; i < ARRAY_SIZE(lrw_uart_console_ports); i++)
				if (lrw_uart_console_ports[i] == sup)
					lrw_uart_console_ports[i] = NULL;
			return ret;
		}
	}

	ret = uart_add_one_port(&lrw_uart_driver, &sup->port);
	if (ret)
		lrw_uart_unregister_port(sup);

	return ret;
}

static const struct serial_rs485 lrw_uart_rs485_supported = {
	.flags = SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND | SER_RS485_RTS_AFTER_SEND |
		 SER_RS485_RX_DURING_TX,
	.delay_rts_before_send = 1,
	.delay_rts_after_send = 1,
};

static int lrw_uart_probe(struct platform_device *pdev)
{
	struct lrw_uart_port *sup;
	struct resource *r;
	int portnr, ret;
	unsigned int clk;
	unsigned int baudrate;

	/*
	 * Check the mandatory baud rate parameter in the DT node early
	 * so that we can easily exit with the error.
	 */
	if (pdev->dev.of_node) {
		struct device_node *np = pdev->dev.of_node;

		ret = of_property_read_u32(np, "current-speed", &baudrate);
		if (ret)
			return ret;
	} else if (has_acpi_companion(&pdev->dev)) {
		ret = device_property_read_u32(&pdev->dev, "current-speed", &baudrate);
		if (ret)
			return ret;
	} else {
		baudrate = 115200;
	}

	portnr = lrw_uart_find_free_port();
	if (portnr < 0)
		return portnr;

	sup = devm_kzalloc(&pdev->dev, sizeof(struct lrw_uart_port),
			   GFP_KERNEL);
	if (!sup)
		return -ENOMEM;

	if (has_acpi_companion(&pdev->dev)) {
		device_property_read_u32(&pdev->dev, "clock-frequency", &clk);
		sup->port.uartclk = clk;
	} else {
		sup->clk = devm_clk_get(&pdev->dev, NULL);
		if (IS_ERR(sup->clk))
			return PTR_ERR(sup->clk);
	}

	ret = platform_get_irq(pdev, 0);
	if (ret < 0)
		return ret;
	sup->port.irq	= ret;

	sup->vendor = &vendor_lrw;

	sup->reg_offset = sup->vendor->reg_offset;
	sup->fifosize	= LRW_UART_TX_FIFO_DEPTH;
	sup->port.iotype = sup->vendor->access_32b ? UPIO_MEM32 : UPIO_MEM;
	sup->port.ops = &lrw_uart_pops;
	sup->port.rs485_config = lrw_uart_rs485_config;
	sup->port.rs485_supported = lrw_uart_rs485_supported;
	sup->fixed_baud = baudrate;

	snprintf(sup->type, sizeof(sup->type), "LRW UART");

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	ret = lrw_uart_setup_port(&pdev->dev, sup, r, portnr);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, sup);

	return lrw_uart_register_port(sup);
}

static int lrw_uart_remove(struct platform_device *dev)
{
	struct lrw_uart_port *sup = platform_get_drvdata(dev);

	uart_remove_one_port(&lrw_uart_driver, &sup->port);
	lrw_uart_unregister_port(sup);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int lrw_uart_suspend(struct device *dev)
{
	struct lrw_uart_port *sup = dev_get_drvdata(dev);

	if (!sup)
		return -EINVAL;

	return uart_suspend_port(&lrw_uart_driver, &sup->port);
}

static int lrw_uart_resume(struct device *dev)
{
	struct lrw_uart_port *sup = dev_get_drvdata(dev);

	if (!sup)
		return -EINVAL;

	return uart_resume_port(&lrw_uart_driver, &sup->port);
}
#endif

static SIMPLE_DEV_PM_OPS(lrw_uart_pm_ops, lrw_uart_suspend, lrw_uart_resume);

static const struct of_device_id lrw_uart_of_match[] = {
	{ .compatible = "lrw,lrw-uart" },
	{},
};
MODULE_DEVICE_TABLE(of, lrw_uart_of_match);

static const struct acpi_device_id __maybe_unused lrw_uart_acpi_match[] = {
	{ "LRWX0000", 0 },
	{},
};
MODULE_DEVICE_TABLE(acpi, lrw_uart_acpi_match);

static struct platform_driver lrw_uart_platform_driver = {
	.probe		= lrw_uart_probe,
	.remove		= lrw_uart_remove,
	.driver		= {
		.name	= LRW_UART_NAME,
		.pm = pm_sleep_ptr(&lrw_uart_pm_ops),
		.of_match_table = of_match_ptr(lrw_uart_of_match),
		.acpi_match_table = ACPI_PTR(lrw_uart_acpi_match),
		.suppress_bind_attrs = IS_BUILTIN(CONFIG_SERIAL_LRW_UART),
	},
};

static int __init lrw_uart_init(void)
{
	pr_info("Serial: LRW UART driver\n");

	int ret;

	ret = uart_register_driver(&lrw_uart_driver);
	if (ret < 0) {
		pr_err("Could not register %s driver\n",
			lrw_uart_driver.driver_name);
		return ret;
	}

	ret = platform_driver_register(&lrw_uart_platform_driver);
	if (ret < 0) {
		pr_err("LRW UART platform driver register failed, e = %d\n", ret);
		uart_unregister_driver(&lrw_uart_driver);
		return ret;
	}

	return 0;
}

static void __exit lrw_uart_exit(void)
{
	platform_driver_unregister(&lrw_uart_platform_driver);
	uart_unregister_driver(&lrw_uart_driver);
}

/*
 * While this can be a module, if builtin it's most likely the console
 * So let's leave module_exit but move module_init to an earlier place
 */
arch_initcall(lrw_uart_init);
module_exit(lrw_uart_exit);

MODULE_AUTHOR("Wenhong Liu/Qingtao Liu");
MODULE_DESCRIPTION("LRW UART serial driver");
MODULE_LICENSE("GPL");
