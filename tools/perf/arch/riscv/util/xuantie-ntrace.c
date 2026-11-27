// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/log2.h>
#include <linux/zalloc.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <internal/lib.h> // page_size
#include "../../../util/auxtrace.h"
#include "../../../util/cpumap.h"
#include "../../../util/debug.h"
#include "../../../util/event.h"
#include "../../../util/evlist.h"
#include "../../../util/evsel.h"
#include "../../../util/pmu.h"
#include "../../../util/record.h"
#include "../../../util/session.h"
#include "../../../util/tsc.h"
#include "../../../util/xuantie-ntrace.h"

#define KiB(x) ((x) * 1024)
#define MiB(x) ((x) * 1024 * 1024)

struct xuantie_ntrace_recording {
	struct auxtrace_record	itr;
	struct perf_pmu *xuantie_ntrace_pmu;
	struct evlist *evlist;
};

static size_t
xuantie_ntrace_info_priv_size(struct auxtrace_record *itr __maybe_unused,
			struct evlist *evlist __maybe_unused)
{
	return XUANTIE_NTRACE_AUXTRACE_PRIV_SIZE;
}

static int xuantie_ntrace_info_fill(struct auxtrace_record *itr,
			      struct perf_session *session,
			      struct perf_record_auxtrace_info *auxtrace_info,
			      size_t priv_size)
{
	struct xuantie_ntrace_recording *pttr =
			container_of(itr, struct xuantie_ntrace_recording, itr);
	struct perf_pmu *xuantie_ntrace_pmu = pttr->xuantie_ntrace_pmu;

	if (priv_size != XUANTIE_NTRACE_AUXTRACE_PRIV_SIZE)
		return -EINVAL;

	if (!session->evlist->core.nr_mmaps)
		return -EINVAL;

	auxtrace_info->type = PERF_AUXTRACE_XUANTIE_NTRACE;
	auxtrace_info->priv[0] = xuantie_ntrace_pmu->type;
	auxtrace_info->priv[1] = xt_parse_mmu_mode();

	return 0;
}

static int xuantie_ntrace_set_auxtrace_mmap_page(struct record_opts *opts)
{
	bool privileged = perf_event_paranoid_check(-1);

	if (!opts->full_auxtrace)
		return 0;

	if (opts->full_auxtrace && !opts->auxtrace_mmap_pages) {
		if (privileged) {
			opts->auxtrace_mmap_pages = MiB(16) / page_size;
		} else {
			opts->auxtrace_mmap_pages = KiB(128) / page_size;
			if (opts->mmap_pages == UINT_MAX)
				opts->mmap_pages = KiB(256) / page_size;
		}
	}

	/* Validate auxtrace_mmap_pages */
	if (opts->auxtrace_mmap_pages) {
		size_t sz = opts->auxtrace_mmap_pages * (size_t)page_size;
		size_t min_sz = KiB(8);

		if (sz < min_sz || !is_power_of_2(sz)) {
			pr_err("Invalid mmap size for xuantie ntrace: must be at least %zuKiB and a power of 2\n",
			       min_sz / 1024);
			return -EINVAL;
		}
	}

	return 0;
}

static int xuantie_ntrace_recording_options(struct auxtrace_record *itr,
				      struct evlist *evlist,
				      struct record_opts *opts)
{
	struct xuantie_ntrace_recording *pttr =
			container_of(itr, struct xuantie_ntrace_recording, itr);
	struct perf_pmu *xuantie_ntrace_pmu = pttr->xuantie_ntrace_pmu;
	struct evsel *evsel, *xuantie_ntrace_evsel = NULL;
	struct evsel *tracking_evsel;
	int err;

	pttr->evlist = evlist;
	evlist__for_each_entry(evlist, evsel) {
		if (evsel->core.attr.type == xuantie_ntrace_pmu->type) {
			if (xuantie_ntrace_evsel) {
				pr_err("There may be only one " XUANTIE_NTRACE_PMU_NAME " event\n");
				return -EINVAL;
			}
			evsel->core.attr.freq = 0;
			evsel->core.attr.sample_period = 1;
			evsel->needs_auxtrace_mmap = true;
			xuantie_ntrace_evsel = evsel;
			opts->full_auxtrace = true;
		}
	}

	err = xuantie_ntrace_set_auxtrace_mmap_page(opts);
	if (err)
		return err;

