// SPDX-License-Identifier: GPL-2.0-only
/*
 * DDR Sub-system PMU Events Monitoring Support
 *
 * Copyright (C) 2025 LRW Corporation or its affiliates.
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

#define LRW_DDR_PMU_MAX_COUNTERS 16
#define LRW_DDR_PMU_MAX_CHB_EVENTS 0x40
#define LRW_DDR_PMU_MAX_DDRC_EVENTS 0x44

#define LRW_DDR_PMU_PA_SHIFT 24
#define LRW_DDR_PMU_CNT_INIT 0

#define LRW_DDR_PMU_CHB_DOMAIN_OFFSET 0x0
#define LRW_DDR_PMU_DDRC_DOMAIN_OFFSET 0x400

#define LRW_DDR_PMU_REG_CFG0 0x04
#define LRW_DDR_PMU_CNT_CLEAR_MASK GENMASK(31, 16)
#define LRW_DDR_PMU_CNT_ENABLE_MASK GENMASK(15, 0)

#define LRW_DDR_PMU_CNT_CLEAR_BIT(n) BIT((n) + 16)
#define LRW_DDR_PMU_CNT_ENABLE_BIT(n) BIT(n)
#define LRW_DDR_PMU_CNT_DISABLE_BIT(n) (~BIT(n))

#define LRW_DDR_PMU_REG_CFG1 0x08
#define LRW_DDR_PMU_EVENT_CFG1_DCH_SEL GENMASK(13, 13)
#define LRW_DDR_PMU_EVENT_CFG1_ID GENMASK(12, 6)
#define LRW_DDR_PMU_EVENT_CFG1_CNT_ID GENMASK(5, 2)
#define LRW_DDR_PMU_EVENT_CFG1_BOUND_EN GENMASK(0, 0)

#define LRW_DDR_PMU_REG_CNT_L(x) (0x0C + ((x) * 0x08))
#define LRW_DDR_PMU_REG_CNT_H(x) (0x10 + ((x) * 0x08))

#define LRW_DDR_PMU_REG_CYCLE_L 0x8C
#define LRW_DDR_PMU_REG_CYCLE_H 0x90

#define LRW_DDR_PMU_REG_OCCUPY_MON_CTL 0x100
#define LRW_DDR_PMU_REG_OCCUPY_MON_CNT_MASK 0xFFFFFF

/* fixed cycle event */
#define LRW_DDR_PMU_FIXED_EVT_ID_CHB_CYCLE 0x200
#define LRW_DDR_PMU_FIXED_EVT_ID_DDRC_CYCLE 0x300

#define LRW_DDR_PMU_EVENT_DCH0 0
#define LRW_DDR_PMU_EVENT_DCH1 1
#define LRW_DDR_PMU_EVENT_DOMAIN_CHB 0
#define LRW_DDR_PMU_EVENT_DOMAIN_DDRC 1
#define LRW_DDR_PMU_EVENT_TYPE_GENERAL 0
#define LRW_DDR_PMU_EVENT_TYPE_FIXED 1

#define LRW_DDR_PMU_EVENT_CONFIG_MASK GENMASK(9, 0)
#define LRW_DDR_PMU_EVENT_ID_MASK GENMASK(6, 0)

#define LRW_DDR_PMU_EVENT_DCH_SHIFT 7
#define LRW_DDR_PMU_EVENT_DOMAIN_SHIFT 8
#define LRW_DDR_PMU_EVENT_TYPE_SHIFT 9

#define LRW_DDR_PMU_EVENT_DCH_MASK BIT(LRW_DDR_PMU_EVENT_DCH_SHIFT)
#define LRW_DDR_PMU_EVENT_DOMAIN_MASK BIT(LRW_DDR_PMU_EVENT_DOMAIN_SHIFT)
#define LRW_DDR_PMU_EVENT_TYPE_MASK BIT(LRW_DDR_PMU_EVENT_TYPE_SHIFT)

#define LRW_DDR_PMU_GET_EVENT_CONFIG(event) \
	FIELD_GET(LRW_DDR_PMU_EVENT_CONFIG_MASK, (event)->attr.config)

