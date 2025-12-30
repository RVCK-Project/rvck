/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Copyright (c) 2025 SpacemiT Technology Co. Ltd
 * Copyright (c) 2025 Alex Elder <elder@riscstar.com>
 */
#ifndef __RESET_SPACEMIT_H
#define __RESET_SPACEMIT_H

#include <linux/reset-controller.h>
#include <linux/types.h>

struct ccu_reset_data {
	u32 offset;
	u32 assert_mask;
	u32 deassert_mask;
};

struct ccu_reset_controller_data {
	const struct ccu_reset_data *reset_data;	/* array */
	size_t count;
};

struct ccu_reset_controller {
	struct reset_controller_dev rcdev;
	const struct ccu_reset_controller_data *data;
	struct regmap *regmap;
};

#define RESET_DATA(_offset, _assert_mask, _deassert_mask)	\
	{							\
		.offset		= (_offset),			\
		.assert_mask	= (_assert_mask),		\
		.deassert_mask	= (_deassert_mask),		\
	}

#endif /* __RESET_SPACEMIT_H */
