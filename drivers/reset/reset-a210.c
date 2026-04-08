// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reset-controller.h>
#include <linux/io.h>
#include <dt-bindings/reset/a210-reset.h>

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
	spinlock_t lock;
#if defined(CONFIG_DEBUG_FS)
	struct a210_rst_debugfs_node *debugfs_node;
#endif
};

/*
 * Reset node info
 */
static struct a210_rst_signal vp_rst_signals[] = {
	[VP_VDEC_PRST & RST_SIGNAL_MASK] = {
		"VP_VDEC_PRST", 0, 0, BIT(0),
	},
	[VP_VDEC_CRST & RST_SIGNAL_MASK] = {
		"VP_VDEC_CRST", 0, 0, BIT(1),
	},
	[VP_VDEC_ARST & RST_SIGNAL_MASK] = {
		"VP_VDEC_ARST", 0, 0, BIT(2),
	},
	[VP_VENC_PRST & RST_SIGNAL_MASK] = {
		"VP_VENC_PRST", 0, 0, BIT(3),
	},
	[VP_VENC_CRST & RST_SIGNAL_MASK] = {
		"VP_VENC_CRST", 0, 0, BIT(4),
	},
	[VP_VENC_ARST & RST_SIGNAL_MASK] = {
		"VP_VENC_ARST", 0, 0, BIT(5),
	},
	[VP_G2D_PRST & RST_SIGNAL_MASK] = {
		"VP_G2D_PRST", 0, 0, BIT(6),
	},
	[VP_G2D_CRST & RST_SIGNAL_MASK] = {
		"VP_G2D_CRST", 0, 0, BIT(7),
	},
	[VP_G2D_ARST & RST_SIGNAL_MASK] = {
		"VP_G2D_ARST", 0, 0, BIT(8),
	},
	[VP_COMP_PRST & RST_SIGNAL_MASK] = {
		"VP_COMP_PRST", 0, 0, BIT(9),
	},
	[VP_COMP_CRST & RST_SIGNAL_MASK] = {
		"VP_COMP_CRST", 0, 0, BIT(10),
	},
	[VP_COMP_ARST & RST_SIGNAL_MASK] = {
		"VP_COMP_ARST", 0, 0, BIT(11),
	},
	[VP_DECOMP_PRST & RST_SIGNAL_MASK] = {
		"VP_DECOMP_PRST", 0, 0, BIT(12),
	},
	[VP_DECOMP_CRST & RST_SIGNAL_MASK] = {
		"VP_DECOMP_CRST", 0, 0, BIT(13),
	},
	[VP_DECOMP_ARST & RST_SIGNAL_MASK] = {
		"VP_DECOMP_ARST", 0, 0, BIT(14),
	},
	[VP_DFMU_PRST & RST_SIGNAL_MASK] = {
		"VP_DFMU_PRST", 0, 0, BIT(15),
	},
	[VP_DFMU_ARST & RST_SIGNAL_MASK] = {
		"VP_DFMU_ARST", 0, 0, BIT(16),
	},
	[VP_ARB_PRST & RST_SIGNAL_MASK] = {
		"VP_ARB_PRST", 0, 0, BIT(18),
	},
	[VP_ARB_ARST & RST_SIGNAL_MASK] = {
		"VP_ARB_ARST", 0, 0, BIT(19),
	},
	[VP_VDEC_RS_ARST & RST_SIGNAL_MASK] = {
		"VP_VDEC_RS_ARST", 0, 0, BIT(20),
	},
	[VP_VENC_RS_ARST & RST_SIGNAL_MASK] = {
		"VP_VENC_RS_ARST", 0, 0, BIT(21),
	},
	[VP_COMP_EXTPRST & RST_SIGNAL_MASK] = {
		"VP_COMP_EXTPRST", 0, 0, BIT(22),
	},
	[VP_DECOMP_EXTPRST & RST_SIGNAL_MASK] = {
		"VP_DECOMP_EXTPRST", 0, 0, BIT(23),
	},
};

static struct a210_rst_signal vi_rst_signals[] = {
	[VI_MIPI0_CSI0_PRST & RST_SIGNAL_MASK] = {
		"VI_MIPI0_CSI0_PRST", 0, 0, BIT(0),
	},
	[VI_MIPI0_CSI1_PRST & RST_SIGNAL_MASK] = {
		"VI_MIPI0_CSI1_PRST", 0, 0, BIT(1),
	},
	[VI_MIPI0_FIFO_RST & RST_SIGNAL_MASK] = {
		"VI_MIPI0_FIFO_RST", 0, 0, BIT(2),
	},
	[VI_MIPI1_CSI0_PRST & RST_SIGNAL_MASK] = {
		"VI_MIPI1_CSI0_PRST", 0, 0, BIT(3),
	},
	[VI_MIPI1_CSI1_PRST & RST_SIGNAL_MASK] = {
		"VI_MIPI1_CSI1_PRST", 0, 0, BIT(4),
	},
	[VI_MIPI1_FIFO_RST & RST_SIGNAL_MASK] = {
		"VI_MIPI1_FIFO_RST", 0, 0, BIT(5),
	},
	[VI_VIPRE_PRST & RST_SIGNAL_MASK] = {
		"VI_VIPRE_PRST", 0, 0, BIT(6),
	},
	[VI_VIPRE_I0_PIX_RST & RST_SIGNAL_MASK] = {
		"VI_VIPRE_I0_PIX_RST", 0, 0, BIT(7),
	},
	[VI_VIPRE_I1_PIX_RST & RST_SIGNAL_MASK] = {
		"VI_VIPRE_I1_PIX_RST", 0, 0, BIT(8),
	},
	[VI_VIPRE_ISPIF_RST & RST_SIGNAL_MASK] = {
		"VI_VIPRE_ISPIF_RST", 0, 0, BIT(9),
	},
	[VI_VIPRE_ARST & RST_SIGNAL_MASK] = {
		"VI_VIPRE_ARST", 0, 0, BIT(10),
	},
	[VI_ISP_RST & RST_SIGNAL_MASK] = {
		"VI_ISP_RST", 0, 0, BIT(11),
	},
	[VI_COMP_PRST & RST_SIGNAL_MASK] = {
		"VI_COMP_PRST", 0, 0, BIT(12),
	},
	[VI_COMP_ARST & RST_SIGNAL_MASK] = {
		"VI_COMP_ARST", 0, 0, BIT(13),
	},
	[VI_COMP_ISPOUT_RST & RST_SIGNAL_MASK] = {
		"VI_COMP_ISPOUT_RST", 0, 0, BIT(14),
	},
	[VI_COMP_DECOUT_RST & RST_SIGNAL_MASK] = {
		"VI_COMP_DECOUT_RST", 0, 0, BIT(15),
	},
	[VI_COMP_VSEOUT_RST & RST_SIGNAL_MASK] = {
		"VI_COMP_VSEOUT_RST", 0, 0, BIT(16),
	},
	[VI_COMP0_RST & RST_SIGNAL_MASK] = {
		"VI_COMP0_RST", 0, 0, BIT(17),
	},
	[VI_COMP1_RST & RST_SIGNAL_MASK] = {
		"VI_COMP1_RST", 0, 0, BIT(18),
	},
	[VI_DECOMP_RST & RST_SIGNAL_MASK] = {
		"VI_DECOMP_RST", 0, 0, BIT(19),
	},
	[VI_DW200_RST & RST_SIGNAL_MASK] = {
		"VI_DW200_RST", 0, 0, BIT(20),
	},
	[VI_DFMU_PRST & RST_SIGNAL_MASK] = {
		"VI_DFMU_PRST", 0, 0, BIT(21),
	},
	[VI_DFMU_ARST & RST_SIGNAL_MASK] = {
		"VI_DFMU_ARST", 0, 0, BIT(22),
	},
	[VI_ARB_PRST & RST_SIGNAL_MASK] = {
		"VI_ARB_PRST", 0, 0, BIT(23),
	},
	[VI_ARB_ARST & RST_SIGNAL_MASK] = {
		"VI_ARB_ARST", 0, 0, BIT(24),
	},
	[VI_X2H0_HRST & RST_SIGNAL_MASK] = {
		"VI_X2H0_HRST", 0, 0, BIT(26),
	},
	[VI_X2H1_HRST & RST_SIGNAL_MASK] = {
		"VI_X2H1_HRST", 0, 0, BIT(27),
	},
	[VI_X2H2_HRST & RST_SIGNAL_MASK] = {
		"VI_X2H2_HRST", 0, 0, BIT(28),
	},
	[VI_REC_PRST & RST_SIGNAL_MASK] = {
		"VI_REC_PRST", 0, 0, BIT(29),
	},
	[VI_REC_ARST & RST_SIGNAL_MASK] = {
		"VI_REC_ARST", 0, 0, BIT(30),
	},
};

