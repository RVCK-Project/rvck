/* SPDX-License-Identifier: GPL-2.0 */
#ifndef INCLUDE__PERF_XUANTIE_NTRACE_H__
#define INCLUDE__PERF_XUANTIE_NTRACE_H__

#define XUANTIE_NTRACE_PMU_NAME		"xuantie_ntrace"
/*
 * auxtrace_info priv layout:
 *   [0] PMU type
 *   [1] MMU mode of the traced machine (enum rv_mmu_type), so offline
 *       decode on a different host does not depend on that host's
 *       /proc/cpuinfo.
 */
#define XUANTIE_NTRACE_AUXTRACE_PRIV_SIZE	(2 * sizeof(u64))

/*
 * --itrace=n print levels.
 */
#define XT_NTRACE_PRINT_LEVEL_EXTRA	15
#define XT_NTRACE_PRINT_LEVEL_MAX	XT_NTRACE_PRINT_LEVEL_EXTRA

enum rv_mmu_type {
	MMU_SV39,
	MMU_SV48,
	MMU_SV57,
	MMU_UNKNOWN
};

/* Parse the MMU mode from the local machine's /proc/cpuinfo. */
enum rv_mmu_type xt_parse_mmu_mode(void);
/* Feed the MMU mode recorded in perf.data to the decode helpers. */
void xt_trace_set_mmu_mode(enum rv_mmu_type mode);

union perf_event;
struct perf_session;
struct perf_pmu;

struct auxtrace_record *xuantie_ntrace_recording_init(int *err,
						struct perf_pmu *xuantie_ntrace_pmu);

int xuantie_ntrace_process_auxtrace_info(union perf_event *event,
				   struct perf_session *session);

#endif
