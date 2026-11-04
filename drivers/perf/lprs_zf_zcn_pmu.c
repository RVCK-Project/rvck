// SPDX-License-Identifier: GPL-2.0+
/*
 * LeapRise ZF(ZhuFeng) ZCN PMU driver.
 *
 * Copyright (C) 2026, LeapRise Technology Co., Ltd.
 */

#include <linux/acpi.h>
#include <linux/bitfield.h>
#include <linux/bitmap.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/property.h>
#include <linux/sysfs.h>
#include <linux/version.h>

#include "lprs_zf_zcn_pmu.h"

#define ZF_ZCN_DBG_AMU_FUNC_CTRL               0x610
#define ZF_ZCN_DBG_TAC_FUNC_CTRL               0x600

#define ZF_ZCN_DBG_AMU_FUNC_CTRL_EN            BIT(0)
#define ZF_ZCN_DBG_TAC_FUNC_CTRL_EN            BIT(11)

#define ZF_ZCN_AMU_EN                          BIT(0)

#define ZF_ZCN_AMU_OCCUP1_ID                   GENMASK(34, 32)
#define ZF_ZCN_VMN_OCCUP1_ID                   GENMASK(27, 24)

#define ZF_ZCN_SBOX_AMU_EVENT_EN               BIT(57)

#define ZF_ZCN_SBOX_EVT_SEL_MASK               GENMASK(8, 0)
#define ZF_ZCN_SBOX_SLOT_SHIFT(idx)            ((idx) * 32)
#define ZF_ZCN_SBOX_SRC_MASK                   GENMASK(15, 0)
#define ZF_ZCN_SBOX_SRC_SHIFT(idx)             (32 + (idx) * 16)
#define ZF_ZCN_SBOX_CFG_LOCAL_EN               GENMASK(2, 0)
#define ZF_ZCN_SBOX_TAM_CTRL_EN                BIT(0)
#define ZF_ZCN_SBOX_CNT_STRIDE                 0x10

#define ZF_ZCN_TAM_SRC_MASK                    GENMASK(2, 0)
#define ZF_ZCN_TAM_SRC_SHIFT(idx)              (1 + (idx) * 3)
#define ZF_ZCN_TAM_CNT_STRIDE                  8

#define ZF_ZCN_MAX_SBOX_LOCAL_COUNTERS         2
#define ZF_ZCN_MAX_DEVICE_LOCAL_COUNTERS       4
#define ZF_ZCN_MAX_CPG_PEERS                   2
#define ZF_ZCN_MAX_DIES                        ZF_ZCN_ENV_DIES_LIMIT

#define ZF_ZCN_IOD_ZRP_TAM_END                 (ZF_ZCN_FIXED_ZRP_TAM + \
					     ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL + 8)
#define ZF_ZCN_IOD_CPG_TAM_END                 (ZF_ZCN_FIXED_CPG_HA_TAM + \
					     ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL + 8)
#define ZF_ZCN_ZRI_WINDOW_END                  (ZF_ZCN_FIXED_ZRI_TAM + \
					     ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL + 8)

#define ZF_ZCN_CONFIG_UNIT                     GENMASK(15, 8)
#define ZF_ZCN_CONFIG_EVENT                    GENMASK(7, 0)
#define ZF_ZCN_CONFIG_OCCUP                    GENMASK(20, 18)
#define ZF_ZCN_CONFIG_NODE                     GENMASK(39, 24)
#define ZF_ZCN_CONFIG_NODE_SEL                 BIT(40)
#define ZF_ZCN_CONFIG_RESERVED                 GENMASK(23, 21)
#define ZF_ZCN_CONFIG_VALID                    (GENMASK_ULL(40, 0) & ~ZF_ZCN_CONFIG_RESERVED)

#define ZF_ZCN_EVENT_UNIT(event)               FIELD_GET(ZF_ZCN_CONFIG_UNIT, (event)->attr.config)
#define ZF_ZCN_EVENT_EVENT(event)              FIELD_GET(ZF_ZCN_CONFIG_EVENT, (event)->attr.config)
#define ZF_ZCN_EVENT_OCCUP(event)              FIELD_GET(ZF_ZCN_CONFIG_OCCUP, (event)->attr.config)
#define ZF_ZCN_EVENT_NODE(event)               FIELD_GET(ZF_ZCN_CONFIG_NODE, (event)->attr.config)
#define ZF_ZCN_EVENT_NODE_SEL(event) \
	FIELD_GET(ZF_ZCN_CONFIG_NODE_SEL, (event)->attr.config)

enum zf_zcn_node_type {
	ZF_ZCN_TYPE_VMN = 0x01,
	ZF_ZCN_TYPE_ZHI = 0x04,
	ZF_ZCN_TYPE_CHN = 0x05,
	ZF_ZCN_TYPE_SBOX = 0x06,
	ZF_ZCN_TYPE_ZRI = 0x0A,
	ZF_ZCN_TYPE_CRT = 0x71,
	ZF_ZCN_TYPE_CRR = 0x72,
	ZF_ZCN_TYPE_CLL = 0x73,
	ZF_ZCN_TYPE_ZHP = 0x76,
	ZF_ZCN_TYPE_ZRP = 0x77,
};

enum zf_zcn_event_source {
	ZF_ZCN_EVENT_SOURCE_DEVICE0 = 0,
	ZF_ZCN_EVENT_SOURCE_DEVICE1 = 1,
	ZF_ZCN_EVENT_SOURCE_DEVICE2 = 2,
	ZF_ZCN_EVENT_SOURCE_DEVICE3 = 3,
};

#define ZF_ZCN_TAM_EVENT_SOURCE_DEVICE_BASE    4

struct zf_zcn_node {
	void __iomem *base;
	void __iomem *tam_base;
	struct zf_zcn_node *counter_node;
	enum zf_zcn_node_type type;
	u16 node_id;

	struct perf_event *local_counters[ZF_ZCN_MAX_DEVICE_LOCAL_COUNTERS];
	struct zf_zcn_node *cpg_peers[ZF_ZCN_MAX_CPG_PEERS];
	u8 nr_cpg_peers;
};

#define ZF_ZCN_EVENT_F_OCCUP                    BIT(0)

struct zf_zcn_pmu_event_desc {
	const char *name;
	enum zf_zcn_node_type unit;
	u16 event;
	u8 occup;
	u8 flags;
};

struct zf_zcn_env_source {
	u32 *unit_ids;
	u16 *tac_id;
	u32 chn_per_die;
	u32 scn_per_die;
	u32 sbox_per_die;
	u32 zri_per_die;
	u32 cio_per_die;
	u32 cpg_per_die;
	u16 *chn;
	u16 *scn;
	u16 *sbox;
	u16 *zri;
	u16 *zrp;
	u16 *zhp;
	u16 *cpg;
	const char **zri_window;
	u64 *scn_base;
};
struct zf_zcn_die_regs {
	void __iomem *ccd_cfg;
	resource_size_t ccd_cfg_size;
	void __iomem **iod_cio;
	void __iomem **iod_cpg;
	void __iomem *iod_socbar;
	resource_size_t iod_socbar_size;
	resource_size_t iod_socbar_offset;
	void __iomem *ccd_socbar;
	resource_size_t ccd_socbar_size;
	resource_size_t ccd_socbar_mapped_offset;
	void __iomem *ccd_socbar_nw;
	resource_size_t ccd_socbar_nw_size;
	resource_size_t ccd_socbar_nw_mapped_offset;
};

struct zf_zcn_pmu {
	struct pmu pmu;
	struct device *dev;
	void __iomem *tac_base;
	bool tac_initialized;

	struct zf_zcn_die_regs regs;
	u8 die_id;
	resource_size_t *iod_cio_size;
	resource_size_t *iod_cpg_size;

	struct zf_zcn_node *nodes;
	u32 num_nodes;
	struct attribute_group event_attr_group;
	const struct attribute_group *attr_groups[4];

	int cpu;

	struct hlist_node cpuhp_node;
	struct zf_zcn_env_topo env;

};

struct zf_zcn_slot {
	struct zf_zcn_node *target;
	struct zf_zcn_node *capture;
	u32 offset;
	u8 idx;
};

struct zf_zcn_hw_event {
	unsigned long *targets;
	const struct zf_zcn_pmu_event_desc *desc;
	struct zf_zcn_slot *slots;
	u32 nr_slots;

	bool active;
	bool running;
};

static int zf_zcn_pmu_hp_state;

#define to_zcn_pmu(p)                   container_of(p, struct zf_zcn_pmu, pmu)
#define to_zcn_hw(event)                ((struct zf_zcn_hw_event *)(event)->pmu_private)

static const struct zf_zcn_pmu_event_desc *
zf_zcn_pmu_find_desc(enum zf_zcn_node_type unit, unsigned int event);
static ssize_t zf_zcn_pmu_event_show(struct device *dev,
				 struct device_attribute *attr, char *buf);

static const struct zf_zcn_pmu_event_desc zf_zcn_pmu_event_descs[];

static inline u64 zf_zcn_pmu_readq(struct zf_zcn_node *node, u32 offset)
{
	if (!node || !node->base) {
		WARN_ONCE(1, "ZCN PMU: Invalid register access\n");
		return 0;
	}
	return readq_relaxed((void __iomem *)((u8 __iomem *)node->base + offset));
}

static inline void zf_zcn_pmu_writeq(struct zf_zcn_node *node, u64 val, u32 offset)
{
	if (!node || !node->base) {
		WARN_ONCE(1, "ZCN PMU: Invalid register access\n");
		return;
	}
	writeq_relaxed(val, (void __iomem *)((u8 __iomem *)node->base + offset));
}

static inline u64 zf_zcn_pmu_read_tamq(struct zf_zcn_node *node, u32 offset)
{
	void __iomem *base = node ? (node->tam_base ? node->tam_base : node->base) : NULL;

	if (!base) {
		WARN_ONCE(1, "ZCN PMU: Invalid TAM register access\n");
		return 0;
	}
	return readq_relaxed((void __iomem *)((u8 __iomem *)base + offset));
}

static inline void zf_zcn_pmu_write_tamq(struct zf_zcn_node *node, u64 val, u32 offset)
{
	void __iomem *base = node ? (node->tam_base ? node->tam_base : node->base) : NULL;

	if (!base) {
		WARN_ONCE(1, "ZCN PMU: Invalid TAM register access\n");
		return;
	}
	writeq_relaxed(val, (void __iomem *)((u8 __iomem *)base + offset));
}

static inline u64 zf_zcn_field_mask(u32 width, u32 shift)
{
	return (((u64)1 << width) - 1) << shift;
}

static void zf_zcn_pmu_prepare_tac(struct zf_zcn_pmu *pmu)
{
	if (!pmu->tac_base || pmu->tac_initialized)
		return;

	writel_relaxed(ZF_ZCN_DBG_AMU_FUNC_CTRL_EN,
		       pmu->tac_base + ZF_ZCN_DBG_AMU_FUNC_CTRL);
	writel_relaxed(ZF_ZCN_DBG_TAC_FUNC_CTRL_EN,
		       pmu->tac_base + ZF_ZCN_DBG_TAC_FUNC_CTRL);
	pmu->tac_initialized = true;
}

static struct zf_zcn_node *zf_zcn_pmu_get_counter_node(struct zf_zcn_node *target_node)
{
	return target_node->counter_node ?
		target_node->counter_node : target_node;
}

static int zf_zcn_pmu_local_counter_count(const struct zf_zcn_node *node)
{
	return node->type == ZF_ZCN_TYPE_SBOX ? ZF_ZCN_MAX_SBOX_LOCAL_COUNTERS :
		ZF_ZCN_MAX_DEVICE_LOCAL_COUNTERS;
}

static bool zf_zcn_pmu_is_cpg(enum zf_zcn_node_type type)
{
	return type == ZF_ZCN_TYPE_CLL || type == ZF_ZCN_TYPE_CRT ||
	       type == ZF_ZCN_TYPE_CRR;
}

