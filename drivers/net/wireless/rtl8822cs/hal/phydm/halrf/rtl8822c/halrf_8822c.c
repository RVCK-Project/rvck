/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#include "../../mp_precomp.h"
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if RT_PLATFORM == PLATFORM_MACOSX
#include "phydm_precomp.h"
#else
#include "../phydm_precomp.h"
#endif
#else
#include "../../phydm_precomp.h"
#endif

#if (RTL8822C_SUPPORT == 1)
void halrf_rf_lna_setting_8822c(struct dm_struct *dm_void,
				enum halrf_lna_set type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 path = 0x0;

	for (path = 0x0; path < 2; path++)
		if (type == HALRF_LNA_DISABLE) {
			/*S0*/
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0xef, BIT(19),
				       0x1);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x33,
				       RFREGOFFSETMASK, 0x00003);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x3e,
				       RFREGOFFSETMASK, 0x00064);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x3f,
				       RFREGOFFSETMASK, 0x0afce);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0xef, BIT(19),
				       0x0);
		} else if (type == HALRF_LNA_ENABLE) {
			/*S0*/
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0xef, BIT(19),
				       0x1);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x33,
				       RFREGOFFSETMASK, 0x00003);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x3e,
				       RFREGOFFSETMASK, 0x00064);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0x3f,
				       RFREGOFFSETMASK, 0x1afce);
			odm_set_rf_reg(dm, (enum rf_path)path, RF_0xef, BIT(19),
				       0x0);
		}
}

void odm_tx_pwr_track_set_pwr8822c(void *dm_void, enum pwrtrack_method method,
				   u8 rf_path, u8 channel_mapped_index)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_rf_calibration_struct *cali_info = &(dm->rf_calibrate_info);
	struct _hal_rf_ *rf = &dm->rf_table;
	struct _halrf_tssi_data *tssi = &rf->halrf_tssi_data;

	u32 bitmask_6_0 = BIT(6) | BIT(5) | BIT(4) | BIT(3) |
				BIT(2) | BIT(1) | BIT(0);

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
	       "pRF->absolute_ofdm_swing_idx=%d   pRF->remnant_ofdm_swing_idx=%d   pRF->absolute_cck_swing_idx=%d   pRF->remnant_cck_swing_idx=%d   rf_path=%d\n",
	       cali_info->absolute_ofdm_swing_idx[rf_path], cali_info->remnant_ofdm_swing_idx[rf_path], cali_info->absolute_cck_swing_idx[rf_path], cali_info->remnant_cck_swing_idx, rf_path);

	if (method == CLEAN_MODE) { /*use for mp driver clean power tracking status*/
		RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "===> %s method=%d clear power tracking rf_path=%d\n",
		       __func__, method, rf_path);
		tssi->tssi_trk_txagc_offset[rf_path] = 0;

		switch (rf_path) {
		case RF_PATH_A:
			odm_set_bb_reg(dm, R_0x18a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			odm_set_rf_reg(dm, rf_path, RF_0x7f, 0x00002, 0x0);
			odm_set_rf_reg(dm, rf_path, RF_0x7f, 0x00100, 0x0);
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x18a0,
			       odm_get_bb_reg(dm, R_0x18a0, bitmask_6_0));
			break;
		case RF_PATH_B:
			odm_set_bb_reg(dm, R_0x41a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			odm_set_rf_reg(dm, rf_path, RF_0x7f, 0x00002, 0x0);
			odm_set_rf_reg(dm, rf_path, RF_0x7f, 0x00100, 0x0);
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x41a0,
			       odm_get_bb_reg(dm, R_0x41a0, bitmask_6_0));
			break;
		default:
			break;
		}
	} else if (method == BBSWING) { /*use for mp driver clean power tracking status*/
		switch (rf_path) {
		case RF_PATH_A:
			odm_set_bb_reg(dm, R_0x18a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x18a0, odm_get_bb_reg(dm, R_0x18a0, bitmask_6_0));
			break;
		case RF_PATH_B:
			odm_set_bb_reg(dm, R_0x41a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x41a0, odm_get_bb_reg(dm, R_0x41a0, bitmask_6_0));
			break;
		default:
			break;
		}
	} else if (method == MIX_MODE) {
		switch (rf_path) {
		case RF_PATH_A:
			odm_set_bb_reg(dm, R_0x18a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x18a0, odm_get_bb_reg(dm, R_0x18a0, bitmask_6_0));
			break;
		case RF_PATH_B:
			odm_set_bb_reg(dm, R_0x41a0, bitmask_6_0, (cali_info->absolute_ofdm_swing_idx[rf_path] & 0x7f));
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
			       "Path-%d 0x%x=0x%x\n", rf_path, R_0x41a0, odm_get_bb_reg(dm, R_0x41a0, bitmask_6_0));
			break;
		default:
			break;
		}
	}
}

void get_delta_swing_table_8822c(void *dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_rf_calibration_struct *cali_info = &dm->rf_calibrate_info;
	u8 channel = *dm->channel;
	u8 tx_rate = phydm_get_tx_rate(dm);

	if (channel >= 1 && channel <= 14) {
		if (IS_CCK_RATE(tx_rate)) {
			*temperature_up_a = cali_info->delta_swing_table_idx_2g_cck_a_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2g_cck_a_n;
			*temperature_up_b = cali_info->delta_swing_table_idx_2g_cck_b_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2g_cck_b_n;
		} else {
			*temperature_up_a = cali_info->delta_swing_table_idx_2ga_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2ga_n;
			*temperature_up_b = cali_info->delta_swing_table_idx_2gb_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2gb_n;
		}
	}

	if (channel >= 36 && channel <= 64) {
		*temperature_up_a = cali_info->delta_swing_table_idx_5ga_p[0];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[0];
		*temperature_up_b = cali_info->delta_swing_table_idx_5gb_p[0];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[0];
	} else if (channel >= 100 && channel <= 144) {
		*temperature_up_a = cali_info->delta_swing_table_idx_5ga_p[1];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[1];
		*temperature_up_b = cali_info->delta_swing_table_idx_5gb_p[1];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[1];
	} else if (channel >= 149 && channel <= 177) {
		*temperature_up_a = cali_info->delta_swing_table_idx_5ga_p[2];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[2];
		*temperature_up_b = cali_info->delta_swing_table_idx_5gb_p[2];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[2];
	}
}

void _phy_aac_calibrate_8822c(struct dm_struct *dm)
{
#if 1
	u32 cnt = 0;

	RF_DBG(dm, DBG_RF_LCK, "[AACK]AACK start!!!!!!!\n");
	//odm_set_rf_reg(dm, RF_PATH_A, 0xbb, RFREGOFFSETMASK, 0x80010);
	odm_set_rf_reg(dm, RF_PATH_A, 0xb0, RFREGOFFSETMASK, 0x1F0FA);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xca, RFREGOFFSETMASK, 0x80000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xc9, RFREGOFFSETMASK, 0x80001);
	for (cnt = 0; cnt < 100; cnt++) {
		ODM_delay_ms(1);
		if (odm_get_rf_reg(dm, RF_PATH_A, RF_0xca, 0x1000) != 0x1)
			break;
	}

	odm_set_rf_reg(dm, RF_PATH_A, RF_0xb0, RFREGOFFSETMASK, 0x1F0F8);
	//odm_set_rf_reg(dm, RF_PATH_B, 0xbb, RFREGOFFSETMASK, 0x80010);

	RF_DBG(dm, DBG_RF_IQK, "[AACK]AACK end!!!!!!!\n");
#endif
}
void _phy_rt_calibrate_8822c(struct dm_struct *dm)
{
	RF_DBG(dm, DBG_RF_IQK, "[RTK]RTK start!!!!!!!\n");
	odm_set_rf_reg(dm, RF_PATH_A, 0xcc, RFREGOFFSETMASK, 0x0f000);
	odm_set_rf_reg(dm, RF_PATH_A, 0xcc, RFREGOFFSETMASK, 0x4f000);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_A, 0xcc, RFREGOFFSETMASK, 0x0f000);
	RF_DBG(dm, DBG_RF_IQK, "[RTK]RTK end!!!!!!!\n");
}

void halrf_reload_bp_8822c(struct dm_struct *dm, u32 *bp_reg, u32 *bp)
{
	u32 i;

	for (i = 0; i < DACK_REG_8822C; i++)
		odm_write_4byte(dm, bp_reg[i], bp[i]);
}