#define LRW_DDR_PMU_GET_EVENT_ID(event) \
	FIELD_GET(LRW_DDR_PMU_EVENT_ID_MASK, (event)->attr.config)

#define LRW_DDR_PMU_GET_EVENT_DCH(event) \
	FIELD_GET(LRW_DDR_PMU_EVENT_DCH_MASK, (event)->attr.config)

#define LRW_DDR_PMU_GET_EVENT_DOMAIN(event) \
	FIELD_GET(LRW_DDR_PMU_EVENT_DOMAIN_MASK, (event)->attr.config)

#define LRW_DDR_PMU_GET_EVENT_TYPE(event) \
	FIELD_GET(LRW_DDR_PMU_EVENT_TYPE_MASK, (event)->attr.config)

#define LRW_DDR_PMU_GET_DOMAIN_OFFSET(event)     \
	((LRW_DDR_PMU_GET_EVENT_DOMAIN(event) == \
	  LRW_DDR_PMU_EVENT_DOMAIN_CHB) ?        \
		 LRW_DDR_PMU_CHB_DOMAIN_OFFSET : \
		 LRW_DDR_PMU_DDRC_DOMAIN_OFFSET)

#define LRW_DDR_PMU_GET_PMU_DOMAIN(event, pmu)   \
	((LRW_DDR_PMU_GET_EVENT_DOMAIN(event) == \
	  LRW_DDR_PMU_EVENT_DOMAIN_CHB) ?        \
		 &(pmu)->chb :                   \
		 &(pmu)->ddrc)

#define to_lrw_ddr_pmu(p) (container_of(p, struct lrw_ddr_pmu, pmu))

static int lrw_ddr_pmu_cpuhp_state_num;

static const struct acpi_device_id lrw_ddr_acpi_match[] = {
	{
		"LRWX0001",
	},
	{},
};
MODULE_DEVICE_TABLE(acpi, lrw_ddr_acpi_match);

struct lrw_ddr_pmu_domain {
	int num_counters;
	unsigned long *used_mask;
	struct perf_event **events;
	int *config;
};

struct lrw_ddr_pmu {
	void __iomem *cfg_base;
	struct device *dev;
	struct pmu pmu;
	int cpu;

	struct lrw_ddr_pmu_domain ddrc;
	struct lrw_ddr_pmu_domain chb;
	struct hlist_node node;
};

static ssize_t lrw_ddr_pmu_event_show(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *ext =
		container_of(attr, struct dev_ext_attribute, attr);
	return sprintf(buf, "config=0x%llx\n", (u64)ext->var);
}

static ssize_t lrw_ddr_pmu_cpumask_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(ddr_pmu->cpu));
}

static ssize_t lrw_ddr_pmu_format_show(struct device *dev,
				       struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *ext =
		container_of(attr, struct dev_ext_attribute, attr);
	return sprintf(buf, "%s\n", (char *)ext->var);
}

#define LRW_DDR_PMU_ATTR(_name, _show_func, _config)     \
	(&((struct dev_ext_attribute[]){                 \
		{ __ATTR(_name, 0444, _show_func, NULL), \
		  (void *)(_config) } })[0]              \
		  .attr.attr)

#define LRW_DDR_PMU_FORMAT_ATTR(_name, _config) \
	LRW_DDR_PMU_ATTR(_name, lrw_ddr_pmu_format_show, (void *)(_config))

#define LRW_DDR_PMU_EVENT_ATTR(_name, _config)          \
	LRW_DDR_PMU_ATTR(_name, lrw_ddr_pmu_event_show, \
			 (unsigned long)(_config))

#define LRW_DDR_PMU_EVENT_DCH0_SHIFTED \
	((LRW_DDR_PMU_EVENT_DCH0) << (LRW_DDR_PMU_EVENT_DCH_SHIFT))
#define LRW_DDR_PMU_EVENT_DCH1_SHIFTED \
	((LRW_DDR_PMU_EVENT_DCH1) << (LRW_DDR_PMU_EVENT_DCH_SHIFT))
