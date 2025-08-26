// SPDX-License-Identifier: GPL-2.0
/*
 * spacemit k1 emac driver
 *
 * Copyright (c) 2023, spacemit Corporation.
 *
 */

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/crc32.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/dma-direct.h>
#include <linux/in.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/ip.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_irq.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/pm.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/udp.h>
#include <linux/workqueue.h>
#include <linux/reset.h>
#include <linux/pm_wakeirq.h>
#include "k1-emac.h"

#define DRIVER_NAME			"k1_emac"

#define TUNING_CMD_LEN			50
#define CLK_PHASE_CNT			256
#define CLK_PHASE_REVERT		180

#define TXCLK_PHASE_DEFAULT		0
#define RXCLK_PHASE_DEFAULT		0

#define TX_PHASE			1
#define RX_PHASE			0

#define DEFAULT_TX_THRESHOLD		(192)
#define DEFAULT_RX_THRESHOLD		(12)
#define DEFAULT_TX_RING_NUM		(128)
#define DEFAULT_RX_RING_NUM		(128)
#define DEFAULT_DMA_BURST_LEN		(1)
#define HASH_TABLE_SIZE			(64)

#define EMAC_DMA_REG_CNT		16
#define EMAC_MAC_REG_CNT		124
#define EMAC_REG_SPACE_SIZE		((EMAC_DMA_REG_CNT + EMAC_MAC_REG_CNT) * 4)

/* for ptp event message , udp port is 319 */
#define DEFAULT_UDP_PORT		(0x13F)

/* ptp ethernet type */
#define DEFAULT_ETH_TYPE		(0x88F7)

#define EMAC_SYSTIM_OVERFLOW_PERIOD	(HZ * 60 * 60 * 4)

#define INCVALUE_100MHZ			10
#define INCVALUE_SHIFT_100MHZ		17
#define INCPERIOD_100MHZ		1

#define EMAC_ETHTOOL_STAT(x) { #x, offsetof(struct emac_hw_stats, x) / sizeof(u32) }

/* strings used by ethtool */
static const struct emac_ethtool_stats {
	char str[ETH_GSTRING_LEN];
	u32 offset;
} emac_ethtool_stats[] = {
	EMAC_ETHTOOL_STAT(tx_ok_pkts),
	EMAC_ETHTOOL_STAT(tx_total_pkts),
	EMAC_ETHTOOL_STAT(tx_ok_bytes),
	EMAC_ETHTOOL_STAT(tx_err_pkts),
	EMAC_ETHTOOL_STAT(tx_singleclsn_pkts),
	EMAC_ETHTOOL_STAT(tx_multiclsn_pkts),
	EMAC_ETHTOOL_STAT(tx_lateclsn_pkts),
	EMAC_ETHTOOL_STAT(tx_excessclsn_pkts),
	EMAC_ETHTOOL_STAT(tx_unicast_pkts),
	EMAC_ETHTOOL_STAT(tx_multicast_pkts),
	EMAC_ETHTOOL_STAT(tx_broadcast_pkts),
	EMAC_ETHTOOL_STAT(tx_pause_pkts),
	EMAC_ETHTOOL_STAT(rx_ok_pkts),
	EMAC_ETHTOOL_STAT(rx_total_pkts),
	EMAC_ETHTOOL_STAT(rx_crc_err_pkts),
	EMAC_ETHTOOL_STAT(rx_align_err_pkts),
	EMAC_ETHTOOL_STAT(rx_err_total_pkts),
	EMAC_ETHTOOL_STAT(rx_ok_bytes),
	EMAC_ETHTOOL_STAT(rx_total_bytes),
	EMAC_ETHTOOL_STAT(rx_unicast_pkts),
	EMAC_ETHTOOL_STAT(rx_multicast_pkts),
	EMAC_ETHTOOL_STAT(rx_broadcast_pkts),
	EMAC_ETHTOOL_STAT(rx_pause_pkts),
	EMAC_ETHTOOL_STAT(rx_len_err_pkts),
	EMAC_ETHTOOL_STAT(rx_len_undersize_pkts),
	EMAC_ETHTOOL_STAT(rx_len_oversize_pkts),
	EMAC_ETHTOOL_STAT(rx_len_fragment_pkts),
	EMAC_ETHTOOL_STAT(rx_len_jabber_pkts),
	EMAC_ETHTOOL_STAT(rx_64_pkts),
	EMAC_ETHTOOL_STAT(rx_65_127_pkts),
	EMAC_ETHTOOL_STAT(rx_128_255_pkts),
	EMAC_ETHTOOL_STAT(rx_256_511_pkts),
	EMAC_ETHTOOL_STAT(rx_512_1023_pkts),
	EMAC_ETHTOOL_STAT(rx_1024_1518_pkts),
	EMAC_ETHTOOL_STAT(rx_1519_plus_pkts),
	EMAC_ETHTOOL_STAT(rx_drp_fifo_full_pkts),
	EMAC_ETHTOOL_STAT(rx_truncate_fifo_full_pkts),
};

enum clk_tuning_way {
	/* fpga clk tuning register */
	CLK_TUNING_BY_REG,
	/* zebu/evb rgmii delayline register */
	CLK_TUNING_BY_DLINE,
	/* evb rmii only revert tx/rx clock for clk tuning */
	CLK_TUNING_BY_CLK_REVERT,
	CLK_TUNING_MAX,
};

static void emac_hw_timestamp_config(struct emac_priv *priv, u32 enable, u8 rx_ptp_type, u32 ptp_msg_id)
{
	void __iomem *ioaddr = priv->iobase;
	u32 val;

	if (enable) {
		/* enable tx/rx timestamp and config rx ptp type */
		val = TX_TIMESTAMP_EN | RX_TIMESTAMP_EN;
		val |= (rx_ptp_type << RX_PTP_PKT_TYPE_OFST) & RX_PTP_PKT_TYPE_MSK;
		writel(val, ioaddr + PTP_1588_CTRL);

		/* config ptp message id */
		writel(ptp_msg_id, ioaddr + PTP_MSG_ID);

		/* config ptp ethernet type */
		writel(DEFAULT_ETH_TYPE, ioaddr + PTP_ETH_TYPE);

		/* config ptp udp port */
		writel(DEFAULT_UDP_PORT, ioaddr + PTP_UDP_PORT);

	} else {
		writel(0, ioaddr + PTP_1588_CTRL);
	}
}

static u32 emac_hw_config_systime_increment(struct emac_priv *priv, u32 ptp_clock, u32 adj_clock)
{
	void __iomem *ioaddr = priv->iobase;
	u32 incr_val;
	u32 incr_period;
	u32 val;
	u32 period = 0, def_period = 0;

	/* set system time counter resolution as ns
	 * if ptp clock is 50Mhz,  20ns per clock cycle,
	 * so increment value should be 20,
	 * increment period should be 1m
	 */
	if (ptp_clock == adj_clock) {
		incr_val = INCVALUE_100MHZ << INCVALUE_SHIFT_100MHZ;
		incr_period = INCPERIOD_100MHZ;
	} else {
		def_period = div_u64(1000000000ULL, ptp_clock);
		period = div_u64(1000000000ULL, adj_clock);
		if (def_period == period)
			return 0;

		incr_period = 1;
		incr_val = (def_period * def_period) / period;
	}

	val = (incr_val | (incr_period << INCR_PERIOD_OFST));
	writel(val, ioaddr + PTP_INRC_ATTR);

	return 0;
}

static u64 emac_hw_get_systime(struct emac_priv *priv)
{
	void __iomem *ioaddr = priv->iobase;
	u64 systimel, systimeh;
	u64 systim;

	/* update system time adjust low register */
	systimel = readl(ioaddr + SYS_TIME_GET_LOW);
	systimeh = readl(ioaddr + SYS_TIME_GET_HI);
	/* perform system time adjust */
	systim = (systimeh << 32) | systimel;

	return systim;
}

static u64 emac_hw_get_phc_time(struct emac_priv *priv)
{
	unsigned long flags;
	u64 cycles, ns;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	/* first read system time low register */
	cycles = emac_hw_get_systime(priv);
	ns = timecounter_cyc2time(&priv->tc, cycles);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return ns;
}

static u64 emac_hw_get_tx_timestamp(struct emac_priv *priv)
{
	void __iomem *ioaddr = priv->iobase;
	unsigned long flags;
	u64 systimel, systimeh;
	u64 systim;
	u64 ns;

	/* first read system time low register */
	systimel = readl(ioaddr + TX_TIMESTAMP_LOW);
	systimeh = readl(ioaddr + TX_TIMESTAMP_HI);
	systim = (systimeh << 32) | systimel;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	ns = timecounter_cyc2time(&priv->tc, systim);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return ns;
}

static u64 emac_hw_get_rx_timestamp(struct emac_priv *priv)
{
	void __iomem *ioaddr = priv->iobase;
	unsigned long flags;
	u64 systimel, systimeh;
	u64 systim;
	u64 ns;

	/* first read system time low register */
	systimel = readl(ioaddr + RX_TIMESTAMP_LOW);
	systimeh = readl(ioaddr + RX_TIMESTAMP_HI);
	systim = (systimeh << 32) | systimel;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	ns = timecounter_cyc2time(&priv->tc, systim);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return ns;
}

static u64 emac_cyclecounter_read(const struct cyclecounter *cc)
{
	struct emac_priv *priv = container_of(cc, struct emac_priv, cc);

	return emac_hw_get_systime(priv);
}

static int emac_hw_init_systime(struct emac_priv *priv, u64 set_ns)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	timecounter_init(&priv->tc, &priv->cc, set_ns);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static struct emac_hw_ptp emac_hwptp = {
	.config_hw_tstamping = emac_hw_timestamp_config,
	.config_systime_increment = emac_hw_config_systime_increment,
	.init_systime = emac_hw_init_systime,
	.get_phc_time = emac_hw_get_phc_time,
	.get_tx_timestamp = emac_hw_get_tx_timestamp,
	.get_rx_timestamp = emac_hw_get_rx_timestamp,
};

static int emac_adjust_freq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct emac_priv *priv = container_of(ptp, struct emac_priv, ptp_clock_ops);
	void __iomem *ioaddr = priv->iobase;
	unsigned long flags;
	u32 addend, incvalue;
	int neg_adj = 0;
	u64 adj;

	if (ppb > ptp->max_adj || (ppb <= -1000000000))
		return -EINVAL;
	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}

	spin_lock_irqsave(&priv->ptp_lock, flags);

	/* ppb = (Fnew - F0)/F0
	 * diff = F0 * ppb
	 */
	incvalue = INCVALUE_100MHZ << INCVALUE_SHIFT_100MHZ;
	adj = incvalue;
	adj *= ppb;
	adj = div_u64(adj, 1000000000);
	addend = neg_adj ? (incvalue - adj) : (incvalue + adj);
	addend = (addend | (INCPERIOD_100MHZ << INCR_PERIOD_OFST));
	writel(addend, ioaddr + PTP_INRC_ATTR);

	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static int emac_adjust_fine(struct ptp_clock_info *ptp, long scaled_ppm)
{
	s32 ppb;

	ppb = scaled_ppm_to_ppb(scaled_ppm);
	return emac_adjust_freq(ptp, ppb);
}

static int emac_adjust_time(struct ptp_clock_info *ptp, s64 delta)
{
	struct emac_priv *priv = container_of(ptp, struct emac_priv, ptp_clock_ops);
	unsigned long flags;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	timecounter_adjtime(&priv->tc, delta);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static int emac_phc_get_time(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	struct emac_priv *priv = container_of(ptp, struct emac_priv, ptp_clock_ops);
	unsigned long flags;
	u64 cycles, ns;

	spin_lock_irqsave(&priv->ptp_lock, flags);
	cycles = emac_hw_get_systime(priv);
	ns = timecounter_cyc2time(&priv->tc, cycles);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);
	*ts = ns_to_timespec64(ns);

	return 0;
}

