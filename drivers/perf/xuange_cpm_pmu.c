// SPDX-License-Identifier: GPL-2.0
/*
 * Xuange CPM performance counter support.
 */

#include <linux/acpi.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/math64.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/smp.h>

#define XUANGE_CPM_PDEV_NAME    "xuange_cpm_pmu"

#define CPM_MAX_COUNTERS	8	/* 8 hw counters */
#define CPM_MAX_STAT_EVENTS	(CPM_MAX_COUNTERS + 16)	/* 8 hw counters + up to 16 stat events */

#define CPM_EVENT_MONITOR	0
#define CPM_OSTD_STAT		1
#define CPM_LCRD_STAT		2
/* CPM input clocks, independent of the CPU clock. */
#define CPM_SOC_CLK_HZ          1900000000U
#define CPM_FPGA_CLK_HZ         7600000U
#define CPM_SOC_MON_PERIOD      24
#define CPM_FPGA_MON_PERIOD     16
#define CPM_STOP_MARGIN_US      1000
#define CPM_RESET_DELAY_US      1
#define CPM_DEFAULT_EVT_DURATION  4096    /* Transaction timeout threshold in bus cycles. */

#define CPM_GET_EVENT(attr)		((attr)->config & 0x3FF)
#define CPM_GET_FILTER_EN(attr)		(((attr)->config >> 10) & 0x3F)
#define CPM_GET_FLT_ALIGN(attr)		(((attr)->config >> 16) & 0x3)
#define CPM_GET_SRCID_TGT(attr)		(((attr)->config >> 18) & 0x1FF)
#define CPM_GET_SRCID_MASK(attr)	(((attr)->config >> 27) & 0x1FF)
#define CPM_GET_TGTID_TGT(attr)		(((attr)->config >> 36) & 0x1FF)
#define CPM_GET_TGTID_MASK(attr)	(((attr)->config >> 45) & 0x1FF)
#define CPM_GET_PARTID_TGT(attr)	(((attr)->config >> 54) & 0x1F)
#define CPM_GET_PARTID_MASK(attr)	(((attr)->config >> 59) & 0x1F)

#define CPM_GET_ADDR_LO(attr)		((attr)->config1 & 0xFFFFFFFFFFFFULL)
#define CPM_GET_OPCODE_MASK(attr)	(((attr)->config1 >> 48) & 0x7F)
#define CPM_GET_OPCODE_TGT(attr)	(((attr)->config1 >> 55) & 0x7F)
#define CPM_GET_SEL(attr)		(((attr)->config1 >> 62) & 0x1)

#define CPM_GET_ADDR_HI(attr)		((attr)->config2 & 0xFFFFFFFFFFFFULL)

/* CSR registers */
#define CPM_CSR0              0x00
#define CPM_CSR1              0x04
#define CPM_CSR2              0x08
/* CSR0 bit definitions */
#define CPM_SWRST_N           BIT(0)
#define CPM_INT_EN_MASK       GENMASK(12, 8)
#define CPM_MON_DN_INT_EN     BIT(8)
#define CPM_MON_EN            BIT(4)
#define CPM_MON_DN_BIT        BIT(16)     /* mon_dn status in int_s */
#define CPM_MON_DN_INT        BIT(24)     /* mon_dn status in int_rs (W1C) */
#define CPM_INT_MASK          GENMASK(28, 24)
/* CSR1: mon_mode=0 selects continuous monitoring; trigger_re=0 is SW. */
#define CPM_CONTINUOUS_SW     0
#define CPM_TRIGGER_SW       BIT(4)
/* OSTD status registers */
#define CPM_STT_OSTD0         0x10
#define CPM_STT_OSTD1         0x14
#define CPM_STT_OSTD2         0x18
#define CPM_STT_OSTD3         0x1C
/* LCRD status registers */
#define CPM_STT_LCRD0         0x20
#define CPM_STT_LCRD1         0x24
#define CPM_STT_LCRD2         0x28
/* EVT slot registers — each slot occupies 0x40 bytes */
#define CPM_EVT_EN            BIT(0)
#define CPM_EVT_STRIDE        0x40
#define CPM_EVT_BASE(idx)     (0x40 + (idx) * CPM_EVT_STRIDE)
#define CPM_EVTx_CTL1(idx)    (CPM_EVT_BASE(idx) + 0x00)   /* +0x00 */
#define CPM_EVTx_CTL2(idx)    (CPM_EVT_BASE(idx) + 0x04)   /* +0x04 */
#define CPM_EVT_CNT(idx)      (CPM_EVT_BASE(idx) + 0x08)   /* +0x08 */
#define CPM_EVTx_FLT0(idx)    (CPM_EVT_BASE(idx) + 0x10)   /* +0x10 */
#define CPM_EVTx_FLT1(idx)    (CPM_EVT_BASE(idx) + 0x14)   /* +0x14 */
#define CPM_EVTx_FLT2(idx)    (CPM_EVT_BASE(idx) + 0x18)   /* +0x18 */
#define CPM_EVTx_FLT3(idx)    (CPM_EVT_BASE(idx) + 0x1C)   /* +0x1C */
#define CPM_EVTx_FLT4(idx)    (CPM_EVT_BASE(idx) + 0x20)   /* +0x20 */
#define CPM_EVTx_FLT5(idx)    (CPM_EVT_BASE(idx) + 0x24)   /* +0x24 */
#define CPM_EVTx_FLT6(idx)    (CPM_EVT_BASE(idx) + 0x28)   /* +0x28 */
#define CPM_EVTx_FLT7(idx)    (CPM_EVT_BASE(idx) + 0x2C)   /* +0x2C */

enum cpm_count_mode {
	CPM_COUNT_ACCUM,    /* delta accumulation (count events) */
	CPM_COUNT_SET,      /* overwrite (avg) */
	CPM_COUNT_MAX,      /* global maximum across windows */
	CPM_COUNT_MIN,      /* global minimum across windows */
	CPM_COUNT_AVG,      /* running average across windows (filters invalid samples) */
};

static enum cpuhp_state cpm_cpuhp_state;

struct cpm_pmu {
	struct device	*dev;
	void __iomem	*base;		/* CPM event registers (CSR/EVT/OSTD/LCRD) */
	void __iomem	*sysreg_base;	/* TPE_SYS_REG space, only for CPM_SEL at offset 0x34 */
	int		irq;
	struct pmu	pmu;
	int		cpu;		/* administrative CPU for uncore PMU */
	u32		cpm_sel;	/* 0=CPU Core0, 1=TPE */
	u32		mon_period;	/* monitoring period exponent, written to CSR2 */
	u64		stop_timeout_us;
	int		active_events;	/* number of currently active events */
	bool		shutting_down;	/* block new monitoring during shutdown */
	bool		hw_started;	/* whether hardware has been started */
	struct hlist_node	node;	/* CPU hotplug instance list node */
	DECLARE_BITMAP(used_mask, CPM_MAX_COUNTERS);	/* tracks allocated hw counters (bit 0-7) */
	/*
	 * Unified event array:
	 *   [0, CPM_MAX_COUNTERS)      — hardware counter slots, indexed by hw counter id.
	 *                                 Allocated via used_mask / find_first_zero_bit.
	 *   [CPM_MAX_COUNTERS, CPM_MAX_STAT_EVENTS) — OSTD/LCRD stat events (avg/min/max/real).
	 *                                 No hardware counter, read STT registers in ISR.
	 *   "real" events are sampled on read and stop; the ISR skips them.
	 *   Other statistics are updated on mon_dn.
	 *
	 * ISR iterates the full array, skipping NULL entries.
	 * cpm_event_update() dispatches by event_base to read the correct register.
	 */
	struct perf_event *events[CPM_MAX_STAT_EVENTS];
	u64 avg_sum[CPM_MAX_STAT_EVENTS];
	u32 avg_nr[CPM_MAX_STAT_EVENTS];
};

