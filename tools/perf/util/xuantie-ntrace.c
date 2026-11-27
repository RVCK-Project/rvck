// SPDX-License-Identifier: GPL-2.0
#include <byteswap.h>
#include <endian.h>
#include <errno.h>
#include <inttypes.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/types.h>
#include <linux/zalloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "auxtrace.h"
#include "color.h"
#include "debug.h"
#include "evsel.h"
#include "machine.h"
#include "session.h"
#include "tool.h"
#include <internal/lib.h>
#include "xuantie-ntrace.h"
#include "xuantie-ntrace-decoder/xuantie-ntrace-decoder.h"

#include "thread.h"
#include "thread-stack.h"
#include "map.h"
#include "map_symbol.h"
#include "symbol.h"
#include "dso.h"

/*
 * Script output options, selected by the --itrace n<N> print level:
 *
 *   level | messages | address ranges | insns | notes
 *   ------+----------+----------------+-------+---------------------------
 *      0  |    -     |       -        |   -   |
 *      1  |   yes    |       -        |   -   |
 *      2  |    -     |      yes       |   -   | default
 *      3  |   yes    |      yes       |   -   |
 *      4  |    -     |      yes       |  yes  |
 *     15  |   yes    |      yes       |   -   | same as 3, plus extra
 *         |          |                |       | resource-full messages
 *  others |   yes    |      yes       |  yes  |
 */
struct xt_script_opts {
	bool with_insn;
	bool with_ranges;
	bool with_msg;
};

static struct xt_script_opts xt_script_opts;

struct auxtrace_full {
	u64 reference;
	bool full;
};

struct xuantie_ntrace {
	struct auxtrace auxtrace;
	u32 auxtrace_type;
	struct perf_session *session;
	struct machine *machine;
	u32 pmu_type;

	struct auxtrace_queues queues;
	bool data_queued;
	bool per_thread_decoding;

	struct auxtrace_full afull[256];
	u32 afull_count;
};

static bool xuantie_ntrace_get_afull(u64 reference, struct xuantie_ntrace *ntrace)
{
	u32 i = 0;

	for (i = 0; i < ntrace->afull_count; i++) {
		if (ntrace->afull[i].reference == reference)
			return ntrace->afull[i].full;
	}

	return false;
}

static void xuantie_ntrace_set_afull(u64 reference, struct xuantie_ntrace *ntrace)
{
	if (ntrace->afull_count == 256) {
		pr_err("Auxtrace Event count is bigger than 256.\n");
		return;
	}
	ntrace->afull[ntrace->afull_count].reference = reference;
	ntrace->afull[ntrace->afull_count].full = true;
	ntrace->afull_count++;
}

static int
xuantie_ntrace_process_event(struct perf_session *session __maybe_unused,
			     union perf_event *event __maybe_unused,
			     struct perf_sample *sample __maybe_unused,
			     struct perf_tool *tool __maybe_unused)
{
	return 0;
}

static int xuantie_ntrace_check_saved_config(struct xuantie_saved_config *config)
{
	if (config->_size < 0x18) {
		pr_err("XUANTIE NTRACE: saved_config->_size 0x%x < 0x18\n", config->_size);
		return -1;
	}
	if (config->inst_mode != 3 && config->inst_mode != 6) {
		pr_err("XUANTIE NTRACE: saved_config->inst_mode %d (not 3 or 6)\n", config->inst_mode);
		return -1;
	}
	if (config->src_bits > 15) {
		pr_err("XUANTIE NTRACE: saved_config->src_bits %d is bigger than 15.\n", config->src_bits);
		return -1;
	}
	if (config->timestamp_bits > 63) {
		pr_err("XUANTIE NTRACE: saved_config->timestamp_bits %d is bigger than 63\n", config->timestamp_bits);
		return -1;
	}

	return 0;
}

/*
 * A single auxtrace buffer may hold several [saved_config][trace data]
 * segments back to back: the driver opens one AUX transaction per sink and
 * per commit, while perf groups everything written between two aux mmap reads
 * into one PERF_RECORD_AUXTRACE. The header carries no segment length, so
 * locate the next one by its signature and decode each segment on its own.
 */
