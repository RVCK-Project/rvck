// SPDX-License-Identifier: GPL-2.0

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include "thread.h"
#include "thread-stack.h"
#include "map.h"
#include "map_symbol.h"
#include "symbol.h"
#include "dso.h"
#include "debug.h"
#include "xuantie-ntrace.h"
#include <linux/zalloc.h>

#include "xuantie-ntrace-decoder.h"
#include "xuantie-mseo-mdo.h"
#include "xuantie-ntrace-message.h"

#define XT_MAX_REPEAT_COUNT	(1U << 20)

enum GET_AN_INSN_STATE {
	GET_AN_INSN_FIRST = 0,
	GET_AN_INSN_MIDDLE = 1,
	GET_AN_INSN_END = 2,
};

static bool xt_with_extra_msg;

void xt_trace_set_extra_msg(bool on)
{
	xt_with_extra_msg = on;
}

/* MMU mode recorded in perf.data by the recording side, if available. */
static enum rv_mmu_type xt_recorded_mmu_mode = MMU_UNKNOWN;

// used for i-cnt and hist
struct xt_trace_address_range {
	uint64_t start_addr;
	uint64_t end_addr;
	int extra_msg;
	char *msg;
	struct xt_trace_address_range *next;
};

#define XT_EXTRA_MSG_SIZE 1024
#define STR_LEN_MAX (1024 * 8)

struct xt_trace_msg {
	char *pos;
	size_t room;
};

#define XT_TRACE_MSG(buf, size) ((struct xt_trace_msg){ (buf), (size) })

static size_t xt_trace_msg_append(struct xt_trace_msg *msg, const char *fmt, ...)
{
	va_list ap;
	int written;

	if (!msg->room)
		return 0;

	va_start(ap, fmt);
	written = vsnprintf(msg->pos, msg->room, fmt, ap);
	va_end(ap);

	if (written < 0)
		return 0;

	if ((size_t)written >= msg->room) {
		pr_err("Trace message truncated: %d bytes needed, %zu left\n",
		       written, msg->room);
		msg->pos += msg->room - 1;
		msg->room = 0;
		return 0;
	}

	msg->pos += written;
	msg->room -= written;
	return written;
}

static struct xt_trace_address_range *xt_trace_alloc_extra_msg_range(void)
{
	struct xt_trace_address_range *range = zalloc(sizeof(*range));

	if (!range)
		return NULL;

	range->msg = zalloc(XT_EXTRA_MSG_SIZE);
	if (!range->msg) {
		free(range);
		return NULL;
	}
	range->extra_msg = 1;

	return range;
}

struct xt_trace_program_flow_node {
	uint64_t start_addr; // the start addr of this message
	char saddr_dso[PATH_MAX];
	char saddr_sym[PATH_MAX];
	// the start addr of the next message
	// also means the destination address of this message
	uint64_t full_addr;
	char faddr_dso[PATH_MAX];
	char faddr_sym[PATH_MAX];
	struct xt_trace_address_range *addr_range; // need free
	struct xt_riscv_nexus_trace_message msg;
	struct xt_riscv_nexus_trace_message *ownership; // no need to free

	// We need to go through the list from head to end
	// and from end to head.
	struct xt_trace_program_flow_node *next;
	struct xt_trace_program_flow_node *prev;

	//set if can't get full addr or addr_range
	int invalid;
};

/*************************************************************
 * Classify the insns
 *************************************************************/
static inline unsigned int riscv_insn_length(uint8_t insn)
{
	if ((insn & 0x3) != 0x3) /* RVC.  */
		return 2;
	if ((insn & 0x1f) !=
	    0x1f) /* Base ISA and extensions in 32-bit space.  */
		return 4;
	if ((insn & 0x7f) == 0x7f) /* DROPPED. p extensions.  */
		return 4;
	if ((insn & 0x3f) == 0x1f) /* 48-bit extensions.  */
		return 6;
	if ((insn & 0x7f) == 0x3f) /* 64-bit extensions.  */
		return 8;
	/* Longer instructions not supported at the moment.  */
	return 2;
}

enum rv_mmu_type xt_parse_mmu_mode(void)
{
	FILE *fp;
	char line[256];
	const char *target = "mmu";
	const char *delimiter = ":";
	char *key, *value;
	enum rv_mmu_type mmu_mode = MMU_UNKNOWN;

	fp = fopen("/proc/cpuinfo", "r");
	if (!fp) {
		pr_err("Failed to open /proc/cpuinfo\n");
		return mmu_mode;
	}