static bool zf_zcn_pmu_cpg_conflict(struct zf_zcn_pmu *pmu,
				    struct zf_zcn_node *capture,
				    struct zf_zcn_node *node)
{
	unsigned int i, j;

	for (i = 0; i < zf_zcn_pmu_local_counter_count(capture); i++) {
		struct perf_event *event = capture->local_counters[i];
		const struct zf_zcn_hw_event *hw;

		if (!event)
			continue;
		hw = to_zcn_hw(event);
		for (j = 0; j < node->nr_cpg_peers; j++)
			if (test_bit(node->cpg_peers[j] - pmu->nodes, hw->targets))
				return true;
	}

	return false;
}

static void zf_zcn_pmu_clear_cpg_peers(struct zf_zcn_node *node)
{
	unsigned int i;

	for (i = 0; i < node->nr_cpg_peers; i++)
		zf_zcn_pmu_writeq(node->cpg_peers[i], 0, ZF_ZCN_FIXED_EVT_SEL);
}

static u32 zf_zcn_pmu_local_counter_offset(const struct zf_zcn_node *node,
					  int local_idx)
{
	if (node->type == ZF_ZCN_TYPE_SBOX)
		return ZF_ZCN_FIXED_SBOX_CNT_BASE + local_idx * ZF_ZCN_SBOX_CNT_STRIDE;
	return ZF_ZCN_FIXED_TAM_CNT_BASE + local_idx * ZF_ZCN_TAM_CNT_STRIDE;
}

static int zf_zcn_pmu_alloc_local_counter(struct zf_zcn_node *capture_node,
				       struct perf_event *event)
{
	int max_counters = zf_zcn_pmu_local_counter_count(capture_node);
	int idx;

	for (idx = 0; idx < max_counters; idx++) {
		if (!capture_node->local_counters[idx]) {
			capture_node->local_counters[idx] = event;
			return idx;
		}
	}

	return -1;
}

static void zf_zcn_pmu_release_event_counters(struct perf_event *event)
{
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	u32 i;

	for (i = 0; i < hw->nr_slots; i++)
		hw->slots[i].capture->local_counters[hw->slots[i].idx] = NULL;
	hw->nr_slots = 0;
}

struct zf_zcn_event_sel {
	u32 reg;
	u32 width;
};

static struct zf_zcn_event_sel zf_zcn_pmu_event_sel(enum zf_zcn_node_type type)
{
	struct zf_zcn_event_sel sel = {
		.reg = ZF_ZCN_FIXED_EVT_SEL,
		.width = 8,
	};

	if (type == ZF_ZCN_TYPE_VMN)
		sel.width = 6;
	else if (type == ZF_ZCN_TYPE_ZRP)
		sel.reg = ZF_ZCN_FIXED_EVT_SEL_ZRP;

	return sel;
}

static void zf_zcn_pmu_configure_node_event(struct zf_zcn_node *target_node,
					 int slot,
					 u32 event_id,
					 u32 occup,
					 const struct zf_zcn_pmu_event_desc *desc)
{
	struct zf_zcn_event_sel sel = zf_zcn_pmu_event_sel(target_node->type);
	u32 shift = slot * sel.width;
	u64 reg_val;

	reg_val = zf_zcn_pmu_readq(target_node, sel.reg);

	reg_val &= ~zf_zcn_field_mask(sel.width, shift);

	reg_val |= ((u64)(event_id & ((1U << sel.width) - 1U)) << shift);

	if (target_node->type == ZF_ZCN_TYPE_VMN) {
		reg_val &= ~ZF_ZCN_VMN_OCCUP1_ID;
		reg_val |= FIELD_PREP(ZF_ZCN_VMN_OCCUP1_ID, occup);
	} else if ((desc->flags & ZF_ZCN_EVENT_F_OCCUP) || occup) {
		reg_val &= ~ZF_ZCN_AMU_OCCUP1_ID;
		reg_val |= FIELD_PREP(ZF_ZCN_AMU_OCCUP1_ID, occup);
	}
	zf_zcn_pmu_writeq(target_node, reg_val, sel.reg);
	pr_debug("Configure node_id=%u type=%d: EVENT_SEL(0x%x)=0x%llx (slot=%d, event_id=0x%x)\n",
			 target_node->node_id, target_node->type, sel.reg,
			 (unsigned long long)reg_val, slot, event_id);
}

static void zf_zcn_pmu_configure_sbox_event(struct zf_zcn_node *sbox_node,
					 int slot,
					 u32 event_config)
{
	u64 reg_val = zf_zcn_pmu_readq(sbox_node, ZF_ZCN_FIXED_EVT_SEL);
	u32 shift = ZF_ZCN_SBOX_SLOT_SHIFT(slot);

	reg_val &= ~((u64)ZF_ZCN_SBOX_EVT_SEL_MASK << shift);

	reg_val |= ((u64)(event_config & ZF_ZCN_SBOX_EVT_SEL_MASK) << shift);

	reg_val |= ZF_ZCN_SBOX_AMU_EVENT_EN;

	zf_zcn_pmu_writeq(sbox_node, reg_val, ZF_ZCN_FIXED_EVT_SEL);
}

static void zf_zcn_pmu_configure_local_counter(struct zf_zcn_node *capture_node,
					    int local_idx,
					    enum zf_zcn_event_source source);
static void zf_zcn_pmu_clear_local_counter(struct zf_zcn_node *capture_node,
					    u32 offset);

static void zf_zcn_pmu_program_event_on_node(struct perf_event *event,
					  const struct zf_zcn_slot *slot)
{
	enum zf_zcn_node_type unit = ZF_ZCN_EVENT_UNIT(event);
	enum zf_zcn_event_source source;
	u32 event_id = ZF_ZCN_EVENT_EVENT(event);

	zf_zcn_pmu_clear_local_counter(slot->capture, slot->offset);
	if (unit == ZF_ZCN_TYPE_SBOX) {
		zf_zcn_pmu_configure_sbox_event(slot->target, slot->idx, event_id);
		source = slot->idx ? ZF_ZCN_EVENT_SOURCE_DEVICE2 : ZF_ZCN_EVENT_SOURCE_DEVICE0;
	} else {
		zf_zcn_pmu_configure_node_event(slot->target, slot->idx, event_id,
			ZF_ZCN_EVENT_OCCUP(event), to_zcn_hw(event)->desc);
		source = ZF_ZCN_EVENT_SOURCE_DEVICE0 + slot->idx;
	}
	zf_zcn_pmu_configure_local_counter(slot->capture, slot->idx, source);
}

static bool zf_zcn_pmu_node_has_other_event(struct zf_zcn_node *node,
					 struct perf_event *event)
{
	int i;

	for (i = 0; i < zf_zcn_pmu_local_counter_count(node); i++)
		if (node->local_counters[i] && node->local_counters[i] != event)
			return true;
	return false;
}

static bool zf_zcn_pmu_node_has_other_running_occup_event(
					struct zf_zcn_pmu *pmu,
					struct zf_zcn_node *target_node,
					struct perf_event *event);

static void zf_zcn_pmu_disable_event_on_node(struct perf_event *event,
					  const struct zf_zcn_slot *slot)
{
	struct zf_zcn_node *target_node = slot->target;
	struct zf_zcn_node *capture_node = slot->capture;
	bool other_event;

	other_event = zf_zcn_pmu_node_has_other_event(capture_node, event);
	if (capture_node->type == ZF_ZCN_TYPE_SBOX) {
		u64 value = zf_zcn_pmu_readq(target_node, ZF_ZCN_FIXED_EVT_SEL);
		u32 shift = ZF_ZCN_SBOX_SLOT_SHIFT(slot->idx);

		value &= ~((u64)ZF_ZCN_SBOX_EVT_SEL_MASK << shift);
		if (!other_event)
			value &= ~ZF_ZCN_SBOX_AMU_EVENT_EN;
		zf_zcn_pmu_writeq(target_node, value, ZF_ZCN_FIXED_EVT_SEL);
		value = zf_zcn_pmu_readq(capture_node, ZF_ZCN_FIXED_SBOX_TAM_CFG);
		value &= ~((u64)ZF_ZCN_SBOX_SRC_MASK <<
			   ZF_ZCN_SBOX_SRC_SHIFT(slot->idx));
		if (!other_event)
			value &= ~ZF_ZCN_SBOX_CFG_LOCAL_EN;
		zf_zcn_pmu_writeq(capture_node, value, ZF_ZCN_FIXED_SBOX_TAM_CFG);
		if (!other_event)
			zf_zcn_pmu_writeq(capture_node, 0, ZF_ZCN_FIXED_SBOX_TAM_CTRL);
	} else {
		struct zf_zcn_event_sel sel = zf_zcn_pmu_event_sel(target_node->type);
		u64 value;
		u32 shift = slot->idx * sel.width;

		value = zf_zcn_pmu_readq(target_node, sel.reg);
		value &= ~zf_zcn_field_mask(sel.width, shift);
		if ((to_zcn_hw(event)->desc->flags & ZF_ZCN_EVENT_F_OCCUP) &&
		    !zf_zcn_pmu_node_has_other_running_occup_event(
				to_zcn_pmu(event->pmu), target_node, event))
			value &= ~ZF_ZCN_AMU_OCCUP1_ID;
		zf_zcn_pmu_writeq(target_node, value, sel.reg);

		value = zf_zcn_pmu_read_tamq(capture_node,
			ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL);
		value &= ~((u64)ZF_ZCN_TAM_SRC_MASK <<
			   ZF_ZCN_TAM_SRC_SHIFT(slot->idx));
		if (!other_event)
			value &= ~ZF_ZCN_AMU_EN;
		zf_zcn_pmu_write_tamq(capture_node, value,
			ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL);
	}
}

static void zf_zcn_pmu_program_event(struct perf_event *event)
{
	const struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	u32 i;

	for (i = 0; i < hw->nr_slots; i++)
		zf_zcn_pmu_program_event_on_node(event, &hw->slots[i]);
}

static void zf_zcn_pmu_disable_event(struct perf_event *event)
{
	const struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	u32 i;

	for (i = 0; i < hw->nr_slots; i++)
		zf_zcn_pmu_disable_event_on_node(event, &hw->slots[i]);
}

static void zf_zcn_pmu_configure_local_counter(struct zf_zcn_node *capture_node,
					    int local_idx,
					    enum zf_zcn_event_source source)
{
	u64 reg_val;
	u32 event_sel_shift;

	if (capture_node->type == ZF_ZCN_TYPE_SBOX) {
		reg_val = zf_zcn_pmu_readq(capture_node, ZF_ZCN_FIXED_SBOX_TAM_CFG);

		event_sel_shift = ZF_ZCN_SBOX_SRC_SHIFT(local_idx);

		reg_val &= ~((u64)ZF_ZCN_SBOX_SRC_MASK << event_sel_shift);

		reg_val |= ((u64)source << event_sel_shift);

		reg_val |= ZF_ZCN_SBOX_CFG_LOCAL_EN;

		zf_zcn_pmu_writeq(capture_node, reg_val, ZF_ZCN_FIXED_SBOX_TAM_CFG);

		zf_zcn_pmu_writeq(capture_node, ZF_ZCN_SBOX_TAM_CTRL_EN,
			       ZF_ZCN_FIXED_SBOX_TAM_CTRL);

	} else {
		reg_val = zf_zcn_pmu_read_tamq(capture_node, ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL);
		event_sel_shift = ZF_ZCN_TAM_SRC_SHIFT(local_idx);
		reg_val &= ~((u64)ZF_ZCN_TAM_SRC_MASK << event_sel_shift);

		reg_val |= ((u64)(ZF_ZCN_TAM_EVENT_SOURCE_DEVICE_BASE + source) << event_sel_shift);

		reg_val |= ZF_ZCN_AMU_EN;

		zf_zcn_pmu_write_tamq(capture_node, reg_val, ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL);
	}
}