void halrf_reload_bprf_8822c(struct dm_struct *dm, u32 *bp_reg, u32 bp[][2])
{
	u32 i;

	for (i = 0; i < DACK_RF_8822C; i++) {
		odm_set_rf_reg(dm, RF_PATH_A, bp_reg[i], MASK20BITS,
			       bp[i][RF_PATH_A]);
		odm_set_rf_reg(dm, RF_PATH_B, bp_reg[i], MASK20BITS,
			       bp[i][RF_PATH_B]);
	}
}

void halrf_bp_8822c(struct dm_struct *dm, u32 *bp_reg, u32 *bp)
{
	u32 i;

	for (i = 0; i < DACK_REG_8822C; i++)
		bp[i] = odm_read_4byte(dm, bp_reg[i]);
}

void halrf_bprf_8822c(struct dm_struct *dm, u32 *bp_reg, u32 bp[][2])
{
	u32 i;

	for (i = 0; i < DACK_RF_8822C; i++) {
		bp[i][RF_PATH_A] =
			odm_get_rf_reg(dm, RF_PATH_A, bp_reg[i], MASK20BITS);
		bp[i][RF_PATH_B] =
			odm_get_rf_reg(dm, RF_PATH_B, bp_reg[i], MASK20BITS);
	}
}

void halrf_swap_8822c(struct dm_struct *dm, u32 *v1, u32 *v2)
{
	u32 temp;

	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void halrf_bubble_8822c(struct dm_struct *dm, u32 *v1, u32 *v2)
{
	u32 temp;

	if (*v1 >= 0x200 && *v2 >= 0x200) {
		if (*v1 > *v2)
			halrf_swap_8822c(dm, v1, v2);
	} else if (*v1 < 0x200 && *v2 < 0x200) {
		if (*v1 > *v2)
			halrf_swap_8822c(dm, v1, v2);
	} else if (*v1 < 0x200 && *v2 >= 0x200) {
		halrf_swap_8822c(dm, v1, v2);
	}
}

void halrf_b_sort_8822c(struct dm_struct *dm, u32 *iv, u32 *qv)
{
	u32 temp;
	u32 i, j;

	RF_DBG(dm, DBG_RF_DACK, "[DACK]bubble!!!!!!!!!!!!");
	for (i = 0; i < SN - 1; i++) {
		for (j = 0; j < (SN - 1 - i) ; j++) {
			halrf_bubble_8822c(dm, &iv[j], &iv[j + 1]);
			halrf_bubble_8822c(dm, &qv[j], &qv[j + 1]);
		}
	}
}

void halrf_minmax_compare_8822c(struct dm_struct *dm, u32 value, u32 *min,
				u32 *max)
{
	if (value >= 0x200) {
		if (*min >= 0x200) {
			if (*min > value)
				*min = value;
		} else {
			*min = value;
		}
		if (*max >= 0x200) {
			if (*max < value)
				*max = value;
		}
	} else {
		if (*min < 0x200) {
			if (*min > value)
				*min = value;
		}

		if (*max  >= 0x200) {
			*max = value;
		} else {
			if (*max < value)
				*max = value;
		}
	}
}

boolean halrf_compare_8822c(struct dm_struct *dm, u32 value)
{
	boolean fail = false;

	if (value >= 0x200 && (0x400 - value) > 0x64)
		fail = true;
	else if (value < 0x200 && value > 0x64)
		fail = true;

	if (fail)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]overflow!!!!!!!!!!!!!!!");
	return fail;
}

void halrf_mode_8822c(struct dm_struct *dm, u32 *i_value, u32 *q_value)
{
	u32 iv[SN], qv[SN], im[SN], qm[SN], temp, temp1, temp2;
	u32 p, m, t;
	u32 i_max = 0, q_max = 0, i_min = 0x0, q_min = 0x0, c = 0x0;
	u32 i_delta, q_delta;
	u8 i, j, ii = 0, qi = 0;
	boolean fail = false;

//	ODM_delay_ms(10);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]pathA RF0x0 = 0x%x",
	       odm_get_rf_reg(dm, 0x0, 0x0, 0xfffff));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]pathB RF0x0 = 0x%x",
	       odm_get_rf_reg(dm, 0x1, 0x0, 0xfffff));

	for (i = 0; i < SN; i++) {
		im[i] = 0;
		qm[i] = 0;
	}

	i = 0;
	c = 0;
	while (i < SN && c < 10000) {
		c++;
		temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
		iv[i] = (temp & 0x3ff000) >> 12;
		qv[i] = temp & 0x3ff;

		fail = false;
		if (halrf_compare_8822c(dm, iv[i]))
			fail = true;
		if (halrf_compare_8822c(dm, qv[i]))
			fail = true;
		if (!fail)
			i++;
	}

	c = 0;
	do {
		i_min = iv[0];
		i_max = iv[0];
		q_min = qv[0];
		q_max = qv[0];
		for (i = 0; i < SN; i++) {
			halrf_minmax_compare_8822c(dm, iv[i], &i_min, &i_max);
			halrf_minmax_compare_8822c(dm, qv[i], &q_min, &q_max);
		}

		c++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]i_min=0x%x, i_max=0x%x",
		       i_min, i_max);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]q_min=0x%x, q_max=0x%x",
		       q_min, q_max);

		if (i_max < 0x200 && i_min < 0x200)
			i_delta = i_max - i_min;
		else if (i_max >= 0x200 && i_min >= 0x200)
			i_delta = i_max - i_min;
		else
			i_delta = i_max + (0x400 - i_min);

		if (q_max < 0x200 && q_min < 0x200)
			q_delta = q_max - q_min;
		else if (q_max >= 0x200 && q_min >= 0x200)
			q_delta = q_max - q_min;
		else
			q_delta = q_max + (0x400 - q_min);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]i_delta=0x%x, q_delta=0x%x",
		       i_delta, q_delta);
		halrf_b_sort_8822c(dm, iv, qv);
		if (i_delta > 5 || q_delta > 5) {
//			halrf_b_sort_8822c(dm, iv, qv);
			temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
			iv[0] = (temp & 0x3ff000) >> 12;
			qv[0] = temp & 0x3ff;
			temp = odm_get_bb_reg(dm, 0x2dbc, 0x3fffff);
			iv[SN - 1] = (temp & 0x3ff000) >> 12;
			qv[SN - 1] = temp & 0x3ff;
		} else {
			break;
		}
	} while (c < 100);
#if 0
	for (i = 0; i < SN; i++) {
		for (j = 0; j < SN; j++) {
			if (i != j) {
				if (iv[i] == iv[j])
					im[i]++;
				if (qv[i] == qv[j])
					qm[i]++;
			}
		}
	}

	for (i = 0; i < SN; i++)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]iv[%d] = 0x%x\n", i, iv[i]);

	for (i = 0; i < SN; i++)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]qv[%d] = 0x%x\n", i, qv[i]);

	for (i = 1; i < SN; i++) {
		if (im[ii] < im[i])
			ii = i;
		if (qm[qi] < qm[i])
			qi = i;
	}

	*i_value = iv[ii];
	*q_value = qv[qi];
#endif
#if 1
#if 0
	for (i = 0; i < SN; i++)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]iv[%d] = 0x%x\n", i, iv[i]);

	for (i = 0; i < SN; i++)
		RF_DBG(dm, DBG_RF_DACK, "[DACK]qv[%d] = 0x%x\n", i, qv[i]);
#endif
	/*i*/
	m = 0;
	p = 0;
	for (i = 10; i < SN - 10; i++) {
		if (iv[i] > 0x200)
			m = (0x400 - iv[i]) + m;
		else
			p = iv[i] + p;
	}

	if (p > m) {
		t = p - m;
		t = t / (SN - 20);
	} else {
		t = m - p;
		t = t / (SN - 20);
		if (t != 0x0)
			t = 0x400 - t;
	}
	*i_value = t;
	/*q*/
	m = 0;
	p = 0;
	for (i = 10; i < SN - 10; i++) {
		if (qv[i] > 0x200)
			m = (0x400 - qv[i]) + m;
		else
			p = qv[i] + p;
	}
	if (p > m) {
		t = p - m;
		t = t / (SN - 20);
	} else {
		t = m - p;
		t = t / (SN - 20);
		if (t != 0x0)
			t = 0x400 - t;
	}
	*q_value = t;