#define to_cpm_pmu(p) container_of(p, struct cpm_pmu, pmu)

#define CPM_PMU_EVENT_ATTR(_name, _config) \
	PMU_EVENT_ATTR_STRING(_name, cpm_event_attr_##_name, "event=" __stringify(_config))

/* event monitoring (requires counter allocation)
 * _config number reflects actual event number in CPM spec
 */
CPM_PMU_EVENT_ATTR(rd_trans,		0x00);
CPM_PMU_EVENT_ATTR(wr_trans,		0x01);
CPM_PMU_EVENT_ATTR(atomic_trans,	0x02);
CPM_PMU_EVENT_ATTR(stash_trans,		0x03);
CPM_PMU_EVENT_ATTR(dataless_trans,	0x04);
CPM_PMU_EVENT_ATTR(prefetch_trans,	0x05);
CPM_PMU_EVENT_ATTR(dvm_trans,		0x06);
CPM_PMU_EVENT_ATTR(all_trans,		0x07);
CPM_PMU_EVENT_ATTR(rd_retry,		0x08);
CPM_PMU_EVENT_ATTR(wr_retry,		0x09);
CPM_PMU_EVENT_ATTR(rd_timeout,		0x10);
CPM_PMU_EVENT_ATTR(wr_timeout,		0x11);
CPM_PMU_EVENT_ATTR(rd_avg_lat,		0x20);
CPM_PMU_EVENT_ATTR(wr_avg_lat,		0x21);
CPM_PMU_EVENT_ATTR(rd_max_lat,		0x28);
CPM_PMU_EVENT_ATTR(wr_max_lat,		0x29);
CPM_PMU_EVENT_ATTR(rd_min_lat,		0x30);
CPM_PMU_EVENT_ATTR(wr_min_lat,		0x31);

/* Latency values are measured in CPM input bus-clock cycles. */
PMU_EVENT_ATTR_STRING(rd_avg_lat.unit, cpm_event_attr_rd_avg_lat_unit, "cycles");
PMU_EVENT_ATTR_STRING(wr_avg_lat.unit, cpm_event_attr_wr_avg_lat_unit, "cycles");
PMU_EVENT_ATTR_STRING(rd_max_lat.unit, cpm_event_attr_rd_max_lat_unit, "cycles");
PMU_EVENT_ATTR_STRING(wr_max_lat.unit, cpm_event_attr_wr_max_lat_unit, "cycles");
PMU_EVENT_ATTR_STRING(rd_min_lat.unit, cpm_event_attr_rd_min_lat_unit, "cycles");
PMU_EVENT_ATTR_STRING(wr_min_lat.unit, cpm_event_attr_wr_min_lat_unit, "cycles");

/*
 * OSTD / LCRD events — no hardware counter slot needed.
 *   - "real" and "avg/min/max" events: idx = 8+ (placed in events[] stat
 *     region). The ISR updates avg/min/max on mon_dn.
 *   - "real" is read on demand while running and sampled once on stop;
 *     subsequent reads preserve the final snapshot.
 */
CPM_PMU_EVENT_ATTR(ostd_rd_avg,		0x100);
CPM_PMU_EVENT_ATTR(ostd_rd_real,	0x101);  /* real-time */
CPM_PMU_EVENT_ATTR(ostd_rd_min,		0x102);
CPM_PMU_EVENT_ATTR(ostd_rd_max,		0x103);
CPM_PMU_EVENT_ATTR(ostd_wr_avg,		0x110);
CPM_PMU_EVENT_ATTR(ostd_wr_real,	0x111);  /* real-time */
CPM_PMU_EVENT_ATTR(ostd_wr_min,		0x112);
CPM_PMU_EVENT_ATTR(ostd_wr_max,		0x113);
CPM_PMU_EVENT_ATTR(lcrd_txdat_avg,	0x200);
CPM_PMU_EVENT_ATTR(lcrd_txdat_min,	0x201);
CPM_PMU_EVENT_ATTR(lcrd_txdat_max,	0x202);
CPM_PMU_EVENT_ATTR(lcrd_txdat_real,	0x203);  /* real-time */
CPM_PMU_EVENT_ATTR(lcrd_txreq_avg,	0x204);
CPM_PMU_EVENT_ATTR(lcrd_txreq_min,	0x205);
CPM_PMU_EVENT_ATTR(lcrd_txreq_max,	0x206);
CPM_PMU_EVENT_ATTR(lcrd_txreq_real,	0x207);  /* real-time */
CPM_PMU_EVENT_ATTR(lcrd_rxdat_avg,	0x208);
CPM_PMU_EVENT_ATTR(lcrd_rxdat_min,	0x209);
CPM_PMU_EVENT_ATTR(lcrd_rxdat_max,	0x20A);
CPM_PMU_EVENT_ATTR(lcrd_rxdat_real,	0x20B);  /* real-time */
CPM_PMU_EVENT_ATTR(lcrd_txrsp_avg,	0x20C);
CPM_PMU_EVENT_ATTR(lcrd_txrsp_min,	0x20D);
CPM_PMU_EVENT_ATTR(lcrd_txrsp_max,	0x20E);
CPM_PMU_EVENT_ATTR(lcrd_txrsp_real,	0x20F);  /* real-time */
CPM_PMU_EVENT_ATTR(lcrd_rxsnp_avg,	0x210);
CPM_PMU_EVENT_ATTR(lcrd_rxsnp_min,	0x211);
CPM_PMU_EVENT_ATTR(lcrd_rxsnp_max,	0x212);
CPM_PMU_EVENT_ATTR(lcrd_rxsnp_real,	0x213);  /* real-time */
CPM_PMU_EVENT_ATTR(lcrd_rxreq_avg,	0x214);
CPM_PMU_EVENT_ATTR(lcrd_rxreq_min,	0x215);
CPM_PMU_EVENT_ATTR(lcrd_rxreq_max,	0x216);
CPM_PMU_EVENT_ATTR(lcrd_rxreq_real,	0x217);	/* real-time */

/* Report statistics directly, rather than subtracting successive values. */
PMU_EVENT_ATTR_STRING(rd_avg_lat.snapshot,
		      cpm_event_attr_rd_avg_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(wr_avg_lat.snapshot,
		      cpm_event_attr_wr_avg_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(rd_max_lat.snapshot,
		      cpm_event_attr_rd_max_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(wr_max_lat.snapshot,
		      cpm_event_attr_wr_max_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(rd_min_lat.snapshot,
		      cpm_event_attr_rd_min_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(wr_min_lat.snapshot,
		      cpm_event_attr_wr_min_lat_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_rd_avg.snapshot,
		      cpm_event_attr_ostd_rd_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_rd_min.snapshot,
		      cpm_event_attr_ostd_rd_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_rd_max.snapshot,
		      cpm_event_attr_ostd_rd_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_wr_avg.snapshot,
		      cpm_event_attr_ostd_wr_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_wr_min.snapshot,
		      cpm_event_attr_ostd_wr_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_wr_max.snapshot,
		      cpm_event_attr_ostd_wr_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txdat_avg.snapshot,
		      cpm_event_attr_lcrd_txdat_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txdat_min.snapshot,
		      cpm_event_attr_lcrd_txdat_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txdat_max.snapshot,
		      cpm_event_attr_lcrd_txdat_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txreq_avg.snapshot,
		      cpm_event_attr_lcrd_txreq_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txreq_min.snapshot,
		      cpm_event_attr_lcrd_txreq_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txreq_max.snapshot,
		      cpm_event_attr_lcrd_txreq_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxdat_avg.snapshot,
		      cpm_event_attr_lcrd_rxdat_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxdat_min.snapshot,
		      cpm_event_attr_lcrd_rxdat_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxdat_max.snapshot,
		      cpm_event_attr_lcrd_rxdat_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txrsp_avg.snapshot,
		      cpm_event_attr_lcrd_txrsp_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txrsp_min.snapshot,
		      cpm_event_attr_lcrd_txrsp_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txrsp_max.snapshot,
		      cpm_event_attr_lcrd_txrsp_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxsnp_avg.snapshot,
		      cpm_event_attr_lcrd_rxsnp_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxsnp_min.snapshot,
		      cpm_event_attr_lcrd_rxsnp_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxsnp_max.snapshot,
		      cpm_event_attr_lcrd_rxsnp_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxreq_avg.snapshot,
		      cpm_event_attr_lcrd_rxreq_avg_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxreq_min.snapshot,
		      cpm_event_attr_lcrd_rxreq_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxreq_max.snapshot,
		      cpm_event_attr_lcrd_rxreq_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_rd_real.snapshot,
		      cpm_event_attr_ostd_rd_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(ostd_wr_real.snapshot,
		      cpm_event_attr_ostd_wr_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txdat_real.snapshot,
		      cpm_event_attr_lcrd_txdat_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txreq_real.snapshot,
		      cpm_event_attr_lcrd_txreq_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxdat_real.snapshot,
		      cpm_event_attr_lcrd_rxdat_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_txrsp_real.snapshot,
		      cpm_event_attr_lcrd_txrsp_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxsnp_real.snapshot,
		      cpm_event_attr_lcrd_rxsnp_real_snapshot, "1");
PMU_EVENT_ATTR_STRING(lcrd_rxreq_real.snapshot,
		      cpm_event_attr_lcrd_rxreq_real_snapshot, "1");

static struct attribute *cpm_pmu_events_attrs[] = {
	&cpm_event_attr_rd_trans.attr.attr,
	&cpm_event_attr_wr_trans.attr.attr,
	&cpm_event_attr_atomic_trans.attr.attr,
	&cpm_event_attr_stash_trans.attr.attr,
	&cpm_event_attr_dataless_trans.attr.attr,
	&cpm_event_attr_prefetch_trans.attr.attr,
	&cpm_event_attr_dvm_trans.attr.attr,
	&cpm_event_attr_all_trans.attr.attr,
	&cpm_event_attr_rd_retry.attr.attr,
	&cpm_event_attr_wr_retry.attr.attr,
	&cpm_event_attr_rd_timeout.attr.attr,
	&cpm_event_attr_wr_timeout.attr.attr,
	&cpm_event_attr_rd_avg_lat.attr.attr,
	&cpm_event_attr_rd_avg_lat_snapshot.attr.attr,
	&cpm_event_attr_rd_avg_lat_unit.attr.attr,
	&cpm_event_attr_wr_avg_lat.attr.attr,
	&cpm_event_attr_wr_avg_lat_snapshot.attr.attr,
	&cpm_event_attr_wr_avg_lat_unit.attr.attr,
	&cpm_event_attr_rd_max_lat.attr.attr,
	&cpm_event_attr_rd_max_lat_snapshot.attr.attr,
	&cpm_event_attr_rd_max_lat_unit.attr.attr,
	&cpm_event_attr_wr_max_lat.attr.attr,
	&cpm_event_attr_wr_max_lat_snapshot.attr.attr,
	&cpm_event_attr_wr_max_lat_unit.attr.attr,
	&cpm_event_attr_rd_min_lat.attr.attr,
	&cpm_event_attr_rd_min_lat_snapshot.attr.attr,
	&cpm_event_attr_rd_min_lat_unit.attr.attr,
	&cpm_event_attr_wr_min_lat.attr.attr,
	&cpm_event_attr_wr_min_lat_snapshot.attr.attr,
	&cpm_event_attr_wr_min_lat_unit.attr.attr,
	&cpm_event_attr_ostd_rd_avg.attr.attr,
	&cpm_event_attr_ostd_rd_avg_snapshot.attr.attr,
	&cpm_event_attr_ostd_rd_real.attr.attr,
	&cpm_event_attr_ostd_rd_real_snapshot.attr.attr,
	&cpm_event_attr_ostd_rd_min.attr.attr,
	&cpm_event_attr_ostd_rd_min_snapshot.attr.attr,
	&cpm_event_attr_ostd_rd_max.attr.attr,
	&cpm_event_attr_ostd_rd_max_snapshot.attr.attr,
	&cpm_event_attr_ostd_wr_avg.attr.attr,
	&cpm_event_attr_ostd_wr_avg_snapshot.attr.attr,
	&cpm_event_attr_ostd_wr_real.attr.attr,
	&cpm_event_attr_ostd_wr_real_snapshot.attr.attr,
	&cpm_event_attr_ostd_wr_min.attr.attr,
	&cpm_event_attr_ostd_wr_min_snapshot.attr.attr,
	&cpm_event_attr_ostd_wr_max.attr.attr,
	&cpm_event_attr_ostd_wr_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txdat_avg.attr.attr,
	&cpm_event_attr_lcrd_txdat_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txdat_min.attr.attr,
	&cpm_event_attr_lcrd_txdat_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txdat_max.attr.attr,
	&cpm_event_attr_lcrd_txdat_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txdat_real.attr.attr,
	&cpm_event_attr_lcrd_txdat_real_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txreq_avg.attr.attr,
	&cpm_event_attr_lcrd_txreq_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txreq_min.attr.attr,
	&cpm_event_attr_lcrd_txreq_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txreq_max.attr.attr,
	&cpm_event_attr_lcrd_txreq_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txreq_real.attr.attr,
	&cpm_event_attr_lcrd_txreq_real_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxdat_avg.attr.attr,
	&cpm_event_attr_lcrd_rxdat_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxdat_min.attr.attr,
	&cpm_event_attr_lcrd_rxdat_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxdat_max.attr.attr,
	&cpm_event_attr_lcrd_rxdat_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxdat_real.attr.attr,
	&cpm_event_attr_lcrd_rxdat_real_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txrsp_avg.attr.attr,
	&cpm_event_attr_lcrd_txrsp_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txrsp_min.attr.attr,
	&cpm_event_attr_lcrd_txrsp_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txrsp_max.attr.attr,
	&cpm_event_attr_lcrd_txrsp_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_txrsp_real.attr.attr,
	&cpm_event_attr_lcrd_txrsp_real_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_avg.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_min.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_max.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_real.attr.attr,
	&cpm_event_attr_lcrd_rxsnp_real_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxreq_avg.attr.attr,
	&cpm_event_attr_lcrd_rxreq_avg_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxreq_min.attr.attr,
	&cpm_event_attr_lcrd_rxreq_min_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxreq_max.attr.attr,
	&cpm_event_attr_lcrd_rxreq_max_snapshot.attr.attr,
	&cpm_event_attr_lcrd_rxreq_real.attr.attr,
	&cpm_event_attr_lcrd_rxreq_real_snapshot.attr.attr,
	NULL,
};

/*
 * 10 bits: not just hardware evt_type (CTL0[13:8], 6 bits).
 * Software event_id namespace:
 *   0x00-0x3F  — CPM_EVENT_MONITOR, low 6 bits written to evt_type
 *   0x100-0x1FF — CPM_EVENT_OSTD, direct register read
 *   0x200-0x2FF — CPM_EVENT_LCRD, direct register read
 */
PMU_FORMAT_ATTR(event,        "config:0-9");
PMU_FORMAT_ATTR(filter_en,    "config:10-15");
PMU_FORMAT_ATTR(flt_align,    "config:16-17");
PMU_FORMAT_ATTR(srcid_tgt,    "config:18-26");
PMU_FORMAT_ATTR(srcid_mask,   "config:27-35");
PMU_FORMAT_ATTR(tgtid_tgt,    "config:36-44");
PMU_FORMAT_ATTR(tgtid_mask,   "config:45-53");
PMU_FORMAT_ATTR(partid_tgt,   "config:54-58");
PMU_FORMAT_ATTR(partid_mask,  "config:59-63");
PMU_FORMAT_ATTR(addr_lo,      "config1:0-47");
PMU_FORMAT_ATTR(opcode_mask,  "config1:48-54");
PMU_FORMAT_ATTR(opcode_tgt,   "config1:55-61");
PMU_FORMAT_ATTR(cpm_sel,      "config1:62");
PMU_FORMAT_ATTR(addr_hi,      "config2:0-47");

static struct attribute *cpm_pmu_format_attrs[] = {
	&format_attr_event.attr,
	&format_attr_filter_en.attr,
	&format_attr_flt_align.attr,
	&format_attr_srcid_tgt.attr,
	&format_attr_srcid_mask.attr,
	&format_attr_tgtid_tgt.attr,
	&format_attr_tgtid_mask.attr,
	&format_attr_partid_tgt.attr,
	&format_attr_partid_mask.attr,
	&format_attr_addr_lo.attr,
	&format_attr_opcode_mask.attr,
	&format_attr_opcode_tgt.attr,
	&format_attr_cpm_sel.attr,
	&format_attr_addr_hi.attr,
	NULL,
};

static ssize_t cpm_pmu_cpumask_show(struct device *dev,
				    struct device_attribute *attr, char *buf)
{
	struct cpm_pmu *cpm = to_cpm_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(cpm->cpu));
}

static struct device_attribute cpm_pmu_cpumask_attr =
	__ATTR(cpumask, 0444, cpm_pmu_cpumask_show, NULL);

static struct attribute *cpm_pmu_cpumask_attrs[] = {
	&cpm_pmu_cpumask_attr.attr,
	NULL,
};

static struct attribute_group cpm_pmu_events_attr_group = {
	.name = "events",
	.attrs = cpm_pmu_events_attrs,
};

static struct attribute_group cpm_pmu_format_attr_group = {
	.name = "format",
	.attrs = cpm_pmu_format_attrs,
};

static struct attribute_group cpm_pmu_cpumask_attr_group = {
	.attrs = cpm_pmu_cpumask_attrs,
};

static const struct attribute_group *cpm_pmu_attr_groups[] = {
	&cpm_pmu_events_attr_group,
	&cpm_pmu_format_attr_group,
	&cpm_pmu_cpumask_attr_group,
	NULL,
};

static bool cpm_is_avg_latency_event(u32 event_id)
{
	return (event_id >= 0x20 && event_id <= 0x21);
}

/* Only real statistics are valid independently of window completion. */
static bool cpm_is_realtime_stat(struct perf_event *event)
{
	u32 event_id = CPM_GET_EVENT(&event->attr);

	if (event->hw.event_base == CPM_OSTD_STAT)
		return (event_id & 0x3) == 1;
	if (event->hw.event_base == CPM_LCRD_STAT)
		return (event_id & 0x3) == 3;
	return false;
}

static int cpm_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct cpm_pmu *cpm = hlist_entry_safe(node, struct cpm_pmu, node);
	unsigned int target;
	int nid = dev_to_node(cpm->dev);
	int ret;

	if (cpu != cpm->cpu)
		return 0;

	/* Prefer the device node, even after an earlier remote fallback. */
	target = nr_cpu_ids;
	if (nid != NUMA_NO_NODE) {
		for_each_cpu_and(target, cpumask_of_node(nid), cpu_online_mask) {
			if (target != cpu)
				break;
		}
	}
	if (target >= nr_cpu_ids)
		target = cpumask_any_but(cpu_online_mask, cpu);
	if (target >= nr_cpu_ids)
		return -EBUSY;

	/* Quiesce IRQ updates until affinity and perf context agree again. */
	disable_irq(cpm->irq);
	ret = irq_set_affinity(cpm->irq, cpumask_of(target));
	if (ret) {
		dev_err(cpm->dev, "failed to move IRQ to CPU%u: %d\n",
			target, ret);
		enable_irq(cpm->irq);
		return ret;
	}

	/* On affinity failure above, the original context remains untouched. */
	perf_pmu_migrate_context(&cpm->pmu, cpu, target);
	cpm->cpu = target;
	enable_irq(cpm->irq);

	return 0;
}

static int cpm_event_init(struct perf_event *event)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	u64 config = event->attr.config;
	struct hw_perf_event *hwc = &event->hw;
	u32 event_id = CPM_GET_EVENT(&event->attr);

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	if (is_sampling_event(event))
		return -EOPNOTSUPP;

	/* Instances without a selector resource only support the default domain. */
	if (!cpm->sysreg_base && CPM_GET_SEL(&event->attr))
		return -EINVAL;

	/* config1 bit 63 is reserved. */
	if (event->attr.config1 & BIT_ULL(63))
		return -EINVAL;

	/* config2 only carries the 48-bit upper address filter. */
	if (event->attr.config2 & GENMASK_ULL(63, 48))
		return -EINVAL;

	event->cpu = cpm->cpu;

	hwc->config = config;
	hwc->idx = -1;

	if (event_id < 0x100) {
		/* The hardware monitor event selector is six bits wide. */
		if (event_id > 0x3f)
			return -EINVAL;
		hwc->event_base = CPM_EVENT_MONITOR;
	} else if ((event_id >= 0x100 && event_id <= 0x103) ||
		   (event_id >= 0x110 && event_id <= 0x113)) {
		/* Two OSTD directions, each with avg/real/min/max fields. */
		hwc->event_base = CPM_OSTD_STAT;
	} else if (event_id >= 0x200 && event_id <= 0x217) {
		/* Six LCRD channels, each with four statistics. */
		hwc->event_base = CPM_LCRD_STAT;
	} else {
		return -EINVAL;
	}

	return 0;
}

static void cpm_hw_config_filters(struct cpm_pmu *cpm, int idx, struct perf_event *event)
{
	struct perf_event_attr *attr = &event->attr;
	void __iomem *base = cpm->base;
	u64 addr_lo, addr_hi;
	u32 flt4, flt5, flt6, flt7;

	/* FLT0/FLT1: address range lower bound (48-bit addr_lo) */
	addr_lo = CPM_GET_ADDR_LO(attr);

	writel(lower_32_bits(addr_lo), base + CPM_EVTx_FLT0(idx));
	writel(lower_16_bits(upper_32_bits(addr_lo)), base + CPM_EVTx_FLT1(idx));

	/* FLT2/FLT3: address range upper bound (48-bit addr_hi) */
	addr_hi = CPM_GET_ADDR_HI(attr);

	writel(lower_32_bits(addr_hi), base + CPM_EVTx_FLT2(idx));
	writel(lower_16_bits(upper_32_bits(addr_hi)), base + CPM_EVTx_FLT3(idx));

	/* FLT4: opcode filter + address alignment */
	flt4 = (CPM_GET_OPCODE_MASK(attr) << 24)
		 | (CPM_GET_OPCODE_TGT(attr) << 16)
		 | CPM_GET_FLT_ALIGN(attr);
	writel(flt4, base + CPM_EVTx_FLT4(idx));

	/* FLT5: srcid filter */
	flt5 = (CPM_GET_SRCID_MASK(attr) << 16)
		 | CPM_GET_SRCID_TGT(attr);
	writel(flt5, base + CPM_EVTx_FLT5(idx));

	/* FLT6: tgtid filter */
	flt6 = (CPM_GET_TGTID_MASK(attr) << 16)
		 | CPM_GET_TGTID_TGT(attr);
	writel(flt6, base + CPM_EVTx_FLT6(idx));

	/* FLT7: partid filter — [HW Bug] partid_tgt must be shifted left by 1 */
	flt7 = (CPM_GET_PARTID_MASK(attr) << 16)
		 | (CPM_GET_PARTID_TGT(attr) << 1);
	writel(flt7, base + CPM_EVTx_FLT7(idx));
}

/*
 * Read one OSTD field (16-bit) from STT_OSTD0–OSTD3.
 *
 * event_id encoding: 0x1[group][sub]
 *   group (bit 4): 0 = rd,  1 = wr
 *   sub (bit 1:0): 0 = avg, 1 = real, 2 = min, 3 = max
 *
 * Register layout (each field 16 bits):
 *   OSTD0 (0x10): rd_avg[31:16]  rd_real[15:0]
 *   OSTD1 (0x14): rd_min[31:16]  rd_max[15:0]
 *   OSTD2 (0x18): wr_avg[31:16]  wr_real[15:0]
 *   OSTD3 (0x1C): wr_min[31:16]  wr_max[15:0]
 */
static u32 cpm_read_ostd(struct cpm_pmu *cpm, u32 event_id)
{
	u32 group = (event_id >> 4) & 0x1;       /* 0=rd, 1=wr */
	u32 sub   = event_id & 0x3;              /* 0=avg, 1=real, 2=min, 3=max */
	u32 reg   = CPM_STT_OSTD0 + group * 8 + (sub / 2) * 4;
	u32 val   = readl(cpm->base + reg);

	return (sub & 1) ? (val & 0xFFFF) : (val >> 16);
}

/*
 * Read one LCRD field (4-bit) from STT_LCRD0–LCRD2.
 *
 * event_id encoding: 0x2[offset], offset 0x00–0x17
 *   Every 8 offsets → one 32-bit register, each containing 2 channels × 4 stats.
 *   Field order within register (MSB first):
 *     [31:28] avg  [27:24] min  [23:20] max  [19:16] real  ← upper channel
 *     [15:12] avg  [11:8]  min  [7:4]   max  [3:0]   real  ← lower channel
 *
 * Register layout:
 *   LCRD0 (0x20): txdat[31:16] + txreq[15:0]
 *   LCRD1 (0x24): rxdat[31:16] + txrsp[15:0]
 *   LCRD2 (0x28): rxsnp[31:16] + rxreq[15:0]
 */
static u32 cpm_read_lcrd(struct cpm_pmu *cpm, u32 event_id)
{
	u32 off   = event_id & 0xFF;             /* 0x00–0x17 */
	u32 reg   = CPM_STT_LCRD0 + (off / 8) * 4;
	u32 shift = (7 - (off % 8)) * 4;
	u32 val   = readl(cpm->base + reg);

	return (val >> shift) & 0xF;
}

static enum cpm_count_mode cpm_update_mode(u32 event_id)
{
	/* EVENT_MONITOR: 0x00-0x3F */
	if (event_id < 0x100) {
		switch (event_id) {
		case 0x28: case 0x29: return CPM_COUNT_MAX;   /* rd/wr_max_lat */
		case 0x30: case 0x31: return CPM_COUNT_MIN;   /* rd/wr_min_lat */
		case 0x20: case 0x21: return CPM_COUNT_AVG;   /* rd/wr_avg_lat */
		default:              return CPM_COUNT_ACCUM;  /* trans/retry/timeout */
		}
	}
	/* OSTD: sub encoding — 0=avg, 1=real, 2=min, 3=max */
	if (event_id < 0x200) {
		switch (event_id & 0x3) {
		case 3:  return CPM_COUNT_MAX;
		case 2:  return CPM_COUNT_MIN;
		case 0:  return CPM_COUNT_AVG;
		default: return CPM_COUNT_SET;
		}
	}
	/* LCRD: sub encoding — 0=avg, 1=min, 2=max, 3=real */
	switch (event_id & 0x3) {
	case 2:  return CPM_COUNT_MAX;
	case 1:  return CPM_COUNT_MIN;
	case 0:  return CPM_COUNT_AVG;
	default: return CPM_COUNT_SET;
	}
}

static void cpm_count_update(struct perf_event *event, u64 val,
			     enum cpm_count_mode mode)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	switch (mode) {
	case CPM_COUNT_ACCUM:
		local64_add(val, &event->count);
		break;
	case CPM_COUNT_MAX: {
		u64 cur = local64_read(&event->count);

		if (val > cur)
			local64_set(&event->count, val);
		break;
	}
	case CPM_COUNT_MIN: {
		u64 cur = local64_read(&event->count);

		if (cur == 0 || val < cur)
			local64_set(&event->count, val);
		break;
	}
	case CPM_COUNT_AVG: {
		int idx = hwc->idx;

		if (idx < 0 || idx >= CPM_MAX_STAT_EVENTS)
			break;
		cpm->avg_sum[idx] += val;
		cpm->avg_nr[idx]++;
		local64_set(&event->count, cpm->avg_sum[idx] / cpm->avg_nr[idx]);
		break;
	}
	default: /* CPM_COUNT_SET */
		local64_set(&event->count, val);
		break;
	}
}

static void cpm_event_update(struct perf_event *event)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 event_id = CPM_GET_EVENT(&event->attr);
	enum cpm_count_mode mode = cpm_update_mode(event_id);
	u64 val;

	if (hwc->event_base == CPM_OSTD_STAT) {
		val = cpm_read_ostd(cpm, event_id);
	} else if (hwc->event_base == CPM_LCRD_STAT) {
		val = cpm_read_lcrd(cpm, event_id);
	} else {
		val = readl(cpm->base + CPM_EVT_CNT(hwc->idx));
		if (cpm_is_avg_latency_event(event_id) && val == 0xFFFFFFFF)
			return;
	}

	if (mode == CPM_COUNT_AVG && val == 0)
		return;
	/*
	 * Known IP erratum: OSTD/LCRD min can be wrong when the live value
	 * reaches zero (even avg < min). These minima are not reliable.
	 * Keep the existing zero-sample policy for now: accepting zero or
	 * changing the software sentinel cannot repair the hardware result.
	 * This erratum does not establish the validity of zero latency samples;
	 * their no-transaction encoding needs separate hardware confirmation.
	 */
	if (mode == CPM_COUNT_MIN && val == 0)
		return;

	cpm_count_update(event, val, mode);
}

static void cpm_hw_start(struct cpm_pmu *cpm)
{
	if (cpm->sysreg_base)
		writel(cpm->cpm_sel, cpm->sysreg_base);

	/* CSR0: release active-low reset and enable window-done interrupts. */
	writel(CPM_SWRST_N | CPM_MON_DN_INT_EN, cpm->base + CPM_CSR0);
	writel(cpm->mon_period, cpm->base + CPM_CSR2);
	writel(CPM_CONTINUOUS_SW, cpm->base + CPM_CSR1);
	writel(CPM_SWRST_N | CPM_MON_DN_INT_EN | CPM_MON_EN,
	       cpm->base + CPM_CSR0);
	/* CSR1 trigger_sw starts monitoring and is cleared by hardware. */
	writel(CPM_TRIGGER_SW, cpm->base + CPM_CSR1);

	cpm->hw_started = true;
}

static void cpm_pmu_enable(struct pmu *pmu)
{
	struct cpm_pmu *cpm = to_cpm_pmu(pmu);
	int idx;

	if (cpm->shutting_down || cpm->hw_started || !cpm->active_events)
		return;

	/*
	 * The default ADD transaction and perf's outer disable/enable nesting
	 * keep this callback after the current batch of add/start calls.
	 * Start once, with all slots configured, including independent groups.
	 * active_events counts installed events; add() need not request START.
	 */
	for (idx = 0; idx < CPM_MAX_STAT_EVENTS; idx++) {
		struct perf_event *event = cpm->events[idx];

		if (event && !(event->hw.state & PERF_HES_STOPPED)) {
			cpm_hw_start(cpm);
			break;
		}
	}
}

/*
 * Used after the last event is removed or during shutdown. Leave interrupts
 * disabled; the next add/start sequence programs the hardware again.
 * There is no reset-completion status to poll.
 */
static void cpm_hw_reset(struct cpm_pmu *cpm)
{
	writel(CPM_SWRST_N, cpm->base + CPM_CSR0);
	readl(cpm->base + CPM_CSR0);
	udelay(CPM_RESET_DELAY_US);
	writel(0, cpm->base + CPM_CSR0);
	readl(cpm->base + CPM_CSR0);
	udelay(CPM_RESET_DELAY_US);
	writel(CPM_SWRST_N, cpm->base + CPM_CSR0);
	readl(cpm->base + CPM_CSR0);
	udelay(CPM_RESET_DELAY_US);
}

static void cpm_hw_stop(struct cpm_pmu *cpm)
{
	u32 csr0;
	int idx, ret;

	disable_irq_nosync(cpm->irq);

	/*
	 * Discard any old completion in the same write that requests stop.
	 * Only the final window may satisfy the completion poll below.
	 * Clearing MON_EN requests stop; wait for hardware to quiesce before
	 * clearing slot controls or allowing another configuration. The final
	 * window is discarded because it can include traffic after stop.
	 */
	csr0 = readl(cpm->base + CPM_CSR0);
	csr0 &= ~(CPM_MON_EN | CPM_INT_MASK);
	writel(csr0 | CPM_MON_DN_INT, cpm->base + CPM_CSR0);

	ret = readl_poll_timeout_atomic(cpm->base + CPM_CSR0, csr0,
					csr0 & CPM_MON_DN_INT, 10,
					cpm->stop_timeout_us);
	if (ret) {
		/* Discard the incomplete window instead of accepting its counts. */
		cpm_hw_reset(cpm);
		dev_err_ratelimited(cpm->dev,
				    "stop timeout (%llu us, period=%u, CSR0=%#x); window discarded, reset issued\n",
				    cpm->stop_timeout_us, cpm->mon_period, csr0);
		goto out;
	}

	/* Acknowledge observed interrupts and keep monitoring disabled. */
	writel(csr0 & ~CPM_MON_EN, cpm->base + CPM_CSR0);

out:
	/* Hardware is stopped (or was reset); slot controls may be cleared. */
	for (idx = 0; idx < CPM_MAX_COUNTERS; idx++)
		writel(0, cpm->base + CPM_EVTx_CTL1(idx));
	readl(cpm->base + CPM_EVTx_CTL1(CPM_MAX_COUNTERS - 1));
	cpm->hw_started = false;
	enable_irq(cpm->irq);
}

static void cpm_event_start(struct perf_event *event, int flags)
{
	/*
	 * Mark this event runnable. Starting here would assert mon_en before
	 * later members of the same scheduling batch have been configured.
	 */
	event->hw.state = 0;
}

static void cpm_event_stop(struct perf_event *event, int flags)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	/*
	 * Only real statistics can be sampled independently of mon_dn.
	 * Window results cannot be truncated at the asynchronous stop time,
	 * even with PERF_EF_UPDATE. Freeze them at the last IRQ update and
	 * let the ISR skip this stopped event to avoid counting later traffic.
	 * A completed window whose IRQ is still pending can also be lost.
	 * Capture real statistics before del() removes the event.
	 */
	if (cpm->hw_started && cpm_is_realtime_stat(event))
		cpm_event_update(event);

	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static int cpm_event_add(struct perf_event *event, int flags)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 event_id = CPM_GET_EVENT(&event->attr);
	u32 sel = CPM_GET_SEL(&event->attr);
	u32 ctl1;
	int idx;

	if (cpm->shutting_down)
		return -ENODEV;

	/*
	 * A pinned event may survive a flexible reschedule. Check hardware
	 * state, not attr.pinned: no new configuration is safe while running.
	 * Reject before allocating a slot or writing any control register.
	 * The IP forbids changing control signals while mon_en is asserted.
	 * Stopping the whole CPM to admit a new event would interrupt existing
	 * measurements, including pinned events. Configure each idle batch
	 * first; pmu_enable() starts it only after all add/start calls finish.
	 * This is not a one-process restriction: flexible rescheduling may
	 * remove the old set and admit events from multiple sessions together.
	 * Retained events (for example pinned ones) keep hardware running and
	 * therefore prevent new configuration.
	 */
	if (cpm->hw_started) {
		dev_warn_ratelimited(cpm->dev,
				     "CPM is running; cannot add events until all active events are removed\n");
		return -EBUSY;
	}

	/* cpm_sel is shared by all events on this CPM instance. */
	if (cpm->active_events == 0) {
		cpm->cpm_sel = sel;
	} else if (cpm->cpm_sel != sel) {
		dev_err(cpm->dev,
			"All events must use the same cpm_sel (cpu=0 or tpe=1)\n");
		return -EINVAL;
	}

	switch (hwc->event_base) {
	case CPM_OSTD_STAT:
	case CPM_LCRD_STAT:
		for (idx = CPM_MAX_COUNTERS; idx < CPM_MAX_STAT_EVENTS; idx++)
			if (!cpm->events[idx])
				break;
		if (idx == CPM_MAX_STAT_EVENTS)
			return -ENOSPC;
		break;
	case CPM_EVENT_MONITOR:
		idx = find_first_zero_bit(cpm->used_mask, CPM_MAX_COUNTERS);
		if (idx >= CPM_MAX_COUNTERS)
			return -ENOSPC;

		set_bit(idx, cpm->used_mask);

		ctl1 = (CPM_GET_FILTER_EN(&event->attr) << 16)
			| ((event_id & 0x3F) << 8)
			| CPM_EVT_EN;
		writel(ctl1, cpm->base + CPM_EVTx_CTL1(idx));

		writel(CPM_DEFAULT_EVT_DURATION, cpm->base + CPM_EVTx_CTL2(idx));

		if (CPM_GET_FILTER_EN(&event->attr))
			cpm_hw_config_filters(cpm, idx, event);
		break;
	default:
		return -EINVAL;
	}

	hwc->idx = idx;
	cpm->avg_sum[idx] = 0;
	cpm->avg_nr[idx] = 0;
	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	/* Perf callbacks and the affine IRQ serialize on the management CPU. */
	cpm->events[idx] = event;
	cpm->active_events++;

	if (flags & PERF_EF_START)
		cpm_event_start(event, PERF_EF_START);

	return 0;
}

static void cpm_event_del(struct perf_event *event, int flags)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);
	int idx = event->hw.idx;

	cpm_event_stop(event, PERF_EF_UPDATE);

	if (idx >= 0) {
		cpm->events[idx] = NULL;
		if (idx < CPM_MAX_COUNTERS) {
			/* Running slots are cleared only after the global stop. */
			if (!cpm->hw_started) {
				writel(0, cpm->base + CPM_EVTx_CTL1(idx));
				readl(cpm->base + CPM_EVTx_CTL1(idx));
			}
			/* add() rejects reuse for as long as CPM is running. */
			clear_bit(idx, cpm->used_mask);
		}
	}

	cpm->active_events--;

	if (cpm->active_events == 0 && cpm->hw_started)
		cpm_hw_stop(cpm);

	event->hw.idx = -1;
}

