// SPDX-License-Identifier: GPL-2.0-only

#include <linux/device.h>
#include <linux/property.h>

#include "xuantie_ntrace.h"

int xuantie_ntrace_funnel_init(struct xuantie_ntrace_component *comp)
{
	struct device *dev = &comp->pdev->dev;
	u32 level;
	int ret;

	ret = device_property_read_u32(dev, "level", &level);
	if (ret)
		level = LEVEL1_FUNNEL;
	comp->funnel.level = level;
	dev_info(dev, "funnel level=%d\n", comp->funnel.level);

	return xuantie_ntrace_graph_parse(comp);
}
