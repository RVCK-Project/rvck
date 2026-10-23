// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/slab.h>

/* USB20 BLK SYSREG registers */
#define PHY_CFG		0x4
#define PHY_DM_PULLDOWN	BIT(1)
#define PHY_DP_PULLDOWN	BIT(0)
#define PHY_DMDP_PULLDOWN	(PHY_DM_PULLDOWN | PHY_DP_PULLDOWN)

struct usb2_phy_seq {
	u32 addr;
	u32 val;
};

struct zhihe_usb2_phy_priv {
	struct device *dev;
	struct phy *phy;
	struct regmap *regs;
	enum phy_mode mode;
	u32 phy_id;
	int num_clks;
	struct clk_bulk_data *clks;
	struct reset_control *phy_rst;
	struct usb2_phy_seq *init_seq;
	int num_init_seq;
	void (*usb_phy_config)(struct zhihe_usb2_phy_priv *priv);
};

static const struct regmap_config zhihe_usb2_phy_regmap_cfg = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.disable_locking = true,
	.max_register = 0x40,
};

static void usb_phy0_config(struct zhihe_usb2_phy_priv *priv)
{
	regmap_set_bits(priv->regs, PHY_CFG, PHY_DMDP_PULLDOWN);
}

static void usb_phy1_config(struct zhihe_usb2_phy_priv *priv)
{
	regmap_set_bits(priv->regs, PHY_CFG, PHY_DMDP_PULLDOWN << 2);
}

static int zhihe_usb2_phy_init(struct phy *phy)
{
	struct zhihe_usb2_phy_priv *priv = phy_get_drvdata(phy);
	int ret;

	ret = clk_bulk_prepare_enable(priv->num_clks, priv->clks);
	if (ret)
		return ret;
	/* Pull-up the  PHY reset */
	if (priv->usb_phy_config)
		priv->usb_phy_config(priv);

	ret = reset_control_assert(priv->phy_rst);
	if (ret)
		goto disable_clocks;

	usleep_range(100, 150);

	for (int i = 0; i < priv->num_init_seq; i++) {
		struct usb2_phy_seq *seq = &priv->init_seq[i];

		regmap_write(priv->regs, seq->addr, seq->val);
		dev_info(priv->dev, " [0x%03x] = 0x%08x\n", seq->addr, seq->val);
	}

	ret = reset_control_deassert(priv->phy_rst);
	if (ret)
		goto disable_clocks;

	usleep_range(80, 100);

	return 0;
disable_clocks:
	clk_bulk_disable_unprepare(priv->num_clks, priv->clks);
	return ret;
}

static int zhihe_usb2_phy_exit(struct phy *phy)
{
	struct zhihe_usb2_phy_priv *priv = phy_get_drvdata(phy);

	/* Assert reset to power down PHY */
	reset_control_assert(priv->phy_rst);

	return 0;
}

static int zhihe_usb2_set_mode(struct phy *phy, enum phy_mode mode, int submode)
{
	struct zhihe_usb2_phy_priv *priv = phy_get_drvdata(phy);

	priv->mode = mode;
	return 0;
}

static const struct phy_ops zhihe_usb2_phy_ops = {
	.init = zhihe_usb2_phy_init,
	.exit = zhihe_usb2_phy_exit,
	.set_mode = zhihe_usb2_set_mode,
	.owner = THIS_MODULE,
};

static int zhihe_usb2_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node	*np  = dev->of_node;
	struct zhihe_usb2_phy_priv *priv;
	struct phy_provider *phy_provider;
	int ret, size;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;

	ret = of_property_read_u32(dev->of_node, "zhihe,phy-id", &priv->phy_id);
	if (ret)
		return dev_err_probe(dev, ret, "missing zhihe,phy-id\n");

	switch (priv->phy_id) {
	case 0:
		priv->usb_phy_config = usb_phy0_config;
		break;
	case 1:
		priv->usb_phy_config = usb_phy1_config;
		break;
	}

	/* Check if this PHY shares registers with USB31 controller */
	if (priv->phy_id == 2) {
		priv->regs = syscon_regmap_lookup_by_phandle(np, "syscon");
		if (IS_ERR(priv->regs))
			return dev_err_probe(dev, PTR_ERR(priv->regs), "Failed to get regmap\n");
	} else {
		/* PHY0/PHY1 have dedicated register space */
		void __iomem *base;

		base = devm_platform_get_and_ioremap_resource(pdev, 0, NULL);
		if (IS_ERR(base))
			return PTR_ERR(base);
		priv->regs = devm_regmap_init_mmio(dev, base, &zhihe_usb2_phy_regmap_cfg);
		if (IS_ERR(priv->regs)) {
			dev_err(dev, "Couldn't create regmap\n");
			return PTR_ERR(priv->regs);
		}
	}

	/* Get PHY reset control */
	priv->phy_rst = devm_reset_control_get_optional_exclusive(dev, "phy-rst");
	if (IS_ERR(priv->phy_rst))
		return dev_err_probe(dev, PTR_ERR(priv->phy_rst),
				     "Couldn't get phy-rst\n");

	size = of_property_count_u32_elems(dev->of_node, "zhihe,init-seq");
	if (size < 0)
		size = 0;
	priv->num_init_seq = size / 2;
	priv->init_seq = devm_kmalloc_array(dev, priv->num_init_seq,
					    sizeof(*priv->init_seq), GFP_KERNEL);
	if (!priv->init_seq)
		return dev_err_probe(dev, -ENOMEM, "Couldn't allocate init_seq\n");

	ret = of_property_read_u32_array(dev->of_node, "zhihe,init-seq",
					 (u32 *)priv->init_seq, size);
	if (ret)
		return dev_err_probe(dev, ret,  "Couldn't read init_seq\n");

	/* Create PHY */
	priv->phy = devm_phy_create(dev, NULL, &zhihe_usb2_phy_ops);
	if (IS_ERR(priv->phy))
		return dev_err_probe(dev, PTR_ERR(priv->phy), "Couldn't create phy\n");

	dev_set_drvdata(dev, priv);
	phy_set_drvdata(priv->phy, priv);

	/* Register PHY provider */
	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		return dev_err_probe(dev, PTR_ERR(phy_provider),
				     "failed to register phy provider\n");

	return 0;
}

static const struct of_device_id zhihe_usb2_phy_of_match[] = {
	{ .compatible = "zhihe,a210-usb2-phy", },
	{ }
};
MODULE_DEVICE_TABLE(of, zhihe_usb2_phy_of_match);

static struct platform_driver zhihe_usb2_phy_driver = {
	.probe		= zhihe_usb2_phy_probe,
	.driver = {
		.name	= "phy-a210-usb2",
		.of_match_table = zhihe_usb2_phy_of_match,
	},
};

module_platform_driver(zhihe_usb2_phy_driver);

MODULE_DESCRIPTION("ZHIHE USB 2.0 PHY driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhu Guangzhao <zhuzg.zhu@zhcomputing.com>");
