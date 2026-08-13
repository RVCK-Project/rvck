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

#define div_mask(d)	((1 << ((d)->width)) - 1)

DEFINE_SPINLOCK(zhihe_clk_lock);

static void zhihe_clk_unregister_divider(struct clk *clk)
{
	struct clk_hw *hw = __clk_get_hw(clk);
	struct clk_zhihediv *zhihe_div;

	if (!hw)
		return;

	zhihe_div = container_of(to_clk_divider(hw), struct clk_zhihediv,
				 divider);
	clk_hw_unregister(hw);
	kfree(zhihe_div);
}

static void zhihe_clk_unregister_gate_shared(struct clk *clk)
{
	struct clk_hw *hw = __clk_get_hw(clk);
	struct clk_zhihegate *zhihe_gate;

	if (!hw)
		return;

	zhihe_gate = container_of(to_clk_gate(hw), struct clk_zhihegate, gate);
	clk_hw_unregister(hw);
	kfree(zhihe_gate);
}

void zhihe_unregister_clocks(struct zhihe_clk_subsys *priv)
{
	struct clk **clks = priv->clk_data->clks;
	unsigned int i;

	for (i = priv->num_info; i-- > 0;) {
		const struct zhihe_clk_info *info = &priv->info[i];
		u32 id = info->id;

		if (id >= priv->clk_data->clk_num || IS_ERR_OR_NULL(clks[id]))
			continue;

		switch (info->type) {
		case CLK_TYPE_FIXED:
			clk_unregister_fixed_rate(clks[id]);
			break;
		case CLK_TYPE_PLL:
			zhihe_pll_unregister(clks[id]);
			break;
		case CLK_TYPE_FIXED_FACTOR:
			clk_unregister_fixed_factor(clks[id]);
			break;
		case CLK_TYPE_DIVIDER:
		case CLK_TYPE_DIVIDER_CLOSEST:
			zhihe_clk_unregister_divider(clks[id]);
			break;
		case CLK_TYPE_GATE:
			clk_unregister_gate(clks[id]);
			break;
		case CLK_TYPE_GATE_SHARED:
			zhihe_clk_unregister_gate_shared(clks[id]);
			break;
		case CLK_TYPE_MUX:
			clk_unregister_mux(clks[id]);
			break;
		default:
			WARN_ON_ONCE(1);
			clk_unregister(clks[id]);
			break;
		}

		clks[id] = ERR_PTR(-ENOENT);
	}
}

static inline struct clk_zhihediv *to_clk_zhihediv(struct clk_hw *hw)
{
	struct clk_divider *divider = to_clk_divider(hw);

	return container_of(divider, struct clk_zhihediv, divider);
}

static unsigned long clk_zhihediv_recalc_rate(struct clk_hw *hw,
					      unsigned long parent_rate)
{
	struct clk_zhihediv *zhihe_div = to_clk_zhihediv(hw);

	return zhihe_div->ops->recalc_rate(&zhihe_div->divider.hw, parent_rate);
}

static long clk_zhihediv_round_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long *prate)
{
	struct clk_zhihediv *zhihe_div = to_clk_zhihediv(hw);

	return zhihe_div->ops->round_rate(&zhihe_div->divider.hw, rate, prate);
}

static int clk_zhihediv_set_rate(struct clk_hw *hw, unsigned long rate,
				 unsigned long parent_rate)
{
	struct clk_zhihediv *zhihe_div = to_clk_zhihediv(hw);
	struct clk_divider *div = to_clk_divider(hw);
	unsigned int divider, value;
	unsigned long flags = 0;
	u32 val;

	/**
	 * The clk-divider will calculate the node frequency by rounding up
	 * based on the parent frequency and the target divider.
	 * This calculation is to restore accurate frequency divider.
	 */
	divider = DIV64_U64_ROUND_CLOSEST(parent_rate, rate);

	/* DIV is zero based divider, but CDE is not */
	if (zhihe_div->div_type == MUX_TYPE_DIV)
		value = divider;
	else
		value = divider - 1;

	/* handle the div valid range */
	if (value > zhihe_div->max_div)
		value = zhihe_div->max_div;
	if (value < zhihe_div->min_div)
		value = zhihe_div->min_div;

	spin_lock_irqsave(div->lock, flags);

	val = readl(div->reg);

	if (zhihe_div->sync_en != NO_DIV_EN) {
		val &= ~BIT(zhihe_div->sync_en);
		writel(val, div->reg);
		udelay(1);
	}

	val &= ~(div_mask(div) << div->shift);
	val |= value << div->shift;
	writel(val, div->reg);

	if (zhihe_div->sync_en != NO_DIV_EN) {
		udelay(1);
		val |= BIT(zhihe_div->sync_en);
		writel(val, div->reg);
	}

	spin_unlock_irqrestore(div->lock, flags);

	return 0;
}

