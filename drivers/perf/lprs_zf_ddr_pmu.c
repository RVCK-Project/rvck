// SPDX-License-Identifier: GPL-2.0+
/*
 * LeapRise ZF(Zhufeng) DDR Sub-system PMU Events Monitoring Support.
 *
 * Copyright (C) 2026, LeapRise Technology Co., Ltd.
 */

#include <linux/acpi.h>
#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/bits.h>
#include <linux/cpuhotplug.h>
#include <linux/cpumask.h>
#include <linux/errno.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>

#define ZF_DDR_PMU_MAX_COUNTERS 16
#define ZF_DDR_PMU_MAX_CHB_EVENTS 0x40
#define ZF_DDR_PMU_MAX_DDRC_EVENTS 0x44

#define ZF_DDR_PMU_PA_SHIFT 24
#define ZF_DDR_PMU_CNT_INIT 0

#define ZF_DDR_PMU_CHB_DOMAIN_OFFSET 0x0
#define ZF_DDR_PMU_DDRC_DOMAIN_OFFSET 0x400

#define ZF_DDR_PMU_REG_CFG0 0x04
#define ZF_DDR_PMU_CNT_CLEAR_MASK GENMASK(31, 16)
#define ZF_DDR_PMU_CNT_ENABLE_MASK GENMASK(15, 0)

#define ZF_DDR_PMU_CNT_CLEAR_BIT(n) BIT((n) + 16)
#define ZF_DDR_PMU_CNT_ENABLE_BIT(n) BIT(n)
#define ZF_DDR_PMU_CNT_DISABLE_BIT(n) (~BIT(n))

#define ZF_DDR_PMU_REG_CFG1 0x08
#define ZF_DDR_PMU_EVENT_CFG1_DCH_SEL GENMASK(13, 13)
#define ZF_DDR_PMU_EVENT_CFG1_ID GENMASK(12, 6)
#define ZF_DDR_PMU_EVENT_CFG1_CNT_ID GENMASK(5, 2)
#define ZF_DDR_PMU_EVENT_CFG1_BOUND_EN GENMASK(0, 0)

#define ZF_DDR_PMU_REG_CNT_L(x) (0x0C + ((x) * 0x08))
#define ZF_DDR_PMU_REG_CNT_H(x) (0x10 + ((x) * 0x08))

#define ZF_DDR_PMU_REG_CYCLE_L 0x8C
#define ZF_DDR_PMU_REG_CYCLE_H 0x90

/* fixed cycle event */
#define ZF_DDR_PMU_FIXED_EVT_ID_CHB_CYCLE 0x200
#define ZF_DDR_PMU_FIXED_EVT_ID_DDRC_CYCLE 0x300

#define ZF_DDR_PMU_EVENT_DCH0 0
#define ZF_DDR_PMU_EVENT_DCH1 1
#define ZF_DDR_PMU_EVENT_DOMAIN_CHB 0
#define ZF_DDR_PMU_EVENT_DOMAIN_DDRC 1
#define ZF_DDR_PMU_EVENT_TYPE_GENERAL 0
#define ZF_DDR_PMU_EVENT_TYPE_FIXED 1

#define ZF_DDR_PMU_EVENT_CONFIG_MASK GENMASK(9, 0)
#define ZF_DDR_PMU_EVENT_ID_MASK GENMASK(6, 0)

#define ZF_DDR_PMU_EVENT_DCH_SHIFT 7
#define ZF_DDR_PMU_EVENT_DOMAIN_SHIFT 8
#define ZF_DDR_PMU_EVENT_TYPE_SHIFT 9

#define ZF_DDR_PMU_EVENT_DCH_MASK BIT(ZF_DDR_PMU_EVENT_DCH_SHIFT)
#define ZF_DDR_PMU_EVENT_DOMAIN_MASK BIT(ZF_DDR_PMU_EVENT_DOMAIN_SHIFT)
#define ZF_DDR_PMU_EVENT_TYPE_MASK BIT(ZF_DDR_PMU_EVENT_TYPE_SHIFT)

#define ZF_DDR_PMU_GET_EVENT_CONFIG(event) \
	FIELD_GET(ZF_DDR_PMU_EVENT_CONFIG_MASK, (event)->attr.config)

#define ZF_DDR_PMU_GET_EVENT_ID(event) \
	FIELD_GET(ZF_DDR_PMU_EVENT_ID_MASK, (event)->attr.config)

