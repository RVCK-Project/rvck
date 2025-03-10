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
#define _RTL8822CS_XMIT_C_

#include <drv_types.h>		/* PADAPTER, rtw_xmit.h and etc. */
#include <hal_data.h>		/* HAL_DATA_TYPE */
#include "../../hal_halmac.h"	/* rtw_halmac_sdio_tx_allowed() and etc. */
#include "../rtl8822c.h"	/* rtl8822c_update_txdesc() and etc. */


static s32 dequeue_writeport(PADAPTER adapter)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(adapter);
	struct xmit_buf *pxmitbuf;
	u32 polling_num = 0;

#ifdef CONFIG_SDIO_MULTI_FUNCTION_COEX
	if (!ex_hal_sdio_multi_if_bus_available(adapter)) {
		#if DBG_SDIO_MULTI_FUNCTION_COEX
		RTW_INFO("%s: TX, SDIO_MULTI_BT\n", __FUNCTION__);
		#endif
		/* wait signal instead of sleep? */
		rtw_usleep_os(10);
		return _FALSE;
	}
#endif

	pxmitbuf = select_and_dequeue_pending_xmitbuf(adapter);

	if (pxmitbuf == NULL)
		return _TRUE;

	/* check if hardware tx fifo page is enough */
	while (rtw_halmac_sdio_tx_allowed(pdvobjpriv, pxmitbuf->pdata, pxmitbuf->len)) {
		if (RTW_CANNOT_RUN(adapter)) {
			RTW_INFO("%s: bSurpriseRemoved(write port)\n", __func__);
			goto free_xmitbuf;
		}

		polling_num++;
		/* Only polling (0x7F / 10) times here, since rtw_halmac_sdio_tx_allowed() has polled 10 times within */
		if (((polling_num % (0x7F / 10))) == 0) {
			enqueue_pending_xmitbuf_to_head(pxmitpriv, pxmitbuf);
			rtw_msleep_os(1);
			return _FALSE;
		}
	}

#ifdef CONFIG_CHECK_LEAVE_LPS
	#ifdef CONFIG_LPS_CHK_BY_TP
	if (!adapter_to_pwrctl(adapter)->lps_chk_by_tp)
	#endif
	traffic_check_for_leave_lps(adapter, _TRUE, pxmitbuf->agg_num);
#endif

	rtw_write_port(adapter, 0, pxmitbuf->len, (u8 *)pxmitbuf);

free_xmitbuf:
	rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
#ifdef SDIO_FREE_XMIT_BUF_SEMA
	if (pxmitbuf->buf_tag == XMITBUF_DATA)
		rtw_sdio_free_xmitbuf_sema_up(pxmitpriv);
#endif

#ifdef CONFIG_SDIO_TX_TASKLET
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif

	return _FALSE;
}

/*
 * Description
 *	For MI call.
 */
s32 rtl8822cs_dequeue_writeport(PADAPTER adapter)
{
	return dequeue_writeport(adapter);
}

/*
 * Description
 *	Transmit xmitbuf to hardware tx fifo
 *
 * Return
 *	_SUCCESS	ok
 *	_FAIL		something error
 */
s32 rtl8822cs_xmit_buf_handler(PADAPTER adapter)
{
	struct xmit_priv *pxmitpriv;
	u8 queue_empty, queue_pending;
	s32 ret;


	pxmitpriv = &adapter->xmitpriv;

	ret = _rtw_down_sema(&pxmitpriv->xmit_sema);
	if (_FAIL == ret) {
		RTW_ERR("%s: down SdioXmitBufSema fail!\n", __FUNCTION__);
		return _FAIL;
	}

	if (RTW_CANNOT_RUN(adapter)) {
		RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
			 FUNC_ADPT_ARG(adapter),
			 rtw_is_drv_stopped(adapter) ? "True" : "False",
			 rtw_is_surprise_removed(adapter) ? "True" : "False");
		return _FAIL;
	}

	if (rtw_mi_check_pending_xmitbuf(adapter) == 0)
		return _SUCCESS;

