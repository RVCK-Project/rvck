// SPDX-License-Identifier: GPL-2.0
/*
 * ZhiHe A210 USB 2.0 PHY driver
 *
 * Copyright (C) 2025 ZhiHe Computing Limited.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>

/* USB20 BLK SYSREG registers */
#define PHY_CFG				0x4

#define USB2_PHY_INIT			0x0
#define USB2_PHY_INIT_VALUE		0x40b67b5b
#define USB31_USB2_PHY_INIT		0xa0
#define USB31_USB2_PHY_INIT_VALUE	0x1000

struct zhihe_usb2_phy_data {
	u32 init_offset;
	u32 init_value;
	bool syscon_regmap;
};

struct zhihe_usb2_phy {
	struct phy *phy;
	struct regmap *regs;
	struct reset_control *reset;
	u32 init_offset;
	u32 init_value;
	u32 pulldown_mask;
};

static const struct regmap_config zhihe_usb2_phy_regmap_cfg = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.disable_locking = true,
};

static int zhihe_usb2_phy_init(struct phy *phy)
{
	struct zhihe_usb2_phy *priv = phy_get_drvdata(phy);
	int ret;

	if (priv->pulldown_mask) {
		ret = regmap_set_bits(priv->regs, PHY_CFG,
				      priv->pulldown_mask);
		if (ret)
			return ret;
	}

	ret = reset_control_assert(priv->reset);
	if (ret)
		return ret;

	usleep_range(100, 150);

	ret = regmap_write(priv->regs, priv->init_offset, priv->init_value);
	if (ret)
		return ret;

	ret = reset_control_deassert(priv->reset);
	if (ret)
		return ret;

	usleep_range(80, 100);

	return 0;
}

static int zhihe_usb2_phy_exit(struct phy *phy)
{
	struct zhihe_usb2_phy *priv = phy_get_drvdata(phy);

	return reset_control_assert(priv->reset);
}

static const struct phy_ops zhihe_usb2_phy_ops = {
	.init = zhihe_usb2_phy_init,
	.exit = zhihe_usb2_phy_exit,
	.owner = THIS_MODULE,
};

static struct regmap *zhihe_usb2_phy_mmio_regmap(struct platform_device *pdev)
{
	struct regmap_config cfg = zhihe_usb2_phy_regmap_cfg;
	struct resource *res;
	void __iomem *base;

	base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
	if (IS_ERR(base))
		return ERR_CAST(base);

	cfg.max_register = resource_size(res) - sizeof(u32);

	return devm_regmap_init_mmio(&pdev->dev, base, &cfg);
}

static struct regmap *zhihe_usb2_phy_get_shared_regmap(struct device *dev)
{
	struct platform_device *syscon_pdev;
	struct device_node *syscon_np;
	struct regmap *regmap;

	syscon_np = of_parse_phandle(dev->of_node, "zhihe,usb-syscon", 0);
	if (!syscon_np)
		return ERR_PTR(-ENODEV);

	syscon_pdev = of_find_device_by_node(syscon_np);
	of_node_put(syscon_np);
	if (!syscon_pdev)
		return ERR_PTR(-EPROBE_DEFER);

	regmap = dev_get_regmap(&syscon_pdev->dev, NULL);
	if (!regmap) {
		regmap = ERR_PTR(-EPROBE_DEFER);
		goto put_device;
	}

	if (!device_link_add(dev, &syscon_pdev->dev,
			     DL_FLAG_AUTOREMOVE_CONSUMER))
		regmap = ERR_PTR(-EINVAL);

put_device:
	put_device(&syscon_pdev->dev);

	return regmap;
}

static int zhihe_usb2_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct zhihe_usb2_phy_data *data = device_get_match_data(dev);
	struct zhihe_usb2_phy *priv;
	struct phy_provider *provider;
	int ret;

	if (!data)
		return -EINVAL;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	if (data->syscon_regmap) {
		priv->regs = zhihe_usb2_phy_get_shared_regmap(dev);
	} else {
		priv->regs = zhihe_usb2_phy_mmio_regmap(pdev);
		ret = device_property_read_u32(dev, "zhihe,pulldown-mask",
					       &priv->pulldown_mask);
		if (ret)
			return dev_err_probe(dev, ret,
					     "missing zhihe,pulldown-mask\n");
	}
	if (IS_ERR(priv->regs))
		return dev_err_probe(dev, PTR_ERR(priv->regs),
				     "failed to get PHY regmap\n");

	priv->reset = devm_reset_control_get_exclusive(dev, NULL);
	if (IS_ERR(priv->reset))
		return dev_err_probe(dev, PTR_ERR(priv->reset),
				     "failed to get PHY reset\n");

	priv->init_offset = data->init_offset;
	priv->init_value = data->init_value;

	priv->phy = devm_phy_create(dev, NULL, &zhihe_usb2_phy_ops);
	if (IS_ERR(priv->phy))
		return dev_err_probe(dev, PTR_ERR(priv->phy),
				     "failed to create PHY\n");

	phy_set_drvdata(priv->phy, priv);

	provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(provider))
		return dev_err_probe(dev, PTR_ERR(provider),
				     "failed to register PHY provider\n");

	return 0;
}

static const struct zhihe_usb2_phy_data a210_dwc2_usb2_phy_data = {
	.init_offset = USB2_PHY_INIT,
	.init_value = USB2_PHY_INIT_VALUE,
};

static const struct zhihe_usb2_phy_data a210_dwc3_usb2_phy_data = {
	.init_offset = USB31_USB2_PHY_INIT,
	.init_value = USB31_USB2_PHY_INIT_VALUE,
	.syscon_regmap = true,
};

static const struct of_device_id zhihe_usb2_phy_of_match[] = {
	{
		.compatible = "zhihe,a210-dwc2-usb2-phy",
		.data = &a210_dwc2_usb2_phy_data,
	},
	{
		.compatible = "zhihe,a210-dwc3-usb2-phy",
		.data = &a210_dwc3_usb2_phy_data,
	},
	{ }
};
MODULE_DEVICE_TABLE(of, zhihe_usb2_phy_of_match);

static struct platform_driver zhihe_usb2_phy_driver = {
	.probe = zhihe_usb2_phy_probe,
	.driver = {
		.name = "phy-zhihe-a210-usb2",
		.of_match_table = zhihe_usb2_phy_of_match,
	},
};

module_platform_driver(zhihe_usb2_phy_driver);

MODULE_DESCRIPTION("ZhiHe A210 USB 2.0 PHY driver");
MODULE_LICENSE("GPL");
