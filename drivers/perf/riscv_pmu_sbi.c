// SPDX-License-Identifier: GPL-2.0
/*
 * RISC-V performance counter support.
 *
 * Copyright (C) 2021 Western Digital Corporation or its affiliates.
 *
 * This code is based on ARM perf event code which is in turn based on
 * sparc64 and x86 code.
 */

#define pr_fmt(fmt) "riscv-pmu-sbi: " fmt

#include <linux/mod_devicetable.h>
#include <linux/limits.h>
#include <linux/perf/riscv_pmu.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/riscv_sse.h>
#include <linux/cpu_pm.h>
#include <linux/sched/clock.h>
#include <linux/soc/andes/irq.h>
#include <linux/workqueue.h>

#include <asm/errata_list.h>
#include <asm/sbi.h>
#include <asm/cpufeature.h>
#include <asm/vendor_extensions.h>
#include <asm/vendor_extensions/andes.h>
#include <asm/hwcap.h>
#include <asm/csr_indirect.h>

#define ALT_SBI_PMU_OVERFLOW(__ovl)					\
asm volatile(ALTERNATIVE_2(						\
	"csrr %0, " __stringify(CSR_SCOUNTOVF),				\
	"csrr %0, " __stringify(THEAD_C9XX_CSR_SCOUNTEROF),		\
		THEAD_VENDOR_ID, ERRATA_THEAD_PMU,			\
		CONFIG_ERRATA_THEAD_PMU,				\
	"csrr %0, " __stringify(ANDES_CSR_SCOUNTEROF),			\
		ANDES_VENDOR_ID,					\
		RISCV_ISA_VENDOR_EXT_XANDESPMU + RISCV_VENDOR_EXT_ALTERNATIVES_BASE, \
		CONFIG_ANDES_CUSTOM_PMU)				\
	: "=r" (__ovl) :						\
	: "memory")

#define ALT_SBI_PMU_OVF_CLEAR_PENDING(__irq_mask)			\
asm volatile(ALTERNATIVE(						\
	"csrc " __stringify(CSR_IP) ", %0\n\t",				\
	"csrc " __stringify(ANDES_CSR_SLIP) ", %0\n\t",			\
		ANDES_VENDOR_ID,					\
		RISCV_ISA_VENDOR_EXT_XANDESPMU + RISCV_VENDOR_EXT_ALTERNATIVES_BASE, \
		CONFIG_ANDES_CUSTOM_PMU)				\
	: : "r"(__irq_mask)						\
	: "memory")

#define SYSCTL_NO_USER_ACCESS	0
#define SYSCTL_USER_ACCESS	1
#define SYSCTL_LEGACY		2

#define PERF_EVENT_FLAG_NO_USER_ACCESS	BIT(SYSCTL_NO_USER_ACCESS)
#define PERF_EVENT_FLAG_USER_ACCESS	BIT(SYSCTL_USER_ACCESS)
#define PERF_EVENT_FLAG_LEGACY		BIT(SYSCTL_LEGACY)

#define RVPMU_SBI_PMU_FORMAT_ATTR	"config:0-47"
#define RVPMU_CDELEG_PMU_FORMAT_ATTR	"config:0-55"

static ssize_t __maybe_unused rvpmu_format_show(struct device *dev, struct device_attribute *attr,
						char *buf);

#define RVPMU_ATTR_ENTRY(_name, _func, _config)	(			\
	&((struct dev_ext_attribute[]) {				\
		{ __ATTR(_name, 0444, _func, NULL), (void *)_config }	\
	})[0].attr.attr)

#define RVPMU_FORMAT_ATTR_ENTRY(_name, _config) \
	RVPMU_ATTR_ENTRY(_name, rvpmu_format_show, (char *)_config)

PMU_FORMAT_ATTR(firmware, "config:62-63");

static bool sbi_v2_available;
static bool sbi_v3_available;
static DEFINE_STATIC_KEY_FALSE(sbi_pmu_snapshot_available);
#define sbi_pmu_snapshot_available() \
	static_branch_unlikely(&sbi_pmu_snapshot_available)

static DEFINE_STATIC_KEY_FALSE(riscv_pmu_sbi_available);
#define riscv_pmu_sbi_available() \
		static_branch_likely(&riscv_pmu_sbi_available)

static DEFINE_STATIC_KEY_FALSE(riscv_pmu_cdeleg_available);

/* Avoid unnecessary code patching in the one time booting path*/
#define riscv_pmu_cdeleg_available_boot() \
	static_key_enabled(&riscv_pmu_cdeleg_available)
#define riscv_pmu_sbi_available_boot() \
	static_key_enabled(&riscv_pmu_sbi_available)

/* Perform a runtime code patching with static key */
#define riscv_pmu_cdeleg_available() \
	static_branch_unlikely(&riscv_pmu_cdeleg_available)
#define riscv_pmu_sbi_available() \
		static_branch_likely(&riscv_pmu_sbi_available)

static struct attribute *riscv_sbi_pmu_formats_attr[] = {
	RVPMU_FORMAT_ATTR_ENTRY(event, RVPMU_SBI_PMU_FORMAT_ATTR),
	&format_attr_firmware.attr,
	NULL,
};

static struct attribute_group riscv_sbi_pmu_format_group = {
	.name = "format",
	.attrs = riscv_sbi_pmu_formats_attr,
};

static const struct attribute_group *riscv_sbi_pmu_attr_groups[] = {
	&riscv_sbi_pmu_format_group,
	NULL,
};

static struct attribute *riscv_cdeleg_pmu_formats_attr[] = {
	RVPMU_FORMAT_ATTR_ENTRY(event, RVPMU_CDELEG_PMU_FORMAT_ATTR),
	&format_attr_firmware.attr,
	NULL,
};

static struct attribute_group riscv_cdeleg_pmu_format_group = {
	.name = "format",
	.attrs = riscv_cdeleg_pmu_formats_attr,
};

static const struct attribute_group *riscv_cdeleg_pmu_attr_groups[] = {
	&riscv_cdeleg_pmu_format_group,
	NULL,
};

/* Allow user mode access by default */
static int sysctl_perf_user_access __read_mostly = SYSCTL_USER_ACCESS;

/*
 * This structure is SBI specific but counter delegation also require counter
 * width, csr mapping. Reuse it for now we can have firmware counters for
 * platfroms with counter delegation support.
 * RISC-V doesn't have heterogeneous harts yet. This need to be part of
 * per_cpu in case of harts with different pmu counters
 */
static union sbi_pmu_ctr_info *pmu_ctr_list;
static bool riscv_pmu_use_irq;
static unsigned int riscv_pmu_irq_num;
static unsigned int riscv_pmu_irq_mask;
static unsigned int riscv_pmu_irq;

/* Cache the available counters in a bitmask */
static unsigned long cmask;
/* Cache the available firmware counters in another bitmask */
static unsigned long firmware_cmask;

static int sbi_pmu_event_find_cache(u64 config);
struct sbi_pmu_event_data {
	union {
		union {
			struct hw_gen_event {
				uint32_t event_code:16;
				uint32_t event_type:4;
				uint32_t reserved:12;
			} hw_gen_event;
			struct hw_cache_event {
				uint32_t result_id:1;
				uint32_t op_id:2;
				uint32_t cache_id:13;
				uint32_t event_type:4;
				uint32_t reserved:12;
			} hw_cache_event;
		};
		uint32_t event_idx;
	};
};