static bool xuantie_ntrace_header_at(const unsigned char *data, size_t remain)
{
	struct xuantie_saved_config config;

	if (remain < sizeof(config))
		return false;

	memcpy(&config, data, sizeof(config));

	return config._size == sizeof(config) &&
	       (config.inst_mode == 3 || config.inst_mode == 6) &&
	       config.src_bits <= 15 && config.timestamp_bits <= 63 &&
	       config.trace_ram_wrap <= 1 && config._align == 0;
}

static size_t xuantie_ntrace_next_segment(const unsigned char *data, size_t size,
					  size_t from)
{
	/* Leading byte of the header's _size field, used to skip ahead. */
	const unsigned char tag = sizeof(struct xuantie_saved_config);
	size_t pos = from;

	while (pos < size) {
		const unsigned char *hit = memchr(data + pos, tag, size - pos);

		if (!hit)
			break;

		pos = hit - data;
		if (xuantie_ntrace_header_at(data + pos, size - pos))
			return pos;
		pos++;
	}

	return size;
}

static void xuantie_ntrace__dump_segment(struct perf_session *session,
					 struct auxtrace_buffer *buffer,
					 unsigned char *data, size_t size)
{
	struct xuantie_saved_config *saved_config;
	const char *color = PERF_COLOR_BLUE;

	if (size < sizeof(*saved_config)) {
		pr_err("XUANTIE NTRACE: error aux size 0x%zx\n", size);
		return;
	}

	saved_config = (struct xuantie_saved_config *)data;
	fprintf(stdout, "\n");
	color_fprintf(stdout, color, ". xuantie_ntrace_dump: saved_configs are:\n");
	fprintf(stdout, ".  saved_config->_size is 0x%x.\n", saved_config->_size);
	fprintf(stdout, ".  saved_config->inst_mode is 0x%x.\n", saved_config->inst_mode);
	fprintf(stdout, ".  saved_config->src_bits is 0x%x.\n", saved_config->src_bits);
	fprintf(stdout, ".  saved_config->timestamp_bits is 0x%x.\n", saved_config->timestamp_bits);
	fprintf(stdout, ".  saved_config->trace_ram_wrap is 0x%x.\n", saved_config->trace_ram_wrap);
	fprintf(stdout, ".  metadata size is 0x%zx.\n", size - sizeof(*saved_config));

	if (xuantie_ntrace_check_saved_config(saved_config))
		return;

	if (size > sizeof(*saved_config)) {
		if (xuantie_ntrace_decoder__process_metadata(
			    saved_config,
			    data + sizeof(*saved_config),
			    size - sizeof(*saved_config),
			    session, buffer, true, false, false) == 0) {
			color_fprintf(stdout, color, ". ntrace messages are(There is a scheduling event happening here.):\n");
		}
	}
}

static void xuantie_ntrace__dump_event(struct perf_session *session __maybe_unused,
				       struct auxtrace_buffer *buffer)
{
	unsigned char *data = buffer->data;
	size_t off = 0;

	while (off < buffer->size) {
		size_t from = off + sizeof(struct xuantie_saved_config);
		size_t next = xuantie_ntrace_next_segment(data, buffer->size, from);

		xuantie_ntrace__dump_segment(session, buffer, data + off,
					     next - off);
		off = next;
	}
}

static void dump_queued_data(struct xuantie_ntrace *ntrace, struct perf_record_auxtrace *event)
{
	struct auxtrace_buffer *buffer;
	unsigned int i;
	/*
	 * Find all buffers with same reference in the queues and dump them.
	 * This is because the queues can contain multiple entries of the same
	 * buffer that were split on aux records.
	 */
	for (i = 0; i < ntrace->queues.nr_queues; ++i)
		list_for_each_entry(buffer, &ntrace->queues.queue_array[i].head, list)
			if (buffer->reference == event->reference)
				xuantie_ntrace__dump_event(ntrace->session, buffer);
}

static int
xuantie_ntrace_process_auxtrace_event(struct perf_session *session,
				      union perf_event *event,
				      struct perf_tool *tool __maybe_unused)
{
	struct xuantie_ntrace *ntrace = container_of(session->auxtrace, struct xuantie_ntrace, auxtrace);

