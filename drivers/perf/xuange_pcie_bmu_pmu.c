// SPDX-License-Identifier: GPL-2.0-only
/*
 * PCIe Bus Monitor Unit (BMU) Performance Monitor Driver
 *
 * Discovers BMU topology from PNP0A08 _DSD "bmu-entries" property
 * (UUID BABA0000-F4F6-4143-BF1C-E2446051D92C) and registers
 * per-instance PMU devices for performance monitoring.
 *
 * Firmware _DSD format (per entry):
 *   Package(4) { type, count, start_addr, total_length }
 * AXI entries require a fifth integer: the first instance ACPI GSI.
 * Instance i uses GSI base + i. Legacy four-element AXI entries are rejected.
 * Per-instance MMIO stride = total_length / count
 *
 * Hardware layouts retained in this driver:
 *   AXI (fw types 0,1,4): periodic counters, hardware IRQ collection
 *   A4S (fw type 3)      — message counters (free-run, no enable bit)
 *
 * A4S exposes four free-running message counters through perf
 * using software deltas. A4S filtered counters and channel latency min/max
 * are not exposed; AXI address handshake latency counters are supported.
 *
 * Copyright (C) 2024 Alibaba Inc
 * Author: Jiankang Chen <jkchen@linux.alibaba.com>
 */

#define pr_fmt(fmt) "xuange_pcie_bmu_pmu: " fmt

#include <linux/acpi.h>
#include <linux/bitfield.h>
#include <linux/cpuhotplug.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/hrtimer.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/overflow.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/string.h>

/* ================================================================
 * AXI BMU register offsets
 * ================================================================
 */
#define BMU_CFG0		0x00
#define BMU_CFG1		0x04
#define BMU_CFG3		0x0C
#define BMU_CFG4		0x10
#define BMU_CFG5		0x14
#define BMU_DURATION_FLT	0x28
#define BMU_RD_SAMPLE_MASK	(BIT(8) | GENMASK(2, 0))
#define BMU_WR_SAMPLE_MASK	(BIT(9) | GENMASK(6, 4))
#define BMU_RD_STS0		0x40	/* read duration */
#define BMU_RD_STS1		0x44	/* read transaction count */
#define BMU_RD_STS2		0x48	/* read bytes */
#define BMU_RD_STS3		0x4C	/* read over-threshold cmd */
#define BMU_WR_STS0		0x50	/* write duration */
#define BMU_WR_STS1		0x54	/* write transaction count */
#define BMU_WR_STS2		0x58	/* write bytes */
#define BMU_WR_STS3		0x5C	/* write over-threshold cmd */
#define BMU_VRD_STS0		0x60	/* variable read counter 0 */
#define BMU_VRD_STS1		0x64	/* variable read counter 1 */
#define BMU_VWR_STS0		0x70	/* variable write counter 0 */
#define BMU_VWR_STS1		0x74	/* variable write counter 1 */
#define BMU_VERSION1		0x8C
#define BMU_IR_STS0		0x80
#define BMU_IR_STS1		0x84
#define BMU_RD_STS4		0xA0	/* read cmd latency */
#define BMU_WR_STS4		0xA4	/* write cmd latency */

/* BMU_CFG0 bit fields */
#define BMU_CFG0_SW_RSTN	BIT(0)
#define BMU_CFG0_MON_EN		BIT(1)
#define BMU_CFG0_IRQ_PERIOD	BIT(4)
#define BMU_IRQ_PERIOD_EXPIRED	BIT(5)
#define BMU_IRQ_ERROR_MASK	GENMASK(14, 7)

/*
 * Fixed AXI defaults: period is in APB cycles, not wall-clock time.
 * Software must service each window before the next latch overwrites it.
 * Duration over-threshold tracking samples one in four read/write commands.
 */
#define BMU_AXI_PERIOD_CYCLES	BIT(18)
#define BMU_RD_SAMPLE_DIV4	BIT(1)
#define BMU_WR_SAMPLE_DIV4	BIT(5)

/* BMU_IR_STS0 bit fields */
#define BMU_IR_STS0_SW_INT_CLR	BIT(0)

/* ================================================================
 * A4S BMU register offsets
 * ================================================================
 */
#define A4S_BMU_RXA4S_TOTAL_CNT		0x10
#define A4S_BMU_TXA4S_TOTAL_CNT		0x14
#define A4S_BMU_RXA4S_TYPE12_CNT		0x18
#define A4S_BMU_TXA4S_TYPE12_CNT		0x1c

/* ================================================================
 * Event ID encoding and filter config
 * ================================================================
 */
#define BMU_EVENT_ID_MASK	GENMASK_ULL(7, 0)

/* Filter config: config1[2:0]=size_flt, config1[15:8]=len_flt,
 *                config1[19:16]=addr_aln, config1[27:24]=filter_en
 */
#define BMU_FLT_SIZE_MASK	GENMASK(2, 0)
#define BMU_FLT_LEN_MASK	GENMASK(15, 8)
#define BMU_FLT_ALN_MASK	GENMASK(19, 16)
#define BMU_FLT_EN_MASK		GENMASK(27, 24)

#define BMU_CONFIG_VALID_MASK	BMU_EVENT_ID_MASK
#define BMU_CONFIG1_VALID_MASK	(BMU_FLT_SIZE_MASK | BMU_FLT_LEN_MASK | \
				 BMU_FLT_ALN_MASK | BMU_FLT_EN_MASK)

/* Periodically extend the 32-bit A4S counters while events are active. */
#define BMU_POLL_PERIOD_NS	(2ULL * NSEC_PER_SEC)

/* Default duration threshold: 256 cycles */
#define BMU_DEFAULT_DURA_THRESH	256

/* ================================================================
 * Hardware types and firmware mapping
 * ================================================================
 */
#define BMU_HW_AXI	0
#define BMU_HW_A4S	2

/*
 * Firmware _DSD type -> hardware type mapping:
 *   fw type 0 (AXI dma)   -> HW AXI
 *   fw type 1 (AXI mmio)   -> HW AXI
 *   fw type 2             -> unsupported; keep firmware numbering stable
 *   fw type 3 (A4S)       -> HW A4S
 *   fw type 4 (AXI ltidu) -> HW AXI
 */
static const int fw_type_to_hw[] = {
	[0] = BMU_HW_AXI,
	[1] = BMU_HW_AXI,
	[2] = -1,
	[3] = BMU_HW_A4S,
	[4] = BMU_HW_AXI,
};

static const char * const fw_type_names[] = {
	[0] = "axi_dma",
	[1] = "axi_mmio",
	[2] = "unsupported",
	[3] = "a4s",
	[4] = "axi_ltidu",
};

#define FW_TYPE_COUNT	ARRAY_SIZE(fw_type_to_hw)

/* ================================================================
 * Data structures
 * ================================================================
 */
struct pcie_bmu_pmu {
	struct pmu pmu;
	void __iomem *base;
	struct device *dev;		/* parent platform device */
	int hw_type;			/* BMU_HW_AXI / BMU_HW_A4S */
	int fw_type;			/* firmware type (0-4) */
	int rc_index;			/* root complex index */
	char name[64];
	struct perf_event *events[16];	/* indexed by event_id */
	int num_events;			/* max events for hw_type */
	int num_active_events;
	int irq;
	u32 gsi;
	bool irq_running;
	u32 diag_seen_errors;
	u32 version1;
	bool skip_window[16];
	struct hrtimer hrtimer;
	u32 duration_threshold;		/* AXI only */
	spinlock_t events_lock;
	struct hlist_node cpuhp_node;
	int on_cpu;			/* single published management CPU */
	bool migrating;			/* protected by events_lock */
	bool shutting_down;		/* protected by events_lock */
	struct list_head node;
};