static void zf_zcn_pmu_clear_local_counter(struct zf_zcn_node *capture_node,
					    u32 offset)
{
	if (capture_node->type == ZF_ZCN_TYPE_SBOX) {
		zf_zcn_pmu_writeq(capture_node, 0, offset);
		zf_zcn_pmu_writeq(capture_node, 0, offset + 8);
	} else {
		zf_zcn_pmu_write_tamq(capture_node, 0, offset);
	}
}

static u64 zf_zcn_pmu_read_local_counter(struct zf_zcn_node *capture_node, u32 offset)
{
	u64 count;

	if (capture_node->type == ZF_ZCN_TYPE_SBOX) {
		u32 high, high_after, low;

		do {
			high = zf_zcn_pmu_readq(capture_node, offset + 8) & U32_MAX;
			low = zf_zcn_pmu_readq(capture_node, offset) & U32_MAX;
			high_after = zf_zcn_pmu_readq(capture_node, offset + 8) & U32_MAX;
		} while (high != high_after);
		count = ((u64)high_after << 32) | low;
	} else {
		count = zf_zcn_pmu_read_tamq(capture_node, offset);
	}

	return count;
}


static u64 zf_zcn_pmu_read_total_count(struct perf_event *event)
{
	const struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	u64 total_count = 0;
	u32 i;

	for (i = 0; i < hw->nr_slots; i++)
		total_count += zf_zcn_pmu_read_local_counter(hw->slots[i].capture,
							      hw->slots[i].offset);

	return total_count;
}

static void zf_zcn_pmu_event_read(struct perf_event *event)
{
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	u64 new_count, prev_count;

	if (!hw->running)
		return;

	new_count = zf_zcn_pmu_read_total_count(event);
	prev_count = local64_xchg(&event->hw.prev_count, new_count);

	local64_add(new_count - prev_count, &event->count);
}

static void zf_zcn_pmu_event_start(struct perf_event *event, int flags)
{
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);

	if (!hw->active || hw->running)
		return;
	zf_zcn_pmu_prepare_tac(to_zcn_pmu(event->pmu));
	local64_set(&event->hw.prev_count, 0);
	zf_zcn_pmu_program_event(event);
	hw->running = true;
}

static void zf_zcn_pmu_event_stop(struct perf_event *event, int flags)
{
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);

	if (!hw->running)
		return;
	zf_zcn_pmu_disable_event(event);
	zf_zcn_pmu_event_read(event);
	hw->running = false;
}

static const struct zf_zcn_pmu_event_desc *
zf_zcn_pmu_find_desc(enum zf_zcn_node_type unit, unsigned int event)
{
	unsigned int i;

	for (i = 0; zf_zcn_pmu_event_descs[i].name; i++)
		if (zf_zcn_pmu_event_descs[i].unit == unit &&
		    zf_zcn_pmu_event_descs[i].event == event)
			return &zf_zcn_pmu_event_descs[i];
	return NULL;
}

static bool zf_zcn_pmu_occup_conflict(struct zf_zcn_node *capture,
				     unsigned int target_idx,
				     struct perf_event *event)
{
	const struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	unsigned int i;

	if (!(hw->desc->flags & ZF_ZCN_EVENT_F_OCCUP))
		return false;

	for (i = 0; i < zf_zcn_pmu_local_counter_count(capture); i++) {
		struct perf_event *peer = capture->local_counters[i];
		const struct zf_zcn_hw_event *peer_hw;

		if (!peer || peer == event ||
		    !test_bit(target_idx, to_zcn_hw(peer)->targets))
			continue;
		peer_hw = to_zcn_hw(peer);
		if ((peer_hw->desc->flags & ZF_ZCN_EVENT_F_OCCUP) &&
		    ZF_ZCN_EVENT_OCCUP(peer) != ZF_ZCN_EVENT_OCCUP(event))
			return true;
	}

	return false;
}

static bool zf_zcn_pmu_node_has_other_running_occup_event(
					struct zf_zcn_pmu *pmu,
					struct zf_zcn_node *target_node,
					struct perf_event *event)
{
	struct zf_zcn_node *capture = zf_zcn_pmu_get_counter_node(target_node);
	unsigned int target_idx = target_node - pmu->nodes;
	unsigned int i;

	for (i = 0; i < zf_zcn_pmu_local_counter_count(capture); i++) {
		struct perf_event *peer = capture->local_counters[i];
		struct zf_zcn_hw_event *peer_hw;

		if (!peer || peer == event)
			continue;
		peer_hw = to_zcn_hw(peer);
		if (!peer_hw->running || !test_bit(target_idx, peer_hw->targets))
			continue;
		if (peer_hw->desc->flags & ZF_ZCN_EVENT_F_OCCUP)
			return true;
	}

	return false;
}


static int zf_zcn_pmu_select_targets(struct zf_zcn_pmu *pmu, struct zf_zcn_hw_event *hw,
				  enum zf_zcn_node_type unit, bool node_sel, u16 node_id)
{
	u32 i;

	for (i = 0; i < pmu->num_nodes; i++) {
		if (pmu->nodes[i].type != unit)
			continue;
		if (node_sel && pmu->nodes[i].node_id != node_id)
			continue;
		__set_bit(i, hw->targets);
	}

	if (bitmap_empty(hw->targets, pmu->num_nodes))
		return -ENODEV;

	return 0;
}

static void zf_zcn_pmu_note_group_usage(struct zf_zcn_pmu *pmu,
					const struct zf_zcn_hw_event *hw,
					unsigned int *used)
{
	unsigned int i;

	for_each_set_bit(i, hw->targets, pmu->num_nodes)
		used[zf_zcn_pmu_get_counter_node(&pmu->nodes[i]) - pmu->nodes]++;
}

static int zf_zcn_pmu_check_group_slots(struct zf_zcn_pmu *pmu,
				     struct perf_event *event,
				     const struct zf_zcn_hw_event *hw)
{
	struct perf_event *leader = event->group_leader;
	struct perf_event *sibling;
	unsigned int *used;
	unsigned int i;
	int ret = 0;

	used = kcalloc(pmu->num_nodes, sizeof(*used), GFP_KERNEL);
	if (!used)
		return -ENOMEM;

	if (leader != event && leader->pmu == event->pmu)
		zf_zcn_pmu_note_group_usage(pmu, to_zcn_hw(leader), used);

	list_for_each_entry(sibling, &leader->sibling_list, sibling_list)
		if (sibling != event && sibling->pmu == event->pmu)
			zf_zcn_pmu_note_group_usage(pmu, to_zcn_hw(sibling), used);

	for_each_set_bit(i, hw->targets, pmu->num_nodes) {
		struct zf_zcn_node *capture = zf_zcn_pmu_get_counter_node(&pmu->nodes[i]);
		unsigned int capture_idx = capture - pmu->nodes;

		if (used[capture_idx] >=
		    (unsigned int)zf_zcn_pmu_local_counter_count(capture)) {
			ret = -EINVAL;
			break;
		}
	}

	kfree(used);
	return ret;
}
static void zf_zcn_pmu_free_hw_event(struct zf_zcn_hw_event *hw)
{
	kfree(hw->slots);
	bitmap_free(hw->targets);
	kfree(hw);
}

static void zf_zcn_pmu_event_destroy(struct perf_event *event)
{
	if (event->pmu_private)
		zf_zcn_pmu_free_hw_event(event->pmu_private);
}

static int zf_zcn_pmu_event_init(struct perf_event *event)
{
	struct zf_zcn_pmu *pmu = to_zcn_pmu(event->pmu);
	const struct zf_zcn_pmu_event_desc *desc;
	struct zf_zcn_hw_event *hw;
	enum zf_zcn_node_type unit;
	u16 node_id;
	int ret;

	dev_dbg(pmu->dev, "event_init config=0x%llx, type=%u\n",
		 event->attr.config, event->attr.type);

	if (event->attr.type != event->pmu->type)
		return -ENOENT;
	if (event->attr.config & ~ZF_ZCN_CONFIG_VALID ||
		event->attr.config1 || event->attr.config2)
		return -EINVAL;

	if (is_sampling_event(event) || event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	event->cpu = pmu->cpu;

	unit = ZF_ZCN_EVENT_UNIT(event);
	node_id = ZF_ZCN_EVENT_NODE(event);

	desc = zf_zcn_pmu_find_desc(unit, ZF_ZCN_EVENT_EVENT(event));
	if (!desc)
		return -EINVAL;

	hw = kzalloc(sizeof(*hw), GFP_KERNEL);
	if (!hw)
		return -ENOMEM;
	hw->targets = bitmap_zalloc(pmu->num_nodes, GFP_KERNEL);
	if (!hw->targets) {
		kfree(hw);
		return -ENOMEM;
	}
	hw->desc = desc;

	ret = zf_zcn_pmu_select_targets(pmu, hw, unit, ZF_ZCN_EVENT_NODE_SEL(event), node_id);
	if (ret) {
		dev_dbg(pmu->dev, "No ZCN node matches type=%u node=%u\n",
			unit, node_id);
		zf_zcn_pmu_free_hw_event(hw);
		return ret;
	}

	hw->slots = kcalloc(bitmap_weight(hw->targets, pmu->num_nodes),
			    sizeof(*hw->slots), GFP_KERNEL);
	if (!hw->slots) {
		zf_zcn_pmu_free_hw_event(hw);
		return -ENOMEM;
	}

	ret = zf_zcn_pmu_check_group_slots(pmu, event, hw);
	if (ret) {
		zf_zcn_pmu_free_hw_event(hw);
		return ret;
	}

	event->pmu_private = hw;
	event->destroy = zf_zcn_pmu_event_destroy;
	return 0;
}



static int zf_zcn_pmu_event_add(struct perf_event *event, int flags)
{
	struct zf_zcn_pmu *pmu = to_zcn_pmu(event->pmu);
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);
	unsigned int i;

	dev_dbg(pmu->dev, "event_add config=0x%llx, flags=0x%x\n",
		 event->attr.config, flags);

	for_each_set_bit(i, hw->targets, pmu->num_nodes) {
		struct zf_zcn_node *node = &pmu->nodes[i];
		struct zf_zcn_node *capture_node;

		capture_node = zf_zcn_pmu_get_counter_node(node);
		if (zf_zcn_pmu_occup_conflict(capture_node, i, event)) {
			dev_warn(pmu->dev,
				 "Occupancy selector on node %u is already in use\n",
				 node->node_id);
			return -EBUSY;
		}
		if (!zf_zcn_pmu_is_cpg(node->type))
			continue;
		if (zf_zcn_pmu_cpg_conflict(pmu, capture_node, node)) {
			dev_warn(pmu->dev,
				 "CPG TAM cannot mix CLL/CRT/CRR events (config=0x%llx)\n",
				 event->attr.config);
			return -EBUSY;
		}
	}

	for_each_set_bit(i, hw->targets, pmu->num_nodes) {
		struct zf_zcn_node *node = &pmu->nodes[i];
		struct zf_zcn_node *capture_node;
		struct zf_zcn_slot *slot;
		int local_idx;

		if (zf_zcn_pmu_is_cpg(node->type))
			zf_zcn_pmu_clear_cpg_peers(node);

		capture_node = zf_zcn_pmu_get_counter_node(node);
		local_idx = zf_zcn_pmu_alloc_local_counter(capture_node, event);
		if (local_idx < 0)
			goto err_release;

		slot = &hw->slots[hw->nr_slots++];
		slot->target = node;
		slot->capture = capture_node;
		slot->idx = local_idx;
		slot->offset = zf_zcn_pmu_local_counter_offset(capture_node, local_idx);
	}

	hw->active = true;

	if (flags & PERF_EF_START)
		zf_zcn_pmu_event_start(event, 0);

	return 0;

err_release:
	zf_zcn_pmu_release_event_counters(event);
	hw->active = false;
	dev_dbg(pmu->dev,
		"No complete counter allocation for config=0x%llx; retrying multiplex\n",
		event->attr.config);
	return -EAGAIN;
}

