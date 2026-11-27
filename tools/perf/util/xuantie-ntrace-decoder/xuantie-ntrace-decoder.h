/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __XUANTIE_NTRACE_DECODER_H
#define __XUANTIE_NTRACE_DECODER_H

#include <linux/types.h>
#include "machine.h"
#include "session.h"
#include "auxtrace.h"

struct xuantie_saved_config {
	u32 _size;
	u32 inst_mode;
	u32 src_bits;
	u32 timestamp_bits;
	u32 trace_ram_wrap;
	u32 _align;
};

int32_t xuantie_ntrace_decoder__process_metadata(struct xuantie_saved_config *saved_config,
		unsigned char *buf, size_t len, struct perf_session *session,
		struct auxtrace_buffer *buffer, bool with_msg, bool with_addr,
		bool with_insn);
int32_t xuantie_ntrace_decoder__process_full_message(struct perf_session *session,
		struct auxtrace_buffer *buffer, bool analysis_ranges,
		uint64_t init_pre_full_addr, uint64_t *out_pre_full_addr);
int32_t xt_trace_program_trace_display(bool with_msg, bool with_addr, bool with_insn);
void xt_trace_set_extra_msg(bool on);

/* When user do a CTRL_C, it will be handled by "static void sig_handler(int sig __maybe_unused)"
 * in builtin-script.c, and var "session_done" will be set to 1.
 *
 * Later, code can get "session_done" by function session_done(), which is defined in session.h
 */
#define XT_PERF_GET_CTRL_C()  (session_done())

#endif
