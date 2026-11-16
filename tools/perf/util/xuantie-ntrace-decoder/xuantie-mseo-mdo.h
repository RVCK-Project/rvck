/* SPDX-License-Identifier: GPL-2.0 */

#pragma once

#include "xuantie-ntrace-message.h"

void xt_trace_mseo_mdo_init(unsigned char *buf, uint64_t len);

int32_t xt_trace_analyze_message_field(struct xt_riscv_nexus_trace_message *msg,
				       uint32_t trace_data_wrapped,
				       uint32_t wrapped_used, uint32_t src_bits,
				       uint32_t timestamp_bits);