#define LRW_DDR_PMU_EVENT_DOMAIN_CHB_SHIFTED \
	((LRW_DDR_PMU_EVENT_DOMAIN_CHB) << (LRW_DDR_PMU_EVENT_DOMAIN_SHIFT))
#define LRW_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED \
	((LRW_DDR_PMU_EVENT_DOMAIN_DDRC) << (LRW_DDR_PMU_EVENT_DOMAIN_SHIFT))

#define LRW_DDR_PMU_GEN_DDRC_EVENTS_0(prefix, cfg)                       \
	(LRW_DDR_PMU_EVENT_ATTR(ddrc_##prefix##_sub0,                    \
				((cfg) |                                 \
				 LRW_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED | \
				 LRW_DDR_PMU_EVENT_DCH0_SHIFTED)))

#define LRW_DDR_PMU_GEN_DDRC_EVENTS_1(prefix, cfg)                       \
	(LRW_DDR_PMU_EVENT_ATTR(ddrc_##prefix##_sub1,                    \
				((cfg) |                                 \
				 LRW_DDR_PMU_EVENT_DOMAIN_DDRC_SHIFTED | \
				 LRW_DDR_PMU_EVENT_DCH1_SHIFTED)))

#define LRW_DDR_PMU_GEN_FIXED_EVENT(prefix, cfg) \
	LRW_DDR_PMU_EVENT_ATTR(fixed_##prefix, (cfg))

static struct attribute *lrw_ddr_pmu_events_attrs[] = {
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rd_or_wr_ps0, 0),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rd_or_wr_ps1, 1),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_wr_ps0, 2),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_wr_ps1, 3),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rd_ps0, 4),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rd_ps1, 5),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rmw_ps0, 6),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_rmw_ps1, 7),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_hi_pri_rd_ps0, 8),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hif_hi_pri_rd_ps1, 9),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(dfi_wr_data_cycles_ps0, 10),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(dfi_wr_data_cycles_ps1, 11),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(dfi_rd_data_cycles_ps0, 12),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(dfi_rd_data_cycles_ps1, 13),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(hpr_xact_when_critical, 14),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(lpr_xact_when_critical, 15),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(wr_xact_when_critical, 16),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_activate_ps0, 17),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_activate_ps1, 18),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_or_wr_ps0, 19),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_or_wr_ps1, 20),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_activate_ps0, 21),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_activate_ps1, 22),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_ps0, 23),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_rd_ps1, 24),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_wr_ps0, 25),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_wr_ps1, 26),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_mwr_ps0, 27),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_mwr_ps1, 28),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_precharge_ps0, 29),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_precharge_ps1, 30),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_for_rdwr_ps0, 31),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_for_rdwr_ps1, 32),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_for_other_ps0, 33),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(precharge_for_other_ps1, 34),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(rdwr_transitions, 35),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(write_combine_ps0, 36),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(write_combine_ps1, 37),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(war_hazard, 38),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(raw_hazard, 39),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(waw_hazard, 40),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_selfref_rank0, 41),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_selfref_rank1, 42),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_selfref_rank2, 43),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_selfref_rank3, 44),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_powerdown_rank0, 45),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_powerdown_rank1, 46),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_powerdown_rank2, 47),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_enter_powerdown_rank3, 48),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_refresh_ps0, 53),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_refresh_ps1, 54),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_crit_ref_ps0, 55),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_crit_ref_ps1, 56),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_spec_ref_ps0, 57),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_spec_ref_ps1, 58),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_load_mode, 59),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(bsm_alloc, 60),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(bsm_starvation, 61),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(visible_window_limit_reached_rd, 62),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(visible_window_limit_reached_wr, 63),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_dqsosc_mpc, 64),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_dqsosc_mrr, 65),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_tcr_mrr, 66),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_zqstart, 67),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_0(op_is_zqlatch, 68),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rd_or_wr_ps0, 0),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rd_or_wr_ps1, 1),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_wr_ps0, 2),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_wr_ps1, 3),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rd_ps0, 4),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rd_ps1, 5),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rmw_ps0, 6),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_rmw_ps1, 7),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_hi_pri_rd_ps0, 8),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hif_hi_pri_rd_ps1, 9),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(dfi_wr_data_cycles_ps0, 10),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(dfi_wr_data_cycles_ps1, 11),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(dfi_rd_data_cycles_ps0, 12),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(dfi_rd_data_cycles_ps1, 13),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(hpr_xact_when_critical, 14),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(lpr_xact_when_critical, 15),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(wr_xact_when_critical, 16),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_activate_ps0, 17),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_activate_ps1, 18),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_or_wr_ps0, 19),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_or_wr_ps1, 20),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_activate_ps0, 21),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_activate_ps1, 22),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_ps0, 23),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_rd_ps1, 24),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_wr_ps0, 25),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_wr_ps1, 26),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_mwr_ps0, 27),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_mwr_ps1, 28),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_precharge_ps0, 29),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_precharge_ps1, 30),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_for_rdwr_ps0, 31),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_for_rdwr_ps1, 32),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_for_other_ps0, 33),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(precharge_for_other_ps1, 34),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(rdwr_transitions, 35),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(write_combine_ps0, 36),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(write_combine_ps1, 37),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(war_hazard, 38),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(raw_hazard, 39),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(waw_hazard, 40),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_selfref_rank0, 41),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_selfref_rank1, 42),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_selfref_rank2, 43),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_selfref_rank3, 44),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_powerdown_rank0, 45),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_powerdown_rank1, 46),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_powerdown_rank2, 47),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_enter_powerdown_rank3, 48),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_refresh_ps0, 53),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_refresh_ps1, 54),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_crit_ref_ps0, 55),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_crit_ref_ps1, 56),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_spec_ref_ps0, 57),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_spec_ref_ps1, 58),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_load_mode, 59),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(bsm_alloc, 60),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(bsm_starvation, 61),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(visible_window_limit_reached_rd, 62),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(visible_window_limit_reached_wr, 63),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_dqsosc_mpc, 64),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_dqsosc_mrr, 65),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_tcr_mrr, 66),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_zqstart, 67),
	LRW_DDR_PMU_GEN_DDRC_EVENTS_1(op_is_zqlatch, 68),
	LRW_DDR_PMU_GEN_FIXED_EVENT(chb_cycle,
				    LRW_DDR_PMU_FIXED_EVT_ID_CHB_CYCLE),
	LRW_DDR_PMU_GEN_FIXED_EVENT(ddrc_cycle,
				    LRW_DDR_PMU_FIXED_EVT_ID_DDRC_CYCLE),
	NULL,
};