#define to_bmu_pmu(p)	container_of(p, struct pcie_bmu_pmu, pmu)

static LIST_HEAD(bmu_pmu_list);
static DEFINE_MUTEX(bmu_pmu_lock);

/* Multi-instance cpuhp state assigned at module init */
static enum cpuhp_state pcie_bmu_pmu_hp_state;

/* ================================================================
 * Event-to-register offset tables
 * ================================================================
 */

/* AXI v2: 14 events; IDs 10 and 13 remain reserved for ABI stability. */
static const u32 axi_event_regs[] = {
	[0]  = BMU_RD_STS1,	/* read_trans */
	[1]  = BMU_RD_STS2,	/* read_bytes */
	[2]  = BMU_RD_STS0,	/* read_duration */
	[3]  = BMU_RD_STS3,	/* read_over_thresh */
	[4]  = BMU_WR_STS1,	/* write_trans */
	[5]  = BMU_WR_STS2,	/* write_bytes */
	[6]  = BMU_WR_STS0,	/* write_duration */
	[7]  = BMU_WR_STS3,	/* write_over_thresh */
	[8]  = BMU_VRD_STS0,	/* var_read_0 */
	[9]  = BMU_VRD_STS1,	/* var_read_1 */
	[11] = BMU_VWR_STS0,	/* var_write_0 */
	[12] = BMU_VWR_STS1,	/* var_write_1 */
	[14] = BMU_RD_STS4,	/* read_latency */
	[15] = BMU_WR_STS4,	/* write_latency */
};

/* A4S: four unfiltered, free-running 32-bit counters. */
static const u32 a4s_event_regs[] = {
	[0] = A4S_BMU_RXA4S_TOTAL_CNT,		/* rxa4s_total */
	[1] = A4S_BMU_TXA4S_TOTAL_CNT,		/* txa4s_total */
	[2] = A4S_BMU_RXA4S_TYPE12_CNT,		/* rxa4s_type12 */
	[3] = A4S_BMU_TXA4S_TYPE12_CNT,		/* txa4s_type12 */
};

/* ================================================================
 * sysfs: events (per hardware type)
 * ================================================================
 */

PMU_EVENT_ATTR_STRING(read_trans,		axi_eattr_read_trans,		"event=0x00");
PMU_EVENT_ATTR_STRING(read_bytes,		axi_eattr_read_bytes,		"event=0x01");
PMU_EVENT_ATTR_STRING(read_duration,		axi_eattr_read_duration,	"event=0x02");
PMU_EVENT_ATTR_STRING(read_over_thresh,		axi_eattr_read_over_thresh,	"event=0x03");
PMU_EVENT_ATTR_STRING(write_trans,		axi_eattr_write_trans,		"event=0x04");
PMU_EVENT_ATTR_STRING(write_bytes,		axi_eattr_write_bytes,		"event=0x05");
PMU_EVENT_ATTR_STRING(write_duration,		axi_eattr_write_duration,	"event=0x06");
PMU_EVENT_ATTR_STRING(write_over_thresh,	axi_eattr_write_over_thresh,	"event=0x07");
PMU_EVENT_ATTR_STRING(var_read_0,		axi_eattr_var_read_0,		"event=0x08");
PMU_EVENT_ATTR_STRING(var_read_1,		axi_eattr_var_read_1,		"event=0x09");
PMU_EVENT_ATTR_STRING(var_write_0,		axi_eattr_var_write_0,		"event=0x0B");
PMU_EVENT_ATTR_STRING(var_write_1,		axi_eattr_var_write_1,		"event=0x0C");
PMU_EVENT_ATTR_STRING(read_latency,		axi_eattr_read_latency,		"event=0x0E");
PMU_EVENT_ATTR_STRING(write_latency,		axi_eattr_write_latency,	"event=0x0F");

static struct attribute *axi_pmu_event_attrs[] = {
	&axi_eattr_read_trans.attr.attr,
	&axi_eattr_read_bytes.attr.attr,
	&axi_eattr_read_duration.attr.attr,
	&axi_eattr_read_over_thresh.attr.attr,
	&axi_eattr_write_trans.attr.attr,
	&axi_eattr_write_bytes.attr.attr,
	&axi_eattr_write_duration.attr.attr,
	&axi_eattr_write_over_thresh.attr.attr,
	&axi_eattr_var_read_0.attr.attr,
	&axi_eattr_var_read_1.attr.attr,
	&axi_eattr_var_write_0.attr.attr,
	&axi_eattr_var_write_1.attr.attr,
	&axi_eattr_read_latency.attr.attr,
	&axi_eattr_write_latency.attr.attr,
	NULL,
};

PMU_EVENT_ATTR_STRING(rxa4s_total,		a4s_eattr_rxa4s_total,		"event=0x00");
PMU_EVENT_ATTR_STRING(txa4s_total,		a4s_eattr_txa4s_total,		"event=0x01");
PMU_EVENT_ATTR_STRING(rxa4s_type12,		a4s_eattr_rxa4s_type12,		"event=0x02");
PMU_EVENT_ATTR_STRING(txa4s_type12,		a4s_eattr_txa4s_type12,		"event=0x03");

static struct attribute *a4s_pmu_event_attrs[] = {
	&a4s_eattr_rxa4s_total.attr.attr,
	&a4s_eattr_txa4s_total.attr.attr,
	&a4s_eattr_rxa4s_type12.attr.attr,
	&a4s_eattr_txa4s_type12.attr.attr,
	NULL,
};

static umode_t bmu_axi_event_visible(struct kobject *kobj,
				     struct attribute *attr, int index)
{
	struct device *dev = kobj_to_dev(kobj);
	struct pmu *pmu = dev_get_drvdata(dev);
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(pmu);
	u32 count = (bmu->version1 >> 8) & 0xff;

	if ((!strcmp(attr->name, "var_read_0") ||
	     !strcmp(attr->name, "var_write_0")) && count < 1)
		return 0;
	if ((!strcmp(attr->name, "var_read_1") ||
	     !strcmp(attr->name, "var_write_1")) && count < 2)
		return 0;
	return attr->mode;
}

static const struct attribute_group axi_pmu_events_group = {
	.name = "events", .attrs = axi_pmu_event_attrs,
	.is_visible = bmu_axi_event_visible,
};

static const struct attribute_group a4s_pmu_events_group = {
	.name = "events", .attrs = a4s_pmu_event_attrs,
};

/* ================================================================
 * sysfs: format
 * ================================================================
 */

PMU_FORMAT_ATTR(event, "config:0-7");
PMU_FORMAT_ATTR(size_flt, "config1:0-2");
PMU_FORMAT_ATTR(len_flt, "config1:8-15");
PMU_FORMAT_ATTR(addr_aln, "config1:16-19");
PMU_FORMAT_ATTR(filter_en, "config1:24-27");

static struct attribute *pmu_format_attrs[] = {
	&format_attr_event.attr,
	NULL,
};

