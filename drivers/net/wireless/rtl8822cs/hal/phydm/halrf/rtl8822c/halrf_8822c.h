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

#ifndef __HALRF_8822C_H__
#define __HALRF_8822C_H__

#define AVG_THERMAL_NUM_8822C 4
#define RF_T_METER_8822C 0x42
#define DACK_REG_8822C 16
#define DACK_RF_8822C 1
void halrf_rf_lna_setting_8822c(
	struct dm_struct *p_dm_void,
	enum halrf_lna_set type);

void configure_txpower_track_8822c(
	struct txpwrtrack_cfg *config);

void odm_tx_pwr_track_set_pwr8822c(
	void *dm_void,
	enum pwrtrack_method method,
	u8 rf_path,
	u8 channel_mapped_index);

void get_delta_swing_table_8198f(
	void *dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b,
	u8 **temperature_up_cck_a,
	u8 **temperature_down_cck_a,
	u8 **temperature_up_cck_b,
	u8 **temperature_down_cck_b
	);

void get_delta_swing_table_8822c_ex(
	void *p_dm_void,
	u8 **temperature_up_c,
	u8 **temperature_down_c,
	u8 **temperature_up_d,
	u8 **temperature_down_d,
	u8 **temperature_up_cck_c,
	u8 **temperature_down_cck_c,
	u8 **temperature_up_cck_d,
	u8 **temperature_down_cck_d
	);

void halrf_dac_cal_all_8822c(void *dm_void);

void halrf_dac_cal_8822c(void *dm_void, boolean force);

void halrf_dack_dbg_8822c(void *dm_void);

void phy_lc_calibrate_8822c(
	void *dm_void);

void halrf_rxdck_8822c(void *dm_void);

void phy_x2_check_8822c(void *dm_void);

void phy_set_rf_path_switch_8822c(
#if ((DM_ODM_SUPPORT_TYPE & ODM_AP) || (DM_ODM_SUPPORT_TYPE == ODM_CE))
	struct dm_struct *dm,
#else
	void *adapter,
#endif
	boolean is_main);

void halrf_rxbb_dc_cal_8822c(void *dm_void);

void halrf_rfk_handshake_8822c(void *dm_void, boolean is_before_k);

void halrf_dack_restore_8822c(void *dm_void);

void halrf_rfk_power_save_8822c(void *dm_void, boolean is_power_save);

u8 halrf_get_thermal_8822c(void *dm_void, u8 path);

#endif /*__HALRF_8822C_H__*/