static struct a210_rst_signal npu_rst_signals[] = {
	[NPU_AAB_MST_RST & RST_SIGNAL_MASK] = {
		"NPU_AAB_MST_RST", 0, 0, BIT(0),
	},
	[NPU_AFENCE_RST & RST_SIGNAL_MASK] = {
		"NPU_AFENCE_RST", 0, 0, BIT(1),
	},
	[NPU_AXI_CCU_PRST & RST_SIGNAL_MASK] = {
		"NPU_AXI_CCU_PRST", 0, 0, BIT(2),
	},
	[NPU_AXI_CCU_RST & RST_SIGNAL_MASK] = {
		"NPU_AXI_CCU_RST", 0, 0, BIT(3),
	},
	[NPU_AXI_M2S1_RST & RST_SIGNAL_MASK] = {
		"NPU_AXI_M2S1_RST", 0, 0, BIT(4),
	},
	[NPU_AXI_RST & RST_SIGNAL_MASK] = {
		"NPU_AXI_RST", 0, 0, BIT(5),
	},
	[NPU_CLK_CALC_RST & RST_SIGNAL_MASK] = {
		"NPU_CLK_CALC_RST", 0, 0, BIT(6),
	},
	[NPU_DFMU_ARST & RST_SIGNAL_MASK] = {
		"NPU_DFMU_ARST", 0, 0, BIT(7),
	},
	[NPU_DFMU_PRST & RST_SIGNAL_MASK] = {
		"NPU_DFMU_PRST", 0, 0, BIT(8),
	},
	[NPU_LPC_C_RST & RST_SIGNAL_MASK] = {
		"NPU_LPC_C_RST", 0, 0, BIT(9),
	},
	[NPU_LPE_C_PRST & RST_SIGNAL_MASK] = {
		"NPU_LPE_C_PRST", 0, 0, BIT(10),
	},
	[NPU_LPE_C_RST & RST_SIGNAL_MASK] = {
		"NPU_LPE_C_RST", 0, 0, BIT(11),
	},
	[NPU_ARST & RST_SIGNAL_MASK] = {
		"NPU_ARST", 0, 0, BIT(12),
	},
	[NPU_CRST & RST_SIGNAL_MASK] = {
		"NPU_CRST", 0, 0, BIT(13),
	},
	[NPU_HRST & RST_SIGNAL_MASK] = {
		"NPU_HRST", 0, 0, BIT(14),
	},
	[NPU_PCTRL_PB_RST & RST_SIGNAL_MASK] = {
		"NPU_PCTRL_PB_RST", 0, 0, BIT(15),
	},
	[NPU_PCTRL_PRST & RST_SIGNAL_MASK] = {
		"NPU_PCTRL_PRST", 0, 0, BIT(16),
	},
	[NPU_PCTRL_RST & RST_SIGNAL_MASK] = {
		"NPU_PCTRL_RST", 0, 0, BIT(17),
	},
	[NPU_PTW_CCU_PRST & RST_SIGNAL_MASK] = {
		"NPU_PTW_CCU_PRST", 0, 0, BIT(18),
	},
	[NPU_PTW_CCU_RST & RST_SIGNAL_MASK] = {
		"NPU_PTW_CCU_RST", 0, 0, BIT(19),
	},
	[NPU_SEMA_ARST & RST_SIGNAL_MASK] = {
		"NPU_SEMA_ARST", 0, 0, BIT(20),
	},
	[NPU_SEMA_PRST & RST_SIGNAL_MASK] = {
		"NPU_SEMA_PRST", 0, 0, BIT(21),
	},
	[NPU_DROOP_CCTRL_PRST & RST_SIGNAL_MASK] = {
		"NPU_DROOP_CCTRL_PRST", 0, 0, BIT(22),
	},
	[NPU_DROOP_CCTRL_RST & RST_SIGNAL_MASK] = {
		"NPU_DROOP_CCTRL_RST", 0, 0, BIT(23),
	},
	[NPU_X2H_RST & RST_SIGNAL_MASK] = {
		"NPU_X2H_RST", 0, 0, BIT(24),
	},
	[NPU_X2P_RST & RST_SIGNAL_MASK] = {
		"NPU_X2P_RST", 0, 0, BIT(25),
	},
	[NPU_LDIP_RST & RST_SIGNAL_MASK] = {
		"NPU_LDIP_RST", 0, 0, BIT(26),
	},
	[NPU_SRAM_RST & RST_SIGNAL_MASK] = {
		"NPU_SRAM_RST", 0, 0, BIT(27),
	},
};

