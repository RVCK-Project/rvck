// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/extcon-provider.h>
#include <linux/gpio/consumer.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/usb/role.h>
#include <linux/usb/typec_dp.h>
#include <linux/usb/typec_mux.h>

/* PHY Lane Mux Selection */
#define PHY_LANE_MUX_USB	0
#define PHY_LANE_MUX_DP		1

/* Base DWC3 Control Register */
#define DWC3_LCSR_TX_DEEMPH_2	0xd068

/*
 * DPTX_SYS Register Offsets - From soc sys databook
 */
#define DPTX_CTRL	0x00
#define DPTX_LSFR_CTRL	0x10
#define DPTX_LSFR_SEED	0x14
#define DPTX_MTN_LINK	0x20
#define DPTX_AUX_CTRL	0x40
#define DPTX_DBG0	0x100
#define DPTX_DBG1	0x104

/* DPTX_AUX_CTRL bits*/
#define AUX_DP_DN_SWAP	BIT(8)

/*
 * TCA Register Offsets - From Databook Section 3.2 (Page 36)
 * These are VERIFIED from the databook
 */
#define TCA_CLK_RST			0x00
#define TCA_CLK_RST_SW			BIT(9)
#define TCA_CLK_RST_REF_CLK_EN		BIT(1)
#define TCA_CLK_RST_SUSPEND_CLK_EN	BIT(0)

#define TCA_INTR_EN			0x04
#define TCA_INTR_EN_MASK	GENMASK(1, 0)

#define TCA_INTR_STS			0x08
#define TCA_INTR_STS_MASK		GENMASK(15, 0)

#define TCA_GCFG			0x10
#define TCA_GCFG_ROLE_HSTDEV		BIT(4)
#define TCA_GCFG_OP_MODE		GENMASK(1, 0)
#define TCA_GCFG_OP_MODE_SYSMODE	0
#define TCA_GCFG_OP_MODE_SYNCMODE	1

#define TCA_TCPC			0x14
#define TCA_TCPC_VALID			BIT(4)
#define TCA_TCPC_LOW_POWER_EN		BIT(3)
#define TCA_TCPC_ORIENTATION_NORMAL	BIT(2)
#define TCA_TCPC_MUX_CONTRL		GENMASK(1, 0)
#define TCA_TCPC_MUX_CONTRL_NO_CONN	0
#define TCA_TCPC_MUX_CONTRL_USB_CONN	1
#define TCA_TCPC_MUX_CONTRL_DP_CONN	2
#define TCA_TCPC_MUX_CONTRL_USBDP_CONN	3

#define TCA_SYSMODE_CFG			0x18
#define TCA_SYSMODE_TCPC_DISABLE	BIT(3)
#define TCA_SYSMODE_TCPC_FLIP		BIT(2)
#define TCA_SYSMODE_TCPC_CONN_MODE	GENMASK(1, 0)
#define TCA_SYSMODE_TCPC_CONN_CE	0x02
#define TCA_SYSMODE_TCPC_CONN_DF	0x03

#define TCA_CTRLSYNCMODE_CFG0		0x20
#define TCA_CTRLSYNCMODE_CFG1		0x20

#define TCA_PSTATE			0x30
#define TCA_PSTATE_CM_STS		BIT(4)
#define TCA_PSTATE_TX_STS		BIT(3)
#define TCA_PSTATE_RX_PLL_STS		BIT(2)
#define TCA_PSTATE_PIPE0_POWER_DOWN	GENMASK(1, 0)

#define TCA_GEN_STATUS			0x34
#define TCA_GEN_DEV_POR			BIT(12)
#define TCA_GEN_REF_CLK_SEL		BIT(8)
#define TCA_GEN_TYPEC_FLIP_INVERT	BIT(4)
#define TCA_GEN_PHY_TYPEC_DISABLE	BIT(3)
#define TCA_GEN_PHY_TYPEC_FLIP		BIT(2)
#define TCA_GEN_PHY_TYPEC_CONN		GENMASK(1, 0)

#define TCA_VBUS_CTRL			0x40
#define TCA_VBUS_STATUS			0x44

#define TCA_INFO			0xfc

/* PHY Operation Modes - Custom flags for internal use */
#define PHY_MODE_DISABLED	0
#define PHY_MODE_USB3		BIT(0)
#define PHY_MODE_DP_CUSTOM	BIT(1)
#define PHY_MODE_USB3_DP	(BIT(0) | BIT(1))

enum {
	UDPHY_MODE_NONE		= 0,
	UDPHY_MODE_USB		= BIT(0),
	UDPHY_MODE_DP		= BIT(1),
	UDPHY_MODE_DP_USB	= BIT(1) | BIT(0),
};