static const struct clk_ops clk_zhihediv_ops = {
	.recalc_rate = clk_zhihediv_recalc_rate,
	.round_rate = clk_zhihediv_round_rate,
	.set_rate = clk_zhihediv_set_rate,
};

static struct clk *zhihe_clk_divider_internal(struct device *dev,
					      const char *name,
					      const char *parent,
					      void __iomem *reg, u8 shift,
					      u8 width, u8 sync,
					      enum zhihe_div_type div_type,
					      u16 min, u16 max, bool closest)
{
	struct clk_zhihediv *zhihe_div;
	struct clk_hw *hw;
	struct clk_init_data init;
	int ret;

	zhihe_div = kzalloc(sizeof(*zhihe_div), GFP_KERNEL);
	if (!zhihe_div)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &clk_zhihediv_ops;
	init.flags = CLK_SET_RATE_PARENT;
	init.parent_names = parent ? &parent : NULL;
	init.num_parents = parent ? 1 : 0;

	zhihe_div->divider.reg = reg;
	zhihe_div->divider.shift = shift;
	zhihe_div->divider.width = width;
	zhihe_div->divider.lock = &zhihe_clk_lock;
	zhihe_div->divider.hw.init = &init;
	zhihe_div->ops = &clk_divider_ops;
	zhihe_div->sync_en = sync;
	zhihe_div->div_type = div_type;
	if (zhihe_div->div_type == MUX_TYPE_DIV)
		zhihe_div->divider.flags = CLK_DIVIDER_ONE_BASED;

	if (closest)
		zhihe_div->divider.flags |= CLK_DIVIDER_ROUND_CLOSEST;

	zhihe_div->min_div = min > ((1 << width) - 1) ?
				((1 << width) - 1) : min;
	zhihe_div->max_div = max > ((1 << width) - 1) ?
				((1 << width) - 1) : max;

	hw = &zhihe_div->divider.hw;

	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(zhihe_div);
		return ERR_PTR(ret);
	}

	return hw->clk;
}

static struct clk *zhihe_clk_divider(struct device *dev, const char *name,
				     const char *parent, void __iomem *reg,
			     u8 shift, u8 width, u8 sync,
			     enum zhihe_div_type div_type,
			     u16 min, u16 max)
{
	return zhihe_clk_divider_internal(dev, name, parent, reg, shift,
					  width, sync, div_type, min,
					  max, false);
}

static struct clk *zhihe_clk_divider_closest(struct device *dev,
					     const char *name, const char *parent,
					     void __iomem *reg,
				     u8 shift, u8 width, u8 sync,
				     enum zhihe_div_type div_type,
				     u16 min, u16 max)
{
	return zhihe_clk_divider_internal(dev, name, parent, reg, shift,
					  width, sync, div_type, min,
					  max, true);
}

static inline struct clk_zhihegate *to_clk_zhihegate(struct clk_hw *hw)
{
	struct clk_gate *gate = to_clk_gate(hw);

	return container_of(gate, struct clk_zhihegate, gate);
}

static int clk_zhihe_gate_share_is_enabled(struct clk_hw *hw)
{
	struct clk_zhihegate *zhihe_gate = to_clk_zhihegate(hw);

	return zhihe_gate->ops->is_enabled(hw);
}

