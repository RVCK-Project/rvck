// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include <linux/memory.h>
#include "xt_ntrace_control_interface.h"

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
