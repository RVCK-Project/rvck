// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/debugfs.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reset-controller.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <dt-bindings/reset/zhihe,a210-resets.h>

#define A210_RST_NAME_SIZE 40

/* reset subsys enumeration */
enum {
	VP_RST,
	VI_RST,
	NPU_RST,
	VO_RST,
	PERI0_RST,
	PERI1_RST,
	PERI2_RST,
	PERI3_RST,
	PCIE_RST,
	USB_RST,
	TEE_RST,
	GPU_RST,
	A210_RESET_SUBSYS_MAX,
};

/*
 * Reset Signal
 *
 * nextid
 *  Next associated signal that needs to be processed together
 * offset:
 *  Address offset of the register where the reset signal is located
 * bit:
 *	Signal data, can be multiple bits
 * subsys:
 *	The subsystem where this reset sinal is located.
 */
struct a210_rst_signal {
	const char *name;
	const unsigned short nextid;
	const unsigned short offset;
	const unsigned int bit;
	const struct a210_rst_subsys *subsys;
};

/* reset info within a subsys */
struct a210_rst_subsys {
	void __iomem *base;
	struct a210_rst_signal *signals;
	u32 num_signals;
	const char *name;
};

struct a210_rst_match_data {
	u32 id;
	const char *name;
	struct a210_rst_signal *signals;
	u32 num_signals;
};

#if defined(CONFIG_DEBUG_FS)
struct a210_rst_debugfs_node {
	struct a210_rst_subsys *subsys;
	struct a210_rst_signal *signal;
};
#endif

struct a210_reset {
	struct device *dev;
	struct reset_controller_dev reset;
	struct a210_rst_subsys *subsys;
	u32 num_subsys;
	u32 subsys_id;
	/* Protects reset register read-modify-write sequences. */
	spinlock_t lock;
#if defined(CONFIG_DEBUG_FS)
	struct a210_rst_debugfs_node *debugfs_node;
#endif
};

/*
 * Reset node info
 */
static struct a210_rst_signal vp_rst_signals[] = {
	[VP_VDEC_PRST] = {
		"VP_VDEC_PRST", 0, 0, BIT(0),
	},
	[VP_VDEC_CRST] = {
		"VP_VDEC_CRST", 0, 0, BIT(1),
	},
	[VP_VDEC_ARST] = {
		"VP_VDEC_ARST", 0, 0, BIT(2),
	},
	[VP_VENC_PRST] = {
		"VP_VENC_PRST", 0, 0, BIT(3),
	},
	[VP_VENC_CRST] = {
		"VP_VENC_CRST", 0, 0, BIT(4),
	},
	[VP_VENC_ARST] = {
		"VP_VENC_ARST", 0, 0, BIT(5),
	},
	[VP_G2D_PRST] = {
		"VP_G2D_PRST", 0, 0, BIT(6),
	},
	[VP_G2D_CRST] = {
		"VP_G2D_CRST", 0, 0, BIT(7),
	},
	[VP_G2D_ARST] = {
		"VP_G2D_ARST", 0, 0, BIT(8),
	},
	[VP_COMP_PRST] = {
		"VP_COMP_PRST", 0, 0, BIT(9),
	},
	[VP_COMP_CRST] = {
		"VP_COMP_CRST", 0, 0, BIT(10),
	},
	[VP_COMP_ARST] = {
		"VP_COMP_ARST", 0, 0, BIT(11),
	},
	[VP_DECOMP_PRST] = {
		"VP_DECOMP_PRST", 0, 0, BIT(12),
	},
	[VP_DECOMP_CRST] = {
		"VP_DECOMP_CRST", 0, 0, BIT(13),
	},
	[VP_DECOMP_ARST] = {
		"VP_DECOMP_ARST", 0, 0, BIT(14),
	},
	[VP_DFMU_PRST] = {
		"VP_DFMU_PRST", 0, 0, BIT(15),
	},
	[VP_DFMU_ARST] = {
		"VP_DFMU_ARST", 0, 0, BIT(16),
	},
	[VP_ARB_PRST] = {
		"VP_ARB_PRST", 0, 0, BIT(18),
	},
	[VP_ARB_ARST] = {
		"VP_ARB_ARST", 0, 0, BIT(19),
	},
	[VP_VDEC_RS_ARST] = {
		"VP_VDEC_RS_ARST", 0, 0, BIT(20),
	},
	[VP_VENC_RS_ARST] = {
		"VP_VENC_RS_ARST", 0, 0, BIT(21),
	},
	[VP_COMP_EXTPRST] = {
		"VP_COMP_EXTPRST", 0, 0, BIT(22),
	},
	[VP_DECOMP_EXTPRST] = {
		"VP_DECOMP_EXTPRST", 0, 0, BIT(23),
	},
};

static struct a210_rst_signal vi_rst_signals[] = {
	[VI_MIPI0_CSI0_PRST] = {
		"VI_MIPI0_CSI0_PRST", 0, 0, BIT(0),
	},
	[VI_MIPI0_CSI1_PRST] = {
		"VI_MIPI0_CSI1_PRST", 0, 0, BIT(1),
	},
	[VI_MIPI0_FIFO_RST] = {
		"VI_MIPI0_FIFO_RST", 0, 0, BIT(2),
	},
	[VI_MIPI1_CSI0_PRST] = {
		"VI_MIPI1_CSI0_PRST", 0, 0, BIT(3),
	},
	[VI_MIPI1_CSI1_PRST] = {
		"VI_MIPI1_CSI1_PRST", 0, 0, BIT(4),
	},
	[VI_MIPI1_FIFO_RST] = {
		"VI_MIPI1_FIFO_RST", 0, 0, BIT(5),
	},
	[VI_VIPRE_PRST] = {
		"VI_VIPRE_PRST", 0, 0, BIT(6),
	},
	[VI_VIPRE_I0_PIX_RST] = {
		"VI_VIPRE_I0_PIX_RST", 0, 0, BIT(7),
	},
	[VI_VIPRE_I1_PIX_RST] = {
		"VI_VIPRE_I1_PIX_RST", 0, 0, BIT(8),
	},
	[VI_VIPRE_ISPIF_RST] = {
		"VI_VIPRE_ISPIF_RST", 0, 0, BIT(9),
	},
	[VI_VIPRE_ARST] = {
		"VI_VIPRE_ARST", 0, 0, BIT(10),
	},
	[VI_ISP_RST] = {
		"VI_ISP_RST", 0, 0, BIT(11),
	},
	[VI_COMP_PRST] = {
		"VI_COMP_PRST", 0, 0, BIT(12),
	},
	[VI_COMP_ARST] = {
		"VI_COMP_ARST", 0, 0, BIT(13),
	},
	[VI_COMP_ISPOUT_RST] = {
		"VI_COMP_ISPOUT_RST", 0, 0, BIT(14),
	},
	[VI_COMP_DECOUT_RST] = {
		"VI_COMP_DECOUT_RST", 0, 0, BIT(15),
	},
	[VI_COMP_VSEOUT_RST] = {
		"VI_COMP_VSEOUT_RST", 0, 0, BIT(16),
	},
	[VI_COMP0_RST] = {
		"VI_COMP0_RST", 0, 0, BIT(17),
	},
	[VI_COMP1_RST] = {
		"VI_COMP1_RST", 0, 0, BIT(18),
	},
	[VI_DECOMP_RST] = {
		"VI_DECOMP_RST", 0, 0, BIT(19),
	},
	[VI_DW200_RST] = {
		"VI_DW200_RST", 0, 0, BIT(20),
	},
	[VI_DFMU_PRST] = {
		"VI_DFMU_PRST", 0, 0, BIT(21),
	},
	[VI_DFMU_ARST] = {
		"VI_DFMU_ARST", 0, 0, BIT(22),
	},
	[VI_ARB_PRST] = {
		"VI_ARB_PRST", 0, 0, BIT(23),
	},
	[VI_ARB_ARST] = {
		"VI_ARB_ARST", 0, 0, BIT(24),
	},
	[VI_X2H0_HRST] = {
		"VI_X2H0_HRST", 0, 0, BIT(26),
	},
	[VI_X2H1_HRST] = {
		"VI_X2H1_HRST", 0, 0, BIT(27),
	},
	[VI_X2H2_HRST] = {
		"VI_X2H2_HRST", 0, 0, BIT(28),
	},
	[VI_REC_PRST] = {
		"VI_REC_PRST", 0, 0, BIT(29),
	},
	[VI_REC_ARST] = {
		"VI_REC_ARST", 0, 0, BIT(30),
	},
};

