// SPDX-License-Identifier: GPL-2.0+
/*
 *  Based on drivers/serial/8250.c by Russell King.
 *
 *  Author:	Nicolas Pitre
 *  Created:	Feb 20, 2003
 *  Copyright:	(C) 2003 Monta Vista Software, Inc.
 *  Copyright:  (C) 2023 Spacemit Co., Ltd.
 * Note 1: This driver is made separate from the already too overloaded
 * 8250.c because it needs some kirks of its own and that'll make it
 * easier to add DMA support.
 *
 * Note 2: I'm too sick of device allocation policies for serial ports.
 * If someone else wants to request an "official" allocation of major/minor
 * for this driver please be my guest.  And don't forget that new hardware
 * to come from Intel might have more than 3 or 4 of those UARTs.  Let's
 * hope for a better port registration and dynamic device allocation scheme
 * with the serial core maintainer satisfaction to appear soon.
 */

#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/serial.h>
#include <linux/serial_reg.h>
#include <linux/circ_buf.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/dma-direct.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/pm_qos.h>
#include <linux/pm_wakeup.h>
#include <linux/timer.h>
#include <linux/pm.h>

#define	DMA_BLOCK		UART_XMIT_SIZE
#define	DMA_BURST_SIZE		(8)
#define	DMA_FIFO_THRESHOLD	(32)
#define DMA_RX_BLOCK_SIZE	DMA_BLOCK
#define DMA_BUF_POLLING_SWITCH	(1)

#define PXA_UART_TX		(0)
#define PXA_UART_RX		(1)

#define NUM_UART_PORTS		(10)
#define BT_UART_PORT		(2)

#define UART_FCR_PXA_BUS32	(0x20)
#define UART_FCR_PXA_TRAIL	(0x10)
#define UART_FOR		(9)

#define PXA_NAME_LEN		(8)

#define SUPPORT_POWER_QOS	(1)

#define TX_DMA_RUNNING		BIT(0)
#define RX_DMA_RUNNING		BIT(1)

#define PXA_TIMER_TIMEOUT	(3 * HZ)
#define BLOCK_SUSPEND_TIMEOUT	(3000)

struct uart_pxa_dma {
	unsigned int		dma_status;
	struct dma_chan		*txdma_chan;
	struct dma_chan		*rxdma_chan;
	struct dma_async_tx_descriptor	*rx_desc;
	struct dma_async_tx_descriptor	*tx_desc;
	void			*txdma_addr;
	void			*rxdma_addr;
	dma_addr_t		txdma_addr_phys;
	dma_addr_t		rxdma_addr_phys;
	int			tx_stop;
	int			rx_stop;
	dma_cookie_t		rx_cookie;
	dma_cookie_t		tx_cookie;
	int			tx_size;
	struct	tasklet_struct	tklet;

#ifdef CONFIG_PM
	void			*tx_buf_save;
	int			tx_saved_len;
#endif

	bool			dma_init;

#if (DMA_BUF_POLLING_SWITCH == 1)
	int			dma_poll_timeout;
	int			dma_poll_max_time;
#endif
};

struct uart_pxa_port {
	struct uart_port	port;
	unsigned char		ier;
	unsigned char		lcr;
	unsigned char		mcr;
	unsigned int		lsr_break_flag;
	struct clk		*fclk;
	struct clk		*gclk;
	struct reset_control    *resets;
	char			name[PXA_NAME_LEN];

	struct timer_list	pxa_timer;
	int			edge_wakeup_gpio;
	struct work_struct	uart_tx_lpm_work;
	int			dma_enable;
	struct uart_pxa_dma	uart_dma;
	unsigned long		flags;
	unsigned int		cons_udelay;
	bool			from_resume;
	bool			device_ctrl_rts;
	bool			in_resume;
	unsigned int		current_baud;
};

static void pxa_uart_transmit_dma_cb(void *data);
static void pxa_uart_receive_dma_cb(void *data);
static void pxa_uart_transmit_dma_start(struct uart_pxa_port *up, int count);
static void pxa_uart_receive_dma_start(struct uart_pxa_port *up);
static inline void wait_for_xmitr(struct uart_pxa_port *up);
static unsigned int serial_pxa_tx_empty(struct uart_port *port);
#ifdef CONFIG_PM
static void _pxa_timer_handler(struct uart_pxa_port *up);
#endif

static inline void stop_dma(struct uart_pxa_port *up, int read)
{
	unsigned long flags;
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	struct dma_chan *channel;

	if (!pxa_dma->dma_init)
		return;

	channel = read ? pxa_dma->rxdma_chan : pxa_dma->txdma_chan;

	dmaengine_terminate_all(channel);
	spin_lock_irqsave(&up->port.lock, flags);
	if (read)
		pxa_dma->dma_status &= ~RX_DMA_RUNNING;
	else
		pxa_dma->dma_status &= ~TX_DMA_RUNNING;
	spin_unlock_irqrestore(&up->port.lock, flags);
}

static inline unsigned int serial_in(struct uart_pxa_port *up, int offset)
{
	offset <<= 2;
	return readl(up->port.membase + offset);
}

static inline void serial_out(struct uart_pxa_port *up, int offset, int value)
{
	offset <<= 2;
	writel(value, up->port.membase + offset);
}

static void serial_pxa_enable_ms(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	if (up->dma_enable)
		return;

	up->ier |= UART_IER_MSI;
	serial_out(up, UART_IER, up->ier);
}

static void serial_pxa_stop_tx(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned int timeout = 0x100000 / up->cons_udelay;
	unsigned long flags;

	if (up->dma_enable) {
		up->uart_dma.tx_stop = 1;

		if (up->ier & UART_IER_DMAE && up->uart_dma.dma_init) {
			while (dma_async_is_tx_complete(up->uart_dma.txdma_chan,
							up->uart_dma.tx_cookie, NULL, NULL)
			       != DMA_COMPLETE && (timeout-- > 0)) {
				spin_unlock(&up->port.lock);
				local_irq_save(flags);
				local_irq_enable();
				udelay(up->cons_udelay);
				local_irq_disable();
				local_irq_restore(flags);
				spin_lock(&up->port.lock);
			}

			WARN_ON_ONCE(timeout == 0);
		}
	} else {
		if (up->ier & UART_IER_THRI) {
			up->ier &= ~UART_IER_THRI;
			serial_out(up, UART_IER, up->ier);
		}
	}
}

static void serial_pxa_stop_rx(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	if (up->dma_enable) {
		if (up->ier & UART_IER_DMAE) {
			spin_unlock_irqrestore(&up->port.lock, up->flags);
			stop_dma(up, PXA_UART_RX);
			spin_lock_irqsave(&up->port.lock, up->flags);
		}
		up->uart_dma.rx_stop = 1;
	} else {
		up->ier &= ~UART_IER_RLSI;
		up->port.read_status_mask &= ~UART_LSR_DR;
		serial_out(up, UART_IER, up->ier);
	}
}

