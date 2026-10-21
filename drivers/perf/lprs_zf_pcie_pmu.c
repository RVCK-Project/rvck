// SPDX-License-Identifier: GPL-2.0-only
/*
 * LeapRise ZF(ZhuFeng) pcie Sub-system PMU Events Monitoring Support
 *
 * Copyright (C) 2026 LeapRise Technology Co., Ltd.
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/cpuhotplug.h>
#include <linux/cpumask.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/perf_event.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/smp.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#define ZF_PCIE_VSEC_RAS_DES_ID 0x02
#define ZF_PCIE_EVENT_CNT_CTL   0x8
#define ZF_PCIE_VSEC_REV        0x4
#define ZF_PCIE_VSEC_LEN        0x100
#define PCI_VENDOR_ID_LPRS      0x1cf2

/*
 * Event Counter Data Select includes two parts:
 * - 27-24: Group number(4-bit: 0..0x7)
 * - 23-16: Event number(8-bit: 0..0x13) within the Group
 *
 * Put them together as TRM does.
 */
#define ZF_PCIE_CNT_EVENT_SEL GENMASK(27, 16)
#define ZF_PCIE_CNT_LANE_SEL GENMASK(11, 8)
#define ZF_PCIE_CNT_STATUS BIT(7)
#define ZF_PCIE_CNT_ENABLE GENMASK(4, 2)
#define ZF_PCIE_PER_EVENT_OFF 0x1
#define ZF_PCIE_PER_EVENT_ON 0x3
#define ZF_PCIE_EVENT_CLEAR GENMASK(1, 0)
#define ZF_PCIE_EVENT_PER_CLEAR 0x1

#define ZF_PCIE_EVENT_CNT_DATA 0xC

#define ZF_PCIE_TIME_BASED_ANAL_CTL 0x10
#define ZF_PCIE_TIME_BASED_REPORT_SEL GENMASK(31, 24)
#define ZF_PCIE_TIME_BASED_DURATION_SEL GENMASK(15, 8)
#define ZF_PCIE_DURATION_MANUAL_CTL 0x0
#define ZF_PCIE_DURATION_1MS 0x1
#define ZF_PCIE_DURATION_10MS 0x2
#define ZF_PCIE_DURATION_100MS 0x3
#define ZF_PCIE_DURATION_1S 0x4
#define ZF_PCIE_DURATION_2S 0x5
#define ZF_PCIE_DURATION_4S 0x6
#define ZF_PCIE_DURATION_4US 0xFF
#define ZF_PCIE_TIME_BASED_TIMER_START BIT(0)
#define ZF_PCIE_TIME_BASED_CNT_ENABLE 0x1

#define ZF_PCIE_TIME_BASED_ANAL_DATA_REG_LOW 0x14
#define ZF_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH 0x18

/* Event attributes */
#define ZF_PCIE_CONFIG_EVENTID GENMASK(15, 0)
#define ZF_PCIE_CONFIG_TYPE GENMASK(19, 16)
#define ZF_PCIE_CONFIG_LANE GENMASK(27, 20)

#define ZF_PCIE_EVENT_ID(event) \
	FIELD_GET(ZF_PCIE_CONFIG_EVENTID, (event)->attr.config)
#define ZF_PCIE_EVENT_TYPE(event) \
	FIELD_GET(ZF_PCIE_CONFIG_TYPE, (event)->attr.config)
#define ZF_PCIE_EVENT_LANE(event) \
	FIELD_GET(ZF_PCIE_CONFIG_LANE, (event)->attr.config)

enum zf_pcie_event_type {
	ZF_PCIE_TYPE_INVALID,
	ZF_PCIE_TBA_EVENT,
	ZF_PCIE_EC_EVENT,
	ZF_PCIE_EVENT_TYPE_MAX,
};

#define ZF_PCIE_EC_EVENT_MAX_PERIOD GENMASK_ULL(31, 0)
#define ZF_PCIE_TBA_EVENT_MAX_PERIOD GENMASK_ULL(63, 0)