static struct attribute_group lrw_ddr_pmu_events_attr_group = {
	.name = "events",
	.attrs = lrw_ddr_pmu_events_attrs,
};

static struct device_attribute lrw_ddr_pmu_cpumask_attr =
	__ATTR(cpumask, 0444, lrw_ddr_pmu_cpumask_show, NULL);

static struct attribute *lrw_ddr_pmu_cpumask_attrs[] = {
	&lrw_ddr_pmu_cpumask_attr.attr,
	NULL,
};

static const struct attribute_group lrw_ddr_pmu_cpumask_attr_group = {
	.attrs = lrw_ddr_pmu_cpumask_attrs,
};

static struct attribute *lrw_ddr_pmu_format_attrs[] = {
	LRW_DDR_PMU_FORMAT_ATTR(event, "config:0-6"),
	LRW_DDR_PMU_FORMAT_ATTR(dch, "config:7"),
	LRW_DDR_PMU_FORMAT_ATTR(domain, "config:8"),
	LRW_DDR_PMU_FORMAT_ATTR(type, "config:9"),
	NULL,
};

static const struct attribute_group lrw_ddr_pmu_format_group = {
	.name = "format",
	.attrs = lrw_ddr_pmu_format_attrs,
};

static const struct attribute_group *lrw_ddr_pmu_attr_groups[] = {
	&lrw_ddr_pmu_events_attr_group,
	&lrw_ddr_pmu_cpumask_attr_group,
	&lrw_ddr_pmu_format_group,
	NULL,
};

