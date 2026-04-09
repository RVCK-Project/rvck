/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2021 ZHIHE Group Holding Limited.
 */

#ifndef __ZHIHE_PROC_DEBUG_H_
#define __ZHIHE_PROC_DEBUG_H_


void *zhihe_create_panic_log_proc(phys_addr_t log_phy, void *dir, void *log_addr, size_t size);
void zhihe_remove_panic_log_proc(void *arg);

#endif
