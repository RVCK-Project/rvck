// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/memory.h>

#include "xt_ntrace_control_interface.h"

TRACE_CONTROL_LIB_API void
xt_init_trace_sink_control_info(struct xt_trace_sink_control_info *sink_info,
				uint64_t base_addr)
{
	// clear sink_info
	memset(sink_info, 0, sizeof(struct xt_trace_sink_control_info));

	sink_info->base_addr = base_addr;
	sink_info->has_detected = false;

	// current only support version 1.0
	sink_info->main_ver = 1;
	sink_info->minor_ver = 0;

	// default trace ram sink
	sink_info->comp_type = TRCOMP_RAMSINK;

	sink_info->support_sram_sink = false;
	sink_info->support_smem_sink = false;
	sink_info->support_stop_on_wrap = false;
	sink_info->support_mem_format_plain_bytes = false;
	sink_info->support_mem_format_reserved1 = false;
	sink_info->support_mem_format_reserved2 = false;
	sink_info->support_mem_format_custom = false;

	sink_info->limit_low_max = 0; // only for sram sink
	sink_info->limit_high_max = 0; // only for sram sink

	// for pib sink
	// trPibMode
	sink_info->support_pibmode_swt_machester = false;
	sink_info->support_pibmode_swt_uart = false;
	sink_info->support_pibmode_trc_1trcdata = false;
	sink_info->support_pibmode_trc_2trcdata = false;
	sink_info->support_pibmode_trc_4trcdata = false;
	sink_info->support_pibmode_trc_8trcdata = false;
	sink_info->support_pibmode_trc_16trcdata = false;
	// trPibClkCenter
	sink_info->support_pibclk_center_0 = false;
	sink_info->support_pibclk_center_1 = false;
	// trPibCalibrate
	sink_info->support_pibcalibrate = false;
	// trPibDivider
	sink_info->support_pibdivider_min = 0;
	sink_info->support_pibdivider_max = 0;

	// for ram sink, pib sink and atb bridge
	sink_info->support_async_freq_0 = false;
	sink_info->support_async_freq_1 = false;
	sink_info->support_async_freq_2 = false;
	sink_info->support_async_freq_3 = false;
	sink_info->support_async_freq_4 = false;
	sink_info->support_async_freq_5 = false;
	sink_info->support_async_freq_6 = false;
	sink_info->support_async_freq_7 = false;
}

TRACE_CONTROL_LIB_API void xt_init_trace_encoder_control_info(
	struct xt_trace_encoder_control_info *encoder_info, uint64_t base_addr)
{
	// clear encoder_info
	memset(encoder_info, 0, sizeof(struct xt_trace_encoder_control_info));

	encoder_info->base_addr = base_addr;
	encoder_info->has_detected = false;
	encoder_info->main_ver = 1;
	encoder_info->minor_ver = 0;
	encoder_info->protocol_main_ver = 1;
	encoder_info->protocol_minor_ver = 0;

	//+--- for inst ---+
	// trTeInstMode
	// inst trace disable
	encoder_info->support_inst_mode_disable = false;
	// reserved for subsets of branch trace mode
	encoder_info->support_inst_mode_reserved1 = false;
	encoder_info->support_inst_mode_reserved2 = false;
	// branch trace mode
	encoder_info->support_inst_mode_branch_trace = false;
	// reserved for subsets for branch history mode
	encoder_info->support_inst_mode_reserved4 = false;
	encoder_info->support_inst_mode_reserved5 = false;
	// branch history mode
	encoder_info->support_inst_mode_branch_history = false;
	// reserved for vendor-defined
	encoder_info->support_inst_mode_vendor = false;
	// trTeContext
	encoder_info->support_context_0 = false;
	encoder_info->support_context_1 = false;
	// trTeInstTrigEnable
	encoder_info->support_inst_trigger_enable = false;
	// trTeInstStallEna
	encoder_info->support_inst_stall_ena_0 = false;
	encoder_info->support_inst_stall_ena_1 = false;
	// trTeInhibitSrc
	encoder_info->support_inhibit_src_0 = false;
	encoder_info->support_inhibit_src_1 = false;
	// trTeInstSyncMode
	encoder_info->support_inst_sync_mode_off = false;
	encoder_info->support_inst_sync_mode_count_message = false;
	encoder_info->support_inst_sync_mode_count_hart_clock = false;
	encoder_info->support_inst_sync_mode_count_instruction = false;
	// trTeInstSyncMax
	encoder_info->inst_sync_min = 0;
	encoder_info->inst_sync_max = 0;
	// trTeFormat
	encoder_info->support_format_etrace = false;
	encoder_info->support_format_ntrace = false;
	encoder_info->support_format_vendor_trace = false;
	// trTeInstFeatures
	encoder_info->support_inst_no_addr_diff = false;
	encoder_info->support_inst_no_trap_addr = false;
	encoder_info->support_inst_en_sequential_tail_jump = false;
	encoder_info->support_inst_en_implicit_return = false;
	encoder_info->support_inst_en_branch_prediction = false;
	encoder_info->support_inst_en_jump_target_cache = false;
	encoder_info->support_inst_implicit_return_mode = 0;
	encoder_info->support_inst_en_repeated_history = false;
	encoder_info->support_inst_en_all_jumps = false;
	encoder_info->support_inst_extend_addr_msb = false;
	encoder_info->default_src_id = 0;
	encoder_info->default_src_bits = 0;

