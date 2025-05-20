// SPDX-License-Identifier: GPL-2.0-only
/*
 * adc driver for Spacemit P1
 *
 * Copyright (c) 2023, Spacemit Co., Ltd
 */

#include <linux/of.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_wakeirq.h>
#include <linux/iio/iio.h>
#include <linux/iio/types.h>
#include <linux/of_device.h>
#include <linux/iio/driver.h>
#include <linux/mfd/spacemit_p1.h>

struct spm_p1_adc_info {
	int irq;
	struct regmap *regmap;
	struct mutex lock;
	struct completion completion;
};

struct adc_match_data {
	int nr_desc;
	const char *name;
	struct iio_chan_spec *iio_desc;
};

static struct adc_match_data *match_data;

static struct iio_chan_spec spm_p1_iio_desc[] = {
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 0,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 1,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 2,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 3,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 4,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.indexed = 1,
		.type = IIO_VOLTAGE,
		.channel = 5,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
	},
};

static struct adc_match_data spm_p1_adc_match_data = {
	.iio_desc = spm_p1_iio_desc,
	.nr_desc = ARRAY_SIZE(spm_p1_iio_desc),
	.name = "spm_p1",
};

static const struct of_device_id spm_p1_adc_id_table[] = {
	{ .compatible = "spacemit,p1,adc", .data = (void *)&spm_p1_adc_match_data },
	{ }
};
MODULE_DEVICE_TABLE(of, spm_p1_adc_id_table);

static int spm_p1_adc_raw(struct iio_dev *indio_dev, struct iio_chan_spec const *chan, int *val)
{
	unsigned int value;
	unsigned int adc_val_h, adc_val_l;
	struct spm_p1_adc_info *info;

	info = iio_priv(indio_dev);

	mutex_lock(&info->lock);

	/* reset the ADC auto register */
	regmap_update_bits(info->regmap, SPM_P1_ADC_AUTO_REG,
			SPM_P1_ADC_AUTO_BIT_MSK, 0);

	/* enable the ADC : ADC_CTRL[0] */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CTRL_REG,
			SPM_P1_ADC_CTRL_BIT_MSK, (1 << SPM_P1_ADC_CTRL_EN_BIT_OFFSET));

	/* choose the channel of adc : ADC_CFG[1] */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CFG1_REG,
			SPM_P1_ADC_CFG1_ADC_CHNNL_SEL_BIT_MSK,
			(chan->channel + SPM_P1_ADC_EXTERNAL_CHANNEL_OFFSET) <<
			 SPM_P1_ADC_CFG1_ADC_CHNNL_SEL_BIT_OFFSET);

	/* ADC go */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CTRL_REG,
			SPM_P1_ADC_CTRL_BIT_MSK, (1 << SPM_P1_ADC_CTRL_GO_BIT_OFFSET)  |
						(1 << SPM_P1_ADC_CTRL_EN_BIT_OFFSET));

	/* then wait the completion */
	wait_for_completion(&info->completion);

	regmap_read(info->regmap, SPM_P1_ADCIN0_RES_H_REG + chan->channel * 2, &adc_val_h);
	regmap_read(info->regmap, SPM_P1_ADCIN0_RES_L_REG + chan->channel * 2, &adc_val_l);

	regmap_read(info->regmap, SPM_P1_VERSION_ID_REG, &value);

	*val = (adc_val_h << (ffs(SPM_P1_ADCIN0_REG_L_BIT_MSK) - 1)) | (
			(adc_val_l & SPM_P1_ADCIN0_REG_L_BIT_MSK) >>
			(ffs(SPM_P1_ADCIN0_REG_L_BIT_MSK) - 1));

	if (value == 0) {
		/*
		 * if the version of P1 is A, the data read from the
		 * register is the inverse of the real data and the
		 * conversion accuracy of P1 is 12 bits
		 */
		*val = 4095 - *val;
	}

	pr_debug("%s:%d, read channel:%d, val:%u\n", __func__, __LINE__, chan->channel,
			*val);

	mutex_unlock(&info->lock);

	return IIO_VAL_INT;
}

static int spm_p1_adc_scale(struct iio_chan_spec const *chan, int *val, int *val2)
{
	switch (chan->type) {
	case IIO_VOLTAGE:
		*val = 0;
		/* 3000 % 4095 ~ 0.7326mv */
		*val2 = 732600;
		return IIO_VAL_INT_PLUS_MICRO;

	default:
		return -EINVAL;
	}

	return -EINVAL;
}