static struct a210_rst_signal npu_rst_signals[] = {
	[NPU_AAB_MST_RST] = {
		"NPU_AAB_MST_RST", 0, 0, BIT(0),
	},
	[NPU_AFENCE_RST] = {
		"NPU_AFENCE_RST", 0, 0, BIT(1),
	},
	[NPU_AXI_CCU_PRST] = {
		"NPU_AXI_CCU_PRST", 0, 0, BIT(2),
	},
	[NPU_AXI_CCU_RST] = {
		"NPU_AXI_CCU_RST", 0, 0, BIT(3),
	},
	[NPU_AXI_M2S1_RST] = {
		"NPU_AXI_M2S1_RST", 0, 0, BIT(4),
	},
	[NPU_AXI_RST] = {
		"NPU_AXI_RST", 0, 0, BIT(5),
	},
	[NPU_CLK_CALC_RST] = {
		"NPU_CLK_CALC_RST", 0, 0, BIT(6),
	},
	[NPU_DFMU_ARST] = {
		"NPU_DFMU_ARST", 0, 0, BIT(7),
	},
	[NPU_DFMU_PRST] = {
		"NPU_DFMU_PRST", 0, 0, BIT(8),
	},
	[NPU_LPC_C_RST] = {
		"NPU_LPC_C_RST", 0, 0, BIT(9),
	},
	[NPU_LPE_C_PRST] = {
		"NPU_LPE_C_PRST", 0, 0, BIT(10),
	},
	[NPU_LPE_C_RST] = {
		"NPU_LPE_C_RST", 0, 0, BIT(11),
	},
	[NPU_ARST] = {
		"NPU_ARST", 0, 0, BIT(12),
	},
	[NPU_CRST] = {
		"NPU_CRST", 0, 0, BIT(13),
	},
	[NPU_HRST] = {
		"NPU_HRST", 0, 0, BIT(14),
	},
	[NPU_PCTRL_PB_RST] = {
		"NPU_PCTRL_PB_RST", 0, 0, BIT(15),
	},
	[NPU_PCTRL_PRST] = {
		"NPU_PCTRL_PRST", 0, 0, BIT(16),
	},
	[NPU_PCTRL_RST] = {
		"NPU_PCTRL_RST", 0, 0, BIT(17),
	},
	[NPU_PTW_CCU_PRST] = {
		"NPU_PTW_CCU_PRST", 0, 0, BIT(18),
	},
	[NPU_PTW_CCU_RST] = {
		"NPU_PTW_CCU_RST", 0, 0, BIT(19),
	},
	[NPU_SEMA_ARST] = {
		"NPU_SEMA_ARST", 0, 0, BIT(20),
	},
	[NPU_SEMA_PRST] = {
		"NPU_SEMA_PRST", 0, 0, BIT(21),
	},
	[NPU_DROOP_CCTRL_PRST] = {
		"NPU_DROOP_CCTRL_PRST", 0, 0, BIT(22),
	},
	[NPU_DROOP_CCTRL_RST] = {
		"NPU_DROOP_CCTRL_RST", 0, 0, BIT(23),
	},
	[NPU_X2H_RST] = {
		"NPU_X2H_RST", 0, 0, BIT(24),
	},
	[NPU_X2P_RST] = {
		"NPU_X2P_RST", 0, 0, BIT(25),
	},
	[NPU_LDIP_RST] = {
		"NPU_LDIP_RST", 0, 0, BIT(26),
	},
	[NPU_SRAM_RST] = {
		"NPU_SRAM_RST", 0, 0, BIT(27),
	},
};

static struct a210_rst_signal vo_rst_signals[] = {
	[VO_X2H0_RST] = {
		"VO_X2H0_RST", 0, 0, BIT(0),
	},
	[VO_X2H1_RST] = {
		"VO_X2H1_RST", 0, 0, BIT(1),
	},
	[VO_DPU_HRST] = {
		"VO_DPU_HRST", 0, 0, BIT(2),
	},
	[VO_DPU_CRST] = {
		"VO_DPU_CRST", 0, 0, BIT(3),
	},
	[VO_DPU_ARST] = {
		"VO_DPU_ARST", 0, 0, BIT(4),
	},
	[VO_AUXDISP_PRST] = {
		"VO_AUXDISP_PRST", 0, 0, BIT(5),
	},
	[VO_AUXDISP_PIX_RST] = {
		"VO_AUXDISP_PIX_RST", 0, 0, BIT(6),
	},
	[VO_AUXDISP_ARST] = {
		"VO_AUXDISP_ARST", 0, 0, BIT(7),
	},
	[VO_HDMI_PRST] = {
		"VO_HDMI_PRST", 0, 0, BIT(8),
	},
	[VO_HDMI_MAIN_RST] = {
		"VO_HDMI_MAIN_RST", 0, 0, BIT(9),
	},
	[VO_MIPI_PRST] = {
		"VO_MIPI_PRST", 0, 0, BIT(10),
	},
	[VO_DECOMP_PRST] = {
		"VO_DECOMP_PRST", 0, 0, BIT(11),
	},
	[VO_DECOMP0_CRST] = {
		"VO_DECOMP0_CRST", 0, 0, BIT(12),
	},
	[VO_DECOMP1_CRST] = {
		"VO_DECOMP1_CRST", 0, 0, BIT(13),
	},
	[VO_DECOMP_ARST] = {
		"VO_DECOMP_ARST", 0, 0, BIT(14),
	},
	[VO_ARB_PRST] = {
		"VO_ARB_PRST", 0, 0, BIT(15),
	},
	[VO_ARB_ARST] = {
		"VO_ARB_ARST", 0, 0, BIT(16),
	},
	[VO_DFMU_PRST] = {
		"VO_DFMU_PRST", 0, 0, BIT(17),
	},
	[VO_DFMU_ARST] = {
		"VO_DFMU_ARST", 0, 0, BIT(18),
	},
};

