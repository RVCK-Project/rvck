// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 */

#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/reset.h>

#include "core.h"

struct dwc3_zhihe {
	struct device *dev;
	struct regmap *usb31_sysreg;
	struct platform_device *dwc3;
	struct platform_device *usb20_phy;
	struct clk_bulk_data *clks;
	int num_clocks;
};

static int dwc3_zhihe_probe(struct platform_device *pdev)
{
	struct device		*dev = &pdev->dev;
	struct device_node	*np  = dev->of_node;
	struct dwc3_zhihe	*zhihe;
	struct device_node *dwc3_np;
	int			ret;

	zhihe = devm_kzalloc(&pdev->dev, sizeof(*zhihe), GFP_KERNEL);
	if (!zhihe)
		return -ENOMEM;

	platform_set_drvdata(pdev, zhihe);
	zhihe->dev = &pdev->dev;

	zhihe->usb31_sysreg = syscon_regmap_lookup_by_phandle(np, "syscon");
	if (IS_ERR(zhihe->usb31_sysreg))
		return dev_err_probe(dev, PTR_ERR(zhihe->usb31_sysreg), "Failed to get regmap\n");

	ret = clk_bulk_get_all(zhihe->dev, &zhihe->clks);
	if (ret < 0)
		return dev_err_probe(dev, ret,
				     "failed to get DWC3 bulk clks\n");
	zhihe->num_clocks = ret;

	pm_runtime_set_active(dev);
	pm_runtime_enable(dev);
	pm_runtime_get_noresume(dev);

	ret = clk_bulk_prepare_enable(zhihe->num_clocks, zhihe->clks);
	if (ret) {
		pm_runtime_put_noidle(dev);
		pm_runtime_disable(dev);
		return dev_err_probe(dev, ret, "failed to enable DWC3 bulk clks\n");
	}

	dwc3_np = of_get_compatible_child(np, "snps,dwc3");
	if (!dwc3_np) {
		clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);
		pm_runtime_put_noidle(dev);
		pm_runtime_disable(dev);
		return dev_err_probe(dev, -ENODEV, "No DWC3 subnode found\n");
	}

	ret = of_platform_populate(np, NULL, NULL, dev);
	if (ret) {
		of_node_put(dwc3_np);
		clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);
		pm_runtime_put_noidle(dev);
		pm_runtime_disable(dev);
		return dev_err_probe(dev, ret, "failed to register dwc3 core\n");
	}

	zhihe->dwc3 = of_find_device_by_node(dwc3_np);
	of_node_put(dwc3_np);
	if (!zhihe->dwc3) {
		of_platform_depopulate(dev);
		clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);
		pm_runtime_put_noidle(dev);
		pm_runtime_disable(dev);
		return dev_err_probe(dev, -ENODEV, "failed to get dwc3 platform device\n");
	}

	return 0;
}

static int dwc3_zhihe_remove(struct platform_device *pdev)
{
	struct dwc3_zhihe *zhihe = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	of_platform_depopulate(zhihe->dev);

	clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);
	pm_runtime_put_sync(dev);
	pm_runtime_disable(dev);
	pm_runtime_set_suspended(dev);

	return 0;
}

static const struct of_device_id dwc3_zhihe_of_match[] = {
	{ .compatible = "zhihe,usb31" },
	{ },
};
MODULE_DEVICE_TABLE(of, dwc3_zhihe_of_match);

#ifdef CONFIG_PM
static int __maybe_unused dwc3_zhihe_suspend(struct device *dev)
{
	return 0;
}

static int __maybe_unused dwc3_zhihe_resume(struct device *dev)
{
	return 0;
}

static int __maybe_unused dwc3_zhihe_runtime_suspend(struct device *dev)
{
	struct dwc3_zhihe *priv = dev_get_drvdata(dev);

	clk_bulk_disable_unprepare(priv->num_clocks, priv->clks);

	return 0;
}

static int __maybe_unused dwc3_zhihe_runtime_resume(struct device *dev)
{
	struct dwc3_zhihe *priv = dev_get_drvdata(dev);
	int ret;

	ret = clk_bulk_prepare_enable(priv->num_clocks, priv->clks);
	if (ret)
		return ret;

	return 0;
}

static const struct dev_pm_ops dwc3_zhihe_dev_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(dwc3_zhihe_suspend, dwc3_zhihe_resume)
	SET_RUNTIME_PM_OPS(dwc3_zhihe_runtime_suspend,
			   dwc3_zhihe_runtime_resume, NULL)
};

#define DEV_PM_OPS	(&dwc3_zhihe_dev_pm_ops)
#else
#define DEV_PM_OPS	NULL
#endif /* CONFIG_PM */

static struct platform_driver dwc3_zhihe_driver = {
	.probe		= dwc3_zhihe_probe,
	.remove		= dwc3_zhihe_remove,
	.driver		= {
		.name	= "dwc3-zhihe",
		.pm	= DEV_PM_OPS,
		.of_match_table	= dwc3_zhihe_of_match,
	},
};

module_platform_driver(dwc3_zhihe_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DesignWare DWC3 ZHIHE Glue Driver");
MODULE_AUTHOR("Zhiguo Zhu <zhiguo.zhu@linux.alibaba.com>");
