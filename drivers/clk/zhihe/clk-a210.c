// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/types.h>

#include <dt-bindings/clock/zhihe,a210-clk.h>

#include "clk-helper.h"

/* top reg idx */
#define PLL_WRAP		0
#define TOP_CRG			1
#define CPU_SS_CLK_SYSREG	2
#define CPU_SS_CPU_PLL		3
#define DDR0_SYSREG		4
#define DDR1_SYSREG		5
#define SLC_DUAL_SYSREG		6
#define TOP_CRG_T		7
#define CPU_SS_CCU		8
/* gpu reg idx */
#define GPU_SS_PWRAP_CLK_EN	0
#define GPU_SS_TOP_CLK_EN	1
/* pcie reg idx */
#define PCIE_CLK_EN		0
/* usb reg idx */
#define USB_CLK_EN		0
/* vi reg idx */
#define VI_CLK			0
#define VI_MISC_CTRL		1
/* vp reg idx */
#define VP_CLK			0
/* vo reg idx */
#define VO_CLK			0
#define VO_PATH_CTRL		1
/* npu reg idx */
#define NPU_CLK			0
#define NPU_TOP_CLK		1
/* peri reg idx */
#define PERI0_SYSREG		0
#define PERI1_SYSREG		1
#define PERI2_SYSREG		2
#define PERI3_SYSREG		3
#define TEE_CRG			4

static u32 share_cnt_ddr_pll_clk_en;

enum a210_pll_clktype {
	AUDIO0_PLL,
	AUDIO1_PLL,
	VIDEO_PLL,
	GMAC_PLL,
	DVFS_PLL,
	DPU0_PLL,
	DPU1_PLL,
	DPU2_PLL,
	TEE_PLL,
	DDR_PLL,
	C920_PLL,
	C908_PLL,
};

static const char *const noc_cclk_mux_parents[] = {
	"dpu1_pll_foutvco",
	"video_pll_foutvco",
	"gmac_pll_foutvco",
};

static const char *const top_cpu_ddr1_aclk_parents[] = {
	"cbus2ddr_aclk1",
	"gmac_pll_foutpostdiv",
};

static const char *const top_cpu_ddr0_aclk_parents[] = {
	"cbus2ddr_aclk0",
	"gmac_pll_foutpostdiv",
};

static const char *const ddr_pll_clkout_parents[] = {
	"ddr_pll_foutpostdiv",
	"ddr_pll_fout1ph0",
	"ddr_pll_fout2",
};

static const char *const top_cpu_bak_pll0_clk_mux0_parents[] = {
	"audio0_pll_foutvco",
	"dvfs_pll_foutvco",
	"top_cpu_bak_pll0_clk_div0",
	"top_cpu_bak_pll0_clk_div1",
};

static const char *const top_cpu_bak_pll0_clk_mux_parents[] = {
	"dpu1_pll_foutvco",
	"top_cpu_bak_pll0_clk_mux0",
	"dvfs_pll_foutvco",
	"audio0_pll_foutvco",
	"top_cpu_bak_pll0_clk_div0",
	"top_cpu_bak_pll0_clk_div4",
	"top_cpu_bak_pll0_clk_div3",
	"top_cpu_bak_pll0_clk_div1",
};

static const char *const top_cpu_bak_pll1_clk_mux0_parents[] = {
	"audio0_pll_foutvco",
	"dvfs_pll_foutvco",
	"top_cpu_bak_pll1_clk_div0",
	"top_cpu_bak_pll1_clk_div1",
};

static const char *const top_cpu_bak_pll1_clk_mux_parents[] = {
	"dpu1_pll_foutvco",
	"top_cpu_bak_pll1_clk_mux0",
	"dvfs_pll_foutvco",
	"audio0_pll_foutvco",
	"top_cpu_bak_pll1_clk_div0",
	"top_cpu_bak_pll1_clk_div4",
	"top_cpu_bak_pll1_clk_div3",
	"top_cpu_bak_pll1_clk_div1",
};

static const char *const top_cpusys_pic_clk_mux_parents[] = {
	"gmac_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const top_cpusys_bus_clk_mux_parents[] = {
	"video_pll_foutvco",
	"gmac_pll_foutvco",
	"audio0_pll_foutvco",
	"dpu2_pll_foutvco",
};

static const char *const d2d_phy_ref_clk_parents[] = {
	"d2d_phy_ref_clk_100M",
	"aon_osc_clk_logic",
};

static const char *const top_d2d_aclk_mux_parents[] = {
	"dpu1_pll_foutvco",
	"gmac_pll_foutvco",
	"audio1_pll_foutvco",
	"audio0_pll_foutvco",
};

static const char *const top_gpu_core_clk_mux_parents[] = {
	"dpu1_pll_foutvco",
	"video_pll_foutvco",
	"dvfs_pll_foutvco",
	"audio0_pll_foutvco",
};

static const char *const pcie_ss_axi_m_aclk_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const peri0_timer_clk_parents[] = {
	"peri0_timer_clk_100M",
	"aon_osc_clk_logic",
};

static const char *const peri2_i2s3_src_clk_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const peri1_i2s0_src_clk_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const peri2_i2s1_src_clk_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const peri2_i2s2_src_clk_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const peri1_qspi_ssi_clk_parents[] = {
	"peri1_qspi_ssi_clk_div1",
	"peri1_qspi_ssi_clk_div0",
};

static const char *const peri2_qspi_ssi_clk_parents[] = {
	"peri2_qspi_ssi_clk_div1",
	"peri2_qspi_ssi_clk_div0",
};

static const char *const peri1_tdm_src_clk_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const pdm_clk_mux_parents[] = {
	"audio1_pll_fout2",
	"audio0_pll_fout2",
};

static const char *const emmc_ref_clk_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const pad_sensor_vclk0_mux_parents[] = {
	"dpu0_pll_foutpostdiv",
	"dpu1_pll_foutpostdiv",
	"dpu2_pll_foutpostdiv",
	"aon_osc_clk_logic",
};

static const char *const pad_sensor_vclk1_mux_parents[] = {
	"dpu0_pll_foutpostdiv",
	"dpu1_pll_foutpostdiv",
	"dpu2_pll_foutpostdiv",
	"aon_osc_clk_logic",
};

static const char *const vp_aclk_mux_parents[] = {
	"video_pll_foutvco",
	"audio0_pll_foutvco",
};

static const char *const g2d_cclk_mux_parents[] = {
	"audio0_pll_foutvco",
	"gmac_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const vdec_cclk_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
	"gmac_pll_foutvco",
	"audio1_pll_foutvco",
};

static const char *const venc_cclk_mux_parents[] = {
	"gmac_pll_foutvco",
	"video_pll_foutvco",
	"audio0_pll_foutvco",
	"audio1_pll_foutvco",
};

static const char *const npu_aclk_mux_parents[] = {
	"gmac_pll_foutvco",
	"video_pll_foutvco",
	"audio0_pll_foutvco",
	"audio1_pll_foutvco",
};

static const char *const npu_cclk_mux_parents[] = {
	"gmac_pll_foutvco",
	"video_pll_foutvco",
	"audio0_pll_foutvco",
	"audio1_pll_foutvco",
};

static const char *const c908_cpu_clk_parents[] = {
	"c908_pll_foutvco",
	"top_cpu_bak_pll0_clk",
};

static const char *const c920_cpu_clk_parents[] = {
	"c920_pll_foutvco",
	"top_cpu_bak_pll1_clk",
};

static const char *const tee_clk_mux_parents[] = {
	"video_pll_foutvco",
	"tee_pll_foutvco",
};

static const char *const isp_if_ref_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const vi_preocc_mipi0_ifclk_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const vi_preocc_mipi1_ifclk_mux_parents[] = {
	"audio0_pll_foutvco",
	"video_pll_foutvco",
};

static const char *const vo_mipi_pixclk_mux_parents[] = {
	"dpu0_pixclk",
	"dpu1_pixclk",
	"dpu2_pixclk",
};

static const char *const vo_hdmi_pixclk_mux_parents[] = {
	"dpu0_pixclk",
	"dpu1_pixclk",
	"dpu2_pixclk",
};

static const char *const vo_dptx_pixclk_mux_parents[] = {
	"dpu0_pixclk",
	"dpu1_pixclk",
	"dpu2_pixclk",
};

static struct zhihe_pll_rate_table a210_teepll_tbl[] = {
	PLL_RATE(2400000000, 800000000U, 1, 100, 0, 3, 1),
};

static struct zhihe_pll_rate_table a210_dpu2pll_tbl[] = {
	PLL_RATE(2376000000, 1188000000U, 1, 99, 0, 2, 1),
};

static struct zhihe_pll_rate_table a210_dpu1pll_tbl[] = {
	PLL_RATE(2376000000, 1188000000U, 1, 99, 0, 2, 1),
};

static struct zhihe_pll_rate_table a210_dpu0pll_tbl[] = {
	PLL_RATE(2376000000, 1188000000U, 1, 99, 0, 2, 1),
};

static struct zhihe_pll_rate_table a210_dvfspll_tbl[] = {
	PLL_RATE(1920000000, 960000000U, 1, 80, 0, 2, 1),
};

static struct zhihe_pll_rate_table a210_gmacpll_tbl[] = {
	PLL_RATE(3000000000, 1000000000U, 1, 125, 0, 3, 1),
};

static struct zhihe_pll_rate_table a210_videopll_tbl[] = {
	PLL_RATE(2640000000U, 1320000000U, 1, 110, 0, 2, 1),
};

static struct zhihe_pll_rate_table a210_audio0pll_tbl[] = {
	PLL_RATE(2359296000U, 1179648000U, 1, 98, 5100274, 2, 1),
};

static struct zhihe_pll_rate_table a210_audio1pll_tbl[] = {
	PLL_RATE(2528870400U, 1264435200U, 1, 105, 6200859, 2, 1),
};

static struct zhihe_pll_rate_table a210_c908pll_tbl[] = {
	PLL_RATE(1200000000U, 1200000000U, 1, 50, 0, 1, 1),
	PLL_RATE(1500000000U, 1500000000U, 2, 125, 0, 1, 1),
	PLL_RATE(1698000000U, 1698000000U, 4, 283, 0, 1, 1),
	PLL_RATE(1896000000U, 1896000000U, 1, 79, 0, 1, 1),
};

static struct zhihe_pll_rate_table a210_c920pll_tbl[] = {
	PLL_RATE(1500000000U, 1500000000U, 2, 125, 0, 1, 1),
	PLL_RATE(1698000000U, 1698000000U, 4, 283, 0, 1, 1),
	PLL_RATE(1896000000U, 1896000000U, 1, 79, 0, 1, 1),
	PLL_RATE(2298000000U, 2298000000U, 4, 383, 0, 1, 1),
};

static struct zhihe_clk_info_pll plls_top[] = {
	PLL_PARAM(TEE_PLL, ZHIHE_PLL_VCO, a210_teepll_tbl,
		  0x160, 0x170, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "tee_pll_foutvco_frequency"),
	PLL_PARAM(DPU2_PLL, ZHIHE_PLL_VCO, a210_dpu2pll_tbl,
		  0x120, 0x130, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "dpu2_pll_foutvco_frequency"),
	PLL_PARAM(DPU1_PLL, ZHIHE_PLL_VCO, a210_dpu1pll_tbl,
		  0x100, 0x110, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "dpu1_pll_foutvco_frequency"),
	PLL_PARAM(DPU0_PLL, ZHIHE_PLL_VCO, a210_dpu0pll_tbl,
		  0x80, 0x90, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "dpu0_pll_foutvco_frequency"),
	PLL_PARAM(DVFS_PLL, ZHIHE_PLL_VCO, a210_dvfspll_tbl,
		  0x60, 0x70, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "dvfs_pll_foutvco_frequency"),
	PLL_PARAM(AUDIO0_PLL, ZHIHE_PLL_VCO, a210_audio0pll_tbl,
		  0x0, 0x10, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_FRAC, "audio0_pll_foutvco_frequency"),
	PLL_PARAM(AUDIO1_PLL, ZHIHE_PLL_VCO, a210_audio1pll_tbl,
		  0x20, 0x30, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_FRAC, "audio1_pll_foutvco_frequency"),
	PLL_PARAM(GMAC_PLL, ZHIHE_PLL_VCO, a210_gmacpll_tbl,
		  0x40, 0x50, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "gmac_pll_foutvco_frequency"),
	PLL_PARAM(VIDEO_PLL, ZHIHE_PLL_VCO, a210_videopll_tbl,
		  0x140, 0x150, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, "video_pll_foutvco_frequency"),
	PLL_PARAM(C908_PLL, ZHIHE_PLL_VCO, a210_c908pll_tbl,
		  0x0, 0x10, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, ""),
	PLL_PARAM(C920_PLL, ZHIHE_PLL_VCO, a210_c920pll_tbl,
		  0x40, 0x50, BIT(0), BIT(31),
		  BIT(30), PLL_MODE_INT, ""),
};

