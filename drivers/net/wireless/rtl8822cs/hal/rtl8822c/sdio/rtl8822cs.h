/******************************************************************************
 *
 * Copyright(c) 2015 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _RTL8822CS_H_
#define _RTL8822CS_H_

#include <drv_types.h>		/* PADAPTER, struct dvobj_priv and etc. */


/* rtl8822cs_halinit.c */
u32 rtl8822cs_init(PADAPTER);
u32 rtl8822cs_deinit(PADAPTER adapter);
void rtl8822cs_init_default_value(PADAPTER);

/* rtl8822cs_halmac.c */
int rtl8822cs_halmac_init_adapter(PADAPTER);

/* rtl8822cs_io.c */
u32 rtl8822cs_read_port(struct dvobj_priv *, u32 cnt, u8 *mem);
u32 rtl8822cs_write_port(struct dvobj_priv *, u32 cnt, u8 *mem);

/* rtl8822cs_led.c */
void rtl8822cs_initswleds(PADAPTER);
void rtl8822cs_deinitswleds(PADAPTER);

/* rtl8822cs_xmit.c */
s32 rtl8822cs_init_xmit_priv(PADAPTER);
void rtl8822cs_free_xmit_priv(PADAPTER);
#ifdef CONFIG_RTW_MGMT_QUEUE
s32 rtl8822cs_hal_mgmt_xmit_enqueue(PADAPTER, struct xmit_frame *);
#endif
s32 rtl8822cs_hal_xmit_enqueue(PADAPTER, struct xmit_frame *);
s32 rtl8822cs_hal_xmit(PADAPTER, struct xmit_frame *);
s32 rtl8822cs_mgnt_xmit(PADAPTER, struct xmit_frame *);
s32 rtl8822cs_xmit_buf_handler(PADAPTER);
thread_return rtl8822cs_xmit_thread(thread_context);

/* rtl8822cs_recv.c */
s32 rtl8822cs_init_recv_priv(PADAPTER);
void rtl8822cs_free_recv_priv(PADAPTER);
_pkt *rtl8822cs_alloc_recvbuf_skb(struct recv_buf *, u32 size);
void rtl8822cs_free_recvbuf_skb(struct recv_buf *);
s32 rtl8822cs_recv_hdl(_adapter *adapter);
void rtl8822cs_rxhandler(PADAPTER, struct recv_buf *);

/* rtl8822cs_ops.c */
void rtl8822cs_get_interrupt(PADAPTER, u32 *hisr, u32 *rx_len);
void rtl8822cs_clear_interrupt(PADAPTER, u32 hisr);

#endif /* _RTL8822CS_H_ */