static inline void receive_chars(struct uart_pxa_port *up, int *status)
{
	unsigned int ch, flag;
	int max_count = 256;

	do {
		spin_lock_irqsave(&up->port.lock, up->flags);
		up->ier &= ~UART_IER_RTOIE;
		serial_out(up, UART_IER, up->ier);
		spin_unlock_irqrestore(&up->port.lock, up->flags);

		ch = serial_in(up, UART_RX);
		flag = TTY_NORMAL;
		up->port.icount.rx++;

		if (unlikely(*status & (UART_LSR_BI | UART_LSR_PE |
					   UART_LSR_FE | UART_LSR_OE))) {
			if (*status & UART_LSR_BI) {
				*status &= ~(UART_LSR_FE | UART_LSR_PE);
				up->port.icount.brk++;
				if (uart_handle_break(&up->port))
					goto ignore_char;
			} else if (*status & UART_LSR_PE) {
				up->port.icount.parity++;
			} else if (*status & UART_LSR_FE) {
				up->port.icount.frame++;
			}

			if (*status & UART_LSR_OE)
				up->port.icount.overrun++;

			*status &= up->port.read_status_mask;

#ifdef CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE
			if (up->port.line == up->port.cons->index) {
				*status |= up->lsr_break_flag;
				up->lsr_break_flag = 0;
			}
#endif
			if (*status & UART_LSR_BI)
				flag = TTY_BREAK;
			else if (*status & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (*status & UART_LSR_FE)
				flag = TTY_FRAME;
		}
		if (!uart_handle_sysrq_char(&up->port, ch))
			uart_insert_char(&up->port, *status, UART_LSR_OE, ch, flag);

ignore_char:
		*status = serial_in(up, UART_LSR);
	} while ((*status & UART_LSR_DR) && (max_count-- > 0));
	tty_flip_buffer_push(&up->port.state->port);

	spin_lock_irqsave(&up->port.lock, up->flags);
	up->ier |= UART_IER_RTOIE;
	serial_out(up, UART_IER, up->ier);
	spin_unlock_irqrestore(&up->port.lock, up->flags);
}

static void transmit_chars(struct uart_pxa_port *up)
{
	struct circ_buf *xmit = &up->port.state->xmit;
	int count;

	if (up->port.x_char) {
		serial_out(up, UART_TX, up->port.x_char);
		up->port.icount.tx++;
		up->port.x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(&up->port)) {
		spin_lock_irqsave(&up->port.lock, up->flags);
		serial_pxa_stop_tx(&up->port);
		spin_unlock_irqrestore(&up->port.lock, up->flags);
		return;
	}

	count = up->port.fifosize / 2;
	do {
		serial_out(up, UART_TX, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		up->port.icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	} while (--count > 0);

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&up->port);

	if (uart_circ_empty(xmit)) {
		spin_lock_irqsave(&up->port.lock, up->flags);
		serial_pxa_stop_tx(&up->port);
		spin_unlock_irqrestore(&up->port.lock, up->flags);
	}
}

static inline void dma_receive_chars(struct uart_pxa_port *up, int *status)
{
	struct tty_port *port = &up->port.state->port;
	unsigned char ch;
	int max_count = 256;
	int count = 0;
	unsigned char *tmp;
	unsigned int flag = TTY_NORMAL;
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	struct dma_tx_state dma_state;

	if (!pxa_dma->dma_init)
		return;

	dmaengine_pause(pxa_dma->rxdma_chan);
	dmaengine_tx_status(pxa_dma->rxdma_chan, pxa_dma->rx_cookie,
			    &dma_state);
	count = DMA_RX_BLOCK_SIZE - dma_state.residue;
	tmp = pxa_dma->rxdma_addr;
	if (up->port.sysrq) {
		while (count > 0) {
			if (!uart_handle_sysrq_char(&up->port, *tmp)) {
				uart_insert_char(&up->port, *status, 0, *tmp, flag);
				up->port.icount.rx++;
			}
			tmp++;
			count--;
		}
	} else {
		tty_insert_flip_string(port, tmp, count);
		up->port.icount.rx += count;
	}

	do {
		ch = serial_in(up, UART_RX);
		flag = TTY_NORMAL;
		up->port.icount.rx++;

		if (unlikely(*status & (UART_LSR_BI | UART_LSR_PE |
					UART_LSR_FE | UART_LSR_OE))) {
			if (*status & UART_LSR_BI) {
				*status &= ~(UART_LSR_FE | UART_LSR_PE);
				up->port.icount.brk++;
				if (uart_handle_break(&up->port))
					goto ignore_char2;
			} else if (*status & UART_LSR_PE) {
				up->port.icount.parity++;
			} else if (*status & UART_LSR_FE) {
				up->port.icount.frame++;
			}

			if (*status & UART_LSR_OE)
				up->port.icount.overrun++;

			*status &= up->port.read_status_mask;

#ifdef CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE
			if (up->port.line == up->port.cons->index) {
				*status |= up->lsr_break_flag;
				up->lsr_break_flag = 0;
			}
#endif
			if (*status & UART_LSR_BI)
				flag = TTY_BREAK;
			else if (*status & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (*status & UART_LSR_FE)
				flag = TTY_FRAME;
		}
		if (!uart_handle_sysrq_char(&up->port, ch))
			uart_insert_char(&up->port, *status, UART_LSR_OE,
					 ch, flag);
ignore_char2:
		*status = serial_in(up, UART_LSR);
	} while ((*status & UART_LSR_DR) && (max_count-- > 0));

	tty_flip_buffer_push(port);
	stop_dma(up, 1);
	if (pxa_dma->rx_stop)
		return;
	pxa_uart_receive_dma_start(up);
}

static void serial_pxa_start_tx(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	if (up->dma_enable) {
		up->uart_dma.tx_stop = 0;
		tasklet_schedule(&up->uart_dma.tklet);
	} else {
		if (!(up->ier & UART_IER_THRI)) {
			up->ier |= UART_IER_THRI;
			serial_out(up, UART_IER, up->ier);
		}
	}
}

static inline void check_modem_status(struct uart_pxa_port *up)
{
	int status;

	status = serial_in(up, UART_MSR);

	if ((status & UART_MSR_ANY_DELTA) == 0)
		return;

	spin_lock(&up->port.lock);
	if (status & UART_MSR_TERI)
		up->port.icount.rng++;
	if (status & UART_MSR_DDSR)
		up->port.icount.dsr++;
	if (status & UART_MSR_DDCD)
		uart_handle_dcd_change(&up->port, status & UART_MSR_DCD);
	if (status & UART_MSR_DCTS)
		uart_handle_cts_change(&up->port, status & UART_MSR_CTS);
	spin_unlock(&up->port.lock);

	wake_up_interruptible(&up->port.state->port.delta_msr_wait);
}

static int serial_pxa_is_open(struct uart_pxa_port *up);

static inline irqreturn_t serial_pxa_irq(int irq, void *dev_id)
{
	struct uart_pxa_port *up = dev_id;
	unsigned int iir, lsr;

	iir = serial_in(up, UART_IIR);
	if (iir & UART_IIR_NO_INT)
		return IRQ_NONE;

	if (!serial_pxa_is_open(up))
		return IRQ_HANDLED;

#ifdef CONFIG_PM
#if SUPPORT_POWER_QOS
	if (!mod_timer(&up->pxa_timer, jiffies + PXA_TIMER_TIMEOUT))
		pm_runtime_get_sync(up->port.dev);
#endif
#endif

	lsr = serial_in(up, UART_LSR);
	if (up->dma_enable) {
		if (lsr & UART_LSR_FIFOE)
			dma_receive_chars(up, &lsr);
	} else {
		if (lsr & UART_LSR_DR) {
			receive_chars(up, &lsr);
			if (up->edge_wakeup_gpio >= 0)
				pm_wakeup_event(up->port.dev, BLOCK_SUSPEND_TIMEOUT);
		}

		check_modem_status(up);
		if (lsr & UART_LSR_THRE) {
			transmit_chars(up);
			while (!serial_pxa_tx_empty((struct uart_port *)dev_id))
				;
		}
	}

	return IRQ_HANDLED;
}

static unsigned int serial_pxa_tx_empty(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&up->port.lock, flags);
	if (up->dma_enable) {
		if (up->ier & UART_IER_DMAE) {
			if (up->uart_dma.dma_status & TX_DMA_RUNNING) {
				spin_unlock_irqrestore(&up->port.lock, flags);
				return 0;
			}
		}
	}
	ret = serial_in(up, UART_LSR) & UART_LSR_TEMT ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&up->port.lock, flags);

	return ret;
}

static unsigned int serial_pxa_get_mctrl(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned char status;
	unsigned int ret;

	status = serial_in(up, UART_MSR);

	ret = 0;
	if (status & UART_MSR_DCD)
		ret |= TIOCM_CAR;
	if (status & UART_MSR_RI)
		ret |= TIOCM_RNG;
	if (status & UART_MSR_DSR)
		ret |= TIOCM_DSR;
	if (status & UART_MSR_CTS)
		ret |= TIOCM_CTS;
	return ret;
}