struct zf_pcie_pmu {
	struct pmu pmu;
	struct pci_dev *pdev; /* Root Port device */
	u16 ras_des; /* RAS DES capability offset */
	u32 nr_lanes;

	struct list_head pmu_node;
	struct hlist_node cpuhp_node;
	struct perf_event *event[ZF_PCIE_EVENT_TYPE_MAX - 1];
	int on_cpu;
	bool registered;
};

#define to_zf_pcie_pmu(p) (container_of(p, struct zf_pcie_pmu, pmu))

static struct platform_device *zf_pcie_pmu_dev;
static int zf_pcie_pmu_hp_state;
static struct list_head zf_pcie_pmu_head = LIST_HEAD_INIT(zf_pcie_pmu_head);
static bool zf_pcie_pmu_notify;

static ssize_t cpumask_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(pcie_pmu->on_cpu));
}
static DEVICE_ATTR_RO(cpumask);

static struct attribute *zf_pcie_pmu_cpumask_attrs[] = {
	&dev_attr_cpumask.attr, NULL
};

static struct attribute_group zf_pcie_cpumask_attr_group = {
	.attrs = zf_pcie_pmu_cpumask_attrs,
};

struct zf_pcie_format_attr {
	struct device_attribute attr;
	u64 field;
	int config;
};

static ssize_t zf_pcie_pmu_format_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct zf_pcie_format_attr *fmt =
		container_of(attr, typeof(*fmt), attr);
	int lo = __ffs(fmt->field), hi = __fls(fmt->field);

	return sysfs_emit(buf, "config:%d-%d\n", lo, hi);
}

#define _zf_pcie_format_attr(_name, _cfg, _fld)                             \
	(&((struct zf_pcie_format_attr[]){ {                                \
		.attr = __ATTR(_name, 0444, zf_pcie_pmu_format_show, NULL), \
		.config = _cfg,                                              \
		.field = _fld,                                               \
	} })[0]                                                              \
		  .attr.attr)

#define zf_pcie_format_attr(_name, _fld) _zf_pcie_format_attr(_name, 0, _fld)

static struct attribute *zf_pcie_format_attrs[] = {
	zf_pcie_format_attr(type, ZF_PCIE_CONFIG_TYPE),
	zf_pcie_format_attr(eventid, ZF_PCIE_CONFIG_EVENTID),
	zf_pcie_format_attr(lane, ZF_PCIE_CONFIG_LANE),
	NULL,
};

static struct attribute_group zf_pcie_format_attrs_group = {
	.name = "format",
	.attrs = zf_pcie_format_attrs,
};

struct zf_pcie_event_attr {
	struct device_attribute attr;
	enum zf_pcie_event_type type;
	u16 eventid;
	u8 lane;
};

static ssize_t zf_pcie_event_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct zf_pcie_event_attr *eattr;

	eattr = container_of(attr, typeof(*eattr), attr);

	if (eattr->type == ZF_PCIE_EC_EVENT)
		return sysfs_emit(buf, "eventid=0x%x,type=0x%x,lane=?\n",
				  eattr->eventid, eattr->type);
	else if (eattr->type == ZF_PCIE_TBA_EVENT)
		return sysfs_emit(buf, "eventid=0x%x,type=0x%x\n",
				  eattr->eventid, eattr->type);

	return 0;
}

#define ZF_PCIE_EVENT_ATTR(_name, _type, _eventid, _lane)              \
	(&((struct zf_pcie_event_attr[]){ {                            \
		.attr = __ATTR(_name, 0444, zf_pcie_event_show, NULL), \
		.type = _type,                                          \
		.eventid = _eventid,                                    \
		.lane = _lane,                                          \
	} })[0]                                                         \
		  .attr.attr)

#define ZF_PCIE_PMU_TBA_EVENT_ATTR(_name, _eventid) \
	ZF_PCIE_EVENT_ATTR(_name, ZF_PCIE_TBA_EVENT, _eventid, 0)
