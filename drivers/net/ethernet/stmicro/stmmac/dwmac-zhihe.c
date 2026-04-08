// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/bitfield.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_net.h>
#include <linux/regmap.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>

#include "stmmac_platform.h"

/* SYSCFG registers */
#define GMAC_CLKCTRL (0x00)
#define GMAC_CTRL    (0x04)
#define GMAC_ST      (0x08)
#define GMAC_TMI     (0x0C)
#define GMAC_PMI     (0x10)
#define GMAC_SBDI    (0x14)

/* SYSCFG value */
#define GMAC_CLKTRL_EN_ALL        0x1F
#define GMAC_CLKCTRL_SPEED_MASK  (0x3 << 8)
#define GMAC_CLKCTRL_1G          (0x0 << 8)
#define GMAC_CLKCTRL_10M         (0x2 << 8)
#define GMAC_CLKCTRL_100M        (0x3 << 8)

#define GMAC_CTRL_INTF_MASK 0xF
#define GMAC_CTRL_RGMII     0x1
#define GMAC_CTRL_RMII      0x4

struct zhihe_dwmac {
	struct device *dev;
	struct regmap *sys_regmap;
	struct clk *gmac_aclk;
	struct clk *gmac_pclk;
	struct clk *gmac_x2h_aclk;
	struct clk *gmac_x2h_hclk;
	struct plat_stmmacenet_data *plat_dat;
};

/* zhihe a210 */
static void zhihe_dwmac_set_speed(struct zhihe_dwmac *dwmac, unsigned int speed)
{
	phy_interface_t interface = dwmac->plat_dat->phy_interface;
	struct device *dev = dwmac->dev;
	unsigned int reg = 0;

	/* Configure mac speed */
	regmap_read(dwmac->sys_regmap, GMAC_CLKCTRL, &reg);
	switch (speed) {
	case SPEED_10:
		reg &= ~GMAC_CLKCTRL_SPEED_MASK;
		reg |= GMAC_CLKCTRL_10M;
		break;
	case SPEED_100:
		reg &= ~GMAC_CLKCTRL_SPEED_MASK;
		reg |= GMAC_CLKCTRL_100M;
		break;
	case SPEED_1000:
		reg &= ~GMAC_CLKCTRL_SPEED_MASK;
		reg |= GMAC_CLKCTRL_1G;
		break;
	default:
		dev_err(dev, "Invalid speed %u\n", speed);
		return;
	}
	regmap_write(dwmac->sys_regmap, GMAC_CLKCTRL, reg);

	/* Configure phy interface */
	regmap_read(dwmac->sys_regmap, GMAC_CTRL, &reg);
	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
		reg &= ~GMAC_CTRL_INTF_MASK;
		reg |= GMAC_CTRL_RGMII;
		break;
	case PHY_INTERFACE_MODE_RMII:
		reg &= ~GMAC_CTRL_INTF_MASK;
		reg |= GMAC_CTRL_RMII;
		break;
	default:
		dev_err(dev, "Invalid phy interface: %d\n", interface);
		return;
	}
	regmap_write(dwmac->sys_regmap, GMAC_CTRL, reg);
}

static void zhihe_dwmac_clkctrl_enable(struct zhihe_dwmac *dwmac, bool enable)
{
	unsigned int reg = 0;

	regmap_read(dwmac->sys_regmap, GMAC_CLKCTRL, &reg);
	reg &= ~GMAC_CLKTRL_EN_ALL;
	if (enable)
		reg |= GMAC_CLKTRL_EN_ALL;
	regmap_write(dwmac->sys_regmap, GMAC_CLKCTRL, reg);
}

static int zhihe_dwmac_peri_resource(struct zhihe_dwmac *dwmac)
{
	struct device *dev = dwmac->dev;
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = pdev->dev.of_node;

	/* clks resource */
	dwmac->gmac_aclk = devm_clk_get(dev, "stmmaceth");
	if (IS_ERR(dwmac->gmac_aclk)) {
		return dev_err_probe(dev, PTR_ERR(dwmac->gmac_aclk),
					"Error getting stmmaceth reference clock\n");
	}

	dwmac->gmac_pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(dwmac->gmac_pclk)) {
		return dev_err_probe(dev, PTR_ERR(dwmac->gmac_pclk),
					"Error getting pclk reference clock\n");
	}

	dwmac->gmac_x2h_aclk = devm_clk_get(dev, "x2h-aclk");
	if (IS_ERR(dwmac->gmac_x2h_aclk)) {
		return dev_err_probe(dev, PTR_ERR(dwmac->gmac_x2h_aclk),
					"Error getting x2h-aclk reference clock\n");
	}

	dwmac->gmac_x2h_hclk = devm_clk_get(dev, "x2h-hclk");
	if (IS_ERR(dwmac->gmac_x2h_hclk)) {
		return dev_err_probe(dev, PTR_ERR(dwmac->gmac_x2h_hclk),
					"Error getting x2h-hclk reference clock\n");
	}

	dwmac->sys_regmap = syscon_regmap_lookup_by_phandle(np, "zhihe,gmacsys");
	if (IS_ERR(dwmac->sys_regmap)) {
		return dev_err_probe(dev, PTR_ERR(dwmac->sys_regmap),
					"Error getting zhihe,gmacsys remap\n");
	}
	return 0;
}

