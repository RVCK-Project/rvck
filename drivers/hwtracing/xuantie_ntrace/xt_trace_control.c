// SPDX-License-Identifier: GPL-2.0

#include <linux/kernel.h>
#include "xt_ntrace_control_interface.h"

// reading and writing the registers of the trace component
static int32_t (*trace_component_register_write)(uint64_t addr,
						 uint32_t value);
static int32_t (*trace_component_register_read)(uint64_t addr,
						uint32_t *value);
static int32_t (*trace_component_memory_read)(uint64_t addr, uint8_t *buff,
					      uint32_t size);
static int (*trace_msgout)(const char *);

TRACE_CONTROL_LIB_API void xt_trace_control_init_rw_trace_interface(
	int (*msgout)(const char *),
	int32_t (*trace_register_write)(uint64_t addr, uint32_t value),
	int32_t (*trace_register_read)(uint64_t addr, uint32_t *value),
	int32_t (*trace_memory_read)(uint64_t addr, uint8_t *buf,
				     uint32_t size))
{
	trace_msgout = msgout;
	trace_component_register_write = trace_register_write;
	trace_component_register_read = trace_register_read;
	trace_component_memory_read = trace_memory_read;
}
int32_t xt_trace_msgout(const char *str)
{
	if (trace_msgout)
		return trace_msgout(str);

	return 0;
}

int32_t xt_trace_register_write(uint64_t addr, uint32_t value)
{
	int ret = 0;
	char str[256];

	if (trace_component_register_write) {
		ret = trace_component_register_write(addr, value);
		if (ret) {
			snprintf(str, sizeof(str), "Fail to write 0x%x to addr %llx.\n",
				value, addr);
			xt_trace_msgout(str);
		}
		return ret;
	}

	xt_trace_msgout("trace_component_register_write interface is NULL.\n");
	return -1;
}

int32_t xt_trace_register_read(uint64_t addr, uint32_t *value)
{
	int ret = 0;
	char str[256];

	if (trace_component_register_read) {
		ret = trace_component_register_read(addr, value);
		if (ret) {
			snprintf(str, sizeof(str), "Fail to read addr %llx\n", addr);
			xt_trace_msgout(str);
		}
		return ret;
	}

	xt_trace_msgout("trace_component_register_read interface is NULL.\n");
	return -1;
}

int32_t xt_trace_memory_read(uint64_t addr, uint8_t *buf, uint32_t size)
{
	int ret = 0;
	char str[256];

	if (trace_component_memory_read) {
		ret = trace_component_memory_read(addr, buf, size);
		if (ret) {
			snprintf(str, sizeof(str), "Fail to read memory addr %llx\n", addr);
			xt_trace_msgout(str);
		}
		return ret;
	}

	xt_trace_msgout("trace_component_memory_read interface is NULL.\n");
	return -1;
}

/**
 * Enable interface
 */
int32_t enable_trace_component(uint64_t base_addr)
{
	uint32_t i;
	uint32_t tr_any_control = 0;
	char str[256];

	if (xt_trace_register_read(base_addr, &tr_any_control))
		return -1;

	if (tr_any_control & 0x2)
		return 0;

	tr_any_control = u32_set_fields(tr_any_control, 1, 1, 1);

	xt_trace_register_write(base_addr, tr_any_control);

	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ENABLE(tr_any_control) == TR_ANY_CONTROL_ENABLE1)
			break;
		cpu_relax();
	}

	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout enabling trace component with base addr 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

/*
 * primary enable a component, try to set tr??Active to 1
 * return tr??Control, success
 * return 0xffffffff, error(rw trace register error)
 * return 0, Some implementations do not meet expectation(bits error)
 */

int32_t primary_enable_trace_component(uint64_t base_addr)
{
	uint32_t i;
	uint32_t tr_any_control = 0;
	char str[256];

	if (xt_trace_register_write(base_addr, TR_ANY_CONTROL_ACTIVE0))
		return -1;

	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ACTIVE(tr_any_control) ==
		    TR_ANY_CONTROL_ACTIVE0)
			break;
		cpu_relax();
	}

	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout waiting trActive=0 on component 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	if (xt_trace_register_write(base_addr, TR_ANY_CONTROL_ACTIVE1))
		return -1;

	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ACTIVE(tr_any_control) ==
		    TR_ANY_CONTROL_ACTIVE1)
			break;
		cpu_relax();
	}

	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout waiting trActive=1 on component 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return tr_any_control;
}

/*
 * reset a component, try to set tr??Active to 0
 * return 0xffffffff, error(rw trace register error)
 * return 0, success
 */
int32_t reset_trace_component(uint64_t base_addr)
{
	uint32_t i;
	uint32_t tr_any_control = 0;
	char str[256];

	if (xt_trace_register_write(base_addr, TR_ANY_CONTROL_ACTIVE0))
		return -1;

	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ACTIVE(tr_any_control) ==
		    TR_ANY_CONTROL_ACTIVE0)
			break;
		cpu_relax();
	}

	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout resetting trace component with base addr 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

/*
 * return -1, failed, else, return the read value
 */
int32_t xt_trace_register_readafterwrite(uint64_t reg_addr, uint32_t value)
{
	uint32_t return_value = 0;

	if (xt_trace_register_write(reg_addr, value))
		return -1;

	if (xt_trace_register_read(reg_addr, &return_value))
		return -1;

	return return_value;
}