static struct sbi_pmu_event_data pmu_hw_event_sbi_map[] = {
	[PERF_COUNT_HW_CPU_CYCLES]		= {.hw_gen_event = {
							SBI_PMU_HW_CPU_CYCLES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_INSTRUCTIONS]		= {.hw_gen_event = {
							SBI_PMU_HW_INSTRUCTIONS,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_CACHE_REFERENCES]	= {.hw_gen_event = {
							SBI_PMU_HW_CACHE_REFERENCES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_CACHE_MISSES]		= {.hw_gen_event = {
							SBI_PMU_HW_CACHE_MISSES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= {.hw_gen_event = {
							SBI_PMU_HW_BRANCH_INSTRUCTIONS,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_BRANCH_MISSES]		= {.hw_gen_event = {
							SBI_PMU_HW_BRANCH_MISSES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_BUS_CYCLES]		= {.hw_gen_event = {
							SBI_PMU_HW_BUS_CYCLES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND]	= {.hw_gen_event = {
							SBI_PMU_HW_STALLED_CYCLES_FRONTEND,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= {.hw_gen_event = {
							SBI_PMU_HW_STALLED_CYCLES_BACKEND,
							SBI_PMU_EVENT_TYPE_HW, 0}},
	[PERF_COUNT_HW_REF_CPU_CYCLES]		= {.hw_gen_event = {
							SBI_PMU_HW_REF_CPU_CYCLES,
							SBI_PMU_EVENT_TYPE_HW, 0}},
};

#define C(x) PERF_COUNT_HW_CACHE_##x
static struct sbi_pmu_event_data pmu_cache_event_sbi_map[PERF_COUNT_HW_CACHE_MAX]
[PERF_COUNT_HW_CACHE_OP_MAX]
[PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	[C(L1D)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(L1D), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(L1I)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event =	{C(RESULT_ACCESS),
					C(OP_READ), C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS), C(OP_READ),
					C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(L1I), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(LL)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(LL), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(DTLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(DTLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(ITLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(ITLB), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(BPU)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(BPU), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
	[C(NODE)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_READ), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_READ), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_WRITE), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_WRITE), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)] = {.hw_cache_event = {C(RESULT_ACCESS),
					C(OP_PREFETCH), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
			[C(RESULT_MISS)] = {.hw_cache_event = {C(RESULT_MISS),
					C(OP_PREFETCH), C(NODE), SBI_PMU_EVENT_TYPE_CACHE, 0}},
		},
	},
};

static int pmu_sbi_check_event_info(void)
{
	int num_events = ARRAY_SIZE(pmu_hw_event_sbi_map) + PERF_COUNT_HW_CACHE_MAX *
			 PERF_COUNT_HW_CACHE_OP_MAX * PERF_COUNT_HW_CACHE_RESULT_MAX;
	struct riscv_pmu_event_info *event_info_shmem;
	phys_addr_t base_addr;
	int i, j, k, result = 0, count = 0;
	struct sbiret ret;

	event_info_shmem = kcalloc(num_events, sizeof(*event_info_shmem), GFP_KERNEL);
	if (!event_info_shmem)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(pmu_hw_event_sbi_map); i++)
		event_info_shmem[count++].event_idx = pmu_hw_event_sbi_map[i].event_idx;

	for (i = 0; i < ARRAY_SIZE(pmu_cache_event_sbi_map); i++) {
		for (j = 0; j < ARRAY_SIZE(pmu_cache_event_sbi_map[i]); j++) {
			for (k = 0; k < ARRAY_SIZE(pmu_cache_event_sbi_map[i][j]); k++)
				event_info_shmem[count++].event_idx =
							pmu_cache_event_sbi_map[i][j][k].event_idx;
		}
	}

	base_addr = __pa(event_info_shmem);
	if (IS_ENABLED(CONFIG_32BIT))
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_EVENT_GET_INFO, lower_32_bits(base_addr),
				upper_32_bits(base_addr), count, 0, 0, 0);
	else
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_EVENT_GET_INFO, base_addr, 0,
				count, 0, 0, 0);
	if (ret.error) {
		result = -EOPNOTSUPP;
		goto free_mem;
	}

	for (i = 0; i < ARRAY_SIZE(pmu_hw_event_sbi_map); i++) {
		if (!(event_info_shmem[i].output & RISCV_PMU_EVENT_INFO_OUTPUT_MASK))
			pmu_hw_event_sbi_map[i].event_idx = -ENOENT;
	}

	count = ARRAY_SIZE(pmu_hw_event_sbi_map);

	for (i = 0; i < ARRAY_SIZE(pmu_cache_event_sbi_map); i++) {
		for (j = 0; j < ARRAY_SIZE(pmu_cache_event_sbi_map[i]); j++) {
			for (k = 0; k < ARRAY_SIZE(pmu_cache_event_sbi_map[i][j]); k++) {
				if (!(event_info_shmem[count].output &
				      RISCV_PMU_EVENT_INFO_OUTPUT_MASK))
					pmu_cache_event_sbi_map[i][j][k].event_idx = -ENOENT;
				count++;
			}
		}
	}

free_mem:
	kfree(event_info_shmem);

	return result;
}

/*
 * Vendor specific PMU events.
 */
struct riscv_pmu_event {
	u64 event_id;
	u32 counterid_mask;
};

#define HW_OP_UNSUPPORTED		U64_MAX
#define CACHE_OP_UNSUPPORTED		U64_MAX

#define PERF_MAP_ALL_UNSUPPORTED					\
	[0 ... PERF_COUNT_HW_MAX - 1] = {HW_OP_UNSUPPORTED, 0x0}

#define PERF_CACHE_MAP_ALL_UNSUPPORTED					\
[0 ... PERF_COUNT_HW_CACHE_MAX - 1] = {					\
	[0 ... PERF_COUNT_HW_CACHE_OP_MAX - 1] = {			\
		[0 ... PERF_COUNT_HW_CACHE_RESULT_MAX - 1] = {		\
			CACHE_OP_UNSUPPORTED, 0x0			\
		},							\
	},								\
}

struct riscv_vendor_pmu_events {
	unsigned long vendorid;
	unsigned long archid;
	unsigned long implid;
	const struct riscv_pmu_event *hw_event_map;
	const struct riscv_pmu_event (*cache_event_map)[PERF_COUNT_HW_CACHE_OP_MAX]
						       [PERF_COUNT_HW_CACHE_RESULT_MAX];
};

#define RISCV_VENDOR_PMU_EVENTS(_vendorid, _archid, _implid, _hw_event_map, _cache_event_map) \
	{ .vendorid = _vendorid, .archid = _archid, .implid = _implid, \
	  .hw_event_map = _hw_event_map, .cache_event_map = _cache_event_map },

static struct riscv_vendor_pmu_events pmu_vendor_events_table[] = {
};

static const struct riscv_pmu_event *current_pmu_hw_event_map;
static const struct riscv_pmu_event (*current_pmu_cache_event_map)[PERF_COUNT_HW_CACHE_OP_MAX]
							   [PERF_COUNT_HW_CACHE_RESULT_MAX];

static void __init rvpmu_vendor_register_events(void)
{
	int cpu = raw_smp_processor_id();
	unsigned long vendor_id = riscv_cached_mvendorid(cpu);
	unsigned long impl_id = riscv_cached_mimpid(cpu);
	unsigned long arch_id = riscv_cached_marchid(cpu);

	for (int i = 0; i < ARRAY_SIZE(pmu_vendor_events_table); i++) {
		if (pmu_vendor_events_table[i].vendorid == vendor_id &&
		    pmu_vendor_events_table[i].implid == impl_id &&
		    pmu_vendor_events_table[i].archid == arch_id) {
			current_pmu_hw_event_map = pmu_vendor_events_table[i].hw_event_map;
			current_pmu_cache_event_map = pmu_vendor_events_table[i].cache_event_map;
			break;
		}
	}

	if (!current_pmu_hw_event_map && !current_pmu_cache_event_map)
		pr_info("No default PMU events found\n");
}

static void rvpmu_sbi_check_event(struct sbi_pmu_event_data *edata)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_CFG_MATCH,
			0, cmask, 0, edata->event_idx, 0, 0);
	if (!ret.error) {
		sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_STOP,
			  ret.value, 0x1, SBI_PMU_STOP_FLAG_RESET, 0, 0, 0);
	} else if (ret.error == SBI_ERR_NOT_SUPPORTED) {
		/* This event cannot be monitored by any counter */
		edata->event_idx = -ENOENT;
	}
}

static void rvpmu_check_std_events(struct work_struct *work)
{
	int ret;

	if (riscv_pmu_sbi_available()) {
		if (sbi_v3_available) {
			ret = pmu_sbi_check_event_info();
			if (ret)
				pr_err("pmu_sbi_check_event_info failed with error %d\n", ret);
			return;
		}

		for (int i = 0; i < ARRAY_SIZE(pmu_hw_event_sbi_map); i++)
			rvpmu_sbi_check_event(&pmu_hw_event_sbi_map[i]);

		for (int i = 0; i < ARRAY_SIZE(pmu_cache_event_sbi_map); i++)
			for (int j = 0; j < ARRAY_SIZE(pmu_cache_event_sbi_map[i]); j++)
				for (int k = 0; k < ARRAY_SIZE(pmu_cache_event_sbi_map[i][j]); k++)
					rvpmu_sbi_check_event(&pmu_cache_event_sbi_map[i][j][k]);
	} else {
		DO_ONCE_LITE_IF(1, pr_info,
				"Boot time config matching not required for smcdeleg\n");
	}
}

static DECLARE_WORK(check_std_events_work, rvpmu_check_std_events);

static ssize_t rvpmu_format_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = container_of(attr,
				struct dev_ext_attribute, attr);
	return sysfs_emit(buf, "%s\n", (char *)eattr->var);
}

static int rvpmu_ctr_get_width(int idx)
{
	return pmu_ctr_list[idx].width;
}

static bool rvpmu_ctr_is_fw(int cidx)
{
	union sbi_pmu_ctr_info *info;

	info = &pmu_ctr_list[cidx];
	if (!info)
		return false;

	return (info->type == SBI_PMU_CTR_TYPE_FW) ? true : false;
}

int riscv_pmu_get_event_info(u32 type, u64 config, u64 *econfig)
{
	int ret = -ENOENT;

	switch (type) {
	case PERF_TYPE_HARDWARE:
		if (config >= PERF_COUNT_HW_MAX)
			return -EINVAL;
		ret = pmu_hw_event_sbi_map[config].event_idx;
		break;
	case PERF_TYPE_HW_CACHE:
		ret = sbi_pmu_event_find_cache(config);
		break;
	case PERF_TYPE_RAW:
		/*
		 * As per SBI v0.3 specification,
		 *  -- the upper 16 bits must be unused for a hardware raw event.
		 * As per SBI v2.0 specification,
		 *  -- the upper 8 bits must be unused for a hardware raw event.
		 * Bits 63:62 are used to distinguish between raw events
		 * 00 - Hardware raw event
		 * 10 - SBI firmware events
		 * 11 - Risc-V platform specific firmware event
		 */
		switch (config >> 62) {
		case 0:
			if (sbi_v3_available) {
			/* Return error any bits [56-63] is set  as it is not allowed by the spec */
				if (!(config & ~RISCV_PMU_RAW_EVENT_V2_MASK)) {
					if (econfig)
						*econfig = config & RISCV_PMU_RAW_EVENT_V2_MASK;
					ret = RISCV_PMU_RAW_EVENT_V2_IDX;
				}
			/* Return error any bits [48-63] is set  as it is not allowed by the spec */
			} else if (!(config & ~RISCV_PMU_RAW_EVENT_MASK)) {
				if (econfig)
					*econfig = config & RISCV_PMU_RAW_EVENT_MASK;
				ret = RISCV_PMU_RAW_EVENT_IDX;
			}
			break;
		case 2:
			ret = (config & 0xFFFF) | (SBI_PMU_EVENT_TYPE_FW << 16);
			break;
		case 3:
			/*
			 * For Risc-V platform specific firmware events
			 * Event code - 0xFFFF
			 * Event data - raw event encoding
			 */
			ret = SBI_PMU_EVENT_TYPE_FW << 16 | RISCV_PLAT_FW_EVENT;
			if (econfig)
				*econfig = config & RISCV_PMU_PLAT_FW_EVENT_MASK;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return ret;
}
EXPORT_SYMBOL_GPL(riscv_pmu_get_event_info);

/*
 * Returns the counter width of a programmable counter and number of hardware
 * counters. As we don't support heterogeneous CPUs yet, it is okay to just
 * return the counter width of the first programmable counter.
 */
int riscv_pmu_get_hpm_info(u32 *hw_ctr_width, u32 *num_hw_ctr)
{
	int i;
	union sbi_pmu_ctr_info *info;
	u32 hpm_width = 0, hpm_count = 0;

	if (!cmask)
		return -EINVAL;

	for_each_set_bit(i, &cmask, RISCV_MAX_COUNTERS) {
		info = &pmu_ctr_list[i];
		if (!info)
			continue;
		if (!hpm_width && info->csr != CSR_CYCLE && info->csr != CSR_INSTRET)
			hpm_width = info->width;
		if (info->type == SBI_PMU_CTR_TYPE_HW)
			hpm_count++;
	}

	*hw_ctr_width = hpm_width;
	*num_hw_ctr = hpm_count;

	return 0;
}
EXPORT_SYMBOL_GPL(riscv_pmu_get_hpm_info);

static uint8_t rvpmu_csr_index(struct perf_event *event)
{
	return pmu_ctr_list[event->hw.idx].csr - CSR_CYCLE;
}

static uint64_t get_deleg_priv_filter_bits(struct perf_event *event)
{
	u64 priv_filter_bits = 0;
	bool guest_events = false;

	if (event->attr.config1 & RISCV_PMU_CONFIG1_GUEST_EVENTS)
		guest_events = true;
	if (event->attr.exclude_kernel)
		priv_filter_bits |= guest_events ? HPMEVENT_VSINH : HPMEVENT_SINH;
	if (event->attr.exclude_user)
		priv_filter_bits |= guest_events ? HPMEVENT_VUINH : HPMEVENT_UINH;
	if (guest_events && event->attr.exclude_hv)
		priv_filter_bits |= HPMEVENT_SINH;
	if (event->attr.exclude_host)
		priv_filter_bits |= HPMEVENT_UINH | HPMEVENT_SINH;
	if (event->attr.exclude_guest)
		priv_filter_bits |= HPMEVENT_VSINH | HPMEVENT_VUINH;

	return priv_filter_bits;
}

static bool pmu_sbi_is_fw_event(struct perf_event *event)
{
	u32 type = event->attr.type;
	u64 config = event->attr.config;

	if (type == PERF_TYPE_RAW && ((config >> 63) == 1))
		return true;
	else
		return false;
}

/* Hardware counters are driven from S-mode; firmware counters still go via SBI */
static __always_inline bool rvpmu_is_deleg_event(struct perf_event *event)
{
	return riscv_pmu_cdeleg_available() && !pmu_sbi_is_fw_event(event);
}

static unsigned long rvpmu_sbi_get_filter_flags(struct perf_event *event)
{
	unsigned long cflags = 0;
	bool guest_events = false;

	if (event->attr.config1 & RISCV_PMU_CONFIG1_GUEST_EVENTS)
		guest_events = true;
	if (event->attr.exclude_kernel)
		cflags |= guest_events ? SBI_PMU_CFG_FLAG_SET_VSINH : SBI_PMU_CFG_FLAG_SET_SINH;
	if (event->attr.exclude_user)
		cflags |= guest_events ? SBI_PMU_CFG_FLAG_SET_VUINH : SBI_PMU_CFG_FLAG_SET_UINH;
	if (guest_events && event->attr.exclude_hv)
		cflags |= SBI_PMU_CFG_FLAG_SET_SINH;
	if (event->attr.exclude_host)
		cflags |= SBI_PMU_CFG_FLAG_SET_UINH | SBI_PMU_CFG_FLAG_SET_SINH;
	if (event->attr.exclude_guest)
		cflags |= SBI_PMU_CFG_FLAG_SET_VSINH | SBI_PMU_CFG_FLAG_SET_VUINH;

	return cflags;
}

static int rvpmu_sbi_ctr_get_idx(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct riscv_pmu *rvpmu = to_riscv_pmu(event->pmu);
	struct cpu_hw_events *cpuc = this_cpu_ptr(rvpmu->hw_events);
	struct sbiret ret;
	int idx;
	u64 cbase = 0;
	unsigned long ctr_mask = rvpmu->cmask;
	unsigned long cflags = 0;

	cflags = rvpmu_sbi_get_filter_flags(event);

	if (!riscv_pmu_sbi_available())
		return -ENOENT;

	/*
	 * In legacy mode, we have to force the fixed counters for those events
	 * but not in the user access mode as we want to use the other counters
	 * that support sampling/filtering.
	 */
	if ((hwc->flags & PERF_EVENT_FLAG_LEGACY) && (event->attr.type == PERF_TYPE_HARDWARE)) {
		if (event->attr.config == PERF_COUNT_HW_CPU_CYCLES) {
			cflags |= SBI_PMU_CFG_FLAG_SKIP_MATCH;
			ctr_mask = 1;
		} else if (event->attr.config == PERF_COUNT_HW_INSTRUCTIONS) {
			cflags |= SBI_PMU_CFG_FLAG_SKIP_MATCH;
			ctr_mask = BIT(CSR_INSTRET - CSR_CYCLE);
		}
	} else if (pmu_sbi_is_fw_event(event)) {
		ctr_mask = firmware_cmask;
	}

	/* retrieve the available counter index */
#if defined(CONFIG_32BIT)
	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_CFG_MATCH, cbase,
			ctr_mask, cflags, hwc->event_base, hwc->config,
			hwc->config >> 32);
#else
	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_CFG_MATCH, cbase,
			ctr_mask, cflags, hwc->event_base, hwc->config, 0);
#endif
	if (ret.error) {
		pr_debug("Not able to find a counter for event %lx config %llx\n",
			hwc->event_base, hwc->config);
		return sbi_err_map_linux_errno(ret.error);
	}

	idx = ret.value;
	if (!test_bit(idx, &ctr_mask) || !pmu_ctr_list[idx].value)
		return -ENOENT;

	/* Additional sanity check for the counter id */
	if (rvpmu_ctr_is_fw(idx)) {
		if (!test_and_set_bit(idx, cpuc->used_fw_ctrs))
			return idx;
	} else {
		if (!test_and_set_bit(idx, cpuc->used_hw_ctrs))
			return idx;
	}

	return -ENOENT;
}

static void rvpmu_ctr_clear_idx(struct perf_event *event)
{

	struct hw_perf_event *hwc = &event->hw;
	struct riscv_pmu *rvpmu = to_riscv_pmu(event->pmu);
	struct cpu_hw_events *cpuc = this_cpu_ptr(rvpmu->hw_events);
	int idx = hwc->idx;

	if (rvpmu_ctr_is_fw(idx))
		clear_bit(idx, cpuc->used_fw_ctrs);
	else
		clear_bit(idx, cpuc->used_hw_ctrs);
}

static int sbi_pmu_event_find_cache(u64 config)
{
	unsigned int cache_type, cache_op, cache_result, ret;

	cache_type = (config >>  0) & 0xff;
	if (cache_type >= PERF_COUNT_HW_CACHE_MAX)
		return -EINVAL;

	cache_op = (config >>  8) & 0xff;
	if (cache_op >= PERF_COUNT_HW_CACHE_OP_MAX)
		return -EINVAL;

	cache_result = (config >> 16) & 0xff;
	if (cache_result >= PERF_COUNT_HW_CACHE_RESULT_MAX)
		return -EINVAL;

	ret = pmu_cache_event_sbi_map[cache_type][cache_op][cache_result].event_idx;

	return ret;
}

static int rvpmu_sbi_event_map(struct perf_event *event, u64 *econfig)
{
	u32 type = event->attr.type;
	u64 config = event->attr.config;

	/*
	 * Ensure we are finished checking standard hardware events for
	 * validity before allowing userspace to configure any events.
	 */
	flush_work(&check_std_events_work);

	return riscv_pmu_get_event_info(type, config, econfig);
}

static int cdeleg_pmu_event_find_cache(u64 config, u64 *eventid, uint32_t *counterid_mask)
{
	unsigned int cache_type, cache_op, cache_result;

	if (!current_pmu_cache_event_map)
		return -ENOENT;

	cache_type = (config >>  0) & 0xff;
	if (cache_type >= PERF_COUNT_HW_CACHE_MAX)
		return -EINVAL;

	cache_op = (config >>  8) & 0xff;
	if (cache_op >= PERF_COUNT_HW_CACHE_OP_MAX)
		return -EINVAL;

	cache_result = (config >> 16) & 0xff;
	if (cache_result >= PERF_COUNT_HW_CACHE_RESULT_MAX)
		return -EINVAL;

	if (eventid)
		*eventid = current_pmu_cache_event_map[cache_type][cache_op]
						      [cache_result].event_id;
	if (counterid_mask)
		*counterid_mask = current_pmu_cache_event_map[cache_type][cache_op]
							   [cache_result].counterid_mask;

	return 0;
}

static int rvpmu_cdeleg_event_map(struct perf_event *event, u64 *econfig)
{
	u32 type = event->attr.type;
	u64 config = event->attr.config;
	int ret = 0;

	/*
	 * There are two ways standard perf events can be mapped to platform specific
	 * encoding.
	 * 1. The vendor may specify the encodings in the driver.
	 * 2. The Perf tool for RISC-V may remap the standard perf event to platform
	 * specific encoding.
	 *
	 * As RISC-V ISA doesn't define any standard event encoding. Thus, perf tool allows
	 * vendor to define it via json file. The encoding defined in the json will override
	 * the perf legacy encoding. However, some user may want to run performance
	 * monitoring without perf tool as well. That's why, vendors may specify the event
	 * encoding in the driver as well if they want to support that use case too.
	 * If an encoding is defined in the json, it will be encoded as a raw event.
	 */

	switch (type) {
	case PERF_TYPE_HARDWARE:
		if (config >= PERF_COUNT_HW_MAX)
			return -EINVAL;
		if (!current_pmu_hw_event_map)
			return -ENOENT;

		*econfig = current_pmu_hw_event_map[config].event_id;
		if (*econfig == HW_OP_UNSUPPORTED)
			ret = -ENOENT;
		break;
	case PERF_TYPE_HW_CACHE:
		ret = cdeleg_pmu_event_find_cache(config, econfig, NULL);
		if (ret)
			break;
		if (*econfig == CACHE_OP_UNSUPPORTED)
			ret = -ENOENT;
		break;
	case PERF_TYPE_RAW:
		/*
		 * counterid_mask is config2:0-31; higher bits would silently
		 * truncate to 0 and read as "no constraint".
		 */
		if (event->attr.config2 & ~GENMASK_ULL(31, 0))
			return -EINVAL;
		*econfig = config & RISCV_PMU_DELEG_RAW_EVENT_MASK;
		break;
	default:
		ret = -ENOENT;
		break;
	}

	/* event_base is not used for counter delegation */
	return ret;
}

static void pmu_sbi_snapshot_free(struct riscv_pmu *pmu)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct cpu_hw_events *cpu_hw_evt = per_cpu_ptr(pmu->hw_events, cpu);

		if (!cpu_hw_evt->snapshot_addr)
			continue;

		free_page((unsigned long)cpu_hw_evt->snapshot_addr);
		cpu_hw_evt->snapshot_addr = NULL;
		cpu_hw_evt->snapshot_addr_phys = 0;
	}
}

