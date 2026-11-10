/* SPDX-License-Identifier: GPL-2.0 */
/* ZCN register ABI. Board-specific IDs, bases and layouts live in zcn_env.h. */
#ifndef _LPRS_ZF_ZCN_PMU_H
#define _LPRS_ZF_ZCN_PMU_H

#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#else
#include <errno.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))
#endif

#define ZF_ZCN_ENV_UNITS_LIMIT	128
#define ZF_ZCN_ENV_DIES_LIMIT	8

#define ZF_ZCN_FIXED_NODEID_SHIFT		18

#define ZF_ZCN_FIXED_CHN_CFG_DELTA		0x8000ULL
#define ZF_ZCN_FIXED_CHN_TAM_DELTA		0x20000ULL
#define ZF_ZCN_FIXED_SCN_TAM_DELTA		0x30000ULL
#define ZF_ZCN_FIXED_VMN_CFG_DELTA		0x10000ULL
#define ZF_ZCN_FIXED_ZRI_CFG		0x940000ULL
#define ZF_ZCN_FIXED_ZRI_TAM		0x950000ULL
#define ZF_ZCN_FIXED_CIO_STRIDE		0x1000000ULL
#define ZF_ZCN_FIXED_ZRP_CFG		0x40000ULL
#define ZF_ZCN_FIXED_ZRP_TAM		0x60000ULL
#define ZF_ZCN_FIXED_ZHP_TAM		0x10000ULL
#define ZF_ZCN_FIXED_CPG_STRIDE		0x800000ULL
#define ZF_ZCN_FIXED_CPG_RA		0x10000ULL
#define ZF_ZCN_FIXED_CPG_HA_TAM		0x20000ULL

#define ZF_ZCN_FIXED_EVT_SEL		0x300ULL
#define ZF_ZCN_FIXED_EVT_SEL_ZRP		0x730ULL
#define ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL	0xa608ULL
#define ZF_ZCN_FIXED_TAM_CNT_BASE		0xa300ULL
#define ZF_ZCN_FIXED_SBOX_TAM_CFG		0x308ULL
#define ZF_ZCN_FIXED_SBOX_CNT_BASE		0x310ULL
#define ZF_ZCN_FIXED_SBOX_TAM_CTRL		0x550ULL

enum zf_zcn_fixed_region {
	ZF_ZCN_FIXED_CCD_CFG,
	ZF_ZCN_FIXED_IOD_CIO,
	ZF_ZCN_FIXED_IOD_CPG,
	ZF_ZCN_FIXED_IOD_SOCBAR,
	ZF_ZCN_FIXED_CCD_SOCBAR_NE,
	ZF_ZCN_FIXED_CCD_SOCBAR_NW,
};
#define ZF_ZCN_FIXED_NR_REGIONS	6

enum zf_zcn_fixed_type {
	ZF_ZCN_FIXED_T_CHN,
	ZF_ZCN_FIXED_T_SBOX,
	ZF_ZCN_FIXED_T_ZHI,
	ZF_ZCN_FIXED_T_VMN,
	ZF_ZCN_FIXED_T_ZRI,
	ZF_ZCN_FIXED_T_ZRP,
	ZF_ZCN_FIXED_T_ZHP,
	ZF_ZCN_FIXED_T_CLL,
	ZF_ZCN_FIXED_T_CRT,
	ZF_ZCN_FIXED_T_CRR,
};

struct zf_zcn_fixed_row {
	enum zf_zcn_fixed_type type;
	enum zf_zcn_fixed_region region;
	u16 node;
	u64 cfg;
	u64 tam;
};

struct zf_zcn_env_topo {
	u32 chn_count;
	u32 scn_count;
	u32 zri_count;
	u32 cio_count;
	u32 cpg_count;
	u32 sbox_count;
	u32 node_count;
	u16 tac_id;
	const u16 *chn_ids;
	const u16 *scn_ids;
	const u16 *sbox_ids;
	const u16 *zri_ids;
	const u16 *zrp_ids;
	const u16 *zhp_ids;
	const u16 *cpg_ids;
	const u64 *scn_base;
	const char *const *zri_window;
};

