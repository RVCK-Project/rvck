// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Regulator driver for Spacemit P1
 *
 * Copyright (c) 2023, SPACEMIT Co., Ltd
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/gpio/consumer.h>
#include <linux/mfd/spacemit_p1.h>

/* regulator: match data */
struct regulator_match_data {
	int nr_desc;
	int sleep_reg_offset;
	const struct regulator_desc *desc;
	const char *name;
};

static struct regulator_match_data *match_data;

static int spm_p1_regulator_set_suspend_voltage(struct regulator_dev *rdev, int uV)
{
	unsigned int reg;

	int sel = regulator_map_voltage_linear_range(rdev, uV, uV);

	if (sel < 0)
		return -EINVAL;

	/* means that we will disable this vol in suspend */
	if (uV == rdev->constraints->max_uV)
		/* BUCK will set 0xff to close the power */
		sel = rdev->desc->vsel_mask;
	else if (uV == rdev->constraints->min_uV)
		/* LDO will set zero to close the power */
		sel = 0;

	reg = rdev->desc->vsel_reg + match_data->sleep_reg_offset;

	return regmap_update_bits(rdev->regmap, reg, rdev->desc->vsel_mask, sel);
}

static const struct regulator_ops pmic_dcdc_ldo_ops = {
	.list_voltage = regulator_list_voltage_linear_range,
	.map_voltage = regulator_map_voltage_linear_range,
	.get_voltage_sel = regulator_get_voltage_sel_regmap,
	.set_voltage_sel = regulator_set_voltage_sel_regmap,
	.set_voltage_time_sel = regulator_set_voltage_time_sel,
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.is_enabled = regulator_is_enabled_regmap,
	.set_suspend_voltage = spm_p1_regulator_set_suspend_voltage,
};

static const struct regulator_ops pmic_switch_ops = {
	.enable = regulator_enable_regmap,
	.disable = regulator_disable_regmap,
	.is_enabled = regulator_is_enabled_regmap,
};

static const struct linear_range spm_p1_buck_ranges[] = {
	REGULATOR_LINEAR_RANGE(500000, 0x0, 0xaa, 5000),
	REGULATOR_LINEAR_RANGE(1375000, 0xab, 0xff, 25000),
};

static const struct linear_range spm_p1_ldo_ranges[] = {
	REGULATOR_LINEAR_RANGE(500000, 0xb, 0x7f, 25000),
};

/* common regulator defination */
#define SPM8XX_DESC_COMMON(_id, _match, _supply, _nv, _vr, _vm, _er, _em, _lr, _ops)       \
	{								\
		.name		= (_match),				\
		.supply_name	= (_supply),				\
		.of_match	= of_match_ptr(_match),			\
		.regulators_node = of_match_ptr("regulators"),		\
		.ops		= _ops,			\
		.type		= REGULATOR_VOLTAGE,			\
		.id		= (_id),				\
		.n_voltages     = (_nv),				\
		.owner		= THIS_MODULE,				\
		.vsel_reg       = (_vr),				\
		.vsel_mask      = (_vm),				\
		.enable_reg	= (_er),				\
		.enable_mask	= (_em),				\
		.volt_table	= NULL,					\
		.linear_ranges	= (_lr),				\
		.n_linear_ranges	= ARRAY_SIZE(_lr),		\
	}

#define SPM8XX_DESC_SWITCH_COM(_id, _match, _supply, _ereg, _emask,	\
	_enval, _disval, _ops)						\
	{								\
		.name		= (_match),				\
		.supply_name	= (_supply),				\
		.of_match	= of_match_ptr(_match),			\
		.regulators_node = of_match_ptr("regulators"),		\
		.type		= REGULATOR_VOLTAGE,			\
		.id		= (_id),				\
		.enable_reg	= (_ereg),				\
		.enable_mask	= (_emask),				\
		.enable_val     = (_enval),				\
		.disable_val     = (_disval),				\
		.owner		= THIS_MODULE,				\
		.ops		= _ops					\
	}

/* regulator configuration */
#define SPM_P1_DESC(_id, _match, _supply, _nv, _vr, _vm, _er, _em, _lr)	\
	SPM8XX_DESC_COMMON(_id, _match, _supply, _nv, _vr, _vm, _er, _em, _lr,	\
			&pmic_dcdc_ldo_ops)

#define SPM_P1_DESC_SWITCH(_id, _match, _supply, _ereg, _emask)	\
	SPM8XX_DESC_SWITCH_COM(_id, _match, _supply, _ereg, _emask,	\
	0, 0, &pmic_switch_ops)