static int pmu_sbi_snapshot_alloc(struct riscv_pmu *pmu)
{
	int cpu;
	struct page *snapshot_page;

	for_each_possible_cpu(cpu) {
		struct cpu_hw_events *cpu_hw_evt = per_cpu_ptr(pmu->hw_events, cpu);

		snapshot_page = alloc_page(GFP_ATOMIC | __GFP_ZERO);
		if (!snapshot_page) {
			pmu_sbi_snapshot_free(pmu);
			return -ENOMEM;
		}
		cpu_hw_evt->snapshot_addr = page_to_virt(snapshot_page);
		cpu_hw_evt->snapshot_addr_phys = page_to_phys(snapshot_page);
	}

	return 0;
}

static int pmu_sbi_snapshot_disable(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_SNAPSHOT_SET_SHMEM, SBI_SHMEM_DISABLE,
			SBI_SHMEM_DISABLE, 0, 0, 0, 0);
	if (ret.error) {
		pr_warn("failed to disable snapshot shared memory\n");
		return sbi_err_map_linux_errno(ret.error);
	}

	return 0;
}

static int pmu_sbi_snapshot_setup(struct riscv_pmu *pmu, int cpu)
{
	struct cpu_hw_events *cpu_hw_evt;
	struct sbiret ret = {0};

	cpu_hw_evt = per_cpu_ptr(pmu->hw_events, cpu);
	if (!cpu_hw_evt->snapshot_addr_phys)
		return -EINVAL;

	if (cpu_hw_evt->snapshot_set_done)
		return 0;

	if (IS_ENABLED(CONFIG_32BIT))
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_SNAPSHOT_SET_SHMEM,
				cpu_hw_evt->snapshot_addr_phys,
				(u64)(cpu_hw_evt->snapshot_addr_phys) >> 32, 0, 0, 0, 0);
	else
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_SNAPSHOT_SET_SHMEM,
				cpu_hw_evt->snapshot_addr_phys, 0, 0, 0, 0, 0);

	/* Free up the snapshot area memory and fall back to SBI PMU calls without snapshot */
	if (ret.error) {
		if (ret.error != SBI_ERR_NOT_SUPPORTED)
			pr_warn("pmu snapshot setup failed with error %ld\n", ret.error);
		return sbi_err_map_linux_errno(ret.error);
	}

	memset(cpu_hw_evt->snapshot_cval_shcopy, 0, sizeof(u64) * RISCV_MAX_COUNTERS);
	cpu_hw_evt->snapshot_set_done = true;

	return 0;
}