	// for data trace
	encoder_info->data_trace_implemented = false;
	encoder_info->support_data_trace_0 = false;
	encoder_info->support_data_trace_1 = false;
	encoder_info->support_data_trace_tigger_enable = false;
	encoder_info->support_data_trace_stall_enable = false;
	encoder_info->support_data_trace_drop_enable = false;
	encoder_info->support_data_trace_no_value_0 = false;
	encoder_info->support_data_trace_no_value_1 = false;
	encoder_info->support_data_trace_no_addr_0 = false;
	encoder_info->support_data_trace_no_addr_1 = false;
	encoder_info->support_data_trace_addr_compress_full_addr = false;
	encoder_info->support_data_trace_addr_compress_xor = false;
	encoder_info->support_data_trace_addr_compress_diff = false;
	encoder_info->support_data_trace_addr_compress_dynamic = false;
	encoder_info->tr_te_data_control_default = 0;

	// for filter
	encoder_info->filter_count = 0;
	// match
	encoder_info->support_filter_match_privilege = false;
	encoder_info->support_filter_match_ecause = false;
	encoder_info->support_filter_match_interrupt = false;
	// vendor match
	encoder_info->support_filter_match_impdef = false;
	encoder_info->default_filter_match_impdef_value = 0;
	encoder_info->default_filter_match_impdef_mask = 0;
	// compare
	encoder_info->support_filter_match_comparator1st = false;
	encoder_info->default_filter_comparator1st = 0;
	encoder_info->support_filter_match_comparator2nd = false;
	encoder_info->default_filter_comparator2nd = 0;
	encoder_info->support_filter_match_comparator3rd = false;
	encoder_info->default_filter_comparator3rd = 0;
	// data trace
	encoder_info->support_filter_match_dtype = false;
	encoder_info->support_filter_match_dsize = false;
	// comparators
	encoder_info->comparator_count = 0;
	encoder_info->support_primary_compare_iaddr = false;
	encoder_info->support_primary_compare_context = false;
	encoder_info->support_primary_compare_tval = false;
	encoder_info->support_primary_compare_daddr = false;
	encoder_info->support_primary_compare_func_equal = false;
	encoder_info->support_primary_compare_func_notequal = false;
	encoder_info->support_primary_compare_func_lessthan = false;
	encoder_info->support_primary_compare_func_lessthanorequal = false;
	encoder_info->support_primary_compare_func_greaterthan = false;
	encoder_info->support_primary_compare_func_greaterthanorequal = false;
	encoder_info->support_primary_compare_func_false = false;
	encoder_info->support_primary_compare_func_true = false;
	encoder_info->support_secondary_compare_iaddr = false;
	encoder_info->support_secondary_compare_context = false;
	encoder_info->support_secondary_compare_tval = false;
	encoder_info->support_secondary_compare_daddr = false;
	encoder_info->support_secondary_compare_func_equal = false;
	encoder_info->support_secondary_compare_func_notequal = false;
	encoder_info->support_secondary_compare_func_lessthan = false;
	encoder_info->support_secondary_compare_func_lessthanorequal = false;
	encoder_info->support_secondary_compare_func_greaterthan = false;
	encoder_info->support_secondary_compare_func_greaterthanorequal = false;
	encoder_info->support_secondary_compare_func_false = false;
	encoder_info->support_secondary_compare_func_true = false;
	encoder_info->support_compare_primary_true = false;
	encoder_info->support_compare_both_true = false;
	encoder_info->support_compare_either_false = false;
	encoder_info->support_compare_between_primary_and_secondary_true =
		false;
	encoder_info->support_compare_primary_notify_0 = false;
	encoder_info->support_compare_primary_notify_1 = false;
	encoder_info->support_compare_secondary_notify_0 = false;
	encoder_info->support_compare_secondary_notify_1 = false;
	encoder_info->filter_used = 0;
	encoder_info->comparator_used = 0;