static struct zhihe_clk_reg regs_top[] = {
	REG_NAMED(PLL_WRAP, "pll-wrap"),
	REG_NAMED(TOP_CRG, "top-crg"),
	REG_NAMED(CPU_SS_CLK_SYSREG, "cpu-ss-clk-sysreg"),
	REG_NAMED(CPU_SS_CPU_PLL, "cpu-ss-cpu-pll"),
	REG_NAMED(DDR0_SYSREG, "ddr0-sysreg"),
	REG_NAMED(DDR1_SYSREG, "ddr1-sysreg"),
	REG_NAMED(SLC_DUAL_SYSREG, "slc-dual-sysreg"),
	REG_NAMED(TOP_CRG_T, "top-crg-t"),
	REG_NAMED(CPU_SS_CCU, "cpu-ss-ccu"),
};

static struct zhihe_clk_reg regs_gpu[] = {
	REG_NAMED(GPU_SS_PWRAP_CLK_EN, "gpu-ss-pwrap-clk-en"),
	REG_NAMED(GPU_SS_TOP_CLK_EN, "gpu-ss-top-clk-en"),
};

static struct zhihe_clk_reg regs_pcie[] = {
	REG_NAMED(PCIE_CLK_EN, "pcie-clk-en"),
};

static struct zhihe_clk_reg regs_usb[] = {
	REG_NAMED(USB_CLK_EN, "usb-clk-en"),
};

static struct zhihe_clk_reg regs_vi[] = {
	REG_NAMED(VI_CLK, "vi-clk"),
	REG_NAMED(VI_MISC_CTRL, "vi-misc-ctrl"),
};

static struct zhihe_clk_reg regs_vp[] = {
	REG_NAMED(VP_CLK, "vp-clk"),
};

static struct zhihe_clk_reg regs_vo[] = {
	REG_NAMED(VO_CLK, "vo-clk"),
	REG_NAMED(VO_PATH_CTRL, "vo-path-ctrl"),
};

static struct zhihe_clk_reg regs_npu[] = {
	REG_NAMED(NPU_CLK, "npu-clk"),
	REG_NAMED(NPU_TOP_CLK, "npu-top-clk"),
};

static struct zhihe_clk_reg regs_peri[] = {
	REG_NAMED(PERI0_SYSREG, "peri0-sysreg"),
	REG_NAMED(PERI1_SYSREG, "peri1-sysreg"),
	REG_NAMED(PERI2_SYSREG, "peri2-sysreg"),
	REG_NAMED(PERI3_SYSREG, "peri3-sysreg"),
	REG_NAMED(TEE_CRG, "tee-crg"),
};