static struct attribute *axi_pmu_format_attrs[] = {
	&format_attr_event.attr,
	&format_attr_size_flt.attr,
	&format_attr_len_flt.attr,
	&format_attr_addr_aln.attr,
	&format_attr_filter_en.attr,
	NULL,
};

static const struct attribute_group pmu_format_group = {
	.name = "format", .attrs = pmu_format_attrs,
};

static const struct attribute_group axi_pmu_format_group = {
	.name = "format", .attrs = axi_pmu_format_attrs,
};

/* ================================================================
 * sysfs: cpumask and AXI duration_threshold
 * ================================================================
 */

/*
 * Find our bmu struct from the sysfs device pointer.
 * The perf core stores struct pmu in the sysfs device driver data.
 */
static struct pcie_bmu_pmu *dev_to_bmu(struct device *dev)
{
	struct pmu *pmu = dev_get_drvdata(dev);

	return pmu ? to_bmu_pmu(pmu) : NULL;
}

static ssize_t cpumask_show(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	struct pcie_bmu_pmu *bmu = dev_to_bmu(dev);
	int cpu;

	if (!bmu)
		return -ENODEV;
	cpu = READ_ONCE(bmu->on_cpu);
	if ((unsigned int)cpu >= nr_cpu_ids)
		return -ENODEV;
	return cpumap_print_to_pagebuf(true, buf, cpumask_of(cpu));
}
static DEVICE_ATTR_RO(cpumask);

static ssize_t duration_threshold_show(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct pcie_bmu_pmu *bmu = dev_to_bmu(dev);

	if (!bmu)
		return -ENODEV;
	return sysfs_emit(buf, "%u\n", READ_ONCE(bmu->duration_threshold));
}

static ssize_t duration_threshold_store(struct device *dev,
					struct device_attribute *attr,
				       const char *buf, size_t count)
{
	struct pcie_bmu_pmu *bmu = dev_to_bmu(dev);
	unsigned long flags;
	u32 val;
	int ret;

	if (!bmu)
		return -ENODEV;
	ret = kstrtou32(buf, 0, &val);
	if (ret)
		return ret;
	if (!val || val > U16_MAX)
		return -EINVAL;

	spin_lock_irqsave(&bmu->events_lock, flags);
	if (bmu->num_active_events) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return -EBUSY;
	}
	bmu->duration_threshold = val;
	spin_unlock_irqrestore(&bmu->events_lock, flags);
	return count;
}
static DEVICE_ATTR_RW(duration_threshold);

static struct attribute *pmu_cpumask_attrs[] = {
	&dev_attr_cpumask.attr,
	NULL,
};

static struct attribute *axi_pmu_extra_attrs[] = {
	&dev_attr_cpumask.attr,
	&dev_attr_duration_threshold.attr,
	NULL,
};

static const struct attribute_group pmu_cpumask_group = {
	.attrs = pmu_cpumask_attrs,
};

static const struct attribute_group axi_pmu_extra_group = {
	.attrs = axi_pmu_extra_attrs,
};

static const struct attribute_group *axi_pmu_attr_groups[] = {
	&axi_pmu_events_group,
	&axi_pmu_format_group,
	&axi_pmu_extra_group,
	NULL,
};

static const struct attribute_group *a4s_pmu_attr_groups[] = {
	&a4s_pmu_events_group,
	&pmu_format_group,
	&pmu_cpumask_group,
	NULL,
};

/* ================================================================
 * Variable counter filter (AXI only)
 * ================================================================
 */

static int bmu_var_counter_idx(u32 event_id)
{
	if (event_id < 8 || event_id > 12 || event_id == 10)
		return -1;
	return (event_id >= 11) ? (event_id - 11) : (event_id - 8);
}

static void bmu_clear_var_filter(struct pcie_bmu_pmu *bmu, int counter_idx)
{
	u32 cfg0, cfg4, cfg5;

	if (counter_idx < 0)
		return;

	cfg0 = readl(bmu->base + BMU_CFG0);
	cfg4 = readl(bmu->base + BMU_CFG4);
	cfg5 = readl(bmu->base + BMU_CFG5);

	cfg4 &= ~(BIT(counter_idx) | BIT(counter_idx + 4) |
		  BIT(counter_idx + 8) | BIT(counter_idx + 12));
	cfg4 &= ~(0x7u << (counter_idx * 3 + 16));
	cfg5 &= ~(0xFFu << (counter_idx * 8));

	if (counter_idx < 2)
		cfg0 &= ~(0xFu << (16 + counter_idx * 4));

	writel(cfg0, bmu->base + BMU_CFG0);
	writel(cfg4, bmu->base + BMU_CFG4);
	writel(cfg5, bmu->base + BMU_CFG5);
}

static void bmu_configure_var_filters(struct pcie_bmu_pmu *bmu,
				      struct perf_event *event)
{
	u64 config1 = event->attr.config1;
	u32 event_id = event->attr.config & BMU_EVENT_ID_MASK;
	u32 cfg4, cfg5, cfg0;
	u32 size_val, len_val, aln_val, en_val;
	int counter_idx;

	if (bmu->hw_type != BMU_HW_AXI)
		return;

	counter_idx = bmu_var_counter_idx(event_id);
	if (counter_idx < 0)
		return;

	bmu_clear_var_filter(bmu, counter_idx);

	if (!config1)
		return;

	cfg0 = readl(bmu->base + BMU_CFG0);
	cfg4 = readl(bmu->base + BMU_CFG4);
	cfg5 = readl(bmu->base + BMU_CFG5);

	size_val = FIELD_GET(BMU_FLT_SIZE_MASK, config1);
	len_val  = FIELD_GET(BMU_FLT_LEN_MASK,  config1);
	aln_val  = FIELD_GET(BMU_FLT_ALN_MASK,  config1);
	en_val   = FIELD_GET(BMU_FLT_EN_MASK,   config1);

	cfg4 |= (size_val << (counter_idx * 3 + 16));
	cfg5 |= (len_val  << (counter_idx * 8));
	if (counter_idx < 2)
		cfg0 |= (aln_val << (16 + counter_idx * 4));

	if (en_val & 0x1)
		cfg4 |= BIT(counter_idx);
	if (en_val & 0x2)
		cfg4 |= BIT(counter_idx + 4);
	if (en_val & 0x4)
		cfg4 |= BIT(counter_idx + 8);
	if (en_val & 0x8)
		cfg4 |= BIT(counter_idx + 12);

	writel(cfg0, bmu->base + BMU_CFG0);
	writel(cfg4, bmu->base + BMU_CFG4);
	writel(cfg5, bmu->base + BMU_CFG5);
}

/* ================================================================
 * Helper: register offset for event
 * ================================================================
 */

static int bmu_max_events(int hw_type)
{
	switch (hw_type) {
	case BMU_HW_A4S: return ARRAY_SIZE(a4s_event_regs);
	default:         return ARRAY_SIZE(axi_event_regs);
	}
}

/* ================================================================
 * A4S polling: accumulate modulo-32-bit deltas; no hardware IRQ
 * ================================================================
 */

static void bmu_a4s_update_locked(struct perf_event *event);

static enum hrtimer_restart bmu_hrtimer_handler(struct hrtimer *hr)
{
	struct pcie_bmu_pmu *bmu = container_of(hr, struct pcie_bmu_pmu, hrtimer);
	unsigned long flags;
	int i;