static int zhihe_dwmac_peri_clk_enable(struct zhihe_dwmac *dwmac)
{
	int ret;
	struct device *dev = dwmac->dev;

	ret = clk_prepare_enable(dwmac->gmac_aclk);
	if (ret) {
		dev_err(dev, "Failed to enable aclk clock\n");
		return ret;
	}

	ret = clk_prepare_enable(dwmac->gmac_pclk);
	if (ret) {
		clk_disable_unprepare(dwmac->gmac_aclk);
		dev_err(dev, "Failed to enable pclk clock\n");
		return ret;
	}

	ret = clk_prepare_enable(dwmac->gmac_x2h_aclk);
	if (ret) {
		clk_disable_unprepare(dwmac->gmac_pclk);
		clk_disable_unprepare(dwmac->gmac_aclk);
		dev_err(dev, "Failed to enable x2h-aclk clock\n");
		return ret;
	}

	ret = clk_prepare_enable(dwmac->gmac_x2h_hclk);
	if (ret) {
		clk_disable_unprepare(dwmac->gmac_x2h_aclk);
		clk_disable_unprepare(dwmac->gmac_pclk);
		clk_disable_unprepare(dwmac->gmac_aclk);
		dev_err(dev, "Failed to enable x2h-hclk clock\n");
		return ret;
	}
	return ret;
}

static void zhihe_dwmac_peri_clk_disable(struct zhihe_dwmac *dwmac)
{
	clk_disable_unprepare(dwmac->gmac_aclk);
	clk_disable_unprepare(dwmac->gmac_pclk);
	clk_disable_unprepare(dwmac->gmac_x2h_aclk);
	clk_disable_unprepare(dwmac->gmac_x2h_hclk);
}

/* STM platform Callback */
static void plat_fix_mac_speed(void *priv, unsigned int speed,
			       unsigned int mode)
{
	struct zhihe_dwmac *dwmac = priv;

	zhihe_dwmac_set_speed(dwmac, speed);
}

static int plat_clks_config(void *priv, bool enabled)
{
	struct zhihe_dwmac *dwmac = priv;
	int ret;

	if (enabled) {
		ret = zhihe_dwmac_peri_clk_enable(dwmac);
		if (ret)
			return ret;

		ret = reset_control_deassert(dwmac->plat_dat->stmmac_rst);
		if (ret)
			goto disable_clks;

		ret = reset_control_deassert(dwmac->plat_dat->stmmac_ahb_rst);
		if (ret)
			goto assert_stmmac_rst;

		zhihe_dwmac_clkctrl_enable(dwmac, true);
	} else {
		zhihe_dwmac_clkctrl_enable(dwmac, false);
		reset_control_assert(dwmac->plat_dat->stmmac_ahb_rst);
		reset_control_assert(dwmac->plat_dat->stmmac_rst);
		zhihe_dwmac_peri_clk_disable(dwmac);
	}

	return 0;

assert_stmmac_rst:
	reset_control_assert(dwmac->plat_dat->stmmac_rst);
disable_clks:
	zhihe_dwmac_peri_clk_disable(dwmac);

	return ret;
}

/* zhihe probe */
static int zhihe_dwmac_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct zhihe_dwmac *dwmac;
	struct device *dev = &pdev->dev;
	int ret;

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (ret)
		return dev_err_probe(dev, ret, "Failed to set DMA mask\n");

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret)
		return dev_err_probe(dev, ret,
					"Failed to get platform resources\n");

	plat_dat = devm_stmmac_probe_config_dt(pdev, stmmac_res.mac);
	if (IS_ERR(plat_dat)) {
		return dev_err_probe(dev, PTR_ERR(plat_dat),
					"dt configuration failed\n");
	}

	dwmac = devm_kzalloc(dev, sizeof(*dwmac), GFP_KERNEL);
	if (!dwmac)
		return -ENOMEM;

	/* Init BSP priv data */
	dwmac->dev = &pdev->dev;
	dwmac->plat_dat = plat_dat;

	/* Init Plat data */
	plat_dat->bsp_priv = dwmac;
	plat_dat->fix_mac_speed = plat_fix_mac_speed;
	plat_dat->clks_config = plat_clks_config;

	/* BSP clks & resets init */
	ret = zhihe_dwmac_peri_resource(dwmac);
	if (ret)
		goto err_remove_config_dt;
	ret = zhihe_dwmac_peri_clk_enable(dwmac);
	if (ret)
		goto err_remove_config_dt;

	/* Plat probe
	 *   "stmmaceth", "ahb" resets, hw init, caps config, pm init
	 */
	ret = stmmac_dvr_probe(&pdev->dev, plat_dat, &stmmac_res);
	if (ret)
		goto err_clk_disable;

	return 0;

err_clk_disable:
	zhihe_dwmac_peri_clk_disable(dwmac);

err_remove_config_dt:
	stmmac_remove_config_dt(pdev, plat_dat);

	return ret;
}

static const struct of_device_id zhihe_dwmac_match[] = {
	{ .compatible = "zhihe,a210-dwmac" },
	{}
};
MODULE_DEVICE_TABLE(of, zhihe_dwmac_match);

static struct platform_driver zhihe_dwmac_driver = {
	.probe  = zhihe_dwmac_probe,
	.remove_new = stmmac_pltfr_remove,
	.driver = {
		.name	= "zhihe-dwmac",
		.pm		= &stmmac_pltfr_pm_ops,
		.of_match_table = of_match_ptr(zhihe_dwmac_match),
	},
};
module_platform_driver(zhihe_dwmac_driver);

MODULE_AUTHOR("ZHIHE");
MODULE_DESCRIPTION("ZHIHE dwmac platform driver");
MODULE_LICENSE("GPL");