static void zf_zcn_pmu_event_del(struct perf_event *event, int flags)
{
	struct zf_zcn_hw_event *hw = to_zcn_hw(event);

	zf_zcn_pmu_event_stop(event, PERF_EF_UPDATE);

	if (hw->active) {
		zf_zcn_pmu_release_event_counters(event);
		hw->active = false;
	}
}

static int zf_zcn_pmu_cpu_offline(unsigned int cpu, struct hlist_node *node)
{
	struct zf_zcn_pmu *pmu = hlist_entry_safe(node, struct zf_zcn_pmu, cpuhp_node);
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

#define ZF_ZCN_PMU_EVENT(_name, _unit, _event)				\
	{ .name = __stringify(_name), .unit = _unit, .event = _event }

#define ZF_ZCN_PMU_EVENT_OCC(_name, _unit, _event, _occup)			\
	{ .name = __stringify(_name), .unit = _unit, .event = _event,	\
	  .occup = _occup, .flags = ZF_ZCN_EVENT_F_OCCUP }

static const struct zf_zcn_pmu_event_desc zf_zcn_pmu_event_descs[] = {
	ZF_ZCN_PMU_EVENT(chn_llc_sf_cache_access, ZF_ZCN_TYPE_CHN, 0x01),
	ZF_ZCN_PMU_EVENT(chn_llc_cache_miss, ZF_ZCN_TYPE_CHN, 0x02),
	ZF_ZCN_PMU_EVENT(chn_sf_hit, ZF_ZCN_TYPE_CHN, 0x03),
	ZF_ZCN_PMU_EVENT(chn_llc_sf_remote_access, ZF_ZCN_TYPE_CHN, 0x04),
	ZF_ZCN_PMU_EVENT(chn_remote_llc_hit, ZF_ZCN_TYPE_CHN, 0x05),
	ZF_ZCN_PMU_EVENT(chn_remote_sf_hit, ZF_ZCN_TYPE_CHN, 0x06),
	ZF_ZCN_PMU_EVENT(chn_local_llc_hit, ZF_ZCN_TYPE_CHN, 0x07),
	ZF_ZCN_PMU_EVENT(chn_local_sf_hit, ZF_ZCN_TYPE_CHN, 0x08),
	ZF_ZCN_PMU_EVENT(chn_llc_sf_cross_group_access, ZF_ZCN_TYPE_CHN, 0x09),
	ZF_ZCN_PMU_EVENT(chn_cross_group_llc_hit, ZF_ZCN_TYPE_CHN, 0x0A),
	ZF_ZCN_PMU_EVENT(chn_cross_group_sf_hit, ZF_ZCN_TYPE_CHN, 0x0B),
	ZF_ZCN_PMU_EVENT(chn_llc_sf_cross_die_access, ZF_ZCN_TYPE_CHN, 0x0C),
	ZF_ZCN_PMU_EVENT(chn_cross_die_llc_hit, ZF_ZCN_TYPE_CHN, 0x0D),
	ZF_ZCN_PMU_EVENT(chn_cross_die_sf_hit, ZF_ZCN_TYPE_CHN, 0x0E),
	ZF_ZCN_PMU_EVENT(chn_llc_sf_cross_chip_access, ZF_ZCN_TYPE_CHN, 0x0F),
	ZF_ZCN_PMU_EVENT(chn_cross_chip_llc_hit, ZF_ZCN_TYPE_CHN, 0x10),
	ZF_ZCN_PMU_EVENT(chn_cross_chip_sf_hit, ZF_ZCN_TYPE_CHN, 0x11),
	ZF_ZCN_PMU_EVENT(chn_cache_fill, ZF_ZCN_TYPE_CHN, 0x12),
	ZF_ZCN_PMU_EVENT(chn_llc_fill_invalid_way, ZF_ZCN_TYPE_CHN, 0x13),
	ZF_ZCN_PMU_EVENT(chn_llc_evict, ZF_ZCN_TYPE_CHN, 0x14),
	ZF_ZCN_PMU_EVENT(chn_sf_evict0, ZF_ZCN_TYPE_CHN, 0x15),
	ZF_ZCN_PMU_EVENT(chn_sf_evict1, ZF_ZCN_TYPE_CHN, 0x16),
	ZF_ZCN_PMU_EVENT(chn_seq_full, ZF_ZCN_TYPE_CHN, 0x1C),
	ZF_ZCN_PMU_EVENT(chn_seq_hit, ZF_ZCN_TYPE_CHN, 0x1D),
	ZF_ZCN_PMU_EVENT(chn_sf_imprecise_evict, ZF_ZCN_TYPE_CHN, 0x1F),
	ZF_ZCN_PMU_EVENT(chn_cbqri_req_over_hardlim, ZF_ZCN_TYPE_CHN, 0x23),
	ZF_ZCN_PMU_EVENT(chn_cbqri_req_over_softlim, ZF_ZCN_TYPE_CHN, 0x24),
	ZF_ZCN_PMU_EVENT(chn_snp_sent, ZF_ZCN_TYPE_CHN, 0x25),
	ZF_ZCN_PMU_EVENT(chn_snp_fwded, ZF_ZCN_TYPE_CHN, 0x2A),
	ZF_ZCN_PMU_EVENT(chn_snp_retry, ZF_ZCN_TYPE_CHN, 0x2C),
	ZF_ZCN_PMU_EVENT(chn_snp_recvd, ZF_ZCN_TYPE_CHN, 0x2D),
	ZF_ZCN_PMU_EVENT(chn_req_recvd, ZF_ZCN_TYPE_CHN, 0x2E),
	ZF_ZCN_PMU_EVENT(chn_req_retry, ZF_ZCN_TYPE_CHN, 0x2F),
	ZF_ZCN_PMU_EVENT(chn_mc_retry, ZF_ZCN_TYPE_CHN, 0x32),
	ZF_ZCN_PMU_EVENT(chn_mc_req, ZF_ZCN_TYPE_CHN, 0x33),
	ZF_ZCN_PMU_EVENT(chn_nc_excl, ZF_ZCN_TYPE_CHN, 0x40),
	ZF_ZCN_PMU_EVENT(chn_excl_mon_ovfl, ZF_ZCN_TYPE_CHN, 0x41),
	ZF_ZCN_PMU_EVENT(chn_excl_access, ZF_ZCN_TYPE_CHN, 0x42),
	ZF_ZCN_PMU_EVENT(chn_excl_access_fail, ZF_ZCN_TYPE_CHN, 0x43),

	ZF_ZCN_PMU_EVENT_OCC(chn_occupevent_cntr_ovflw_all, ZF_ZCN_TYPE_CHN, 0x45, 0),
	ZF_ZCN_PMU_EVENT_OCC(chn_occupevent_cntr_ovflw_read, ZF_ZCN_TYPE_CHN, 0x45, 1),
	ZF_ZCN_PMU_EVENT_OCC(chn_occupevent_cntr_ovflw_write, ZF_ZCN_TYPE_CHN, 0x45, 2),
	ZF_ZCN_PMU_EVENT_OCC(chn_occupevent_cntr_ovflw_atomic, ZF_ZCN_TYPE_CHN, 0x45, 3),
	ZF_ZCN_PMU_EVENT_OCC(chn_occupevent_cntr_ovflw_stash, ZF_ZCN_TYPE_CHN, 0x45, 4),

	ZF_ZCN_PMU_EVENT(zrp_rxdat_flits_received, ZF_ZCN_TYPE_ZRP, 0x0B),
	ZF_ZCN_PMU_EVENT(zrp_txdat_flits_sent, ZF_ZCN_TYPE_ZRP, 0x0C),
	ZF_ZCN_PMU_EVENT(zrp_rxsnp_data_flits_sent, ZF_ZCN_TYPE_ZRP, 0x0D),
	ZF_ZCN_PMU_EVENT(zrp_total_txreq_flits_sent, ZF_ZCN_TYPE_ZRP, 0x0E),
	ZF_ZCN_PMU_EVENT(zrp_retried_txreq_flits_sent, ZF_ZCN_TYPE_ZRP, 0x0F),
	ZF_ZCN_PMU_EVENT(zrp_wrt_ovfl, ZF_ZCN_TYPE_ZRP, 0x10),
	ZF_ZCN_PMU_EVENT(zrp_write_cancel_sent, ZF_ZCN_TYPE_ZRP, 0x11),
	ZF_ZCN_PMU_EVENT(zrp_rrt_allocation, ZF_ZCN_TYPE_ZRP, 0x1C),
	ZF_ZCN_PMU_EVENT(zrp_wrt_allocation, ZF_ZCN_TYPE_ZRP, 0x1D),
	ZF_ZCN_PMU_EVENT(zrp_wrt_req_throttled, ZF_ZCN_TYPE_ZRP, 0x22),

	ZF_ZCN_PMU_EVENT(zhp_rrt_rd_ovfl, ZF_ZCN_TYPE_ZHP, 0x01),
	ZF_ZCN_PMU_EVENT(zhp_rrt_wr_ovfl, ZF_ZCN_TYPE_ZHP, 0x02),
	ZF_ZCN_PMU_EVENT(zhp_rrt_np_ovfl, ZF_ZCN_TYPE_ZHP, 0x03),
	ZF_ZCN_PMU_EVENT(zhp_rrt_p_ovfl, ZF_ZCN_TYPE_ZHP, 0x04),
	ZF_ZCN_PMU_EVENT(zhp_rdt_rd_ovfl, ZF_ZCN_TYPE_ZHP, 0x05),
	ZF_ZCN_PMU_EVENT(zhp_rdt_wr_ovfl, ZF_ZCN_TYPE_ZHP, 0x06),
	ZF_ZCN_PMU_EVENT(zhp_rdt_np_ovfl, ZF_ZCN_TYPE_ZHP, 0x07),
	ZF_ZCN_PMU_EVENT(zhp_rdt_p_ovfl, ZF_ZCN_TYPE_ZHP, 0x08),
	ZF_ZCN_PMU_EVENT(zhp_wdb_ovfl, ZF_ZCN_TYPE_ZHP, 0x09),
	ZF_ZCN_PMU_EVENT(zhp_rrt_rd_alloc, ZF_ZCN_TYPE_ZHP, 0x0A),
	ZF_ZCN_PMU_EVENT(zhp_rrt_wr_alloc, ZF_ZCN_TYPE_ZHP, 0x0B),
	ZF_ZCN_PMU_EVENT(zhp_rrt_np_alloc, ZF_ZCN_TYPE_ZHP, 0x0C),
	ZF_ZCN_PMU_EVENT(zhp_rrt_p_alloc, ZF_ZCN_TYPE_ZHP, 0x0D),
	ZF_ZCN_PMU_EVENT(zhp_rdt_rd_alloc, ZF_ZCN_TYPE_ZHP, 0x0E),
	ZF_ZCN_PMU_EVENT(zhp_rdt_wr_alloc, ZF_ZCN_TYPE_ZHP, 0x0F),
	ZF_ZCN_PMU_EVENT(zhp_rdt_np_alloc, ZF_ZCN_TYPE_ZHP, 0x10),
	ZF_ZCN_PMU_EVENT(zhp_rdt_p_alloc, ZF_ZCN_TYPE_ZHP, 0x11),
	ZF_ZCN_PMU_EVENT(zhp_p2p_wdb_alloc, ZF_ZCN_TYPE_ZHP, 0x12),
	ZF_ZCN_PMU_EVENT(zhp_np2p_wdb_alloc, ZF_ZCN_TYPE_ZHP, 0x13),
	ZF_ZCN_PMU_EVENT(zhp_retryack_txrsp_sent, ZF_ZCN_TYPE_ZHP, 0x14),

	ZF_ZCN_PMU_EVENT(vmn_write_local_entry, ZF_ZCN_TYPE_VMN, 0x01),
	ZF_ZCN_PMU_EVENT(vmn_write_remote_entry, ZF_ZCN_TYPE_VMN, 0x02),
	ZF_ZCN_PMU_EVENT(vmn_write_peer_entry, ZF_ZCN_TYPE_VMN, 0x03),
	ZF_ZCN_PMU_EVENT(vmn_write_main_entry, ZF_ZCN_TYPE_VMN, 0x04),
	ZF_ZCN_PMU_EVENT(vmn_rcb_entry_full, ZF_ZCN_TYPE_VMN, 0x05),
	ZF_ZCN_PMU_EVENT(vmn_recv_tlbi_req, ZF_ZCN_TYPE_VMN, 0x06),
	ZF_ZCN_PMU_EVENT(vmn_recv_sync_req, ZF_ZCN_TYPE_VMN, 0x0A),
	ZF_ZCN_PMU_EVENT(vmn_txsnp_flit_seen, ZF_ZCN_TYPE_VMN, 0x0B),
	ZF_ZCN_PMU_EVENT(vmn_txrsp_flit_seen, ZF_ZCN_TYPE_VMN, 0x0D),
	ZF_ZCN_PMU_EVENT(vmn_txfst_flit_seen, ZF_ZCN_TYPE_VMN, 0x0F),
	ZF_ZCN_PMU_EVENT(vmn_bcast_filtered_gscid, ZF_ZCN_TYPE_VMN, 0x11),
	ZF_ZCN_PMU_EVENT(vmn_req_retried, ZF_ZCN_TYPE_VMN, 0x13),
	ZF_ZCN_PMU_EVENT(vmn_local_tracker_ovfl, ZF_ZCN_TYPE_VMN, 0x14),
	ZF_ZCN_PMU_EVENT(vmn_remote_tracker_ovfl, ZF_ZCN_TYPE_VMN, 0x15),
	ZF_ZCN_PMU_EVENT(vmn_peer_tracker_ovfl, ZF_ZCN_TYPE_VMN, 0x16),
	ZF_ZCN_PMU_EVENT(vmn_main_tracker_ovfl, ZF_ZCN_TYPE_VMN, 0x17),
	ZF_ZCN_PMU_EVENT(vmn_snp_hold_os, ZF_ZCN_TYPE_VMN, 0x18),
	ZF_ZCN_PMU_EVENT(vmn_starvation_trigger, ZF_ZCN_TYPE_VMN, 0x19),
	ZF_ZCN_PMU_EVENT(vmn_sync_hzd, ZF_ZCN_TYPE_VMN, 0x1A),

	ZF_ZCN_PMU_EVENT(crt_snp_cnt, ZF_ZCN_TYPE_CRT, 0x41),
	ZF_ZCN_PMU_EVENT(crt_req_recorder_used_acc, ZF_ZCN_TYPE_CRT, 0x4B),
	ZF_ZCN_PMU_EVENT(crt_req_recorder_used, ZF_ZCN_TYPE_CRT, 0x4C),
	ZF_ZCN_PMU_EVENT(crt_snp_recorder_used_acc, ZF_ZCN_TYPE_CRT, 0x4D),
	ZF_ZCN_PMU_EVENT(crt_snp_recorder_used, ZF_ZCN_TYPE_CRT, 0x4E),
	ZF_ZCN_PMU_EVENT(crt_rd_dat_buf_used_acc, ZF_ZCN_TYPE_CRT, 0x4F),
	ZF_ZCN_PMU_EVENT(crt_rd_dat_buf_used, ZF_ZCN_TYPE_CRT, 0x50),
	ZF_ZCN_PMU_EVENT(crt_wr_dat_buf_used_acc, ZF_ZCN_TYPE_CRT, 0x51),
	ZF_ZCN_PMU_EVENT(crt_wr_dat_buf_used, ZF_ZCN_TYPE_CRT, 0x52),

	ZF_ZCN_PMU_EVENT(crr_req_recorder_used_acc, ZF_ZCN_TYPE_CRR, 0x64),
	ZF_ZCN_PMU_EVENT(crr_req_recorder_used, ZF_ZCN_TYPE_CRR, 0x65),
	ZF_ZCN_PMU_EVENT(crr_rd_dat_buf_used_acc, ZF_ZCN_TYPE_CRR, 0x66),
	ZF_ZCN_PMU_EVENT(crr_rd_dat_buf_used, ZF_ZCN_TYPE_CRR, 0x67),
	ZF_ZCN_PMU_EVENT(crr_rd_dat_buf_byp_used_acc, ZF_ZCN_TYPE_CRR, 0x68),
	ZF_ZCN_PMU_EVENT(crr_rd_dat_buf_byp_used, ZF_ZCN_TYPE_CRR, 0x69),
	ZF_ZCN_PMU_EVENT(crr_wr_dat_buf_used_acc, ZF_ZCN_TYPE_CRR, 0x6A),
	ZF_ZCN_PMU_EVENT(crr_wr_dat_buf_used, ZF_ZCN_TYPE_CRR, 0x6B),
	ZF_ZCN_PMU_EVENT(crr_snp_recorder_used_acc, ZF_ZCN_TYPE_CRR, 0x6C),
	ZF_ZCN_PMU_EVENT(crr_snp_recorder_used, ZF_ZCN_TYPE_CRR, 0x6D),
	ZF_ZCN_PMU_EVENT(crr_snp_dat_buf_used_acc, ZF_ZCN_TYPE_CRR, 0x6E),
	ZF_ZCN_PMU_EVENT(crr_snp_dat_buf_used, ZF_ZCN_TYPE_CRR, 0x6F),
	ZF_ZCN_PMU_EVENT(crr_snp_haz_used_acc, ZF_ZCN_TYPE_CRR, 0x70),
	ZF_ZCN_PMU_EVENT(crr_snp_haz_used, ZF_ZCN_TYPE_CRR, 0x71),
	{ /* End marker. */ },
};

static ssize_t zf_zcn_pmu_event_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *ext =
		container_of(attr, struct dev_ext_attribute, attr);
	const struct zf_zcn_pmu_event_desc *desc = ext->var;

	if (desc->flags & ZF_ZCN_EVENT_F_OCCUP)
		return sysfs_emit(buf, "unit=0x%x,event=0x%x,occup=0x%x\n",
				  desc->unit, desc->event, desc->occup);
	return sysfs_emit(buf, "unit=0x%x,event=0x%x\n", desc->unit, desc->event);
}

