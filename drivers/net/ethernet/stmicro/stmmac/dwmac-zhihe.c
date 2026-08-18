// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>

#include "stmmac_platform.h"

#define GMAC_CLKCTRL			0x00
#define GMAC_CTRL			0x04

#define GMAC_CLKCTRL_EN_ALL		GENMASK(4, 0)
#define GMAC_CLKCTRL_SPEED_MASK		GENMASK(9, 8)
#define GMAC_CLKCTRL_SPEED_1G		FIELD_PREP(GMAC_CLKCTRL_SPEED_MASK, 0)
#define GMAC_CLKCTRL_SPEED_10M		FIELD_PREP(GMAC_CLKCTRL_SPEED_MASK, 2)
#define GMAC_CLKCTRL_SPEED_100M		FIELD_PREP(GMAC_CLKCTRL_SPEED_MASK, 3)

#define GMAC_CTRL_INTF_MASK		GENMASK(3, 0)
#define GMAC_CTRL_INTF_RGMII		FIELD_PREP(GMAC_CTRL_INTF_MASK, 1)
#define GMAC_CTRL_INTF_RMII		FIELD_PREP(GMAC_CTRL_INTF_MASK, 4)

#define ZHIHE_DWMAC_NUM_CLKS		4

static const char * const zhihe_dwmac_clk_names[ZHIHE_DWMAC_NUM_CLKS] = {
	"stmmaceth",
	"pclk",
	"x2h_aclk",
	"x2h_hclk",
};

struct zhihe_dwmac {
	struct device *dev;
	struct regmap *sys_regmap;
	struct clk_bulk_data clks[ZHIHE_DWMAC_NUM_CLKS];
	struct plat_stmmacenet_data *plat_dat;
};

static int zhihe_dwmac_set_speed(struct zhihe_dwmac *dwmac,
				 unsigned int speed)
{
	phy_interface_t interface = dwmac->plat_dat->phy_interface;
	u32 interface_val, speed_val;
	int ret;

	switch (speed) {
	case SPEED_10:
		speed_val = GMAC_CLKCTRL_SPEED_10M;
		break;
	case SPEED_100:
		speed_val = GMAC_CLKCTRL_SPEED_100M;
		break;
	case SPEED_1000:
		speed_val = GMAC_CLKCTRL_SPEED_1G;
		break;
	default:
		return -EINVAL;
	}

	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
		interface_val = GMAC_CTRL_INTF_RGMII;
		break;
	case PHY_INTERFACE_MODE_RMII:
		interface_val = GMAC_CTRL_INTF_RMII;
		break;
	default:
		return -EINVAL;
	}

	ret = regmap_update_bits(dwmac->sys_regmap, GMAC_CLKCTRL,
				 GMAC_CLKCTRL_SPEED_MASK, speed_val);
	if (ret)
		return ret;

	return regmap_update_bits(dwmac->sys_regmap, GMAC_CTRL,
				  GMAC_CTRL_INTF_MASK, interface_val);
}

static int zhihe_dwmac_clkctrl_enable(struct zhihe_dwmac *dwmac,
				      bool enable)
{
	return regmap_update_bits(dwmac->sys_regmap, GMAC_CLKCTRL,
				  GMAC_CLKCTRL_EN_ALL,
				  enable ? GMAC_CLKCTRL_EN_ALL : 0);
}