static void serial_pxa_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned char mcr = 0;
	int hostwake = 0;

	if (mctrl & TIOCM_RTS)
		mcr |= UART_MCR_RTS;
	if (mctrl & TIOCM_DTR)
		mcr |= UART_MCR_DTR;
	if (mctrl & TIOCM_OUT1)
		mcr |= UART_MCR_OUT1;
	if (mctrl & TIOCM_OUT2)
		mcr |= UART_MCR_OUT2;
	if (mctrl & TIOCM_LOOP)
		mcr |= UART_MCR_LOOP;

	if (up->device_ctrl_rts) {
		if ((hostwake || up->in_resume) && (mctrl & TIOCM_RTS))
			mcr &= ~UART_MCR_RTS;
	}

	mcr |= up->mcr;

	serial_out(up, UART_MCR, mcr);

#ifdef CONFIG_BT
	if (up->port.line == BT_UART_PORT)
		pr_debug("%s: rts: 0x%x\n", __func__, mcr & UART_MCR_RTS);
#endif
}

static void serial_pxa_break_ctl(struct uart_port *port, int break_state)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned long flags;

	spin_lock_irqsave(&up->port.lock, flags);
	if (break_state == -1)
		up->lcr |= UART_LCR_SBC;
	else
		up->lcr &= ~UART_LCR_SBC;
	serial_out(up, UART_LCR, up->lcr);
	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void pxa_uart_transmit_dma_start(struct uart_pxa_port *up, int count)
{
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	struct dma_slave_config slave_config;
	int ret;

	if (!pxa_dma->txdma_chan) {
		dev_err(up->port.dev, "tx dma channel is not initialized\n");
		return;
	}

	slave_config.direction = DMA_MEM_TO_DEV;
	slave_config.dst_addr = up->port.mapbase;
	slave_config.dst_maxburst = DMA_BURST_SIZE;
	slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;

	ret = dmaengine_slave_config(pxa_dma->txdma_chan, &slave_config);
	if (ret) {
		dev_err(up->port.dev,
			"%s: dmaengine slave config err.\n", __func__);
		return;
	}

	pxa_dma->tx_size = count;
	pxa_dma->tx_desc =
		dmaengine_prep_slave_single(pxa_dma->txdma_chan, pxa_dma->txdma_addr_phys,
					    count, DMA_MEM_TO_DEV, 0);
	if (!pxa_dma->tx_desc) {
		dev_err(up->port.dev,
			"%s: Unable to get desc for Tx\n", __func__);
		return;
	}
	pxa_dma->tx_desc->callback = pxa_uart_transmit_dma_cb;
	pxa_dma->tx_desc->callback_param = up;

	pxa_dma->tx_cookie = dmaengine_submit(pxa_dma->tx_desc);
#ifdef CONFIG_PM
#if SUPPORT_POWER_QOS
	pm_runtime_get_sync(up->port.dev);
#endif
#endif

	dma_async_issue_pending(pxa_dma->txdma_chan);
}

static void pxa_uart_receive_dma_start(struct uart_pxa_port *up)
{
	unsigned long flags;
	struct uart_pxa_dma *uart_dma = &up->uart_dma;
	struct dma_slave_config slave_config;
	int ret;

	if (!uart_dma->rxdma_chan) {
		dev_err(up->port.dev, "rx dma channel is not initialized\n");
		return;
	}

	spin_lock_irqsave(&up->port.lock, flags);
	if (uart_dma->dma_status & RX_DMA_RUNNING) {
		spin_unlock_irqrestore(&up->port.lock, flags);
		return;
	}
	uart_dma->dma_status |= RX_DMA_RUNNING;
	spin_unlock_irqrestore(&up->port.lock, flags);

	slave_config.direction = DMA_DEV_TO_MEM;
	slave_config.src_addr = up->port.mapbase;
	slave_config.src_maxburst = DMA_BURST_SIZE;
	slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_1_BYTE;

	ret = dmaengine_slave_config(uart_dma->rxdma_chan, &slave_config);
	if (ret) {
		dev_err(up->port.dev, "%s: dmaengine slave config err.\n", __func__);
		return;
	}

	uart_dma->rx_desc =
		dmaengine_prep_slave_single(uart_dma->rxdma_chan,
					    uart_dma->rxdma_addr_phys, DMA_RX_BLOCK_SIZE,
					    DMA_DEV_TO_MEM, 0);
	if (!uart_dma->rx_desc) {
		dev_err(up->port.dev, "%s: Unable to get desc for Rx\n", __func__);
		return;
	}
	uart_dma->rx_desc->callback = pxa_uart_receive_dma_cb;
	uart_dma->rx_desc->callback_param = up;

	uart_dma->rx_cookie = dmaengine_submit(uart_dma->rx_desc);
	dma_async_issue_pending(uart_dma->rxdma_chan);
}

static void pxa_uart_receive_dma_cb(void *data)
{
	unsigned long flags;
	struct uart_pxa_port *up = (struct uart_pxa_port *)data;
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	struct tty_port *port = &up->port.state->port;
	unsigned int count;
	unsigned char *tmp = pxa_dma->rxdma_addr;
	struct dma_tx_state dma_state;
#if (DMA_BUF_POLLING_SWITCH == 1)
	unsigned int buf_used, trail_cnt = 0;
	unsigned char *trail_addr, *last_burst_addr;
	u8 mark_1bytes = 0xff;
	int timeout = 5000, cycle = 2; /* us */
	int times_1 = 0, times_2 = 0, duration_time_us;
#endif

#ifdef CONFIG_PM
#if SUPPORT_POWER_QOS
	if (!mod_timer(&up->pxa_timer, jiffies + PXA_TIMER_TIMEOUT))
		pm_runtime_get_sync(up->port.dev);
#endif
#endif

	dmaengine_tx_status(pxa_dma->rxdma_chan, pxa_dma->rx_cookie, &dma_state);
	count = DMA_RX_BLOCK_SIZE - dma_state.residue;

#if (DMA_BUF_POLLING_SWITCH == 1)
	buf_used = count;
	if (count > 0 && count < DMA_FIFO_THRESHOLD) {
		trail_cnt = count;
		trail_addr = tmp;
		times_1 = timeout / cycle;
		times_2 = timeout / cycle;

		while ((*trail_addr == mark_1bytes) && (times_1-- >= 0))
			udelay(cycle);

		if (trail_cnt > 1) {
			trail_addr = trail_addr + trail_cnt - 1;
			while ((*trail_addr == mark_1bytes) && (times_2-- >= 0))
				udelay(cycle);
		}

		if (times_1 <= 0 || times_2 <= 0)
			pxa_dma->dma_poll_timeout++;
	}

	if (count >= DMA_FIFO_THRESHOLD && count < DMA_RX_BLOCK_SIZE) {
		trail_cnt = (count % DMA_BURST_SIZE) + (DMA_FIFO_THRESHOLD - DMA_BURST_SIZE);
		trail_addr = tmp + count - trail_cnt;

		#if (DMA_BURST_SIZE == DMA_FIFO_THRESHOLD)
		if (trail_cnt == 0) {
			trail_addr = tmp + count - DMA_BURST_SIZE;
			trail_cnt = DMA_BURST_SIZE;
		}
		#endif

		times_1 = timeout / cycle;
		times_2 = timeout / cycle;

		while ((*trail_addr == mark_1bytes) && (times_1-- >= 0))
			udelay(cycle);

		if (trail_cnt > 1) {
			trail_addr = trail_addr + trail_cnt - 1;
			while ((*trail_addr == mark_1bytes) && (times_2-- >= 0))
				udelay(cycle);
		}

		if (times_1 <= 0 || times_2 <= 0)
			pxa_dma->dma_poll_timeout++;
	}

	if (count == DMA_RX_BLOCK_SIZE) {
		last_burst_addr = tmp + DMA_RX_BLOCK_SIZE - DMA_BURST_SIZE;
		trail_cnt = DMA_BURST_SIZE;
		times_1 = timeout / cycle;
		times_2 = timeout / cycle;

		while ((*last_burst_addr == mark_1bytes) && (times_1-- >= 0))
			udelay(cycle);

		if (trail_cnt > 1) {
			last_burst_addr = tmp + DMA_RX_BLOCK_SIZE - 1;
			while ((*last_burst_addr == mark_1bytes) && (times_2-- >= 0))
				udelay(cycle);
		}

		if (times_1 <= 0 || times_2 <= 0)
			pxa_dma->dma_poll_timeout++;
	}
#endif	/* #if (DMA_BUF_POLLING_SWITCH == 1) */

	if (up->port.sysrq) {
		while (count > 0) {
			if (!uart_handle_sysrq_char(&up->port, *tmp)) {
				tty_insert_flip_char(port, *tmp, TTY_NORMAL);
				up->port.icount.rx++;
			}
			tmp++;
			count--;
		}
	} else {
		tty_insert_flip_string(port, tmp, count);
		up->port.icount.rx += count;
	}
	tty_flip_buffer_push(port);

	spin_lock_irqsave(&up->port.lock, flags);
	pxa_dma->dma_status &= ~RX_DMA_RUNNING;
	spin_unlock_irqrestore(&up->port.lock, flags);

#if (DMA_BUF_POLLING_SWITCH == 1)
	if (buf_used > 0) {
		tmp = pxa_dma->rxdma_addr;
		memset(tmp, mark_1bytes, buf_used);
	}

	if (times_1 > 0) {
		duration_time_us = (timeout / cycle - times_1) * cycle;
		if (pxa_dma->dma_poll_max_time < duration_time_us)
			pxa_dma->dma_poll_max_time = duration_time_us;
	}
	if (times_2 > 0) {
		duration_time_us = (timeout / cycle - times_2) * cycle;
		if (pxa_dma->dma_poll_max_time < duration_time_us)
			pxa_dma->dma_poll_max_time = duration_time_us;
	}
	if (times_1 > 0 && times_2 > 0) {
		duration_time_us = (2 * timeout / cycle - times_1 - times_2) * cycle;
		if (pxa_dma->dma_poll_max_time < duration_time_us)
			pxa_dma->dma_poll_max_time = duration_time_us;
	}
#endif	/* #if (DMA_BUF_POLLING_SWITCH == 1) */

	if (pxa_dma->rx_stop || !serial_pxa_is_open(up))
		return;
	pxa_uart_receive_dma_start(up);
	if (up->edge_wakeup_gpio >= 0)
		pm_wakeup_event(up->port.dev, BLOCK_SUSPEND_TIMEOUT);
}

