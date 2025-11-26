/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Copyright (c) 2025 SpacemiT Technology Co. Ltd
 * Copyright (c) 2025 Alex Elder <elder@riscstar.com>
 */
#ifndef __RESET_SPACEMIT_K1_H
#define __RESET_SPACEMIT_K1_H
#include "reset-spacemit.h"

#define K1_AUX_DEV_ID(_unit) \
	{ \
		.name = "ccu_k1." #_unit "-reset", \
		.driver_data = (kernel_ulong_t)&k1_ ## _unit ## _reset_data, \
	}

extern const struct ccu_reset_controller_data k1_mpmu_reset_data;
extern const struct ccu_reset_controller_data k1_apbc_reset_data;
extern const struct ccu_reset_controller_data k1_apmu_reset_data;
extern const struct ccu_reset_controller_data k1_rcpu_reset_data;
extern const struct ccu_reset_controller_data k1_rcpu2_reset_data;
extern const struct ccu_reset_controller_data k1_apbc2_reset_data;

#endif /* __RESET_SPACEMIT_K1_H */