static void cpm_pmu_disable(struct pmu *pmu)
{
	/*
	 * Keep an existing measurement running across perf scheduling calls.
	 * add() rejects configuration changes until the last del() stops CPM.
	 * For an idle CPM, pmu_enable() starts the completed add/start batch.
	 * Keep the default perf transactions; a single group transaction does
	 * not necessarily cover the whole scheduling batch.
	 */
}

static irqreturn_t cpm_irq_handler(int irq, void *data)
{
	struct cpm_pmu *cpm = data;
	u32 status = readl(cpm->base + CPM_CSR0);
	int idx;

	if (!(status & CPM_INT_MASK))
		return IRQ_NONE;

	if (status & CPM_MON_DN_INT) {
		for (idx = 0; idx < CPM_MAX_STAT_EVENTS; idx++) {
			struct perf_event *event = cpm->events[idx];

			if (!event || (event->hw.state & PERF_HES_STOPPED) ||
			    cpm_is_realtime_stat(event))
				continue;
			cpm_event_update(event);
		}
	}

	/* clear interrupt */
	writel(status, cpm->base + CPM_CSR0);

	return IRQ_HANDLED;
}

static void cpm_event_read(struct perf_event *event)
{
	struct cpm_pmu *cpm = to_cpm_pmu(event->pmu);

	/* Window statistics are maintained by the mon_dn ISR. */
	if (!cpm_is_realtime_stat(event) || !cpm->hw_started ||
	    (event->hw.state & PERF_HES_STOPPED))
		return;

	cpm_event_update(event);
}

