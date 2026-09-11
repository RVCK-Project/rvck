/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2021 SiFive
 */
#ifndef ASM_VENDOR_LIST_H
#define ASM_VENDOR_LIST_H

#define ANDES_VENDOR_ID		0x31e
#define SIFIVE_VENDOR_ID	0x489
#define THEAD_VENDOR_ID		0x5b7

#define QEMU_VIRT_VENDOR_ID		0x000
#define QEMU_VIRT_IMPL_ID		0x000
#define QEMU_VIRT_ARCH_ID		0x000
/* Newer Qemu reports the spec-allocated marchid 0x2a (42) for non-vendor CPUs */
#define QEMU_VIRT_ARCH_ID_SPEC		0x2a

#define LRW_VENDOR_ID		0x0
#define LRW_ARCH_ID		0x8000000000000920
#define LRW_IMPL_ID		0x3000020240831

#endif
