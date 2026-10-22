// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 SiFive
 *
 * Authors
 *	Zong Li <zong.li@sifive.com>
 */

#include <linux/auxiliary_bus.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/io-64-nonatomic-hi-lo.h>
#include <linux/perf_event.h>

#include "../iommu/riscv/iommu.h"

/* 5.19 Performance monitoring counter overflow status (32bits) */
#define RISCV_IOMMU_REG_IOCOUNTOVF	0x0058
#define RISCV_IOMMU_IOCOUNTOVF_CY	BIT(0)
#define RISCV_IOMMU_IOCOUNTOVF_HPM	GENMASK_ULL(31, 1)

/* 5.20 Performance monitoring counter inhibits (32bits) */
#define RISCV_IOMMU_REG_IOCOUNTINH	0x005C
#define RISCV_IOMMU_IOCOUNTINH_CY	BIT(0)
#define RISCV_IOMMU_IOCOUNTINH_HPM	GENMASK(31, 0)

/* 5.21 Performance monitoring cycles counter (64bits) */
#define RISCV_IOMMU_REG_IOHPMCYCLES	0x0060
#define RISCV_IOMMU_IOHPMCYCLES_COUNTER	GENMASK_ULL(62, 0)
#define RISCV_IOMMU_IOHPMCYCLES_OF	BIT_ULL(63)
#define RISCV_IOMMU_REG_IOHPMCTR(_n)	(RISCV_IOMMU_REG_IOHPMCYCLES + ((_n) * 0x8))

/* 5.22 Performance monitoring event counters (31 * 64bits) */
#define RISCV_IOMMU_REG_IOHPMCTR_BASE	0x0068
#define RISCV_IOMMU_IOHPMCTR_COUNTER	GENMASK_ULL(63, 0)

/* 5.23 Performance monitoring event selectors (31 * 64bits) */
#define RISCV_IOMMU_REG_IOHPMEVT_BASE	0x0160
#define RISCV_IOMMU_REG_IOHPMEVT(_n)	(RISCV_IOMMU_REG_IOHPMEVT_BASE + ((_n) * 0x8))
#define RISCV_IOMMU_IOHPMEVT_EVENTID	GENMASK_ULL(14, 0)
#define RISCV_IOMMU_IOHPMEVT_DMASK	BIT_ULL(15)
#define RISCV_IOMMU_IOHPMEVT_PID_PSCID	GENMASK_ULL(35, 16)
#define RISCV_IOMMU_IOHPMEVT_DID_GSCID	GENMASK_ULL(59, 36)
#define RISCV_IOMMU_IOHPMEVT_PV_PSCV	BIT_ULL(60)
#define RISCV_IOMMU_IOHPMEVT_DV_GSCV	BIT_ULL(61)
#define RISCV_IOMMU_IOHPMEVT_IDT	BIT_ULL(62)
#define RISCV_IOMMU_IOHPMEVT_OF		BIT_ULL(63)
#define RISCV_IOMMU_IOHPMEVT_EVENT	GENMASK_ULL(62, 0)

/* The total number of counters is 31 event counters plus 1 cycle counter */
#define RISCV_IOMMU_HPM_COUNTER_NUM	32

/* Counter index 0 is the cycle counter, the event counters start at index 1 */
#define RISCV_IOMMU_HPM_CYCLE_IDX	0

static int cpuhp_state;

/**
 * enum riscv_iommu_hpmevent_id - Performance-monitoring event identifier
 *
 * @RISCV_IOMMU_HPMEVENT_CYCLE: Clock cycle counter
 * @RISCV_IOMMU_HPMEVENT_URQ: Untranslated requests
 * @RISCV_IOMMU_HPMEVENT_TRQ: Translated requests
 * @RISCV_IOMMU_HPMEVENT_ATS_RQ: ATS translation requests
 * @RISCV_IOMMU_HPMEVENT_TLB_MISS: TLB misses
 * @RISCV_IOMMU_HPMEVENT_DD_WALK: Device directory walks
 * @RISCV_IOMMU_HPMEVENT_PD_WALK: Process directory walks
 * @RISCV_IOMMU_HPMEVENT_S_VS_WALKS: First-stage page table walks
 * @RISCV_IOMMU_HPMEVENT_G_WALKS: Second-stage page table walks
 * @RISCV_IOMMU_HPMEVENT_MAX: Value to denote maximum Event IDs
 *
 * The specification does not define an event ID for counting the
 * number of clock cycles, meaning there is no associated 'iohpmevt0'.
 * Event ID 0 is an invalid event and does not overlap with any valid
 * event ID. Let's repurpose ID 0 as the cycle for perf, the cycle
 * event is not actually written into any register, it serves solely
 * as an identifier.
 */
enum riscv_iommu_hpmevent_id {
	RISCV_IOMMU_HPMEVENT_CYCLE	= 0,
	RISCV_IOMMU_HPMEVENT_URQ        = 1,
	RISCV_IOMMU_HPMEVENT_TRQ        = 2,
	RISCV_IOMMU_HPMEVENT_ATS_RQ     = 3,
	RISCV_IOMMU_HPMEVENT_TLB_MISS   = 4,
	RISCV_IOMMU_HPMEVENT_DD_WALK    = 5,
	RISCV_IOMMU_HPMEVENT_PD_WALK    = 6,
	RISCV_IOMMU_HPMEVENT_S_VS_WALKS = 7,
	RISCV_IOMMU_HPMEVENT_G_WALKS    = 8,
	RISCV_IOMMU_HPMEVENT_MAX        = 9
};

/*
 * Section 6.23 reserves event IDs 16384-32767 for custom events. IDs between
 * RISCV_IOMMU_HPMEVENT_MAX and this are reserved by the specification.
 */
#define RISCV_IOMMU_HPMEVENT_CUSTOM_FIRST	16384