static struct a210_rst_signal vo_rst_signals[] = {
	[VO_X2H0_RST & RST_SIGNAL_MASK] = {
		"VO_X2H0_RST", 0, 0, BIT(0),
	},
	[VO_X2H1_RST & RST_SIGNAL_MASK] = {
		"VO_X2H1_RST", 0, 0, BIT(1),
	},
	[VO_DPU_HRST & RST_SIGNAL_MASK] = {
		"VO_DPU_HRST", 0, 0, BIT(2),
	},
	[VO_DPU_CRST & RST_SIGNAL_MASK] = {
		"VO_DPU_CRST", 0, 0, BIT(3),
	},
	[VO_DPU_ARST & RST_SIGNAL_MASK] = {
		"VO_DPU_ARST", 0, 0, BIT(4),
	},
	[VO_AUXDISP_PRST & RST_SIGNAL_MASK] = {
		"VO_AUXDISP_PRST", 0, 0, BIT(5),
	},
	[VO_AUXDISP_PIX_RST & RST_SIGNAL_MASK] = {
		"VO_AUXDISP_PIX_RST", 0, 0, BIT(6),
	},
	[VO_AUXDISP_ARST & RST_SIGNAL_MASK] = {
		"VO_AUXDISP_ARST", 0, 0, BIT(7),
	},
	[VO_HDMI_PRST & RST_SIGNAL_MASK] = {
		"VO_HDMI_PRST", 0, 0, BIT(8),
	},
	[VO_HDMI_MAIN_RST & RST_SIGNAL_MASK] = {
		"VO_HDMI_MAIN_RST", 0, 0, BIT(9),
	},
	[VO_MIPI_PRST & RST_SIGNAL_MASK] = {
		"VO_MIPI_PRST", 0, 0, BIT(10),
	},
	[VO_DECOMP_PRST & RST_SIGNAL_MASK] = {
		"VO_DECOMP_PRST", 0, 0, BIT(11),
	},
	[VO_DECOMP0_CRST & RST_SIGNAL_MASK] = {
		"VO_DECOMP0_CRST", 0, 0, BIT(12),
	},
	[VO_DECOMP1_CRST & RST_SIGNAL_MASK] = {
		"VO_DECOMP1_CRST", 0, 0, BIT(13),
	},
	[VO_DECOMP_ARST & RST_SIGNAL_MASK] = {
		"VO_DECOMP_ARST", 0, 0, BIT(14),
	},
	[VO_ARB_PRST & RST_SIGNAL_MASK] = {
		"VO_ARB_PRST", 0, 0, BIT(15),
	},
	[VO_ARB_ARST & RST_SIGNAL_MASK] = {
		"VO_ARB_ARST", 0, 0, BIT(16),
	},
	[VO_DFMU_PRST & RST_SIGNAL_MASK] = {
		"VO_DFMU_PRST", 0, 0, BIT(17),
	},
	[VO_DFMU_ARST & RST_SIGNAL_MASK] = {
		"VO_DFMU_ARST", 0, 0, BIT(18),
	},
};

static struct a210_rst_signal peri0_rst_signals[] = {
	[PERI0_TIMER0_CRST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER0_CRST", 0, 0, BIT(0),
	},
	[PERI0_TIMER0_PRST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER0_PRST", 0, 0, BIT(1),
	},
	[PERI0_TIMER1_CRST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER1_CRST", 0, 0, BIT(2),
	},
	[PERI0_TIMER1_PRST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER1_PRST", 0, 0, BIT(3),
	},
	[PERI0_WDT0_PRST & RST_SIGNAL_MASK] = {
		"PERI0_WDT0_PRST", 0, 0, BIT(4),
	},
	[PERI0_MBOX0_PRST & RST_SIGNAL_MASK] = {
		"PERI0_MBOX0_PRST", 0, 0, BIT(5),
	},
	[PERI0_MBOX1_PRST & RST_SIGNAL_MASK] = {
		"PERI0_MBOX1_PRST", 0, 0, BIT(6),
	},
	[PERI0_TIMER0_RST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER0_RST", 0, 0, BIT(0) | BIT(1),
	},
	[PERI0_TIMER1_RST & RST_SIGNAL_MASK] = {
		"PERI0_TIMER1_RST", 0, 0, BIT(2) | BIT(3),
	},
};