	/*
	 * Must come after set_auxtrace_mmap_page(): that is where
	 * auxtrace_mmap_pages gets its default value. snapshot_size caps how
	 * much the snapshot-on-exit read consumes; 0 would truncate it to
	 * nothing while still advancing the read pointer (losing the data),
	 * so allow the full AUX buffer.
	 */
	opts->auxtrace_snapshot_size = (size_t)opts->auxtrace_mmap_pages * page_size;
	opts->auxtrace_snapshot_on_exit = true;

	/*
	 * The snapshot-on-exit hook uses the PMU sysfs "commit" attribute.
	 * If it is not writable the deferred trace is not committed at
	 * snapshot time; task exit still commits through event_del(), so
	 * warn instead of refusing to record.
	 */
	{
		char path[PATH_MAX];

		perf_pmu__pathname_scnprintf(path, sizeof(path),
					     xuantie_ntrace_pmu->name, "commit");
		if (access(path, W_OK))
			pr_warning("Warning: %s is not writable: trace may not be committed on exit unless the session ends via task exit.\n",
				   path);
	}

	/*
	 * To obtain the auxtrace buffer file descriptor, the auxtrace event
	 * must come first.
	 */
	evlist__to_front(evlist, xuantie_ntrace_evsel);
	evsel__set_sample_bit(xuantie_ntrace_evsel, TIME);

	/* Add dummy event to keep tracking */
	err = parse_event(evlist, "dummy:u");
	if (err)
		return err;

	tracking_evsel = evlist__last(evlist);
	evlist__set_tracking_event(evlist, tracking_evsel);

	tracking_evsel->core.attr.freq = 0;
	tracking_evsel->core.attr.sample_period = 1;
	evsel__set_sample_bit(tracking_evsel, TIME);

	return 0;
}

static u64 xuantie_ntrace_reference(struct auxtrace_record *itr __maybe_unused)
{
	return rdtsc();
}

/*
 * Invoked by record__auxtrace_snapshot_exit() after the main loop ends
 * (auxtrace_snapshot_on_exit) and before the final buffer flush. Writing
 * the PMU sysfs "commit" file makes the driver commit the whole deferred
 * trace into the AUX buffer synchronously, so the flush that follows is
 * guaranteed to see the data. A no-op (in the driver) if the session was
 * already committed by the task-exit path.
 */
static int xuantie_ntrace_snapshot_start(struct auxtrace_record *itr)
{
	struct xuantie_ntrace_recording *pttr =
			container_of(itr, struct xuantie_ntrace_recording, itr);
	char path[PATH_MAX];
	int fd, ret = 0;

	perf_pmu__pathname_scnprintf(path, sizeof(path),
				     pttr->xuantie_ntrace_pmu->name, "commit");

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		pr_err("Failed to open %s\n", path);
		return -1;
	}

	if (write(fd, "1", 1) != 1) {
		pr_err("Failed to trigger commit via %s\n", path);
		ret = -1;
	}

	close(fd);
	return ret;
}

static void xuantie_ntrace_recording_free(struct auxtrace_record *itr)
{
	struct xuantie_ntrace_recording *pttr =
			container_of(itr, struct xuantie_ntrace_recording, itr);

	free(pttr);
}

struct auxtrace_record *xuantie_ntrace_recording_init(int *err,
						struct perf_pmu *xuantie_ntrace_pmu)
{
	struct xuantie_ntrace_recording *pttr;

	if (!xuantie_ntrace_pmu) {
		*err = -ENODEV;
		return NULL;
	}

	pttr = zalloc(sizeof(*pttr));
	if (!pttr) {
		*err = -ENOMEM;
		return NULL;
	}

	pttr->xuantie_ntrace_pmu = xuantie_ntrace_pmu;
	pttr->itr.pmu = xuantie_ntrace_pmu;
	pttr->itr.recording_options = xuantie_ntrace_recording_options;
	pttr->itr.info_priv_size = xuantie_ntrace_info_priv_size;
	pttr->itr.info_fill = xuantie_ntrace_info_fill;
	pttr->itr.free = xuantie_ntrace_recording_free;
	pttr->itr.reference = xuantie_ntrace_reference;
	pttr->itr.read_finish = auxtrace_record__read_finish;
	pttr->itr.snapshot_start = xuantie_ntrace_snapshot_start;
	pttr->itr.alignment = 0;

	*err = 0;
	return &pttr->itr;
}