struct riscv_iommu_pmu {
	struct pmu pmu;
	struct hlist_node node;
	void __iomem *reg;
	int on_cpu;
	unsigned int irq;
	int numa_node;
	unsigned int num_counters;
	/* Per-counter writable width; index 0 is IOHPMCYCLES */
	u64 cntr_mask[RISCV_IOMMU_HPM_COUNTER_NUM];
	/* IOCOUNTINH value which inhibits every implemented counter */
	u32 inhibit_all;
	struct perf_event *events[RISCV_IOMMU_HPM_COUNTER_NUM];
	/* Counter indices the hardware actually implements */
	DECLARE_BITMAP(implemented_counters, RISCV_IOMMU_HPM_COUNTER_NUM);
	DECLARE_BITMAP(used_counters, RISCV_IOMMU_HPM_COUNTER_NUM);
	/*
	 * Serialises used_counters/events[]/IOCOUNTINH for
	 * CPU hotplug race and IRQCHIP_MOVE_DEFERRED (PCI MSI/MSI-X on IMSIC)
	 */
	raw_spinlock_t lock;
};

#define to_riscv_iommu_pmu(p) (container_of(p, struct riscv_iommu_pmu, pmu))

#define RISCV_IOMMU_PMU_ATTR_EXTRACTOR(_name, _mask)			\
	static inline u32 get_##_name(struct perf_event *event)		\
	{								\
		return FIELD_GET(_mask, event->attr.config);		\
	}								\

RISCV_IOMMU_PMU_ATTR_EXTRACTOR(event, RISCV_IOMMU_IOHPMEVT_EVENTID);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(partial_matching, RISCV_IOMMU_IOHPMEVT_DMASK);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(pid_pscid, RISCV_IOMMU_IOHPMEVT_PID_PSCID);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(did_gscid, RISCV_IOMMU_IOHPMEVT_DID_GSCID);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(filter_pid_pscid, RISCV_IOMMU_IOHPMEVT_PV_PSCV);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(filter_did_gscid, RISCV_IOMMU_IOHPMEVT_DV_GSCV);
RISCV_IOMMU_PMU_ATTR_EXTRACTOR(filter_id_type, RISCV_IOMMU_IOHPMEVT_IDT);

/* Formats */
PMU_FORMAT_ATTR(event,            "config:0-14");
PMU_FORMAT_ATTR(partial_matching, "config:15");
PMU_FORMAT_ATTR(pid_pscid,        "config:16-35");
PMU_FORMAT_ATTR(did_gscid,        "config:36-59");
PMU_FORMAT_ATTR(filter_pid_pscid, "config:60");
PMU_FORMAT_ATTR(filter_did_gscid, "config:61");
PMU_FORMAT_ATTR(filter_id_type,   "config:62");

static struct attribute *riscv_iommu_pmu_formats[] = {
	&format_attr_event.attr,
	&format_attr_partial_matching.attr,
	&format_attr_pid_pscid.attr,
	&format_attr_did_gscid.attr,
	&format_attr_filter_pid_pscid.attr,
	&format_attr_filter_did_gscid.attr,
	&format_attr_filter_id_type.attr,
	NULL,
};

static const struct attribute_group riscv_iommu_pmu_format_group = {
	.name = "format",
	.attrs = riscv_iommu_pmu_formats,
};

/* Events */
static ssize_t riscv_iommu_pmu_event_show(struct device *dev,
					  struct device_attribute *attr,
					  char *page)
{
	struct perf_pmu_events_attr *pmu_attr;

	pmu_attr = container_of(attr, struct perf_pmu_events_attr, attr);

	return sysfs_emit(page, "event=0x%02llx\n", pmu_attr->id);
}

#define RISCV_IOMMU_PMU_EVENT_ATTR(name, id)			\
	PMU_EVENT_ATTR_ID(name, riscv_iommu_pmu_event_show, id)

static struct attribute *riscv_iommu_pmu_events[] = {
	RISCV_IOMMU_PMU_EVENT_ATTR(cycle, RISCV_IOMMU_HPMEVENT_CYCLE),
	RISCV_IOMMU_PMU_EVENT_ATTR(untranslated_req, RISCV_IOMMU_HPMEVENT_URQ),
	RISCV_IOMMU_PMU_EVENT_ATTR(translated_req, RISCV_IOMMU_HPMEVENT_TRQ),
	RISCV_IOMMU_PMU_EVENT_ATTR(ats_trans_req, RISCV_IOMMU_HPMEVENT_ATS_RQ),
	RISCV_IOMMU_PMU_EVENT_ATTR(tlb_miss, RISCV_IOMMU_HPMEVENT_TLB_MISS),
	RISCV_IOMMU_PMU_EVENT_ATTR(ddt_walks, RISCV_IOMMU_HPMEVENT_DD_WALK),
	RISCV_IOMMU_PMU_EVENT_ATTR(pdt_walks, RISCV_IOMMU_HPMEVENT_PD_WALK),
	RISCV_IOMMU_PMU_EVENT_ATTR(s_vs_pt_walks, RISCV_IOMMU_HPMEVENT_S_VS_WALKS),
	RISCV_IOMMU_PMU_EVENT_ATTR(g_pt_walks, RISCV_IOMMU_HPMEVENT_G_WALKS),
	NULL,
};

static const struct attribute_group riscv_iommu_pmu_events_group = {
	.name = "events",
	.attrs = riscv_iommu_pmu_events,
};

/* cpumask */
static ssize_t riscv_iommu_cpumask_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(dev_get_drvdata(dev));
	int on_cpu = pmu->on_cpu;

	/*
	 * riscv_iommu_pmu_offline_cpu() leaves on_cpu at -1 when it cannot
	 * find another online CPU to migrate to. Report an empty mask rather
	 * than feeding -1 to cpumask_of(), which indexes out of bounds.
	 */
	if (on_cpu < 0)
		return sysfs_emit(buf, "%*pbl\n", cpumask_pr_args(cpu_none_mask));

	return sysfs_emit(buf, "%*pbl\n", cpumask_pr_args(cpumask_of(on_cpu)));
}