static int clk_zhihe_gate_share_enable(struct clk_hw *hw)
{
	struct clk_zhihegate *zhihe_gate = to_clk_zhihegate(hw);

	if (zhihe_gate->share_count && (*zhihe_gate->share_count)++ > 0)
		return 0;

	return zhihe_gate->ops->enable(hw);
}

static void clk_zhihe_gate_share_disable(struct clk_hw *hw)
{
	struct clk_zhihegate *zhihe_gate = to_clk_zhihegate(hw);

	if (zhihe_gate->share_count) {
		if (WARN_ON(*zhihe_gate->share_count == 0))
			return;
		else if (--(*zhihe_gate->share_count) > 0)
			return;
	}

	zhihe_gate->ops->disable(hw);
}

static void clk_zhihe_gate_share_disable_unused(struct clk_hw *hw)
{
	struct clk_zhihegate *zhihe_gate = to_clk_zhihegate(hw);

	if (!zhihe_gate->share_count || *zhihe_gate->share_count == 0)
		return zhihe_gate->ops->disable(hw);
}

static const struct clk_ops clk_zhihegate_share_ops = {
	.enable = clk_zhihe_gate_share_enable,
	.disable = clk_zhihe_gate_share_disable,
	.disable_unused = clk_zhihe_gate_share_disable_unused,
	.is_enabled = clk_zhihe_gate_share_is_enabled,
};

struct clk *zhihe_clk_register_gate_shared(struct device *dev, const char *name,
					   const char *parent, unsigned long flags,
					   void __iomem *reg, u8 shift,
					   spinlock_t *lock, unsigned int *share_count)
{
	struct clk_zhihegate *zhihe_gate;
	struct clk_hw *hw;
	struct clk_init_data init;
	int ret;

	zhihe_gate = kzalloc(sizeof(*zhihe_gate), GFP_KERNEL);
	if (!zhihe_gate)
		return ERR_PTR(-ENOMEM);

	zhihe_gate->gate.reg = reg;
	zhihe_gate->gate.bit_idx = shift;
	zhihe_gate->gate.flags = 0;
	zhihe_gate->gate.lock = lock;
	zhihe_gate->gate.hw.init = &init;
	zhihe_gate->ops = &clk_gate_ops;
	zhihe_gate->share_count = share_count;

	init.name = name;
	init.ops = &clk_zhihegate_share_ops;
	init.flags = flags;
	init.parent_names = parent ? &parent : NULL;
	init.num_parents = parent ? 1 : 0;

	hw = &zhihe_gate->gate.hw;

	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(zhihe_gate);
		return ERR_PTR(ret);
	}

	return hw->clk;
}

int zhihe_parse_regbase(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zhihe_clk_subsys *priv = dev_get_drvdata(dev);
	int ret = 0;

	for (int i = 0; i < priv->num_regs; i++) {
		priv->regs[i].base = devm_platform_ioremap_resource_byname(pdev,
									   priv->regs[i].name);
		if (WARN_ON(IS_ERR(priv->regs[i].base))) {
			ret = PTR_ERR(priv->regs[i].base);
			return ret;
		}
	}

	return ret;
}

static char *str_die_suffix(struct device *dev, const char *name, int id)
{
	return devm_kasprintf(dev, GFP_KERNEL, "%s_die%d", name, id);
}

static const char *const *zhihe_add_mux_suffix(struct platform_device *pdev, int i)
{
	struct device *dev = &pdev->dev;
	struct zhihe_clk_subsys *priv = dev_get_drvdata(dev);
	const char *const *original_parents = priv->info[i].mux.parents;
	u8 num_parents = priv->info[i].mux.num_parents;
	const char **new_parents;

	if (priv->die_num == 0)
		return original_parents;

	new_parents = devm_kzalloc(dev, sizeof(char *) * num_parents, GFP_KERNEL);
	if (!new_parents)
		return ERR_PTR(-ENOMEM);

	for (int j = 0; j < num_parents; j++) {
		const char *original_name = original_parents[j];
		char *suffix_name;

		suffix_name = str_die_suffix(dev, original_name, priv->die_num);
		if (!suffix_name) {
			dev_err(dev, "Failed to allocate memory for suffix_name\n");
			return ERR_PTR(-ENOMEM);
		}

		new_parents[j] = suffix_name;
	}
	return new_parents;
}

