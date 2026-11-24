// SPDX-License-Identifier: GPL-2.0
/*
 * Xuange DDR Sub-System PMU driver
 *
 * Copyright (C) 2026 Alibaba Inc
 */

#define XUANGE_DDR_PMUNAME		"xuange_ddr"
#define XUANGE_DDR_DRVNAME		XUANGE_DDR_PMUNAME "_pmu"
#define pr_fmt(fmt)		XUANGE_DDR_DRVNAME ": " fmt

#include <linux/acpi.h>
#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/cpuhotplug.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/printk.h>

#define XUANGE_DDR_PMU_COMMON_MAX_COUNTERS		\
	(XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS + XUANGE_DDR_PMU_DBG_EVENT_MAX_COUNTERS)
#define XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS	16
#define XUANGE_DDR_PMU_DBG_EVENT_MAX_COUNTERS	16

#define XUANGE_DDR_PMU_DBG_CYCLE_CNT_MASK	GENMASK(15, 0)

#define XUANGE_DDR_PMU_CNT_CTRL			0x0

#define XUANGE_DDR_PMU_DURATION_LOW		0x4
#define XUANGE_DDR_PMU_DURATION_HIGH		0x8

#define XUANGE_DDR_PMU_STAT_EN			0xC

#define XUANGE_DDR_PMU_EVT_SEL			0x10

#define XUANGE_DDR_PMU_SWCTRL			0x1C

/* PMU COMMON COUNTER 0-15, are paired in 64-bit registers on a 8-byte stride */
#define XUANGE_DDR_PMU_COMMON_COUNTER0		0x30
#define XUANGE_DDR_PMU_COMMON_COUNTER_N_LOW(n) \
	(XUANGE_DDR_PMU_COMMON_COUNTER0 + 0x8 * (n))

#define XUANGE_DDR_PMU_COMMON_COUNTER_N_HIGH(n) \
	(XUANGE_DDR_PMU_COMMON_COUNTER0 + 0x8 * (n) + 4)

/* Debug slots 16-31: min [15:0] and max [31:16], one 32-bit word per slot. */
#define XUANGE_DDR_PMU_DBG_COMMON_COUNTER_N(n) \
	(XUANGE_DDR_PMU_COMMON_COUNTER0 + 0x8 * 16 + 0x4 * ((n) - 16))

static int xuange_ddr_cpuhp_state_num;

/* Structure to manage min-max pair for debug events */
struct xuange_ddr_debug_event_status {
	struct perf_event *events[2]; /* min/max owners, valid until del(). */
	bool snapshot_valid;
	bool running;
	u32 shadow_min;
	u32 shadow_max;
};

struct xuange_ddr_pmu {
	void	__iomem *cfg_base;
	struct	device *dev;
	int		cpu;
	struct hlist_node	node;	/* CPU hotplug instance list node */
	DECLARE_BITMAP(event_used, XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS);
	struct	pmu pmu;
	raw_spinlock_t reg_lock;
	/* Status for 16 debug min/max pairs. */
	struct xuange_ddr_debug_event_status dbg_sts[XUANGE_DDR_PMU_DBG_EVENT_MAX_COUNTERS];
	u32 dbg_used;
};

#define to_xuange_ddr_pmu(p) (container_of(p, struct xuange_ddr_pmu, pmu))

#define XUANGE_DDR_CONFIG_EVENTID		GENMASK(7, 0)
#define GET_XUANGE_DDR_EVENTID(event)	FIELD_GET(XUANGE_DDR_CONFIG_EVENTID, (event)->attr.config)

