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

static int32_t
xt_trace_analyze_i_cnt_vs_hist(struct perf_session *session,
			       struct auxtrace_buffer *buffer, uint32_t i_cnt,
			       uint64_t start_addr, uint32_t hist,
			       struct xt_trace_address_range **range)
{
	int32_t ret = -1;
	uint64_t insn_addr = start_addr;
	uint32_t insn_len = 0;
	uint64_t insn_value = 0;
	struct xt_trace_address_range *range_last_p = NULL;
	int64_t insn_cnt = 0;
	bool get_range_end = false;
	enum GET_AN_INSN_STATE state = GET_AN_INSN_FIRST;
	struct resourcefull_message *icnt_resource_full = icnt_resourcefull_header;
	int64_t icnt_resource_full_rdata0 = icnt_resource_full ? icnt_resource_full->rdata0 : 0;
	struct xt_trace_msg extra;

	if (add_resource_full_hist(hist))
		return -1;
	insn_cnt = get_resource_full_icnt(i_cnt);

	// If only hist == 0, we can't just set end_addr = start_addr + i_cnt * 2
	// as the i_cnt may contain c.j, c.jal or jal

	(*range) = (struct xt_trace_address_range *)zalloc(
		sizeof(struct xt_trace_address_range));
	if ((*range) == NULL) {
		pr_err("Failed to allocate address range\n");
		return -1;
	}
	(*range)->start_addr = start_addr;
	(*range)->end_addr = start_addr;
	(*range)->next = NULL;
	range_last_p = *range;

