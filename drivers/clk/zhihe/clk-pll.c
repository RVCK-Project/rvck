// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include "clk-helper.h"

#define ZHIHE_PLL_CFG0		0x0
#define ZHIHE_PLL_CFG1		0x04
#define ZHIHE_PLL_CFG2		0x8
#define ZHIHE_POSTDIV2_SHIFT	24
#define ZHIHE_POSTDIV2_MASK	GENMASK(26, 24)
#define ZHIHE_POSTDIV1_SHIFT	20
#define ZHIHE_POSTDIV1_MASK	GENMASK(22, 20)
#define ZHIHE_FBDIV_SHIFT	8
#define ZHIHE_FBDIV_MASK	GENMASK(19, 8)
#define ZHIHE_REFDIV_SHIFT	0
#define ZHIHE_REFDIV_MASK	GENMASK(5, 0)
#define ZHIHE_BYPASS_MASK	BIT(30)
#define ZHIHE_RST_MASK		BIT(29)
#define ZHIHE_DSMPD_MASK	BIT(24)
#define ZHIHE_DACPD_MASK	BIT(25)
#define ZHIHE_FRAC_MASK		GENMASK(23, 0)
#define ZHIHE_FRAC_SHIFT	0
#define ZHIHE_FRAC_DIV		BIT(24)

#define LOCK_TIMEOUT_US		10000

#define to_clk_zhihepll(_hw) container_of(_hw, struct clk_zhihepll, hw)

static int clk_zhihe_pll_wait_lock(struct clk_zhihepll *pll)
{
	u32 val;

	return readl_poll_timeout(pll->base + pll->pll_sts_off, val,
				  val & pll->pll_lock_bit, 0,
				  LOCK_TIMEOUT_US);
}

static int clk_zhihe_pll_prepare(struct clk_hw *hw)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	void __iomem *cfg1_off;
	u32 val;
	int ret;

	cfg1_off = pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1;
	val = readl_relaxed(cfg1_off);
	if (!(val & pll->pll_rst_bit))
		return 0;

	/* Enable RST */
	val |= pll->pll_rst_bit;
	writel_relaxed(val, cfg1_off);

	udelay(3);

	/* Disable RST */
	val &= ~pll->pll_rst_bit;
	writel_relaxed(val, cfg1_off);

	ret = clk_zhihe_pll_wait_lock(pll);
	if (ret)
		return ret;

	return 0;
}

static int clk_zhihe_pll_is_prepared(struct clk_hw *hw)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	u32 val;

	val = readl_relaxed(pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);

	return (val & pll->pll_rst_bit) ? 0 : 1;
}

static void clk_zhihe_pll_unprepare(struct clk_hw *hw)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	u32 val;

	val = readl_relaxed(pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);
	val |= pll->pll_rst_bit;
	writel_relaxed(val, pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);
}

static unsigned long clk_zhihe_pll_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	u32 refdiv, fbdiv, postdiv1, postdiv2, frac;
	u32 pll_cfg0, pll_cfg1;
	u64 fvco = 0;

	pll_cfg0 = readl_relaxed(pll->base + pll->cfg0_reg_off);
	pll_cfg1 = readl_relaxed(pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);
	refdiv = (pll_cfg0 & ZHIHE_REFDIV_MASK) >> ZHIHE_REFDIV_SHIFT;
	fbdiv = (pll_cfg0 & ZHIHE_FBDIV_MASK) >> ZHIHE_FBDIV_SHIFT;
	postdiv1 = (pll_cfg0 & ZHIHE_POSTDIV1_MASK) >> ZHIHE_POSTDIV1_SHIFT;
	postdiv2 = (pll_cfg0 & ZHIHE_POSTDIV2_MASK) >> ZHIHE_POSTDIV2_SHIFT;
	frac = (pll_cfg1 & ZHIHE_FRAC_MASK) >> ZHIHE_FRAC_SHIFT;

	/* rate calculation:
	 * INT mode: FOUTVCO = FREE * FBDIV / REFDIV
	 * FRAC mode:FOUTVCO = (FREE * FBDIV + FREE * FRAC/BIT(24)) / REFDIV
	 */
	if (pll->pll_mode == PLL_MODE_FRAC)
		fvco = (parent_rate * frac) / ZHIHE_FRAC_DIV;

	fvco += (parent_rate * fbdiv);
	do_div(fvco, refdiv);

	if (pll->out_type == ZHIHE_PLL_DIV)
		do_div(fvco, postdiv1 * postdiv2);

	return fvco;
}