	// for timestamp
	encoder_info->support_timestamp_run_in_debugmode = false;
	encoder_info->support_timestamp_none = false;
	encoder_info->support_timestamp_external = false;
	encoder_info->support_timestamp_internal_system = false;
	encoder_info->support_timestamp_internal_core = false;
	encoder_info->support_timestamp_shared = false;
	encoder_info->support_timestamp_vendor5 = false;
	encoder_info->support_timestamp_vendor6 = false;
	encoder_info->support_timestamp_vendor7 = false;
	encoder_info->support_timestamp_prescale_1 = false;
	encoder_info->support_timestamp_prescale_4 = false;
	encoder_info->support_timestamp_prescale_16 = false;
	encoder_info->support_timestamp_prescale_64 = false;
	encoder_info->support_timestamp_enable = false;
	encoder_info->timestamp_width = 0;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some implementations do not meet expectation(bits error)
 */
TRACE_CONTROL_LIB_API int32_t
xt_trace_detect_trace_sink(struct xt_trace_sink_control_info *sink_info)
{
	int32_t tr_sink_control = 0;
	uint32_t tr_sink_impl = 0;
	uint64_t base_addr = sink_info->base_addr;
	uint32_t async_freq_reg_offset = 0;
	char str[256] = { '\0' };

	tr_sink_control = primary_enable_trace_component(base_addr);
	if (tr_sink_control <= 0) {
		snprintf(str, sizeof(str),
			"Fail to primary enable the trace sink with base addr 0x%llx\n",
			base_addr);
		xt_trace_msgout(str);
		return tr_sink_control;
	}
	sink_info->tr_sink_control_default = tr_sink_control;

	if (xt_trace_register_read(base_addr + OFFSET_TRRAMIMPL, &tr_sink_impl))
		return -1;
	if (TR_ANY_GET_IMPL_MAIN_VER(tr_sink_impl) != XT_NTRACE_SPEC_MAIN_VER ||
	    TR_ANY_GET_IMPL_MINOR_VER(tr_sink_impl) !=
		    XT_NTRACE_SPEC_MINOR_VER) {
		sink_info->main_ver = TR_ANY_GET_IMPL_MAIN_VER(tr_sink_impl);
		sink_info->minor_ver = TR_ANY_GET_IMPL_MINOR_VER(tr_sink_impl);
		snprintf(str, sizeof(str),
			"Get unknown trace sink version %d.%d with base addr 0x%llx.\n",
			sink_info->main_ver, sink_info->minor_ver, base_addr);
		xt_trace_msgout(str);

		// disable this component
		tr_sink_control = reset_trace_component(base_addr);
		if (tr_sink_control < 0) {
			snprintf(str, sizeof(str),
				"Fail to reset the trace sink with base addr 0x%llx.\n",
				base_addr);
			xt_trace_msgout(str);
			return tr_sink_control;
		}

		return 1;
	}

	sink_info->has_detected = true;
	sink_info->main_ver = XT_NTRACE_SPEC_MAIN_VER;
	sink_info->minor_ver = XT_NTRACE_SPEC_MINOR_VER;

	// detect ram sink
	if (TR_ANY_GET_IMPL_COMP_TYPE(tr_sink_impl) == TRCOMP_RAMSINK) {
		sink_info->comp_type = TRCOMP_RAMSINK;

		sink_info->support_sram_sink =
			(tr_sink_impl & TR_RAM_IMPL_HAS_SRAM) ? true : false;
		sink_info->support_smem_sink =
			(tr_sink_impl & TR_RAM_IMPL_HAS_SMEM) ? true : false;

		// check trRamStopOnWrap1, trRamMemFormat0, trRamAsyncFreq0
		tr_sink_control = u32_set_fields(tr_sink_control, 8, 8, 1);
		tr_sink_control = u32_set_fields(tr_sink_control, 10, 9, 0);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRRAMCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 8, 8) == 1)
			sink_info->support_stop_on_wrap = true;
		if (u32_get_fields(tr_sink_control, 10, 9) == 0)
			sink_info->support_mem_format_plain_bytes = true;