static char *udphy_mode_str[] = {
	"UDPHY_MODE_NONE",
	"UDPHY_MODE_USB",
	"UDPHY_MODE_DP",
	"UDPHY_MODE_DP_USB",
};

struct zhihe_c10phy_priv {
	struct device		*dev;
	struct phy		*phy;
	struct phy		*dp_phy;
	void __iomem		*phy_ctrl_base;
	void __iomem		*tca_base;
	void __iomem		*sysreg_base;
	void __iomem		*dptxsys_base;
	struct reset_control	*c10phy_rst;
	bool			initialized;
	enum typec_orientation orientation;	/* Type-C orientation */
	u32 lane_mux_sel[4];
	u32 dp_lane_sel[4];
	/* GPIO for AUX control */
	struct gpio_desc *aux_p_gpio;	/* AUX_P pull-down control */
	struct gpio_desc *aux_n_gpio;	/* AUX_N pull-up control */
	enum usb_role usb_role;		/* Current USB role: host or device */
	struct typec_mux_dev *mux;
	u8 mode;
	bool mode_change;
	struct typec_switch_dev *sw;
	struct mutex lock; /* Protects PHY mode and Type-C state. */
	struct extcon_dev *extcon;	/* Extcon device for HPD notification */
	struct clk_bulk_data *clks;
	int num_clocks;
	struct reset_control *apb_rst;
};

/* Supported extcon cables - must be static to persist after probe returns */
static const unsigned int c10phy_extcon_cables[] = {
	EXTCON_DISP_DP,
	EXTCON_NONE,
};

static void tca_blk_orientation_set(struct zhihe_c10phy_priv *priv,
				    enum typec_orientation orientation);

static int udphy_set_mux(struct zhihe_c10phy_priv *priv);

static int tca_blk_typec_switch_set(struct typec_switch_dev *sw,
				    enum typec_orientation orientation)
{
	struct zhihe_c10phy_priv *priv = typec_switch_get_drvdata(sw);

	tca_blk_orientation_set(priv, orientation);

	return 0;
}

static void configure_aux_gpio(struct zhihe_c10phy_priv *priv, enum typec_orientation orientation)
{
	if (orientation == TYPEC_ORIENTATION_REVERSE) {
		/* Flipped orientation: swap the polarity */
		if (priv->aux_p_gpio) {
			gpiod_set_value_cansleep(priv->aux_p_gpio, 1);
			dev_dbg(priv->dev, "AUX_P GPIO set to high (pull-up) - flipped\n");
		}

		if (priv->aux_n_gpio) {
			gpiod_set_value_cansleep(priv->aux_n_gpio, 0);
			dev_dbg(priv->dev, "AUX_N GPIO set to low (pull-down) - flipped\n");
		}
	} else {
		/* Normal orientation */
		if (priv->aux_p_gpio) {
			gpiod_set_value_cansleep(priv->aux_p_gpio, 0);
			dev_dbg(priv->dev, "AUX_P GPIO set to low (pull-down) - normal\n");
		}

		if (priv->aux_n_gpio) {
			gpiod_set_value_cansleep(priv->aux_n_gpio, 1);
			dev_dbg(priv->dev, "AUX_N GPIO set to high (pull-up) - normal\n");
		}
	}
}

static void tca_blk_orientation_set(struct zhihe_c10phy_priv *priv,
				    enum typec_orientation orientation)
{
	u32 val;

	if (priv->orientation == orientation)
		return;

	mutex_lock(&priv->lock);

	if (orientation == TYPEC_ORIENTATION_NONE) {
		/*
		 * use Controller Synced Mode for TCA low power enable and
		 * put PHY to USB safe state.
		 */
		val = FIELD_PREP(TCA_GCFG_OP_MODE, TCA_GCFG_OP_MODE_SYNCMODE);
		writel(val, priv->tca_base + TCA_GCFG);

		val = TCA_TCPC_VALID | TCA_TCPC_LOW_POWER_EN;
		writel(val, priv->tca_base + TCA_TCPC);
		goto out;
	}

	/* use System Configuration Mode for TCA mux control. */
	val = FIELD_PREP(TCA_GCFG_OP_MODE, TCA_GCFG_OP_MODE_SYSMODE);
	writel(val, priv->tca_base + TCA_GCFG);

	/* Disable TCA module */
	val = readl(priv->tca_base + TCA_SYSMODE_CFG);
	val |= TCA_SYSMODE_TCPC_DISABLE;
	writel(val, priv->tca_base + TCA_SYSMODE_CFG);

