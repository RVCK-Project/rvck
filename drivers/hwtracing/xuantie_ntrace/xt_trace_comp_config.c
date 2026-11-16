// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/memory.h>
#include "xt_ntrace_control_interface.h"

struct xt_trace_encoder_filter_config_info *
xt_trace_add_insn_filter(struct xt_trace_encoder_control_info *encoder_info,
			 bool filter_priv, uint8_t priv, bool filter_start_addr,
			 uint64_t start_addr, bool filter_end_addr,
			 uint64_t end_addr)
{
	return NULL;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some configurations do not meet expectation(bits error)
 */
TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_config(struct xt_trace_sink_control_info *sink_info, struct xt_trace_sink_config_info *config_info)
{
	int32_t tr_sink_control = 0;
	int32_t read_value_tmp = 0;
	char str[256] = { '\0' };

	if (sink_info->comp_type != config_info->component_type) {
		snprintf(str, sizeof(str), "Error: sink_info with type %d and config_info with type %d.\n", sink_info->comp_type, config_info->component_type);
		xt_trace_msgout(str);
		return 1;
	}

	if (sink_info->has_detected) {
		// check cfg for ram sink
		if (config_info->component_type == TRCOMP_RAMSINK) {
			// type
			if ((config_info->type == TRACE_SRAM_SINK && sink_info->support_sram_sink == false) || (config_info->type == TRACE_SMEM_SINK && sink_info->support_smem_sink == false)) {
				snprintf(str, sizeof(str), "Error: sink configs sink type is %s, sink(0x%llx) supports sram sink %s, supports smem_sink %s.\n",
					config_info->type == TRACE_SRAM_SINK ? "sram sink" : "smem sink",
					sink_info->base_addr,
					sink_info->support_sram_sink ? "true" : "false",
					sink_info->support_smem_sink ? "true" : "false");
				xt_trace_msgout(str);
				return 1;
			}

			// ram_sink_stop_on_wrap
			if (config_info->ram_sink_stop_on_wrap == true && sink_info->support_stop_on_wrap == false) {
				snprintf(str, sizeof(str), "Error: sink configs ram_sink_stop_on_wrap true, but the sink(0x%llx) does not support.\n", sink_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}

			// ram_sink_mem_format
			if ((config_info->ram_sink_mem_format == 0 && sink_info->support_mem_format_plain_bytes == false) || (config_info->ram_sink_mem_format == 3 && sink_info->support_mem_format_custom == false)) {
				snprintf(str, sizeof(str), "Error: sink configs ram_sink_mem_format is %d, sink(0x%llx) supports ram_sink_mem_format 0 %s, supports ram_sink_mem_format 3 %s.\n",
					config_info->ram_sink_mem_format,
					sink_info->base_addr,
					sink_info->support_mem_format_plain_bytes ? "true" : "false",
					sink_info->support_mem_format_custom ? "true" : "false");
				xt_trace_msgout(str);
				return 1;
			} else if (config_info->ram_sink_mem_format != 0 && config_info->ram_sink_mem_format != 3) {
				snprintf(str, sizeof(str), "Error: unknown sink configs ram_sink_mem_format %d for sink(0x%llx).\n", config_info->ram_sink_mem_format, sink_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		} else if (config_info->component_type == TRCOMP_PIBSINK) {
			// pin_sink_mode
			if ((config_info->pib_sink_mode == PIB_MODE_SWT_MANCHESTER && sink_info->support_pibmode_swt_machester == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_SWT_UART && sink_info->support_pibmode_swt_uart == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_TRC_1TRCDATA && sink_info->support_pibmode_trc_1trcdata == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_TRC_2TRCDATA && sink_info->support_pibmode_trc_2trcdata == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_TRC_4TRCDATA && sink_info->support_pibmode_trc_4trcdata == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_TRC_8TRCDATA && sink_info->support_pibmode_trc_8trcdata == false) ||
			    (config_info->pib_sink_mode == PIB_MODE_TRC_16TRCDATA && sink_info->support_pibmode_trc_16trcdata == false)) {
				snprintf(str, sizeof(str), "Error: pib sink configs pib_sink_mode %d,sink(0x%llx) supports mode pibmode_swt_machester %s, swt_uart %s, 1trcdata %s, 2trcdata %s, 4trcdata %s, 8trcdata %s, 16trcdata %s.\n",
					config_info->pib_sink_mode,
					sink_info->base_addr,
					sink_info->support_pibmode_swt_machester ? "true" : "false",
					sink_info->support_pibmode_swt_machester ? "true" : "false",
					sink_info->support_pibmode_trc_1trcdata ? "true" : "false",
					sink_info->support_pibmode_trc_2trcdata ? "true" : "false",
					sink_info->support_pibmode_trc_4trcdata ? "true" : "false",
					sink_info->support_pibmode_trc_8trcdata ? "true" : "false",
					sink_info->support_pibmode_trc_16trcdata ? "true" : "false");
				xt_trace_msgout(str);
				return 1;
			} else if (config_info->pib_sink_mode != PIB_MODE_SWT_MANCHESTER &&
			    config_info->pib_sink_mode != PIB_MODE_SWT_UART &&
			    config_info->pib_sink_mode != PIB_MODE_TRC_1TRCDATA &&
			    config_info->pib_sink_mode != PIB_MODE_TRC_2TRCDATA &&
			    config_info->pib_sink_mode != PIB_MODE_TRC_4TRCDATA &&
			    config_info->pib_sink_mode != PIB_MODE_TRC_8TRCDATA &&
			    config_info->pib_sink_mode != PIB_MODE_TRC_16TRCDATA) {
				snprintf(str, sizeof(str), "Error: unsupported pib_sink_mode %d.\n", config_info->pib_sink_mode);
				xt_trace_msgout(str);
				return 1;
			}

			// pib_sink_clk_center
			if ((config_info->pib_sink_clk_center == 0 && sink_info->support_pibclk_center_0 == false) ||
			    (config_info->pib_sink_clk_center == 1 && sink_info->support_pibclk_center_1 == false)) {
				snprintf(str, sizeof(str), "Error: sink configs pib_sink_clk_center %d, but the sink(0x%llx) does not support.\n", config_info->pib_sink_clk_center, sink_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}

			// pib_sink_divider
			if (config_info->pib_sink_divider < sink_info->support_pibdivider_min) {
				snprintf(str, sizeof(str), "Error: sink configs pib_sink_divider 0x%x which is lower than sink(0x%llx) supported pibdivider_min 0x%x.\n",
					config_info->pib_sink_divider, sink_info->base_addr, sink_info->support_pibdivider_min);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->pib_sink_divider > sink_info->support_pibdivider_max) {
				snprintf(str, sizeof(str), "Error: sink configs pib_sink_divider 0x%x which is bigger than sink(0x%llx) supported pibdivider_max 0x%x.\n",
					config_info->pib_sink_divider, sink_info->base_addr, sink_info->support_pibdivider_max);
				xt_trace_msgout(str);
				return 1;
			}
		} else if (config_info->component_type == TRCOMP_ATBBRIDGE) {
			snprintf(str, sizeof(str), "Warn: component_type = TRCOMP_ATBBRIDGE.\n");
			xt_trace_msgout(str);
		}

		// config async freq
		if (config_info->sink_async_freq > 7) {
			snprintf(str, sizeof(str), "Error: sink configs sink_async_freq %d which is bigger than 7.\n", config_info->sink_async_freq);
			xt_trace_msgout(str);
			return 1;
		} else if ((config_info->sink_async_freq == 0 && sink_info->support_async_freq_0 == false) ||
			    (config_info->sink_async_freq == 1 && sink_info->support_async_freq_1 == false) ||
			    (config_info->sink_async_freq == 2 && sink_info->support_async_freq_2 == false) ||
			    (config_info->sink_async_freq == 3 && sink_info->support_async_freq_3 == false) ||
			    (config_info->sink_async_freq == 4 && sink_info->support_async_freq_4 == false) ||
			    (config_info->sink_async_freq == 5 && sink_info->support_async_freq_5 == false) ||
			    (config_info->sink_async_freq == 6 && sink_info->support_async_freq_6 == false) ||
			    (config_info->sink_async_freq == 7 && sink_info->support_async_freq_7 == false)) {
			snprintf(str, sizeof(str), "Error: sink configs sink_async_freq %d, but the sink(0x%llx) does not support.\n", config_info->sink_async_freq, sink_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
	}

	// enable component
	tr_sink_control = primary_enable_trace_component(sink_info->base_addr);
	if (tr_sink_control <= 0) {
		snprintf(str, sizeof(str), "Failed to primary enable trace sink with base addr 0x%llx.\n", sink_info->base_addr);
		xt_trace_msgout(str);
		return tr_sink_control;
	}

	if (config_info->component_type == TRCOMP_RAMSINK) {
		// configure trRamControl
		tr_sink_control = u32_set_fields(tr_sink_control, 4, 4, config_info->type);
		tr_sink_control = u32_set_fields(tr_sink_control, 8, 8, config_info->ram_sink_stop_on_wrap ? 1 : 0);
		tr_sink_control = u32_set_fields(tr_sink_control, 10, 9, config_info->ram_sink_mem_format);
		tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, config_info->sink_async_freq);
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr, tr_sink_control);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != tr_sink_control) {
			snprintf(str, sizeof(str), "Config trRamControl(0x%llx) 0x%x for ram sink get 0x%x.\n", sink_info->base_addr, tr_sink_control, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}

		// config region
		if (config_info->type == TRACE_SMEM_SINK) {
			read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMSTARTLOW, (uint32_t)config_info->ram_sink_start);
			if (read_value_tmp == -1) {
				snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
				xt_trace_msgout(str);
				return -1;
			}
			if (read_value_tmp != (uint32_t)config_info->ram_sink_start) {
				snprintf(str, sizeof(str), "Config trRamStartLow(0x%llx) 0x%x for smem sink get 0x%x.\n", sink_info->base_addr, (uint32_t)config_info->ram_sink_start, read_value_tmp);
				xt_trace_msgout(str);
				goto config_failed;
			}
		}
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMSTARTHIGH, (uint32_t)(config_info->ram_sink_start >> 32));
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != (uint32_t)(config_info->ram_sink_start >> 32)) {
			snprintf(str, sizeof(str), "Config trRamStartHigh(0x%llx) 0x%x for smem sink get 0x%x.\n", sink_info->base_addr, (uint32_t)(config_info->ram_sink_start >> 32), read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMLIMITLOW, (uint32_t)config_info->ram_sink_limit);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		/* FIXME: the H100 bit will get error here. */
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMLIMITHIGH, (uint32_t)(config_info->ram_sink_limit >> 32));
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != (uint32_t)(config_info->ram_sink_limit >> 32)) {
			snprintf(str, sizeof(str), "Config trRamStartLimitHigh(0x%llx) 0x%x for smem sink get 0x%x.\n", sink_info->base_addr, (uint32_t)(config_info->ram_sink_limit >> 32), read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMWPLOW, (uint32_t)config_info->ram_sink_write_point);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != (uint32_t)config_info->ram_sink_write_point) {
			snprintf(str, sizeof(str), "Config trRamWpLow(0x%llx) 0x%x for smem sink get 0x%x.\n", sink_info->base_addr, (uint32_t)config_info->ram_sink_write_point, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRRAMWPHIGH, (uint32_t)(config_info->ram_sink_write_point >> 32));
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != (uint32_t)(config_info->ram_sink_write_point >> 32)) {
			snprintf(str, sizeof(str), "Config trRamWpHigh(0x%llx) 0x%x for smem sink get 0x%x.\n", sink_info->base_addr, (uint32_t)(config_info->ram_sink_write_point >> 32), read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	} else if (config_info->component_type == TRCOMP_PIBSINK) {
		// config trPibControl
		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4, config_info->pib_sink_mode);
		tr_sink_control = u32_set_fields(tr_sink_control, 8, 8, config_info->pib_sink_clk_center);
		tr_sink_control = u32_set_fields(tr_sink_control, 14, 12, config_info->sink_async_freq);
		tr_sink_control = u32_set_fields(tr_sink_control, 31, 16, config_info->pib_sink_divider);
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr, tr_sink_control);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != tr_sink_control) {
			snprintf(str, sizeof(str), "Config trPibControl(0x%llx) 0x%x for pib sink get 0x%x.\n", sink_info->base_addr, tr_sink_control, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	} else if (config_info->component_type == TRCOMP_ATBBRIDGE) {
		// config trAtbBridgeControl:
		tr_sink_control = u32_set_fields(tr_sink_control, 7, 4, config_info->atb_bridge_id);
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr, tr_sink_control);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (read_value_tmp != tr_sink_control) {
			snprintf(str, sizeof(str), "Config trAtbBridgeControl(0x%llx) 0x%x for atb bridge get 0x%x.\n", sink_info->base_addr, tr_sink_control, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trAtbBridgeImpl
		read_value_tmp = xt_trace_register_readafterwrite(sink_info->base_addr + OFFSET_TRATBBRIDGEIMPL, config_info->sink_async_freq);
		if (read_value_tmp == -1) {
			snprintf(str, sizeof(str), "Failed xt_trace_register_readafterwrite_%d 0x%x.\n", __LINE__, read_value_tmp);
			xt_trace_msgout(str);
			return -1;
		}
		if (u32_get_fields(read_value_tmp, 14, 12) != config_info->sink_async_freq) {
			snprintf(str, sizeof(str), "Config trAtbBridgeImpl(0x%llx) 0x%x for atb bridge get 0x%x.\n", sink_info->base_addr, config_info->sink_async_freq, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}

	return 0;

config_failed:
	// disable component
	tr_sink_control = reset_trace_component(sink_info->base_addr);
	if (tr_sink_control < 0) {
		snprintf(str, sizeof(str), "Failed to disable trace sink with base addr 0x%llx.\n", sink_info->base_addr);
		xt_trace_msgout(str);
		return tr_sink_control;
	}

	snprintf(str, sizeof(str), "Failed to xt_trace_sink_config.\n");
	xt_trace_msgout(str);
	return 1;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some configurations do not meet expectation(bits error)
 */
int32_t
xt_trace_funnel_config(struct xt_trace_funnel_control_info *funnel_info,
		       struct xt_trace_funnel_config_info *config_info)
{
	char str[256] = { '\0' };
	int32_t tr_funnel_control = 0;
	int32_t read_value_tmp = 0;

	if (funnel_info->has_detected) {
		// FIXME: input count means ???
		if (funnel_info->input_count & config_info->disable_input) {
			snprintf(str, sizeof(str),
				"Error: disable input 0x%x for trace funnl(0x%llx) with input %d.\n",
				config_info->disable_input,
				funnel_info->base_addr,
				funnel_info->input_count);
			xt_trace_msgout(str);
			return 1;
		}
	}

	// enable component
	tr_funnel_control =
		primary_enable_trace_component(funnel_info->base_addr);
	if (tr_funnel_control <= 0) {
		snprintf(str, sizeof(str),
			"Failed to primary enable the trace funnl with base addr 0x%llx.\n",
			funnel_info->base_addr);
		xt_trace_msgout(str);
		return tr_funnel_control;
	}

	read_value_tmp = xt_trace_register_readafterwrite(
		funnel_info->base_addr + OFFSET_TRFUNNELDISINPUT,
		config_info->disable_input);
	if (read_value_tmp == -1)
		return -1;
	if (read_value_tmp != config_info->disable_input) {
		// msg
		snprintf(str, sizeof(str),
			"Error: write trFunnelDisinput with 0x%x and get 0x%x with base addr 0x%llx.\n",
			config_info->disable_input, read_value_tmp,
			funnel_info->base_addr);
		xt_trace_msgout(str);

		// disable component
		tr_funnel_control =
			reset_trace_component(funnel_info->base_addr);
		if (tr_funnel_control < 0) {
			snprintf(str, sizeof(str),
				"Fail to disable the trace funnel with base addr 0x%llx.\n",
				funnel_info->base_addr);
			xt_trace_msgout(str);
			return tr_funnel_control;
		}

		return 1;
	}

	return 0;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some configurations do not meet expectation(bits error)
 */
int32_t
xt_trace_encoder_config(struct xt_trace_encoder_control_info *encoder_info,
			struct xt_trace_encoder_config_info *config_info)
{
	char str[256] = { '\0' };
	int32_t tr_encoder_control = 0;
	uint32_t tr_te_inst_features = 0;
	uint32_t read_value_tmp = 0;

	if (encoder_info->has_detected) {
		// check inst mode
		if ((config_info->inst_mode == 0 &&
		     !encoder_info->support_inst_mode_disable) ||
		    (config_info->inst_mode == 1 &&
		     !encoder_info->support_inst_mode_reserved1) ||
		    (config_info->inst_mode == 2 &&
		     !encoder_info->support_inst_mode_reserved2) ||
		    (config_info->inst_mode == 3 &&
		     !encoder_info->support_inst_mode_branch_trace) ||
		    (config_info->inst_mode == 4 &&
		     !encoder_info->support_inst_mode_reserved4) ||
		    (config_info->inst_mode == 5 &&
		     !encoder_info->support_inst_mode_reserved5) ||
		    (config_info->inst_mode == 6 &&
		     !encoder_info->support_inst_mode_branch_history) ||
		    (config_info->inst_mode == 7 &&
		     !encoder_info->support_inst_mode_vendor)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_mode %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_mode,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check send_context
		if ((config_info->send_context == false &&
		     !encoder_info->support_context_0) ||
		    (config_info->send_context == true &&
		     !encoder_info->support_context_1)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs send_context %d, but the encoder(0x%llx) does not support.\n",
				config_info->send_context,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check enable inst trigger enable
		if (config_info->inst_trigger_enable == true &&
		    !encoder_info->support_inst_trigger_enable) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_trigger_enable %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_trigger_enable,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check inst stall ena
		if ((config_info->inst_stall_ena == false &&
		     !encoder_info->support_inst_stall_ena_0) ||
		    (config_info->inst_stall_ena == true &&
		     !encoder_info->support_inst_stall_ena_1)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_stall_ena %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_stall_ena,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check inhibit src
		if ((config_info->inhibit_src == false &&
		     !encoder_info->support_inhibit_src_0) ||
		    (config_info->inhibit_src == true &&
		     !encoder_info->support_inhibit_src_1)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inhibit_src %d, but the encoder(0x%llx) does not support.\n",
				config_info->inhibit_src,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check inst sync mode
		if ((config_info->inst_sync_mode == 0 &&
		     !encoder_info->support_inst_sync_mode_off) ||
		    (config_info->inst_sync_mode == 1 &&
		     !encoder_info->support_inst_sync_mode_count_message) ||
		    (config_info->inst_sync_mode == 2 &&
		     !encoder_info->support_inst_sync_mode_count_hart_clock) ||
		    (config_info->inst_sync_mode == 3 &&
		     !encoder_info->support_inst_sync_mode_count_instruction)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_sync_mode %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_sync_mode,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check inst sync max
		if (config_info->inst_sync_max < encoder_info->inst_sync_min) {
			// msg: modify config_info->inst_sync_max to encoder_info->inst_sync_min
			config_info->inst_sync_max =
				encoder_info->inst_sync_min;
		} else if (config_info->inst_sync_max >
			   encoder_info->inst_sync_max) {
			// msg: modify config_info->inst_sync_max to encoder_info->inst_sync_max
			config_info->inst_sync_max =
				encoder_info->inst_sync_max;
		}
		// check record_format
		if ((config_info->record_format == 0 &&
		     !encoder_info->support_format_etrace) ||
		    (config_info->record_format == 1 &&
		     !encoder_info->support_format_ntrace) ||
		    (config_info->record_format == 7 &&
		     !encoder_info->support_format_vendor_trace)) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs record_format %d, but the encoder(0x%llx) does not support.\n",
				config_info->record_format,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		} else if (config_info->record_format != 0 &&
			   config_info->record_format != 1 &&
			   config_info->record_format != 7) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs an unknown record_format %d for the encoder(0x%llx).\n",
				config_info->record_format,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check feature
		if (config_info->inst_no_addr_diff &&
		    encoder_info->support_inst_no_addr_diff == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_no_addr_diff %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_no_addr_diff,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_no_trap_addr &&
		    encoder_info->support_inst_no_trap_addr == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_no_trap_addr %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_no_trap_addr,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_en_sequential_tail_jump &&
		    encoder_info->support_inst_en_sequential_tail_jump ==
			    false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_sequential_tail_jump %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_sequential_tail_jump,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if ((config_info->inst_en_implicit_return &&
		     encoder_info->support_inst_en_implicit_return == false) ||
		    (config_info->inst_en_implicit_return &&
		     (config_info->inst_implicit_return_mode !=
		      encoder_info->support_inst_implicit_return_mode))) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_implicit_return %d and mode %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_sequential_tail_jump,
				config_info->inst_implicit_return_mode,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_en_branch_prediction &&
		    encoder_info->support_inst_en_branch_prediction == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_branch_prediction %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_branch_prediction,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_en_jump_target_cache &&
		    encoder_info->support_inst_en_jump_target_cache == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_jump_target_cache %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_jump_target_cache,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_en_repeated_history &&
		    encoder_info->support_inst_en_repeated_history == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_repeated_history %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_repeated_history,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_en_all_jumps &&
		    encoder_info->support_inst_en_all_jumps == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_en_all_jumps %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_en_all_jumps,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->inst_extend_addr_msb &&
		    encoder_info->support_inst_extend_addr_msb == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_extend_addr_msb %d, but the encoder(0x%llx) does not support.\n",
				config_info->inst_extend_addr_msb,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		// check data trace
		if (config_info->data_trace_enable &&
		    encoder_info->data_trace_implemented == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs data_trace_enable %d, but the encoder(0x%llx) does not support.\n",
				config_info->data_trace_enable,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return -1;
		} else if (config_info->data_trace_enable) {
			if (config_info->data_trigger_enable &&
			    encoder_info->support_data_trace_tigger_enable ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_trigger_enable %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_trigger_enable,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_stall_enable &&
			    encoder_info->support_data_trace_stall_enable ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_stall_enable %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_stall_enable,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_drop_enable &&
			    encoder_info->support_data_trace_drop_enable ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_drop_enable %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_drop_enable,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_no_value &&
			    encoder_info->support_data_trace_no_value_1 ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_no_value %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_no_value,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_no_value == false &&
			    encoder_info->support_data_trace_no_value_0 ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_no_value %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_no_value,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_no_addr &&
			    encoder_info->support_data_trace_no_addr_1 ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_no_addr %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_no_addr,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if (config_info->data_no_addr == false &&
			    encoder_info->support_data_trace_no_addr_0 ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_no_addr %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_no_addr,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->data_addr_compress == 0 &&
			     encoder_info->support_data_trace_addr_compress_full_addr ==
				     false) ||
			    (config_info->data_addr_compress == 1 &&
			     encoder_info->support_data_trace_addr_compress_xor ==
				     false) ||
			    (config_info->data_addr_compress == 2 &&
			     encoder_info->support_data_trace_addr_compress_diff ==
				     false) ||
			    (config_info->data_addr_compress == 3 &&
			     encoder_info->support_data_trace_addr_compress_dynamic ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs data_addr_compress %d, but the encoder(0x%llx) does not support.\n",
					config_info->data_addr_compress,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		}
		// check timestamp
		if (encoder_info->support_timestamp_external == false &&
		    encoder_info->support_timestamp_internal_system == false &&
		    encoder_info->support_timestamp_internal_core == false &&
		    encoder_info->support_timestamp_shared == false &&
		    encoder_info->support_timestamp_vendor5 == false &&
		    encoder_info->support_timestamp_vendor6 == false &&
		    encoder_info->support_timestamp_vendor7 == false &&
		    config_info->timestamp_enable) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs timestamp_enable true, but the encoder(0x%llx) does not support.\n",
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		} else if (config_info->timestamp_enable) {
			if (config_info->timestamp_run_in_debugmode &&
			    encoder_info->support_timestamp_run_in_debugmode ==
				    false) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs timestamp_run_in_debugmode %d, but the encoder(0x%llx) does not support.\n",
					config_info->timestamp_run_in_debugmode,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			} else if ((config_info->timestamp_type == 0 &&
				    encoder_info->support_timestamp_none ==
					    false) ||
				   (config_info->timestamp_type == 1 &&
				    encoder_info->support_timestamp_external ==
					    false) ||
				   (config_info->timestamp_type == 2 &&
				    encoder_info->support_timestamp_internal_system ==
					    false) ||
				   (config_info->timestamp_type == 3 &&
				    encoder_info->support_timestamp_internal_core ==
					    false) ||
				   (config_info->timestamp_type == 4 &&
				    encoder_info->support_timestamp_shared ==
					    false) ||
				   (config_info->timestamp_type == 5 &&
				    encoder_info->support_timestamp_vendor5 ==
					    false) ||
				   (config_info->timestamp_type == 6 &&
				    encoder_info->support_timestamp_vendor6 ==
					    false) ||
				   (config_info->timestamp_type == 7 &&
				    encoder_info->support_timestamp_vendor7 ==
					    false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs timestamp_type %d, but the encoder(0x%llx) does not support.\n",
					config_info->timestamp_type,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			} else if ((config_info->timestamp_prescale == 0 &&
				    encoder_info->support_timestamp_prescale_1 ==
					    false) ||
				   (config_info->timestamp_prescale == 1 &&
				    encoder_info->support_timestamp_prescale_4 ==
					    false) ||
				   (config_info->timestamp_prescale == 2 &&
				    encoder_info->support_timestamp_prescale_16 ==
					    false) ||
				   (config_info->timestamp_prescale == 3 &&
				    encoder_info->support_timestamp_prescale_64 ==
					    false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs timestamp_prescale %d, but the encoder(0x%llx) does not support.\n",
					config_info->timestamp_prescale,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		}
	}

	// enable component
	tr_encoder_control =
		primary_enable_trace_component(encoder_info->base_addr);
	if (tr_encoder_control <= 0) {
		snprintf(str, sizeof(str), "Fail to enable the encoder(0x%llx).\n",
			(u64)encoder_info->base_addr);
		xt_trace_msgout(str);
		return tr_encoder_control;
	}

	// config trTeControl
	tr_encoder_control = u32_set_fields(tr_encoder_control, 6, 4,
					    config_info->inst_mode);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 9, 9,
					    config_info->send_context ? 1 : 0);
	tr_encoder_control =
		u32_set_fields(tr_encoder_control, 11, 11,
			       config_info->inst_trigger_enable ? 1 : 0);
	tr_encoder_control =
		u32_set_fields(tr_encoder_control, 13, 13,
			       config_info->inst_stall_ena ? 1 : 0);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 15, 15,
					    config_info->inhibit_src ? 1 : 0);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 17, 16,
					    config_info->inst_sync_mode);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 23, 20,
					    config_info->inst_sync_max);
	tr_encoder_control = u32_set_fields(tr_encoder_control, 26, 24,
					    config_info->record_format);
	read_value_tmp = xt_trace_register_readafterwrite(
		encoder_info->base_addr, tr_encoder_control);
	if (read_value_tmp == -1)
		return -1;
	if (read_value_tmp != tr_encoder_control) {
		// msg
		str[0] = '\0';
		if (u32_get_fields(tr_encoder_control, 6, 4) !=
		    config_info->inst_mode)
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_mode %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->inst_mode, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 9, 9) !=
			 (config_info->send_context ? 1 : 0))
			snprintf(str, sizeof(str),
				"Error: encoder configs send_context %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->send_context, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 11, 11) !=
			 (config_info->inst_trigger_enable ? 1 : 0))
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_trigger_enable %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->inst_trigger_enable,
				tr_encoder_control, read_value_tmp,
				encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 13, 13) !=
			 (config_info->inst_stall_ena ? 1 : 0))
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_stall_ena %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->inst_stall_ena, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 15, 15) !=
			 (config_info->inhibit_src ? 1 : 0))
			snprintf(str, sizeof(str),
				"Error: encoder configs inhibit_src %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->inhibit_src, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 17, 16) !=
			 (config_info->inst_sync_mode))
			snprintf(str, sizeof(str),
				"Error: encoder configs inst_sync_mode %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->inst_sync_mode, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);
		else if (u32_get_fields(tr_encoder_control, 23, 20) !=
			 (config_info->record_format))
			snprintf(str, sizeof(str),
				"Error: encoder configs record_format %d failed(write trTeControl with 0x%x and get 0x%x) with base addr 0x%llx.\n",
				config_info->record_format, tr_encoder_control,
				read_value_tmp, encoder_info->base_addr);

		if (str[0] != '\0') {
			xt_trace_msgout(str);
			goto config_failed;
		}
	}

	// config trTeInstFeatures
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 0, 0,
			       config_info->inst_no_addr_diff ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 1, 1,
			       config_info->inst_no_trap_addr ? 1 : 0);
	tr_te_inst_features = u32_set_fields(
		tr_te_inst_features, 2, 2,
		config_info->inst_en_sequential_tail_jump ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 3, 3,
			       config_info->inst_en_implicit_return ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 4, 4,
			       config_info->inst_en_branch_prediction ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 5, 5,
			       config_info->inst_en_jump_target_cache ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 7, 6,
			       config_info->inst_implicit_return_mode);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 8, 8,
			       config_info->inst_en_repeated_history ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 9, 9,
			       config_info->inst_en_all_jumps ? 1 : 0);
	tr_te_inst_features =
		u32_set_fields(tr_te_inst_features, 10, 10,
			       config_info->inst_extend_addr_msb ? 1 : 0);
	tr_te_inst_features = u32_set_fields(tr_te_inst_features, 27, 16,
					     config_info->src_id);
	tr_te_inst_features = u32_set_fields(tr_te_inst_features, 31, 28,
					     config_info->src_bits);
	read_value_tmp = xt_trace_register_readafterwrite(
		encoder_info->base_addr + OFFSET_TRTEINSTFEATURES,
		tr_te_inst_features);
	if (read_value_tmp == -1)
		return -1;
	if (read_value_tmp != tr_te_inst_features) {
		// msg
		snprintf(str, sizeof(str),
			"Config trTeInstFeatures 0x%x for trace encoder(0x%llx) get 0x%x.\n",
			tr_te_inst_features, encoder_info->base_addr,
			read_value_tmp);
		xt_trace_msgout(str);
		goto config_failed;
	}

	// config trTeDataControl
	if (config_info->data_trace_enable) {
		uint32_t tr_te_data_control = 0;

		tr_te_data_control = u32_set_fields(
			tr_te_data_control, 2, 2,
			config_info->data_trigger_enable ? 1 : 0);
		tr_te_data_control =
			u32_set_fields(tr_te_data_control, 4, 4,
				       config_info->data_stall_enable ? 1 : 0);
		tr_te_data_control =
			u32_set_fields(tr_te_data_control, 6, 6,
				       config_info->data_drop_enable ? 1 : 0);
		tr_te_data_control =
			u32_set_fields(tr_te_data_control, 16, 16,
				       config_info->data_no_value ? 1 : 0);
		tr_te_data_control =
			u32_set_fields(tr_te_data_control, 17, 17,
				       config_info->data_no_addr ? 1 : 0);
		tr_te_data_control =
			u32_set_fields(tr_te_data_control, 19, 18,
				       config_info->data_addr_compress);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr + OFFSET_TRTEDATACONTROL,
			tr_te_data_control);

		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_data_control) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeDataControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_data_control, encoder_info->base_addr,
				read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}

	// config timestamp
	if (config_info->timestamp_enable) {
		int32_t tr_ts_control = 0;
		// enable timestamp component
		tr_ts_control = primary_enable_trace_component(
			encoder_info->base_addr + OFFSET_TRTSCONTROL);
		if (tr_ts_control <= 0) {
			snprintf(str, sizeof(str),
				"Fail to primary enable timestamp with base addr 0x%llx.\n",
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return tr_ts_control;
		}

		// config trTsControl
		tr_ts_control = u32_set_fields(
			tr_ts_control, 3, 3,
			config_info->timestamp_run_in_debugmode ? 1 : 0);
		tr_ts_control = u32_set_fields(tr_ts_control, 6, 4,
					       config_info->timestamp_type);
		tr_ts_control = u32_set_fields(tr_ts_control, 9, 8,
					       config_info->timestamp_prescale);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr + OFFSET_TRTSCONTROL,
			tr_ts_control);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_ts_control) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTsControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_ts_control, encoder_info->base_addr,
				read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}

	return 0;

config_failed:
	// disable component
	tr_encoder_control = reset_trace_component(encoder_info->base_addr);
	if (tr_encoder_control < 0) {
		snprintf(str, sizeof(str),
			"Fail to disable timestamp with base addr 0x%llx.\n",
			encoder_info->base_addr);
		xt_trace_msgout(str);
		return tr_encoder_control;
	}
	return 1;
}

/*
 * return -1, Unrecoverable error(rw trace register error)
 * return 0, Success
 * return 1, Some configurations do not meet expectation(bits error)
 */
int32_t
xt_trace_filter_config(struct xt_trace_encoder_control_info *encoder_info,
		       struct xt_trace_encoder_filter_config_info *config_info)
{
	// insure the trace encoder has been primary enabled
	char str[256] = { '\0' };
	uint32_t tr_te_filter_i_control = 0;
	uint32_t tr_te_inst_filters = 0;
	uint32_t read_value_tmp = 0;

	if (encoder_info->has_detected) {
		// check num
		if (config_info->filter_i >= encoder_info->filter_count) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs filter_%d ,but the encoder(0x%llx) supports max count is %d.\n",
				config_info->filter_i, encoder_info->base_addr,
				encoder_info->filter_count);
			xt_trace_msgout(str);
			return 1;
		}

		// check trTeFilteriControl
		if (config_info->filter_match_privilege &&
		    encoder_info->support_filter_match_privilege == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs filter_match_privilege %d, but the encoder(0x%llx) does not support.\n",
				config_info->filter_match_privilege,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->filter_match_ecause &&
		    encoder_info->support_filter_match_ecause == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs filter_match_ecause %d, but the encoder(0x%llx) does not support.\n",
				config_info->filter_match_ecause,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->filter_match_interrupt &&
		    encoder_info->support_filter_match_interrupt == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs filter_match_interrupt %d, but the encoder(0x%llx) does not support.\n",
				config_info->filter_match_interrupt,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->filter_match_dtype &&
		    encoder_info->support_filter_match_dtype == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs filter_match_dtype %d, but the encoder(0x%llx) does not support.\n",
				config_info->filter_match_dtype,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}
		if (config_info->match_value_dsize &&
		    encoder_info->support_filter_match_dsize == false) {
			// msg
			snprintf(str, sizeof(str),
				"Error: encoder configs match_value_dsize %d, but the encoder(0x%llx) does not support.\n",
				config_info->match_value_dsize,
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return 1;
		}

		// check trTeCompjControl
		if (config_info->comparator1_enable) {
			if ((config_info->comp1_primary_input == 0 &&
			     encoder_info->support_primary_compare_iaddr ==
				     false) ||
			    (config_info->comp1_primary_input == 1 &&
			     encoder_info->support_primary_compare_context ==
				     false) ||
			    (config_info->comp1_primary_input == 2 &&
			     encoder_info->support_primary_compare_tval ==
				     false) ||
			    (config_info->comp1_primary_input == 3 &&
			     encoder_info->support_primary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_primary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_primary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_primary_function == 0 &&
			     encoder_info->support_primary_compare_func_equal ==
				     false) ||
			    (config_info->comp1_primary_function == 1 &&
			     encoder_info->support_primary_compare_func_notequal ==
				     false) ||
			    (config_info->comp1_primary_function == 2 &&
			     encoder_info->support_primary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp1_primary_function == 3 &&
			     encoder_info->support_primary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp1_primary_function == 4 &&
			     encoder_info->support_primary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp1_primary_function == 5 &&
			     encoder_info->support_primary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp1_primary_function == 6 &&
			     encoder_info->support_primary_compare_func_false ==
				     false) ||
			    (config_info->comp1_primary_function == 7 &&
			     encoder_info->support_primary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_primary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_primary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_primary_notify == true &&
			     encoder_info->support_compare_primary_notify_1 ==
				     false) ||
			    (config_info->comp1_primary_notify == false &&
			     encoder_info->support_compare_primary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_primary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_primary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_secondary_input == 0 &&
			     encoder_info->support_secondary_compare_iaddr ==
				     false) ||
			    (config_info->comp1_secondary_input == 1 &&
			     encoder_info->support_secondary_compare_context ==
				     false) ||
			    (config_info->comp1_secondary_input == 2 &&
			     encoder_info->support_secondary_compare_tval ==
				     false) ||
			    (config_info->comp1_secondary_input == 3 &&
			     encoder_info->support_secondary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_secondary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_secondary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_secondary_function == 0 &&
			     encoder_info->support_secondary_compare_func_equal ==
				     false) ||
			    (config_info->comp1_secondary_function == 1 &&
			     encoder_info->support_secondary_compare_func_notequal ==
				     false) ||
			    (config_info->comp1_secondary_function == 2 &&
			     encoder_info->support_secondary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp1_secondary_function == 3 &&
			     encoder_info->support_secondary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp1_secondary_function == 4 &&
			     encoder_info->support_secondary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp1_secondary_function == 5 &&
			     encoder_info->support_secondary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp1_secondary_function == 6 &&
			     encoder_info->support_secondary_compare_func_false ==
				     false) ||
			    (config_info->comp1_secondary_function == 7 &&
			     encoder_info->support_secondary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_secondary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_secondary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_secondary_notify == true &&
			     encoder_info->support_compare_secondary_notify_1 ==
				     false) ||
			    (config_info->comp1_secondary_notify == false &&
			     encoder_info->support_compare_secondary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_secondary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_secondary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp1_match_mode == 0 &&
			     encoder_info->support_compare_primary_true ==
				     false) ||
			    (config_info->comp1_match_mode == 1 &&
			     encoder_info->support_compare_both_true ==
				     false) ||
			    (config_info->comp1_match_mode == 2 &&
			     encoder_info->support_compare_either_false ==
				     false) ||
			    (config_info->comp1_match_mode == 3 &&
			     encoder_info->support_compare_between_primary_and_secondary_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp1_match_mode %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp1_match_mode,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		}
		if (config_info->comparator2_enable) {
			if ((config_info->comp2_primary_input == 0 &&
			     encoder_info->support_primary_compare_iaddr ==
				     false) ||
			    (config_info->comp2_primary_input == 1 &&
			     encoder_info->support_primary_compare_context ==
				     false) ||
			    (config_info->comp2_primary_input == 2 &&
			     encoder_info->support_primary_compare_tval ==
				     false) ||
			    (config_info->comp2_primary_input == 3 &&
			     encoder_info->support_primary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_primary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_primary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_primary_function == 0 &&
			     encoder_info->support_primary_compare_func_equal ==
				     false) ||
			    (config_info->comp2_primary_function == 1 &&
			     encoder_info->support_primary_compare_func_notequal ==
				     false) ||
			    (config_info->comp2_primary_function == 2 &&
			     encoder_info->support_primary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp2_primary_function == 3 &&
			     encoder_info->support_primary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp2_primary_function == 4 &&
			     encoder_info->support_primary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp2_primary_function == 5 &&
			     encoder_info->support_primary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp2_primary_function == 6 &&
			     encoder_info->support_primary_compare_func_false ==
				     false) ||
			    (config_info->comp2_primary_function == 7 &&
			     encoder_info->support_primary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_primary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_primary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_primary_notify == true &&
			     encoder_info->support_compare_primary_notify_1 ==
				     false) ||
			    (config_info->comp2_primary_notify == false &&
			     encoder_info->support_compare_primary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_primary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_primary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_secondary_input == 0 &&
			     encoder_info->support_secondary_compare_iaddr ==
				     false) ||
			    (config_info->comp2_secondary_input == 1 &&
			     encoder_info->support_secondary_compare_context ==
				     false) ||
			    (config_info->comp2_secondary_input == 2 &&
			     encoder_info->support_secondary_compare_tval ==
				     false) ||
			    (config_info->comp2_secondary_input == 3 &&
			     encoder_info->support_secondary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_secondary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_secondary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_secondary_function == 0 &&
			     encoder_info->support_secondary_compare_func_equal ==
				     false) ||
			    (config_info->comp2_secondary_function == 1 &&
			     encoder_info->support_secondary_compare_func_notequal ==
				     false) ||
			    (config_info->comp2_secondary_function == 2 &&
			     encoder_info->support_secondary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp2_secondary_function == 3 &&
			     encoder_info->support_secondary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp2_secondary_function == 4 &&
			     encoder_info->support_secondary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp2_secondary_function == 5 &&
			     encoder_info->support_secondary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp2_secondary_function == 6 &&
			     encoder_info->support_secondary_compare_func_false ==
				     false) ||
			    (config_info->comp2_secondary_function == 7 &&
			     encoder_info->support_secondary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_secondary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_secondary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_secondary_notify == true &&
			     encoder_info->support_compare_secondary_notify_1 ==
				     false) ||
			    (config_info->comp2_secondary_notify == false &&
			     encoder_info->support_compare_secondary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_secondary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_secondary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp2_match_mode == 0 &&
			     encoder_info->support_compare_primary_true ==
				     false) ||
			    (config_info->comp2_match_mode == 1 &&
			     encoder_info->support_compare_both_true ==
				     false) ||
			    (config_info->comp2_match_mode == 2 &&
			     encoder_info->support_compare_either_false ==
				     false) ||
			    (config_info->comp2_match_mode == 3 &&
			     encoder_info->support_compare_between_primary_and_secondary_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp2_match_mode %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp2_match_mode,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		}
		if (config_info->comparator3_enable) {
			if ((config_info->comp3_primary_input == 0 &&
			     encoder_info->support_primary_compare_iaddr ==
				     false) ||
			    (config_info->comp3_primary_input == 1 &&
			     encoder_info->support_primary_compare_context ==
				     false) ||
			    (config_info->comp3_primary_input == 2 &&
			     encoder_info->support_primary_compare_tval ==
				     false) ||
			    (config_info->comp3_primary_input == 3 &&
			     encoder_info->support_primary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_primary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_primary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_primary_function == 0 &&
			     encoder_info->support_primary_compare_func_equal ==
				     false) ||
			    (config_info->comp3_primary_function == 1 &&
			     encoder_info->support_primary_compare_func_notequal ==
				     false) ||
			    (config_info->comp3_primary_function == 2 &&
			     encoder_info->support_primary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp3_primary_function == 3 &&
			     encoder_info->support_primary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp3_primary_function == 4 &&
			     encoder_info->support_primary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp3_primary_function == 5 &&
			     encoder_info->support_primary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp3_primary_function == 6 &&
			     encoder_info->support_primary_compare_func_false ==
				     false) ||
			    (config_info->comp3_primary_function == 7 &&
			     encoder_info->support_primary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_primary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_primary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_primary_notify == true &&
			     encoder_info->support_compare_primary_notify_1 ==
				     false) ||
			    (config_info->comp3_primary_notify == false &&
			     encoder_info->support_compare_primary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_primary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_primary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_secondary_input == 0 &&
			     encoder_info->support_secondary_compare_iaddr ==
				     false) ||
			    (config_info->comp3_secondary_input == 1 &&
			     encoder_info->support_secondary_compare_context ==
				     false) ||
			    (config_info->comp3_secondary_input == 2 &&
			     encoder_info->support_secondary_compare_tval ==
				     false) ||
			    (config_info->comp3_secondary_input == 3 &&
			     encoder_info->support_secondary_compare_daddr ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_secondary_input %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_secondary_input,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_secondary_function == 0 &&
			     encoder_info->support_secondary_compare_func_equal ==
				     false) ||
			    (config_info->comp3_secondary_function == 1 &&
			     encoder_info->support_secondary_compare_func_notequal ==
				     false) ||
			    (config_info->comp3_secondary_function == 2 &&
			     encoder_info->support_secondary_compare_func_lessthan ==
				     false) ||
			    (config_info->comp3_secondary_function == 3 &&
			     encoder_info->support_secondary_compare_func_lessthanorequal ==
				     false) ||
			    (config_info->comp3_secondary_function == 4 &&
			     encoder_info->support_secondary_compare_func_greaterthan ==
				     false) ||
			    (config_info->comp3_secondary_function == 5 &&
			     encoder_info->support_secondary_compare_func_greaterthanorequal ==
				     false) ||
			    (config_info->comp3_secondary_function == 6 &&
			     encoder_info->support_secondary_compare_func_false ==
				     false) ||
			    (config_info->comp3_secondary_function == 7 &&
			     encoder_info->support_secondary_compare_func_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_secondary_function %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_secondary_function,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_secondary_notify == true &&
			     encoder_info->support_compare_secondary_notify_1 ==
				     false) ||
			    (config_info->comp3_secondary_notify == false &&
			     encoder_info->support_compare_secondary_notify_0 ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_secondary_notify %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_secondary_notify,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
			if ((config_info->comp3_match_mode == 0 &&
			     encoder_info->support_compare_primary_true ==
				     false) ||
			    (config_info->comp3_match_mode == 1 &&
			     encoder_info->support_compare_both_true ==
				     false) ||
			    (config_info->comp3_match_mode == 2 &&
			     encoder_info->support_compare_either_false ==
				     false) ||
			    (config_info->comp3_match_mode == 3 &&
			     encoder_info->support_compare_between_primary_and_secondary_true ==
				     false)) {
				// msg
				snprintf(str, sizeof(str),
					"Error: encoder configs comp3_match_mode %d, but the encoder(0x%llx) does not support.\n",
					config_info->comp3_match_mode,
					encoder_info->base_addr);
				xt_trace_msgout(str);
				return 1;
			}
		}
	}

	// config trTeFilteriControl
	tr_te_filter_i_control = 1;
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 1, 1,
			       config_info->filter_match_privilege ? 1 : 0);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 2, 2,
			       config_info->filter_match_ecause ? 1 : 0);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 3, 3,
			       config_info->filter_match_interrupt ? 1 : 0);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 4, 4,
			       config_info->comparator1_enable ? 1 : 0);
	// FIXME: if trTeFilterComp1 can't be changed??
	tr_te_filter_i_control = u32_set_fields(
		tr_te_filter_i_control, 7, 5, config_info->comp1_filter_number);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 8, 8,
			       config_info->comparator2_enable ? 1 : 0);
	// FIXME: if trTeFilterComp2 can't be changed??
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 11, 9,
			       config_info->comp2_filter_number);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 12, 12,
			       config_info->comparator3_enable ? 1 : 0);
	// FIXME: if trTeFilterComp3 can't be changed??
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 15, 13,
			       config_info->comp3_filter_number);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 24, 24,
			       config_info->match_value_dtype ? 1 : 0);
	tr_te_filter_i_control =
		u32_set_fields(tr_te_filter_i_control, 25, 25,
			       config_info->match_value_dsize ? 1 : 0);
	read_value_tmp = xt_trace_register_readafterwrite(
		encoder_info->base_addr +
			OFFSET_TRFILTERCONTROLI(config_info->filter_i),
		tr_te_filter_i_control);
	if (read_value_tmp == -1)
		return -1;
	/* For Perf Trace, only comparator1 will be enabled, we will only care bit[7:0] */
	if ((read_value_tmp & 0xff) != (tr_te_filter_i_control & 0xff)) {
		// msg
		snprintf(str, sizeof(str),
			"Config trTeFilteriControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
			tr_te_filter_i_control, encoder_info->base_addr,
			read_value_tmp);
		xt_trace_msgout(str);
		goto config_failed;
	}

	// config trTeFilteriMatchInst
	if (config_info->filter_match_ecause ||
	    config_info->filter_match_interrupt ||
	    config_info->filter_match_privilege) {
		uint32_t tr_te_filter_i_match_inst = 0;

		tr_te_filter_i_match_inst =
			u32_set_fields(tr_te_filter_i_match_inst, 7, 0,
				       config_info->match_privilege_value);
		tr_te_filter_i_match_inst = u32_set_fields(
			tr_te_filter_i_match_inst, 25, 25,
			config_info->match_value_interrupt ? 1 : 0);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRFILTERMATCHINSTI(
					config_info->filter_i),
			tr_te_filter_i_match_inst);

		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_filter_i_match_inst) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeFilteriMatchInst 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_filter_i_match_inst,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}
	// config trTeFilteriMatchEcause
	if (config_info->filter_match_ecause ||
	    config_info->filter_match_interrupt) {
		uint32_t tr_te_filter_i_match_ecause = 0;

		tr_te_filter_i_match_ecause = config_info->match_choice_ecause;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRFILTERMATCHECAUSEI(
					config_info->filter_i),
			tr_te_filter_i_match_ecause);

		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_filter_i_match_ecause) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeFilteriMatchEcause 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_filter_i_match_ecause,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}
	// config trTeFilteriMatchData
	if (config_info->filter_match_dtype ||
	    config_info->filter_match_dsize) {
		uint32_t tr_te_filter_i_match_data = 0;

		tr_te_filter_i_match_data =
			u32_set_fields(tr_te_filter_i_match_data, 15, 0,
				       config_info->match_value_dtype);
		tr_te_filter_i_match_data =
			u32_set_fields(tr_te_filter_i_match_data, 23, 16,
				       config_info->match_value_dsize);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRFILTERMATCHDATAI(
					config_info->filter_i),
			tr_te_filter_i_match_data);

		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_filter_i_match_data) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeFilteriMatchData 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_filter_i_match_data,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}
	// config trTeCompjControl
	if (config_info->comparator1_enable) {
		uint32_t tr_te_comp_j_control = 0;
		uint32_t tr_te_comp_j_pmatch_low = 0;
		uint32_t tr_te_comp_j_pmatch_high = 0;
		uint32_t tr_te_comp_j_smatch_low = 0;
		uint32_t tr_te_comp_j_smatch_high = 0;

		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 1, 0,
				       config_info->comp1_primary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 3, 2,
				       config_info->comp1_secondary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 6, 4,
				       config_info->comp1_primary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 10, 8,
				       config_info->comp1_secondary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 13, 12,
				       config_info->comp1_match_mode);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 14, 14,
			config_info->comp1_primary_notify ? 1 : 0);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 15, 15,
			config_info->comp1_secondary_notify ? 1 : 0);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPCONTROLJ(
					config_info->comp1_filter_number),
			tr_te_comp_j_control);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_control) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_control, encoder_info->base_addr,
				read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchLow
		tr_te_comp_j_pmatch_low =
			config_info->comp1_primary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHLOWJ(
					config_info->comp1_filter_number),
			tr_te_comp_j_pmatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchHigh
		tr_te_comp_j_pmatch_high =
			config_info->comp1_primary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHHIGHJ(
					config_info->comp1_filter_number),
			tr_te_comp_j_pmatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchLow
		tr_te_comp_j_smatch_low =
			config_info->comp1_secondary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHLOWJ(
					config_info->comp1_filter_number),
			tr_te_comp_j_smatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchHigh
		tr_te_comp_j_smatch_high =
			config_info->comp1_secondary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHHIGHJ(
					config_info->comp1_filter_number),
			tr_te_comp_j_smatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}
	if (config_info->comparator2_enable) {
		uint32_t tr_te_comp_j_control = 0;
		uint32_t tr_te_comp_j_pmatch_low = 0;
		uint32_t tr_te_comp_j_pmatch_high = 0;
		uint32_t tr_te_comp_j_smatch_low = 0;
		uint32_t tr_te_comp_j_smatch_high = 0;

		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 1, 0,
				       config_info->comp2_primary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 3, 2,
				       config_info->comp2_secondary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 6, 4,
				       config_info->comp2_primary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 10, 8,
				       config_info->comp2_secondary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 13, 12,
				       config_info->comp2_match_mode);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 14, 14,
			config_info->comp2_primary_notify ? 1 : 0);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 15, 15,
			config_info->comp2_secondary_notify ? 1 : 0);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPCONTROLJ(
					config_info->comp2_filter_number),
			tr_te_comp_j_control);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_control) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompJControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_control, encoder_info->base_addr,
				read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchLow
		tr_te_comp_j_pmatch_low =
			config_info->comp2_primary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHLOWJ(
					config_info->comp2_filter_number),
			tr_te_comp_j_pmatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchHigh
		tr_te_comp_j_pmatch_high =
			config_info->comp2_primary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHHIGHJ(
					config_info->comp2_filter_number),
			tr_te_comp_j_pmatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchLow
		tr_te_comp_j_smatch_low =
			config_info->comp2_secondary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHLOWJ(
					config_info->comp2_filter_number),
			tr_te_comp_j_smatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchHigh
		tr_te_comp_j_smatch_high =
			config_info->comp2_secondary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHHIGHJ(
					config_info->comp2_filter_number),
			tr_te_comp_j_smatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}
	if (config_info->comparator3_enable) {
		uint32_t tr_te_comp_j_control = 0;
		uint32_t tr_te_comp_j_pmatch_low = 0;
		uint32_t tr_te_comp_j_pmatch_high = 0;
		uint32_t tr_te_comp_j_smatch_low = 0;
		uint32_t tr_te_comp_j_smatch_high = 0;

		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 1, 0,
				       config_info->comp3_primary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 3, 2,
				       config_info->comp3_secondary_input);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 6, 4,
				       config_info->comp3_primary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 10, 8,
				       config_info->comp3_secondary_function);
		tr_te_comp_j_control =
			u32_set_fields(tr_te_comp_j_control, 13, 12,
				       config_info->comp3_match_mode);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 14, 14,
			config_info->comp3_primary_notify ? 1 : 0);
		tr_te_comp_j_control = u32_set_fields(
			tr_te_comp_j_control, 15, 15,
			config_info->comp3_secondary_notify ? 1 : 0);
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPCONTROLJ(
					config_info->comp3_filter_number),
			tr_te_comp_j_control);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_control) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompJControl 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_control, encoder_info->base_addr,
				read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchLow
		tr_te_comp_j_pmatch_low =
			config_info->comp3_primary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHLOWJ(
					config_info->comp3_filter_number),
			tr_te_comp_j_pmatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjPMatchHigh
		tr_te_comp_j_pmatch_high =
			config_info->comp3_primary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPPMATCHHIGHJ(
					config_info->comp3_filter_number),
			tr_te_comp_j_pmatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_pmatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjPMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_pmatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchLow
		tr_te_comp_j_smatch_low =
			config_info->comp3_secondary_match_low_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHLOWJ(
					config_info->comp3_filter_number),
			tr_te_comp_j_smatch_low);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_low) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchLow 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_low,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
		// config trTeCompjSMatchHigh
		tr_te_comp_j_smatch_high =
			config_info->comp3_secondary_match_high_value;
		read_value_tmp = xt_trace_register_readafterwrite(
			encoder_info->base_addr +
				OFFSET_TRCOMPSMATCHHIGHJ(
					config_info->comp3_filter_number),
			tr_te_comp_j_smatch_high);
		if (read_value_tmp == -1)
			return -1;
		if (read_value_tmp != tr_te_comp_j_smatch_high) {
			// msg
			snprintf(str, sizeof(str),
				"Config trTeCompjSMatchHigh 0x%x for trace encoder(0x%llx) get 0x%x.\n",
				tr_te_comp_j_smatch_high,
				encoder_info->base_addr, read_value_tmp);
			xt_trace_msgout(str);
			goto config_failed;
		}
	}

	/* Add filter i to current Encoder */
	if (xt_trace_register_read(encoder_info->base_addr +
					   OFFSET_TRTEINSTFILTERS,
				   &tr_te_inst_filters))
		return -1;
	tr_te_inst_filters |= 1 << config_info->filter_i;
	if (xt_trace_register_write(encoder_info->base_addr +
					    OFFSET_TRTEINSTFILTERS,
				    tr_te_inst_filters))
		return -1;

	return 0;

config_failed:
	return 1;
}
