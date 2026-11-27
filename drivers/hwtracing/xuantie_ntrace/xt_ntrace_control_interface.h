/* SPDX-License-Identifier: GPL-2.0 */

#pragma once

#include <linux/types.h>

#define XT_NTRACE_SPEC_MAIN_VER 1
#define XT_NTRACE_SPEC_MINOR_VER 0

enum xt_trace_component_type {
	TRCOMP_ENCODER = 0x1, // Trace Encoder
	TRCOMP_FUNNEL = 0x8, // Trace Funnel
	TRCOMP_RAMSINK = 0x9, // Trace RAM Sink
	TRCOMP_PIBSINK = 0xa, // Trace PIB Sink
	TRCOMP_ATBBRIDGE = 0xe // Trace ATB Bridge
};

#define ADDR_SIZE (1)

// Macros for Trace Ram Sink
#define OFFSET_TRRAMCONTROL ((0x0) / ADDR_SIZE)
#define OFFSET_TRRAMIMPL ((0x4) / ADDR_SIZE)
#define OFFSET_TRRAMSTARTLOW ((0x10) / ADDR_SIZE)
#define OFFSET_TRRAMSTARTHIGH ((0x14) / ADDR_SIZE)
#define OFFSET_TRRAMLIMITLOW ((0x18) / ADDR_SIZE)
#define OFFSET_TRRAMLIMITHIGH ((0x1C) / ADDR_SIZE)
#define OFFSET_TRRAMWPLOW ((0x20) / ADDR_SIZE)
#define OFFSET_TRRAMWPHIGH ((0x24) / ADDR_SIZE)
#define OFFSET_TRRAMRPLOW ((0x28) / ADDR_SIZE)
#define OFFSET_TRRAMRPHIGH ((0x2c) / ADDR_SIZE)
#define OFFSET_TRRAMDATA ((0x40) / ADDR_SIZE)

// macros for PIB Sink
#define OFFSET_TRPIBCONTROL ((0x0) / ADDR_SIZE)
#define OFFSET_TRATBBRIDGEIMPL ((0x4) / ADDR_SIZE)

// Macros for Trace FUNNEL
#define OFFSET_TRFUNNELCONTROL ((0x0) / ADDR_SIZE)
#define OFFSET_TRFUNNELIMPL ((0x4) / ADDR_SIZE)
#define OFFSET_TRFUNNELDISINPUT ((0x8) / ADDR_SIZE)

