// SPDX-License-Identifier: GPL-2.0

#include <string.h>
#include "debug.h"
#include "xuantie-mseo-mdo.h"

static uint64_t trace_data_offset;
static uint64_t trace_data_total_size;
static uint8_t trace_data_left_size;
static uint8_t trace_data_left_value;
static uint8_t trace_data_left_mseo;
static uint8_t *trace_data_buf;

void xt_trace_mseo_mdo_init(unsigned char *buf, uint64_t len)
{
	trace_data_offset = 0;
	trace_data_left_size = 0;
	trace_data_left_value = 0;
	trace_data_left_mseo = 0;
	trace_data_total_size = len;
	trace_data_buf = buf;
}

static inline uint8_t trace_data_get_mseo(uint8_t data)
{
	return (data & 0x3);
}

static inline uint8_t trace_data_get_mdo(uint8_t data)
{
	return (data >> 2);
}

static int32_t xt_trace_get_trace_data(uint64_t size, uint8_t *buffer)
{
	if ((trace_data_offset + size) > trace_data_total_size) {
		printf(".   get end of the trace data.\n");
		return 1;
	}

	// copy data from start+trace_data_offset with size SIZE to BUFFER
	memcpy(buffer, trace_data_buf + trace_data_offset, size);
	trace_data_offset += size;
	return 0;
}

static int32_t xt_trace_get_tcode_middle(uint8_t *tcode)
{
	int32_t ret = 0;
	uint8_t buffer[2] = { 0, 0 };

	if (trace_data_offset)
		trace_data_offset--;

	while (1) {
		ret = xt_trace_get_trace_data(1, (uint8_t *)&buffer[1]);
		if (ret != 0)
			return ret;

		if (trace_data_get_mseo(buffer[0]) == 0x3 &&
		    trace_data_get_mseo(buffer[1]) == 0x0)
			break;

		buffer[0] = buffer[1];
	}

	*tcode = trace_data_get_mdo(buffer[1]);
	return 0;
}

static int32_t xt_trace_get_tcode_start(uint8_t *tcode)
{
	int32_t ret = 0;

	ret = xt_trace_get_trace_data(1, tcode);
	if (ret != 0)
		return ret;

	*tcode = trace_data_get_mdo(*tcode);
	return 0;
}

static void xt_trace_clear_left_data(void)
{
	trace_data_left_size = 0;
	trace_data_left_value = 0;
	trace_data_left_mseo = 0;
}

/*
 * Get FIXED MESSAGE FIELD
 */
static int32_t xt_trace_get_fixed_message_fields(uint32_t size, uint64_t *value)
{
	int32_t ret = 0;
	uint32_t i = 0;
	uint32_t obtained_size = 0;
	uint32_t read_size = 0;
	uint8_t read_buffer[11] = { 0 };
	uint8_t left_size = 0;

	// init value to 0
	*value = 0;

	// get value from pre byte left
	if (trace_data_left_size) {
		if (trace_data_left_size >= size) {
			*value = (trace_data_left_value << (8 - size)) >>
				 (8 - size);
			trace_data_left_value = trace_data_left_value >> size;
			trace_data_left_size -= size;
			return 0;
		}

		*value = trace_data_left_value;
		obtained_size = trace_data_left_size;
		size -= obtained_size;
		trace_data_left_size = 0;
	}

	// read trace data
	read_size = (size + 5) / 6;
	ret = xt_trace_get_trace_data(read_size, read_buffer);
	if (ret != 0)
		return ret;

	// get MDO aligned with 6
	for (i = 0; i < (size / 6); i++) {
		*value += (uint64_t)trace_data_get_mdo(read_buffer[i])
			  << obtained_size;
		obtained_size += 6;
	}

	// get tail size which is not aligned with 6
	left_size = size % 6;
	if (left_size) {
		*value += (uint64_t)((trace_data_get_mdo(read_buffer[i])
				      << (8 - left_size)) >>
				     (8 - left_size))
			  << obtained_size;
		obtained_size += 6;

		if (left_size < 6) {
			trace_data_left_size = 6 - left_size;
			trace_data_left_value =
				trace_data_get_mdo(read_buffer[i]) >> left_size;
			trace_data_left_mseo =
				trace_data_get_mseo(read_buffer[i]);
		}
	}

	return 0;
}

/*
 * Get Variable Message fileds
 */
