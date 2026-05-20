// SPDX-License-Identifier: GPL-2.0-only
/*
 * PCIe Sub-system PMU Events Monitoring Support
 *
 * Copyright (C) 2025 LRW Corporation or its affiliates.
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

#define LRW_PCIE_VSEC_RAS_DES_ID 0x02
#define LRW_PCIE_EVENT_CNT_CTL   0x8
#define LRW_PCIE_VSEC_REV        0x4
#define LRW_PCIE_VSEC_LEN        0x100
#define PCI_VENDOR_ID_LRW        0x1cf2

/*
 * Event Counter Data Select includes two parts:
 * - 27-24: Group number(4-bit: 0..0x7)
 * - 23-16: Event number(8-bit: 0..0x13) within the Group
 *
 * Put them togother as TRM used.
 */
#define LRW_PCIE_CNT_EVENT_SEL GENMASK(27, 16)
#define LRW_PCIE_CNT_LANE_SEL GENMASK(11, 8)
#define LRW_PCIE_CNT_STATUS BIT(7)
#define LRW_PCIE_CNT_ENABLE GENMASK(4, 2)
#define LRW_PCIE_PER_EVENT_OFF 0x1
#define LRW_PCIE_PER_EVENT_ON 0x3
#define LRW_PCIE_EVENT_CLEAR GENMASK(1, 0)
#define LRW_PCIE_EVENT_PER_CLEAR 0x1

#define LRW_PCIE_EVENT_CNT_DATA 0xC

#define LRW_PCIE_TIME_BASED_ANAL_CTL 0x10
#define LRW_PCIE_TIME_BASED_REPORT_SEL GENMASK(31, 24)
#define LRW_PCIE_TIME_BASED_DURATION_SEL GENMASK(15, 8)
#define LRW_PCIE_DURATION_MANUAL_CTL 0x0
#define LRW_PCIE_DURATION_1MS 0x1
#define LRW_PCIE_DURATION_10MS 0x2
#define LRW_PCIE_DURATION_100MS 0x3
#define LRW_PCIE_DURATION_1S 0x4
#define LRW_PCIE_DURATION_2S 0x5
#define LRW_PCIE_DURATION_4S 0x6
#define LRW_PCIE_DURATION_4US 0xFF
#define LRW_PCIE_TIME_BASED_TIMER_START BIT(0)
#define LRW_PCIE_TIME_BASED_CNT_ENABLE 0x1

#define LRW_PCIE_TIME_BASED_ANAL_DATA_REG_LOW 0x14
#define LRW_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH 0x18

/* Event attributes */
#define LRW_PCIE_CONFIG_EVENTID GENMASK(15, 0)
#define LRW_PCIE_CONFIG_TYPE GENMASK(19, 16)
#define LRW_PCIE_CONFIG_LANE GENMASK(27, 20)

#define LRW_PCIE_EVENT_ID(event) \
	FIELD_GET(LRW_PCIE_CONFIG_EVENTID, (event)->attr.config)
#define LRW_PCIE_EVENT_TYPE(event) \
	FIELD_GET(LRW_PCIE_CONFIG_TYPE, (event)->attr.config)
#define LRW_PCIE_EVENT_LANE(event) \
	FIELD_GET(LRW_PCIE_CONFIG_LANE, (event)->attr.config)

enum lrw_pcie_event_type {
	LRW_PCIE_TYPE_INVALID,
	LRW_PCIE_TIME_BASE_EVENT,
	LRW_PCIE_LANE_EVENT,
	LRW_PCIE_EVENT_TYPE_MAX,
};

#define LRW_PCIE_LANE_EVENT_MAX_PERIOD GENMASK_ULL(31, 0)
#define LRW_PCIE_TIME_BASED_EVENT_MAX_PERIOD GENMASK_ULL(63, 0)

struct lrw_pcie_pmu {
	struct pmu pmu;
	struct pci_dev *pdev; /* Root Port device */
	u16 ras_des; /* RAS DES capability offset */
	u32 nr_lanes;

	struct list_head pmu_node;
	struct hlist_node cpuhp_node;
	struct perf_event *event[LRW_PCIE_EVENT_TYPE_MAX - 1];
	int on_cpu;
	bool registered;
};

#define to_lrw_pcie_pmu(p) (container_of(p, struct lrw_pcie_pmu, pmu))