#define ZF_PCIE_PMU_EC_EVENT_ATTR(_name, _eventid) \
	ZF_PCIE_EVENT_ATTR(_name, ZF_PCIE_EC_EVENT, _eventid, 0)

static struct attribute *zf_pcie_pmu_time_event_attrs[] = {
	/* ZF TBA Group #0 - LTSSM time counters */
	ZF_PCIE_PMU_TBA_EVENT_ATTR(L1, 0x01),
	ZF_PCIE_PMU_TBA_EVENT_ATTR(L0, 0x02),
	ZF_PCIE_PMU_TBA_EVENT_ATTR(cfg_rcvry, 0x07),

	/*
	 * ZF TBA Group #1 - TLP data payload counters.
	 *
	 * Per TRM, these counters measure the payload in 16-byte units, i.e. the
	 * raw counter value is the number of 16-byte blocks, not bytes. The value
	 * read back is reported as-is (unit: 16-byte blocks); multiply by 16 to
	 * obtain the byte count.
	 */
	ZF_PCIE_PMU_TBA_EVENT_ATTR(tlp_data_ply_rx, 0x20),
	ZF_PCIE_PMU_TBA_EVENT_ATTR(tlp_data_ply_tx, 0x21),

	/*
	 * ZF EC Group #6 - DLLP counters (flow control, ACK),
	 * Abnormal TLP counters (duplicate/nullified TLP)
	 */
	ZF_PCIE_PMU_EC_EVENT_ATTR(update_dllp_fc_tx, 0x600),
	ZF_PCIE_PMU_EC_EVENT_ATTR(dllp_ack_tx, 0x601),
	ZF_PCIE_PMU_EC_EVENT_ATTR(update_dllp_fc_rx, 0x602),
	ZF_PCIE_PMU_EC_EVENT_ATTR(dllp_ack_rx, 0x603),
	ZF_PCIE_PMU_EC_EVENT_ATTR(dup_tlp_rx, 0x604),
	ZF_PCIE_PMU_EC_EVENT_ATTR(null_tlp_rx, 0x606),

	/*
	 * ZF EC Group #7 - TLP transaction counters (Memory, Config,
	 * Completion, Atomic, Message)
	 */
	ZF_PCIE_PMU_EC_EVENT_ATTR(tlp_pfx_tx, 0x700),
	ZF_PCIE_PMU_EC_EVENT_ATTR(mem_wr_tx, 0x702),
	ZF_PCIE_PMU_EC_EVENT_ATTR(mem_rd_tx, 0x703),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cfg_wr_tx, 0x706),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cfg_rd_tx, 0x707),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpl_tx, 0x708),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpld_tx, 0x709),
	ZF_PCIE_PMU_EC_EVENT_ATTR(dfr_mem_wr_rx, 0x70c),
	ZF_PCIE_PMU_EC_EVENT_ATTR(mem_wr_rx, 0x70d),
	ZF_PCIE_PMU_EC_EVENT_ATTR(mem_rd_rx, 0x70e),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpl_ca_rx, 0x70f),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpl_ur_rx, 0x710),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cfg_wr_rx, 0x711),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cfg_rd_rx, 0x712),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpl_rx, 0x713),
	ZF_PCIE_PMU_EC_EVENT_ATTR(cpld_rx, 0x714),
	ZF_PCIE_PMU_EC_EVENT_ATTR(atomic_rx, 0x715),
	ZF_PCIE_PMU_EC_EVENT_ATTR(msg_tlp_tx, 0x718),
	ZF_PCIE_PMU_EC_EVENT_ATTR(msg_tlp_rx, 0x719), NULL
};

static const struct attribute_group zf_pcie_event_attrs_group = {
	.name = "events",
	.attrs = zf_pcie_pmu_time_event_attrs,
};

static const struct attribute_group *zf_pcie_attr_groups[] = {
	&zf_pcie_event_attrs_group, &zf_pcie_format_attrs_group,
	&zf_pcie_cpumask_attr_group, NULL
};