static int32_t xt_trace_get_variable_message_fields(uint32_t *length,
						    uint64_t *value)
{
	int32_t ret = 0;
	uint32_t obtained_size = 0;
	uint8_t read_buffer = 0;

	// init value to 0
	*value = 0;

	// get value from pre byte left
	if (trace_data_left_size) {
		*value = trace_data_left_value;
		obtained_size = trace_data_left_size;
		trace_data_left_size = 0;
		trace_data_left_value = 0;

		// FIXME: if var.size < trace_data_left_size ???
		// FIXED.
		if (trace_data_left_mseo) {
			// the bits size of value will be obtained_size
			*length = obtained_size;
			return 0;
		}
	}

	// read a byte tail until MSEO != 00
	while (1) {
		/* A 63-bit field plus the carried leftover spans at most
		 * 67 MDO bits; anything longer is malformed.
		 */
		if (obtained_size >= 67)
			return -1;

		ret = xt_trace_get_trace_data(1, &read_buffer);
		if (ret != 0)
			return ret;

		*value += (uint64_t)trace_data_get_mdo(read_buffer)
			  << obtained_size;
		obtained_size += 6;

		if (trace_data_get_mseo(read_buffer) != 0)
			break;
	}

	// the bits size of value will be obtained_size
	*length = obtained_size;

	return 0;
}

