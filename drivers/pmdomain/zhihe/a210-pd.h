/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#ifndef __A210_PD_H
#define __A210_PD_H

#define A210_PD_NAME_SIZE 20
#define A210_PD_STATE_NAME_SIZE 10

#define A210_PD_GPU			0
#define A210_PD_NPU_WRAPPER		1
#define A210_PD_NPU_IP			2
#define A210_PD_D2D			3
#define A210_PD_PCIE0			4
#define A210_PD_PCIE1			5
#define A210_PD_SATA			6
#define A210_PD_USB			7
#define A210_PD_VI_WRAP			8
#define A210_PD_VI_ISP			9
#define A210_PD_VO			10
#define A210_PD_VP_WRAP			11
#define A210_PD_VENC			12
#define A210_PD_VDEC			13
#define A210_PD_TOP			14
#define A210_PD_PERI0			15
#define A210_PD_PERI1			16
#define A210_PD_PERI2			17
#define A210_PD_PERI3			18
#define A210_PD_CAN			19
#define A210_PD_SPI			20
#define A210_PD_QSPI			21
#define A210_PD_UART			22
#define A210_PD_PWM			23
#define A210_PD_EMMC			24
#define A210_PD_SDIO			25
#define A210_PD_DMAC			26
#define A210_POWER_DOMAINS_MAX		27
#define CCU_MODE_GATING			0
#define CCU_MODE_SCALING		1

#define BPC_SW_MODEL 1
#define BPC_HW_MODEL 0
#define PCU_REG_TRIGGER 1
#define PCU_R2P_TRIGGER 0

enum {
	PMIC_CTRL,
	VP_PCA,
	VP_WRAP_BPC,
	VP_WRAP_PCU,
	VP_VENC_BPC,
	VP_VENC_PCU,
	VP_VDEC_BPC,
	VP_VDEC_PCU,
	VP_R2P,
	GPU_PCA,
	GPU_BPC,
	GPU_PCU,
	NPU_PCA,
	NPU_WRAP_BPC,
	NPU_WRAP_PCU,
	NPU_IP_BPC,
	NPU_IP_PCU,
	PCIE0_BPC,
	PCIE0_PCU,
	PCIE1_BPC,
	PCIE1_PCU,
	SATA_BPC,
	SATA_PCU,
	USB_BPC,
	USB_PCU,
	VI_R2P,
	VI_WRAP_BPC,
	VI_WRAP_PCU,
	VI_ISP_BPC,
	VI_ISP_PCU,
	VO_BPC,
	VO_PCU,
	A210_POWER_DOMAIN_REGS_MAX,
};

/* represent power domains info at soc level */
struct a210_pd_soc {
	struct device *dev;
	struct a210_pm_domain *domains[A210_POWER_DOMAINS_MAX];
	u32 num_domains;
	struct regulator *regulators[A210_POWER_DOMAINS_MAX];
};

struct a210_ccu {
	u32 delay;
	u32 step;
	u32 ratio;
	void __iomem *base;
};

struct a210_ccu_grp {
	bool enable;
	u32 mode;
	u32 num_ccus;
	struct list_head list;
	struct a210_ccu ccu[];
};

/* represent a single power domain */
struct a210_pm_domain {
	struct generic_pm_domain pd;
	u16 index;
	struct a210_pd_soc *soc;
	struct reset_control *reset;
	struct clk_bulk_data *clks;
	u32 num_clks;
	void __iomem *pca_base;
	void __iomem *bpc_base;
	void __iomem *pcu_base;
	struct list_head ccu_grp;
};

enum power_mode {
	OFF                 = 0x0,
	MEM_SD              = 0x1,
	MEM_RET             = 0x2,
	MEM_SD_ONLY         = 0x9,
	MEM_DSLP            = 0xa,
	MEM_SLP             = 0xb,
	CG                  = 0xf,
	ON                  = 0x1f,
	WAIT_OFF            = 0x100,
	WAIT_MEM_SD         = 0x101,
	WAIT_MEM_RET        = 0x102,
	WAIT_MEM_SD_ONLY    = 0x109,
	WAIT_MEM_DSLP       = 0x10a,
	WAIT_MEM_SLP        = 0x10b,
	WAIT_CG             = 0x10f,
	WAIT_ON             = 0x11f
};

#endif