static struct platform_device *lrw_pcie_pmu_dev;
static int lrw_pcie_pmu_hp_state;
static struct list_head lrw_pcie_pmu_head = LIST_HEAD_INIT(lrw_pcie_pmu_head);
static bool lrw_pcie_pmu_notify;

static ssize_t cpumask_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(dev_get_drvdata(dev));

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(pcie_pmu->on_cpu));
}
static DEVICE_ATTR_RO(cpumask);

static struct attribute *lrw_pcie_pmu_cpumask_attrs[] = {
	&dev_attr_cpumask.attr, NULL
};

static struct attribute_group lrw_pcie_cpumask_attr_group = {
	.attrs = lrw_pcie_pmu_cpumask_attrs,
};

struct lrw_pcie_format_attr {
	struct device_attribute attr;
	u64 field;
	int config;
};

u16 pci_find_vsec_capability(struct pci_dev *dev, u16 vendor, int cap)
{
	u16 vsec = 0;
	u32 header;
	int ret;

	if (vendor != dev->vendor)
		return 0;

	while ((vsec = pci_find_next_ext_capability(dev, vsec,
						    PCI_EXT_CAP_ID_VNDR))) {
		ret = pci_read_config_dword(dev, vsec + PCI_VNDR_HEADER,
					    &header);
		if (ret != PCIBIOS_SUCCESSFUL) {
			pci_err(dev, "Failed to read VSEC header at 0x%x\n",
				vsec);
			continue;
		}

		if (PCI_VNDR_HEADER_ID(header) == cap)
			return vsec;
	}
	return 0;
}

static ssize_t lrw_pcie_pmu_format_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	struct lrw_pcie_format_attr *fmt =
		container_of(attr, typeof(*fmt), attr);
	int lo = __ffs(fmt->field), hi = __fls(fmt->field);

	return sysfs_emit(buf, "config:%d-%d\n", lo, hi);
}

#define _lrw_pcie_format_attr(_name, _cfg, _fld)                             \
	(&((struct lrw_pcie_format_attr[]){ {                                \
		.attr = __ATTR(_name, 0444, lrw_pcie_pmu_format_show, NULL), \
		.config = _cfg,                                              \
		.field = _fld,                                               \
	} })[0]                                                              \
		  .attr.attr)

#define lrw_pcie_format_attr(_name, _fld) _lrw_pcie_format_attr(_name, 0, _fld)

static struct attribute *lrw_pcie_format_attrs[] = {
	lrw_pcie_format_attr(type, LRW_PCIE_CONFIG_TYPE),
	lrw_pcie_format_attr(eventid, LRW_PCIE_CONFIG_EVENTID),
	lrw_pcie_format_attr(lane, LRW_PCIE_CONFIG_LANE),
	NULL,
};

static struct attribute_group lrw_pcie_format_attrs_group = {
	.name = "format",
	.attrs = lrw_pcie_format_attrs,
};

struct lrw_pcie_event_attr {
	struct device_attribute attr;
	enum lrw_pcie_event_type type;
	u16 eventid;
	u8 lane;
};

static ssize_t lrw_pcie_event_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct lrw_pcie_event_attr *eattr;

	eattr = container_of(attr, typeof(*eattr), attr);

	if (eattr->type == LRW_PCIE_LANE_EVENT)
		return sysfs_emit(buf, "eventid=0x%x,type=0x%x,lane=?\n",
				  eattr->eventid, eattr->type);
	else if (eattr->type == LRW_PCIE_TIME_BASE_EVENT)
		return sysfs_emit(buf, "eventid=0x%x,type=0x%x\n",
				  eattr->eventid, eattr->type);

	return 0;
}

#define LRW_PCIE_EVENT_ATTR(_name, _type, _eventid, _lane)              \
	(&((struct lrw_pcie_event_attr[]){ {                            \
		.attr = __ATTR(_name, 0444, lrw_pcie_event_show, NULL), \
		.type = _type,                                          \
		.eventid = _eventid,                                    \
		.lane = _lane,                                          \
	} })[0]                                                         \
		  .attr.attr)

#define LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(_name, _eventid) \
	LRW_PCIE_EVENT_ATTR(_name, LRW_PCIE_TIME_BASE_EVENT, _eventid, 0)
#define LRW_PCIE_PMU_LANE_EVENT_ATTR(_name, _eventid) \
	LRW_PCIE_EVENT_ATTR(_name, LRW_PCIE_LANE_EVENT, _eventid, 0)