		// check trRamMemFormat1, 2, 3
		tr_sink_control = u32_set_fields(tr_sink_control, 10, 9, 1);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRRAMCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 10, 9) == 1)
			sink_info->support_mem_format_reserved1 = true;
		tr_sink_control = u32_set_fields(tr_sink_control, 10, 9, 2);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRRAMCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 10, 9) == 2)
			sink_info->support_mem_format_reserved2 = true;
		tr_sink_control = u32_set_fields(tr_sink_control, 10, 9, 3);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRRAMCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 10, 9) == 3)
			sink_info->support_mem_format_custom = true;

		// check limit low max
		if (sink_info->support_sram_sink) {
			// set trRamControl.trRamMode to 0, config it in SRAM Mode
			if (u32_get_fields(tr_sink_control, 4, 4) == 1) {
				tr_sink_control = u32_set_fields(
					tr_sink_control, 4, 4, 0);
				if (tr_sink_control !=
				    xt_trace_register_readafterwrite(
					    base_addr + OFFSET_TRRAMCONTROL,
					    tr_sink_control))
					return -1;
			}

			// write limit low vs high to 0xffffffffffffffff, then read
			if (xt_trace_register_write(
				    base_addr + OFFSET_TRRAMLIMITHIGH,
				    0xffffffff))
				return -1;
			if (xt_trace_register_write(
				    base_addr + OFFSET_TRRAMLIMITLOW,
				    0xffffffff))
				return -1;
			if (xt_trace_register_read(
				    base_addr + OFFSET_TRRAMLIMITHIGH,
				    &(sink_info->limit_high_max)))
				return -1;
			if (xt_trace_register_read(base_addr +
							   OFFSET_TRRAMLIMITLOW,
						   &(sink_info->limit_low_max)))
				return -1;
		}
	}
	// detect pib sink
	else if (TR_ANY_GET_IMPL_COMP_TYPE(tr_sink_impl) == TRCOMP_PIBSINK) {
		sink_info->comp_type = TRCOMP_PIBSINK;

		async_freq_reg_offset = OFFSET_TRATBBRIDGEIMPL;

		// check trPibMode
		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_SWT_MANCHESTER);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_SWT_MANCHESTER)
			sink_info->support_pibmode_swt_machester = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_SWT_UART);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) == PIB_MODE_SWT_UART)
			sink_info->support_pibmode_swt_uart = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_TRC_1TRCDATA);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_TRC_1TRCDATA)
			sink_info->support_pibmode_trc_1trcdata = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_TRC_2TRCDATA);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_TRC_2TRCDATA)
			sink_info->support_pibmode_trc_2trcdata = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_TRC_4TRCDATA);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_TRC_4TRCDATA)
			sink_info->support_pibmode_trc_4trcdata = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_TRC_8TRCDATA);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_TRC_8TRCDATA)
			sink_info->support_pibmode_trc_8trcdata = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4,
						 PIB_MODE_TRC_16TRCDATA);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 7, 4) ==
		    PIB_MODE_TRC_16TRCDATA)
			sink_info->support_pibmode_trc_16trcdata = true;

		// check trPibClkCenter
		tr_sink_control = u32_set_fields(tr_sink_control, 8, 8, 0);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 8, 8) == 0)
			sink_info->support_pibclk_center_0 = true;

		tr_sink_control = u32_set_fields(tr_sink_control, 8, 8, 1);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 8, 8) == 1)
			sink_info->support_pibclk_center_1 = true;

		// check trPibCalibrate
		tr_sink_control = u32_set_fields(tr_sink_control, 9, 9, 1);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		if (u32_get_fields(tr_sink_control, 9, 9) == 1) {
			tr_sink_control =
				u32_set_fields(tr_sink_control, 9, 9, 0);
			if (xt_trace_register_write(base_addr +
							    OFFSET_TRPIBCONTROL,
						    tr_sink_control))
				return -1;
			sink_info->support_pibcalibrate = true;
		}

		// check trPibDivider
		tr_sink_control = u32_set_fields(tr_sink_control, 31, 16, 0);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		sink_info->support_pibdivider_min =
			u32_get_fields(tr_sink_control, 31, 16);

		tr_sink_control =
			u32_set_fields(tr_sink_control, 31, 16, 0xffff);
		tr_sink_control = xt_trace_register_readafterwrite(
			base_addr + OFFSET_TRPIBCONTROL, tr_sink_control);
		if (tr_sink_control == -1)
			return -1;
		sink_info->support_pibdivider_max =
			u32_get_fields(tr_sink_control, 31, 16);
	}
	// detect atb bridge
	else if (TR_ANY_GET_IMPL_COMP_TYPE(tr_sink_impl) == TRCOMP_ATBBRIDGE) {
		sink_info->comp_type = TRCOMP_ATBBRIDGE;
	} else {
		// unknown sink msg
		snprintf(str, sizeof(str),
			"This component is not a trace sink as type in impliment is %d with base addr 0x%llx.\n",
			TR_ANY_GET_IMPL_COMP_TYPE(tr_sink_impl), base_addr);
		xt_trace_msgout(str);

		// disable this component
		tr_sink_control = reset_trace_component(base_addr);
		if (tr_sink_control < 0) {
			snprintf(str, sizeof(str),
				"Failed to disable the component with addr 0x%llx.\n",
				base_addr);
			xt_trace_msgout(str);
			return tr_sink_control;
		}

		return 1;
	}

	// check async freq
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 0);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 0)
		sink_info->support_async_freq_0 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 1);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 1)
		sink_info->support_async_freq_1 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 2);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 2)
		sink_info->support_async_freq_2 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 3);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 3)
		sink_info->support_async_freq_3 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 4);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 4)
		sink_info->support_async_freq_4 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 5);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 5)
		sink_info->support_async_freq_5 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 6);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 6)
		sink_info->support_async_freq_6 = true;
	tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, 7);
	tr_sink_control = xt_trace_register_readafterwrite(
		base_addr + async_freq_reg_offset, tr_sink_control);
	if (tr_sink_control == -1)
		return -1;
	if (u32_get_fields(tr_sink_control, 14, 12) == 7)
		sink_info->support_async_freq_7 = true;

	// disable this component
	tr_sink_control = reset_trace_component(base_addr);
	if (tr_sink_control < 0) {
		snprintf(str, sizeof(str),
			"Failed to disable the trace sink with addr 0x%llx after detection.\n",
			base_addr);
		xt_trace_msgout(str);
		return tr_sink_control;
	}

	return 0;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some implementations do not meet expectation(bits error)
 */