uint32_t enable_trace_encoder_inst_tracing(uint64_t base_addr)
{
	uint32_t tr_te_control = 0;

	if (xt_trace_register_read(base_addr, &tr_te_control))
		return -1;

	if ((tr_te_control & TR_ANY_CONTROL_ACTIVE1) == 0)
		return -1;

	tr_te_control = tr_te_control | (1 << 2);

	if (xt_trace_register_write(base_addr, tr_te_control))
		return -1;

	return 0;
}

int32_t enable_trace_encoder_timestamp(uint64_t base_addr)
{
	uint32_t tr_ts_control = 0;

	if (xt_trace_register_read(base_addr, &tr_ts_control))
		return -1;

	if ((tr_ts_control & TR_ANY_CONTROL_ACTIVE1) == 0)
		return -1;

	// enable trTsEnable
	tr_ts_control = tr_ts_control | (1 << 15);

	if (xt_trace_register_write(base_addr, tr_ts_control))
		return -1;

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_enable(struct xt_trace_encoder_control_info *encoder_info,
			bool enable_timestamp)
{
	char str[256] = { '\0' };

	if (enable_timestamp) {
		if (enable_trace_encoder_timestamp(encoder_info->base_addr +
						   OFFSET_TRTSCONTROL)) {
			snprintf(str, sizeof(str),
				"Fail to enable trTsEnable for the trace encoder(0x%llx).\n",
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return -1;
		}
	}

	if (enable_trace_component(encoder_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to enable the trace encoder(0x%llx).\n",
			encoder_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	// trTeInstTracing can only be written after trTeEnable is 1
	if (enable_trace_encoder_inst_tracing(encoder_info->base_addr)) {
		snprintf(str, sizeof(str),
			"Fail to set trTeInstTracing for the trace encoder(0x%llx).\n",
			encoder_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_enable(struct xt_trace_sink_control_info *sink_info)
{
	char str[256];

	if (enable_trace_component(sink_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to enable the trace sink(0x%llx).\n",
			sink_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_enable(struct xt_trace_funnel_control_info *funnel_info)
{
	char str[256];

	if (enable_trace_component(funnel_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to enable the trace funnel(0x%llx).\n",
			funnel_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

/**
 * Disable interface
 */
int32_t disable_trace_component(uint64_t base_addr)
{
	uint32_t i;
	uint32_t tr_any_control = 0;
	char str[256];

	if (xt_trace_register_read(base_addr, &tr_any_control))
		return -1;

	if ((tr_any_control & 0x1) == 0)
		return 0;

	tr_any_control = u32_set_fields(tr_any_control, 1, 1, 0);

	if (xt_trace_register_write(base_addr, tr_any_control))
		return -1;
	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ENABLE(tr_any_control) == TR_ANY_CONTROL_ENABLE0)
			break;
		cpu_relax();
	}
	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout disabling trace component with base addr 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_disable(struct xt_trace_encoder_control_info *encoder_info,
			 bool disable_timestamp)
{
	char str[256];

	if (disable_timestamp) {
		if (disable_trace_component(encoder_info->base_addr +
					    OFFSET_TRTSCONTROL)) {
			snprintf(str, sizeof(str),
				"Fail to disable timestamp for the trace encoder(0x%llx).\n",
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return -1;
		}
	}

	if (disable_trace_component(encoder_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to disable the trace encoder(0x%llx).\n",
			encoder_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_disable(struct xt_trace_sink_control_info *sink_info)
{
	char str[256];

	if (disable_trace_component(sink_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to disable the trace sink(0x%llx).\n",
			sink_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_disable(struct xt_trace_funnel_control_info *funnel_info)
{
	char str[256];

	if (disable_trace_component(funnel_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to disable the trace funnel(0x%llx).\n",
			funnel_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

/**
 * Close interface
 */
int32_t close_trace_component(uint64_t base_addr)
{
	uint32_t i;
	uint32_t tr_any_control = 0;
	char str[256];

	if (xt_trace_register_write(base_addr, TR_ANY_CONTROL_ACTIVE0))
		return -1;

	for (i = 0; i < XT_CHECK_TRANYCONTROL_TIMES; i++) {
		if (xt_trace_register_read(base_addr, &tr_any_control))
			return -1;
		if (TR_ANY_CONTROL_GET_ACTIVE(tr_any_control) ==
		    TR_ANY_CONTROL_ACTIVE0)
			break;
		cpu_relax();
	}

	if (i == XT_CHECK_TRANYCONTROL_TIMES) {
		snprintf(str, sizeof(str), "Timeout closing trace component with base addr 0x%llx.\n",
			base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_encoder_close(struct xt_trace_encoder_control_info *encoder_info,
		       bool close_timestamp)
{
	char str[256];

	if (close_timestamp) {
		if (close_trace_component(encoder_info->base_addr +
					  OFFSET_TRTSCONTROL)) {
			snprintf(str, sizeof(str),
				"Fail to close timestamp for the trace encoder(0x%llx).\n",
				encoder_info->base_addr);
			xt_trace_msgout(str);
			return -1;
		}
	}

	if (close_trace_component(encoder_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to close the trace encoder(0x%llx).\n",
			encoder_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_sink_close(struct xt_trace_sink_control_info *sink_info)
{
	char str[256];

	if (close_trace_component(sink_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to close the trace sink(0x%llx).\n",
			sink_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}

TRACE_CONTROL_LIB_API int32_t
xt_trace_funnel_close(struct xt_trace_funnel_control_info *funnel_info)
{
	char str[256];

	if (close_trace_component(funnel_info->base_addr)) {
		snprintf(str, sizeof(str), "Fail to close the trace funnel(0x%llx).\n",
			funnel_info->base_addr);
		xt_trace_msgout(str);
		return -1;
	}

	return 0;
}