static struct attribute *lrw_pcie_pmu_time_event_attrs[] = {
	/* Time Based Analysis Group #0 */
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(one_cycle, 0x00),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(L1, 0x01),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(L0, 0x02),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(RX_L0S, 0x03),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(TX_L0S, 0x04),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(L1_1, 0x05),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(L1_2, 0x06),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(CFG_RCVRY, 0x07),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(TX_RX_L0S, 0x08),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(L1_AUX, 0x09),

	/* Time Based Analysis Group #1 */
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(Rx_PCIe_TLP_Data_Payload, 0x20),
	LRW_PCIE_PMU_TIME_BASE_EVENT_ATTR(Tx_PCIe_TLP_Data_Payload, 0x21),

	/* Error Counter Group #6 */
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_update_fc_dllp, 0x600),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_ack_dllp, 0x601),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_update_fc_dllp, 0x602),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_ack_dllp, 0x603),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_duplicate_tlp, 0x604),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_nulified_tlp, 0x605),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_nulified_tlp, 0x606),

	/* Error Counter Group #7 */
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_tlp_with_prefix, 0x700),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_deferrable_memory_write_tlp, 0x701),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_memory_write, 0x702),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_memory_read, 0x703),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_io_write, 0x704),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_io_read, 0x705),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_configuration_write, 0x706),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_configuration_read, 0x707),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_completion_without_data, 0x708),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_completion_with_data, 0x709),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_atomic, 0x70a),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_tlp_with_prefix, 0x70b),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_deferrable_memory_write_tlp, 0x70c),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_memory_write, 0x70d),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_memory_read, 0x70e),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_io_write, 0x70F),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_io_read, 0x710),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_configuration_write, 0x711),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_configuration_read, 0x712),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_completion_without_data, 0x713),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_completion_with_data, 0x714),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_atomic, 0x715),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_ccix_tlp, 0x716),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_ccix_tlp, 0x717),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(tx_message_tlp, 0x718),
	LRW_PCIE_PMU_LANE_EVENT_ATTR(rx_message_tlp, 0x719), NULL
};

static const struct attribute_group lrw_pcie_event_attrs_group = {
	.name = "events",
	.attrs = lrw_pcie_pmu_time_event_attrs,
};

static const struct attribute_group *lrw_pcie_attr_groups[] = {
	&lrw_pcie_event_attrs_group, &lrw_pcie_format_attrs_group,
	&lrw_pcie_cpumask_attr_group, NULL
};

static void lrw_pcie_pmu_lane_event_enable(struct lrw_pcie_pmu *pcie_pmu,
					   bool enable)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + LRW_PCIE_EVENT_CNT_CTL, &val);

	/* Clear LRW_PCIE_CNT_ENABLE field first */
	val &= ~LRW_PCIE_CNT_ENABLE;
	if (enable)
		val |= FIELD_PREP(LRW_PCIE_CNT_ENABLE, LRW_PCIE_PER_EVENT_ON);
	else
		val |= FIELD_PREP(LRW_PCIE_CNT_ENABLE, LRW_PCIE_PER_EVENT_OFF);

	pci_write_config_dword(pdev, ras_des + LRW_PCIE_EVENT_CNT_CTL, val);
}

static void lrw_pcie_pmu_time_based_event_enable(struct lrw_pcie_pmu *pcie_pmu,
						 bool enable)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_CTL,
			      &val);

	if (enable)
		val |= LRW_PCIE_TIME_BASED_CNT_ENABLE;
	else
		val &= ~LRW_PCIE_TIME_BASED_CNT_ENABLE;

	pci_write_config_dword(pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_CTL,
			       val);
}

static u64 lrw_pcie_pmu_read_lane_event_counter(struct lrw_pcie_pmu *pcie_pmu)
{
	struct pci_dev *pdev = pcie_pmu->pdev;
	u16 ras_des = pcie_pmu->ras_des;
	u32 val;

	pci_read_config_dword(pdev, ras_des + LRW_PCIE_EVENT_CNT_DATA, &val);

	return val;
}

static u64 lrw_pcie_pmu_read_time_based_counter(struct lrw_pcie_pmu *pcie_pmu)
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
		pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH, &hi);
	do {
		/* snapshot the high 32 bits */
		ss = hi;

		pci_read_config_dword(
			pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_DATA_REG_LOW,
			&lo);
		pci_read_config_dword(
			pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_DATA_REG_HIGH,
			&hi);
	} while (hi != ss);

	return ((u64)hi << 32) | lo;
}