TRACE_CONTROL_LIB_API int32_t xt_trace_detect_trace_encoder(
	struct xt_trace_encoder_control_info *encoder_info)
{
	char str[256];
	int32_t tr_encoder_control = 0;
	uint32_t tr_encoder_impl = 0;
	uint32_t tr_te_inst_features = 0;
	uint64_t base_addr = encoder_info->base_addr;

	tr_encoder_control = primary_enable_trace_component(base_addr);
	if (tr_encoder_control <= 0) {
		snprintf(str, sizeof(str),
			"Fail to primary enable encoder with base addr 0x%llx\n",
			base_addr);
		xt_trace_msgout(str);
		return tr_encoder_control;
	}

	if (xt_trace_register_read(base_addr + OFFSET_TRENCODERIMPL,
				   &tr_encoder_impl))
		return -1;

	if (TR_ANY_GET_IMPL_COMP_TYPE(tr_encoder_impl) != TRCOMP_ENCODER) {
		// msg
		snprintf(str, sizeof(str),
			"This component is not a trace encoder as type in impliment is %d with base addr 0x%llx.\n",
			TR_ANY_GET_IMPL_COMP_TYPE(tr_encoder_impl), base_addr);
		xt_trace_msgout(str);

		// disable this component
		tr_encoder_control = reset_trace_component(base_addr);
		if (tr_encoder_control < 0) {
			snprintf(str, sizeof(str),
				"Fail to disable the component with base addr 0x%llx\n",
				base_addr);
			xt_trace_msgout(str);
			return tr_encoder_control;
		}

		return 1;
	}

