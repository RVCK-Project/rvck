/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#ifndef __CLK_ZHIHE_HELPER_H
#define __CLK_ZHIHE_HELPER_H

#include <linux/clk-provider.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#include <dt-bindings/clock/zhihe,a210-clk.h>

extern spinlock_t zhihe_clk_lock;

#define ZHIHE_CLK_NAME_SIZE 40

#define PLL_RATE(_vco, _rate, _r, _b, _f, _p, _k)       \
	{                                   \
		.vco_rate	=	(_vco),         \
		.rate		=	(_rate),        \
		.refdiv		=	(_r),           \
		.fbdiv		=	(_b),           \
		.frac		=	(_f),           \
		.postdiv1	=	(_p),           \
		.postdiv2	=	(_k),           \
	}

#define REG_NAMED(n, _name)	\
	[(n)] = {.name = (_name)}

#define CLK(_type, _id, _name, _parent) \
	.type = (_type),                \
	.id = (_id),                    \
	.name = (_name),                \
	.parent = (_parent)

#define FIXED(_id, _name, _parent, _freq) {             \
	CLK(CLK_TYPE_FIXED, (_id), (_name), (_parent)),     \
	.fixed = {.freq = (_freq),},                        \
}

#define PLL(_id, _name, _parent, _reg, _shift, _pll) {  \
	CLK(CLK_TYPE_PLL, (_id), (_name), (_parent)),   \
	.reg = (_reg),          \
	.shift = (_shift),      \
	.pll = (_pll),          \
}

#define FIXED_FACTOR(_id, _name, _parent, _mult, _div) {       \
	CLK(CLK_TYPE_FIXED_FACTOR, (_id), (_name), (_parent)), \
	.fixed_factor = {.mult = (_mult), .div = (_div),},     \
}

#define DIV(_id, _name, _parent, _reg, _shift, _bit_idx, _width, _sync, _div_type, _min, _max)  { \
	CLK(CLK_TYPE_DIVIDER, (_id), (_name), (_parent)),         \
	.reg = (_reg),               \
	.shift = (_shift),           \
	.bit_idx = (_bit_idx),       \
	.width = (_width),           \
	.divider = {                 \
		.sync = (_sync),         \
		.div_type = (_div_type), \
		.min = (_min),           \
		.max = (_max)            \
	}                            \
}

#define DIV_CLOSEST(_id, _name, _parent, _reg, _shift, _bit_idx,	\
		    _width, _sync, _div_type, _min, _max) {	\
	CLK(CLK_TYPE_DIVIDER_CLOSEST, (_id), (_name), (_parent)),\
	.reg = (_reg),                      \
	.shift = (_shift),                  \
	.bit_idx = (_bit_idx),              \
	.width = (_width),                  \
	.divider = {                        \
		.sync = (_sync),                \
		.div_type = (_div_type),        \
		.min = (_min),                  \
		.max = (_max)                   \
	}                                   \
}

#define GATE(_id, _name, _parent, _reg, _shift, _bit_idx) { \
	CLK(CLK_TYPE_GATE, (_id), (_name), (_parent)),      \
	.reg = (_reg),                                      \
	.shift = (_shift),                                  \
	.bit_idx = (_bit_idx),                              \
}

#define GATE_FLAGS(_id, _name, _parent, _reg, _shift, _bit_idx, _flags) { \
	CLK(CLK_TYPE_GATE, (_id), (_name), (_parent)),      \
	.reg = (_reg),                                      \
	.shift = (_shift),                                  \
	.bit_idx = (_bit_idx),                              \
	.flags = (_flags),                                  \
}

#define GATE_SHARED(_id, _name, _parent, _reg, _shift, _bit_idx, _count) { \
	CLK(CLK_TYPE_GATE_SHARED, (_id), (_name), (_parent)),              \
	.reg = (_reg),                     \
	.shift = (_shift),                 \
	.bit_idx = (_bit_idx),             \
	.gate_shared = {                   \
		.share_count = (_count)        \
	},                                 \
}

