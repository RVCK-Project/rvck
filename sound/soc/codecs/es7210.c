/*
 * ALSA SoC ES7210 adc driver
 *
 * Copyright:   (C) 2018 Everest Semiconductor Co Ltd.,
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Notes:
 *  ES7210 is a 4-ch ADC of Everest
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/tlv.h>
#include <sound/initval.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include "es7210.h"


#define DOUBLESPEED 0

#define ENABLE      1
#define DISABLE     0

#define MIC_CHN_16  16
#define MIC_CHN_14  14
#define MIC_CHN_12  12
#define MIC_CHN_10  10
#define MIC_CHN_8   8
#define MIC_CHN_6   6
#define MIC_CHN_4   4
#define MIC_CHN_2   2

#define ES7210_TDM_ENABLE   ENABLE
//#define ES7210_CHANNELS_MAX MIC_CHN_8

static int ADC_DEV_MAXNUM = 1;
static int ES7210_CHANNELS_MAX = 1;
#if 0
#if ES7210_CHANNELS_MAX == MIC_CHN_2
        #define ADC_DEV_MAXNUM  1
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_4
        #define ADC_DEV_MAXNUM  1
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_6
        #define ADC_DEV_MAXNUM  2
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_8
        #define ADC_DEV_MAXNUM  2
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_10
        #define ADC_DEV_MAXNUM  3
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_12
        #define ADC_DEV_MAXNUM  3
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_14
        #define ADC_DEV_MAXNUM  4
#endif
#if ES7210_CHANNELS_MAX == MIC_CHN_16
        #define ADC_DEV_MAXNUM  4
#endif
#endif

#define ES7210_TDM_1LRCK_DSPA                 0
#define ES7210_TDM_1LRCK_DSPB                 1
#define ES7210_TDM_1LRCK_I2S                  2
#define ES7210_TDM_1LRCK_LJ                   3
#define ES7210_TDM_NLRCK_DSPA                 4
#define ES7210_TDM_NLRCK_DSPB                 5
#define ES7210_TDM_NLRCK_I2S                  6
#define ES7210_TDM_NLRCK_LJ                   7
#define ES7210_NORMAL_I2S                     8

#define ES7210_WORK_MODE    ES7210_TDM_1LRCK_DSPB


#define ES7210_I2C_BUS_NUM      1
#define ES7210_CODEC_RW_TEST_EN     0
#define ES7210_IDLE_RESET_EN        1   //reset ES7210 when in idle time

#define ES7210_MIC_GAIN 0x1c  // need check hw design and channel
#define ES7210_AEC_GAIN 0x13  // need check hw design and channel

/*  to set internal mclk and adclrclk ratio   */

#define RATIO_768  0xC3
#define RATIO_256  0xC1
#define RATIO_128  0x01
#define RATIO_64  0x41 /* mclk from bclk pin */

#define ES7210_MCLK_LRCK_RATIO   RATIO_256 // mclk=sclk. mclk/lrck = sclk / lrck = 16bit*4slot=RATIO_64.    32bit*4slot=16bit*8slot=RATIO_128  32bit*8slot=RATIO_256

struct i2c_client *i2c_clt1[4] = {0};

struct es7210_priv *resume_es7210 = NULL;

struct snd_soc_component *tron_codec1[4];

int es7210_init_reg = 0;
int es7210_hw_param_reg = 0;
static int es7210_codec_num = 0;

static const struct regmap_config es7210_regmap_config = {
        .reg_bits = 8,  //Number of bits in a register address
        .val_bits = 8,  //Number of bits in a register value
};
/*
* ES7210 register cache
*/
static const u8 es7210_reg[] = {
        0x32, 0x40, 0x02, 0x04, 0x01, 0x00, 0x00, 0x20, /* 0 - 7 */
        0x10, 0x40, 0x40, 0x00, 0x00, 0x09, 0x00, 0x00, /* 8 - F */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 10 - 17 */
        0xf7, 0xf7, 0x00, 0xbf, 0xbf, 0xbf, 0xbf, 0x00, /* 18 - 1f */
        0x26, 0x26, 0x06, 0x26, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2f */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 30 - 37 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x10, 0x00, /* 38 - 3f */
        0x80, 0x71, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, /* 40 - 47 */
        0x00, 0x00, 0x00, 0xff, 0xff,           /* 48 - 4c */
};

struct es7210_reg_config {
        unsigned char reg_addr;
        unsigned char reg_v;
};
static const struct es7210_reg_config es7210_tdm_reg_common_cfg1[] = {
        { 0x00, 0xFF },
        { 0x00, 0x32 },
        { 0x09, 0x30 },
        { 0x0A, 0x30 },
        { 0x23, 0x2a },
        { 0x22, 0x0a },
        { 0x21, 0x2a },
        { 0x20, 0x0a },
};
static const struct es7210_reg_config es7210_tdm_reg_fmt_cfg[] = {
        { 0x11, 0x63 },
        { 0x12, 0x01 },
};
static const struct es7210_reg_config es7210_tdm_reg_common_cfg2[] = {
        { 0x40, 0xC3 },
        { 0x41, 0x70 },
        { 0x42, 0x70 },
        { 0x43, 0x1E },
        { 0x44, 0x1E },
        { 0x45, 0x1E },
        { 0x46, 0x1E },
        { 0x47, 0x08 },
        { 0x48, 0x08 },
        { 0x49, 0x08 },
        { 0x4A, 0x08 },
        { 0x07, 0x20 },
};
static const struct es7210_reg_config es7210_tdm_reg_mclk_cfg[] = {
        { 0x02, 0xC1 },
};
static const struct es7210_reg_config es7210_tdm_reg_common_cfg3[] = {
        { 0x06, 0x00 },
        { 0x4B, 0x0F },
        { 0x4C, 0x0F },
        { 0x00, 0x71 },
        { 0x00, 0x41 },
};
static int es7210_read(u8 reg, u8 *rt_value, struct i2c_client *client)
{
        int ret;
        u8 read_cmd[3] = {0};
        u8 cmd_len = 0;

        read_cmd[0] = reg;
        cmd_len = 1;

        if (client->adapter == NULL)
                pr_err("es7210_read client->adapter==NULL\n");

        ret = i2c_master_send(client, read_cmd, cmd_len);
        if (ret != cmd_len) {
                return -1;
        }

        ret = i2c_master_recv(client, rt_value, 1);
        if (ret != 1) {
                return -1;
        }

        return 0;
}
static int es7210_write(u8 reg, unsigned char value, struct i2c_client *client)
{
        int ret;
        u8 write_cmd[2] = {0};

        write_cmd[0] = reg;
        write_cmd[1] = value;

        ret = i2c_master_send(client, write_cmd, 2);
        if (ret != 2) {
                return -1;
        }

        return 0;
}
static int es7210_update_bits(u8 reg, u8 mask, u8 value, struct i2c_client *client)
{
        u8 val_old, val_new;

        es7210_read(reg, &val_old, client);
        val_new = (val_old & ~mask) | (value & mask);
        if (val_new != val_old) {
                es7210_write(reg, val_new, client);
        }

        return 0;
}
/*
static int es7210_multi_chips_read(u8 reg, unsigned char *rt_value)
{
    u8 i;

    for(i=0; i< ADC_DEV_MAXNUM; i++){
        es7210_read(reg, rt_value++, i2c_clt1[i]);
    }

    return 0;
}
*/
static int es7210_multi_chips_write(u8 reg, unsigned char value)
{
        u8 i;

        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                es7210_write(reg, value, i2c_clt1[i]);
        }

        return 0;
}