static inline const char *zf_zcn_fixed_region_name(enum zf_zcn_fixed_region region);

static inline int zf_zcn_fixed_region_by_name(const char *name)
{
	enum zf_zcn_fixed_region r;

	for (r = 0; r < ZF_ZCN_FIXED_NR_REGIONS; r++) {
		const char *known = zf_zcn_fixed_region_name(r);

		if (name && known && !strcmp(name, known))
			return r;
	}
	return -EINVAL;
}

static inline u64 zf_zcn_fixed_scn_base(const struct zf_zcn_env_topo *env, u32 index)
{
	if (env->scn_base[index])
		return env->scn_base[index];
	return (u64)env->scn_ids[index] << ZF_ZCN_FIXED_NODEID_SHIFT;
}

static inline u32 zf_zcn_env_node_count(const struct zf_zcn_env_topo *env)
{
	return env->chn_count + env->sbox_count + env->scn_count * 2 +
		env->zri_count + env->cio_count * 2 + env->cpg_count * 3;
}

static inline int zf_zcn_env_row_at(const struct zf_zcn_env_topo *env, u32 idx,
				 struct zf_zcn_fixed_row *row)
{
	u32 k;
	u64 base;

	if (idx >= env->node_count)
		return -ERANGE;

	if (idx < env->chn_count) {
		row->type = ZF_ZCN_FIXED_T_CHN;
		row->region = ZF_ZCN_FIXED_CCD_CFG;
		row->node = env->chn_ids[idx];
		base = (u64)row->node << ZF_ZCN_FIXED_NODEID_SHIFT;
		row->cfg = base + ZF_ZCN_FIXED_CHN_CFG_DELTA;
		row->tam = base + ZF_ZCN_FIXED_CHN_TAM_DELTA;
		return 0;
	}
	idx -= env->chn_count;

	if (idx < env->sbox_count) {
		row->type = ZF_ZCN_FIXED_T_SBOX;
		row->region = ZF_ZCN_FIXED_CCD_CFG;
		row->node = env->sbox_ids[idx];
		row->cfg = (u64)row->node << ZF_ZCN_FIXED_NODEID_SHIFT;
		row->tam = row->cfg;
		return 0;
	}
	idx -= env->sbox_count;

	if (idx < env->scn_count * 2) {
		k = idx / 2;
		base = zf_zcn_fixed_scn_base(env, k);
		row->region = ZF_ZCN_FIXED_CCD_CFG;
		row->node = env->scn_ids[k];
		row->type = (idx % 2) ? ZF_ZCN_FIXED_T_VMN : ZF_ZCN_FIXED_T_ZHI;
		row->cfg = (idx % 2) ? base + ZF_ZCN_FIXED_VMN_CFG_DELTA : base;
		row->tam = base + ZF_ZCN_FIXED_SCN_TAM_DELTA;
		return 0;
	}
	idx -= env->scn_count * 2;

	if (idx < env->zri_count) {
		int region = zf_zcn_fixed_region_by_name(env->zri_window[idx]);

		if (region < 0)
			return -EINVAL;
		row->type = ZF_ZCN_FIXED_T_ZRI;
		row->node = env->zri_ids[idx];
		row->region = (enum zf_zcn_fixed_region)region;
		row->cfg = ZF_ZCN_FIXED_ZRI_CFG;
		row->tam = ZF_ZCN_FIXED_ZRI_TAM;
		return 0;
	}
	idx -= env->zri_count;

	if (idx < env->cio_count * 2) {
		k = idx / 2;
		base = (u64)k * ZF_ZCN_FIXED_CIO_STRIDE;
		row->region = ZF_ZCN_FIXED_IOD_CIO;
		if ((idx % 2) == 0) {
			row->type = ZF_ZCN_FIXED_T_ZRP;
			row->node = env->zrp_ids[k];
			row->cfg = base + ZF_ZCN_FIXED_ZRP_CFG;
			row->tam = base + ZF_ZCN_FIXED_ZRP_TAM;
		} else {
			row->type = ZF_ZCN_FIXED_T_ZHP;
			row->node = env->zhp_ids[k];
			row->cfg = base;
			row->tam = base + ZF_ZCN_FIXED_ZHP_TAM;
		}
		return 0;
	}
	idx -= env->cio_count * 2;