#endif
}

void halrf_biask_backup_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;

	dack->biask_d[0][0]= (u8)odm_get_bb_reg(dm, 0x2810, 0x1ff8);
	dack->biask_d[0][1]= (u8)odm_get_bb_reg(dm, 0x283c, 0x1ff8);
	dack->biask_d[1][0]= (u8)odm_get_bb_reg(dm, 0x4510, 0x1ff8);
	dack->biask_d[1][1]= (u8)odm_get_bb_reg(dm, 0x453c, 0x1ff8);
}

void halrf_dck_backup_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;

	dack->dck_d[0][0][0] = (u8)odm_get_bb_reg(dm, 0x18bc, 0xf0000000);
	dack->dck_d[0][0][1] = (u8)odm_get_bb_reg(dm, 0x18c0, 0xf);
	dack->dck_d[0][1][0] = (u8)odm_get_bb_reg(dm, 0x18d8, 0xf0000000);
	dack->dck_d[0][1][1] = (u8)odm_get_bb_reg(dm, 0x18dc, 0xf);

	dack->dck_d[1][0][0] = (u8)odm_get_bb_reg(dm, 0x41bc, 0xf0000000);
	dack->dck_d[1][0][1] = (u8)odm_get_bb_reg(dm, 0x41c0, 0xf);
	dack->dck_d[1][1][0] = (u8)odm_get_bb_reg(dm, 0x41d8, 0xf0000000);
	dack->dck_d[1][1][1] = (u8)odm_get_bb_reg(dm, 0x41dc, 0xf);
}
void halrf_dack_backup_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;

	u8 i;
	u32 temp1, temp2, temp3;

	temp1 = odm_get_bb_reg(dm, 0x1860, MASKDWORD);
	temp2 = odm_get_bb_reg(dm, 0x4160, MASKDWORD);
	temp3 = odm_get_bb_reg(dm, 0x9b4, MASKDWORD);

	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, 0xdb66db00);

	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x1860, 0xfc000000, 0x3c);

	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18b0, 0xf0000000, i);
		dack->msbk_d[0][0][i] = (u16)odm_get_bb_reg(dm, 0x2810,
							    0x7fc0000);
	}

	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18cc, 0xf0000000, i);
		dack->msbk_d[0][1][i] = (u16)odm_get_bb_reg(dm, 0x283c,
							    0x7fc0000);
	}

	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x4160, 0xfc000000, 0x3c);

	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41b0, 0xf0000000, i);
		dack->msbk_d[1][0][i] = (u16)odm_get_bb_reg(dm, 0x4510,
							    0x7fc0000);
	}

	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41cc, 0xf0000000, i);
		dack->msbk_d[1][1][i] = (u16)odm_get_bb_reg(dm, 0x453c,
							    0x7fc0000);
	}
	halrf_dck_backup_8822c(dm);
	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x1860, MASKDWORD, temp1);
	odm_set_bb_reg(dm, 0x4160, MASKDWORD, temp2);
	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, temp3);
	halrf_biask_backup_8822c(dm);
}

void halrf_biask_restore_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;

	odm_set_bb_reg(dm, 0x18b0, 0x1ff8000, dack->biask_d[0][0]);
	odm_set_bb_reg(dm, 0x18cc, 0x1ff8000, dack->biask_d[0][1]);
	odm_set_bb_reg(dm, 0x41b0, 0x1ff8000, dack->biask_d[1][0]);
	odm_set_bb_reg(dm, 0x41cc, 0x1ff8000, dack->biask_d[1][1]);
}

void halrf_dck_restore_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;

	odm_set_bb_reg(dm, 0x18bc, BIT(19), 0x1);
	odm_set_bb_reg(dm, 0x18bc, 0xf0000000, dack->dck_d[0][0][0]);
	odm_set_bb_reg(dm, 0x18c0, 0xf, dack->dck_d[0][0][1]);
	odm_set_bb_reg(dm, 0x18d8, BIT(19), 0x1);
	odm_set_bb_reg(dm, 0x18d8, 0xf0000000, dack->dck_d[0][1][0]);
	odm_set_bb_reg(dm, 0x18dc, 0xf, dack->dck_d[0][1][1]);

	odm_set_bb_reg(dm, 0x41bc, BIT(19), 0x1);
	odm_set_bb_reg(dm, 0x41bc, 0xf0000000, dack->dck_d[1][0][0]);
	odm_set_bb_reg(dm, 0x41c0, 0xf, dack->dck_d[1][0][1]);
	odm_set_bb_reg(dm, 0x41d8, BIT(19), 0x1);
	odm_set_bb_reg(dm, 0x41d8, 0xf0000000, dack->dck_d[1][1][0]);
	odm_set_bb_reg(dm, 0x41dc, 0xf, dack->dck_d[1][1][1]);
}
void halrf_dack_restore_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;
	u8 i;
	u32 c = 0x0;
	u32 temp1, temp2, temp3;

	if (dack->dack_en == false)
		return;

	temp1 = odm_get_bb_reg(dm, 0x1860, MASKDWORD);
	temp2 = odm_get_bb_reg(dm, 0x4160, MASKDWORD);
	temp3 = odm_get_bb_reg(dm, 0x9b4, MASKDWORD);

	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, 0xdb66db00);

	odm_set_bb_reg(dm, 0x18b0, BIT(27), 0x0);
	odm_set_bb_reg(dm, 0x18cc, BIT(27), 0x0);
	odm_set_bb_reg(dm, 0x41b0, BIT(27), 0x0);
	odm_set_bb_reg(dm, 0x41cc, BIT(27), 0x0);

	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x1860, 0xfc000000, 0x3c);
	odm_set_bb_reg(dm, 0x18b4, BIT(0), 0x1);
	odm_set_bb_reg(dm, 0x18d0, BIT(0), 0x1);

	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x4160, 0xfc000000, 0x3c);
	odm_set_bb_reg(dm, 0x41b4, BIT(0), 0x1);
	odm_set_bb_reg(dm, 0x41d0, BIT(0), 0x1);

	odm_set_bb_reg(dm, 0x18b0, 0xf00, 0x0);
	odm_set_bb_reg(dm, 0x18c0, BIT(14), 0x0);
	odm_set_bb_reg(dm, 0x18cc, 0xf00, 0x0);
	odm_set_bb_reg(dm, 0x18dc, BIT(14), 0x0);

	odm_set_bb_reg(dm, 0x18b0, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x18cc, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x18b0, BIT(0), 0x1);
	odm_set_bb_reg(dm, 0x18cc, BIT(0), 0x1);

	halrf_dck_restore_8822c(dm);

	odm_set_bb_reg(dm, 0x18c0, 0x38000, 0x7);
	odm_set_bb_reg(dm, 0x18dc, 0x38000, 0x7);
	odm_set_bb_reg(dm, 0x41c0, 0x38000, 0x7);
	odm_set_bb_reg(dm, 0x41dc, 0x38000, 0x7);

	odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x1);
	odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x1);

	odm_set_bb_reg(dm, 0x41b0, 0xf00, 0x0);
	odm_set_bb_reg(dm, 0x41c0, BIT(14), 0x0);
	odm_set_bb_reg(dm, 0x41cc, 0xf00, 0x0);
	odm_set_bb_reg(dm, 0x41dc, BIT(14), 0x0);

	odm_set_bb_reg(dm, 0x41b0, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x41cc, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x41b0, BIT(0), 0x1);
	odm_set_bb_reg(dm, 0x41cc, BIT(0), 0x1);

	odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x1);
	odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x1);
#if 1
	c = 0x0;
	while (c < 10000) {
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x2808=0x%x",
		      odm_get_bb_reg(dm, 0x2808, 0x7fff80));
		c++;
		if (odm_get_bb_reg(dm, 0x2808, 0x7fff80) == 0xffff)
			break;
	}
	c = 0x0;
	while (c < 10000) {
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x2834=0x%x",
		      odm_get_bb_reg(dm, 0x2834, 0x7fff80));
		c++;
		if (odm_get_bb_reg(dm, 0x2834, 0x7fff80) == 0xffff)
			break;
	}
	c = 0x0;
	while (c < 10000) {
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x4508=0x%x",
		      odm_get_bb_reg(dm, 0x4508, 0x7fff80));
		c++;
		if (odm_get_bb_reg(dm, 0x4508, 0x7fff80) == 0xffff)
			break;
	}
	c = 0x0;
	while (c < 10000) {
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x4534=0x%x",
		      odm_get_bb_reg(dm, 0x4534, 0x7fff80));
		c++;
		if (odm_get_bb_reg(dm, 0x4534, 0x7fff80) == 0xffff)
			break;
	}
