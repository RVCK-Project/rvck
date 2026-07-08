/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 *    *** IMPORTANT ***
 * This file is not only included from C-code but also from devicetree source
 * files. As such this file MUST only contain comments and defines.
 *
 * Copyright (c) 2025 Xuliang Lin <linxuliang@zhcomputing.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#ifndef _ZH_DTS_IOPMP_H
#define _ZH_DTS_IOPMP_H


/* PERI1_SS */
#define IOPMP_DEV_IOMMU_PTW      0x00
#define IOPMP_DEV_PERI1_DFMU     0x01
#define IOPMP_DEV_DMAC_AP        0x02
#define IOPMP_DEV_GMAC_0         0x03
#define IOPMP_DEV_GMAC_1         0x04
#define IOPMP_DEV_GMAC_2         0x05
#define IOPMP_DEV_SD             0x06
#define IOPMP_DEV_EMMC           0x08
#define IOPMP_DEVICE_AON         0x37
#define IOPMP_DEVICE_CHIP_DBG    0x3e

/* USB_SS */
#define IOPMP_DEV_USB_DFMU       0x09
#define IOPMP_DEV_USB3_0         0x0a
#define IOPMP_DEV_USB2_1         0x0b
#define IOPMP_DEV_USB2_2         0x0c

/* PCIE_SS */
#define IOPMP_DEV_PCIE_DFMU      0x10
#define IOPMP_DEV_PCIE_0         0x11
#define IOPMP_DEV_PCIE_1         0x12
#define IOPMP_DEV_SATA_0         0x14
#define IOPMP_DEV_TEE_EIP120SI   0x16
#define IOPMP_DEV_TEE_EIP120SII  0x17
#define IOPMP_DEV_TEE_EIP120SIII 0x18
#define IOPMP_DEV_TEE_DMAC       0x19

//D2D RX DFMU device id lists
#define IOPMP_DEV_D2D_RX         0x1A

/* VI_SS */
#define IOPMP_DEV_VI_DFMU        0x20
#define IOPMP_DEV_ISP            0x21
#define IOPMP_DEV_VIPRE          0x22
#define IOPMP_DEV_DW200          0x23
#define IOPMP_DEV_VI_COMP_DECOMP 0x24

/* VP_SS */
#define IOPMP_DEV_VP_DFMU        0x25
#define IOPMP_DEV_VENC           0x26
#define IOPMP_DEV_VDEC           0x27
#define IOPMP_DEV_G2D            0x28

/* VO_SS */
#define IOPMP_DEV_VO_DFMU        0x2b
#define IOPMP_DEV_DISPLAY_0      0x2c
#define IOPMP_DEV_DISPLAY_1      0x2d
#define IOPMP_DEV_AUXDISP        0x2e

#define IOPMP_DEV_PIP_REC        0x2f

//GPUSS DFMU device id lists
#define IOPMP_DEV_GPU_SS         0x33
#define IOPMP_DEV_GPU            0x34

/* NPU_SS */
#define IOPMP_DEV_NPU_DFMU       0x30
#define IOPMP_DEV_NPU            0x31

//CPU SS DFMU device id lists
#define IOPMP_DEV_REMOTE_CPU     0x3A

/* IOMMU IOPMP DEVCIE ID*/
#define IOPMP_DEV_IOMMU_START    0x40
#define IOPMP_DEV_VP_IOMMU       (IOPMP_DEV_IOMMU_START + 0)
#define IOPMP_DEV_VI_IOMMU       (IOPMP_DEV_IOMMU_START + 1)
#define IOPMP_DEV_NPU_IOMMU      (IOPMP_DEV_IOMMU_START + 2)
#define IOPMP_DEV_VO_IOMMU       (IOPMP_DEV_IOMMU_START + 3)
#define IOPMP_DEV_PERI1_IOMMU    (IOPMP_DEV_IOMMU_START + 4)
#define IOPMP_DEV_PCIE_IOMMU     (IOPMP_DEV_IOMMU_START + 5)
#define IOPMP_DEV_USB_IOMMU      (IOPMP_DEV_IOMMU_START + 6)
#define IOPMP_DEV_GPU_IOMMU      (IOPMP_DEV_IOMMU_START + 7)
#define IOPMP_DEV_D2D_RX_IOMMU   (IOPMP_DEV_IOMMU_START + 8)
#define IOPMP_DEV_D2D_SS_IOMMU   (IOPMP_DEV_IOMMU_START + 9)

#endif