static int emac_phc_set_time(struct ptp_clock_info *ptp, const struct timespec64 *ts)
{
	struct emac_priv *priv = container_of(ptp, struct emac_priv, ptp_clock_ops);
	unsigned long flags;
	u64 ns;

	ns = timespec64_to_ns(ts);
	spin_lock_irqsave(&priv->ptp_lock, flags);
	timecounter_init(&priv->tc, &priv->cc, ns);
	spin_unlock_irqrestore(&priv->ptp_lock, flags);

	return 0;
}

static void emac_systim_overflow_work(struct work_struct *work)
{
	struct emac_priv *priv = container_of(work, struct emac_priv, systim_overflow_work.work);
	struct timespec64 ts;
	u64 ns;

	ns = timecounter_read(&priv->tc);
	ts = ns_to_timespec64(ns);
	pr_debug("SYSTIM overflow check at %lld.%09lu\n",
		 (long long)ts.tv_sec, ts.tv_nsec);
	schedule_delayed_work(&priv->systim_overflow_work, EMAC_SYSTIM_OVERFLOW_PERIOD);
}

static struct ptp_clock_info emac_ptp_clock_ops = {
	.owner = THIS_MODULE,
	.name = "emac_ptp_clock",
	.max_adj = 1000000000,
	.n_alarm = 0,
	.n_ext_ts = 0,
	.n_per_out = 0,
	.n_pins = 0,
	.pps = 0,
	.adjfine = emac_adjust_fine,
	.adjtime = emac_adjust_time,
	.gettime64 = emac_phc_get_time,
	.settime64 = emac_phc_set_time,
};

static void emac_ptp_register(struct emac_priv *priv)
{
	unsigned long flags;

	priv->cc.read = emac_cyclecounter_read;
	priv->cc.mask = CYCLECOUNTER_MASK(64);
	priv->cc.mult = 1;
	priv->cc.shift = INCVALUE_SHIFT_100MHZ;
	spin_lock_init(&priv->ptp_lock);
	priv->ptp_clock_ops = emac_ptp_clock_ops;

	INIT_DELAYED_WORK(&priv->systim_overflow_work, emac_systim_overflow_work);
	schedule_delayed_work(&priv->systim_overflow_work, EMAC_SYSTIM_OVERFLOW_PERIOD);
	priv->ptp_clock = ptp_clock_register(&priv->ptp_clock_ops, NULL);
	if (IS_ERR(priv->ptp_clock)) {
		netdev_err(priv->ndev, "ptp_clock_register failed\n");
		priv->ptp_clock = NULL;
	} else if (priv->ptp_clock) {
		netdev_info(priv->ndev, "registered PTP clock\n");
	} else {
		netdev_info(priv->ndev, "PTP_1588_CLOCK maybe not enabled\n");
	}

	spin_lock_irqsave(&priv->ptp_lock, flags);
	timecounter_init(&priv->tc, &priv->cc, ktime_to_ns(ktime_get_real()));
	spin_unlock_irqrestore(&priv->ptp_lock, flags);
	priv->hwptp = &emac_hwptp;
}

static void emac_ptp_unregister(struct emac_priv *priv)
{
	cancel_delayed_work_sync(&priv->systim_overflow_work);
	if (priv->ptp_clock) {
		ptp_clock_unregister(priv->ptp_clock);
		priv->ptp_clock = NULL;
		pr_debug("Removed PTP HW clock successfully on %s\n", priv->ndev->name);
	}
	priv->hwptp = NULL;
}

static bool emac_is_rmii(struct emac_priv *priv)
{
	return priv->phy_interface == PHY_INTERFACE_MODE_RMII;
}

static void emac_enable_axi_single_id_mode(struct emac_priv *priv, int en)
{
	u32 val;

	val = readl(priv->ctrl_reg);
	if (en)
		val |= AXI_SINGLE_ID;
	else
		val &= ~AXI_SINGLE_ID;
	writel(val, priv->ctrl_reg);
}

static void emac_phy_interface_config(struct emac_priv *priv)
{
	u32 val;

	val = readl(priv->ctrl_reg);
	if (emac_is_rmii(priv)) {
		val &= ~PHY_INTF_RGMII;
		if (priv->ref_clk_frm_soc)
			val |= REF_CLK_SEL;
		else
			val &= ~REF_CLK_SEL;
	} else {
		val |= PHY_INTF_RGMII;
		if (priv->ref_clk_frm_soc)
			val |= RGMII_TX_CLK_SEL;
	}
	writel(val, priv->ctrl_reg);
}

static int emac_reset_hw(struct emac_priv *priv)
{
	/* disable all the interrupts */
	emac_wr(priv, MAC_INTERRUPT_ENABLE, 0x0000);
	emac_wr(priv, DMA_INTERRUPT_ENABLE, 0x0000);

	/* disable transmit and receive units */
	emac_wr(priv, MAC_RECEIVE_CONTROL, 0x0000);
	emac_wr(priv, MAC_TRANSMIT_CONTROL, 0x0000);

	/* stop the DMA */
	emac_wr(priv, DMA_CONTROL, 0x0000);

	/* reset mac, statistic counters */
	emac_wr(priv, MAC_GLOBAL_CONTROL, 0x0018);
	emac_wr(priv, MAC_GLOBAL_CONTROL, 0x0000);

	return 0;
}

static int emac_init_hw(struct emac_priv *priv)
{
	u32 val = 0;

	emac_enable_axi_single_id_mode(priv, 1);

	/* disable transmit and receive units */
	emac_wr(priv, MAC_RECEIVE_CONTROL, 0x0000);
	emac_wr(priv, MAC_TRANSMIT_CONTROL, 0x0000);

	/* enable mac address 1 filtering */
	emac_wr(priv, MAC_ADDRESS_CONTROL, MREGBIT_MAC_ADDRESS1_ENABLE);

	/* zero initialize the multicast hash table */
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE1, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE2, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE3, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE4, 0x0);
	emac_wr(priv, MAC_TRANSMIT_FIFO_ALMOST_FULL, 0x1f8);
	emac_wr(priv, MAC_TRANSMIT_PACKET_START_THRESHOLD, priv->tx_threshold);
	emac_wr(priv, MAC_RECEIVE_PACKET_START_THRESHOLD, priv->rx_threshold);

	/* set emac rx mitigation frame count */
	val = priv->rx_coal_frames & MREGBIT_RECEIVE_IRQ_FRAME_COUNTER_MSK;

	/* set emac rx mitigation timeout */
	val |= ((priv->rx_coal_timeout * AXI_CLK_CYCLES_PER_US) <<
		 MREGBIT_RECEIVE_IRQ_TIMEOUT_COUNTER_OFST) &
		 MREGBIT_RECEIVE_IRQ_TIMEOUT_COUNTER_MSK;

	/* enable emac rx irq mitigation */
	val |= MRGEBIT_RECEIVE_IRQ_MITIGATION_ENABLE;

	emac_wr(priv, DMA_RECEIVE_IRQ_MITIGATION_CTRL, val);
	emac_wr(priv, MAC_FC_CONTROL, MREGBIT_FC_DECODE_ENABLE);

	/* reset dma */
	emac_wr(priv, DMA_CONTROL, 0x0000);
	emac_wr(priv, DMA_CONFIGURATION, 0x01);
	usleep_range(9000, 10000);
	emac_wr(priv, DMA_CONFIGURATION, 0x00);
	usleep_range(9000, 10000);

	val = MREGBIT_STRICT_BURST | MREGBIT_DMA_64BIT_MODE;
	if (priv->dma_burst_len)
		val |= 1 << priv->dma_burst_len;
	else
		val |= MREGBIT_BURST_1WORD;
	emac_wr(priv, DMA_CONFIGURATION, val);

	/* if emac has ptp 1588 support, so enable PTP 1588 irq */
	if (priv->ptp_support)
		emac_wr(priv, PTP_1588_IRQ_EN, PTP_TX_TIMESTAMP | PTP_RX_TIMESTAMP);

	return 0;
}

static int emac_set_mac_addr(struct emac_priv *priv, const unsigned char *addr)
{
	emac_wr(priv, MAC_ADDRESS1_HIGH, ((addr[1] << 8) | addr[0]));
	emac_wr(priv, MAC_ADDRESS1_MED, ((addr[3] << 8) | addr[2]));
	emac_wr(priv, MAC_ADDRESS1_LOW, ((addr[5] << 8) | addr[4]));

	return 0;
}

static void emac_dma_start_transmit(struct emac_priv *priv)
{
	emac_wr(priv, DMA_TRANSMIT_POLL_DEMAND, 0xFF);
}

static void emac_enable_interrupt(struct emac_priv *priv)
{
	u32 val;

	val = emac_rd(priv, DMA_INTERRUPT_ENABLE);
	val |= MREGBIT_TRANSMIT_TRANSFER_DONE_INTR_ENABLE;
	val |= MREGBIT_RECEIVE_TRANSFER_DONE_INTR_ENABLE;
	emac_wr(priv, DMA_INTERRUPT_ENABLE, val);
}

static void emac_disable_interrupt(struct emac_priv *priv)
{
	u32 val;

	val = emac_rd(priv, DMA_INTERRUPT_ENABLE);
	val &= ~MREGBIT_TRANSMIT_TRANSFER_DONE_INTR_ENABLE;
	val &= ~MREGBIT_RECEIVE_TRANSFER_DONE_INTR_ENABLE;
	emac_wr(priv, DMA_INTERRUPT_ENABLE, val);
}

static inline u32 emac_tx_avail(struct emac_priv *priv)
{
	struct emac_desc_ring *tx_ring = &priv->tx_ring;
	u32 avail;

	if (tx_ring->tail > tx_ring->head)
		avail = tx_ring->tail - tx_ring->head - 1;
	else
		avail = tx_ring->total_cnt - tx_ring->head + tx_ring->tail - 1;

	return avail;
}

static void emac_tx_coal_timer_resched(struct emac_priv *priv)
{
	mod_timer(&priv->txtimer, jiffies + usecs_to_jiffies(priv->tx_coal_timeout));
}

static void emac_tx_coal_timer(struct timer_list *t)
{
	struct emac_priv *priv = from_timer(priv, t, txtimer);

	if (likely(napi_schedule_prep(&priv->napi)))
		__napi_schedule(&priv->napi);
}

static int emac_tx_coal(struct emac_priv *priv, u32 pkt_num)
{
	/* Manage tx mitigation */
	priv->tx_count_frames += pkt_num;
	if (likely(priv->tx_coal_frames > priv->tx_count_frames)) {
		emac_tx_coal_timer_resched(priv);
		return false;
	}

	priv->tx_count_frames = 0;
	return true;
}

static void emac_get_tx_hwtstamp(struct emac_priv *priv, struct sk_buff *skb)
{
	struct skb_shared_hwtstamps shhwtstamp;
	u64 ns;

	if (!priv->hwts_tx_en)
		return;

	/* exit if skb doesn't support hw tstamp */
	if (likely(!skb || !(skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS)))
		return;

	/* get the valid tstamp */
	ns = priv->hwptp->get_tx_timestamp(priv);
	memset(&shhwtstamp, 0, sizeof(struct skb_shared_hwtstamps));
	shhwtstamp.hwtstamp = ns_to_ktime(ns);
	/* pass tstamp to stack */
	skb_tstamp_tx(skb, &shhwtstamp);
}

static void emac_get_rx_hwtstamp(struct emac_priv *priv, struct emac_rx_desc *p,
				 struct sk_buff *skb)
{
	struct skb_shared_hwtstamps *shhwtstamp = NULL;
	u64 ns;

	if (!priv->hwts_rx_en)
		return;

	/* Check if timestamp is available */
	if (p->ptp_pkt && p->rx_timestamp) {
		ns = priv->hwptp->get_rx_timestamp(priv);
		netdev_dbg(priv->ndev, "get valid RX hw timestamp %llu\n", ns);
		shhwtstamp = skb_hwtstamps(skb);
		memset(shhwtstamp, 0, sizeof(struct skb_shared_hwtstamps));
		shhwtstamp->hwtstamp = ns_to_ktime(ns);
	} else  {
		netdev_dbg(priv->ndev, "cannot get RX hw timestamp\n");
	}
}