static struct a210_rst_signal peri1_rst_signals[] = {
	[PERI1_GMAC0_ARST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC0_ARST", 0, 0, BIT(0),
	},
	[PERI1_GMAC0_HRST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC0_HRST", 0, 0, BIT(1),
	},
	[PERI1_GMAC1_ARST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC1_ARST", 0, 0, BIT(2),
	},
	[PERI1_GMAC1_HRST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC1_HRST", 0, 0, BIT(3),
	},
	[PERI1_GPIO0_DBRST & RST_SIGNAL_MASK] = {
		"PERI1_GPIO0_DBRST", 0, 0, BIT(4),
	},
	[PERI1_GPIO0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_GPIO0_PRST", 0, 0, BIT(5),
	},
	[PERI1_GPIO1_DBRST & RST_SIGNAL_MASK] = {
		"PERI1_GPIO1_DBRST", 0, 0, BIT(6),
	},
	[PERI1_GPIO1_PRST & RST_SIGNAL_MASK] = {
		"PERI1_GPIO1_PRST", 0, 0, BIT(7),
	},
	[PERI1_I2C0_IC_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C0_IC_RST", 0, 0, BIT(8),
	},
	[PERI1_I2C0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_I2C0_PRST", 0, 0, BIT(9),
	},
	[PERI1_I2C1_IC_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C1_IC_RST", 0, 0, BIT(10),
	},
	[PERI1_I2C1_PRST & RST_SIGNAL_MASK] = {
		"PERI1_I2C1_PRST", 0, 0, BIT(11),
	},
	[PERI1_I2C2_IC_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C2_IC_RST", 0, 0, BIT(12),
	},
	[PERI1_I2C2_PRST & RST_SIGNAL_MASK] = {
		"PERI1_I2C2_PRST", 0, 0, BIT(13),
	},
	[PERI1_I2S0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_I2S0_PRST", 0, 0, BIT(14),
	},
	[PERI1_MST_BUS_ARST & RST_SIGNAL_MASK] = {
		"PERI1_MST_BUS_ARST", 0, 0, BIT(15),
	},
	[PERI1_MST_BUS_PRST & RST_SIGNAL_MASK] = {
		"PERI1_MST_BUS_PRST", 0, 0, BIT(16),
	},
	[PERI1_PWM0_CRST & RST_SIGNAL_MASK] = {
		"PERI1_PWM0_CRST", 0, 0, BIT(17),
	},
	[PERI1_PWM0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_PWM0_PRST", 0, 0, BIT(18),
	},
	[PERI1_QSPI0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_QSPI0_PRST", 0, 0, BIT(19),
	},
	[PERI1_QSPI0_SSI_RST & RST_SIGNAL_MASK] = {
		"PERI1_QSPI0_SSI_RST", 0, 0, BIT(20),
	},
	[PERI1_SPI0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_SPI0_PRST", 0, 0, BIT(21),
	},
	[PERI1_SPI0_SSI_RST & RST_SIGNAL_MASK] = {
		"PERI1_SPI0_SSI_RST", 0, 0, BIT(22),
	},
	[PERI1_UART0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_UART0_PRST", PERI1_UART0_S_RST, 0, BIT(23),
	},
	[PERI1_UART0_S_RST & RST_SIGNAL_MASK] = {
		"PERI1_UART0_S_RST", 0, 0, BIT(24),
	},
	[PERI1_UART1_PRST & RST_SIGNAL_MASK] = {
		"PERI1_UART1_PRST", PERI1_UART1_S_RST, 0, BIT(25),
	},
	[PERI1_UART1_S_RST & RST_SIGNAL_MASK] = {
		"PERI1_UART1_S_RST", 0, 0, BIT(26),
	},
	[PERI1_UART2_PRST & RST_SIGNAL_MASK] = {
		"PERI1_UART2_PRST", PERI1_UART2_S_RST, 0, BIT(27),
	},
	[PERI1_UART2_S_RST & RST_SIGNAL_MASK] = {
		"PERI1_UART2_S_RST", 0, 0, BIT(28),
	},
	[PERI1_UART3_PRST & RST_SIGNAL_MASK] = {
		"PERI1_UART3_PRST", PERI1_UART3_S_RST, 0, BIT(29),
	},
	[PERI1_UART3_S_RST & RST_SIGNAL_MASK] = {
		"PERI1_UART3_S_RST", 0, 0, BIT(30),
	},
	[PERI1_X2H_GMAC0_ARST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC0_ARST", 0, 0, BIT(31),
	},
	[PERI1_X2H_GMAC0_HRST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC0_HRST", 0, 0x4, BIT(0),
	},
	[PERI1_X2H_GMAC1_ARST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC1_ARST", 0, 0x4, BIT(1),
	},
	[PERI1_X2H_GMAC1_HRST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC1_HRST", 0, 0x4, BIT(2),
	},
	[PERI1_DFMU_ARESET & RST_SIGNAL_MASK] = {
		"PERI1_DFMU_ARESET", 0, 0x4, BIT(3),
	},
	[PERI1_DFMU_PRESET & RST_SIGNAL_MASK] = {
		"PERI1_DFMU_PRESET", 0, 0x4, BIT(4),
	},
	[PERI1_PAD_CTRL_PRST & RST_SIGNAL_MASK] = {
		"PERI1_PAD_CTRL_PRST", 0, 0x4, BIT(5),
	},
	[PERI1_PDM0_MRST & RST_SIGNAL_MASK] = {
		"PERI1_PDM0_MRST", 0, 0x4, BIT(6),
	},
	[PERI1_PDM0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_PDM0_PRST", 0, 0x4, BIT(7),
	},
	[PERI1_TDM0_RST & RST_SIGNAL_MASK] = {
		"PERI1_TDM0_RST", 0, 0x4, BIT(8),
	},
	[PERI1_CAN0_IPG_PE_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN0_IPG_PE_RST", 0, 0x4, BIT(9),
	},
	[PERI1_CAN0_IPG_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN0_IPG_RST", 0, 0x4, BIT(10),
	},
	[PERI1_CAN0_IPG_SOFT_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN0_IPG_SOFT_RST", 0, 0x4, BIT(11),
	},
	[PERI1_CAN0_IPG_TS_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN0_IPG_TS_RST", 0, 0x4, BIT(12),
	},
	[PERI1_CAN0_PRST & RST_SIGNAL_MASK] = {
		"PERI1_CAN0_PRST", 0, 0x4, BIT(13),
	},
	[PERI1_CAN1_IPG_PE_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN1_IPG_PE_RST", 0, 0x4, BIT(14),
	},
	[PERI1_CAN1_IPG_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN1_IPG_RST", 0, 0x4, BIT(15),
	},
	[PERI1_CAN1_IPG_SOFT_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN1_IPG_SOFT_RST", 0, 0x4, BIT(16),
	},
	[PERI1_CAN1_IPG_TS_RST & RST_SIGNAL_MASK] = {
		"PERI1_CAN1_IPG_TS_RST", 0, 0x4, BIT(17),
	},
	[PERI1_CAN1_PRST & RST_SIGNAL_MASK] = {
		"PERI1_CAN1_PRST", 0, 0x4, BIT(18),
	},
	[PERI1_CHIP_DBG_ARST & RST_SIGNAL_MASK] = {
		"PERI1_CHIP_DBG_ARST", 0, 0x4, BIT(19),
	},
	[PERI1_CHIP_DBG_CRST & RST_SIGNAL_MASK] = {
		"PERI1_CHIP_DBG_CRST", 0, 0x4, BIT(20),
	},
	[PERI1_CHIP_DBG_PRST & RST_SIGNAL_MASK] = {
		"PERI1_CHIP_DBG_PRST", 0, 0x4, BIT(21),
	},
	[PERI1_GMAC_CRST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC_CRST", 0, 0x4, BIT(22),
	},
	[PERI1_X2H_GMAC2_ARST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC2_ARST", 0, 0x4, BIT(23),
	},
	[PERI1_X2H_GMAC2_HRST & RST_SIGNAL_MASK] = {
		"PERI1_X2H_GMAC2_HRST", 0, 0x4, BIT(24),
	},
	[PERI1_ZGMAC_X2X_ARST & RST_SIGNAL_MASK] = {
		"PERI1_ZGMAC_X2X_ARST", 0, 0x4, BIT(25),
	},
	[PERI1_SPI0_RST & RST_SIGNAL_MASK] = {
		"PERI1_SPI0_RST", 0, 0, BIT(21) | BIT(22),
	},
	[PERI1_I2C0_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C0_RST", 0, 0, BIT(8) | BIT(9),
	},
	[PERI1_I2C1_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C1_RST", 0, 0, BIT(10) | BIT(11),
	},
	[PERI1_I2C2_RST & RST_SIGNAL_MASK] = {
		"PERI1_I2C2_RST", 0, 0, BIT(12) | BIT(13),
	},
	[PERI1_GMAC0_RST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC0_RST", 0, 0, BIT(0) | BIT(1) | BIT(31),
	},
	[PERI1_GMAC0_X2H_RST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC0_X2H_RST", 0, 0x4, BIT(0),
	},
	[PERI1_GMAC1_RST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC1_RST", 0, 0, BIT(2) | BIT(3),
	},
	[PERI1_GMAC1_X2H_RST & RST_SIGNAL_MASK] = {
		"PERI1_GMAC1_X2H_RST", 0, 0x4, BIT(1) | BIT(2),
	},
};