	if (idx < env->cpg_count * 3) {
		static const u64 cpg_cfg[ZF_ZCN_FIXED_T_CRR - ZF_ZCN_FIXED_T_CLL + 1] = {
			0, ZF_ZCN_FIXED_CPG_RA, ZF_ZCN_FIXED_CPG_HA_TAM,
		};

		k = idx / 3;
		base = (u64)k * ZF_ZCN_FIXED_CPG_STRIDE;
		row->type = (enum zf_zcn_fixed_type)(ZF_ZCN_FIXED_T_CLL + idx % 3);
		row->region = ZF_ZCN_FIXED_IOD_CPG;
		row->node = env->cpg_ids[k];
		row->cfg = base + cpg_cfg[idx % 3];
		row->tam = base + ZF_ZCN_FIXED_CPG_HA_TAM;
		return 0;
	}

	return -ERANGE;
}

static inline u64 zf_zcn_fixed_evt_sel_offset(enum zf_zcn_fixed_type type)
{
	return type == ZF_ZCN_FIXED_T_ZRP ? ZF_ZCN_FIXED_EVT_SEL_ZRP : ZF_ZCN_FIXED_EVT_SEL;
}

static inline unsigned int zf_zcn_fixed_num_slots(enum zf_zcn_fixed_type type)
{
	return type == ZF_ZCN_FIXED_T_SBOX ? 2 : 4;
}

static inline u64 zf_zcn_fixed_counter_offset(enum zf_zcn_fixed_type type,
					   unsigned int slot)
{
	u64 base = type == ZF_ZCN_FIXED_T_SBOX ? ZF_ZCN_FIXED_SBOX_CNT_BASE :
					       ZF_ZCN_FIXED_TAM_CNT_BASE;

	return base + slot * 8;
}

static inline u64 zf_zcn_fixed_control_offset(enum zf_zcn_fixed_type type)
{
	return type == ZF_ZCN_FIXED_T_SBOX ? ZF_ZCN_FIXED_SBOX_TAM_CTRL :
					  ZF_ZCN_FIXED_TAM_AMU_FUNC_CTRL;
}

static inline const char *zf_zcn_fixed_type_name(enum zf_zcn_fixed_type type)
{
	static const char *const names[] = {
		[ZF_ZCN_FIXED_T_CHN]	= "chn",
		[ZF_ZCN_FIXED_T_SBOX]	= "sbox",
		[ZF_ZCN_FIXED_T_ZHI]	= "zhi",
		[ZF_ZCN_FIXED_T_VMN]	= "vmn",
		[ZF_ZCN_FIXED_T_ZRI]	= "zri",
		[ZF_ZCN_FIXED_T_ZRP]	= "zrp",
		[ZF_ZCN_FIXED_T_ZHP]	= "zhp",
		[ZF_ZCN_FIXED_T_CLL]	= "cll",
		[ZF_ZCN_FIXED_T_CRT]	= "crt",
		[ZF_ZCN_FIXED_T_CRR]	= "crr",
	};

	return (unsigned int)type < ARRAY_SIZE(names) ? names[type] : NULL;
}

static inline const char *zf_zcn_fixed_region_name(enum zf_zcn_fixed_region region)
{
	static const char *const names[] = {
		[ZF_ZCN_FIXED_CCD_CFG]		= "ccd_cfg",
		[ZF_ZCN_FIXED_IOD_CIO]		= "iod_cio",
		[ZF_ZCN_FIXED_IOD_CPG]		= "iod_cpg",
		[ZF_ZCN_FIXED_IOD_SOCBAR]		= "iod_socbar",
		[ZF_ZCN_FIXED_CCD_SOCBAR_NE]		= "ccd_socbar_ne",
		[ZF_ZCN_FIXED_CCD_SOCBAR_NW]		= "ccd_socbar_nw",
	};

	return (unsigned int)region < ARRAY_SIZE(names) ? names[region] : NULL;
}

#endif /* _LPRS_ZF_ZCN_PMU_H */