static int emac_hwtstamp_ioctl(struct net_device *dev, struct ifreq *ifr)
{
	struct emac_priv *priv = netdev_priv(dev);
	struct hwtstamp_config config;
	struct timespec64 now;
	u64 ns_ptp;
	u32 ptp_event_msg_id = 0;
	u32 rx_ptp_type = 0;

	if (!priv->ptp_support) {
		netdev_alert(priv->ndev, "No support for HW time stamping\n");
		priv->hwts_tx_en = 0;
		priv->hwts_rx_en = 0;

		return -EOPNOTSUPP;
	}

	if (copy_from_user(&config, ifr->ifr_data, sizeof(struct hwtstamp_config)))
		return -EFAULT;

	netdev_dbg(priv->ndev, "%s config flags:0x%x, tx_type:0x%x, rx_filter:0x%x\n",
		   __func__, config.flags, config.tx_type, config.rx_filter);

	/* reserved for future extensions */
	if (config.flags)
		return -EINVAL;

	if (config.tx_type != HWTSTAMP_TX_OFF && config.tx_type != HWTSTAMP_TX_ON)
		return -ERANGE;

	switch (config.rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		/* time stamp no incoming packet at all */
		config.rx_filter = HWTSTAMP_FILTER_NONE;
		break;

	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		/* PTP v1, UDP, Sync packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V1_L4_SYNC;
		/* take time stamp for SYNC messages only */
		ptp_event_msg_id = MSG_SYNC;
		rx_ptp_type = PTP_V1_L4_ONLY;
		break;

	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		/* PTP v1, UDP, Delay_req packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ;
		/* take time stamp for Delay_Req messages only */
		ptp_event_msg_id = MSG_DELAY_REQ;
		rx_ptp_type = PTP_V1_L4_ONLY;
		break;

	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		/* PTP v2, UDP, Sync packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V2_L4_SYNC;
		/* take time stamp for SYNC messages only */
		ptp_event_msg_id = MSG_SYNC;
		rx_ptp_type = PTP_V2_L2_L4;
		break;

	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		/* PTP v2, UDP, Delay_req packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ;
		/* take time stamp for Delay_Req messages only */
		ptp_event_msg_id = MSG_DELAY_REQ;
		rx_ptp_type = PTP_V2_L2_L4;
		break;

	case HWTSTAMP_FILTER_PTP_V2_EVENT:
		/* PTP v2/802.AS1 any layer, any kind of event packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V2_EVENT;
		ptp_event_msg_id = ALL_EVENTS;
		rx_ptp_type = PTP_V2_L2_L4;
		break;

	case HWTSTAMP_FILTER_PTP_V2_SYNC:
		/* PTP v2/802.AS1, any layer, Sync packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V2_SYNC;
		/* take time stamp for SYNC messages only */
		ptp_event_msg_id = MSG_SYNC;
		rx_ptp_type = PTP_V2_L2_L4;
		break;

	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		/* PTP v2/802.AS1, any layer, Delay_req packet */
		config.rx_filter = HWTSTAMP_FILTER_PTP_V2_DELAY_REQ;
		/* take time stamp for Delay_Req messages only */
		ptp_event_msg_id = MSG_DELAY_REQ;
		rx_ptp_type = PTP_V2_L2_L4;
		break;

	default:
		return -ERANGE;
	}

	priv->hwts_rx_en = ((config.rx_filter == HWTSTAMP_FILTER_NONE) ? 0 : 1);
	priv->hwts_tx_en = config.tx_type == HWTSTAMP_TX_ON;

	if (!priv->hwts_tx_en && !priv->hwts_rx_en) {
		priv->hwptp->config_hw_tstamping(priv, 0, 0, 0);
	} else {
		priv->hwptp->config_hw_tstamping(priv, 1, rx_ptp_type, ptp_event_msg_id);

		/* initialize system time */
		ktime_get_real_ts64(&now);
		priv->hwptp->init_systime(priv, timespec64_to_ns(&now));

		/* program Increment reg */
		priv->hwptp->config_systime_increment(priv, priv->ptp_clk_rate, priv->ptp_clk_rate);

		ns_ptp = priv->hwptp->get_phc_time(priv);
		ktime_get_real_ts64(&now);
		/* check the diff between ptp timer and system time */
		if (abs(timespec64_to_ns(&now) - ns_ptp) > 5000)
			priv->hwptp->init_systime(priv, timespec64_to_ns(&now));
	}
	return copy_to_user(ifr->ifr_data, &config, sizeof(struct hwtstamp_config)) ? -EFAULT : 0;
}

static irqreturn_t emac_interrupt_handler(int irq, void *dev_id)
{
	struct net_device *ndev = (struct net_device *)dev_id;
	struct emac_priv *priv = netdev_priv(ndev);
	u32 status;
	u32 clr = 0;

	/* Check if emac is up */
	if (test_bit(EMAC_DOWN, &priv->state))
		return IRQ_HANDLED;

	/* read the status register for IRQ received */
	status = emac_rd(priv, DMA_STATUS_IRQ);

	if (status & MREGBIT_TRANSMIT_TRANSFER_DONE_IRQ) {
		emac_disable_interrupt(priv);
		clr |= MREGBIT_TRANSMIT_TRANSFER_DONE_IRQ;
		napi_schedule(&priv->napi);
	}

	if (status & MREGBIT_TRANSMIT_DES_UNAVAILABLE_IRQ)
		clr |= MREGBIT_TRANSMIT_DES_UNAVAILABLE_IRQ;

	if (status & MREGBIT_TRANSMIT_DMA_STOPPED_IRQ)
		clr |= MREGBIT_TRANSMIT_DMA_STOPPED_IRQ;

	if (status & MREGBIT_RECEIVE_TRANSFER_DONE_IRQ) {
		emac_disable_interrupt(priv);
		clr |= MREGBIT_RECEIVE_TRANSFER_DONE_IRQ;
		napi_schedule(&priv->napi);
	}

	if (status & MREGBIT_RECEIVE_DES_UNAVAILABLE_IRQ)
		clr |= MREGBIT_RECEIVE_DES_UNAVAILABLE_IRQ;

	if (status & MREGBIT_RECEIVE_DMA_STOPPED_IRQ)
		clr |= MREGBIT_RECEIVE_DMA_STOPPED_IRQ;

	if (status & MREGBIT_RECEIVE_MISSED_FRAME_IRQ)
		clr |= MREGBIT_RECEIVE_MISSED_FRAME_IRQ;

	emac_wr(priv, DMA_STATUS_IRQ, clr);

	if (priv->ptp_support) {
		status = emac_rd(priv, PTP_1588_IRQ_STS);
		if ((status & PTP_TX_TIMESTAMP) || (status & PTP_RX_TIMESTAMP))
			napi_schedule(&priv->napi);

		emac_wr(priv, PTP_1588_IRQ_STS, status);
	}

	return IRQ_HANDLED;
}

static void emac_configure_tx(struct emac_priv *priv)
{
	u32 val;

	/* set the transmit base address */
	val = (u32)(priv->tx_ring.desc_dma_addr);
	emac_wr(priv, DMA_TRANSMIT_BASE_ADDRESS, val);

	/* Tx Inter Packet Gap value and enable the transmit */
	val = emac_rd(priv, MAC_TRANSMIT_CONTROL);
	val &= (~MREGBIT_IFG_LEN);
	val |= MREGBIT_TRANSMIT_ENABLE;
	val |= MREGBIT_TRANSMIT_AUTO_RETRY;
	emac_wr(priv, MAC_TRANSMIT_CONTROL, val);
	emac_wr(priv, DMA_TRANSMIT_AUTO_POLL_COUNTER, 0x00);

	/* start tx dma */
	val = emac_rd(priv, DMA_CONTROL);
	val |= MREGBIT_START_STOP_TRANSMIT_DMA;
	emac_wr(priv, DMA_CONTROL, val);
}

static void emac_configure_rx(struct emac_priv *priv)
{
	u32 val;

	/* set the receive base address */
	val = (u32)(priv->rx_ring.desc_dma_addr);
	emac_wr(priv, DMA_RECEIVE_BASE_ADDRESS, val);

	/* enable the receive */
	val = emac_rd(priv, MAC_RECEIVE_CONTROL);
	val |= MREGBIT_RECEIVE_ENABLE;
	val |= MREGBIT_STORE_FORWARD;
	emac_wr(priv, MAC_RECEIVE_CONTROL, val);

	/* start rx dma */
	val = emac_rd(priv, DMA_CONTROL);
	val |= MREGBIT_START_STOP_RECEIVE_DMA;
	emac_wr(priv, DMA_CONTROL, val);
}

static int emac_free_tx_buf(struct emac_priv *priv, int i)
{
	struct emac_desc_ring *tx_ring;
	struct emac_tx_desc_buffer *tx_buf;
	struct desc_buf *buf;
	int j;

	tx_ring = &priv->tx_ring;
	tx_buf = &tx_ring->tx_desc_buf[i];

	for (j = 0; j < 2; j++) {
		buf = &tx_buf->buf[j];
		if (buf->dma_addr) {
			if (buf->map_as_page)
				dma_unmap_page(&priv->pdev->dev, buf->dma_addr,
					       buf->dma_len, DMA_TO_DEVICE);
			else
				dma_unmap_single(&priv->pdev->dev, buf->dma_addr,
						 buf->dma_len, DMA_TO_DEVICE);

			buf->dma_addr = 0;
			buf->map_as_page = false;
			buf->buff_addr = NULL;
		}
	}

	if (tx_buf->skb) {
		dev_kfree_skb_any(tx_buf->skb);
		tx_buf->skb = NULL;
	}

	return 0;
}

static void emac_clean_tx_desc_ring(struct emac_priv *priv)
{
	struct emac_desc_ring *tx_ring = &priv->tx_ring;
	u32 i;

	/* Free all the Tx ring sk_buffs */
	for (i = 0; i < tx_ring->total_cnt; i++)
		emac_free_tx_buf(priv, i);

	tx_ring->head = 0;
	tx_ring->tail = 0;
}

static void emac_clean_rx_desc_ring(struct emac_priv *priv)
{
	struct emac_desc_ring *rx_ring;
	struct emac_desc_buffer *rx_buf;
	u32 i;

	rx_ring = &priv->rx_ring;

	/* Free all the Rx ring sk_buffs */
	for (i = 0; i < rx_ring->total_cnt; i++) {
		rx_buf = &rx_ring->desc_buf[i];
		if (rx_buf->skb) {
			dma_unmap_single(&priv->pdev->dev, rx_buf->dma_addr,
					 rx_buf->dma_len, DMA_FROM_DEVICE);

			dev_kfree_skb(rx_buf->skb);
			rx_buf->skb = NULL;
		}
	}

	rx_ring->tail = 0;
	rx_ring->head = 0;
}

static void emac_ptp_init(struct emac_priv *priv)
{
	int ret;

	if (priv->ptp_support) {
		ret = clk_prepare_enable(priv->ptp_clk);
		if (ret < 0) {
			pr_warn("ptp clock failed to enable\n");
			priv->ptp_clk = NULL;
		}
		emac_ptp_register(priv);
	}
}

static void emac_ptp_deinit(struct emac_priv *priv)
{
	if (priv->ptp_support) {
		if (priv->ptp_clk)
			clk_disable_unprepare(priv->ptp_clk);
		emac_ptp_unregister(priv);
	}
}

static int emac_down(struct emac_priv *priv)
{
	struct net_device *ndev = priv->ndev;

	netif_stop_queue(ndev);
	/* Stop and disconnect the PHY */
	if (ndev->phydev) {
		phy_stop(ndev->phydev);
		phy_disconnect(ndev->phydev);
	}

	priv->link = false;
	priv->duplex = DUPLEX_UNKNOWN;
	priv->speed = SPEED_UNKNOWN;

	napi_disable(&priv->napi);
	emac_wr(priv, MAC_INTERRUPT_ENABLE, 0x0000);
	emac_wr(priv, DMA_INTERRUPT_ENABLE, 0x0000);
	free_irq(priv->irq, ndev);
	emac_ptp_deinit(priv);
	emac_reset_hw(priv);
	netif_carrier_off(ndev);

	return 0;
}