static struct a210_rst_signal peri2_rst_signals[] = {
	[PERI2_CAN2_IPG_PE_RST & RST_SIGNAL_MASK] = {
		"PERI2_CAN2_IPG_PE_RST", 0, 0, BIT(0),
	},
	[PERI2_CAN2_IPG_RST & RST_SIGNAL_MASK] = {
		"PERI2_CAN2_IPG_RST", 0, 0, BIT(1),
	},
	[PERI2_CAN2_IPG_SOFT_RST & RST_SIGNAL_MASK] = {
		"PERI2_CAN2_IPG_SOFT_RST", 0, 0, BIT(2),
	},
	[PERI2_CAN2_IPG_TS_RST & RST_SIGNAL_MASK] = {
		"PERI2_CAN2_IPG_TS_RST", 0, 0, BIT(3),
	},
	[PERI2_CAN2_PRST & RST_SIGNAL_MASK] = {
		"PERI2_CAN2_PRST", 0, 0, BIT(4),
	},
	[PERI2_GPIO2_DBRST & RST_SIGNAL_MASK] = {
		"PERI2_GPIO2_DBRST", 0, 0, BIT(5),
	},
	[PERI2_GPIO2_PRST & RST_SIGNAL_MASK] = {
		"PERI2_GPIO2_PRST", 0, 0, BIT(6),
	},
	[PERI2_I2C4_IC_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C4_IC_RST", 0, 0, BIT(7),
	},
	[PERI2_I2C4_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2C4_PRST", 0, 0, BIT(8),
	},
	[PERI2_I2S2_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2S2_PRST", 0, 0, BIT(9),
	},
	[PERI2_SPI1_PRST & RST_SIGNAL_MASK] = {
		"PERI2_SPI1_PRST", 0, 0, BIT(10),
	},
	[PERI2_SPI1_SSI_RST & RST_SIGNAL_MASK] = {
		"PERI2_SPI1_SSI_RST", 0, 0, BIT(11),
	},
	[PERI2_UART4_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART4_PRST", PERI2_UART4_S_RST, 0, BIT(12),
	},
	[PERI2_UART4_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART4_S_RST", 0, 0, BIT(13),
	},
	[PERI2_UART5_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART5_PRST", PERI2_UART5_S_RST, 0, BIT(14),
	},
	[PERI2_UART5_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART5_S_RST", 0, 0, BIT(15),
	},
	[PERI2_UART6_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART6_PRST", PERI2_UART6_S_RST, 0, BIT(16),
	},
	[PERI2_UART6_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART6_S_RST", 0, 0, BIT(17),
	},
	[PERI2_PAD_CTRL_PRST & RST_SIGNAL_MASK] = {
		"PERI2_PAD_CTRL_PRST", 0, 0, BIT(18),
	},
	[PERI2_GPIO3_DBRST & RST_SIGNAL_MASK] = {
		"PERI2_GPIO3_DBRST", 0, 0, BIT(19),
	},
	[PERI2_GPIO3_PRST & RST_SIGNAL_MASK] = {
		"PERI2_GPIO3_PRST", 0, 0, BIT(20),
	},
	[PERI2_I2C3_IC_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C3_IC_RST", 0, 0, BIT(21),
	},
	[PERI2_I2C3_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2C3_PRST", 0, 0, BIT(22),
	},
	[PERI2_I2C5_IC_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C5_IC_RST", 0, 0, BIT(23),
	},
	[PERI2_I2C5_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2C5_PRST", 0, 0, BIT(24),
	},
	[PERI2_I2C6_IC_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C6_IC_RST", 0, 0, BIT(25),
	},
	[PERI2_I2C6_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2C6_PRST", 0, 0, BIT(26),
	},
	[PERI2_I2C7_IC_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C7_IC_RST", 0, 0, BIT(27),
	},
	[PERI2_I2C7_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2C7_PRST", 0, 0, BIT(28),
	},
	[PERI2_I2S1_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2S1_PRST", 0, 0, BIT(29),
	},
	[PERI2_I2S3_PRST & RST_SIGNAL_MASK] = {
		"PERI2_I2S3_PRST", 0, 0, BIT(30),
	},
	[PERI2_UART7_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART7_PRST", PERI2_UART7_S_RST, 0, BIT(31),
	},
	[PERI2_UART7_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART7_S_RST", 0, 0x4, BIT(0),
	},
	[PERI2_UART8_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART8_PRST", PERI2_UART8_S_RST, 0x4, BIT(1),
	},
	[PERI2_UART8_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART8_S_RST", 0, 0x4, BIT(2),
	},
	[PERI2_UART9_PRST & RST_SIGNAL_MASK] = {
		"PERI2_UART9_PRST", PERI2_UART9_S_RST, 0x4, BIT(3),
	},
	[PERI2_UART9_S_RST & RST_SIGNAL_MASK] = {
		"PERI2_UART9_S_RST", 0, 0x4, BIT(4),
	},
	[PERI2_QSPI1_PRST & RST_SIGNAL_MASK] = {
		"PERI2_QSPI1_PRST", 0, 0x4, BIT(5),
	},
	[PERI2_QSPI1_SSI_RST & RST_SIGNAL_MASK] = {
		"PERI2_QSPI1_SSI_RST", 0, 0x4, BIT(6),
	},
	[PERI2_PWM1_CRST & RST_SIGNAL_MASK] = {
		"PERI2_PWM1_CRST", 0, 0x4, BIT(7),
	},
	[PERI2_PWM1_PRST & RST_SIGNAL_MASK] = {
		"PERI2_PWM1_PRST", 0, 0x4, BIT(8),
	},
	[PERI2_PWM2_CRST & RST_SIGNAL_MASK] = {
		"PERI2_PWM2_CRST", 0, 0x4, BIT(9),
	},
	[PERI2_PWM2_PRST & RST_SIGNAL_MASK] = {
		"PERI2_PWM2_PRST", 0, 0x4, BIT(10),
	},
	[PERI2_I2C3_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C3_RST", 0, 0, BIT(21) | BIT(22),
	},
	[PERI2_I2C4_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C4_RST", 0, 0, BIT(7) | BIT(8),
	},
	[PERI2_I2C5_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C5_RST", 0, 0, BIT(23) | BIT(24),
	},
	[PERI2_I2C6_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C6_RST", 0, 0, BIT(25) | BIT(26),
	},
	[PERI2_I2C7_RST & RST_SIGNAL_MASK] = {
		"PERI2_I2C7_RST", 0, 0, BIT(27) | BIT(28),
	},
	[PERI2_SPI1_RST & RST_SIGNAL_MASK] = {
		"PERI2_SPI1_RST", 0, 0, BIT(10) | BIT(11),
	},
};