static void zf_pcie_pmu_lane_event_enable(struct zf_pcie_pmu *pcie_pmu,
					   bool enable)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + ZF_PCIE_EVENT_CNT_CTL, &val);

	/* Clear ZF_PCIE_CNT_ENABLE field first */
	val &= ~ZF_PCIE_CNT_ENABLE;
	if (enable)
		val |= FIELD_PREP(ZF_PCIE_CNT_ENABLE, ZF_PCIE_PER_EVENT_ON);
	else
		val |= FIELD_PREP(ZF_PCIE_CNT_ENABLE, ZF_PCIE_PER_EVENT_OFF);

	pci_write_config_dword(pdev, ras_des + ZF_PCIE_EVENT_CNT_CTL, val);
}

static void zf_pcie_pmu_time_based_event_enable(struct zf_pcie_pmu *pcie_pmu,
						 bool enable)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_CTL,
			      &val);

	if (enable)
		val |= ZF_PCIE_TIME_BASED_CNT_ENABLE;
	else
		val &= ~ZF_PCIE_TIME_BASED_CNT_ENABLE;

	pci_write_config_dword(pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_CTL,
			       val);
}

static u64 zf_pcie_pmu_read_lane_event_counter(struct zf_pcie_pmu *pcie_pmu)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + ZF_PCIE_EVENT_CNT_DATA, &val);

	return val;
}

static u64 zf_pcie_pmu_read_time_based_counter(struct zf_pcie_pmu *pcie_pmu)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 lo, hi, ss;

	/*
	 * The 64-bit value of the data counter is spread across two
	 * registers that are not synchronized. In order to read them
	 * atomically, ensure that the high 32 bits match before and after
	 * reading the low 32 bits.
	 */
	pci_read_config_dword(
		pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH, &hi);
	do {
		/* snapshot the high 32 bits */
		ss = hi;

		pci_read_config_dword(
			pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_DATA_REG_LOW,
			&lo);
		pci_read_config_dword(
			pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH,
			&hi);
	} while (hi != ss);

	return ((u64)hi << 32) | lo;
}

static void zf_pcie_pmu_event_update(struct perf_event *event)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);
	u64 delta = 0, prev = 0, now = 0;

	if (type <= ZF_PCIE_TYPE_INVALID || type >= ZF_PCIE_EVENT_TYPE_MAX)
		return;

	do {
		prev = local64_read(&hwc->prev_count);

		if (type == ZF_PCIE_EC_EVENT)
			now = zf_pcie_pmu_read_lane_event_counter(pcie_pmu);
		else if (type == ZF_PCIE_TBA_EVENT)
			now = zf_pcie_pmu_read_time_based_counter(pcie_pmu);

	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	if (type == ZF_PCIE_EC_EVENT)
		delta = (now - prev) & ZF_PCIE_EC_EVENT_MAX_PERIOD;
	else if (type == ZF_PCIE_TBA_EVENT)
		delta = (now - prev) & ZF_PCIE_TBA_EVENT_MAX_PERIOD;
	local64_add(delta, &event->count);
}

static int zf_pcie_pmu_event_init(struct perf_event *event)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);
	struct perf_event *sibling;
	u32 lane;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	/* We don't support sampling */
	if (is_sampling_event(event))
		return -EINVAL;

	/* We cannot support task bound events */
	if (event->cpu < 0 || event->attach_state & PERF_ATTACH_TASK)
		return -EINVAL;

	if (event->group_leader != event &&
	    !is_software_event(event->group_leader))
		return -EINVAL;

	for_each_sibling_event(sibling, event->group_leader) {
		if (sibling->pmu != event->pmu && !is_software_event(sibling))
			return -EINVAL;
	}

	if (type == ZF_PCIE_EC_EVENT) {
		lane = ZF_PCIE_EVENT_LANE(event);
		if (lane >= pcie_pmu->nr_lanes)
			return -EINVAL;

		if (ZF_PCIE_EVENT_ID(event) > FIELD_MAX(ZF_PCIE_CNT_EVENT_SEL))
			return -EINVAL;
	} else if (type == ZF_PCIE_TBA_EVENT &&
		   ZF_PCIE_EVENT_ID(event) >
			   FIELD_MAX(ZF_PCIE_TIME_BASED_REPORT_SEL)) {
		return -EINVAL;
	}

	event->cpu = pcie_pmu->on_cpu;

	return 0;
}