	while (insn_cnt) {
		insn_value = get_an_insn(session, buffer, start_addr, &insn_len, state, insn_addr);
		if (insn_value == FAILED_TO_GET_AN_INSN) {
			ret = 1;
			goto error_end;
		}
		state = GET_AN_INSN_MIDDLE;

		if (xt_trace_is_condition_branch_insn(insn_len, insn_value)) {
			int32_t condition = 0;

			/*
			 * If the last insn is a condition branch in message ProgramCorrection,
			 * HIST may records or not records the condition.
			 */
			if (insn_cnt == (insn_len/2)) {
				if (get_hist_count())
					condition = get_one_hist();
				else
					condition = false;
			} else {
				// get one hist
				condition = get_one_hist();
				if (condition < 0) {
					if (insn_cnt == (insn_len/2))
						condition = false;
					else
						goto error_end;
				}
			}

			// if condition == true, create new address range
			if (condition != 0) {
				uint64_t new_addr = 0;
				struct xt_trace_address_range *range_tmp = NULL;

				if (insn_len == 2 &&
				    (RV_IS_C_BEQZ(insn_value) ||
				     RV_IS_C_BNEZ(insn_value))) {
					if (INSN_C_CONDITION_BRANCH_GET_IMM(
						    insn_value) >>
					    8)
						new_addr =
							insn_addr -
							((((~INSN_C_CONDITION_BRANCH_GET_IMM(
								   insn_value)) &
							   0xff) +
							  1) &
							 0xff);
					else
						new_addr =
							insn_addr +
							(INSN_C_CONDITION_BRANCH_GET_IMM(
								 insn_value) &
							 0xff);
				} else if (insn_len == 4 &&
					   (RV_IS_BEQ(insn_value) ||
					    RV_IS_BGE(insn_value) ||
					    RV_IS_BGEU(insn_value) ||
					    RV_IS_BLT(insn_value) ||
					    RV_IS_BLTU(insn_value) ||
					    RV_IS_BNE(insn_value))) {
					if (INSN_CONDITION_BRANCH_GET_IMM(
						    insn_value) >>
					    12)
						new_addr =
							insn_addr -
							((((~INSN_CONDITION_BRANCH_GET_IMM(
								   insn_value)) &
							   0xfff) +
							  1) &
							 0xfff);
					else
						new_addr =
							insn_addr +
							(INSN_CONDITION_BRANCH_GET_IMM(
								 insn_value) &
							 0xfff);
				} else {
					pr_err("insn value 0x%lx at addr 0x%lx is not a condition branch insn\n",
						insn_value, insn_addr);
					goto error_end;
				}

				// set range_last_p->end_addr to insn_addr and add
				// range_tmp to the range_last_p, then set range_last_p
				// to range_tmp
				// set range_last_p->end_addr to insn_addr
				range_last_p->end_addr = insn_addr + insn_len;

				//add extra message
				if (xt_with_extra_msg && icnt_resource_full) {
					int64_t icnt_tmp = (range_last_p->end_addr - range_last_p->start_addr)/2;

					if (icnt_resource_full_rdata0 > icnt_tmp)
						icnt_resource_full_rdata0 -= icnt_tmp;
					else {
						struct xt_trace_address_range *range_tmp1 = NULL;

						range_tmp1 = xt_trace_alloc_extra_msg_range();
						if (!range_tmp1) {
							pr_err("Failed to allocate extra message range\n");
							goto error_end;
						}
						extra = XT_TRACE_MSG(range_tmp1->msg, XT_EXTRA_MSG_SIZE);

						if (icnt_resource_full_rdata0 == icnt_tmp) {
							xt_trace_msg_append(&extra,
									    "i-cnt resource full 0x%x with pc 0x%lx(end).\n",
									    icnt_resource_full->rdata0,
									    range_last_p->end_addr);
							icnt_resource_full = icnt_resource_full->next;
							if (icnt_resource_full)
								icnt_resource_full_rdata0 = (icnt_resource_full_rdata0 + icnt_resource_full->rdata0) - icnt_tmp;
							else
								icnt_resource_full_rdata0 = 0;
						} else {
							int64_t tmp_pc = range_last_p->start_addr + (icnt_resource_full_rdata0 * 2);

							icnt_tmp = icnt_tmp - icnt_resource_full_rdata0;

							while (1) {
								xt_trace_msg_append(&extra,
										    "i-cnt resource full 0x%x with pc 0x%lx, ",
										    icnt_resource_full->rdata0, tmp_pc);

								icnt_resource_full = icnt_resource_full->next;
								if (icnt_resource_full)
									icnt_resource_full_rdata0 = icnt_resource_full->rdata0;
								else {
									icnt_resource_full_rdata0 = 0;
									break;
								}

								if (icnt_resource_full_rdata0 <= icnt_tmp) {
									tmp_pc = tmp_pc + icnt_resource_full_rdata0 * 2;
									icnt_tmp -= icnt_resource_full_rdata0;
								} else {
									icnt_resource_full_rdata0 -= icnt_tmp;
									break;
								}
							}

							xt_trace_msg_append(&extra, "(in previous)\n");

						}

						range_tmp1->next = NULL;

						// add range_tmp1 to last
						range_last_p->next = range_tmp1;
						range_last_p = range_tmp1;
					}
				}

				/* If this condition branch insn is the last excuted
				 * no need to add range_tmp any more.
				 */
				if (insn_cnt == (insn_len/2)) {
					range_last_p->next = NULL;
					get_range_end = true;
				} else {
					// get new address range and add range_tmp to the range_last_p
					// then set range_last_p to range_tmp
					range_tmp = (struct xt_trace_address_range *)zalloc(sizeof(struct xt_trace_address_range));
					if (range_tmp == NULL) {
						// msg
						goto error_end;
					}
					range_tmp->start_addr = new_addr;
					range_tmp->end_addr = new_addr;
					range_tmp->next = NULL;

					// add range_tmp to last
					range_last_p->next = range_tmp;
					range_last_p = range_tmp;
				}

				// update insn_addr
				insn_addr = new_addr;
			} else
				insn_addr += insn_len;
		} else if (xt_trace_is_indirect_branch_insn(insn_len,
							    insn_value)) {
			if (xt_trace_is_inferable_jump(insn_len, insn_value)) {
				uint64_t new_addr = 0;
				struct xt_trace_address_range *range_tmp = NULL;

				if (insn_len == 2) {
					if (INSN_C_CONTROL_TRANSFER_GET_IMM(
						    insn_value) >>
					    11)
						new_addr =
							insn_addr -
							((((~INSN_C_CONTROL_TRANSFER_GET_IMM(
								   insn_value)) &
							   0x7ff) +
							  1) &
							 0x7ff);
					else
						new_addr =
							insn_addr +
							(INSN_C_CONTROL_TRANSFER_GET_IMM(
								 insn_value) &
							 0x7ff);
				} else {
					if (INSN_CONTROL_TRANSFER_GET_IMM(
						    insn_value) >>
					    20)
						new_addr =
							insn_addr -
							((((~INSN_CONTROL_TRANSFER_GET_IMM(
								   insn_value)) &
							   0xfffff) +
							  1) &
							 0xfffff);
					else
						new_addr =
							insn_addr +
							(INSN_CONTROL_TRANSFER_GET_IMM(
								 insn_value) &
							 0xfffff);
				}

				// set range_last_p->end_addr to insn_addr
				range_last_p->end_addr = insn_addr + insn_len;

				//add extra message
				if (xt_with_extra_msg && icnt_resource_full) {
					int64_t icnt_tmp = (range_last_p->end_addr - range_last_p->start_addr)/2;

					if (icnt_resource_full_rdata0 > icnt_tmp)
						icnt_resource_full_rdata0 -= icnt_tmp;
					else {
						struct xt_trace_address_range *range_tmp1 = NULL;

						range_tmp1 = xt_trace_alloc_extra_msg_range();
						if (!range_tmp1) {
							pr_err("Failed to allocate extra message range\n");
							goto error_end;
						}
						extra = XT_TRACE_MSG(range_tmp1->msg, XT_EXTRA_MSG_SIZE);

						if (icnt_resource_full_rdata0 == icnt_tmp) {
							xt_trace_msg_append(&extra,
									    "i-cnt resource full 0x%x with pc 0x%lx(end).\n",
									    icnt_resource_full->rdata0,
									    range_last_p->end_addr);
							icnt_resource_full = icnt_resource_full->next;
							if (icnt_resource_full)
								icnt_resource_full_rdata0 = (icnt_resource_full_rdata0 + icnt_resource_full->rdata0) - icnt_tmp;
							else
								icnt_resource_full_rdata0 = 0;
						} else {
							int64_t tmp_pc = range_last_p->start_addr + (icnt_resource_full_rdata0 * 2);

							icnt_tmp = icnt_tmp - icnt_resource_full_rdata0;

							while (1) {
								xt_trace_msg_append(&extra,
										    "i-cnt resource full 0x%x with pc 0x%lx, ",
										    icnt_resource_full->rdata0, tmp_pc);

								icnt_resource_full = icnt_resource_full->next;
								if (icnt_resource_full)
									icnt_resource_full_rdata0 = icnt_resource_full->rdata0;
								else {
									icnt_resource_full_rdata0 = 0;
									break;
								}

								if (icnt_resource_full_rdata0 <= icnt_tmp) {
									tmp_pc = tmp_pc + icnt_resource_full_rdata0 * 2;
									icnt_tmp -= icnt_resource_full_rdata0;
								} else {
									icnt_resource_full_rdata0 -= icnt_tmp;
									break;
								}
							}

							xt_trace_msg_append(&extra, "(in previous)\n");
						}
						range_tmp1->next = NULL;

						// add range_tmp1 to last
						range_last_p->next = range_tmp1;
						range_last_p = range_tmp1;
					}
				}

				/* If this indirect insn is the last excuted,
				 * no need to add range_tmp any more.
				 */
				if (insn_cnt == (insn_len/2)) {
					range_last_p->next = NULL;
					get_range_end = true;
				} else {
					// get new address range
					range_tmp = (struct xt_trace_address_range *)zalloc(sizeof(struct xt_trace_address_range));
					if (range_tmp == NULL) {
						// msg
						pr_err("Failed to allocate address range\n");
						goto error_end;
					}
					range_tmp->start_addr = new_addr;
					range_tmp->end_addr = new_addr;
					range_tmp->next = NULL;

					// add range_tmp to the range_last_p, then set range_last_p
					// to range_tmp
					range_last_p->next = range_tmp;
					range_last_p = range_tmp;
				}

				// update insn_addr
				insn_addr = new_addr;
			} else {
				if (insn_cnt != insn_len / 2) {
					if (xt_with_extra_msg) {
						struct xt_trace_address_range *range_tmp1 = NULL;

						range_tmp1 = xt_trace_alloc_extra_msg_range();
						if (!range_tmp1) {
							pr_err("Failed to allocate extra message range\n");
							goto error_end;
						}
						extra = XT_TRACE_MSG(range_tmp1->msg, XT_EXTRA_MSG_SIZE);
						xt_trace_msg_append(&extra,
								    "I-cnt error start_addr 0x%lx, hist 0x%x, i-cnt 0x%x, insn_cnt %ld, insn_len %d, insn_addr 0x%lx.\n",
								    start_addr, hist, i_cnt, insn_cnt,
								    insn_len, insn_addr);

						// add range_tmp1 to range list
						range_last_p->next = range_tmp1;
						range_last_p = range_tmp1;
					} else {
						pr_err("I-cnt decoding error\n");
					}

					goto error_end;
				}

				insn_addr += insn_len;
			}
		} else
			insn_addr += insn_len;

		insn_cnt -= insn_len / 2;
		if (insn_cnt == 0)
			break;
		else if (insn_cnt < 0) {
			// msg
			if (xt_with_extra_msg) {
				struct xt_trace_address_range *range_tmp1 = NULL;

				range_tmp1 = xt_trace_alloc_extra_msg_range();
				if (!range_tmp1) {
					pr_err("Failed to allocate extra message range\n");
					goto error_end;
				}
				extra = XT_TRACE_MSG(range_tmp1->msg, XT_EXTRA_MSG_SIZE);
				xt_trace_msg_append(&extra,
						    "I-cnt2 error start_addr 0x%lx, hist 0x%x, i-cnt 0x%x, insn_cnt %ld, insn_len %d, insn_addr 0x%lx.\n",
						    start_addr, hist, i_cnt, insn_cnt, insn_len,
						    insn_addr - insn_len);

				//add range_tmp1 to range list
				range_last_p->next = range_tmp1;
				range_last_p = range_tmp1;
			} else {
				pr_err("I-cnt decoding error (count underflow)\n");
			}
			goto error_end;
		}
	}