#endif
	odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x2);
	c = 0x0;
	while (c < 10000) {
		c++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x2808=0x%x",
		      odm_get_bb_reg(dm, 0x2808, 0xff));
		if (odm_get_bb_reg(dm, 0x2808, 0xf) == 0x6)
			break;
		odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x0);
		odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x2);
	}
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18b4, BIT(2), 0x0);
		odm_set_bb_reg(dm, 0x18b4, 0xff8, dack->msbk_d[0][0][i]);
		odm_set_bb_reg(dm, 0x18b0, 0xf0000000, i);
		odm_set_bb_reg(dm, 0x18b4, BIT(2), 0x1);
	}
	odm_set_bb_reg(dm, 0x18b4, BIT(2), 0x0);
	odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x2);
	c = 0x0;
	while (c < 10000) {
		c++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x2834=0x%x",
		      odm_get_bb_reg(dm, 0x2834, 0xff));
		if (odm_get_bb_reg(dm,0x2834,0xf) == 0x6)
			break;
		odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x0);
		odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x2);
	}

	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18d0, BIT(2), 0x0);
		odm_set_bb_reg(dm, 0x18d0, 0xff8, dack->msbk_d[0][1][i]);
		odm_set_bb_reg(dm, 0x18cc, 0xf0000000, i);
		odm_set_bb_reg(dm, 0x18d0, BIT(2), 0x1);
	}
	odm_set_bb_reg(dm, 0x18d0, BIT(2), 0x0);
	odm_set_bb_reg(dm, 0x18b8, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x18d4, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x18b4, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x18d0, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x2);
	c = 0x0;
	while (c < 10000) {
		c++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x4508=0x%x",
		      odm_get_bb_reg(dm, 0x4508, 0xff));
		if (odm_get_bb_reg(dm,0x4508,0xf) == 0x6)
			break;
		odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x0);
		odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x2);
	}
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41b4, BIT(2), 0x0);
		odm_set_bb_reg(dm, 0x41b4, 0xff8, dack->msbk_d[1][0][i]);
		odm_set_bb_reg(dm, 0x41b0, 0xf0000000, i);
		odm_set_bb_reg(dm, 0x41b4, BIT(2), 0x1);
	}
	odm_set_bb_reg(dm, 0x41b4, BIT(2), 0x0);
	odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x2);
	c = 0x0;
	while (c < 10000) {
		c++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x4534=0x%x",
		      odm_get_bb_reg(dm, 0x4534, 0xff));
		if (odm_get_bb_reg(dm,0x4534,0xf) == 0x6)
			break;
		odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x0);
		odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x2);
	}
	for (i = 0x0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41d0, BIT(2), 0x0);
		odm_set_bb_reg(dm, 0x41d0, 0xff8, dack->msbk_d[1][1][i]);
		odm_set_bb_reg(dm, 0x41cc, 0xf0000000, i);
		odm_set_bb_reg(dm, 0x41d0, BIT(2), 0x1);
	}
	odm_set_bb_reg(dm, 0x41d0, BIT(2), 0x0);
	odm_set_bb_reg(dm, 0x41b8, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x41d4, BIT(26) | BIT(25), 0x0);
	odm_set_bb_reg(dm, 0x41b4, BIT(0), 0x0);
	odm_set_bb_reg(dm, 0x41d0, BIT(0), 0x0);

	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x1860, MASKDWORD, temp1);
	odm_set_bb_reg(dm, 0x4160, MASKDWORD, temp2);
	odm_set_bb_reg(dm, 0x18b0, BIT(27), 0x1);
	odm_set_bb_reg(dm, 0x18cc, BIT(27), 0x1);
	odm_set_bb_reg(dm, 0x41b0, BIT(27), 0x1);
	odm_set_bb_reg(dm, 0x41cc, BIT(27), 0x1);
	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, temp3);

	halrf_biask_restore_8822c(dm);
}

void halrf_polling_check_8822c(void *dm_void, u32 add, u32 bmask, u32 data)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 c = 0;

	c = 0;
	while (c < 100000) {
		c++;
		if (odm_get_bb_reg(dm, add, bmask) == data)
			break;
	}
	RF_DBG(dm, DBG_RF_DACK, "[DACK]c=%d\n",c);
}

void halrf_dac_cal_8822c(void *dm_void, boolean force)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_dack_info *dack = &dm->dack_info;
	static u32 count = 1;
#if 1
	u32 ic = 0, qc = 0, temp = 0, temp1 = 0, i = 0;
	u32 bp[DACK_REG_8822C];
	u32 bp_reg[DACK_REG_8822C] = {0x180c, 0x1810, 0x410c, 0x4110, 0x1c3c, 0x1c24,
				      0x1d70, 0x9b4, 0x1a00, 0x1a14, 0x1d58,
				      0x1c38, 0x1e24, 0x1e28, 0x1860, 0x4160};
	u32 bp_rf[DACK_RF_8822C][2];
	u32 bp_rfreg[DACK_RF_8822C] = {0x8f};
	u32 i_a = 0x0, q_a = 0x0, i_b = 0x0, q_b = 0x0;
	u32 ic_a = 0x0, qc_a = 0x0, ic_b = 0x0, qc_b = 0x0;
	u32 adc_ic_a = 0x0, adc_qc_a = 0x0, adc_ic_b = 0x0, adc_qc_b = 0x0;
#if 1
	if (dack->dack_en) {
		if (!force) {
			halrf_dack_restore_8822c(dm);
			return;
		}
	} else {
		dack->dack_en = true;
	}
#endif
	count++;
	RF_DBG(dm, DBG_RF_DACK, "[DACK]count = %d", count);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK start!!!!!!!");
	halrf_bp_8822c(dm, bp_reg, bp);
	halrf_bprf_8822c(dm, bp_rfreg, bp_rf);
	/*BB setting*/
	odm_set_bb_reg(dm, 0x1d58, 0xff8, 0x1ff);
	odm_set_bb_reg(dm, 0x1a00, 0x3, 0x2);
	odm_set_bb_reg(dm, 0x1a14, 0x300, 0x3);
	odm_write_4byte(dm, 0x1d70, 0x7e7e7e7e);
	odm_set_bb_reg(dm, 0x180c, 0x3, 0x0);
	odm_set_bb_reg(dm, 0x410c, 0x3, 0x0);
	odm_write_4byte(dm, 0x1b00, 0x00000008);
	odm_write_1byte(dm, 0x1bcc, 0x3f);
	odm_write_4byte(dm, 0x1b00, 0x0000000a);
	odm_write_1byte(dm, 0x1bcc, 0x3f);
	odm_set_bb_reg(dm, 0x1e24, BIT(31), 0x0);
	odm_set_bb_reg(dm, 0x1e28, 0xf, 0x3);
/*path-A*/
	RF_DBG(dm, DBG_RF_DACK, "[DACK]pathA DACK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
/*1.ADCK step1*/
	RF_DBG(dm, DBG_RF_DACK, "[DACK]step1 ADCK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x0);
	odm_write_4byte(dm, 0x1860, 0xf0040ff0);
	odm_write_4byte(dm, 0x180c, 0xdff00220);
	odm_write_4byte(dm, 0x1810, 0x02dd08c4);
	odm_write_4byte(dm, 0x180c, 0x10000260);
	odm_set_rf_reg(dm, RF_PATH_A, 0x0, RFREGOFFSETMASK, 0x10000);
	odm_set_rf_reg(dm, RF_PATH_B, 0x0, RFREGOFFSETMASK, 0x10000);

	i = 0;
	while (i < 10) {
		i++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]ADCK count=%d", i);
		odm_write_4byte(dm, 0x1c3c, 0x00088003);
		odm_write_4byte(dm, 0x1c24, 0x00010002);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]before ADCK i=0x%x, q=0x%x",
		       ic, qc);
		/*compensation value*/
		if (ic != 0x0) {
			ic = 0x400 - ic;
			adc_ic_a = ic;
		}
		if (qc != 0x0) {
			qc = 0x400 - qc;
			adc_qc_a = qc;
		}
		temp = (ic & 0x3ff) | ((qc & 0x3ff) << 10);
		odm_write_4byte(dm, 0x1868, temp);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]ADCK 0x1868 =0x%x\n", temp);