	if (!ntrace->data_queued) {
		struct auxtrace_buffer *buffer;
		off_t data_offset;
		int fd = perf_data__fd(session->data);
		bool is_pipe = perf_data__is_pipe(session->data);
		int err;

		if (is_pipe)
			data_offset = 0;
		else {
			data_offset = lseek(fd, 0, SEEK_CUR);
			if (data_offset == -1)
				return -errno;
		}

		err = auxtrace_queues__add_event(&ntrace->queues, session, event, data_offset, &buffer);
		if (err)
			return err;

		if (dump_trace)
			if (auxtrace_buffer__get_data(buffer, fd)) {
				xuantie_ntrace__dump_event(session, buffer);
				auxtrace_buffer__put_data(buffer);
			}
	} else if (dump_trace) {
		dump_queued_data(ntrace, &event->auxtrace);
	}

	return 0;
}

static int xuantie_ntrace_flush(struct perf_session *session,
				struct perf_tool *tool __maybe_unused)
{
	unsigned int i;
	struct xuantie_ntrace *ntrace = container_of(session->auxtrace, struct xuantie_ntrace, auxtrace);
	struct auxtrace_queues *queues = &ntrace->queues;
	int fd = perf_data__fd(session->data);

	if (dump_trace)
		return 0;

	for (i = 0; i < queues->nr_queues; i++) {
		struct auxtrace_queue *queue = &queues->queue_array[i];
		struct auxtrace_buffer *buffer = NULL;

		if (XT_PERF_GET_CTRL_C())
			break;

		buffer = auxtrace_buffer__next(queue, NULL);

		while (buffer) {
			unsigned char *data;
			size_t off = 0;
			bool failed = false;

			if (!auxtrace_buffer__get_data(buffer, fd)) {
				buffer = auxtrace_buffer__next(queue, buffer);
				continue;
			}

			data = buffer->data;

			while (off < buffer->size) {
				struct xuantie_saved_config *saved_config;
				size_t from = off + sizeof(*saved_config);
				size_t size;
				size_t next;

				next = xuantie_ntrace_next_segment(data, buffer->size, from);
				size = next - off;

				if (size < sizeof(*saved_config)) {
					pr_err("XUANTIE NTRACE: error aux size 0x%zx\n", size);
					break;
				}

				saved_config = (struct xuantie_saved_config *)(data + off);

				if (xuantie_ntrace_check_saved_config(saved_config))
					break;

				if (size > sizeof(*saved_config)) {
					pr_info("Parsing buffer: size 0x%zx, offset 0x%lx.\n",
						size, (unsigned long)(buffer->offset + off));
					if (xuantie_ntrace_decoder__process_metadata(
						    saved_config,
						    data + off + sizeof(*saved_config),
						    size - sizeof(*saved_config),
						    session, buffer,
						    xt_script_opts.with_msg,
						    xt_script_opts.with_ranges,
						    xt_script_opts.with_insn)) {
						pr_err("Failed to parse trace metadata\n");
						failed = true;
						break;
					}
				}

				off = next;
			}

			auxtrace_buffer__put_data(buffer);

			if (failed || XT_PERF_GET_CTRL_C())
				return 0;

			buffer = auxtrace_buffer__next(queue, buffer);
		}
	}

	return 0;
}

static void
xuantie_ntrace_free_events(struct perf_session *session __maybe_unused)
{
}

static void xuantie_ntrace_free(struct perf_session *session)
{
	struct xuantie_ntrace *ntrace = container_of(session->auxtrace, struct xuantie_ntrace, auxtrace);

	auxtrace_queues__free(&ntrace->queues);
	session->auxtrace = NULL;
	free(ntrace);
}

static bool xuantie_ntrace_evsel_is_auxtrace(struct perf_session *session,
					     struct evsel *evsel)
{
	struct xuantie_ntrace *ntrace = container_of(session->auxtrace, struct xuantie_ntrace, auxtrace);

	return evsel->core.attr.type == ntrace->pmu_type;
}

static void xuantie_ntrace_print_info(__u64 type)
{
	if (!dump_trace)
		return;

	fprintf(stdout, "  PMU Type           %" PRId64 "\n", (s64)type);
}

/**
 * Puts a fragment of an auxtrace buffer into the auxtrace queues based
 * on the bounds of aux_event, if it matches with the buffer that's at
 * file_offset.
 */
