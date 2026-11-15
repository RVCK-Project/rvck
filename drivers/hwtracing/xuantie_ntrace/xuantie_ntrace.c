// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for XUANTIE RISC-V Nexus Trace Device
 *
 * Copyright (c) 2025 Alibaba DAMO Academy.
 * Author: Jiangshuai Li <jiangshuai_li@linux.alibaba.com>
 * Author: Chen Pei <cp0613@linux.alibaba.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/types.h>
#include <linux/align.h>
#include <linux/sizes.h>
#include <linux/log2.h>
#include <linux/minmax.h>
#include <linux/bits.h>
#include <linux/bitfield.h>
#include <linux/limits.h>
#include <linux/slab.h>
#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/rculist.h>
#include <linux/smp.h>
#include <linux/dma-mapping.h>
#include <linux/iommu.h>
#include <linux/vmalloc.h>

#include "xuantie_ntrace.h"
#include "xt_ntrace_control_interface.h"

#define TEST_LOG_ON	0
#define XUANTIE_NTRACE_PMU_NAME "xuantie_ntrace"

/* Trace data is emitted in 256-bit (32-byte) units; keep partial commits aligned. */
#define XUANTIE_NTRACE_DATA_UNIT 32

/* Offset of the tr*Impl register. */
#define XUANTIE_NTRACE_IMPL_OFFSET	0x4

static struct xuantie_ntrace_drvdata *ntrace_drvdata;
static LIST_HEAD(xuantie_ntrace_controllers);
static DEFINE_MUTEX(xuantie_ntrace_lock);
static bool trace_component_detected;

/*
 * Serializes session_committed and all trace component hardware ops.
 * add/del/start/stop of different events (one per cpu/thread) may run
 * concurrently on different CPUs, so a committing event_del can race
 * with another event's start/stop. Raw spinlock: these callbacks run
 * with IRQs disabled and must not sleep.
 */
static DEFINE_RAW_SPINLOCK(xuantie_ntrace_hw_lock);

static int console_msg_out(const char *str)
{
	pr_err("%s", str);
	return 0;
}

static int trace_register_write(u64 addr, u32 value)
{
	iowrite32(value, (void __iomem *)(unsigned long)addr);
#if TEST_LOG_ON
	pr_info("--->write 0x%llx with 0x%x\n", addr, value);
#endif
	return 0;
}

static int trace_register_read(u64 addr, u32 *value)
{
	*value = ioread32((void __iomem *)(unsigned long)addr);
#if TEST_LOG_ON
	pr_info("--->read 0x%llx get 0x%x\n", addr, *value);
#endif
	return 0;
}

/*
 * A link endpoint points at another component's firmware node.
 */
static void ntrace_remote_put(struct fwnode_handle **remote)
{
	struct fwnode_handle *fwnode = *remote;

	if (!fwnode)
		return;

	fwnode_handle_put(fwnode);
	*remote = NULL;
}

/*
 * The component, its register mapping and its trace buffer are all managed by
 * the device core, so releasing the firmware node references is everything
 * that is left to do here.
 */
static void xuantie_ntrace_release_ports(void *data)
{
	struct xuantie_ntrace_component *comp = data;
	int i;

	for (i = 0; i < comp->in_num; i++)
		ntrace_remote_put(&comp->in[i]->remote);
	for (i = 0; i < comp->out_num; i++)
		ntrace_remote_put(&comp->out[i]->remote);
}

#if IS_ENABLED(CONFIG_OF)
static const char *const ntrace_in_containers[] = {
	"in-ports", "input_port", NULL
};

static const char *const ntrace_out_containers[] = {
	"out-ports", "output_port", NULL
};

static struct fwnode_handle *
ntrace_find_port_container(struct fwnode_handle *fwnode,
			   const char *const *names, bool *legacy)
{
	struct fwnode_handle *container;
	int i;

	for (i = 0; names[i]; i++) {
		container = fwnode_get_named_child_node(fwnode, names[i]);
		if (container) {
			*legacy = i > 0;
			return container;
		}
	}

	return NULL;
}