static void pxa_uart_transmit_dma_cb(void *data)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)data;
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	struct circ_buf *xmit = &up->port.state->xmit;

	if (up->from_resume)
		up->from_resume = false;

	if (dma_async_is_tx_complete(pxa_dma->txdma_chan, pxa_dma->tx_cookie,
				     NULL, NULL) == DMA_COMPLETE)
		schedule_work(&up->uart_tx_lpm_work);

	spin_lock_irqsave(&up->port.lock, up->flags);
	pxa_dma->dma_status &= ~TX_DMA_RUNNING;
	spin_unlock_irqrestore(&up->port.lock, up->flags);

	if (pxa_dma->tx_stop || !serial_pxa_is_open(up))
		return;

	if (up->port.x_char) {
		serial_out(up, UART_TX, up->port.x_char);
		up->port.icount.tx++;
		up->port.x_char = 0;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&up->port);

	if (!uart_circ_empty(xmit))
		tasklet_schedule(&pxa_dma->tklet);
}

static void pxa_uart_dma_init(struct uart_pxa_port *up)
{
	struct uart_pxa_dma *pxa_dma = &up->uart_dma;
	dma_cap_mask_t mask;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	if (!pxa_dma->rxdma_chan) {
		pxa_dma->rxdma_chan = dma_request_slave_channel(up->port.dev, "rx");
		if (IS_ERR_OR_NULL(pxa_dma->rxdma_chan)) {
			dev_WARN_ONCE(up->port.dev, 1, "failed to request rx dma channel\n");
			goto out;
		}
	}

	if (!pxa_dma->txdma_chan) {
		pxa_dma->txdma_chan = dma_request_slave_channel(up->port.dev, "tx");
		if (IS_ERR_OR_NULL(pxa_dma->txdma_chan)) {
			dev_WARN_ONCE(up->port.dev, 1, "failed to request tx dma channel\n");
			goto err_txdma;
		}
	}

	if (!pxa_dma->txdma_addr) {
		pxa_dma->txdma_addr =
			dma_direct_alloc(up->port.dev, DMA_BLOCK,
					 &pxa_dma->txdma_addr_phys, GFP_KERNEL,
					 DMA_ATTR_FORCE_CONTIGUOUS);
		if (!pxa_dma->txdma_addr) {
			dev_WARN_ONCE(up->port.dev, 1, "failed to allocate tx dma memory\n");
			goto txdma_err_alloc;
		}
	}

	if (!pxa_dma->rxdma_addr) {
		pxa_dma->rxdma_addr =
			dma_direct_alloc(up->port.dev, DMA_RX_BLOCK_SIZE,
					 &pxa_dma->rxdma_addr_phys, GFP_KERNEL,
					 DMA_ATTR_FORCE_CONTIGUOUS);
		if (!pxa_dma->rxdma_addr) {
			dev_WARN_ONCE(up->port.dev, 1, "failed to allocate rx dma memory\n");
			goto rxdma_err_alloc;
		}
	}

	pxa_dma->dma_status = 0;
	pxa_dma->dma_init = true;
	return;

rxdma_err_alloc:
	dma_direct_free(up->port.dev, DMA_BLOCK, pxa_dma->txdma_addr,
			pxa_dma->txdma_addr_phys, DMA_ATTR_FORCE_CONTIGUOUS);
	pxa_dma->txdma_addr = NULL;
txdma_err_alloc:
	dma_release_channel(pxa_dma->txdma_chan);
	pxa_dma->txdma_chan = NULL;
err_txdma:
	dma_release_channel(pxa_dma->rxdma_chan);
	pxa_dma->rxdma_chan = NULL;
out:
	pxa_dma->dma_init = false;
}

static void pxa_uart_dma_uninit(struct uart_pxa_port *up)
{
	struct uart_pxa_dma *pxa_dma;

	pxa_dma = &up->uart_dma;

	stop_dma(up, PXA_UART_TX);
	stop_dma(up, PXA_UART_RX);

	pxa_dma->dma_init = false;

	if (pxa_dma->txdma_chan) {
		dma_release_channel(pxa_dma->txdma_chan);
		pxa_dma->txdma_chan = NULL;
	}

	if (pxa_dma->txdma_addr) {
		dma_direct_free(up->port.dev, DMA_BLOCK, pxa_dma->txdma_addr,
				pxa_dma->txdma_addr_phys,
				DMA_ATTR_FORCE_CONTIGUOUS);
		pxa_dma->txdma_addr = NULL;
	}

	if (pxa_dma->rxdma_chan) {
		dma_release_channel(pxa_dma->rxdma_chan);
		pxa_dma->rxdma_chan = NULL;
	}

	if (pxa_dma->rxdma_addr) {
		dma_direct_free(up->port.dev, DMA_RX_BLOCK_SIZE, pxa_dma->rxdma_addr,
				pxa_dma->rxdma_addr_phys,
				DMA_ATTR_FORCE_CONTIGUOUS);
		pxa_dma->rxdma_addr = NULL;
	}
}