static struct a210_rst_signal peri0_rst_signals[] = {
	[PERI0_TIMER0_CRST] = {
		"PERI0_TIMER0_CRST", 0, 0, BIT(0),
	},
	[PERI0_TIMER0_PRST] = {
		"PERI0_TIMER0_PRST", 0, 0, BIT(1),
	},
	[PERI0_TIMER1_CRST] = {
		"PERI0_TIMER1_CRST", 0, 0, BIT(2),
	},
	[PERI0_TIMER1_PRST] = {
		"PERI0_TIMER1_PRST", 0, 0, BIT(3),
	},
	[PERI0_WDT0_PRST] = {
		"PERI0_WDT0_PRST", 0, 0, BIT(4),
	},
	[PERI0_MBOX0_PRST] = {
		"PERI0_MBOX0_PRST", 0, 0, BIT(5),
	},
	[PERI0_MBOX1_PRST] = {
		"PERI0_MBOX1_PRST", 0, 0, BIT(6),
	},
	[PERI0_TIMER0_RST] = {
		"PERI0_TIMER0_RST", 0, 0, BIT(0) | BIT(1),
	},
	[PERI0_TIMER1_RST] = {
		"PERI0_TIMER1_RST", 0, 0, BIT(2) | BIT(3),
	},
};

static struct a210_rst_signal peri1_rst_signals[] = {
	[PERI1_GMAC0_ARST] = {
		"PERI1_GMAC0_ARST", 0, 0, BIT(0),
	},
	[PERI1_GMAC0_HRST] = {
		"PERI1_GMAC0_HRST", 0, 0, BIT(1),
	},
	[PERI1_GMAC1_ARST] = {
		"PERI1_GMAC1_ARST", 0, 0, BIT(2),
	},
	[PERI1_GMAC1_HRST] = {
		"PERI1_GMAC1_HRST", 0, 0, BIT(3),
	},
	[PERI1_GPIO0_DBRST] = {
		"PERI1_GPIO0_DBRST", 0, 0, BIT(4),
	},
	[PERI1_GPIO0_PRST] = {
		"PERI1_GPIO0_PRST", 0, 0, BIT(5),
	},
	[PERI1_GPIO1_DBRST] = {
		"PERI1_GPIO1_DBRST", 0, 0, BIT(6),
	},
	[PERI1_GPIO1_PRST] = {
		"PERI1_GPIO1_PRST", 0, 0, BIT(7),
	},
	[PERI1_I2C0_IC_RST] = {
		"PERI1_I2C0_IC_RST", 0, 0, BIT(8),
	},
	[PERI1_I2C0_PRST] = {
		"PERI1_I2C0_PRST", 0, 0, BIT(9),
	},
	[PERI1_I2C1_IC_RST] = {
		"PERI1_I2C1_IC_RST", 0, 0, BIT(10),
	},
	[PERI1_I2C1_PRST] = {
		"PERI1_I2C1_PRST", 0, 0, BIT(11),
	},
	[PERI1_I2C2_IC_RST] = {
		"PERI1_I2C2_IC_RST", 0, 0, BIT(12),
	},
	[PERI1_I2C2_PRST] = {
		"PERI1_I2C2_PRST", 0, 0, BIT(13),
	},
	[PERI1_I2S0_PRST] = {
		"PERI1_I2S0_PRST", 0, 0, BIT(14),
	},
	[PERI1_MST_BUS_ARST] = {
		"PERI1_MST_BUS_ARST", 0, 0, BIT(15),
	},
	[PERI1_MST_BUS_PRST] = {
		"PERI1_MST_BUS_PRST", 0, 0, BIT(16),
	},
	[PERI1_PWM0_CRST] = {
		"PERI1_PWM0_CRST", 0, 0, BIT(17),
	},
	[PERI1_PWM0_PRST] = {
		"PERI1_PWM0_PRST", 0, 0, BIT(18),
	},
	[PERI1_QSPI0_PRST] = {
		"PERI1_QSPI0_PRST", 0, 0, BIT(19),
	},
	[PERI1_QSPI0_SSI_RST] = {
		"PERI1_QSPI0_SSI_RST", 0, 0, BIT(20),
	},
	[PERI1_SPI0_PRST] = {
		"PERI1_SPI0_PRST", 0, 0, BIT(21),
	},
	[PERI1_SPI0_SSI_RST] = {
		"PERI1_SPI0_SSI_RST", 0, 0, BIT(22),
	},
	[PERI1_UART0_PRST] = {
		"PERI1_UART0_PRST", PERI1_UART0_S_RST, 0, BIT(23),
	},
	[PERI1_UART0_S_RST] = {
		"PERI1_UART0_S_RST", 0, 0, BIT(24),
	},
	[PERI1_UART1_PRST] = {
		"PERI1_UART1_PRST", PERI1_UART1_S_RST, 0, BIT(25),
	},
	[PERI1_UART1_S_RST] = {
		"PERI1_UART1_S_RST", 0, 0, BIT(26),
	},
	[PERI1_UART2_PRST] = {
		"PERI1_UART2_PRST", PERI1_UART2_S_RST, 0, BIT(27),
	},
	[PERI1_UART2_S_RST] = {
		"PERI1_UART2_S_RST", 0, 0, BIT(28),
	},
	[PERI1_UART3_PRST] = {
		"PERI1_UART3_PRST", PERI1_UART3_S_RST, 0, BIT(29),
	},
	[PERI1_UART3_S_RST] = {
		"PERI1_UART3_S_RST", 0, 0, BIT(30),
	},
	[PERI1_X2H_GMAC0_ARST] = {
		"PERI1_X2H_GMAC0_ARST", 0, 0, BIT(31),
	},
	[PERI1_X2H_GMAC0_HRST] = {
		"PERI1_X2H_GMAC0_HRST", 0, 0x4, BIT(0),
	},
	[PERI1_X2H_GMAC1_ARST] = {
		"PERI1_X2H_GMAC1_ARST", 0, 0x4, BIT(1),
	},
	[PERI1_X2H_GMAC1_HRST] = {
		"PERI1_X2H_GMAC1_HRST", 0, 0x4, BIT(2),
	},
	[PERI1_DFMU_ARESET] = {
		"PERI1_DFMU_ARESET", 0, 0x4, BIT(3),
	},
	[PERI1_DFMU_PRESET] = {
		"PERI1_DFMU_PRESET", 0, 0x4, BIT(4),
	},
	[PERI1_PAD_CTRL_PRST] = {
		"PERI1_PAD_CTRL_PRST", 0, 0x4, BIT(5),
	},
	[PERI1_PDM0_MRST] = {
		"PERI1_PDM0_MRST", 0, 0x4, BIT(6),
	},
	[PERI1_PDM0_PRST] = {
		"PERI1_PDM0_PRST", 0, 0x4, BIT(7),
	},
	[PERI1_TDM0_RST] = {
		"PERI1_TDM0_RST", 0, 0x4, BIT(8),
	},
	[PERI1_CAN0_IPG_PE_RST] = {
		"PERI1_CAN0_IPG_PE_RST", 0, 0x4, BIT(9),
	},
	[PERI1_CAN0_IPG_RST] = {
		"PERI1_CAN0_IPG_RST", 0, 0x4, BIT(10),
	},
	[PERI1_CAN0_IPG_SOFT_RST] = {
		"PERI1_CAN0_IPG_SOFT_RST", 0, 0x4, BIT(11),
	},
	[PERI1_CAN0_IPG_TS_RST] = {
		"PERI1_CAN0_IPG_TS_RST", 0, 0x4, BIT(12),
	},
	[PERI1_CAN0_PRST] = {
		"PERI1_CAN0_PRST", 0, 0x4, BIT(13),
	},
	[PERI1_CAN1_IPG_PE_RST] = {
		"PERI1_CAN1_IPG_PE_RST", 0, 0x4, BIT(14),
	},
	[PERI1_CAN1_IPG_RST] = {
		"PERI1_CAN1_IPG_RST", 0, 0x4, BIT(15),
	},
	[PERI1_CAN1_IPG_SOFT_RST] = {
		"PERI1_CAN1_IPG_SOFT_RST", 0, 0x4, BIT(16),
	},
	[PERI1_CAN1_IPG_TS_RST] = {
		"PERI1_CAN1_IPG_TS_RST", 0, 0x4, BIT(17),
	},
	[PERI1_CAN1_PRST] = {
		"PERI1_CAN1_PRST", 0, 0x4, BIT(18),
	},
	[PERI1_CHIP_DBG_ARST] = {
		"PERI1_CHIP_DBG_ARST", 0, 0x4, BIT(19),
	},
	[PERI1_CHIP_DBG_CRST] = {
		"PERI1_CHIP_DBG_CRST", 0, 0x4, BIT(20),
	},
	[PERI1_CHIP_DBG_PRST] = {
		"PERI1_CHIP_DBG_PRST", 0, 0x4, BIT(21),
	},
	[PERI1_GMAC_CRST] = {
		"PERI1_GMAC_CRST", 0, 0x4, BIT(22),
	},
	[PERI1_X2H_GMAC2_ARST] = {
		"PERI1_X2H_GMAC2_ARST", 0, 0x4, BIT(23),
	},
	[PERI1_X2H_GMAC2_HRST] = {
		"PERI1_X2H_GMAC2_HRST", 0, 0x4, BIT(24),
	},
	[PERI1_ZGMAC_X2X_ARST] = {
		"PERI1_ZGMAC_X2X_ARST", 0, 0x4, BIT(25),
	},
	[PERI1_SPI0_RST] = {
		"PERI1_SPI0_RST", 0, 0, BIT(21) | BIT(22),
	},
	[PERI1_I2C0_RST] = {
		"PERI1_I2C0_RST", 0, 0, BIT(8) | BIT(9),
	},
	[PERI1_I2C1_RST] = {
		"PERI1_I2C1_RST", 0, 0, BIT(10) | BIT(11),
	},
	[PERI1_I2C2_RST] = {
		"PERI1_I2C2_RST", 0, 0, BIT(12) | BIT(13),
	},
	[PERI1_GMAC0_RST] = {
		"PERI1_GMAC0_RST", 0, 0, BIT(0) | BIT(1) | BIT(31),
	},
	[PERI1_GMAC0_X2H_RST] = {
		"PERI1_GMAC0_X2H_RST", 0, 0x4, BIT(0),
	},
	[PERI1_GMAC1_RST] = {
		"PERI1_GMAC1_RST", 0, 0, BIT(2) | BIT(3),
	},
	[PERI1_GMAC1_X2H_RST] = {
		"PERI1_GMAC1_X2H_RST", 0, 0x4, BIT(1) | BIT(2),
	},
};