PMU_FORMAT_ATTR(unit, "config:8-15");
PMU_FORMAT_ATTR(event, "config:0-7");
PMU_FORMAT_ATTR(occup, "config:18-20");
PMU_FORMAT_ATTR(node, "config:24-39");
PMU_FORMAT_ATTR(node_sel, "config:40");

static struct attribute *zf_zcn_pmu_format_attrs[] = {
	&format_attr_unit.attr,
	&format_attr_event.attr,
	&format_attr_occup.attr,
	&format_attr_node.attr,
	&format_attr_node_sel.attr,
	NULL,
};

static struct attribute_group zf_zcn_pmu_format_attrs_group = {
	.name = "format",
	.attrs = zf_zcn_pmu_format_attrs,
};

static ssize_t cpumask_show(struct device *dev,
			   struct device_attribute *attr,
			   char *buf)
{
	struct zf_zcn_pmu *pmu = to_zcn_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(pmu->cpu));
}

static DEVICE_ATTR_RO(cpumask);

static ssize_t nodes_read(struct file *file, struct kobject *kobj,
			  struct bin_attribute *attr, char *buf,
			  loff_t off, size_t count)
{
	struct device *dev = kobj_to_dev(kobj);
	struct zf_zcn_pmu *pmu = to_zcn_pmu(dev_get_drvdata(dev));
	size_t position = 0, copied = 0;
	u32 i;

	for (i = 0; i < pmu->num_nodes && copied < count; i++) {
		char line[48];
		size_t len, start, chunk;

		len = scnprintf(line, sizeof(line),
				"unit=0x%02x,node=%u,physical=%u\n",
				pmu->nodes[i].type, pmu->nodes[i].node_id,
				pmu->nodes[i].node_id);
		if (off < position + len) {
			start = off > position ? off - position : 0;
			chunk = min_t(size_t, count - copied, len - start);
			memcpy(buf + copied, line + start, chunk);
			copied += chunk;
		}
		position += len;
	}
	return copied;
}

static struct bin_attribute zf_zcn_pmu_nodes_attr = {
	.attr = {
		.name = "nodes",
		.mode = 0444,
	},
	.read = nodes_read,
};

static struct bin_attribute *zf_zcn_pmu_device_bin_attrs[] = {
	&zf_zcn_pmu_nodes_attr,
	NULL,
};

static struct attribute *zf_zcn_pmu_device_attrs[] = {
	&dev_attr_cpumask.attr,
	NULL,
};

static struct attribute_group zf_zcn_pmu_device_attrs_group = {
	.attrs = zf_zcn_pmu_device_attrs,
	.bin_attrs = zf_zcn_pmu_device_bin_attrs,
};

static int zf_zcn_pmu_setup_event_attrs(struct zf_zcn_pmu *pmu)
{
	struct dev_ext_attribute *event_attrs;
	struct attribute **attrs;
	unsigned int i;
	unsigned int count = 0;

	while (count < ARRAY_SIZE(zf_zcn_pmu_event_descs) &&
	       zf_zcn_pmu_event_descs[count].name)
		count++;

	event_attrs = devm_kcalloc(pmu->dev, count, sizeof(*event_attrs), GFP_KERNEL);
	attrs = devm_kcalloc(pmu->dev, count + 1, sizeof(*attrs), GFP_KERNEL);
	if (!event_attrs || !attrs)
		return -ENOMEM;

	for (i = 0; i < count; i++) {
		struct device_attribute *attr = &event_attrs[i].attr;

		sysfs_attr_init(&attr->attr);
		attr->attr.name = zf_zcn_pmu_event_descs[i].name;
		attr->attr.mode = 0444;
		attr->show = zf_zcn_pmu_event_show;
		event_attrs[i].var = (void *)&zf_zcn_pmu_event_descs[i];
		attrs[i] = &attr->attr;
	}

	pmu->event_attr_group.name = "events";
	pmu->event_attr_group.attrs = attrs;
	pmu->attr_groups[0] = &pmu->event_attr_group;
	pmu->attr_groups[1] = &zf_zcn_pmu_format_attrs_group;
	pmu->attr_groups[2] = &zf_zcn_pmu_device_attrs_group;
	pmu->attr_groups[3] = NULL;
	return 0;
}

static bool zf_zcn_iod_window_fits(resource_size_t size, resource_size_t offset,
				resource_size_t last_byte)
{
	return offset < size && last_byte < size - offset;
}

static int zf_zcn_pmu_map_iod_slots(struct platform_device *pdev, u8 die_id,
				 const char *region_name,
				 unsigned int slot_count, resource_size_t stride,
				 resource_size_t required_end,
				 void __iomem * const *enabled_slots,
				 unsigned int enabled_count, void __iomem **slots);

static void zf_zcn_pmu_init_node(struct zf_zcn_node *node, void __iomem *base,
			      void __iomem *tam_base,
			      enum zf_zcn_node_type type, u16 node_id)
{
	node->base = base;
	node->tam_base = tam_base;
	node->type = type;
	node->node_id = node_id;
}


enum zf_zcn_env_kind { ZF_ZCN_ENV_U16, ZF_ZCN_ENV_U32, ZF_ZCN_ENV_U64, ZF_ZCN_ENV_STR };

struct zf_zcn_env_array {
	const char *key;
	void *slot;
	enum zf_zcn_env_kind kind;
	const u32 *per_unit;
	bool optional;
};

#define ZF_ZCN_ENV_READ_NUM(_type, _count, _read) do {                         \
		_type *v = devm_kcalloc(dev, items, sizeof(*v), GFP_KERNEL);  \
		if (!v)                                                       \
			return -ENOMEM;                                       \
		n = _count(dev, a->key);                                      \
		if (!(a->optional && n < 0)) {                                \
			if (n != (int)items || _read(dev, a->key, v, items))   \
				goto bad;                                     \
		}                                                             \
		buf = v;                                                      \
	} while (0)