static bool lrw_ddr_pmu_is_support_event(struct perf_event *event)
{
	u32 id = LRW_DDR_PMU_GET_EVENT_ID(event);
	u32 domain = LRW_DDR_PMU_GET_EVENT_DOMAIN(event);

	switch (domain) {
	case LRW_DDR_PMU_EVENT_DOMAIN_CHB:
		return id <= LRW_DDR_PMU_MAX_CHB_EVENTS;
	case LRW_DDR_PMU_EVENT_DOMAIN_DDRC:
		return id <= LRW_DDR_PMU_MAX_DDRC_EVENTS;
	default:
		return false;
	}
}

static int lrw_ddr_pmu_get_event_idx(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct lrw_ddr_pmu_domain *domain;
	int idx;

	domain = LRW_DDR_PMU_GET_PMU_DOMAIN(event, ddr_pmu);
	for (idx = 0; idx < domain->num_counters; ++idx) {
		if (!test_and_set_bit(idx, domain->used_mask))
			return idx;
	}

	return -EBUSY;
}

static int lrw_ddr_pmu_event_init(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (is_sampling_event(event))
		return -EOPNOTSUPP;

	event->cpu = ddr_pmu->cpu;
	if (event->cpu < 0 || event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	if (!lrw_ddr_pmu_is_support_event(event))
		return -EOPNOTSUPP;

	hwc->idx = -1;

	return 0;
}

static void lrw_ddr_pmu_bind_counter(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 cfg = LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) + LRW_DDR_PMU_REG_CFG1;
	u32 val = 0;

	val = FIELD_PREP(LRW_DDR_PMU_EVENT_CFG1_BOUND_EN, 1) |
	      FIELD_PREP(LRW_DDR_PMU_EVENT_CFG1_CNT_ID, hwc->idx) |
	      FIELD_PREP(LRW_DDR_PMU_EVENT_CFG1_ID,
			 LRW_DDR_PMU_GET_EVENT_ID(event)) |
	      FIELD_PREP(LRW_DDR_PMU_EVENT_CFG1_DCH_SEL,
			 LRW_DDR_PMU_GET_EVENT_DCH(event));
	writel(val, ddr_pmu->cfg_base + cfg);
}

static void lrw_ddr_pmu_enable_counter(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 reg, val;

	reg = LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) + LRW_DDR_PMU_REG_CFG0;
	val = readl(ddr_pmu->cfg_base + reg);
	val |= LRW_DDR_PMU_CNT_ENABLE_BIT(hwc->idx);
	val |= LRW_DDR_PMU_CNT_CLEAR_BIT(hwc->idx);
	writel(val, ddr_pmu->cfg_base + reg);
}

static void lrw_ddr_pmu_disable_counter(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u32 reg, val;

	reg = LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) + LRW_DDR_PMU_REG_CFG0;
	val = readl(ddr_pmu->cfg_base + reg);

	val &= LRW_DDR_PMU_CNT_DISABLE_BIT(hwc->idx);
	writel(val, ddr_pmu->cfg_base + reg);
}

static u64 lrw_ddr_pmu_read_counter(struct perf_event *event)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	u64 cycle_high, cycle_low;

	if (LRW_DDR_PMU_GET_EVENT_TYPE(event) == LRW_DDR_PMU_EVENT_TYPE_FIXED) {
		cycle_high = readl(ddr_pmu->cfg_base +
				   LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				   LRW_DDR_PMU_REG_CYCLE_H);
		cycle_low = readl(ddr_pmu->cfg_base +
				  LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				  LRW_DDR_PMU_REG_CYCLE_L);
	} else {
		cycle_high = readl(ddr_pmu->cfg_base +
				   LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				   LRW_DDR_PMU_REG_CNT_H(hwc->idx));
		cycle_low = readl(ddr_pmu->cfg_base +
				  LRW_DDR_PMU_GET_DOMAIN_OFFSET(event) +
				  LRW_DDR_PMU_REG_CNT_L(hwc->idx));
	}

	return (u64)(cycle_high << 32 | cycle_low);
}