static int emac_alloc_tx_resources(struct emac_priv *priv)
{
	struct emac_desc_ring *tx_ring = &priv->tx_ring;
	struct platform_device *pdev  = priv->pdev;
	u32 size;

	size = sizeof(struct emac_tx_desc_buffer) * tx_ring->total_cnt;

	/* allocate memory */
	tx_ring->tx_desc_buf = kzalloc(size, GFP_KERNEL);
	if (!tx_ring->tx_desc_buf)
		return -ENOMEM;

	memset(tx_ring->tx_desc_buf, 0, size);
	tx_ring->total_size = tx_ring->total_cnt * sizeof(struct emac_tx_desc);
	EMAC_ROUNDUP(tx_ring->total_size, 1024);
	tx_ring->desc_addr = dma_alloc_coherent(&pdev->dev, tx_ring->total_size,
						&tx_ring->desc_dma_addr, GFP_KERNEL);
	if (!tx_ring->desc_addr) {
		kfree(tx_ring->tx_desc_buf);
		return -ENOMEM;
	}

	memset(tx_ring->desc_addr, 0, tx_ring->total_size);

	tx_ring->head = 0;
	tx_ring->tail = 0;

	return 0;
}

static int emac_alloc_rx_resources(struct emac_priv *priv)
{
	struct emac_desc_ring *rx_ring = &priv->rx_ring;
	struct platform_device *pdev  = priv->pdev;
	u32 buf_len;

	buf_len = sizeof(struct emac_desc_buffer) * rx_ring->total_cnt;

	rx_ring->desc_buf = kzalloc(buf_len, GFP_KERNEL);
	if (!rx_ring->desc_buf)
		return -ENOMEM;

	memset(rx_ring->desc_buf, 0, buf_len);
	/* round up to nearest 4K */
	rx_ring->total_size = rx_ring->total_cnt * sizeof(struct emac_rx_desc);
	EMAC_ROUNDUP(rx_ring->total_size, 1024);
	rx_ring->desc_addr = dma_alloc_coherent(&pdev->dev, rx_ring->total_size,
						&rx_ring->desc_dma_addr, GFP_KERNEL);
	if (!rx_ring->desc_addr) {
		kfree(rx_ring->desc_buf);
		return -ENOMEM;
	}

	memset(rx_ring->desc_addr, 0, rx_ring->total_size);

	rx_ring->head = 0;
	rx_ring->tail = 0;

	return 0;
}

static void emac_free_tx_resources(struct emac_priv *priv)
{
	emac_clean_tx_desc_ring(priv);
	kfree(priv->tx_ring.tx_desc_buf);
	priv->tx_ring.tx_desc_buf = NULL;
	dma_free_coherent(&priv->pdev->dev, priv->tx_ring.total_size,
			  priv->tx_ring.desc_addr, priv->tx_ring.desc_dma_addr);
	priv->tx_ring.desc_addr = NULL;
}

static void emac_free_rx_resources(struct emac_priv *priv)
{
	emac_clean_rx_desc_ring(priv);
	kfree(priv->rx_ring.desc_buf);
	priv->rx_ring.desc_buf = NULL;
	dma_free_coherent(&priv->pdev->dev, priv->rx_ring.total_size,
			  priv->rx_ring.desc_addr, priv->rx_ring.desc_dma_addr);
	priv->rx_ring.desc_addr = NULL;
}

static int emac_tx_clean_desc(struct emac_priv *priv)
{
	struct emac_desc_ring *tx_ring;
	struct emac_tx_desc_buffer *tx_buf;
	struct emac_tx_desc *tx_desc;
	struct net_device *ndev = priv->ndev;
	u32 i;

	netif_tx_lock(ndev);

	tx_ring = &priv->tx_ring;

	i = tx_ring->tail;

	while (i != tx_ring->head) {
		tx_desc = &((struct emac_tx_desc *)tx_ring->desc_addr)[i];

		/* if desc still own by dma, so we quit it */
		if (tx_desc->own)
			break;

		tx_buf = &tx_ring->tx_desc_buf[i];

		if (tx_buf->timestamped && tx_buf->skb) {
			emac_get_tx_hwtstamp(priv, tx_buf->skb);
			tx_buf->timestamped = 0;
		}

		emac_free_tx_buf(priv, i);
		memset(tx_desc, 0, sizeof(struct emac_tx_desc));

		if (++i == tx_ring->total_cnt)
			i = 0;
	}

	tx_ring->tail = i;

	if (unlikely(netif_queue_stopped(ndev) && emac_tx_avail(priv) > tx_ring->total_cnt / 4))
		netif_wake_queue(ndev);

	netif_tx_unlock(ndev);

	return 0;
}

static int emac_rx_frame_status(struct emac_priv *priv, struct emac_rx_desc *dsc)
{
	/* if last descritpor isn't set, so we drop it*/
	if (!dsc->last_descriptor) {
		netdev_dbg(priv->ndev, "rx LD bit isn't set, drop it.\n");
		return FRAME_DISCARD;
	}

	/* A Frame that is less than 64-bytes (from DA thru the FCS field)
	 * is considered as Runt Frame.
	 * Most of the Runt Frames happen because of collisions.
	 */
	if (dsc->apllication_status & EMAC_RX_FRAME_RUNT) {
		netdev_dbg(priv->ndev, "rx frame less than 64.\n");
		return FRAME_DISCARD;
	}

	/* When the frame fails the CRC check,
	 * the frame is assumed to have the CRC error
	 */
	if (dsc->apllication_status & EMAC_RX_FRAME_CRC_ERR) {
		netdev_dbg(priv->ndev, "rx frame crc error\n");
		return FRAME_DISCARD;
	}

	/* When the length of the frame exceeds
	 * the Programmed Max Frame Length
	 */
	if (dsc->apllication_status & EMAC_RX_FRAME_MAX_LEN_ERR) {
		netdev_dbg(priv->ndev, "rx frame too long\n");
		return FRAME_DISCARD;
	}

	/* frame reception is truncated at that point and
	 * frame is considered to have Jabber Error
	 */
	if (dsc->apllication_status & EMAC_RX_FRAME_JABBER_ERR) {
		netdev_dbg(priv->ndev, "rx frame has been truncated\n");
		return FRAME_DISCARD;
	}

	/* this bit is only for 802.3 Type Frames */
	if (dsc->apllication_status & EMAC_RX_FRAME_LENGTH_ERR) {
		netdev_dbg(priv->ndev, "rx frame length err for 802.3\n");
		return FRAME_DISCARD;
	}

	if (dsc->frm_packet_len <= ETHERNET_FCS_SIZE ||
	    dsc->frm_packet_len > priv->dma_buf_sz) {
		netdev_dbg(priv->ndev, "rx frame len too small or too long\n");
		return FRAME_DISCARD;
	}

	return FRAME_OK;
}

static void emac_alloc_rx_desc_buffers(struct emac_priv *priv)
{
	struct net_device *ndev = priv->ndev;
	struct emac_desc_ring *rx_ring = &priv->rx_ring;
	struct emac_desc_buffer *rx_buf;
	struct sk_buff *skb;
	struct emac_rx_desc *rx_desc;
	u32 i;

	i = rx_ring->head;
	rx_buf = &rx_ring->desc_buf[i];

	while (!rx_buf->skb) {
		skb = netdev_alloc_skb_ip_align(ndev, priv->dma_buf_sz);
		if (!skb)
			break;

		skb->dev = ndev;

		rx_buf->skb = skb;
		rx_buf->dma_len = priv->dma_buf_sz;
		rx_buf->dma_addr = dma_map_single(&priv->pdev->dev, skb->data,
						  priv->dma_buf_sz, DMA_FROM_DEVICE);
		if (dma_mapping_error(&priv->pdev->dev, rx_buf->dma_addr)) {
			netdev_err(ndev, "dma mapping_error\n");
			goto dma_map_err;
		}

		rx_desc = &((struct emac_rx_desc *)rx_ring->desc_addr)[i];

		memset(rx_desc, 0, sizeof(struct emac_rx_desc));

		rx_desc->buf_addr1 = rx_buf->dma_addr;
		rx_desc->buf_size1 = rx_buf->dma_len;

		rx_desc->first_descriptor = 0;
		rx_desc->last_descriptor = 0;
		if (++i == rx_ring->total_cnt) {
			rx_desc->end_ring = 1;
			i = 0;
		}
		dma_wmb();
		rx_desc->own = 1;

		rx_buf = &rx_ring->desc_buf[i];
	}
	rx_ring->head = i;

	return;

dma_map_err:
	dev_kfree_skb_any(skb);
	rx_buf->skb = NULL;
}

static int emac_rx_clean_desc(struct emac_priv *priv, int budget)
{
	struct emac_desc_ring *rx_ring;
	struct emac_desc_buffer *rx_buf;
	struct net_device *ndev = priv->ndev;
	struct emac_rx_desc *rx_desc;
	struct sk_buff *skb = NULL;
	int status;
	u32 receive_packet = 0;
	u32 i;
	u32 skb_len;

	rx_ring = &priv->rx_ring;

	i = rx_ring->tail;

	while (budget--) {
		/* get rx desc */
		rx_desc = &((struct emac_rx_desc *)rx_ring->desc_addr)[i];

		/* if rx_desc still owned by DMA, so we need to wait */
		if (rx_desc->own)
			break;

		rx_buf = &rx_ring->desc_buf[i];

		if (!rx_buf->skb)
			break;

		receive_packet++;

		dma_unmap_single(&priv->pdev->dev, rx_buf->dma_addr,
				 rx_buf->dma_len, DMA_FROM_DEVICE);

		status = emac_rx_frame_status(priv, rx_desc);
		if (unlikely(status == FRAME_DISCARD)) {
			ndev->stats.rx_dropped++;
			dev_kfree_skb_irq(rx_buf->skb);
			rx_buf->skb = NULL;
		} else {
			skb = rx_buf->skb;
			skb_len = rx_desc->frm_packet_len - ETHERNET_FCS_SIZE;
			skb_put(skb, skb_len);
			skb->dev = ndev;
			ndev->hard_header_len = ETH_HLEN;

			emac_get_rx_hwtstamp(priv, rx_desc, skb);

			skb->protocol = eth_type_trans(skb, ndev);

			skb->ip_summed = CHECKSUM_NONE;

			napi_gro_receive(&priv->napi, skb);

			ndev->stats.rx_packets++;
			ndev->stats.rx_bytes += skb_len;

			memset(rx_desc, 0, sizeof(struct emac_rx_desc));
			rx_buf->skb = NULL;
		}

		if (++i == rx_ring->total_cnt)
			i = 0;
	}

	rx_ring->tail = i;

	emac_alloc_rx_desc_buffers(priv);

	return receive_packet;
}

static int emac_rx_poll(struct napi_struct *napi, int budget)
{
	struct emac_priv *priv = container_of(napi, struct emac_priv, napi);
	int work_done;

	emac_tx_clean_desc(priv);

	work_done = emac_rx_clean_desc(priv, budget);
	if (work_done < budget) {
		napi_complete(napi);
		emac_enable_interrupt(priv);
	}

	return work_done;
}

