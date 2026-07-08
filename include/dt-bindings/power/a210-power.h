/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#ifndef _DT_BINDINGS_POWER_A210_H_
#define _DT_BINDINGS_POWER_A210_H_

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

#endif /* _DT_BINDINGS_POWER_A210_H_ */
