// SPDX-License-Identifier: GPL-2.0-only
/*
 * Spacemit memory range process
 *
 * Copyright (c) 2023, spacemit Corporation.
 *
 */

#include <linux/init.h>
#include <linux/of_address.h>
#include <linux/of_clk.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

static const struct of_device_id spacemit_mem_range_dt_match[] = {
	{ .compatible = "spacemit-dram-bus", },
	{ },
};

static int spacemit_mem_range_probe(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver spacemit_mem_range_driver = {
	.probe = spacemit_mem_range_probe,
	.driver = {
		.name   = "spacemit-mem-range",
		.of_match_table = spacemit_mem_range_dt_match,
	},
};

static int __init spacemit_mem_range_drv_register(void)
{
	return platform_driver_register(&spacemit_mem_range_driver);
}

core_initcall(spacemit_mem_range_drv_register);