static int emac_tx_mem_map(struct emac_priv *priv, struct sk_buff *skb,
			   u32 max_tx_len, u32 frag_num)
{
	struct emac_desc_ring *tx_ring;
	struct emac_tx_desc_buffer *tx_buf;
	struct emac_tx_desc *tx_desc;
	u32 skb_linear_len = skb_headlen(skb);
	u32 len, i, f, first, buf_idx = 0;
	phys_addr_t addr;
	u8 do_tx_timestamp = 0;

	tx_ring = &priv->tx_ring;

	i = tx_ring->head;
	first = i;

	skb_tx_timestamp(skb);

	if (unlikely((skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP) && priv->hwts_tx_en)) {
		/* declare that device is doing timestamping */
		skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
		do_tx_timestamp = 1;
	}

	if (++i == tx_ring->total_cnt)
		i = 0;

	/* if the data is fragmented */
	for (f = 0; f < frag_num; f++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[f];

		len = skb_frag_size(frag);

		buf_idx = (f + 1) % 2;

		/* first frag fill into second buffer of first descriptor */
		if (f == 0) {
			tx_buf = &tx_ring->tx_desc_buf[first];
			tx_desc = &((struct emac_tx_desc *)tx_ring->desc_addr)[first];
		} else {
			/* from second frags to more frags,
			 * we only get new descriptor when it frag num is odd.
			 */
			if (!buf_idx) {
				tx_buf = &tx_ring->tx_desc_buf[i];
				tx_desc = &((struct emac_tx_desc *)tx_ring->desc_addr)[i];
			}
		}
		tx_buf->buf[buf_idx].dma_len = len;

		addr = skb_frag_dma_map(&priv->pdev->dev, frag, 0,
					skb_frag_size(frag), DMA_TO_DEVICE);

		if (dma_mapping_error(&priv->pdev->dev, addr)) {
			netdev_err(priv->ndev, "%s dma map page:%d error\n", __func__, f);
			goto dma_map_err;
		}
		tx_buf->buf[buf_idx].dma_addr = addr;
		tx_buf->buf[buf_idx].map_as_page = true;

		if (do_tx_timestamp)
			tx_buf->timestamped = 1;

		/* every desc has two buffer for packet */

		if (buf_idx) {
			tx_desc->buf_addr2 = addr;
			tx_desc->buf_size2 = len;
		} else {
			tx_desc->buf_addr1 = addr;
			tx_desc->buf_size1 = len;

			if (++i == tx_ring->total_cnt) {
				tx_desc->end_ring = 1;
				i = 0;
			}
		}

		/* if frag num equal 1, we don't set tx_desc except buffer addr & size */
		if (f > 0) {
			if (f == (frag_num - 1)) {
				tx_desc->last_segment = 1;
				tx_buf->skb = skb;
				if (emac_tx_coal(priv, frag_num + 1))
					tx_desc->int_on_complet = 1;
			}

			tx_desc->own = 1;
		}
	}

	/* fill out first descriptor for skb linear data */
	tx_buf = &tx_ring->tx_desc_buf[first];

	tx_buf->buf[0].dma_len = skb_linear_len;

	addr = dma_map_single(&priv->pdev->dev, skb->data, skb_linear_len, DMA_TO_DEVICE);
	if (dma_mapping_error(&priv->pdev->dev, addr)) {
		netdev_err(priv->ndev, "%s dma mapping_error\n", __func__);
		goto dma_map_err;
	}

	tx_buf->buf[0].dma_addr = addr;
	tx_buf->buf[0].buff_addr = skb->data;
	tx_buf->buf[0].map_as_page = false;

	/* fill tx descriptor */
	tx_desc = &((struct emac_tx_desc *)tx_ring->desc_addr)[first];
	tx_desc->buf_addr1 = addr;
	tx_desc->buf_size1 = skb_linear_len;
	tx_desc->first_segment = 1;

	/* if last desc for ring, need to end ring flag */
	if (first == (tx_ring->total_cnt - 1))
		tx_desc->end_ring = 1;

	/* if frag num more than 1, that means data need another desc
	 * so current descriptor isn't last piece of packet data.
	 */
	tx_desc->last_segment = frag_num > 1 ? 0 : 1;
	if (frag_num <= 1 && emac_tx_coal(priv, 1))
		tx_desc->int_on_complet = 1;

	if (do_tx_timestamp) {
		tx_desc->tx_timestamp = 1;
		tx_buf->timestamped = 1;
	}

	/* only last descriptor had skb pointer */
	if (tx_desc->last_segment)
		tx_buf->skb = skb;

	tx_desc->own = 1;

	dma_wmb();

	emac_dma_start_transmit(priv);

	/* update tx ring head */
	tx_ring->head = i;

	return 0;

dma_map_err:
	dev_kfree_skb_any(skb);
	priv->ndev->stats.tx_dropped++;
	return 0;
}

static u32 read_tx_stat_cntr(struct emac_priv *priv, u8 cnt)
{
	u32 val, tmp;

	val = 0x8000 | cnt;
	emac_wr(priv, MAC_TX_STATCTR_CONTROL, val);
	val = emac_rd(priv, MAC_TX_STATCTR_CONTROL);

	if (readl_poll_timeout_atomic(priv->iobase + MAC_TX_STATCTR_CONTROL,
				      val, !(val & 0x8000), 100, 10000)) {
		pr_err("%s timeout!!\n", __func__);
		return -EINVAL;
	}

	tmp = emac_rd(priv, MAC_TX_STATCTR_DATA_HIGH);
	val = tmp << 16;
	tmp = emac_rd(priv, MAC_TX_STATCTR_DATA_LOW);
	val |= tmp;

	return val;
}

static u32 read_rx_stat_cntr(struct emac_priv *priv, u8 cnt)
{
	u32 val, tmp;

	val = 0x8000 | cnt;
	emac_wr(priv, MAC_RX_STATCTR_CONTROL, val);
	val = emac_rd(priv, MAC_RX_STATCTR_CONTROL);

	if (readl_poll_timeout_atomic(priv->iobase + MAC_RX_STATCTR_CONTROL,
				      val, !(val & 0x8000), 100, 10000)) {
		pr_err("%s timeout!!\n", __func__);
		return -EINVAL;
	}

	tmp = emac_rd(priv, MAC_RX_STATCTR_DATA_HIGH);
	val = tmp << 16;
	tmp = emac_rd(priv, MAC_RX_STATCTR_DATA_LOW);
	val |= tmp;

	return val;
}

static void emac_mac_multicast_filter_clear(struct emac_priv *priv)
{
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE1, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE2, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE3, 0x0);
	emac_wr(priv, MAC_MULTICAST_HASH_TABLE4, 0x0);
}

static void emac_reset(struct emac_priv *priv)
{
	if (!test_and_clear_bit(EMAC_RESET_REQUESTED, &priv->state))
		return;
	if (test_bit(EMAC_DOWN, &priv->state))
		return;

	netdev_err(priv->ndev, "Reset controller.\n");

	rtnl_lock();
	netif_trans_update(priv->ndev);
	while (test_and_set_bit(EMAC_RESETING, &priv->state))
		usleep_range(1000, 2000);
	set_bit(EMAC_DOWN, &priv->state);
	dev_close(priv->ndev);
	dev_open(priv->ndev, NULL);
	clear_bit(EMAC_DOWN, &priv->state);
	clear_bit(EMAC_RESETING, &priv->state);
	rtnl_unlock();
}

static void emac_tx_timeout_task(struct work_struct *work)
{
	struct emac_priv *priv = container_of(work, struct emac_priv, tx_timeout_task);

	emac_reset(priv);
	clear_bit(EMAC_TASK_SCHED, &priv->state);
}

static int clk_phase_rgmii_set(struct emac_priv *priv, bool is_tx)
{
	u32 val;

	switch (priv->clk_tuning_way) {
	case CLK_TUNING_BY_REG:
		val = readl(priv->ctrl_reg);
		if (is_tx) {
			val &= ~RGMII_TX_PHASE_MASK;
			val |= (priv->tx_clk_phase & 0x7) << RGMII_TX_PHASE_OFFSET;
		} else {
			val &= ~RGMII_RX_PHASE_MASK;
			val |= (priv->rx_clk_phase & 0x7) << RGMII_RX_PHASE_OFFSET;
		}
		writel(val, priv->ctrl_reg);
		break;

	case CLK_TUNING_BY_DLINE:
		val = readl(priv->dline_reg);
		if (is_tx) {
			val &= ~EMAC_TX_DLINE_CODE_MASK;
			val |= priv->tx_clk_phase << EMAC_TX_DLINE_CODE_OFFSET;
			val |= EMAC_TX_DLINE_EN;
		} else {
			val &= ~EMAC_RX_DLINE_CODE_MASK;
			val |= priv->rx_clk_phase << EMAC_RX_DLINE_CODE_OFFSET;
			val |= EMAC_RX_DLINE_EN;
		}
		writel(val, priv->dline_reg);
		break;

	default:
		pr_err("wrong clk tuning way:%d !!\n", priv->clk_tuning_way);
		return -1;
	}
	pr_debug("%s tx phase:%d rx phase:%d\n", __func__, priv->tx_clk_phase, priv->rx_clk_phase);

	return 0;
}

static int clk_phase_rmii_set(struct emac_priv *priv, bool is_tx)
{
	u32 val;

	switch (priv->clk_tuning_way) {
	case CLK_TUNING_BY_REG:
		val = readl(priv->ctrl_reg);
		if (is_tx) {
			val &= ~RMII_TX_PHASE_MASK;
			val |= (priv->tx_clk_phase & 0x7) << RMII_TX_PHASE_OFFSET;
		} else {
			val &= ~RMII_RX_PHASE_MASK;
			val |= (priv->rx_clk_phase & 0x7) << RMII_RX_PHASE_OFFSET;
		}
		writel(val, priv->ctrl_reg);
		break;

	case CLK_TUNING_BY_CLK_REVERT:
		val = readl(priv->ctrl_reg);
		if (is_tx) {
			if (priv->tx_clk_phase == CLK_PHASE_REVERT)
				val |= RMII_TX_CLK_SEL;
			else
				val &= ~RMII_TX_CLK_SEL;
		} else {
			if (priv->rx_clk_phase == CLK_PHASE_REVERT)
				val |= RMII_RX_CLK_SEL;
			else
				val &= ~RMII_RX_CLK_SEL;
		}
		writel(val, priv->ctrl_reg);
		break;

	default:
		pr_err("wrong clk tuning way:%d !!\n", priv->clk_tuning_way);
		return -1;
	}
	pr_debug("%s tx phase:%d rx phase:%d\n", __func__, priv->tx_clk_phase, priv->rx_clk_phase);

	return 0;
}

static int rx_coal_param_set(struct emac_priv *priv)
{
	u32 val;

	val = emac_rd(priv, DMA_RECEIVE_IRQ_MITIGATION_CTRL);
	val &= ~MREGBIT_RECEIVE_IRQ_FRAME_COUNTER_MSK;
	val |= (priv->rx_coal_frames & MREGBIT_RECEIVE_IRQ_FRAME_COUNTER_MSK);
	val &= ~MREGBIT_RECEIVE_IRQ_TIMEOUT_COUNTER_MSK;
	val |= ((priv->rx_coal_timeout * AXI_CLK_CYCLES_PER_US) <<
		MREGBIT_RECEIVE_IRQ_TIMEOUT_COUNTER_OFST) &
		MREGBIT_RECEIVE_IRQ_TIMEOUT_COUNTER_MSK;
	emac_wr(priv, DMA_RECEIVE_IRQ_MITIGATION_CTRL, val);

	return 0;
}

static int clk_phase_set(struct emac_priv *priv, bool is_tx)
{
	if (priv->clk_tuning_enable) {
		if (emac_is_rmii(priv))
			clk_phase_rmii_set(priv, is_tx);
		else
			clk_phase_rgmii_set(priv, is_tx);
	}

	return 0;
}