#define ZF_DDR_PMU_GET_EVENT_DCH(event) \
	FIELD_GET(ZF_DDR_PMU_EVENT_DCH_MASK, (event)->attr.config)

#define ZF_DDR_PMU_GET_EVENT_DOMAIN(event) \
	FIELD_GET(ZF_DDR_PMU_EVENT_DOMAIN_MASK, (event)->attr.config)

#define ZF_DDR_PMU_GET_EVENT_TYPE(event) \
	FIELD_GET(ZF_DDR_PMU_EVENT_TYPE_MASK, (event)->attr.config)

#define ZF_DDR_PMU_GET_DOMAIN_OFFSET(event)                                    \
	((ZF_DDR_PMU_GET_EVENT_DOMAIN(event) == ZF_DDR_PMU_EVENT_DOMAIN_CHB) ? \
		 ZF_DDR_PMU_CHB_DOMAIN_OFFSET :                                \
		 ZF_DDR_PMU_DDRC_DOMAIN_OFFSET)

#define ZF_DDR_PMU_GET_PMU_DOMAIN(event, pmu)                                  \
	((ZF_DDR_PMU_GET_EVENT_DOMAIN(event) == ZF_DDR_PMU_EVENT_DOMAIN_CHB) ? \
		 &(pmu)->chb :                                                 \
		 &(pmu)->ddrc)

#define to_zf_ddr_pmu(p) (container_of(p, struct zf_ddr_pmu, pmu))

static int zf_ddr_pmu_cpuhp_state_num;

static const struct acpi_device_id zf_ddr_acpi_match[] = {
	{
		"LPRS0001",
	},
	{
		"LRWX0001", //deprecated, kept for backward compatibility
	},
	{},
};
MODULE_DEVICE_TABLE(acpi, zf_ddr_acpi_match);

struct zf_ddr_pmu_domain {
	int num_counters;
	unsigned long *used_mask;
};

struct zf_ddr_pmu {
	void __iomem *cfg_base;
	struct device *dev;
	struct pmu pmu;
	int cpu;

	struct zf_ddr_pmu_domain ddrc;
	struct zf_ddr_pmu_domain chb;
	struct hlist_node node;
};

static ssize_t zf_ddr_pmu_event_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *ext =
		container_of(attr, struct dev_ext_attribute, attr);
	return sysfs_emit(buf, "config=0x%llx\n", (u64)ext->var);
}

static ssize_t zf_ddr_pmu_cpumask_show(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(ddr_pmu->cpu));
}

static ssize_t zf_ddr_pmu_format_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *ext =
		container_of(attr, struct dev_ext_attribute, attr);
	return sysfs_emit(buf, "%s\n", (char *)ext->var);
}

#define ZF_DDR_PMU_ATTR(_name, _show_func, _config)      \
	(&((struct dev_ext_attribute[]){                 \
		{ __ATTR(_name, 0444, _show_func, NULL), \
		  (void *)(_config) } })[0]              \
		  .attr.attr)

#define ZF_DDR_PMU_FORMAT_ATTR(_name, _config) \
	ZF_DDR_PMU_ATTR(_name, zf_ddr_pmu_format_show, (void *)(_config))

#define ZF_DDR_PMU_EVENT_ATTR(_name, _config) \
	ZF_DDR_PMU_ATTR(_name, zf_ddr_pmu_event_show, (unsigned long)(_config))

#define ZF_DDR_PMU_EVENT_DCH0_SHIFTED \
	((ZF_DDR_PMU_EVENT_DCH0) << (ZF_DDR_PMU_EVENT_DCH_SHIFT))
#define ZF_DDR_PMU_EVENT_DCH1_SHIFTED \
	((ZF_DDR_PMU_EVENT_DCH1) << (ZF_DDR_PMU_EVENT_DCH_SHIFT))
#define ZF_DDR_PMU_EVENT_DOMAIN_CHB_SHIFTED \
	((ZF_DDR_PMU_EVENT_DOMAIN_CHB) << (ZF_DDR_PMU_EVENT_DOMAIN_SHIFT))
#define ZF_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED \
	((ZF_DDR_PMU_EVENT_DOMAIN_DDRC) << (ZF_DDR_PMU_EVENT_DOMAIN_SHIFT))

