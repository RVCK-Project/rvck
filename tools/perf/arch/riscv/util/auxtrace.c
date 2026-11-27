// SPDX-License-Identifier: GPL-2.0

#include <dirent.h>
#include <stdbool.h>
#include <linux/zalloc.h>
#include <api/fs/fs.h>

#include "../../../util/auxtrace.h"
#include "../../../util/debug.h"
#include "../../../util/evlist.h"
#include "../../../util/pmu.h"
#include "../../../util/pmus.h"
#include "xuantie-ntrace.h"

struct auxtrace_record
*auxtrace_record__init(struct evlist *evlist, int *err)
{
	struct perf_pmu	*xuantie_ntrace_pmu = NULL;
	struct evsel *evsel;
	bool found_xuantie_ntrace = false;

	xuantie_ntrace_pmu = perf_pmus__find(XUANTIE_NTRACE_PMU_NAME);

	evlist__for_each_entry(evlist, evsel) {
		if (xuantie_ntrace_pmu && evsel->core.attr.type == xuantie_ntrace_pmu->type)
			found_xuantie_ntrace = true;
	}

	if (found_xuantie_ntrace)
		return xuantie_ntrace_recording_init(err, xuantie_ntrace_pmu);

	*err = 0;
	return NULL;
}