	if (get_hist_count()) {
		// msg
		if (xt_with_extra_msg) {
			struct xt_trace_address_range *range_tmp1 = NULL;
			range_tmp1 = xt_trace_alloc_extra_msg_range();
			if (!range_tmp1) {
				pr_err("Failed to allocate extra message range\n");
				goto error_end;
			}
			extra = XT_TRACE_MSG(range_tmp1->msg, XT_EXTRA_MSG_SIZE);
			xt_trace_msg_append(&extra,
					    "Hist error start_addr 0x%lx, hist 0x%x, i-cnt 0x%x, insn_cnt %ld, insn_len %d, insn_addr 0x%lx hist_left %d.\n",
					    start_addr, hist, i_cnt, insn_cnt, insn_len,
					    insn_addr - insn_len, get_hist_count());

			//add range_tmp1 to range list
			range_last_p->next = range_tmp1;
			range_last_p = range_tmp1;
		} else {
			pr_err("Hist decoding error (leftover hist bits)\n");
		}
		goto error_end;
	}

	if (!get_range_end)
		range_last_p->end_addr = insn_addr;

	get_an_insn(NULL, NULL, 0, NULL, GET_AN_INSN_END, 0);

	return 0;

error_end:
	// free range
	if (!xt_with_extra_msg) {
		while (*range) {
			struct xt_trace_address_range *range_tmp = *range;

			*range = (*range)->next;
			free(range_tmp);
		}
	}

	get_an_insn(NULL, NULL, 0, NULL, GET_AN_INSN_END, 0);

	return ret;
}

/*************************************************************
 * Build struct xt_trace_program_flow_node
 *************************************************************/
static struct xt_trace_program_flow_node *trace_program_header;

/* Per-src context: when src_bits > 0 and src != 0, each source keeps
 * its own node list and decode state so streams from different CPUs
 * are processed independently.
 */
struct xt_src_ctx {
	uint32_t src;
	struct xt_trace_program_flow_node *header;
	struct xt_trace_program_flow_node *tail;
	bool error_message_happened;
	uint64_t last_full_addr;
	uint64_t repeat_count;
	bool has_ownership;
	struct xt_riscv_nexus_trace_message ownership_msg;
	struct xt_src_ctx *next;
};

static struct xt_src_ctx *src_ctx_list;

static struct xt_src_ctx *xt_src_ctx_get(uint32_t src)
{
	struct xt_src_ctx *ctx = src_ctx_list;

	while (ctx) {
		if (ctx->src == src)
			return ctx;
		ctx = ctx->next;
	}

	ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		return NULL;
	ctx->src = src;
	ctx->error_message_happened = true;
	ctx->next = src_ctx_list;
	src_ctx_list = ctx;
	return ctx;
}

/* forward declaration */
static void xt_trace_free_program_flow_node(void);

static void xt_trace_free_node_list(struct xt_trace_program_flow_node *header)
{
	struct xt_trace_program_flow_node *node_p = header;
	struct xt_trace_address_range *range_p = NULL;

	while (node_p) {
		struct xt_trace_program_flow_node *next = node_p->next;

		while (node_p->addr_range) {
			range_p = node_p->addr_range;
			node_p->addr_range = node_p->addr_range->next;
			if (range_p->extra_msg == 1)
				free(range_p->msg);
			free(range_p);
		}

		free(node_p);
		node_p = next;
	}
}

static void xt_src_ctx_free_all(void)
{
	struct xt_src_ctx *ctx = src_ctx_list;

	while (ctx) {
		struct xt_src_ctx *next = ctx->next;

		if (ctx->header)
			xt_trace_free_node_list(ctx->header);
		free(ctx);
		ctx = next;
	}
	src_ctx_list = NULL;
}

static struct xt_trace_program_flow_node *
xt_trace_malloc_program_flow_node(void)
{
	struct xt_trace_program_flow_node *node =
		(struct xt_trace_program_flow_node *)malloc(
			sizeof(struct xt_trace_program_flow_node));

	if (node)
		memset(node, 0, sizeof(struct xt_trace_program_flow_node));

	return node;
}



static bool xt_trace_msg_has_full_addr(struct xt_riscv_nexus_trace_message *msg)
{
	if (msg->tcode == TCODE_PROGTRACESYNC ||
	    msg->tcode == TCODE_DIRECTBRANCHSYNC ||
	    msg->tcode == TCODE_INDIRECTBRANCHSYNC ||
	    msg->tcode == TCODE_INDIRECTBRANCHHISTSYNC)
		return true;

	return false;
}

static void xt_trace_free_program_flow_node(void)
{
	xt_trace_free_node_list(trace_program_header);
	trace_program_header = NULL;
}

static void xt_src_ctx_add_node(struct xt_src_ctx *ctx,
				struct xt_trace_program_flow_node *node)
{
	node->prev = ctx->tail;
	node->next = NULL;
	if (ctx->tail)
		ctx->tail->next = node;
	else
		ctx->header = node;
	ctx->tail = node;
}

static void xt_src_ctx_flush(struct xt_src_ctx *ctx,
			     struct perf_session *session,
			     struct auxtrace_buffer *buffer,
			     bool with_msg, bool with_addr, bool with_insn)
{
	if (!ctx->header)
		return;
	trace_program_header = ctx->header;
	xuantie_ntrace_decoder__process_full_message(
		session, buffer, with_addr,
		ctx->last_full_addr, &ctx->last_full_addr);
	xt_trace_program_trace_display(with_msg, with_addr, with_insn);
	xt_trace_free_program_flow_node();
	ctx->header = NULL;
	ctx->tail = NULL;
}

/*************************************************************
 * Process U-addr and FULL-addr
 *************************************************************/
static void xt_trace_resolve_addr(struct thread *thread, u64 addr,
				  char *dso_out, size_t dso_size,
				  char *sym_out, size_t sym_size)
{
	struct addr_location al;
	u8 cpumode = riscv_get_cpu_mode(addr);
	struct symbol *sym;
	struct dso *dso;

	dso_out[0] = '\0';
	sym_out[0] = '\0';

	addr_location__init(&al);
	sym = thread__find_symbol_fb(thread, cpumode, addr, &al);
	if (sym && sym->name[0] != '\0')
		scnprintf(sym_out, sym_size, "%s", sym->name);

	if (al.map) {
		dso = map__dso(al.map);
		if (dso && dso->long_name && dso->long_name[0] != '\0')
			scnprintf(dso_out, dso_size, "%s", dso->long_name);
	}
	addr_location__exit(&al);
}

static void xt_trace_get_dso_and_symbol_for_node(
	struct xt_trace_program_flow_node *node,
	struct perf_session *session, struct auxtrace_buffer *buffer)
{
	struct thread *thread;

	thread = machine__findnew_thread(&session->machines.host, buffer->pid, buffer->tid);
	if (!thread)
		return;

	xt_trace_resolve_addr(thread, node->start_addr,
			      node->saddr_dso, sizeof(node->saddr_dso),
			      node->saddr_sym, sizeof(node->saddr_sym));

