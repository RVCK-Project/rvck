// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZhiHe A210 DWC3 glue driver
 *
 * Copyright (C) 2025 ZhiHe Computing Limited.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

struct dwc3_zhihe {
	struct device *dev;
	struct clk_bulk_data *clks;
	int num_clocks;
	struct reset_control *resets;
};

static int dwc3_zhihe_enable(struct dwc3_zhihe *zhihe)
{
	int ret;

	ret = clk_bulk_prepare_enable(zhihe->num_clocks, zhihe->clks);
	if (ret)
		return ret;

	ret = reset_control_assert(zhihe->resets);
	if (ret)
		goto disable_clocks;

	/* Not strict timing, just for safety. */
	udelay(2);

	ret = reset_control_deassert(zhihe->resets);
	if (ret)
		goto assert_reset;

	return 0;

assert_reset:
	reset_control_assert(zhihe->resets);
disable_clocks:
	clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);

	return ret;
}

static void dwc3_zhihe_disable(struct dwc3_zhihe *zhihe)
{
	reset_control_assert(zhihe->resets);
	clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);
}

static int dwc3_zhihe_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dwc3_zhihe *zhihe;
	int ret;

	zhihe = devm_kzalloc(dev, sizeof(*zhihe), GFP_KERNEL);
	if (!zhihe)
		return -ENOMEM;

	zhihe->dev = dev;
	platform_set_drvdata(pdev, zhihe);

	ret = devm_clk_bulk_get_all(dev, &zhihe->clks);
	if (ret < 0)
		return dev_err_probe(dev, ret, "failed to get clocks\n");

	zhihe->num_clocks = ret;

	zhihe->resets = devm_reset_control_array_get_exclusive(dev);
	if (IS_ERR(zhihe->resets))
		return dev_err_probe(dev, PTR_ERR(zhihe->resets),
				     "failed to get resets\n");

	ret = dwc3_zhihe_enable(zhihe);
	if (ret)
		return dev_err_probe(dev, ret,
				     "failed to enable USB controller\n");

	ret = of_platform_populate(dev->of_node, NULL, NULL, dev);
	if (ret) {
		dev_err_probe(dev, ret, "failed to populate DWC3 core\n");
		dwc3_zhihe_disable(zhihe);
		return ret;
	}

	return 0;
}

static void dwc3_zhihe_remove(struct platform_device *pdev)
{
	struct dwc3_zhihe *zhihe = platform_get_drvdata(pdev);

	of_platform_depopulate(zhihe->dev);
	dwc3_zhihe_disable(zhihe);
}

static int dwc3_zhihe_suspend(struct device *dev)
{
	struct dwc3_zhihe *zhihe = dev_get_drvdata(dev);
	int ret;

	ret = reset_control_assert(zhihe->resets);
	if (ret)
		return ret;

	clk_bulk_disable_unprepare(zhihe->num_clocks, zhihe->clks);

	return 0;
}

static int dwc3_zhihe_resume(struct device *dev)
{
	struct dwc3_zhihe *zhihe = dev_get_drvdata(dev);

	return dwc3_zhihe_enable(zhihe);
}

static const struct dev_pm_ops dwc3_zhihe_pm_ops = {
	SYSTEM_SLEEP_PM_OPS(dwc3_zhihe_suspend, dwc3_zhihe_resume)
};

static const struct of_device_id dwc3_zhihe_of_match[] = {
	{ .compatible = "zhihe,a210-dwc3" },
	{ }
};
MODULE_DEVICE_TABLE(of, dwc3_zhihe_of_match);

static struct platform_driver dwc3_zhihe_driver = {
	.probe = dwc3_zhihe_probe,
	.remove_new = dwc3_zhihe_remove,
	.driver = {
		.name = "dwc3-zhihe",
		.of_match_table = dwc3_zhihe_of_match,
		.pm = pm_sleep_ptr(&dwc3_zhihe_pm_ops),
	},
};
module_platform_driver(dwc3_zhihe_driver);

MODULE_AUTHOR("Zhiguo Zhu <zhiguo.zhu@linux.alibaba.com>");
MODULE_DESCRIPTION("ZhiHe A210 DWC3 glue driver");
MODULE_LICENSE("GPL");