#define ZF_DDR_PMU_GEN_DDRC_EVENTS_0(prefix, cfg)                              \
	(ZF_DDR_PMU_EVENT_ATTR(ddrc_##prefix##_sub0,                           \
			       ((cfg) | ZF_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED | \
				ZF_DDR_PMU_EVENT_DCH0_SHIFTED)))

#define ZF_DDR_PMU_GEN_DDRC_EVENTS_1(prefix, cfg)                              \
	(ZF_DDR_PMU_EVENT_ATTR(ddrc_##prefix##_sub1,                           \
			       ((cfg) | ZF_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED | \
				ZF_DDR_PMU_EVENT_DCH1_SHIFTED)))

#define ZF_DDR_PMU_GEN_FIXED_EVENT(prefix, cfg) \
	ZF_DDR_PMU_EVENT_ATTR(fixed_##prefix, (cfg))

static struct attribute *zf_ddr_pmu_events_attrs[] = {
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rd_or_wr_ps0, 0),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rd_or_wr_ps1, 1),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_wr_ps0, 2),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_wr_ps1, 3),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rd_ps0, 4),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rd_ps1, 5),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rmw_ps0, 6),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(in_rmw_ps1, 7),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(activate_ps0, 17),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(activate_ps1, 18),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rd_or_wr_ps0, 19),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rd_or_wr_ps1, 20),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(rd_activate_ps0, 21),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(rd_activate_ps1, 22),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rd_ps0, 23),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rd_ps1, 24),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_wr_ps0, 25),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_wr_ps1, 26),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rmw_ps0, 27),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(out_rmw_ps1, 28),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_ps0, 29),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_ps1, 30),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(rdwr_trans, 35),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(wcomb_ps0, 36),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(wcomb_ps1, 37),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(war, 38),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(raw, 39),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(waw, 40),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(ref_ps0, 53),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_0(ref_ps1, 54),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rd_or_wr_ps0, 0),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rd_or_wr_ps1, 1),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_wr_ps0, 2),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_wr_ps1, 3),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rd_ps0, 4),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rd_ps1, 5),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rmw_ps0, 6),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(in_rmw_ps1, 7),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(activate_ps0, 17),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(activate_ps1, 18),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rd_or_wr_ps0, 19),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rd_or_wr_ps1, 20),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(rd_activate_ps0, 21),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(rd_activate_ps1, 22),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rd_ps0, 23),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rd_ps1, 24),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_wr_ps0, 25),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_wr_ps1, 26),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rmw_ps0, 27),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(out_rmw_ps1, 28),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_ps0, 29),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_ps1, 30),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(rdwr_trans, 35),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(wcomb_ps0, 36),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(wcomb_ps1, 37),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(war, 38),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(raw, 39),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(waw, 40),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(ref_ps0, 53),
	ZF_DDR_PMU_GEN_DDRC_EVENTS_1(ref_ps1, 54),
	ZF_DDR_PMU_GEN_FIXED_EVENT(chb_cycle,
				   ZF_DDR_PMU_FIXED_EVT_ID_CHB_CYCLE),
	ZF_DDR_PMU_GEN_FIXED_EVENT(ddrc_cycle,
				   ZF_DDR_PMU_FIXED_EVT_ID_DDRC_CYCLE),
	NULL,
};

static const struct attribute_group zf_ddr_pmu_events_attr_group = {
	.name = "events",
	.attrs = zf_ddr_pmu_events_attrs,
};

static struct device_attribute zf_ddr_pmu_cpumask_attr =
	__ATTR(cpumask, 0444, zf_ddr_pmu_cpumask_show, NULL);

static struct attribute *zf_ddr_pmu_cpumask_attrs[] = {
	&zf_ddr_pmu_cpumask_attr.attr,
	NULL,
};

static const struct attribute_group zf_ddr_pmu_cpumask_attr_group = {
	.attrs = zf_ddr_pmu_cpumask_attrs,
};

static struct attribute *zf_ddr_pmu_format_attrs[] = {
	ZF_DDR_PMU_FORMAT_ATTR(event, "config:0-6"),
	ZF_DDR_PMU_FORMAT_ATTR(dch, "config:7"),
	ZF_DDR_PMU_FORMAT_ATTR(domain, "config:8"),
	ZF_DDR_PMU_FORMAT_ATTR(type, "config:9"),
	NULL,
};