// macros for Trace Encoder
#define OFFSET_TRENCODERCONTROL ((0x0) / ADDR_SIZE)
#define OFFSET_TRENCODERIMPL ((0x4) / ADDR_SIZE)
#define OFFSET_TRTEINSTFEATURES ((0x8) / ADDR_SIZE)
#define OFFSET_TRTEINSTFILTERS ((0xc) / ADDR_SIZE)
#define OFFSET_TRTEDATACONTROL ((0x10) / ADDR_SIZE)
#define OFFSET_TRTEDATAFILTERS ((0x1c) / ADDR_SIZE)
#define OFFSET_TRTSCONTROL ((0x40) / ADDR_SIZE)
#define OFFSET_TRTSCOUNTERLOW ((0x48) / ADDR_SIZE)
#define OFFSET_TRTSCOUNTERHIGH ((0x4c) / ADDR_SIZE)
// filter i
#define OFFSET_TRFILTERCONTROLI(i) \
	((0x400 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (i))
#define OFFSET_TRFILTERMATCHINSTI(i) \
	((0x404 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (i))
#define OFFSET_TRFILTERMATCHECAUSEI(i) \
	((0x408 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (i))
#define OFFSET_TRFILTERMATCHDATAI(i) \
	((0x418 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (i))
// comparator j
#define OFFSET_TRCOMPCONTROLJ(j) \
	((0x600 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (j))
#define OFFSET_TRCOMPPMATCHLOWJ(j) \
	((0x610 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (j))
#define OFFSET_TRCOMPPMATCHHIGHJ(j) \
	((0x614 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (j))
#define OFFSET_TRCOMPSMATCHLOWJ(j) \
	((0x618 / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (j))
#define OFFSET_TRCOMPSMATCHHIGHJ(j) \
	((0x61c / ADDR_SIZE) + (0x20 / ADDR_SIZE) * (j))

enum xt_trace_ram_sink_type {
	TRACE_SRAM_SINK = 0,
	TRACE_SMEM_SINK = 1,
};

enum xt_pib_sink_mode {
	PIB_MODE_SWT_MANCHESTER = 4,
	PIB_MODE_SWT_UART = 5,
	PIB_MODE_TRC_1TRCDATA = 8,
	PIB_MODE_TRC_2TRCDATA = 9,
	PIB_MODE_TRC_4TRCDATA = 10,
	PIB_MODE_TRC_8TRCDATA = 11,
	PIB_MODE_TRC_16TRCDATA = 12,
};

/*========================================================================*/
//  For Deecting the Trace Component
/*========================================================================*/

struct xt_trace_sink_control_info {
	bool has_detected;

	uint32_t main_ver;
	uint32_t minor_ver;
	uint64_t base_addr;
	enum xt_trace_component_type comp_type; // RAM/PIB Sink or ATB Bridge

	// save default value for trSinkControl
	uint32_t tr_sink_control_default;

	// for ram sink
	// in trRamControl
	bool support_sram_sink;
	bool support_smem_sink;
	bool support_stop_on_wrap;
	bool support_mem_format_plain_bytes;
	bool support_mem_format_reserved1;
	bool support_mem_format_reserved2;
	bool support_mem_format_custom;
	// size
	uint32_t limit_low_max; // only for sram sink
	uint32_t limit_high_max; // only for sram sink

	// for pib sink
	// trPibMode
	bool support_pibmode_swt_machester;
	bool support_pibmode_swt_uart;
	bool support_pibmode_trc_1trcdata;
	bool support_pibmode_trc_2trcdata;
	bool support_pibmode_trc_4trcdata;
	bool support_pibmode_trc_8trcdata;
	bool support_pibmode_trc_16trcdata;
	// trPibClkCenter
	bool support_pibclk_center_0;
	bool support_pibclk_center_1;
	// trPibCalibrate
	bool support_pibcalibrate;
	// trPibDivider
	uint32_t support_pibdivider_min;
	uint32_t support_pibdivider_max;

	// for ram sink, pib sink and atb bridge
	bool support_async_freq_0;
	bool support_async_freq_1;
	bool support_async_freq_2;
	bool support_async_freq_3;
	bool support_async_freq_4;
	bool support_async_freq_5;
	bool support_async_freq_6;
	bool support_async_freq_7;
};

struct xt_trace_funnel_control_info {
	bool has_detected;

	uint32_t main_ver;
	uint32_t minor_ver;
	uint64_t base_addr;

	// info
	uint32_t input_count;
};

struct xt_trace_encoder_control_info {
	bool has_detected;

	uint32_t main_ver;
	uint32_t minor_ver;
	uint32_t protocol_main_ver;
	uint32_t protocol_minor_ver;
	uint64_t base_addr;

	// save default value for trTeControl
	uint32_t tr_te_control_default;

	//+--- for inst ---+
	// trTeInstMode
	bool support_inst_mode_disable; // inst trace disable
	bool support_inst_mode_reserved1; // reserved for subsets of branch trace mode
	bool support_inst_mode_reserved2; // ditto
	bool support_inst_mode_branch_trace; // branch trace mode
	bool support_inst_mode_reserved4; // reserved for subsets for branch history mode
	bool support_inst_mode_reserved5; // ditto
	bool support_inst_mode_branch_history; // branch history mode
	bool support_inst_mode_vendor; // reserved for vendor-defined
		// trTeContext
	bool support_context_0;
	bool support_context_1;
	// trTeInstTrigEnable
	bool support_inst_trigger_enable;
	// trTeInstStallEna
	bool support_inst_stall_ena_0;
	bool support_inst_stall_ena_1;
	// trTeInhibitSrc
	bool support_inhibit_src_0;
	bool support_inhibit_src_1;
	// trTeInstSyncMode
	bool support_inst_sync_mode_off;
	bool support_inst_sync_mode_count_message;
	bool support_inst_sync_mode_count_hart_clock;
	bool support_inst_sync_mode_count_instruction;
	// trTeInstSyncMax
	uint32_t inst_sync_min;
	uint32_t inst_sync_max;
	// trTeFormat
	bool support_format_etrace;
	bool support_format_ntrace;
	bool support_format_vendor_trace;
	// trTeInstFeatures
	bool support_inst_no_addr_diff;
	bool support_inst_no_trap_addr;
	bool support_inst_en_sequential_tail_jump;
	bool support_inst_en_implicit_return;
	bool support_inst_en_branch_prediction;
	bool support_inst_en_jump_target_cache;
	uint32_t support_inst_implicit_return_mode;
	bool support_inst_en_repeated_history;
	bool support_inst_en_all_jumps;
	bool support_inst_extend_addr_msb;
	uint32_t default_src_id;
	uint32_t default_src_bits;

	//+-- for data trace --+
	bool data_trace_implemented;
	bool support_data_trace_0;
	bool support_data_trace_1;
	bool support_data_trace_tigger_enable;
	bool support_data_trace_stall_enable;
	bool support_data_trace_drop_enable;
	bool support_data_trace_no_value_0;
	bool support_data_trace_no_value_1;
	bool support_data_trace_no_addr_0;
	bool support_data_trace_no_addr_1;
	bool support_data_trace_addr_compress_full_addr;
	bool support_data_trace_addr_compress_xor;
	bool support_data_trace_addr_compress_diff;
	bool support_data_trace_addr_compress_dynamic;
	// save default value for trTeDataControl
	uint32_t tr_te_data_control_default;

	// for filter
	uint32_t filter_count;
	// match
	bool support_filter_match_privilege;
	bool support_filter_match_ecause;
	bool support_filter_match_interrupt;
	// vendor match
	bool support_filter_match_impdef;
	uint32_t default_filter_match_impdef_value;
	uint32_t default_filter_match_impdef_mask;
	// compare
	bool support_filter_match_comparator1st;
	uint32_t default_filter_comparator1st;
	bool support_filter_match_comparator2nd;
	uint32_t default_filter_comparator2nd;
	bool support_filter_match_comparator3rd;
	uint32_t default_filter_comparator3rd;
	// data trace
	bool support_filter_match_dtype;
	bool support_filter_match_dsize;
	// comparators
	uint32_t comparator_count;
	bool support_primary_compare_iaddr;
	bool support_primary_compare_context;
	bool support_primary_compare_tval;
	bool support_primary_compare_daddr;
	bool support_primary_compare_func_equal;
	bool support_primary_compare_func_notequal;
	bool support_primary_compare_func_lessthan;
	bool support_primary_compare_func_lessthanorequal;
	bool support_primary_compare_func_greaterthan;
	bool support_primary_compare_func_greaterthanorequal;
	bool support_primary_compare_func_false;
	bool support_primary_compare_func_true;
	bool support_secondary_compare_iaddr;
	bool support_secondary_compare_context;
	bool support_secondary_compare_tval;
	bool support_secondary_compare_daddr;
	bool support_secondary_compare_func_equal;
	bool support_secondary_compare_func_notequal;
	bool support_secondary_compare_func_lessthan;
	bool support_secondary_compare_func_lessthanorequal;
	bool support_secondary_compare_func_greaterthan;
	bool support_secondary_compare_func_greaterthanorequal;
	bool support_secondary_compare_func_false;
	bool support_secondary_compare_func_true;
	bool support_compare_primary_true;
	bool support_compare_both_true;
	bool support_compare_either_false;
	bool support_compare_between_primary_and_secondary_true;
	bool support_compare_primary_notify_0;
	bool support_compare_primary_notify_1;
	bool support_compare_secondary_notify_0;
	bool support_compare_secondary_notify_1;
	// variables for using filter
	uint32_t filter_used;
	uint32_t comparator_used;

	// for timestamp
	bool support_timestamp_run_in_debugmode;
	bool support_timestamp_none;
	bool support_timestamp_external;
	bool support_timestamp_internal_system;
	bool support_timestamp_internal_core;
	bool support_timestamp_shared;
	bool support_timestamp_vendor5;
	bool support_timestamp_vendor6;
	bool support_timestamp_vendor7;
	bool support_timestamp_prescale_1;
	bool support_timestamp_prescale_4;
	bool support_timestamp_prescale_16;
	bool support_timestamp_prescale_64;
	bool support_timestamp_enable;
	uint32_t timestamp_width;
};

// macros for trace component
#define TR_ANY_CONTROL_ACTIVE0 0
#define TR_ANY_CONTROL_ACTIVE1 1
#define TR_ANY_CONTROL_ENABLE0 0
#define TR_ANY_CONTROL_ENABLE1 1
#define TR_ANY_CONTROL_GET_ACTIVE(trctrl) ((trctrl)&1)
#define TR_ANY_CONTROL_GET_ENABLE(trctrl) (((trctrl) >> 1)&1)
#define TR_ANY_IMPL_OFFSET 0x4
#define TR_ANY_GET_IMPL_MAIN_VER(trimpl) ((trimpl)&0xf)
#define TR_ANY_GET_IMPL_MINOR_VER(trimpl) (((trimpl) >> 4) & 0xf)
#define TR_ANY_GET_IMPL_COMP_TYPE(trimpl) (((trimpl) >> 8) & 0xf)

// macros for trRamImpl
#define TR_RAM_IMPL_HAS_SRAM (1 << 12)
#define TR_RAM_IMPL_HAS_SMEM (1 << 13)
// set
#define TRRAMCONTROL_SET_MODE(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x1) << 4)
#define TRRAMCONTROL_SET_STOPONWRAP(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x1) << 8)
#define TRRAMCONTROL_SET_MEMFORMAT(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x3) << 9)
#define TRRAMCONTROL_SET_ASYNCFREQ(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x7) << 12)
// get
#define TRRAMCONTROL_GET_MODE(tr_sink_control) (((tr_sink_control) >> 4) & 0x1)
#define TRRAMCONTROL_GET_STOPONWRAP(tr_sink_control) \
	(((tr_sink_control) >> 8) & 0x1)
#define TRRAMCONTROL_GET_MEMFORMAT(tr_sink_control) \
	(((tr_sink_control) >> 9) & 0x3)
#define TRRAMCONTROL_GET_ASYNCFREQ(tr_sink_control) \
	(((tr_sink_control) >> 12) & 0x7)

// macros for trPibControl
// set
#define TRPIBCONTROL_SET_MODE(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0xf) << 4)
#define TRPIBCONTROL_SET_CLKCENTER(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x1) << 8)
#define TRPIBCONTROL_SET_CALIBRATE(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0x1) << 9)
#define TRPIBCONTROL_SET_DIVIDER(tr_sink_control, value) \
	((tr_sink_control) | ((value)&0xffff) << 16)
// get
#define TRPIBCONTROL_GET_MODE(tr_sink_control) (((tr_sink_control) >> 4) & 0xf)
#define TRPIBCONTROL_GET_CLKCENTER(tr_sink_control) \
	(((tr_sink_control) >> 8) & 0x1)
#define TRPIBCONTROL_GET_CALIBRATE(tr_sink_control) \
	(((tr_sink_control) >> 9) & 0x1)
#define TRPIBCONTROL_GET_DIVIDER(tr_sink_control) \
	(((tr_sink_control) >> 16) & 0xffff)

#define u32_set_fields(value, field_high, field_low, field_value)             \
	((((0xffffffff >> (31 - (field_high - field_low))) & (field_value))     \
	  << field_low) |                                                     \
	 ((~((0xffffffff >> (31 - (field_high - field_low))) << field_low)) & \
	  value))

#define u32_get_fields(value, field_high, field_low) \
	((value >> field_low) & (0xffffffff >> (31 - (field_high - field_low))))

#define XT_CHECK_TRANYCONTROL_TIMES 0x2000

/*========================================================================*/
//  For Configing the Trace Component
/*========================================================================*/
struct xt_trace_sink_config_info {
	enum xt_trace_component_type component_type;

	// for trace ram sink
	enum xt_trace_ram_sink_type type; // configure to SRAM or SMEM
	bool ram_sink_stop_on_wrap;
	uint32_t ram_sink_mem_format;
	uint64_t ram_sink_start;
	uint64_t ram_sink_limit;
	uint64_t ram_sink_write_point;

	// for pib sink
	uint32_t pib_sink_mode;
	uint32_t pib_sink_clk_center;
	uint32_t pib_sink_divider;

	// for atb bridge
	uint32_t atb_bridge_id;

	// common config
	uint32_t sink_async_freq;
};

struct xt_trace_funnel_config_info {
	uint32_t disable_input;
};

struct xt_trace_encoder_config_info {
	// inst config
	uint32_t inst_mode;
	bool send_context;
	bool inst_trigger_enable;
	bool inst_stall_ena;
	bool inhibit_src;
	uint32_t inst_sync_mode;
	uint32_t inst_sync_max;
	uint32_t record_format;

	// inst feature enable
	bool inst_no_addr_diff;
	bool inst_no_trap_addr;
	bool inst_en_sequential_tail_jump;
	bool inst_en_implicit_return;
	bool inst_en_branch_prediction;
	bool inst_en_jump_target_cache;
	uint32_t inst_implicit_return_mode;
	bool inst_en_repeated_history;
	bool inst_en_all_jumps;
	bool inst_extend_addr_msb;
	uint32_t src_id;
	uint32_t src_bits;

	// data trace
	bool data_trace_enable; // flag for user will use data trace
	bool data_trigger_enable;
	bool data_stall_enable;
	bool data_drop_enable;
	bool data_no_value;
	bool data_no_addr;
	uint32_t data_addr_compress;

	// filter will be configured at other places

	// timestamp
	bool timestamp_enable; // flag for user will use timestamp
	bool timestamp_run_in_debugmode;
	uint32_t timestamp_type;
	uint32_t timestamp_prescale;
};

#define FILTER_U_MODE    (1 << 0)
#define FILTER_S_HS_MODE (1 << 1)
#define FILTER_M_MODE    (1 << 3)
#define FILTER_D_MODE    (1 << 4)
#define FILTER_VU_MODE   (1 << 5)
#define FILTER_VS_MODE   (1 << 6)

enum filter_comparator_type {
	COMPTYPE_IADDR = 0,
	COMPTYPE_CONTEXT = 1,
	COMPTYPE_TVAL = 2,
	COMPTYPE_DADDR = 3,
};

enum filter_comparator_function {
	COMPFUNCTION_EQUAL = 0,
	COMPFUNCTION_NOT_EQUAL = 1,
	COMPFUNCTION_LESS_THAN = 2,
	COMPFUNCTION_LESS_OR_EQUAL = 3,
	COMPFUNCTION_GREATER_THAN = 4,
	COMPFUNCTION_GREATER_OR_EQUAL = 5,
	COMPFUNCTION_ALWAYS_FALSE = 6,
	COMPFUNCTION_ALWAYS_TRUE = 7,
};

enum filter_comparator_match_mode {
	COMPMATCHMODE_PRIMARY_TRUE = 0,
	COMPMATCHMODE_BOTH_TRUE = 1,
	COMPMATCHMODE_EITHER_FALSE = 2,
	COMPMATCHMODE_PRIMARY_TRUE_UNTIL_SECOND_TRUE = 3,
};

struct xt_trace_encoder_filter_config_info {
	uint32_t filter_i;

	// filter
	bool filter_match_privilege;
	bool filter_match_ecause;
	bool filter_match_interrupt;
	uint32_t match_privilege_value;
	uint32_t match_value_interrupt;
	uint32_t match_choice_ecause;
	bool filter_match_dtype;
	bool filter_match_dsize;
	uint32_t match_value_dtype;
	uint32_t match_value_dsize;

	// comparator 1
	bool comparator1_enable;
	uint32_t comp1_filter_number;
	// config
	uint32_t comp1_primary_input;
	uint32_t comp1_primary_function;
	bool comp1_primary_notify;
	uint32_t comp1_secondary_input;
	uint32_t comp1_secondary_function;
	bool comp1_secondary_notify;
	uint32_t comp1_match_mode;
	uint32_t comp1_primary_match_low_value;
	uint32_t comp1_primary_match_high_value;
	uint32_t comp1_secondary_match_low_value;
	uint32_t comp1_secondary_match_high_value;

	// comparator 2
	bool comparator2_enable;
	uint32_t comp2_filter_number;
	// config
	uint32_t comp2_primary_input;
	uint32_t comp2_primary_function;
	bool comp2_primary_notify;
	uint32_t comp2_secondary_input;
	uint32_t comp2_secondary_function;
	bool comp2_secondary_notify;
	uint32_t comp2_match_mode;
	uint32_t comp2_primary_match_low_value;
	uint32_t comp2_primary_match_high_value;
	uint32_t comp2_secondary_match_low_value;
	uint32_t comp2_secondary_match_high_value;

	// comparator 3
	bool comparator3_enable;
	uint32_t comp3_filter_number;
	// config
	uint32_t comp3_primary_input;
	uint32_t comp3_primary_function;
	bool comp3_primary_notify;
	uint32_t comp3_secondary_input;
	uint32_t comp3_secondary_function;
	bool comp3_secondary_notify;
	uint32_t comp3_match_mode;
	uint32_t comp3_primary_match_low_value;
	uint32_t comp3_primary_match_high_value;
	uint32_t comp3_secondary_match_low_value;
	uint32_t comp3_secondary_match_high_value;
};

#if defined(_WIN32) && !defined(__CYGWIN)
#ifdef TRACE_CONTROL_LIB_EXPORTS
#define TRACE_CONTROL_LIB_API __declspec(dllexport)
#else
#define TRACE_CONTROL_LIB_API __declspec(dllimport)
#endif /* TRACE_CONTROL_LIB_EXPORTS */
#else
#define TRACE_CONTROL_LIB_API
#endif /* _WIN32 && !__CYGWIN */

// Init reading and writing register interfaces of the Trace Component
TRACE_CONTROL_LIB_API void xt_trace_control_init_rw_trace_interface(
	int (*msgout)(const char *),
	int32_t (*trace_register_write)(uint64_t addr, uint32_t value),
	int32_t (*trace_register_read)(uint64_t addr, uint32_t *value),
	int32_t (*trace_memory_read)(uint64_t addr, uint8_t *buf,
				     uint32_t size));

// Trace Init Interfaces
TRACE_CONTROL_LIB_API void
xt_init_trace_sink_control_info(struct xt_trace_sink_control_info *sink_info,
				uint64_t base_addr);
TRACE_CONTROL_LIB_API void xt_init_trace_funnel_control_info(
	struct xt_trace_funnel_control_info *funnel_info, uint64_t base_addr);
TRACE_CONTROL_LIB_API void xt_init_trace_encoder_control_info(
	struct xt_trace_encoder_control_info *encoder_info, uint64_t base_addr);

// Trace Detect Interfaces
TRACE_CONTROL_LIB_API int32_t
xt_trace_detect_trace_sink(struct xt_trace_sink_control_info *sink_info);
TRACE_CONTROL_LIB_API int32_t
xt_trace_detect_trace_funnel(struct xt_trace_funnel_control_info *funnel_info);
TRACE_CONTROL_LIB_API int32_t xt_trace_detect_trace_encoder(
	struct xt_trace_encoder_control_info *encoder_info);

// Trace Enable, Disable, Close Interfaces
// Enable
TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_enable(struct xt_trace_encoder_control_info *encoder_info,
			bool enable_timestamp);
TRACE_CONTROL_LIB_API int32_t xt_trace_filter_enable(
	struct xt_trace_encoder_control_info *encoder_info, uint32_t filter_i);
TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_enable(struct xt_trace_sink_control_info *sink_info);
TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_enable(struct xt_trace_funnel_control_info *funnel_info);
// Disable
TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_disable(struct xt_trace_encoder_control_info *encoder_info,
			 bool disable_timestamp);
TRACE_CONTROL_LIB_API int32_t xt_trace_filter_disable(
	struct xt_trace_encoder_control_info *encoder_info, uint32_t filter_i);
TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_disable(struct xt_trace_sink_control_info *sink_info);
TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_disable(struct xt_trace_funnel_control_info *funnel_info);
// Close
TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_close(struct xt_trace_encoder_control_info *encoder_info,
		       bool close_timestamp);
TRACE_CONTROL_LIB_API int32_t xt_trace_filter_close(
	struct xt_trace_encoder_control_info *encoder_info, uint32_t filter_i);
TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_close(struct xt_trace_sink_control_info *sink_info);
TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_close(struct xt_trace_funnel_control_info *funnel_info);

// Config
TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_config(struct xt_trace_sink_control_info *sink_info,
		     struct xt_trace_sink_config_info *sink_config);
TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_config(struct xt_trace_funnel_control_info *funnel_info,
		       struct xt_trace_funnel_config_info *funel_config);
TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_config(struct xt_trace_encoder_control_info *encoder_info,
			struct xt_trace_encoder_config_info *encoder_config);
TRACE_CONTROL_LIB_API int32_t xt_trace_filter_config(
	struct xt_trace_encoder_control_info *encoder_info,
	struct xt_trace_encoder_filter_config_info *filter_config);

// Read Data from Sink
TRACE_CONTROL_LIB_API int32_t xt_trace_ram_sink_get_data_size(
	struct xt_trace_sink_control_info *sink_info, uint64_t *write_point);

TRACE_CONTROL_LIB_API int32_t xt_trace_register_write(uint64_t addr, uint32_t value);
TRACE_CONTROL_LIB_API int32_t xt_trace_register_read(uint64_t addr, uint32_t *value);
TRACE_CONTROL_LIB_API int32_t xt_trace_memory_read(uint64_t addr, uint8_t *buf, uint32_t size);
TRACE_CONTROL_LIB_API int32_t enable_trace_component(uint64_t base_addr);
TRACE_CONTROL_LIB_API int32_t primary_enable_trace_component(uint64_t base_addr);
TRACE_CONTROL_LIB_API int32_t reset_trace_component(uint64_t base_addr);
TRACE_CONTROL_LIB_API int32_t xt_trace_register_readafterwrite(uint64_t reg_addr,
						 uint32_t value);
TRACE_CONTROL_LIB_API int32_t xt_trace_msgout(const char *str);