static int zf_zcn_env_read_array(struct device *dev, const struct zf_zcn_env_array *a,
			      unsigned int items)
{
	void *buf = NULL;
	int n;

	if (!items)
		return 0;

	switch (a->kind) {
	case ZF_ZCN_ENV_U32:
		ZF_ZCN_ENV_READ_NUM(u32, device_property_count_u32,
			       device_property_read_u32_array);
		break;
	case ZF_ZCN_ENV_U16:
		ZF_ZCN_ENV_READ_NUM(u16, device_property_count_u16,
			       device_property_read_u16_array);
		break;
	case ZF_ZCN_ENV_U64:
		ZF_ZCN_ENV_READ_NUM(u64, device_property_count_u64,
			       device_property_read_u64_array);
		break;
	case ZF_ZCN_ENV_STR:
		buf = devm_kcalloc(dev, items, sizeof(char *), GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		n = device_property_read_string_array(dev, a->key, buf, items);
		if (!(a->optional && n < 0) && n != (int)items)
			goto bad;
		break;
	}

	*(void **)a->slot = buf;
	return 0;

bad:
	dev_err(dev, "Environment property %s: expected %u item(s), read %d\n",
		a->key, items, n);
	return -EINVAL;
}
static int zf_zcn_env_read_counts(struct device *dev, struct zf_zcn_env_source *src)
{
	static const char *const keys[6] = {
		"zcn,chn-per-unit", "zcn,scn-per-unit", "zcn,zri-per-unit",
		"zcn,cio-per-unit", "zcn,cpg-per-unit", "zcn,sbox-per-unit",
	};
	u32 *values[6] = {
		&src->chn_per_die, &src->scn_per_die, &src->zri_per_die,
		&src->cio_per_die, &src->cpg_per_die, &src->sbox_per_die,
	};
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(keys); i++) {
		if (device_property_read_u32(dev, keys[i], values[i])) {
			dev_err(dev, "Environment property %s is absent\n", keys[i]);
			return -EINVAL;
		}
		if (*values[i] > ZF_ZCN_ENV_UNITS_LIMIT) {
			dev_err(dev, "Environment property %s exceeds the allocation guard\n",
				keys[i]);
			return -E2BIG;
		}
	}
	return 0;
}

static int zf_zcn_env_read_source(struct device *dev, u32 num_dies,
			      struct zf_zcn_env_source *src)
{
	const struct zf_zcn_env_array arrays[] = {
		{ "zcn,anchor-node-ids", &src->tac_id, ZF_ZCN_ENV_U16, NULL, false },
		{ "zcn,chn-node-ids", &src->chn, ZF_ZCN_ENV_U16, &src->chn_per_die,
		  false },
		{ "zcn,scn-node-ids", &src->scn, ZF_ZCN_ENV_U16, &src->scn_per_die,
		  false },
		{ "zcn,sbox-node-ids", &src->sbox, ZF_ZCN_ENV_U16, &src->sbox_per_die,
		  false },
		/* Optional for older _DSD tables. */
		{ "zcn,scn-base-addrs", &src->scn_base, ZF_ZCN_ENV_U64,
		  &src->scn_per_die, true },
		{ "zcn,zri-node-ids", &src->zri, ZF_ZCN_ENV_U16, &src->zri_per_die,
		  false },
		{ "zcn,zri-window-names", &src->zri_window, ZF_ZCN_ENV_STR,
		  &src->zri_per_die, false },
		{ "zcn,zrp-node-ids", &src->zrp, ZF_ZCN_ENV_U16, &src->cio_per_die,
		  false },
		{ "zcn,zhp-node-ids", &src->zhp, ZF_ZCN_ENV_U16, &src->cio_per_die,
		  false },
		{ "zcn,cpg-node-ids", &src->cpg, ZF_ZCN_ENV_U16, &src->cpg_per_die,
		  false },
	};
	unsigned int i;
	int ret;

	if (!device_property_present(dev, "zcn,chn-per-unit"))
		return -ENOENT;

	ret = zf_zcn_env_read_counts(dev, src);
	if (ret)
		return ret;

	if (device_property_present(dev, "zcn,unit-ids")) {
		int count = device_property_count_u32(dev, "zcn,unit-ids");

		if (count != num_dies) {
			dev_err(dev, "Environment property zcn,unit-ids: expected %u item(s), read %d\n",
				num_dies, count);
			return -EINVAL;
		}
		src->unit_ids = devm_kcalloc(dev, num_dies,
					     sizeof(*src->unit_ids), GFP_KERNEL);
		if (!src->unit_ids)
			return -ENOMEM;
		ret = device_property_read_u32_array(dev, "zcn,unit-ids",
					     src->unit_ids, num_dies);
		if (ret)
			return ret;
	}