#ifdef CONFIG_LPS_LCLK
	ret = rtw_register_tx_alive(adapter);
	if (ret != _SUCCESS)
		return _SUCCESS;
#endif

	do {
		queue_empty = rtw_mi_dequeue_writeport(adapter);
	} while (!queue_empty);

#ifdef CONFIG_LPS_LCLK
	rtw_unregister_tx_alive(adapter);
#endif

	return _SUCCESS;
}

/*
 * Description:
 *	Aggregation packets and send to hardware
 *
 * Return:
 *	0	Success
 *	-1	Hardware resource(TX FIFO) not ready
 *	-2	Software resource(xmitbuf) not ready
 */
static s32 xmit_xmitframes(PADAPTER adapter, struct xmit_priv *pxmitpriv)
{
	s32 err, ret;
	u32 k = 0;
	u8 max_agg_num;
	struct hw_xmit *hwxmits, *phwxmit;
	u8 idx, hwentry;
	_irqL irql;
	struct tx_servq *ptxservq;
	_list *sta_plist, *sta_phead, *frame_plist, *frame_phead;
	struct xmit_frame *pxmitframe;
	_queue *pframe_queue;
	struct xmit_buf *pxmitbuf;
	u32 txlen, max_txbuf_len, max_pg_num;
	u32 page_size, desc_size;
	int inx[4];
	u8 pre_qsel = 0xFF, next_qsel = 0xFF;
	u8 single_sta_in_queue = _FALSE;
#ifdef SDIO_FREE_XMIT_BUF_SEMA
	u32 consume;
#endif

	err = 0;
	hwxmits = pxmitpriv->hwxmits;
	hwentry = pxmitpriv->hwxmit_entry;
	ptxservq = NULL;
	pxmitframe = NULL;
	pframe_queue = NULL;
	pxmitbuf = NULL;
	max_txbuf_len = MAX_XMITBUF_SZ;
	max_agg_num = 0xFF;
	rtw_halmac_get_oqt_size(adapter_to_dvobj(adapter), &max_agg_num);
	rtw_hal_get_def_var(adapter, HAL_DEF_TX_PAGE_SIZE, &page_size);
	desc_size = rtl8822c_get_tx_desc_size(adapter);

	/* Limit max tx agg num to match with tx_allow rule */
	if (max_agg_num > 31) {
		max_agg_num = 31;
	}

#ifdef CONFIG_RTW_MGMT_QUEUE
	/* dump management frame directly */
	do {
		pxmitframe = rtw_dequeue_mgmt_xframe(pxmitpriv);
		if (pxmitframe)
			adapter->hal_func.mgnt_xmit(adapter, pxmitframe);
	} while (pxmitframe != NULL);

	hwentry--;
#endif

	if (adapter->registrypriv.wifi_spec == 1) {
		for (idx = 0; idx < 4; idx++)
			inx[idx] = pxmitpriv->wmm_para_seq[idx];
	} else {
		inx[0] = 0;
		inx[1] = 1;
		inx[2] = 2;
		inx[3] = 3;
	}

	/* 0(VO), 1(VI), 2(BE), 3(BK) */
	for (idx = 0; idx < hwentry; idx++) {
		phwxmit = hwxmits + inx[idx];
	#ifdef SDIO_FREE_XMIT_BUF_SEMA
		consume = 0;
	#endif

		if ((check_pending_xmitbuf(pxmitpriv) == _TRUE)
		    && (adapter->mlmepriv.LinkDetectInfo.bHigherBusyTxTraffic == _TRUE)) {
			if ((phwxmit->accnt > 0) && (phwxmit->accnt < 5)) {
				err = RTW_TX_WAIT_MORE_FRAME;
				break;
			}
		}

		rtw_halmac_get_tx_queue_page_num(adapter_to_dvobj(adapter), inx[idx], &max_pg_num);

		_enter_critical_bh(&pxmitpriv->lock, &irql);

		sta_phead = get_list_head(phwxmit->sta_queue);
		sta_plist = get_next(sta_phead);
		/*
		 * Because stop_sta_xmit may delete sta_plist at any time,
		 * so we should add lock here, or while loop can not exit
		 */

		single_sta_in_queue = rtw_end_of_queue_search(sta_phead, get_next(sta_plist));

		while (rtw_end_of_queue_search(sta_phead, sta_plist) == _FALSE) {
			ptxservq = LIST_CONTAINOR(sta_plist, struct tx_servq, tx_pending);
			sta_plist = get_next(sta_plist);

#ifdef DBG_XMIT_BUF
			RTW_INFO("%s idx:%d hwxmit_pkt_num:%d ptxservq_pkt_num:%d\n", __FUNCTION__, idx, phwxmit->accnt, ptxservq->qcnt);
			RTW_INFO("%s free_xmit_extbuf_cnt=%d free_xmitbuf_cnt=%d free_xmitframe_cnt=%d\n",
				__FUNCTION__, pxmitpriv->free_xmit_extbuf_cnt, pxmitpriv->free_xmitbuf_cnt,
				 pxmitpriv->free_xmitframe_cnt);
#endif
			pframe_queue = &ptxservq->sta_pending;

			frame_phead = get_list_head(pframe_queue);

			while (rtw_is_list_empty(frame_phead) == _FALSE) {
				frame_plist = get_next(frame_phead);
				pxmitframe = LIST_CONTAINOR(frame_plist, struct xmit_frame, list);

				/* check xmit_buf size enough or not */
				txlen = desc_size + rtw_wlan_pkt_size(pxmitframe);
				next_qsel = pxmitframe->attrib.qsel;
				if ((NULL == pxmitbuf)
				    || ((_RND(pxmitbuf->len, 8) + txlen) > max_txbuf_len)
				    || ((pxmitbuf->pg_num + PageNum(txlen, page_size)) > max_pg_num)
				    || (k == max_agg_num)
				    || ((k != 0) && (_FAIL == rtw_hal_busagg_qsel_check(adapter, pre_qsel, next_qsel)))) {
					if (pxmitbuf) {
						if (pxmitbuf->len > 0 && pxmitbuf->priv_data) {
							struct xmit_frame *pframe;

							pframe = (struct xmit_frame *)pxmitbuf->priv_data;
							pframe->agg_num = k;
							pxmitbuf->agg_num = k;
							rtl8822c_update_txdesc(pframe, pframe->buf_addr);
							rtw_free_xmitframe(pxmitpriv, pframe);
							pxmitbuf->priv_data = NULL;
							enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

							if (single_sta_in_queue == _FALSE) {
								/* break the loop in case there is more than one sta in this ac queue */
								pxmitbuf = NULL;
								err = RTW_TX_BALANCE;
								break;
							}
						} else {
							rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
						#ifdef SDIO_FREE_XMIT_BUF_SEMA
							consume--;
						#endif
						}
					}

					pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
					if (pxmitbuf == NULL) {
					#ifdef DBG_XMIT_BUF
						RTW_ERR("%s: xmit_buf is not enough!\n", __FUNCTION__);
					#endif
						err = RTW_XBUF_UNAVAIL;
					#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
						_rtw_up_sema(&GET_PRIMARY_ADAPTER(adapter)->xmitpriv.xmit_sema);
					#endif
						break;
					}

				#ifdef SDIO_FREE_XMIT_BUF_SEMA
					consume++;
				#endif
					k = 0;
				}

#ifdef CONFIG_AP_MODE
				if (MLME_IS_AP(adapter) || MLME_IS_MESH(adapter)) {
					if ((pxmitframe->attrib.psta->state & WIFI_SLEEP_STATE)
					    && (pxmitframe->attrib.triggered == 0)) {
						RTW_INFO("%s: one not triggered pkt in queue when this STA sleep,"
							" move to sleep_q and goto next sta\n", __func__);
						if (xmitframe_enqueue_for_sleeping_sta(adapter, pxmitframe) == _TRUE) {
							ptxservq->qcnt--;
							phwxmit->accnt--;
						}
						break;
					}
				}
#endif

				/* ok to send, remove frame from queue */
				rtw_list_delete(&pxmitframe->list);
				ptxservq->qcnt--;
				phwxmit->accnt--;

				if (k == 0) {
					pxmitbuf->ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);
					pxmitbuf->priv_data = (u8 *)pxmitframe;
				}

				/* coalesce the xmitframe to xmitbuf */
				pxmitframe->pxmitbuf = pxmitbuf;
				pxmitframe->buf_addr = pxmitbuf->ptail;

				ret = rtw_xmitframe_coalesce(adapter, pxmitframe->pkt, pxmitframe);
				if (ret == _FAIL) {
					RTW_ERR("%s: coalesce FAIL!", __FUNCTION__);
					/* Todo: error handler */
				} else {
					k++;
					if (k != 1)
						rtl8822c_update_txdesc(pxmitframe, pxmitframe->buf_addr);
					rtw_count_tx_stats(adapter, pxmitframe, pxmitframe->attrib.last_txcmdsz);
					pre_qsel = pxmitframe->attrib.qsel;
					txlen = desc_size + pxmitframe->attrib.last_txcmdsz;
					pxmitframe->pg_num =  PageNum(txlen, page_size);
					pxmitbuf->pg_num += pxmitframe->pg_num;
					pxmitbuf->ptail += _RND(txlen, 8); /* round to 8 bytes alignment */
					pxmitbuf->len = _RND(pxmitbuf->len, 8) + txlen;
				}

				if (k != 1)
					rtw_free_xmitframe(pxmitpriv, pxmitframe);
				pxmitframe = NULL;
			}
#if 0
			/* dump xmit_buf to hw tx fifo */
			if (pxmitbuf && (pxmitbuf->len > 0)) {
				struct xmit_frame *pframe;

				RTW_INFO("STA pxmitbuf->len=%d enqueue\n", pxmitbuf->len);

				pframe = (struct xmit_frame *)pxmitbuf->priv_data;
				pframe->agg_num = k;
				pxmitbuf->agg_num = k;
				rtl8822c_update_txdesc(pframe, pframe->buf_addr);
				rtw_free_xmitframe(pxmitpriv, pframe);
				pxmitbuf->priv_data = NULL;
				enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

				pxmitbuf = NULL;
			}
#endif
			if (_rtw_queue_empty(pframe_queue) == _TRUE)
				rtw_list_delete(&ptxservq->tx_pending);
			else if (err == RTW_TX_BALANCE) {
				/* Re-arrange the order of stations in this ac queue to balance the service for these stations */
				rtw_list_delete(&ptxservq->tx_pending);
				rtw_list_insert_tail(&ptxservq->tx_pending, get_list_head(phwxmit->sta_queue));
				err = 0;
			}

			if (err)
				break;
		}
		_exit_critical_bh(&pxmitpriv->lock, &irql);


#ifdef SDIO_FREE_XMIT_BUF_SEMA
#ifdef DBG_SDIO_FREE_XMIT_BUF_SEMA
		if (consume)
			RTW_INFO(FUNC_ADPT_FMT" acq[%u], consume:%u\n", FUNC_ADPT_ARG(adapter), inx[idx], consume);
#endif
		while (consume--)
			rtw_sdio_free_xmitbuf_sema_down(pxmitpriv);
#endif

		/* dump xmit_buf to hw tx fifo */
		if (pxmitbuf) {
			if (pxmitbuf->len > 0) {
				struct xmit_frame *pframe;

				pframe = (struct xmit_frame *)pxmitbuf->priv_data;
				pframe->agg_num = k;
				pxmitbuf->agg_num = k;
				rtl8822c_update_txdesc(pframe, pframe->buf_addr);
				rtw_free_xmitframe(pxmitpriv, pframe);
				pxmitbuf->priv_data = NULL;
				enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);
				rtw_yield_os();
			} else {
				rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
			#ifdef SDIO_FREE_XMIT_BUF_SEMA
				rtw_sdio_free_xmitbuf_sema_up(pxmitpriv);
			#endif
			}

			pxmitbuf = NULL;
		}

		if (err == RTW_XBUF_UNAVAIL)
			break;
	}

	return err;
}