#if 1
		/*check ADC DC offset*/
		odm_write_4byte(dm, 0x1c3c, 0x00088103);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]after ADCK i=0x%x, q=0x%x",
		       ic, qc);
#endif
		if (ic >= 0x200)
			ic = 0x400 - ic;
		if (qc >= 0x200)
			qc = 0x400 - qc;
		if (ic < 5 && qc < 5)
			break;
	}
	/*2.ADCK step2*/
	odm_write_4byte(dm, 0x1c3c, 0x00000003);
	odm_write_4byte(dm, 0x180c, 0x10000260);
	odm_write_4byte(dm, 0x1810, 0x02d508c4);
	/*3.release pull low switch on IQ path*/
	odm_set_rf_reg(dm, RF_PATH_A, 0x8f, BIT(13), 0x1);

	RF_DBG(dm, DBG_RF_DACK, "[DACK]step2 DACK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	i = 0;
	while (i < 10) {
		odm_write_4byte(dm, 0x1868, temp);
		/*DACK step1*/
		i++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK count=%d\n", i);
		odm_write_4byte(dm, 0x180c, 0xdff00220);
		odm_write_4byte(dm, 0x1860, 0xf0040ff0);
		odm_write_4byte(dm, 0x1c38, 0xffffffff);
		odm_write_4byte(dm, 0x1810, 0x02d508c5);
		odm_write_4byte(dm, 0x9b4, 0xdb66db00);
		odm_write_4byte(dm, 0x18b0, 0x0a11fb88);
		odm_write_4byte(dm, 0x18bc, 0x0008ff81);
		odm_write_4byte(dm, 0x18c0, 0x0003d208);
		odm_write_4byte(dm, 0x18cc, 0x0a11fb88);
		odm_write_4byte(dm, 0x18d8, 0x0008ff81);
		odm_write_4byte(dm, 0x18dc, 0x0003d208);

		odm_write_4byte(dm, 0x18b8, 0x60000000);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x18bc, 0x000aff8d);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x18b0, 0x0a11fb89);
		odm_write_4byte(dm, 0x18cc, 0x0a11fb89);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x18b8, 0x62000000);
//		ODM_delay_ms(20);
		odm_write_4byte(dm, 0x18d4, 0x62000000);
		ODM_delay_ms(1);
		halrf_polling_check_8822c(dm, 0x2808, 0x7fff80, 0xffff);
		halrf_polling_check_8822c(dm, 0x2834, 0x7fff80, 0xffff);
		odm_write_4byte(dm, 0x18b8, 0x02000000);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x18bc, 0x0008ff87);
		odm_write_4byte(dm, 0x9b4, 0xdb6db600);

		odm_write_4byte(dm, 0x1810, 0x02d508c5);
		odm_write_4byte(dm, 0x18bc, 0x0008ff87);
		odm_write_4byte(dm, 0x1860, 0xf0000000);
		/*4.DACK step2*/
		odm_set_bb_reg(dm, 0x18bc, 0xf0000000, 0x0);
		odm_set_bb_reg(dm, 0x18c0, 0xf, 0x8);
		odm_set_bb_reg(dm, 0x18d8, 0xf0000000, 0x0);
		odm_set_bb_reg(dm, 0x18dc, 0xf, 0x8);

		odm_write_4byte(dm, 0x1b00, 0x00000008);
		odm_write_1byte(dm, 0x1bcc, 0x03f);
		odm_write_4byte(dm, 0x180c, 0xdff00220);
		odm_write_4byte(dm, 0x1810, 0x02d508c5);
		odm_write_4byte(dm, 0x1c3c, 0x00088103);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]before DACK i =0x%x, q = 0x%x",
		       ic, qc);
		/*compensation value*/
		if (ic != 0x0)
			ic = 0x400 - ic;
		if (qc != 0x0)
			qc = 0x400 - qc;
		if (ic < 0x300) {
			ic = ic * 2 * 6 / 5;
			ic = ic + 0x80;
		} else {
			ic = (0x400 - ic) * 2 * 6 / 5;
			ic = 0x7f - ic;
		}
		if (qc < 0x300) {
			qc = qc * 2 * 6 / 5;
			qc = qc + 0x80;
		} else {
			qc = (0x400 - qc) * 2 * 6 / 5;
			qc = 0x7f - qc;
		}
		RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK ic =0x%x, qc = 0x%x\n",
		       ic, qc);
		ic_a = ic;
		qc_a = qc;
	/*5.DACK step3*/
		odm_write_4byte(dm, 0x180c, 0xdff00220);
		odm_write_4byte(dm, 0x1810, 0x02d508c5);
		odm_write_4byte(dm, 0x9b4, 0xdb66db00);
		odm_write_4byte(dm, 0x18b0, 0x0a11fb88);
		odm_write_4byte(dm, 0x18bc, 0xc008ff81);
		odm_write_4byte(dm, 0x18c0, 0x0003d208);
		odm_set_bb_reg(dm, 0x18bc, 0xf0000000, ic & 0xf);
		odm_set_bb_reg(dm, 0x18c0, 0xf, (ic & 0xf0) >> 4);
		odm_write_4byte(dm, 0x18cc, 0x0a11fb88);
		odm_write_4byte(dm, 0x18d8, 0xe008ff81);
		odm_write_4byte(dm, 0x18dc, 0x0003d208);
		odm_set_bb_reg(dm, 0x18d8, 0xf0000000, qc & 0xf);
		odm_set_bb_reg(dm, 0x18dc, 0xf, (qc & 0xf0) >> 4);
		odm_write_4byte(dm, 0x18b8, 0x60000000);
		ODM_delay_ms(2);
		odm_set_bb_reg(dm, 0x18bc, 0xe, 0x6);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x18b0, 0x0a11fb89);
		odm_write_4byte(dm, 0x18cc, 0x0a11fb89);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x18b8, 0x62000000);
		odm_write_4byte(dm, 0x18d4, 0x62000000);
		ODM_delay_ms(1);
		halrf_polling_check_8822c(dm, 0x2824, 0x07f80000, ic);
		halrf_polling_check_8822c(dm, 0x2850, 0x07f80000, qc);
		odm_write_4byte(dm, 0x18b8, 0x02000000);
		ODM_delay_ms(1);
		odm_set_bb_reg(dm, 0x18bc, 0xe, 0x3);
		odm_write_4byte(dm, 0x9b4, 0xdb6db600);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18bc =0x%x",
		       odm_read_4byte(dm, 0x18bc));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18c0 =0x%x",
		       odm_read_4byte(dm, 0x18c0));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18d8 =0x%x",
		       odm_read_4byte(dm, 0x18d8));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18dc =0x%x",
		       odm_read_4byte(dm, 0x18dc));
#if 1
		/*check DAC DC offset*/
		temp1 = ((adc_ic_a + 0x10) & 0x3ff) |
		       (((adc_qc_a + 0x10) & 0x3ff) << 10);
		odm_write_4byte(dm, 0x1868, temp1);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]shift 0x1868 =0x%x",
		       odm_read_4byte(dm, 0x1868));
		odm_write_4byte(dm, 0x1810, 0x02d508c5);
		odm_write_4byte(dm, 0x1860, 0xf0000000);
		halrf_mode_8822c(dm, &ic, &qc);
		if (ic >= 0x10)
			ic = ic - 0x10;
		else
			ic = 0x400 - (0x10 - ic);

		if (qc >= 0x10)
			qc = qc - 0x10;
		else
			qc = 0x400 - (0x10 - qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]after DACK i=0x%x, q=0x%x",
		       ic, qc);
		i_a = ic;
		q_a = qc;
		if (ic >= 0x200)
			ic = 0x400 - ic;
		if (qc >= 0x200)
			qc = 0x400 - qc;
		if (ic < 5 && qc < 5)
			break;
#endif
	}
	odm_write_4byte(dm, 0x1868, 0x0);
	odm_write_4byte(dm, 0x1810, 0x02d508c4);
	odm_set_bb_reg(dm, 0x18bc, 0x1, 0x0);
	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x1);