static int emac_mii_reset(struct mii_bus *bus)
{
	struct emac_priv *priv = bus->priv;
	struct device *dev = &priv->pdev->dev;
	int rst_gpio, ldo_gpio;
	int active_state;
	u32 delays[3] = {0};

	if (dev->of_node) {
		struct device_node *np = dev->of_node;

		if (!np)
			return 0;

		ldo_gpio = of_get_named_gpio(np, "emac,ldo-gpio", 0);
		if (ldo_gpio >= 0) {
			if (gpio_request(ldo_gpio, "mdio-ldo"))
				return 0;

			gpio_direction_output(ldo_gpio, 1);
		}

		rst_gpio = of_get_named_gpio(np, "emac,reset-gpio", 0);
		if (rst_gpio < 0)
			return 0;

		active_state = of_property_read_bool(np, "emac,reset-active-low");
		of_property_read_u32_array(np, "emac,reset-delays-us", delays, 3);

		if (gpio_request(rst_gpio, "mdio-reset"))
			return 0;

		gpio_direction_output(rst_gpio, active_state ? 1 : 0);
		if (delays[0])
			msleep(DIV_ROUND_UP(delays[0], 1000));

		gpio_set_value(rst_gpio, active_state ? 0 : 1);
		if (delays[1])
			msleep(DIV_ROUND_UP(delays[1], 1000));

		gpio_set_value(rst_gpio, active_state ? 1 : 0);
		if (delays[2])
			msleep(DIV_ROUND_UP(delays[2], 1000));
	}
	return 0;
}

static int emac_mii_read(struct mii_bus *bus, int phy_addr, int regnum)
{
	struct emac_priv *priv = bus->priv;
	u32 cmd = 0;
	u32 val;

	cmd |= phy_addr & 0x1F;
	cmd |= (regnum & 0x1F) << 5;
	cmd |= MREGBIT_START_MDIO_TRANS | MREGBIT_MDIO_READ_WRITE;

	emac_wr(priv, MAC_MDIO_DATA, 0x0);
	emac_wr(priv, MAC_MDIO_CONTROL, cmd);

	if (readl_poll_timeout(priv->iobase + MAC_MDIO_CONTROL,
			       val, !((val >> 15) & 0x1), 100, 10000))
		return -EBUSY;

	val = emac_rd(priv, MAC_MDIO_DATA);
	return val;
}

static int emac_mii_write(struct mii_bus *bus, int phy_addr, int regnum, u16 value)
{
	struct emac_priv *priv = bus->priv;
	u32 cmd = 0;
	u32 val;

	emac_wr(priv, MAC_MDIO_DATA, value);

	cmd |= phy_addr & 0x1F;
	cmd |= (regnum & 0x1F) << 5;
	cmd |= MREGBIT_START_MDIO_TRANS;
	emac_wr(priv, MAC_MDIO_CONTROL, cmd);

	if (readl_poll_timeout(priv->iobase + MAC_MDIO_CONTROL,
			       val, !((val >> 15) & 0x1), 100, 10000))
		return -EBUSY;

	return 0;
}

static void emac_adjust_link(struct net_device *dev)
{
	struct phy_device *phydev = dev->phydev;
	struct emac_priv *priv = netdev_priv(dev);
	bool link_changed = false;
	u32 ctrl;

	if (!phydev)
		return;

	if (phydev->link) {
		ctrl = emac_rd(priv, MAC_GLOBAL_CONTROL);

		/* Now we make sure that we can be in full duplex mode
		 * If not, we operate in half-duplex mode.
		 */
		if (phydev->duplex != priv->duplex) {
			link_changed = true;

			if (!phydev->duplex)
				ctrl &= ~MREGBIT_FULL_DUPLEX_MODE;
			else
				ctrl |= MREGBIT_FULL_DUPLEX_MODE;
			priv->duplex = phydev->duplex;
		}

		if (phydev->speed != priv->speed) {
			link_changed = true;

			ctrl &= ~MREGBIT_SPEED;

			switch (phydev->speed) {
			case SPEED_1000:
				ctrl |= MREGBIT_SPEED_1000M;
				break;

			case SPEED_100:
				ctrl |= MREGBIT_SPEED_100M;
				break;

			case SPEED_10:
				ctrl |= MREGBIT_SPEED_10M;
				break;

			default:
				pr_err("broken speed: %d\n", phydev->speed);
				phydev->speed = SPEED_UNKNOWN;
				break;
			}
			if (phydev->speed != SPEED_UNKNOWN)
				priv->speed = phydev->speed;
		}

		emac_wr(priv, MAC_GLOBAL_CONTROL, ctrl);

		if (!priv->link) {
			priv->link = true;
			link_changed = true;
		}
	} else if (priv->link) {
		priv->link = false;
		link_changed = true;
		priv->duplex = DUPLEX_UNKNOWN;
		priv->speed = SPEED_UNKNOWN;
	}

	if (link_changed)
		phy_print_status(phydev);
}

static int emac_phy_connect(struct net_device *dev)
{
	struct phy_device *phydev;
	struct device_node *np;
	struct emac_priv *priv = netdev_priv(dev);

	np = of_parse_phandle(priv->pdev->dev.of_node, "phy-handle", 0);
	if (!np && of_phy_is_fixed_link(priv->pdev->dev.of_node))
		np = of_node_get(priv->pdev->dev.of_node);
	if (!np)
		return -ENODEV;

	of_get_phy_mode(np, &priv->phy_interface);
	pr_info("priv phy_interface = %d\n", priv->phy_interface);

	emac_phy_interface_config(priv);

	phydev = of_phy_connect(dev, np, &emac_adjust_link, 0, priv->phy_interface);
	if (IS_ERR_OR_NULL(phydev)) {
		pr_err("Could not attach to PHY\n");
		if (!phydev)
			return -ENODEV;
		return PTR_ERR(phydev);
	}

	pr_info("%s:  %s: attached to PHY (UID 0x%x) Link = %d\n",
		__func__, dev->name, phydev->phy_id, phydev->link);

	/* Indicate that the MAC is responsible for PHY PM */
	phydev->mac_managed_pm = true;
	dev->phydev = phydev;

	clk_phase_set(priv, TX_PHASE);
	clk_phase_set(priv, RX_PHASE);
	return 0;
}

static int emac_mdio_init(struct emac_priv *priv)
{
	struct device_node *mii_np;
	struct device *dev = &priv->pdev->dev;
	int ret;

	mii_np = of_get_child_by_name(dev->of_node, "mdio-bus");
	if (!mii_np) {
		if (of_phy_is_fixed_link(dev->of_node)) {
			if ((of_phy_register_fixed_link(dev->of_node) < 0))
				return -ENODEV;
			dev_dbg(dev, "find fixed link\n");
			return 0;
		}

		dev_err(dev, "no %s child node found", "mdio-bus");
		return -ENODEV;
	}

	if (!of_device_is_available(mii_np)) {
		ret = -ENODEV;
		goto err_put_node;
	}

	priv->mii = devm_mdiobus_alloc(dev);
	if (!priv->mii) {
		ret = -ENOMEM;
		goto err_put_node;
	}
	priv->mii->priv = priv;
	priv->mii->name = "emac mii";
	priv->mii->reset = emac_mii_reset;
	priv->mii->read = emac_mii_read;
	priv->mii->write = emac_mii_write;
	snprintf(priv->mii->id, MII_BUS_ID_SIZE, "%s", priv->pdev->name);
	priv->mii->parent = dev;
	priv->mii->phy_mask = 0xffffffff;
	ret = of_mdiobus_register(priv->mii, mii_np);
	if (ret) {
		dev_err(dev, "Failed to register mdio bus.\n");
		goto err_put_node;
	}

	priv->phy = phy_find_first(priv->mii);
	if (!priv->phy) {
		dev_err(dev, "no PHY found\n");
		return -ENODEV;
	}

err_put_node:
	of_node_put(mii_np);
	return ret;
}

static int emac_mdio_deinit(struct emac_priv *priv)
{
	if (!priv->mii)
		return 0;

	mdiobus_unregister(priv->mii);
	return 0;
}

static void emac_stats_update(struct emac_priv *priv)
{
	struct emac_hw_stats *hwstats = priv->hw_stats;
	int i;
	u32 *p;

	p = (u32 *)(hwstats);

	for (i = 0; i < MAX_TX_STATS_NUM; i++)
		*(p + i) = read_tx_stat_cntr(priv, i);

	p = (u32 *)hwstats + MAX_TX_STATS_NUM;

	for (i = 0; i < MAX_RX_STATS_NUM; i++)
		*(p + i) = read_rx_stat_cntr(priv, i);
}

static int emac_get_link_ksettings(struct net_device *ndev, struct ethtool_link_ksettings *cmd)
{
	if (!ndev->phydev)
		return -ENODEV;

	phy_ethtool_ksettings_get(ndev->phydev, cmd);
	return 0;
}

static int emac_set_link_ksettings(struct net_device *ndev,
				   const struct ethtool_link_ksettings *cmd)
{
	if (!ndev->phydev)
		return -ENODEV;

	return phy_ethtool_ksettings_set(ndev->phydev, cmd);
}

static void emac_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strscpy(info->driver, DRIVER_NAME, sizeof(info->driver));
	info->n_stats = ARRAY_SIZE(emac_ethtool_stats);
}

static void emac_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	int i;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < ARRAY_SIZE(emac_ethtool_stats); i++) {
			memcpy(data, emac_ethtool_stats[i].str, ETH_GSTRING_LEN);
			data += ETH_GSTRING_LEN;
		}
	break;
	}
}

static void emac_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	wol->supported = 0;
	wol->wolopts = 0;

	if (dev->phydev)
		phy_ethtool_get_wol(dev->phydev, wol);
}

static int emac_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct emac_priv *priv = netdev_priv(dev);
	struct device *kdev = &priv->pdev->dev;
	int ret;

	if (!device_can_wakeup(kdev))
		return -EOPNOTSUPP;

	/* Try Wake-on-LAN from the PHY first */
	if (dev->phydev) {
		ret = phy_ethtool_set_wol(dev->phydev, wol);
		if (!ret)
			device_set_wakeup_enable(kdev, !!wol->wolopts);
	}

	return ret;
}

static int emac_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(emac_ethtool_stats);
	default:
		return -EOPNOTSUPP;
	}
}

static void emac_get_ethtool_stats(struct net_device *dev, struct ethtool_stats *stats, u64 *data)
{
	struct emac_priv *priv = netdev_priv(dev);
	struct emac_hw_stats *hwstats = priv->hw_stats;
	u32 *data_src;
	u64 *data_dst;
	int i;

	if (netif_running(dev) && netif_device_present(dev)) {
		if (spin_trylock_bh(&hwstats->stats_lock)) {
			emac_stats_update(priv);
			spin_unlock_bh(&hwstats->stats_lock);
		}
	}

	data_dst = data;

	for (i = 0; i < ARRAY_SIZE(emac_ethtool_stats); i++) {
		data_src = (u32 *)hwstats + emac_ethtool_stats[i].offset;
		*data_dst++ = (u64)(*data_src);
	}
}

static void emac_ethtool_get_regs(struct net_device *dev,
				  struct ethtool_regs *regs, void *space)
{
	struct emac_priv *priv = netdev_priv(dev);
	u32 *reg_space = (u32 *)space;
	void __iomem *base = priv->iobase;
	int i;

	regs->version = 1;

	memset(reg_space, 0x0, EMAC_REG_SPACE_SIZE);

	for (i = 0; i < EMAC_DMA_REG_CNT; i++)
		reg_space[i] = readl(base + DMA_CONFIGURATION + i * 4);

	for (i = 0; i < EMAC_MAC_REG_CNT; i++)
		reg_space[i + EMAC_DMA_REG_CNT] = readl(base + MAC_GLOBAL_CONTROL + i * 4);
}

static int emac_ethtool_get_regs_len(struct net_device *dev)
{
	return EMAC_REG_SPACE_SIZE;
}

static int emac_get_ts_info(struct net_device *dev, struct ethtool_ts_info *info)
{
	struct emac_priv *priv = netdev_priv(dev);

	if (priv->ptp_support) {
		info->so_timestamping = SOF_TIMESTAMPING_TX_SOFTWARE |
					SOF_TIMESTAMPING_TX_HARDWARE |
					SOF_TIMESTAMPING_RX_SOFTWARE |
					SOF_TIMESTAMPING_RX_HARDWARE |
					SOF_TIMESTAMPING_SOFTWARE |
					SOF_TIMESTAMPING_RAW_HARDWARE;

		if (priv->ptp_clock)
			info->phc_index = ptp_clock_index(priv->ptp_clock);

		info->tx_types = (1 << HWTSTAMP_TX_OFF) | (1 << HWTSTAMP_TX_ON);

		info->rx_filters = ((1 << HWTSTAMP_FILTER_NONE) |
				    (1 << HWTSTAMP_FILTER_PTP_V1_L4_EVENT) |
				    (1 << HWTSTAMP_FILTER_PTP_V1_L4_SYNC) |
				    (1 << HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_L4_EVENT) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_L4_SYNC) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_EVENT) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_SYNC) |
				    (1 << HWTSTAMP_FILTER_PTP_V2_DELAY_REQ) |
				    (1 << HWTSTAMP_FILTER_ALL));
		return 0;
	} else {
		return ethtool_op_get_ts_info(dev, info);
	}
}