/*
 * Walk up from a node referenced by a port until the device owning it.
 */
static struct fwnode_handle *ntrace_peer_device(struct fwnode_handle *node)
{
	struct fwnode_handle *parent;

	while (is_of_node(node) && !fwnode_property_present(node, "compatible")) {
		parent = fwnode_get_parent(node);
		fwnode_handle_put(node);
		node = parent;
	}

	return node;
}

/*
 * Return the firmware node of the component on the far side of @port, with a
 * reference attached, or %NULL when the port describes no connection.
 */
static struct fwnode_handle *ntrace_port_peer(struct fwnode_handle *port,
					      bool legacy)
{
	struct fwnode_reference_args args;
	struct fwnode_handle *child, *peer = NULL;

	if (legacy) {
		if (fwnode_property_get_reference_args(port, "endpoint", NULL, 0, 0,
						       &args))
			return NULL;
		return ntrace_peer_device(args.fwnode);
	}

	/*
	 * One endpoint per port is all the trace binding needs, so the first
	 * endpoint holding a connection wins. The ACPI core synthesizes this
	 * same shape from a _DSD device graph, which is why the graph is
	 * walked through the generic fwnode API here.
	 *
	 * An ACPI reference carries the remote port and endpoint numbers, a
	 * device tree one is a bare phandle, so both spellings are tried.
	 */
	fwnode_for_each_child_node(port, child) {
		if (fwnode_property_get_reference_args(child, "remote-endpoint",
						       NULL, 2, 0, &args) &&
		    fwnode_property_get_reference_args(child, "remote-endpoint",
						       NULL, 0, 0, &args))
			continue;

		peer = ntrace_peer_device(args.fwnode);
		fwnode_handle_put(child);
		break;
	}

	return peer;
}

static int ntrace_port_count(struct fwnode_handle *container)
{
	struct fwnode_handle *port;
	int nr = 0;

	fwnode_for_each_child_node(container, port)
		nr++;

	return nr;
}

/*
 * Collect the connections of one direction of @comp into comp->in or comp->out.
 */
static int xuantie_ntrace_parse_ports(struct xuantie_ntrace_component *comp,
				      const char *const *names, bool is_input)
{
	struct device *dev = &comp->pdev->dev;
	struct fwnode_handle *fwnode = dev_fwnode(dev);
	struct xuantie_io_port *io_port;
	struct xuantie_io_port **ports;
	struct fwnode_handle *container, *port;
	bool legacy;
	u32 found = 0;
	int ret = 0, nr;

	container = ntrace_find_port_container(fwnode, names, &legacy);
	if (!container) {
		dev_err(dev, "Failed to find '%s'\n", names[0]);
		return -ENODEV;
	}

	nr = ntrace_port_count(container);
	if (nr) {
		ports = devm_kcalloc(dev, nr, sizeof(*ports), GFP_KERNEL);
		if (!ports) {
			ret = -ENOMEM;
			goto out_put_container;
		}
	} else {
		ports = NULL;
	}

	fwnode_for_each_child_node(container, port) {
		struct fwnode_handle *peer;

		peer = ntrace_port_peer(port, legacy);
		if (!peer) {
			dev_err(dev, "Missing connection in %s port %u\n",
				names[legacy], found);
			fwnode_handle_put(port);
			ret = -ENODEV;
			goto out_put_ports;
		}

		/* A peer whose status says "disabled" is simply not connected. */
		if (!fwnode_device_is_available(peer)) {
			ntrace_remote_put(&peer);
			continue;
		}

		io_port = devm_kzalloc(dev, sizeof(*io_port), GFP_KERNEL);
		if (!io_port) {
			ntrace_remote_put(&peer);
			fwnode_handle_put(port);
			ret = -ENOMEM;
			goto out_put_ports;
		}

		if (fwnode_property_read_u32(port, "reg", &io_port->endpoint_num))
			io_port->endpoint_num = found;
		io_port->is_input = is_input;
		io_port->remote = peer;
		ports[found++] = io_port;
	}

	if (is_input) {
		comp->in = ports;
		comp->in_num = found;
	} else {
		comp->out = ports;
		comp->out_num = found;
	}

	goto out_put_container;

out_put_ports:
	while (found--)
		ntrace_remote_put(&ports[found]->remote);
out_put_container:
	fwnode_handle_put(container);
	return ret;
}