static int cpm_pmu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cpm_pmu *cpm;
	struct resource *res, *sysreg_res;
	const char *cpm_name;
	char *name;
	u32 clk_hz;
	u64 window_us;
	int ret;

	cpm = devm_kzalloc(dev, sizeof(*cpm), GFP_KERNEL);
	if (!cpm)
		return -ENOMEM;
	cpm->dev = dev;

	/* Optional DT / ACPI _DSD boolean; absent means production silicon. */
	if (device_property_read_bool(dev, "xuange,fpga")) {
		clk_hz = CPM_FPGA_CLK_HZ;
		cpm->mon_period = CPM_FPGA_MON_PERIOD;
	} else {
		clk_hz = CPM_SOC_CLK_HZ;
		cpm->mon_period = CPM_SOC_MON_PERIOD;
	}
	/* CSR2 encodes the exponent in five bits. */
	if (cpm->mon_period > 31)
		return -EINVAL;

	window_us = DIV_ROUND_UP_ULL((1ULL << cpm->mon_period) * USEC_PER_SEC,
				     clk_hz);
	cpm->stop_timeout_us = 2 * window_us + CPM_STOP_MARGIN_US;

	/* MMIO mapping: CPM event registers */
	cpm->base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(cpm->base))
		return PTR_ERR(cpm->base);

	/* MMIO mapping: CPM_SEL register (CPU_SS only, DDR_SS has no second reg) */
	sysreg_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);

	if (sysreg_res) {
		cpm->sysreg_base = devm_ioremap_resource(dev, sysreg_res);
		if (IS_ERR(cpm->sysreg_base))
			return PTR_ERR(cpm->sysreg_base);
	}

	/* read PMU name (works for both DTS of_property and ACPI _DSD) */
	ret = device_property_read_string(&pdev->dev, "xuange,cpm-name", &cpm_name);
	if (ret)
		name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "cpm_%llx",
				      (u64)(res->start >> 12));
	else
		name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "cpm_%s", cpm_name);
	if (!name)
		return -ENOMEM;

	/* Prefer an online management CPU local to the device NUMA node. */
	cpm->cpu = cpumask_local_spread(0, dev_to_node(dev));

	cpm->irq = platform_get_irq(pdev, 0);
	if (cpm->irq < 0)
		return cpm->irq;

	ret = devm_request_irq(dev, cpm->irq, cpm_irq_handler,
			       IRQF_NOBALANCING | IRQF_NO_THREAD,
			       dev_name(dev), cpm);
	if (ret)
		return ret;

	/* Serialize IRQ updates with perf callbacks on the management CPU. */
	ret = irq_set_affinity(cpm->irq, cpumask_of(cpm->cpu));
	if (ret)
		return dev_err_probe(dev, ret, "failed to set IRQ affinity\n");

	/* register instance to hotplug state (callback registered in module init) */
	ret = cpuhp_state_add_instance(cpm_cpuhp_state, &cpm->node);
	if (ret)
		return ret;

	cpm->pmu = (struct pmu) {
		.module = THIS_MODULE,
		.task_ctx_nr = perf_invalid_context,	/* uncore: no per-task support */
		.pmu_enable = cpm_pmu_enable,
		.pmu_disable = cpm_pmu_disable,
		.event_init = cpm_event_init,
		.add = cpm_event_add,
		.del = cpm_event_del,
		.start = cpm_event_start,
		.stop = cpm_event_stop,
		.read = cpm_event_read,
		.attr_groups = cpm_pmu_attr_groups,
		.capabilities = PERF_PMU_CAP_NO_EXCLUDE,	/* no privilege level filtering */
	};

	ret = perf_pmu_register(&cpm->pmu, name, -1);
	if (ret) {
		dev_err(dev, "CPM PMU register failed: %d\n", ret);
		goto out_cpuhp;
	}

	platform_set_drvdata(pdev, cpm);

	dev_info(dev, "CPM PMU registered as %s\n", name);

	return 0;

