/* SPDX-License-Identifier: GPL-2.0 */

#pragma once

#include <stdio.h>
#include <linux/types.h>
#include <inttypes.h>

// define for TCODE
#define TCODE_OWNERSHIP              2
#define TCODE_DIRECTBRANCH           3
#define TCODE_INDIRECTBRANCH         4
#define TCODE_ERROR                  8
#define TCODE_PROGTRACESYNC          9
#define TCODE_DIRECTBRANCHSYNC       11
#define TCODE_INDIRECTBRANCHSYNC     12
#define TCODE_RESOURCEFULL           27
#define TCODE_INDIRECTBRANCHHIST     28
#define TCODE_INDIRECTBRANCHHISTSYNC 29
#define TCODE_REPEATBRANCH           30
#define TCODE_PROGTRACECORRELATION   33

enum xt_trace_privilege_mode {
	U_MODE = 0x0,
	S_MODE = 0x1,
	M_MODE = 0x3,
	VU_MODE = 0x10,
	VS_MODE = 0x11,
};

struct xt_riscv_nexus_trace_message {
	// FIXME: save the original n-trace message ??

	uint32_t tcode;
	uint32_t has_src;
	uint32_t src;
	uint32_t has_timestamp;
	uint64_t timestamp;
	union {
		struct ownershop {
			// FIXME: a field may bigger than 64
			// FIXED: we can separate process field to three fields
			//  2-bits format
			//  3-bits virtual + privilege
			//  ???
			uint8_t is_virtual;
			uint8_t privilege;
			uint8_t has_context;
			uint8_t is_scontext;
			uint64_t context;
		} ownership;

		struct directbranch {
			uint64_t i_cnt;
		} directbranch;

		struct indirectbranch {
			uint8_t b_type;
			uint64_t i_cnt;
			uint64_t u_addr;
		} indirectbranch;

		struct error {
			uint8_t etype;
			uint64_t ecode;
		} error;

		struct progtracesync {
			uint8_t sync;
			uint64_t i_cnt;
			uint64_t f_addr;
		} progtracesync;

		struct directbranchsync {
			uint8_t sync;
			uint64_t i_cnt;
			uint64_t f_addr;
		} directbranchsync;

		struct indirectbranchsync {
			uint8_t sync;
			uint8_t b_type;
			uint64_t i_cnt;
			uint64_t f_addr;
		} indirectbranchsync;

		struct resourcefull {
			uint8_t rcode;
			uint64_t rdata0;
			uint64_t rdata1;
		} resourcefull;

		struct indirectbranchhist {
			uint8_t b_type;
			uint64_t i_cnt;
			uint64_t u_addr;
			uint64_t hist;
		} indirectbranchhist;

		struct indirectbranchhistsync {
			uint8_t sync;
			uint8_t b_type;
			uint64_t i_cnt;
			uint64_t f_addr;
			uint64_t hist;
		} indirectbranchhistsync;

		struct repeatbranch {
			uint64_t b_cnt;
		} repeatbranch;

		struct progtracecorrelation {
			uint8_t evcode;
			uint8_t cdf;
			uint64_t i_cnt;
			uint64_t hist;
		} progtracecorrelation;
	} sub_value;
};

// define for recognize special insns
//  16-bits
#define RV_IS_C_BEQZ(insn)   (((insn)&0xe003) == 0xc001)
#define RV_IS_C_BNEZ(insn)   (((insn)&0xe003) == 0xe001)
#define RV_IS_C_EBREAK(insn) (((insn)&0xffff) == 0x9002)
#define RV_IS_C_JALR(insn)   (((insn)&0xf07f) == 0x9002)
#define RV_IS_C_JR(insn)     (((insn)&0xf07f) == 0x8002)
#define RV_IS_C_J(insn)      (((insn)&0xe003) == 0xa001)
#define RV_IS_C_JAL(insn)    (((insn)&0xe003) == 0x2001)

// 32-bits
#define RV_IS_BEQ(insn)    (((insn)&0x707f) == 0x63)
#define RV_IS_BGE(insn)    (((insn)&0x707f) == 0x5063)
#define RV_IS_BGEU(insn)   (((insn)&0x707f) == 0x7063)
#define RV_IS_BLT(insn)    (((insn)&0x707f) == 0x4063)
#define RV_IS_BLTU(insn)   (((insn)&0x707f) == 0x6063)
#define RV_IS_BNE(insn)    (((insn)&0x707f) == 0x1063)
#define RV_IS_EBREAK(insn) (((insn)&0xffffffff) == 0x100073)
#define RV_IS_ECALL(insn)  (((insn)&0xffffffff) == 0x73)
#define RV_IS_MRET(insn)   (((insn)&0xffffffff) == 0x30200073)
#define RV_IS_SRET(insn)   (((insn)&0xffffffff) == 0x10200073)
#define RV_IS_URET(insn)   (((insn)&0xffffffff) == 0x200073)
#define RV_IS_JAL(insn)    (((insn)&0x7f) == 0x6f)
#define RV_IS_JALR(insn)   (((insn)&0x707f) == 0x67)

#define ERROR_MSG_ETYPE_STANDARD  0
#define ERROR_MSG_ECODE_OWNERSHIP (1 << 3)

struct segment_message {
	uint32_t type;
	uint64_t file_offset;
	uint64_t virtual_addr;
	uint64_t size;
};

struct ntrace_message_handler {
	const char *elf_path;
	const char *trace_file_path;

	// for elf
	bool is_elf32;

	struct segment_message *segment;
	uint32_t segment_count;

	struct xt_trace_encoder_config_info *encoder_config;
};