static void lrw_ddr_pmu_event_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 delta, prev, now;

	do {
		prev = local64_read(&hwc->prev_count);
		now = lrw_ddr_pmu_read_counter(event);
	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	delta = now - prev;
	local64_add(delta, &event->count);
}

static void lrw_ddr_pmu_start(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 pre_val;

	hwc->state = 0;

	if (LRW_DDR_PMU_GET_EVENT_TYPE(event) == LRW_DDR_PMU_EVENT_TYPE_FIXED) {
		pre_val = lrw_ddr_pmu_read_counter(event);
		local64_set(&hwc->prev_count, pre_val);
		return;
	}

	pre_val = LRW_DDR_PMU_CNT_INIT;
	local64_set(&hwc->prev_count, pre_val);

	lrw_ddr_pmu_bind_counter(event);
	lrw_ddr_pmu_enable_counter(event);
}

static int lrw_ddr_pmu_add(struct perf_event *event, int flags)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct lrw_ddr_pmu_domain *domain;
	struct hw_perf_event *hwc = &event->hw;
	int idx = -1;

	if (LRW_DDR_PMU_EVENT_TYPE_GENERAL ==
	    LRW_DDR_PMU_GET_EVENT_TYPE(event)) {
		idx = lrw_ddr_pmu_get_event_idx(event);
		if (idx < 0)
			return idx;

		/* allocate resources for general events */
		domain = LRW_DDR_PMU_GET_PMU_DOMAIN(event, ddr_pmu);
		domain->events[idx] = event;
		domain->config[idx] = LRW_DDR_PMU_GET_EVENT_CONFIG(event);
	}

	hwc->idx = idx;

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	if (flags & PERF_EF_START)
		lrw_ddr_pmu_start(event, PERF_EF_RELOAD);

	perf_event_update_userpage(event);

	return 0;
}