	for (i = 0; i < ARRAY_SIZE(arrays); i++) {
		const struct zf_zcn_env_array *a = &arrays[i];
		unsigned int items = (a->per_unit ? *a->per_unit : 1) * num_dies;

		ret = zf_zcn_env_read_array(dev, a, items);
		if (ret)
			return ret;
	}
	return 0;
}
static int zf_zcn_env_pick_die(struct zf_zcn_pmu *pmu, struct zf_zcn_env_source *src,
			    u32 die_local)
{
	struct zf_zcn_env_topo *env = &pmu->env;
	unsigned int i;

	env->tac_id = src->tac_id[die_local];
	env->chn_count = src->chn_per_die;
	env->scn_count = src->scn_per_die;
	env->sbox_count = src->sbox_per_die;
	env->zri_count = src->zri_per_die;
	env->cio_count = src->cio_per_die;
	env->cpg_count = src->cpg_per_die;

	env->chn_ids = src->chn + die_local * env->chn_count;
	env->scn_ids = src->scn + die_local * env->scn_count;
	env->sbox_ids = src->sbox + die_local * env->sbox_count;
	env->scn_base = src->scn_base + die_local * env->scn_count;
	env->zri_ids = src->zri + die_local * env->zri_count;
	env->zri_window = src->zri_window + die_local * env->zri_count;
	env->zrp_ids = src->zrp + die_local * env->cio_count;
	env->zhp_ids = src->zhp + die_local * env->cio_count;
	env->cpg_ids = src->cpg + die_local * env->cpg_count;

	env->node_count = zf_zcn_env_node_count(env);

	for (i = 0; i < env->zri_count; i++)
		if (zf_zcn_fixed_region_by_name(env->zri_window[i]) < 0) {
			dev_err(pmu->dev, "Die %u: unknown ZRI window in the environment data\n",
				pmu->die_id);
			return -EINVAL;
		}
	dev_dbg(pmu->dev,
		"Die %u: env chn %u scn %u sbox %u zri %u cio %u cpg %u, %u nodes\n",
		pmu->die_id, env->chn_count, env->scn_count, env->sbox_count,
		env->zri_count, env->cio_count, env->cpg_count, env->node_count);
	return 0;
}
static int zf_zcn_fixed_node_type(enum zf_zcn_fixed_type fixed_type,
			       enum zf_zcn_node_type *node_type)
{
	switch (fixed_type) {
	case ZF_ZCN_FIXED_T_CHN:
		*node_type = ZF_ZCN_TYPE_CHN;
		break;
	case ZF_ZCN_FIXED_T_SBOX:
		*node_type = ZF_ZCN_TYPE_SBOX;
		break;
	case ZF_ZCN_FIXED_T_ZHI:
		*node_type = ZF_ZCN_TYPE_ZHI;
		break;
	case ZF_ZCN_FIXED_T_VMN:
		*node_type = ZF_ZCN_TYPE_VMN;
		break;
	case ZF_ZCN_FIXED_T_ZRI:
		*node_type = ZF_ZCN_TYPE_ZRI;
		break;
	case ZF_ZCN_FIXED_T_ZRP:
		*node_type = ZF_ZCN_TYPE_ZRP;
		break;
	case ZF_ZCN_FIXED_T_ZHP:
		*node_type = ZF_ZCN_TYPE_ZHP;
		break;
	case ZF_ZCN_FIXED_T_CLL:
		*node_type = ZF_ZCN_TYPE_CLL;
		break;
	case ZF_ZCN_FIXED_T_CRT:
		*node_type = ZF_ZCN_TYPE_CRT;
		break;
	case ZF_ZCN_FIXED_T_CRR:
		*node_type = ZF_ZCN_TYPE_CRR;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int zf_zcn_fixed_region_window(struct zf_zcn_pmu *pmu,
				   enum zf_zcn_fixed_region region,
				   u64 row_offset, void __iomem **base,
				   resource_size_t *region_offset,
				   resource_size_t *region_size)
{
	unsigned int slot;

	*base = NULL;
	*region_offset = 0;
	*region_size = 0;
	switch (region) {
	case ZF_ZCN_FIXED_CCD_CFG:
		*base = pmu->regs.ccd_cfg;
		*region_size = pmu->regs.ccd_cfg_size;
		break;
	case ZF_ZCN_FIXED_IOD_CIO:
		slot = row_offset / ZF_ZCN_FIXED_CIO_STRIDE;
		if (slot >= pmu->env.cio_count)
			return -ERANGE;
		*base = pmu->regs.iod_cio[slot];
		*region_offset = (resource_size_t)slot * ZF_ZCN_FIXED_CIO_STRIDE;
		*region_size = pmu->iod_cio_size[slot];
		break;
	case ZF_ZCN_FIXED_IOD_CPG:
		slot = row_offset / ZF_ZCN_FIXED_CPG_STRIDE;
		if (slot >= pmu->env.cpg_count)
			return -ERANGE;
		*base = pmu->regs.iod_cpg[slot];
		*region_offset = (resource_size_t)slot * ZF_ZCN_FIXED_CPG_STRIDE;
		*region_size = pmu->iod_cpg_size[slot];
		break;
	case ZF_ZCN_FIXED_IOD_SOCBAR:
		*base = pmu->regs.iod_socbar;
		*region_offset = pmu->regs.iod_socbar_offset;
		*region_size = pmu->regs.iod_socbar_size;
		break;
	case ZF_ZCN_FIXED_CCD_SOCBAR_NE:
		*base = pmu->regs.ccd_socbar;
		*region_offset = pmu->regs.ccd_socbar_mapped_offset;
		*region_size = pmu->regs.ccd_socbar_size;
		break;
	case ZF_ZCN_FIXED_CCD_SOCBAR_NW:
		*base = pmu->regs.ccd_socbar_nw;
		*region_offset = pmu->regs.ccd_socbar_nw_mapped_offset;
		*region_size = pmu->regs.ccd_socbar_nw_size;
		break;
	default:
		return -EINVAL;
	}

	return *base ? 0 : -ENODEV;
}

static int zf_zcn_pmu_add_fixed_node(struct zf_zcn_pmu *pmu,
				   const struct zf_zcn_fixed_row *row,
				   struct zf_zcn_node *node)
{
	enum zf_zcn_node_type type;
	void __iomem *region_base;
	void __iomem *cfg_base;
	void __iomem *tam_base;
	resource_size_t region_offset, region_size;
	resource_size_t cfg_offset, tam_offset;
	u64 cfg_last, tam_last;
	int ret;

	ret = zf_zcn_fixed_node_type(row->type, &type);
	if (ret)
		return ret;
	ret = zf_zcn_fixed_region_window(pmu, row->region, row->cfg,
				      &region_base, &region_offset, &region_size);
	if (ret)
		return ret;
	if (row->cfg < region_offset || row->tam < region_offset)
		return -ERANGE;

	cfg_offset = row->cfg - region_offset;
	tam_offset = row->tam - region_offset;
	cfg_last = cfg_offset + zf_zcn_fixed_evt_sel_offset(row->type) +
		sizeof(u64) - 1;
	tam_last = max_t(u64, zf_zcn_fixed_control_offset(row->type),
			 zf_zcn_fixed_counter_offset(row->type,
						   zf_zcn_fixed_num_slots(row->type) - 1)) +
		sizeof(u64) - 1;
	if (cfg_last >= region_size || tam_offset + tam_last >= region_size)
		return -ERANGE;

	cfg_base = (u8 __iomem *)region_base + cfg_offset;
	tam_base = (u8 __iomem *)region_base + tam_offset;
	zf_zcn_pmu_init_node(node, cfg_base, tam_base, type, row->node);
	return 0;
}

static int zf_zcn_pmu_init_single_die(struct platform_device *pdev,
				   struct zf_zcn_pmu *pmu, u8 die_id)
{
	struct zf_zcn_fixed_row row;
	u32 node_idx = 0;
	unsigned int i;
	int ret;

	pmu->regs.iod_cio = devm_kcalloc(pmu->dev, pmu->env.cio_count,
					 sizeof(*pmu->regs.iod_cio), GFP_KERNEL);
	pmu->regs.iod_cpg = devm_kcalloc(pmu->dev, pmu->env.cpg_count,
					 sizeof(*pmu->regs.iod_cpg), GFP_KERNEL);
	pmu->iod_cio_size = devm_kcalloc(pmu->dev, pmu->env.cio_count,
					 sizeof(*pmu->iod_cio_size), GFP_KERNEL);
	pmu->iod_cpg_size = devm_kcalloc(pmu->dev, pmu->env.cpg_count,
					 sizeof(*pmu->iod_cpg_size), GFP_KERNEL);
	if (!pmu->regs.iod_cio || !pmu->regs.iod_cpg ||
	    !pmu->iod_cio_size || !pmu->iod_cpg_size)
		return -ENOMEM;

	ret = zf_zcn_pmu_map_iod_slots(pdev, die_id, "iod_cio",
				    pmu->env.cio_count, ZF_ZCN_FIXED_CIO_STRIDE,
				    ZF_ZCN_IOD_ZRP_TAM_END, NULL, 0,
				    pmu->regs.iod_cio);
	if (ret)
		return ret;
	ret = zf_zcn_pmu_map_iod_slots(pdev, die_id, "iod_cpg",
				    pmu->env.cpg_count, ZF_ZCN_FIXED_CPG_STRIDE,
				    ZF_ZCN_IOD_CPG_TAM_END,
				    pmu->regs.iod_cio, pmu->env.cio_count,
				    pmu->regs.iod_cpg);
	if (ret)
		return ret;

	for (i = 0; i < pmu->env.cio_count; i++)
		if (pmu->regs.iod_cio[i])
			pmu->iod_cio_size[i] = ZF_ZCN_IOD_ZRP_TAM_END + 1;
	for (i = 0; i < pmu->env.cpg_count; i++)
		if (pmu->regs.iod_cpg[i])
			pmu->iod_cpg_size[i] = ZF_ZCN_IOD_CPG_TAM_END + 1;

	pmu->nodes = devm_kcalloc(pmu->dev, pmu->env.node_count,
				  sizeof(*pmu->nodes), GFP_KERNEL);
	if (!pmu->nodes)
		return -ENOMEM;

	for (i = 0; i < pmu->env.node_count; i++) {
		ret = zf_zcn_env_row_at(&pmu->env, i, &row);
		if (ret) {
			dev_err(pmu->dev, "Die %u: cannot build fixed row %u: %d\n",
				die_id, i, ret);
			return ret;
		}
		ret = zf_zcn_pmu_add_fixed_node(pmu, &row, &pmu->nodes[node_idx]);
		if (ret == -ENODEV) {
			dev_dbg(pmu->dev, "Die %u: skip %s node %u without resource\n",
				die_id, zf_zcn_fixed_type_name(row.type), row.node);
			continue;
		}
		if (ret) {
			dev_err(pmu->dev,
				"Die %u: invalid %s node %u mapping (cfg=0x%llx tam=0x%llx): %d\n",
				die_id, zf_zcn_fixed_type_name(row.type), row.node,
				(unsigned long long)row.cfg,
				(unsigned long long)row.tam, ret);
			return ret;
		}
		node_idx++;

		if (row.type == ZF_ZCN_FIXED_T_ZHI &&
		    row.node == pmu->env.tac_id)
			pmu->tac_base = (u8 __iomem *)pmu->regs.ccd_cfg + row.tam;
	}

	if (!node_idx)
		return -ENODEV;
	pmu->num_nodes = node_idx;
	dev_dbg(pmu->dev,
		 "Die %u: fixed table registered %u nodes\n",
		die_id, pmu->num_nodes);

	for (i = 0; i < pmu->num_nodes; i++) {
		struct zf_zcn_node *node = &pmu->nodes[i];
		unsigned int j;

		node->counter_node = node;
		for (j = 0; j < i; j++) {
			struct zf_zcn_node *candidate = &pmu->nodes[j];

			if (candidate->type == node->type &&
			    candidate->node_id == node->node_id) {
				dev_err(pmu->dev,
					"Environment data repeats one node entry\n");
				return -EEXIST;
			}
			if (node->type == ZF_ZCN_TYPE_SBOX ||
			    candidate->type == ZF_ZCN_TYPE_SBOX ||
			    candidate->tam_base != node->tam_base ||
			    node->counter_node != node)
				continue;
			node->counter_node = candidate->counter_node;
		}
	}

	for (i = 0; i < pmu->num_nodes; i++) {
		struct zf_zcn_node *node = &pmu->nodes[i];
		unsigned int j;

		if (!zf_zcn_pmu_is_cpg(node->type))
			continue;
		for (j = 0; j < pmu->num_nodes; j++) {
			struct zf_zcn_node *peer = &pmu->nodes[j];

			if (peer == node || peer->type == node->type ||
			    !zf_zcn_pmu_is_cpg(peer->type) ||
			    peer->tam_base != node->tam_base)
				continue;
			if (node->nr_cpg_peers < ZF_ZCN_MAX_CPG_PEERS)
				node->cpg_peers[node->nr_cpg_peers++] = peer;
		}
	}

	return 0;
}

static const struct pmu zf_zcn_pmu_pmu = {
	.task_ctx_nr    = perf_invalid_context,
	.module         = THIS_MODULE,
	.event_init     = zf_zcn_pmu_event_init,
	.add            = zf_zcn_pmu_event_add,
	.del            = zf_zcn_pmu_event_del,
	.start          = zf_zcn_pmu_event_start,
	.stop           = zf_zcn_pmu_event_stop,
	.read           = zf_zcn_pmu_event_read,
	.capabilities   = PERF_PMU_CAP_NO_EXCLUDE | PERF_PMU_CAP_NO_INTERRUPT,
};

static int zf_zcn_pmu_register_die_pmu(struct zf_zcn_pmu *pmu)
{
	char *name;
	int ret;

	pmu->cpu = raw_smp_processor_id();

	pmu->pmu = zf_zcn_pmu_pmu;

	ret = zf_zcn_pmu_setup_event_attrs(pmu);
	if (ret)
		return ret;
	pmu->pmu.attr_groups = pmu->attr_groups;

	name = devm_kasprintf(pmu->dev, GFP_KERNEL, "zcn_pmu_%u",
						 pmu->die_id);
	if (!name)
		return -ENOMEM;

	ret = cpuhp_state_add_instance(zf_zcn_pmu_hp_state, &pmu->cpuhp_node);
	if (ret)
		return ret;

	ret = perf_pmu_register(&pmu->pmu, name, -1);
	if (ret)
		goto err_cpuhp;

	dev_info(pmu->dev, "ZCN PMU '%s' registered for die %d on CPU %d\n",
			 name, pmu->die_id, pmu->cpu);

	return 0;

err_cpuhp:
	cpuhp_state_remove_instance(zf_zcn_pmu_hp_state, &pmu->cpuhp_node);
	return ret;
}

static void zf_zcn_pmu_unregister_dies(struct zf_zcn_pmu **pmus, int count)
{
	while (--count >= 0)
		if (pmus[count]) {
			perf_pmu_unregister(&pmus[count]->pmu);
			cpuhp_state_remove_instance(zf_zcn_pmu_hp_state,
						    &pmus[count]->cpuhp_node);
		}
}

static struct resource *zf_zcn_pmu_get_mem_resource(struct platform_device *pdev,
						  const char *name)
{
	struct resource *res;
	int index;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, name);
	if (res)
		return res;

	index = device_property_match_string(&pdev->dev,
					     "zcn,resource-names", name);
	if (index < 0)
		return NULL;

	res = platform_get_resource(pdev, IORESOURCE_MEM, index);
	if (!res)
		dev_err(&pdev->dev,
			"_DSD maps resource %s to missing MEM resource %d\n",
			name, index);
	return res;
}

static int zf_zcn_pmu_map_zri_window(struct platform_device *pdev,
				 struct resource *res, const char *name,
				 void __iomem **base,
				 resource_size_t *mapped_size,
				 resource_size_t *mapped_offset)
{
	resource_size_t map_offset = ZF_ZCN_FIXED_ZRI_CFG;
	resource_size_t map_end = ZF_ZCN_ZRI_WINDOW_END;
	struct resource map_res = *res;

	if (resource_size(res) < map_end) {
		dev_err(&pdev->dev,
			"Resource %s too small: size=0x%llx need=0x%llx\n",
			name, (unsigned long long)resource_size(res),
			(unsigned long long)map_end);
		return -ERANGE;
	}

	map_res.start += map_offset;
	map_res.end = map_res.start + map_end - map_offset - 1;
	*base = devm_ioremap_resource(&pdev->dev, &map_res);
	if (IS_ERR(*base)) {
		dev_err(&pdev->dev,
			"Failed to map %s ZRI window [0x%llx-0x%llx]: %pe\n",
			name, (unsigned long long)map_res.start,
			(unsigned long long)map_res.end, *base);
		return PTR_ERR(*base);
	}
	*mapped_size = map_end - map_offset;
	*mapped_offset = ZF_ZCN_FIXED_ZRI_CFG;

	dev_dbg(&pdev->dev, "Mapped %s ZRI window: 0x%llx-0x%llx -> %p\n",
		 name, (unsigned long long)map_res.start,
		 (unsigned long long)map_res.end, *base);
	return 0;
}

static int zf_zcn_pmu_map_die_window(struct platform_device *pdev, u8 die_id,
				  const char *suffix,
				  void __iomem **base, resource_size_t *size,
				  resource_size_t *mapped_offset)
{
	char res_name[32];
	struct resource *res;

	snprintf(res_name, sizeof(res_name), "die%u_%s", die_id, suffix);
	res = zf_zcn_pmu_get_mem_resource(pdev, res_name);
	if (!res)
		return -ENOENT;

	return zf_zcn_pmu_map_zri_window(pdev, res, res_name,
				      base, size, mapped_offset);
}

static void __iomem *zf_zcn_pmu_map_span(struct platform_device *pdev,
				     const struct resource *res,
				     resource_size_t offset,
				     resource_size_t span)
{
	struct resource sub = *res;

	sub.start = res->start + offset;
	sub.end = sub.start + span - 1;
	return devm_ioremap_resource(&pdev->dev, &sub);
}

static inline bool zf_zcn_slot_enabled(void __iomem *const *enabled,
				    unsigned int count, unsigned int i)
{
	return !enabled || (i < count && enabled[i]);
}

static int zf_zcn_pmu_map_iod_slot(struct platform_device *pdev,
				struct resource *res, resource_size_t offset,
				resource_size_t required_end,
				const char *region_name, unsigned int slot,
				void __iomem **slots)
{
	void __iomem *mapped;

	if (!zf_zcn_iod_window_fits(resource_size(res), offset, required_end)) {
		dev_err(&pdev->dev,
			"Resource %s slot %u too small: size=0x%llx offset=0x%llx need=0x%llx\n",
			region_name, slot, (unsigned long long)resource_size(res),
			(unsigned long long)offset,
			(unsigned long long)(required_end + 1));
		return -ERANGE;
	}

	mapped = zf_zcn_pmu_map_span(pdev, res, offset, required_end + 1);
	if (IS_ERR(mapped)) {
		dev_warn(&pdev->dev,
			 "Skipping unavailable IOD %s slot %u: %pe\n",
			 region_name, slot, mapped);
		return PTR_ERR(mapped);
	}

	slots[slot] = mapped;
	dev_dbg(&pdev->dev, "Mapped IOD %s slot %u -> %p\n",
		region_name, slot, mapped);
	return 0;
}

static int zf_zcn_pmu_map_iod_slots(struct platform_device *pdev, u8 die_id,
				 const char *region_name,
				 unsigned int slot_count, resource_size_t stride,
				 resource_size_t required_end,
				 void __iomem * const *enabled_slots,
				 unsigned int enabled_count, void __iomem **slots)
{
	char res_name[32];
	char slot_name[40];
	struct resource *res;
	bool short_base = false;
	unsigned int i;

	for (i = 0; i < slot_count; i++)
		slots[i] = NULL;

	snprintf(res_name, sizeof(res_name), "die%u_%s", die_id, region_name);
	res = zf_zcn_pmu_get_mem_resource(pdev, res_name);

	if (res) {
		for (i = 0; i < slot_count; i++) {
			if (!zf_zcn_slot_enabled(enabled_slots, enabled_count, i))
				continue;

			if (zf_zcn_pmu_map_iod_slot(pdev, res,
					     (resource_size_t)i * stride,
					     required_end, region_name, i, slots) == -ERANGE)
				short_base = true;
		}
	}

	for (i = 0; i < slot_count; i++) {
		if (slots[i] || !zf_zcn_slot_enabled(enabled_slots, enabled_count, i))
			continue;
		snprintf(slot_name, sizeof(slot_name), "%s_slot%u", res_name, i);
		res = zf_zcn_pmu_get_mem_resource(pdev, slot_name);
		if (!res) {
			dev_dbg(&pdev->dev,
				 "IOD %s slot %u resource not present\n",
				 region_name, i);
			continue;
		}
		if (!zf_zcn_iod_window_fits(resource_size(res), 0, required_end)) {
			dev_err(&pdev->dev,
				 "Resource %s_slot%u too small: size=0x%llx need=0x%llx\n",
				 region_name, i,
				 (unsigned long long)resource_size(res),
				 (unsigned long long)(required_end + 1));
			return -ERANGE;
		}

		zf_zcn_pmu_map_iod_slot(pdev, res, 0, required_end, region_name, i,
				     slots);
	}

	if (short_base) {
		for (i = 0; i < slot_count; i++) {
			if (zf_zcn_slot_enabled(enabled_slots, enabled_count, i) &&
			    !slots[i])
				return -ERANGE;
		}
	}

	return 0;
}

static bool zf_zcn_env_uses_window(const struct zf_zcn_env_topo *env,
				enum zf_zcn_fixed_region region)
{
	unsigned int i;