static const struct attribute_group zf_ddr_pmu_format_group = {
	.name = "format",
	.attrs = zf_ddr_pmu_format_attrs,
};

static const struct attribute_group *zf_ddr_pmu_attr_groups[] = {
	&zf_ddr_pmu_events_attr_group,
	&zf_ddr_pmu_cpumask_attr_group,
	&zf_ddr_pmu_format_group,
	NULL,
};

static bool zf_ddr_pmu_is_support_event(struct perf_event *event)
{
	u32 id = ZF_DDR_PMU_GET_EVENT_ID(event);
	u32 domain = ZF_DDR_PMU_GET_EVENT_DOMAIN(event);

	switch (domain) {
	case ZF_DDR_PMU_EVENT_DOMAIN_CHB:
		return id <= ZF_DDR_PMU_MAX_CHB_EVENTS;
	case ZF_DDR_PMU_EVENT_DOMAIN_DDRC:
		return id <= ZF_DDR_PMU_MAX_DDRC_EVENTS;
	default:
		return false;
	}
}

static int zf_ddr_pmu_get_event_idx(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct zf_ddr_pmu_domain *domain;
	int idx;

	domain = ZF_DDR_PMU_GET_PMU_DOMAIN(event, ddr_pmu);
	for (idx = 0; idx < domain->num_counters; ++idx) {
		if (!test_and_set_bit(idx, domain->used_mask))
			return idx;
	}

	return -EBUSY;
}

static int zf_ddr_pmu_event_init(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (is_sampling_event(event))
		return -EOPNOTSUPP;

	event->cpu = ddr_pmu->cpu;
	if (event->cpu < 0 || event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	if (!zf_ddr_pmu_is_support_event(event))
		return -EOPNOTSUPP;

	hwc->idx = -1;

	return 0;
}

static void zf_ddr_pmu_bind_counter(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 cfg = ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) + ZF_DDR_PMU_REG_CFG1;
	u32 val = 0;

	val = FIELD_PREP(ZF_DDR_PMU_EVENT_CFG1_BOUND_EN, 1) |
	      FIELD_PREP(ZF_DDR_PMU_EVENT_CFG1_CNT_ID, hwc->idx) |
	      FIELD_PREP(ZF_DDR_PMU_EVENT_CFG1_ID,
			 ZF_DDR_PMU_GET_EVENT_ID(event)) |
	      FIELD_PREP(ZF_DDR_PMU_EVENT_CFG1_DCH_SEL,
			 ZF_DDR_PMU_GET_EVENT_DCH(event));
	writel(val, ddr_pmu->cfg_base + cfg);
}

static void zf_ddr_pmu_enable_counter(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 reg, val;

	reg = ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) + ZF_DDR_PMU_REG_CFG0;
	val = readl(ddr_pmu->cfg_base + reg);
	val |= ZF_DDR_PMU_CNT_ENABLE_BIT(hwc->idx);
	val |= ZF_DDR_PMU_CNT_CLEAR_BIT(hwc->idx);
	writel(val, ddr_pmu->cfg_base + reg);
}

static void zf_ddr_pmu_disable_counter(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 reg, val;

	reg = ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) + ZF_DDR_PMU_REG_CFG0;
	val = readl(ddr_pmu->cfg_base + reg);

	val &= ZF_DDR_PMU_CNT_DISABLE_BIT(hwc->idx);
	writel(val, ddr_pmu->cfg_base + reg);
}

static u64 zf_ddr_pmu_read_counter(struct perf_event *event)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u64 cycle_high, cycle_low, high2;

	if (ZF_DDR_PMU_GET_EVENT_TYPE(event) == ZF_DDR_PMU_EVENT_TYPE_FIXED) {
		do {
			cycle_high = readl(ddr_pmu->cfg_base +
					   ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
					   ZF_DDR_PMU_REG_CYCLE_H);
			cycle_low = readl(ddr_pmu->cfg_base +
					  ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
					  ZF_DDR_PMU_REG_CYCLE_L);
			high2 = readl(ddr_pmu->cfg_base +
				      ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				      ZF_DDR_PMU_REG_CYCLE_H);
		} while (cycle_high != high2);
	} else {
		do {
			cycle_high = readl(ddr_pmu->cfg_base +
					   ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
					   ZF_DDR_PMU_REG_CNT_H(hwc->idx));
			cycle_low = readl(ddr_pmu->cfg_base +
					  ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
					  ZF_DDR_PMU_REG_CNT_L(hwc->idx));
			high2 = readl(ddr_pmu->cfg_base +
				      ZF_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				      ZF_DDR_PMU_REG_CNT_H(hwc->idx));
		} while (cycle_high != high2);
	}

	return (u64)(cycle_high << 32 | cycle_low);
}

