/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 ZHIHE Group Holding Limited.
 */

#ifndef _ZHIHE_IPC_H
#define _ZHIHE_IPC_H

#include <linux/device.h>
#include <linux/types.h>
#include <linux/firmware/thead/thead,th1520-aon.h>

struct zhihe_aon_ipc;

int zhihe_aon_call_rpc(struct zhihe_aon_ipc *ipc,	void *msg, void *ack_msg, bool have_resp);
int zhihe_aon_get_handle(struct zhihe_aon_ipc **ipc, char *name);
int zhihe_aon_misc_set_control(struct zhihe_aon_ipc *ipc,	u16 resource, u32 ctrl, u32 val);
int zhihe_aon_misc_get_control(struct zhihe_aon_ipc *ipc, u16 resource, u32 ctrl, u32 *val);

#endif /* _ZHIHE_IPC_H */
