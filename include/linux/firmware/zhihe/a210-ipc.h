/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 ZHIHE Group Holding Limited.
 */

#ifndef _ZHIHE_A210_IPC_H
#define _ZHIHE_A210_IPC_H

#include <linux/device.h>
#include <linux/types.h>

#define ZHIHE_A210_AON_RPC_VERSION 2
#define ZHIHE_A210_AON_RPC_MSG_NUM 7

struct zhihe_a210_aon_ipc;

enum zhihe_a210_aon_rpc_svc {
	ZHIHE_A210_AON_RPC_SVC_UNKNOWN = 0,
	ZHIHE_A210_AON_RPC_SVC_PM = 1,
	ZHIHE_A210_AON_RPC_SVC_MISC = 2,
	ZHIHE_A210_AON_RPC_SVC_AVFS = 3,
	ZHIHE_A210_AON_RPC_SVC_SYS = 4,
	ZHIHE_A210_AON_RPC_SVC_WDG = 5,
	ZHIHE_A210_AON_RPC_SVC_LPM = 6,
	ZHIHE_A210_AON_RPC_SVC_MAX = 0x3F,
};

enum zhihe_a210_aon_pm_func {
	ZHIHE_A210_AON_PM_FUNC_UNKNOWN = 0,
	ZHIHE_A210_AON_PM_FUNC_SET_RESOURCE_REGULATOR = 1,
	ZHIHE_A210_AON_PM_FUNC_GET_RESOURCE_REGULATOR = 2,
	ZHIHE_A210_AON_PM_FUNC_SET_RESOURCE_POWER_MODE = 3,
	ZHIHE_A210_AON_PM_FUNC_PWR_SET = 4,
	ZHIHE_A210_AON_PM_FUNC_PWR_GET = 5,
	ZHIHE_A210_AON_PM_FUNC_CHECK_FAULT = 6,
	ZHIHE_A210_AON_PM_FUNC_GET_TEMPERATURE = 7,
};

struct zhihe_a210_aon_rpc_msg_hdr {
	u8 ver; /* version of msg hdr */
	u8 size; /* msg size in bytes, includes the rpc msg header itself */
	u8 svc; /* rpc main service id */
	u8 func; /* rpc sub func id of specific service, sent by caller */
} __packed __aligned(1);

struct zhihe_a210_aon_rpc_ack_common {
	struct zhihe_a210_aon_rpc_msg_hdr hdr;
	u8 err_code;
} __packed __aligned(1);

#define ZHIHE_A210_RPC_SVC_MSG_TYPE_DATA 0
#define ZHIHE_A210_RPC_SVC_MSG_TYPE_ACK 1
#define ZHIHE_A210_RPC_SVC_MSG_NEED_ACK 0
#define ZHIHE_A210_RPC_SVC_MSG_NO_NEED_ACK 1

#define ZHIHE_A210_RPC_SET_VER(MESG, VER) ((MESG)->ver = (VER))
#define ZHIHE_A210_RPC_SET_SVC_ID(MESG, ID) ((MESG)->svc |= 0x3F & (ID))
#define ZHIHE_A210_RPC_SET_SVC_FLAG_MSG_TYPE(MESG, TYPE) \
	((MESG)->svc |= (TYPE) << 7)
#define ZHIHE_A210_RPC_SET_SVC_FLAG_ACK_TYPE(MESG, ACK) \
	((MESG)->svc |= (ACK) << 6)

#define ZHIHE_A210_RPC_SET_BE32(MESG, OFFSET, SET_DATA)	    \
	do {							    \
		u8 *data = (u8 *)(MESG);			    \
		u64 _offset = (OFFSET);				    \
		u64 _set_data = (SET_DATA);			    \
		data[_offset + 3] = (_set_data) & 0xFF;		    \
		data[_offset + 2] = (_set_data & 0xFF00) >> 8;	    \
		data[_offset + 1] = (_set_data & 0xFF0000) >> 16;   \
		data[_offset + 0] = (_set_data & 0xFF000000) >> 24; \
	} while (0)

#define ZHIHE_A210_RPC_SET_BE16(MESG, OFFSET, SET_DATA)	       \
	do {						       \
		u8 *data = (u8 *)(MESG);		       \
		u64 _offset = (OFFSET);			       \
		u64 _set_data = (SET_DATA);		       \
		data[_offset + 1] = (_set_data) & 0xFF;	       \
		data[_offset + 0] = (_set_data & 0xFF00) >> 8; \
	} while (0)

#define ZHIHE_A210_RPC_GET_BE32(MESG, OFFSET, PTR)                            \
	do {                                                                  \
		u8 *data = (u8 *)(MESG);                                      \
		u64 _offset = (OFFSET);                                       \
		*(u32 *)(PTR) =                                               \
			(data[_offset + 3] | data[_offset + 2] << 8 |         \
			 data[_offset + 1] << 16 | data[_offset + 0] << 24);  \
	} while (0)

#define ZHIHE_A210_RPC_GET_BE16(MESG, OFFSET, PTR)                          \
	do {                                                                \
		u8 *data = (u8 *)(MESG);                                    \
		u64 _offset = (OFFSET);                                     \
		*(u16 *)(PTR) = (data[_offset + 1] | data[_offset + 0] << 8); \
	} while (0)

int zhihe_a210_aon_call_rpc(struct zhihe_a210_aon_ipc *ipc, void *msg,
			    void *ack_msg, size_t ack_size, bool have_resp);
int zhihe_a210_aon_get_handle(struct zhihe_a210_aon_ipc **ipc, char *name);

#endif /* _ZHIHE_A210_IPC_H */
