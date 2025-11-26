/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Copyright (c) 2025 SpacemiT Technology Co. Ltd
 * Copyright (c) 2025 Alex Elder <elder@riscstar.com>
 */
#ifndef __RESET_SPACEMIT_K3_H
#define __RESET_SPACEMIT_K3_H
#include "reset-spacemit.h"

#define K3_AUX_DEV_ID(_unit) \
	{ \
		.name = "ccu_k3." #_unit "-reset", \
		.driver_data = (kernel_ulong_t)&k3_ ## _unit ## _reset_data, \
	}

extern const struct ccu_reset_controller_data k3_mpmu_reset_data;
extern const struct ccu_reset_controller_data k3_apbc_reset_data;
extern const struct ccu_reset_controller_data k3_apmu_reset_data;
extern const struct ccu_reset_controller_data k3_dciu_reset_data;
extern const struct ccu_reset_controller_data k3_rsysctrl_reset_data;
extern const struct ccu_reset_controller_data k3_ruartctrl_reset_data;
extern const struct ccu_reset_controller_data k3_ri2sctrl_reset_data;
extern const struct ccu_reset_controller_data k3_rspictrl_reset_data;
extern const struct ccu_reset_controller_data k3_ri2cctrl_reset_data;
extern const struct ccu_reset_controller_data k3_rpmu_reset_data;
extern const struct ccu_reset_controller_data k3_rpwmctrl_reset_data;
extern const struct ccu_reset_controller_data k3_apbc2_reset_data;

#endif /* __RESET_SPACEMIT_K3_H */