static void zf_pcie_pmu_set_period(struct hw_perf_event *hwc)
{
	local64_set(&hwc->prev_count, 0);
}

static void zf_pcie_pmu_event_start(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);

	hwc->state = 0;
	zf_pcie_pmu_set_period(hwc);

	if (type == ZF_PCIE_EC_EVENT)
		zf_pcie_pmu_lane_event_enable(pcie_pmu, true);
	else if (type == ZF_PCIE_TBA_EVENT)
		zf_pcie_pmu_time_based_event_enable(pcie_pmu, true);
}

static void zf_pcie_pmu_event_stop(struct perf_event *event, int flags)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);
	struct hw_perf_event *hwc = &event->hw;

	if (event->hw.state & PERF_HES_STOPPED)
		return;

	if (type == ZF_PCIE_EC_EVENT)
		zf_pcie_pmu_lane_event_enable(pcie_pmu, false);
	else if (type == ZF_PCIE_TBA_EVENT)
		zf_pcie_pmu_time_based_event_enable(pcie_pmu, false);

	zf_pcie_pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static int zf_pcie_pmu_event_add(struct perf_event *event, int flags)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	struct pci_dev *pdev = pcie_pmu->pdev;
	struct hw_perf_event *hwc = &event->hw;
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);
	int event_id = ZF_PCIE_EVENT_ID(event);
	int lane = ZF_PCIE_EVENT_LANE(event);
	u16 ras_des = pcie_pmu->ras_des;
	u32 ctrl;

	if (type <= ZF_PCIE_TYPE_INVALID || type >= ZF_PCIE_EVENT_TYPE_MAX)
		return -EINVAL;
	/* Only one counter and it is in use */
	if (pcie_pmu->event[type - 1])
		return -ENOSPC;

	pcie_pmu->event[type - 1] = event;
	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	if (type == ZF_PCIE_EC_EVENT) {
		/* EVENT_COUNTER_DATA_REG needs clear manually */
		ctrl = FIELD_PREP(ZF_PCIE_CNT_EVENT_SEL, event_id) |
		       FIELD_PREP(ZF_PCIE_CNT_LANE_SEL, lane) |
		       FIELD_PREP(ZF_PCIE_CNT_ENABLE, ZF_PCIE_PER_EVENT_OFF) |
		       FIELD_PREP(ZF_PCIE_EVENT_CLEAR,
				  ZF_PCIE_EVENT_PER_CLEAR);
		pci_write_config_dword(pdev, ras_des + ZF_PCIE_EVENT_CNT_CTL,
				       ctrl);
	} else if (type == ZF_PCIE_TBA_EVENT) {
		/*
		 * TIME_BASED_ANAL_DATA_REG is a 64 bit register, we can safely
		 * use it with any manually controlled duration. And it is
		 * cleared when next measurement starts.
		 */
		ctrl = FIELD_PREP(ZF_PCIE_TIME_BASED_REPORT_SEL, event_id) |
		       FIELD_PREP(ZF_PCIE_TIME_BASED_DURATION_SEL,
				  ZF_PCIE_DURATION_MANUAL_CTL) |
		       ZF_PCIE_TIME_BASED_CNT_ENABLE;
		pci_write_config_dword(
			pdev, ras_des + ZF_PCIE_TIME_BASED_ANAL_CTL, ctrl);
	}

	if (flags & PERF_EF_START)
		zf_pcie_pmu_event_start(event, PERF_EF_RELOAD);

	perf_event_update_userpage(event);

	return 0;
}