int zhihe_register_clock(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zhihe_clk_subsys *priv = dev_get_drvdata(dev);
	struct device_node *np = dev->of_node;
	struct clk **clks = priv->clk_data->clks;
	unsigned int freq;
	int i, ret;

	/* Update PLL frequency if defined in DTS. */
	for (i = 0; i < priv->num_plls; i++) {
		ret = of_property_read_u32(np, priv->plls[i].name, &freq);
		if (ret)
			continue;
		priv->plls[i].rate_table[0].vco_rate = freq;
		priv->plls[i].rate_table[0].rate =
			freq / priv->plls[i].rate_table[0].postdiv1;
	}

	for (i = 0; i < priv->num_info; i++) {
		struct zhihe_clk_info *info = &priv->info[i];
		const char *name = info->name;
		const char *parent = info->parent;
		struct clk *clk;
		void __iomem *base;
		u32 id = info->id;

		if (id >= priv->clk_data->clk_num)
			return dev_err_probe(dev, -EINVAL,
					     "invalid clock id %u for %s\n",
					     id, info->name);
		if (info->reg >= priv->num_regs)
			return dev_err_probe(dev, -EINVAL,
					     "invalid register index %u for %s\n",
					     info->reg, info->name);

		base = priv->regs[info->reg].base + info->shift;
		if (priv->die_num != 0) {
			name = str_die_suffix(dev, info->name, priv->die_num);
			if (!name)
				return -ENOMEM;

			if (info->parent[0]) {
				parent = str_die_suffix(dev, info->parent,
							priv->die_num);
				if (!parent)
					return -ENOMEM;
			}
		}

		switch (info->type) {
		case CLK_TYPE_FIXED:
			clk = zhihe_clk_fixed(dev, name, parent, info->fixed.freq);
			break;
		case CLK_TYPE_FIXED_FACTOR:
			clk = zhihe_clk_fixed_factor(dev, name, parent,
						     info->fixed_factor.mult,
						     info->fixed_factor.div);
			break;
		case CLK_TYPE_PLL:
			clk = zhihe_pll(dev, name, parent, base, info->pll);
			break;
		case CLK_TYPE_DIVIDER:
			clk = zhihe_clk_divider(dev, name, parent, base,
						info->bit_idx, info->width,
						info->divider.sync,
						info->divider.div_type,
						info->divider.min,
						info->divider.max);
			break;
		case CLK_TYPE_DIVIDER_CLOSEST:
			clk = zhihe_clk_divider_closest(dev, name, parent, base,
							info->bit_idx, info->width,
							info->divider.sync,
							info->divider.div_type,
							info->divider.min,
							info->divider.max);
			break;
		case CLK_TYPE_GATE:
			clk = zhihe_clk_gate(dev, name, parent, base,
					     info->bit_idx, info->flags);
			break;
		case CLK_TYPE_GATE_SHARED:
			clk = zhihe_clk_gate_shared(dev, name, parent, base,
						    info->bit_idx,
						    info->gate_shared.share_count);
			break;
		case CLK_TYPE_MUX: {
			const char *const *parents;

			parents = zhihe_add_mux_suffix(pdev, i);
			if (IS_ERR(parents))
				return PTR_ERR(parents);
			clk = zhihe_clk_mux_flags(dev, name, base,
						  info->bit_idx, info->width,
						  parents, info->mux.num_parents,
						  info->mux.flags);
			break;
		}
		default:
			return dev_err_probe(dev, -EINVAL,
					     "invalid clock type %u for %s\n",
					     info->type, name);
		}

		if (IS_ERR(clk))
			return dev_err_probe(dev, PTR_ERR(clk),
					     "failed to register clock %s\n", name);
		clks[id] = clk;
	}

	return 0;
}
