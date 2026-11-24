// SPDX-License-Identifier: GPL-2.0
/*
 * XuanTie DWXGMAC PCI glue driver
 *
 * Copyright (C) 2021 Alibaba Group Holding Limited.
 * Author: David Li <xiaojin.cxj@linux.alibaba.com>
 */

#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/property.h>

#include "stmmac.h"
#include "hwif.h"

#define XUANTIE_STMMAC_DRIVER_NAME	"dwmac-xuantie"
#define XUANTIE_STMMAC_MSI_VEC_MAX	64
/* TX/RX vectors 0-31 plus the MAC vector 32 */
#define XUANTIE_STMMAC_MSI_VEC_NEEDED	33
#define XUANTIE_STMMAC_MAX_RX_QUEUE	8
#define XUANTIE_STMMAC_MAX_TX_QUEUE	8

/* The MAC register block sits at a fixed offset inside the 32GB BAR0
 * window; map only that block or riscv vmalloc would be exhausted.
 */
#define XUANTIE_STMMAC_MAC_REG_OFFSET	0x400f00000ULL
#define XUANTIE_STMMAC_MAC_REG_LEN	0x10000

/* XuanTie ACE (AXI Cache Encoding) register offsets */
#define XUANTIE_TX_AR_ACE_CONTROL	0x3010
#define XUANTIE_RX_AW_ACE_CONTROL	0x3018
#define XUANTIE_TXRX_AWAR_ACE_CONTROL	0x301c

/* XGMAC DMA_MODE interrupt mode (INTM) field */
#define XUANTIE_DMA_MODE		0x3000
#define XUANTIE_INTM_MASK		0x3000
#define XUANTIE_INTM_MODE1		0x1000

struct stmmac_pci_info {
	int (*setup)(struct pci_dev *pdev, struct plat_stmmacenet_data *plat);
};

/* Per-device DMA ops copy, safe against concurrent probes */
struct xuantie_stmmac_priv {
	struct stmmac_dma_ops dma_ops;
	void (*orig_dma_init)(void __iomem *ioaddr,
			      struct stmmac_dma_cfg *dma_cfg, int atds);
};

/* Lets the DMA init hook find the per-device state via container_of() */
struct xuantie_stmmac_dma_cfg {
	struct stmmac_dma_cfg dma_cfg;	/* must be first */
	struct xuantie_stmmac_priv *xpriv;
};

static int stmmac_config_single_msi(struct pci_dev *pdev,
				    struct plat_stmmacenet_data *plat,
				    struct stmmac_resources *res)
{
	int ret;

	ret = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_ALL_TYPES);
	if (ret < 0) {
		dev_err(&pdev->dev, "%s: Single IRQ enablement failed\n",
			__func__);
		return ret;
	}

	res->irq = pci_irq_vector(pdev, 0);
	res->wol_irq = res->irq;
	plat->flags &= ~STMMAC_FLAG_MULTI_MSI_EN;
	dev_info(&pdev->dev, "%s: Single IRQ enablement successful\n",
		 __func__);

	return 0;
}

static int stmmac_config_multi_msi(struct pci_dev *pdev,
				   struct plat_stmmacenet_data *plat,
				   struct stmmac_resources *res)
{
	int i, ret;

	if (plat->msi_rx_base_vec >= XUANTIE_STMMAC_MSI_VEC_MAX ||
	    plat->msi_tx_base_vec >= XUANTIE_STMMAC_MSI_VEC_MAX) {
		dev_err(&pdev->dev, "%s: Invalid RX & TX vector defined\n",
			__func__);
		return -EINVAL;
	}

	ret = pci_alloc_irq_vectors(pdev, XUANTIE_STMMAC_MSI_VEC_NEEDED,
				    XUANTIE_STMMAC_MSI_VEC_MAX,
				    PCI_IRQ_MSIX);
	if (ret < XUANTIE_STMMAC_MSI_VEC_NEEDED) {
		if (ret > 0)
			pci_free_irq_vectors(pdev);
		dev_info(&pdev->dev,
			 "%s: multi MSI unavailable, falling back to single MSI\n",
			 __func__);
		return ret < 0 ? ret : -EINVAL;
	}