static void zf_pcie_pmu_event_del(struct perf_event *event, int flags)
{
	struct zf_pcie_pmu *pcie_pmu = to_zf_pcie_pmu(event->pmu);
	enum zf_pcie_event_type type = ZF_PCIE_EVENT_TYPE(event);

	if (type <= ZF_PCIE_TYPE_INVALID || type >= ZF_PCIE_EVENT_TYPE_MAX)
		return;

	zf_pcie_pmu_event_stop(event, flags | PERF_EF_UPDATE);
	perf_event_update_userpage(event);
	pcie_pmu->event[type - 1] = NULL;
}

static void zf_pcie_pmu_remove_cpuhp_instance(void *hotplug_node)
{
	struct hlist_node *node = hotplug_node;

	/*
	 * On BUS_NOTIFY_DEL_DEVICE the instance is torn down early via
	 * hlist_del_init(); skip it here so the module-exit devm unwind does
	 * not call hlist_del() a second time on an already-poisoned node.
	 */
	if (hlist_unhashed(node))
		return;

	cpuhp_state_remove_instance_nocalls(zf_pcie_pmu_hp_state, node);
}

static void zf_pcie_pmu_unregister_pmu(void *data)
{
	struct zf_pcie_pmu *pcie_pmu = data;

	if (!pcie_pmu->registered)
		return;

	list_del_init(&pcie_pmu->pmu_node);
	perf_pmu_unregister(&pcie_pmu->pmu);
	pcie_pmu->registered = false;
}

/*
 * Find the PMU of a PCI device.
 * @pdev: The PCI device.
 */
static struct zf_pcie_pmu *zf_pcie_find_dev_pmu(struct pci_dev *pdev)
{
	struct zf_pcie_pmu *pcie_pmu, *tmp;

	list_for_each_entry_safe(pcie_pmu, tmp, &zf_pcie_pmu_head, pmu_node) {
		if (pcie_pmu->pdev == pdev) {
			list_del_init(&pcie_pmu->pmu_node);
			return pcie_pmu;
		}
	}

	return NULL;
}

static int zf_pcie_pmu_notifier(struct notifier_block *nb,
				 unsigned long action, void *data)
{
	struct device *dev = data;
	struct pci_dev *pdev = to_pci_dev(dev);
	struct zf_pcie_pmu *pcie_pmu;

	/* Unregister the PMU when the device is going to be deleted. */
	if (action != BUS_NOTIFY_DEL_DEVICE)
		return NOTIFY_DONE;

	pcie_pmu = zf_pcie_find_dev_pmu(pdev);
	if (!pcie_pmu)
		return NOTIFY_DONE;

	/*
	 * The root port is about to be released. Detach the instance from the
	 * CPU hotplug state machine under the API's lock protocol before the
	 * (freed) pdev can be dereferenced, and reset the node afterwards so the
	 * devm unwind in zf_pcie_pmu_remove_cpuhp_instance() skips it.
	 */
	cpuhp_state_remove_instance_nocalls(zf_pcie_pmu_hp_state,
					    &pcie_pmu->cpuhp_node);
	INIT_HLIST_NODE(&pcie_pmu->cpuhp_node);

	zf_pcie_pmu_unregister_pmu(pcie_pmu);

	return NOTIFY_OK;
}

static struct notifier_block zf_pcie_pmu_nb = {
	.notifier_call = zf_pcie_pmu_notifier,
};