static int ntrace_of_graph_parse(struct xuantie_ntrace_component *comp)
{
	int ret;

	/* An encoder is a graph source and a sink is a graph drain. */
	if (comp->type != NTRACE_TYPE_ENCODER) {
		ret = xuantie_ntrace_parse_ports(comp, ntrace_in_containers, true);
		if (ret)
			return ret;
	}

	if (comp->type != NTRACE_TYPE_SINK_SMEM) {
		ret = xuantie_ntrace_parse_ports(comp, ntrace_out_containers, false);
		if (ret)
			return ret;
	}

	return 0;
}
#endif /* CONFIG_OF */

int xuantie_ntrace_graph_parse(struct xuantie_ntrace_component *comp)
{
	struct device *dev = &comp->pdev->dev;
	int ret;

#if IS_ENABLED(CONFIG_OF)
	ret = ntrace_of_graph_parse(comp);
#else
	ret = -ENODEV;
#endif
	if (ret)
		return ret;

	return devm_add_action_or_reset(dev, xuantie_ntrace_release_ports, comp);
}

/*
 * Bind an encoder to the hart it traces. Both firmwares carry the number in a
 * "cpu" property: a property of the device tree node, or the matching _DSD
 * data property of the ACPI device.
 */
int xuantie_ntrace_get_hart_id(struct device *dev, u32 *hart_id)
{
	return device_property_read_u32(dev, "cpu", hart_id);
}

static const struct of_device_id xuantie_ntrace_of_match[] = {
	{ .compatible = "xuantie,ntrace-encoder",
	  .data = (void *)NTRACE_TYPE_ENCODER },
	{ .compatible = "xuantie,ntrace-funnel",
	  .data = (void *)NTRACE_TYPE_FUNNEL },
	{ .compatible = "xuantie,ntrace-sink",
	  .data = (void *)NTRACE_TYPE_SINK_SMEM },
	{ }
};
MODULE_DEVICE_TABLE(of, xuantie_ntrace_of_match);

/*
 * Count the trace components the firmware describes, so that the PMU is only
 * registered once the whole trace pipeline has probed. Only one of the two
 * firmwares is populated on a given machine, so both counts are simply added.
 */
static void xuantie_ntrace_count_expected_devices(struct xuantie_ntrace_drvdata *drvdata)
{
	u32 expected = 0, nr;
	int i;

#if IS_ENABLED(CONFIG_OF)
	for (i = 0; xuantie_ntrace_of_match[i].compatible[0]; i++) {
		struct device_node *np;

		nr = 0;
		for_each_compatible_node(np, NULL,
					 xuantie_ntrace_of_match[i].compatible) {
			if (of_device_is_available(np))
				nr++;
		}
		expected += nr;
		if (nr)
			pr_info("Found %u %s device(s) in DT\n", nr,
				xuantie_ntrace_of_match[i].compatible);
	}
#endif

	drvdata->expected_count = expected;
}

/*
 * Name the firmware nodes that never reached a bound driver, so a machine with
 * an incomplete trace pipeline is diagnosable from the log alone.
 */