static const struct zhihe_pll_rate_table *
zhihe_get_pll_div_settings(struct clk_zhihepll *pll, unsigned long rate)
{
	const struct zhihe_pll_rate_table *rate_table = pll->rate_table;
	int i;

	for (i = 0; i < pll->rate_count; i++)
		if (rate == rate_table[i].rate)
			return &rate_table[i];

	return NULL;
}

static const struct zhihe_pll_rate_table *
zhihe_get_pll_vco_settings(struct clk_zhihepll *pll, unsigned long rate)
{
	const struct zhihe_pll_rate_table *rate_table = pll->rate_table;
	int i;

	for (i = 0; i < pll->rate_count; i++)
		if (rate == rate_table[i].vco_rate)
			return &rate_table[i];

	return NULL;
}

static inline bool clk_zhihe_pll_change(struct clk_zhihepll *pll,
					const struct zhihe_pll_rate_table *rate)
{
	u32 refdiv_old, fbdiv_old, postdiv1_old, postdiv2_old, frac_old;
	u32 cfg0, cfg1;
	bool pll_changed;

	cfg0 = readl_relaxed(pll->base + pll->cfg0_reg_off);
	cfg1 = readl_relaxed(pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);

	refdiv_old = (cfg0 & ZHIHE_REFDIV_MASK) >> ZHIHE_REFDIV_SHIFT;
	fbdiv_old = (cfg0 & ZHIHE_FBDIV_MASK) >> ZHIHE_FBDIV_SHIFT;
	postdiv1_old = (cfg0 & ZHIHE_POSTDIV1_MASK) >> ZHIHE_POSTDIV1_SHIFT;
	postdiv2_old = (cfg0 & ZHIHE_POSTDIV2_MASK) >> ZHIHE_POSTDIV2_SHIFT;
	frac_old = (cfg1 & ZHIHE_FRAC_MASK) >> ZHIHE_FRAC_SHIFT;

	pll_changed = rate->refdiv != refdiv_old || rate->fbdiv != fbdiv_old ||
			rate->postdiv1 != postdiv1_old || rate->postdiv2 != postdiv2_old;
	if (pll->pll_mode == PLL_MODE_FRAC)
		pll_changed |= (rate->frac != frac_old);

	return pll_changed;
}

static int clk_zhihe_pll_set_rate(struct clk_hw *hw, unsigned long drate,
				  unsigned long prate)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	const struct zhihe_pll_rate_table *rate;
	void __iomem *cfg1_off;
	u32 tmp, div_val;
	int ret;

	if (pll->out_type == ZHIHE_PLL_VCO) {
		rate = zhihe_get_pll_vco_settings(pll, drate);
		if (!rate) {
			pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
			       drate, clk_hw_get_name(hw));
			return -EINVAL;
		}
	} else {
		rate = zhihe_get_pll_div_settings(pll, drate);
		if (!rate) {
			pr_err("%s: Invalid rate : %lu for pll clk %s\n", __func__,
			       drate, clk_hw_get_name(hw));
			return -EINVAL;
		}
	}

	if (!clk_zhihe_pll_change(pll, rate))
		return 0;

	/* Enable RST */
	cfg1_off = pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1;
	tmp = readl_relaxed(cfg1_off);
	tmp |= pll->pll_rst_bit;
	writel_relaxed(tmp, cfg1_off);

	div_val = (rate->refdiv << ZHIHE_REFDIV_SHIFT) |
		  (rate->fbdiv << ZHIHE_FBDIV_SHIFT) |
		  (rate->postdiv1 << ZHIHE_POSTDIV1_SHIFT) |
		  (rate->postdiv2 << ZHIHE_POSTDIV2_SHIFT);
	writel_relaxed(div_val, pll->base + pll->cfg0_reg_off);

	if (pll->pll_mode == PLL_MODE_FRAC) {
		tmp &= ~(ZHIHE_FRAC_MASK << ZHIHE_FRAC_SHIFT);
		tmp |= rate->frac;
		writel_relaxed(tmp, cfg1_off);
	}

	udelay(3);

	/* Disable RST */
	tmp &= ~pll->pll_rst_bit;
	writel_relaxed(tmp, cfg1_off);

	/* Wait Lock, ~20us cost */
	ret = clk_zhihe_pll_wait_lock(pll);
	if (ret)
		return ret;

	/* HW requires 30us for pll stable */
	usleep_range(30, 40);

	return 0;
}