	while (fgets(line, sizeof(line), fp)) {
		if (strstr(line, target)) {
			key = strtok(line, delimiter);
			value = strtok(NULL, delimiter);
			if (key && value) {
				key = strtok(key, " \t\n");
				value = strtok(value, " \t\n");
				if (strcmp(key, target) == 0) {
					if (strcmp(value, "sv39") == 0)
						mmu_mode = MMU_SV39;
					else if (strcmp(value, "sv48") == 0)
						mmu_mode = MMU_SV48;
					else if (strcmp(value, "sv57") == 0)
						mmu_mode = MMU_SV57;
					else
						mmu_mode = MMU_UNKNOWN;
					break;
				}
			}
		}
	}

	fclose(fp);

	return mmu_mode;
}

void xt_trace_set_mmu_mode(enum rv_mmu_type mode)
{
	xt_recorded_mmu_mode = mode;
}

/*
 * Resolve the MMU mode: prefer the mode recorded in perf.data (correct
 * for offline decode on a different host), fall back to parsing the
 * local /proc/cpuinfo.
 */
static enum rv_mmu_type xt_trace_resolve_mmu_mode(void)
{
	if (xt_recorded_mmu_mode != MMU_UNKNOWN)
		return xt_recorded_mmu_mode;
	return xt_parse_mmu_mode();
}

/* https://www.kernel.org/doc/html/next/riscv/vm-layout.html */
static u8 riscv_get_cpu_mode(u64 vaddr)
{
	static enum rv_mmu_type mmu_mode = MMU_UNKNOWN;
	static bool warned;

	if (mmu_mode == MMU_UNKNOWN)
		mmu_mode = xt_trace_resolve_mmu_mode();

	switch (mmu_mode) {
	case MMU_SV39:
		if (vaddr > 0x3fffffffff) //SV39:256G
			return PERF_RECORD_MISC_KERNEL;
		break;
	case MMU_SV48:
		if (vaddr > 0x7fffffffffff) //SV48:128T
			return PERF_RECORD_MISC_KERNEL;
		break;
	case MMU_SV57:
		if (vaddr > 0xffffffffffffff) //SV57:64P
			return PERF_RECORD_MISC_KERNEL;
		break;
	case MMU_UNKNOWN:
	default:
		if (!warned) {
			warned = true;
			pr_warning("Unknown MMU mode, treating addresses as user\n");
		}
		break;
	}

	return PERF_RECORD_MISC_USER;
}

#define FAILED_TO_GET_AN_INSN 0xabcddeadbeef1234

static uint64_t get_an_insn(struct perf_session *session,
			    struct auxtrace_buffer *buffer, uint64_t addr,
			    uint32_t *len, enum GET_AN_INSN_STATE state, uint64_t addr2)
{
	static struct addr_location al;
	static struct dso *dso;
	static struct thread *thread;
	static u64 start_addr_offset;
	u64 offset;
	uint8_t buf[8] = {0};
	long length;
	u8 cpumode;

	if (state == GET_AN_INSN_FIRST) {
		cpumode = PERF_RECORD_MISC_KERNEL;

		thread__put(thread);
		thread = machine__findnew_thread(&session->machines.host, buffer->pid, buffer->tid);

		cpumode = riscv_get_cpu_mode(addr);

		addr_location__init(&al);
		if (!thread__find_map(thread, cpumode, addr, &al))
			goto error_end;
		dso = map__dso(al.map);
		if (!dso)
			goto error_end;
		if (dso->data.status == DSO_DATA_STATUS_ERROR &&
		    dso__data_status_seen(dso, DSO_DATA_STATUS_SEEN_ITRACE))
			goto error_end;
		offset = map__map_ip(al.map, addr);
		if (cpumode == PERF_RECORD_MISC_KERNEL)
			offset -= (0xffffffff80002000 - 0x3000); // readelf -S vmlinux in .text
		map__load(al.map);
		length = dso__data_read_offset(dso, maps__machine(thread__maps(thread)),
			offset, buf, 2);
		if (length <= 0)
			goto error_end;
		/* insn length */
		*len = riscv_insn_length(buf[0]);
		if (*len > 2) {
			length = dso__data_read_offset(dso, maps__machine(thread__maps(thread)),
					offset + 2, buf + 2, *len - 2);
			if (length <= 0) {
				pr_err("Debug data not found for address %#" PRIx64 " in %s\n",
						addr + 2, dso->long_name ? dso->long_name : "Unknown");
				goto error_end;
			}

			if (length != (*len - 2)) {
				pr_err("Can't get full insn value for address %#" PRIx64 " in %s\n",
					addr, dso->long_name ? dso->long_name : "Unknown");
				goto error_end;
			}
		}

		start_addr_offset = offset;
		return *(uint64_t *)buf;
	} else if (state == GET_AN_INSN_MIDDLE) {
		if (dso == NULL || thread == NULL)
			return FAILED_TO_GET_AN_INSN;
		if (addr2 > addr)
			offset = start_addr_offset + (addr2 - addr);
		else
			offset = start_addr_offset - (addr - addr2);

		length = dso__data_read_offset(dso, maps__machine(thread__maps(thread)), offset, buf, 2);
		if (length <= 0) {
			pr_err("Debug data not found for address %#" PRIx64 " in %s\n",
				addr2, dso->long_name ? dso->long_name : "Unknown");
			goto error_end;
		}

		/* insn length */
		*len = riscv_insn_length(buf[0]);
		if (*len > 2) {
			length = dso__data_read_offset(dso, maps__machine(thread__maps(thread)),
				offset + 2, buf + 2, *len - 2);
			if (length <= 0) {
				pr_err("Debug data not found for address %#" PRIx64 " in %s\n",
					addr2 + 2, dso->long_name ? dso->long_name : "Unknown");
				goto error_end;
			}

			if (length != (*len - 2)) {
				pr_err("Can't get full insn value for address %#" PRIx64 " in %s\n",
					addr2, dso->long_name ? dso->long_name : "Unknown");
				goto error_end;
			}
		}
		return *(uint64_t *)buf;
	} else {
		if (dso == NULL || thread == NULL)
			return FAILED_TO_GET_AN_INSN;
	}

error_end:
	addr_location__exit(&al);
	dso = NULL;
	thread__put(thread);
	thread = NULL;
	return FAILED_TO_GET_AN_INSN;
}