static struct a210_rst_signal peri3_rst_signals[] = {
	[PERI3_DMAC_ARST & RST_SIGNAL_MASK] = {
		"PERI3_DMAC_ARST", 0, 0, BIT(0),
	},
	[PERI3_DMAC_HRST & RST_SIGNAL_MASK] = {
		"PERI3_DMAC_HRST", 0, 0, BIT(1),
	},
	[PERI3_EMMC_SDIO_CLKGEN_RST & RST_SIGNAL_MASK] = {
		"PERI3_EMMC_SDIO_CLKGEN_RST", 0, 0, BIT(2),
	},
	[PERI3_EMMC_RST & RST_SIGNAL_MASK] = {
		"PERI3_EMMC_RST", 0, 0, BIT(3),
	},
	[PERI3_EMMC_X2X_ARST_M & RST_SIGNAL_MASK] = {
		"PERI3_EMMC_X2X_ARST_M", 0, 0, BIT(4),
	},
	[PERI3_EMMC_X2X_ARST_S & RST_SIGNAL_MASK] = {
		"PERI3_EMMC_X2X_ARST_S", 0, 0, BIT(5),
	},
	[PERI3_SDIO_RST & RST_SIGNAL_MASK] = {
		"PERI3_SDIO_RST", 0, 0, BIT(6),
	},
	[PERI3_SDIO_X2X_ARST_M & RST_SIGNAL_MASK] = {
		"PERI3_SDIO_X2X_ARST_M", 0, 0, BIT(7),
	},
	[PERI3_SDIO_X2X_ARST_S & RST_SIGNAL_MASK] = {
		"PERI3_SDIO_X2X_ARST_S", 0, 0, BIT(8),
	},
	[PERI3_AXI_MST_ARST & RST_SIGNAL_MASK] = {
		"PERI3_AXI_MST_ARST", 0, 0, BIT(9),
	},
	[PERI3_AXI_MST_PRST & RST_SIGNAL_MASK] = {
		"PERI3_AXI_MST_PRST", 0, 0, BIT(10),
	},
	[PERI3_ADC_PRST & RST_SIGNAL_MASK] = {
		"PERI3_ADC_PRST", 0, 0, BIT(11),
	},
	[PERI3_TEE_X2X_ARST_M & RST_SIGNAL_MASK] = {
		"PERI3_TEE_X2X_ARST_M", 0, 0, BIT(12),
	},
	[PERI3_TEE_X2X_ARST_S & RST_SIGNAL_MASK] = {
		"PERI3_TEE_X2X_ARST_S", 0, 0, BIT(13),
	},
	[PERI3_TEE_H2H_HRST & RST_SIGNAL_MASK] = {
		"PERI3_TEE_H2H_HRST", 0, 0, BIT(14),
	},
	[PERI3_GPIO4_DBRST & RST_SIGNAL_MASK] = {
		"PERI3_GPIO4_DBRST", 0, 0, BIT(15),
	},
	[PERI3_GPIO4_PRST & RST_SIGNAL_MASK] = {
		"PERI3_GPIO4_PRST", 0, 0, BIT(16),
	},
	[PERI3_PAD_CTRL_PRST & RST_SIGNAL_MASK] = {
		"PERI3_PAD_CTRL_PRST", 0, 0, BIT(17),
	},
};

static struct a210_rst_signal pcie_rst_signals[] = {
	[PCIE_X2X_PERI_SLV_ARST & RST_SIGNAL_MASK] = {
		"PCIE_X2X_PERI_SLV_ARST", 0, 0, BIT(0),
	},
	[PCIE_X2X_PERI_MST_ARST & RST_SIGNAL_MASK] = {
		"PCIE_X2X_PERI_MST_ARST", 0, 0, BIT(4),
	},
	[PCIE_AXI4_PCIE_MST_ARST & RST_SIGNAL_MASK] = {
		"PCIE_AXI4_PCIE_MST_ARST", 0, 0, BIT(8),
	},
	[PCIE_AXI4_PCIE_MST_PRST & RST_SIGNAL_MASK] = {
		"PCIE_AXI4_PCIE_MST_PRST", 0, 0, BIT(12),
	},
	[PCIE_DFMU_PRST & RST_SIGNAL_MASK] = {
		"PCIE_DFMU_PRST", 0, 0x4, BIT(0),
	},
	[PCIE_DFMU_ARST & RST_SIGNAL_MASK] = {
		"PCIE_DFMU_ARST", 0, 0x4, BIT(4),
	},
	[PCIE_E16PHY_PHY_RST & RST_SIGNAL_MASK] = {
		"PCIE_E16PHY_PHY_RST", 0, 0x8, BIT(0),
	},
	[PCIE_E16PHY_APBS_PRST & RST_SIGNAL_MASK] = {
		"PCIE_E16PHY_APBS_PRST", 0, 0x8, BIT(4),
	},
	[PCIE_X2X_SATA_MST_ARST & RST_SIGNAL_MASK] = {
		"PCIE_X2X_SATA_MST_ARST", 0, 0x10, BIT(0),
	},
	[PCIE_X2X_SATA_SLV_ARST & RST_SIGNAL_MASK] = {
		"PCIE_X2X_SATA_SLV_ARST", 0, 0x10, BIT(4),
	},
	[PCIE_SATA_ARESET & RST_SIGNAL_MASK] = {
		"PCIE_SATA_ARESET", 0, 0x10, BIT(8),
	},
	[PCIE_SATA_RST_PMALIVE & RST_SIGNAL_MASK] = {
		"PCIE_SATA_RST_PMALIVE", 0, 0x10, BIT(12),
	},
	[PCIE_SATA_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_SATA_SLV_AFENCE_ARST", 0, 0x10, BIT(16),
	},
	[PCIE_SATA_MST_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_SATA_MST_AFENCE_ARST", 0, 0x10, BIT(20),
	},
	[PCIE_SATA_RST_ASIC0 & RST_SIGNAL_MASK] = {
		"PCIE_SATA_RST_ASIC0", 0, 0x14, BIT(0),
	},
	[PCIE_SATA_RST_ASIC1 & RST_SIGNAL_MASK] = {
		"PCIE_SATA_RST_ASIC1", 0, 0x14, BIT(4),
	},
	[PCIE_SATA_RST_RXOOB0 & RST_SIGNAL_MASK] = {
		"PCIE_SATA_RST_RXOOB0", 0, 0x14, BIT(8),
	},
	[PCIE_SATA_RST_RXOOB1 & RST_SIGNAL_MASK] = {
		"PCIE_SATA_RST_RXOOB1", 0, 0x14, BIT(12),
	},
	[PCIE_DM_GEN3X4_APBS_PRST & RST_SIGNAL_MASK] = {
		"PCIE_DM_GEN3X4_APBS_PRST", 0, 0x20, BIT(0),
	},
	[PCIE_DM_GEN3X4_POWER_UP_RST & RST_SIGNAL_MASK] = {
		"PCIE_DM_GEN3X4_POWER_UP_RST", 0, 0x20, BIT(4),
	},
	[PCIE_DM_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_DM_SLV_AFENCE_ARST", 0, 0x20, BIT(16),
	},
	[PCIE_DM_MST_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_DM_MST_AFENCE_ARST", 0, 0x20, BIT(20),
	},
	[PCIE_RP_GEN3X1_APBS_PRST & RST_SIGNAL_MASK] = {
		"PCIE_RP_GEN3X1_APBS_PRST", 0, 0x24, BIT(0),
	},
	[PCIE_RP_GEN3X1_POWER_UP_RST & RST_SIGNAL_MASK] = {
		"PCIE_RP_GEN3X1_POWER_UP_RST", 0, 0x24, BIT(4),
	},
	[PCIE_RP_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_RP_SLV_AFENCE_ARST", 0, 0x24, BIT(16),
	},
	[PCIE_RP_MST_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"PCIE_RP_MST_AFENCE_ARST", 0, 0x24, BIT(20),
	},
};