static void lrw_pcie_pmu_event_update(struct perf_event *event)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);
	u64 delta, prev, now;

	do {
		prev = local64_read(&hwc->prev_count);

		if (type == LRW_PCIE_LANE_EVENT)
			now = lrw_pcie_pmu_read_lane_event_counter(pcie_pmu);
		else if (type == LRW_PCIE_TIME_BASE_EVENT)
			now = lrw_pcie_pmu_read_time_based_counter(pcie_pmu);

	} while (local64_cmpxchg(&hwc->prev_count, prev, now) != prev);

	if (type == LRW_PCIE_LANE_EVENT)
		delta = (now - prev) & LRW_PCIE_LANE_EVENT_MAX_PERIOD;
	else if (type == LRW_PCIE_TIME_BASE_EVENT)
		delta = (now - prev) & LRW_PCIE_TIME_BASED_EVENT_MAX_PERIOD;
	local64_add(delta, &event->count);
}

static int lrw_pcie_pmu_event_init(struct perf_event *event)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);
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

	if (type == LRW_PCIE_LANE_EVENT) {
		lane = LRW_PCIE_EVENT_LANE(event);
		if (lane < 0 || lane >= pcie_pmu->nr_lanes)
			return -EINVAL;
	}

	event->cpu = pcie_pmu->on_cpu;

	return 0;
}

static void lrw_pcie_pmu_set_period(struct hw_perf_event *hwc)
{
	local64_set(&hwc->prev_count, 0);
}

static void lrw_pcie_pmu_event_start(struct perf_event *event, int flags)
{
	struct hw_perf_event *hwc = &event->hw;
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);

	hwc->state = 0;
	lrw_pcie_pmu_set_period(hwc);

	if (type == LRW_PCIE_LANE_EVENT)
		lrw_pcie_pmu_lane_event_enable(pcie_pmu, true);
	else if (type == LRW_PCIE_TIME_BASE_EVENT)
		lrw_pcie_pmu_time_based_event_enable(pcie_pmu, true);
}

static void lrw_pcie_pmu_event_stop(struct perf_event *event, int flags)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);
	struct hw_perf_event *hwc = &event->hw;

	if (event->hw.state & PERF_HES_STOPPED)
		return;

	if (type == LRW_PCIE_LANE_EVENT)
		lrw_pcie_pmu_lane_event_enable(pcie_pmu, false);
	else if (type == LRW_PCIE_TIME_BASE_EVENT)
		lrw_pcie_pmu_time_based_event_enable(pcie_pmu, false);

	lrw_pcie_pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static int lrw_pcie_pmu_event_add(struct perf_event *event, int flags)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	struct pci_dev *pdev = pcie_pmu->pdev;
	struct hw_perf_event *hwc = &event->hw;
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);
	int event_id = LRW_PCIE_EVENT_ID(event);
	int lane = LRW_PCIE_EVENT_LANE(event);
	u16 ras_des = pcie_pmu->ras_des;
	u32 ctrl;

	if (type <= LRW_PCIE_TYPE_INVALID)
		return -EINVAL;
	/* Only one counter and it is in use */
	if (pcie_pmu->event[type - 1])
		return -ENOSPC;

	pcie_pmu->event[type - 1] = event;
	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;

	if (type == LRW_PCIE_LANE_EVENT) {
		/* EVENT_COUNTER_DATA_REG needs clear manually */
		ctrl = FIELD_PREP(LRW_PCIE_CNT_EVENT_SEL, event_id) |
		       FIELD_PREP(LRW_PCIE_CNT_LANE_SEL, lane) |
		       FIELD_PREP(LRW_PCIE_CNT_ENABLE, LRW_PCIE_PER_EVENT_OFF) |
		       FIELD_PREP(LRW_PCIE_EVENT_CLEAR,
				  LRW_PCIE_EVENT_PER_CLEAR);
		pci_write_config_dword(pdev, ras_des + LRW_PCIE_EVENT_CNT_CTL,
				       ctrl);
	} else if (type == LRW_PCIE_TIME_BASE_EVENT) {
		/*
		 * TIME_BASED_ANAL_DATA_REG is a 64 bit register, we can safely
		 * use it with any manually controlled duration. And it is
		 * cleared when next measurement starts.
		 */
		ctrl = FIELD_PREP(LRW_PCIE_TIME_BASED_REPORT_SEL, event_id) |
		       FIELD_PREP(LRW_PCIE_TIME_BASED_DURATION_SEL,
				  LRW_PCIE_DURATION_MANUAL_CTL) |
		       LRW_PCIE_TIME_BASED_CNT_ENABLE;
		pci_write_config_dword(
			pdev, ras_des + LRW_PCIE_TIME_BASED_ANAL_CTL, ctrl);
	}

	if (flags & PERF_EF_START)
		lrw_pcie_pmu_event_start(event, PERF_EF_RELOAD);

	perf_event_update_userpage(event);

	return 0;
}