int32_t xt_trace_analyze_message_field(struct xt_riscv_nexus_trace_message *msg,
				       uint32_t trace_data_wrapped,
				       uint32_t wrapped_used, uint32_t src_bits,
				       uint32_t timestamp_bits)
{
	int32_t ret = 0;
	uint8_t tcode = 0;
	uint64_t tmp_value = 0;
	uint32_t tmp_size = 0;

	memset(msg, 0, sizeof(struct xt_riscv_nexus_trace_message));

	xt_trace_clear_left_data();
	if (!trace_data_wrapped && wrapped_used)
		ret = xt_trace_get_tcode_start(&tcode);
	else
		ret = xt_trace_get_tcode_middle(&tcode);
	if (ret != 0)
		return ret;
	msg->tcode = tcode;

	// get src
	if (src_bits) {
		ret = xt_trace_get_fixed_message_fields(src_bits, &tmp_value);
		if (ret != 0)
			return ret;

		msg->has_src = true;
		msg->src = (uint32_t)tmp_value;
	} else
		msg->has_src = false;

	switch (tcode) {
	case TCODE_OWNERSHIP: {
		// get format
		ret = xt_trace_get_fixed_message_fields(2, &tmp_value);
		if (ret != 0)
			return ret;
		if ((tmp_value & 0x3) == 0) {
			msg->sub_value.ownership.has_context = false;
		} else if ((tmp_value & 0x3) == 2) {
			msg->sub_value.ownership.has_context = true;
			msg->sub_value.ownership.is_scontext = true;
		} else if ((tmp_value & 0x3) == 3) {
			msg->sub_value.ownership.has_context = true;
			msg->sub_value.ownership.is_scontext = false;
		} else {
			// error format
			pr_err("Unknown format %d in ownership message\n",
			       (uint32_t)(tmp_value & 3));
			return -1;
		}

		// get priv and virtual
		ret = xt_trace_get_fixed_message_fields(3, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.ownership.privilege = (uint8_t)tmp_value & 0x3;
		msg->sub_value.ownership.is_virtual = (tmp_value & 0x4) ? true :
									  false;

		// get context
		if (msg->sub_value.ownership.has_context) {
			ret = xt_trace_get_variable_message_fields(&tmp_size,
								   &tmp_value);
			if (ret != 0)
				return ret;
			msg->sub_value.ownership.context = tmp_value;
		} else {
			// As the field of proccess is a variable size and we get
			// proccess.format,prv,v used xt_trace_get_fixed_message_fields,
			// if trace_data_left_size !=0, the trace_data_left_value is only
			// zero extended with mseo 01 or 11, it is meaningless.
			xt_trace_clear_left_data();
		}
	} break;
	case TCODE_DIRECTBRANCH: {
		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.directbranch.i_cnt = tmp_value;
	} break;
	case TCODE_INDIRECTBRANCH: {
		// get b-type
		ret = xt_trace_get_fixed_message_fields(2, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranch.b_type = (uint8_t)tmp_value & 0x3;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranch.i_cnt = tmp_value;

		// gey u_addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranch.u_addr = tmp_value;
	} break;
	case TCODE_ERROR: {
		// get etype
		ret = xt_trace_get_fixed_message_fields(4, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.error.etype = (uint8_t)tmp_value & 0xf;

		// get ecode
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.error.ecode = tmp_value;
	} break;
	case TCODE_PROGTRACESYNC: {
		// get sync
		ret = xt_trace_get_fixed_message_fields(4, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.progtracesync.sync = (uint8_t)tmp_value & 0xf;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.progtracesync.i_cnt = tmp_value;

		// get f-addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.progtracesync.f_addr = tmp_value;
	} break;
	case TCODE_DIRECTBRANCHSYNC: {
		// get sync
		ret = xt_trace_get_fixed_message_fields(4, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.directbranchsync.sync = (uint8_t)tmp_value & 0xf;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.directbranchsync.i_cnt = tmp_value;

		// get f-addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.directbranchsync.f_addr = tmp_value;
	} break;
	case TCODE_INDIRECTBRANCHSYNC: {
		// get sync and b_type
		ret = xt_trace_get_fixed_message_fields(6, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchsync.sync = (uint8_t)tmp_value &
							 0xf;
		msg->sub_value.indirectbranchsync.b_type =
			((uint8_t)tmp_value >> 4) & 0x3;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchsync.i_cnt = tmp_value;

		// get f-addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchsync.f_addr = tmp_value;
	} break;
	case TCODE_RESOURCEFULL: {
		// get rcode
		ret = xt_trace_get_fixed_message_fields(4, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.resourcefull.rcode = (uint8_t)tmp_value & 0xf;

		// get rdata0
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.resourcefull.rdata0 = tmp_value;

		if (msg->sub_value.resourcefull.rcode == 2) {
			// get rdata1
			ret = xt_trace_get_variable_message_fields(&tmp_size,
								   &tmp_value);
			if (ret != 0)
				return ret;
			msg->sub_value.resourcefull.rdata1 = tmp_value;
		}
	} break;
	case TCODE_INDIRECTBRANCHHIST: {
		// get b-type
		ret = xt_trace_get_fixed_message_fields(2, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhist.b_type = (uint8_t)tmp_value &
							   0x3;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhist.i_cnt = tmp_value;

		// get u-addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhist.u_addr = tmp_value;

		// get hist
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhist.hist = tmp_value;
	} break;
	case TCODE_INDIRECTBRANCHHISTSYNC: {
		// get sync b-type
		ret = xt_trace_get_fixed_message_fields(6, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhistsync.sync =
			(uint8_t)tmp_value & 0xf;
		msg->sub_value.indirectbranchhistsync.b_type =
			((uint8_t)tmp_value >> 4) & 0x3;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhistsync.i_cnt = tmp_value;

		// get f-addr
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhistsync.f_addr = tmp_value;

		// get hist
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.indirectbranchhistsync.hist = tmp_value;
	} break;
	case TCODE_REPEATBRANCH: {
		// get b-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.repeatbranch.b_cnt = tmp_value;
	} break;
	case TCODE_PROGTRACECORRELATION: {
		// get evcode and cdf
		ret = xt_trace_get_fixed_message_fields(6, &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.progtracecorrelation.evcode =
			(uint8_t)tmp_value & 0xf;
		msg->sub_value.progtracecorrelation.cdf =
			((uint8_t)tmp_value >> 4) & 0x3;

		// get i-cnt
		ret = xt_trace_get_variable_message_fields(&tmp_size,
							   &tmp_value);
		if (ret != 0)
			return ret;
		msg->sub_value.progtracecorrelation.i_cnt = tmp_value;

		if (msg->sub_value.progtracecorrelation.cdf == 0)
			msg->sub_value.progtracecorrelation.hist = 0;
		else {
			// get hist
			ret = xt_trace_get_variable_message_fields(&tmp_size,
								   &tmp_value);
			if (ret != 0)
				return ret;
			msg->sub_value.progtracecorrelation.hist = tmp_value;
		}
	} break;
	default: {
		// msg unknown TCODE
		pr_err("Unknown TCODE 0x%x\n", tcode);
		return -1;
	}
	}

	// get timestamp
	if (timestamp_bits) {
		ret = xt_trace_get_fixed_message_fields(timestamp_bits,
							&tmp_value);
		if (ret != 0)
			return ret;
		msg->has_timestamp = true;
		msg->timestamp = tmp_value;
	} else {
		msg->has_timestamp = false;
	}

	return 0;
}