out_cpuhp:
	cpuhp_state_remove_instance_nocalls(cpm_cpuhp_state, &cpm->node);
	return ret;
}

static void cpm_pmu_remove(struct platform_device *pdev)
{
	struct cpm_pmu *cpm = platform_get_drvdata(pdev);
	u32 csr0;

	cpuhp_state_remove_instance_nocalls(cpm_cpuhp_state, &cpm->node);
	perf_pmu_unregister(&cpm->pmu);

	/* Stop monitoring and mask interrupts without acknowledging W1C bits. */
	csr0 = readl(cpm->base + CPM_CSR0);
	csr0 &= ~(CPM_MON_EN | CPM_INT_EN_MASK | CPM_INT_MASK);
	writel(csr0, cpm->base + CPM_CSR0);
	readl(cpm->base + CPM_CSR0);

	/* Wait for any running handler before devres releases the PMU memory. */
	devm_free_irq(&pdev->dev, cpm->irq, cpm);
}

/*
 * Run on the management CPU with local interrupts disabled, like perf
 * callbacks. The caller has disabled and synchronized the CPM IRQ.
 */
static void cpm_shutdown_cpu(void *info)
{
	struct cpm_pmu *cpm = info;
	u32 csr0;

	cpm->shutting_down = true;
	if (cpm->hw_started)
		cpm_hw_stop(cpm);

	csr0 = readl(cpm->base + CPM_CSR0);
	csr0 &= ~(CPM_MON_EN | CPM_INT_EN_MASK | CPM_INT_MASK);
	writel(csr0 | CPM_INT_MASK, cpm->base + CPM_CSR0);
	readl(cpm->base + CPM_CSR0);
}