	if (orientation == TYPEC_ORIENTATION_REVERSE)
		val |= TCA_SYSMODE_TCPC_FLIP;
	else if (orientation == TYPEC_ORIENTATION_NORMAL)
		val &= ~TCA_SYSMODE_TCPC_FLIP;

	writel(val, priv->tca_base + TCA_SYSMODE_CFG);

	/* Enable TCA module */
	val &= ~TCA_SYSMODE_TCPC_DISABLE;
	writel(val, priv->tca_base + TCA_SYSMODE_CFG);

out:
	priv->orientation = orientation;
	mutex_unlock(&priv->lock);
}

static void tca_blk_init(struct zhihe_c10phy_priv *priv)
{
	u32 val;
	enum typec_orientation orientation;

	/* reset XBar block */
	val = readl(priv->tca_base + TCA_CLK_RST);
	val &= ~TCA_CLK_RST_SW;
	writel(val, priv->tca_base + TCA_CLK_RST);

	udelay(100);

	/* clear reset */
	val |= TCA_CLK_RST_SW;
	writel(val, priv->tca_base + TCA_CLK_RST);

	/*
	 * After XBar reset, TCA registers are lost. Invalidate the cached
	 * orientation so tca_blk_orientation_set() will re-program the
	 * hardware unconditionally.
	 */
	orientation = priv->orientation;
	priv->orientation = TYPEC_ORIENTATION_NONE;
	tca_blk_orientation_set(priv, orientation);
}

/* Setup Type-C orientation switch */
static int zhihe_setup_typec_switch(struct zhihe_c10phy_priv *priv)
{
	struct typec_switch_desc sw_desc = {};

	sw_desc.drvdata = priv;
	sw_desc.fwnode = dev_fwnode(priv->dev);
	sw_desc.set = tca_blk_typec_switch_set;
	sw_desc.name = NULL;

	priv->sw = typec_switch_register(priv->dev, &sw_desc);
	if (IS_ERR(priv->sw)) {
		dev_err(priv->dev, "Failed to register typec switch: %ld\n", PTR_ERR(priv->sw));
		return PTR_ERR(priv->sw);
	}

	return 0;
}

static void zhihe_switch_unregister(void *data)
{
	struct zhihe_c10phy_priv *priv = data;

	typec_switch_unregister(priv->sw);
}

static int c10phy_init(struct phy *phy)
{
	int ret;
	struct zhihe_c10phy_priv *priv = phy_get_drvdata(phy);

	ret = clk_bulk_prepare_enable(priv->num_clocks, priv->clks);
	if (ret)
		return ret;

	reset_control_assert(priv->c10phy_rst);
	usleep_range(10, 20);
	reset_control_deassert(priv->c10phy_rst);
	/* Databook 5.2: wait up to 18ms for PHY calibration after warm-reset */
	msleep(20);

	/*
	 * XBar reset only on first init (power-up). On resume the phy_reset
	 * already resets the PHY, and XBar reset would destroy the TCA
	 * internal CONN state causing SuperSpeed link training failure.
	 */
	if (!priv->initialized) {
		tca_blk_init(priv);
		priv->initialized = true;
	}

	return udphy_set_mux(priv);
}

/*
 * Display insertion/removal updates the extcon state (long HPD). Link
 * status changes, EDID updates and resolution switches keep HPD high
 * and notify the DP host through extcon_sync() (short HPD).
 */
static int usbdp_typec_mux_set(struct typec_mux_dev *mux,
			       struct typec_mux_state *state)
{
	int ret;
	struct zhihe_c10phy_priv *priv = typec_mux_get_drvdata(mux);
	u8 mode;
	struct device *dev = priv->dev;

	mutex_lock(&priv->lock);

	switch (state->mode) {
	case TYPEC_DP_STATE_C:
		fallthrough;
	case TYPEC_DP_STATE_E:
		mode = UDPHY_MODE_DP;
		break;
	case TYPEC_DP_STATE_D:
		fallthrough;
	default:
		mode = UDPHY_MODE_DP_USB;
		break;
	}

	dev_info(dev, "change mux_mode %s to %s\n",
		 udphy_mode_str[priv->mode], udphy_mode_str[mode]);

	if (mode != priv->mode) {
		priv->mode = mode;
		ret = udphy_set_mux(priv);
	}