static int xuantie_ntrace__queue_aux_fragment(struct perf_session *session,
					      off_t file_offset, size_t sz,
					      struct perf_record_aux *aux_event)
{
	int err;
	char buf[PERF_SAMPLE_MAX_SIZE];
	union perf_event *auxtrace_event_union;
	struct perf_record_auxtrace *auxtrace_event;
	union perf_event auxtrace_fragment;
	__u64 aux_offset, aux_size;
	struct xuantie_ntrace *ntrace = container_of(session->auxtrace, struct xuantie_ntrace, auxtrace);

	/*
	 * There should be a PERF_RECORD_AUXTRACE event at the file_offset that we got
	 * from looping through the auxtrace index.
	 */
	err = perf_session__peek_event(session, file_offset, buf,
				       PERF_SAMPLE_MAX_SIZE,
				       &auxtrace_event_union, NULL);
	if (err)
		return err;
	auxtrace_event = &auxtrace_event_union->auxtrace;
	if (auxtrace_event->header.type != PERF_RECORD_AUXTRACE)
		return -EINVAL;

	if (auxtrace_event->header.size < sizeof(struct perf_record_auxtrace) ||
	    auxtrace_event->header.size != sz) {
		return -EINVAL;
	}

	/* Here, we only compare aux_offset, aux_size, auxtrace_event->offset, auxtrace_event->size,
	 * which may call two aux events with offset 0 are connected to one auxtrace event.
	 * Here, try to use auxtrace_full as a flag to mark this auxtrace event can not be used any
	 * more.
	 *
	 * FIXME: clearify how to connect an aux event to auxtrace event?
	 */
	if (xuantie_ntrace_get_afull(auxtrace_event->reference, ntrace))
		return 1;

	if (aux_event->flags & PERF_AUX_FLAG_OVERWRITE) {
		/*
		 * Clamp size in snapshot mode. The buffer size is clamped in
		 * __auxtrace_mmap__read() for snapshots, so the aux record size doesn't reflect
		 * the buffer size.
		 */
		aux_size = min(aux_event->aux_size, auxtrace_event->size);

		/*
		 * In this mode, the head also points to the end of the buffer so aux_offset
		 * needs to have the size subtracted so it points to the beginning as in normal mode
		 */
		aux_offset = aux_event->aux_offset - aux_size;
	} else {
		aux_size = aux_event->aux_size;
		aux_offset = aux_event->aux_offset;
	}

	if (aux_offset >= auxtrace_event->offset &&
	    aux_offset + aux_size <=
		    auxtrace_event->offset + auxtrace_event->size) {
		/*
		 * If this AUX event was inside this buffer somewhere, create a new auxtrace event
		 * based on the sizes of the aux event, and queue that fragment.
		 */
		auxtrace_fragment.auxtrace = *auxtrace_event;
		auxtrace_fragment.auxtrace.size = aux_size;
		auxtrace_fragment.auxtrace.offset = aux_offset;
		file_offset += aux_offset - auxtrace_event->offset + auxtrace_event->header.size;

		err = auxtrace_queues__add_event(&ntrace->queues, session,
						 &auxtrace_fragment,
						 file_offset, NULL);
		if (err)
			return err;

		if ((aux_offset + aux_size) == (auxtrace_event->offset + auxtrace_event->size))
			xuantie_ntrace_set_afull(auxtrace_event->reference, ntrace);

		return 0;
	}

	/* Wasn't inside this buffer, but there were no parse errors. 1 == 'not found' */
	return 1;
}

static int xuantie_ntrace__queue_aux_records_cb(struct perf_session *session,
						union perf_event *event,
						u64 offset __maybe_unused,
						void *data __maybe_unused)
{
	int ret;
	struct auxtrace_index_entry *ent;
	struct auxtrace_index *auxtrace_index;
	size_t i;

	/* Don't care about any other events, we're only queuing buffers for AUX events */
	if (event->header.type != PERF_RECORD_AUX)
		return 0;

	if (event->header.size < sizeof(struct perf_record_aux))
		return -EINVAL;

	/* Truncated Aux records can have 0 size and shouldn't result in anything being queued. */
	if (!event->aux.aux_size)
		return 0;

	/*
	 * Loop through the auxtrace index to find the buffer that matches up with this aux event.
	 */
	list_for_each_entry(auxtrace_index, &session->auxtrace_index, list) {
		for (i = 0; i < auxtrace_index->nr; i++) {
			ent = &auxtrace_index->entries[i];
			ret = xuantie_ntrace__queue_aux_fragment(session, ent->file_offset, ent->sz, &event->aux);
			/*
			 * Stop search on error or successful values. Continue search on
			 * 1 ('not found')
			 */
			if (ret != 1)
				return ret;
		}
	}

	/*
	 * Couldn't find the buffer corresponding to this aux record, something went wrong. Warn but
	 * don't exit with an error because it will still be possible to decode other aux records.
	 */
	pr_err("Couldn't find auxtrace buffer for aux_offset: %#" PRI_lx64 "\n", event->aux.aux_offset);

	return 0;
}

