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