static void ntrace_dump_not_probed(void)
{
#if IS_ENABLED(CONFIG_OF)
	struct device_node *np;
	int i;

	for (i = 0; xuantie_ntrace_of_match[i].compatible[0]; i++)
		for_each_compatible_node(np, NULL,
					 xuantie_ntrace_of_match[i].compatible) {
			struct platform_device *pdev;

			if (!of_device_is_available(np))
				continue;

			pdev = of_find_device_by_node(np);
			if (!pdev) {
				pr_warn("node %pOFn has no platform device\n", np);
			} else {
				if (!pdev->dev.driver)
					pr_warn("node %pOFn has no driver bound\n", np);
				put_device(&pdev->dev);
			}
		}
#endif

#if IS_ENABLED(CONFIG_ACPI)
	struct acpi_device *adev;

	for_each_acpi_dev_match(adev, XUANTIE_NTRACE_ACPI_HID, NULL, -1) {
		struct device *physical = acpi_get_first_physical_node(adev);

		if (!physical)
			pr_warn("ACPI device %s has no physical device\n",
				dev_name(&adev->dev));
		else if (!physical->driver)
			pr_warn("ACPI device %s has no driver bound\n",
				dev_name(&adev->dev));
	}
#endif
}

int xuantie_ntrace_add_device_node(struct platform_device *pdev,
				enum xuantie_ntrace_dev_type type,
				struct xuantie_ntrace_component *comp)
{
	struct xuantie_ntrace_drvdata *drvdata = ntrace_drvdata;
	struct ntrace_dev_node *node;

	if (!drvdata) {
		dev_err(&pdev->dev, "Driver data not initialized\n");
		return -ENODEV;
	}

	node = devm_kzalloc(&pdev->dev, sizeof(*node), GFP_KERNEL);
	if (!node)
		return -ENOMEM;

	node->pdev = pdev;
	node->dev = &pdev->dev;
	node->type = type;
	node->component = comp;

	platform_set_drvdata(pdev, node);

	mutex_lock(&xuantie_ntrace_lock);
	raw_spin_lock_irq(&xuantie_ntrace_hw_lock);
	list_add_tail_rcu(&comp->list, &xuantie_ntrace_controllers);
	raw_spin_unlock_irq(&xuantie_ntrace_hw_lock);
	mutex_unlock(&xuantie_ntrace_lock);

	mutex_lock(&drvdata->lock);
	drvdata->probed_count++;

	dev_info(&pdev->dev, "%s probed (%u/%u)\n",
		 xuantie_ntrace_type2str(type), drvdata->probed_count,
		 drvdata->expected_count);

	mutex_unlock(&drvdata->lock);

	return 0;
}

int xuantie_ntrace_remove_device_node(struct platform_device *pdev)
{
	struct xuantie_ntrace_drvdata *drvdata = ntrace_drvdata;
	struct ntrace_dev_node *node = platform_get_drvdata(pdev);

	if (!drvdata || !node)
		return 0;

	mutex_lock(&drvdata->lock);
	drvdata->probed_count--;
	mutex_unlock(&drvdata->lock);

	if (node->component) {
		mutex_lock(&xuantie_ntrace_lock);
		raw_spin_lock_irq(&xuantie_ntrace_hw_lock);
		list_del_rcu(&node->component->list);
		raw_spin_unlock_irq(&xuantie_ntrace_hw_lock);
		mutex_unlock(&xuantie_ntrace_lock);

		synchronize_rcu();
	}

	return 0;
}