	if (node->full_addr == node->start_addr) {
		memcpy(node->faddr_dso, node->saddr_dso,
		       sizeof(node->faddr_dso));
		memcpy(node->faddr_sym, node->saddr_sym,
		       sizeof(node->faddr_sym));
	} else {
		xt_trace_resolve_addr(thread, node->full_addr,
			      node->faddr_dso, sizeof(node->faddr_dso),
			      node->faddr_sym, sizeof(node->faddr_sym));
	}

	thread__put(thread);
}

static int32_t
xt_trace_get_start_and_full_addr(struct xt_trace_program_flow_node *node,
	struct perf_session *session, struct auxtrace_buffer *buffer)
{
	bool unknown_tcode = false;
	static enum rv_mmu_type mmu_mode = MMU_UNKNOWN;
	static int rv_svmode_int = 39;

	if (mmu_mode == MMU_UNKNOWN) {
		mmu_mode = xt_trace_resolve_mmu_mode();
		if (mmu_mode == MMU_SV48)
			rv_svmode_int = 48;
		else if (mmu_mode == MMU_SV57)
			rv_svmode_int = 57;
		else
			rv_svmode_int = 39; //default
	}

	switch (node->msg.tcode) {
	case TCODE_DIRECTBRANCH:
		node->full_addr = node->start_addr +
			get_resource_full_icnt(node->msg.sub_value.directbranch.i_cnt) * 2;
		break;
		// u-addr
	case TCODE_INDIRECTBRANCH:
		node->full_addr =
			node->start_addr ^
			(node->msg.sub_value.indirectbranch.u_addr * 2);
		break;
	case TCODE_INDIRECTBRANCHHIST:
		node->full_addr =
			node->start_addr ^
			(node->msg.sub_value.indirectbranchhist.u_addr * 2);
		break;
		// f-addr
	case TCODE_PROGTRACESYNC:
		node->full_addr = node->msg.sub_value.progtracesync.f_addr * 2;
		break;
	case TCODE_DIRECTBRANCHSYNC:
		node->full_addr =
			node->msg.sub_value.directbranchsync.f_addr * 2;
		break;
	case TCODE_INDIRECTBRANCHSYNC:
		node->full_addr =
			node->msg.sub_value.indirectbranchsync.f_addr * 2;
		break;
	case TCODE_INDIRECTBRANCHHISTSYNC:
		node->full_addr =
			node->msg.sub_value.indirectbranchhistsync.f_addr * 2;
		break;
	case TCODE_RESOURCEFULL:
		pr_err("Unexpected full-address resolution for TCODE_RESOURCEFULL\n");
		return -1;
	case TCODE_PROGTRACECORRELATION: {
		struct xt_trace_address_range *range = node->addr_range;

		if (!node->addr_range) {
			node->full_addr = 0xffffffffffffffff;
			return 0;
		}

		while (range->next)
			range = range->next;
		node->full_addr = range->end_addr;
	} break;
	default:
		unknown_tcode = true;
		break;
	}

	node->full_addr = sign_extend64(node->full_addr, rv_svmode_int - 1);
	if (!unknown_tcode)
		xt_trace_get_dso_and_symbol_for_node(node, session, buffer);

	return 0;
}

int32_t xuantie_ntrace_decoder__process_metadata(
	struct xuantie_saved_config *saved_config, unsigned char *buf,
	size_t len, struct perf_session *session,
	struct auxtrace_buffer *buffer, bool with_msg, bool with_addr,
	bool with_insn)
{
	int32_t ret = 0;
	struct xt_riscv_nexus_trace_message msg;
	struct xt_trace_program_flow_node *node = NULL;
	struct xt_src_ctx *ctx = NULL;
	uint32_t trace_data_wrapped = saved_config->trace_ram_wrap;
	uint32_t wrap_used = 1;

	xt_trace_mseo_mdo_init(buf, len);

	// Each message is routed to a per-src context.
	// When src_bits==0, msg.src is always 0 so there is only one context.
	// When src_bits!=0, each src value gets its own independent context.

	while (!XT_PERF_GET_CTRL_C()) {
		// If repeat_count !=0, not update msg
		if (ctx && ctx->repeat_count) {
			ctx->repeat_count--;
		} else {
			ret = xt_trace_analyze_message_field(
				&msg, trace_data_wrapped, wrap_used,
				saved_config->src_bits,
				saved_config->timestamp_bits);
			trace_data_wrapped = 0;
			wrap_used = 0;
			if (ret > 0) {
				/* End of trace data: break and let end-of-function flush handle it */
				break;
			} else if (ret < 0) {
				// msg: any error happens
				pr_err("Failed to parse the next trace message\n");
				goto error_end;
			}
		}

		ctx = xt_src_ctx_get(msg.src);
		if (!ctx) {
			pr_err("Failed to allocate src context\n");
			goto error_end;
		}

		// If true, we should find a message with full address
		if (ctx->error_message_happened) {
			if (!xt_trace_msg_has_full_addr(&msg))
				continue;
			ctx->error_message_happened = false;
		}

		/*
		 * When a full-addr (SYNC) message arrives, flush the existing
		 * segment first, then add this SYNC node as the start of the
		 * next segment. This guarantees:
		 *   - every segment starts with exactly one SYNC node
		 *   - last_full_addr is passed across flush boundaries so SYNC
		 *     nodes with non-zero i-cnt/hist are processed correctly
		 */
		if (xt_trace_msg_has_full_addr(&msg) && ctx->header)
			xt_src_ctx_flush(ctx, session, buffer,
					 with_msg, with_addr, with_insn);

		// malloc space
		node = xt_trace_malloc_program_flow_node();
		if (node == NULL) {
			pr_err("Failed to allocate program flow node\n");
			goto error_end;
		}
		memcpy(&node->msg, &msg, sizeof(msg));

		switch (msg.tcode) {
		case TCODE_OWNERSHIP:
			ctx->has_ownership = true;
			memcpy(&ctx->ownership_msg, &msg, sizeof(msg));
			break;
		case TCODE_DIRECTBRANCH:
		case TCODE_INDIRECTBRANCH:
		case TCODE_PROGTRACESYNC:
		case TCODE_DIRECTBRANCHSYNC:
		case TCODE_INDIRECTBRANCHSYNC:
		case TCODE_RESOURCEFULL:
		case TCODE_INDIRECTBRANCHHIST:
		case TCODE_INDIRECTBRANCHHISTSYNC:
			node->ownership = ctx->has_ownership ? &ctx->ownership_msg : NULL;
			break;
		case TCODE_PROGTRACECORRELATION:
			/* PROGTRACECORRELATION ends a trace run; flush the current
			 * segment including this node, then wait for a new SYNC.
			 */
			node->ownership = ctx->has_ownership ? &ctx->ownership_msg : NULL;
			xt_src_ctx_add_node(ctx, node);
			xt_src_ctx_flush(ctx, session, buffer,
					 with_msg, with_addr, with_insn);
			ctx->last_full_addr = 0;
			ctx->error_message_happened = true;
			continue;
		case TCODE_ERROR:
			/* Keep existing nodes, add error node to list, then
			 * flush so data between last SYNC and this error is
			 * not lost. Next segment waits for a new SYNC.
			 */
			node->ownership = ctx->has_ownership ? &ctx->ownership_msg : NULL;
			xt_src_ctx_add_node(ctx, node);
			xt_src_ctx_flush(ctx, session, buffer,
					 with_msg, with_addr, with_insn);
			ctx->last_full_addr = 0;
			ctx->error_message_happened = true;
			if (msg.sub_value.error.etype == ERROR_MSG_ETYPE_STANDARD &&
				(msg.sub_value.error.ecode & ERROR_MSG_ECODE_OWNERSHIP))
				ctx->has_ownership = false;
			continue;
		case TCODE_REPEATBRANCH:
			{
				struct xt_trace_program_flow_node *node_p = ctx->tail;

				if (node_p) {
					memcpy(&msg, &node_p->msg, sizeof(msg));
					ctx->repeat_count = min_t(uint64_t,
						msg.sub_value.repeatbranch.b_cnt,
						XT_MAX_REPEAT_COUNT);
				} else {
					ctx->last_full_addr = 0;
					ctx->error_message_happened = true;
				}
				free(node);
				continue;
			}
		default:
			goto error_end;
		}

		// add node to program_flow_header
		xt_src_ctx_add_node(ctx, node);
	}

	/* Flush all contexts (one per src value, or just src=0 if src_bits==0) */
	{
		struct xt_src_ctx *flush_ctx = src_ctx_list;

		while (flush_ctx) {
			xt_src_ctx_flush(flush_ctx, session, buffer,
					 with_msg, with_addr, with_insn);
			flush_ctx = flush_ctx->next;
		}
		xt_src_ctx_free_all();
	}

	return 0;

error_end:
	xt_src_ctx_free_all();
	return -1;
}