static void lrw_ddr_pmu_stop(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	if (LRW_DDR_PMU_EVENT_TYPE_GENERAL ==
	    LRW_DDR_PMU_GET_EVENT_TYPE(event)) {
		lrw_ddr_pmu_disable_counter(event);
	}

	lrw_ddr_pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static void lrw_ddr_pmu_del(struct perf_event *event, int flags)
{
	struct lrw_ddr_pmu *ddr_pmu = to_lrw_ddr_pmu(event->pmu);
	struct lrw_ddr_pmu_domain *domain;
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	lrw_ddr_pmu_stop(event, PERF_EF_UPDATE);

	domain = LRW_DDR_PMU_GET_PMU_DOMAIN(event, ddr_pmu);
	/* release resources for general events */
	if (idx >= 0 && idx < domain->num_counters) {
		domain->events[idx] = NULL;
		domain->config[idx] = -1;
		clear_bit(idx, domain->used_mask);
		hwc->idx = -1;
	}
	perf_event_update_userpage(event);
}

static void lrw_ddr_pmu_read(struct perf_event *event)
{
	lrw_ddr_pmu_event_update(event);
}

static int lrw_ddr_pmu_domain_init(struct lrw_ddr_pmu_domain *domain,
				   struct device *dev, int num_counters)
{
	if (!domain || !dev || num_counters <= 0 ||
	    num_counters > LRW_DDR_PMU_MAX_COUNTERS)
		return -EINVAL;

	domain->num_counters = num_counters;

	domain->used_mask = devm_kzalloc(
		dev, BITS_TO_LONGS(num_counters) * sizeof(u64), GFP_KERNEL);
	if (!domain->used_mask)
		return -ENOMEM;

	domain->events = devm_kzalloc(
		dev, num_counters * sizeof(struct perf_event *), GFP_KERNEL);
	if (!domain->events)
		return -ENOMEM;

	domain->config =
		devm_kzalloc(dev, num_counters * sizeof(int), GFP_KERNEL);
	if (!domain->config)
		return -ENOMEM;

	return 0;
}

static int lrw_ddr_pmu_probe(struct platform_device *pdev)
{
	struct lrw_ddr_pmu *ddr_pmu;
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

	name = devm_kasprintf(ddr_pmu->dev, GFP_KERNEL, "lrw_ddr_%llx",
			      (u64)(res->start >> LRW_DDR_PMU_PA_SHIFT));

	if (!name)
		return -ENOMEM;

	ddr_pmu->cpu = smp_processor_id();

	ret = lrw_ddr_pmu_domain_init(&ddr_pmu->ddrc, ddr_pmu->dev,
				      LRW_DDR_PMU_MAX_COUNTERS);
	if (ret)
		return ret;

	ret = lrw_ddr_pmu_domain_init(&ddr_pmu->chb, ddr_pmu->dev,
				      LRW_DDR_PMU_MAX_COUNTERS);
	if (ret)
		return ret;

	/* clear all chb counters */
	writel(LRW_DDR_PMU_CNT_CLEAR_MASK,
	       ddr_pmu->cfg_base + LRW_DDR_PMU_CHB_DOMAIN_OFFSET +
		       LRW_DDR_PMU_REG_CFG0);
	/* clear all ddrc counters */
	writel(LRW_DDR_PMU_CNT_CLEAR_MASK,
	       ddr_pmu->cfg_base + LRW_DDR_PMU_DDRC_DOMAIN_OFFSET +
		       LRW_DDR_PMU_REG_CFG0);

	ddr_pmu->pmu = (struct pmu){
		.module = THIS_MODULE,
		.parent = &pdev->dev,
		.task_ctx_nr = perf_invalid_context,
		.event_init = lrw_ddr_pmu_event_init,
		.add = lrw_ddr_pmu_add,
		.del = lrw_ddr_pmu_del,
		.start = lrw_ddr_pmu_start,
		.stop = lrw_ddr_pmu_stop,
		.read = lrw_ddr_pmu_read,
		.attr_groups = lrw_ddr_pmu_attr_groups,
		.capabilities = PERF_PMU_CAP_NO_EXCLUDE,
	};

	ret = perf_pmu_register(&ddr_pmu->pmu, name, -1);
	if (ret)
		dev_err(ddr_pmu->dev, "LRW DDR PMU register failed!\n");

	return ret;
}

static int lrw_ddr_pmu_remove(struct platform_device *pdev)
{
	struct lrw_ddr_pmu *ddr_pmu = platform_get_drvdata(pdev);

	perf_pmu_unregister(&ddr_pmu->pmu);

	return 0;
}

static struct platform_driver lrw_ddr_pmu_driver = {
	.driver = {
		.name = "lrw_ddr_pmu",
		   .acpi_match_table = lrw_ddr_acpi_match,
		   },
	.probe = lrw_ddr_pmu_probe,
	.remove = lrw_ddr_pmu_remove,
};

static int lrw_ddr_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct lrw_ddr_pmu *pmu =
		hlist_entry_safe(node, struct lrw_ddr_pmu, node);
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

static int __init lrw_ddr_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN, "lrw_ddr_pmu:online",
				      NULL, lrw_ddr_pmu_offline_cpu);

	if (ret < 0) {
		pr_err("LRW DDR PMU: setup hotplug failed, ret = %d\n", ret);
		return ret;
	}
	lrw_ddr_pmu_cpuhp_state_num = ret;

	ret = platform_driver_register(&lrw_ddr_pmu_driver);
	if (ret)
		cpuhp_remove_multi_state(lrw_ddr_pmu_cpuhp_state_num);

	return ret;
}
module_init(lrw_ddr_pmu_init);

static void __exit lrw_ddr_pmu_exit(void)
{
	platform_driver_unregister(&lrw_ddr_pmu_driver);
	cpuhp_remove_multi_state(lrw_ddr_pmu_cpuhp_state_num);
}
module_exit(lrw_ddr_pmu_exit);

MODULE_AUTHOR("Wenjia Guo");
MODULE_AUTHOR("Yong Ma");
MODULE_AUTHOR("Jie Feng");
MODULE_DESCRIPTION("LRW DDR PMU driver");
MODULE_LICENSE("GPL");
