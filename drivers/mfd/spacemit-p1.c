// SPDX-License-Identifier: GPL-2.0-only
/*
 * mfd driver for Spacemit P1
 *
 * Copyright (c) 2023, Spacemit Co., Ltd
 */

#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/mfd/core.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/reboot.h>
#include <linux/ioport.h>
#include <linux/pm_wakeirq.h>
#include <linux/mfd/spacemit_p1.h>

struct mfd_match_data {
	const struct regmap_config *regmap_cfg;
	const struct regmap_irq_chip *regmap_irq_chip;
	const struct mfd_cell *mfd_cells;
	int nr_cells;
	const char *name;
	void *ptr;

	/* shutdown - reboot support */
	struct {
		unsigned char reg;
		unsigned char bit;
	} shutdown;

	struct {
		unsigned int reg;
		unsigned char bit;
	} reboot;

	/* value will be kept in register while reset pmic  */
	struct {
		unsigned int reg;
		unsigned char bit;
	} non_reset;
};

static const struct regmap_config spm_p1_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = SPM_P1_MAX_REG,
	.cache_type = REGCACHE_RBTREE,
};

static const struct regmap_irq spm_p1_irqs[] = {
	[SPM_P1_E_GPI0] = {
		.mask = SPM_P1_E_GPI0_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_GPI1] = {
		.mask = SPM_P1_E_GPI1_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_GPI2] = {
		.mask = SPM_P1_E_GPI2_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_GPI3] = {
		.mask = SPM_P1_E_GPI3_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_GPI4] = {
		.mask = SPM_P1_E_GPI4_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_GPI5] = {
		.mask = SPM_P1_E_GPI5_MSK,
		.reg_offset = 0,
	},

	[SPM_P1_E_ADC_TEMP] = {
		.mask = SPM_P1_E_ADC_TEMP_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_ADC_EOC] = {
		.mask = SPM_P1_E_ADC_EOC_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_ADC_EOS] = {
		.mask = SPM_P1_E_ADC_EOS_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_WDT_TO] = {
		.mask = SPM_P1_E_WDT_TO_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_ALARM] = {
		.mask = SPM_P1_E_ALARM_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_TICK] = {
		.mask = SPM_P1_E_TICK_MSK,
		.reg_offset = 1,
	},

	[SPM_P1_E_LDO_OV] = {
		.mask = SPM_P1_E_LDO_OV_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_LDO_UV] = {
		.mask = SPM_P1_E_LDO_UV_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_LDO_SC] = {
		.mask = SPM_P1_E_LDO_SC_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_SW_SC] = {
		.mask = SPM_P1_E_SW_SC_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_TEMP_WARN] = {
		.mask = SPM_P1_E_TEMP_WARN_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_TEMP_SEVERE] = {
		.mask = SPM_P1_E_TEMP_SEVERE_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_TEMP_CRIT] = {
		.mask = SPM_P1_E_TEMP_CRIT_MSK,
		.reg_offset = 2,
	},

	[SPM_P1_E_BUCK1_OV] = {
		.mask = SPM_P1_E_BUCK1_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK2_OV] = {
		.mask = SPM_P1_E_BUCK2_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK3_OV] = {
		.mask = SPM_P1_E_BUCK3_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK4_OV] = {
		.mask = SPM_P1_E_BUCK4_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK5_OV] = {
		.mask = SPM_P1_E_BUCK5_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK6_OV] = {
		.mask = SPM_P1_E_BUCK6_OV_MSK,
		.reg_offset = 3,
	},

	[SPM_P1_E_BUCK1_UV] = {
		.mask = SPM_P1_E_BUCK1_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK2_UV] = {
		.mask = SPM_P1_E_BUCK2_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK3_UV] = {
		.mask = SPM_P1_E_BUCK3_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK4_UV] = {
		.mask = SPM_P1_E_BUCK4_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK5_UV] = {
		.mask = SPM_P1_E_BUCK5_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK6_UV] = {
		.mask = SPM_P1_E_BUCK6_UV_MSK,
		.reg_offset = 4,
	},

	[SPM_P1_E_BUCK1_SC] = {
		.mask = SPM_P1_E_BUCK1_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_BUCK2_SC] = {
		.mask = SPM_P1_E_BUCK2_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_BUCK3_SC] = {
		.mask = SPM_P1_E_BUCK3_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_BUCK4_SC] = {
		.mask = SPM_P1_E_BUCK4_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_BUCK5_SC] = {
		.mask = SPM_P1_E_BUCK5_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_BUCK6_SC] = {
		.mask = SPM_P1_E_BUCK6_SC_MSK,
		.reg_offset = 5,
	},

	[SPM_P1_E_PWRON_RINTR] = {
		.mask = SPM_P1_E_PWRON_RINTR_MSK,
		.reg_offset = 6,
	},

	[SPM_P1_E_PWRON_FINTR] = {
		.mask = SPM_P1_E_PWRON_FINTR_MSK,
		.reg_offset = 6,
	},

	[SPM_P1_E_PWRON_SINTR] = {
		.mask = SPM_P1_E_PWRON_SINTR_MSK,
		.reg_offset = 6,
	},

	[SPM_P1_E_PWRON_LINTR] = {
		.mask = SPM_P1_E_PWRON_LINTR_MSK,
		.reg_offset = 6,
	},

	[SPM_P1_E_PWRON_SDINTR] = {
		.mask = SPM_P1_E_PWRON_SDINTR_MSK,
		.reg_offset = 6,
	},

	[SPM_P1_E_VSYS_OV] = {
		.mask = SPM_P1_E_VSYS_OV_MSK,
		.reg_offset = 6,
	},
};

