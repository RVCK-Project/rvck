/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_RWONCE_H
#define __ASM_RWONCE_H

#include <linux/compiler_types.h>
#include <asm/alternative-macros.h>
#include <asm/vendorid_list.h>
#include <asm/errata_list_vendors.h>

#if defined(CONFIG_ERRATA_THEAD_WRITE_ONCE) && !defined(NO_ALTERNATIVE) \
					    && !defined(BUILD_VDSO)

#define __WRITE_ONCE(x, val)				\
do {							\
	*(__volatile__ typeof(x) *)&(x) = (val);	\
	asm volatile(ALTERNATIVE(			\
		"nop",					\
		"fence w, o",				\
		THEAD_VENDOR_ID,			\
		ERRATA_THEAD_WRITE_ONCE,		\
		CONFIG_ERRATA_THEAD_WRITE_ONCE)		\
		: : : "memory");			\
} while (0)

#endif /* CONFIG_ERRATA_THEAD_WRITE_ONCE && !NO_ALTERNATIVE && !BUILD_VDSO */

#include <asm-generic/rwonce.h>

#endif	/* __ASM_RWONCE_H */
