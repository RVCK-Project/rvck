// SPDX-License-Identifier: GPL-2.0

#include <linux/types.h>

#include "../../../util/tsc.h"

u64 rdtsc(void)
{
	u64 val;

#if __riscv_xlen == 32
	u32 lo, hi, hi2;

	/* rdtime is not atomic across hi/lo on RV32; re-read hi to detect rollover */
	do {
		asm volatile("rdtimeh %0" : "=r"(hi));
		asm volatile("rdtime %0" : "=r"(lo));
		asm volatile("rdtimeh %0" : "=r"(hi2));
	} while (hi != hi2);
	val = ((u64)hi << 32) | lo;
#else
	/* https://lore.kernel.org/all/YxIzgYP3MujXdqwj@aurel32.net/ */
	asm volatile("rdtime %0" : "=r"(val));
#endif

	return val;
}