/*
 * Description
 *	Transmit xmitframe from queue
 *
 * Return
 *	_SUCCESS	ok
 *	_FAIL		something error
 */
static s32 xmit_handler(PADAPTER adapter)
{
	struct xmit_priv *pxmitpriv;
	s32 ret;
	_irqL irql;


	pxmitpriv = &adapter->xmitpriv;

	ret = _rtw_down_sema(&pxmitpriv->SdioXmitSema);
	if (_FAIL == ret) {
		RTW_ERR("%s: down sema fail!\n", __FUNCTION__);
		return _FAIL;
	}

next:
	if (RTW_CANNOT_RUN(adapter)) {
		RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
			 FUNC_ADPT_ARG(adapter),
			 rtw_is_drv_stopped(adapter) ? "True" : "False",
			 rtw_is_surprise_removed(adapter) ? "True" : "False");
		return _FAIL;
	}

	_enter_critical_bh(&pxmitpriv->lock, &irql);
	ret = rtw_txframes_pending(adapter);
	_exit_critical_bh(&pxmitpriv->lock, &irql);
	/* All queues are empty! */
	if (!ret)
		return _SUCCESS;

	/* Dequeue frame and write to hardware */
	ret = xmit_xmitframes(adapter, pxmitpriv);
	if ((ret == RTW_XBUF_UNAVAIL) ||
		(ret == RTW_TX_WAIT_MORE_FRAME)) {
	#ifdef SDIO_FREE_XMIT_BUF_SEMA
		if (ret == RTW_XBUF_UNAVAIL) {
			rtw_sdio_free_xmitbuf_sema_down(pxmitpriv);
			rtw_sdio_free_xmitbuf_sema_up(pxmitpriv);
			goto next;
		}
	#endif

		/* _rtw_up_sema(&pxmitpriv->SdioXmitSema); */
		/*
		 * here sleep 1ms will cause big TP loss of TX
		 * from 50+ to 40+
		 */
		if (adapter->registrypriv.wifi_spec)
			rtw_msleep_os(1);
		else
#ifdef CONFIG_REDUCE_TX_CPU_LOADING
			rtw_msleep_os(1);
#else
#ifdef RTW_XMIT_THREAD_HIGH_PRIORITY_AGG
			rtw_usleep_os(10);
#else
			rtw_yield_os();
#endif
#endif
			goto next;
	}

	return _SUCCESS;
}

