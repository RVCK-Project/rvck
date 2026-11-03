// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 ZhiHe Computing Limited.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>

/* USB20 BLK SYSREG registers */
#define PHY_CFG				0x4
#define PHY_DM_PULLDOWN			BIT(1)
#define PHY_DP_PULLDOWN			BIT(0)
#define PHY_DMDP_PULLDOWN		(PHY_DM_PULLDOWN | PHY_DP_PULLDOWN)

#define USB2_PHY_INIT			0x0
#define USB2_PHY_INIT_VALUE		0x40b67b5b
#define USB31_USB2_PHY_INIT_VALUE	0x1000

struct zhihe_usb2_phy;

struct zhihe_usb2_phy_soc_data {
	unsigned int num_phys;
};

struct zhihe_usb2_phy_instance {
	struct phy *phy;
	struct regmap *regs;
	struct reset_control *reset;
	u32 init_reg;
	u32 init_value;
	u32 pulldown_mask;
};

struct zhihe_usb2_phy {
	const struct zhihe_usb2_phy_soc_data *data;
	struct zhihe_usb2_phy_instance phys[2];
};

static const struct regmap_config zhihe_usb2_phy_regmap_cfg = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.disable_locking = true,
	.max_register = 0x40,
};

static struct regmap *
zhihe_usb2_phy_regmap_init(struct device *dev, void __iomem *base)
{
	return devm_regmap_init_mmio(dev, base,
				     &zhihe_usb2_phy_regmap_cfg);
}

static int zhihe_usb2_phy_init(struct phy *phy)
{
	struct zhihe_usb2_phy_instance *instance = phy_get_drvdata(phy);
	int ret;

	if (instance->pulldown_mask) {
		ret = regmap_set_bits(instance->regs, PHY_CFG,
				      instance->pulldown_mask);
		if (ret)
			return ret;
	}

	ret = reset_control_assert(instance->reset);
	if (ret)
		return ret;

	usleep_range(100, 150);

	ret = regmap_write(instance->regs, instance->init_reg,
			   instance->init_value);
	if (ret)
		return ret;

	ret = reset_control_deassert(instance->reset);
	if (ret)
		return ret;

	usleep_range(80, 100);

	return 0;
}

static int zhihe_usb2_phy_exit(struct phy *phy)
{
	struct zhihe_usb2_phy_instance *instance = phy_get_drvdata(phy);

	return reset_control_assert(instance->reset);
}

static const struct phy_ops zhihe_usb2_phy_ops = {
	.init = zhihe_usb2_phy_init,
	.exit = zhihe_usb2_phy_exit,
	.owner = THIS_MODULE,
};

static struct phy *zhihe_usb2_phy_xlate(struct device *dev,
					struct of_phandle_args *args)
{
	struct zhihe_usb2_phy *priv = dev_get_drvdata(dev);
	unsigned int index = 0;

	if (priv->data->num_phys > 1) {
		if (args->args_count != 1)
			return ERR_PTR(-EINVAL);
		index = args->args[0];
	} else if (args->args_count) {
		return ERR_PTR(-EINVAL);
	}

	if (index >= priv->data->num_phys)
		return ERR_PTR(-ENODEV);

	return priv->phys[index].phy;
}

static int zhihe_usb2_phy_get_resources(struct platform_device *pdev,
					struct zhihe_usb2_phy *priv)
{
	struct device *dev = &pdev->dev;
	static const char * const reset_names[] = { "usb0", "usb1" };
	unsigned int i;

	if (priv->data->num_phys == 1) {
		struct zhihe_usb2_phy_instance *instance = &priv->phys[0];
		void __iomem *base;

		base = devm_platform_ioremap_resource(pdev, 0);
		if (IS_ERR(base))
			return PTR_ERR(base);

		instance->regs = zhihe_usb2_phy_regmap_init(dev, base);
		if (IS_ERR(instance->regs))
			return dev_err_probe(dev, PTR_ERR(instance->regs),
					     "failed to create PHY regmap\n");

		instance->reset =
			devm_reset_control_get_exclusive(dev, "phy");
		if (IS_ERR(instance->reset))
			return dev_err_probe(dev, PTR_ERR(instance->reset),
					     "failed to get PHY reset\n");

		instance->init_reg = 0;
		instance->init_value = USB31_USB2_PHY_INIT_VALUE;

		return 0;
	}

	for (i = 0; i < priv->data->num_phys; i++) {
		struct zhihe_usb2_phy_instance *instance = &priv->phys[i];
		void __iomem *base;

		base = devm_platform_ioremap_resource(pdev, i);
		if (IS_ERR(base))
			return PTR_ERR(base);

		instance->regs = zhihe_usb2_phy_regmap_init(dev, base);
		if (IS_ERR(instance->regs))
			return dev_err_probe(dev, PTR_ERR(instance->regs),
					     "failed to create PHY regmap\n");

		instance->reset =
			devm_reset_control_get_exclusive(dev, reset_names[i]);
		if (IS_ERR(instance->reset))
			return dev_err_probe(dev, PTR_ERR(instance->reset),
					     "failed to get %s reset\n",
					     reset_names[i]);

		instance->init_reg = USB2_PHY_INIT;
		instance->init_value = USB2_PHY_INIT_VALUE;
		instance->pulldown_mask = PHY_DMDP_PULLDOWN << (2 * i);
	}

	return 0;
}

static int zhihe_usb2_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy_provider *provider;
	struct zhihe_usb2_phy *priv;
	unsigned int i;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->data = device_get_match_data(dev);
	if (!priv->data)
		return -EINVAL;

	ret = zhihe_usb2_phy_get_resources(pdev, priv);
	if (ret)
		return ret;

	for (i = 0; i < priv->data->num_phys; i++) {
		struct zhihe_usb2_phy_instance *instance = &priv->phys[i];

		instance->phy = devm_phy_create(dev, NULL,
						&zhihe_usb2_phy_ops);
		if (IS_ERR(instance->phy))
			return dev_err_probe(dev, PTR_ERR(instance->phy),
					     "failed to create PHY%u\n", i);

		phy_set_drvdata(instance->phy, instance);
	}

	dev_set_drvdata(dev, priv);

	provider = devm_of_phy_provider_register(dev, zhihe_usb2_phy_xlate);
	if (IS_ERR(provider))
		return dev_err_probe(dev, PTR_ERR(provider),
				     "failed to register PHY provider\n");

	return 0;
}

static const struct zhihe_usb2_phy_soc_data a210_dwc2_usb2_phy_data = {
	.num_phys = 2,
};

static const struct zhihe_usb2_phy_soc_data a210_dwc3_usb2_phy_data = {
	.num_phys = 1,
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
MODULE_AUTHOR("Zhu Guangzhao <zhuzg.zhu@zhcomputing.com>");