static void cpm_pmu_shutdown(struct platform_device *pdev)
{
	struct cpm_pmu *cpm = platform_get_drvdata(pdev);
	int ret;

	/* Keep the management CPU online and its affinity stable throughout. */
	cpus_read_lock();
	disable_irq(cpm->irq);
	ret = smp_call_function_single(cpm->cpu, cpm_shutdown_cpu, cpm, 1);
	if (ret)
		dev_err(cpm->dev, "failed to shut down CPM on CPU%d: %d\n",
			cpm->cpu, ret);
	/* Leave the IRQ disabled across the reboot or normal kexec handoff. */
	cpus_read_unlock();
}

static const struct of_device_id cpm_pmu_of_match[] = {
	{ .compatible = "xuange,cpm-pmu-v1" },
	{},
};
MODULE_DEVICE_TABLE(of, cpm_pmu_of_match);

static const struct acpi_device_id cpm_acpi_match[] = {
	{ "BABA5004", 0 },
	{},
};
MODULE_DEVICE_TABLE(acpi, cpm_acpi_match);

static struct platform_driver cpm_pmu_driver = {
	.probe		= cpm_pmu_probe,
	.remove_new	= cpm_pmu_remove,
	.shutdown	= cpm_pmu_shutdown,
	.driver		= {
		.name	= XUANGE_CPM_PDEV_NAME,
		/* Open perf events pin the module, but not a sysfs unbind. */
		.suppress_bind_attrs = true,
		.of_match_table = cpm_pmu_of_match,
		.acpi_match_table = ACPI_PTR(cpm_acpi_match),
	},
};

static int __init cpm_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "perf/xuange/cpm:online",
				      NULL, cpm_pmu_offline_cpu);
	if (ret < 0)
		return ret;
	cpm_cpuhp_state = ret;

	ret = platform_driver_register(&cpm_pmu_driver);
	if (ret)
		cpuhp_remove_multi_state(cpm_cpuhp_state);

	return ret;
}

static void __exit cpm_pmu_exit(void)
{
	platform_driver_unregister(&cpm_pmu_driver);
	cpuhp_remove_multi_state(cpm_cpuhp_state);
}

module_init(cpm_pmu_init);
module_exit(cpm_pmu_exit);

MODULE_AUTHOR("Dong Yan <yandong@linux.alibaba.com>");
MODULE_DESCRIPTION("Xuange CPM uncore performance monitor driver");
MODULE_LICENSE("GPL");