static int zf_pcie_pmu_probe(struct platform_device *plat_dev)
{
	struct pci_dev *pdev = NULL;
	struct zf_pcie_pmu *pcie_pmu;
	bool notify = false;
	char *name;
	u32 bdf;
	int ret;

	/* Match the rootport with VSEC_RAS_DES_ID, and register a PMU for it */
	for_each_pci_dev(pdev) {
		u16 vsec;
		u32 val;

		if (!(pci_is_pcie(pdev) &&
		      pci_pcie_type(pdev) == PCI_EXP_TYPE_ROOT_PORT)) {
			continue;
		}

		vsec = pci_find_vsec_capability(pdev, PCI_VENDOR_ID_LPRS,
						ZF_PCIE_VSEC_RAS_DES_ID);
		if (!vsec)
			continue;

		pci_read_config_dword(pdev, vsec + PCI_VNDR_HEADER, &val);
		if (PCI_VNDR_HEADER_REV(val) != ZF_PCIE_VSEC_REV ||
		    PCI_VNDR_HEADER_LEN(val) != ZF_PCIE_VSEC_LEN) {
			continue;
		}

		pci_dbg(pdev,
			"Detected PCIe Vendor-Specific Extended Capability RAS DES\n");

		bdf = PCI_DEVID(pdev->bus->number, pdev->devfn);
		bdf |= pci_domain_nr(pdev->bus) << 16;
		name = devm_kasprintf(&plat_dev->dev, GFP_KERNEL,
				      "zf_rootport_%x", bdf);
		if (!name) {
			ret = -ENOMEM;
			goto out;
		}

		/* All checks passed*/
		pcie_pmu = devm_kzalloc(&plat_dev->dev, sizeof(*pcie_pmu),
					GFP_KERNEL);
		if (!pcie_pmu) {
			ret = -ENOMEM;
			goto out;
		}

		pcie_pmu->pdev = pdev;
		pcie_pmu->ras_des = vsec;
		pcie_pmu->nr_lanes = pcie_get_width_cap(pdev);
		pcie_pmu->on_cpu = -1;
		pcie_pmu->pmu = (struct pmu){
			.module = THIS_MODULE,
			.attr_groups = zf_pcie_attr_groups,
			.capabilities = PERF_PMU_CAP_NO_EXCLUDE,
			.task_ctx_nr = perf_invalid_context,
			.event_init = zf_pcie_pmu_event_init,
			.add = zf_pcie_pmu_event_add,
			.del = zf_pcie_pmu_event_del,
			.start = zf_pcie_pmu_event_start,
			.stop = zf_pcie_pmu_event_stop,
			.read = zf_pcie_pmu_event_update,
		};

		/* Add this instance to the list used by the offline callback */
		ret = cpuhp_state_add_instance(zf_pcie_pmu_hp_state,
					       &pcie_pmu->cpuhp_node);
		if (ret) {
			pci_err(pcie_pmu->pdev,
				"Error %d registering hotplug @%x\n", ret, bdf);
			goto out;
		}

		/* Unwind when platform driver removes */
		ret = devm_add_action_or_reset(
			&plat_dev->dev, zf_pcie_pmu_remove_cpuhp_instance,
			&pcie_pmu->cpuhp_node);
		if (ret)
			goto out;

		ret = perf_pmu_register(&pcie_pmu->pmu, name, -1);
		if (ret) {
			pci_err(pcie_pmu->pdev,
				"Error %d registering PMU @%x\n", ret, bdf);
			goto out;
		}
		ret = devm_add_action_or_reset(
			&plat_dev->dev, zf_pcie_pmu_unregister_pmu, pcie_pmu);
		if (ret)
			goto out;

		/* Cache PMU to handle pci device hotplug */
		list_add(&pcie_pmu->pmu_node, &zf_pcie_pmu_head);
		pcie_pmu->registered = true;
		notify = true;
	}

	if (notify) {
		ret = bus_register_notifier(&pci_bus_type, &zf_pcie_pmu_nb);
		if (ret) {
			struct zf_pcie_pmu *pmu, *tmp;

			list_for_each_entry_safe(pmu, tmp, &zf_pcie_pmu_head,
						 pmu_node) {
				list_del_init(&pmu->pmu_node);
				zf_pcie_pmu_unregister_pmu(pmu);
			}
			goto out;
		}
		zf_pcie_pmu_notify = true;
	}

	return 0;

out:
	pci_dev_put(pdev);

	return ret;
}