thread_return rtl8822cs_xmit_thread(thread_context context)
{
	s32 ret;
	PADAPTER adapter;
	u8 thread_name[20] = {0};
#ifdef RTW_XMIT_THREAD_HIGH_PRIORITY_AGG
#ifdef PLATFORM_LINUX
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0))
	sched_set_fifo_low(current);
#else
	struct sched_param param = { .sched_priority = 1 };

	sched_setscheduler(current, SCHED_FIFO, &param);
#endif
#endif /* PLATFORM_LINUX */
#endif /* RTW_XMIT_THREAD_HIGH_PRIORITY_AGG */

	ret = _SUCCESS;
	adapter = (PADAPTER)context;

	rtw_sprintf(thread_name, 20, "RTWHALXT-"ADPT_FMT, ADPT_ARG(adapter));
	thread_enter(thread_name);

	RTW_INFO("start "FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));

	do {
		ret = xmit_handler(adapter);
		flush_signals_thread();
	} while (_SUCCESS == ret);

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(adapter));

	rtw_thread_wait_stop();

	return 0;
}

/*
 * Description:
 *	Transmit manage frame
 *
 * Return:
 *	_SUCCESS	ok or enqueue
 *	_FAIL		fail
 */
s32 rtl8822cs_mgnt_xmit(PADAPTER adapter, struct xmit_frame *pmgntframe)
{
	s32 ret = _SUCCESS;
	struct xmit_priv *pxmitpriv;
	struct pkt_attrib *pattrib;
	struct xmit_buf *pxmitbuf;
	u32 page_size, desc_size;
	u16 subtype;
	u8 *pframe;

	pxmitpriv = &adapter->xmitpriv;
	pattrib = &pmgntframe->attrib;
	pxmitbuf = pmgntframe->pxmitbuf;
	rtw_hal_get_def_var(adapter, HAL_DEF_TX_PAGE_SIZE, &page_size);
	desc_size = rtl8822c_get_tx_desc_size(adapter);

	rtl8822c_update_txdesc(pmgntframe, pmgntframe->buf_addr);

	pxmitbuf->len = desc_size + pattrib->last_txcmdsz;
	pxmitbuf->pg_num = PageNum(pxmitbuf->len, page_size);
	pxmitbuf->ptail = pmgntframe->buf_addr + pxmitbuf->len;

	pframe = pmgntframe->buf_addr + desc_size;
	subtype = get_frame_sub_type(pframe);

	rtw_count_tx_stats(adapter, pmgntframe, pattrib->last_txcmdsz);

	rtw_free_xmitframe(pxmitpriv, pmgntframe);
	pxmitbuf->priv_data = NULL;

	if (subtype == WIFI_BEACON) {
		/* dump beacon directly */
		ret = rtw_write_port(adapter, 0, pxmitbuf->len, (u8 *)pxmitbuf);
		if (ret != _SUCCESS)
			rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_WRITE_PORT_ERR);

		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
	} else
		enqueue_pending_xmitbuf(pxmitpriv, pxmitbuf);

	return ret;
}