static void zf_ddr_pmu_event_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 delta, prev, now;

	do {
		prev = local64_read(&hwc->prev_count);
		now = zf_ddr_pmu_read_counter(event);
	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	delta = now - prev;
	local64_add(delta, &event->count);
}

static void zf_ddr_pmu_start(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 pre_val;

	hwc->state = 0;

	if (ZF_DDR_PMU_GET_EVENT_TYPE(event) == ZF_DDR_PMU_EVENT_TYPE_FIXED) {
		pre_val = zf_ddr_pmu_read_counter(event);
		local64_set(&hwc->prev_count, pre_val);
		return;
	}

	pre_val = ZF_DDR_PMU_CNT_INIT;
	local64_set(&hwc->prev_count, pre_val);

	zf_ddr_pmu_bind_counter(event);
	zf_ddr_pmu_enable_counter(event);
}

static int zf_ddr_pmu_add(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	int idx = -1;

	if (ZF_DDR_PMU_GET_EVENT_TYPE(event) == ZF_DDR_PMU_EVENT_TYPE_GENERAL) {
		idx = zf_ddr_pmu_get_event_idx(event);
		if (idx < 0)
			return idx;
	}

	hwc->idx = idx;

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	if (flags & PERF_EF_START)
		zf_ddr_pmu_start(event, PERF_EF_RELOAD);

	perf_event_update_userpage(event);

	return 0;
}