static int es7210_multi_chips_update_bits(u8 reg, u8 mask, u8 value)
{
        u8 i;

        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                es7210_update_bits(reg, mask, value, i2c_clt1[i]);
        }

        return 0;
}

/*
* Note that this should be called from init rather than from hw_params.
*/
static int es7210_set_dai_sysclk(struct snd_soc_dai *codec_dai,
                                 int clk_id, unsigned int freq, int dir)
{
        return 0;
}

static int es7210_set_dai_fmt(struct snd_soc_dai *codec_dai,
                              unsigned int fmt)
{
        return 0;
}
static void es7210_tdm_init_ratio(struct es7210_priv *priv)
{
        int cnt, channel, width;

        channel = ES7210_CHANNELS_MAX;

        switch(priv->tdm_mode) {
                case ES7210_TDM_1LRCK_DSPB:
                        switch(priv->pcm_format) {
                                case SNDRV_PCM_FORMAT_S16_LE:
                                        width = 16;
                                        break;
                                case SNDRV_PCM_FORMAT_S32_LE:
                                        width = 32;
                        }
                        break;
                case ES7210_NORMAL_I2S:
                        width = 32;
                        if (channel > 2) // i2s 8ch has 2 channels in actual.
                                channel = 2;
                        break;
                default:
                        break;
        }

        priv->sclk_lrck_ratio = channel * width ;

        switch(priv->sclk_lrck_ratio * priv->mclk_sclk_ratio) {
                case 64:
                        priv->mclk_lrck_ratio = RATIO_64;
                        break;
                case 128:
                        priv->mclk_lrck_ratio = RATIO_128;
                        break;
                case 256:
                        priv->mclk_lrck_ratio = RATIO_256;
                        break;
                case 768:
                        priv->mclk_lrck_ratio = RATIO_768;
                        break;
                default:
                        pr_err("%s Unable to calculate proper mclk_lrck_ratio with sclk_lrck_ratio=%d!\n", __func__, priv->sclk_lrck_ratio);
                        break;
        }

        switch (priv->tdm_mode) {
        case ES7210_TDM_1LRCK_DSPA:
        case ES7210_TDM_1LRCK_DSPB:
        case ES7210_TDM_1LRCK_I2S:
        case ES7210_TDM_1LRCK_LJ:
        case ES7210_NORMAL_I2S:
                /*
                * to set internal mclk
                * here, we assume that cpu/soc always provides
                * 256FS i2s clock
                * to es7210.
                * dll bypassed, use clock doubler to get double
                * frequency for
                * internal modem which need
                * 512FS clock. the clk divider ratio is 1.
                * user must modify the setting of register0x02
                * according to FS
                * ratio provided by CPU/SOC.
                */

                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_MCLK_CTL_REG02,
                                     priv->mclk_lrck_ratio, i2c_clt1[cnt]);
                }
                // es7210_multi_chips_write(ES7210_MCLK_CTL_REG02,
                // 0xc1);
                break;
        case ES7210_TDM_NLRCK_DSPA:
        case ES7210_TDM_NLRCK_DSPB:
        case ES7210_TDM_NLRCK_I2S:
        case ES7210_TDM_NLRCK_LJ:
                /*
                * Here to set TDM format for DSP-A with
                * multiple LRCK TDM mode
                */
                channel = ES7210_CHANNELS_MAX;
                /*
                * Set the microphone numbers in array
                */
                switch (channel) {
                case 2:
                        /* ES7210_CHANNELS_MAX=2 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x10);
                        break;
                case 4:
                        /* ES7210_CHANNELS_MAX=4 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x20);
                        break;
                case 6:
                        /* ES7210_CHANNELS_MAX=6 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x30);
                        break;
                case 8:
                        /* ES7210_CHANNELS_MAX=8 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x40);
                        break;
                case 10:
                        /* ES7210_CHANNELS_MAX=10 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x50);
                        break;
                case 12:
                        /* ES7210_CHANNELS_MAX=12 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x60);
                        break;
                case 14:
                        /* ES7210_CHANNELS_MAX=14 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x70);
                        break;
                case 16:
                        /* ES7210_CHANNELS_MAX=16 */
                        es7210_multi_chips_write(
                                ES7210_MODE_CFG_REG08, 0x80);
                        break;
                default:
                        break;
                }
                /*
                * to set internal mclk
                * here, we assume that cpu/soc always provides
                * 256FS i2s clock
                * to es7210 and there is four
                    * es7210 devices in tdm link. so the
                * internal FS in es7210
                * is only FS/4;
                * dll bypassed, clock doubler bypassed. the clk
                * divider ratio is
                * 2. so the clock of internal
                    * modem equals to (256FS / (FS/4) / 2) * FS
                * = 512FS
                * user must modify the setting of register0x02
                * according to FS
                * ratio provided by CPU/SOC.
                */

                es7210_multi_chips_write(ES7210_MCLK_CTL_REG02,
                                         priv->mclk_lrck_ratio);// NFS MODE:RATIO_768 ,12.288M/48K(16K 6 CH),12.288M/64K(16K 8 CH)
                break;
        default:
                /*
                * to set internal mclk for normal mode
                * here, we assume that cpu/soc always provides
                * 256FS i2s clock
                * to es7210.
                * dll bypassed, use clock doubler to get double
                * frequency for
                * internal modem which need
                * 512FS clock. the clk divider ratio is 1.
                * user must modify the setting of register0x02
                * according to FS
                * ratio provided by CPU/SOC.
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_MCLK_CTL_REG02,
                                     priv->mclk_lrck_ratio, i2c_clt1[cnt]);
                }

                break;
        }
        return;
}

/*
* to initialize es7210 for tdm mode
*/
static void es7210_tdm_init_codec(u8 mode)
{
        int cnt, i;

        for (cnt = 0;
             cnt < sizeof(es7210_tdm_reg_common_cfg1) /
             sizeof(es7210_tdm_reg_common_cfg1[0]);
             cnt++) {
                es7210_multi_chips_write(
                        es7210_tdm_reg_common_cfg1[cnt].reg_addr,
                        es7210_tdm_reg_common_cfg1[cnt].reg_v);
        }
        if(DOUBLESPEED)
                es7210_multi_chips_update_bits(ES7210_MODE_CFG_REG08, 0x02, 0x02);

        switch (mode) {
        case ES7210_TDM_1LRCK_DSPA:
                /*
                * Here to set TDM format for DSP-A mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x63, i2c_clt1[cnt]);
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x01, i2c_clt1[cnt]);
                }
                break;
        case ES7210_TDM_1LRCK_DSPB:
                /*
                * Here to set TDM format for DSP-B mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x73, i2c_clt1[cnt]);
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x01, i2c_clt1[cnt]);
                }
                break;
        case ES7210_TDM_1LRCK_I2S:
                /*
                * Here to set TDM format for I2S mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x60, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x02, i2c_clt1[cnt]);
                }
                break;
        case ES7210_TDM_1LRCK_LJ:
                /*
                * Here to set TDM format for Left Justified mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x61, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x02, i2c_clt1[cnt]);
                }
                break;
        case ES7210_TDM_NLRCK_DSPA:
                /*
                * set format, dsp-a with multiple LRCK tdm mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x63, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        if (cnt == 0) {
                                /*
                                * set tdm flag in the interface
                                * chip
                                */
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x07,
                                        i2c_clt1[cnt]);
                        } else {
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x03,
                                        i2c_clt1[cnt]);
                        }
                }
                break;
        case ES7210_TDM_NLRCK_DSPB:
                /*
                * set format, dsp-b with multiple LRCK tdm mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x73, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        if (cnt == 0) {
                                /*
                                * set tdm flag in the interface
                                * chip
                                */
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x07,
                                        i2c_clt1[cnt]);
                        } else {
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x03,
                                        i2c_clt1[cnt]);
                        }
                }

                break;
        case ES7210_TDM_NLRCK_I2S:
                /*
                * set format, I2S with multiple LRCK tdm mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x60, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        if (cnt == 0) {
                                /*
                                * set tdm flag in the interface
                                * chip
                                */
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x07,
                                        i2c_clt1[cnt]);
                        } else {
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x03,
                                        i2c_clt1[cnt]);
                        }
                }

                break;
        case ES7210_TDM_NLRCK_LJ:
                /*
                * set format, left justified with multiple LRCK
                * tdm mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x61, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        if (cnt == 0) {
                                /*
                                * set tdm flag in the interface
                                * chip
                                */
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x07,
                                        i2c_clt1[cnt]);
                        } else {
                                es7210_write(
                                        ES7210_SDP_CFG2_REG12, 0x03,
                                        i2c_clt1[cnt]);
                        }
                }

                break;
        case ES7210_NORMAL_I2S:
                 /*
                * Here to set Normal  i2s mode format for SD1/SD2 output
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x60, i2c_clt1[cnt]);
                }
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x00, i2c_clt1[cnt]);
                }

                break;

        default:
                /*
                * here to disable tdm and set i2s-16bit for
                * normal mode
                */
                for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                        es7210_write(ES7210_SDP_CFG1_REG11,
                                     0x60, i2c_clt1[cnt]);
                        es7210_write(ES7210_SDP_CFG2_REG12,
                                     0x00, i2c_clt1[cnt]);
                }
                break;
        }
        for (cnt = 0;
             cnt < sizeof(es7210_tdm_reg_common_cfg2) /
             sizeof(es7210_tdm_reg_common_cfg2[0]);
             cnt++) {
                es7210_multi_chips_write(
                        es7210_tdm_reg_common_cfg2[cnt].reg_addr,
                        es7210_tdm_reg_common_cfg2[cnt].reg_v);
        }

        for (cnt = 0;
             cnt < sizeof(es7210_tdm_reg_common_cfg3) /
             sizeof(es7210_tdm_reg_common_cfg3[0]);
             cnt++) {
                es7210_multi_chips_write(
                        es7210_tdm_reg_common_cfg3[cnt].reg_addr,
                        es7210_tdm_reg_common_cfg3[cnt].reg_v);
        }
        /*
        * Mute All ADC
        */
        for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                es7210_write(ES7210_ADC34_MUTE_REG14, 0x03,
                             i2c_clt1[cnt]);
                es7210_write(ES7210_ADC12_MUTE_REG15, 0x03,
                             i2c_clt1[cnt]);
        }

        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                if (i == 0) {
                        /* set first es7210 PGA GAIN */
                        es7210_write(ES7210_MIC1_GAIN_REG43, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC2_GAIN_REG44, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC3_GAIN_REG45, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC4_GAIN_REG46, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                }
                if (i == 1) {
                        /* set second es7210 PGA GAIN */
                        es7210_write(ES7210_MIC1_GAIN_REG43, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC2_GAIN_REG44, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC3_GAIN_REG45, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC4_GAIN_REG46, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                }
                if (i == 2) {
                        /* set third es7210 PGA GAIN */
                        es7210_write(ES7210_MIC1_GAIN_REG43, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC2_GAIN_REG44, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC3_GAIN_REG45, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC4_GAIN_REG46, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                }

                if (i == 3) {
                        /* set third es7210 PGA GAIN */
                        es7210_write(ES7210_MIC1_GAIN_REG43, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC2_GAIN_REG44, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC3_GAIN_REG45, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                        es7210_write(ES7210_MIC4_GAIN_REG46, ES7210_MIC_GAIN,
                                     i2c_clt1[i]);
                }
        }
}
static void es7210_unmute(void)
{
        es7210_multi_chips_update_bits(ES7210_ADC34_MUTE_REG14, 0x03, 0x00);
        es7210_multi_chips_update_bits(ES7210_ADC12_MUTE_REG15, 0x03, 0x00);
        es7210_multi_chips_update_bits(ES7210_MIC1_GAIN_REG43, 0x1f, ES7210_MIC_GAIN);
        es7210_multi_chips_update_bits(ES7210_MIC2_GAIN_REG44, 0x1f, ES7210_MIC_GAIN);
        es7210_multi_chips_update_bits(ES7210_MIC3_GAIN_REG45, 0x1f, ES7210_MIC_GAIN);
        es7210_multi_chips_update_bits(ES7210_MIC4_GAIN_REG46, 0x1f, ES7210_MIC_GAIN);
}

static void pcm_pop_work_events(struct work_struct *work)
{
        es7210_unmute();
}

static int es7210_pcm_startup(struct snd_pcm_substream *substream,
                              struct snd_soc_dai *dai)
{
        struct snd_soc_component *component = dai->component;
        struct es7210_priv *es7210 = snd_soc_component_get_drvdata(component);

        if (es7210_init_reg == 0) {
                es7210_init_reg = 1;
                schedule_delayed_work(&es7210->pcm_pop_work, msecs_to_jiffies(100));
        }
        return 0;
}
static int es7210_pcm_hw_params(struct snd_pcm_substream *substream,
                                struct snd_pcm_hw_params *params,
                                struct snd_soc_dai *dai)
{
        struct snd_soc_component *component = dai->component;
        struct es7210_priv *priv = snd_soc_component_get_drvdata(component);
        int i;

        if (es7210_hw_param_reg == 1 && params_format(params) == priv->pcm_format) {
                return 0;
        }

        es7210_hw_param_reg = 1;
        priv->pcm_format = params_format(params);
        es7210_tdm_init_ratio(priv);

        es7210_multi_chips_update_bits(ES7210_RESET_CTL_REG00, 0x30, 0x30);
        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                /* set es7210 bit size */
                switch (priv->pcm_format) {
                case SNDRV_PCM_FORMAT_S16_LE:
                        es7210_update_bits(0x11, 0xE0, 0x60,
                                           i2c_clt1[i]);
                        break;
                case SNDRV_PCM_FORMAT_S20_3LE:
                        es7210_update_bits(0x11, 0xE0, 0x20,
                                           i2c_clt1[i]);
                        break;
                case SNDRV_PCM_FORMAT_S24_3LE:
                        es7210_update_bits(0x11, 0xE0, 0x00,
                                           i2c_clt1[i]);
                        break;
                case SNDRV_PCM_FORMAT_S24_LE:
                case SNDRV_PCM_FORMAT_S32_LE:
                        es7210_update_bits(0x11, 0xE0, 0x80,
                                           i2c_clt1[i]);
                        break;
                }
        }
        es7210_multi_chips_update_bits(ES7210_RESET_CTL_REG00, 0x30, 0x00);
        switch (params_rate(params)) {
        case 16000:
                break;
        case 32000:
                break;
        case 48000:
                break;
        case 64000:
                break;
        default:
                break;
        }

        return 0;
}

#define es7210_RATES SNDRV_PCM_RATE_8000_48000|SNDRV_PCM_RATE_64000|SNDRV_PCM_RATE_88200

#define es7210_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | SNDRV_PCM_FMTBIT_S24_3LE |\
    SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static struct snd_soc_dai_ops es7210_ops = {
        .startup = es7210_pcm_startup,
        .hw_params = es7210_pcm_hw_params,
        .set_fmt = es7210_set_dai_fmt,
        .set_sysclk = es7210_set_dai_sysclk,
};

static struct snd_soc_dai_driver es7210_dai0 = {
	.name = "ES7210 ADC 0",
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
                .channels_max = 2,
                .rates = es7210_RATES,
                .formats = es7210_FORMATS,
        },
        .ops = &es7210_ops,
};