static struct device_attribute riscv_iommu_cpumask_attr =
	__ATTR(cpumask, 0444, riscv_iommu_cpumask_show, NULL);

static struct attribute *riscv_iommu_cpumask_attrs[] = {
	&riscv_iommu_cpumask_attr.attr,
	NULL
};

static const struct attribute_group riscv_iommu_pmu_cpumask_group = {
	.attrs = riscv_iommu_cpumask_attrs,
};

static const struct attribute_group *riscv_iommu_pmu_attr_grps[] = {
	&riscv_iommu_pmu_cpumask_group,
	&riscv_iommu_pmu_format_group,
	&riscv_iommu_pmu_events_group,
	NULL,
};

/*
 * Register access wrapper
 *
 * According to RISC-V IOMMU specification Chapter 6:
 * A 4 byte access to an IOMMU register must be single-copy atomic.
 * Whether an 8 byte access to an IOMMU register is single-copy atomic is UNSPECIFIED
 *
 * Use two separate 4 byte accesses for hardware compatibility
 */
static u64 riscv_iommu_pmu_readq(void __iomem *addr)
{
	return hi_lo_readq(addr);
}

static void riscv_iommu_pmu_writeq(u64 value, void __iomem *addr)
{
	hi_lo_writeq(value, addr);
}

/* PMU Operations */
static void riscv_iommu_pmu_set_counter(struct riscv_iommu_pmu *pmu, u32 idx,
					u64 value)
{
	u64 counter_mask = pmu->cntr_mask[idx];

	riscv_iommu_pmu_writeq(value & counter_mask, pmu->reg + RISCV_IOMMU_REG_IOHPMCTR(idx));
}

/*
 * As stated in the RISC-V IOMMU Specification, Chapter 6:
 * Whether an 8 byte access to an IOMMU register is single-copy atomic
 * is UNSPECIFIED, and such an access may appear, internally to the
 * IOMMU, as if two separate 4 byte accesses - first to the high half
 * and second to the low half - were performed
 *
 * To make sure the driver works correctly on different hardware,
 * the software will always use two 4-byte access for the counter.
 *
 * This function implements the hi-lo-hi pattern to detect and handle
 * wraparound during the read operation:
 *   1. Read high half (hi)
 *   2. Read low half (lo)
 *   3. Read high half again (hi_again)
 *
 * If both reads of the high half agree, then the low half did not carry
 * into the high half in between, so the two halves belong together. If
 * they differ, the low half wrapped during the read and the whole
 * sequence is retried. A single re-read of the low half is not enough:
 * if the caller (or a hypervisor running it) is preempted for long enough,
 * the counter may wrap again before that re-read completes, pairing a
 * stale high half with a low half from yet another wrap. Retrying the
 * full hi/lo/hi sequence until two consecutive high-half reads agree
 * converges on a consistent pair regardless of how long the preemption
 * lasts.
 */
static u64 riscv_iommu_pmu_get_counter(struct riscv_iommu_pmu *pmu, u32 idx)
{
	void __iomem *addr = pmu->reg + RISCV_IOMMU_REG_IOHPMCTR(idx);
	u64 value, counter_mask = pmu->cntr_mask[idx];
	u32 hi, lo, hi_again;

	do {
		hi = readl(addr + 4);
		lo = readl(addr);
		hi_again = readl(addr + 4);
	} while (hi_again != hi);

	value = (((u64)hi << 32) | lo) & counter_mask;

	/* The bit 63 of cycle counter (i.e., idx == 0) is OF bit */
	return idx ? value : (value & ~RISCV_IOMMU_IOHPMCYCLES_OF);
}

static bool is_cycle_event(u64 event)
{
	return FIELD_GET(RISCV_IOMMU_IOHPMEVT_EVENTID, event) ==
	       RISCV_IOMMU_HPMEVENT_CYCLE;
}

static void riscv_iommu_pmu_set_event(struct riscv_iommu_pmu *pmu, u32 idx,
				      u64 value)
{
	/* There is no associated IOHPMEVT0 for IOHPMCYCLES */
	if (is_cycle_event(value))
		return;

	/* Event counter start from idx 1 */
	riscv_iommu_pmu_writeq(FIELD_GET(RISCV_IOMMU_IOHPMEVT_EVENT, value),
			       pmu->reg + RISCV_IOMMU_REG_IOHPMEVT(idx - 1));
}

static void riscv_iommu_pmu_enable_counter(struct riscv_iommu_pmu *pmu, u32 idx)
{
	void __iomem *addr = pmu->reg + RISCV_IOMMU_REG_IOCOUNTINH;
	u32 value = readl(addr);

	writel(value & ~BIT(idx), addr);
}

static void riscv_iommu_pmu_disable_counter(struct riscv_iommu_pmu *pmu, u32 idx)
{
	void __iomem *addr = pmu->reg + RISCV_IOMMU_REG_IOCOUNTINH;
	u32 value = readl(addr);

	writel(value | BIT(idx), addr);
}

static void riscv_iommu_pmu_clear_ovf(struct riscv_iommu_pmu *pmu, u32 idx)
{
	u64 value;

	/* Counter is disabled here, making it safe to read and write registers */
	if (idx == RISCV_IOMMU_HPM_CYCLE_IDX) {
		value = riscv_iommu_pmu_readq(pmu->reg + RISCV_IOMMU_REG_IOHPMCYCLES) &
					      ~RISCV_IOMMU_IOHPMCYCLES_OF;
		riscv_iommu_pmu_writeq(value, pmu->reg + RISCV_IOMMU_REG_IOHPMCYCLES);
	} else {
		/* Event counter start from idx 1 */
		value = riscv_iommu_pmu_readq(pmu->reg + RISCV_IOMMU_REG_IOHPMEVT(idx - 1)) &
					      ~RISCV_IOMMU_IOHPMEVT_OF;
		riscv_iommu_pmu_writeq(value, pmu->reg + RISCV_IOMMU_REG_IOHPMEVT(idx - 1));
	}
}