#if 1
/*path-B*/
	RF_DBG(dm, DBG_RF_DACK, "[DACK]pathB DACK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
/*1.ADCK step1*/
	RF_DBG(dm, DBG_RF_DACK, "[DACK]step1 ADCK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x0);
	odm_write_4byte(dm, 0x4130, 0x30db8041);
	odm_write_4byte(dm, 0x4160, 0xf0040ff0);
	odm_write_4byte(dm, 0x410c, 0xdff00220);
	odm_write_4byte(dm, 0x4110, 0x02dd08c4);
	odm_write_4byte(dm, 0x410c, 0x10000260);
	odm_set_rf_reg(dm, RF_PATH_A, 0x0, RFREGOFFSETMASK, 0x10000);
	odm_set_rf_reg(dm, RF_PATH_B, 0x0, RFREGOFFSETMASK, 0x10000);
	i = 0;
	while (i < 10) {
		i++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]ADCK count=%d\n", i);
		odm_write_4byte(dm, 0x1c3c, 0x000a8003);
		odm_write_4byte(dm, 0x1c24, 0x00010002);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]before ADCK i=0x%x, q=0x%x",
		       ic, qc);
		/*compensation value*/
		if (ic != 0x0) {
			ic = 0x400 - ic;
			adc_ic_b = ic;
		}
		if (qc != 0x0) {
			qc = 0x400 - qc;
			adc_qc_b = qc;
		}
		temp = (ic & 0x3ff) | ((qc & 0x3ff) << 10);
		odm_write_4byte(dm, 0x4168, temp);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]ADCK 0x4168 =0x%x\n", temp);
#if 1
		/*check ADC DC offset*/
		odm_write_4byte(dm, 0x1c3c, 0x000a8103);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]after ADCK i=0x%x, q=0x%x",
		       ic, qc);
#endif
		if (ic >= 0x200)
			ic = 0x400 - ic;
		if (qc >= 0x200)
			qc = 0x400 - qc;
		if (ic < 5 && qc < 5)
			break;
	}
/*2.ADCK step2*/
	odm_write_4byte(dm, 0x1c3c, 0x00000003);
	odm_write_4byte(dm, 0x410c, 0x10000260);
	odm_write_4byte(dm, 0x4110, 0x02d508c4);

	/*3.release pull low switch on IQ path*/
	odm_set_rf_reg(dm, RF_PATH_B, 0x8f, BIT(13), 0x1);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]step2 DACK!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	i = 0;
	while (i < 10) {
		odm_write_4byte(dm, 0x4168, temp);
/*DACK step1*/
		i++;
		RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK count=%d\n", i);
		odm_write_4byte(dm, 0x410c, 0xdff00220);
		odm_write_4byte(dm, 0x4110, 0x02d508c5);
		odm_write_4byte(dm, 0x9b4, 0xdb66db00);
		odm_write_4byte(dm, 0x41b0, 0x0a11fb88);
		odm_write_4byte(dm, 0x41bc, 0x0008ff81);
		odm_write_4byte(dm, 0x41c0, 0x0003d208);
		odm_write_4byte(dm, 0x41cc, 0x0a11fb88);
		odm_write_4byte(dm, 0x41d8, 0x0008ff81);
		odm_write_4byte(dm, 0x41dc, 0x0003d208);

		odm_write_4byte(dm, 0x41b8, 0x60000000);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x41bc, 0x000aff8d);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x41b0, 0x0a11fb89);
		odm_write_4byte(dm, 0x41cc, 0x0a11fb89);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x41b8, 0x62000000);
		odm_write_4byte(dm, 0x41d4, 0x62000000);
		ODM_delay_ms(1);
		halrf_polling_check_8822c(dm, 0x4508, 0x7fff80, 0xffff);
		halrf_polling_check_8822c(dm, 0x4534, 0x7fff80, 0xffff);
		odm_write_4byte(dm, 0x41b8, 0x02000000);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x41bc, 0x0008ff87);
		odm_write_4byte(dm, 0x9b4, 0xdb6db600);

		odm_write_4byte(dm, 0x4110, 0x02d508c5);
		odm_write_4byte(dm, 0x41bc, 0x0008ff87);
		odm_write_4byte(dm, 0x4160, 0xf0000000);
	/*4.DACK step2*/
		odm_set_bb_reg(dm, 0x41bc, 0xf0000000, 0x0);
		odm_set_bb_reg(dm, 0x41c0, 0xf, 0x8);
		odm_set_bb_reg(dm, 0x41d8, 0xf0000000, 0x0);
		odm_set_bb_reg(dm, 0x41dc, 0xf, 0x8);
		odm_write_4byte(dm, 0x1b00, 0x0000000a);
		odm_write_1byte(dm, 0x1bcc, 0x3f);
		odm_write_4byte(dm, 0x410c, 0xdff00220);
		odm_write_4byte(dm, 0x4110, 0x02d508c5);
		odm_write_4byte(dm, 0x1c3c, 0x000a8103);
		halrf_mode_8822c(dm, &ic, &qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]before DACK i=0x%x, q=0x%x",
		       ic, qc);
		/*compensation value*/
		if (ic != 0x0)
			ic = 0x400 - ic;
		if (qc != 0x0)
			qc = 0x400 - qc;
		if (ic < 0x300) {
			ic = ic  * 2 * 6 / 5;
			ic = ic + 0x80;
		} else {
			ic = (0x400 - ic) * 2 * 6 / 5;
			ic = 0x7f - ic;
		}
		if (qc < 0x300) {
			qc = qc * 2 * 6 / 5;
			qc = qc + 0x80;
		} else {
			qc = (0x400 - qc) * 2 * 6 / 5;
			qc = 0x7f - qc;
		}
		RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK ic=0x%x, qc=0x%x",
		       ic, qc);
		ic_b = ic;
		qc_b = qc;
		/*5.DACK step3*/
		odm_write_4byte(dm, 0x410c, 0xdff00220);
		odm_write_4byte(dm, 0x4110, 0x02d508c5);
		odm_write_4byte(dm, 0x9b4, 0xdb66db00);
		odm_write_4byte(dm, 0x41b0, 0x0a11fb88);
		odm_write_4byte(dm, 0x41bc, 0xc008ff81);
		odm_write_4byte(dm, 0x41c0, 0x0003d208);
		odm_set_bb_reg(dm, 0x41bc, 0xf0000000, ic & 0xf);
		odm_set_bb_reg(dm, 0x41c0, 0xf, (ic & 0xf0) >> 4);
		odm_write_4byte(dm, 0x41cc, 0x0a11fb88);
		odm_write_4byte(dm, 0x41d8, 0xe008ff81);
		odm_write_4byte(dm, 0x41dc, 0x0003d208);
		odm_set_bb_reg(dm, 0x41d8, 0xf0000000, qc & 0xf);
		odm_set_bb_reg(dm, 0x41dc, 0xf, (qc & 0xf0) >> 4);
		odm_write_4byte(dm, 0x41b8, 0x60000000);
		ODM_delay_ms(2);
		odm_set_bb_reg(dm, 0x41bc, 0xe, 0x6);
		ODM_delay_ms(2);
		odm_write_4byte(dm, 0x41b0, 0x0a11fb89);
		odm_write_4byte(dm, 0x41cc, 0x0a11fb89);
		ODM_delay_ms(1);
		odm_write_4byte(dm, 0x41b8, 0x62000000);
		odm_write_4byte(dm, 0x41d4, 0x62000000);
		ODM_delay_ms(1);
		halrf_polling_check_8822c(dm, 0x4524, 0x07f80000, ic);
		halrf_polling_check_8822c(dm, 0x4550, 0x07f80000, qc);
		odm_write_4byte(dm, 0x41b8, 0x02000000);
		ODM_delay_ms(1);
		odm_set_bb_reg(dm, 0x41bc, 0xe, 0x3);
		odm_write_4byte(dm, 0x9b4, 0xdb6db600);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41bc =0x%x",
		       odm_read_4byte(dm, 0x41bc));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41c0 =0x%x",
		       odm_read_4byte(dm, 0x41c0));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41d8 =0x%x",
		       odm_read_4byte(dm, 0x41d8));
		RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41dc =0x%x",
		       odm_read_4byte(dm, 0x41dc));