static int zhihe_dwmac_get_resources(struct zhihe_dwmac *dwmac)
{
	struct device *dev = dwmac->dev;
	int i, ret;

	for (i = 0; i < ZHIHE_DWMAC_NUM_CLKS; i++)
		dwmac->clks[i].id = zhihe_dwmac_clk_names[i];

	ret = devm_clk_bulk_get(dev, ZHIHE_DWMAC_NUM_CLKS, dwmac->clks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to get clocks\n");

	dwmac->sys_regmap =
		syscon_regmap_lookup_by_phandle(dev->of_node, "zhihe,gmacsys");
	if (IS_ERR(dwmac->sys_regmap))
		return dev_err_probe(dev, PTR_ERR(dwmac->sys_regmap),
				     "failed to get GMAC syscon regmap\n");

	return 0;
}

static int zhihe_dwmac_peri_clk_enable(struct zhihe_dwmac *dwmac)
{
	int ret;

	ret = clk_bulk_prepare_enable(ZHIHE_DWMAC_NUM_CLKS, dwmac->clks);
	if (ret)
		dev_err(dwmac->dev, "failed to enable clocks: %d\n", ret);

	return ret;
}

static void zhihe_dwmac_peri_clk_disable(void *data)
{
	struct zhihe_dwmac *dwmac = data;

	clk_bulk_disable_unprepare(ZHIHE_DWMAC_NUM_CLKS, dwmac->clks);
}

static void zhihe_dwmac_fix_mac_speed(void *priv, unsigned int speed,
				      unsigned int mode)
{
	struct zhihe_dwmac *dwmac = priv;
	int ret;

	ret = zhihe_dwmac_set_speed(dwmac, speed);
	if (ret)
		dev_err(dwmac->dev, "failed to configure link speed %u: %d\n",
			speed, ret);
}

static int zhihe_dwmac_clks_config(void *priv, bool enabled)
{
	struct zhihe_dwmac *dwmac = priv;
	int err, ret = 0;

	if (enabled) {
		ret = zhihe_dwmac_peri_clk_enable(dwmac);
		if (ret)
			return ret;

		ret = reset_control_deassert(dwmac->plat_dat->stmmac_rst);
		if (ret)
			goto err_assert_stmmac;

		ret = reset_control_deassert(dwmac->plat_dat->stmmac_ahb_rst);
		if (ret)
			goto err_assert_ahb;

		ret = zhihe_dwmac_clkctrl_enable(dwmac, true);
		if (ret)
			goto err_assert_ahb;

		return 0;
	}

	ret = zhihe_dwmac_clkctrl_enable(dwmac, false);
	if (ret)
		dev_err(dwmac->dev, "failed to disable internal clocks: %d\n",
			ret);

	err = reset_control_assert(dwmac->plat_dat->stmmac_ahb_rst);
	if (err) {
		dev_err(dwmac->dev, "failed to assert AHB reset: %d\n", err);
		if (!ret)
			ret = err;
	}

	err = reset_control_assert(dwmac->plat_dat->stmmac_rst);
	if (err) {
		dev_err(dwmac->dev, "failed to assert GMAC reset: %d\n", err);
		if (!ret)
			ret = err;
	}

	zhihe_dwmac_peri_clk_disable(dwmac);

	return ret;

err_assert_ahb:
	reset_control_assert(dwmac->plat_dat->stmmac_ahb_rst);
err_assert_stmmac:
	reset_control_assert(dwmac->plat_dat->stmmac_rst);
	zhihe_dwmac_peri_clk_disable(dwmac);
	dev_err(dwmac->dev, "failed to enable GMAC resources: %d\n", ret);

	return ret;
}

static int zhihe_dwmac_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct device *dev = &pdev->dev;
	struct zhihe_dwmac *dwmac;
	int ret;

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret)
		return dev_err_probe(dev, ret,
				     "failed to get platform resources\n");

	plat_dat = devm_stmmac_probe_config_dt(pdev, stmmac_res.mac);
	if (IS_ERR(plat_dat))
		return dev_err_probe(dev, PTR_ERR(plat_dat),
				     "failed to parse device tree\n");

	dwmac = devm_kzalloc(dev, sizeof(*dwmac), GFP_KERNEL);
	if (!dwmac)
		return -ENOMEM;

	dwmac->dev = dev;
	dwmac->plat_dat = plat_dat;

	plat_dat->bsp_priv = dwmac;
	plat_dat->fix_mac_speed = zhihe_dwmac_fix_mac_speed;
	plat_dat->clks_config = zhihe_dwmac_clks_config;
	plat_dat->host_dma_width = 32;

	ret = zhihe_dwmac_get_resources(dwmac);
	if (ret)
		return ret;

	ret = zhihe_dwmac_peri_clk_enable(dwmac);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev, zhihe_dwmac_peri_clk_disable,
				       dwmac);
	if (ret)
		return ret;

	return stmmac_dvr_probe(dev, plat_dat, &stmmac_res);
}

static const struct of_device_id zhihe_dwmac_match[] = {
	{ .compatible = "zhihe,a210-dwmac" },
	{ }
};
MODULE_DEVICE_TABLE(of, zhihe_dwmac_match);

static struct platform_driver zhihe_dwmac_driver = {
	.probe = zhihe_dwmac_probe,
	.remove_new = stmmac_pltfr_remove,
	.driver = {
		.name = "zhihe-dwmac",
		.pm = &stmmac_pltfr_pm_ops,
		.of_match_table = zhihe_dwmac_match,
	},
};
module_platform_driver(zhihe_dwmac_driver);

MODULE_AUTHOR("Zhiguo Zhu <zhiguo.zhu@linux.alibaba.com>");
MODULE_DESCRIPTION("ZhiHe A210 DWMAC glue driver");
MODULE_LICENSE("GPL");