static bool xt_trace_is_condition_branch_insn(uint32_t insn_length,
					      uint32_t insn_value)
{
	if (insn_length == 2) {
		if (RV_IS_C_BEQZ(insn_value) || RV_IS_C_BNEZ(insn_value))
			return true;
	} else {
		if (RV_IS_BEQ(insn_value) || RV_IS_BGE(insn_value) ||
		    RV_IS_BGEU(insn_value) || RV_IS_BLT(insn_value) ||
		    RV_IS_BLTU(insn_value) || RV_IS_BNE(insn_value))
			return true;
	}

	return false;
}

static bool xt_trace_is_special_jump_insn(uint32_t insn_length,
					  uint32_t insn_value)
{
	if (insn_length == 2) {
		if (RV_IS_C_EBREAK(insn_value))
			return true;
	} else {
		if (RV_IS_EBREAK(insn_value) || RV_IS_ECALL(insn_value))
			return true;
	}

	return false;
}

static bool xt_trace_is_ret_insn(uint32_t insn_length, uint32_t insn_value)
{
	if (insn_length == 4) {
		if (RV_IS_MRET(insn_value) || RV_IS_SRET(insn_value) ||
		    RV_IS_URET(insn_value))
			return true;
	}

	return false;
}

static bool xt_trace_is_indirect_jump_insn(uint32_t insn_length,
					   uint32_t insn_value)
{
	if (insn_length == 2) {
		if (RV_IS_C_JALR(insn_value) || RV_IS_C_JR(insn_value) ||
		    RV_IS_C_J(insn_value))
			return true;
	} else {
		if (RV_IS_JAL(insn_value) || RV_IS_JALR(insn_value))
			return true;
	}

	return false;
}

static bool xt_trace_is_inferable_jump(uint32_t insn_length,
				       uint32_t insn_value)
{
	if (insn_length == 2) {
		if (RV_IS_C_J(insn_value) || RV_IS_C_JAL(insn_value))
			return true;
	} else {
		if (RV_IS_JAL(insn_value))
			return true;
	}

	return false;
}

static bool xt_trace_is_indirect_branch_insn(uint32_t insn_length,
					     uint32_t insn_value)
{
	if (xt_trace_is_special_jump_insn(insn_length, insn_value))
		return true;
	if (xt_trace_is_ret_insn(insn_length, insn_value))
		return true;
	if (xt_trace_is_indirect_jump_insn(insn_length, insn_value))
		return true;
	return false;
}

/*************************************************************
 * Proccess I-CNT and HIST
 *************************************************************/
#define INSN_C_CONDITION_BRANCH_GET_IMM(insn)                           \
	(((((insn) >> 2) & 0x1) << 5) + ((((insn) >> 3) & 0x3) << 1) +  \
	 ((((insn) >> 5) & 0x3) << 6) + ((((insn) >> 10) & 0x3) << 3) + \
	 ((((insn) >> 12) & 0x1) << 8))

#define INSN_CONDITION_BRANCH_GET_IMM(insn)                             \
	(((((insn) >> 7) & 0x1) << 11) + ((((insn) >> 8) & 0xf) << 1) + \
	 ((((insn) >> 25) & 0x3f) << 5) + ((((insn) >> 31) & 0x1) << 12))