static void riscv_iommu_pmu_start_all(struct riscv_iommu_pmu *pmu, u32 inhibit)
{
	writel(inhibit, pmu->reg + RISCV_IOMMU_REG_IOCOUNTINH);
}

/* Returns the inhibit state prior to stopping, so callers can restore it later */
static u32 riscv_iommu_pmu_stop_all(struct riscv_iommu_pmu *pmu)
{
	void __iomem *addr = pmu->reg + RISCV_IOMMU_REG_IOCOUNTINH;
	u32 inhibit = readl(addr);

	/*
	 * Inhibit exactly the implemented counters. A truncated mask would
	 * clear the inhibit bit of an implemented but out-of-range counter,
	 * letting it run unprogrammed until it overflows.
	 */
	writel(pmu->inhibit_all, addr);

	return inhibit;
}

/* PMU APIs */
static void riscv_iommu_pmu_set_period(struct perf_event *event)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u64 counter_mask = pmu->cntr_mask[hwc->idx];
	u64 period;

	/*
	 * Limit the maximum period to prevent the counter value
	 * from overtaking the one we are about to program.
	 * In effect we are reducing max_period to account for
	 * interrupt latency (and we are being very conservative).
	 */
	period = counter_mask >> 1;

	/*
	 * Publish prev_count before resetting the hardware counter, not after.
	 *
	 * riscv_iommu_pmu_update() runs locklessly from ->read(), possibly on a
	 * different CPU than this (the PMU interrupt can be routed elsewhere,
	 * see IRQCHIP_MOVE_DEFERRED), and detects a concurrent reset by
	 * re-reading prev_count after it has sampled the hardware. Updating
	 * prev_count first means any reader which observes the reset hardware
	 * value must also observe the new prev_count on its re-check, so it
	 * retries. The reverse order leaves a window where the hardware has
	 * been reset but prev_count has not, which the re-check cannot see: the
	 * reader would pair a pre-reset prev with a post-reset sample and
	 * compute a huge bogus delta.
	 */
	local64_set(&hwc->prev_count, period);
	riscv_iommu_pmu_set_counter(pmu, hwc->idx, period);
}

/*
 * Tally @config against what the hardware implements: one cycle counter plus
 * pmu->num_counters - 1 event counters. Returns false once the group would need
 * more of either than exist, so that groups which could never be scheduled are
 * rejected in ->event_init() instead of failing with -EAGAIN in ->add() forever.
 */
static bool riscv_iommu_pmu_claim_counter(struct riscv_iommu_pmu *pmu, u64 config,
					  unsigned int *nr_cycles,
					  unsigned int *nr_events)
{
	if (is_cycle_event(config))
		return ++(*nr_cycles) <= 1;

	return ++(*nr_events) <= pmu->num_counters - 1;
}

/*
 * Section 6.23 Table 19 only defines IDT=1 for these events. For the others an
 * unsupported IDT setting means the counter does not increment at all.
 */
static bool riscv_iommu_pmu_idt_supported(u32 event_id)
{
	switch (event_id) {
	case RISCV_IOMMU_HPMEVENT_TLB_MISS:
	case RISCV_IOMMU_HPMEVENT_S_VS_WALKS:
	case RISCV_IOMMU_HPMEVENT_G_WALKS:
		return true;
	default:
		return false;
	}
}

static int riscv_iommu_pmu_event_init(struct perf_event *event)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	struct perf_event *sibling;
	unsigned int nr_cycles = 0;
	unsigned int nr_events = 0;
	int on_cpu;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (is_sampling_event(event))
		return -EOPNOTSUPP;

	if (event->cpu < 0)
		return -EOPNOTSUPP;

	/*
	 * Reject the range reserved between the standard events and the custom
	 * event space.
	 * Custom IDs are passed through: their meaning is implementation-defined,
	 * so this driver cannot validate them any further.
	 */
	if (get_event(event) >= RISCV_IOMMU_HPMEVENT_MAX &&
	    get_event(event) < RISCV_IOMMU_HPMEVENT_CUSTOM_FIRST)
		return -EINVAL;

	/*
	 * An IDT setting the event does not support makes the hardware simply
	 * not increment the counter, which is indistinguishable from an idle
	 * one. Reject it rather than silently reporting zero. Custom events are
	 * exempt: their IDT semantics are implementation-defined.
	 */
	if (get_filter_id_type(event) &&
	    get_event(event) < RISCV_IOMMU_HPMEVENT_MAX &&
	    !riscv_iommu_pmu_idt_supported(get_event(event)))
		return -EINVAL;

	/*
	 * There is no IOHPMEVT register associated with IOHPMCYCLES, so none
	 * of the filtering fields can be programmed for the cycle event.
	 * Reject them here instead of counting unfiltered cycles behind the
	 * user's back.
	 */
	if (is_cycle_event(event->attr.config) &&
	    (event->attr.config & ~RISCV_IOMMU_IOHPMEVT_EVENTID))
		return -EINVAL;

	/*
	 * All events are bound to the CPU the interrupt is affine to. That
	 * CPU is unset while no online CPU could be found for this PMU, and
	 * assigning -1 here would turn this into a task bound event, which is
	 * not something this PMU can serve.
	 */
	on_cpu = pmu->on_cpu;
	if (on_cpu < 0)
		return -ENODEV;

	event->cpu = on_cpu;

	hwc->idx = -1;
	hwc->config = event->attr.config;

	/*
	 * Account for this event itself first. It has to be done before the
	 * check below, otherwise an event which is on its own would never be
	 * matched against the number of counters the hardware implements.
	 */
	if (!riscv_iommu_pmu_claim_counter(pmu, event->attr.config,
					   &nr_cycles, &nr_events))
		return -EINVAL;

	/* On its own, so there is no group to validate */
	if (event->group_leader == event)
		return 0;

	/*
	 * Software events never occupy a hardware counter, so they do not have
	 * to sit on this pmu and do not consume any of its budget. Anything
	 * else in the group does, starting with the leader.
	 */
	if (!is_software_event(event->group_leader)) {
		/* A hardware leader has to share this pmu's counters */
		if (event->group_leader->pmu != event->pmu)
			return -EINVAL;

		if (!riscv_iommu_pmu_claim_counter(pmu,
						   event->group_leader->attr.config,
						   &nr_cycles, &nr_events))
			return -EINVAL;
	}

	/*
	 * Then the rest of the group. This walks group_leader->sibling_list,
	 * which does not contain the event being initialised yet - hence
	 * accounting for it separately above.
	 */
	for_each_sibling_event(sibling, event->group_leader) {
		if (is_software_event(sibling))
			continue;

		if (sibling->pmu != event->pmu)
			return -EINVAL;

		if (!riscv_iommu_pmu_claim_counter(pmu, sibling->attr.config,
						   &nr_cycles, &nr_events))
			return -EINVAL;
	}

	return 0;
}

