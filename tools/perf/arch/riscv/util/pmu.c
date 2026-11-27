// SPDX-License-Identifier: GPL-2.0

#include <string.h>
#include <linux/perf_event.h>
#include <linux/string.h>

#include "xuantie-ntrace.h"
#include "../../../util/pmu.h"

void perf_pmu__arch_init(struct perf_pmu *pmu __maybe_unused)
{
#ifdef HAVE_AUXTRACE_SUPPORT
	if (!strcmp(pmu->name, XUANTIE_NTRACE_PMU_NAME)) {
		pmu->auxtrace = true;
		pmu->selectable = true;
	}
#endif
}