static void uart_task_action(unsigned long data)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)data;
	struct circ_buf *xmit = &up->port.state->xmit;
	unsigned char *tmp = up->uart_dma.txdma_addr;
	unsigned long flags;
	int count = 0, c;

	if (up->uart_dma.tx_stop || up->port.suspended ||
	    !serial_pxa_is_open(up) || up->from_resume)
		return;

	spin_lock_irqsave(&up->port.lock, flags);
	if (up->uart_dma.dma_status & TX_DMA_RUNNING) {
		spin_unlock_irqrestore(&up->port.lock, flags);
		return;
	}

	up->uart_dma.dma_status |= TX_DMA_RUNNING;
	while (1) {
		c = CIRC_CNT_TO_END(xmit->head, xmit->tail, UART_XMIT_SIZE);
		if (c <= 0)
			break;

		memcpy(tmp, xmit->buf + xmit->tail, c);
		xmit->tail = (xmit->tail + c) & (UART_XMIT_SIZE - 1);
		tmp += c;
		count += c;
		up->port.icount.tx += c;
	}
	spin_unlock_irqrestore(&up->port.lock, flags);

	pr_debug("count =%d", count);
	pxa_uart_transmit_dma_start(up, count);
}

static int serial_pxa_startup(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned long flags;
	int tmp = 0;

	if (port->line == 3)
		up->mcr |= UART_MCR_AFE;
	else
		up->mcr = 0;

	up->port.uartclk = clk_get_rate(up->fclk);

	enable_irq(up->port.irq);

	serial_out(up, UART_FCR, UART_FCR_ENABLE_FIFO);
	serial_out(up, UART_FCR, UART_FCR_ENABLE_FIFO |
			UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT);
	serial_out(up, UART_FCR, 0);

	(void)serial_in(up, UART_LSR);
	(void)serial_in(up, UART_RX);
	(void)serial_in(up, UART_IIR);
	(void)serial_in(up, UART_MSR);

	serial_out(up, UART_LCR, UART_LCR_WLEN8);

	spin_lock_irqsave(&up->port.lock, flags);
	up->port.mctrl |= TIOCM_OUT2;
	tmp = serial_in(up, UART_MCR);
	tmp |= TIOCM_OUT2;
	serial_out(up, UART_MCR, tmp);
	spin_unlock_irqrestore(&up->port.lock, flags);

	if (up->dma_enable) {
		pxa_uart_dma_init(up);
		up->uart_dma.rx_stop = 0;
		pxa_uart_receive_dma_start(up);
		tasklet_init(&up->uart_dma.tklet, uart_task_action, (unsigned long)up);
	}

	spin_lock_irqsave(&up->port.lock, flags);
	if (up->dma_enable)
		up->ier = UART_IER_DMAE | UART_IER_UUE;
	else
		up->ier = UART_IER_RLSI | UART_IER_RDI | UART_IER_RTOIE | UART_IER_UUE;
	serial_out(up, UART_IER, up->ier);
	spin_unlock_irqrestore(&up->port.lock, flags);

	(void)serial_in(up, UART_LSR);
	(void)serial_in(up, UART_RX);
	(void)serial_in(up, UART_IIR);
	(void)serial_in(up, UART_MSR);

	return 0;
}

static void serial_pxa_shutdown(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned long flags;
	unsigned int tmp = 0;

	disable_irq(up->port.irq);
	if (up->dma_enable) {
		tasklet_kill(&up->uart_dma.tklet);
		up->uart_dma.tx_stop = 1;
		up->uart_dma.rx_stop = 1;
		pxa_uart_dma_uninit(up);
	}

	flush_work(&up->uart_tx_lpm_work);

	spin_lock_irqsave(&up->port.lock, flags);
	up->ier = 0;
	serial_out(up, UART_IER, 0);

	up->port.mctrl &= ~TIOCM_OUT2;
	tmp = serial_in(up, UART_MCR);
	tmp &= ~TIOCM_OUT2;
	serial_out(up, UART_MCR, tmp);
	spin_unlock_irqrestore(&up->port.lock, flags);

	serial_out(up, UART_LCR, serial_in(up, UART_LCR) & ~UART_LCR_SBC);
	serial_out(up, UART_FCR, UART_FCR_ENABLE_FIFO |
				  UART_FCR_CLEAR_RCVR |
				  UART_FCR_CLEAR_XMIT);
	serial_out(up, UART_FCR, 0);
}

static int pxa_set_baudrate_clk(struct uart_port *port, unsigned int baud)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned long rate;
	int ret;

	if (up->current_baud == baud)
		return 0;

	switch (baud) {
	case 500000:
	case 1000000:
	case 1500000:
	case 3000000:
		rate = 48000000;
		break;
	case 576000:
	case 1152000:
	case 2500000:
	case 4000000:
		rate = 73000000;
		break;
	case 2000000:
	case 3500000:
		rate = 58000000;
		break;
	default:
		rate = 14700000;
		break;
	}

	ret = clk_set_rate(up->fclk, rate);
	if (ret < 0) {
		dev_err(port->dev,
			"Failed to set clk rate %lu\n", rate);
		return ret;
	}

	up->port.uartclk = clk_get_rate(up->fclk);
	up->current_baud = baud;

	return 0;
}

static void serial_pxa_set_termios(struct uart_port *port,
				   struct ktermios *termios,
				   const struct ktermios *old)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned char cval, fcr = 0;
	unsigned long flags;
	unsigned int baud, quot;
	unsigned int dll;
	int ret;

	if (up->dma_enable && up->uart_dma.dma_init)
		stop_dma(up, PXA_UART_RX);

	cval = UART_LCR_WLEN(tty_get_char_size(termios->c_cflag));

	if (termios->c_cflag & CSTOPB)
		cval |= UART_LCR_STOP;
	if (termios->c_cflag & PARENB)
		cval |= UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		cval |= UART_LCR_EPAR;

	baud = uart_get_baud_rate(port, termios, old, 0, 4000000);
	if (!baud)
		baud = 9600;
	ret = pxa_set_baudrate_clk(port, baud);
	if (ret < 0) {
		dev_err(port->dev, "Failed to set baud rate clk: %d\n", ret);
		return;
	}
	if (tty_termios_baud_rate(termios))
		tty_termios_encode_baud_rate(termios, baud, baud);

	quot = uart_get_divisor(port, baud);

	if (!quot)
		quot = 1;
	if (up->dma_enable) {
		fcr = UART_FCR_ENABLE_FIFO | UART_FCR_PXAR32 | UART_FCR_PXA_TRAIL;
		fcr &= ~UART_FCR_PXA_BUS32;
	} else {
		if ((up->port.uartclk / quot) < (2400 * 16))
			fcr = UART_FCR_ENABLE_FIFO | UART_FCR_PXAR1;
		else if ((up->port.uartclk / quot) < (230400 * 16))
			fcr = UART_FCR_ENABLE_FIFO | UART_FCR_PXAR8;
		else
			fcr = UART_FCR_ENABLE_FIFO | UART_FCR_PXAR32;
	}

	spin_lock_irqsave(&up->port.lock, flags);
	up->ier |= UART_IER_UUE;
	uart_update_timeout(port, termios->c_cflag, baud);
	up->port.read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		up->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (IGNBRK | BRKINT | PARMRK))
		up->port.read_status_mask |= UART_LSR_BI;

	up->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		up->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		up->port.ignore_status_mask |= UART_LSR_BI;
		if (termios->c_iflag & IGNPAR)
			up->port.ignore_status_mask |= UART_LSR_OE;
	}

	if ((termios->c_cflag & CREAD) == 0)
		up->port.ignore_status_mask |= UART_LSR_DR;

	if (!up->dma_enable) {
		up->ier &= ~UART_IER_MSI;
		if (UART_ENABLE_MS(&up->port, termios->c_cflag))
			up->ier |= UART_IER_MSI;
	}

	serial_out(up, UART_IER, up->ier);

	if (termios->c_cflag & CRTSCTS)
		up->mcr |= UART_MCR_AFE;
	else
		up->mcr &= ~UART_MCR_AFE;

	serial_out(up, UART_LCR, cval | UART_LCR_DLAB);
	serial_out(up, UART_DLM, (quot >> 8) & 0xff);
	(void)serial_in(up, UART_DLM);
	serial_out(up, UART_DLL, quot & 0xff);

	(void)serial_in(up, UART_DLL);
	dll = serial_in(up, UART_DLL);
	WARN(dll != (quot & 0xff), "uart %d baud %d target 0x%x real 0x%x\n",
	     up->port.line, baud, quot & 0xff, dll);

	serial_out(up, UART_LCR, cval);
	up->lcr = cval;
	serial_pxa_set_mctrl(&up->port, up->port.mctrl);
	serial_out(up, UART_FCR, fcr);
	spin_unlock_irqrestore(&up->port.lock, flags);

	if (uart_console(&up->port)) {
		up->cons_udelay = 1000000000 / baud * 10 / 8 / 1000;
		if (up->cons_udelay <= 0)
			up->cons_udelay = 1;
		if (up->cons_udelay > 20)
			up->cons_udelay = 20;
	}

	if (up->dma_enable && up->uart_dma.dma_init)
		pxa_uart_receive_dma_start(up);
}