static struct a210_rst_signal peri2_rst_signals[] = {
	[PERI2_CAN2_IPG_PE_RST] = {
		"PERI2_CAN2_IPG_PE_RST", 0, 0, BIT(0),
	},
	[PERI2_CAN2_IPG_RST] = {
		"PERI2_CAN2_IPG_RST", 0, 0, BIT(1),
	},
	[PERI2_CAN2_IPG_SOFT_RST] = {
		"PERI2_CAN2_IPG_SOFT_RST", 0, 0, BIT(2),
	},
	[PERI2_CAN2_IPG_TS_RST] = {
		"PERI2_CAN2_IPG_TS_RST", 0, 0, BIT(3),
	},
	[PERI2_CAN2_PRST] = {
		"PERI2_CAN2_PRST", 0, 0, BIT(4),
	},
	[PERI2_GPIO2_DBRST] = {
		"PERI2_GPIO2_DBRST", 0, 0, BIT(5),
	},
	[PERI2_GPIO2_PRST] = {
		"PERI2_GPIO2_PRST", 0, 0, BIT(6),
	},
	[PERI2_I2C4_IC_RST] = {
		"PERI2_I2C4_IC_RST", 0, 0, BIT(7),
	},
	[PERI2_I2C4_PRST] = {
		"PERI2_I2C4_PRST", 0, 0, BIT(8),
	},
	[PERI2_I2S2_PRST] = {
		"PERI2_I2S2_PRST", 0, 0, BIT(9),
	},
	[PERI2_SPI1_PRST] = {
		"PERI2_SPI1_PRST", 0, 0, BIT(10),
	},
	[PERI2_SPI1_SSI_RST] = {
		"PERI2_SPI1_SSI_RST", 0, 0, BIT(11),
	},
	[PERI2_UART4_PRST] = {
		"PERI2_UART4_PRST", PERI2_UART4_S_RST, 0, BIT(12),
	},
	[PERI2_UART4_S_RST] = {
		"PERI2_UART4_S_RST", 0, 0, BIT(13),
	},
	[PERI2_UART5_PRST] = {
		"PERI2_UART5_PRST", PERI2_UART5_S_RST, 0, BIT(14),
	},
	[PERI2_UART5_S_RST] = {
		"PERI2_UART5_S_RST", 0, 0, BIT(15),
	},
	[PERI2_UART6_PRST] = {
		"PERI2_UART6_PRST", PERI2_UART6_S_RST, 0, BIT(16),
	},
	[PERI2_UART6_S_RST] = {
		"PERI2_UART6_S_RST", 0, 0, BIT(17),
	},
	[PERI2_PAD_CTRL_PRST] = {
		"PERI2_PAD_CTRL_PRST", 0, 0, BIT(18),
	},
	[PERI2_GPIO3_DBRST] = {
		"PERI2_GPIO3_DBRST", 0, 0, BIT(19),
	},
	[PERI2_GPIO3_PRST] = {
		"PERI2_GPIO3_PRST", 0, 0, BIT(20),
	},
	[PERI2_I2C3_IC_RST] = {
		"PERI2_I2C3_IC_RST", 0, 0, BIT(21),
	},
	[PERI2_I2C3_PRST] = {
		"PERI2_I2C3_PRST", 0, 0, BIT(22),
	},
	[PERI2_I2C5_IC_RST] = {
		"PERI2_I2C5_IC_RST", 0, 0, BIT(23),
	},
	[PERI2_I2C5_PRST] = {
		"PERI2_I2C5_PRST", 0, 0, BIT(24),
	},
	[PERI2_I2C6_IC_RST] = {
		"PERI2_I2C6_IC_RST", 0, 0, BIT(25),
	},
	[PERI2_I2C6_PRST] = {
		"PERI2_I2C6_PRST", 0, 0, BIT(26),
	},
	[PERI2_I2C7_IC_RST] = {
		"PERI2_I2C7_IC_RST", 0, 0, BIT(27),
	},
	[PERI2_I2C7_PRST] = {
		"PERI2_I2C7_PRST", 0, 0, BIT(28),
	},
	[PERI2_I2S1_PRST] = {
		"PERI2_I2S1_PRST", 0, 0, BIT(29),
	},
	[PERI2_I2S3_PRST] = {
		"PERI2_I2S3_PRST", 0, 0, BIT(30),
	},
	[PERI2_UART7_PRST] = {
		"PERI2_UART7_PRST", PERI2_UART7_S_RST, 0, BIT(31),
	},
	[PERI2_UART7_S_RST] = {
		"PERI2_UART7_S_RST", 0, 0x4, BIT(0),
	},
	[PERI2_UART8_PRST] = {
		"PERI2_UART8_PRST", PERI2_UART8_S_RST, 0x4, BIT(1),
	},
	[PERI2_UART8_S_RST] = {
		"PERI2_UART8_S_RST", 0, 0x4, BIT(2),
	},
	[PERI2_UART9_PRST] = {
		"PERI2_UART9_PRST", PERI2_UART9_S_RST, 0x4, BIT(3),
	},
	[PERI2_UART9_S_RST] = {
		"PERI2_UART9_S_RST", 0, 0x4, BIT(4),
	},
	[PERI2_QSPI1_PRST] = {
		"PERI2_QSPI1_PRST", 0, 0x4, BIT(5),
	},
	[PERI2_QSPI1_SSI_RST] = {
		"PERI2_QSPI1_SSI_RST", 0, 0x4, BIT(6),
	},
	[PERI2_PWM1_CRST] = {
		"PERI2_PWM1_CRST", 0, 0x4, BIT(7),
	},
	[PERI2_PWM1_PRST] = {
		"PERI2_PWM1_PRST", 0, 0x4, BIT(8),
	},
	[PERI2_PWM2_CRST] = {
		"PERI2_PWM2_CRST", 0, 0x4, BIT(9),
	},
	[PERI2_PWM2_PRST] = {
		"PERI2_PWM2_PRST", 0, 0x4, BIT(10),
	},
	[PERI2_I2C3_RST] = {
		"PERI2_I2C3_RST", 0, 0, BIT(21) | BIT(22),
	},
	[PERI2_I2C4_RST] = {
		"PERI2_I2C4_RST", 0, 0, BIT(7) | BIT(8),
	},
	[PERI2_I2C5_RST] = {
		"PERI2_I2C5_RST", 0, 0, BIT(23) | BIT(24),
	},
	[PERI2_I2C6_RST] = {
		"PERI2_I2C6_RST", 0, 0, BIT(25) | BIT(26),
	},
	[PERI2_I2C7_RST] = {
		"PERI2_I2C7_RST", 0, 0, BIT(27) | BIT(28),
	},
	[PERI2_SPI1_RST] = {
		"PERI2_SPI1_RST", 0, 0, BIT(10) | BIT(11),
	},
};

