// SPDX-License-Identifier: GPL-2.0-only

/*
 * Copyright (c) 2025 SpacemiT Technology Co. Ltd
 * Copyright (c) 2025 Alex Elder <elder@riscstar.com>
 */
#include <linux/auxiliary_bus.h>
#include <linux/container_of.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>
#include <linux/types.h>
#include <soc/spacemit/spacemit-syscon.h>
#include "reset-spacemit.h"
#include "reset-k1.h"
#include "reset-k3.h"

static int spacemit_reset_update(struct reset_controller_dev *rcdev,
				 unsigned long id, bool assert)
{
	struct ccu_reset_controller *controller;
	const struct ccu_reset_data *data;
	u32 mask;
	u32 val;

	controller = container_of(rcdev, struct ccu_reset_controller, rcdev);
	data = &controller->data->reset_data[id];
	mask = data->assert_mask | data->deassert_mask;
	val = assert ? data->assert_mask : data->deassert_mask;

	return regmap_update_bits(controller->regmap, data->offset, mask, val);
}

static int spacemit_reset_assert(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	return spacemit_reset_update(rcdev, id, true);
}

static int spacemit_reset_deassert(struct reset_controller_dev *rcdev,
				   unsigned long id)
{
	return spacemit_reset_update(rcdev, id, false);
}

static const struct reset_control_ops spacemit_reset_control_ops = {
	.assert		= spacemit_reset_assert,
	.deassert	= spacemit_reset_deassert,
};

static int spacemit_reset_controller_register(struct device *dev,
					      struct ccu_reset_controller *controller)
{
	struct reset_controller_dev *rcdev = &controller->rcdev;

	rcdev->ops = &spacemit_reset_control_ops;
	rcdev->owner = THIS_MODULE;
	rcdev->of_node = dev->of_node;
	rcdev->nr_resets = controller->data->count;

	return devm_reset_controller_register(dev, &controller->rcdev);
}

static int spacemit_reset_probe(struct auxiliary_device *adev,
				const struct auxiliary_device_id *id)
{
	struct spacemit_ccu_adev *rdev = to_spacemit_ccu_adev(adev);
	const void *data = (void *)id->driver_data;
	struct ccu_reset_controller *controller;
	struct device *dev = &adev->dev;

	controller = devm_kzalloc(dev, sizeof(*controller), GFP_KERNEL);
	if (!controller)
		return -ENOMEM;
	controller->data = data;
	controller->regmap = rdev->regmap;

	return spacemit_reset_controller_register(dev, controller);
}

static const struct auxiliary_device_id spacemit_reset_ids[] = {
#ifdef CONFIG_RESET_SPACEMIT_K1
	K1_AUX_DEV_ID(mpmu),
	K1_AUX_DEV_ID(apbc),
	K1_AUX_DEV_ID(apmu),
	K1_AUX_DEV_ID(rcpu),
	K1_AUX_DEV_ID(rcpu2),
	K1_AUX_DEV_ID(apbc2),
#endif

#ifdef CONFIG_RESET_SPACEMIT_K3
	K3_AUX_DEV_ID(mpmu),
	K3_AUX_DEV_ID(apbc),
	K3_AUX_DEV_ID(apmu),
	K3_AUX_DEV_ID(dciu),
	K3_AUX_DEV_ID(rsysctrl),
	K3_AUX_DEV_ID(ruartctrl),
	K3_AUX_DEV_ID(ri2sctrl),
	K3_AUX_DEV_ID(rspictrl),
	K3_AUX_DEV_ID(ri2cctrl),
	K3_AUX_DEV_ID(rpmu),
	K3_AUX_DEV_ID(rpwmctrl),
	K3_AUX_DEV_ID(apbc2),
#endif
	{ },
};
MODULE_DEVICE_TABLE(auxiliary, spacemit_reset_ids);

static struct auxiliary_driver spacemit_k1_reset_driver = {
	.probe          = spacemit_reset_probe,
	.id_table       = spacemit_reset_ids,
};
module_auxiliary_driver(spacemit_k1_reset_driver);

MODULE_AUTHOR("Alex Elder <elder@kernel.org>");
MODULE_DESCRIPTION("SpacemiT reset controller driver");
MODULE_LICENSE("GPL");