/*
 * Description:
 *	Enqueue management xmitframe
 *
 * Return:
 *	_TRUE	enqueue ok
 *	_FALSE	fail
 */
#ifdef CONFIG_RTW_MGMT_QUEUE
s32 rtl8822cs_hal_mgmt_xmit_enqueue(PADAPTER adapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv *pxmitpriv;
	s32 ret;

	pxmitpriv = &adapter->xmitpriv;

	ret = rtw_mgmt_xmitframe_enqueue(adapter, pxmitframe);
	if (ret != _SUCCESS) {
		rtw_free_xmitbuf(pxmitpriv, pxmitframe->pxmitbuf);
		rtw_free_xmitframe(pxmitpriv, pxmitframe);
		pxmitpriv->tx_drop++;
		return _FALSE;
	}

#ifdef CONFIG_SDIO_TX_TASKLET
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#else
	_rtw_up_sema(&pxmitpriv->SdioXmitSema);
#endif

	return _TRUE;
}
#endif

/*
 * Description:
 *	Enqueue xmitframe
 *
 * Return:
 *	_TRUE	enqueue ok
 *	_FALSE	fail
 */
s32 rtl8822cs_hal_xmit_enqueue(PADAPTER adapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv *pxmitpriv;
	s32 ret;


	pxmitpriv = &adapter->xmitpriv;

	ret = rtw_xmitframe_enqueue(adapter, pxmitframe);
	if (ret != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pxmitframe);
		pxmitpriv->tx_drop++;
		return _FALSE;
	}