static struct a210_rst_signal peri3_rst_signals[] = {
	[PERI3_DMAC_ARST] = {
		"PERI3_DMAC_ARST", 0, 0, BIT(0),
	},
	[PERI3_DMAC_HRST] = {
		"PERI3_DMAC_HRST", 0, 0, BIT(1),
	},
	[PERI3_EMMC_SDIO_CLKGEN_RST] = {
		"PERI3_EMMC_SDIO_CLKGEN_RST", 0, 0, BIT(2),
	},
	[PERI3_EMMC_RST] = {
		"PERI3_EMMC_RST", 0, 0, BIT(3),
	},
	[PERI3_EMMC_X2X_ARST_M] = {
		"PERI3_EMMC_X2X_ARST_M", 0, 0, BIT(4),
	},
	[PERI3_EMMC_X2X_ARST_S] = {
		"PERI3_EMMC_X2X_ARST_S", 0, 0, BIT(5),
	},
	[PERI3_SDIO_RST] = {
		"PERI3_SDIO_RST", 0, 0, BIT(6),
	},
	[PERI3_SDIO_X2X_ARST_M] = {
		"PERI3_SDIO_X2X_ARST_M", 0, 0, BIT(7),
	},
	[PERI3_SDIO_X2X_ARST_S] = {
		"PERI3_SDIO_X2X_ARST_S", 0, 0, BIT(8),
	},
	[PERI3_AXI_MST_ARST] = {
		"PERI3_AXI_MST_ARST", 0, 0, BIT(9),
	},
	[PERI3_AXI_MST_PRST] = {
		"PERI3_AXI_MST_PRST", 0, 0, BIT(10),
	},
	[PERI3_ADC_PRST] = {
		"PERI3_ADC_PRST", 0, 0, BIT(11),
	},
	[PERI3_TEE_X2X_ARST_M] = {
		"PERI3_TEE_X2X_ARST_M", 0, 0, BIT(12),
	},
	[PERI3_TEE_X2X_ARST_S] = {
		"PERI3_TEE_X2X_ARST_S", 0, 0, BIT(13),
	},
	[PERI3_TEE_H2H_HRST] = {
		"PERI3_TEE_H2H_HRST", 0, 0, BIT(14),
	},
	[PERI3_GPIO4_DBRST] = {
		"PERI3_GPIO4_DBRST", 0, 0, BIT(15),
	},
	[PERI3_GPIO4_PRST] = {
		"PERI3_GPIO4_PRST", 0, 0, BIT(16),
	},
	[PERI3_PAD_CTRL_PRST] = {
		"PERI3_PAD_CTRL_PRST", 0, 0, BIT(17),
	},
};

static struct a210_rst_signal pcie_rst_signals[] = {
	[PCIE_X2X_PERI_SLV_ARST] = {
		"PCIE_X2X_PERI_SLV_ARST", 0, 0, BIT(0),
	},
	[PCIE_X2X_PERI_MST_ARST] = {
		"PCIE_X2X_PERI_MST_ARST", 0, 0, BIT(4),
	},
	[PCIE_AXI4_PCIE_MST_ARST] = {
		"PCIE_AXI4_PCIE_MST_ARST", 0, 0, BIT(8),
	},
	[PCIE_AXI4_PCIE_MST_PRST] = {
		"PCIE_AXI4_PCIE_MST_PRST", 0, 0, BIT(12),
	},
	[PCIE_DFMU_PRST] = {
		"PCIE_DFMU_PRST", 0, 0x4, BIT(0),
	},
	[PCIE_DFMU_ARST] = {
		"PCIE_DFMU_ARST", 0, 0x4, BIT(4),
	},
	[PCIE_E16PHY_PHY_RST] = {
		"PCIE_E16PHY_PHY_RST", 0, 0x8, BIT(0),
	},
	[PCIE_E16PHY_APBS_PRST] = {
		"PCIE_E16PHY_APBS_PRST", 0, 0x8, BIT(4),
	},
	[PCIE_X2X_SATA_MST_ARST] = {
		"PCIE_X2X_SATA_MST_ARST", 0, 0x10, BIT(0),
	},
	[PCIE_X2X_SATA_SLV_ARST] = {
		"PCIE_X2X_SATA_SLV_ARST", 0, 0x10, BIT(4),
	},
	[PCIE_SATA_ARESET] = {
		"PCIE_SATA_ARESET", 0, 0x10, BIT(8),
	},
	[PCIE_SATA_RST_PMALIVE] = {
		"PCIE_SATA_RST_PMALIVE", 0, 0x10, BIT(12),
	},
	[PCIE_SATA_SLV_AFENCE_ARST] = {
		"PCIE_SATA_SLV_AFENCE_ARST", 0, 0x10, BIT(16),
	},
	[PCIE_SATA_MST_AFENCE_ARST] = {
		"PCIE_SATA_MST_AFENCE_ARST", 0, 0x10, BIT(20),
	},
	[PCIE_SATA_RST_ASIC0] = {
		"PCIE_SATA_RST_ASIC0", 0, 0x14, BIT(0),
	},
	[PCIE_SATA_RST_ASIC1] = {
		"PCIE_SATA_RST_ASIC1", 0, 0x14, BIT(4),
	},
	[PCIE_SATA_RST_RXOOB0] = {
		"PCIE_SATA_RST_RXOOB0", 0, 0x14, BIT(8),
	},
	[PCIE_SATA_RST_RXOOB1] = {
		"PCIE_SATA_RST_RXOOB1", 0, 0x14, BIT(12),
	},
	[PCIE_DM_GEN3X4_APBS_PRST] = {
		"PCIE_DM_GEN3X4_APBS_PRST", 0, 0x20, BIT(0),
	},
	[PCIE_DM_GEN3X4_POWER_UP_RST] = {
		"PCIE_DM_GEN3X4_POWER_UP_RST", 0, 0x20, BIT(4),
	},
	[PCIE_DM_SLV_AFENCE_ARST] = {
		"PCIE_DM_SLV_AFENCE_ARST", 0, 0x20, BIT(16),
	},
	[PCIE_DM_MST_AFENCE_ARST] = {
		"PCIE_DM_MST_AFENCE_ARST", 0, 0x20, BIT(20),
	},
	[PCIE_RP_GEN3X1_APBS_PRST] = {
		"PCIE_RP_GEN3X1_APBS_PRST", 0, 0x24, BIT(0),
	},
	[PCIE_RP_GEN3X1_POWER_UP_RST] = {
		"PCIE_RP_GEN3X1_POWER_UP_RST", 0, 0x24, BIT(4),
	},
	[PCIE_RP_SLV_AFENCE_ARST] = {
		"PCIE_RP_SLV_AFENCE_ARST", 0, 0x24, BIT(16),
	},
	[PCIE_RP_MST_AFENCE_ARST] = {
		"PCIE_RP_MST_AFENCE_ARST", 0, 0x24, BIT(20),
	},
};