static void riscv_iommu_pmu_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	u64 delta, prev, now;
	u32 idx = hwc->idx;
	u64 counter_mask = pmu->cntr_mask[idx];

	/*
	 * riscv_iommu_pmu_set_period() resets the hardware counter and
	 * prev_count as two separate writes, and deliberately updates
	 * prev_count first.
	 * Pairing a "prev" read from before that reset with a "now" read from
	 * after it would produce a nonsensical, possibly huge, delta. Because
	 * prev_count is published before the hardware is reset, a sample taken
	 * from after the reset is always accompanied by an updated prev_count,
	 * so re-checking prev_count after the hardware read detects that torn
	 * pairing and retries.
	 */
	do {
		prev = local64_read(&hwc->prev_count);
		now = riscv_iommu_pmu_get_counter(pmu, idx);
	} while (prev != local64_read(&hwc->prev_count) ||
		 local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	delta = (now - prev) & counter_mask;
	local64_add(delta, &event->count);
}

static void riscv_iommu_pmu_start(struct perf_event *event, int flags)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	unsigned long irqflags;

	if (WARN_ON_ONCE(!(event->hw.state & PERF_HES_STOPPED)))
		return;

	if (flags & PERF_EF_RELOAD)
		WARN_ON_ONCE(!(event->hw.state & PERF_HES_UPTODATE));

	raw_spin_lock_irqsave(&pmu->lock, irqflags);
	hwc->state = 0;
	riscv_iommu_pmu_set_period(event);
	riscv_iommu_pmu_set_event(pmu, hwc->idx, hwc->config);
	riscv_iommu_pmu_enable_counter(pmu, hwc->idx);
	raw_spin_unlock_irqrestore(&pmu->lock, irqflags);

	perf_event_update_userpage(event);
}

static void riscv_iommu_pmu_stop(struct perf_event *event, int flags)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	unsigned long irqflags;
	int idx = hwc->idx;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	raw_spin_lock_irqsave(&pmu->lock, irqflags);
	riscv_iommu_pmu_disable_counter(pmu, idx);
	if ((flags & PERF_EF_UPDATE) && !(hwc->state & PERF_HES_UPTODATE))
		riscv_iommu_pmu_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
	raw_spin_unlock_irqrestore(&pmu->lock, irqflags);
}

static int riscv_iommu_pmu_add(struct perf_event *event, int flags)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	unsigned long irqflags;
	unsigned int idx;

	raw_spin_lock_irqsave(&pmu->lock, irqflags);

	/* Reserve index zero for iohpmcycles */
	if (is_cycle_event(event->attr.config))
		idx = RISCV_IOMMU_HPM_CYCLE_IDX;
	else
		/*
		 * Pick a counter which is implemented and not already taken.
		 * The implemented set may be sparse, so the free index cannot be
		 * searched for within a consecutive range.
		 */
		idx = find_next_andnot_bit(pmu->implemented_counters,
					   pmu->used_counters,
					   RISCV_IOMMU_HPM_COUNTER_NUM,
					   RISCV_IOMMU_HPM_CYCLE_IDX + 1);

	/* All event counters or cycle counter are in use */
	if (idx >= RISCV_IOMMU_HPM_COUNTER_NUM || pmu->events[idx]) {
		raw_spin_unlock_irqrestore(&pmu->lock, irqflags);
		return -EAGAIN;
	}

	set_bit(idx, pmu->used_counters);

	pmu->events[idx] = event;
	hwc->idx = idx;
	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	local64_set(&hwc->prev_count, 0);

	raw_spin_unlock_irqrestore(&pmu->lock, irqflags);

	if (flags & PERF_EF_START)
		riscv_iommu_pmu_start(event, flags);

	/* Propagate changes to the userspace mapping. */
	perf_event_update_userpage(event);

	return 0;
}

static void riscv_iommu_pmu_read(struct perf_event *event)
{
	riscv_iommu_pmu_update(event);
}

static void riscv_iommu_pmu_del(struct perf_event *event, int flags)
{
	struct riscv_iommu_pmu *pmu = to_riscv_iommu_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	unsigned long irqflags;
	int idx = hwc->idx;

	riscv_iommu_pmu_stop(event, PERF_EF_UPDATE);

	raw_spin_lock_irqsave(&pmu->lock, irqflags);
	pmu->events[idx] = NULL;
	clear_bit(idx, pmu->used_counters);
	raw_spin_unlock_irqrestore(&pmu->lock, irqflags);

	perf_event_update_userpage(event);
}