static const struct regmap_irq_chip spm_p1_irq_chip = {
	.name = "spm_p1",
	.irqs = spm_p1_irqs,
	.num_irqs = ARRAY_SIZE(spm_p1_irqs),
	.num_regs = 7,
	.status_base = SPM_P1_E_STATUS_REG_BASE,
	.mask_base = SPM_P1_E_EN_REG_BASE,
	.unmask_base = SPM_P1_E_EN_REG_BASE,
	.ack_base = SPM_P1_E_STATUS_REG_BASE,
	.init_ack_masked = true,
	.mask_unmask_non_inverted = true,
};

/* power-key desc */
static const struct resource spm_p1_pwrkey_resources[] = {
	DEFINE_RES_IRQ(SPM_P1_E_PWRON_RINTR),
	DEFINE_RES_IRQ(SPM_P1_E_PWRON_FINTR),
	DEFINE_RES_IRQ(SPM_P1_E_PWRON_SINTR),
	DEFINE_RES_IRQ(SPM_P1_E_PWRON_LINTR),
};

/* rtc desc */
static const struct resource spm_p1_rtc_resources[] = {
	DEFINE_RES_IRQ(SPM_P1_E_ALARM),
};

/* adc desc */
static const struct resource spm_p1_adc_resources[] = {
	DEFINE_RES_IRQ(SPM_P1_E_ADC_EOC),
};

/* mfd configuration */
static const struct mfd_cell spm_p1[] = {
	{
		.name = "spm-p1-regulator",
		.of_compatible = "spacemit,p1,regulator",
	},
	{
		.name = "spm-p1-pinctrl",
		.of_compatible = "spacemit,p1,pinctrl",
	},
	{
		.name = "spm-p1-pwrkey",
		.of_compatible = "spacemit,p1,pwrkey",
		.num_resources = ARRAY_SIZE(spm_p1_pwrkey_resources),
		.resources = &spm_p1_pwrkey_resources[0],
	},
	{
		.name = "spm-p1-rtc",
		.of_compatible = "spacemit,p1,rtc",
		.num_resources = ARRAY_SIZE(spm_p1_rtc_resources),
		.resources = &spm_p1_rtc_resources[0],
	},
	{
		.name = "spm-p1-adc",
		.of_compatible = "spacemit,p1,adc",
		.num_resources = ARRAY_SIZE(spm_p1_adc_resources),
		.resources = &spm_p1_adc_resources[0],
	},
};

static struct mfd_match_data spm_p1_mfd_match_data = {
	.regmap_cfg = &spm_p1_regmap_config,
	.regmap_irq_chip = &spm_p1_irq_chip,
	.mfd_cells = spm_p1,
	.nr_cells = ARRAY_SIZE(spm_p1),
	.name = "spm_p1",
	.shutdown = {
		.reg = SPM_P1_PWR_CTRL2,
		.bit = SPM_P1_SW_SHUTDOWN_BIT_MSK,
	},
	.reboot = {
		.reg = SPM_P1_PWR_CTRL2,
		.bit = SPM_P1_SW_RESET_BIT_MSK,
	},
	.non_reset = {
		.reg = SPM_P1_NON_RESET_REG,
		.bit = SPM_P1_RESTART_CFG_BIT_MSK,
	},
};

