// SPDX-License-Identifier: GPL-2.0-only

#include <linux/device.h>
#include <linux/property.h>

#include "xuantie_ntrace.h"

int xuantie_ntrace_encoder_init(struct xuantie_ntrace_component *comp)
{
	struct device *dev = &comp->pdev->dev;
	struct xuantie_ntrace_encoder *encoder = &comp->encoder;
	const char *str_tmp;
	int ret;
	static const char * const trace_format[] = {"E-trace", "N-Trace", "Reserved2",
		"Reserved2", "Reserved2", "Reserved2", "Reserved2", "Reserved2", "Vendor-Specific"};
	static const char * const inst_sync_mode[] = {"Off", "Count Trace Messages/packets",
		"Count Hart Clock Cycles", "Count Instruction 16-bit half-words"};
	static const char * const insn_mode[] = {"INST Trace disable", "Protocol defined1",
		"Protocol defined2", "Branch Trace",	"Protocol defined4", "Protocol defined5",
		"Branch History Trace", "Vendor-defined"};
	static const char * const timestamp_mode[] = {"None", "External", "Internal System",
		"Internal Core", "Shared", "Vendor-Specific5", "Vendor-Specific6",
		"Vendor-Specific7"};

	ret = xuantie_ntrace_get_hart_id(dev, &encoder->hart_id);
	if (ret) {
		dev_err(dev, "Failed to get the hart this encoder belongs to\n");
		return ret;
	}
	dev_info(dev, "cpu=%d\n", encoder->hart_id);

	ret = device_property_read_u32(dev, "trace_encoder_format",
				       &encoder->trace_format);
	if (ret)
		encoder->trace_format = 1; //default N-Trace
	else
		encoder->trace_format &= 0x7;
	dev_info(dev, "trace_encoder_format=%d(%s)\n", encoder->trace_format,
		 trace_format[encoder->trace_format]);

	ret = device_property_read_u32(dev, "trace_encoder_inst_mode",
				       &encoder->inst_mode);
	if (ret)
		encoder->inst_mode = 6; //default Branch History Trace
	else
		encoder->inst_mode &= 7;
	dev_info(dev, "trace_encoder_inst_mode=%d(%s)\n", encoder->inst_mode,
		 insn_mode[encoder->inst_mode]);

	ret = device_property_read_string(dev, "trace_encoder_context", &str_tmp);
	if (ret)
		encoder->send_context = true; //default Send Context
	else
		encoder->send_context = !strcmp(str_tmp, "true");
	dev_info(dev, "trace_encoder_context=%d\n", encoder->send_context);

	ret = device_property_read_string(dev, "trace_encoder_inst_stall_enable", &str_tmp);
	if (ret)
		encoder->stall_enable = true; //default Stall Enable
	else
		encoder->stall_enable = !strcmp(str_tmp, "true");
	dev_info(dev, "trace_encoder_inst_stall_enable=%d\n", encoder->stall_enable);

	ret = device_property_read_string(dev, "trace_encoder_inhibit_src", &str_tmp);
	if (ret)
		encoder->inhibit_src = false; //defaut Send Src
	else
		encoder->inhibit_src = !strcmp(str_tmp, "true");
	dev_info(dev, "trace_encoder_inhibit_src=%d\n", encoder->inhibit_src);

	ret = device_property_read_u32(dev, "trace_encoder_src_id", &encoder->src_id);
	if (ret)
		encoder->src_id = encoder->hart_id; //default using CPU
	dev_info(dev, "trace_encoder_src_id=%d\n", encoder->src_id);

	ret = device_property_read_u32(dev, "trace_encoder_inst_sync_mode",
				       &encoder->inst_sync_mode);
	if (ret)
		encoder->inst_sync_mode = 0; //default No Sync
	else
		encoder->inst_sync_mode &= 3;
	dev_info(dev, "trace_encoder_inst_sync_mode=%d(%s)\n", encoder->inst_sync_mode,
		 inst_sync_mode[encoder->inst_sync_mode]);

	ret = device_property_read_u32(dev, "trace_encoder_inst_sync_max",
				       &encoder->inst_sync_max);
	if (ret && encoder->inst_sync_mode) {
		dev_err(dev, "Failed to read 'trace_encoder_inst_sync_max'\n");
		return ret;
	}
	dev_info(dev, "trace_encoder_inst_sync_max=0x%x\n", encoder->inst_sync_max);

	ret = device_property_read_string(dev, "trace_encoder_inst_trigger_enable", &str_tmp);
	if (ret)
		encoder->inst_trigger_enable = true; //default True
	else
		encoder->inst_trigger_enable = !strcmp(str_tmp, "true");
	dev_info(dev, "trace_encoder_inst_trigger_enable=%d\n", encoder->inst_trigger_enable);

	ret = device_property_read_string(dev, "trace_timestamp_enable", &str_tmp);
	if (ret)
		encoder->enable_timestamp = false; // default Not Enable Timestamp
	else
		encoder->enable_timestamp = !strcmp(str_tmp, "true");
	dev_info(dev, "trace_timestamp_enable=%d\n", encoder->enable_timestamp);

	ret = device_property_read_u32(dev, "trace_timestamp_mode",
				       &encoder->timestamp_mode);
	if (ret && encoder->enable_timestamp) {
		dev_err(dev, "Failed to read 'trace_timestamp_mode'\n");
		return ret;
	}
	encoder->timestamp_mode &= 7;
	dev_info(dev, "trace_timestamp_mode=%d(%s)\n", encoder->timestamp_mode,
		 timestamp_mode[encoder->timestamp_mode]);

	ret = device_property_read_u32(dev, "trace_timestamp_prescale",
				       &encoder->timestamp_prescale);
	if (ret)
		encoder->timestamp_prescale = 0; //default 0
	dev_info(dev, "trace_timestamp_prescale=%d\n", encoder->timestamp_prescale);

	return xuantie_ntrace_graph_parse(comp);
}