	for (i = 0; i < XUANTIE_STMMAC_MAX_RX_QUEUE; i++)
		res->rx_irq[i] = pci_irq_vector(pdev, plat->msi_rx_base_vec + i);

	for (i = 0; i < XUANTIE_STMMAC_MAX_TX_QUEUE; i++)
		res->tx_irq[i] = pci_irq_vector(pdev, plat->msi_tx_base_vec + i);

	if (plat->msi_mac_vec < XUANTIE_STMMAC_MSI_VEC_MAX)
		res->irq = pci_irq_vector(pdev, plat->msi_mac_vec);
	if (plat->msi_wol_vec < XUANTIE_STMMAC_MSI_VEC_MAX)
		res->wol_irq = pci_irq_vector(pdev, plat->msi_wol_vec);
	if (plat->msi_sfty_ce_vec < XUANTIE_STMMAC_MSI_VEC_MAX)
		res->sfty_ce_irq = pci_irq_vector(pdev, plat->msi_sfty_ce_vec);
	if (plat->msi_sfty_ue_vec < XUANTIE_STMMAC_MSI_VEC_MAX)
		res->sfty_ue_irq = pci_irq_vector(pdev, plat->msi_sfty_ue_vec);

	plat->flags |= STMMAC_FLAG_MULTI_MSI_EN;
	dev_info(&pdev->dev, "%s: multi MSI enablement successful\n", __func__);

	return 0;
}

static int stmmac_xuantie_data(struct pci_dev *pdev,
			       struct plat_stmmacenet_data *plat, bool tbs_en)
{
	int i;

	plat->has_gmac = 0;
	plat->has_gmac4 = 0;
	plat->has_xgmac = 1;
	plat->mdio_bus_data->needs_reset = true;

	plat->multicast_filter_bins = HASH_TABLE_SIZE;

	plat->unicast_filter_entries = 1;

	plat->maxmtu = JUMBO_LEN;

	plat->tx_sched_algorithm = MTL_TX_ALGORITHM_WRR;
	for (i = 0; i < XUANTIE_STMMAC_MAX_TX_QUEUE; i++) {
		plat->tx_queues_cfg[i].use_prio = false;
		plat->tx_queues_cfg[i].mode_to_use = MTL_QUEUE_DCB;
		plat->tx_queues_cfg[i].weight = 25;
		if (tbs_en && i > 0)
			plat->tx_queues_cfg[i].tbs_en = 1;
	}

	plat->rx_sched_algorithm = MTL_RX_ALGORITHM_SP;
	for (i = 0; i < XUANTIE_STMMAC_MAX_RX_QUEUE; i++) {
		plat->rx_queues_cfg[i].use_prio = false;
		plat->rx_queues_cfg[i].mode_to_use = MTL_QUEUE_DCB;
		plat->rx_queues_cfg[i].pkt_route = 0x0;
		plat->rx_queues_cfg[i].chan = i;
	}

	/* Distinct MDIO bus id per port: both RCs use the same BDF */
	plat->bus_id = (pci_domain_nr(pdev->bus) << 16) |
		       (pdev->bus->number << 8) | pdev->devfn;
	/* The onboard PHY address varies across board revisions */
	plat->phy_addr = -1;
	/* Match the RCiEP CSR clock (60-100 MHz): MDC = clk_csr / 42 */
	plat->clk_csr = STMMAC_CSR_60_100M;

	plat->dma_cfg->pbl = 32;
	plat->dma_cfg->pblx8 = true;

	plat->axi = devm_kzalloc(&pdev->dev, sizeof(*plat->axi), GFP_KERNEL);
	if (!plat->axi)
		return -ENOMEM;