	if (TR_ANY_GET_IMPL_MAIN_VER(tr_encoder_impl) !=
		    XT_NTRACE_SPEC_MAIN_VER ||
	    TR_ANY_GET_IMPL_MINOR_VER(tr_encoder_impl) !=
		    XT_NTRACE_SPEC_MINOR_VER) {
		// get unknown version
		encoder_info->main_ver =
			TR_ANY_GET_IMPL_MAIN_VER(tr_encoder_impl);
		encoder_info->minor_ver =
			TR_ANY_GET_IMPL_MINOR_VER(tr_encoder_impl);
		snprintf(str, sizeof(str),
			"Get unknown encoder version %d.%d with base addr 0x%llx.\n",
			encoder_info->main_ver, encoder_info->minor_ver,
			base_addr);
		xt_trace_msgout(str);

		// disable this component
		tr_encoder_control = reset_trace_component(base_addr);
		if (tr_encoder_control < 0) {
			snprintf(str, sizeof(str),
				"Fail to reset encoder with base addr 0x%llx\n",
				base_addr);
			xt_trace_msgout(str);
			return tr_encoder_control;
		}

		// msg
		return 1;
	}

	// save trTeControl
	encoder_info->tr_te_control_default = tr_encoder_control;

	encoder_info->has_detected = true;
	encoder_info->main_ver = XT_NTRACE_SPEC_MAIN_VER;
	encoder_info->minor_ver = XT_NTRACE_SPEC_MINOR_VER;
	encoder_info->protocol_main_ver =
		u32_get_fields(tr_encoder_control, 19, 16);
	encoder_info->protocol_minor_ver =
		u32_get_fields(tr_encoder_control, 23, 20);