static struct snd_soc_dai_driver es7210_dai1 = {
        .name = "ES7210 4CH ADC 1",
        .capture = {
                .stream_name = "Capture",
                .channels_min = 1,
                .channels_max = 4,
                .rates = es7210_RATES,
                .formats = es7210_FORMATS,
        },
        .ops = &es7210_ops,
};

static struct snd_soc_dai_driver es7210_dai2 = {
        .name = "ES7210 4CH ADC 2",
        .capture = {
                .stream_name = "Capture",
                .channels_min = 1,
                .channels_max = 4,
                .rates = es7210_RATES,
                .formats = es7210_FORMATS,
        },
        .ops = &es7210_ops,
};

static struct snd_soc_dai_driver es7210_dai3 = {
        .name = "ES7210 4CH ADC 3",
        .capture = {
                .stream_name = "Capture",
                .channels_min = 1,
                .channels_max = 4,
                .rates = es7210_RATES,
                .formats = es7210_FORMATS,
        },
        .ops = &es7210_ops,
};

static struct snd_soc_dai_driver *es7210_dai[] = {
        &es7210_dai0,
        &es7210_dai1,
        &es7210_dai2,
        &es7210_dai3,
};

#ifdef CONFIG_PM_SLEEP
static int es7210_suspend(struct snd_soc_component *component)
{
        int i = 0;
        struct es7210_priv *priv = snd_soc_component_get_drvdata(component);

        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                es7210_read(ES7210_RESET_CTL_REG00, &priv->suspend_reg00[i], i2c_clt1[i]);
                es7210_read(ES7210_CLK_ON_OFF_REG01, &priv->suspend_reg01[i], i2c_clt1[i]);
                es7210_read(ES7210_MCLK_CTL_REG02, &priv->suspend_reg02[i], i2c_clt1[i]);
                es7210_read(ES7210_MST_CLK_CTL_REG03, &priv->suspend_reg03[i], i2c_clt1[i]);
                es7210_read(ES7210_MST_LRCDIVH_REG04, &priv->suspend_reg04[i], i2c_clt1[i]);
                es7210_read(ES7210_MST_LRCDIVL_REG05, &priv->suspend_reg05[i], i2c_clt1[i]);
                es7210_read(ES7210_DIGITAL_PDN_REG06, &priv->suspend_reg06[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC_OSR_REG07, &priv->suspend_reg07[i], i2c_clt1[i]);
                es7210_read(ES7210_MODE_CFG_REG08, &priv->suspend_reg08[i], i2c_clt1[i]);
                es7210_read(ES7210_TCT0_CHPINI_REG09, &priv->suspend_reg09[i], i2c_clt1[i]);
                es7210_read(ES7210_TCT1_CHPINI_REG0A, &priv->suspend_reg0A[i], i2c_clt1[i]);
                es7210_read(ES7210_CHIP_STA_REG0B, &priv->suspend_reg0B[i], i2c_clt1[i]);
                es7210_read(ES7210_IRQ_CTL_REG0C, &priv->suspend_reg0C[i], i2c_clt1[i]);
                es7210_read(ES7210_MISC_CTL_REG0D, &priv->suspend_reg0D[i], i2c_clt1[i]);
                es7210_read(ES7210_DMIC_CTL_REG10, &priv->suspend_reg10[i], i2c_clt1[i]);
                es7210_read(ES7210_SDP_CFG1_REG11, &priv->suspend_reg11[i], i2c_clt1[i]);
                es7210_read(ES7210_SDP_CFG2_REG12, &priv->suspend_reg12[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC_AUTOMUTE_REG13, &priv->suspend_reg13[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC34_MUTE_REG14, &priv->suspend_reg14[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC12_MUTE_REG15, &priv->suspend_reg15[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC_SEL_REG16, &priv->suspend_reg16[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC_COM_CFG1_REG17, &priv->suspend_reg17[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC34_LVL_REG18, &priv->suspend_reg18[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC12_LVL_REG19, &priv->suspend_reg19[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC_COM_CFG2_REG1A, &priv->suspend_reg1A[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC4_MAX_GAIN_REG1B, &priv->suspend_reg1B[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC3_MAX_GAIN_REG1C, &priv->suspend_reg1C[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC2_MAX_GAIN_REG1D, &priv->suspend_reg1D[i], i2c_clt1[i]);
                es7210_read(ES7210_ALC1_MAX_GAIN_REG1E, &priv->suspend_reg1E[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC34_HPF2_REG20, &priv->suspend_reg20[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC34_HPF1_REG21, &priv->suspend_reg21[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC12_HPF2_REG22, &priv->suspend_reg22[i], i2c_clt1[i]);
                es7210_read(ES7210_ADC12_HPF1_REG23, &priv->suspend_reg23[i], i2c_clt1[i]);
                es7210_read(ES7210_ANALOG_SYS_REG40, &priv->suspend_reg40[i], i2c_clt1[i]);
                es7210_read(ES7210_MICBIAS12_REG41, &priv->suspend_reg41[i], i2c_clt1[i]);
                es7210_read(ES7210_MICBIAS34_REG42, &priv->suspend_reg42[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC1_GAIN_REG43, &priv->suspend_reg43[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC2_GAIN_REG44, &priv->suspend_reg44[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC3_GAIN_REG45, &priv->suspend_reg45[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC4_GAIN_REG46, &priv->suspend_reg46[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC1_LP_REG47, &priv->suspend_reg47[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC2_LP_REG48, &priv->suspend_reg48[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC3_LP_REG49, &priv->suspend_reg49[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC4_LP_REG4A, &priv->suspend_reg4A[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC12_PDN_REG4B, &priv->suspend_reg4B[i], i2c_clt1[i]);
                es7210_read(ES7210_MIC34_PDN_REG4C, &priv->suspend_reg4C[i], i2c_clt1[i]);
        }

        es7210_init_reg = 0;

        /* power down the controller */
        if (priv->pvdd)
                regulator_disable(priv->pvdd);
        if (priv->dvdd)
                regulator_disable(priv->dvdd);
        if (priv->avdd)
                regulator_disable(priv->avdd);
        if (priv->mvdd)
                regulator_disable(priv->mvdd);

        return 0;
}

static int es7210_resume(struct snd_soc_component *component)
{
        int ret = 0, i = 0;
        struct es7210_priv *priv = snd_soc_component_get_drvdata(component);

        /* power up the controller */
        if (priv->mvdd)
                ret |= regulator_enable(priv->mvdd);
        if (priv->avdd)
                ret |= regulator_enable(priv->avdd);
        if (priv->dvdd)
                ret |= regulator_enable(priv->dvdd);
        if (priv->pvdd)
                ret |= regulator_enable(priv->pvdd);
        if (ret) {
                pr_err("Failed to enable VDD regulator: %d\n", ret);
                return ret;
        }
        mdelay(10); // es7210 need 10ms setup time after power up.

        for (i = 0; i < ADC_DEV_MAXNUM; i++) {
                es7210_write(ES7210_RESET_CTL_REG00, priv->suspend_reg00[i], i2c_clt1[i]);
                es7210_write(ES7210_CLK_ON_OFF_REG01, priv->suspend_reg01[i], i2c_clt1[i]);
                es7210_write(ES7210_MCLK_CTL_REG02, priv->suspend_reg02[i], i2c_clt1[i]);
                es7210_write(ES7210_MST_CLK_CTL_REG03, priv->suspend_reg03[i], i2c_clt1[i]);
                es7210_write(ES7210_MST_LRCDIVH_REG04, priv->suspend_reg04[i], i2c_clt1[i]);
                es7210_write(ES7210_MST_LRCDIVL_REG05, priv->suspend_reg05[i], i2c_clt1[i]);
                es7210_write(ES7210_DIGITAL_PDN_REG06, priv->suspend_reg06[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC_OSR_REG07, priv->suspend_reg07[i], i2c_clt1[i]);
                es7210_write(ES7210_MODE_CFG_REG08, priv->suspend_reg08[i], i2c_clt1[i]);
                es7210_write(ES7210_TCT0_CHPINI_REG09, priv->suspend_reg09[i], i2c_clt1[i]);
                es7210_write(ES7210_TCT1_CHPINI_REG0A, priv->suspend_reg0A[i], i2c_clt1[i]);
                es7210_write(ES7210_CHIP_STA_REG0B, priv->suspend_reg0B[i], i2c_clt1[i]);
                es7210_write(ES7210_IRQ_CTL_REG0C, priv->suspend_reg0C[i], i2c_clt1[i]);
                es7210_write(ES7210_MISC_CTL_REG0D, priv->suspend_reg0D[i], i2c_clt1[i]);
                es7210_write(ES7210_DMIC_CTL_REG10, priv->suspend_reg10[i], i2c_clt1[i]);
                es7210_write(ES7210_SDP_CFG1_REG11, priv->suspend_reg11[i], i2c_clt1[i]);
                es7210_write(ES7210_SDP_CFG2_REG12, priv->suspend_reg12[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC_AUTOMUTE_REG13, priv->suspend_reg13[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC34_MUTE_REG14, priv->suspend_reg14[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC12_MUTE_REG15, priv->suspend_reg15[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC_SEL_REG16, priv->suspend_reg16[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC_COM_CFG1_REG17, priv->suspend_reg17[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC34_LVL_REG18, priv->suspend_reg18[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC12_LVL_REG19, priv->suspend_reg19[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC_COM_CFG2_REG1A, priv->suspend_reg1A[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC4_MAX_GAIN_REG1B, priv->suspend_reg1B[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC3_MAX_GAIN_REG1C, priv->suspend_reg1C[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC2_MAX_GAIN_REG1D, priv->suspend_reg1D[i], i2c_clt1[i]);
                es7210_write(ES7210_ALC1_MAX_GAIN_REG1E, priv->suspend_reg1E[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC34_HPF2_REG20, priv->suspend_reg20[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC34_HPF1_REG21, priv->suspend_reg21[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC12_HPF2_REG22, priv->suspend_reg22[i], i2c_clt1[i]);
                es7210_write(ES7210_ADC12_HPF1_REG23, priv->suspend_reg23[i], i2c_clt1[i]);
                es7210_write(ES7210_ANALOG_SYS_REG40, priv->suspend_reg40[i], i2c_clt1[i]);
                es7210_write(ES7210_MICBIAS12_REG41, priv->suspend_reg41[i], i2c_clt1[i]);
                es7210_write(ES7210_MICBIAS34_REG42, priv->suspend_reg42[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC1_GAIN_REG43, priv->suspend_reg43[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC2_GAIN_REG44, priv->suspend_reg44[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC3_GAIN_REG45, priv->suspend_reg45[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC4_GAIN_REG46, priv->suspend_reg46[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC1_LP_REG47, priv->suspend_reg47[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC2_LP_REG48, priv->suspend_reg48[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC3_LP_REG49, priv->suspend_reg49[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC4_LP_REG4A, priv->suspend_reg4A[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC12_PDN_REG4B, priv->suspend_reg4B[i], i2c_clt1[i]);
                es7210_write(ES7210_MIC34_PDN_REG4C, priv->suspend_reg4C[i], i2c_clt1[i]);
        }

        return ret;
}
#endif

static int es7210_probe(struct snd_soc_component *component)
{
        struct es7210_priv *es7210 = snd_soc_component_get_drvdata(component);
        u8 val, val1, cnt;
        int ret = 0;
        resume_es7210 = es7210;

        /* power up the controller */
        if (es7210->mvdd)
                ret |= regulator_enable(es7210->mvdd);
        if (es7210->avdd)
                ret |= regulator_enable(es7210->avdd);
        if (es7210->dvdd)
                ret |= regulator_enable(es7210->dvdd);
        if (es7210->pvdd)
                ret |= regulator_enable(es7210->pvdd);
        if (ret) {
                pr_err("Failed to enable VDD regulator: %d\n", ret);
                return ret;
        }

        tron_codec1[es7210_codec_num++] = component;

        INIT_DELAYED_WORK(&es7210->pcm_pop_work, pcm_pop_work_events);

        for (cnt = 0; cnt < ADC_DEV_MAXNUM; cnt++) {
                /* es7210 chip id */
                ret = es7210_read(0x3D, &val, i2c_clt1[cnt]);
                ret = es7210_read(0x3E, &val1, i2c_clt1[cnt]);
                if (ret < 0) {
                        pr_err("%s: read chipid failed %d\n", __func__, ret);
                        goto exit_i2c_check_id_failed;
                }
        }

        es7210_tdm_init_codec(es7210->tdm_mode);

exit_i2c_check_id_failed:
        return 0;
}

static void es7210_remove(struct snd_soc_component *component)
{
        struct es7210_priv *es7210 = snd_soc_component_get_drvdata(component);

        /* power down the controller */
        if (es7210->pvdd)
                regulator_disable(es7210->pvdd);
        if (es7210->dvdd)
                regulator_disable(es7210->dvdd);
        if (es7210->avdd)
                regulator_disable(es7210->avdd);
        if (es7210->mvdd)
                regulator_disable(es7210->mvdd);
}

static int es7210_set_bias_level(struct snd_soc_component *component,
                                 enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		break;

	case SND_SOC_BIAS_STANDBY:
		break;

	case SND_SOC_BIAS_OFF:
		break;
	}
	return 0;
}

static const DECLARE_TLV_DB_SCALE(mic_boost_tlv, 0, 300, 0);

static int es7210_micboost1_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(0x43, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[0]);
        return 0;
}

static int es7210_micboost1_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(0x43, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}

static int es7210_micboost2_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(0x44, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[0]);
        return 0;
}

static int es7210_micboost2_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(0x44, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}

static int es7210_micboost3_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(0x45, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[0]);
        return 0;
}

static int es7210_micboost3_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(0x45, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost4_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(0x46, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[0]);
        return 0;
}

static int es7210_micboost4_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(0x46, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}

static int es7210_adc1_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x01,
    ucontrol->value.integer.value[0]&0x01, i2c_clt1[0]);
        return 0;
}

static int es7210_adc1_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}

static int es7210_adc2_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x02,
                         (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[0]);
        return 0;
}

static int es7210_adc2_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_adc3_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x01,
                           ucontrol->value.integer.value[0]&0x01, i2c_clt1[0]);
        return 0;
}

static int es7210_adc3_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}
static int es7210_adc4_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x02,
                           (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[0]);
        return 0;
}

static int es7210_adc4_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[0]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_micboost5_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(0x43, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[1]);
        return 0;
}

static int es7210_micboost5_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(0x43, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost6_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(0x44, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[1]);
        return 0;
}

static int es7210_micboost6_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(0x44, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost7_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(0x45, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[1]);
        return 0;
}

static int es7210_micboost7_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(0x45, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost8_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(0x46, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[1]);
        return 0;
}

static int es7210_micboost8_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(0x46, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_adc5_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x01,
        ucontrol->value.integer.value[0]&0x01, i2c_clt1[1]);
        return 0;
}

static int es7210_adc5_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}

static int es7210_adc6_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x02,
                         (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[1]);
        return 0;
}

static int es7210_adc6_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_adc7_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x01,
                           ucontrol->value.integer.value[0]&0x01, i2c_clt1[1]);
        return 0;
}

static int es7210_adc7_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}
static int es7210_adc8_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x02,
                           (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[1]);
        return 0;
}

static int es7210_adc8_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[1] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[1]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_micboost9_setting_set(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(0x43, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[2]);
        return 0;
}

static int es7210_micboost9_setting_get(struct snd_kcontrol *kcontrol,
                                        struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(0x43, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost10_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(0x44, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[2]);
        return 0;
}

static int es7210_micboost10_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(0x44, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost11_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(0x45, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[2]);
        return 0;
}

static int es7210_micboost11_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(0x45, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost12_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(0x46, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[2]);
        return 0;
}

static int es7210_micboost12_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(0x46, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_adc9_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x01,
        ucontrol->value.integer.value[0]&0x01, i2c_clt1[2]);
        return 0;
}

static int es7210_adc9_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}

static int es7210_adc10_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x02,
                         (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[2]);
        return 0;
}

static int es7210_adc10_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_adc11_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x01,
                           ucontrol->value.integer.value[0]&0x01, i2c_clt1[2]);
        return 0;
}

static int es7210_adc11_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}
static int es7210_adc12_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x02,
                           (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[2]);
        return 0;
}

static int es7210_adc12_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[2] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[2]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_micboost13_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(0x43, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[3]);
        return 0;
}

static int es7210_micboost13_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(0x43, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost14_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(0x44, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[3]);
        return 0;
}

static int es7210_micboost14_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(0x44, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost15_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(0x45, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[3]);
        return 0;
}

static int es7210_micboost15_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(0x45, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_micboost16_setting_set(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(0x46, 0x0F, ucontrol->value.integer.value[0], i2c_clt1[3]);
        return 0;
}

static int es7210_micboost16_setting_get(struct snd_kcontrol *kcontrol,
                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(0x46, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val;
        return 0;
}
static int es7210_adc13_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x01,
        ucontrol->value.integer.value[0]&0x01, i2c_clt1[3]);
        return 0;
}

static int es7210_adc13_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}

static int es7210_adc14_mute_set(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC12_MUTE_REG15, 0x02,
                         (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[3]);
        return 0;
}

static int es7210_adc14_mute_get(struct snd_kcontrol *kcontrol,
                               struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(ES7210_ADC12_MUTE_REG15, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static int es7210_adc15_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x01,
                           ucontrol->value.integer.value[0]&0x01, i2c_clt1[3]);
        return 0;
}

static int es7210_adc15_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = val & 0x01;
        return 0;
}
static int es7210_adc16_mute_set(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_update_bits(ES7210_ADC34_MUTE_REG14, 0x02,
                           (ucontrol->value.integer.value[0] & 0x01) << 1, i2c_clt1[3]);
        return 0;
}

static int es7210_adc16_mute_get(struct snd_kcontrol *kcontrol,
                                struct snd_ctl_elem_value *ucontrol)
{
        u8 val;
        if (i2c_clt1[3] == NULL)
                return 0;
        es7210_read(ES7210_ADC34_MUTE_REG14, &val, i2c_clt1[3]);
        ucontrol->value.integer.value[0] = (val & 0x02) >> 1;
        return 0;
}

static const struct snd_kcontrol_new es7210_snd_controls[] = {
        SOC_SINGLE_EXT_TLV("PGA1_setting",
        0x43, 0, 0x0E, 0,
        es7210_micboost1_setting_get, es7210_micboost1_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA2_setting",
        0x44, 0, 0x0E, 0,
        es7210_micboost2_setting_get, es7210_micboost2_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA3_setting",
        0x45, 0, 0x0E, 0,
        es7210_micboost3_setting_get, es7210_micboost3_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA4_setting",
        0x46, 0, 0x0E, 0,
        es7210_micboost4_setting_get, es7210_micboost4_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT("ADC1_MUTE", ES7210_ADC12_MUTE_REG15, 0, 1, 0,
        es7210_adc1_mute_get, es7210_adc1_mute_set),
        SOC_SINGLE_EXT("ADC2_MUTE", ES7210_ADC12_MUTE_REG15, 1, 1, 0,
        es7210_adc2_mute_get, es7210_adc2_mute_set),
        SOC_SINGLE_EXT("ADC3_MUTE", ES7210_ADC34_MUTE_REG14, 0, 1, 0,
        es7210_adc3_mute_get, es7210_adc3_mute_set),
        SOC_SINGLE_EXT("ADC4_MUTE", ES7210_ADC34_MUTE_REG14, 1, 1, 0,
        es7210_adc4_mute_get, es7210_adc4_mute_set),
        SOC_SINGLE_EXT_TLV("PGA5_setting",
        0x43, 0, 0x0E, 0,
        es7210_micboost5_setting_get, es7210_micboost5_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA6_setting",
        0x44, 0, 0x0E, 0,
        es7210_micboost6_setting_get, es7210_micboost6_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA7_setting",
        0x45, 0, 0x0E, 0,
        es7210_micboost7_setting_get, es7210_micboost7_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA8_setting",
        0x46, 0, 0x0E, 0,
        es7210_micboost8_setting_get, es7210_micboost8_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT("ADC5_MUTE", ES7210_ADC12_MUTE_REG15, 0, 1, 0,
        es7210_adc5_mute_get, es7210_adc5_mute_set),
        SOC_SINGLE_EXT("ADC6_MUTE", ES7210_ADC12_MUTE_REG15, 1, 1, 0,
        es7210_adc6_mute_get, es7210_adc6_mute_set),
        SOC_SINGLE_EXT("ADC7_MUTE", ES7210_ADC34_MUTE_REG14, 0, 1, 0,
        es7210_adc7_mute_get, es7210_adc7_mute_set),
        SOC_SINGLE_EXT("ADC8_MUTE", ES7210_ADC34_MUTE_REG14, 1, 1, 0,
        es7210_adc8_mute_get, es7210_adc8_mute_set),
        SOC_SINGLE_EXT_TLV("PGA9_setting",
        0x43, 0, 0x0E, 0,
        es7210_micboost9_setting_get, es7210_micboost9_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA10_setting",
        0x44, 0, 0x0E, 0,
        es7210_micboost10_setting_get, es7210_micboost10_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA11_setting",
        0x45, 0, 0x0E, 0,
        es7210_micboost11_setting_get, es7210_micboost11_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA12_setting",
        0x46, 0, 0x0E, 0,
        es7210_micboost12_setting_get, es7210_micboost12_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT("ADC9_MUTE", ES7210_ADC12_MUTE_REG15, 0, 1, 0,
        es7210_adc9_mute_get, es7210_adc9_mute_set),
        SOC_SINGLE_EXT("ADC10_MUTE", ES7210_ADC12_MUTE_REG15, 1, 1, 0,
        es7210_adc10_mute_get, es7210_adc10_mute_set),
        SOC_SINGLE_EXT("ADC11_MUTE", ES7210_ADC34_MUTE_REG14, 0, 1, 0,
        es7210_adc11_mute_get, es7210_adc11_mute_set),
        SOC_SINGLE_EXT("ADC12_MUTE", ES7210_ADC34_MUTE_REG14, 1, 1, 0,
        es7210_adc12_mute_get, es7210_adc12_mute_set),
        SOC_SINGLE_EXT_TLV("PGA13_setting",
        0x43, 0, 0x0E, 0,
        es7210_micboost13_setting_get, es7210_micboost13_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA14_setting",
        0x44, 0, 0x0E, 0,
        es7210_micboost14_setting_get, es7210_micboost14_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA15_setting",
        0x45, 0, 0x0E, 0,
        es7210_micboost15_setting_get, es7210_micboost15_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT_TLV("PGA16_setting",
        0x46, 0, 0x0E, 0,
        es7210_micboost16_setting_get, es7210_micboost16_setting_set,
        mic_boost_tlv),
        SOC_SINGLE_EXT("ADC13_MUTE", ES7210_ADC12_MUTE_REG15, 0, 1, 0,
        es7210_adc13_mute_get, es7210_adc13_mute_set),
        SOC_SINGLE_EXT("ADC14_MUTE", ES7210_ADC12_MUTE_REG15, 1, 1, 0,
        es7210_adc14_mute_get, es7210_adc14_mute_set),
        SOC_SINGLE_EXT("ADC15_MUTE", ES7210_ADC34_MUTE_REG14, 0, 1, 0,
        es7210_adc15_mute_get, es7210_adc15_mute_set),
        SOC_SINGLE_EXT("ADC16_MUTE", ES7210_ADC34_MUTE_REG14, 1, 1, 0,
        es7210_adc16_mute_get, es7210_adc16_mute_set),
};

static struct snd_soc_component_driver soc_codec_dev_es7210 = {
        .name = "es7210",
        .probe = es7210_probe,
        .remove = es7210_remove,
#ifdef CONFIG_PM_SLEEP
        .suspend = es7210_suspend,
        .resume = es7210_resume,
#endif
        .set_bias_level = es7210_set_bias_level,
        .controls = es7210_snd_controls,
        .num_controls = ARRAY_SIZE(es7210_snd_controls),

};

static ssize_t es7210_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
        int val = 0, flag = 0;
        u8 i = 0, reg, num, value_w, value_r;

        struct es7210_priv *es7210 = dev_get_drvdata(dev);
        val = simple_strtol(buf, NULL, 16);
        flag = (val >> 16) & 0xFF;

        if (flag) {
                reg = (val >> 8) & 0xFF;
                value_w = val & 0xFF;
                printk("\nWrite: start REG:0x%02x,val:0x%02x,count:0x%02x\n", reg, value_w, flag);
                while (flag--) {
                        es7210_write(reg, value_w, es7210->i2c_client);
                        printk("Write 0x%02x to REG:0x%02x\n", value_w, reg);
                        reg++;
                }
        } else {
                reg = (val >> 8) & 0xFF;
                num = val & 0xff;
                printk("\nRead: start REG:0x%02x,count:0x%02x\n", reg, num);
                do {
                        value_r = 0;
                        es7210_read(reg, &value_r, es7210->i2c_client);
                        printk("REG[0x%02x]: 0x%02x;  ", reg, value_r);
                        reg++;
                        i++;
                        if ((i == num) || (i % 4 == 0))   printk("\n");
                } while (i < num);
        }

        return count;
}

static ssize_t es7210_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        u8 value, i;
        struct es7210_priv *es7210 = dev_get_drvdata(dev);

        printk("echo flag|reg|val > es7210\n");
        printk("eg read star address=0x06,count 0x10:echo 0610 >es7210\n");
        printk("eg write star address=0x90,value=0x3c,count=4:echo 4903c >es7210\n");

        for (i = 0; i < 0x4d; i++) {
                es7210_read(i, &value, es7210->i2c_client);
                printk("reg[0x%x]=0x%x\n", i, value);
        }
        return 0;
}

static DEVICE_ATTR(es7210, 0644, es7210_show, es7210_store);

static struct attribute *es7210_debug_attrs[] = {
        &dev_attr_es7210.attr,
        NULL,
};

static struct attribute_group es7210_debug_attr_group = {
        .name   = "es7210_debug",
        .attrs  = es7210_debug_attrs,
};

/*
 * If the i2c layer weren't so broken, we could pass this kind of data
 * around
 */
static int es7210_i2c_probe(struct i2c_client *i2c_client)
{
		const struct i2c_device_id *i2c_id = i2c_client_get_device_id(i2c_client);
		struct es7210_priv *es7210;
        struct device_node *np = i2c_client->dev.of_node;
        const char* property;
        int ret;

        es7210 = devm_kzalloc(&i2c_client->dev, sizeof(struct es7210_priv),
			      GFP_KERNEL);
        if (es7210 == NULL)
                return -ENOMEM;
        es7210->i2c_client = i2c_client;

        property = of_get_property(np, "work-mode", NULL);
        if (property) {
                if (!strcmp(property, "ES7210_TDM_1LRCK_DSPB")) {
                        es7210->tdm_mode =  ES7210_TDM_1LRCK_DSPB;
                } else if (!strcmp(property, "ES7210_NORMAL_I2S")) {
                        es7210->tdm_mode =  ES7210_NORMAL_I2S;
                } else {
                        pr_err("unsupported work mode\n");
                        return -EINVAL;
                }
        } else {
                pr_warn("es7210 work-mode not defined.using ES7210_NORMAL_I2S by default\n");
                es7210->tdm_mode =  ES7210_NORMAL_I2S;
        }

        if (of_property_read_u32(np, "channels-max", &ES7210_CHANNELS_MAX) == 0) {
                if (ES7210_CHANNELS_MAX == MIC_CHN_2 || ES7210_CHANNELS_MAX == MIC_CHN_4) {
                        ADC_DEV_MAXNUM =  1;
                } else if (ES7210_CHANNELS_MAX == MIC_CHN_6 || ES7210_CHANNELS_MAX == MIC_CHN_8) {
                        ADC_DEV_MAXNUM =  2;
                } else if (ES7210_CHANNELS_MAX == MIC_CHN_10 || ES7210_CHANNELS_MAX == MIC_CHN_12) {
                        ADC_DEV_MAXNUM =  3;
                } else if (ES7210_CHANNELS_MAX == MIC_CHN_14 || ES7210_CHANNELS_MAX == MIC_CHN_16) {
                        ADC_DEV_MAXNUM =  4;
                } else {
                        pr_err("es7210 unsupported channels-max\n");
                        return -EINVAL;
                }
        } else {
                pr_warn("es7210 channels-max not defined.using MIC_CHN_2 by default\n");
                ES7210_CHANNELS_MAX = MIC_CHN_2;
                ADC_DEV_MAXNUM =  1;
        }
        pr_debug("%s es7210->tdm_mode=%d channels-max=%d ADC_DEV_MAXNUM=%d\n", __func__, es7210->tdm_mode, ES7210_CHANNELS_MAX, ADC_DEV_MAXNUM);

        es7210->mvdd = devm_regulator_get(&i2c_client->dev, "MVDD");
        if (IS_ERR(es7210->mvdd)) {
                ret = PTR_ERR(es7210->mvdd);
                dev_warn(&i2c_client->dev, "Failed to get MVDD regulator: %d\n", ret);
                es7210->mvdd = NULL;
        }
        es7210->avdd = devm_regulator_get(&i2c_client->dev, "AVDD");
        if (IS_ERR(es7210->avdd)) {
                ret = PTR_ERR(es7210->avdd);
                dev_warn(&i2c_client->dev, "Failed to get AVDD regulator: %d\n", ret);
                es7210->avdd = NULL;
        }
        es7210->dvdd = devm_regulator_get(&i2c_client->dev, "DVDD");
        if (IS_ERR(es7210->dvdd)) {
                ret = PTR_ERR(es7210->dvdd);
                dev_warn(&i2c_client->dev, "Failed to get DVDD regulator: %d\n", ret);
                es7210->dvdd = NULL;
        }
        es7210->pvdd = devm_regulator_get(&i2c_client->dev, "PVDD");
        if (IS_ERR(es7210->pvdd)) {
                ret = PTR_ERR(es7210->pvdd);
                dev_warn(&i2c_client->dev, "Failed to get PVDD regulator: %d\n", ret);
                es7210->pvdd = NULL;
        }

        if (of_property_read_u32(np, "mclk-sclk-ratio", &es7210->mclk_sclk_ratio) != 0) {
                es7210->mclk_sclk_ratio = 1;
        }

        i2c_set_clientdata(i2c_client, es7210);
        if (i2c_id->driver_data < ADC_DEV_MAXNUM) {
                i2c_clt1[i2c_id->driver_data] = i2c_client;
                ret = devm_snd_soc_register_component(&i2c_client->dev, &soc_codec_dev_es7210,
                                             es7210_dai[i2c_id->driver_data], 1);
                if (ret < 0) {
                        kfree(es7210);
                        return ret;
                }
        }
        ret = sysfs_create_group(&i2c_client->dev.kobj, &es7210_debug_attr_group);
        if (ret) {
                pr_err("failed to create attr group\n");
        }
        return ret;
}


static const struct i2c_device_id es7210_i2c_id[] = {
        { "MicArray_0", 0 },//es7210_0
        { "MicArray_1", 1 },//es7210_1
        { "MicArray_2", 2 },//es7210_2
        { "MicArray_3", 3 },//es7210_3
        { }
};
MODULE_DEVICE_TABLE(i2c_client, es7210_i2c_id);

static const struct of_device_id es7210_dt_ids[] = {
        { .compatible = "MicArray_0", },//es7210_0
        { .compatible = "MicArray_1", },//es7210_1
        { .compatible = "MicArray_2", },//es7210_2
        { .compatible = "MicArray_3", },//es7210_3
        {}
};
MODULE_DEVICE_TABLE(of, es7210_dt_ids);

static struct i2c_driver es7210_i2c_driver = {
        .driver = {
                .name = "es7210",
                .owner = THIS_MODULE,
                .of_match_table = es7210_dt_ids,
        },
        .probe = es7210_i2c_probe,
        .id_table = es7210_i2c_id,

};


static int __init es7210_modinit(void)
{
        int ret;
	ret = i2c_add_driver(&es7210_i2c_driver);
        if (ret != 0)
                pr_err("Failed to register es7210 i2c driver : %d \n", ret);
        return ret;

}
module_init(es7210_modinit);

MODULE_DESCRIPTION("ASoC ES7210 audio adc driver");
MODULE_LICENSE("GPL v2");