	spin_lock_irqsave(&bmu->events_lock, flags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return HRTIMER_NORESTART;
	}
	for (i = 0; i < bmu->num_events; i++) {
		if (bmu->events[i] &&
		    !(bmu->events[i]->hw.state & PERF_HES_STOPPED))
			bmu_a4s_update_locked(bmu->events[i]);
	}
	spin_unlock_irqrestore(&bmu->events_lock, flags);

	hrtimer_forward_now(hr, ns_to_ktime(BMU_POLL_PERIOD_NS));
	return HRTIMER_RESTART;
}

static void bmu_hrtimer_start(struct pcie_bmu_pmu *bmu)
{
	unsigned long flags;

	/* Serialize a delayed add() with shutdown's terminal-state update. */
	spin_lock_irqsave(&bmu->events_lock, flags);
	if (!bmu->shutting_down && bmu->num_active_events)
		hrtimer_start(&bmu->hrtimer, ns_to_ktime(BMU_POLL_PERIOD_NS),
			      HRTIMER_MODE_REL_PINNED);
	spin_unlock_irqrestore(&bmu->events_lock, flags);
}

static void bmu_hrtimer_stop(struct pcie_bmu_pmu *bmu)
{
	hrtimer_cancel(&bmu->hrtimer);
}

/* ================================================================
 * PMU ops
 * ================================================================
 */

static int bmu_pmu_event_init(struct perf_event *event)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long flags;
	u32 event_id;
	int cpu;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (is_sampling_event(event) || event->attach_state & PERF_ATTACH_TASK ||
	    event->cpu < 0)
		return -EOPNOTSUPP;

	/* Reject unadvertised config bits */
	if ((event->attr.config  & ~(u64)BMU_CONFIG_VALID_MASK) ||
	    (event->attr.config1 & ~(u64)BMU_CONFIG1_VALID_MASK))
		return -EINVAL;

	event_id = event->attr.config & BMU_EVENT_ID_MASK;
	if (event_id >= bmu_max_events(bmu->hw_type))
		return -EINVAL;

	/* Do not select a CPU while its events are being migrated. */
	spin_lock_irqsave(&bmu->events_lock, flags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return -ENODEV;
	}
	if (bmu->migrating) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return -EAGAIN;
	}
	cpu = READ_ONCE(bmu->on_cpu);
	spin_unlock_irqrestore(&bmu->events_lock, flags);
	if ((unsigned int)cpu >= nr_cpu_ids || !cpu_online(cpu))
		return -ENODEV;

	event->cpu = cpu;
	return 0;
}

/* A4S callbacks never write CTRL0 or clear the shared counters. */
static int bmu_a4s_init(struct perf_event *event)
{
	struct perf_event *leader = event->group_leader, *sibling;
	int ret;

	ret = bmu_pmu_event_init(event);
	if (ret)
		return ret;
	if (event->attr.config1 || event->attr.config2)
		return -EINVAL;

	/* Each fixed counter has one tracking slot in this PMU. */
	if (leader != event && leader->pmu == event->pmu &&
	    leader->attr.config == event->attr.config)
		return -EINVAL;
	for_each_sibling_event(sibling, leader) {
		if (sibling != event && sibling->pmu == event->pmu &&
		    sibling->attr.config == event->attr.config)
			return -EINVAL;
	}
	return 0;
}

/* Caller holds events_lock, including reads issued from another CPU. */
static void bmu_a4s_update_locked(struct perf_event *event)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	u32 reg_off = a4s_event_regs[event->attr.config];
	u32 raw, prev;

	if (bmu->shutting_down)
		return;
	raw = readl(bmu->base + reg_off);
	prev = local64_read(&event->hw.prev_count);
	local64_set(&event->hw.prev_count, raw);
	local64_add((u32)(raw - prev), &event->count);
}

static void bmu_a4s_start_locked(struct perf_event *event)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);

	if (bmu->shutting_down || !(event->hw.state & PERF_HES_STOPPED))
		return;
	/* Exclude counts accumulated before start or while disabled. */
	local64_set(&event->hw.prev_count,
		    readl(bmu->base + a4s_event_regs[event->attr.config]));
	event->hw.state = 0;
}

static void bmu_a4s_stop_locked(struct perf_event *event)
{
	if (event->hw.state & PERF_HES_STOPPED)
		return;
	/* Always capture the final delta: the hardware continues counting. */
	bmu_a4s_update_locked(event);
	event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

/* Normal counting starts in add(); keep ->start for explicit restarts. */
static void bmu_pmu_event_start(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	bmu_a4s_start_locked(event);
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
}

static void bmu_pmu_event_stop(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	bmu_a4s_stop_locked(event);
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
}

static void bmu_pmu_event_read(struct perf_event *event)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long flags;

	spin_lock_irqsave(&bmu->events_lock, flags);
	if (!(event->hw.state & PERF_HES_STOPPED))
		bmu_a4s_update_locked(event);
	spin_unlock_irqrestore(&bmu->events_lock, flags);
}

static int bmu_pmu_event_add(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	u32 event_id = event->attr.config;
	unsigned long irqflags;
	bool first;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -ENODEV;
	}
	/* A delayed open may still carry the pre-migration CPU. */
	if (event->cpu != bmu->on_cpu ||
	    smp_processor_id() != bmu->on_cpu) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -EAGAIN;
	}
	if (bmu->events[event_id]) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -EAGAIN;
	}
	event->hw.idx = event_id;
	event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	bmu->events[event_id] = event;
	first = (++bmu->num_active_events == 1);
	if (flags & PERF_EF_START)
		bmu_a4s_start_locked(event);
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);

	if (first)
		bmu_hrtimer_start(bmu);
	return 0;
}

static void bmu_pmu_event_del(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;
	bool last;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	bmu_a4s_stop_locked(event);
	bmu->events[event->hw.idx] = NULL;
	event->hw.idx = -1;
	last = (--bmu->num_active_events == 0);
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);

	/* Never wait for the timer while holding its callback's lock. */
	if (last)
		bmu_hrtimer_stop(bmu);
}

/*
 * AXI v2 window counters are latched totals, not free-running counters.
 * Each completed hardware window is consumed once, by this IRQ handler.
 * Incomplete windows at disable are discarded; read() never samples MMIO.
 */
static void bmu_axi_collect_locked(struct pcie_bmu_pmu *bmu)
{
	struct perf_event *event;
	int i;

	for (i = 0; i < ARRAY_SIZE(axi_event_regs); i++) {
		event = bmu->events[i];
		if (!event || !axi_event_regs[i] ||
		    (event->hw.state & PERF_HES_STOPPED))
			continue;
		if (bmu->skip_window[i]) {
			bmu->skip_window[i] = false;
			continue;
		}
		local64_add(readl(bmu->base + axi_event_regs[i]), &event->count);
	}
}

/* Capture each newly observed error bit once per measurement. */
static bool bmu_diag_capture(struct pcie_bmu_pmu *bmu, u32 status, u32 *ids)
{
	u32 errors = status & BMU_IRQ_ERROR_MASK;

	if (bmu->rc_index || bmu->fw_type || !(errors & ~bmu->diag_seen_errors))
		return false;

	bmu->diag_seen_errors |= errors;
	*ids = readl(bmu->base + BMU_IR_STS1);
	return true;
}