	plat->axi->axi_wr_osr_lmt = 63;
	plat->axi->axi_rd_osr_lmt = 63;
	plat->axi->axi_fb = false;
	plat->axi->axi_blen[0] = 32;
	plat->axi->axi_blen[1] = 64;
	plat->axi->axi_blen[2] = 128;
	plat->axi->axi_blen[3] = 256;
	plat->msi_mac_vec = 32;
	plat->msi_rx_base_vec = 16;
	plat->msi_tx_base_vec = 0;
	plat->tx_fifo_size = 155648;
	plat->rx_fifo_size = 163840;

	return 0;
}

static void xuantie_stmmac_remove_swnode(void *data)
{
	fwnode_remove_software_node(data);
}

static void xuantie_stmmac_unregister_sw_node(void *data)
{
	software_node_unregister(data);
}

/* Publish a fixed link via a software node so phylink skips PHY scanning
 * and applies the speed as-is.
 */
static int xuantie_stmmac_add_fixed_link(struct pci_dev *pdev,
					 struct plat_stmmacenet_data *plat,
					 u32 speed)
{
	struct property_entry *fixed_props;
	struct software_node *fixed_node;
	struct fwnode_handle *swnode;
	int ret;

	swnode = fwnode_create_software_node(NULL, NULL);
	if (IS_ERR(swnode))
		return PTR_ERR(swnode);

	ret = devm_add_action(&pdev->dev, xuantie_stmmac_remove_swnode,
			      swnode);
	if (ret) {
		fwnode_remove_software_node(swnode);
		return ret;
	}

	fixed_props = devm_kzalloc(&pdev->dev, sizeof(*fixed_props) * 3,
				   GFP_KERNEL);
	if (!fixed_props)
		return -ENOMEM;

	fixed_props[0] = PROPERTY_ENTRY_U32("speed", speed);
	fixed_props[1] = PROPERTY_ENTRY_BOOL("full-duplex");

	fixed_node = devm_kzalloc(&pdev->dev, sizeof(*fixed_node), GFP_KERNEL);
	if (!fixed_node)
		return -ENOMEM;

	fixed_node->name = "fixed-link";
	fixed_node->parent = to_software_node(swnode);
	fixed_node->properties = fixed_props;

	ret = software_node_register(fixed_node);
	if (ret)
		return ret;

	ret = devm_add_action(&pdev->dev, xuantie_stmmac_unregister_sw_node,
			      fixed_node);
	if (ret) {
		software_node_unregister(fixed_node);
		return ret;
	}

	plat->port_node = swnode;
	dev_info(&pdev->dev, "fixed link at %u Mbps, no PHY\n", speed);

	return 0;
}

static int stmmac_xlgmac_data(struct pci_dev *pdev,
			      struct plat_stmmacenet_data *plat)
{
	int ret;

	ret = stmmac_xuantie_data(pdev, plat, false);
	if (ret)
		return ret;

	/* 100G port: fixed link only */
	ret = xuantie_stmmac_add_fixed_link(pdev, plat, 100000);
	if (ret)
		return ret;

	plat->phy_interface = PHY_INTERFACE_MODE_XLGMII;

	return 0;
}

static int stmmac_xgmac_data(struct pci_dev *pdev,
			     struct plat_stmmacenet_data *plat)
{
	int ret;

	ret = stmmac_xuantie_data(pdev, plat, true);
	if (ret)
		return ret;

	/* 1G over an external PHY: interface mode set after the MDIO scan */
	plat->phy_interface = PHY_INTERFACE_MODE_GMII;

	return 0;
}

static int stmmac_xgmac_10g_data(struct pci_dev *pdev,
				 struct plat_stmmacenet_data *plat)
{
	int ret;

	ret = stmmac_xuantie_data(pdev, plat, true);
	if (ret)
		return ret;

	/* 10G over SerDes: fixed link, no PHY */
	ret = xuantie_stmmac_add_fixed_link(pdev, plat, 10000);
	if (ret)
		return ret;

	plat->phy_interface = PHY_INTERFACE_MODE_XGMII;

	return 0;
}