static struct a210_rst_signal usb_rst_signals[] = {
	[USB_DFMU_PRST] = {
		"USB_DFMU_PRST", 0, 0, BIT(0),
	},
	[USB_DFMU_ARST] = {
		"USB_DFMU_ARST", 0, 0, BIT(4),
	},
	[USB_AXI4_MST_ARST] = {
		"USB_AXI4_MST_ARST", 0, 0, BIT(8),
	},
	[USB_AXI4_MST_PRST] = {
		"USB_AXI4_MST_PRST", 0, 0, BIT(12),
	},
	[USB_DPTX_APBS_PRST] = {
		"USB_DPTX_APBS_PRST", 0, 0x4, BIT(0),
	},
	[USB_DPTX_VCC_RST] = {
		"USB_DPTX_VCC_RST", 0, 0x4, BIT(4),
	},
	[USB_USB31_APBS_PRST] = {
		"USB_USB31_APBS_PRST", 0, 0x8, BIT(0),
	},
	[USB_USB31_VCC_RST] = {
		"USB_USB31_VCC_RST", 0, 0x8, BIT(4),
	},
	[USB_USB31_PHY_RST] = {
		"USB_USB31_PHY_RST", 0, 0x8, BIT(8),
	},
	[USB_USB31_SLV_AFENCE_ARST] = {
		"USB_USB31_SLV_AFENCE_ARST", 0, 0x8, BIT(12),
	},
	[USB_C10PHY_PHY_RST] = {
		"USB_C10PHY_PHY_RST", 0, 0xc, BIT(0),
	},
	[USB_C10PHY_APBS_PRST] = {
		"USB_C10PHY_APBS_PRST", 0, 0xc, BIT(4),
	},
	[USB_USB20_BLK_X2H_HRST] = {
		"USB_USB20_BLK_X2H_HRST", 0, 0x10, BIT(0),
	},
	[USB_USB20_BLK_X2H_ARST] = {
		"USB_USB20_BLK_X2H_ARST", 0, 0x10, BIT(4),
	},
	[USB_USB20_SLV_AFENCE_ARST] = {
		"USB_USB20_SLV_AFENCE_ARST", 0, 0x10, BIT(8),
	},
	[USB_USB20_BLK_H2P_HRST] = {
		"USB_USB20_BLK_H2P_HRST", 0, 0x10, BIT(16),
	},
	[USB_USB20_BLK_AHB_SLV_HRST] = {
		"USB_USB20_BLK_AHB_SLV_HRST", 0, 0x10, BIT(20),
	},
	[USB_USB20_BLK_AXI_MST_ARST] = {
		"USB_USB20_BLK_AXI_MST_ARST", 0, 0x10, BIT(24),
	},
	[USB_USB20_BLK_USB2_SYSREG_PRST] = {
		"USB_USB20_BLK_USB2_SYSREG_PRST", 0, 0x10, BIT(28),
	},
	[USB_USB20_BLK_USB2_WRAP0_PRST] = {
		"USB_USB20_BLK_USB2_WRAP0_PRST", 0, 0x14, BIT(0),
	},
	[USB_USB20_BLK_USB2_WRAP0_HRST] = {
		"USB_USB20_BLK_USB2_WRAP0_HRST", 0, 0x14, BIT(4),
	},
	[USB_USB20_BLK_USB2_WRAP1_PRST] = {
		"USB_USB20_BLK_USB2_WRAP1_PRST", 0, 0x14, BIT(8),
	},
	[USB_USB20_BLK_USB2_WRAP1_HRST] = {
		"USB_USB20_BLK_USB2_WRAP1_HRST", 0, 0x14, BIT(12),
	},
	[USB_USB20_BLK_A2X0_HRST] = {
		"USB_USB20_BLK_A2X0_HRST", 0, 0x14, BIT(16),
	},
	[USB_USB20_BLK_A2X0_ARST] = {
		"USB_USB20_BLK_A2X0_ARST", 0, 0x14, BIT(20),
	},
	[USB_USB20_BLK_A2X1_HRST] = {
		"USB_USB20_BLK_A2X1_HRST", 0, 0x14, BIT(24),
	},
	[USB_USB20_BLK_A2X1_ARST] = {
		"USB_USB20_BLK_A2X1_ARST", 0, 0x14, BIT(28),
	},
	[USB_USB20_BLK_USB0_PHY_PON_RESET] = {
		"USB_USB20_BLK_USB0_PHY_PON_RESET", 0, 0x18, BIT(0),
	},
	[USB_USB20_BLK_USB1_PHY_PON_RESET] = {
		"USB_USB20_BLK_USB1_PHY_PON_RESET", 0, 0x18, BIT(4),
	},
	[USB_PERI2_SS_RST] = {
		"USB_PERI2_SS_RST", 0, 0x20, BIT(0),
	},
	[USB_PERI2_SLV_AFENCE_ARST] = {
		"USB_PERI2_SLV_AFENCE_ARST", 0, 0x20, BIT(4),
	},
};