#define XUANGE_DDR_PMU_EVENT_ATTR(_name, _config) \
	PMU_EVENT_ATTR_STRING(_name, xuange_ddr_event_attr_##_name, "event=" __stringify(_config))

XUANGE_DDR_PMU_EVENT_ATTR(hif_wr,				0x0);
XUANGE_DDR_PMU_EVENT_ATTR(hpr_xact_when_critical,		0x1);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_selfref_rk0,		0x2);
XUANGE_DDR_PMU_EVENT_ATTR(hif_rd_or_wr,			0x3);
XUANGE_DDR_PMU_EVENT_ATTR(hif_rd,				0x4);
XUANGE_DDR_PMU_EVENT_ATTR(lpr_xact_when_critical,		0x5);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_selfref_rk1,		0x6);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_load_mode,			0x7);
XUANGE_DDR_PMU_EVENT_ATTR(hif_rmw,				0x8);
XUANGE_DDR_PMU_EVENT_ATTR(wr_xact_when_critical,		0x9);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_selfref_rk2,		0xa);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_tcr_mrr,			0xb);
XUANGE_DDR_PMU_EVENT_ATTR(dfi_wr_data_cycles,		0xc);
XUANGE_DDR_PMU_EVENT_ATTR(visible_window_limit_reached_rd, 0xd);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_selfref_rk3,		0xe);
XUANGE_DDR_PMU_EVENT_ATTR(precharge_for_other,		0xf);
XUANGE_DDR_PMU_EVENT_ATTR(dfi_rd_data_cycles,		0x10);
XUANGE_DDR_PMU_EVENT_ATTR(visible_window_limit_reached_wr, 0x11);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_powerdown_rk0,	0x12);
XUANGE_DDR_PMU_EVENT_ATTR(chb_hif_rcmd_src_prefetch,	0x13);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_activate,			0x14);
XUANGE_DDR_PMU_EVENT_ATTR(hif_hi_pri_rd,			0x15);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_powerdown_rk1,	0x16);
XUANGE_DDR_PMU_EVENT_ATTR(chb_chi_req_dmdhit_drop,		0x17);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_rd_activate,		0x18);
XUANGE_DDR_PMU_EVENT_ATTR(war_hazard,			0x19);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_powerdown_rk2,	0x1a);
XUANGE_DDR_PMU_EVENT_ATTR(chb_chi_req_dmdhit_nodrop,	0x1b);
XUANGE_DDR_PMU_EVENT_ATTR(precharge_for_rdwr,		0x1c);
XUANGE_DDR_PMU_EVENT_ATTR(raw_hazard,			0x1d);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_powerdown_rk3,	0x1e);
XUANGE_DDR_PMU_EVENT_ATTR(chb_hif_rcmd_src_sbr,		0x1f);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_precharge,			0x20);
XUANGE_DDR_PMU_EVENT_ATTR(waw_hazard,			0x21);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_mpsm_rk0,		0x22);
XUANGE_DDR_PMU_EVENT_ATTR(chb_hif_wcmd_src_sbr,		0x23);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_refresh,			0x24);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_dqsosc_mpc,		0x25);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_mpsm_rk1,		0x26);
XUANGE_DDR_PMU_EVENT_ATTR(chb_valid_hif_rcmd_p0,		0x27);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_rd_or_wr,			0x28);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_dqsosc_mrr,		0x29);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_mpsm_rk2,		0x2a);
XUANGE_DDR_PMU_EVENT_ATTR(chb_valid_hif_wcmd_p0,		0x2b);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_rd,			0x2c);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_crit_ref,			0x2d);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_enter_mpsm_rk3,		0x2e);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_wr,			0x30);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_spec_ref,			0x31);
XUANGE_DDR_PMU_EVENT_ATTR(selfref_mode_rk0,		0x32);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_mwr,			0x34);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_rfm,			0x35);
XUANGE_DDR_PMU_EVENT_ATTR(selfref_mode_rk1,		0x36);
XUANGE_DDR_PMU_EVENT_ATTR(rdwr_transitions,		0x38);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_zqstart,			0x39);
XUANGE_DDR_PMU_EVENT_ATTR(selfref_mode_rk2,		0x3a);
XUANGE_DDR_PMU_EVENT_ATTR(write_combine,			0x3c);
XUANGE_DDR_PMU_EVENT_ATTR(op_is_zqlatch,			0x3d);

XUANGE_DDR_PMU_EVENT_ATTR(hif_hpr_crd_cnt_min,		0x40);
XUANGE_DDR_PMU_EVENT_ATTR(hif_hpr_crd_cnt_max,		0x41);
XUANGE_DDR_PMU_EVENT_ATTR(hif_lpr_crd_cnt_min,		0x44);
XUANGE_DDR_PMU_EVENT_ATTR(hif_lpr_crd_cnt_max,		0x45);
XUANGE_DDR_PMU_EVENT_ATTR(hif_wr_crd_cnt_min,		0x48);
XUANGE_DDR_PMU_EVENT_ATTR(hif_wr_crd_cnt_max,		0x49);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_hpr_fill_level_min,	0x4c);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_hpr_fill_level_max,	0x4d);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_lpr_fill_level_min,	0x50);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_lpr_fill_level_max,	0x51);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_vpr_fill_level_min,	0x54);
XUANGE_DDR_PMU_EVENT_ATTR(rpq_vpr_fill_level_max,	0x55);
XUANGE_DDR_PMU_EVENT_ATTR(wpq_fill_level_min,	0x58);
XUANGE_DDR_PMU_EVENT_ATTR(wpq_fill_level_max,		0x59);
XUANGE_DDR_PMU_EVENT_ATTR(rxdat_flitq_fill_level_min,	0x5c);
XUANGE_DDR_PMU_EVENT_ATTR(rxdat_flitq_fill_level_max,	0x5d);
XUANGE_DDR_PMU_EVENT_ATTR(rxreq_flitq_fill_level_min,	0x60);
XUANGE_DDR_PMU_EVENT_ATTR(rxreq_flitq_fill_level_max,	0x61);
XUANGE_DDR_PMU_EVENT_ATTR(txdat_flitq_fill_level_min,	0x64);
XUANGE_DDR_PMU_EVENT_ATTR(txdat_flitq_fill_level_max,	0x65);
XUANGE_DDR_PMU_EVENT_ATTR(txrsp_flitq_fill_level_min,	0x68);
XUANGE_DDR_PMU_EVENT_ATTR(txrsp_flitq_fill_level_max,	0x69);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_hpr_fill_level_min,	0x6c);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_hpr_fill_level_max,	0x6d);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_lpr_fill_level_min,	0x70);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_lpr_fill_level_max,	0x71);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_npr_fill_level_min,	0x74);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_npr_fill_level_max,	0x75);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_vpr_fill_level_min,	0x78);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_vpr_fill_level_max,	0x79);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_vpw_fill_level_min,	0x7c);
XUANGE_DDR_PMU_EVENT_ATTR(rtlst_vpw_fill_level_max,	0x7d);