static void bmu_diag_report(struct pcie_bmu_pmu *bmu, const char *where,
			    u32 status, u32 ids)
{
	dev_info(bmu->dev, "%s: diag %s IR_STS0=%#x IR_STS1=%#x rd_id=%#x wr_id=%#x\n",
		 bmu->name, where, status, ids, ids >> 16, ids & 0xffff);
}

static irqreturn_t bmu_axi_irq(int irq, void *data)
{
	struct pcie_bmu_pmu *bmu = data;
	unsigned long flags;
	u32 status, ids;
	bool captured;

	spin_lock_irqsave(&bmu->events_lock, flags);
	status = readl(bmu->base + BMU_IR_STS0);
	if (!(status & BMU_IRQ_PERIOD_EXPIRED)) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return IRQ_NONE;
	}
	captured = bmu_diag_capture(bmu, status, &ids);
	if (bmu->irq_running)
		bmu_axi_collect_locked(bmu);
	writel(BMU_IR_STS0_SW_INT_CLR, bmu->base + BMU_IR_STS0);
	readl(bmu->base + BMU_IR_STS0);
	spin_unlock_irqrestore(&bmu->events_lock, flags);
	if (captured)
		bmu_diag_report(bmu, "irq", status, ids);
	/* FIFO/deferred-duration overflow and AXI response errors are sticky
	 * window status, independent of the period-expired interrupt selection.
	 */
	if (status & BMU_IRQ_ERROR_MASK)
		dev_warn_ratelimited(bmu->dev,
				     "%s: AXI error status %#x; check duration/FIFO/response status\n",
			bmu->name, (u32)(status & BMU_IRQ_ERROR_MASK));
	return IRQ_HANDLED;
}

static int bmu_axi_init(struct perf_event *event)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	struct perf_event *leader = event->group_leader, *sibling;
	u64 config1 = event->attr.config1;
	u32 id = event->attr.config, len_bits;
	int index, ret;

	ret = bmu_pmu_event_init(event);
	if (ret)
		return ret;
	if (event->attr.config2 || !axi_event_regs[id])
		return -EINVAL;
	index = bmu_var_counter_idx(id);
	if (index < 0 && config1)
		return -EINVAL;
	if (index >= 0 && index >= ((bmu->version1 >> 8) & 0xff))
		return -EOPNOTSUPP;
	/* Address-range bounds have no userspace ABI yet: reject range enable. */
	if (config1 & BIT_ULL(25))
		return -EOPNOTSUPP;
	if ((config1 & BIT_ULL(26)) &&
	    (8U << FIELD_GET(BMU_FLT_SIZE_MASK, config1)) >
		(bmu->version1 >> 16))
		return -EINVAL;
	len_bits = bmu->version1 & 0xff;
	if ((config1 & BIT_ULL(27)) && len_bits < 8 &&
	    FIELD_GET(BMU_FLT_LEN_MASK, config1) >= BIT(len_bits))
		return -EINVAL;

	/* A fixed counter cannot serve duplicate events in the same group. */
	if (leader != event && !is_software_event(leader)) {
		if (leader->pmu != event->pmu || leader->attr.config == id)
			return -EINVAL;
		if (index >= 0 && bmu_var_counter_idx(leader->attr.config) == index &&
		    leader->attr.config1 != config1)
			return -EINVAL;
	}
	for_each_sibling_event(sibling, leader) {
		if (sibling == event || is_software_event(sibling))
			continue;
		if (sibling->pmu != event->pmu || sibling->attr.config == id)
			return -EINVAL;
		if (index >= 0 && bmu_var_counter_idx(sibling->attr.config) == index &&
		    sibling->attr.config1 != config1)
			return -EINVAL;
	}
	return 0;
}

static void bmu_axi_set_duration_filter(struct pcie_bmu_pmu *bmu)
{
	u32 flt = readl(bmu->base + BMU_DURATION_FLT);

	flt &= ~(BMU_RD_SAMPLE_MASK | BMU_WR_SAMPLE_MASK);
	/* Fixed divisor four: select bit 1 for reads and bit 5 for writes. */
	flt |= BMU_RD_SAMPLE_DIV4 | BMU_WR_SAMPLE_DIV4;
	writel(flt, bmu->base + BMU_DURATION_FLT);
}

static void bmu_axi_enable(struct pmu *pmu)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(pmu);
	unsigned long flags;
	int i;
	bool running = false;

	spin_lock_irqsave(&bmu->events_lock, flags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, flags);
		return;
	}
	for (i = 0; i < bmu->num_events; i++) {
		if (bmu->events[i] && !(bmu->events[i]->hw.state & PERF_HES_STOPPED))
			running = true;
	}
	if (running && !bmu->irq_running) {
		memset(bmu->skip_window, 0, sizeof(bmu->skip_window));
		/* Reset stale window results before starting a fresh collection. */
		writel(0, bmu->base + BMU_CFG0);
		readl(bmu->base + BMU_CFG0);
		udelay(1);
		writel(BMU_AXI_PERIOD_CYCLES, bmu->base + BMU_CFG3);
		writel((bmu->duration_threshold << 16) | bmu->duration_threshold,
		       bmu->base + BMU_CFG1);
		writel(BMU_CFG0_SW_RSTN, bmu->base + BMU_CFG0);
		bmu_axi_set_duration_filter(bmu);
		writel(0, bmu->base + BMU_CFG4);
		writel(0, bmu->base + BMU_CFG5);
		for (i = 8; i <= 12; i++) {
			if (bmu->events[i])
				bmu_configure_var_filters(bmu, bmu->events[i]);
		}
		writel(BMU_IR_STS0_SW_INT_CLR, bmu->base + BMU_IR_STS0);
		bmu->diag_seen_errors = 0;
		bmu->irq_running = true;
		writel(readl(bmu->base + BMU_CFG0) | BMU_CFG0_MON_EN | BMU_CFG0_IRQ_PERIOD,
		       bmu->base + BMU_CFG0);
	}
	spin_unlock_irqrestore(&bmu->events_lock, flags);
}

static void bmu_axi_disable(struct pmu *pmu)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(pmu);
	unsigned long flags;
	u32 status = 0, ids;
	bool captured = false;

	spin_lock_irqsave(&bmu->events_lock, flags);
	if (bmu->irq_running) {
		/* Stop the source before consuming an already completed window.
		 * Clearing MON_EN does not guarantee a latch of the partial tail.
		 */
		writel(BMU_CFG0_SW_RSTN, bmu->base + BMU_CFG0);
		readl(bmu->base + BMU_CFG0);
		status = readl(bmu->base + BMU_IR_STS0);
		captured = bmu_diag_capture(bmu, status, &ids);
		if (status & BMU_IRQ_PERIOD_EXPIRED)
			bmu_axi_collect_locked(bmu);
		bmu->irq_running = false;
		writel(BMU_IR_STS0_SW_INT_CLR, bmu->base + BMU_IR_STS0);
		readl(bmu->base + BMU_IR_STS0);
	}
	spin_unlock_irqrestore(&bmu->events_lock, flags);
	if (captured)
		bmu_diag_report(bmu, "stop", status, ids);
}

/* Keep restart semantics, including exclusion of a pre-start window. */
static void bmu_axi_start(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return;
	}
	/* Do not charge the pre-start portion of an in-flight window. */
	bmu->skip_window[event->attr.config] = bmu->irq_running;
	event->hw.state = 0;
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
}