struct mfd_match_data *match_data;

static void spm_p1_power_off(void)
{
	struct spacemit_pmic *pmic = (struct spacemit_pmic *)match_data->ptr;

	regmap_update_bits(pmic->regmap, match_data->shutdown.reg, match_data->shutdown.bit,
			   match_data->shutdown.bit);

	/* Wait for poweroff */
	for (;;)
		cpu_relax();
}

static int spm_p1_restart_notify(struct notifier_block *this, unsigned long mode, void *cmd)
{
	struct spacemit_pmic *pmic = (struct spacemit_pmic *)match_data->ptr;

	regmap_update_bits(pmic->regmap, match_data->reboot.reg, match_data->reboot.bit,
			   match_data->reboot.bit);

	return NOTIFY_DONE;
}

static struct notifier_block spm_p1_restart_handler = {
	.notifier_call = spm_p1_restart_notify,
	.priority = 0,
};

static int spm_p1_probe(struct i2c_client *client)
{
	int ret;
	int nr_cells;
	struct device_node *np;
	struct spacemit_pmic *pmic;
	const struct mfd_cell *cells;
	const struct of_device_id *of_id;

	pmic = devm_kzalloc(&client->dev, sizeof(*pmic), GFP_KERNEL);
	if (!pmic)
		return -ENOMEM;

	of_id = of_match_device(client->dev.driver->of_match_table, &client->dev);
	if (!of_id) {
		pr_err("Unable to match OF ID\n");
		return -ENODEV;
	}

	/* find the property in device node */
	np = of_find_compatible_node(NULL, NULL, of_id->compatible);
	if (!np)
		return 0;

	of_node_put(np);

	match_data = (struct mfd_match_data *)of_id->data;
	match_data->ptr = (void *)pmic;

	pmic->regmap_cfg = match_data->regmap_cfg;
	pmic->regmap_irq_chip = match_data->regmap_irq_chip;
	cells = match_data->mfd_cells;
	nr_cells = match_data->nr_cells;

	pmic->i2c = client;

	i2c_set_clientdata(client, pmic);

	pmic->regmap = devm_regmap_init_i2c(client, pmic->regmap_cfg);
	if (IS_ERR(pmic->regmap)) {
		pr_err("%s:%d, regmap initialization failed\n", __func__, __LINE__);
		return PTR_ERR(pmic->regmap);
	}

	regcache_cache_bypass(pmic->regmap, true);

	if (!client->irq)
		pr_warn("%s:%d, No interrupt supported\n", __func__, __LINE__);
	else {
		if (pmic->regmap_irq_chip) {
			ret = regmap_add_irq_chip(pmic->regmap, client->irq, IRQF_ONESHOT, -1,
						  pmic->regmap_irq_chip, &pmic->irq_data);
			if (ret) {
				pr_err("failed to add irqchip %d\n", ret);
				return ret;
			}
		}

		dev_pm_set_wake_irq(&client->dev, client->irq);
		device_init_wakeup(&client->dev, true);
	}

	ret = devm_mfd_add_devices(&client->dev, PLATFORM_DEVID_NONE, cells, nr_cells, NULL, 0,
				   regmap_irq_get_domain(pmic->irq_data));
	if (ret) {
		pr_err("failed to add MFD devices %d\n", ret);
		return -EINVAL;
	}

	if (match_data->shutdown.reg)
		pm_power_off = spm_p1_power_off;

	if (match_data->reboot.reg) {
		ret = register_restart_handler(&spm_p1_restart_handler);
		if (ret)
			pr_warn("failed to register rst handler, %d\n", ret);
	}

	return 0;
}

static const struct of_device_id spm_p1_of_match[] = {
	{ .compatible = "spacemit,p1", .data = (void *)&spm_p1_mfd_match_data },
	{ },
};
MODULE_DEVICE_TABLE(of, spm_p1_of_match);

static struct i2c_driver spm_p1_i2c_driver = {
	.driver = {
		.name = "spm-p1",
		.of_match_table = spm_p1_of_match,
	},
	.probe    = spm_p1_probe,
};

static int spacemit_mfd_init(void)
{
	return i2c_add_driver(&spm_p1_i2c_driver);
}
subsys_initcall(spacemit_mfd_init);

MODULE_DESCRIPTION("Spacemit P1 mfd driver");
MODULE_LICENSE("GPL");