#ifdef CONFIG_SDIO_TX_TASKLET
	tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#else /* !CONFIG_SDIO_TX_TASKLET */
	_rtw_up_sema(&pxmitpriv->SdioXmitSema);
#endif /* !CONFIG_SDIO_TX_TASKLET */

	return _TRUE;
}

/*
 * Description:
 *	Handle xmitframe(packet) come from rtw_xmit()
 *
 * Return:
 *	_TRUE	handle packet directly, maybe ok or drop
 *	_FALSE	enqueue, temporary can't transmit packets to hardware
 */
s32 rtl8822cs_hal_xmit(PADAPTER adapter, struct xmit_frame *pxmitframe)
{
	struct xmit_priv *pxmitpriv;
	_irqL irql;
	s32 ret;


	pxmitframe->attrib.qsel = pxmitframe->attrib.priority;
	pxmitpriv = &adapter->xmitpriv;

#ifdef CONFIG_80211N_HT
	if ((pxmitframe->frame_tag == DATA_FRAMETAG)
	    && (pxmitframe->attrib.ether_type != 0x0806)
	    && (pxmitframe->attrib.ether_type != 0x888e)
	    && (pxmitframe->attrib.dhcp_pkt != 1)) {
		rtw_issue_addbareq_cmd(adapter, pxmitframe, _TRUE);
	}
#endif /* CONFIG_80211N_HT */

	_enter_critical_bh(&pxmitpriv->lock, &irql);
	ret = rtl8822cs_hal_xmit_enqueue(adapter, pxmitframe);
	_exit_critical_bh(&pxmitpriv->lock, &irql);
	if (ret != _TRUE) {
		RTW_INFO("%s: enqueue xmitframe FAIL!\n", __FUNCTION__);
		return _TRUE;
	}

	return _FALSE;
}
/*
 * Return
 *	_SUCCESS	start thread ok
 *	_FAIL		start thread fail
 *
 */