static long clk_zhihe_pllvco_round_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long *prate)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	const struct zhihe_pll_rate_table *rate_table = pll->rate_table;
	unsigned long best = 0, now = 0;
	unsigned int i, best_i = 0;

	for (i = 0; i < pll->rate_count; i++) {
		now = rate_table[i].vco_rate;

		if (rate == now) {
			return rate_table[i].vco_rate;
		} else if (abs(now - rate) < abs(best - rate)) {
			best = now;
			best_i = i;
		}
	}

	/* return minimum supported value */
	return rate_table[best_i].vco_rate;
}

static long clk_zhihe_plldiv_round_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long *prate)
{
	struct clk_zhihepll *pll = to_clk_zhihepll(hw);
	const struct zhihe_pll_rate_table *rate_table = pll->rate_table;
	unsigned long best = 0, now = 0;
	unsigned int i, best_i = 0;

	for (i = 0; i < pll->rate_count; i++) {
		now = rate_table[i].rate;

		if (rate == now) {
			return rate_table[i].rate;
		} else if (abs(now - rate) < abs(best - rate)) {
			best = now;
			best_i = i;
		}
	}

	/* return minimum supported value */
	return rate_table[best_i].rate;
}

static const struct clk_ops clk_zhihe_pll_def_ops = {
	.recalc_rate	= clk_zhihe_pll_recalc_rate,
};

static const struct clk_ops clk_zhihe_pllvco_ops = {
	.prepare	= clk_zhihe_pll_prepare,
	.unprepare	= clk_zhihe_pll_unprepare,
	.is_prepared	= clk_zhihe_pll_is_prepared,
	.recalc_rate	= clk_zhihe_pll_recalc_rate,
	.round_rate	= clk_zhihe_pllvco_round_rate,
	.set_rate	= clk_zhihe_pll_set_rate,
};

static const struct clk_ops clk_zhihe_plldiv_ops = {
	.prepare	= clk_zhihe_pll_prepare,
	.unprepare	= clk_zhihe_pll_unprepare,
	.is_prepared	= clk_zhihe_pll_is_prepared,
	.recalc_rate	= clk_zhihe_pll_recalc_rate,
	.round_rate	= clk_zhihe_plldiv_round_rate,
	.set_rate	= clk_zhihe_pll_set_rate,
};

struct clk *zhihe_pll(struct device *dev, const char *name,
		      const char *parent_name, void __iomem *base,
		      const struct zhihe_clk_info_pll *pll_clk)
{
	struct clk_zhihepll *pll;
	struct clk *clk;
	struct clk_init_data init;
	u32 val;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.flags = pll_clk->flags;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	switch (pll_clk->out_type) {
	case ZHIHE_PLL_VCO:
		if (pll_clk->rate_table)
			init.ops = &clk_zhihe_pllvco_ops;
		break;
	case ZHIHE_PLL_DIV:
		if (pll_clk->rate_table)
			init.ops = &clk_zhihe_plldiv_ops;
		break;
	default:
		pr_err("%s: Unknown pll out type for pll clk %s\n",
		       __func__, name);
	};

	if (!pll_clk->rate_table)
		init.ops = &clk_zhihe_pll_def_ops;

	pll->base = base;
	pll->hw.init = &init;
	pll->out_type = pll_clk->out_type;
	pll->rate_table = pll_clk->rate_table;
	pll->rate_count = pll_clk->rate_count;
	pll->cfg0_reg_off = pll_clk->cfg0_reg_off;
	pll->pll_sts_off = pll_clk->pll_sts_off;
	pll->pll_lock_bit = pll_clk->pll_lock_bit;
	pll->pll_bypass_bit = pll_clk->pll_bypass_bit;
	pll->pll_rst_bit = pll_clk->pll_rst_bit;
	pll->pll_mode = pll_clk->pll_mode;

	val = readl_relaxed(pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);
	val &= ~pll->pll_bypass_bit;
	val |= ZHIHE_DACPD_MASK;
	val |= ZHIHE_DSMPD_MASK;
	if (pll->pll_mode == PLL_MODE_FRAC) {
		val &= ~ZHIHE_DSMPD_MASK;
		val &= ~ZHIHE_DACPD_MASK;
	}
	writel_relaxed(val, pll->base + pll->cfg0_reg_off + ZHIHE_PLL_CFG1);

	clk = clk_register(dev, &pll->hw);
	if (IS_ERR(clk)) {
		pr_err("failed to register pll %s %ld\n",
		       name, PTR_ERR(clk));
		kfree(pll);
	}

	return clk;
}

void zhihe_pll_unregister(struct clk *clk)
{
	struct clk_hw *hw = __clk_get_hw(clk);
	struct clk_zhihepll *pll;

	if (!hw)
		return;

	pll = to_clk_zhihepll(hw);
	clk_hw_unregister(hw);
	kfree(pll);
}