static void zf_ddr_pmu_stop(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	if (ZF_DDR_PMU_GET_EVENT_TYPE(event) == ZF_DDR_PMU_EVENT_TYPE_GENERAL)
		zf_ddr_pmu_disable_counter(event);

	zf_ddr_pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static void zf_ddr_pmu_del(struct perf_event *event, int flags)
{
	struct zf_ddr_pmu *ddr_pmu = to_zf_ddr_pmu(event->pmu);
	struct zf_ddr_pmu_domain *domain;
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	zf_ddr_pmu_stop(event, PERF_EF_UPDATE);

	domain = ZF_DDR_PMU_GET_PMU_DOMAIN(event, ddr_pmu);
	if (idx >= 0 && idx < domain->num_counters) {
		clear_bit(idx, domain->used_mask);
		hwc->idx = -1;
	}
	perf_event_update_userpage(event);
}

static void zf_ddr_pmu_read(struct perf_event *event)
{
	zf_ddr_pmu_event_update(event);
}

static int zf_ddr_pmu_domain_init(struct zf_ddr_pmu_domain *domain,
				  struct device *dev, int num_counters)
{
	if (!domain || !dev || num_counters <= 0 ||
	    num_counters > ZF_DDR_PMU_MAX_COUNTERS)
		return -EINVAL;

	domain->num_counters = num_counters;

	domain->used_mask = devm_bitmap_zalloc(dev, num_counters, GFP_KERNEL);
	if (!domain->used_mask)
		return -ENOMEM;

	return 0;
}

static int zf_ddr_pmu_probe(struct platform_device *pdev)
{
	struct zf_ddr_pmu *ddr_pmu;
	struct resource *res;
	char *name;
	int ret;

	ddr_pmu = devm_kzalloc(&pdev->dev, sizeof(*ddr_pmu), GFP_KERNEL);
	if (!ddr_pmu)
		return -ENOMEM;

	ddr_pmu->dev = &pdev->dev;

	platform_set_drvdata(pdev, ddr_pmu);

	ddr_pmu->cfg_base =
		devm_platform_get_and_ioremap_resource(pdev, 0, &res);

	if (IS_ERR(ddr_pmu->cfg_base))
		return PTR_ERR(ddr_pmu->cfg_base);

	name = devm_kasprintf(ddr_pmu->dev, GFP_KERNEL, "zf_ddr_%llx",
			      (u64)(res->start >> ZF_DDR_PMU_PA_SHIFT));

	if (!name)
		return -ENOMEM;

	ddr_pmu->cpu = raw_smp_processor_id();

	ret = zf_ddr_pmu_domain_init(&ddr_pmu->ddrc, ddr_pmu->dev,
				     ZF_DDR_PMU_MAX_COUNTERS);
	if (ret)
		return ret;

	ret = zf_ddr_pmu_domain_init(&ddr_pmu->chb, ddr_pmu->dev,
				     ZF_DDR_PMU_MAX_COUNTERS);
	if (ret)
		return ret;

	writel(ZF_DDR_PMU_CNT_CLEAR_MASK, ddr_pmu->cfg_base +
						  ZF_DDR_PMU_CHB_DOMAIN_OFFSET +
						  ZF_DDR_PMU_REG_CFG0);

	writel(ZF_DDR_PMU_CNT_CLEAR_MASK,
	       ddr_pmu->cfg_base + ZF_DDR_PMU_DDRC_DOMAIN_OFFSET +
		       ZF_DDR_PMU_REG_CFG0);

	ddr_pmu->pmu = (struct pmu){
		.module = THIS_MODULE,
		.parent = &pdev->dev,
		.task_ctx_nr = perf_invalid_context,
		.event_init = zf_ddr_pmu_event_init,
		.add = zf_ddr_pmu_add,
		.del = zf_ddr_pmu_del,
		.start = zf_ddr_pmu_start,
		.stop = zf_ddr_pmu_stop,
		.read = zf_ddr_pmu_read,
		.attr_groups = zf_ddr_pmu_attr_groups,
		.capabilities = PERF_PMU_CAP_NO_EXCLUDE,
	};

	ret = perf_pmu_register(&ddr_pmu->pmu, name, -1);
	if (ret)
		dev_err(ddr_pmu->dev, "ZF DDR PMU register failed!\n");
	else {
		ret = cpuhp_state_add_instance_nocalls(
			zf_ddr_pmu_cpuhp_state_num, &ddr_pmu->node);
		if (ret) {
			dev_err(ddr_pmu->dev,
				"ZF DDR PMU hotplug register failed!\n");
			perf_pmu_unregister(&ddr_pmu->pmu);
			return ret;
		}
	}

	return ret;
}

static int zf_ddr_pmu_remove(struct platform_device *pdev)
{
	struct zf_ddr_pmu *ddr_pmu = platform_get_drvdata(pdev);

	cpuhp_state_remove_instance_nocalls(zf_ddr_pmu_cpuhp_state_num,
					    &ddr_pmu->node);

	perf_pmu_unregister(&ddr_pmu->pmu);
	return 0;
}

static struct platform_driver zf_ddr_pmu_driver = {
	.driver = {
		.name = "zf_ddr_pmu",
		   .acpi_match_table = zf_ddr_acpi_match,
		   },
	.probe = zf_ddr_pmu_probe,
	.remove = zf_ddr_pmu_remove,
};

static int zf_ddr_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct zf_ddr_pmu *pmu =
		hlist_entry_safe(node, struct zf_ddr_pmu, node);
	unsigned int target;

	if (cpu != pmu->cpu)
		return 0;

	target = cpumask_any_but(cpu_online_mask, cpu);
	if (target >= nr_cpu_ids)
		return 0;

	perf_pmu_migrate_context(&pmu->pmu, cpu, target);

	pmu->cpu = target;
	return 0;
}

static int __init zf_ddr_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN, "zf_ddr_pmu:online",
				      NULL, zf_ddr_pmu_offline_cpu);

	if (ret < 0) {
		pr_err("ZF DDR PMU: setup hotplug failed, ret = %d\n", ret);
		return ret;
	}
	zf_ddr_pmu_cpuhp_state_num = ret;

	ret = platform_driver_register(&zf_ddr_pmu_driver);
	if (ret)
		cpuhp_remove_multi_state(zf_ddr_pmu_cpuhp_state_num);

	return ret;
}
module_init(zf_ddr_pmu_init);

static void __exit zf_ddr_pmu_exit(void)
{
	platform_driver_unregister(&zf_ddr_pmu_driver);
	cpuhp_remove_multi_state(zf_ddr_pmu_cpuhp_state_num);
}
module_exit(zf_ddr_pmu_exit);

MODULE_DESCRIPTION("LeapRise ZF(ZhuFeng) DDR PMU driver");
MODULE_AUTHOR("Feng Jie");
MODULE_LICENSE("GPL");