static struct a210_rst_signal tee_rst_signals[] = {
	[TEE_KEYRAM_PRST] = {
		"TEE_KEYRAM_PRST", 0, 0, BIT(0),
	},
	[TEE_DS_PRST] = {
		"TEE_DS_PRST", 0, 0, BIT(1),
	},
	[TEE_EFUSE_PRST] = {
		"TEE_EFUSE_PRST", 0, 0, BIT(2),
	},
	[TEE_OCRAM_HRST] = {
		"TEE_OCRAM_HRST", 0, 0, BIT(3),
	},
	[TEE_SYSREG_PRST] = {
		"TEE_SYSREG_PRST", 0, 0, BIT(4),
	},
	[TEE_CCU_PRST] = {
		"TEE_CCU_PRST", 0, 0, BIT(5),
	},
	[TEE_CCU_CRST] = {
		"TEE_CCU_CRST", 0, 0, BIT(6),
	},
	[TEE_EIP150B_HRST] = {
		"TEE_EIP150B_HRST", 0, 0, BIT(8),
	},
	[TEE_EIP120SIII_HRST] = {
		"TEE_EIP120SIII_HRST", 0, 0, BIT(9),
	},
	[TEE_EIP120SIII_ARST] = {
		"TEE_EIP120SIII_ARST", 0, 0, BIT(10),
	},
	[TEE_EIP120SII_HRST] = {
		"TEE_EIP120SII_HRST", 0, 0, BIT(11),
	},
	[TEE_EIP120SII_ARST] = {
		"TEE_EIP120SII_ARST", 0, 0, BIT(12),
	},
	[TEE_EIP120SI_HRST] = {
		"TEE_EIP120SI_HRST", 0, 0, BIT(13),
	},
	[TEE_EIP120SI_ARST] = {
		"TEE_EIP120SI_ARST", 0, 0, BIT(14),
	},
	[TEE_DMAC_HRST] = {
		"TEE_DMAC_HRST", 0, 0, BIT(15),
	},
	[TEE_DMAC_ARST] = {
		"TEE_DMAC_ARST", 0, 0, BIT(16),
	},
	[TEE_X2P_TEESYS_PRST] = {
		"TEE_X2P_TEESYS_PRST", 0, 0, BIT(24),
	},
	[TEE_X2P_TEESYS_ARST] = {
		"TEE_X2P_TEESYS_ARST", 0, 0, BIT(25),
	},
	[TEE_AXI4_TEESYS_ARST] = {
		"TEE_AXI4_TEESYS_ARST", 0, 0, BIT(26),
	},
	[TEE_APB3_TEESYS_PRST] = {
		"TEE_APB3_TEESYS_PRST", 0, 0, BIT(27),
	},
	[TEE_APB3_TEESYS_HRST] = {
		"TEE_APB3_TEESYS_HRST", 0, 0, BIT(28),
	},
	[TEE_AHB2_TEESYS_HRST] = {
		"TEE_AHB2_TEESYS_HRST", 0, 0, BIT(29),
	},
};

static struct a210_rst_signal gpu_rst_signals[] = {
	[GPU_PWR_WRAP_RGX_HOOD_RST] = {
		"GPU_PWR_WRAP_RGX_HOOD_RST", 0, 0, BIT(0),
	},
	[GPU_PWR_WRAP_DFMU_RST] = {
		"GPU_PWR_WRAP_DFMU_RST", 0, 0, BIT(1),
	},
};

static const struct a210_rst_match_data a210_rst_match_data[] = {
	[VP_RST] = {
		.id = VP_RST,
		.name = "vp-rst",
		.signals = vp_rst_signals,
		.num_signals = ARRAY_SIZE(vp_rst_signals),
	},
	[VI_RST] = {
		.id = VI_RST,
		.name = "vi-rst",
		.signals = vi_rst_signals,
		.num_signals = ARRAY_SIZE(vi_rst_signals),
	},
	[NPU_RST] = {
		.id = NPU_RST,
		.name = "npu-rst",
		.signals = npu_rst_signals,
		.num_signals = ARRAY_SIZE(npu_rst_signals),
	},
	[VO_RST] = {
		.id = VO_RST,
		.name = "vo-rst",
		.signals = vo_rst_signals,
		.num_signals = ARRAY_SIZE(vo_rst_signals),
	},
	[PERI0_RST] = {
		.id = PERI0_RST,
		.name = "peri0-rst",
		.signals = peri0_rst_signals,
		.num_signals = ARRAY_SIZE(peri0_rst_signals),
	},
	[PERI1_RST] = {
		.id = PERI1_RST,
		.name = "peri1-rst",
		.signals = peri1_rst_signals,
		.num_signals = ARRAY_SIZE(peri1_rst_signals),
	},
	[PERI2_RST] = {
		.id = PERI2_RST,
		.name = "peri2-rst",
		.signals = peri2_rst_signals,
		.num_signals = ARRAY_SIZE(peri2_rst_signals),
	},
	[PERI3_RST] = {
		.id = PERI3_RST,
		.name = "peri3-rst",
		.signals = peri3_rst_signals,
		.num_signals = ARRAY_SIZE(peri3_rst_signals),
	},
	[PCIE_RST] = {
		.id = PCIE_RST,
		.name = "pcie-rst",
		.signals = pcie_rst_signals,
		.num_signals = ARRAY_SIZE(pcie_rst_signals),
	},
	[USB_RST] = {
		.id = USB_RST,
		.name = "usb-rst",
		.signals = usb_rst_signals,
		.num_signals = ARRAY_SIZE(usb_rst_signals),
	},
	[TEE_RST] = {
		.id = TEE_RST,
		.name = "tee-rst",
		.signals = tee_rst_signals,
		.num_signals = ARRAY_SIZE(tee_rst_signals),
	},
	[GPU_RST] = {
		.id = GPU_RST,
		.name = "gpu-rst",
		.signals = gpu_rst_signals,
		.num_signals = ARRAY_SIZE(gpu_rst_signals),
	},
};

static int a210_reset_init_subsys(struct platform_device *pdev,
				  const struct a210_rst_match_data *data)
{
	struct device *dev = &pdev->dev;
	struct a210_reset *priv = dev_get_drvdata(dev);

	if (!data)
		return -EINVAL;

	priv->subsys = devm_kzalloc(dev, sizeof(*priv->subsys), GFP_KERNEL);
	if (!priv->subsys)
		return -ENOMEM;

	priv->subsys_id = data->id;
	priv->num_subsys = 1;
	priv->subsys[0].signals = data->signals;
	priv->subsys[0].num_signals = data->num_signals;
	priv->subsys[0].name = data->name;
	priv->subsys[0].base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->subsys[0].base))
		return PTR_ERR(priv->subsys[0].base);

	for (int i = 0; i < priv->subsys[0].num_signals; i++)
		priv->subsys[0].signals[i].subsys = &priv->subsys[0];

	return 0;
}

static inline struct a210_reset *to_a210_reset(struct reset_controller_dev *rcdev)
{
	return container_of(rcdev, struct a210_reset, reset);
}

static int a210_reset_signal_lookup(struct a210_reset *priv, unsigned long id,
				    struct a210_rst_subsys **subsys,
				   struct a210_rst_signal **signal)
{
	u32 signal_id = id;

	*subsys = &priv->subsys[0];

	if (signal_id >= (*subsys)->num_signals)
		return -ENODEV;

	*signal = &(*subsys)->signals[signal_id];

	return 0;
}