/*
 * Pick the CPU perf assigns these events to: riscv_iommu_pmu_event_init()
 * sets event->cpu to pmu->on_cpu, and perf_pmu_migrate_context() moves
 * already-installed events to a new on_cpu when this changes. This has no
 * bearing on which CPU riscv_iommu_pmu_irq_handler() itself runs on -
 * pmu->lock serialises the two regardless of that, rather than requiring
 * them to run on the same CPU. Only consider CPUs which the irqchip
 * actually accepts - committing on_cpu to a CPU which irq_set_affinity()
 * then rejects would leave events bound to a CPU whose interrupt the
 * irqchip refuses to deliver.
 *
 * cpumask_local_spread() walks the online CPUs in order of NUMA distance from
 * the iommu, so the closest usable one wins.
 *
 * Returns the chosen CPU, or nr_cpu_ids if none could be used.
 */
static unsigned int riscv_iommu_pmu_bind_cpu(struct riscv_iommu_pmu *pmu,
					     unsigned int skip_cpu)
{
	unsigned int cpu, i;

	for (i = 0; i < num_online_cpus(); i++) {
		cpu = cpumask_local_spread(i, pmu->numa_node);
		if (cpu == skip_cpu)
			continue;
		if (!irq_set_affinity(pmu->irq, cpumask_of(cpu)))
			return cpu;
	}

	return nr_cpu_ids;
}

static int riscv_iommu_pmu_online_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct riscv_iommu_pmu *iommu_pmu;
	unsigned int target_cpu;

	iommu_pmu = hlist_entry_safe(node, struct riscv_iommu_pmu, node);

	if (READ_ONCE(iommu_pmu->on_cpu) != -1)
		return 0;

	target_cpu = riscv_iommu_pmu_bind_cpu(iommu_pmu, nr_cpu_ids);
	if (target_cpu >= nr_cpu_ids) {
		/* on_cpu stays unset, so a later callback tries again */
		pr_debug("failed to point irq %u at any online cpu\n",
			 iommu_pmu->irq);
		return 0;
	}

	WRITE_ONCE(iommu_pmu->on_cpu, target_cpu);

	return 0;
}

static int riscv_iommu_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct riscv_iommu_pmu *iommu_pmu;
	unsigned int target_cpu;

	iommu_pmu = hlist_entry_safe(node, struct riscv_iommu_pmu, node);

	if (READ_ONCE(iommu_pmu->on_cpu) != (int)cpu)
		return 0;

	/*
	 * The last online CPU cannot be taken offline, and this callback runs
	 * before __cpu_disable() clears the outgoing CPU from cpu_online_mask,
	 * so there is always another online CPU to move to.
	 *
	 * Should that ever fail, unset on_cpu rather than leaving it pointing at
	 * the CPU which is going away. That keeps events from being bound to a
	 * dead CPU and lets riscv_iommu_pmu_online_cpu() pick again once a CPU
	 * comes back.
	 */
	target_cpu = riscv_iommu_pmu_bind_cpu(iommu_pmu, cpu);
	if (WARN_ON_ONCE(target_cpu >= nr_cpu_ids)) {
		WRITE_ONCE(iommu_pmu->on_cpu, -1);
	} else {
		WRITE_ONCE(iommu_pmu->on_cpu, target_cpu);
		/*
		 * perf_pmu_migrate_context() runs ->del() on cpu and ->add()
		 * on target_cpu with a synchronize_rcu() gap in between.
		 * riscv_iommu_pmu_irq_handler() can run concurrently with
		 * either step, on whichever CPU the interrupt physically
		 * lands on - pmu->lock serialises it against them instead of
		 * racing.
		 */
		perf_pmu_migrate_context(&iommu_pmu->pmu, cpu, target_cpu);
	}

	return 0;
}

/*
 * pmu->lock serialises this against ->add()/->del()/->start()/->stop(),
 * which can run on a different CPU than this while
 * riscv_iommu_pmu_offline_cpu() is migrating events across a
 * perf_pmu_migrate_context() call. ->read() does not take the lock: it
 * only calls riscv_iommu_pmu_update(), whose retry loop already resolves
 * concurrency against the set_period() done here.
 */
static irqreturn_t riscv_iommu_pmu_irq_handler(int irq, void *dev_id)
{
	struct riscv_iommu_pmu *pmu = (struct riscv_iommu_pmu *)dev_id;
	DECLARE_BITMAP(ovf_bitmap, BITS_PER_TYPE(u64));
	unsigned long irqflags;
	u32 ovf, idx, inhibit;

	/* Check whether this interrupt is for PMU */
	if (!(readl_relaxed(pmu->reg + RISCV_IOMMU_REG_IPSR) & RISCV_IOMMU_IPSR_PMIP))
		return IRQ_NONE;

	raw_spin_lock_irqsave(&pmu->lock, irqflags);

	inhibit = riscv_iommu_pmu_stop_all(pmu);

	ovf = readl(pmu->reg + RISCV_IOMMU_REG_IOCOUNTOVF);
	if (ovf) {
		bitmap_from_u64(ovf_bitmap, ovf);

		for_each_set_bit(idx, ovf_bitmap, RISCV_IOMMU_HPM_COUNTER_NUM) {
			struct perf_event *event = pmu->events[idx];

			/*
			 * pmu->events[idx] only means the counter is allocated,
			 * not that it is counting. A counter which has not been
			 * started has no valid prev_count to compute a delta
			 * against, and one which has been stopped must not be
			 * reprogrammed here. The overflow bit still has to be
			 * cleared below in either case, including when the event
			 * was already removed by riscv_iommu_pmu_del(),
			 * otherwise the interrupt would stay pending forever.
			 */
			if (event && !(event->hw.state & PERF_HES_STOPPED)) {
				riscv_iommu_pmu_update(event);
				riscv_iommu_pmu_set_period(event);
			}

			riscv_iommu_pmu_clear_ovf(pmu, idx);
		}
	}

	/* Clear performance monitoring interrupt pending bit */
	writel_relaxed(RISCV_IOMMU_IPSR_PMIP, pmu->reg + RISCV_IOMMU_REG_IPSR);

	riscv_iommu_pmu_start_all(pmu, inhibit);

	raw_spin_unlock_irqrestore(&pmu->lock, irqflags);

	return IRQ_HANDLED;
}