#define MUX(_id, _name, _reg, _shift, _bit_idx, _width, _parents, _flags) { \
	CLK(CLK_TYPE_MUX, (_id), (_name), ""),    \
	.reg = (_reg),                            \
	.shift = (_shift),                        \
	.bit_idx = (_bit_idx),                    \
	.width = (_width),                        \
	.mux = {                                  \
		.parents = (_parents),                \
		.num_parents = ARRAY_SIZE(_parents),  \
		.flags = (_flags)                     \
	},                                        \
}

#define PLL_PARAM(_pll, _out, _table, _cfg0, _sts, _lock, \
			  _bypass, _rst, _mode, _name) \
	[(_pll)] = {                              \
		.out_type = (_out),                   \
		.rate_table = (_table),               \
		.rate_count = ARRAY_SIZE(_table),     \
		.cfg0_reg_off = (_cfg0),              \
		.pll_sts_off = (_sts),                \
		.pll_lock_bit = (_lock),              \
		.pll_bypass_bit = (_bypass),          \
		.pll_rst_bit = (_rst),                \
		.pll_mode = (_mode),                  \
		.name = (_name),                      \
	}

#define CLK_SUBSYS(_name, _regs, _info, _pll, _num_pll, _die_num) { \
	.name = (_name),                    \
	.regs = (_regs),                    \
	.num_regs = ARRAY_SIZE(_regs),      \
	.info = (_info),                    \
	.num_info = ARRAY_SIZE(_info),      \
	.plls = (_pll),                     \
	.num_plls = (_num_pll),             \
	.die_num = (_die_num),              \
}

enum zhihe_pll_outtype {
	ZHIHE_PLL_VCO,
	ZHIHE_PLL_DIV,
};

enum zhihe_div_type {
	MUX_TYPE_DIV,
	MUX_TYPE_CDE,
};

enum zhihe_div_sync_type {
	NO_DIV_EN = 255,
};

enum zhihe_clk_types {
	CLK_TYPE_FIXED,
	CLK_TYPE_PLL,
	CLK_TYPE_FIXED_FACTOR,
	CLK_TYPE_DIVIDER,
	CLK_TYPE_DIVIDER_CLOSEST,
	CLK_TYPE_GATE,
	CLK_TYPE_GATE_SHARED,
	CLK_TYPE_MUX,
};

enum zhihe_pll_mode {
	PLL_MODE_FRAC,
	PLL_MODE_INT,
};

struct zhihe_clk_reg {
	void __iomem *base;
	char name[ZHIHE_CLK_NAME_SIZE];
};

/* clk summary info at subsys level */
struct zhihe_clk_subsys {
	char name[ZHIHE_CLK_NAME_SIZE];
	struct zhihe_clk_reg *regs;
	u32 num_regs;
	struct zhihe_clk_info *info;
	u32 num_info;
	struct zhihe_clk_info_pll *plls;
	u32 num_plls;
	struct clk_onecell_data *clk_data;
	char die_num;
};

struct clk_zhihepll {
	struct clk_hw hw;
	void __iomem *base;
	enum zhihe_pll_outtype out_type;
	enum zhihe_pll_mode pll_mode;
	const struct zhihe_pll_rate_table *rate_table;
	int rate_count;

	u32 cfg0_reg_off;
	u32 pll_sts_off;
	int pll_lock_bit;
	int pll_rst_bit;
	int pll_bypass_bit;
};

struct clk_zhihediv {
	struct clk_divider divider;
	enum zhihe_div_type div_type;
	u16 min_div;
	u16 max_div;
	u8 sync_en;
	const struct clk_ops *ops;
};

struct clk_zhihegate {
	struct clk_gate gate;
	unsigned int *share_count;
	const struct clk_ops *ops;
};

struct zhihe_pll_rate_table {
	unsigned long vco_rate;
	unsigned long rate;
	unsigned int refdiv;
	unsigned int fbdiv;
	unsigned int frac;
	unsigned int postdiv1;
	unsigned int postdiv2;
};