#define INSN_C_CONTROL_TRANSFER_GET_IMM(insn)                           \
	(((((insn) >> 2) & 0x1) << 5) + ((((insn) >> 3) & 0x7) << 1) +  \
	 ((((insn) >> 6) & 0x1) << 7) + ((((insn) >> 7) & 0x1) << 6) +  \
	 ((((insn) >> 8) & 0x1) << 10) + ((((insn) >> 9) & 0x3) << 8) + \
	 ((((insn) >> 11) & 0x1) << 4) + ((((insn) >> 12) & 0x1) << 11))

#define INSN_CONTROL_TRANSFER_GET_IMM(insn)                                 \
	(((((insn) >> 12) & 0xff) << 12) + ((((insn) >> 20) & 0x1) << 11) + \
	 ((((insn) >> 21) & 0x3ff) << 1) + ((((insn) >> 31) & 0x1) << 20))

#define RESOURCEFULL_MSG_MAX 64
struct resourcefull_message {
	uint32_t rdata0;
	uint32_t hist_count;
	struct resourcefull_message *next;
};

struct hist_resourcefull_message {
	struct resourcefull_message *msg;
	uint32_t total_hist_count;
};

static struct resourcefull_message *icnt_resourcefull_header;
static struct hist_resourcefull_message hist_resourcefull_header;

static void resource_full_clear(void)
{
	if (icnt_resourcefull_header) {
		struct resourcefull_message *tmp = NULL;

		while (icnt_resourcefull_header) {
			tmp = icnt_resourcefull_header->next;
			free(icnt_resourcefull_header);
			icnt_resourcefull_header = tmp;
		}
	}

	if (hist_resourcefull_header.msg) {
		struct resourcefull_message *tmp = NULL;

		while (hist_resourcefull_header.msg) {
			tmp = hist_resourcefull_header.msg->next;
			free(hist_resourcefull_header.msg);
			hist_resourcefull_header.msg = tmp;
		}

		hist_resourcefull_header.total_hist_count = 0;
	}
}

static int32_t add_resource_full_icnt(uint32_t icnt)
{
	struct resourcefull_message *msg =
		(struct resourcefull_message *)malloc(sizeof(struct resourcefull_message));

	if (msg == NULL) {
		pr_err("Failed to allocate i-cnt resource-full message\n");
		return -1;
	}
	msg->rdata0 = icnt;
	msg->next = NULL;

	if (icnt_resourcefull_header) {
		struct resourcefull_message *tmp = icnt_resourcefull_header;

		while (tmp->next)
			tmp = tmp->next;
		tmp->next = msg;
	} else
		icnt_resourcefull_header = msg;

	return 0;
}

static uint64_t get_resource_full_icnt(uint32_t icnt)
{
	uint64_t total_icnt = icnt;
	struct resourcefull_message *tmp = icnt_resourcefull_header;

	while (tmp) {
		total_icnt += tmp->rdata0;
		tmp = tmp->next;
	}

	return total_icnt;
}

static int32_t add_resource_full_hist(uint32_t hist)
{
	uint32_t i = 0;
	struct resourcefull_message *msg = NULL;

	if (hist == 0 || hist == 1)
		return 0;

	msg = (struct resourcefull_message *)malloc(sizeof(struct resourcefull_message));
	if (msg == NULL) {
		pr_err("Failed to allocate hist resource-full message\n");
		return -1;
	}

	for (i = 31; i > 0; i--) {
		if ((1 << i) & hist)
			break;
	}
	msg->hist_count = i;
	msg->rdata0 = hist;
	msg->next = NULL;

	if (hist_resourcefull_header.msg != NULL) {
		struct resourcefull_message *tmp = hist_resourcefull_header.msg;

		while (tmp->next)
			tmp = tmp->next;
		tmp->next = msg;
	} else
		hist_resourcefull_header.msg = msg;

	hist_resourcefull_header.total_hist_count += i;

	return 0;
}

static int32_t get_one_hist(void)
{
	uint32_t condition = 0;
	struct resourcefull_message *tmp = hist_resourcefull_header.msg;

	if (hist_resourcefull_header.total_hist_count == 0) {
		pr_err("No hist bits left for condition branch decoding\n");
		return -1;
	}

	while (tmp) {
		if (tmp->hist_count != 0) {
			condition = (1 << (tmp->hist_count - 1)) & tmp->rdata0;
			tmp->hist_count--;
			hist_resourcefull_header.total_hist_count--;
			return condition ? 1 : 0;
		} else
			tmp = tmp->next;
	}

	pr_err("No hist bits left for condition branch decoding\n");
	return -1;
}

static uint32_t get_hist_count(void)
{
	return hist_resourcefull_header.total_hist_count;
}