static const struct regulator_desc spm_p1_reg[] = {
	/* BUCK */
	SPM_P1_DESC(SPM_P1_ID_DCDC1, "DCDC_REG1", "vcc_sys",
			255, SPM_P1_BUCK1_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK1_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),

	SPM_P1_DESC(SPM_P1_ID_DCDC2, "DCDC_REG2", "vcc_sys",
			255, SPM_P1_BUCK2_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK2_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),

	SPM_P1_DESC(SPM_P1_ID_DCDC3, "DCDC_REG3", "vcc_sys",
			255, SPM_P1_BUCK3_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK3_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),

	SPM_P1_DESC(SPM_P1_ID_DCDC4, "DCDC_REG4", "vcc_sys",
			255, SPM_P1_BUCK4_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK4_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),

	SPM_P1_DESC(SPM_P1_ID_DCDC5, "DCDC_REG5", "vcc_sys",
			255, SPM_P1_BUCK5_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK5_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),

	SPM_P1_DESC(SPM_P1_ID_DCDC6, "DCDC_REG6", "vcc_sys",
			255, SPM_P1_BUCK6_VSEL_REG, SPM_P1_BUCK_VSEL_MASK,
			SPM_P1_BUCK6_CTRL_REG, SMP8821_BUCK_EN_MASK,
			spm_p1_buck_ranges),
	/* ALDO */
	SPM_P1_DESC(SPM_P1_ID_LDO1, "LDO_REG1", "vcc_sys",
			128, SPM_P1_ALDO1_VOLT_REG, SPM_P1_ALDO_VSEL_MASK,
			SPM_P1_ALDO1_CTRL_REG, SPM_P1_ALDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO2, "LDO_REG2", "vcc_sys",
			128, SPM_P1_ALDO2_VOLT_REG, SPM_P1_ALDO_VSEL_MASK,
			SPM_P1_ALDO2_CTRL_REG, SPM_P1_ALDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO3, "LDO_REG3", "vcc_sys",
			128, SPM_P1_ALDO3_VOLT_REG, SPM_P1_ALDO_VSEL_MASK,
			SPM_P1_ALDO3_CTRL_REG, SPM_P1_ALDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO4, "LDO_REG4", "vcc_sys",
			128, SPM_P1_ALDO4_VOLT_REG, SPM_P1_ALDO_VSEL_MASK,
			SPM_P1_ALDO4_CTRL_REG, SPM_P1_ALDO_EN_MASK, spm_p1_ldo_ranges),

	/* DLDO */
	SPM_P1_DESC(SPM_P1_ID_LDO5, "LDO_REG5", "dcdc5",
			128, SPM_P1_DLDO1_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO1_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO6, "LDO_REG6", "dcdc5",
			128, SPM_P1_DLDO2_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO2_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO7, "LDO_REG7", "dcdc5",
			128, SPM_P1_DLDO3_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO3_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO8, "LDO_REG8", "dcdc5",
			128, SPM_P1_DLDO4_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO4_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO9, "LDO_REG9", "dcdc5",
			128, SPM_P1_DLDO5_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO5_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO10, "LDO_REG10", "dcdc5",
			128, SPM_P1_DLDO6_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO6_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	SPM_P1_DESC(SPM_P1_ID_LDO11, "LDO_REG11", "dcdc5",
			128, SPM_P1_DLDO7_VOLT_REG, SPM_P1_DLDO_VSEL_MASK,
			SPM_P1_DLDO7_CTRL_REG, SPM_P1_DLDO_EN_MASK, spm_p1_ldo_ranges),

	/* PWR SWITCH */
	SPM_P1_DESC_SWITCH(SPM_P1_ID_SWITCH1, "SWITCH_REG1", "vcc_sys",
			   SPM_P1_SWITCH_CTRL_REG, SPM_P1_SWTICH_EN_MASK),
};

static struct regulator_match_data spm_p1_regulator_match_data = {
	.nr_desc = ARRAY_SIZE(spm_p1_reg),
	.desc = spm_p1_reg,
	.name = "spm_p1",
	.sleep_reg_offset = SPM_P1_SLEEP_REG_OFFSET,
};

static const struct of_device_id spm_p1_regulator_of_match[] = {
	{ .compatible = "spacemit,p1,regulator", .data = (void *)&spm_p1_regulator_match_data },
	{ },
};
MODULE_DEVICE_TABLE(of, spm_p1_regulator_of_match);

static int spm_p1_regulator_probe(struct platform_device *pdev)
{
	struct regulator_config config = {};
	struct spacemit_pmic *pmic = dev_get_drvdata(pdev->dev.parent);
	struct i2c_client *client;
	const struct of_device_id *of_id;
	struct regulator_dev *regulator_dev;
	int i;

	of_id = of_match_device(spm_p1_regulator_of_match, &pdev->dev);
	if (!of_id) {
		pr_err("Unable to match OF ID\n");
		return -ENODEV;
	}

	match_data = (struct regulator_match_data *)of_id->data;

	client = pmic->i2c;
	config.dev = &client->dev;
	config.regmap = pmic->regmap;

	for (i = 0; i < match_data->nr_desc; ++i) {
		regulator_dev = devm_regulator_register(&pdev->dev,
				match_data->desc + i, &config);
		if (IS_ERR(regulator_dev)) {
			pr_err("failed to register %d regulator\n", i);
			return PTR_ERR(regulator_dev);
		}
	}

	return 0;
}

static struct platform_driver spm_p1_regulator_driver = {
	.probe = spm_p1_regulator_probe,
	.driver = {
		.name = "spm-p1-regulator",
		.of_match_table = spm_p1_regulator_of_match,
	},
};

static int spm_p1_regulator_init(void)
{
	return platform_driver_register(&spm_p1_regulator_driver);
}
subsys_initcall(spm_p1_regulator_init);

MODULE_DESCRIPTION("regulator drivers for the Spacemit series PMICs");
MODULE_LICENSE("GPL");