static void serial_pxa_pm(struct uart_port *port, unsigned int state, unsigned int oldstate)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	if (!state) {
		clk_prepare_enable(up->gclk);
		clk_prepare_enable(up->fclk);
	} else {
		clk_disable_unprepare(up->fclk);
		clk_disable_unprepare(up->gclk);
	}
}

static void serial_pxa_release_port(struct uart_port *port)
{
}

static int serial_pxa_request_port(struct uart_port *port)
{
	return 0;
}

static void serial_pxa_config_port(struct uart_port *port, int flags)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	up->port.type = PORT_PXA;
}

static int serial_pxa_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return -EINVAL;
}

static const char *serial_pxa_type(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	return up->name;
}

static struct uart_pxa_port *serial_pxa_ports[NUM_UART_PORTS];
static struct uart_driver serial_pxa_reg;

#ifdef CONFIG_PM
void serial_pxa_get_qos(int port)
{
	struct uart_pxa_port *up;

	if (port < 0 || port >= NUM_UART_PORTS) {
		pr_err("%s: wrong uart port %d\n", __func__, port);
		return;
	}

	up = serial_pxa_ports[port];
	if (!mod_timer(&up->pxa_timer, jiffies + PXA_TIMER_TIMEOUT))
		pm_runtime_get_sync(up->port.dev);
}
EXPORT_SYMBOL_GPL(serial_pxa_get_qos);
#endif

void serial_pxa_assert_rts(int port)
{
	struct uart_pxa_port *up;
	unsigned long flags;

	if (port < 0 || port >= NUM_UART_PORTS) {
		pr_err("%s: wrong uart port %d\n", __func__, port);
		return;
	}

	up = serial_pxa_ports[port];

	spin_lock_irqsave(&up->port.lock, flags);
	if (!serial_pxa_is_open(up)) {
		spin_unlock_irqrestore(&up->port.lock, flags);
		pr_err("%s: uart %d is shutdown\n", __func__, port);
		return;
	}
	serial_pxa_set_mctrl(&up->port, up->port.mctrl | TIOCM_RTS);
	uart_handle_cts_change(&up->port, UART_MSR_CTS);
	spin_unlock_irqrestore(&up->port.lock, flags);
}
EXPORT_SYMBOL_GPL(serial_pxa_assert_rts);

void serial_pxa_deassert_rts(int port)
{
	struct uart_pxa_port *up;
	unsigned long flags;

	if (port < 0 || port >= NUM_UART_PORTS) {
		pr_err("%s: wrong uart port %d\n", __func__, port);
		return;
	}

	up = serial_pxa_ports[port];

	spin_lock_irqsave(&up->port.lock, flags);
	if (!serial_pxa_is_open(up)) {
		spin_unlock_irqrestore(&up->port.lock, flags);
		pr_err("%s: uart %d is shutdown\n", __func__, port);
		return;
	}
	serial_pxa_set_mctrl(&up->port, up->port.mctrl & ~TIOCM_RTS);
	spin_unlock_irqrestore(&up->port.lock, flags);
}
EXPORT_SYMBOL_GPL(serial_pxa_deassert_rts);

#ifdef CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

static void wait_for_xmitr(struct uart_pxa_port *up)
{
	unsigned int status, tmout = 10000;
	unsigned int cycle;

	if (uart_console(&up->port))
		cycle = up->cons_udelay;
	else
		cycle = 1;

	tmout = 10000 / cycle;

	do {
		status = serial_in(up, UART_LSR);

		if (status & UART_LSR_BI)
			up->lsr_break_flag = UART_LSR_BI;

		if ((status & BOTH_EMPTY) == BOTH_EMPTY)
			break;
		udelay(cycle);
	} while (--tmout);

	if (up->port.flags & UPF_CONS_FLOW) {
		tmout = 1000000;
		while (--tmout && ((serial_in(up, UART_MSR) & UART_MSR_CTS) == 0))
			udelay(cycle);
	}

	if (!tmout) {
		if (up->port.flags & UPF_CONS_FLOW)
			status = serial_in(up, UART_MSR);
		else
			status = serial_in(up, UART_LSR);
		panic("failed to read uart status, status:0x%08x\n", status);
	}
}

static void serial_pxa_console_putchar(struct uart_port *port, unsigned char ch)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	wait_for_xmitr(up);
	serial_out(up, UART_TX, ch);
}

static void serial_pxa_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_pxa_port *up = serial_pxa_ports[co->index];
	unsigned int ier;
	unsigned long flags;
	int locked = 1;

	clk_enable(up->gclk);
	clk_enable(up->fclk);

	local_irq_save(flags);
	if (up->port.sysrq)
		locked = 0;
	else if (oops_in_progress)
		locked = spin_trylock(&up->port.lock);
	else
		spin_lock(&up->port.lock);

	ier = serial_in(up, UART_IER);
	serial_out(up, UART_IER, UART_IER_UUE);

	uart_console_write(&up->port, s, count, serial_pxa_console_putchar);

	wait_for_xmitr(up);
	serial_out(up, UART_IER, ier);

	if (locked)
		spin_unlock(&up->port.lock);
	local_irq_restore(flags);
	clk_disable(up->fclk);
	clk_disable(up->gclk);
}

#ifdef CONFIG_CONSOLE_POLL
static int serial_pxa_get_poll_char(struct uart_port *port)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;
	unsigned char lsr = serial_in(up, UART_LSR);

	while (!(lsr & UART_LSR_DR))
		lsr = serial_in(up, UART_LSR);

	return serial_in(up, UART_RX);
}

static void serial_pxa_put_poll_char(struct uart_port *port,
				     unsigned char c)
{
	unsigned int ier;
	struct uart_pxa_port *up = (struct uart_pxa_port *)port;

	ier = serial_in(up, UART_IER);
	serial_out(up, UART_IER, UART_IER_UUE);

	wait_for_xmitr(up);
	serial_out(up, UART_TX, c);
	if (c == 10) {
		wait_for_xmitr(up);
		serial_out(up, UART_TX, 13);
	}

	wait_for_xmitr(up);
	serial_out(up, UART_IER, ier);
}

#endif /* CONFIG_CONSOLE_POLL */

static int __init
serial_pxa_console_setup(struct console *co, char *options)
{
	struct uart_pxa_port *up;
	int baud = 9600;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index == -1 || co->index >= serial_pxa_reg.nr)
		co->index = 0;
	up = serial_pxa_ports[co->index];
	if (!up)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(&up->port, co, baud, parity, bits, flow);
}

static struct console serial_pxa_console = {
	.name	= "ttySP",
	.write	= serial_pxa_console_write,
	.device	= uart_console_device,
	.setup	= serial_pxa_console_setup,
	.flags	= CON_PRINTBUFFER,
	.index	= -1,
	.data	= &serial_pxa_reg,
};

static void pxa_early_write(struct console *con, const char *s,
			    unsigned int n)
{
	struct earlycon_device *dev = con->data;

	uart_console_write(&dev->port, s, n, serial_pxa_console_putchar);
}

static int __init pxa_early_console_setup(struct earlycon_device *device, const char *opt)
{
	if (!device->port.membase)
		return -ENODEV;

	device->con->write = pxa_early_write;
	return 0;
}