static int a210_reset_update(struct reset_controller_dev *rcdev, unsigned long id, bool assert)
{
	u32 reg;
	struct a210_reset *priv = to_a210_reset(rcdev);
	struct a210_rst_subsys *subsys;
	struct a210_rst_signal *signal;
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&priv->lock, flags);
	do {
		ret = a210_reset_signal_lookup(priv, id, &subsys, &signal);
		if (ret) {
			spin_unlock_irqrestore(&priv->lock, flags);
			dev_err(rcdev->dev, "fail to find signal 0x%lx\n", id);
			return ret;
		}

		reg = readl(subsys->base + signal->offset);
		if (assert)
			reg &= ~signal->bit;
		else
			reg |= signal->bit;
		writel(reg, subsys->base + signal->offset);

		id = signal->nextid;
	} while (id > 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int a210_reset_assert(struct reset_controller_dev *rcdev, unsigned long id)
{
	return a210_reset_update(rcdev, id, true);
}

static int a210_reset_deassert(struct reset_controller_dev *rcdev, unsigned long id)
{
	return a210_reset_update(rcdev, id, false);
}

static int a210_reset_status(struct reset_controller_dev *rcdev, unsigned long id)
{
	struct a210_reset *priv = to_a210_reset(rcdev);
	struct a210_rst_subsys *subsys;
	struct a210_rst_signal *signal;
	int ret;

	ret = a210_reset_signal_lookup(priv, id, &subsys, &signal);
	if (ret)
		return ret;

	return !(readl(subsys->base + signal->offset) & signal->bit);
}

static const struct reset_control_ops a210_reset_ops = {
	.assert = a210_reset_assert,
	.deassert = a210_reset_deassert,
	.status = a210_reset_status,
};

static int a210_register_reset(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct a210_reset *priv = dev_get_drvdata(dev);

	priv->reset.ops = &a210_reset_ops;
	priv->reset.owner = THIS_MODULE;
	priv->reset.of_node = dev->of_node;
	priv->reset.of_reset_n_cells = 1;
	priv->reset.nr_resets = priv->subsys[0].num_signals;

	spin_lock_init(&priv->lock);

	return devm_reset_controller_register(dev, &priv->reset);
}

/*
 * Debugfs
 */
#if defined(CONFIG_DEBUG_FS)
static void str_to_lower(char *str)
{
	while (*str) {
		*str = tolower(*str);
		str++;
	}
}

static struct a210_rst_debugfs_node *alloc_debugfs_nodes(struct platform_device *pdev)
{
	int i;
	int signal_count = 0;

	struct a210_reset *priv = dev_get_drvdata(&pdev->dev);

	/* Get signal count */
	for (i = 0; i < priv->num_subsys; i++)
		signal_count += priv->subsys[i].num_signals;

	return devm_kzalloc(&pdev->dev,
			    sizeof(struct a210_rst_debugfs_node) * signal_count,
			    GFP_KERNEL);
}

static int a210_signal_status_show(struct seq_file *s, void *data)
{
	struct a210_rst_debugfs_node *node = (struct a210_rst_debugfs_node *)s->private;
	struct a210_rst_subsys *subsys = node->subsys;
	struct a210_rst_signal *signal = node->signal;
	int status = !(readl(subsys->base + signal->offset) & signal->bit);

	seq_printf(s, "%d\n", status);

	return 0;
}

static int a210_subsys_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, a210_signal_status_show, inode->i_private);
}

static const struct file_operations a210_subsys_status_fops = {
	.owner = THIS_MODULE,
	.open = a210_subsys_status_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void a210_reset_debugfs_remove(void *data)
{
	debugfs_remove_recursive(data);
}

static void a210_reset_debugfs_init(struct a210_reset *priv)
{
	struct dentry *entry;
	u32 i, j;
	char name[A210_RST_NAME_SIZE];
	char debugfs_entry_name[32] = "a210-reset";
	struct dentry *debugfs_entry;
	const char *addr_ptr;
	int signal_idx = 0;
	int ret;

	/* Multiple instance DTS node example
	 * rst1: reset-controller@1
	 */
	addr_ptr = strchr(of_node_full_name(priv->dev->of_node), '@');
	if (addr_ptr) {
		addr_ptr++;
		strcat(debugfs_entry_name, addr_ptr);
	}

	/* Create the main debugfs directory for a210 reset */
	debugfs_entry = debugfs_create_dir(debugfs_entry_name, NULL);
	if (IS_ERR(debugfs_entry)) {
		dev_err(priv->dev, "failed to create %s debugfs directory: %ld\n",
			debugfs_entry_name, PTR_ERR(debugfs_entry));
		return;
	}

	ret = devm_add_action_or_reset(priv->dev, a210_reset_debugfs_remove,
				       debugfs_entry);
	if (ret)
		return;

	/* Iterate through all subsys */
	for (i = 0; i < priv->num_subsys; i++) {
		/* Create signal files for each subsystem */
		for (j = 0; j < priv->subsys[i].num_signals; j++) {
			struct a210_rst_debugfs_node *node = &priv->debugfs_node[signal_idx];

			strscpy(name, priv->subsys[i].signals[j].name, sizeof(name));
			str_to_lower(name);

			node->signal = &priv->subsys[i].signals[j];
			node->subsys = &priv->subsys[i];
			entry = debugfs_create_file(name, 0444, debugfs_entry, node,
						    &a210_subsys_status_fops);

			signal_idx++;
			if (IS_ERR(entry)) {
				dev_err(priv->dev,
					"failed to create %s file in debugfs: %ld\n",
					name, PTR_ERR(entry));
				continue;
			}
		}
	}
}
#endif

/*
 * Probe
 */
static int a210_reset_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct a210_reset *priv;
	int ret;

	/* Alloc priv data */
	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	dev_set_drvdata(dev, priv);

	ret = a210_reset_init_subsys(pdev, of_device_get_match_data(dev));
	if (ret)
		goto fail;

#if defined(CONFIG_DEBUG_FS)
	/* Alloc debugfs nodes */
	priv->debugfs_node = alloc_debugfs_nodes(pdev);
	if (!priv->debugfs_node) {
		ret = -ENOMEM;
		goto fail;
	}
#endif

	ret = a210_register_reset(pdev);
	if (ret)
		goto fail;

#if defined(CONFIG_DEBUG_FS)
	a210_reset_debugfs_init(priv);
#endif
	dev_info(dev, "succeed to register a210 reset driver\n");

	return ret;

fail:
#if defined(CONFIG_DEBUG_FS)
	devm_kfree(dev, priv->debugfs_node);
#endif
	devm_kfree(dev, priv->subsys);
	devm_kfree(dev, priv);
	return ret;
}

static const struct of_device_id a210_reset_of_match[] = {
	{ .compatible = "zhihe,a210-vp-reset", .data = &a210_rst_match_data[VP_RST] },
	{ .compatible = "zhihe,a210-vi-reset", .data = &a210_rst_match_data[VI_RST] },
	{ .compatible = "zhihe,a210-npu-reset", .data = &a210_rst_match_data[NPU_RST] },
	{ .compatible = "zhihe,a210-vo-reset", .data = &a210_rst_match_data[VO_RST] },
	{ .compatible = "zhihe,a210-peri0-reset", .data = &a210_rst_match_data[PERI0_RST] },
	{ .compatible = "zhihe,a210-peri1-reset", .data = &a210_rst_match_data[PERI1_RST] },
	{ .compatible = "zhihe,a210-peri2-reset", .data = &a210_rst_match_data[PERI2_RST] },
	{ .compatible = "zhihe,a210-peri3-reset", .data = &a210_rst_match_data[PERI3_RST] },
	{ .compatible = "zhihe,a210-pcie-reset", .data = &a210_rst_match_data[PCIE_RST] },
	{ .compatible = "zhihe,a210-usb-reset", .data = &a210_rst_match_data[USB_RST] },
	{ .compatible = "zhihe,a210-tee-reset", .data = &a210_rst_match_data[TEE_RST] },
	{ .compatible = "zhihe,a210-gpu-reset", .data = &a210_rst_match_data[GPU_RST] },
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, a210_reset_of_match);

struct platform_driver a210_reset_driver = {
	.probe = a210_reset_probe,
	.driver = {
		.name = "a210-reset",
		.of_match_table = of_match_ptr(a210_reset_of_match),
	},
};

module_platform_driver(a210_reset_driver);

MODULE_AUTHOR("dong.yan <yand@zhcomputing.com>");
MODULE_DESCRIPTION("Zhihe A210 reset controller driver");
MODULE_LICENSE("GPL");