s32 rtl8822cs_init_xmit_priv(PADAPTER adapter)
{
	struct xmit_priv *xmitpriv;


	xmitpriv = &adapter->xmitpriv;

	_rtw_init_sema(&xmitpriv->SdioXmitSema, 0);
#ifdef SDIO_FREE_XMIT_BUF_SEMA
	_rtw_init_sema(&xmitpriv->sdio_free_xmitbuf_sema,
		xmitpriv->free_xmitbuf_cnt);
#endif
	rtl8822c_init_xmit_priv(adapter);
	return _SUCCESS;
}

void rtl8822cs_free_xmit_priv(PADAPTER adapter)
{
	struct xmit_priv *pxmitpriv;
	struct xmit_buf *pxmitbuf;
	_queue *pqueue;
	_list *plist, *phead;
	_list tmplist;
	_irqL irql;


	pxmitpriv = &adapter->xmitpriv;
	pqueue = &pxmitpriv->pending_xmitbuf_queue;
	phead = get_list_head(pqueue);
	_rtw_init_listhead(&tmplist);

	_enter_critical_bh(&pqueue->lock, &irql);
	if (_rtw_queue_empty(pqueue) == _FALSE) {
		/*
		 * Insert tmplist to end of queue, and delete phead
		 * then tmplist become head of queue.
		 */
		rtw_list_insert_tail(&tmplist, phead);
		rtw_list_delete(phead);
	}
	_exit_critical_bh(&pqueue->lock, &irql);

	phead = &tmplist;
	while (rtw_is_list_empty(phead) == _FALSE) {
		plist = get_next(phead);
		rtw_list_delete(plist);

		pxmitbuf = LIST_CONTAINOR(plist, struct xmit_buf, list);
		rtw_free_xmitframe(pxmitpriv, (struct xmit_frame *)pxmitbuf->priv_data);
		pxmitbuf->priv_data = NULL;
		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
	#ifdef SDIO_FREE_XMIT_BUF_SEMA
		if (pxmitbuf->buf_tag == XMITBUF_DATA)
			rtw_sdio_free_xmitbuf_sema_up(pxmitpriv);
	#endif
	}
}