static int emac_get_coalesce(struct net_device *ndev, struct ethtool_coalesce *c,
			     struct kernel_ethtool_coalesce *kernel_coal,
			     struct netlink_ext_ack *extack)
{
	struct emac_priv *priv = netdev_priv(ndev);

	c->rx_coalesce_usecs = priv->rx_coal_timeout;
	c->rx_max_coalesced_frames = priv->rx_coal_frames;

	c->rx_max_coalesced_frames_low = MIN_RX_COAL_FRAMES;
	c->rx_max_coalesced_frames_high = MAX_RX_COAL_FRAMES;

	c->rx_coalesce_usecs_low = MIN_RX_COAL_TIMEOUT;
	c->rx_coalesce_usecs_high = MAX_RX_COAL_TIMEOUT;

	return 0;
}

static int emac_set_coalesce(struct net_device *ndev, struct ethtool_coalesce *c,
			     struct kernel_ethtool_coalesce *kernel_coal,
			     struct netlink_ext_ack *extack)
{
	struct emac_priv *priv = netdev_priv(ndev);

	if (c->rx_max_coalesced_frames > MAX_RX_COAL_FRAMES ||
	    c->rx_max_coalesced_frames < MIN_RX_COAL_FRAMES)
		return -EINVAL;

	if (c->rx_coalesce_usecs > MAX_RX_COAL_TIMEOUT ||
	    c->rx_coalesce_usecs < MIN_RX_COAL_TIMEOUT)
		return -EINVAL;

	/* Only update hardware if a change occurred. */
	if (priv->rx_coal_frames == c->rx_max_coalesced_frames &&
	    priv->rx_coal_timeout == c->rx_coalesce_usecs)
		return 0;

	priv->rx_coal_frames = c->rx_max_coalesced_frames;
	priv->rx_coal_timeout = c->rx_coalesce_usecs;

	rx_coal_param_set(priv);

	return 0;
}

static const struct ethtool_ops emac_ethtool_ops = {
	.supported_coalesce_params = ETHTOOL_COALESCE_RX_USECS | ETHTOOL_COALESCE_RX_MAX_FRAMES,
	.get_link_ksettings = emac_get_link_ksettings,
	.set_link_ksettings = emac_set_link_ksettings,
	.get_drvinfo = emac_get_drvinfo,
	.nway_reset = phy_ethtool_nway_reset,
	.get_link = ethtool_op_get_link,
	.get_strings = emac_get_strings,
	.get_wol = emac_get_wol,
	.set_wol = emac_set_wol,
	.get_sset_count = emac_get_sset_count,
	.get_ethtool_stats = emac_get_ethtool_stats,
	.get_regs = emac_ethtool_get_regs,
	.get_regs_len = emac_ethtool_get_regs_len,
	.get_ts_info = emac_get_ts_info,
	.get_coalesce = emac_get_coalesce,
	.set_coalesce = emac_set_coalesce,
};

static int emac_up(struct emac_priv *priv)
{
	struct net_device *ndev = priv->ndev;
	int ret;
	u32 val = 0;

	ret = emac_phy_connect(ndev);
	if (ret) {
		pr_err("%s  phy_connet failed\n", __func__);
		return ret;
	}

	/* init hardware */
	emac_init_hw(priv);
	emac_ptp_init(priv);
	emac_set_mac_addr(priv, ndev->dev_addr);
	/* configure transmit unit */
	emac_configure_tx(priv);
	/* configure rx unit */
	emac_configure_rx(priv);
	/* allocate buffers for receive descriptors */
	emac_alloc_rx_desc_buffers(priv);
	phy_set_asym_pause(ndev->phydev, true, false);
	if (ndev->phydev)
		phy_start(ndev->phydev);

	/* allocates interrupt resources and
	 * enables the interrupt line and IRQ handling
	 */
	ret = request_irq(priv->irq, emac_interrupt_handler, IRQF_SHARED, ndev->name, ndev);
	if (ret) {
		pr_err("request_irq failed\n");
		goto request_irq_failed;
	}

	/* enable mac interrupt */
	emac_wr(priv, MAC_INTERRUPT_ENABLE, 0x0000);

	val = MREGBIT_TRANSMIT_TRANSFER_DONE_INTR_ENABLE |
	      MREGBIT_TRANSMIT_DMA_STOPPED_INTR_ENABLE |
	      MREGBIT_RECEIVE_TRANSFER_DONE_INTR_ENABLE |
	      MREGBIT_RECEIVE_DMA_STOPPED_INTR_ENABLE |
	      MREGBIT_RECEIVE_MISSED_FRAME_INTR_ENABLE;
	emac_wr(priv, DMA_INTERRUPT_ENABLE, val);

	napi_enable(&priv->napi);
	netif_start_queue(ndev);
	return 0;

request_irq_failed:
	emac_reset_hw(priv);
	if (ndev->phydev) {
		phy_stop(ndev->phydev);
		phy_disconnect(ndev->phydev);
	}
	return ret;
}

static int emac_open(struct net_device *ndev)
{
	struct emac_priv *priv = netdev_priv(ndev);
	int ret;

	ret = emac_alloc_tx_resources(priv);
	if (ret) {
		pr_err("Error in setting up the Tx resources\n");
		goto emac_alloc_tx_resource_fail;
	}

	ret = emac_alloc_rx_resources(priv);
	if (ret) {
		pr_err("Error in setting up the Rx resources\n");
		goto emac_alloc_rx_resource_fail;
	}

	ret = emac_up(priv);
	if (ret) {
		pr_err("Error in making the net intrface up\n");
		goto emac_up_fail;
	}
	return 0;

emac_up_fail:
	emac_free_rx_resources(priv);
emac_alloc_rx_resource_fail:
	emac_free_tx_resources(priv);
emac_alloc_tx_resource_fail:
	return ret;
}

static int emac_close(struct net_device *ndev)
{
	struct emac_priv *priv = netdev_priv(ndev);

	emac_down(priv);
	emac_free_tx_resources(priv);
	emac_free_rx_resources(priv);

	return 0;
}

static int emac_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct emac_priv *priv = netdev_priv(ndev);
	int nfrags = skb_shinfo(skb)->nr_frags;

	if (unlikely(emac_tx_avail(priv) < nfrags + 1)) {
		if (!netif_queue_stopped(ndev)) {
			netif_stop_queue(ndev);
			pr_err_ratelimited("tx ring full, stop tx queue\n");
		}
		return NETDEV_TX_BUSY;
	}

	emac_tx_mem_map(priv, skb, MAX_DATA_LEN_TX_DES, nfrags);

	ndev->stats.tx_packets++;
	ndev->stats.tx_bytes += skb->len;

	/* Make sure there is space in the ring for the next send. */
	if (unlikely(emac_tx_avail(priv) <= (MAX_SKB_FRAGS + 2)))
		netif_stop_queue(ndev);

	return NETDEV_TX_OK;
}

static int emac_set_mac_address(struct net_device *ndev, void *addr)
{
	struct emac_priv *priv = netdev_priv(ndev);
	int ret = 0;

	ret = eth_mac_addr(ndev, addr);
	if (ret)
		return ret;

	/* if nic not running, we just save addr
	 * it will be set during device_open;
	 * otherwise directly change hw mac setting.
	 */
	if (netif_running(ndev))
		emac_set_mac_addr(priv, ndev->dev_addr);

	return ret;
}

static int emac_ioctl(struct net_device *ndev, struct ifreq *rq, int cmd)
{
	int ret = -EOPNOTSUPP;

	if (!netif_running(ndev))
		return -EINVAL;

	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		if (!ndev->phydev)
			return -EINVAL;
		ret = phy_mii_ioctl(ndev->phydev, rq, cmd);
		break;
	case SIOCSHWTSTAMP:
		ret = emac_hwtstamp_ioctl(ndev, rq);
		break;
	default:
		break;
	}

	return ret;
}

static int emac_change_mtu(struct net_device *ndev, int mtu)
{
	struct emac_priv *priv = netdev_priv(ndev);
	u32 frame_len;

	if (netif_running(ndev)) {
		pr_err("must be stopped to change its MTU\n");
		return -EBUSY;
	}

	frame_len = mtu + ETHERNET_HEADER_SIZE + ETHERNET_FCS_SIZE;

	if (frame_len < MINIMUM_ETHERNET_FRAME_SIZE || frame_len > EMAC_RX_BUF_4K) {
		pr_err("Invalid MTU setting\n");
		return -EINVAL;
	}

	if (frame_len <= EMAC_DEFAULT_BUFSIZE)
		priv->dma_buf_sz = EMAC_DEFAULT_BUFSIZE;
	else if (frame_len <= EMAC_RX_BUF_2K)
		priv->dma_buf_sz = EMAC_RX_BUF_2K;
	else
		priv->dma_buf_sz = EMAC_RX_BUF_4K;

	ndev->mtu = mtu;

	return 0;
}

static void emac_tx_timeout(struct net_device *ndev, unsigned int txqueue)
{
	struct emac_priv *priv = netdev_priv(ndev);

	netdev_info(ndev, "TX timeout\n");

	netif_carrier_off(priv->ndev);
	set_bit(EMAC_RESET_REQUESTED, &priv->state);

	if (!test_bit(EMAC_DOWN, &priv->state) && !test_and_set_bit(EMAC_TASK_SCHED, &priv->state))
		schedule_work(&priv->tx_timeout_task);
}

static void emac_rx_mode_set(struct net_device *ndev)
{
	struct emac_priv *priv = netdev_priv(ndev);
	struct netdev_hw_addr *ha;
	u32 mc_filter[4] = {0};
	u32 val;
	u32 crc32, bit, reg, hash;

	val = emac_rd(priv, MAC_ADDRESS_CONTROL);
	val &= ~MREGBIT_PROMISCUOUS_MODE;
	if (ndev->flags & IFF_PROMISC) {
		/* enable promisc mode */
		val |= MREGBIT_PROMISCUOUS_MODE;
	} else if ((ndev->flags & IFF_ALLMULTI) || (netdev_mc_count(ndev) > HASH_TABLE_SIZE)) {
		/* Pass all multi */
		/* Set the 64 bits of the HASH tab. To be updated if taller
		 * hash table is used
		 */
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE1, 0xffff);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE2, 0xffff);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE3, 0xffff);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE4, 0xffff);
	} else if (!netdev_mc_empty(ndev)) {
		emac_mac_multicast_filter_clear(priv);
		netdev_for_each_mc_addr(ha, ndev) {
			/* Calculate the CRC of the MAC address */
			crc32 = ether_crc(ETH_ALEN, ha->addr);

			/* The HASH Table is an array of 4 16-bit registers. It is
			 * treated like an array of 64 bits (BitArray[hash_value]).
			 * Use the upper 6 bits of the above CRC as the hash value.
			 */
			hash = (crc32 >> 26) & 0x3F;
			reg = hash / 16;
			bit = hash % 16;
			mc_filter[reg] |= BIT(bit);
			pr_debug("%s %pM crc32:0x%x hash:0x%x reg:%d bit:%d\n",
				 __func__, ha->addr, crc32, hash, reg, bit);
		}
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE1, mc_filter[0]);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE2, mc_filter[1]);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE3, mc_filter[2]);
		emac_wr(priv, MAC_MULTICAST_HASH_TABLE4, mc_filter[3]);
	}
	emac_wr(priv, MAC_ADDRESS_CONTROL, val);
}