EARLYCON_DECLARE(pxa_serial, pxa_early_console_setup);
OF_EARLYCON_DECLARE(pxa_serial, "spacemit,pxa-uart", pxa_early_console_setup);

#define PXA_CONSOLE	(&serial_pxa_console)
#else
#define PXA_CONSOLE	NULL
#endif	/* CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE */

static const struct uart_ops serial_pxa_pops = {
	.tx_empty	= serial_pxa_tx_empty,
	.set_mctrl	= serial_pxa_set_mctrl,
	.get_mctrl	= serial_pxa_get_mctrl,
	.stop_tx	= serial_pxa_stop_tx,
	.start_tx	= serial_pxa_start_tx,
	.stop_rx	= serial_pxa_stop_rx,
	.enable_ms	= serial_pxa_enable_ms,
	.break_ctl	= serial_pxa_break_ctl,
	.startup	= serial_pxa_startup,
	.shutdown	= serial_pxa_shutdown,
	.set_termios	= serial_pxa_set_termios,
	.pm		= serial_pxa_pm,
	.type		= serial_pxa_type,
	.release_port	= serial_pxa_release_port,
	.request_port	= serial_pxa_request_port,
	.config_port	= serial_pxa_config_port,
	.verify_port	= serial_pxa_verify_port,
#if defined(CONFIG_CONSOLE_POLL) && defined(CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE)
	.poll_get_char = serial_pxa_get_poll_char,
	.poll_put_char = serial_pxa_put_poll_char,
#endif
};

static struct uart_driver serial_pxa_reg = {
	.owner		= THIS_MODULE,
	.driver_name	= "PXA serial",
	.dev_name	= "ttySP",
	.major		= TTY_MAJOR,
	.minor		= 128,
	.nr		= NUM_UART_PORTS,
	.cons		= PXA_CONSOLE,
};

static int serial_pxa_is_open(struct uart_pxa_port *up)
{
	struct uart_state *state;
	struct uart_pxa_dma *pxa_dma;

	if (!up)
		return 0;

	state = serial_pxa_reg.state + up->port.line;
	pxa_dma = &up->uart_dma;

	if (up->dma_enable) {
		return ((up->ier & UART_IER_DMAE) && pxa_dma->dma_init &&
			(state->pm_state == UART_PM_STATE_ON));
	} else {
		return (state->pm_state == UART_PM_STATE_ON);
	}
}

#ifdef CONFIG_PM

#ifdef CONFIG_HIBERNATION
unsigned long pxa_clk_freq;
struct clk *pxa_clk_parent;
#endif

static int serial_pxa_suspend(struct device *dev)
{
	struct uart_pxa_port *sport = dev_get_drvdata(dev);
	struct uart_pxa_dma *pxa_dma = &sport->uart_dma;
	struct dma_tx_state dma_state;
	unsigned char tmp[256];
	int fifo_cnt, cnt = 0;

	if (!console_suspend_enabled || !sport)
		return 0;

	if (serial_pxa_is_open(sport) && sport->dma_enable) {
		int sent = 0;
		unsigned long flags;

		local_irq_save(flags);
		pxa_dma->tx_stop = 1;
		pxa_dma->rx_stop = 1;
		pxa_dma->tx_saved_len = 0;
		if (dma_async_is_tx_complete(pxa_dma->txdma_chan,
					     pxa_dma->tx_cookie, NULL, NULL) != DMA_COMPLETE) {
			dmaengine_pause(pxa_dma->txdma_chan);
			udelay(100);
			dmaengine_tx_status(pxa_dma->txdma_chan,
					    pxa_dma->tx_cookie, &dma_state);
			sent = pxa_dma->tx_size - dma_state.residue;
			pxa_dma->tx_saved_len = dma_state.residue;
			memcpy(pxa_dma->tx_buf_save, pxa_dma->txdma_addr + sent,
			       dma_state.residue);
			stop_dma(sport, PXA_UART_TX);
		}

		if (dma_async_is_tx_complete(pxa_dma->rxdma_chan,
					     pxa_dma->rx_cookie, NULL, NULL) != DMA_COMPLETE) {
			spin_lock(&sport->port.lock);
			serial_pxa_set_mctrl(&sport->port, sport->port.mctrl & ~TIOCM_RTS);
			spin_unlock(&sport->port.lock);
			udelay(100);
			dmaengine_pause(pxa_dma->rxdma_chan);
			udelay(100);
			pxa_uart_receive_dma_cb(sport);
			stop_dma(sport, PXA_UART_RX);

			fifo_cnt = serial_in(sport, UART_FOR);
			while (fifo_cnt > 0) {
				*(tmp + cnt) = serial_in(sport, UART_RX) & 0xff;
				cnt++;
				fifo_cnt = serial_in(sport, UART_FOR);
			}

			if (cnt > 0) {
				tty_insert_flip_string(&sport->port.state->port, tmp, cnt);
				sport->port.icount.rx += cnt;
				tty_flip_buffer_push(&sport->port.state->port);
			}
		}
		local_irq_restore(flags);
	}

	if (sport) {
#ifdef CONFIG_HIBERNATION
		pxa_clk_freq = clk_get_rate(sport->fclk);
		pxa_clk_parent = clk_get_parent(sport->fclk);
#endif
		uart_suspend_port(&serial_pxa_reg, &sport->port);
#ifdef CONFIG_HIBERNATION
		clk_set_parent(sport->fclk, NULL);
#endif
	}

#ifdef CONFIG_PM
	if (del_timer_sync(&sport->pxa_timer))
		_pxa_timer_handler(sport);
#endif

	return 0;
}

static int serial_pxa_resume(struct device *dev)
{
	struct uart_pxa_port *sport = dev_get_drvdata(dev);
	struct uart_pxa_dma *pxa_dma = &sport->uart_dma;

	if (!console_suspend_enabled || !sport)
		return 0;

	sport->in_resume = true;

#ifdef CONFIG_HIBERNATION
	clk_set_parent(sport->fclk, pxa_clk_parent);
	clk_set_rate(sport->fclk, pxa_clk_freq);
#endif
	uart_resume_port(&serial_pxa_reg, &sport->port);

	if (serial_pxa_is_open(sport) && sport->dma_enable) {
		if (pxa_dma->tx_saved_len > 0) {
			sport->from_resume = true;
			memcpy(pxa_dma->txdma_addr, pxa_dma->tx_buf_save,
			       pxa_dma->tx_saved_len);
			pxa_uart_transmit_dma_start(sport,
						    pxa_dma->tx_saved_len);
		} else {
			tasklet_schedule(&pxa_dma->tklet);
		}

		pxa_uart_receive_dma_start(sport);
	}
	sport->in_resume = false;

	return 0;
}

static SIMPLE_DEV_PM_OPS(serial_pxa_pm_ops, serial_pxa_suspend, serial_pxa_resume);
#endif

#ifdef CONFIG_PM
static void _pxa_timer_handler(struct uart_pxa_port *up)
{
#if SUPPORT_POWER_QOS
	pm_runtime_put_sync(up->port.dev);
#endif
}

static void pxa_timer_handler(struct timer_list *t)
{
	struct uart_pxa_port *up = from_timer(up, t, pxa_timer);

	_pxa_timer_handler(up);
}

static void __maybe_unused uart_edge_wakeup_handler(int gpio, void *data)
{
	struct uart_pxa_port *up = (struct uart_pxa_port *)data;

	if (!mod_timer(&up->pxa_timer, jiffies + PXA_TIMER_TIMEOUT)) {
#if SUPPORT_POWER_QOS
		pm_runtime_get_sync(up->port.dev);
#endif
	}
	pm_wakeup_event(up->port.dev, BLOCK_SUSPEND_TIMEOUT);
}

static void uart_tx_lpm_handler(struct work_struct *work)
{
	struct uart_pxa_port *up = container_of(work, struct uart_pxa_port, uart_tx_lpm_work);

	while (!(serial_in(up, UART_LSR) & UART_LSR_TEMT))
		usleep_range(1000, 2000);

#if SUPPORT_POWER_QOS
	pm_runtime_put_sync(up->port.dev);
#endif
}
#endif