static unsigned int riscv_iommu_pmu_get_irq_num(struct riscv_iommu_device *iommu)
{
	/* Reuse ICVEC.CIV mask for all interrupt vectors mapping */
	int vec = (iommu->icvec >> (RISCV_IOMMU_INTR_PM * 4)) & RISCV_IOMMU_ICVEC_CIV;

	return iommu->irqs[vec];
}

static int riscv_iommu_pmu_request_irq(struct auxiliary_device *auxdev,
				       struct riscv_iommu_device *iommu,
				       struct riscv_iommu_pmu *pmu)
{
	/*
	 * Bind the handler to the auxiliary device, which is the same devres
	 * scope that frees @pmu. Requesting it on the parent iommu device
	 * would keep the handler registered with a dangling dev_id once @pmu
	 * is freed, either on a later probe failure or on device removal.
	 *
	 * IRQF_SHARED is required because ICVEC maps the performance
	 * monitoring source onto one of the vectors the iommu driver already
	 * requested for its queues whenever fewer than RISCV_IOMMU_INTR_COUNT
	 * vectors are available. Both requesters have to agree on sharing, or
	 * this one is rejected with -EBUSY. IRQF_ONESHOT does not have to be
	 * matched by hand: devm_request_irq() adds IRQF_COND_ONESHOT, so this
	 * handler adopts whatever the first requester picked.
	 */
	return devm_request_irq(&auxdev->dev, pmu->irq, riscv_iommu_pmu_irq_handler,
				IRQF_SHARED | IRQF_NOBALANCING,
				dev_name(iommu->dev), pmu);
}

static void riscv_iommu_pmu_remove_cpuhp_instance(void *data)
{
	struct riscv_iommu_pmu *pmu = data;

	cpuhp_state_remove_instance_nocalls(cpuhp_state, &pmu->node);
}

static void riscv_iommu_pmu_do_unregister(void *data)
{
	struct riscv_iommu_pmu *pmu = data;

	perf_pmu_unregister(&pmu->pmu);
}