static u64 rvpmu_ctr_read(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;
	struct sbiret ret;
	u64 val = 0;
	struct riscv_pmu *pmu = to_riscv_pmu(event->pmu);
	struct cpu_hw_events *cpu_hw_evt = this_cpu_ptr(pmu->hw_events);
	struct riscv_pmu_snapshot_data *sdata = cpu_hw_evt->snapshot_addr;
	union sbi_pmu_ctr_info info = pmu_ctr_list[idx];

	/*
	 * Read from shared memory only if the counter is stopped. Delegated
	 * counters stop via scountinhibit, so their snapshot copy is stale.
	 */
	if (sbi_pmu_snapshot_available() && (hwc->state & PERF_HES_STOPPED) &&
	    !rvpmu_is_deleg_event(event)) {
		val = sdata->ctr_values[idx];
		return val;
	}

	if (pmu_sbi_is_fw_event(event) && riscv_pmu_sbi_available()) {
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_FW_READ,
				hwc->idx, 0, 0, 0, 0, 0);
		if (ret.error)
			return 0;

		val = ret.value;
		if (IS_ENABLED(CONFIG_32BIT) && sbi_v2_available && info.width >= 32) {
			ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_FW_READ_HI,
					hwc->idx, 0, 0, 0, 0, 0);
			if (!ret.error)
				val |= ((u64)ret.value << 32);
			else
				WARN_ONCE(1, "Unable to read upper 32 bits of firmware counter error: %ld\n",
					  ret.error);
		}
	} else {
		val = riscv_pmu_ctr_read_csr(info.csr);
		if (IS_ENABLED(CONFIG_32BIT))
			val |= ((u64)riscv_pmu_ctr_read_csr(info.csr + 0x80)) << 32;
	}

	return val;
}

static void rvpmu_set_scounteren(void *arg)
{
	struct perf_event *event = (struct perf_event *)arg;

	if (event->hw.idx != -1)
		csr_write(CSR_SCOUNTEREN,
			  csr_read(CSR_SCOUNTEREN) | BIT(rvpmu_csr_index(event)));
}

static void rvpmu_reset_scounteren(void *arg)
{
	struct perf_event *event = (struct perf_event *)arg;

	if (event->hw.idx != -1)
		csr_write(CSR_SCOUNTEREN,
			  csr_read(CSR_SCOUNTEREN) & ~BIT(rvpmu_csr_index(event)));
}

static void rvpmu_sbi_ctr_start(struct perf_event *event, u64 ival)
{
	struct sbiret ret;
	struct hw_perf_event *hwc = &event->hw;
	unsigned long flag = SBI_PMU_START_FLAG_SET_INIT_VALUE;

	/* There is no benefit setting SNAPSHOT FLAG for a single counter */
#if defined(CONFIG_32BIT)
	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_START, hwc->idx,
			1, flag, ival, ival >> 32, 0);
#else
	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_START, hwc->idx,
			1, flag, ival, 0, 0);
#endif
	if (ret.error && (ret.error != SBI_ERR_ALREADY_STARTED))
		pr_err("Starting counter idx %d failed with error %d\n",
			hwc->idx, sbi_err_map_linux_errno(ret.error));
}

static void rvpmu_sbi_ctr_stop(struct perf_event *event, unsigned long flag)
{
	struct sbiret ret;
	struct hw_perf_event *hwc = &event->hw;
	struct riscv_pmu *pmu = to_riscv_pmu(event->pmu);
	struct cpu_hw_events *cpu_hw_evt = this_cpu_ptr(pmu->hw_events);
	struct riscv_pmu_snapshot_data *sdata = cpu_hw_evt->snapshot_addr;

	if (sbi_pmu_snapshot_available())
		flag |= SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT;

	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_STOP, hwc->idx, 1, flag, 0, 0, 0);
	if (!ret.error && sbi_pmu_snapshot_available()) {
		/*
		 * The counter snapshot is based on the index base specified by hwc->idx.
		 * The actual counter value is updated in shared memory at index 0 when counter
		 * mask is 0x01. To ensure accurate counter values, it's necessary to transfer
		 * the counter value to shared memory. However, if hwc->idx is zero, the counter
		 * value is already correctly updated in shared memory, requiring no further
		 * adjustment.
		 */
		if (hwc->idx > 0) {
			sdata->ctr_values[hwc->idx] = sdata->ctr_values[0];
			sdata->ctr_values[0] = 0;
		}
	} else if (ret.error && (ret.error != SBI_ERR_ALREADY_STOPPED) &&
		flag != SBI_PMU_STOP_FLAG_RESET) {
		pr_err("Stopping counter idx %d failed with error %d\n",
			hwc->idx, sbi_err_map_linux_errno(ret.error));
	}
}

static int rvpmu_sbi_find_num_ctrs(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_NUM_COUNTERS, 0, 0, 0, 0, 0, 0);
	if (!ret.error)
		return ret.value;
	else
		return sbi_err_map_linux_errno(ret.error);
}

static int rvpmu_sbi_get_ctrinfo(u32 nsbi_ctr, u32 *num_fw_ctr, u32 *num_hw_ctr)
{
	struct sbiret ret;
	int i;
	union sbi_pmu_ctr_info cinfo;

	for (i = 0; i < nsbi_ctr; i++) {
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_GET_INFO, i, 0, 0, 0, 0, 0);
		if (ret.error)
			/* The logical counter ids are not expected to be contiguous */
			continue;

		cinfo.value = ret.value;
		if (cinfo.type == SBI_PMU_CTR_TYPE_FW) {
			/* FW counters use SBI logical ids; skip any that collide */
			if (cmask & BIT(i)) {
				pr_warn_once("SBI firmware counter %d overlaps a delegated hardware counter, ignoring it\n",
					     i);
				continue;
			}
			/* Track firmware counters in a different mask */
			firmware_cmask |= BIT(i);
			pmu_ctr_list[i].value = cinfo.value;
			*num_fw_ctr = *num_fw_ctr + 1;
		} else if (cinfo.type == SBI_PMU_CTR_TYPE_HW &&
			   !riscv_pmu_cdeleg_available_boot()) {
			*num_hw_ctr = *num_hw_ctr + 1;
			cmask |= BIT(i);
			pmu_ctr_list[i].value = cinfo.value;
		}
	}

	return 0;
}

static inline void rvpmu_sbi_stop_all(struct riscv_pmu *pmu)
{
	/*
	 * No need to check the error because we are disabling all the counters
	 * which may include counters that are not enabled yet.
	 */
	sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_STOP,
		  0, pmu->cmask | firmware_cmask, SBI_PMU_STOP_FLAG_RESET, 0, 0, 0);
}