static void bmu_axi_stop(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
}

static int bmu_axi_add(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	u32 id = event->attr.config;
	unsigned long irqflags;
	struct perf_event *peer = NULL;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	if (bmu->shutting_down) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -ENODEV;
	}
	/* A delayed open may still carry the pre-migration CPU. */
	if (event->cpu != bmu->on_cpu ||
	    smp_processor_id() != bmu->on_cpu) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -EAGAIN;
	}
	if (id == 8 || id == 9)
		peer = bmu->events[id + 3];
	else if (id == 11 || id == 12)
		peer = bmu->events[id - 3];
	/* Read/write counters in a pair share the filter registers. */
	if (bmu->irq_running || bmu->events[id] ||
	    (peer && peer->attr.config1 != event->attr.config1)) {
		spin_unlock_irqrestore(&bmu->events_lock, irqflags);
		return -EBUSY;
	}
	event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	if (flags & PERF_EF_START)
		event->hw.state = 0;
	bmu->skip_window[id] = false;
	bmu->events[id] = event;
	bmu->num_active_events++;
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
	return 0;
}

static void bmu_axi_del(struct perf_event *event, int flags)
{
	struct pcie_bmu_pmu *bmu = to_bmu_pmu(event->pmu);
	unsigned long irqflags;

	spin_lock_irqsave(&bmu->events_lock, irqflags);
	event->hw.state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	bmu->events[event->attr.config] = NULL;
	bmu->num_active_events--;
	spin_unlock_irqrestore(&bmu->events_lock, irqflags);
}

static void bmu_axi_read(struct perf_event *event)
{
	/* IRQ/pmu_disable consume completed windows under events_lock.
	 * read() must not count the same latched window a second time.
	 */
}

/* ================================================================
 * CPU hotplug
 * ================================================================
 */

static int bmu_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct pcie_bmu_pmu *bmu;
	unsigned long flags;
	unsigned int target;
	int ret;

	bmu = hlist_entry_safe(node, struct pcie_bmu_pmu, cpuhp_node);
	if (cpu != READ_ONCE(bmu->on_cpu))
		return 0;
	target = cpumask_any_but(cpu_online_mask, cpu);
	if (target >= nr_cpu_ids)
		return -EBUSY;
	if (bmu->irq > 0) {
		ret = irq_set_affinity(bmu->irq, cpumask_of(target));
		if (ret)
			return ret;
	}
	/* Publish the destination before migration can install events there. */
	spin_lock_irqsave(&bmu->events_lock, flags);
	bmu->migrating = true;
	WRITE_ONCE(bmu->on_cpu, target);
	spin_unlock_irqrestore(&bmu->events_lock, flags);

	/*
	 * perf_install_in_context() dispatches add() to the destination CPU.
	 * A4S add() starts the pinned timer there when events are scheduled in.
	 */
	if (bmu->hw_type == BMU_HW_A4S)
		bmu_hrtimer_stop(bmu);
	/* events_lock serializes IRQ and perf callbacks during migration. */
	perf_pmu_migrate_context(&bmu->pmu, cpu, target);

	spin_lock_irqsave(&bmu->events_lock, flags);
	bmu->migrating = false;
	spin_unlock_irqrestore(&bmu->events_lock, flags);
	return 0;
}

/* ================================================================
 * ACPI _DSD parsing
 * ================================================================
 */

struct bmu_entry {
	u32 type;
	u32 count;
	u64 start;
	u64 length;
	u32 gsi;
};

/*
 * Parse "bmu-entries" from PNP0A08 _DSD.
 *
 * _DSD layout (custom UUID BABA0000-... per real hardware ASL):
 *   Package(2) {
 *     UUID,
 *     Package(N) {
 *       Package(2) { "bmu-entries", Package(M) {
 *         Package(5) { type, count, start, length, first_gsi },
 *         ...
 *       }}
 *     }
 *   }
 */
static int bmu_parse_dsd(acpi_handle handle, struct bmu_entry **entries,
			 int *num_entries)
{
	struct acpi_buffer buf = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *dsd, *sub_pkg, *prop, *val;
	acpi_status status;
	int i, j, n, ret = -ENODEV;

	status = acpi_evaluate_object(handle, "_DSD", NULL, &buf);
	if (ACPI_FAILURE(status))
		return -ENODEV;

	dsd = buf.pointer;
	if (!dsd || dsd->type != ACPI_TYPE_PACKAGE || dsd->package.count < 2)
		goto out;

	sub_pkg = &dsd->package.elements[1];
	if (sub_pkg->type != ACPI_TYPE_PACKAGE)
		goto out;

	for (i = 0; i < sub_pkg->package.count; i++) {
		prop = &sub_pkg->package.elements[i];
		if (prop->type != ACPI_TYPE_PACKAGE ||
		    prop->package.count < 2)
			continue;

		if (prop->package.elements[0].type != ACPI_TYPE_STRING)
			continue;
		if (strcmp(prop->package.elements[0].string.pointer,
			   "bmu-entries") != 0)
			continue;

		val = &prop->package.elements[1];
		if (val->type != ACPI_TYPE_PACKAGE)
			continue;

		n = val->package.count;
		if (n == 0)
			goto out;

		*entries = kcalloc(n, sizeof(struct bmu_entry), GFP_KERNEL);
		if (!*entries) {
			ret = -ENOMEM;
			goto out;
		}

		for (j = 0; j < n; j++) {
			union acpi_object *e = &val->package.elements[j];
			int k;

			if (e->type != ACPI_TYPE_PACKAGE ||
			    (e->package.count != 4 && e->package.count != 5))
				continue;

			for (k = 0; k < e->package.count; k++)
				if (e->package.elements[k].type != ACPI_TYPE_INTEGER)
					break;
			if (k != e->package.count ||
			    e->package.elements[0].integer.value >= FW_TYPE_COUNT ||
			    e->package.elements[1].integer.value > U32_MAX)
				continue;

			if (e->package.count == 5) {
				if (e->package.elements[4].integer.value > U32_MAX)
					continue;
				(*entries)[j].gsi = e->package.elements[4].integer.value;
			}

			(*entries)[j].type   = e->package.elements[0].integer.value;
			(*entries)[j].count  = e->package.elements[1].integer.value;
			(*entries)[j].start  = e->package.elements[2].integer.value;
			(*entries)[j].length = e->package.elements[3].integer.value;
		}
		*num_entries = n;
		ret = 0;
		break;
	}

out:
	kfree(buf.pointer);
	return ret;
}

/* ================================================================
 * PMU registration
 * ================================================================
 */