static const struct stmmac_pci_info stmmac_xlgmac_pci_info = {
	.setup = stmmac_xlgmac_data,
};

static const struct stmmac_pci_info stmmac_xgmac_pci_info = {
	.setup = stmmac_xgmac_data,
};

static const struct stmmac_pci_info stmmac_xgmac_10g_pci_info = {
	.setup = stmmac_xgmac_10g_data,
};

/* Hook the shared DMA init to reprogram INTM (cleared by each DMA soft
 * reset) and the ACE cache attributes, keeping the dwxgmac2 core intact.
 */
static void dwmac_xuantie_dma_init(void __iomem *ioaddr,
				   struct stmmac_dma_cfg *dma_cfg, int atds)
{
	struct xuantie_stmmac_dma_cfg *xcfg =
		container_of(dma_cfg, struct xuantie_stmmac_dma_cfg, dma_cfg);
	struct xuantie_stmmac_priv *xpriv = xcfg->xpriv;
	u32 value;

	xpriv->orig_dma_init(ioaddr, dma_cfg, atds);

	/* Program the ACE cache attributes of the DMA AXI transactions */
	writel(0x30303, ioaddr + XUANTIE_TX_AR_ACE_CONTROL);
	writel(0x3030303, ioaddr + XUANTIE_RX_AW_ACE_CONTROL);
	writel(0x0303, ioaddr + XUANTIE_TXRX_AWAR_ACE_CONTROL);

	if (dma_cfg->multi_msi_en) {
		value = readl(ioaddr + XUANTIE_DMA_MODE);
		value &= ~XUANTIE_INTM_MASK;
		value |= XUANTIE_INTM_MODE1;
		writel(value, ioaddr + XUANTIE_DMA_MODE);
	}
}

/* Called after hardware init and before register_netdev(): install the
 * DMA ops hook without racing open(), then derive the 1G port's interface
 * mode from the PHY on the already-populated MDIO bus.
 */
static void dwmac_xuantie_post_hw_init(struct net_device *ndev,
				       struct plat_stmmacenet_data *plat)
{
	struct xuantie_stmmac_dma_cfg *xcfg =
		container_of(plat->dma_cfg, struct xuantie_stmmac_dma_cfg,
			     dma_cfg);
	struct stmmac_priv *priv = netdev_priv(ndev);
	struct xuantie_stmmac_priv *xpriv = xcfg->xpriv;
	struct phy_device *phydev;
	int addr;

	xpriv->dma_ops = *priv->hw->dma;
	xpriv->orig_dma_init = xpriv->dma_ops.init;
	xpriv->dma_ops.init = dwmac_xuantie_dma_init;

	priv->hw->dma = &xpriv->dma_ops;

	/* 100G/10G ports are fixed by the setup callback; only the 1G
	 * port scans for a PHY.
	 */
	if (plat->phy_interface != PHY_INTERFACE_MODE_GMII)
		return;

	/* Derive the interface mode from the first PHY found on the bus */
	for (addr = 0; addr < PHY_MAX_ADDR; addr++) {
		phydev = mdiobus_get_phy(priv->mii, addr);
		if (!phydev)
			continue;

		/* Low 4 bits of phy_id are the revision; case values have
		 * them cleared.
		 */
		switch (phydev->phy_id & GENMASK(31, 4)) {
		case 0x01410cc0:	/* Marvell 88E1111 */
			plat->phy_interface = PHY_INTERFACE_MODE_GMII;
			break;
		case 0x00000110:	/* Motorcomm YT8521S */
		case 0x001cc910:	/* Realtek RTL8211FI-CG / RTL8211FS-CG */
			plat->phy_interface = PHY_INTERFACE_MODE_RGMII;
			break;
		default:
			break;
		}
		return;
	}
}