#if 1
		/*check DAC DC offset*/
		temp1 = ((adc_ic_b + 0x10) & 0x3ff) |
		       (((adc_qc_b + 0x10) & 0x3ff) << 10);
		odm_write_4byte(dm, 0x4168, temp1);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]shift 0x4168 =0x%x\n",
		       odm_read_4byte(dm, 0x4168));
		odm_write_4byte(dm, 0x4110, 0x02d508c5);
		odm_write_4byte(dm, 0x4160, 0xf0000000);
		halrf_mode_8822c(dm, &ic, &qc);
		if (ic >= 0x10)
			ic = ic - 0x10;
		else
			ic = 0x400 - (0x10 - ic);

		if (qc >= 0x10)
			qc = qc - 0x10;
		else
			qc = 0x400 - (0x10 - qc);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]after DACK i=0x%x, q=0x%x",
		       ic, qc);
		i_b = ic;
		q_b = qc;
#endif
		if (ic >= 0x200)
			ic = 0x400 - ic;
		if (qc >= 0x200)
			qc = 0x400 - qc;
		if (ic < 5 && qc < 5)
			break;
	}
#endif
	odm_write_4byte(dm, 0x4168, 0x0);
	odm_write_4byte(dm, 0x4110, 0x02d508c4);
	odm_set_bb_reg(dm, 0x41bc, 0x1, 0x0);
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x1);
	odm_write_4byte(dm, 0x1b00, 0x00000008);
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x1);
	odm_write_1byte(dm, 0x1bcc, 0x0);
	odm_write_4byte(dm, 0x1b00, 0x0000000a);
	odm_write_1byte(dm, 0x1bcc, 0x0);
	i_b = ic;
	q_b = qc;
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH A:ic=0x%x, qc=0x%x", ic_a, qc_a);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH B:ic=0x%x, qc=0x%x", ic_b, qc_b);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH A:i=0x%x, q=0x%x", i_a, q_a);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH B:i=0x%x, q=0x%x", i_b, q_b);
	halrf_reload_bp_8822c(dm, bp_reg, bp);
	halrf_reload_bprf_8822c(dm, bp_rfreg, bp_rf);
	halrf_dack_backup_8822c(dm);
	RF_DBG(dm, DBG_RF_DACK, "[DACK]DACK end!!!!!!!\n");
#endif
}

void halrf_dack_dbg_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 i;
	u32 temp1, temp2, temp3;

	temp1 = odm_get_bb_reg(dm, 0x1860, MASKDWORD);
	temp2 = odm_get_bb_reg(dm, 0x4160, MASKDWORD);
	temp3 = odm_get_bb_reg(dm, 0x9b4, MASKDWORD);

	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, 0xdb66db00);

	RF_DBG(dm, DBG_RF_DACK, "[DACK]MSBK result\n");
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH A\n");
	//pathA
	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x1860, 0xfc000000, 0x3c);
	//i
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18b0, 0xf0000000, i);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]msbk_d[0][0][%d]=0x%x\n", i,
		       odm_get_bb_reg(dm,0x2810,0x7fc0000));
	}
	//q
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x18cc, 0xf0000000, i);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]msbk_d[0][1][%d]=0x%x\n", i,
		       odm_get_bb_reg(dm,0x283c,0x7fc0000));
	}
	//pathB
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH A\n");
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x0);
	odm_set_bb_reg(dm, 0x4160, 0xfc000000, 0x3c);
	//i
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41b0, 0xf0000000, i);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]msbk_d[1][0][%d]=0x%x\n", i,
		       odm_get_bb_reg(dm,0x4510,0x7fc0000));
	}
	//q
	for (i = 0; i < 0xf; i++) {
		odm_set_bb_reg(dm, 0x41cc, 0xf0000000, i);
		RF_DBG(dm, DBG_RF_DACK, "[DACK]msbk_d[1][1][%d]=0x%x\n", i,
		       odm_get_bb_reg(dm,0x453c,0x7fc0000));
	}

	RF_DBG(dm, DBG_RF_DACK, "[DACK]DCK result\n");
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH A\n");
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18bc[31:28]=0x%x\n",
		       odm_get_bb_reg(dm,0x18bc,0xf0000000));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18c0[3:0]=0x%x\n",
		       odm_get_bb_reg(dm,0x18c0,0xf));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18d8[31:28]=0x%x\n",
		       odm_get_bb_reg(dm,0x18d8,0xf0000000));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x18dc[3:0]=0x%x\n",
		       odm_get_bb_reg(dm,0x18dc,0xf));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]PATH B\n");
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41bc[31:28]=0x%x\n",
		       odm_get_bb_reg(dm,0x41bc,0xf0000000));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41c0[3:0]=0x%x\n",
		       odm_get_bb_reg(dm,0x41c0,0xf));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41d8[31:28]=0x%x\n",
		       odm_get_bb_reg(dm,0x41d8,0xf0000000));
	RF_DBG(dm, DBG_RF_DACK, "[DACK]0x41dc[3:0]=0x%x\n",
		       odm_get_bb_reg(dm,0x41dc,0xf));


	//restore to normal
	odm_set_bb_reg(dm, 0x1830, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x4130, BIT(30), 0x1);
	odm_set_bb_reg(dm, 0x1860, MASKDWORD, temp1);
	odm_set_bb_reg(dm, 0x4160, MASKDWORD, temp2);
	odm_set_bb_reg(dm, 0x9b4, MASKDWORD, temp3);
}

void halrf_rxdck_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 temp1, temp2;

	temp1 = odm_get_bb_reg(dm, 0x180c, MASKDWORD);
	temp2 = odm_get_bb_reg(dm, 0x410c, MASKDWORD);
	odm_set_bb_reg(dm, 0x180c, 0x3, 0x0);
	odm_set_bb_reg(dm, 0x410c, 0x3, 0x0);
	odm_set_rf_reg(dm, RF_PATH_A, 0x0, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_B, 0x0, RFREGOFFSETMASK, 0x30000);

	odm_set_rf_reg(dm, RF_PATH_A, 0x92, RFREGOFFSETMASK, 0x84800);
	odm_set_rf_reg(dm, RF_PATH_A, 0x92, RFREGOFFSETMASK, 0x84801);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_A, 0x92, RFREGOFFSETMASK, 0x84800);

	odm_set_rf_reg(dm, RF_PATH_B, 0x92, RFREGOFFSETMASK, 0x84800);
	odm_set_rf_reg(dm, RF_PATH_B, 0x92, RFREGOFFSETMASK, 0x84801);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_B, 0x92, RFREGOFFSETMASK, 0x84800);

	odm_set_bb_reg(dm, 0x180c, MASKDWORD, temp1);
	odm_set_bb_reg(dm, 0x410c, MASKDWORD, temp2);
	odm_set_rf_reg(dm, RF_PATH_A, 0x0, RFREGOFFSETMASK, 0x3ffff);
	odm_set_rf_reg(dm, RF_PATH_B, 0x0, RFREGOFFSETMASK, 0x3ffff);
}
void _phy_x2_calibrate_8822c(struct dm_struct *dm)
{
	RF_DBG(dm, DBG_RF_IQK, "[X2K]X2K start!!!!!!!\n");
	/*X2K*/
	//Path A
	odm_set_rf_reg(dm, RF_PATH_A, 0x18, RFREGOFFSETMASK, 0x13108);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_A, 0xb8, RFREGOFFSETMASK, 0xC0440);
	odm_set_rf_reg(dm, RF_PATH_A, 0xba, RFREGOFFSETMASK, 0xE840D);
	ODM_delay_ms(1);
	odm_set_rf_reg(dm, RF_PATH_A, 0x18, RFREGOFFSETMASK, 0x13124);
	//Path B
	// SYN is in the path A
	RF_DBG(dm, DBG_RF_IQK, "[X2K]X2K end!!!!!!!\n");
}

void phy_x2_check_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 X2K_BUSY;

	RF_DBG(dm, DBG_RF_IQK, "[X2K]X2K check start!!!!!!!\n");
	/*X2K*/
	//Path A
	ODM_delay_ms(1);
	X2K_BUSY = (u8) odm_get_rf_reg(dm, RF_PATH_A, 0xb8, BIT(15));
	if (X2K_BUSY == 1) {
		odm_set_rf_reg(dm, RF_PATH_A, 0xb8, RFREGOFFSETMASK, 0xC4440);
		odm_set_rf_reg(dm, RF_PATH_A, 0xba, RFREGOFFSETMASK, 0x6840D);
		odm_set_rf_reg(dm, RF_PATH_A, 0xb8, RFREGOFFSETMASK, 0x80440);
		ODM_delay_ms(1);
	}
	//Path B
	// SYN is in the path A
	RF_DBG(dm, DBG_RF_IQK, "[X2K]X2K check end!!!!!!!\n");
}