static int bmu_register_one_pmu(struct device *dev, int rc_index,
				const struct bmu_entry *entry,
				u64 stride, int inst_idx)
{
	struct pcie_bmu_pmu *bmu;
	const struct attribute_group **groups;
	u64 addr;
	int hw_type;
	int ret;

	if (entry->type >= FW_TYPE_COUNT) {
		dev_warn(dev, "RC%d: unknown firmware type %d, skipping\n",
			 rc_index, entry->type);
		return -EINVAL;
	}

	hw_type = fw_type_to_hw[entry->type];
	if (hw_type < 0 || (hw_type == BMU_HW_AXI && !entry->gsi))
		return -EOPNOTSUPP;

	bmu = kzalloc(sizeof(*bmu), GFP_KERNEL);
	if (!bmu)
		return -ENOMEM;

	bmu->dev = dev;
	bmu->hw_type = hw_type;
	bmu->fw_type = entry->type;
	bmu->rc_index = rc_index;
	bmu->num_events = bmu_max_events(hw_type);
	spin_lock_init(&bmu->events_lock);

	addr = entry->start + inst_idx * stride;
	bmu->base = ioremap(addr, stride);
	if (!bmu->base) {
		dev_warn(dev, "RC%d: failed to ioremap 0x%llx (stride=0x%llx)\n",
			 rc_index, addr, stride);
		kfree(bmu);
		return -ENOMEM;
	}

	snprintf(bmu->name, sizeof(bmu->name), "pcie_bmu_rc%d_%s_%d",
		 rc_index, fw_type_names[entry->type], inst_idx);

	/* Select attribute groups per hw type */
	switch (hw_type) {
	case BMU_HW_A4S:
		groups = a4s_pmu_attr_groups;
		break;
	default:
		groups = axi_pmu_attr_groups;
		break;
	}

	/* CPU setup */
	bmu->on_cpu = cpumask_first(cpu_online_mask);

	/* hrtimer setup */
	hrtimer_init(&bmu->hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	bmu->hrtimer.function = bmu_hrtimer_handler;

	/* Hardware-specific initialization */
	if (hw_type == BMU_HW_AXI) {
		bmu->duration_threshold = BMU_DEFAULT_DURA_THRESH;
		writel(0, bmu->base + BMU_CFG0);		/* reset */
		writel((BMU_DEFAULT_DURA_THRESH << 16) | BMU_DEFAULT_DURA_THRESH,
		       bmu->base + BMU_CFG1);
		writel(0, bmu->base + BMU_CFG4);
		writel(0, bmu->base + BMU_CFG5);
		writel(BMU_AXI_PERIOD_CYCLES, bmu->base + BMU_CFG3);
		writel(BMU_CFG0_SW_RSTN, bmu->base + BMU_CFG0);
	}

	bmu->pmu = (struct pmu) {
		.module		= THIS_MODULE,
		.task_ctx_nr	= perf_invalid_context,
		.event_init	= bmu_a4s_init,
		.add		= bmu_pmu_event_add,
		.del		= bmu_pmu_event_del,
		.start		= bmu_pmu_event_start,
		.stop		= bmu_pmu_event_stop,
		.read		= bmu_pmu_event_read,
		.attr_groups	= groups,
		.capabilities	= PERF_PMU_CAP_NO_EXCLUDE,
	};

	if (hw_type == BMU_HW_AXI) {
		bmu->version1 = readl(bmu->base + BMU_VERSION1);
		dev_info(dev, "%s: version1=%#x axi_data_width=%u var_counters=%u len_bits=%u\n",
			 bmu->name, bmu->version1, bmu->version1 >> 16,
			 (bmu->version1 >> 8) & 0xff, bmu->version1 & 0xff);

		/* Firmware lists consecutive IRQ sources for the instance array. */
		bmu->gsi = entry->gsi + inst_idx;
		bmu->irq = acpi_register_gsi(dev, bmu->gsi,
					     ACPI_LEVEL_SENSITIVE, ACPI_ACTIVE_HIGH);
		if (bmu->irq <= 0) {
			ret = bmu->irq ?: -EINVAL;
			goto err_unmap;
		}
		ret = request_irq(bmu->irq, bmu_axi_irq, 0, bmu->name, bmu);
		if (ret)
			goto err_gsi;
		ret = irq_set_affinity(bmu->irq, cpumask_of(bmu->on_cpu));
		if (ret)
			goto err_irq;

		bmu->pmu.event_init = bmu_axi_init;
		bmu->pmu.pmu_enable = bmu_axi_enable;
		bmu->pmu.pmu_disable = bmu_axi_disable;
		bmu->pmu.add = bmu_axi_add;
		bmu->pmu.del = bmu_axi_del;
		bmu->pmu.start = bmu_axi_start;
		bmu->pmu.stop = bmu_axi_stop;
		bmu->pmu.read = bmu_axi_read;
	}

	ret = cpuhp_state_add_instance(pcie_bmu_pmu_hp_state, &bmu->cpuhp_node);
	if (ret) {
		dev_err(dev, "RC%d: cpuhp add failed for %s: %d\n",
			rc_index, bmu->name, ret);
		goto err_irq;
	}

	ret = perf_pmu_register(&bmu->pmu, bmu->name, -1);
	if (ret) {
		dev_err(dev, "RC%d: failed to register %s: %d\n",
			rc_index, bmu->name, ret);
		cpuhp_state_remove_instance_nocalls(pcie_bmu_pmu_hp_state,
						    &bmu->cpuhp_node);
		goto err_irq;
	}

	mutex_lock(&bmu_pmu_lock);
	list_add_tail(&bmu->node, &bmu_pmu_list);
	mutex_unlock(&bmu_pmu_lock);

	dev_info(dev, "RC%d: registered %s @ 0x%llx (hw=%s, stride=0x%llx)\n",
		 rc_index, bmu->name, addr,
		 hw_type == BMU_HW_AXI ? "AXI" : "A4S",
		 stride);
	if (hw_type == BMU_HW_AXI)
		dev_info(dev, "%s: GSI=%u IRQ=%d CPU=%d period=%u APB cycles, no hrtimer\n",
			 bmu->name, bmu->gsi, bmu->irq, bmu->on_cpu,
			 BMU_AXI_PERIOD_CYCLES);
	else
		dev_info(dev, "%s: four free-running 32-bit counters, CPU=%d\n",
			 bmu->name, bmu->on_cpu);
	return 0;

err_irq:
	if (bmu->irq > 0)
		free_irq(bmu->irq, bmu);
err_gsi:
	if (bmu->gsi)
		acpi_unregister_gsi(bmu->gsi);
err_unmap:
	iounmap(bmu->base);
	kfree(bmu);
	return ret;
}

/* ================================================================
 * ACPI namespace walker
 * ================================================================
 */

static acpi_status bmu_walk_callback(acpi_handle handle, u32 level,
				     void *context, void **return_value)
{
	struct device *dev = context;
	struct bmu_entry *entries = NULL;
	int num_entries = 0;
	int rc_index = 0;
	int i, total = 0;
	unsigned long long uid;
	struct acpi_device_info *info;
	acpi_status status;

	status = acpi_get_object_info(handle, &info);
	if (ACPI_FAILURE(status))
		return AE_OK;

	if (!(info->valid & ACPI_VALID_HID) ||
	    strcmp(info->hardware_id.string, "PNP0A08") != 0) {
		kfree(info);
		return AE_OK;
	}
	kfree(info);

	if (ACPI_FAILURE(acpi_evaluate_integer(handle, "_UID", NULL, &uid)) ||
	    uid > INT_MAX)
		return AE_OK;
	rc_index = (int)uid;

	if (ACPI_SUCCESS(acpi_evaluate_integer(handle, "_STA", NULL, &uid)) &&
	    !(uid & ACPI_STA_DEVICE_PRESENT))
		return AE_OK;

	if (bmu_parse_dsd(handle, &entries, &num_entries))
		return AE_OK;

	dev_info(dev, "RC%d: found %d bmu-entries in _DSD\n",
		 rc_index, num_entries);

	for (i = 0; i < num_entries; i++) {
		struct bmu_entry *e = &entries[i];
		u64 stride, end;
		int j;

		if (e->type >= FW_TYPE_COUNT || fw_type_to_hw[e->type] < 0)
			continue;
		if (fw_type_to_hw[e->type] == BMU_HW_AXI &&
		    (!e->gsi || !e->count || e->count > 256 ||
		     e->gsi > U32_MAX - (e->count - 1))) {
			dev_warn(dev, "RC%d type %u: missing or invalid AXI IRQ range\n",
				 rc_index, e->type);
			continue;
		}

		if (!e->count || e->count > 256 || e->length < e->count ||
		    e->length % e->count ||
		    check_add_overflow(e->start, e->length, &end)) {
			dev_warn(dev, "RC%d: invalid entry type=%d count=%d len=0x%llx, skipping\n",
				 rc_index, e->type, e->count, e->length);
			continue;
		}
		stride = e->length / e->count;
		if (stride < (fw_type_to_hw[e->type] == BMU_HW_AXI ?
			      BMU_WR_STS4 : A4S_BMU_TXA4S_TYPE12_CNT) + sizeof(u32) ||
		    !IS_ALIGNED(e->start, sizeof(u32)) ||
		    !IS_ALIGNED(stride, sizeof(u32)))
			continue;

		dev_info(dev, "  type=%d count=%d start=0x%llx len=0x%llx stride=0x%llx\n",
			 e->type, e->count, e->start, e->length, stride);

		for (j = 0; j < e->count; j++) {
			if (bmu_register_one_pmu(dev, rc_index, e, stride, j) == 0)
				total++;
		}
	}

	dev_info(dev, "RC%d: registered %d BMU PMUs total\n", rc_index, total);

	kfree(entries);
	return AE_OK;
}

/* ================================================================
 * Module init/exit
 * ================================================================
 */

static void bmu_shutdown(struct platform_device *pdev)
{
	struct pcie_bmu_pmu *bmu;
	unsigned long flags;
	int i;

	mutex_lock(&bmu_pmu_lock);
	list_for_each_entry(bmu, &bmu_pmu_list, node) {
		spin_lock_irqsave(&bmu->events_lock, flags);
		if (bmu->shutting_down) {
			spin_unlock_irqrestore(&bmu->events_lock, flags);
			continue;
		}
		/* Capture A4S final deltas before closing its MMIO paths. */
		if (bmu->hw_type == BMU_HW_A4S) {
			for (i = 0; i < bmu->num_events; i++) {
				if (bmu->events[i])
					bmu_a4s_stop_locked(bmu->events[i]);
			}
		}
		bmu->shutting_down = true;
		spin_unlock_irqrestore(&bmu->events_lock, flags);

		/* Never wait for a handler while holding events_lock. */
		if (bmu->hw_type == BMU_HW_AXI) {
			/* Keep the IRQ disabled; the terminal flag prevents restart. */
			disable_irq(bmu->irq);
			bmu_axi_disable(&bmu->pmu);
			spin_lock_irqsave(&bmu->events_lock, flags);
			for (i = 0; i < bmu->num_events; i++) {
				if (bmu->events[i])
					bmu->events[i]->hw.state =
						PERF_HES_STOPPED | PERF_HES_UPTODATE;
			}
			spin_unlock_irqrestore(&bmu->events_lock, flags);
		} else {
			bmu_hrtimer_stop(bmu);
		}
	}
	mutex_unlock(&bmu_pmu_lock);
}

static int bmu_parent_probe(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver bmu_parent_driver = {
	.probe = bmu_parent_probe,
	.shutdown = bmu_shutdown,
	.driver = {
		.name = "pcie-bmu-pmu",
		.suppress_bind_attrs = true,
	},
};

static struct platform_device *bmu_pdev;

static void bmu_unregister_all(void);

static int __init pcie_bmu_pmu_init(void)
{
	acpi_status status;
	int ret;

	if (acpi_disabled)
		return -ENODEV;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "perf/pcie_bmu:online",
				      NULL,
				      bmu_pmu_offline_cpu);
	if (ret < 0) {
		pr_err("cpuhp_setup_state_multi failed: %d\n", ret);
		return ret;
	}
	pcie_bmu_pmu_hp_state = ret;

	ret = platform_driver_register(&bmu_parent_driver);
	if (ret)
		goto err_cpuhp;

	bmu_pdev = platform_device_alloc("pcie-bmu-pmu", -1);
	if (!bmu_pdev) {
		ret = -ENOMEM;
		goto err_driver;
	}

	ret = platform_device_add(bmu_pdev);
	if (ret) {
		platform_device_put(bmu_pdev);
		bmu_pdev = NULL;
		goto err_driver;
	}

	pr_info("scanning ACPI namespace for PCIe BMU devices\n");

	status = acpi_walk_namespace(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT,
				     ACPI_UINT32_MAX, bmu_walk_callback,
				     NULL, &bmu_pdev->dev, NULL);
	if (ACPI_FAILURE(status)) {
		pr_err("acpi_walk_namespace failed: %d\n", status);
		ret = -ENODEV;
		goto err_pdev;
	}

	if (list_empty(&bmu_pmu_list)) {
		ret = -ENODEV;
		goto err_pdev;
	}
	return 0;

err_pdev:
	bmu_unregister_all();
	platform_device_unregister(bmu_pdev);
	bmu_pdev = NULL;
err_driver:
	platform_driver_unregister(&bmu_parent_driver);
err_cpuhp:
	cpuhp_remove_multi_state(pcie_bmu_pmu_hp_state);
	return ret;
}

static void bmu_unregister_all(void)
{
	struct pcie_bmu_pmu *bmu, *tmp;
	LIST_HEAD(remove_list);

	mutex_lock(&bmu_pmu_lock);
	list_splice_init(&bmu_pmu_list, &remove_list);
	mutex_unlock(&bmu_pmu_lock);
	list_for_each_entry_safe(bmu, tmp, &remove_list, node) {
		cpuhp_state_remove_instance_nocalls(pcie_bmu_pmu_hp_state,
						    &bmu->cpuhp_node);
		perf_pmu_unregister(&bmu->pmu);
		if (bmu->hw_type == BMU_HW_AXI) {
			bmu_axi_disable(&bmu->pmu);
			free_irq(bmu->irq, bmu);
			acpi_unregister_gsi(bmu->gsi);
		}
		hrtimer_cancel(&bmu->hrtimer);
		iounmap(bmu->base);
		list_del(&bmu->node);
		kfree(bmu);
	}
}

static void __exit pcie_bmu_pmu_exit(void)
{
	bmu_unregister_all();
	platform_device_unregister(bmu_pdev);
	platform_driver_unregister(&bmu_parent_driver);
	cpuhp_remove_multi_state(pcie_bmu_pmu_hp_state);
}

module_init(pcie_bmu_pmu_init);
module_exit(pcie_bmu_pmu_exit);

MODULE_DESCRIPTION("Xuange PCIe BMU PMU driver (ACPI bmu-entries discovery)");
MODULE_AUTHOR("Jiankang Chen <jkchen@linux.alibaba.com>");
MODULE_AUTHOR("Dong Yan <yandong@linux.alibaba.com>");
MODULE_LICENSE("GPL");