static inline void rvpmu_sbi_stop_hw_ctrs(struct riscv_pmu *pmu)
{
	struct cpu_hw_events *cpu_hw_evt = this_cpu_ptr(pmu->hw_events);
	struct riscv_pmu_snapshot_data *sdata = cpu_hw_evt->snapshot_addr;
	unsigned long flag = 0;
	int i, idx;
	struct sbiret ret;
	u64 temp_ctr_overflow_mask = 0;

	if (sbi_pmu_snapshot_available())
		flag = SBI_PMU_STOP_FLAG_TAKE_SNAPSHOT;

	/* Reset the shadow copy to avoid save/restore any value from previous overflow */
	memset(cpu_hw_evt->snapshot_cval_shcopy, 0, sizeof(u64) * RISCV_MAX_COUNTERS);

	for (i = 0; i < BITS_TO_LONGS(RISCV_MAX_COUNTERS); i++) {
		/* No need to check the error here as we can't do anything about the error */
		ret = sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_STOP, i * BITS_PER_LONG,
				cpu_hw_evt->used_hw_ctrs[i], flag, 0, 0, 0);
		if (!ret.error && sbi_pmu_snapshot_available()) {
			/* Save the counter values to avoid clobbering */
			for_each_set_bit(idx, &cpu_hw_evt->used_hw_ctrs[i], BITS_PER_LONG)
				cpu_hw_evt->snapshot_cval_shcopy[i * BITS_PER_LONG + idx] =
							sdata->ctr_values[idx];
			/* Save the overflow mask to avoid clobbering */
			temp_ctr_overflow_mask |= sdata->ctr_overflow_mask << (i * BITS_PER_LONG);
		}
	}

	/* Restore the counter values to the shared memory for used hw counters */
	if (sbi_pmu_snapshot_available()) {
		for_each_set_bit(idx, cpu_hw_evt->used_hw_ctrs, RISCV_MAX_COUNTERS)
			sdata->ctr_values[idx] = cpu_hw_evt->snapshot_cval_shcopy[idx];
		if (temp_ctr_overflow_mask)
			sdata->ctr_overflow_mask = temp_ctr_overflow_mask;
	}
}

static void rvpmu_deleg_ctr_start_mask(unsigned long mask)
{
	unsigned long scountinhibit_val = 0;

	scountinhibit_val = csr_read(CSR_SCOUNTINHIBIT);
	scountinhibit_val &= ~mask;

	csr_write(CSR_SCOUNTINHIBIT, scountinhibit_val);
}

static void rvpmu_deleg_ctr_enable_irq(struct perf_event *event)
{
	unsigned long hpmevent_curr;
	unsigned long of_mask;
	struct hw_perf_event *hwc = &event->hw;
	int counter_idx = hwc->idx;
	unsigned long sip_val = csr_read(CSR_SIP);

#if defined(CONFIG_32BIT)
	hpmevent_curr = csr_indirect_read(CSR_SIREG5, SISELECT_SSCCFG_BASE, counter_idx);
	of_mask = (u32)~HPMEVENTH_OF;
#else
	hpmevent_curr = csr_indirect_read(CSR_SIREG2, SISELECT_SSCCFG_BASE, counter_idx);
	of_mask = ~HPMEVENT_OF;
#endif
	hpmevent_curr &= of_mask;
#if defined(CONFIG_32BIT)
	csr_indirect_write(CSR_SIREG5, SISELECT_SSCCFG_BASE, counter_idx, hpmevent_curr);
#else
	csr_indirect_write(CSR_SIREG2, SISELECT_SSCCFG_BASE, counter_idx, hpmevent_curr);
#endif
}

static void rvpmu_deleg_ctr_start(struct perf_event *event, u64 ival)
{
	unsigned long scountinhibit_val = 0;
	struct hw_perf_event *hwc = &event->hw;

#if defined(CONFIG_32BIT)
	csr_indirect_write(CSR_SIREG, SISELECT_SSCCFG_BASE, hwc->idx, ival & 0xFFFFFFFF);
	csr_indirect_write(CSR_SIREG4, SISELECT_SSCCFG_BASE, hwc->idx, ival >> BITS_PER_LONG);
#else
	csr_indirect_write(CSR_SIREG, SISELECT_SSCCFG_BASE, hwc->idx, ival);
#endif

	rvpmu_deleg_ctr_enable_irq(event);

	scountinhibit_val = csr_read(CSR_SCOUNTINHIBIT);
	scountinhibit_val &= ~BIT(hwc->idx);

	csr_write(CSR_SCOUNTINHIBIT, scountinhibit_val);
}

static void rvpmu_deleg_ctr_stop_mask(unsigned long mask)
{
	unsigned long scountinhibit_val = 0;

	scountinhibit_val = csr_read(CSR_SCOUNTINHIBIT);
	scountinhibit_val |= mask;

	csr_write(CSR_SCOUNTINHIBIT, scountinhibit_val);
}

static void rvpmu_sbi_start_ovf_ctrs_snapshot(struct cpu_hw_events *cpu_hw_evt,
					      u64 ctr_ovf_mask)
{
	int i, idx = 0;
	struct perf_event *event;
	unsigned long flag = SBI_PMU_START_FLAG_INIT_SNAPSHOT;
	u64 max_period, init_val = 0;
	struct hw_perf_event *hwc;
	struct riscv_pmu_snapshot_data *sdata = cpu_hw_evt->snapshot_addr;

	for_each_set_bit(idx, cpu_hw_evt->used_hw_ctrs, RISCV_MAX_COUNTERS) {
		if (ctr_ovf_mask & BIT(idx)) {
			event = cpu_hw_evt->events[idx];
			hwc = &event->hw;
			max_period = riscv_pmu_ctr_get_width_mask(event);
			init_val = local64_read(&hwc->prev_count) & max_period;
			cpu_hw_evt->snapshot_cval_shcopy[idx] = init_val;
		}
		/*
		 * We do not need to update the non-overflow counters the previous
		 * value should have been there already.
		 */
	}

	for (i = 0; i < BITS_TO_LONGS(RISCV_MAX_COUNTERS); i++) {
		/* Restore the counter values to relative indices for used hw counters */
		for_each_set_bit(idx, &cpu_hw_evt->used_hw_ctrs[i], BITS_PER_LONG)
			sdata->ctr_values[idx] =
					cpu_hw_evt->snapshot_cval_shcopy[idx + i * BITS_PER_LONG];
		/* Start all the counters in a single shot */
		sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_START, idx * BITS_PER_LONG,
			  cpu_hw_evt->used_hw_ctrs[i], flag, 0, 0, 0);
	}
}

/*
 * This function starts all the used counters in two step approach.
 * Any counter that did not overflow can be start in a single step
 * while the overflowed counters need to be started with updated initialization
 * value.
 */
static void rvpmu_start_overflow_mask(struct riscv_pmu *pmu, u64 ctr_ovf_mask)
{
	int idx = 0, i;
	struct perf_event *event;
	unsigned long ctr_start_mask = 0;
	u64 max_period, init_val = 0;
	struct hw_perf_event *hwc;
	struct cpu_hw_events *cpu_hw_evt = this_cpu_ptr(pmu->hw_events);

	/* Delegated counters must not be re-armed from stale snapshot memory */
	if (sbi_pmu_snapshot_available() && !riscv_pmu_cdeleg_available())
		return rvpmu_sbi_start_ovf_ctrs_snapshot(cpu_hw_evt, ctr_ovf_mask);

	/* Start all the counters that did not overflow */
	if (riscv_pmu_cdeleg_available()) {
		ctr_start_mask = cpu_hw_evt->used_hw_ctrs[0] & ~ctr_ovf_mask;
		rvpmu_deleg_ctr_start_mask(ctr_start_mask);
	} else {
		for (i = 0; i < BITS_TO_LONGS(RISCV_MAX_COUNTERS); i++) {
			ctr_start_mask = cpu_hw_evt->used_hw_ctrs[i] & ~ctr_ovf_mask;
			/* Start all the counters that did not overflow in a single shot */
			sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_START, i * BITS_PER_LONG,
				  ctr_start_mask, 0, 0, 0, 0);
		}
	}

	/* Reinitialize and start all the counter that overflowed */
	while (ctr_ovf_mask) {
		if (ctr_ovf_mask & 0x01) {
			event = cpu_hw_evt->events[idx];
			hwc = &event->hw;
			max_period = riscv_pmu_ctr_get_width_mask(event);
			init_val = local64_read(&hwc->prev_count) & max_period;
			if (riscv_pmu_cdeleg_available())
				rvpmu_deleg_ctr_start(event, init_val);
			else
				rvpmu_sbi_ctr_start(event, init_val);
			perf_event_update_userpage(event);
		}
		ctr_ovf_mask = ctr_ovf_mask >> 1;
		idx++;
	}
}


static irqreturn_t rvpmu_ovf_handler(struct cpu_hw_events *cpu_hw_evt,
				       struct pt_regs *regs, bool from_sse)
{
	struct perf_sample_data data;
	struct hw_perf_event *hw_evt;
	union sbi_pmu_ctr_info *info;
	int lidx, hidx, fidx;
	struct riscv_pmu *pmu;
	struct perf_event *event;
	u64 overflow;
	u64 overflowed_ctrs = 0;
	u64 start_clock = sched_clock();
	struct riscv_pmu_snapshot_data *sdata = cpu_hw_evt->snapshot_addr;

	if (WARN_ON_ONCE(!cpu_hw_evt))
		return IRQ_NONE;

	/* Firmware counter don't support overflow yet */
	fidx = find_first_bit(cpu_hw_evt->used_hw_ctrs, RISCV_MAX_COUNTERS);
	if (fidx == RISCV_MAX_COUNTERS) {
		if (!from_sse)
			csr_clear(CSR_SIP, BIT(riscv_pmu_irq_num));
		return IRQ_NONE;
	}

	event = cpu_hw_evt->events[fidx];
	if (!event) {
		if (!from_sse)
			ALT_SBI_PMU_OVF_CLEAR_PENDING(riscv_pmu_irq_mask);
		return IRQ_NONE;
	}

	pmu = to_riscv_pmu(event->pmu);
	if (riscv_pmu_cdeleg_available())
		rvpmu_deleg_ctr_stop_mask(cpu_hw_evt->used_hw_ctrs[0]);
	else
		rvpmu_sbi_stop_hw_ctrs(pmu);

	/*
	 * Overflow status register should only be read after counter are stopped.
	 * In counter delegation mode the overflows are reported in scountovf, not
	 * in the SBI snapshot area, so read the CSR directly even when an SBI PMU
	 * snapshot is also available.
	 */
	if (sbi_pmu_snapshot_available() && !riscv_pmu_cdeleg_available())
		overflow = sdata->ctr_overflow_mask;
	else
		ALT_SBI_PMU_OVERFLOW(overflow);

	/*
	 * Overflow interrupt pending bit should only be cleared after stopping
	 * all the counters to avoid any race condition. When using SSE,
	 * interrupt is cleared when stopping counters.
	 */
	if (!from_sse)
		ALT_SBI_PMU_OVF_CLEAR_PENDING(riscv_pmu_irq_mask);