/*LCK VERSION:0x1*/
void phy_lc_calibrate_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
#if 1
	_phy_aac_calibrate_8822c(dm);
	_phy_rt_calibrate_8822c(dm);
#endif
}

void configure_txpower_track_8822c(struct txpwrtrack_cfg *config)
{
	config->swing_table_size_cck = TXSCALE_TABLE_SIZE;
	config->swing_table_size_ofdm = TXSCALE_TABLE_SIZE;
	config->threshold_iqk = IQK_THRESHOLD;
	config->threshold_dpk = DPK_THRESHOLD;
	config->average_thermal_num = AVG_THERMAL_NUM_8822C;
	config->rf_path_count = MAX_PATH_NUM_8822C;
	config->thermal_reg_addr = RF_T_METER_8822C;

	config->odm_tx_pwr_track_set_pwr = odm_tx_pwr_track_set_pwr8822c;
	config->do_iqk = do_iqk_8822c;
	config->phy_lc_calibrate = halrf_lck_trigger;
	config->do_tssi_dck = halrf_tssi_dck;
	config->get_delta_swing_table = get_delta_swing_table_8822c;
}

#if ((DM_ODM_SUPPORT_TYPE & ODM_AP) || (DM_ODM_SUPPORT_TYPE == ODM_CE))
void phy_set_rf_path_switch_8822c(struct dm_struct *dm, boolean is_main)
#else
void phy_set_rf_path_switch_8822c(void *adapter, boolean is_main)
#endif
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(((PADAPTER)adapter));
	struct dm_struct *dm = &hal_data->DM_OutSrc;
#endif
#endif
	/*BY mida Request */
	if (is_main) {
		/*WiFi*/
		odm_set_bb_reg(dm, R_0x70, BIT(26), 0x1);
	} else {
		/*BT*/
		odm_set_bb_reg(dm, R_0x70, BIT(26), 0x0);
	}
}

#if ((DM_ODM_SUPPORT_TYPE & ODM_AP) || (DM_ODM_SUPPORT_TYPE == ODM_CE))
boolean _phy_query_rf_path_switch_8822c(struct dm_struct *dm)
#else
boolean _phy_query_rf_path_switch_8822c(void *adapter)
#endif
{
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(((PADAPTER)adapter));
	struct dm_struct *dm = &hal_data->DM_OutSrc;
#endif
#endif
	if (odm_get_bb_reg(dm, R_0x70, BIT(26)) == 0x1)
		return true;	/*WiFi*/
	else
		return false;
}

#if ((DM_ODM_SUPPORT_TYPE & ODM_AP) || (DM_ODM_SUPPORT_TYPE == ODM_CE))
boolean phy_query_rf_path_switch_8822c(struct dm_struct *dm)
#else
boolean phy_query_rf_path_switch_8822c(void *adapter)
#endif
{
#if DISABLE_BB_RF
	return true;
#endif
#if ((DM_ODM_SUPPORT_TYPE & ODM_AP) || (DM_ODM_SUPPORT_TYPE == ODM_CE))
	return _phy_query_rf_path_switch_8822c(dm);
#else
	return _phy_query_rf_path_switch_8822c(adapter);
#endif
}

void halrf_rxbb_dc_cal_8822c(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	u8 path, i;

	for (path = 0; path < 2; path++) {
		odm_set_rf_reg(dm, (enum rf_path)path, 0x92, RFREG_MASK, 0x84800);
		ODM_delay_us(5);
		odm_set_rf_reg(dm, (enum rf_path)path, 0x92, RFREG_MASK, 0x84801);
		for (i = 0; i < 30; i++) /*delay 600us*/
			ODM_delay_us(20);
		odm_set_rf_reg(dm, (enum rf_path)path, 0x92, RFREG_MASK, 0x84800);
	}
}

void halrf_rfk_handshake_8822c(void *dm_void, boolean is_before_k)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	u8 u1b_tmp, h2c_parameter;
	u16 count;

	rf->is_rfk_h2c_timeout = false;

	if (is_before_k) {

		RF_DBG(dm, DBG_RF_IQK | DBG_RF_DPK | DBG_RF_TX_PWR_TRACK,
		       "[RFK] WiFi / BT RFK handshake start!!\n");

		if (!rf->is_bt_iqk_timeout) {
			/* Check if BT request to do IQK (0xaa[6]) or is doing IQK (0xaa[5]), 600ms timeout*/
			count = 0;
			u1b_tmp = (u8)odm_get_mac_reg(dm, 0xa8, BIT(22) | BIT(21));
			while (u1b_tmp != 0 && count < 30000) {
				ODM_delay_us(20);
				u1b_tmp = (u8)odm_get_mac_reg(dm, 0xa8, BIT(22) | BIT(21));
				count++;
			}

			if (count >= 30000) {
				RF_DBG(dm, DBG_RF_IQK | DBG_RF_DPK | DBG_RF_TX_PWR_TRACK,
				       "[RFK] Wait BT IQK finish timeout!!\n");

				rf->is_bt_iqk_timeout = true;
			}
		}

		/* Send RFK start H2C cmd*/
		h2c_parameter = 1;
		odm_fill_h2c_cmd(dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);

		/* Check 0x49c[0] or 100ms timeout*/
		count = 0;
		u1b_tmp = (u8)odm_get_mac_reg(dm, 0x49c, BIT(0));
		while (u1b_tmp != 0x1 && count < 5000) {
			ODM_delay_us(20);
			u1b_tmp = (u8)odm_get_mac_reg(dm, 0x49c, BIT(0));
			count++;
		}

		if (count >= 5000) {
			RF_DBG(dm, DBG_RF_IQK | DBG_RF_DPK | DBG_RF_TX_PWR_TRACK,
			       "[RFK] Send WiFi RFK start H2C cmd FAIL!!\n");

			rf->is_rfk_h2c_timeout = true;
		}

	} else {
		/* Send RFK finish H2C cmd*/
		h2c_parameter = 0;
		odm_fill_h2c_cmd(dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);
		/* Check 0x49c[0] or 100ms timeout*/
		count = 0;
		u1b_tmp = (u8)odm_get_mac_reg(dm, 0x49c, BIT(0));
		while (u1b_tmp != 0 && count < 5000) {
			ODM_delay_us(20);
			u1b_tmp = (u8)odm_get_mac_reg(dm, 0x49c, BIT(0));
			count++;
		}

		if (count >= 5000) {
			RF_DBG(dm, DBG_RF_IQK | DBG_RF_DPK | DBG_RF_TX_PWR_TRACK,
			       "[RFK] Send WiFi RFK finish H2C cmd FAIL!!\n");

			rf->is_rfk_h2c_timeout = true;
		}

		RF_DBG(dm, DBG_RF_IQK | DBG_RF_DPK | DBG_RF_TX_PWR_TRACK,
		       "[RFK] WiFi / BT RFK handshake finish!!\n");
	}
}
void halrf_rfk_power_save_8822c(
	void *dm_void,
	boolean is_power_save)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct dm_iqk_info *iqk_info = &dm->IQK_info;
	u8 path  = 0;

	for(path = 0; path < SS_8822C; path++) {
		odm_set_bb_reg(dm, R_0x1b00, BIT(2)| BIT(1), path);
		if (is_power_save)
			odm_set_bb_reg(dm, R_0x1b08, BIT(7), 0x0);
		else
			odm_set_bb_reg(dm, R_0x1b08, BIT(7), 0x1);
		}
}

u8 halrf_get_thermal_8822c(
	void *dm_void,
	u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	odm_set_rf_reg(dm, (enum rf_path)path, RF_0x42, BIT(19), 0x1);
	odm_set_rf_reg(dm, (enum rf_path)path, RF_0x42, BIT(19), 0x0);
	odm_set_rf_reg(dm, (enum rf_path)path, RF_0x42, BIT(19), 0x1);
	ODM_delay_us(15);

	return (u8)odm_get_rf_reg(dm, (enum rf_path)path, RF_0x42, 0x0007e);
}

#endif /*(RTL8822C_SUPPORT == 0)*/