static const struct of_device_id serial_k1x_dt_ids[] = {
	{ .compatible = "spacemit,k1x-uart", },
	{}
};

static int serial_pxa_probe_dt(struct platform_device *pdev, struct uart_pxa_port *sport)
{
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!np)
		return 1;

	if (of_get_property(np, "dmas", NULL))
		sport->dma_enable = 1;

	ret = of_alias_get_id(np, "serial");
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to get alias id, errno %d\n", ret);
		return ret;
	}
	sport->port.line = ret;

#ifdef CONFIG_PM
	if (of_property_read_u32(np, "edge-wakeup-pin", &sport->edge_wakeup_gpio))
		dev_dbg(&pdev->dev, "no edge-wakeup-pin defined\n");
#endif
	sport->device_ctrl_rts = of_property_read_bool(np, "device-control-rts");

	return 0;
}

static int serial_pxa_probe(struct platform_device *dev)
{
	struct uart_pxa_port *sport;
	struct resource *mmres;
	int ret;
	int irq;
	struct resource *dmares;
	struct uart_pxa_dma *pxa_dma;

	mmres = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!mmres)
		return -ENODEV;

	irq = platform_get_irq(dev, 0);
	if (irq < 0)
		return irq;

	sport = kzalloc(sizeof(*sport), GFP_KERNEL);
	if (!sport)
		return -ENOMEM;

#ifdef CONFIG_PM
	sport->uart_dma.tx_buf_save = kmalloc(DMA_BLOCK, GFP_KERNEL);
	if (!sport->uart_dma.tx_buf_save) {
		kfree(sport);
		return -ENOMEM;
	}
#endif
	sport->gclk = devm_clk_get(&dev->dev, "gate");
	if (IS_ERR(sport->gclk)) {
		ret = PTR_ERR(sport->gclk);
		goto err_free;
	}

	sport->fclk = devm_clk_get(&dev->dev, "func");
	if (IS_ERR(sport->fclk)) {
		ret = PTR_ERR(sport->fclk);
		goto err_free;
	}

	if (sport->gclk) {
		ret = clk_prepare(sport->gclk);
		if (ret) {
			clk_put(sport->gclk);
			goto err_free;
		}
	}

	if (sport->fclk) {
		ret = clk_prepare(sport->fclk);
		if (ret) {
			clk_put(sport->fclk);
			goto err_free;
		}
	}

	sport->port.type = PORT_PXA;
	sport->port.iotype = UPIO_MEM;
	sport->port.mapbase = mmres->start;
	sport->port.irq = irq;
	sport->port.fifosize = 64;
	sport->port.ops = &serial_pxa_pops;
	sport->port.dev = &dev->dev;
	sport->port.flags = UPF_IOREMAP | UPF_BOOT_AUTOCONF;
	sport->port.uartclk = clk_get_rate(sport->fclk);
	sport->resets = devm_reset_control_get_optional(&dev->dev, NULL);
	if (IS_ERR(sport->resets)) {
		ret = PTR_ERR(sport->resets);
		goto err_clk;
	}
	reset_control_deassert(sport->resets);

	sport->port.has_sysrq = IS_ENABLED(CONFIG_SERIAL_SPACEMIT_K1X_CONSOLE);

	sport->edge_wakeup_gpio = -1;

	pxa_dma = &sport->uart_dma;
	pxa_dma->txdma_chan = NULL;
	pxa_dma->rxdma_chan = NULL;
	pxa_dma->txdma_addr = NULL;
	pxa_dma->rxdma_addr = NULL;
	pxa_dma->dma_init = false;
	sport->dma_enable = 0;
	sport->cons_udelay = 1;
	sport->in_resume = false;

	ret = serial_pxa_probe_dt(dev, sport);
	if (ret > 0)
		sport->port.line = dev->id;
	else if (ret < 0)
		goto err_rst;

	if (sport->port.line >= ARRAY_SIZE(serial_pxa_ports)) {
		dev_err(&dev->dev, "serial%d out of range\n", sport->port.line);
		ret = -EINVAL;
		goto err_rst;
	}
	snprintf(sport->name, PXA_NAME_LEN - 1, "UART%d", sport->port.line + 1);

	dma_set_mask(&dev->dev, DMA_BIT_MASK(64));
	dma_set_coherent_mask(&dev->dev, DMA_BIT_MASK(64));
	if (ret > 0 && sport->dma_enable) {
		dmares = platform_get_resource(dev, IORESOURCE_DMA, 0);
		if (dmares) {
			dmares = platform_get_resource(dev, IORESOURCE_DMA, 1);
			if (dmares)
				sport->dma_enable = 1;
		}
	}

	ret = request_irq(sport->port.irq, serial_pxa_irq, 0, sport->name, sport);
	if (ret)
		goto err_rst;

	disable_irq(sport->port.irq);

#ifdef CONFIG_PM
#if SUPPORT_POWER_QOS
	pm_runtime_enable(&dev->dev);
	pm_runtime_set_active(&dev->dev);
	pm_runtime_irq_safe(&dev->dev);
#endif
#endif

	sport->port.membase = ioremap(mmres->start, resource_size(mmres));
	if (!sport->port.membase) {
		ret = -ENOMEM;
		goto err_qos;
	}

#ifdef CONFIG_PM
	INIT_WORK(&sport->uart_tx_lpm_work, uart_tx_lpm_handler);

	timer_setup(&sport->pxa_timer, pxa_timer_handler, 0);
#endif

	serial_pxa_ports[sport->port.line] = sport;
	uart_add_one_port(&serial_pxa_reg, &sport->port);
	dev_dbg(&dev->dev, "uart clk_rate: %lu\n", clk_get_rate(sport->fclk));
	platform_set_drvdata(dev, sport);

	return 0;

#ifdef CONFIG_PM
	uart_remove_one_port(&serial_pxa_reg, &sport->port);
	iounmap(sport->port.membase);
#endif
err_qos:
#ifdef CONFIG_PM
	pm_runtime_disable(&dev->dev);
#endif
	free_irq(sport->port.irq, sport);
err_rst:
	reset_control_assert(sport->resets);
err_clk:
	clk_unprepare(sport->fclk);
	clk_unprepare(sport->gclk);
	clk_put(sport->fclk);
	clk_put(sport->gclk);
err_free:
	kfree(sport);
	return ret;
}

static int serial_pxa_remove(struct platform_device *dev)
{
	struct uart_pxa_port *sport = platform_get_drvdata(dev);

#ifdef CONFIG_PM
	pm_runtime_disable(&dev->dev);
#endif

	uart_remove_one_port(&serial_pxa_reg, &sport->port);

	reset_control_assert(sport->resets);
	free_irq(sport->port.irq, sport);
	clk_unprepare(sport->fclk);
	clk_unprepare(sport->gclk);
	clk_put(sport->fclk);
	clk_put(sport->gclk);

#ifdef CONFIG_PM
	kfree(sport->uart_dma.tx_buf_save);
#endif
	kfree(sport);
	serial_pxa_ports[dev->id] = NULL;

	return 0;
}

static struct platform_driver serial_pxa_driver = {
	.probe = serial_pxa_probe,
	.remove = serial_pxa_remove,
	.driver = {
		.name = "spacemit-k1x-uart",
#ifdef CONFIG_PM
		.pm = &serial_pxa_pm_ops,
#endif
		.suppress_bind_attrs = true,
		.of_match_table = serial_k1x_dt_ids,
	},
};

static int __init serial_pxa_init(void)
{
	int ret;

	ret = uart_register_driver(&serial_pxa_reg);
	if (ret != 0)
		return ret;

	ret = platform_driver_register(&serial_pxa_driver);
	if (ret != 0)
		uart_unregister_driver(&serial_pxa_reg);

	return ret;
}

static void __exit serial_pxa_exit(void)
{
	platform_driver_unregister(&serial_pxa_driver);
	uart_unregister_driver(&serial_pxa_reg);
}
module_init(serial_pxa_init);
module_exit(serial_pxa_exit);