static int xuantie_ntrace_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct xuantie_ntrace_component *component;
	const void *match_data = device_get_match_data(dev);
	enum xuantie_ntrace_dev_type type;
	void __iomem *base;
	struct resource *res;
	int ret;

	/*
	 * A device tree compatible names the component. Absent match data
	 * becomes NTRACE_TYPE_UNKNOWN.
	 */
	type = (enum xuantie_ntrace_dev_type)(unsigned long)match_data;

	component = devm_kzalloc(dev, sizeof(*component), GFP_KERNEL);
	if (!component)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "Failed to get 'reg' resource\n");
		return -EINVAL;
	}

	/*
	 * Not devm_ioremap_resource(): a platform device may describe a
	 * register block that overlaps a region another driver already
	 * claimed, and mapping it must not depend on who got there first.
	 */
	base = devm_ioremap(dev, res->start, resource_size(res));
	if (!base) {
		dev_err(dev, "Failed to ioremap registers\n");
		return -ENXIO;
	}

	component->pdev = pdev;
	component->reg_base = (u64)(unsigned long)base;
	component->phys_base = res->start;
	component->reg_size = resource_size(res);

	if (type == NTRACE_TYPE_UNKNOWN)
		return -ENODEV;

	component->type = type;
	dev_info(dev, "%s at 0x%llx, size 0x%llx\n",
		 xuantie_ntrace_type2str(type), (u64)res->start,
		 (u64)resource_size(res));

	switch (type) {
	case NTRACE_TYPE_ENCODER:
		ret = xuantie_ntrace_encoder_init(component);
		break;
	case NTRACE_TYPE_FUNNEL:
		ret = xuantie_ntrace_funnel_init(component);
		break;
	case NTRACE_TYPE_SINK_SMEM:
		ret = xuantie_ntrace_sink_init(component);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return dev_err_probe(dev, ret, "failed to parse %s\n",
				     xuantie_ntrace_type2str(type));

	ret = xuantie_ntrace_add_device_node(pdev, type, component);
	if (ret)
		return dev_err_probe(dev, ret, "failed to add %s\n",
				     xuantie_ntrace_type2str(type));

	return 0;
}

static int xuantie_ntrace_remove(struct platform_device *pdev)
{
	return xuantie_ntrace_remove_device_node(pdev);
}

static struct platform_driver xuantie_ntrace_driver = {
	.probe	= xuantie_ntrace_probe,
	.remove	= xuantie_ntrace_remove,
	.driver	= {
		.name		= "xuantie-ntrace",
		.of_match_table	= xuantie_ntrace_of_match,
	},
};

static int __init xuantie_ntrace_init(void)
{
	struct xuantie_ntrace_drvdata *drvdata;
	int ret;

	pr_info("Initializing Xuantie NTrace PMU driver\n");

	drvdata = kzalloc(sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	trace_component_detected = false;

	/* Set interfaces needed for Trace Control Lib. */
	xt_trace_control_init_rw_trace_interface(console_msg_out,
						 trace_register_write,
						 trace_register_read, NULL);

	mutex_init(&drvdata->lock);
	init_completion(&drvdata->all_probed);
	drvdata->pmu_registered = false;

	// Count the components the firmware describes
	xuantie_ntrace_count_expected_devices(drvdata);

	ntrace_drvdata = drvdata;

	ret = platform_driver_register(&xuantie_ntrace_driver);
	if (ret) {
		pr_err("Failed to register ntrace driver: %d\n", ret);
		kfree(drvdata);
		ntrace_drvdata = NULL;
		return ret;
	}

	/*
	 * On a machine whose trace pipeline is intact, every component probes
	 * inside platform_driver_register() and the completion is already done
	 * here, so this costs nothing. Only a machine missing a component waits
	 * the timeout, and it stays functional: a component that probes late
	 * still registers the PMU from its own add path.
	 */
	if (drvdata->expected_count && !READ_ONCE(drvdata->pmu_registered) &&
	    !wait_for_completion_timeout(&drvdata->all_probed,
					 msecs_to_jiffies(3000))) {
		pr_warn("PMU not registered: %u/%u trace components probed\n",
			READ_ONCE(drvdata->probed_count), drvdata->expected_count);
		ntrace_dump_not_probed();
	}

	return 0;
}

static void __exit xuantie_ntrace_exit(void)
{
	struct xuantie_ntrace_drvdata *drvdata = ntrace_drvdata;

	if (!drvdata)
		return;

	/* Unbinding every device releases its components through devm. */
	platform_driver_unregister(&xuantie_ntrace_driver);

	kfree(drvdata);
	ntrace_drvdata = NULL;
}

module_init(xuantie_ntrace_init);
module_exit(xuantie_ntrace_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Jiangshuai Li <jiangshuai_li@linux.alibaba.com>");
MODULE_AUTHOR("Chen Pei <cp0613@linux.alibaba.com>");
MODULE_DESCRIPTION("Driver for XUANTIE RISC-V Nexus Trace Device");