static const struct net_device_ops emac_netdev_ops = {
	.ndo_open = emac_open,
	.ndo_stop = emac_close,
	.ndo_start_xmit = emac_start_xmit,
	.ndo_set_mac_address = emac_set_mac_address,
	.ndo_do_ioctl = emac_ioctl,
	.ndo_eth_ioctl = emac_ioctl,
	.ndo_change_mtu = emac_change_mtu,
	.ndo_tx_timeout = emac_tx_timeout,
	.ndo_set_rx_mode = emac_rx_mode_set,
};

static int emac_config_dt(struct platform_device *pdev, struct emac_priv *priv)
{
	struct device_node *np = pdev->dev.of_node;
	struct resource *res;
	u8 mac_addr[ETH_ALEN] = {0};
	u32 tx_phase, rx_phase;
	u32 ctrl_reg;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->iobase = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->iobase)) {
		dev_err(&pdev->dev, "failed to io remap res reg 0\n");
		return -ENOMEM;
	}

	if (of_property_read_u32(np, "k1,apmu-base-reg", &priv->apmu_base)) {
		dev_err(&pdev->dev, "failed to io delay line configuration register\n");
		return -ENOMEM;
	}

	priv->irq = irq_of_parse_and_map(np, 0);
	if (!priv->irq)
		return -ENXIO;

	if (of_property_read_u32(np, "ctrl-reg", &ctrl_reg)) {
		dev_err(&pdev->dev, "cannot find ctrl register in device tree\n");
		return -EINVAL;
	}

	priv->ctrl_reg = ioremap(priv->apmu_base + ctrl_reg, 4);

	if (of_property_read_u32(np, "tx-threshold", &priv->tx_threshold)) {
		priv->tx_threshold = DEFAULT_TX_THRESHOLD;
		dev_dbg(&pdev->dev, "%s tx_threshold using default value:%d\n",
			__func__, priv->tx_threshold);
	}

	if (of_property_read_u32(np, "rx-threshold", &priv->rx_threshold)) {
		priv->rx_threshold = DEFAULT_RX_THRESHOLD;
		dev_dbg(&pdev->dev, "%s rx_threshold using default value:%d\n",
			__func__, priv->rx_threshold);
	}

	if (of_property_read_u32(np, "tx-ring-num", &priv->tx_ring_num)) {
		priv->tx_ring_num = DEFAULT_TX_RING_NUM;
		dev_dbg(&pdev->dev, "%s tx_ring_num using default value:%d\n",
			__func__, priv->tx_ring_num);
	}

	if (of_property_read_u32(np, "rx-ring-num", &priv->rx_ring_num)) {
		priv->rx_ring_num = DEFAULT_RX_RING_NUM;
		dev_dbg(&pdev->dev, "%s rx_ring_num using default value:%d\n",
			__func__, priv->rx_ring_num);
	}

	if (of_property_read_u32(np, "dma-burst-len", &priv->dma_burst_len)) {
		priv->dma_burst_len = DEFAULT_DMA_BURST_LEN;
		dev_dbg(&pdev->dev, "%s dma_burst_len using default value:%d\n",
			__func__, priv->dma_burst_len);
	} else {
		if (priv->dma_burst_len <= 0 || priv->dma_burst_len > 7) {
			dev_err(&pdev->dev, "%s burst len illegal, use default vallue:%d\n",
				__func__, DEFAULT_DMA_BURST_LEN);
			priv->dma_burst_len = DEFAULT_DMA_BURST_LEN;
		}
	}

	if (of_property_read_u32(np, "rx_coal_frames", &priv->rx_coal_frames)) {
		priv->rx_coal_frames = DEFAULT_RX_COAL_FRAMES;
		dev_dbg(&pdev->dev, "%s rx_coal_frames using default value:%d\n",
			__func__, priv->rx_coal_frames);
	}

	if (of_property_read_u32(np, "rx_coal_timeout", &priv->rx_coal_timeout)) {
		priv->rx_coal_timeout = DEFAULT_RX_COAL_TIMEOUT;
		dev_dbg(&pdev->dev, "%s rx_coal_timeout using default value:%d\n",
			__func__, priv->rx_coal_timeout);
	}

	if (of_property_read_bool(np, "ref-clock-from-phy")) {
		priv->ref_clk_frm_soc = 0;
		dev_dbg(&pdev->dev, "%s ref clock from external phy\n", __func__);
	} else {
		priv->ref_clk_frm_soc = 1;
	}

	ret = of_get_mac_address(np, mac_addr);
	if (ret) {
		if (ret == -EPROBE_DEFER)
			return ret;

		dev_info(&pdev->dev, "Using random mac address\n");
		eth_hw_addr_random(priv->ndev);
	} else {
		eth_hw_addr_set(priv->ndev, mac_addr);
	}

	priv->ptp_support = of_property_read_bool(np, "ptp-support");
	if (priv->ptp_support) {
		dev_dbg(&pdev->dev, "EMAC support IEEE1588 PTP Protocol\n");
		if (of_property_read_u32(np, "ptp-clk-rate", &priv->ptp_clk_rate))
			priv->ptp_clk_rate = 20000000;
	}
	priv->clk_tuning_enable = of_property_read_bool(np, "clk-tuning-enable");
	if (priv->clk_tuning_enable) {
		if (of_property_read_bool(np, "clk-tuning-by-reg")) {
			priv->clk_tuning_way = CLK_TUNING_BY_REG;
		} else if (of_property_read_bool(np, "clk-tuning-by-clk-revert")) {
			priv->clk_tuning_way = CLK_TUNING_BY_CLK_REVERT;
		} else if (of_property_read_bool(np, "clk-tuning-by-delayline")) {
			priv->clk_tuning_way = CLK_TUNING_BY_DLINE;
			if (of_property_read_u32(np, "dline-reg", &ctrl_reg)) {
				dev_err(&pdev->dev, "Delayline register not found in dts\n");
				return -EINVAL;
			}
			priv->dline_reg = ioremap(priv->apmu_base + ctrl_reg, 4);
		} else {
			priv->clk_tuning_way = CLK_TUNING_BY_REG;
		}

		if (of_property_read_u32(np, "tx-phase", &tx_phase))
			priv->tx_clk_phase = TXCLK_PHASE_DEFAULT;
		else
			priv->tx_clk_phase = tx_phase;

		if (of_property_read_u32(np, "rx-phase", &rx_phase))
			priv->rx_clk_phase = RXCLK_PHASE_DEFAULT;
		else
			priv->rx_clk_phase = rx_phase;
	}

	return 0;
}

static int emac_sw_init(struct emac_priv *priv)
{
	priv->dma_buf_sz = EMAC_DEFAULT_BUFSIZE;

	priv->tx_ring.total_cnt = priv->tx_ring_num;
	priv->rx_ring.total_cnt = priv->rx_ring_num;

	spin_lock_init(&priv->hw_stats->stats_lock);

	INIT_WORK(&priv->tx_timeout_task, emac_tx_timeout_task);

	priv->tx_coal_frames = EMAC_TX_FRAMES;
	priv->tx_coal_timeout = EMAC_TX_COAL_TIMEOUT;

	timer_setup(&priv->txtimer, emac_tx_coal_timer, 0);

	return 0;
}

static int emac_probe(struct platform_device *pdev)
{
	struct emac_priv *priv;
	struct net_device *ndev = NULL;
	int ret;

	ndev = alloc_etherdev(sizeof(struct emac_priv));
	if (!ndev)
		return -ENOMEM;

	ndev->hw_features = NETIF_F_SG;
	ndev->features |= ndev->hw_features;
	priv = netdev_priv(ndev);
	priv->ndev = ndev;
	priv->pdev = pdev;
	platform_set_drvdata(pdev, priv);
	priv->hw_stats = devm_kzalloc(&pdev->dev, sizeof(*priv->hw_stats), GFP_KERNEL);
	if (!priv->hw_stats) {
		dev_err(&pdev->dev, "failed to allocate counter memory\n");
		ret = -ENOMEM;
		goto err_netdev;
	}

	ret = emac_config_dt(pdev, priv);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to config dt\n");
		goto err_netdev;
	}

	ndev->watchdog_timeo = 5 * HZ;
	ndev->base_addr = (unsigned long)priv->iobase;
	ndev->irq  = priv->irq;

	ndev->ethtool_ops = &emac_ethtool_ops;
	ndev->netdev_ops = &emac_netdev_ops;

	priv->mac_clk = devm_clk_get(&pdev->dev, "emac-clk");
	if (IS_ERR(priv->mac_clk)) {
		dev_err(&pdev->dev, "emac clock not found.\n");
		ret = PTR_ERR(priv->mac_clk);
		goto err_netdev;
	}

	ret = clk_prepare_enable(priv->mac_clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to enable emac clock: %d\n", ret);
		goto err_netdev;
	}

	if (priv->ref_clk_frm_soc) {
		priv->phy_clk = devm_clk_get(&pdev->dev, "phy-clk");
		if (IS_ERR(priv->phy_clk)) {
			dev_err(&pdev->dev, "phy clock not found.\n");
			ret = PTR_ERR(priv->phy_clk);
			goto mac_clk_disable;
		}

		ret = clk_prepare_enable(priv->phy_clk);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to enable phy clock: %d\n", ret);
			goto mac_clk_disable;
		}
	}
	if (priv->ptp_support) {
		priv->ptp_clk = devm_clk_get(&pdev->dev, "ptp-clk");
		if (IS_ERR(priv->ptp_clk)) {
			dev_err(&pdev->dev, "ptp clock not found.\n");
			ret = PTR_ERR(priv->ptp_clk);
			goto phy_clk_disable;
		}
	}

	priv->reset = devm_reset_control_get_optional(&pdev->dev, NULL);
	if (IS_ERR(priv->reset)) {
		dev_err(&pdev->dev, "Failed to get emac's resets\n");
		goto ptp_clk_disable;
	}
	reset_control_deassert(priv->reset);

	emac_sw_init(priv);

	ret = emac_mdio_init(priv);
	if (ret) {
		dev_err(&pdev->dev, "failed to init mdio.\n");
		goto reset_assert;
	}

	SET_NETDEV_DEV(ndev, &pdev->dev);

	ret = register_netdev(ndev);
	if (ret) {
		pr_err("register_netdev failed\n");
		goto err_mdio_deinit;
	}
	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));

	netif_napi_add(ndev, &priv->napi, emac_rx_poll);

	return 0;

err_mdio_deinit:
	emac_mdio_deinit(priv);
reset_assert:
	reset_control_assert(priv->reset);
ptp_clk_disable:
	if (priv->ptp_support)
		clk_disable_unprepare(priv->ptp_clk);
phy_clk_disable:
	if (priv->ref_clk_frm_soc)
		clk_disable_unprepare(priv->phy_clk);
	del_timer_sync(&priv->txtimer);
mac_clk_disable:
	clk_disable_unprepare(priv->mac_clk);
err_netdev:
	free_netdev(ndev);
	return ret;
}

static int emac_remove(struct platform_device *pdev)
{
	struct emac_priv *priv = platform_get_drvdata(pdev);

	unregister_netdev(priv->ndev);
	emac_reset_hw(priv);
	free_netdev(priv->ndev);
	emac_mdio_deinit(priv);
	reset_control_assert(priv->reset);
	clk_disable_unprepare(priv->mac_clk);
	if (priv->ref_clk_frm_soc)
		clk_disable_unprepare(priv->phy_clk);
	return 0;
}

static const struct of_device_id emac_of_match[] = {
	{ .compatible = "spacemit,k1-emac" },
	{ },
};
MODULE_DEVICE_TABLE(of, emac_of_match);

static struct platform_driver emac_driver = {
	.probe = emac_probe,
	.remove = emac_remove,
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = of_match_ptr(emac_of_match),
	},
};

module_platform_driver(emac_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Ethernet driver for Spacemit k1 Emac");
MODULE_ALIAS("platform:spacemit_eth");
