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
#include <linux/mfd/syscon.h>
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

struct zhihe_usb2_phy_inst_data {
	u32 init_value;
	u32 pulldown_mask;
};

struct zhihe_usb2_phy_data {
	unsigned int num_phys;
	unsigned int phy_cells;
	bool syscon_regmap;
	const struct zhihe_usb2_phy_inst_data *insts;
};

struct zhihe_usb2_phy_instance {
	struct phy *phy;
	struct regmap *regs;
	struct reset_control *reset;
	u32 init_value;
	u32 pulldown_mask;
};

struct zhihe_usb2_phy {
	const struct zhihe_usb2_phy_data *data;
	struct zhihe_usb2_phy_instance phys[];
};

static const struct regmap_config zhihe_usb2_phy_regmap_cfg = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.disable_locking = true,
};

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

	ret = regmap_write(instance->regs, USB2_PHY_INIT, instance->init_value);
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

	if (args->args_count != priv->data->phy_cells)
		return ERR_PTR(-EINVAL);

	if (priv->data->phy_cells)
		index = args->args[0];

	if (index >= priv->data->num_phys)
		return ERR_PTR(-ENODEV);

	return priv->phys[index].phy;
}

static struct regmap *zhihe_usb2_phy_mmio_regmap(struct platform_device *pdev,
						 unsigned int index)
{
	struct regmap_config cfg = zhihe_usb2_phy_regmap_cfg;
	struct resource *res;
	void __iomem *base;

	base = devm_platform_get_and_ioremap_resource(pdev, index, &res);
	if (IS_ERR(base))
		return ERR_CAST(base);

	cfg.max_register = resource_size(res) - sizeof(u32);

	return devm_regmap_init_mmio(&pdev->dev, base, &cfg);
}

static int zhihe_usb2_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct zhihe_usb2_phy_data *data = device_get_match_data(dev);
	struct zhihe_usb2_phy *priv;
	struct phy_provider *provider;
	unsigned int i;

	if (!data)
		return -EINVAL;

	priv = devm_kzalloc(dev, struct_size(priv, phys, data->num_phys),
			    GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->data = data;

	for (i = 0; i < data->num_phys; i++) {
		struct zhihe_usb2_phy_instance *instance = &priv->phys[i];

		if (data->syscon_regmap)
			instance->regs =
				syscon_node_to_regmap(dev->of_node->parent);
		else
			instance->regs = zhihe_usb2_phy_mmio_regmap(pdev, i);
		if (IS_ERR(instance->regs))
			return dev_err_probe(dev, PTR_ERR(instance->regs),
					     "failed to get PHY%u regmap\n", i);

		instance->reset =
			devm_reset_control_get_exclusive_by_index(dev, i);
		if (IS_ERR(instance->reset))
			return dev_err_probe(dev, PTR_ERR(instance->reset),
					     "failed to get PHY%u reset\n", i);

		instance->init_value = data->insts[i].init_value;
		instance->pulldown_mask = data->insts[i].pulldown_mask;

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

/*
 * Each DWC2 PHY lives in its own register block, where the DP/DM pulldown
 * fields of port 0 sit at bits [1:0] and those of port 1 at bits [3:2].
 */
static const struct zhihe_usb2_phy_inst_data a210_dwc2_phy_insts[] = {
	{ .init_value = USB2_PHY_INIT_VALUE, .pulldown_mask = PHY_DMDP_PULLDOWN },
	{ .init_value = USB2_PHY_INIT_VALUE, .pulldown_mask = PHY_DMDP_PULLDOWN << 2 },
};

static const struct zhihe_usb2_phy_inst_data a210_dwc3_phy_insts[] = {
	{ .init_value = USB31_USB2_PHY_INIT_VALUE },
};

static const struct zhihe_usb2_phy_data a210_dwc2_usb2_phy_data = {
	.num_phys = ARRAY_SIZE(a210_dwc2_phy_insts),
	.phy_cells = 1,
	.insts = a210_dwc2_phy_insts,
};

static const struct zhihe_usb2_phy_data a210_dwc3_usb2_phy_data = {
	.num_phys = ARRAY_SIZE(a210_dwc3_phy_insts),
	.phy_cells = 0,
	.syscon_regmap = true,
	.insts = a210_dwc3_phy_insts,
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