	// check trTeInstMode
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 0)
		encoder_info->support_inst_mode_disable = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 1)
		encoder_info->support_inst_mode_reserved1 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 2);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 2)
		encoder_info->support_inst_mode_reserved2 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 3);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 3)
		encoder_info->support_inst_mode_branch_trace = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 4);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 4)
		encoder_info->support_inst_mode_reserved4 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 5);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 5)
		encoder_info->support_inst_mode_reserved5 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 6);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 6)
		encoder_info->support_inst_mode_branch_history = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4, 7);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 6, 4) == 7)
		encoder_info->support_inst_mode_vendor = true;

	// check trTeContext
	tr_encoder_control = u32_set_fields(tr_encoder_control, 9, 9, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 9, 9) == 0)
		encoder_info->support_context_0 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 9, 9, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 9, 9) == 1)
		encoder_info->support_context_1 = true;

	// check trTeInstTrigEnable
	tr_encoder_control = u32_set_fields(tr_encoder_control, 11, 11, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 11, 11) == 1)
		encoder_info->support_inst_trigger_enable = true;

	// check trTeInstStallEna
	tr_encoder_control = u32_set_fields(tr_encoder_control, 13, 13, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 13, 13) == 0)
		encoder_info->support_inst_stall_ena_0 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 13, 13, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 13, 13) == 1)
		encoder_info->support_inst_stall_ena_1 = true;

	// check trTeInhibitSrc
	tr_encoder_control = u32_set_fields(tr_encoder_control, 15, 15, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 15, 15) == 0)
		encoder_info->support_inhibit_src_0 = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 15, 15, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 15, 15) == 1)
		encoder_info->support_inhibit_src_1 = true;

	// check trTeInstSyncMode
	tr_encoder_control = u32_set_fields(tr_encoder_control, 17, 16, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 17, 16) == 0)
		encoder_info->support_inst_sync_mode_off = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 17, 16, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 17, 16) == 1)
		encoder_info->support_inst_sync_mode_count_message = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 17, 16, 2);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 17, 16) == 2)
		encoder_info->support_inst_sync_mode_count_hart_clock = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 17, 16, 3);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 17, 16) == 3)
		encoder_info->support_inst_sync_mode_count_instruction = true;

	// check trTeInstSyncMax
	tr_encoder_control = u32_set_fields(tr_encoder_control, 23, 20, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	encoder_info->inst_sync_min =
		u32_get_fields(tr_encoder_control, 23, 20);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 23, 20, 0xf);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	encoder_info->inst_sync_max =
		u32_get_fields(tr_encoder_control, 23, 20);

	// check trTeFormat
	tr_encoder_control = u32_set_fields(tr_encoder_control, 26, 24, 0);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 26, 24) == 0)
		encoder_info->support_format_etrace = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 26, 24, 1);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 26, 24) == 1)
		encoder_info->support_format_ntrace = true;
	tr_encoder_control = u32_set_fields(tr_encoder_control, 26, 24, 7);
	tr_encoder_control = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRENCODERCONTROL, tr_encoder_control);
	if (tr_encoder_control == -1)
		return -1;
	if (u32_get_fields(tr_encoder_control, 26, 24) == 7)
		encoder_info->support_format_vendor_trace = true;
	// clear tr_encoder_control
	if (xt_trace_register_write(base_addr + OFFSET_TRENCODERCONTROL,
				    encoder_info->tr_te_control_default))
		return -1;

	// check trTeInstFeatures
	if (xt_trace_register_read(base_addr + OFFSET_TRTEINSTFEATURES,
				   &tr_te_inst_features))
		return -1;
	encoder_info->default_src_id =
		u32_get_fields(tr_te_inst_features, 27, 16);
	encoder_info->default_src_bits =
		u32_get_fields(tr_te_inst_features, 31, 28);
	tr_te_inst_features |= 0x73f;
	tr_te_inst_features = xt_trace_register_readafterwrite(
		base_addr + OFFSET_TRTEINSTFEATURES, tr_te_inst_features);
	if (tr_te_inst_features == -1)
		return -1;
	if (u32_get_fields(tr_te_inst_features, 0, 0) == 1)
		encoder_info->support_inst_no_addr_diff = true;
	if (u32_get_fields(tr_te_inst_features, 1, 1) == 1)
		encoder_info->support_inst_no_trap_addr = true;
	if (u32_get_fields(tr_te_inst_features, 2, 2) == 1)
		encoder_info->support_inst_en_sequential_tail_jump = true;
	if (u32_get_fields(tr_te_inst_features, 3, 3) == 1)
		encoder_info->support_inst_en_implicit_return = true;
	if (u32_get_fields(tr_te_inst_features, 4, 4) == 1)
		encoder_info->support_inst_en_branch_prediction = true;
	if (u32_get_fields(tr_te_inst_features, 5, 5) == 1)
		encoder_info->support_inst_en_jump_target_cache = true;
	encoder_info->support_inst_implicit_return_mode =
		u32_get_fields(tr_te_inst_features, 7, 6);
	if (u32_get_fields(tr_te_inst_features, 8, 8) == 1)
		encoder_info->support_inst_en_repeated_history = true;
	if (u32_get_fields(tr_te_inst_features, 9, 9) == 1)
		encoder_info->support_inst_en_all_jumps = true;
	if (u32_get_fields(tr_te_inst_features, 10, 10) == 1)
		encoder_info->support_inst_extend_addr_msb = true;
	// clear features
	tr_te_inst_features = u32_set_fields(tr_te_inst_features, 15, 0, 0);
	if (xt_trace_register_write(base_addr + OFFSET_TRTEINSTFEATURES,
				    tr_te_inst_features))
		return -1;

	// disable this component
	tr_encoder_control = reset_trace_component(base_addr);
	if (tr_encoder_control < 0) {
		snprintf(str, sizeof(str),
			"Failed to disable the trace encoder with addr 0x%llx after detection.\n",
			base_addr);
		xt_trace_msgout(str);
		return tr_encoder_control;
	}

	return 0;
}