	/* No overflow bit is set */
	if (!overflow)
		return IRQ_NONE;

	for_each_set_bit(lidx, cpu_hw_evt->used_hw_ctrs, RISCV_MAX_COUNTERS) {
		struct perf_event *event = cpu_hw_evt->events[lidx];

		/* Skip if invalid event or user did not request a sampling */
		if (!event || !is_sampling_event(event))
			continue;

		info = &pmu_ctr_list[lidx];
		/* Do a sanity check */
		if (!info || info->type != SBI_PMU_CTR_TYPE_HW)
			continue;

		if (sbi_pmu_snapshot_available())
			/* SBI implementation already updated the logical indicies */
			hidx = lidx;
		else
			/* compute hardware counter index */
			hidx = info->csr - CSR_CYCLE;

		/* check if the corresponding bit is set in sscountovf or overflow mask in shmem */
		if (!(overflow & BIT(hidx)))
			continue;

		/*
		 * Keep a track of overflowed counters so that they can be started
		 * with updated initial value.
		 */
		overflowed_ctrs |= BIT(lidx);
		hw_evt = &event->hw;
		/* Update the event states here so that we know the state while reading */
		hw_evt->state |= PERF_HES_STOPPED;
		riscv_pmu_event_update(event);
		hw_evt->state |= PERF_HES_UPTODATE;
		perf_sample_data_init(&data, 0, hw_evt->last_period);
		if (riscv_pmu_event_set_period(event)) {
			/*
			 * Unlike other ISAs, RISC-V don't have to disable interrupts
			 * to avoid throttling here. As per the specification, the
			 * interrupt remains disabled until the OF bit is set.
			 * Interrupts are enabled again only during the start.
			 * TODO: We will need to stop the guest counters once
			 * virtualization support is added.
			 */
			perf_event_overflow(event, &data, regs);
		}
		/* Reset the state as we are going to start the counter after the loop */
		hw_evt->state = 0;
	}

	rvpmu_start_overflow_mask(pmu, overflowed_ctrs);
	perf_sample_event_took(sched_clock() - start_clock);

	return IRQ_HANDLED;
}

static int get_deleg_hw_ctr_width(int counter_offset)
{
	unsigned long hpm_warl;
	int num_bits;

	if (counter_offset < 3 || counter_offset > 31)
		return 0;

	hpm_warl = csr_indirect_warl(CSR_SIREG, SISELECT_SSCCFG_BASE, counter_offset, -1);
	if (!hpm_warl)
		return 0;
	num_bits = __fls(hpm_warl);

#if defined(CONFIG_32BIT)
	/*
	 * The low half contributes a full BITS_PER_LONG bits when the counter is
	 * wider than 32 bits; the high half's __fls() gives the remaining width.
	 */
	hpm_warl = csr_indirect_warl(CSR_SIREG4, SISELECT_SSCCFG_BASE, counter_offset, -1);
	if (hpm_warl)
		num_bits = BITS_PER_LONG + __fls(hpm_warl);
#endif
	return num_bits;
}

static int rvpmu_deleg_find_ctrs(void)
{
	int i, num_hw_ctr = 0;
	union sbi_pmu_ctr_info cinfo;
	unsigned long scountinhibit_old = 0;

	/* Do a WARL write/read to detect which hpmcounters have been delegated */
	scountinhibit_old = csr_read(CSR_SCOUNTINHIBIT);
	csr_write(CSR_SCOUNTINHIBIT, -1);
	cmask = csr_read(CSR_SCOUNTINHIBIT);

	csr_write(CSR_SCOUNTINHIBIT, scountinhibit_old);

	for_each_set_bit(i, &cmask, RISCV_MAX_HW_COUNTERS) {
		if (unlikely(i == 1))
			continue; /* This should never happen as TM is read only */
		cinfo.value = 0;
		cinfo.type = SBI_PMU_CTR_TYPE_HW;
		/*
		 * If counter delegation is enabled, the csr stored to the cinfo will
		 * be a virtual counter that the delegation attempts to read.
		 */
		cinfo.csr = CSR_CYCLE + i;
		if (i == 0 || i == 2)
			cinfo.width = 63;
		else
			cinfo.width = get_deleg_hw_ctr_width(i);

		num_hw_ctr++;
		pmu_ctr_list[i].value = cinfo.value;
	}

	return num_hw_ctr;
}

static int get_deleg_fixed_hw_idx(struct cpu_hw_events *cpuc, struct perf_event *event)
{
	return -EINVAL;
}

static int get_deleg_next_hpm_hw_idx(struct cpu_hw_events *cpuc, struct perf_event *event)
{
	unsigned long hw_ctr_mask = 0;

	/*
	 * TODO: Treat every hpmcounter can monitor every event for now.
	 * The event to counter mapping should come from the json file.
	 * The mapping should also tell if sampling is supported or not.
	 */

	/* Select only hpmcounters */
	hw_ctr_mask = cmask & (~0x7);
	hw_ctr_mask &= ~(cpuc->used_hw_ctrs[0]);
	return __ffs(hw_ctr_mask);
}

static void write_deleg_hpmevent(int counter_idx, uint64_t event_value, uint64_t filter_bits)
{
	u64 hpmevent_value = 0;

	/* OF bit should be enable during the start if sampling is requested */
	hpmevent_value = (event_value & ~HPMEVENT_MASK) | filter_bits | HPMEVENT_OF;
#if defined(CONFIG_32BIT)
	csr_indirect_write(CSR_SIREG2, SISELECT_SSCCFG_BASE, counter_idx,
			   hpmevent_value & 0xFFFFFFFF);
	if (riscv_isa_extension_available(NULL, SSCOFPMF))
		csr_indirect_write(CSR_SIREG5, SISELECT_SSCCFG_BASE, counter_idx,
				   hpmevent_value >> BITS_PER_LONG);
#else
	csr_indirect_write(CSR_SIREG2, SISELECT_SSCCFG_BASE, counter_idx, hpmevent_value);
#endif
}

/* cyclecfg/instretcfg only exist with Smcntrpmf; writing them otherwise traps */
static void clear_deleg_hpmevent(int counter_idx)
{
	if ((counter_idx == 0 || counter_idx == 2) &&
	    !riscv_isa_extension_available(NULL, SMCNTRPMF))
		return;

	write_deleg_hpmevent(counter_idx, 0, 0);
}

static int rvpmu_deleg_ctr_get_idx(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	struct riscv_pmu *rvpmu = to_riscv_pmu(event->pmu);
	struct cpu_hw_events *cpuc = this_cpu_ptr(rvpmu->hw_events);
	unsigned long hw_ctr_max_id;
	u64 priv_filter;
	int idx;

	/*
	 * TODO: We should not rely on SBI Perf encoding to check if the event
	 * is a fixed one or not.
	 */
	if (!is_sampling_event(event)) {
		idx = get_deleg_fixed_hw_idx(cpuc, event);
		if (idx == 0 || idx == 2) {
			/* Priv mode filter bits are only available if smcntrpmf is present */
			if (riscv_isa_extension_available(NULL, SMCNTRPMF))
				goto found_idx;
			else
				goto skip_update;
		}
	}

	if (!cmask)
		goto out_err;
	hw_ctr_max_id = __fls(cmask);
	idx = get_deleg_next_hpm_hw_idx(cpuc, event);
	if (idx < 3 || idx > hw_ctr_max_id)
		goto out_err;
found_idx:
	priv_filter = get_deleg_priv_filter_bits(event);
	write_deleg_hpmevent(idx, hwc->config, priv_filter);
skip_update:
	if (!test_and_set_bit(idx, cpuc->used_hw_ctrs))
		return idx;
out_err:
	return -ENOENT;
}


static irqreturn_t pmu_sbi_ovf_irq_handler(int irq, void *dev)
{
	return pmu_sbi_ovf_handler(dev, get_irq_regs(), false);
}

#ifdef CONFIG_RISCV_PMU_SSE
static int pmu_sbi_ovf_sse_handler(u32 evt, void *arg, struct pt_regs *regs)
{
	struct cpu_hw_events __percpu *hw_events = arg;
	struct cpu_hw_events *hw_event = raw_cpu_ptr(hw_events);

	pmu_sbi_ovf_handler(hw_event, regs, true);

	return 0;
}

static int pmu_sbi_setup_sse(struct riscv_pmu *pmu)
{
	int ret;
	struct sse_event *evt;
	struct cpu_hw_events __percpu *hw_events = pmu->hw_events;

	evt = sse_event_register(SBI_SSE_EVENT_LOCAL_PMU_OVERFLOW, 0,
				 pmu_sbi_ovf_sse_handler, hw_events);
	if (IS_ERR(evt))
		return PTR_ERR(evt);

	ret = sse_event_enable(evt);
	if (ret) {
		sse_event_unregister(evt);
		return ret;
	}

	pr_info("using SSE for PMU event delivery\n");
	pmu->sse_evt = evt;

	return ret;
}
#else
static int pmu_sbi_setup_sse(struct riscv_pmu *pmu)
{
	return -EOPNOTSUPP;
}
#endif

static void rvpmu_ctr_start(struct perf_event *event, u64 ival)
{
	struct hw_perf_event *hwc = &event->hw;

	if (rvpmu_is_deleg_event(event))
		rvpmu_deleg_ctr_start(event, ival);
	else
		rvpmu_sbi_ctr_start(event, ival);

	if ((hwc->flags & PERF_EVENT_FLAG_USER_ACCESS) &&
	    (hwc->flags & PERF_EVENT_FLAG_USER_READ_CNT))
		rvpmu_set_scounteren((void *)event);
}

static void rvpmu_ctr_stop(struct perf_event *event, unsigned long flag)
{
	struct hw_perf_event *hwc = &event->hw;

	if ((hwc->flags & PERF_EVENT_FLAG_USER_ACCESS) &&
	    (hwc->flags & PERF_EVENT_FLAG_USER_READ_CNT))
		rvpmu_reset_scounteren((void *)event);

	if (rvpmu_is_deleg_event(event)) {
		/*
		 * The counter is already stopped. No need to stop again. Counter
		 * mapping will be reset in clear_idx function.
		 */
		if (flag != RISCV_PMU_STOP_FLAG_RESET)
			rvpmu_deleg_ctr_stop_mask(BIT(hwc->idx));
		else
			clear_deleg_hpmevent(hwc->idx);
	} else {
		rvpmu_sbi_ctr_stop(event, flag);
	}
}