/* Allocate the per-device state and hand over to the stmmac core */
static int stmmac_pci_probe(struct pci_dev *pdev,
			    const struct pci_device_id *id)
{
	struct stmmac_pci_info *info =
		(struct stmmac_pci_info *)id->driver_data;
	struct xuantie_stmmac_dma_cfg *xcfg;
	struct xuantie_stmmac_priv *xpriv;
	struct plat_stmmacenet_data *plat;
	struct stmmac_resources res;
	int cpu_local, node, ret;

	memset(&res, 0, sizeof(res));

	plat = devm_kzalloc(&pdev->dev, sizeof(*plat), GFP_KERNEL);
	if (!plat)
		return -ENOMEM;

	plat->mdio_bus_data = devm_kzalloc(&pdev->dev,
					   sizeof(*plat->mdio_bus_data),
					   GFP_KERNEL);
	if (!plat->mdio_bus_data)
		return -ENOMEM;

	xcfg = devm_kzalloc(&pdev->dev, sizeof(*xcfg), GFP_KERNEL);
	if (!xcfg)
		return -ENOMEM;
	plat->dma_cfg = &xcfg->dma_cfg;

	xpriv = devm_kzalloc(&pdev->dev, sizeof(*xpriv), GFP_KERNEL);
	if (!xpriv)
		return -ENOMEM;
	xcfg->xpriv = xpriv;

	plat->safety_feat_cfg = devm_kzalloc(&pdev->dev,
					     sizeof(*plat->safety_feat_cfg),
					     GFP_KERNEL);
	if (!plat->safety_feat_cfg)
		return -ENOMEM;

	ret = pcim_enable_device(pdev);
	if (ret) {
		dev_err(&pdev->dev, "%s: ERROR: failed to enable device\n",
			__func__);
		return ret;
	}

	/* Map only the MAC register block inside the 32GB BAR0 window */
	if (pci_resource_len(pdev, 0) <
	    XUANTIE_STMMAC_MAC_REG_OFFSET + XUANTIE_STMMAC_MAC_REG_LEN) {
		dev_err(&pdev->dev, "%s: BAR0 too short for MAC registers\n",
			__func__);
		return -EINVAL;
	}

	/* Claim BAR0 so the register window shows up in /proc/iomem */
	ret = pci_request_region(pdev, 0, pci_name(pdev));
	if (ret) {
		dev_err(&pdev->dev, "%s: failed to request BAR0\n", __func__);
		return ret;
	}

	res.addr = devm_ioremap(&pdev->dev,
				pci_resource_start(pdev, 0) +
				XUANTIE_STMMAC_MAC_REG_OFFSET,
				XUANTIE_STMMAC_MAC_REG_LEN);
	if (!res.addr) {
		ret = -ENOMEM;
		goto err_release_region;
	}

	pci_set_master(pdev);

	/* Mark all MSI vectors invalid; setup() sets the real ones */
	plat->msi_mac_vec = XUANTIE_STMMAC_MSI_VEC_MAX;
	plat->msi_wol_vec = XUANTIE_STMMAC_MSI_VEC_MAX;
	plat->msi_sfty_ce_vec = XUANTIE_STMMAC_MSI_VEC_MAX;
	plat->msi_sfty_ue_vec = XUANTIE_STMMAC_MSI_VEC_MAX;
	plat->msi_rx_base_vec = XUANTIE_STMMAC_MSI_VEC_MAX;
	plat->msi_tx_base_vec = XUANTIE_STMMAC_MSI_VEC_MAX;

	ret = info->setup(pdev, plat);
	if (ret)
		goto err_release_region;

	plat->safety_feat_cfg->tsoee = 1;
	plat->safety_feat_cfg->mrxpee = 1;
	plat->safety_feat_cfg->mestee = 1;
	plat->safety_feat_cfg->mrxee = 1;
	plat->safety_feat_cfg->mtxee = 1;
	plat->safety_feat_cfg->epsi = 1;
	plat->safety_feat_cfg->edpp = 1;
	plat->safety_feat_cfg->prtyen = 1;
	plat->safety_feat_cfg->tmouten = 1;

	node = dev_to_node(&pdev->dev);
	cpu_local = max_t(int, 1, cpumask_weight(cpumask_of_node(node)));
	plat->rx_queues_to_use = min_t(int, XUANTIE_STMMAC_MAX_RX_QUEUE,
				       cpu_local);
	plat->tx_queues_to_use = min_t(int, XUANTIE_STMMAC_MAX_TX_QUEUE,
				       cpu_local);

	ret = stmmac_config_multi_msi(pdev, plat, &res);
	if (ret) {
		ret = stmmac_config_single_msi(pdev, plat, &res);
		if (ret) {
			dev_err(&pdev->dev, "%s: ERROR: failed to enable IRQ\n",
				__func__);
			goto err_release_region;
		}
	}
	plat->flags |= STMMAC_FLAG_TSO_EN;
	plat->rss_en = 1;

	/* Install the DMA ops hook before the device can be opened */
	plat->post_hw_init = dwmac_xuantie_post_hw_init;

	ret = stmmac_dvr_probe(&pdev->dev, plat, &res);
	if (ret) {
		pci_free_irq_vectors(pdev);
		goto err_release_region;
	}

	return 0;

err_release_region:
	pci_release_region(pdev, 0);
	return ret;
}