	/* Handling DP Alt Mode HPD events */
	if (state->alt && state->alt->svid == USB_TYPEC_DP_SID) {
		struct typec_displayport_data *data = state->data;
		bool last_hpd_state, current_hpd_state;

		if (!data) {
			/* No data means DP disconnected */
			dev_info(dev, "DP disconnected (no data)\n");
			last_hpd_state = extcon_get_state(priv->extcon, EXTCON_DISP_DP);
			if (!last_hpd_state)
				extcon_sync(priv->extcon, EXTCON_DISP_DP);
			else
				extcon_set_state_sync(priv->extcon, EXTCON_DISP_DP, false);
			goto out;
		}

		dev_info(dev, "DP Status: 0x%x (HPD_STATE=%d, IRQ_HPD=%d)\n",
			 data->status,
			 !!(data->status & DP_STATUS_HPD_STATE),
			 !!(data->status & DP_STATUS_IRQ_HPD));

		/* Get the last HPD status */
		last_hpd_state = extcon_get_state(priv->extcon, EXTCON_DISP_DP);

		/* Get the current HPD status*/
		current_hpd_state = !!(data->status & DP_STATUS_HPD_STATE);

		/* Prioritize processing IRQ_HPD (short pulse) */
		if (data->status & DP_STATUS_IRQ_HPD) {
			if (current_hpd_state == last_hpd_state && current_hpd_state) {
				/*
				 * A normal short HPD signal: HPD remains high + IRQ pulse.
				 * This is used to notify of link status changes, EDID updates, etc.
				 */
				dev_info(dev, "DP IRQ_HPD: Short pulse (Link status/EDID change)\n");
				extcon_sync(priv->extcon, EXTCON_DISP_DP);
			} else {
				/*
				 * Abnormal condition: HPD low level + IRQ pulse
				 * It could be a partner firmware bug or a transitional
				 * state during unplugging.
				 */
				dev_warn(dev, "Abnormal: IRQ_HPD with current_hpd_state %d, last_hpd_state %d\n",
					 current_hpd_state, last_hpd_state);
				/*
				 * We cannot distinguish a genuine disconnection from
				 * jitter because Type-C does not let us reread the
				 * hardware state;
				 * we can only wait for the Partner to send the next Attention VDM.
				 */
			}
			goto out;
		}

		/* Handling HPD status changes (long HPD: insertion/removal) */
		if (current_hpd_state != last_hpd_state) {
			dev_info(dev, "DP HPD state change: %s -> %s\n",
				 last_hpd_state ? "connected" : "disconnected",
				 current_hpd_state ? "connected" : "disconnected");

			/* Notify DP Host of HPD status change*/
			extcon_set_state_sync(priv->extcon, EXTCON_DISP_DP, current_hpd_state);
		} else {
			/*
			 * The state has not changed and there is no IRQ_HPD.
			 * This could be a duplicate Attention VDM, ignore it.
			 */
			dev_dbg(dev, "DP HPD state unchanged (%s), ignoring\n",
				current_hpd_state ? "connected" : "disconnected");
		}
	}

out:
	mutex_unlock(&priv->lock);
	return ret;
}

static int udphy_setup_typec_mux(struct zhihe_c10phy_priv *priv)
{
	struct typec_mux_desc mux_desc = {};

	mux_desc.drvdata = priv;
	mux_desc.fwnode = dev_fwnode(priv->dev);
	mux_desc.set = usbdp_typec_mux_set;

	priv->mux = typec_mux_register(priv->dev, &mux_desc);
	if (IS_ERR(priv->mux)) {
		dev_err(priv->dev, "Error register typec mux: %ld\n",
			PTR_ERR(priv->mux));
		return PTR_ERR(priv->mux);
	}

	return 0;
}

static void udphy_typec_mux_unregister(void *data)
{
	struct zhihe_c10phy_priv *priv = data;

	typec_mux_unregister(priv->mux);
}

