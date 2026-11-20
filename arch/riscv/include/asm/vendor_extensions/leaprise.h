/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASM_RISCV_VENDOR_EXTENSIONS_LEAPRISE_H
#define _ASM_RISCV_VENDOR_EXTENSIONS_LEAPRISE_H

#include <asm/vendor_extensions.h>
#include <linux/types.h>

/*
 * Extension keys should be strictly less than max.
 * It is safe to increment this when necessary.
 */
#define RISCV_ISA_VENDOR_EXT_MAX_LEAPRISE			32

extern struct riscv_isa_vendor_ext_data_list riscv_isa_vendor_ext_list_leaprise;

#endif