static void lrw_pcie_pmu_event_del(struct perf_event *event, int flags)
{
	struct lrw_pcie_pmu *pcie_pmu = to_lrw_pcie_pmu(event->pmu);
	enum lrw_pcie_event_type type = LRW_PCIE_EVENT_TYPE(event);

	lrw_pcie_pmu_event_stop(event, flags | PERF_EF_UPDATE);
	perf_event_update_userpage(event);
	pcie_pmu->event[type - 1] = NULL;
}

static void lrw_pcie_pmu_remove_cpuhp_instance(void *hotplug_node)
{
	cpuhp_state_remove_instance_nocalls(lrw_pcie_pmu_hp_state,
					    hotplug_node);
}

static void lrw_pcie_pmu_unregister_pmu(void *data)
{
	struct lrw_pcie_pmu *pcie_pmu = data;

	if (!pcie_pmu->registered)
		return;

	perf_pmu_unregister(&pcie_pmu->pmu);
	pcie_pmu->registered = false;
}

/*
 * Find the PMU of a PCI device.
 * @pdev: The PCI device.
 */
static struct lrw_pcie_pmu *lrw_pcie_find_dev_pmu(struct pci_dev *pdev)
{
	struct lrw_pcie_pmu *pcie_pmu, *tmp;

	list_for_each_entry_safe(pcie_pmu, tmp, &lrw_pcie_pmu_head, pmu_node) {
		if (pcie_pmu->pdev == pdev) {
			list_del(&pcie_pmu->pmu_node);
			return pcie_pmu;
		}
	}

	return NULL;
}

static int lrw_pcie_pmu_notifier(struct notifier_block *nb,
				 unsigned long action, void *data)
{
	struct device *dev = data;
	struct pci_dev *pdev = to_pci_dev(dev);
	struct lrw_pcie_pmu *pcie_pmu;

	/* Unregister the PMU when the device is going to be deleted. */
	if (action != BUS_NOTIFY_DEL_DEVICE)
		return NOTIFY_DONE;

	pcie_pmu = lrw_pcie_find_dev_pmu(pdev);
	if (!pcie_pmu)
		return NOTIFY_DONE;

	lrw_pcie_pmu_unregister_pmu(pcie_pmu);

	return NOTIFY_OK;
}

static struct notifier_block lrw_pcie_pmu_nb = {
	.notifier_call = lrw_pcie_pmu_notifier,
};

static int lrw_pcie_pmu_probe(struct platform_device *plat_dev)
{
	struct pci_dev *pdev = NULL;
	struct lrw_pcie_pmu *pcie_pmu;
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

		vsec = pci_find_vsec_capability(pdev, PCI_VENDOR_ID_LRW,
						LRW_PCIE_VSEC_RAS_DES_ID);
		if (!vsec)
			continue;

		pci_read_config_dword(pdev, vsec + PCI_VNDR_HEADER, &val);
		if (PCI_VNDR_HEADER_REV(val) != LRW_PCIE_VSEC_REV ||
		    PCI_VNDR_HEADER_LEN(val) != LRW_PCIE_VSEC_LEN) {
			continue;
		}

		pci_dbg(pdev,
			"Detected PCIe Vendor-Specific Extended Capability RAS DES\n");

		bdf = PCI_DEVID(pdev->bus->number, pdev->devfn);
		bdf |= pci_domain_nr(pdev->bus) << 16;
		name = devm_kasprintf(&plat_dev->dev, GFP_KERNEL,
				      "lrw_rootport_%x", bdf);
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
			.attr_groups = lrw_pcie_attr_groups,
			.capabilities = PERF_PMU_CAP_NO_EXCLUDE,
			.task_ctx_nr = perf_invalid_context,
			.event_init = lrw_pcie_pmu_event_init,
			.add = lrw_pcie_pmu_event_add,
			.del = lrw_pcie_pmu_event_del,
			.start = lrw_pcie_pmu_event_start,
			.stop = lrw_pcie_pmu_event_stop,
			.read = lrw_pcie_pmu_event_update,
		};

		/* Add this instance to the list used by the offline callback */
		ret = cpuhp_state_add_instance(lrw_pcie_pmu_hp_state,
					       &pcie_pmu->cpuhp_node);
		if (ret) {
			pci_err(pcie_pmu->pdev,
				"Error %d registering hotplug @%x\n", ret, bdf);
			goto out;
		}

		/* Unwind when platform driver removes */
		ret = devm_add_action_or_reset(
			&plat_dev->dev, lrw_pcie_pmu_remove_cpuhp_instance,
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
			&plat_dev->dev, lrw_pcie_pmu_unregister_pmu, pcie_pmu);
		if (ret)
			goto out;

		/* Cache PMU to handle pci device hotplug */
		list_add(&pcie_pmu->pmu_node, &lrw_pcie_pmu_head);
		pcie_pmu->registered = true;
		notify = true;
	}

	if (notify && bus_register_notifier(&pci_bus_type, &lrw_pcie_pmu_nb))
		notify = false;

	if (notify)
		lrw_pcie_pmu_notify = true;

	return 0;