static int rvpmu_find_ctrs(void)
{
	int num_sbi_counters = 0;
	u32 num_deleg_counters = 0;
	u32 num_hw_ctr = 0, num_fw_ctr = 0, num_ctr = 0;
	/*
	 * We don't know how many firmware counters are available. Just allocate
	 * for maximum counters the driver can support. The default is 64 anyways.
	 */
	pmu_ctr_list = kcalloc(RISCV_MAX_COUNTERS, sizeof(*pmu_ctr_list),
			       GFP_KERNEL);
	if (!pmu_ctr_list)
		return -ENOMEM;

	if (riscv_pmu_cdeleg_available_boot())
		num_deleg_counters = rvpmu_deleg_find_ctrs();

	/* SBI PMU is only needed for firmware counters; delegation is independent */
	if (riscv_pmu_sbi_available_boot()) {
		num_sbi_counters = rvpmu_sbi_find_num_ctrs();
		if (num_sbi_counters < 0) {
			/* Keep going if delegation already gave us counters */
			if (!num_deleg_counters) {
				kfree(pmu_ctr_list);
				pmu_ctr_list = NULL;
				return num_sbi_counters;
			}
			pr_warn("Firmware counters unavailable (%d), continuing with %u delegated hardware counters\n",
				num_sbi_counters, num_deleg_counters);
			num_sbi_counters = 0;
		}
		if (num_sbi_counters > RISCV_MAX_COUNTERS)
			num_sbi_counters = RISCV_MAX_COUNTERS;
	}

	/* cache all the information about counters now */
	if (riscv_pmu_sbi_available_boot())
		rvpmu_sbi_get_ctrinfo(num_sbi_counters, &num_fw_ctr, &num_hw_ctr);

	if (riscv_pmu_cdeleg_available_boot()) {
		pr_info("%u firmware and %u hardware counters\n", num_fw_ctr, num_deleg_counters);
		num_ctr = num_fw_ctr + num_deleg_counters;
	} else {
		pr_info("%u firmware and %u hardware counters\n", num_fw_ctr, num_hw_ctr);
		num_ctr = num_sbi_counters;
	}

	return num_ctr;
}

static int rvpmu_event_map(struct perf_event *event, u64 *econfig)
{
	if (rvpmu_is_deleg_event(event))
		return rvpmu_cdeleg_event_map(event, econfig);
	else
		return rvpmu_sbi_event_map(event, econfig);
}

static int rvpmu_ctr_get_idx(struct perf_event *event)
{
	if (rvpmu_is_deleg_event(event))
		return rvpmu_deleg_ctr_get_idx(event);
	else
		return rvpmu_sbi_ctr_get_idx(event);
}

static int rvpmu_starting_cpu(unsigned int cpu, struct hlist_node *node)
{
	struct riscv_pmu *pmu = hlist_entry_safe(node, struct riscv_pmu, node);
	struct cpu_hw_events *cpu_hw_evt = this_cpu_ptr(pmu->hw_events);

	/*
	 * We keep enabling userspace access to CYCLE, TIME and INSTRET via the
	 * legacy option but that will be removed in the future.
	 */
	if (sysctl_perf_user_access == SYSCTL_LEGACY)
		csr_write(CSR_SCOUNTEREN, 0x7);
	else
		csr_write(CSR_SCOUNTEREN, 0x2);

	/* Stop all the counters so that they can be enabled from perf */
	if (riscv_pmu_cdeleg_available()) {
		rvpmu_deleg_ctr_stop_mask(cmask);
		if (riscv_pmu_sbi_available()) {
			/* Stop the firmware counters as well */
			sbi_ecall(SBI_EXT_PMU, SBI_EXT_PMU_COUNTER_STOP, 0, firmware_cmask,
				  0, 0, 0, 0);
		}
	} else {
		rvpmu_sbi_stop_all(pmu);
	}

	if (riscv_pmu_use_irq) {
		cpu_hw_evt->irq = riscv_pmu_irq;
		ALT_SBI_PMU_OVF_CLEAR_PENDING(riscv_pmu_irq_mask);
		enable_percpu_irq(riscv_pmu_irq, IRQ_TYPE_NONE);
	}

	if (sbi_pmu_snapshot_available())
		return pmu_sbi_snapshot_setup(pmu, cpu);

	return 0;
}

static int rvpmu_dying_cpu(unsigned int cpu, struct hlist_node *node)
{
	if (riscv_pmu_use_irq) {
		disable_percpu_irq(riscv_pmu_irq);
	}

	/* Disable all counters access for user mode now */
	csr_write(CSR_SCOUNTEREN, 0x0);

	if (sbi_pmu_snapshot_available())
		return pmu_sbi_snapshot_disable();

	return 0;
}

static int rvpmu_setup_irqs(struct riscv_pmu *pmu, struct platform_device *pdev)
{
	int ret;
	struct cpu_hw_events __percpu *hw_events = pmu->hw_events;
	struct irq_domain *domain = NULL;

	ret = pmu_sbi_setup_sse(pmu);
	if (!ret)
		return 0;

	if (riscv_isa_extension_available(NULL, SSCOFPMF)) {
		riscv_pmu_irq_num = RV_IRQ_PMU;
		riscv_pmu_use_irq = true;
	} else if (IS_ENABLED(CONFIG_ERRATA_THEAD_PMU) &&
		   riscv_cached_mvendorid(0) == THEAD_VENDOR_ID &&
		   riscv_cached_marchid(0) == 0 &&
		   riscv_cached_mimpid(0) == 0) {
		riscv_pmu_irq_num = THEAD_C9XX_RV_IRQ_PMU;
		riscv_pmu_use_irq = true;
	} else if (riscv_has_vendor_extension_unlikely(ANDES_VENDOR_ID,
						       RISCV_ISA_VENDOR_EXT_XANDESPMU) &&
		   IS_ENABLED(CONFIG_ANDES_CUSTOM_PMU)) {
		riscv_pmu_irq_num = ANDES_SLI_CAUSE_BASE + ANDES_RV_IRQ_PMOVI;
		riscv_pmu_use_irq = true;
	}

	riscv_pmu_irq_mask = BIT(riscv_pmu_irq_num % BITS_PER_LONG);

	if (!riscv_pmu_use_irq)
		return -EOPNOTSUPP;

	domain = irq_find_matching_fwnode(riscv_get_intc_hwnode(),
					  DOMAIN_BUS_ANY);
	if (!domain) {
		pr_err("Failed to find INTC IRQ root domain\n");
		ret = -ENODEV;
		goto err;
	}

	riscv_pmu_irq = irq_create_mapping(domain, riscv_pmu_irq_num);
	if (!riscv_pmu_irq) {
		pr_err("Failed to map PMU interrupt for node\n");
		ret = -ENODEV;
		goto err;
	}

	ret = request_percpu_irq(riscv_pmu_irq, rvpmu_ovf_handler, "riscv-pmu", hw_events);
	if (ret) {
		pr_err("registering percpu irq failed [%d]\n", ret);
		irq_dispose_mapping(riscv_pmu_irq);
		riscv_pmu_irq = 0;
		goto err;
	}

	return 0;
err:
	riscv_pmu_use_irq = false;
	return ret;
}

#ifdef CONFIG_CPU_PM
static int riscv_pm_pmu_notify(struct notifier_block *b, unsigned long cmd,
				void *v)
{
	struct riscv_pmu *rvpmu = container_of(b, struct riscv_pmu, riscv_pm_nb);
	struct cpu_hw_events *cpuc = this_cpu_ptr(rvpmu->hw_events);
	int enabled = bitmap_weight(cpuc->used_hw_ctrs, RISCV_MAX_COUNTERS);
	struct perf_event *event;
	int idx;

	if (!enabled)
		return NOTIFY_OK;

	for (idx = 0; idx < RISCV_MAX_COUNTERS; idx++) {
		event = cpuc->events[idx];
		if (!event)
			continue;

		switch (cmd) {
		case CPU_PM_ENTER:
			/*
			 * Stop and update the counter
			 */
			riscv_pmu_stop(event, PERF_EF_UPDATE);
			break;
		case CPU_PM_EXIT:
		case CPU_PM_ENTER_FAILED:
			/*
			 * Restore and enable the counter.
			 */
			riscv_pmu_start(event, PERF_EF_RELOAD);
			break;
		default:
			break;
		}
	}

	return NOTIFY_OK;
}

static int riscv_pm_pmu_register(struct riscv_pmu *pmu)
{
	pmu->riscv_pm_nb.notifier_call = riscv_pm_pmu_notify;
	return cpu_pm_register_notifier(&pmu->riscv_pm_nb);
}

static void riscv_pm_pmu_unregister(struct riscv_pmu *pmu)
{
	cpu_pm_unregister_notifier(&pmu->riscv_pm_nb);
}
#else
static inline int riscv_pm_pmu_register(struct riscv_pmu *pmu) { return 0; }
static inline void riscv_pm_pmu_unregister(struct riscv_pmu *pmu) { }
#endif

static void riscv_pmu_destroy(struct riscv_pmu *pmu)
{
	if (sbi_v2_available) {
		if (sbi_pmu_snapshot_available()) {
			pmu_sbi_snapshot_disable();
			pmu_sbi_snapshot_free(pmu);
		}
	}
	riscv_pm_pmu_unregister(pmu);
	if (!hlist_unhashed(&pmu->node))
		cpuhp_state_remove_instance(CPUHP_AP_PERF_RISCV_STARTING, &pmu->node);
}

static void rvpmu_event_init(struct perf_event *event)
{
	/*
	 * The permissions are set at event_init so that we do not depend
	 * on the sysctl value that can change.
	 */
	if (sysctl_perf_user_access == SYSCTL_NO_USER_ACCESS)
		event->hw.flags |= PERF_EVENT_FLAG_NO_USER_ACCESS;
	else if (sysctl_perf_user_access == SYSCTL_USER_ACCESS)
		event->hw.flags |= PERF_EVENT_FLAG_USER_ACCESS;
	else
		event->hw.flags |= PERF_EVENT_FLAG_LEGACY;
}