static int xuantie_ntrace__queue_aux_records(struct perf_session *session)
{
	struct auxtrace_index *index = list_first_entry_or_null(&session->auxtrace_index, struct auxtrace_index, list);
	if (index && index->nr > 0)
		return perf_session__peek_events(
			session, session->header.data_offset,
			session->header.data_size,
			xuantie_ntrace__queue_aux_records_cb, NULL);
	/* No aux event. */
	return 0;
}

int xuantie_ntrace_process_auxtrace_info(union perf_event *event,
					 struct perf_session *session)
{
	int err = 0;
	struct perf_record_auxtrace_info *auxtrace_info = &event->auxtrace_info;
	struct xuantie_ntrace *ntrace;

	if (auxtrace_info->header.size <
	    XUANTIE_NTRACE_AUXTRACE_PRIV_SIZE + sizeof(struct perf_record_auxtrace_info))
		return -EINVAL;

	ntrace = zalloc(sizeof(*ntrace));
	if (!ntrace)
		return -ENOMEM;

	err = auxtrace_queues__init(&ntrace->queues);
	if (err)
		goto err_free_ntrace;

	/* Default: --itrace=n2, address ranges only. */
	xt_script_opts = (struct xt_script_opts){ .with_ranges = true };

	ntrace->session = session;
	ntrace->machine = &session->machines.host; /* No kvm support */
	ntrace->auxtrace_type = auxtrace_info->type;
	ntrace->pmu_type = auxtrace_info->priv[0];

	/* priv[1] records the traced machine's MMU mode for offline decode. */
	if (auxtrace_info->priv[1] < MMU_UNKNOWN)
		xt_trace_set_mmu_mode((enum rv_mmu_type)auxtrace_info->priv[1]);

	ntrace->auxtrace.process_event = xuantie_ntrace_process_event;
	ntrace->auxtrace.process_auxtrace_event = xuantie_ntrace_process_auxtrace_event;
	ntrace->auxtrace.flush_events = xuantie_ntrace_flush;
	ntrace->auxtrace.free_events = xuantie_ntrace_free_events;
	ntrace->auxtrace.free = xuantie_ntrace_free;
	ntrace->auxtrace.evsel_is_auxtrace = xuantie_ntrace_evsel_is_auxtrace;
	session->auxtrace = &ntrace->auxtrace;

	xuantie_ntrace_print_info(auxtrace_info->priv[0]);

	err = xuantie_ntrace__queue_aux_records(session);
	if (err)
		goto err_free_queues;

	ntrace->data_queued = ntrace->queues.populated;

	/* Sync itrace synth opts; see the print level table above. */
	if (session->itrace_synth_opts &&
	    session->itrace_synth_opts->ntrace_print_level_set) {
		int level = session->itrace_synth_opts->ntrace_print_level;

		xt_trace_set_extra_msg(level == XT_NTRACE_PRINT_LEVEL_EXTRA);
		xt_script_opts = (struct xt_script_opts){ };
		switch (level) {
		case 0:
			break;
		case 1:
			xt_script_opts.with_msg = true;
			break;
		case 2:
			xt_script_opts.with_ranges = true;
			break;
		case 3:
		case XT_NTRACE_PRINT_LEVEL_EXTRA:
			xt_script_opts.with_ranges = true;
			xt_script_opts.with_msg = true;
			break;
		case 4:
			xt_script_opts.with_ranges = true;
			xt_script_opts.with_insn = true;
			break;
		default:
			xt_script_opts.with_ranges = true;
			xt_script_opts.with_insn = true;
			xt_script_opts.with_msg = true;
			break;

		}
	}

	return 0;

err_free_queues:
	auxtrace_queues__free(&ntrace->queues);
	session->auxtrace = NULL;
err_free_ntrace:
	zfree(&ntrace);
	return err;
}