	for (i = 0; i < env->zri_count; i++)
		if (zf_zcn_fixed_region_by_name(env->zri_window[i]) == (int)region)
			return true;
	return false;
}

static int zf_zcn_pmu_map_die_resources(struct platform_device *pdev,
				     struct zf_zcn_pmu *pmu,
				     u8 die_id)
{
	struct zf_zcn_die_regs *regs = &pmu->regs;
	char res_name[32];
	struct resource *res;
	int ret;

	snprintf(res_name, sizeof(res_name), "die%u_ccd_cfg", die_id);
	res = zf_zcn_pmu_get_mem_resource(pdev, res_name);
	if (!res) {
		dev_err(&pdev->dev, "Failed to find resource %s\n", res_name);
		return -EINVAL;
	}
	regs->ccd_cfg = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(regs->ccd_cfg)) {
		dev_err(&pdev->dev, "Failed to map resource %s\n", res_name);
		return PTR_ERR(regs->ccd_cfg);
	}
	regs->ccd_cfg_size = resource_size(res);
	dev_dbg(&pdev->dev, "Mapped %s: 0x%llx -> %p\n",
		 res_name, (unsigned long long)res->start, regs->ccd_cfg);

	if (zf_zcn_env_uses_window(&pmu->env, ZF_ZCN_FIXED_IOD_SOCBAR)) {
		ret = zf_zcn_pmu_map_die_window(pdev, die_id, "iod_socbar",
					     &regs->iod_socbar,
					     &regs->iod_socbar_size,
					     &regs->iod_socbar_offset);
		if (ret == -ENOENT)
			dev_info(&pdev->dev,
				 "Resource die%u_iod_socbar not found, skipping\n",
				 die_id);
		else if (ret)
			return ret;
	}

	ret = zf_zcn_pmu_map_die_window(pdev, die_id, "ccd_socbar_ne",
				     &regs->ccd_socbar,
				     &regs->ccd_socbar_size,
				     &regs->ccd_socbar_mapped_offset);
	if (ret == -ENOENT)
		dev_info(&pdev->dev,
			 "Resource die%u_ccd_socbar_ne not found, skipping\n",
			 die_id);
	else if (ret)
		return ret;

	ret = zf_zcn_pmu_map_die_window(pdev, die_id, "ccd_socbar_nw",
				     &regs->ccd_socbar_nw,
				     &regs->ccd_socbar_nw_size,
				     &regs->ccd_socbar_nw_mapped_offset);
	if (ret == -ENOENT)
		dev_info(&pdev->dev,
			 "Resource die%u_ccd_socbar_nw not found, skipping\n",
			 die_id);
	else if (ret)
		return ret;

	return 0;
}

static int zf_zcn_pmu_setup_die(struct platform_device *pdev,
			     struct zf_zcn_env_source *env_src, u32 die_local,
			     u8 die_id, struct zf_zcn_pmu **out)
{
	struct zf_zcn_pmu *pmu;
	int ret;

	pmu = devm_kzalloc(&pdev->dev, sizeof(*pmu), GFP_KERNEL);
	if (!pmu)
		return -ENOMEM;

	pmu->dev = &pdev->dev;
	pmu->die_id = die_id;

	ret = zf_zcn_env_pick_die(pmu, env_src, die_local);
	if (ret) {
		dev_err(&pdev->dev,
			"Failed to resolve environment for die %u: %d\n",
			die_id, ret);
		return ret;
	}

	ret = zf_zcn_pmu_map_die_resources(pdev, pmu, die_id);
	if (ret) {
		dev_err(&pdev->dev, "Failed to map resources for die %u\n",
			die_id);
		return ret;
	}

	ret = zf_zcn_pmu_init_single_die(pdev, pmu, die_id);
	if (ret) {
		dev_err(&pdev->dev, "Failed to initialize die %u\n", die_id);
		return ret;
	}

	*out = pmu;
	return 0;
}

static int zf_zcn_pmu_probe(struct platform_device *pdev)
{
	struct zf_zcn_pmu **pmus;
	struct zf_zcn_env_source *env_src;
	struct device *dev = &pdev->dev;
	int i, ret;
	u32 num_dies = 1;
	u32 first_die_id = 0;

	device_property_read_u32(dev, "zcn,unit-count", &num_dies);
	device_property_read_u32(dev, "zcn,first-unit-id", &first_die_id);

	if (!num_dies || first_die_id >= ZF_ZCN_MAX_DIES ||
		num_dies > ZF_ZCN_MAX_DIES - first_die_id) {
		dev_err(dev, "Invalid physical die range for this driver\n");
		return -EINVAL;
	}

	dev_dbg(dev, "Detected %u dies starting at physical die %u\n",
		num_dies, first_die_id);

	pmus = devm_kcalloc(&pdev->dev, num_dies + 1, sizeof(*pmus),
			    GFP_KERNEL);
	if (!pmus)
		return -ENOMEM;

	env_src = devm_kzalloc(dev, sizeof(*env_src), GFP_KERNEL);
	if (!env_src)
		return -ENOMEM;
	ret = zf_zcn_env_read_source(dev, num_dies, env_src);
	if (ret == -ENOENT) {
		dev_err(dev,
			"No zcn,* environment properties: load zcn-pmu-platform.ko "
			"or describe them in ACPI _DSD\n");
		return ret;
	}
	if (ret) {
		dev_err(dev,
			"Incomplete or inconsistent ZCN environment properties: %d\n",
			ret);
		return ret;
	}

	for (i = 0; i < num_dies; i++) {
		u32 die_id = env_src->unit_ids ? env_src->unit_ids[i] :
			first_die_id + i;
		int j;

		if (die_id >= ZF_ZCN_MAX_DIES) {
			dev_err(dev, "Invalid physical die ID %u\n", die_id);
			ret = -EINVAL;
			goto err_cleanup;
		}
		for (j = 0; j < i; j++) {
			u32 previous = env_src->unit_ids ? env_src->unit_ids[j] :
				first_die_id + j;

			if (previous == die_id) {
				dev_err(dev, "Physical die ID %u is listed more than once\n",
					die_id);
				ret = -EINVAL;
				goto err_cleanup;
			}
		}

		ret = zf_zcn_pmu_setup_die(pdev, env_src, i, die_id, &pmus[i]);
		if (ret)
			goto err_cleanup;

		ret = zf_zcn_pmu_register_die_pmu(pmus[i]);
		if (ret) {
			dev_err(&pdev->dev, "Failed to register PMU for die %u\n",
				die_id);
			goto err_cleanup;
		}
	}

	platform_set_drvdata(pdev, pmus);

	dev_dbg(&pdev->dev, "Registered %u ZCN PMU instances starting at physical die %u\n",
		 num_dies, env_src->unit_ids ? env_src->unit_ids[0] : first_die_id);

	return 0;

err_cleanup:
	zf_zcn_pmu_unregister_dies(pmus, i);
	return ret;
}

static int zf_zcn_pmu_remove_all(struct platform_device *pdev)
{
	struct zf_zcn_pmu **pmus = platform_get_drvdata(pdev);
	int count = 0;

	if (!pmus)
		return 0;

	while (pmus[count])
		count++;

	zf_zcn_pmu_unregister_dies(pmus, count);
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 11, 0)
static void zf_zcn_pmu_remove(struct platform_device *pdev)
{
	zf_zcn_pmu_remove_all(pdev);
}
#else
static int zf_zcn_pmu_remove(struct platform_device *pdev)
{
	return zf_zcn_pmu_remove_all(pdev);
}
#endif

static const struct acpi_device_id zf_zcn_pmu_acpi_match[] = {
	{ "LPRS0005", 0 },
	{},
};
MODULE_DEVICE_TABLE(acpi, zf_zcn_pmu_acpi_match);

static struct platform_driver zf_zcn_pmu_driver = {
	.driver = {
		.name = "zf-zcn-pmu",
		.acpi_match_table = ACPI_PTR(zf_zcn_pmu_acpi_match),
	},
	.probe = zf_zcn_pmu_probe,
	.remove = zf_zcn_pmu_remove,
};

static int __init zf_zcn_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "perf/zcn-pmu:online",
				      NULL, zf_zcn_pmu_cpu_offline);
	if (ret < 0)
		return ret;

	zf_zcn_pmu_hp_state = ret;

	ret = platform_driver_register(&zf_zcn_pmu_driver);
	if (ret) {
		cpuhp_remove_multi_state(zf_zcn_pmu_hp_state);
		return ret;
	}

	return 0;
}

static void __exit zf_zcn_pmu_exit(void)
{
	platform_driver_unregister(&zf_zcn_pmu_driver);
	cpuhp_remove_multi_state(zf_zcn_pmu_hp_state);
}

module_init(zf_zcn_pmu_init);
module_exit(zf_zcn_pmu_exit);

MODULE_DESCRIPTION("LeapRise ZF(ZhuFeng) ZCN PMU driver");
MODULE_AUTHOR("Peng Lijian");
MODULE_AUTHOR("Bai Jian");
MODULE_AUTHOR("Ye Lei");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:zf-zcn-pmu");