static struct a210_rst_signal usb_rst_signals[] = {
	[USB_DFMU_PRST & RST_SIGNAL_MASK] = {
		"USB_DFMU_PRST", 0, 0, BIT(0),
	},
	[USB_DFMU_ARST & RST_SIGNAL_MASK] = {
		"USB_DFMU_ARST", 0, 0, BIT(4),
	},
	[USB_AXI4_MST_ARST & RST_SIGNAL_MASK] = {
		"USB_AXI4_MST_ARST", 0, 0, BIT(8),
	},
	[USB_AXI4_MST_PRST & RST_SIGNAL_MASK] = {
		"USB_AXI4_MST_PRST", 0, 0, BIT(12),
	},
	[USB_DPTX_APBS_PRST & RST_SIGNAL_MASK] = {
		"USB_DPTX_APBS_PRST", 0, 0x4, BIT(0),
	},
	[USB_DPTX_VCC_RST & RST_SIGNAL_MASK] = {
		"USB_DPTX_VCC_RST", 0, 0x4, BIT(4),
	},
	[USB_USB31_APBS_PRST & RST_SIGNAL_MASK] = {
		"USB_USB31_APBS_PRST", 0, 0x8, BIT(0),
	},
	[USB_USB31_VCC_RST & RST_SIGNAL_MASK] = {
		"USB_USB31_VCC_RST", 0, 0x8, BIT(4),
	},
	[USB_USB31_PHY_RST & RST_SIGNAL_MASK] = {
		"USB_USB31_PHY_RST", 0, 0x8, BIT(8),
	},
	[USB_USB31_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"USB_USB31_SLV_AFENCE_ARST", 0, 0x8, BIT(12),
	},
	[USB_C10PHY_PHY_RST & RST_SIGNAL_MASK] = {
		"USB_C10PHY_PHY_RST", 0, 0xc, BIT(0),
	},
	[USB_C10PHY_APBS_PRST & RST_SIGNAL_MASK] = {
		"USB_C10PHY_APBS_PRST", 0, 0xc, BIT(4),
	},
	[USB_USB20_BLK_X2H_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_X2H_HRST", 0, 0x10, BIT(0),
	},
	[USB_USB20_BLK_X2H_ARST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_X2H_ARST", 0, 0x10, BIT(4),
	},
	[USB_USB20_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"USB_USB20_SLV_AFENCE_ARST", 0, 0x10, BIT(8),
	},
	[USB_USB20_BLK_H2P_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_H2P_HRST", 0, 0x10, BIT(16),
	},
	[USB_USB20_BLK_AHB_SLV_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_AHB_SLV_HRST", 0, 0x10, BIT(20),
	},
	[USB_USB20_BLK_AXI_MST_ARST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_AXI_MST_ARST", 0, 0x10, BIT(24),
	},
	[USB_USB20_BLK_USB2_SYSREG_PRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB2_SYSREG_PRST", 0, 0x10, BIT(28),
	},
	[USB_USB20_BLK_USB2_WRAP0_PRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB2_WRAP0_PRST", 0, 0x14, BIT(0),
	},
	[USB_USB20_BLK_USB2_WRAP0_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB2_WRAP0_HRST", 0, 0x14, BIT(4),
	},
	[USB_USB20_BLK_USB2_WRAP1_PRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB2_WRAP1_PRST", 0, 0x14, BIT(8),
	},
	[USB_USB20_BLK_USB2_WRAP1_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB2_WRAP1_HRST", 0, 0x14, BIT(12),
	},
	[USB_USB20_BLK_A2X0_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_A2X0_HRST", 0, 0x14, BIT(16),
	},
	[USB_USB20_BLK_A2X0_ARST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_A2X0_ARST", 0, 0x14, BIT(20),
	},
	[USB_USB20_BLK_A2X1_HRST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_A2X1_HRST", 0, 0x14, BIT(24),
	},
	[USB_USB20_BLK_A2X1_ARST & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_A2X1_ARST", 0, 0x14, BIT(28),
	},
	[USB_USB20_BLK_USB0_PHY_PON_RESET & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB0_PHY_PON_RESET", 0, 0x18, BIT(0),
	},
	[USB_USB20_BLK_USB1_PHY_PON_RESET & RST_SIGNAL_MASK] = {
		"USB_USB20_BLK_USB1_PHY_PON_RESET", 0, 0x18, BIT(4),
	},
	[USB_PERI2_SS_RST & RST_SIGNAL_MASK] = {
		"USB_PERI2_SS_RST", 0, 0x20, BIT(0),
	},
	[USB_PERI2_SLV_AFENCE_ARST & RST_SIGNAL_MASK] = {
		"USB_PERI2_SLV_AFENCE_ARST", 0, 0x20, BIT(4),
	},
};

static struct a210_rst_signal tee_rst_signals[] = {
	[TEE_KEYRAM_PRST & RST_SIGNAL_MASK] = {
		"TEE_KEYRAM_PRST", 0, 0, BIT(0),
	},
	[TEE_DS_PRST & RST_SIGNAL_MASK] = {
		"TEE_DS_PRST", 0, 0, BIT(1),
	},
	[TEE_EFUSE_PRST & RST_SIGNAL_MASK] = {
		"TEE_EFUSE_PRST", 0, 0, BIT(2),
	},
	[TEE_OCRAM_HRST & RST_SIGNAL_MASK] = {
		"TEE_OCRAM_HRST", 0, 0, BIT(3),
	},
	[TEE_SYSREG_PRST & RST_SIGNAL_MASK] = {
		"TEE_SYSREG_PRST", 0, 0, BIT(4),
	},
	[TEE_CCU_PRST & RST_SIGNAL_MASK] = {
		"TEE_CCU_PRST", 0, 0, BIT(5),
	},
	[TEE_CCU_CRST & RST_SIGNAL_MASK] = {
		"TEE_CCU_CRST", 0, 0, BIT(6),
	},
	[TEE_EIP150B_HRST & RST_SIGNAL_MASK] = {
		"TEE_EIP150B_HRST", 0, 0, BIT(8),
	},
	[TEE_EIP120SIII_HRST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SIII_HRST", 0, 0, BIT(9),
	},
	[TEE_EIP120SIII_ARST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SIII_ARST", 0, 0, BIT(10),
	},
	[TEE_EIP120SII_HRST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SII_HRST", 0, 0, BIT(11),
	},
	[TEE_EIP120SII_ARST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SII_ARST", 0, 0, BIT(12),
	},
	[TEE_EIP120SI_HRST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SI_HRST", 0, 0, BIT(13),
	},
	[TEE_EIP120SI_ARST & RST_SIGNAL_MASK] = {
		"TEE_EIP120SI_ARST", 0, 0, BIT(14),
	},
	[TEE_DMAC_HRST & RST_SIGNAL_MASK] = {
		"TEE_DMAC_HRST", 0, 0, BIT(15),
	},
	[TEE_DMAC_ARST & RST_SIGNAL_MASK] = {
		"TEE_DMAC_ARST", 0, 0, BIT(16),
	},
	[TEE_X2P_TEESYS_PRST & RST_SIGNAL_MASK] = {
		"TEE_X2P_TEESYS_PRST", 0, 0, BIT(24),
	},
	[TEE_X2P_TEESYS_ARST & RST_SIGNAL_MASK] = {
		"TEE_X2P_TEESYS_ARST", 0, 0, BIT(25),
	},
	[TEE_AXI4_TEESYS_ARST & RST_SIGNAL_MASK] = {
		"TEE_AXI4_TEESYS_ARST", 0, 0, BIT(26),
	},
	[TEE_APB3_TEESYS_PRST & RST_SIGNAL_MASK] = {
		"TEE_APB3_TEESYS_PRST", 0, 0, BIT(27),
	},
	[TEE_APB3_TEESYS_HRST & RST_SIGNAL_MASK] = {
		"TEE_APB3_TEESYS_HRST", 0, 0, BIT(28),
	},
	[TEE_AHB2_TEESYS_HRST & RST_SIGNAL_MASK] = {
		"TEE_AHB2_TEESYS_HRST", 0, 0, BIT(29),
	},
};

