/* SPDX-License-Identifier: GPL-2.0-only */

/* SpacemiT clock and reset driver definitions for the K1 SoC */

#ifndef __SOC_SPACEMIT_SYSCON_H__
#define __SOC_SPACEMIT_SYSCON_H__

/* Auxiliary device used to represent a CCU reset controller */
struct spacemit_ccu_adev {
	struct auxiliary_device adev;
	struct regmap *regmap;
};

static inline struct spacemit_ccu_adev *
to_spacemit_ccu_adev(struct auxiliary_device *adev)
{
	return container_of(adev, struct spacemit_ccu_adev, adev);
}

#endif /* __SOC_SPACEMIT_SYSCON_H__ */