/*************************************************************
 * Display NODE(msg)
 *************************************************************/
static int32_t
xt_trace_output_ntrace_message(struct xt_riscv_nexus_trace_message *msg,
			       char *buf)
{
	struct xt_trace_msg mbuf = XT_TRACE_MSG(buf, STR_LEN_MAX);
	const char *priv_mode_str[4] = { "U-mode", "S-mode", "Unknown-mode",
					 "M-mode" };
	const char *b_type_str[4] = { "IndirectBranch",
				      "Exception or Interrupt", "Exception",
				      "Interrupt" };
	const char *sync_str[16] = { "External Trace Trigger",
				     "Exit From Reset",
				     "Periodic Synchronization",
				     "Exit from Debug Mode",
				     "Sequential Instruction Counter",
				     "Trace Enable",
				     "Trace Event",
				     "Restart from FIFO",
				     "Reserved8",
				     "Exit from Powerdown",
				     "Reserved10",
				     "Reserved11",
				     "Reserved12",
				     "Reserved13",
				     "Reserved14",
				     "Reserved15" };
	const char *evcode_str[16] = {
		"Entry into Debug Mode",
		"Entry into Low-power Mode",
		"Reserved2",
		"Reserved3",
		"Program Trace Disabled",
		"Reserved5",
		"Reserved6",
		"Reserved7",
		"Reserved8",
		"Reserved9",
		"Reserved10",
		"Reserved11",
		"Reserved12",
		"Reserved13",
		"Reserved14",
		"Reserved15"
	};

	xt_trace_msg_append(&mbuf, ".  ");

	switch (msg->tcode) {
		case TCODE_OWNERSHIP: {
			xt_trace_msg_append(&mbuf, "TCODE=2(OwnerShip)");
			xt_trace_msg_append(&mbuf, ", Privilege is %s%s",
					    msg->sub_value.ownership.is_virtual ? "V" : "",
					    priv_mode_str[msg->sub_value.ownership.privilege & 0x3]);
			if (msg->sub_value.ownership.has_context)
				xt_trace_msg_append(&mbuf, ", %sContext = 0x%lx",
						    msg->sub_value.ownership.is_scontext ?
						    "S" :
						    "H",
						    msg->sub_value.ownership.context);
		} break;
		case TCODE_DIRECTBRANCH: {
			xt_trace_msg_append(&mbuf, "TCODE=3(DIRECTBRANCH)");
			xt_trace_msg_append(&mbuf, ", i-cnt=0x%lx",
					    msg->sub_value.directbranch.i_cnt);
		} break;
		case TCODE_INDIRECTBRANCH: {
			xt_trace_msg_append(&mbuf, "TCODE=4(INDIRECTBRANCH)");
			xt_trace_msg_append(&mbuf, ", b_type=%d(%s), i-cnt=0x%lx, u-addr=0x%lx",
					    msg->sub_value.indirectbranch.b_type,
					    b_type_str[msg->sub_value.indirectbranch.b_type & 0x3],
					    msg->sub_value.indirectbranch.i_cnt,
					    msg->sub_value.indirectbranch.u_addr);
		} break;
		case TCODE_ERROR: {
			xt_trace_msg_append(&mbuf, "TCODE=8(ERROR)");
			xt_trace_msg_append(&mbuf, "etype=%d(%s), ecode=0x%lx",
					    msg->sub_value.error.etype,
					    msg->sub_value.error.etype == 0 ? "Standard" :
					    "Reserved",
					    msg->sub_value.error.ecode);
			if (msg->sub_value.error.etype == 0 &&
				msg->sub_value.error.ecode) {
				xt_trace_msg_append(&mbuf, "(");
				if (msg->sub_value.error.ecode & (1 << 0))
					xt_trace_msg_append(&mbuf, "[0]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 1))
					xt_trace_msg_append(&mbuf, ", [1]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 2))
					xt_trace_msg_append(&mbuf, ", [2]:Program Trace Message(s) lost");
				if (msg->sub_value.error.ecode & (1 << 3))
					xt_trace_msg_append(&mbuf, ", [3]:Ownership Trace Message(s) lost");
				if (msg->sub_value.error.ecode & (1 << 4))
					xt_trace_msg_append(&mbuf, ", [4]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 5))
					xt_trace_msg_append(&mbuf, ", [5]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 6))
					xt_trace_msg_append(&mbuf, ", [6]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 7))
					xt_trace_msg_append(&mbuf, ", [7]:Vendor Defined Message(s) lost");
				xt_trace_msg_append(&mbuf, "),");
			} else if (msg->sub_value.error.etype == 0) {
				xt_trace_msg_append(&mbuf, "(Exact reason unknown/not provided)");
			}
		} break;
		case TCODE_PROGTRACESYNC: {
			xt_trace_msg_append(&mbuf, "TCODE=9(PROGTRACESYNC)");
			xt_trace_msg_append(&mbuf, ", sync=%d(%s), i-cnt=0x%lx, f_addr=0x%lx",
					    msg->sub_value.progtracesync.sync,
					    sync_str[msg->sub_value.progtracesync.sync & 0xf],
					    msg->sub_value.progtracesync.i_cnt,
					    msg->sub_value.progtracesync.f_addr);
		} break;
		case TCODE_DIRECTBRANCHSYNC: {
			xt_trace_msg_append(&mbuf, "TCODE=11(DIRECTBRANCHSYNC)");
			xt_trace_msg_append(&mbuf, ", sync=%d(%s), i-cnt=0x%lx, f_addr=0x%lx",
					    msg->sub_value.directbranchsync.sync,
					    sync_str[msg->sub_value.directbranchsync.sync & 0xf],
					    msg->sub_value.directbranchsync.i_cnt,
					    msg->sub_value.directbranchsync.f_addr);
		} break;
		case TCODE_INDIRECTBRANCHSYNC: {
			xt_trace_msg_append(&mbuf, "TCODE=12(INDIRECTBRANCHSYNC)");
			xt_trace_msg_append(&mbuf, ", sync=%d(%s), b-type=%d(%s), i-cnt=0x%lx, f_addr=0x%lx",
					    msg->sub_value.indirectbranchsync.sync,
					    sync_str[msg->sub_value.indirectbranchsync.sync & 0xf],
					    msg->sub_value.indirectbranchsync.b_type,
					    b_type_str[msg->sub_value.indirectbranchsync.b_type &
					    0x3],
					    msg->sub_value.indirectbranchsync.i_cnt,
					    msg->sub_value.indirectbranchsync.f_addr);
		} break;
		case TCODE_RESOURCEFULL: {
			xt_trace_msg_append(&mbuf, "TCODE=27(RESOURCEFULL)");
			xt_trace_msg_append(&mbuf, ", rcode=%d",
					    msg->sub_value.resourcefull.rcode);
			if (msg->sub_value.resourcefull.rcode == 0)
				xt_trace_msg_append(&mbuf, ", rdata0=0x%lx(i-cnt)",
						    msg->sub_value.resourcefull.rdata0);
			else if (msg->sub_value.resourcefull.rcode == 1)
				xt_trace_msg_append(&mbuf, ", rdata0=0x%lx(hist)",
						    msg->sub_value.resourcefull.rdata0);
			else if (msg->sub_value.resourcefull.rcode == 2)
				xt_trace_msg_append(&mbuf, ", rdata0=0x%lx(hist), rdata1=0x%lx(repeated count)",
						    msg->sub_value.resourcefull.rdata0,
						    msg->sub_value.resourcefull.rdata1);
			else
				xt_trace_msg_append(&mbuf, ", rdata0=0x%lx(unknown), rdata1=0x%lx(unknown)",
						    msg->sub_value.resourcefull.rdata0,
						    msg->sub_value.resourcefull.rdata1);
		} break;
		case TCODE_INDIRECTBRANCHHIST: {
			xt_trace_msg_append(&mbuf, "TCODE=28(INDIRECTBRANCHHIST)");
			xt_trace_msg_append(&mbuf, ", b-type=%d(%s), i-cnt=0x%lx, u_addr=0x%lx, hist=0x%lx",
					    msg->sub_value.indirectbranchhist.b_type,
					    b_type_str[msg->sub_value.indirectbranchhist.b_type &
					    0x3],
					    msg->sub_value.indirectbranchhist.i_cnt,
					    msg->sub_value.indirectbranchhist.u_addr,
					    msg->sub_value.indirectbranchhist.hist);
		} break;
		case TCODE_INDIRECTBRANCHHISTSYNC: {
			xt_trace_msg_append(&mbuf, "TCODE=29(INDIRECTBRANCHHISTSYNC)");
			xt_trace_msg_append(&mbuf, ", sync=%d(%s), b-type=%d(%s), i-cnt=0x%lx, f_addr=0x%lx, hist=0x%lx",
					    msg->sub_value.indirectbranchhistsync.sync,
					    sync_str[msg->sub_value.indirectbranchhistsync.sync &
					    0xf],
					    msg->sub_value.indirectbranchhistsync.b_type,
					    b_type_str[msg->sub_value.indirectbranchhistsync.b_type &
					    0x3],
					    msg->sub_value.indirectbranchhistsync.i_cnt,
					    msg->sub_value.indirectbranchhistsync.f_addr,
					    msg->sub_value.indirectbranchhistsync.hist);
		} break;
		case TCODE_REPEATBRANCH: {
			xt_trace_msg_append(&mbuf, "TCODE=30(REPEATBRANCH)");
			xt_trace_msg_append(&mbuf, ", b-cnt=0x%lx",
					    msg->sub_value.repeatbranch.b_cnt);
		} break;
		case TCODE_PROGTRACECORRELATION: {
			xt_trace_msg_append(&mbuf, "TCODE=33(PROGTRACECORRELATION)");
			xt_trace_msg_append(&mbuf, ", evcode=%d(%s)",
					    msg->sub_value.progtracecorrelation.evcode,
					    evcode_str[msg->sub_value.progtracecorrelation.evcode &
					    0xf]);
			if (msg->sub_value.progtracecorrelation.cdf == 0) {
				xt_trace_msg_append(&mbuf, ", cdf=0(cdata only has i-cnt)");
				xt_trace_msg_append(&mbuf, ", i-cnt=0x%lx",
						    msg->sub_value.progtracecorrelation.i_cnt);
			} else if (msg->sub_value.progtracecorrelation.cdf == 1) {
				xt_trace_msg_append(&mbuf, ", cdf=1(cdata has i-cnt and hist)");
				xt_trace_msg_append(&mbuf, ", i-cnt=0x%lx, hist=0x%lx",
						    msg->sub_value.progtracecorrelation.i_cnt,
						    msg->sub_value.progtracecorrelation.hist);
			} else
				xt_trace_msg_append(&mbuf, ", cdf=%d(unknown)",
						    msg->sub_value.progtracecorrelation.cdf);
		} break;
		default: {
			xt_trace_msg_append(&mbuf, "Unknown TCODE %d", msg->tcode);
		} break;
	}

	if (msg->has_src)
		xt_trace_msg_append(&mbuf, ", src=%d", msg->src);
	if (msg->has_timestamp)
		xt_trace_msg_append(&mbuf, ", timestamp=0x%lx", msg->timestamp);
	xt_trace_msg_append(&mbuf, ".\n");

	return 0;
}

static int32_t
xt_trace_output_specified_message(struct xt_riscv_nexus_trace_message *msg, char *buf)
{
	struct xt_trace_msg mbuf = XT_TRACE_MSG(buf, STR_LEN_MAX);
	const char *priv_mode_str[4] = {
		"U-mode", "S-mode", "Unknown-mode", "M-mode"};
	const char *sync_str[16] = {
		"External Trace Trigger",
		"Exit From Reset",
		"Periodic Synchronization",
		"Exit from Debug Mode",
		"Sequential Instruction Counter",
		"Trace Enable",
		"Trace Event",
		"Restart from FIFO",
		"Reserved8",
		"Exit from Powerdown",
		"Reserved10",
		"Reserved11",
		"Reserved12",
		"Reserved13",
		"Reserved14",
		"Reserved15"};
	const char *evcode_str[16] = {
		"Entry into Debug Mode",
		"Entry into Low-power Mode",
		"Reserved2",
		"Reserved3",
		"Program Trace Disabled",
		"Reserved5",
		"Reserved6",
		"Reserved7",
		"Reserved8",
		"Reserved9",
		"Reserved10",
		"Reserved11",
		"Reserved12",
		"Reserved13",
		"Reserved14",
		"Reserved15"};
	xt_trace_msg_append(&mbuf, ".  ");

	switch (msg->tcode) {
		case TCODE_OWNERSHIP: {
			xt_trace_msg_append(&mbuf, "Messages below are in");
			xt_trace_msg_append(&mbuf, " %s%s", msg->sub_value.ownership.is_virtual ? "V" : "",
					    priv_mode_str[msg->sub_value.ownership.privilege & 0x3]);
		if (msg->sub_value.ownership.has_context)
			xt_trace_msg_append(&mbuf, " with %sContext=0x%lx", msg->sub_value.ownership.is_scontext ? "S" : "H",
					    msg->sub_value.ownership.context);
		} break;
		case TCODE_ERROR: {
			xt_trace_msg_append(&mbuf, "Error happened:");
			xt_trace_msg_append(&mbuf, "error type=%s, error code=0x%lx",
					    msg->sub_value.error.etype == 0 ? "Standard" : "Reserved", msg->sub_value.error.ecode);
			if (msg->sub_value.error.etype == 0 && msg->sub_value.error.ecode) {
				xt_trace_msg_append(&mbuf, "(");
				if (msg->sub_value.error.ecode & (1 << 0))
					xt_trace_msg_append(&mbuf, "[0]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 1))
					xt_trace_msg_append(&mbuf, ", [1]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 2))
					xt_trace_msg_append(&mbuf, ", [2]:Program Trace Message(s) lost");
				if (msg->sub_value.error.ecode & (1 << 3))
					xt_trace_msg_append(&mbuf, ", [3]:Ownership Trace Message(s) lost");
				if (msg->sub_value.error.ecode & (1 << 4))
					xt_trace_msg_append(&mbuf, ", [4]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 5))
					xt_trace_msg_append(&mbuf, ", [5]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 6))
					xt_trace_msg_append(&mbuf, ", [6]:Reserved");
				if (msg->sub_value.error.ecode & (1 << 7))
					xt_trace_msg_append(&mbuf, ", [7]:Vendor Defined Message(s) lost");
				xt_trace_msg_append(&mbuf, "),");
			} else if (msg->sub_value.error.etype == 0)
				xt_trace_msg_append(&mbuf, "(Exact reason unknown/not provided)");
		} break;
		case TCODE_PROGTRACESYNC: {
			xt_trace_msg_append(&mbuf, "Synchronization Reason: %s", sync_str[msg->sub_value.progtracesync.sync & 0xf]);
		} break;
		case TCODE_PROGTRACECORRELATION: {
			xt_trace_msg_append(&mbuf, "Correction Event: %s",
					    evcode_str[msg->sub_value.progtracecorrelation.evcode & 0xf]);
		} break;
		default: {
			xt_trace_msg_append(&mbuf, "Unknown TCODE %d", msg->tcode);
		} break;
	}

	if (msg->has_src)
		xt_trace_msg_append(&mbuf, ", CPU Num=%d", msg->src);
	if (msg->has_timestamp)
		xt_trace_msg_append(&mbuf, ", Timestamp=0x%lx", msg->timestamp);
	xt_trace_msg_append(&mbuf, ".\n");

	return 0;
}

int32_t xuantie_ntrace_decoder__process_full_message(struct perf_session *session,
	struct auxtrace_buffer *buffer, bool analyze_ranges,
	uint64_t init_pre_full_addr, uint64_t *out_pre_full_addr)
{
	int ret = 0;
	bool error_message_happened = true;
	struct xt_trace_program_flow_node *node = trace_program_header;
	uint64_t pre_full_addr = init_pre_full_addr;

	while (node) {
		if (error_message_happened) {
			if (xt_trace_msg_has_full_addr(&node->msg)) {
				if (pre_full_addr == 0) {
					/* Clear left resource full message. */
					resource_full_clear();

					xt_trace_get_start_and_full_addr(node, session, buffer);

					//ignore the i-cnt and hist of the first msg
					node->start_addr = node->full_addr;
					pre_full_addr = node->full_addr;
					if (node->faddr_dso[0] != '\0')
						strcpy(node->saddr_dso, node->faddr_dso);
					if (node->faddr_sym[0] != '\0')
						strcpy(node->saddr_sym, node->faddr_sym);

					//go next
					node = node->next;
					error_message_happened = false;
					continue;
				} else {
					/* pre_full_addr is known: process i-cnt/hist normally */
					node->start_addr = pre_full_addr;
					xt_trace_get_start_and_full_addr(node, session, buffer);
					error_message_happened = false;
				}
			} else
				node->invalid = 1;

			if (error_message_happened) {
				node = node->next;
				continue;
			}
		}

		switch (node->msg.tcode) {
		case TCODE_OWNERSHIP:
			break;
		case TCODE_DIRECTBRANCH:
			node->start_addr = pre_full_addr;
			xt_trace_get_start_and_full_addr(node, session, buffer);
			resource_full_clear();

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_INDIRECTBRANCH:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.indirectbranch.i_cnt,
					pre_full_addr, 0, &node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}

			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_ERROR:
			//FIXME:... reinit pre_full_addr
			pre_full_addr = 0;
			resource_full_clear();
			error_message_happened = true;
			break;
		case TCODE_PROGTRACESYNC:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.progtracesync.i_cnt,
					pre_full_addr, 0, &node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_DIRECTBRANCHSYNC:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.directbranchsync.i_cnt,
					pre_full_addr, 0, &node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_INDIRECTBRANCHSYNC:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.indirectbranchsync.i_cnt,
					pre_full_addr, 0, &node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_RESOURCEFULL:
			node->start_addr = pre_full_addr;
			if (node->msg.sub_value.resourcefull.rcode == 0) {
				add_resource_full_icnt(
					node->msg.sub_value.resourcefull.rdata0);
			} else if (node->msg.sub_value.resourcefull.rcode == 1) {
				add_resource_full_hist(
					node->msg.sub_value.resourcefull.rdata0);
			} else {
				// msg
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}
			break;
		case TCODE_INDIRECTBRANCHHIST:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.indirectbranchhist.i_cnt,
					pre_full_addr,
					node->msg.sub_value.indirectbranchhist.hist,
					&node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_INDIRECTBRANCHHISTSYNC:
			node->start_addr = pre_full_addr;
			if (analyze_ranges)
				ret = xt_trace_analyze_i_cnt_vs_hist(
					session, buffer,
					node->msg.sub_value.indirectbranchhistsync.i_cnt,
					pre_full_addr,
					node->msg.sub_value.indirectbranchhistsync.hist,
					&node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			// update pre_full_addr
			pre_full_addr = node->full_addr;
			break;
		case TCODE_REPEATBRANCH:
			break;
		case TCODE_PROGTRACECORRELATION:
			node->start_addr = pre_full_addr;
			ret = xt_trace_analyze_i_cnt_vs_hist(
				session, buffer,
				node->msg.sub_value.progtracecorrelation.i_cnt,
				pre_full_addr,
				node->msg.sub_value.progtracecorrelation.hist,
				&node->addr_range);
			resource_full_clear();
			if (ret < 0) {
				node->invalid = 1;
			}
			if (xt_trace_get_start_and_full_addr(node, session, buffer)) {
				node->invalid = 1;
				error_message_happened = true;
				pre_full_addr = 0;
				break;
			}

			error_message_happened = true;
			// update pre_full_addr, only for update out_pre_full_addr
			// mark that the next message can not use this addr as its
			// start address.
			pre_full_addr = 0;
			break;
		default:
			// msg
			resource_full_clear();
			node->invalid = 1;
			error_message_happened = true;
			pre_full_addr = 0;
			break;
		}

		// get next node
		node = node->next;
	}

	if (out_pre_full_addr)
		*out_pre_full_addr = pre_full_addr;

	return 0;
}

#ifdef HAVE_LIBBFD_SUPPORT
#define PACKAGE "perf"
#include <bfd.h>
#include <dis-asm.h>
#include <tools/dis-asm-compat.h>

static int
xt_trace_disassemble_and_display(char *file_name, struct xt_trace_address_range *range_p)
{
	struct disassemble_info info;
	bfd *bfdf;
	disassembler_ftype disassemble;
	int count = 0;
	asection *sect = NULL;
	bfd_byte *data = NULL;
	uint64_t addr = 0;

	if (file_name == NULL)
		return 0;

	bfdf = bfd_openr(file_name, NULL);
	if (bfdf == NULL) {
		return 0;
	}
	if (!bfd_check_format(bfdf, bfd_object))
		goto end;

	init_disassemble_info_compat(&info, stdout,
				     (fprintf_ftype) fprintf,
				     fprintf_styled);
	info.arch = bfd_get_arch(bfdf);
	info.mach = bfd_get_mach(bfdf);

	while (range_p && !XT_PERF_GET_CTRL_C()) {
		//printf range start and end first
		printf("      {0x%lx, 0x%lx - 1}\n", range_p->start_addr, range_p->end_addr);

		//disassemble and print insns
		//reuse sect and data
		addr = range_p->start_addr;
		if (sect && data) {
			if (bfd_section_vma(sect) <= addr
			    && addr < (bfd_section_vma(sect) + bfd_section_size(sect))) {
				while (1) {
					if (addr > 0xffffffff)
						printf("      %16lx:    ", addr);
					else
						printf("      %8lx:    ", addr);
					count = disassemble(addr, &info);
					if (count < 0)
						goto end;
					printf("\n");
					addr += count;

					if (addr >= range_p->end_addr)
						break;
				}

				range_p = range_p->next;
				continue;
			}
			free(data);
			data = NULL;
		}


		//find sect and disassmbler
		sect = bfdf->sections;
		for (sect = bfdf->sections; sect != NULL; sect = sect->next)
			if (bfd_section_vma(sect) <= addr
			    && addr < (bfd_section_vma(sect) + bfd_section_size(sect)))
				break;
		if (sect == NULL)
			goto end;
		if (!bfd_malloc_and_get_section(bfdf, sect, &data))
			goto end;
		info.buffer = data;
		info.buffer_length = bfd_section_size(sect);
		info.buffer_vma = bfd_section_vma(sect);

		disassemble_init_for_target(&info);
		disassemble = disassembler(info.arch,
					   bfd_big_endian(bfdf),
					   info.mach,
					   bfdf);
		if (disassemble == NULL)
			goto end;

		while (1) {
			if (addr > 0xffffffff)
				printf("      %16lx:    ", addr);
			else
				printf("      %8lx:    ", addr);
			count = disassemble(addr, &info);
			if (count < 0)
				goto end;
			printf("\n");
			addr += count;

			if (addr >= range_p->end_addr)
				break;
		}

		//next range
		range_p = range_p->next;
	}
end:
	if (data)
		free(data);
	bfd_close(bfdf);

	return 0;
}
#endif

int32_t xt_trace_program_trace_display(bool with_msg, bool with_addr,
						bool with_insn)
{
	char str[STR_LEN_MAX] = { '\0' };
	struct xt_trace_program_flow_node *node_p = trace_program_header;

	while (node_p && !XT_PERF_GET_CTRL_C()) {
		// printf n-trace message contents
		memset(str, 0, STR_LEN_MAX);

		if (with_msg) {
			if (xt_trace_output_ntrace_message(&node_p->msg, str)) {
				pr_err("Failed to format ntrace message\n");
				return -1;
			}
		} else if (node_p->msg.tcode == TCODE_OWNERSHIP
			 || node_p->msg.tcode == TCODE_ERROR
			 || node_p->msg.tcode == TCODE_PROGTRACESYNC
			 || node_p->msg.tcode == TCODE_PROGTRACECORRELATION) {
			if (xt_trace_output_specified_message(&node_p->msg, str)) {
				pr_err("Failed to format specified ntrace message\n");
				return -1;
			}
		}

		printf("%s", str);
		if (with_addr || with_insn) {
			switch (node_p->msg.tcode) {
			case TCODE_OWNERSHIP:
			case TCODE_ERROR:
			case TCODE_REPEATBRANCH:
			case TCODE_RESOURCEFULL:
				break;
			case TCODE_DIRECTBRANCH:
			case TCODE_INDIRECTBRANCH:
			case TCODE_PROGTRACESYNC:
			case TCODE_DIRECTBRANCHSYNC:
			case TCODE_INDIRECTBRANCHSYNC:
			case TCODE_INDIRECTBRANCHHIST:
			case TCODE_INDIRECTBRANCHHISTSYNC:
			case TCODE_PROGTRACECORRELATION:
				printf(
				"    Start from 0x%lx %-30s (%s)  ==>  End to 0x%lx %-30s (%s)\n",
					node_p->start_addr,
					node_p->saddr_sym[0] == '\0' ?
						"unknown" : node_p->saddr_sym,
					node_p->saddr_dso[0] == '\0' ?
						"unknown" : node_p->saddr_dso,
					node_p->full_addr,
					node_p->faddr_sym[0] == '\0' ?
						"unknown" : node_p->faddr_sym,
					node_p->faddr_dso[0] == '\0' ?
						"unknown" : node_p->faddr_dso);
				if (!xt_with_extra_msg && node_p->invalid)
					break;
				if (node_p->addr_range) {
					struct xt_trace_address_range *range_p =
						node_p->addr_range;
					printf("    Include pc ranges:\n");
					while (range_p) {
#ifdef HAVE_LIBBFD_SUPPORT
						if (with_insn &&
							(range_p->end_addr > range_p->start_addr)) {
							xt_trace_disassemble_and_display(
								node_p->saddr_dso, range_p);
							range_p = NULL;
						} else {
							if (range_p->extra_msg == 1)
								printf("%s", range_p->msg);
							else {
								printf("    {0x%lx, 0x%lx - 1}\n",
									range_p->start_addr,
									range_p->end_addr);
							}
							range_p = range_p->next;
						}
#else
						printf("    {0x%lx, 0x%lx - 1}\n",
							range_p->start_addr, range_p->end_addr);
						range_p = range_p->next;
#endif
					}
				}
				break;
			default:
				break;
			}
		}

		node_p = node_p->next;
	}

	xt_trace_free_program_flow_node();
	return 0;
}