static struct a210_rst_signal gpu_rst_signals[] = {
	[GPU_PWR_WRAP_RGX_HOOD_RST & RST_SIGNAL_MASK] = {
		"GPU_PWR_WRAP_RGX_HOOD_RST", 0, 0, BIT(0),
	},
	[GPU_PWR_WRAP_DFMU_RST & RST_SIGNAL_MASK] = {
		"GPU_PWR_WRAP_DFMU_RST", 0, 0, BIT(1),
	},
};

/*
 * Reset ctrl functions
 */
#define RST_DEF(_id, _name, _signals)					\
	[_id] = {							\
		.id = _id,						\
		.name = _name,					\
		.signals = _signals,				\
		.num_signals = ARRAY_SIZE(_signals),		\
	}

static const struct a210_rst_match_data a210_rst_match_data[] = {
	RST_DEF(VP_RST, "vp-rst", vp_rst_signals),
	RST_DEF(VI_RST, "vi-rst", vi_rst_signals),
	RST_DEF(NPU_RST, "npu-rst", npu_rst_signals),
	RST_DEF(VO_RST, "vo-rst", vo_rst_signals),
	RST_DEF(PERI0_RST, "peri0-rst", peri0_rst_signals),
	RST_DEF(PERI1_RST, "peri1-rst", peri1_rst_signals),
	RST_DEF(PERI2_RST, "peri2-rst", peri2_rst_signals),
	RST_DEF(PERI3_RST, "peri3-rst", peri3_rst_signals),
	RST_DEF(PCIE_RST, "pcie-rst", pcie_rst_signals),
	RST_DEF(USB_RST, "usb-rst", usb_rst_signals),
	RST_DEF(TEE_RST, "tee-rst", tee_rst_signals),
	RST_DEF(GPU_RST, "gpu-rst", gpu_rst_signals),
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

static int a210_reset_subsys_signal_lookup(struct a210_reset *priv, unsigned long id,
		   struct a210_rst_subsys **subsys, struct a210_rst_signal **signal)
{
	u32 subsys_id = id >> 8;
	u32 signal_id = id & RST_SIGNAL_MASK;

	if (subsys_id != priv->subsys_id)
		return -ENODEV;

	*subsys = &priv->subsys[0];

	if (signal_id >= (*subsys)->num_signals)
		return -ENODEV;

	*signal = &(*subsys)->signals[signal_id];

	return 0;
}

static int a210_reset_update(struct reset_controller_dev *rcdev,
				 unsigned long id, bool assert)
{
	u32 reg;
	struct a210_reset *priv = to_a210_reset(rcdev);
	struct a210_rst_subsys *subsys;
	struct a210_rst_signal *signal;
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&priv->lock, flags);
	do {
		ret = a210_reset_subsys_signal_lookup(priv, id, &subsys, &signal);
		if (ret) {
			spin_unlock_irqrestore(&priv->lock, flags);
			dev_err(rcdev->dev, "fail to find signal 0x%lx\n", id);
			return ret;
		}

		reg = readl(subsys->base + signal->offset);
		if (assert == true)
			reg &= ~signal->bit;
		else
			reg |= signal->bit;
		writel(reg, subsys->base + signal->offset);

		id = signal->nextid;
	} while (id > 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int a210_reset_assert(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	return a210_reset_update(rcdev, id, true);
}

static int a210_reset_deassert(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	return a210_reset_update(rcdev, id, false);
}

static int a210_reset_status(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	struct a210_reset *priv = to_a210_reset(rcdev);
	struct a210_rst_subsys *subsys;
	struct a210_rst_signal *signal;
	int ret;

	ret = a210_reset_subsys_signal_lookup(priv, id, &subsys, &signal);
	if (ret)
		return ret;

	return !!(readl(subsys->base + signal->offset) & signal->bit);
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
	priv->reset.nr_resets = A210_RESETS_MAX;

	spin_lock_init(&priv->lock);

	return reset_controller_register(&priv->reset);
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
	int status = !!(readl(subsys->base + signal->offset) & signal->bit);

	seq_printf(s, "%d\n", status);

	return 0;
}

static int a210_subsys_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, a210_signal_status_show, inode->i_private);
}

static const struct file_operations a210_subsys_status_fops = {
	.open = a210_subsys_status_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static void a210_reset_debugfs_init(struct a210_reset *priv)
{
	struct dentry *entry;
	u32 i, j;
	char name[A210_RST_NAME_SIZE];
	char debugfs_entry_name[32] = "a210-reset";
	struct dentry *debugfs_entry;
	const char *addr_ptr;

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
	if (!debugfs_entry) {
		dev_err(priv->dev, "Failed to create %s debugfs directory\n", debugfs_entry_name);
		return;
	}

	/* Iterate through all subsys */
	int signal_idx = 0;

	for (i = 0; i < priv->num_subsys; i++) {
		/* Create signal files for each subsystem */
		for (j = 0; j < priv->subsys[i].num_signals; j++) {
			struct a210_rst_debugfs_node *node = &priv->debugfs_node[signal_idx];

			strscpy(name, priv->subsys[i].signals[j].name, sizeof(name));
			str_to_lower(name);

			node->signal = &priv->subsys[i].signals[j];
			node->subsys = &priv->subsys[i];
			entry = debugfs_create_file(name, 0444,
						   debugfs_entry,
						   (void *)node,
						   &a210_subsys_status_fops);

			signal_idx++;
			if (!entry) {
				dev_err(priv->dev, "Failed to create %s file in debugfs\n", name);
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

	/* Alloc debugfs nodes */
	priv->debugfs_node = alloc_debugfs_nodes(pdev);
	if (!priv->debugfs_node) {
		ret = -ENOMEM;
		goto fail;
	}

	ret = a210_register_reset(pdev);
	if (ret)
		goto fail;

#if defined(CONFIG_DEBUG_FS)
	a210_reset_debugfs_init(priv);
#endif
	dev_info(dev, "succeed to register a210 reset driver\n");

	return ret;

fail:
	devm_kfree(dev, priv->debugfs_node);
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

static int a210_reset_init(void)
{
	return platform_driver_register(&a210_reset_driver);
}

arch_initcall(a210_reset_init);

MODULE_AUTHOR("dong.yan <yand@zhcomputing.com>");
MODULE_DESCRIPTION("Zhihe A210 reset controller driver");
MODULE_LICENSE("GPL");