out:
	pci_dev_put(pdev);

	return ret;
}

static int lrw_pcie_pmu_online_cpu(unsigned int cpu,
				   struct hlist_node *cpuhp_node)
{
	struct lrw_pcie_pmu *pcie_pmu;

	pcie_pmu =
		hlist_entry_safe(cpuhp_node, struct lrw_pcie_pmu, cpuhp_node);
	if (pcie_pmu->on_cpu == -1)
		pcie_pmu->on_cpu = cpumask_local_spread(
			0, dev_to_node(&pcie_pmu->pdev->dev));

	return 0;
}

static int lrw_pcie_pmu_offline_cpu(unsigned int cpu,
				    struct hlist_node *cpuhp_node)
{
	struct lrw_pcie_pmu *pcie_pmu;
	struct pci_dev *pdev;
	int node;
	cpumask_t mask;
	unsigned int target;

	pcie_pmu =
		hlist_entry_safe(cpuhp_node, struct lrw_pcie_pmu, cpuhp_node);
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

static struct platform_driver lrw_pcie_pmu_driver = {
	.probe = lrw_pcie_pmu_probe,
	.driver = {.name = "lrw_pcie_pmu",},
};

static int __init lrw_pcie_pmu_init(void)
{
	int ret;

	ret = cpuhp_setup_state_multi(CPUHP_AP_ONLINE_DYN,
				      "perf/lrw_pcie_pmu:online",
				      lrw_pcie_pmu_online_cpu,
				      lrw_pcie_pmu_offline_cpu);
	if (ret < 0) {
		pr_err("Failed to setup PCIe PMU hotplug: %d\n", ret);
		return ret;
	}

	lrw_pcie_pmu_hp_state = ret;

	ret = platform_driver_register(&lrw_pcie_pmu_driver);
	if (ret) {
		cpuhp_remove_multi_state(lrw_pcie_pmu_hp_state);
		return ret;
	}

	lrw_pcie_pmu_dev = platform_device_register_simple(
		"lrw_pcie_pmu", PLATFORM_DEVID_NONE, NULL, 0);
	if (IS_ERR(lrw_pcie_pmu_dev)) {
		platform_driver_unregister(&lrw_pcie_pmu_driver);
		return PTR_ERR(lrw_pcie_pmu_dev);
	}

	return 0;
}

static void __exit lrw_pcie_pmu_exit(void)
{
	struct lrw_pcie_pmu *pcie_pmu, *tmp;

	list_for_each_entry_safe(pcie_pmu, tmp, &lrw_pcie_pmu_head, pmu_node)
		list_del(&pcie_pmu->pmu_node);

	platform_device_unregister(lrw_pcie_pmu_dev);
	platform_driver_unregister(&lrw_pcie_pmu_driver);
	cpuhp_remove_multi_state(lrw_pcie_pmu_hp_state);

	if (lrw_pcie_pmu_notify)
		bus_unregister_notifier(&pci_bus_type, &lrw_pcie_pmu_nb);
}

module_init(lrw_pcie_pmu_init);
module_exit(lrw_pcie_pmu_exit);

MODULE_DESCRIPTION("PMU driver for LRW PCI Express Controller");
MODULE_AUTHOR("Jiabao Wu");
MODULE_AUTHOR("Chang Guo");
MODULE_AUTHOR("Wu Guo");
MODULE_LICENSE("GPL");
