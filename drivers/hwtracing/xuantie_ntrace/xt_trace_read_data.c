// SPDX-License-Identifier: GPL-2.0-only

#include <linux/kernel.h>
#include <linux/memory.h>
#include "xt_ntrace_control_interface.h"

/**
 * \brief        Get trRamWPLow vs High and Trace Data Size in the Sink
 * \param[in]    sink_info
 * \param[in]    config_info
 * \param[out]   write_point, the value of trRamWPLow vs High
 * \param[out]   size, the size of the trace data in the sink
 * \return       return -1 if access trace registers failed, otherwise return 0
 */
TRACE_CONTROL_LIB_API int32_t xt_trace_ram_sink_get_data_size(
	struct xt_trace_sink_control_info *sink_info, uint64_t *write_point)
{
	uint32_t tr_ram_wp_low = 0;
	uint32_t tr_ram_wp_high = 0;

	if (xt_trace_register_read(sink_info->base_addr + OFFSET_TRRAMWPLOW,
				   &tr_ram_wp_low))
		return -1;
	if (xt_trace_register_read(sink_info->base_addr + OFFSET_TRRAMWPHIGH,
				   &tr_ram_wp_high))
		return -1;

	*write_point = (((uint64_t)tr_ram_wp_high) << 32) + tr_ram_wp_low;

	return 0;
}