/* detailed clk info for each clk */

struct zhihe_clk_info_pll {
	enum zhihe_pll_outtype out_type;
	struct zhihe_pll_rate_table *rate_table;
	int rate_count;
	int flags;
	char name[ZHIHE_CLK_NAME_SIZE];
	u32 cfg0_reg_off;
	u32 pll_sts_off;
	int pll_lock_bit;
	int pll_rst_bit;
	int pll_bypass_bit;
	enum zhihe_pll_mode pll_mode;
};

struct zhihe_clk_info_fixed {
	unsigned int freq;
};

struct zhihe_clk_info_fixed_factor {
	unsigned int mult;
	unsigned int div;
};

struct zhihe_clk_info_divider {
	u8 sync;
	enum zhihe_div_type div_type;
	u16 min;
	u16 max;
};

struct zhihe_clk_info_gate_shared {
	unsigned int *share_count;
};

struct zhihe_clk_info_mux {
	const char *const *parents;
	int num_parents;
	unsigned long flags;
};

struct zhihe_clk_info {
	enum zhihe_clk_types type;
	u32 id;
	char name[ZHIHE_CLK_NAME_SIZE];
	char parent[ZHIHE_CLK_NAME_SIZE];
	u8 reg;
	u32 shift;
	u8 width;
	u8 bit_idx;
	unsigned long flags;
	union {
		struct zhihe_clk_info_fixed fixed;
		struct zhihe_clk_info_fixed_factor fixed_factor;
		struct zhihe_clk_info_pll *pll;
		struct zhihe_clk_info_divider divider;
		struct zhihe_clk_info_gate_shared gate_shared;
		struct zhihe_clk_info_mux mux;
	};
};

static inline struct clk *zhihe_clk_fixed_factor(struct device *dev,
						 const char *name, const char *parent,
						 unsigned int mult, unsigned int div)
{
	return clk_register_fixed_factor(dev, name, parent,
					 CLK_SET_RATE_PARENT, mult, div);
}

struct clk *zhihe_pll(struct device *dev, const char *name,
		      const char *parent_name, void __iomem *base,
		      const struct zhihe_clk_info_pll *pll_clk);
void zhihe_pll_unregister(struct clk *clk);

static inline struct clk *zhihe_clk_gate(struct device *dev,
					 const char *name, const char *parent,
					 void __iomem *reg, u8 shift,
					 unsigned long flags)
{
	return clk_register_gate(dev, name, parent, CLK_SET_RATE_PARENT | flags,
					 reg, shift, 0, &zhihe_clk_lock);
}

struct clk *zhihe_clk_register_gate_shared(struct device *dev, const char *name,
					   const char *parent, unsigned long flags,
					   void __iomem *reg, u8 shift,
					   spinlock_t *lock, unsigned int *share_count);

static inline struct clk *zhihe_clk_fixed(struct device *dev, const char *name,
					  const char *parent,
					  unsigned long rate)
{
	return clk_register_fixed_rate(dev, name, parent, 0, rate);
}

static inline struct clk *zhihe_clk_gate_shared(struct device *dev,
						const char *name,
						const char *parent,
						void __iomem *reg, u8 shift,
						unsigned int *share_count)
{
	return zhihe_clk_register_gate_shared(dev, name, parent,
					     CLK_SET_RATE_PARENT, reg,
					     shift, &zhihe_clk_lock,
					     share_count);
}

static inline struct clk *zhihe_clk_mux_flags(struct device *dev, const char *name,
					      void __iomem *reg, u8 shift, u8 width,
					      const char *const *parents, int num_parents,
					      unsigned long flags)
{
	return clk_register_mux(dev, name, parents, num_parents,
					flags, reg, shift, width, 0,
					&zhihe_clk_lock);
}

int add_die_suffix(struct platform_device *pdev);
int zhihe_register_clock(struct platform_device *pdev);
void zhihe_unregister_clocks(struct zhihe_clk_subsys *priv);
int zhihe_parse_regbase(struct platform_device *pdev);

#endif