static int riscv_iommu_pmu_probe(struct auxiliary_device *auxdev,
				 const struct auxiliary_device_id *id)
{
	struct riscv_iommu_device *iommu_dev = dev_get_platdata(&auxdev->dev);
	struct riscv_iommu_pmu *iommu_pmu;
	void __iomem *addr;
	unsigned int idx;
	char *name;
	u32 inhibit;
	int ret;

	iommu_pmu = devm_kzalloc(&auxdev->dev, sizeof(*iommu_pmu), GFP_KERNEL);
	if (!iommu_pmu)
		return -ENOMEM;

	iommu_pmu->reg = iommu_dev->reg;

	raw_spin_lock_init(&iommu_pmu->lock);

	/*
	 * Which counters exist and how wide each one is are both
	 * hardware-implemented, detected by writing 1s and reading back which
	 * bits stuck.
	 *
	 * Section 6.22 defines each IOHPMCTR as an independent 64-bit WARL
	 * register, so neither the set of implemented counters nor their widths
	 * can be assumed uniform. Keep an implemented-counter bitmap and a
	 * per-counter mask rather than a count and one shared width: a
	 * consecutive index bound derived from a popcount would both hand out
	 * counters which do not exist and leave overflows of implemented
	 * counters above the bound unacknowledged, which makes pmip reassert
	 * forever.
	 *
	 * The per-counter masks are still assumed to be a contiguous run of
	 * bits starting at bit 0, which is what riscv_iommu_pmu_update() relies
	 * on when it masks the difference of two samples to handle wraparound.
	 *
	 * The specification requires a minimum of one programmable event
	 * counter besides the cycles counter when capabilities.HPM is 1, which
	 * is the condition under which this device is created. So a compliant
	 * implementation always provides at least two counters, and both
	 * IOHPMCYCLES and the first IOHPMCTR are always present. A readback
	 * which says otherwise is non-compliant hardware and is rejected rather
	 * than worked around.
	 */
	addr = iommu_pmu->reg + RISCV_IOMMU_REG_IOCOUNTINH;
	writel(RISCV_IOMMU_IOCOUNTINH_HPM, addr);
	inhibit = readl(addr);
	bitmap_from_arr32(iommu_pmu->implemented_counters, &inhibit,
			  RISCV_IOMMU_HPM_COUNTER_NUM);

	/* Bit 63 of IOHPMCYCLES is the OF bit, not part of the counter */
	addr = iommu_pmu->reg + RISCV_IOMMU_REG_IOHPMCYCLES;
	riscv_iommu_pmu_writeq(RISCV_IOMMU_IOHPMCYCLES_COUNTER, addr);
	iommu_pmu->cntr_mask[RISCV_IOMMU_HPM_CYCLE_IDX] =
		riscv_iommu_pmu_readq(addr) & RISCV_IOMMU_IOHPMCYCLES_COUNTER;
	if (!iommu_pmu->cntr_mask[RISCV_IOMMU_HPM_CYCLE_IDX]) {
		dev_err(&auxdev->dev, "cycles counter is not implemented\n");
		return -ENODEV;
	}

	/* Detect the width of each implemented event counter individually */
	idx = RISCV_IOMMU_HPM_CYCLE_IDX + 1;
	for_each_set_bit_from(idx, iommu_pmu->implemented_counters,
			      RISCV_IOMMU_HPM_COUNTER_NUM) {
		addr = iommu_pmu->reg + RISCV_IOMMU_REG_IOHPMCTR(idx);
		riscv_iommu_pmu_writeq(RISCV_IOMMU_IOHPMCTR_COUNTER, addr);
		iommu_pmu->cntr_mask[idx] = riscv_iommu_pmu_readq(addr);

		/* A counter with no writable bits is not really there */
		if (!iommu_pmu->cntr_mask[idx])
			clear_bit(idx, iommu_pmu->implemented_counters);
	}

	iommu_pmu->num_counters = bitmap_weight(iommu_pmu->implemented_counters,
						RISCV_IOMMU_HPM_COUNTER_NUM);
	if (iommu_pmu->num_counters < 2) {
		dev_err(&auxdev->dev, "hardware reports %u counter(s)\n",
			iommu_pmu->num_counters);
		return -ENODEV;
	}

	bitmap_to_arr32(&iommu_pmu->inhibit_all, iommu_pmu->implemented_counters,
			RISCV_IOMMU_HPM_COUNTER_NUM);

	iommu_pmu->pmu = (struct pmu) {
		.module		= THIS_MODULE,
		.parent		= &auxdev->dev,
		.task_ctx_nr	= perf_invalid_context,
		.event_init	= riscv_iommu_pmu_event_init,
		.add		= riscv_iommu_pmu_add,
		.del		= riscv_iommu_pmu_del,
		.start		= riscv_iommu_pmu_start,
		.stop		= riscv_iommu_pmu_stop,
		.read		= riscv_iommu_pmu_read,
		.attr_groups	= riscv_iommu_pmu_attr_grps,
		.capabilities	= PERF_PMU_CAP_NO_EXCLUDE,
	};

	auxiliary_set_drvdata(auxdev, iommu_pmu);

	name = devm_kasprintf(&auxdev->dev, GFP_KERNEL,
			      "riscv_iommu_pmu_%u", auxdev->id);
	if (!name) {
		dev_err(&auxdev->dev, "Failed to create name riscv_iommu_pmu_%u\n",
			auxdev->id);
		return -ENOMEM;
	}

	iommu_pmu->numa_node = dev_to_node(iommu_dev->dev);
	iommu_pmu->irq = riscv_iommu_pmu_get_irq_num(iommu_dev);

	ret = riscv_iommu_pmu_request_irq(auxdev, iommu_dev, iommu_pmu);
	if (ret) {
		dev_err(&auxdev->dev, "Failed to request irq %s: %d\n", name, ret);
		return ret;
	}

	/*
	 * Bind all events to the same cpu context to avoid race enabling.
	 * riscv_iommu_pmu_online_cpu() picks the CPU and sets the irq
	 * affinity for us once the instance is registered below.
	 */
	iommu_pmu->on_cpu = -1;

	ret = cpuhp_state_add_instance(cpuhp_state, &iommu_pmu->node);
	if (ret) {
		dev_err(&auxdev->dev, "Failed to register hotplug %s: %d\n", name, ret);
		return ret;
	}

	ret = perf_pmu_register(&iommu_pmu->pmu, name, -1);
	if (ret) {
		dev_err(&auxdev->dev, "Failed to register %s: %d\n", name, ret);
		cpuhp_state_remove_instance_nocalls(cpuhp_state, &iommu_pmu->node);
		return ret;
	}

	/*
	 * Plain devm_add_action(), not _or_reset(): on failure the action must
	 * not run before the cpuhp instance is gone. _or_reset() would call
	 * perf_pmu_unregister() immediately, freeing the per-CPU contexts while
	 * the instance is still on the global cpuhp list, so a concurrent CPU
	 * offline could reach perf_pmu_migrate_context() through
	 * riscv_iommu_pmu_offline_cpu() and walk freed memory. Unwind here
	 * instead, in the same order as the unbind path below.
	 */
	ret = devm_add_action(&auxdev->dev, riscv_iommu_pmu_do_unregister,
			      iommu_pmu);
	if (ret) {
		cpuhp_state_remove_instance_nocalls(cpuhp_state, &iommu_pmu->node);
		perf_pmu_unregister(&iommu_pmu->pmu);
		return ret;
	}

	/*
	 * Registered after do_unregister so it runs first (LIFO) on unbind:
	 * the cpuhp instance must be gone before perf_pmu_unregister() runs.
	 */
	ret = devm_add_action_or_reset(&auxdev->dev,
				       riscv_iommu_pmu_remove_cpuhp_instance,
				       iommu_pmu);
	if (ret)
		return ret;

	/*
	 * The PMU name only carries the aux dev id, not the iommu dev name, so
	 * find the iommu dev name here to map this PMU back to its iommu dev.
	 */
	dev_info(&auxdev->dev, "%s: Registered with %u counters (iommu %s)\n",
		 name, iommu_pmu->num_counters, dev_name(iommu_dev->dev));

	return 0;
}

static const struct auxiliary_device_id riscv_iommu_pmu_id_table[] = {
	{ .name = "riscv-iommu.pmu" },
	{}
};
MODULE_DEVICE_TABLE(auxiliary, riscv_iommu_pmu_id_table);

static struct auxiliary_driver iommu_pmu_driver = {
	.driver = {
		.suppress_bind_attrs = true,
	},
	.probe		= riscv_iommu_pmu_probe,
	.id_table	= riscv_iommu_pmu_id_table,
};

static int __init riscv_iommu_pmu_init(void)
{
	int ret;

	cpuhp_state = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
					      "perf/riscv/iommu:online",
					      riscv_iommu_pmu_online_cpu,
					      riscv_iommu_pmu_offline_cpu);
	if (cpuhp_state < 0)
		return cpuhp_state;

	ret = auxiliary_driver_register(&iommu_pmu_driver);
	if (ret)
		cpuhp_remove_multi_state(cpuhp_state);

	return ret;
}
module_init(riscv_iommu_pmu_init);

MODULE_DESCRIPTION("RISC-V IOMMU PMU");
MODULE_LICENSE("GPL");