static int zf_pcie_pmu_online_cpu(unsigned int cpu,
				   struct hlist_node *cpuhp_node)
{
	struct zf_pcie_pmu *pcie_pmu;

	pcie_pmu =
		hlist_entry_safe(cpuhp_node, struct zf_pcie_pmu, cpuhp_node);
	if (pcie_pmu->on_cpu == -1)
		pcie_pmu->on_cpu = cpumask_local_spread(
			0, dev_to_node(&pcie_pmu->pdev->dev));

	return 0;
}

static int zf_pcie_pmu_offline_cpu(unsigned int cpu,
				    struct hlist_node *cpuhp_node)
{
	struct zf_pcie_pmu *pcie_pmu;
	struct pci_dev *pdev;
	int node;
	cpumask_t mask;
	unsigned int target;

	pcie_pmu =
		hlist_entry_safe(cpuhp_node, struct zf_pcie_pmu, cpuhp_node);
	/* Nothing to do if this CPU doesn't own the PMU */
	if (cpu != pcie_pmu->on_cpu)
		return 0;

	pcie_pmu->on_cpu = -1;
	pdev = pcie_pmu->pdev;
	node = dev_to_node(&pdev->dev);
	if (cpumask_and(&mask, cpumask_of_node(node), cpu_online_mask) &&
	    cpumask_andnot(&mask, &mask, cpumask_of(cpu)))
		target = cpumask_any(&mask);
	else
		target = cpumask_any_but(cpu_online_mask, cpu);

	if (target >= nr_cpu_ids) {
		pci_err(pcie_pmu->pdev, "There is no CPU to set\n");
		return 0;
	}

	/* This PMU does NOT support interrupt, just migrate context. */
	perf_pmu_migrate_context(&pcie_pmu->pmu, cpu, target);
	pcie_pmu->on_cpu = target;

	return 0;
}

static struct platform_driver zf_pcie_pmu_driver = {
	.probe = zf_pcie_pmu_probe,
	.driver = {.name = "zf_pcie_pmu",},
};

static int __init zf_pcie_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "perf/zf_pcie_pmu:online",
				      zf_pcie_pmu_online_cpu,
				      zf_pcie_pmu_offline_cpu);
	if (ret < 0) {
		pr_err("Failed to setup PCIe PMU hotplug: %d\n", ret);
		return ret;
	}

	zf_pcie_pmu_hp_state = ret;

	ret = platform_driver_register(&zf_pcie_pmu_driver);
	if (ret) {
		cpuhp_remove_multi_state(zf_pcie_pmu_hp_state);
		return ret;
	}

	zf_pcie_pmu_dev = platform_device_register_simple(
		"zf_pcie_pmu", PLATFORM_DEVID_NONE, NULL, 0);
	if (IS_ERR(zf_pcie_pmu_dev)) {
		platform_driver_unregister(&zf_pcie_pmu_driver);
		cpuhp_remove_multi_state(zf_pcie_pmu_hp_state);
		return PTR_ERR(zf_pcie_pmu_dev);
	}

	return 0;
}

static void __exit zf_pcie_pmu_exit(void)
{
	struct zf_pcie_pmu *pcie_pmu, *tmp;

	list_for_each_entry_safe(pcie_pmu, tmp, &zf_pcie_pmu_head, pmu_node)
		list_del_init(&pcie_pmu->pmu_node);

	platform_device_unregister(zf_pcie_pmu_dev);
	platform_driver_unregister(&zf_pcie_pmu_driver);
	cpuhp_remove_multi_state(zf_pcie_pmu_hp_state);

	if (zf_pcie_pmu_notify)
		bus_unregister_notifier(&pci_bus_type, &zf_pcie_pmu_nb);
}

module_init(zf_pcie_pmu_init);
module_exit(zf_pcie_pmu_exit);

MODULE_DESCRIPTION("LeapRise ZF(ZhuFeng) PCI Express Controller PMU driver");
MODULE_AUTHOR("Guo Wu");
MODULE_LICENSE("GPL");