/* Undo probe: free the net and PCI resources */
static void stmmac_pci_remove(struct pci_dev *pdev)
{
	stmmac_dvr_remove(&pdev->dev);

	pci_free_irq_vectors(pdev);
	pci_release_region(pdev, 0);
}

static int __maybe_unused xuantie_stmmac_pci_suspend(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	int ret;

	ret = stmmac_suspend(dev);
	if (ret)
		return ret;

	ret = pci_save_state(pdev);
	if (ret)
		return ret;

	pci_disable_device(pdev);
	pci_wake_from_d3(pdev, true);
	return 0;
}

static int __maybe_unused xuantie_stmmac_pci_resume(struct device *dev)
{
	struct pci_dev *pdev = to_pci_dev(dev);
	int ret;

	pci_restore_state(pdev);
	pci_set_power_state(pdev, PCI_D0);

	ret = pci_enable_device(pdev);
	if (ret)
		return ret;

	pci_set_master(pdev);

	return stmmac_resume(dev);
}

static SIMPLE_DEV_PM_OPS(xuantie_stmmac_pm_ops, xuantie_stmmac_pci_suspend,
			 xuantie_stmmac_pci_resume);

#ifndef PCI_VENDOR_ID_ALIBABA
#define PCI_VENDOR_ID_ALIBABA 0x1ded
#endif

/* XuanTie device IDs under the Alibaba PCI-SIG vendor ID */
#define PCI_DEVICE_ID_ALIBABA_XUANTIE_XLGMAC 0x1109
#define PCI_DEVICE_ID_ALIBABA_XUANTIE_XGMAC 0x110a
#define PCI_DEVICE_ID_ALIBABA_XUANTIE_XGMAC_10G 0x110b

static const struct pci_device_id stmmac_id_table[] = {
	{ PCI_DEVICE_DATA(ALIBABA, XUANTIE_XLGMAC, &stmmac_xlgmac_pci_info) },
	{ PCI_DEVICE_DATA(ALIBABA, XUANTIE_XGMAC, &stmmac_xgmac_pci_info) },
	{ PCI_DEVICE_DATA(ALIBABA, XUANTIE_XGMAC_10G,
			  &stmmac_xgmac_10g_pci_info) },
	{ }
};

MODULE_DEVICE_TABLE(pci, stmmac_id_table);

static struct pci_driver stmmac_pci_driver = {
	.name = XUANTIE_STMMAC_DRIVER_NAME,
	.id_table = stmmac_id_table,
	.probe = stmmac_pci_probe,
	.remove = stmmac_pci_remove,
	.driver = {
		.pm = &xuantie_stmmac_pm_ops,
	},
};

module_pci_driver(stmmac_pci_driver);

MODULE_AUTHOR("Alibaba Group Holding Limited");
MODULE_DESCRIPTION("XuanTie XGMAC/XLGMAC PCI Ethernet driver");
MODULE_LICENSE("GPL");