static struct zhihe_clk_info info_top[] = {
	/* FIXED */
	FIXED(AON_OSC_CLK_PHY, "aon_osc_clk_phy", "osc-24m", 24000000),
	FIXED(AON_OSC_CLK_LOGIC, "aon_osc_clk_logic", "osc-24m", 24000000),
	FIXED(TOP_PLL_REF_CLK, "top_pll_ref_clk", "osc-24m", 24000000),
	FIXED(PLL_OSC_CLK, "pll_osc_clk", "osc-24m", 24000000),
	FIXED(AON_RTC_CLK, "aon_rtc_clk", "osc-24m", 32768),
	/* PLL */
	PLL(AUDIO0_PLL_FOUTVCO,
	    "audio0_pll_foutvco", "osc-24m", PLL_WRAP, 0, &plls_top[AUDIO0_PLL]),
	FIXED_FACTOR(AUDIO0_PLL_FOUTPOSTDIV,
		     "audio0_pll_foutpostdiv", "audio0_pll_foutvco", 1, 2),
	FIXED_FACTOR(AUDIO0_PLL_FOUT2,
		     "audio0_pll_fout2", "audio0_pll_foutvco", 1, 8),
	PLL(AUDIO1_PLL_FOUTVCO,
	    "audio1_pll_foutvco", "osc-24m", PLL_WRAP, 0, &plls_top[AUDIO1_PLL]),
	FIXED_FACTOR(AUDIO1_PLL_FOUTPOSTDIV,
		     "audio1_pll_foutpostdiv", "audio1_pll_foutvco", 1, 2),
	FIXED_FACTOR(AUDIO1_PLL_FOUT2,
		     "audio1_pll_fout2", "audio1_pll_foutvco", 1, 8),
	PLL(C908_PLL_FOUTVCO,
	    "c908_pll_foutvco", "osc-24m", CPU_SS_CPU_PLL, 0, &plls_top[C908_PLL]),
	PLL(C920_PLL_FOUTVCO,
	    "c920_pll_foutvco", "osc-24m", CPU_SS_CPU_PLL, 0, &plls_top[C920_PLL]),
	PLL(VIDEO_PLL_FOUTVCO,
	    "video_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[VIDEO_PLL]),
	FIXED_FACTOR(VIDEO_PLL_FOUTPOSTDIV,
		     "video_pll_foutpostdiv", "video_pll_foutvco", 1, 2),
	FIXED_FACTOR(VIDEO_PLL_FOUT1PH0,
		     "video_pll_fout1ph0", "video_pll_foutvco", 1, 4),
	FIXED_FACTOR(VIDEO_PLL_FOUT3,
		     "video_pll_fout3", "video_pll_foutvco", 1, 12),
	PLL(GMAC_PLL_FOUTVCO,
	    "gmac_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[GMAC_PLL]),
	FIXED_FACTOR(GMAC_PLL_FOUTPOSTDIV,
		     "gmac_pll_foutpostdiv", "gmac_pll_foutvco", 1, 3),
	FIXED_FACTOR(GMAC_PLL_FOUT1PH0,
		     "gmac_pll_fout1ph0", "gmac_pll_foutvco", 1, 6),
	PLL(DVFS_PLL_FOUTVCO,
	    "dvfs_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[DVFS_PLL]),
	PLL(DPU0_PLL_FOUTVCO,
	    "dpu0_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[DPU0_PLL]),
	FIXED_FACTOR(DPU0_PLL_FOUTPOSTDIV,
		     "dpu0_pll_foutpostdiv", "dpu0_pll_foutvco", 1, 2),
	PLL(DPU1_PLL_FOUTVCO,
	    "dpu1_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[DPU1_PLL]),
	FIXED_FACTOR(DPU1_PLL_FOUTPOSTDIV,
		     "dpu1_pll_foutpostdiv", "dpu1_pll_foutvco", 1, 2),
	PLL(DPU2_PLL_FOUTVCO,
	    "dpu2_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[DPU2_PLL]),
	FIXED_FACTOR(DPU2_PLL_FOUTPOSTDIV,
		     "dpu2_pll_foutpostdiv", "dpu2_pll_foutvco", 1, 2),
	PLL(TEE_PLL_FOUTVCO,
	    "tee_pll_foutvco", "top_pll_ref_clk", PLL_WRAP, 0, &plls_top[TEE_PLL]),
	/* TOP SS */
	DIV(TOP_CFG_ACLK_DIV,
	    "top_cfg_aclk", "video_pll_fout1ph0", TOP_CRG, 0, 8, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 255),
	DIV(TOP_PCLK_DIV,
	    "top_pclk", "video_pll_fout1ph0", TOP_CRG, 0, 0, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 4, 255),
	DIV(AMUX_CLK_DIV,
	    "top_amux_clk_div", "video_pll_foutvco", TOP_CRG, 0x18, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 4, 15),
	GATE(SW_TOP_OSC_CLK_LOGIC_EN,
	     "top_osc_clk_logic", "aon_osc_clk_logic", TOP_CRG, 0x200, 2),
	GATE(SW_AMUX_660_CLK_EN,
	     "top_amux_clk", "top_amux_clk_div", TOP_CRG, 0x200, 0),
	/* TOP SS - IOMMU_PTW_ACLK */
	GATE(SW_IOMMU_PTW_330_ACLK_EN,
	     "iommu_ptw_aclk", "iommu_ptw_aclk_div", TOP_CRG, 0x200, 1),
	DIV(IOMMU_PTW_ACLK_DIV,
	    "iommu_ptw_aclk_div", "video_pll_fout1ph0", TOP_CRG, 0, 24, 3,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 7),
	/* TOP SS - CPU SS */
	DIV(TOP_CPUSYS_BUS_CLK_DIV,
	    "top_cpusys_bus_clk", "top_cpusys_bus_clk_mux", TOP_CRG, 0x8, 20, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	MUX(TOP_CPUSYS_BUS_CLK_MUX,
	    "top_cpusys_bus_clk_mux", TOP_CRG, 0x8, 16, 2,
		top_cpusys_bus_clk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_CPUSYS_PIC_CLK_DIV,
	    "top_cpusys_pic_clk", "top_cpusys_pic_clk_mux", TOP_CRG, 0x4, 20, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_CPUSYS_PIC_CLK_MUX,
	    "top_cpusys_pic_clk_mux", TOP_CRG, 0x4, 3, 1,
		top_cpusys_pic_clk_mux_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_CPU_BAK_PLL0_CLK_MUX,
	    "top_cpu_bak_pll0_clk", TOP_CRG, 0x4, 0, 3,
		top_cpu_bak_pll0_clk_mux_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_CPU_BAK_PLL0_CLK_MUX0,
	    "top_cpu_bak_pll0_clk_mux0", TOP_CRG, 0x4, 16, 2,
		top_cpu_bak_pll0_clk_mux0_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_CPU_BAK_PLL0_CLK_DIV0,
	    "top_cpu_bak_pll0_clk_div0", "gmac_pll_foutvco", TOP_CRG, 0x4, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_CPU_BAK_PLL0_CLK_DIV1,
	    "top_cpu_bak_pll0_clk_div1", "gmac_pll_foutvco", TOP_CRG, 0x4, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	DIV(TOP_CPU_BAK_PLL0_CLK_DIV3,
	    "top_cpu_bak_pll0_clk_div3", "audio1_pll_foutvco", TOP_CRG, 0x4, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_CPU_BAK_PLL0_CLK_DIV4,
	    "top_cpu_bak_pll0_clk_div4", "video_pll_foutvco", TOP_CRG, 0x4, 24, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 255),
	MUX(TOP_CPU_BAK_PLL1_CLK_MUX,
	    "top_cpu_bak_pll1_clk", TOP_CRG, 0x8, 0, 3,
		top_cpu_bak_pll1_clk_mux_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_CPU_BAK_PLL1_CLK_MUX0,
	    "top_cpu_bak_pll1_clk_mux0", TOP_CRG, 0x8, 8, 2,
		top_cpu_bak_pll1_clk_mux0_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_CPU_BAK_PLL1_CLK_DIV0,
	    "top_cpu_bak_pll1_clk_div0", "gmac_pll_foutvco", TOP_CRG, 0x8, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_CPU_BAK_PLL1_CLK_DIV1,
	    "top_cpu_bak_pll1_clk_div1", "gmac_pll_foutvco", TOP_CRG, 0x8, 24, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	DIV(TOP_CPU_BAK_PLL1_CLK_DIV3,
	    "top_cpu_bak_pll1_clk_div3", "audio1_pll_foutvco", TOP_CRG, 0x8, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_CPU_BAK_PLL1_CLK_DIV4,
	    "top_cpu_bak_pll1_clk_div4", "video_pll_foutvco", TOP_CRG, 0x44, 0, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 255),
	DIV(TOP_CPUSYS_CFG_ACLK_DIV,
	    "top_cpusys_sys_cfg_axi_clk", "video_pll_fout1ph0", CPU_SS_CCU, 0x1c, 0, 16,
		NO_DIV_EN, MUX_TYPE_CDE, 1, 5),
	DIV(TOP_CPUSYS_COM_APB_CLK_DIV,
	    "top_cpusys_sys_com_apb_clk", "video_pll_fout1ph0", CPU_SS_CCU, 0x21c, 0, 16,
		NO_DIV_EN, MUX_TYPE_CDE, 3, 7),
	DIV(TOP_CPUSYS_APB_CLK_DIV,
	    "top_cpusys_cpu_apb_clk", "video_pll_fout1ph0", CPU_SS_CCU, 0x41c, 0, 16,
		NO_DIV_EN, MUX_TYPE_CDE, 3, 7),
	/* TOP SS - NOC_CCLK */
	GATE(SW_NOC_CCLK_EN,
	     "noc_cclk", "noc_cclk_div", TOP_CRG, 0x200, 3),
	DIV(NOC_CCLK_DIV,
	    "noc_cclk_div", "noc_cclk_mux", TOP_CRG, 0, 20, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(NOC_CCLK_MUX,
	    "noc_cclk_mux", TOP_CRG, 0, 16, 2,
		noc_cclk_mux_parents, CLK_SET_RATE_PARENT),
	/* TOP SS - DDR SS */
	MUX(TOP_CPU_DDR1_ACLK_MUX,
	    "top_ddr1_aclk", TOP_CRG, 0x4, 19, 1,
		top_cpu_ddr1_aclk_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_CPU_DDR0_ACLK_MUX,
	    "top_ddr0_aclk", TOP_CRG, 0x4, 18, 1,
		top_cpu_ddr0_aclk_parents, CLK_SET_RATE_PARENT),
	FIXED_FACTOR(DDR_CBUS2DDR_ACLK0,
		     "cbus2ddr_aclk0", "ddr_pll_clk_en", 1, 1),
	FIXED_FACTOR(DDR_CBUS2DDR_ACLK1,
		     "cbus2ddr_aclk1", "ddr_pll_clk_en", 1, 1),
	FIXED(DDR_PLL_FOUTVCO,
	      "ddr_pll_foutvco", "pll_osc_clk", 3192000000),
	FIXED_FACTOR(DDR_PLL_FOUTPOSTDIV,
		     "ddr_pll_foutpostdiv", "ddr_pll_foutvco", 1, 3),
	FIXED_FACTOR(DDR_PLL_FOUT1PH0,
		     "ddr_pll_fout1ph0", "ddr_pll_foutvco", 1, 6),
	FIXED_FACTOR(DDR_PLL_FOUT2,
		     "ddr_pll_fout2", "ddr_pll_foutvco", 1, 12),
	FIXED_FACTOR(DDR_PLL_FOUT4,
		     "ddr_pll_fout4", "ddr_pll_foutvco", 1, 24),
	/* TOP SS - D2D SS */
	FIXED_FACTOR(D2D_PHY_REF_CLK_100M,
		     "d2d_phy_ref_clk_100M", "gmac_pll_foutpostdiv", 1, 10),
	MUX(TOP_D2D_REF_CLK_MUX,
	    "d2d_phy_ref_clk", TOP_CRG, 0x14, 15, 1,
		d2d_phy_ref_clk_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_D2D_SCAN_CLK0_DIV,
	    "d2d_phy_scan_clk0", "dpu1_pll_foutvco", TOP_CRG, 0x14, 12, 3,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 7),
	DIV(TOP_D2D_SCAN_CLK1_DIV,
	    "d2d_phy_scan_clk1", "dpu1_pll_foutvco", TOP_CRG, 0x14, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	FIXED_FACTOR(D2D_PHY_SCAN_CLK2,
		     "d2d_phy_scan_clk2", "dpu2_pll_foutpostdiv", 1, 1),
	DIV(TOP_D2D_ACLK_DIV,
	    "top_d2d_aclk", "top_d2d_aclk_mux", TOP_CRG, 0x14, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_D2D_ACLK_MUX,
	    "top_d2d_aclk_mux", TOP_CRG, 0x14, 0, 2,
		top_d2d_aclk_mux_parents, CLK_SET_RATE_PARENT),
	/* TOP SS - GPU SS */
	DIV(TOP_GPU_CORE_CLK_DIV,
	    "top_gpu_core_clk", "top_gpu_core_clk_mux", TOP_CRG, 0x18, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_GPU_CORE_CLK_MUX,
	    "top_gpu_core_clk_mux", TOP_CRG, 0x18, 0, 2,
		top_gpu_core_clk_mux_parents, CLK_SET_RATE_PARENT),
	/* TOP SS - PCIE SS */
	DIV(TOP_PCIE_SCAN_REF_CLK0_DIV,
	    "scan_ref_clk0", "dpu1_pll_foutvco", TOP_CRG, 0x1c, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 15),
	DIV(TOP_PCIE_SCAN_REF_CLK1_DIV,
	    "scan_ref_clk1", "gmac_pll_foutvco", TOP_CRG, 0x1c, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	DIV(TOP_PCIE_AXI_M_ACLK_DIV,
	    "pcie_ss_axi_m_aclk", "pcie_ss_axi_m_aclk_mux", TOP_CRG, 0x1c, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_PCIE_AXI_M_ACLK_MUX,
	    "pcie_ss_axi_m_aclk_mux", TOP_CRG, 0x1c, 0, 1,
		pcie_ss_axi_m_aclk_mux_parents, CLK_SET_RATE_PARENT),
	/* TOP SS - PERI SS */
	DIV(TOP_PERI_SPI_SSI_CLK0_DIV,
	    "peri1_spi_ssi_clk", "audio1_pll_foutvco", TOP_CRG, 0x20, 20, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 255),
	DIV(TOP_PERI_MST_ACLK0_DIV,
	    "peri1_mst_aclk", "video_pll_fout1ph0", TOP_CRG, 0x20, 16, 3,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 7),
	FIXED_FACTOR(TOP_PERI_TIMER_CLK_100M,
		     "peri0_timer_clk_100M", "gmac_pll_foutpostdiv", 1, 10),
	MUX(TOP_PERI_TIMER_CLK_MUX,
	    "peri0_timer_clk", TOP_CRG, 0x20, 15, 1,
		peri0_timer_clk_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_PERI_I2S_8CH0_SRC_CLK_MUX,
	    "peri2_i2s3_src_clk", TOP_CRG, 0x28, 21, 1,
		peri2_i2s3_src_clk_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_PERI_I2S_2CH0_SRC_CLK_MUX,
	    "peri1_i2s0_src_clk", TOP_CRG, 0x20, 13, 1,
		peri1_i2s0_src_clk_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_PERI_I2S_2CH1_SRC_CLK_MUX,
	    "peri2_i2s1_src_clk", TOP_CRG, 0x20, 14, 1,
		peri2_i2s1_src_clk_parents, CLK_SET_RATE_PARENT),
	MUX(TOP_PERI_I2S_2CH2_SRC_CLK_MUX,
	    "peri2_i2s2_src_clk", TOP_CRG, 0x28, 23, 1,
		peri2_i2s2_src_clk_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_PERI_QSPI0_SSI_CLK_DIV0,
	    "peri1_qspi_ssi_clk_div0", "video_pll_foutvco", TOP_CRG, 0x20, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_PERI_QSPI0_SSI_CLK_DIV1,
	    "peri1_qspi_ssi_clk_div1", "audio1_pll_foutvco", TOP_CRG, 0x20, 0, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 255),
	MUX(TOP_PERI_QSPI_SSI_CLK_MUX0,
	    "peri1_qspi_ssi_clk", TOP_CRG, 0x20, 12, 1,
		peri1_qspi_ssi_clk_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_PERI_SPI_SSI_CLK1_DIV,
	    "peri2_spi_ssi_clk", "audio1_pll_foutvco", TOP_CRG, 0x24, 24, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 255),
	DIV(TOP_PERI_HIRES_CLK0_DIV,
	    "peri1_hires_clk", "video_pll_foutvco", TOP_CRG, 0x24, 16, 6,
		NO_DIV_EN, MUX_TYPE_DIV, 33, 63),
	DIV(TOP_PERI_HIRES_CLK1_DIV,
	    "peri2_hires_clk", "video_pll_foutvco", TOP_CRG, 0x24, 0, 6,
		NO_DIV_EN, MUX_TYPE_DIV, 33, 63),
	MUX(TOP_PERI_TDM_SRC_CLK_MUX,
	    "peri1_tdm_src_clk", TOP_CRG, 0x28, 25, 1,
		peri1_tdm_src_clk_parents, CLK_SET_RATE_PARENT),
	FIXED_FACTOR(UART_SCLK_100M,
		     "uart_sclk", "gmac_pll_foutpostdiv", 1, 10),
	FIXED_FACTOR(PERI1_UART_SCLK,
		     "peri1_uart_sclk", "uart_sclk", 1, 1),
	FIXED_FACTOR(PERI2_UART_SCLK,
		     "peri2_uart_sclk", "uart_sclk", 1, 1),
	DIV(TOP_PERI_PDM_MCLK_DIV,
	    "peri1_pdm_mclk", "pdm_clk_mux", TOP_CRG, 0x2c, 24, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 10, 255),
	MUX(TOP_PERI_PDM_MCLK_MUX,
	    "pdm_clk_mux", TOP_CRG, 0x28, 22, 1,
		pdm_clk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_PERI_QSPI1_SSI_CLK_DIV0,
	    "peri2_qspi_ssi_clk_div0", "video_pll_foutvco", TOP_CRG, 0x28, 16, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 15),
	DIV(TOP_PERI_QSPI1_SSI_CLK_DIV1,
	    "peri2_qspi_ssi_clk_div1", "audio1_pll_foutvco", TOP_CRG, 0x28, 8, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 255),
	MUX(TOP_PERI_QSPI_SSI_CLK_MUX1,
	    "peri2_qspi_ssi_clk", TOP_CRG, 0x28, 20, 1,
		peri2_qspi_ssi_clk_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_PERI_MST_CLK1_DIV,
	    "peri3_mst_aclk", "video_pll_foutvco", TOP_CRG, 0x28, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 15),
	DIV(TOP_PERI_EMMC_REF_CLK_DIV,
	    "emmc_ref_clk", "emmc_ref_clk_mux", TOP_CRG, 0x28, 0, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_PERI_EMMC_REF_CLK_MUX,
	    "emmc_ref_clk_mux", TOP_CRG, 0x2c, 20, 1,
		emmc_ref_clk_mux_parents, CLK_SET_RATE_PARENT),
	FIXED_FACTOR(PERI1_PWM_CLK,
		     "peri1_pwm_clk", "gmac_pll_fout1ph0", 1, 10),
	FIXED_FACTOR(PERI2_PWM_CLK,
		     "peri2_pwm_clk", "gmac_pll_fout1ph0", 1, 10),
	FIXED_FACTOR(I2C_IC_CLK,
		     "i2c_ic_clk", "gmac_pll_foutpostdiv", 1, 10),
	MUX(TOP_PAD_SENSOR_VCLK_MUX0,
	    "pad_sensor_vclk0_mux", TOP_CRG, 0x2c, 16, 2,
		pad_sensor_vclk0_mux_parents,
		CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT),
	DIV(TOP_PAD_SENSOR_VCLK0_DIV,
	    "top_pad_sensor_vclk0_div", "pad_sensor_vclk0_mux", TOP_CRG, 0x2c, 0, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 16, 255),
	MUX(TOP_PAD_SENSOR_VCLK_MUX1,
	    "pad_sensor_vclk1_mux", TOP_CRG, 0x2c, 18, 2,
		pad_sensor_vclk1_mux_parents,
		CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT),
	DIV(TOP_PAD_SENSOR_VCLK1_DIV,
	    "top_pad_sensor_vclk1_div", "pad_sensor_vclk1_mux", TOP_CRG, 0x2c, 8, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 16, 255),
	MUX(TOP_TEE_CLK_MUX,
	    "tee_clk_mux", TOP_CRG, 0x30, 0, 1,
		tee_clk_mux_parents, CLK_SET_RATE_PARENT),
	DIV_CLOSEST(TOP_TEE_CLK_DIV,
		    "tee_clk", "tee_clk_mux", TOP_CRG, 0x30, 4, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 7, 255),
	DIV(TOP_USB_BUS_ACLK_DIV,
	    "clkgen_usb_ss_bus_clk", "video_pll_fout1ph0", TOP_CRG, 0x34, 8, 3,
		NO_DIV_EN, MUX_TYPE_DIV, 2, 7),
	DIV(TOP_USB_DP_AUX_CLK_DIV,
	    "clkgen_usb_ss_aux_clk", "video_pll_foutvco", TOP_CRG, 0x34, 0, 8,
		NO_DIV_EN, MUX_TYPE_DIV, 165, 255),
	DIV(TOP_VP_ACLK_DIV,
	    "vp_aclk", "vp_aclk_mux", TOP_CRG, 0x38, 24, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_VP_ACLK_MUX,
	    "vp_aclk_mux", TOP_CRG, 0x38, 28, 1,
		vp_aclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_VP_G2D_CCLK_DIV,
	    "g2d_cclk", "g2d_cclk_mux", TOP_CRG, 0x38, 20, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_VP_G2D_CCLK_MUX,
	    "g2d_cclk_mux", TOP_CRG, 0x38, 16, 2,
		g2d_cclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_VP_VDEC_CCLK_DIV,
	    "vdec_cclk", "vdec_cclk_mux", TOP_CRG, 0x38, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_VP_VDEC_CCLK_MUX,
	    "vdec_cclk_mux", TOP_CRG, 0x38, 8, 2,
		vdec_cclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_VP_VENC_CCLK_DIV,
	    "venc_cclk", "venc_cclk_mux", TOP_CRG, 0x38, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 5, 15),
	MUX(TOP_VP_VENC_CCLK_MUX,
	    "venc_cclk_mux", TOP_CRG, 0x38, 0, 2,
		venc_cclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_NPU_ACLK_DIV,
	    "npu_aclk", "npu_aclk_mux", TOP_CRG, 0x3c, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_NPU_ACLK_MUX,
	    "npu_aclk_mux", TOP_CRG, 0x3c, 8, 2,
		npu_aclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_NPU_CCLK_DIV,
	    "npu_cclk", "npu_cclk_mux", TOP_CRG, 0x3c, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	MUX(TOP_NPU_CCLK_MUX,
	    "npu_cclk_mux", TOP_CRG, 0x3c, 0, 2,
		npu_cclk_mux_parents, CLK_SET_RATE_PARENT),
	DIV(TOP_USB_USB20_SCAN_REF_CLK_DIV,
	    "clkgen_usb20phy_scan_ref_clk", "dpu1_pll_foutvco", TOP_CRG, 0x40, 16, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 5, 15),
	DIV(TOP_USB_SCAN_REF_CLK3_DIV,
	    "clkgen_c10phy_scan_ref_clk3", "dpu1_pll_foutvco", TOP_CRG, 0x40, 12, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 3, 15),
	DIV(TOP_USB_SCAN_REF_CLK2_DIV,
	    "clkgen_c10phy_scan_ref_clk2", "gmac_pll_foutvco", TOP_CRG, 0x40, 8, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 5, 15),
	DIV(TOP_USB_SCAN_REF_CLK1_DIV,
	    "clkgen_c10phy_scan_ref_clk1", "gmac_pll_foutvco", TOP_CRG, 0x40, 4, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 15),
	DIV(TOP_USB_SCAN_REF_CLK0_DIV,
	    "clkgen_c10phy_scan_ref_clk0", "dpu1_pll_foutvco", TOP_CRG, 0x40, 0, 4,
		NO_DIV_EN, MUX_TYPE_DIV, 6, 15),
	GATE(SW_TOP_PAD_SENSOR_VCLK0_EN,
	     "top_pad_sensor_vclk0", "top_pad_sensor_vclk0_div", TOP_CRG, 0x200, 4),
	GATE(SW_TOP_PAD_SENSOR_VCLK1_EN,
	     "top_pad_sensor_vclk1", "top_pad_sensor_vclk1_div", TOP_CRG, 0x200, 5),
	/* CPU SS */
	MUX(C908_CPU_TO_CDE_CLK_MUX,
	    "c908_cpu_clk", CPU_SS_CLK_SYSREG, 0x0, 0, 1,
		c908_cpu_clk_parents, CLK_SET_RATE_PARENT),
	MUX(C920_CPU_TO_CDE_CLK_MUX,
	    "c920_cpu_clk", CPU_SS_CLK_SYSREG, 0x4, 0, 1,
		c920_cpu_clk_parents, CLK_SET_RATE_PARENT),
	/* DDR SS */
	GATE(DDR0_PHY_DFICLK_EN,
	     "ddr0_phy_dficlk", "ddr_pll_foutpostdiv", DDR0_SYSREG, 0x8, 3),
	GATE(DDR0_DDRC_ACLK_EN,
	     "ddr0_aclk", "ddr_pll_foutpostdiv", DDR0_SYSREG, 0x8, 2),
	GATE(DDR0_DDRC_CCLK_EN,
	     "ddr0_cclk", "ddr_pll_foutpostdiv", DDR0_SYSREG, 0x8, 1),
	GATE(DDR0_SBR_CLK_EN,
	     "ddr0_sbr_clk", "ddr_pll_foutpostdiv", DDR0_SYSREG, 0x8, 0),
	GATE(DDR1_PHY_DFICLK_EN,
	     "ddr1_phy_dficlk", "ddr_pll_foutpostdiv", DDR1_SYSREG, 0x8, 3),
	GATE(DDR1_DDRC_ACLK_EN,
	     "ddr1_aclk", "ddr_pll_foutpostdiv", DDR1_SYSREG, 0x8, 2),
	GATE(DDR1_DDRC_CCLK_EN,
	     "ddr1_cclk", "ddr_pll_foutpostdiv", DDR1_SYSREG, 0x8, 1),
	GATE(DDR1_SBR_CLK_EN,
	     "ddr1_sbr_clk", "ddr_pll_foutpostdiv", DDR1_SYSREG, 0x8, 0),
	GATE_SHARED(DDR_PLL_CLK_EN,
		    "ddr_pll_clk_en", "ddr_pll_clkout", SLC_DUAL_SYSREG, 0x18, 16,
			&share_cnt_ddr_pll_clk_en),
	MUX(DDR_PLL_CLK_SEL,
	    "ddr_pll_clkout", SLC_DUAL_SYSREG, 0x18, 17, 2,
		ddr_pll_clkout_parents, CLK_SET_RATE_PARENT),
};

static struct zhihe_clk_info info_gpu[] = {
	FIXED_FACTOR(GPU_TOP_SYS_CLK,
		     "gpu_top_sys_clk", "top_cfg_aclk", 1, 1),
	FIXED_FACTOR(GPU_PCLK_CDT,
		     "gpu_pclk_cdt", "gpu_top_sys_clk", 1, 2),
	GATE(SW_PWR_WRAP_DFMU_PCLK_EN,
	     "gpu_top_pclk", "gpu_pclk_cdt", GPU_SS_PWRAP_CLK_EN, 0, 7),
	GATE(SW_PWR_WRAP_GPU_SYS_CLK_EN,
	     "gpu_top_aclk", "gpu_top_sys_clk", GPU_SS_PWRAP_CLK_EN, 0, 3),
	GATE(SW_PWR_WRAP_DFMU_CORE_CLK_EN,
	     "gpu_dfmu_core_clk", "top_cfg_aclk", GPU_SS_PWRAP_CLK_EN, 0, 1),
	GATE(SW_PWR_WRAP_GPU_CORE_CLK_EN,
	     "gpu_core_clk", "top_gpu_core_clk", GPU_SS_PWRAP_CLK_EN, 0, 0),
};

static struct zhihe_clk_info info_pcie[] = {
	FIXED_FACTOR(PCIE_SS_APB_CLK,
		     "pcie_ss_apb_clk", "gmac_pll_foutpostdiv", 1, 10),
	GATE(E16PHY_PCLK_EN,
	     "e16phy_apbs_pclk", "pcie_ss_apb_clk", PCIE_CLK_EN, 0x8, 0),
	GATE(SATA_PMALIVE_CLK_EN,
	     "sata_pmalive_clk", "aon_osc_clk_logic", PCIE_CLK_EN, 0x14, 0),
	GATE(SATA_RXOOB0_CLK_EN,
	     "sata_rxoob0_clk", "pcie_ss_apb_clk", PCIE_CLK_EN, 0x14, 16),
	GATE(SATA_RXOOB1_CLK_EN,
	     "sata_rxoob1_clk", "pcie_ss_apb_clk", PCIE_CLK_EN, 0x14, 20),
	GATE(SATA_GEN3X2_ACLK_EN,
	     "sata_gen3x2_aclk", "pcie_ss_axi_m_aclk", PCIE_CLK_EN, 0x10, 8),
	GATE(PCIE_DM_GEN3X4_AUX_CLK_EN,
	     "pcie_dm_gen3x4_aux_clk", "aon_osc_clk_logic", PCIE_CLK_EN, 0x20, 12),
	GATE(PCIE_DM_GEN3X4_SLV_ACLK_EN,
	     "pcie_dm_gen3x4_slv_aclk", "top_cfg_aclk", PCIE_CLK_EN, 0x20, 0),
	GATE(PCIE_DM_GEN3X4_MST_ACLK_EN,
	     "pcie_dm_gen3x4_mst_aclk", "pcie_ss_axi_m_aclk", PCIE_CLK_EN, 0x20, 8),
	GATE(PCIE_DM_GEN3X4_PCLK_EN,
	     "pcie_dm_gen3x4_pclk", "pcie_ss_apb_clk", PCIE_CLK_EN, 0x20, 4),
	GATE(PCIE_RP_GEN3X1_AUX_CLK_EN,
	     "pcie_rp_gen3x1_aux_clk", "aon_osc_clk_logic", PCIE_CLK_EN, 0x24, 12),
	GATE(PCIE_RP_GEN3X1_SLV_ACLK_EN,
	     "pcie_rp_gen3x1_slv_aclk", "top_cfg_aclk", PCIE_CLK_EN, 0x24, 0),
	GATE(PCIE_RP_GEN3X1_MST_ACLK_EN,
	     "pcie_rp_gen3x1_mst_aclk", "pcie_ss_axi_m_aclk", PCIE_CLK_EN, 0x24, 8),
	GATE(PCIE_RP_GEN3X1_PCLK_EN,
	     "pcie_rp_gen3x1_pclk", "pcie_ss_apb_clk", PCIE_CLK_EN, 0x24, 4),
};

static struct zhihe_clk_info info_usb[] = {
	GATE(DPTX_I2S_CLK_EN,
	     "usb_dptx_i2s_clk", "audio0_pll_foutvco", USB_CLK_EN, 0x4, 16),
	GATE(DPTX_IPI_CLK_EN,
	     "usb_dptx_ipi_clk", "usb_ss_gtc_clk", USB_CLK_EN, 0x4, 12),
	GATE(DPTX_AUX_CLK_EN,
	     "usb_dptx_aux_clk", "clkgen_usb_ss_aux_clk", USB_CLK_EN, 0x4, 8),
	FIXED_FACTOR(USB_SS_GTC_CLK,
		     "usb_ss_gtc_clk", "gmac_pll_foutpostdiv", 1, 10),
	GATE(DPTX_GTC_CLK_EN,
	     "usb_dptx_gtc_clk", "usb_ss_gtc_clk", USB_CLK_EN, 0x4, 4),
	GATE(DPTX_PCLK_EN,
	     "usb_dptx_pclk", "usb_ss_gtc_clk", USB_CLK_EN, 0x4, 0),
	GATE(USB31_REF_CLK_EN,
	     "usb31_ref_clk", "aon_osc_clk_logic", USB_CLK_EN, 0x8, 16),
	GATE(USB31_SLV_AFENCE_ACLK_EN,
	     "usb31_slv_afence_aclk", "aon_osc_clk_logic", USB_CLK_EN, 0x8, 20),
	FIXED_FACTOR(USB_SS_PAD_OSC_CLK_1M,
		     "usb_ss_pad_osc_clk_1m", "aon_osc_clk_logic", 1, 24),
	GATE(USB31_SUSPEND_CLK_EN,
	     "usb31_suspend_clk", "usb_ss_pad_osc_clk_1m", USB_CLK_EN, 0x8, 8),
	GATE(USB31_BUS_CLK_EN,
	     "usb31_bus_clk", "clkgen_usb_ss_bus_clk", USB_CLK_EN, 0x8, 4),
	GATE(USB31_PCLK_EN,
	     "usb31_pclk", "usb_ss_gtc_clk", USB_CLK_EN, 0x8, 0),
	GATE(C10PHY_SUSPEND_CLK_EN,
	     "usb_c10phy_suspend_clk", "usb_ss_pad_osc_clk_1m", USB_CLK_EN, 0xc, 4),
	GATE(C10PHY_PCLK_EN,
	     "usb_c10phy_pclk", "usb_ss_gtc_clk", USB_CLK_EN, 0xc, 0),
	FIXED_FACTOR(USB20_CORE_CLK,
		     "usb20_core_clk", "aon_osc_clk_logic", 1, 1),
	GATE(USB_SS_PERI2_CFG_ACLK_EN,
	     "usb_peri2_cfg_aclk", "aon_osc_clk_logic", USB_CLK_EN, 0x20, 4),
	GATE(USB20_BUS_CLK,
	     "usb20_bus_clk", "clkgen_usb_ss_bus_clk", USB_CLK_EN, 0x10, 24),
};

static struct zhihe_clk_info info_vi[] = {
	GATE(VI_VSE_CLK_EN,
	     "dw200_vseclk", "vi_pre_vse_clk_div", VI_CLK, 0, 29),
	GATE(VI_VSEOUT_CLK_EN,
	     "dw200_vseout_clk", "vi_pre_vse_clk_div", VI_CLK, 0, 22),
	DIV(VI_PRE_VSE_CLK_DIV_NUM,
	    "vi_pre_vse_clk_div", "gmac_pll_foutpostdiv", VI_CLK, 0xc, 4, 4,
		25, MUX_TYPE_DIV, 1, 15),
	GATE(VI_DWE_CLK_EN,
	     "dw200_dweclk", "vi_pre_dwe_clk_div", VI_CLK, 0, 28),
	GATE(VI_DW_SCLK_EN,
	     "dw_sclk", "vi_pre_dwe_clk_div", VI_CLK, 0, 30),
	DIV(VI_PRE_DWE_CLK_DIV_NUM,
	    "vi_pre_dwe_clk_div", "video_pll_foutvco", VI_CLK, 0xc, 8, 4,
		26, MUX_TYPE_DIV, 3, 15),
	DIV(VI_PRE_AXI_CLK_DIV_NUM,
	    "vi_preocc_aclk", "video_pll_foutvco", VI_CLK, 0xc, 0, 4,
		24, MUX_TYPE_DIV, 3, 15),
	GATE(VI_DW200_ACLK_EN,
	     "dw200_aclk", "vi_preocc_aclk", VI_CLK, 0, 27),
	GATE(VI_DW200_HCLK_EN,
	     "dw200_hclk", "top_cfg_aclk", VI_CLK, 0, 26),
	DIV(VI_PRE_DECOMP_CLK_DIV_NUM,
	    "vi_pre_decomp_clk", "audio0_pll_foutvco", VI_CLK, 0xc, 20, 4,
		29, MUX_TYPE_DIV, 4, 15),
	GATE(VI_DECOMP_CLK_EN,
	     "decomp_clk", "vi_pre_decomp_clk", VI_CLK, 0, 25),
	GATE(VI_DECOUT_CLK_EN,
	     "decout_clk", "vi_pre_decomp_clk", VI_CLK, 0, 21),
	DIV(VI_PRE_COMP_CLK_DIV_NUM,
	    "vi_pre_comp_clk", "audio0_pll_foutvco", VI_CLK, 0xc, 16, 4,
		28, MUX_TYPE_DIV, 4, 15),
	GATE(VI_COMP0_CLK_EN,
	     "comp0_clk", "vi_pre_comp_clk", VI_CLK, 0, 23),
	GATE(VI_COMP1_CLK_EN,
	     "comp1_clk", "vi_pre_comp_clk", VI_CLK, 0, 24),
	GATE(VI_COMP_ACLK_EN,
	     "comp_aclk", "vi_preocc_aclk", VI_CLK, 0, 19),
	GATE(VI_COMP_PCLK_EN,
	     "comp_pclk", "vi_pclk", VI_CLK, 0, 18),
	GATE(VI_MIPI0CSI0_PCLK_EN,
	     "mipi0_csi0_pclk", "vi_pclk", VI_CLK, 0, 0),
	GATE(VI_MIPI0CSI1_PCLK_EN,
	     "mipi0_csi1_pclk", "vi_pclk", VI_CLK, 0, 1),
	GATE(VI_MIPI1CSI0_PCLK_EN,
	     "mipi1_csi0_pclk", "vi_pclk", VI_CLK, 0, 6),
	GATE(VI_MIPI1CSI1_PCLK_EN,
	     "mipi1_csi1_pclk", "vi_pclk", VI_CLK, 0, 7),
	GATE(VI_VIPRE_PCLK_EN,
	     "vipre_pclk", "vi_pclk", VI_CLK, 0, 12),
	GATE(VI_MIPI0CSI0_FPCLK_EN,
	     "mipi0_csi0_fpclk", "vi_pclk", VI_CLK, 0x4, 8),
	GATE(VI_MIPI0CSI1_FPCLK_EN,
	     "mipi0_csi1_fpclk", "vi_pclk", VI_CLK, 0x4, 9),
	GATE(VI_MIPI1CSI0_FPCLK_EN,
	     "mipi1_csi0_fpclk", "vi_pclk", VI_CLK, 0x4, 10),
	GATE(VI_MIPI1CSI1_FPCLK_EN,
	     "mipi1_csi1_fpclk", "vi_pclk", VI_CLK, 0x4, 11),
	FIXED_FACTOR(VI_PCLK,
		     "vi_pclk", "top_cfg_aclk", 1, 2),
	MUX(VI_ISP_IF_REF_SWITCH_SEL,
	    "isp_if_ref_mux", VI_CLK, 0x8, 22, 2,
		isp_if_ref_mux_parents,
		CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT),
	GATE(VI_VIPRE_SCLK_EN,
	     "vipre_sclk", "isp_if_ref_clk_div", VI_CLK, 0, 15),
	DIV_CLOSEST(VI_PRE_ISP_IF_CLK_DIV_NUM,
		    "isp_if_ref_clk_div", "isp_if_ref_mux", VI_CLK, 0x8, 11, 4,
		26, MUX_TYPE_DIV, 3, 15),
	GATE(VI_ISP_CLK_EN,
	     "isp_entire_clk", "video_pll_foutvco", VI_CLK, 0, 17),
	DIV(VI_PRE_ISP_CORE_CLK_DIV_NUM,
	    "isp_core_clk", "video_pll_foutvco", VI_CLK, 0x8, 16, 4,
		27, MUX_TYPE_DIV, 4, 15),
	GATE(VI_ISPOUT_CLK_EN,
	     "ispout_clk", "isp_core_clk", VI_CLK, 0, 20),
	GATE(VI_VIPRE_ACLK_EN,
	     "vipre_aclk", "vi_preocc_aclk", VI_CLK, 0, 16),
	GATE(VI_VIPRE_I0_PIXCLK_EN,
	     "vipre_m0if_clk", "vi_preocc_mipi0_ifclk", VI_CLK, 0, 13),
	GATE(VI_MIPI0_CSI0_PIXCLK_EN,
	     "mipi0_csi0if_clk", "vi_preocc_mipi0_ifclk", VI_CLK, 0, 4),
	GATE(VI_MIPI0_CSI1_PIXCLK_EN,
	     "mipi0_csi1if_clk", "vi_preocc_mipi0_ifclk", VI_CLK, 0, 5),
	DIV_CLOSEST(VI_PRE_MIPI0_PIXCLK_DIV_NUM,
		    "vi_preocc_mipi0_ifclk", "vi_preocc_mipi0_ifclk_mux", VI_CLK, 0x8, 0, 4,
		24, MUX_TYPE_DIV, 3, 15),
	MUX(VI_MIPI0_PIX_REF_SWITCH_SEL,
	    "vi_preocc_mipi0_ifclk_mux", VI_CLK, 0x8, 20, 1,
		vi_preocc_mipi0_ifclk_mux_parents,
		CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT),
	GATE(VI_VIPRE_I1_PIXCLK_EN,
	     "vipre_m1if_clk", "vi_preocc_mipi1_ifclk", VI_CLK, 0, 14),
	GATE(VI_MIPI1_CSI0_PIXCLK_EN,
	     "mipi1_csi0if_clk", "vi_preocc_mipi1_ifclk", VI_CLK, 0, 10),
	GATE(VI_MIPI1_CSI1_PIXCLK_EN,
	     "mipi1_csi1if_clk", "vi_preocc_mipi1_ifclk", VI_CLK, 0, 11),
	DIV_CLOSEST(VI_PRE_MIPI1_PIXCLK_DIV_NUM,
		    "vi_preocc_mipi1_ifclk", "vi_preocc_mipi1_ifclk_mux", VI_CLK, 0x8, 4, 4,
		25, MUX_TYPE_DIV, 3, 15),
	MUX(VI_MIPI1_PIX_REF_SWITCH_SEL,
	    "vi_preocc_mipi1_ifclk_mux", VI_CLK, 0x8, 21, 1,
		vi_preocc_mipi1_ifclk_mux_parents,
		CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT),
	GATE(VI_MIPI1B_CFGCLK_EN,
	     "mipi1_csi0_cfgclk", "aon_osc_clk_logic", VI_CLK, 0, 9),
	GATE(VI_MIPI1A_CFGCLK_EN,
	     "mipi1_csi1_cfgclk", "aon_osc_clk_logic", VI_CLK, 0, 8),
	GATE(VI_MIPI0B_CFGCLK_EN,
	     "mipi0_csi0_cfgclk", "aon_osc_clk_logic", VI_CLK, 0, 3),
	GATE(VI_MIPI0A_CFGCLK_EN,
	     "mipi0_csi1_cfgclk", "aon_osc_clk_logic", VI_CLK, 0, 2),
	GATE(VI_X2H0_CLK_EN,
	     "vi_x2h0_clk", "top_cfg_aclk", VI_CLK, 0x4, 5),
	GATE(VI_X2H1_CLK_EN,
	     "vi_x2h1_clk", "top_cfg_aclk", VI_CLK, 0x4, 6),
	GATE(VI_X2H2_CLK_EN,
	     "vi_x2h2_clk", "top_cfg_aclk", VI_CLK, 0x4, 7),
	GATE(VI_REC_PCLK_EN,
	     "vi_rec_pclk", "top_cfg_aclk", VI_CLK, 0x4, 13),
	GATE(VI_REC_ACLK_EN,
	     "vi_rec_aclk", "top_cfg_aclk", VI_CLK, 0x4, 14),
	GATE(VI_MISC_CTRL_H0CLK_CLK_EN,
	     "vi_misc_ctrl_h0clk", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 1),
	GATE(VI_MISC_CTRL_H1CLK_CLK_EN,
	     "vi_misc_ctrl_h1clk", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 2),
	GATE(VI_MISC_CTRL_ISP_ACLK_CLK_EN,
	     "vi_misc_ctrl_isp_aclk", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 3),
	GATE(VI_MISC_CTRL_ISP_SCLK_CLK0_EN,
	     "vi_misc_ctrl_isp_sclk0", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 4),
	GATE(VI_MISC_CTRL_ISP_SCLK_CLK1_EN,
	     "vi_misc_ctrl_isp_sclk1", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 5),
	GATE(VI_MISC_CTRL_ISP_SCLK_CLK2_EN,
	     "vi_misc_ctrl_isp_sclk2", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 6),
	GATE(VI_MISC_CTRL_ISP_SCLK_CLK3_EN,
	     "vi_misc_ctrl_isp_sclk3", "top_cfg_aclk", VI_MISC_CTRL, 0x0, 7),
	GATE(VI_MISC_CTRL_ISP_CORE_CLK_EN,
	     "vi_misc_ctrl_isp_core_clk", "isp_core_clk", VI_MISC_CTRL, 0x0, 8),
	GATE(VI_MISC_CTRL_ISP_FLEXA_CLK_EN,
	     "vi_misc_ctrl_isp_flexa_clk", "isp_core_clk", VI_MISC_CTRL, 0x0, 9),
	GATE(VI_MISC_CTRL_ISP_MCM_CLK_EN,
	     "vi_misc_ctrl_isp_mcm_clk", "isp_core_clk", VI_MISC_CTRL, 0x0, 10),
	GATE(VI_MISC_CTRL_BUS_CLK_GT_DISABLE,
	     "vi_misc_ctrl_bus_clk", "isp_core_clk", VI_MISC_CTRL, 0x0, 11),
};

static struct zhihe_clk_info info_vp[] = {
	GATE(VP_DECOMP_EXTPCLK_EN, "vp_decomp_extpclk", "top_cfg_aclk", VP_CLK, 0, 23),
	GATE(VP_COMP_EXTPCLK_EN, "vp_comp_extpclk", "top_cfg_aclk", VP_CLK, 0, 22),
	GATE(VP_DECOMP_ACLK_EN, "vp_decomp_aclk", "vp_aclk", VP_CLK, 0, 14),
	GATE(VP_DECOMP_CCLK_EN, "vp_decomp_cclk", "vp_decomp_clk_div", VP_CLK, 0, 13),
	GATE(VP_DECOMP_PCLK_EN, "vp_decomp_pclk", "top_cfg_aclk", VP_CLK, 0, 12),
	GATE(VP_COMP_ACLK_EN, "vp_comp_aclk", "vp_aclk", VP_CLK, 0, 11),
	GATE(VP_COMP_CCLK_EN, "vp_comp_cclk", "vp_comp_clk_div", VP_CLK, 0, 10),
	GATE(VP_COMP_PCLK_EN, "vp_comp_pclk", "top_cfg_aclk", VP_CLK, 0, 9),
	GATE(VP_G2D_ACLK_EN, "vp_g2d_aclk", "vp_aclk", VP_CLK, 0, 8),
	GATE(VP_G2D_CCLK_EN, "vp_g2d_cclk", "g2d_cclk", VP_CLK, 0, 7),
	GATE(VP_G2D_PCLK_EN, "vp_g2d_pclk", "top_cfg_aclk", VP_CLK, 0, 6),
	GATE(VP_VENC_ACLK_EN, "vp_venc_aclk", "vp_aclk", VP_CLK, 0, 5),
	GATE(VP_VENC_CCLK_EN, "vp_venc_cclk", "venc_cclk", VP_CLK, 0, 4),
	GATE(VP_VENC_PCLK_EN, "vp_venc_pclk", "top_cfg_aclk", VP_CLK, 0, 3),
	GATE(VP_VDEC_ACLK_EN, "vp_vdec_aclk", "vp_aclk", VP_CLK, 0, 2),
	GATE(VP_VDEC_CCLK_EN, "vp_vdec_cclk", "vdec_cclk", VP_CLK, 0, 1),
	GATE(VP_VDEC_PCLK_EN, "vp_vdec_pclk", "top_cfg_aclk", VP_CLK, 0, 0),
	DIV(VP_DECOMP_DIV_NUM, "vp_decomp_clk_div", "vp_aclk", VP_CLK, 0x4, 16, 4,
	    20, MUX_TYPE_DIV, 2, 15),
	DIV(VP_COMP_DIV_NUM, "vp_comp_clk_div", "vp_aclk", VP_CLK, 0x4, 8, 4,
	    12, MUX_TYPE_DIV, 2, 15),
};

#define VO_PIXCLK_MUX_FLAGS	(CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT)

static struct zhihe_clk_info info_vo[] = {
	GATE(VO_X2H1_CLK_EN, "vo_x2h1_clk", "top_cfg_aclk", VO_CLK, 0, 24),
	GATE(VO_X2H0_CLK_EN, "vo_x2h0_clk", "top_cfg_aclk", VO_CLK, 0, 23),
	GATE(VO_ARB_ACLK_EN, "vo_arb_aclk", "vo_preocc_aclk", VO_CLK, 0, 19),
	GATE(VO_DPU_ACLK_EN, "vo_dpu_aclk", "vo_preocc_aclk", VO_CLK, 0, 4),
	GATE(VO_AUXDISP_ACLK_EN, "vo_disp_aclk", "vo_preocc_aclk", VO_CLK, 0, 7),
	GATE(VO_DECOMP_ACLK_EN, "vo_decomp_aclk", "vo_preocc_aclk", VO_CLK, 0, 17),
	DIV(VO_ACLK_DIV_NUM, "vo_preocc_aclk", "video_pll_foutvco", VO_CLK, 0x8, 0, 4,
	    4, MUX_TYPE_DIV, 3, 15),
	GATE(VO_DECOMP_PCLK_EN, "vo_decomp_pclk", "vo_pclk", VO_CLK, 0, 14),
	GATE(VO_HDMI_PCLK_EN, "vo_hdmi_pclk", "vo_pclk", VO_CLK, 0, 8),
	GATE(VO_MIPI_PCLK_EN, "vo_mipi_pclk", "vo_pclk", VO_CLK, 0, 12),
	GATE(VO_AUXDISP_PCLK_EN, "vo_auxdisp_pclk", "vo_pclk", VO_CLK, 0, 6),
	GATE(VO_ARB_PCLK_EN, "vo_arb_pclk", "vo_pclk", VO_CLK, 0, 18),
	FIXED_FACTOR(VO_PCLK, "vo_pclk", "top_cfg_aclk", 1, 2),
	GATE(VO_DECOMP0_CLK_EN, "vo_decomp0_clk", "vo_decomp_div", VO_CLK, 0, 15),
	GATE(VO_DECOMP1_CLK_EN, "vo_decomp1_clk", "vo_decomp_div", VO_CLK, 0, 16),
	DIV(VO_DECOMP_DIV_NUM, "vo_decomp_div", "video_pll_foutvco", VO_CLK, 0x8, 16, 4,
	    20, MUX_TYPE_DIV, 12, 15),
	GATE(VO_MIPI_CFGCLK_EN, "vo_mipi_cfgclk", "aon_osc_clk_logic", VO_CLK, 0, 13),
	GATE(VO_HDMI_SFRCLK_EN, "vo_hdmi_sfrclk", "aon_osc_clk_logic", VO_CLK, 0, 11),
	GATE(VO_CEC_CLK_EN, "vo_hdmi_cecclk", "aon_rtc_clk", VO_CLK, 0, 10),
	GATE(VO_I2S_CLK_EN, "vo_hdmi_i2sclk", "audio0_pll_foutvco", VO_CLK, 0, 9),
	GATE(VO_CH0_PIXCLK_EN, "dpu0_pixclk", "vo_pll0_div", VO_CLK, 0, 0),
	DIV(VO_PLL0_DIV_NUM, "vo_pll0_div", "dpu0_pll_foutpostdiv", VO_CLK, 0x4, 0, 8,
	    8, MUX_TYPE_DIV, 2, 255),
	GATE(VO_CH1_PIXCLK_EN, "dpu1_pixclk", "vo_pll1_div", VO_CLK, 0, 1),
	DIV(VO_PLL1_DIV_NUM, "vo_pll1_div", "dpu1_pll_foutpostdiv", VO_CLK, 0x4, 10, 8,
	    18, MUX_TYPE_DIV, 2, 255),
	GATE(VO_CH2_PIXCLK_EN, "dpu2_pixclk", "vo_pll2_div", VO_CLK, 0, 5),
	DIV(VO_PLL2_DIV_NUM, "vo_pll2_div", "dpu2_pll_foutpostdiv", VO_CLK, 0x4, 20, 8,
	    28, MUX_TYPE_DIV, 2, 255),
	GATE(VO_DPU_HCLK_EN, "vo_dpuc_hclk", "top_cfg_aclk", VO_CLK, 0, 2),
	GATE(VO_DPUC_CLK_EN, "vo_dpuc_clk", "vo_dpuc_div", VO_CLK, 0, 3),
	DIV(VO_DPUC_DIV_NUM, "vo_dpuc_div", "video_pll_foutvco", VO_CLK, 0x8, 8, 4,
	    12, MUX_TYPE_DIV, 3, 15),
	MUX(VO_MIPI_PIXCLK,
	    "vo_mipi_pixclk", VO_PATH_CTRL, 0x0, 0, 2,
		vo_mipi_pixclk_mux_parents, VO_PIXCLK_MUX_FLAGS),
	MUX(VO_HDMI_PIXCLK,
	    "vo_hdmi_pixclk", VO_PATH_CTRL, 0x0, 2, 2,
		vo_hdmi_pixclk_mux_parents, VO_PIXCLK_MUX_FLAGS),
	MUX(VO_DPTX_PIXCLK,
	    "vo_dptx_pixclk", VO_PATH_CTRL, 0x0, 4, 2,
		vo_dptx_pixclk_mux_parents, VO_PIXCLK_MUX_FLAGS),
};

static struct zhihe_clk_info info_npu[] = {
	GATE(SW_SEMA_PCLK_EN, "npu_clkgen_sema_pclk", "top_cfg_aclk", NPU_CLK, 0, 23),
	GATE(SW_SEMA_ACLK_EN, "npu_clkgen_sema_aclk", "npu_aclk", NPU_CLK, 0, 22),
	GATE(SW_NPU_X2P_ACLK_EN, "npu_x2p_aclk", "top_cfg_aclk", NPU_CLK, 0, 21),
	GATE(SW_NPU_X2H_ACLK_EN, "npu_x2h_aclk", "top_cfg_aclk", NPU_CLK, 0, 20),
	GATE(SW_NPU_IP_HCLK_EN, "npu_ip_hclk", "top_cfg_aclk", NPU_CLK, 0, 12),
	GATE(SW_NPU_IP_CCLK_EN, "npu_ip_cclk", "npu_cclk", NPU_CLK, 0, 11),
	GATE(SW_NPU_IP_ACLK_EN, "npu_ip_aclk", "npu_aclk", NPU_CLK, 0, 10),
};

static struct zhihe_clk_info info_peri[] = {
	/* PERI0 SS */
	GATE(PERI0_MBOX1_PCLK_EN,
	     "peri0_mbox1_pclk", "top_cfg_aclk", PERI0_SYSREG, 0, 6),
	GATE(PERI0_MBOX0_PCLK_EN,
	     "peri0_mbox0_pclk", "top_cfg_aclk", PERI0_SYSREG, 0, 5),
	GATE(PERI0_WDT0_PCLK_EN,
	     "peri0_wdt0_pclk", "top_cfg_aclk", PERI0_SYSREG, 0, 4),
	GATE(PERI0_TIMER1_PCLK_EN,
	     "peri0_timer1_pclk", "top_cfg_aclk", PERI0_SYSREG, 0, 3),
	GATE(PERI0_TIMER1_CCLK_EN,
	     "peri0_timer1_cclk", "top_cfg_aclk", PERI0_SYSREG, 0, 2),
	GATE(PERI0_TIMER0_PCLK_EN,
	     "peri0_timer0_pclk", "top_cfg_aclk", PERI0_SYSREG, 0, 1),
	GATE(PERI0_TIMER0_CCLK_EN,
	     "peri0_timer0_cclk", "top_cfg_aclk", PERI0_SYSREG, 0, 0),
	/* PERI1 SS */
	GATE(PERI1_UART3_SCLK_EN,
	     "peri1_clkgen_uart3_sclk", "uart_sclk", PERI1_SYSREG, 0, 31),
	GATE(PERI1_UART3_PCLK_EN,
	     "peri1_clkgen_uart3_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 30),
	GATE(PERI1_UART2_SCLK_EN,
	     "peri1_clkgen_uart2_sclk", "uart_sclk", PERI1_SYSREG, 0, 29),
	GATE(PERI1_UART2_PCLK_EN,
	     "peri1_clkgen_uart2_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 28),
	GATE(PERI1_UART1_SCLK_EN,
	     "peri1_clkgen_uart1_sclk", "uart_sclk", PERI1_SYSREG, 0, 27),
	GATE(PERI1_UART1_PCLK_EN,
	     "peri1_clkgen_uart1_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 26),
	GATE(PERI1_UART0_SCLK_EN,
	     "peri1_clkgen_uart0_sclk", "uart_sclk", PERI1_SYSREG, 0, 25),
	GATE(PERI1_UART0_PCLK_EN,
	     "peri1_clkgen_uart0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 24),
	GATE(PERI1_SPI0_SSI_CLK_EN,
	     "peri1_clkgen_spi0_ssi_clk", "peri1_spi_ssi_clk", PERI1_SYSREG, 0, 23),
	GATE(PERI1_SPI0_PCLK_EN,
	     "peri1_clkgen_spi0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 22),
	GATE(PERI1_QSPI0_SSI_CLK_EN,
	     "peri1_clkgen_qspi0_ssi_clk", "peri1_qspi_ssi_clk", PERI1_SYSREG, 0, 21),
	GATE(PERI1_QSPI0_PCLK_EN,
	     "peri1_clkgen_qspi0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 20),
	GATE(PERI1_PWM0_PCLK_EN,
	     "peri1_clkgen_pwm0_pclk", "peri1_pwm_clk", PERI1_SYSREG, 0, 19),
	GATE(PERI1_PWM0_CCLK_EN,
	     "peri1_clkgen_pwm0_cclk", "peri1_clkgen_pwm0_pclk", PERI1_SYSREG, 0, 18),
	GATE(PERI1_MST_BUS_PCLK_EN,
	     "peri1_clkgen_mst_bus_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 17),
	GATE(PERI1_MST_BUS_ACLK_EN,
	     "peri1_clkgen_mst_bus_aclk", "peri1_mst_aclk", PERI1_SYSREG, 0, 16),
	GATE(PERI1_I2S0_SRC_CLK_EN,
	     "peri1_clkgen_i2s0_src_clk", "peri1_i2s0_src_clk", PERI1_SYSREG, 0, 15),
	GATE(PERI1_I2S0_PCLK_EN,
	     "peri1_clkgen_i2s0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 14),
	GATE(PERI1_I2C2_PCLK_EN,
	     "peri1_clkgen_i2c2_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 13),
	GATE(PERI1_I2C2_IC_CLK_EN,
	     "peri1_clkgen_i2c2_ic_clk", "i2c_ic_clk", PERI1_SYSREG, 0, 12),
	GATE(PERI1_I2C1_PCLK_EN,
	     "peri1_clkgen_i2c1_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 11),
	GATE(PERI1_I2C1_IC_CLK_EN,
	     "peri1_clkgen_i2c1_ic_clk", "i2c_ic_clk", PERI1_SYSREG, 0, 10),
	GATE(PERI1_I2C0_PCLK_EN,
	     "peri1_clkgen_i2c0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 9),
	GATE(PERI1_I2C0_IC_CLK_EN,
	     "peri1_clkgen_i2c0_ic_clk", "i2c_ic_clk", PERI1_SYSREG, 0, 8),
	GATE(PERI1_GPIO1_PCLK_EN,
	     "peri1_clkgen_gpio1_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 7),
	GATE(PERI1_GPIO1_DBCLK_EN,
	     "peri1_clkgen_gpio1_dbclk", "aon_rtc_clk", PERI1_SYSREG, 0, 6),
	GATE(PERI1_GPIO0_PCLK_EN,
	     "peri1_clkgen_gpio0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0, 5),
	GATE(PERI1_GPIO0_DBCLK_EN,
	     "peri1_clkgen_gpio0_dbclk", "aon_rtc_clk", PERI1_SYSREG, 0, 4),
	GATE(PERI1_GMAC1_HCLK_EN,
	     "peri1_gmac1_hclk", "top_cfg_aclk", PERI1_SYSREG, 0, 3),
	GATE(PERI1_GMAC1_ACLK_EN,
	     "peri1_gmac1_aclk", "peri1_mst_aclk", PERI1_SYSREG, 0, 2),
	GATE(PERI1_GMAC0_HCLK_EN,
	     "peri1_gmac0_hclk", "top_cfg_aclk", PERI1_SYSREG, 0, 1),
	GATE(PERI1_GMAC0_ACLK_EN,
	     "peri1_gmac0_aclk", "peri1_mst_aclk", PERI1_SYSREG, 0, 0),
	GATE(PERI1_ZGMAC_X2X_ACLK_EN,
	     "peri1_zgmac0_x2x_aclk", "peri1_mst_aclk", PERI1_SYSREG, 0x4, 25),
	GATE(PERI1_GMAC2_HCLK_EN,
	     "peri1_gmac2_hclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 24),
	GATE(PERI1_GMAC2_ACLK_EN,
	     "peri1_gmac2_aclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 23),
	GATE(PERI1_X2H_GMAC2_HCLK_EN,
	     "peri1_x2h_gmac2_hclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 22),
	GATE(PERI1_X2H_GMAC2_ACLK_EN,
	     "peri1_x2h_gmac2_aclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 21),
	GATE(PERI1_CHIP_DBG_PCLK_EN,
	     "peri1_chip_dbg_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 20),
	GATE(PERI1_CHIP_DBG_CCLK_EN,
	     "peri1_chip_dbg_cclk", "aon_osc_clk_logic", PERI1_SYSREG, 0x4, 19),
	GATE(PERI1_CHIP_DBG_ACLK_EN,
	     "peri1_chip_dbg_aclk", "peri1_mst_aclk", PERI1_SYSREG, 0x4, 18),
	GATE(PERI1_TDM0_SCLK_EN,
	     "peri1_tdm0_sclk", "peri1_tdm_src_clk", PERI1_SYSREG, 0x4, 17),
	GATE(PERI1_TDM0_PCLK_EN,
	     "peri1_tdm0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 16),
	GATE(PERI1_PDM0_PCLK_EN,
	     "peri1_pdm0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 15),
	GATE(PERI1_PDM0_MCLK_EN,
	     "peri1_pdm0_mclk", "peri1_pdm_mclk", PERI1_SYSREG, 0x4, 14),
	GATE(PERI1_CAN1_PCLK_EN,
	     "peri1_can1_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 13),
	GATE(PERI1_CAN1_OSC_CLK_EN,
	     "peri1_can1_osc_clk", "peri1_hires_clk", PERI1_SYSREG, 0x4, 12),
	GATE(PERI1_CAN1_HIRES_CLK_EN,
	     "peri1_can1_hires_clk", "peri1_can1_osc_clk", PERI1_SYSREG, 0x4, 11),
	GATE(PERI1_CAN0_PCLK_EN,
	     "peri1_can0_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 10),
	GATE(PERI1_CAN0_OSC_CLK_EN,
	     "peri1_can0_osc_clk", "peri1_hires_clk", PERI1_SYSREG, 0x4, 9),
	GATE(PERI1_CAN0_HIRES_CLK_EN,
	     "peri1_can0_hires_clk", "peri1_can0_osc_clk", PERI1_SYSREG, 0x4, 8),
	GATE(PERI1_PAD_CTRL_PCLK_EN,
	     "peri1_pad_ctrl_pclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 7),
	GATE(PERI1_X2H_GMAC1_HCLK_EN,
	     "peri1_x2h_gmac1_hclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 3),
	GATE(PERI1_X2H_GMAC1_ACLK_EN,
	     "peri1_x2h_gmac1_aclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 2),
	GATE(PERI1_X2H_GMAC0_HCLK_EN,
	     "peri1_x2h_gmac0_hclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 1),
	GATE(PERI1_X2H_GMAC0_ACLK_EN,
	     "peri1_x2h_gmac0_aclk", "top_cfg_aclk", PERI1_SYSREG, 0x4, 0),
	/* PERI2 SS */
	GATE(PERI2_I2S3_PCLK_EN,
	     "peri2_clkgen_i2s3_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 31),
	GATE(PERI2_I2S3_SRC_CLK_EN,
	     "peri2_clkgen_i2s3_src_clk", "peri2_i2s3_src_clk", PERI2_SYSREG, 0, 30),
	GATE(PERI2_I2S2_PCLK_EN,
	     "peri2_clkgen_i2s2_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 29),
	GATE(PERI2_I2S2_SRC_CLK_EN,
	     "peri2_clkgen_i2s2_src_clk", "peri2_i2s2_src_clk", PERI2_SYSREG, 0, 28),
	GATE(PERI2_I2S1_PCLK_EN,
	     "peri2_clkgen_i2s1_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 27),
	GATE(PERI2_I2S1_SRC_CLK_EN,
	     "peri2_clkgen_i2s1_src_clk", "peri2_i2s1_src_clk", PERI2_SYSREG, 0, 26),
	GATE(PERI2_I2C7_IC_CLK_EN,
	     "peri2_clkgen_i2c7_ic_clk", "i2c_ic_clk", PERI2_SYSREG, 0, 25),
	GATE(PERI2_I2C7_PCLK_EN,
	     "peri2_clkgen_i2c7_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 24),
	GATE(PERI2_I2C6_IC_CLK_EN,
	     "peri2_clkgen_i2c6_ic_clk", "i2c_ic_clk", PERI2_SYSREG, 0, 23),
	GATE(PERI2_I2C6_PCLK_EN,
	     "peri2_clkgen_i2c6_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 22),
	GATE(PERI2_I2C5_IC_CLK_EN,
	     "peri2_clkgen_i2c5_ic_clk", "i2c_ic_clk", PERI2_SYSREG, 0, 21),
	GATE(PERI2_I2C5_PCLK_EN,
	     "peri2_clkgen_i2c5_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 20),
	GATE(PERI2_I2C4_IC_CLK_EN,
	     "peri2_clkgen_i2c4_ic_clk", "i2c_ic_clk", PERI2_SYSREG, 0, 19),
	GATE(PERI2_I2C4_PCLK_EN,
	     "peri2_clkgen_i2c4_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 18),
	GATE(PERI2_I2C3_IC_CLK_EN,
	     "peri2_clkgen_i2c3_ic_clk", "i2c_ic_clk", PERI2_SYSREG, 0, 17),
	GATE(PERI2_I2C3_PCLK_EN,
	     "peri2_clkgen_i2c3_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 16),
	GATE(PERI2_GPIO3_DBCLK_EN,
	     "peri2_clkgen_gpio3_dbclk", "aon_rtc_clk", PERI2_SYSREG, 0, 15),
	GATE(PERI2_GPIO3_PCLK_EN,
	     "peri2_clkgen_gpio3_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 14),
	GATE(PERI2_PAD_CTRL_PCLK_EN,
	     "peri2_clkgen_pad_ctrl_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 13),
	GATE(PERI2_UART6_PCLK_EN,
	     "peri2_clkgen_uart6_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 12),
	GATE(PERI2_UART5_PCLK_EN,
	     "peri2_clkgen_uart5_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 11),
	GATE_FLAGS(PERI2_UART4_PCLK_EN,
		   "peri2_clkgen_uart4_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 10,
		CLK_IS_CRITICAL),
	GATE(PERI2_SPI1_PCLK_EN,
	     "peri2_clkgen_spi1_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 9),
	GATE(PERI2_GPIO2_PCLK_EN,
	     "peri2_clkgen_gpio2_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 8),
	GATE(PERI2_CAN2_PCLK_EN,
	     "peri2_clkgen_can2_pclk", "top_cfg_aclk", PERI2_SYSREG, 0, 7),
	GATE(PERI2_UART6_SCLK_EN,
	     "peri2_clkgen_uart6_sclk", "uart_sclk", PERI2_SYSREG, 0, 6),
	GATE(PERI2_UART5_SCLK_EN,
	     "peri2_clkgen_uart5_sclk", "uart_sclk", PERI2_SYSREG, 0, 5),
	GATE_FLAGS(PERI2_UART4_SCLK_EN,
		   "peri2_clkgen_uart4_sclk", "uart_sclk", PERI2_SYSREG, 0, 4,
		CLK_IS_CRITICAL),
	GATE(PERI2_SPI1_SSI_CLK_EN,
	     "peri2_clkgen_spi1_ssi_clk", "peri2_spi_ssi_clk", PERI2_SYSREG, 0, 3),
	GATE(PERI2_GPIO2_DBCLK_EN,
	     "peri2_clkgen_gpio2_dbclk", "aon_rtc_clk", PERI2_SYSREG, 0, 2),
	GATE(PERI2_CAN2_OSC_CLK_EN,
	     "peri2_clkgen_can2_osc_clk", "peri2_hires_clk", PERI2_SYSREG, 0, 1),
	GATE(PERI2_CAN2_HIRES_CLK_EN,
	     "peri2_clkgen_can2_hires_clk", "peri2_clkgen_can2_osc_clk", PERI2_SYSREG, 0, 0),
	GATE(PERI2_QSPI1_PCLK_EN,
	     "peri2_clkgen_qspi1_pclk", "top_cfg_aclk", PERI2_SYSREG, 0x4, 11),
	GATE(PERI2_QSPI1_SSI_CLK_EN,
	     "peri2_clkgen_qspi1_ssi_clk", "peri2_qspi_ssi_clk", PERI2_SYSREG, 0x4, 10),
	GATE(PERI2_UART9_PCLK_EN,
	     "peri2_clkgen_uart9_pclk", "top_cfg_aclk", PERI2_SYSREG, 0x4, 9),
	GATE(PERI2_UART9_SCLK_EN,
	     "peri2_clkgen_uart9_sclk", "uart_sclk", PERI2_SYSREG, 0x4, 8),
	GATE(PERI2_UART8_PCLK_EN,
	     "peri2_clkgen_uart8_pclk", "top_cfg_aclk", PERI2_SYSREG, 0x4, 7),
	GATE(PERI2_UART8_SCLK_EN,
	     "peri2_clkgen_uart8_sclk", "uart_sclk", PERI2_SYSREG, 0x4, 6),
	GATE(PERI2_UART7_PCLK_EN,
	     "peri2_clkgen_uart7_pclk", "top_cfg_aclk", PERI2_SYSREG, 0x4, 5),
	GATE(PERI2_UART7_SCLK_EN,
	     "peri2_clkgen_uart7_sclk", "uart_sclk", PERI2_SYSREG, 0x4, 4),
	GATE(PERI2_PWM2_PCLK_EN,
	     "peri2_clkgen_pwm2_pclk", "peri2_pwm_clk", PERI2_SYSREG, 0x4, 3),
	GATE(PERI2_PWM2_CCLK_EN,
	     "peri2_clkgen_pwm2_cclk", "peri2_clkgen_pwm2_pclk", PERI2_SYSREG, 0x4, 2),
	GATE(PERI2_PWM1_PCLK_EN,
	     "peri2_clkgen_pwm1_pclk", "peri2_pwm_clk", PERI2_SYSREG, 0x4, 1),
	GATE(PERI2_PWM1_CCLK_EN,
	     "peri2_clkgen_pwm1_cclk", "peri2_clkgen_pwm1_pclk", PERI2_SYSREG, 0x4, 0),
	/* PERI3 SS */
	GATE(PERI3_PAD_CTRL_PCLK_EN,
	     "peri3_clkgen_pad_ctrl_pclk", "top_cfg_aclk", PERI3_SYSREG, 0, 21),
	GATE(PERI3_GPIO4_PCLK_EN,
	     "peri3_clkgen_gpio4_pclk", "top_cfg_aclk", PERI3_SYSREG, 0, 20),
	GATE(PERI3_GPIO4_DBCLK_EN,
	     "peri3_clkgen_gpio4_dbclk", "aon_rtc_clk", PERI3_SYSREG, 0, 19),
	GATE(CLKCTRL_PERI3_TEE_H2H_SHCLKEN,
	     "peri3_clkgen_tee_h2h_shclk", "top_cfg_aclk", PERI3_SYSREG, 0, 18),
	GATE(CLKCTRL_PERI3_TEE_H2H_MHCLKEN,
	     "peri3_clkgen_tee_h2h_mhclk", "top_cfg_aclk", PERI3_SYSREG, 0, 17),
	GATE(PERI3_TEE_X2X_ACLK_S_EN,
	     "peri3_clkgen_tee_x2x_aclk", "top_cfg_aclk", PERI3_SYSREG, 0, 16),
	GATE(PERI3_ADC_PCLK_EN,
	     "peri3_clkgen_adc_pclk", "top_cfg_aclk", PERI3_SYSREG, 0, 15),
	GATE(AXI_PERI3_MST_PCLK_EN,
	     "peri3_clkgen_axi_mst_pclk", "top_cfg_aclk", PERI3_SYSREG, 0, 14),
	GATE(AXI_PERI3_MST_ACLK_EN,
	     "peri3_clkgen_axi_mst_aclk", "peri3_mst_aclk", PERI3_SYSREG, 0, 13),
	GATE(PERI3_SDIO_X2X_ACLK_S_EN,
	     "peri3_clkgen_sdio_x2x_aclk_s", "peri3_mst_aclk", PERI3_SYSREG, 0, 12),
	GATE(PERI3_SDIO_X2X_ACLK_M_EN,
	     "peri3_clkgen_sdio_x2x_aclk_m", "top_cfg_aclk", PERI3_SYSREG, 0, 11),
	GATE(PERI3_SDIO_OSC_CLK_EN,
	     "peri3_clkgen_sdio_osc_clk", "aon_osc_clk_logic", PERI3_SYSREG, 0, 10),
	GATE(PERI3_SDIO_HCLK_EN,
	     "peri3_clkgen_sdio_hclk", "top_cfg_aclk", PERI3_SYSREG, 0, 9),
	GATE(PERI3_SDIO_ACLK_EN,
	     "peri3_clkgen_sdio_aclk", "top_cfg_aclk", PERI3_SYSREG, 0, 8),
	GATE(PERI3_EMMC_X2X_ACLK_S_EN,
	     "peri3_clkgen_emmc_x2x_aclk_s", "peri3_mst_aclk", PERI3_SYSREG, 0, 7),
	GATE(PERI3_EMMC_X2X_ACLK_M_EN,
	     "peri3_clkgen_emmc_x2x_aclk_m", "top_cfg_aclk", PERI3_SYSREG, 0, 6),
	GATE(PERI3_EMMC_OSC_CLK_EN,
	     "peri3_clkgen_emmc_osc_clk", "aon_osc_clk_logic", PERI3_SYSREG, 0, 5),
	GATE(PERI3_EMMC_HCLK_EN,
	     "peri3_clkgen_emmc_hclk", "top_cfg_aclk", PERI3_SYSREG, 0, 4),
	GATE(PERI3_EMMC_ACLK_EN,
	     "peri3_clkgen_emmc_aclk", "top_cfg_aclk", PERI3_SYSREG, 0, 3),
	FIXED_FACTOR(PERI3_EMMC_SDIO_REF_CLK,
		     "peri3_emmc_sdio_ref_clk", "emmc_ref_clk", 1, 4),
	GATE(PERI3_EMMC_SDIO_REF_CLK_CG_EN,
	     "peri3_clkgen_sdio_ref_clk", "peri3_emmc_sdio_ref_clk", PERI3_SYSREG, 0, 2),
	GATE(PERI3_DMAC_HCLK_EN,
	     "peri3_clkgen_dmac_hclk", "top_cfg_aclk", PERI3_SYSREG, 0, 1),
	GATE(PERI3_DMAC_ACLK_EN,
	     "peri3_clkgen_dmac_aclk", "peri3_mst_aclk", PERI3_SYSREG, 0, 0),
	/* TEE SS */
	GATE(TEE_AHB2_TEESYS_CLKEN,
	     "tee_ahb2_teesys_clk", "tee_clk", TEE_CRG, 0, 19),
	GATE(TEE_APB3_TEESYS_CLKEN,
	     "tee_apb3_teesys_clk", "tee_clk", TEE_CRG, 0, 18),
	GATE(TEE_AXI4_TEESYS_CLKEN,
	     "tee_axi4_teesys_clk", "tee_clk", TEE_CRG, 0, 17),
	GATE(TEE_X2P_TEESYS_CLKEN,
	     "tee_x2p_teesys_clk", "tee_clk", TEE_CRG, 0, 16),
	GATE(TEE_EIP120SI_CLKEN,
	     "tee_eip120si_clk", "tee_clk", TEE_CRG, 0, 12),
	GATE(TEE_EIP120SII_CLKEN,
	     "tee_eip120sii_clk", "tee_clk", TEE_CRG, 0, 11),
	GATE(TEE_EIP120SIII_CLKEN,
	     "tee_eip120siii_clk", "tee_clk", TEE_CRG, 0, 10),
	GATE(TEE_EIP150B_CLKEN,
	     "tee_eip150b_clk", "tee_clk", TEE_CRG, 0, 9),
	GATE(TEE_DMAC_CLKEN,
	     "tee_dmac_clk", "tee_clk", TEE_CRG, 0, 8),
	FIXED_FACTOR(TEE_PCLK_CDE_RATIO,
		     "tee_pclk", "tee_clk", 1, 4),
	GATE(TEE_DS_CLKEN,
	     "tee_ds_clk", "tee_pclk", TEE_CRG, 0, 3),
	GATE(TEE_OCRAM_CLKEN,
	     "tee_ocram_clk", "tee_clk", TEE_CRG, 0, 2),
	GATE(TEE_EFUSE_CLKEN,
	     "tee_efuse_clk", "tee_pclk", TEE_CRG, 0, 1),
	GATE(TEE_KEYRAM_CLKEN,
	     "tee_keyram_clk", "tee_pclk", TEE_CRG, 0, 0),
};

static struct zhihe_clk_subsys top_clk = CLK_SUBSYS("top clk", regs_top,
			info_top, plls_top, ARRAY_SIZE(plls_top), 0);
static struct zhihe_clk_subsys gpu_clk = CLK_SUBSYS("gpu clk", regs_gpu,
			info_gpu, NULL, 0, 0);
static struct zhihe_clk_subsys pcie_clk = CLK_SUBSYS("pcie clk", regs_pcie,
			info_pcie, NULL, 0, 0);
static struct zhihe_clk_subsys usb_clk = CLK_SUBSYS("usb clk", regs_usb,
			info_usb, NULL, 0, 0);
static struct zhihe_clk_subsys vi_clk = CLK_SUBSYS("vi clk", regs_vi,
			info_vi, NULL, 0, 0);
static struct zhihe_clk_subsys vp_clk = CLK_SUBSYS("vp clk", regs_vp,
			info_vp, NULL, 0, 0);
static struct zhihe_clk_subsys vo_clk = CLK_SUBSYS("vo clk", regs_vo,
			info_vo, NULL, 0, 0);
static struct zhihe_clk_subsys npu_clk = CLK_SUBSYS("npu clk", regs_npu,
			info_npu, NULL, 0, 0);
static struct zhihe_clk_subsys peri_clk = CLK_SUBSYS("peri clk", regs_peri,
			info_peri, NULL, 0, 0);

static int a210_clocks_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct clk_onecell_data *clk_data;
	struct zhihe_clk_subsys *priv;
	int i, ret;

	ret = devm_pm_runtime_enable(dev);
	if (ret)
		return ret;

	clk_data = devm_kzalloc(dev, sizeof(*clk_data), GFP_KERNEL);
	if (!clk_data)
		return -ENOMEM;

	clk_data->clks = devm_kcalloc(dev, CLK_END,
				      sizeof(*clk_data->clks), GFP_KERNEL);
	if (!clk_data->clks)
		return -ENOMEM;
	clk_data->clk_num = CLK_END;

	for (i = 0; i < CLK_END; i++)
		clk_data->clks[i] = ERR_PTR(-ENOENT);

	priv = (struct zhihe_clk_subsys *)device_get_match_data(dev);
	if (!priv)
		return -EINVAL;
	priv->clk_data = clk_data;
	dev_set_drvdata(dev, priv);

	ret = zhihe_parse_regbase(pdev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to parse register base\n");

	ret = zhihe_register_clock(pdev);
	if (ret)
		goto unregister_clks;

	ret = of_clk_add_provider(np, of_clk_src_onecell_get, priv->clk_data);
	if (ret) {
		dev_err(dev, "failed to register A210 clock provider\n");
		goto unregister_clks;
	}

	dev_info(dev, "registered A210 %s driver on die%d\n",
		 priv->name, priv->die_num);
	return 0;

unregister_clks:
	zhihe_unregister_clocks(priv);
	return ret;
}

static void a210_clocks_remove(struct platform_device *pdev)
{
	struct zhihe_clk_subsys *priv = platform_get_drvdata(pdev);

	of_clk_del_provider(pdev->dev.of_node);
	zhihe_unregister_clocks(priv);
}

static const struct of_device_id a210_clk_of_match[] = {
	{ .compatible = "zhihe,a210-clk", .data = (const void *)&top_clk },
	{ .compatible = "zhihe,a210-gpu-clk", .data = (const void *)&gpu_clk },
	{ .compatible = "zhihe,a210-pcie-clk", .data = (const void *)&pcie_clk },
	{ .compatible = "zhihe,a210-usb-clk", .data = (const void *)&usb_clk },
	{ .compatible = "zhihe,a210-vi-clk", .data = (const void *)&vi_clk },
	{ .compatible = "zhihe,a210-vp-clk", .data = (const void *)&vp_clk },
	{ .compatible = "zhihe,a210-vo-clk", .data = (const void *)&vo_clk },
	{ .compatible = "zhihe,a210-npu-clk", .data = (const void *)&npu_clk },
	{ .compatible = "zhihe,a210-peri-clk", .data = (const void *)&peri_clk },
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, a210_clk_of_match);

static struct platform_driver a210_clk_driver = {
	.probe = a210_clocks_probe,
	.remove_new = a210_clocks_remove,
	.driver = {
		.name = "a210-clk",
		.of_match_table = of_match_ptr(a210_clk_of_match),
	},
};

static int __init a210_clk_driver_init(void)
{
	return platform_driver_register(&a210_clk_driver);
}
core_initcall(a210_clk_driver_init);

static void __exit a210_clk_driver_exit(void)
{
	platform_driver_unregister(&a210_clk_driver);
}
module_exit(a210_clk_driver_exit);

MODULE_AUTHOR("dong.yan <yand@zhihecomputing.com>");
MODULE_DESCRIPTION("Zhihe A210 clock driver");
MODULE_LICENSE("GPL");
