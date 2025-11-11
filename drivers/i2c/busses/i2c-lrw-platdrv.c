// SPDX-License-Identifier: GPL-2.0+
/*
 *  I2C adapter driver for LRW
 *
 *  Copyright (c) 2025, LRW CORPORATION. All rights reserved.
 */

#include <linux/acpi.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/suspend.h>

#include "i2c-lrw-core.h"

static u32 i2c_lrw_get_clk_rate_khz(struct i2c_lrw_dev *dev)
{
	return clk_get_rate(dev->clk) / 1000;
}

#ifdef CONFIG_ACPI
static const struct acpi_device_id i2c_lrw_acpi_match[] = { { "LRWX0002", 0 },
								{} };
MODULE_DEVICE_TABLE(acpi, i2c_lrw_acpi_match);
#endif

#ifdef CONFIG_OF

static int i2c_lrw_of_configure(struct platform_device *pdev)
{
	struct i2c_lrw_dev *dev = platform_get_drvdata(pdev);

	switch (dev->flags & MODEL_MASK) {
	default:
		break;
	}

	return 0;
}

static const struct of_device_id i2c_lrw_of_match[] = {
	{
		.compatible = "lrw,sc-i2c",
	},
	{},
};
MODULE_DEVICE_TABLE(of, i2c_lrw_of_match);
#else

static inline int i2c_lrw_of_configure(struct platform_device *pdev)
{
	return -ENODEV;
}
#endif

static int i2c_lrw_plat_request_regs(struct i2c_lrw_dev *dev)
{
	struct platform_device *pdev = to_platform_device(dev->dev);
	int ret;

	switch (dev->flags & MODEL_MASK) {
	default:
		dev->base = devm_platform_ioremap_resource(pdev, 0);
		ret = PTR_ERR_OR_ZERO(dev->base);
		break;
	}

	return ret;
}

static const struct dmi_system_id i2c_lrw_hwmon_class_dmi[] = {
	{
		.ident = "lrw 0002",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "lrw"),
			DMI_MATCH(DMI_PRODUCT_NAME, "0002"),
		},
	},
	{ } /* terminate list */
};

static int i2c_lrw_plat_probe(struct platform_device *pdev)
{
	struct i2c_adapter *adap;
	struct i2c_lrw_dev *dev;
	struct i2c_timings *t;
	int irq, ret;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	dev = devm_kzalloc(&pdev->dev, sizeof(struct i2c_lrw_dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->flags = (uintptr_t)device_get_match_data(&pdev->dev);
	dev->dev = &pdev->dev;
	dev->irq = irq;

	platform_set_drvdata(pdev, dev);

	ret = i2c_lrw_plat_request_regs(dev);
	if (ret)
		return ret;

	dev->rst = devm_reset_control_get_optional_exclusive(&pdev->dev, NULL);
	if (IS_ERR(dev->rst))
		return PTR_ERR(dev->rst);

	reset_control_deassert(dev->rst);

	t = &dev->timings;
	i2c_parse_fw_timings(&pdev->dev, t, false);

	i2c_lrw_adjust_bus_speed(dev);

	if (pdev->dev.of_node)
		i2c_lrw_of_configure(pdev);

	if (has_acpi_companion(&pdev->dev))
		i2c_lrw_acpi_configure(&pdev->dev);
	else
		i2c_lrw_dt_configure(&pdev->dev);

	ret = i2c_lrw_validate_speed(dev);
	if (ret)
		goto exit_reset;

	ret = i2c_lrw_probe_lock_support(dev);
	if (ret)
		goto exit_reset;

	i2c_lrw_configure(dev);

	dev->pclk = devm_clk_get_optional(&pdev->dev, "pclk");
	if (IS_ERR(dev->pclk)) {
		ret = PTR_ERR(dev->pclk);
		goto exit_reset;
	}

	dev->clk = devm_clk_get_optional(&pdev->dev, NULL);
	if (IS_ERR(dev->clk)) {
		ret = PTR_ERR(dev->clk);
		goto exit_reset;
	}

	ret = i2c_lrw_prepare_clk(dev, true);
	if (ret)
		goto exit_reset;

	if (dev->clk) {
		u64 clk_khz;

		dev->get_clk_rate_khz = i2c_lrw_get_clk_rate_khz;
		clk_khz = dev->get_clk_rate_khz(dev);
		if (!dev->sda_hold_time && t->sda_hold_ns) {
			dev->sda_hold_time = div_u64(
				clk_khz * t->sda_hold_ns + 500000, 1000000);
		}
	}

	adap = &dev->adapter;
	adap->owner = THIS_MODULE;
	adap->class = dmi_check_system(i2c_lrw_hwmon_class_dmi) ?
				I2C_CLASS_HWMON :
				I2C_CLASS_DEPRECATED;
	ACPI_COMPANION_SET(&adap->dev, ACPI_COMPANION(&pdev->dev));
	adap->dev.of_node = pdev->dev.of_node;
	adap->nr = -1;

	ret = i2c_lrw_probe(dev);
	if (ret)
		goto exit_reset;

	return ret;

exit_reset:
	reset_control_assert(dev->rst);
	return ret;
}

static void i2c_lrw_plat_remove(struct platform_device *pdev)
{
	struct i2c_lrw_dev *dev = platform_get_drvdata(pdev);

	i2c_del_adapter(&dev->adapter);

	dev->disable(dev);
	reset_control_assert(dev->rst);
}

MODULE_ALIAS("platform:i2c_lrw");

static struct platform_driver i2c_lrw_driver = {
	.probe = i2c_lrw_plat_probe,
	.remove_new = i2c_lrw_plat_remove,
	.driver		= {
		.name	= "i2c_lrw",
		.of_match_table = of_match_ptr(i2c_lrw_of_match),
		.acpi_match_table = ACPI_PTR(i2c_lrw_acpi_match),
	},
};

static int __init i2c_lrw_init_driver(void)
{
	return platform_driver_register(&i2c_lrw_driver);
}
subsys_initcall(i2c_lrw_init_driver);

static void __exit i2c_lrw_exit_driver(void)
{
	platform_driver_unregister(&i2c_lrw_driver);
}
module_exit(i2c_lrw_exit_driver);

MODULE_AUTHOR("HXW");
MODULE_DESCRIPTION("LRW I2C bus adapter");
MODULE_LICENSE("GPL");
