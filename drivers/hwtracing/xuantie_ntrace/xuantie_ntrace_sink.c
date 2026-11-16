// SPDX-License-Identifier: GPL-2.0-only

#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/iommu.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/property.h>
#include <linux/sizes.h>

#include "xuantie_ntrace.h"

static int xuantie_ntrace_sink_buffer(struct xuantie_ntrace_component *comp)
{
	struct platform_device *pdev = comp->pdev;
	struct device *dev = &pdev->dev;
	struct xuantie_ntrace_sink_smem *sink = &comp->sink;
	struct resource *res;
	int ret;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (res) {
		sink->vaddr = devm_memremap(dev, res->start, resource_size(res),
					    MEMREMAP_WB);
		if (!sink->vaddr) {
			dev_err(dev, "Failed to memremap trace buffer %pR\n", res);
			return -ENOMEM;
		}
		sink->start_addr = res->start;
		sink->limit_addr = res->end;
		sink->is_resv_mem = true;
		dev_info(dev, "trace buffer {0x%llx-0x%llx}@0x%px\n",
			 (u64)sink->start_addr, (u64)sink->limit_addr, sink->vaddr);
		return 0;
	}

	if (IS_ENABLED(CONFIG_OF) && dev->of_node) {
		struct device_node *np;

		np = of_parse_phandle(dev->of_node, "memory-region", 0);
		if (np) {
			struct resource mem;

			ret = of_address_to_resource(np, 0, &mem);
			of_node_put(np);
			if (ret) {
				dev_err(dev, "Failed to find 'reserved-memory'\n");
				return ret;
			}
			sink->vaddr = devm_memremap(dev, mem.start,
						    resource_size(&mem),
						    MEMREMAP_WB);
			if (!sink->vaddr) {
				dev_err(dev, "Failed to memremap reserved-memory\n");
				return -ENOMEM;
			}
			sink->start_addr = mem.start;
			sink->limit_addr = mem.end;
			sink->is_resv_mem = true;
			dev_info(dev, "reserved-memory {0x%llx-0x%llx}@0x%px\n",
				 (u64)sink->start_addr, (u64)sink->limit_addr,
				 sink->vaddr);
			return 0;
		}
	}

	return 0;
}

int xuantie_ntrace_sink_init(struct xuantie_ntrace_component *comp)
{
	struct device *dev = &comp->pdev->dev;
	static const char * const default_work_mode = "circulating buffer";
	static const char * const default_format = "original";
	int ret;

	ret = device_property_read_string(dev, "working_mode",
					  &comp->sink.working_mode);
	if (ret)
		comp->sink.working_mode = default_work_mode;
	dev_info(dev, "working_mode=%s\n", comp->sink.working_mode);

	ret = device_property_read_string(dev, "format", &comp->sink.format);
	if (ret)
		comp->sink.format = default_format;
	dev_info(dev, "format=%s\n", comp->sink.format);

	ret = xuantie_ntrace_sink_buffer(comp);
	if (ret)
		return ret;

	return xuantie_ntrace_graph_parse(comp);
}