static void rvpmu_event_mapped(struct perf_event *event, struct mm_struct *mm)
{
	if (event->hw.flags & PERF_EVENT_FLAG_NO_USER_ACCESS)
		return;

	if (event->hw.flags & PERF_EVENT_FLAG_LEGACY) {
		if (event->attr.config != PERF_COUNT_HW_CPU_CYCLES &&
		    event->attr.config != PERF_COUNT_HW_INSTRUCTIONS) {
			return;
		}
	}

	/*
	 * The user mmapped the event to directly access it: this is where
	 * we determine based on sysctl_perf_user_access if we grant userspace
	 * the direct access to this event. That means that within the same
	 * task, some events may be directly accessible and some other may not,
	 * if the user changes the value of sysctl_perf_user_accesss in the
	 * meantime.
	 */

	event->hw.flags |= PERF_EVENT_FLAG_USER_READ_CNT;

	/*
	 * We must enable userspace access *before* advertising in the user page
	 * that it is possible to do so to avoid any race.
	 * And we must notify all cpus here because threads that currently run
	 * on other cpus will try to directly access the counter too without
	 * calling rvpmu_sbi_ctr_start.
	 */
	if (event->hw.flags & PERF_EVENT_FLAG_USER_ACCESS)
		on_each_cpu_mask(mm_cpumask(mm),
				 rvpmu_set_scounteren, (void *)event, 1);
}

static void rvpmu_event_unmapped(struct perf_event *event, struct mm_struct *mm)
{
	if (event->hw.flags & PERF_EVENT_FLAG_NO_USER_ACCESS)
		return;

	if (event->hw.flags & PERF_EVENT_FLAG_LEGACY) {
		if (event->attr.config != PERF_COUNT_HW_CPU_CYCLES &&
		    event->attr.config != PERF_COUNT_HW_INSTRUCTIONS) {
			return;
		}
	}

	/*
	 * Here we can directly remove user access since the user does not have
	 * access to the user page anymore so we avoid the racy window where the
	 * user could have read cap_user_rdpmc to true right before we disable
	 * it.
	 */
	event->hw.flags &= ~PERF_EVENT_FLAG_USER_READ_CNT;

	if (event->hw.flags & PERF_EVENT_FLAG_USER_ACCESS)
		on_each_cpu_mask(mm_cpumask(mm),
				 rvpmu_reset_scounteren, (void *)event, 1);
}

static void riscv_pmu_update_counter_access(void *info)
{
	if (sysctl_perf_user_access == SYSCTL_LEGACY)
		csr_write(CSR_SCOUNTEREN, 0x7);
	else
		csr_write(CSR_SCOUNTEREN, 0x2);
}

static int riscv_pmu_proc_user_access_handler(struct ctl_table *table,
					      int write, void *buffer,
					      size_t *lenp, loff_t *ppos)
{
	int prev = sysctl_perf_user_access;
	int ret = proc_dointvec_minmax(table, write, buffer, lenp, ppos);

	/*
	 * Test against the previous value since we clear SCOUNTEREN when
	 * sysctl_perf_user_access is set to SYSCTL_USER_ACCESS, but we should
	 * not do that if that was already the case.
	 */
	if (ret || !write || prev == sysctl_perf_user_access)
		return ret;

	on_each_cpu(riscv_pmu_update_counter_access, NULL, 1);

	return 0;
}

static struct ctl_table sbi_pmu_sysctl_table[] = {
	{
		.procname       = "perf_user_access",
		.data		= &sysctl_perf_user_access,
		.maxlen		= sizeof(unsigned int),
		.mode           = 0644,
		.proc_handler	= riscv_pmu_proc_user_access_handler,
		.extra1		= SYSCTL_ZERO,
		.extra2		= SYSCTL_TWO,
	},
	{ }
};

static int rvpmu_device_probe(struct platform_device *pdev)
{
	struct riscv_pmu *pmu = NULL;
	int ret = -ENODEV;
	int num_counters;
	bool irq_requested = false;

	if (riscv_pmu_cdeleg_available_boot()) {
		pr_info("hpmcounters will use the counter delegation ISA extension\n");
		if (riscv_pmu_sbi_available_boot())
			pr_info("Firmware counters will use SBI PMU extension\n");
		else
			pr_info("Firmware counters will not be available as SBI PMU extension is not present\n");
	} else if (riscv_pmu_sbi_available_boot()) {
		pr_info("Both hpmcounters and firmware counters will use SBI PMU extension\n");
	}

	pmu = riscv_pmu_alloc();
	if (!pmu)
		return -ENOMEM;

	num_counters = rvpmu_find_ctrs();
	if (num_counters < 0) {
		pr_err("SBI PMU extension doesn't provide any counters\n");
		goto out_free;
	}

	/* It is possible to get from SBI more than max number of counters */
	if (num_counters > RISCV_MAX_COUNTERS) {
		num_counters = RISCV_MAX_COUNTERS;
		pr_info("SBI returned more than maximum number of counters. Limiting the number of counters to %d\n", num_counters);
	}


	ret = rvpmu_setup_irqs(pmu, pdev);
	if (ret < 0) {
		pr_info("Perf sampling/filtering is not supported as sscof extension is not available\n");
		pmu->pmu.capabilities |= PERF_PMU_CAP_NO_INTERRUPT;
		pmu->pmu.capabilities |= PERF_PMU_CAP_NO_EXCLUDE;
	}
	irq_requested = (ret == 0);

	if (riscv_pmu_cdeleg_available_boot())
		pmu->pmu.attr_groups = riscv_cdeleg_pmu_attr_groups;
	else
		pmu->pmu.attr_groups = riscv_sbi_pmu_attr_groups;

	pmu->cmask = cmask;
	pmu->ctr_start = rvpmu_ctr_start;
	pmu->ctr_stop = rvpmu_ctr_stop;
	pmu->event_map = rvpmu_event_map;
	pmu->ctr_get_idx = rvpmu_ctr_get_idx;
	pmu->ctr_get_width = rvpmu_ctr_get_width;
	pmu->ctr_clear_idx = rvpmu_ctr_clear_idx;
	pmu->ctr_read = rvpmu_ctr_read;
	pmu->event_init = rvpmu_event_init;
	pmu->event_mapped = rvpmu_event_mapped;
	pmu->event_unmapped = rvpmu_event_unmapped;
	pmu->csr_index = rvpmu_csr_index;

	ret = riscv_pm_pmu_register(pmu);
	if (ret)
		goto out_destroy;

	ret = perf_pmu_register(&pmu->pmu, "cpu", PERF_TYPE_RAW);
	if (ret)
		goto out_destroy;

	/* SBI PMU Snapsphot is only available in SBI v2.0 */
	if (sbi_v2_available) {
		ret = pmu_sbi_snapshot_alloc(pmu);
		if (ret)
			goto out_unregister;

		ret = pmu_sbi_snapshot_setup(pmu, smp_processor_id());
		if (ret) {
			/* Snapshot is an optional feature. Continue if not available */
			pmu_sbi_snapshot_free(pmu);
		} else {
			pr_info("SBI PMU snapshot detected\n");
			/*
			 * We enable it once here for the boot cpu. If snapshot shmem setup
			 * fails during cpu hotplug process, it will fail to start the cpu
			 * as we can not handle hetergenous PMUs with different snapshot
			 * capability.
			 */
			static_branch_enable(&sbi_pmu_snapshot_available);
		}
	}

	register_sysctl("kernel", sbi_pmu_sysctl_table);

	ret = cpuhp_state_add_instance(CPUHP_AP_PERF_RISCV_STARTING, &pmu->node);
	if (ret)
		goto out_unregister;

	/* Asynchronously check which standard events are available */
	schedule_work(&check_std_events_work);

	return 0;

out_unregister:
	perf_pmu_unregister(&pmu->pmu);

out_destroy:
	riscv_pmu_destroy(pmu);
	if (irq_requested) {
		free_percpu_irq(riscv_pmu_irq, pmu->hw_events);
		irq_dispose_mapping(riscv_pmu_irq);
		riscv_pmu_irq = 0;
	}

out_free:
	free_percpu(pmu->hw_events);
	kfree(pmu_ctr_list);
	pmu_ctr_list = NULL;
	kfree(pmu);
	return ret;
}

static struct platform_driver rvpmu_driver = {
	.probe		= rvpmu_device_probe,
	.driver		= {
		.name	= RISCV_PMU_SBI_PDEV_NAME,
	},
};

static int __init rvpmu_devinit(void)
{
	int ret;
	struct platform_device *pdev;

	if (sbi_probe_extension(SBI_EXT_PMU)) {
		if (sbi_spec_version >= sbi_mk_version(0, 3)) {
			static_branch_enable(&riscv_pmu_sbi_available);
			if (sbi_spec_version >= sbi_mk_version(2, 0))
				sbi_v2_available = true;
			if (sbi_spec_version >= sbi_mk_version(3, 0))
				sbi_v3_available = true;
		}
	}

	/*
	 * We need all three extensions to be present to access the counters
	 * in S-mode via Supervisor Counter delegation.
	 */
	if (riscv_isa_extension_available(NULL, SSCCFG) &&
	    riscv_isa_extension_available(NULL, SMCDELEG) &&
	    riscv_isa_extension_available(NULL, SSCSRIND)) {
		static_branch_enable(&riscv_pmu_cdeleg_available);
		rvpmu_vendor_register_events();
	}

	if (!(riscv_pmu_sbi_available_boot() || riscv_pmu_cdeleg_available_boot()))
		return 0;

	ret = cpuhp_setup_state_multi(CPUHP_AP_PERF_RISCV_STARTING,
				      "perf/riscv/pmu:starting",
				      rvpmu_starting_cpu, rvpmu_dying_cpu);
	if (ret) {
		pr_err("CPU hotplug notifier could not be registered: %d\n",
		       ret);
		return ret;
	}

	ret = platform_driver_register(&rvpmu_driver);
	if (ret)
		return ret;

	pdev = platform_device_register_simple(RISCV_PMU_SBI_PDEV_NAME, -1, NULL, 0);
	if (IS_ERR(pdev)) {
		platform_driver_unregister(&rvpmu_driver);
		return PTR_ERR(pdev);
	}

	/* Notify legacy implementation that SBI pmu is available*/
	riscv_pmu_legacy_skip_init();

	return ret;
}
device_initcall(rvpmu_devinit)