/* Debug min/max values are snapshots, not cumulative counters. */
PMU_EVENT_ATTR_STRING(hif_hpr_crd_cnt_min.snapshot,
		      xuange_ddr_event_attr_hif_hpr_crd_cnt_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(hif_hpr_crd_cnt_max.snapshot,
		      xuange_ddr_event_attr_hif_hpr_crd_cnt_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(hif_lpr_crd_cnt_min.snapshot,
		      xuange_ddr_event_attr_hif_lpr_crd_cnt_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(hif_lpr_crd_cnt_max.snapshot,
		      xuange_ddr_event_attr_hif_lpr_crd_cnt_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(hif_wr_crd_cnt_min.snapshot,
		      xuange_ddr_event_attr_hif_wr_crd_cnt_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(hif_wr_crd_cnt_max.snapshot,
		      xuange_ddr_event_attr_hif_wr_crd_cnt_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_hpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rpq_hpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_hpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rpq_hpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_lpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rpq_lpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_lpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rpq_lpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_vpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rpq_vpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rpq_vpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rpq_vpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(wpq_fill_level_min.snapshot,
		      xuange_ddr_event_attr_wpq_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(wpq_fill_level_max.snapshot,
		      xuange_ddr_event_attr_wpq_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rxdat_flitq_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rxdat_flitq_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rxdat_flitq_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rxdat_flitq_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rxreq_flitq_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rxreq_flitq_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rxreq_flitq_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rxreq_flitq_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(txdat_flitq_fill_level_min.snapshot,
		      xuange_ddr_event_attr_txdat_flitq_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(txdat_flitq_fill_level_max.snapshot,
		      xuange_ddr_event_attr_txdat_flitq_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(txrsp_flitq_fill_level_min.snapshot,
		      xuange_ddr_event_attr_txrsp_flitq_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(txrsp_flitq_fill_level_max.snapshot,
		      xuange_ddr_event_attr_txrsp_flitq_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_hpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rtlst_hpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_hpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rtlst_hpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_lpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rtlst_lpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_lpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rtlst_lpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_npr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rtlst_npr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_npr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rtlst_npr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_vpr_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rtlst_vpr_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_vpr_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rtlst_vpr_fill_level_max_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_vpw_fill_level_min.snapshot,
		      xuange_ddr_event_attr_rtlst_vpw_fill_level_min_snapshot, "1");
PMU_EVENT_ATTR_STRING(rtlst_vpw_fill_level_max.snapshot,
		      xuange_ddr_event_attr_rtlst_vpw_fill_level_max_snapshot, "1");

static struct attribute *xuange_ddr_pmu_events_attrs[] = {
	/* single-bit events, every four events share same counter */
	&xuange_ddr_event_attr_hif_wr.attr.attr,
	&xuange_ddr_event_attr_hpr_xact_when_critical.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_selfref_rk0.attr.attr,
	&xuange_ddr_event_attr_hif_rd_or_wr.attr.attr,
	&xuange_ddr_event_attr_hif_rd.attr.attr,
	&xuange_ddr_event_attr_lpr_xact_when_critical.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_selfref_rk1.attr.attr,
	&xuange_ddr_event_attr_op_is_load_mode.attr.attr,
	&xuange_ddr_event_attr_hif_rmw.attr.attr,
	&xuange_ddr_event_attr_wr_xact_when_critical.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_selfref_rk2.attr.attr,
	&xuange_ddr_event_attr_op_is_tcr_mrr.attr.attr,
	&xuange_ddr_event_attr_dfi_wr_data_cycles.attr.attr,
	&xuange_ddr_event_attr_visible_window_limit_reached_rd.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_selfref_rk3.attr.attr,
	&xuange_ddr_event_attr_precharge_for_other.attr.attr,
	&xuange_ddr_event_attr_dfi_rd_data_cycles.attr.attr,
	&xuange_ddr_event_attr_visible_window_limit_reached_wr.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_powerdown_rk0.attr.attr,
	&xuange_ddr_event_attr_chb_hif_rcmd_src_prefetch.attr.attr,
	&xuange_ddr_event_attr_op_is_activate.attr.attr,
	&xuange_ddr_event_attr_hif_hi_pri_rd.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_powerdown_rk1.attr.attr,
	&xuange_ddr_event_attr_chb_chi_req_dmdhit_drop.attr.attr,
	&xuange_ddr_event_attr_op_is_rd_activate.attr.attr,
	&xuange_ddr_event_attr_war_hazard.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_powerdown_rk2.attr.attr,
	&xuange_ddr_event_attr_chb_chi_req_dmdhit_nodrop.attr.attr,
	&xuange_ddr_event_attr_precharge_for_rdwr.attr.attr,
	&xuange_ddr_event_attr_raw_hazard.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_powerdown_rk3.attr.attr,
	&xuange_ddr_event_attr_chb_hif_rcmd_src_sbr.attr.attr,
	&xuange_ddr_event_attr_op_is_precharge.attr.attr,
	&xuange_ddr_event_attr_waw_hazard.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_mpsm_rk0.attr.attr,
	&xuange_ddr_event_attr_chb_hif_wcmd_src_sbr.attr.attr,
	&xuange_ddr_event_attr_op_is_refresh.attr.attr,
	&xuange_ddr_event_attr_op_is_dqsosc_mpc.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_mpsm_rk1.attr.attr,
	&xuange_ddr_event_attr_chb_valid_hif_rcmd_p0.attr.attr,
	&xuange_ddr_event_attr_op_is_rd_or_wr.attr.attr,
	&xuange_ddr_event_attr_op_is_dqsosc_mrr.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_mpsm_rk2.attr.attr,
	&xuange_ddr_event_attr_chb_valid_hif_wcmd_p0.attr.attr,
	&xuange_ddr_event_attr_op_is_rd.attr.attr,
	&xuange_ddr_event_attr_op_is_crit_ref.attr.attr,
	&xuange_ddr_event_attr_op_is_enter_mpsm_rk3.attr.attr,
	&xuange_ddr_event_attr_op_is_wr.attr.attr,
	&xuange_ddr_event_attr_op_is_spec_ref.attr.attr,
	&xuange_ddr_event_attr_selfref_mode_rk0.attr.attr,
	&xuange_ddr_event_attr_op_is_mwr.attr.attr,
	&xuange_ddr_event_attr_op_is_rfm.attr.attr,
	&xuange_ddr_event_attr_selfref_mode_rk1.attr.attr,
	&xuange_ddr_event_attr_rdwr_transitions.attr.attr,
	&xuange_ddr_event_attr_op_is_zqstart.attr.attr,
	&xuange_ddr_event_attr_selfref_mode_rk2.attr.attr,
	&xuange_ddr_event_attr_write_combine.attr.attr,
	&xuange_ddr_event_attr_op_is_zqlatch.attr.attr,

	/*
	 * multi-bit events, events and counters have a one-to-one correspondence
	 * counter only record max min data
	 */
	&xuange_ddr_event_attr_hif_hpr_crd_cnt_min.attr.attr,
	&xuange_ddr_event_attr_hif_hpr_crd_cnt_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_hif_hpr_crd_cnt_max.attr.attr,
	&xuange_ddr_event_attr_hif_hpr_crd_cnt_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_hif_lpr_crd_cnt_min.attr.attr,
	&xuange_ddr_event_attr_hif_lpr_crd_cnt_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_hif_lpr_crd_cnt_max.attr.attr,
	&xuange_ddr_event_attr_hif_lpr_crd_cnt_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_hif_wr_crd_cnt_min.attr.attr,
	&xuange_ddr_event_attr_hif_wr_crd_cnt_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_hif_wr_crd_cnt_max.attr.attr,
	&xuange_ddr_event_attr_hif_wr_crd_cnt_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_hpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rpq_hpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_hpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rpq_hpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_lpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rpq_lpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_lpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rpq_lpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_vpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rpq_vpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rpq_vpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rpq_vpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_wpq_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_wpq_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_wpq_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_wpq_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rxdat_flitq_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rxdat_flitq_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rxdat_flitq_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rxdat_flitq_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rxreq_flitq_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rxreq_flitq_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rxreq_flitq_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rxreq_flitq_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_txdat_flitq_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_txdat_flitq_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_txdat_flitq_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_txdat_flitq_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_txrsp_flitq_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_txrsp_flitq_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_txrsp_flitq_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_txrsp_flitq_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_hpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rtlst_hpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_hpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rtlst_hpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_lpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rtlst_lpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_lpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rtlst_lpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_npr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rtlst_npr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_npr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rtlst_npr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpr_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpr_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpr_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpr_fill_level_max_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpw_fill_level_min.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpw_fill_level_min_snapshot.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpw_fill_level_max.attr.attr,
	&xuange_ddr_event_attr_rtlst_vpw_fill_level_max_snapshot.attr.attr,
	NULL,
};

static struct attribute_group xuange_ddr_pmu_events_attr_group = {
	.name = "events",
	.attrs = xuange_ddr_pmu_events_attrs,
};

PMU_FORMAT_ATTR(event, "config:0-7");
static struct attribute *xuange_ddr_pmu_format_attr[] = {
	&format_attr_event.attr,
	NULL,
};

static const struct attribute_group xuange_ddr_pmu_format_group = {
	.name = "format",
	.attrs = xuange_ddr_pmu_format_attr,
};

static ssize_t xuange_ddr_pmu_cpumask_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(ddr_pmu->cpu));
}

static struct device_attribute xuange_ddr_pmu_cpumask_attr =
		__ATTR(cpumask, 0444, xuange_ddr_pmu_cpumask_show, NULL);

static struct attribute *xuange_ddr_pmu_cpumask_attrs[] = {
	&xuange_ddr_pmu_cpumask_attr.attr,
	NULL,
};

static const struct attribute_group xuange_ddr_pmu_cpumask_attr_group = {
	.attrs = xuange_ddr_pmu_cpumask_attrs,
};

static ssize_t xuange_ddr_pmu_identifier_show(struct device *dev,
					      struct device_attribute *attr,
					      char *page)
{
	return sysfs_emit(page, "%s\n", "xuange_ddr_pmu");
}

static struct device_attribute xuange_ddr_pmu_identifier_attr =
	__ATTR(identifier, 0444, xuange_ddr_pmu_identifier_show, NULL);

static struct attribute *xuange_ddr_pmu_identifier_attrs[] = {
	&xuange_ddr_pmu_identifier_attr.attr,
	NULL
};

static const struct attribute_group xuange_ddr_pmu_identifier_attr_group = {
	.attrs = xuange_ddr_pmu_identifier_attrs,
};

static const struct attribute_group *xuange_ddr_pmu_attr_groups[] = {
	&xuange_ddr_pmu_events_attr_group,
	&xuange_ddr_pmu_cpumask_attr_group,
	&xuange_ddr_pmu_format_group,
	&xuange_ddr_pmu_identifier_attr_group,
	NULL,
};

/* find a counter for event, then in add func, hw.idx will equal to counter */
static int xuange_ddr_get_counter_idx(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	int idx = GET_XUANGE_DDR_EVENTID(event) / 4;

	if (idx < XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS) {
		if (!test_and_set_bit(idx, ddr_pmu->event_used))
			return idx;
	} else if (idx < XUANGE_DDR_PMU_COMMON_MAX_COUNTERS) {
		unsigned int dbg_idx = idx - XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS;
		struct xuange_ddr_debug_event_status *dbg_sts = &ddr_pmu->dbg_sts[dbg_idx];
		unsigned int member = GET_XUANGE_DDR_EVENTID(event) & 1;
		struct perf_event *other;
		u32 dbg_shift = dbg_idx * 2;
		u32 dbg_slot = 1U << (dbg_shift + member);
		int ret = idx;

		raw_spin_lock(&ddr_pmu->reg_lock);
		if (ddr_pmu->dbg_used & dbg_slot) {
			ret = -EBUSY;
			goto out_unlock;
		}
		/*
		 * Keep the scheduled member, not its leader: perf can promote a
		 * sibling when the leader is closed. Read its current group here.
		 * del() clears each member pointer before that event can be freed.
		 */
		other = dbg_sts->events[member ^ 1];
		if (other && other->group_leader != event->group_leader) {
			ret = -ENOSPC;
			goto out_unlock;
		}
		dbg_sts->events[member] = event;
		ddr_pmu->dbg_used |= dbg_slot;
out_unlock:
		raw_spin_unlock(&ddr_pmu->reg_lock);
		return ret;
	} else {
		return -EINVAL;
	}

	return -EBUSY;
}

static u64 xuange_ddr_pmu_read_counter(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	u64 cycle_high, cycle_low;
	u32 high_check;

	/* Only ordinary counters use this path; debug pairs are read in stop(). */
	/* Retry if the low word rolled over between the MMIO reads. */
	do {
		cycle_high = readl(ddr_pmu->cfg_base +
			XUANGE_DDR_PMU_COMMON_COUNTER_N_HIGH(event->hw.idx));
		cycle_low = readl(ddr_pmu->cfg_base +
			XUANGE_DDR_PMU_COMMON_COUNTER_N_LOW(event->hw.idx));
		high_check = readl(ddr_pmu->cfg_base +
			XUANGE_DDR_PMU_COMMON_COUNTER_N_HIGH(event->hw.idx));
	} while (cycle_high != high_check);

	cycle_low |= cycle_high << 32;

	return cycle_low;
}

static void xuange_ddr_pmu_config_default(struct xuange_ddr_pmu *ddr_pmu)
{
	writel(0xFFFFFFFF, ddr_pmu->cfg_base + XUANGE_DDR_PMU_SWCTRL);
	ndelay(100);
	writel(0, ddr_pmu->cfg_base + XUANGE_DDR_PMU_SWCTRL);
	/* Establish an idle configuration even after a previous kernel. */
	writel(0, ddr_pmu->cfg_base + XUANGE_DDR_PMU_STAT_EN);
	writel(0, ddr_pmu->cfg_base + XUANGE_DDR_PMU_EVT_SEL);
	writel(0xFFFFFFFF, ddr_pmu->cfg_base + XUANGE_DDR_PMU_DURATION_HIGH);
	writel(0xFFFFFFFF, ddr_pmu->cfg_base + XUANGE_DDR_PMU_DURATION_LOW);
	writel(0, ddr_pmu->cfg_base + XUANGE_DDR_PMU_CNT_CTRL);
}

static void xuange_ddr_pmu_event_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 delta, prev, now;

	do {
		prev = local64_read(&hwc->prev_count);
		now = xuange_ddr_pmu_read_counter(event);
	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	/* handle overflow. */
	delta = now - prev;

	local64_add(delta, &event->count);
}

/* Caller holds reg_lock across the shared state and register update. */
static void xuange_ddr_pmu_stat_en_locked(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	u32 enable_reg;

	lockdep_assert_held(&ddr_pmu->reg_lock);
	enable_reg = readl(ddr_pmu->cfg_base + XUANGE_DDR_PMU_STAT_EN);
	enable_reg |= 1U << event->hw.idx;
	writel(enable_reg, ddr_pmu->cfg_base + XUANGE_DDR_PMU_STAT_EN);
}

static void xuange_ddr_pmu_enable_counter(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	int idx = event->hw.idx;

	raw_spin_lock(&ddr_pmu->reg_lock);
	/*
	 * Probe resets the internal counters. Clearing STAT_EN also resets
	 * the corresponding internal counter, while preserving its latched
	 * result register. Each subsequent measurement therefore starts at
	 * zero; the old latched result must not be used as the new baseline.
	 */
	local64_set(&event->hw.prev_count, 0);
	if (idx >= XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS) {
		struct xuange_ddr_debug_event_status *dbg_sts =
			&ddr_pmu->dbg_sts[idx - XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS];

		local64_set(&event->count, 0);
		if (!dbg_sts->running) {
			dbg_sts->snapshot_valid = false;
			xuange_ddr_pmu_stat_en_locked(event);
			dbg_sts->running = true;
		}
	} else {
		xuange_ddr_pmu_stat_en_locked(event);
	}
	raw_spin_unlock(&ddr_pmu->reg_lock);
}

static void xuange_ddr_pmu_disable_counter_locked(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	u32 enable_reg;

	lockdep_assert_held(&ddr_pmu->reg_lock);
	enable_reg = readl(ddr_pmu->cfg_base + XUANGE_DDR_PMU_STAT_EN);
	enable_reg &= ~(1U << event->hw.idx);
	writel(enable_reg, ddr_pmu->cfg_base + XUANGE_DDR_PMU_STAT_EN);
}

/* Only event IDs defined by the hardware event table are accepted. */
static bool xuange_ddr_pmu_event_valid(u64 config)
{
	unsigned int id = config;

	if (config & ~XUANGE_DDR_CONFIG_EVENTID)
		return false;

	if (id < 0x40) {
		switch (id) {
		case 0x2f: /* Reserved function selections. */
		case 0x33:
		case 0x37:
		case 0x3b:
		case 0x3e:
		case 0x3f:
			return false;
		default:
			return true;
		}
	}

	/* Each debug slot exposes only min (0) and max (1). */
	return id < 0x80 && (id & 0x3) < 2;
}

static int xuange_ddr_pmu_event_init(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	if (!xuange_ddr_pmu_event_valid(event->attr.config) ||
	    event->attr.config1 || event->attr.config2)
		return -EINVAL;

	if (is_sampling_event(event))
		return -EOPNOTSUPP;

	if (event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	event->cpu = ddr_pmu->cpu;
	hwc->idx = -1;

	return 0;
}

static void xuange_ddr_pmu_start(struct perf_event *event, int flags)
{
	event->hw.state = 0;
	xuange_ddr_pmu_enable_counter(event);
}

static void xuange_ddr_pmu_stop(struct perf_event *event, int flags)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	int idx = event->hw.idx;

	if (event->hw.state & PERF_HES_STOPPED)
		return;

	if (idx < XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS) {
		raw_spin_lock(&ddr_pmu->reg_lock);
		xuange_ddr_pmu_disable_counter_locked(event);
		raw_spin_unlock(&ddr_pmu->reg_lock);
		xuange_ddr_pmu_event_update(event);
	} else {
		struct xuange_ddr_debug_event_status *dbg_sts =
			&ddr_pmu->dbg_sts[idx - XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS];
		u32 dbg_result, value;

		/*
		 * A debug pair shares STAT_EN: the first stop latches both values
		 * and subsequent stops consume that snapshot. This supports min
		 * and max being stopped together as a perf group. Independently
		 * stopping/restarting a sibling while its partner remains active
		 * is not supported; group membership does not prevent that API use.
		 */
		raw_spin_lock(&ddr_pmu->reg_lock);
		if (dbg_sts->running) {
			xuange_ddr_pmu_disable_counter_locked(event);
			dbg_result = readl(ddr_pmu->cfg_base +
					   XUANGE_DDR_PMU_DBG_COMMON_COUNTER_N(idx));
			dbg_sts->shadow_min = dbg_result & XUANGE_DDR_PMU_DBG_CYCLE_CNT_MASK;
			dbg_sts->shadow_max = (dbg_result >> 16) &
					      XUANGE_DDR_PMU_DBG_CYCLE_CNT_MASK;
			dbg_sts->running = false;
			dbg_sts->snapshot_valid = true;
		}
		if (dbg_sts->snapshot_valid) {
			value = (GET_XUANGE_DDR_EVENTID(event) & 1) ?
				dbg_sts->shadow_max : dbg_sts->shadow_min;
			local64_set(&event->count, value);
		}
		raw_spin_unlock(&ddr_pmu->reg_lock);
	}
	event->hw.state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static void xuange_ddr_func_sel(struct perf_event *event)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	u32 val, func_id, idx, mask, shift;

	idx = event->hw.idx;
	func_id = GET_XUANGE_DDR_EVENTID(event) & 0x3;

	raw_spin_lock(&ddr_pmu->reg_lock);
	val = readl(ddr_pmu->cfg_base + XUANGE_DDR_PMU_EVT_SEL);
	shift = idx * 2;
	mask = 0x3U << shift;
	val = (val & ~mask) | (func_id << shift);
	writel(val, ddr_pmu->cfg_base + XUANGE_DDR_PMU_EVT_SEL);

	raw_spin_unlock(&ddr_pmu->reg_lock);
}

static int xuange_ddr_pmu_add(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	int idx = -1;

	idx = xuange_ddr_get_counter_idx(event);
	if (idx < 0)
		return idx;

	hwc->idx = idx;

	if (idx < XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS)
		xuange_ddr_func_sel(event);

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	if (flags & PERF_EF_START)
		xuange_ddr_pmu_start(event, PERF_EF_RELOAD);

	/* Propagate our changes to the userspace mapping. */
	perf_event_update_userpage(event);

	return 0;
}

static void xuange_ddr_pmu_del(struct perf_event *event, int flags)
{
	struct xuange_ddr_pmu *ddr_pmu = to_xuange_ddr_pmu(event->pmu);
	int idx = event->hw.idx;

	/* stop() takes reg_lock itself; do not hold it across this call. */
	xuange_ddr_pmu_stop(event, PERF_EF_UPDATE);

	if (idx >= 0 && idx < XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS) {
		clear_bit(idx, ddr_pmu->event_used);
	} else if (idx >= XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS &&
		   idx < XUANGE_DDR_PMU_COMMON_MAX_COUNTERS) {
		unsigned int dbg_idx = idx - XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS;
		struct xuange_ddr_debug_event_status *dbg_sts = &ddr_pmu->dbg_sts[dbg_idx];
		unsigned int member = GET_XUANGE_DDR_EVENTID(event) & 1;
		u32 dbg_shift = dbg_idx * 2;
		u32 dbg_slot = 1U << (dbg_shift + member);
		u32 paired_mask = 3U << dbg_shift;

		raw_spin_lock(&ddr_pmu->reg_lock);
		dbg_sts->events[member] = NULL;
		ddr_pmu->dbg_used &= ~dbg_slot;
		/* Only the last removal releases ownership and the cached pair. */
		if (!(ddr_pmu->dbg_used & paired_mask)) {
			dbg_sts->running = false;
			dbg_sts->snapshot_valid = false;
		}
		raw_spin_unlock(&ddr_pmu->reg_lock);
	}
	perf_event_update_userpage(event);
}

static void xuange_ddr_pmu_read(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;

	/*
	 * In software-stop mode debug min/max registers expose the previous
	 * measurement until STAT_EN is cleared. stop() publishes the new
	 * snapshot; read() must not publish stale hardware values. Interval
	 * reads (e.g. perf stat -I) cannot provide live debug measurements.
	 * The .snapshot attribute describes the value, not a refresh command.
	 */
	if (hwc->idx < XUANGE_DDR_PMU_PERF_EVENT_MAX_COUNTERS)
		xuange_ddr_pmu_event_update(event);
}

static int xuange_ddr_pmu_probe(struct platform_device *pdev)
{
	struct xuange_ddr_pmu *ddr_pmu;
	struct resource *res;
	char *name;
	int ret;

	ddr_pmu = devm_kzalloc(&pdev->dev, sizeof(*ddr_pmu), GFP_KERNEL);
	if (!ddr_pmu)
		return -ENOMEM;

	ddr_pmu->dev = &pdev->dev;
	platform_set_drvdata(pdev, ddr_pmu);

	ddr_pmu->cfg_base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(ddr_pmu->cfg_base))
		return PTR_ERR(ddr_pmu->cfg_base);

	name = devm_kasprintf(ddr_pmu->dev, GFP_KERNEL, "xuange_ddr_%llx",
			      (u64)(res->start >> 12));
	if (!name)
		return -ENOMEM;

	raw_spin_lock_init(&ddr_pmu->reg_lock);

	xuange_ddr_pmu_config_default(ddr_pmu);

	ddr_pmu->cpu = cpumask_local_spread(0, dev_to_node(&pdev->dev));

	ret = cpuhp_state_add_instance(xuange_ddr_cpuhp_state_num, &ddr_pmu->node);
	if (ret)
		return ret;

	ddr_pmu->pmu = (struct pmu) {
		.module		= THIS_MODULE,
		.parent		= &pdev->dev,
		.task_ctx_nr	= perf_invalid_context,
		.event_init	= xuange_ddr_pmu_event_init,
		.add		= xuange_ddr_pmu_add,
		.del		= xuange_ddr_pmu_del,
		.start		= xuange_ddr_pmu_start,
		.stop		= xuange_ddr_pmu_stop,
		.read		= xuange_ddr_pmu_read,
		.attr_groups	= xuange_ddr_pmu_attr_groups,
		.capabilities	= PERF_PMU_CAP_NO_EXCLUDE |
				  PERF_PMU_CAP_NO_INTERRUPT,
	};

	ret = perf_pmu_register(&ddr_pmu->pmu, name, -1);
	if (ret) {
		dev_err(ddr_pmu->dev, "Xuange DDR PMU register failed!\n");
		goto out_cpuhp;
	}

	return 0;
out_cpuhp:
	cpuhp_state_remove_instance_nocalls(xuange_ddr_cpuhp_state_num, &ddr_pmu->node);
	return ret;
}

static int xuange_ddr_pmu_remove(struct platform_device *pdev)
{
	struct xuange_ddr_pmu *ddr_pmu = platform_get_drvdata(pdev);

	cpuhp_state_remove_instance_nocalls(xuange_ddr_cpuhp_state_num, &ddr_pmu->node);
	perf_pmu_unregister(&ddr_pmu->pmu);

	return 0;
}

static int xuange_ddr_pmu_offline_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct xuange_ddr_pmu *ddr_pmu = hlist_entry_safe(node, struct xuange_ddr_pmu, node);
	unsigned int target;
	cpumask_t node_online_cpus;
	int nid;

	if (cpu != ddr_pmu->cpu)
		return 0;

	target = nr_cpu_ids;
	nid = dev_to_node(ddr_pmu->dev);

	if (nid != NUMA_NO_NODE) {
		cpumask_and(&node_online_cpus,
			    cpumask_of_node(nid), cpu_online_mask);
		target = cpumask_any_but(&node_online_cpus, cpu);
	}

	if (target >= nr_cpu_ids)
		target = cpumask_any_but(cpu_online_mask, cpu);

	if (target >= nr_cpu_ids)
		return -EBUSY;

	perf_pmu_migrate_context(&ddr_pmu->pmu, cpu, target);
	ddr_pmu->cpu = target;

	return 0;
}

static const struct of_device_id ddr_of_match[] = {
	{ .compatible = "xuange,ddr-pmu-v1" },
	{}
};
MODULE_DEVICE_TABLE(of, ddr_of_match);

/* Xuange DDR PMU ACPI HID, shared with platform firmware. */
static const struct acpi_device_id xuange_ddr_acpi_match[] = {
	{"BABA5003", 0},
	{}
};

MODULE_DEVICE_TABLE(acpi, xuange_ddr_acpi_match);

static struct platform_driver xuange_ddr_pmu_driver = {
	.driver = {
			.name = "xuange_ddr_pmu",
			.suppress_bind_attrs = true,
			.of_match_table = ddr_of_match,
			.acpi_match_table = xuange_ddr_acpi_match,
		   },
	.probe = xuange_ddr_pmu_probe,
	.remove = xuange_ddr_pmu_remove,
};

static int __init xuange_ddr_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "xuange_ddr_pmu:online",
				      NULL, xuange_ddr_pmu_offline_cpu);

	if (ret < 0) {
		pr_err("setup hotplug failed, ret = %d\n",
		       ret);
		return ret;
	}
	xuange_ddr_cpuhp_state_num = ret;

	ret = platform_driver_register(&xuange_ddr_pmu_driver);
	if (ret)
		cpuhp_remove_multi_state(xuange_ddr_cpuhp_state_num);

	return ret;
}

static void __exit xuange_ddr_pmu_exit(void)
{
	platform_driver_unregister(&xuange_ddr_pmu_driver);
	cpuhp_remove_multi_state(xuange_ddr_cpuhp_state_num);
}

module_init(xuange_ddr_pmu_init);
module_exit(xuange_ddr_pmu_exit);

MODULE_AUTHOR("Dong Yan <yandong@linux.alibaba.com>");
MODULE_AUTHOR("Neng Chen <nengchen@linux.alibaba.com>");
MODULE_AUTHOR("Shuai Xue <xueshuai@linux.alibaba.com>");
MODULE_DESCRIPTION("Xuange DDR Sub-System PMU driver");
MODULE_LICENSE("GPL");