static int spm_p1_adc_read_raw(struct iio_dev *indio_dev,
			       struct iio_chan_spec const *chan,
			       int *val, int *val2, long mask)
{
	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		return spm_p1_adc_raw(indio_dev, chan, val);

	case IIO_CHAN_INFO_SCALE:
		return spm_p1_adc_scale(chan, val, val2);

	default:
		return -EINVAL;
	}

	return -EINVAL;
}

static const struct iio_info spm_p1_adc_iio_info = {
	.read_raw = &spm_p1_adc_read_raw,
};

static irqreturn_t adc_complete_irq(int irq, void *_adc)
{
	struct spm_p1_adc_info *info;
	struct iio_dev *indio_dev = (struct iio_dev *)_adc;

	info = iio_priv(indio_dev);

	complete(&info->completion);

	return IRQ_HANDLED;
}

static void spm_p1_adc_init(struct iio_dev *indio_dev)
{
	struct spm_p1_adc_info *info = iio_priv(indio_dev);

	/* enable chop */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CFG1_REG, SPM_P1_ADC_CFG1_ADC_CHOP_EN_BIT_MSK,
			   1 << SPM_P1_ADC_CFG1_ADC_CHOP_EN_BIT_OFFSET);

	/* set the vref: 3v3 */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CFG2_REG, SPM_P1_ADC_CFG2_REF_SEL_BIT_MSK,
			   SPM_P1_ADC_CFG2_3V3_REF << SPM_P1_ADC_CFG2_REF_SEL_BIT_OFFSET);
	/* set adc deb num: 7 */
	regmap_update_bits(info->regmap, SPM_P1_ADC_CFG2_REG, SPM_P1_ADC_CFG2_DEB_NUM_BIT_MSK,
			   SPM_P1_ADC_CFG2_7_DEB_NUM << SPM_P1_ADC_CFG2_DEB_NUM_BIT_OFFSET);
}

static int spm_p1_adc_probe(struct platform_device *pdev)
{
	int ret;
	struct iio_dev *indio_dev;
	struct spm_p1_adc_info *info;
	const struct of_device_id *of_id;
	struct spacemit_pmic *pmic = dev_get_drvdata(pdev->dev.parent);

	indio_dev = devm_iio_device_alloc(&pdev->dev, sizeof(*info));
	if (!indio_dev)
		return -ENOMEM;

	info = iio_priv(indio_dev);
	info->irq = platform_get_irq(pdev, 0);
	if (info->irq < 0) {
		dev_err(&pdev->dev, "get irq failed\n");
		return info->irq;
	}

	ret = devm_request_any_context_irq(&pdev->dev, info->irq,
				adc_complete_irq, IRQF_TRIGGER_NONE | IRQF_ONESHOT,
				"p1-adc", indio_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "Can't register adc irq: %d\n", ret);
		return ret;
	}

	info->regmap = pmic->regmap;

	mutex_init(&info->lock);
	init_completion(&info->completion);

	of_id = of_match_device(spm_p1_adc_id_table, &pdev->dev);
	if (!of_id) {
		dev_err(&pdev->dev, "Unable to match OF ID\n");
		return -ENODEV;
	}

	/* adc init */
	spm_p1_adc_init(indio_dev);

	match_data = (struct adc_match_data *)of_id->data;

	indio_dev->name = pdev->name;
	indio_dev->channels = match_data->iio_desc;
	indio_dev->num_channels = match_data->nr_desc;
	indio_dev->info = &spm_p1_adc_iio_info;
	indio_dev->modes = INDIO_DIRECT_MODE;

	ret = devm_iio_map_array_register(&pdev->dev, indio_dev, NULL);
	if (ret < 0)
		return ret;

	return devm_iio_device_register(&pdev->dev, indio_dev);
}

static struct platform_driver spm_p1_adc_driver = {
	.probe = spm_p1_adc_probe,
	.driver = {
		.name = "spm-p1-adc",
		.of_match_table = of_match_ptr(spm_p1_adc_id_table),
	},
};
module_platform_driver(spm_p1_adc_driver);

MODULE_DESCRIPTION("Spacemit p1 adc driver");
MODULE_LICENSE("GPL");