static int zhihe_dp_phy_verify_link_rate(unsigned int link_rate)
{
	switch (link_rate) {
	case 1620:
	case 2700:
	case 5400:
	case 8100:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static u32 udphy_dp_get_max_link_rate(struct zhihe_c10phy_priv *priv, struct device_node *np)
{
	u32 max_link_rate;
	int ret;

	ret = of_property_read_u32(np, "max-link-rate", &max_link_rate);
	if (ret)
		return 8100;

	ret = zhihe_dp_phy_verify_link_rate(max_link_rate);
	if (ret) {
		dev_warn(priv->dev, "invalid max-link-rate value:%d\n", max_link_rate);
		max_link_rate = 8100;
	}

	return max_link_rate;
}

static int zhihe_dp_phy_verify_config(struct zhihe_c10phy_priv *priv,
				      struct phy_configure_opts_dp *dp)
{
	int ret = 0;

	/* If changing link rate was required, verify it's supported. */
	ret = zhihe_dp_phy_verify_link_rate(dp->link_rate);
	if (ret)
		return ret;

	/* Verify lane count. */
	switch (dp->lanes) {
	case 1:
	case 2:
	case 4:
		/* valid lane count. */
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int zhihe_dp_phy_configure(struct phy *phy,
				  union phy_configure_opts *opts)
{
	struct zhihe_c10phy_priv *priv = phy_get_drvdata(phy);
	int ret;

	ret = zhihe_dp_phy_verify_config(priv, &opts->dp);
	if (ret)
		return ret;

	return 0;
}

static int udphy_dplane_get(struct zhihe_c10phy_priv *priv)
{
	int dp_lanes;

	switch (priv->mode) {
	case UDPHY_MODE_DP:
		dp_lanes = 4;
		break;
	case UDPHY_MODE_DP_USB:
		dp_lanes = 2;
		break;
	case UDPHY_MODE_USB:
		fallthrough;
	default:
		dp_lanes = 0;
		break;
	}

	return dp_lanes;
}

static int udphy_sys_cfg(struct zhihe_c10phy_priv *priv)
{
	u32 val;

	/* use System Configuration Mode for TCA mux control. */
	val = FIELD_PREP(TCA_GCFG_OP_MODE, TCA_GCFG_OP_MODE_SYSMODE);
	writel(val, priv->tca_base + TCA_GCFG);

	/*
	 * Build SYSMODE_CFG directly from software state instead of reading
	 * GEN_STATUS. After PHY reset, GEN_STATUS FLIP bit is cleared and
	 * does not reflect the desired orientation. Reading it and writing
	 * it back to SYSMODE_CFG causes a transient FLIP=0 state that
	 * corrupts the TCA crossbar configuration.
	 */

	/* set typec disable first */
	val = TCA_SYSMODE_TCPC_DISABLE;
	if (priv->orientation == TYPEC_ORIENTATION_REVERSE)
		val |= TCA_SYSMODE_TCPC_FLIP;

	switch (priv->mode) {
	case UDPHY_MODE_DP:
		val |= TCA_SYSMODE_TCPC_CONN_CE;
		break;
	case UDPHY_MODE_DP_USB:
	case UDPHY_MODE_USB:
	default:
		val |= TCA_SYSMODE_TCPC_CONN_DF;
		break;
	}
	writel(val, priv->tca_base + TCA_SYSMODE_CFG);
	usleep_range(10, 20);

	/* de-assert typec_disable to apply */
	val &= ~TCA_SYSMODE_TCPC_DISABLE;
	writel(val, priv->tca_base + TCA_SYSMODE_CFG);

	return 0;
}

static int tca_wait_ack(struct zhihe_c10phy_priv *priv)
{
	u32 val;
	int timeout = 1000; /* 1ms max */

	while (timeout--) {
		val = readl(priv->tca_base + TCA_INTR_STS);
		if (val & TCA_INTR_STS_MASK) {
			/* Clear interrupt status */
			writel(val, priv->tca_base + TCA_INTR_STS);
			return 0;
		}
		udelay(1);
	}
	dev_warn(priv->dev, "TCA ack timeout, INTR_STS=0x%x\n", val);
	return -ETIMEDOUT;
}

static int udphy_sync_cfg(struct zhihe_c10phy_priv *priv)
{
	u32 val;

	/* Use sync configuration mode for TCA mux control. */
	val = FIELD_PREP(TCA_GCFG_OP_MODE, TCA_GCFG_OP_MODE_SYNCMODE);
	writel(val, priv->tca_base + TCA_GCFG);

	/*enable tca ack and timeout irq*/

	writel(TCA_INTR_STS_MASK, priv->tca_base + TCA_INTR_STS);
	writel(TCA_INTR_EN_MASK, priv->tca_base + TCA_INTR_EN);

	/*mode config */
	val = readl(priv->tca_base + TCA_TCPC);
	/*default set to usb mode*/
	val &= ~TCA_TCPC_MUX_CONTRL;
	val |= TCA_TCPC_MUX_CONTRL_USB_CONN;
	val |= TCA_TCPC_VALID;
	val &= ~TCA_TCPC_LOW_POWER_EN;
	if (priv->orientation == TYPEC_ORIENTATION_REVERSE)
		val |= TCA_TCPC_ORIENTATION_NORMAL;
	else
		val &= ~TCA_TCPC_ORIENTATION_NORMAL;
	writel(val, priv->tca_base + TCA_TCPC);
	tca_wait_ack(priv);

	val = readl(priv->tca_base + TCA_TCPC);
	val &= ~TCA_TCPC_LOW_POWER_EN;
	val &= ~TCA_TCPC_MUX_CONTRL;
	switch (priv->mode) {
	case UDPHY_MODE_USB:
		return 0;
	case UDPHY_MODE_DP:
		val |= TCA_TCPC_MUX_CONTRL_DP_CONN;
		break;
	case UDPHY_MODE_DP_USB:
	default:
		val |= TCA_TCPC_MUX_CONTRL_USBDP_CONN;
		break;
	}

	val |= TCA_TCPC_VALID;
	writel(val, priv->tca_base + TCA_TCPC);
	tca_wait_ack(priv);

	return 0;
}

static int udphy_set_mux(struct zhihe_c10phy_priv *priv)
{
	udphy_sys_cfg(priv);
	usleep_range(10, 20);
	udphy_sync_cfg(priv);

	return 0;
}

static int zhihe_dp_phy_power_on(struct phy *phy)
{
	struct zhihe_c10phy_priv *priv = phy_get_drvdata(phy);
	int ret = 0, dp_lanes, dptx_aux_ctrl;

	mutex_lock(&priv->lock);
	dptx_aux_ctrl = readl(priv->dptxsys_base + DPTX_AUX_CTRL);
	dp_lanes = udphy_dplane_get(priv);
	phy_set_bus_width(phy, dp_lanes);

	if (priv->orientation == TYPEC_ORIENTATION_REVERSE)
		dptx_aux_ctrl |= AUX_DP_DN_SWAP;
	else if (priv->orientation == TYPEC_ORIENTATION_NORMAL)
		dptx_aux_ctrl &= ~AUX_DP_DN_SWAP;

	writel(dptx_aux_ctrl, priv->dptxsys_base + DPTX_AUX_CTRL);
	configure_aux_gpio(priv, priv->orientation);
	mutex_unlock(&priv->lock);

	/*
	 * If data send by aux channel too fast after phy power on,
	 * the aux may be not ready which will cause aux error. Adding
	 * delay to avoid this issue.
	 */
	usleep_range(10000, 11000);
	return ret;
}

static int zhihe_dp_phy_power_off(struct phy *phy)
{
	return 0;
}

static const struct phy_ops zhihe_dp_phy_ops = {
	.power_on	= zhihe_dp_phy_power_on,
	.power_off	= zhihe_dp_phy_power_off,
	.configure	= zhihe_dp_phy_configure,
	.owner		= THIS_MODULE,
};

static int c10phy_exit(struct phy *phy)
{
	struct zhihe_c10phy_priv *priv = phy_get_drvdata(phy);

	clk_bulk_disable_unprepare(priv->num_clocks, priv->clks);
	reset_control_assert(priv->c10phy_rst);

	/*
	 * TCA registers are inaccessible after clocks off. Invalidate
	 * cached orientation so tca_blk_orientation_set() from fusb302
	 * won't skip updating priv->orientation while clocks are off.
	 */
	priv->orientation = TYPEC_ORIENTATION_NONE;

	return 0;
}

static const struct phy_ops c10phy_ops = {
	.init = c10phy_init,
	.exit = c10phy_exit,
	.owner = THIS_MODULE,
};

/* Sysfs attribute for orientation control */
static ssize_t orientation_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct zhihe_c10phy_priv *priv = dev_get_drvdata(dev);
	const char *orientation_str;

	switch (priv->orientation) {
	case TYPEC_ORIENTATION_NONE:
		orientation_str = "none";
		break;
	case TYPEC_ORIENTATION_NORMAL:
		orientation_str = "normal";
		break;
	case TYPEC_ORIENTATION_REVERSE:
		orientation_str = "reverse";
		break;
	default:
		orientation_str = "unknown";
		break;
	}

	return sprintf(buf, "%s\n", orientation_str);
}

static ssize_t orientation_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct zhihe_c10phy_priv *priv = dev_get_drvdata(dev);
	enum typec_orientation orientation;

	if (sysfs_streq(buf, "none"))
		orientation = TYPEC_ORIENTATION_NONE;
	else if (sysfs_streq(buf, "normal"))
		orientation = TYPEC_ORIENTATION_NORMAL;
	else if (sysfs_streq(buf, "reverse"))
		orientation = TYPEC_ORIENTATION_REVERSE;
	else
		return -EINVAL;

	tca_blk_orientation_set(priv, orientation);

	return count;
}

static DEVICE_ATTR_RW(orientation);

static struct attribute *c10phy_attrs[] = {
	&dev_attr_orientation.attr,
	NULL,
};

static const struct attribute_group c10phy_attr_group = {
	.attrs = c10phy_attrs,
};

static int udphy_parse_lane_mux_data(struct zhihe_c10phy_priv *priv, struct device *dev)
{
	struct device_node *np = dev->of_node;
	struct property *prop;
	int ret, i, len, num_lanes;

	prop = of_find_property(np, "zhihe,dp-lane-mux", &len);
	if (!prop) {
		dev_dbg(dev, "failed to find dp lane mux, following dp alt mode\n");
		priv->mode = UDPHY_MODE_USB;
		return 0;
	}

	num_lanes = len / sizeof(u32);

	if (num_lanes != 2 && num_lanes != 4) {
		dev_err(dev, "invalid number of lane mux\n");
		return -EINVAL;
	}

	ret = of_property_read_u32_array(np, "zhihe,dp-lane-mux", priv->dp_lane_sel, num_lanes);
	if (ret) {
		dev_err(dev, "get dp lane mux failed\n");
		return -EINVAL;
	}

	for (i = 0; i < num_lanes; i++) {
		int j;

		if (priv->dp_lane_sel[i] > 3) {
			dev_err(dev, "lane mux between 0 and 3, exceeding the range\n");
			return -EINVAL;
		}

		priv->lane_mux_sel[priv->dp_lane_sel[i]] = PHY_LANE_MUX_DP;

		for (j = i + 1; j < num_lanes; j++) {
			if (priv->dp_lane_sel[i] == priv->dp_lane_sel[j]) {
				dev_err(dev, "set repeat lane mux value\n");
				return -EINVAL;
			}
		}
	}

	priv->mode = UDPHY_MODE_DP;
	if (num_lanes == 2) {
		priv->mode |= UDPHY_MODE_USB;
		if (priv->lane_mux_sel[0] == PHY_LANE_MUX_DP)
			priv->orientation = TYPEC_ORIENTATION_REVERSE;
	}

	return 0;
}

static int c10phy_parse_dt(struct zhihe_c10phy_priv *priv, struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;

	ret = clk_bulk_get_all(dev, &priv->clks);
	if (ret < 0)
		return dev_err_probe(dev, ret,
				     "failed to get c10phy bulk clks\n");
	priv->num_clocks = ret;

	/* Get TCA register base */
	priv->tca_base = devm_platform_ioremap_resource_byname(pdev, "tca");
	if (IS_ERR(priv->tca_base))
		return dev_err_probe(dev, PTR_ERR(priv->tca_base),
				     "Couldn't get TCA register base\n");

	/* Get PHY_CTRL register base */
	priv->phy_ctrl_base = devm_platform_ioremap_resource_byname(pdev, "phy_ctrl");
	if (IS_ERR(priv->phy_ctrl_base))
		return dev_err_probe(dev, PTR_ERR(priv->phy_ctrl_base),
				     "Couldn't get PHY_CTRL register base\n");

	/* Get SYSREG register base */
	priv->sysreg_base = devm_platform_ioremap_resource_byname(pdev, "sysreg");
	if (IS_ERR(priv->sysreg_base))
		return dev_err_probe(dev, PTR_ERR(priv->sysreg_base),
				     "Couldn't get SYSREG register base\n");

	/* Get dptx SYSREG register base */
	priv->dptxsys_base = devm_platform_ioremap_resource_byname(pdev, "dptx_sys");
	if (IS_ERR(priv->sysreg_base))
		return dev_err_probe(dev, PTR_ERR(priv->sysreg_base),
				     "Couldn't get SYSREG register base\n");

	priv->c10phy_rst = devm_reset_control_get_exclusive(dev, "phy-rst");
	if (IS_ERR(priv->c10phy_rst))
		return dev_err_probe(dev, PTR_ERR(priv->c10phy_rst),
				     "Couldn't get phy-rst\n");

	priv->apb_rst = devm_reset_control_get_exclusive(dev, "apb-rst");
	if (IS_ERR(priv->apb_rst))
		return dev_err_probe(dev, PTR_ERR(priv->apb_rst),
				     "Couldn't get apb-rst\n");

	/* Get GPIO for AUX control (optional) */
	priv->aux_p_gpio = devm_gpiod_get_optional(dev, "aux-p", GPIOD_OUT_LOW);
	if (IS_ERR(priv->aux_p_gpio)) {
		dev_err(dev, "Failed to get AUX_P GPIO\n");
		return PTR_ERR(priv->aux_p_gpio);
	}
	if (priv->aux_p_gpio)
		dev_info(dev, "AUX_P GPIO configured for pull-down control\n");

	priv->aux_n_gpio = devm_gpiod_get_optional(dev, "aux-n", GPIOD_OUT_HIGH);
	if (IS_ERR(priv->aux_n_gpio)) {
		dev_err(dev, "Failed to get AUX_N GPIO\n");
		return PTR_ERR(priv->aux_n_gpio);
	}
	if (priv->aux_n_gpio)
		dev_info(dev, "AUX_N GPIO configured for pull-up control\n");

	ret = udphy_parse_lane_mux_data(priv, dev);
	if (ret)
		dev_err(dev, "Failed to get lane mux\n");

	return ret;
}

static int c10phy_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *child_np;
	struct zhihe_c10phy_priv *priv;
	struct phy_provider *phy_provider;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	platform_set_drvdata(pdev, priv);

	/* Default to host mode, can be changed dynamically */
	priv->usb_role = USB_ROLE_HOST;

	ret = c10phy_parse_dt(priv, pdev);
	if (ret)
		return ret;

	/* Deassert APB reset early and keep it deasserted */
	reset_control_deassert(priv->apb_rst);

	/* Setup Type-C support if enabled */
	if (device_property_present(dev, "orientation-switch")) {
		ret = zhihe_setup_typec_switch(priv);
		if (ret)
			return ret;

		ret = devm_add_action_or_reset(dev, zhihe_switch_unregister, priv);
		if (ret)
			return ret;
	}

	priv->orientation = TYPEC_ORIENTATION_NORMAL;

	mutex_init(&priv->lock);

	/* Initialize extcon device for HPD notification to DP driver */
	priv->extcon = devm_extcon_dev_allocate(dev, c10phy_extcon_cables);

	if (IS_ERR(priv->extcon)) {
		dev_err(dev, "Failed to allocate extcon device: %ld\n", PTR_ERR(priv->extcon));
		return PTR_ERR(priv->extcon);
	}

	ret = devm_extcon_dev_register(dev, priv->extcon);
	if (ret) {
		dev_err(dev, "Failed to register extcon device: %d\n", ret);
		return ret;
	}

	dev_info(dev, "Extcon device registered for HPD notification, %s\n",
		 extcon_get_edev_name(priv->extcon));

	if (device_property_present(dev, "svid")) {
		ret = udphy_setup_typec_mux(priv);
		if (ret)
			return ret;

		ret = devm_add_action_or_reset(dev, udphy_typec_mux_unregister, priv);
		if (ret)
			return ret;
	}

	for_each_available_child_of_node(np, child_np) {
		if (of_node_name_eq(child_np, "dp-port")) {
			priv->dp_phy = devm_phy_create(dev, child_np, &zhihe_dp_phy_ops);
			if (IS_ERR(priv->dp_phy)) {
				dev_err(dev, "failed to create dp phy: %pOFn\n", child_np);
				goto put_child;
			}

			phy_set_bus_width(priv->dp_phy, udphy_dplane_get(priv));
			priv->dp_phy->attrs.max_link_rate =
				udphy_dp_get_max_link_rate(priv, child_np);
			phy_set_drvdata(priv->dp_phy, priv);
		} else if (of_node_name_eq(child_np, "u3-port")) {
			priv->phy = devm_phy_create(dev, child_np, &c10phy_ops);
			if (IS_ERR(priv->phy)) {
				dev_err(dev, "failed to create usb phy: %pOFn\n", child_np);
				goto put_child;
			}
			phy_set_drvdata(priv->phy, priv);
		} else {
			continue;
		}
	}
	dev_set_drvdata(dev, priv);

	/* Register PHY provider */
	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		return dev_err_probe(dev, PTR_ERR(phy_provider),
				     "failed to register phy provider\n");

	/* Create sysfs attribute for orientation control */
	ret = sysfs_create_group(&dev->kobj, &c10phy_attr_group);
	if (ret) {
		dev_err(dev, "Failed to create sysfs group: %d\n", ret);
		return ret;
	}

	return 0;

put_child:
	of_node_put(child_np);
	return ret;
}

static int c10phy_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	sysfs_remove_group(&dev->kobj, &c10phy_attr_group);

	return 0;
}

static const struct of_device_id c10phy_of_match[] = {
	{ .compatible = "zhihe,a210-c10phy" },
	{ }
};
MODULE_DEVICE_TABLE(of, c10phy_of_match);

static struct platform_driver c10phy_driver = {
	.probe		= c10phy_probe,
	.remove		= c10phy_remove,
	.driver		= {
		.name	= "phy-zhihe-a210-c10phy",
		.of_match_table	= c10phy_of_match,
	},
};

module_platform_driver(c10phy_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ZHIHE C10PHY USB3.1 PHY Driver");
MODULE_AUTHOR("Zhiguo Zhu <zhiguo.zhu@linux.alibaba.com>");
