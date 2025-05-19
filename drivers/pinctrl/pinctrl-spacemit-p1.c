// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Pinctrl driver for Spacemit P1
 *
 * Copyright (c) 2023, Spacemit Co., Ltd
 */

#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/machine.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinconf.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/gpio/driver.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/mfd/spacemit_p1.h>

#include "core.h"
#include "pinctrl-utils.h"
#include "pinmux.h"

struct pin_config_desc {
	unsigned int pin_id;
	/* input config desc */
	struct {
		unsigned char reg;
		unsigned char msk;
	} input;

	/* output config desc */
	struct {
		unsigned char reg;
		unsigned char msk;
	} output;

	/* pull-down desc */
	struct {
		unsigned char reg;
		unsigned char msk;
	} pup;

	/* deb */
	struct {
		unsigned char reg;
		unsigned char timemsk;

		struct {
			unsigned char msk;
		} en;
	} deb;

	/* OD */
	struct {
		unsigned char reg;
		unsigned char msk;
	} od;

	struct {
		unsigned char reg;
		unsigned char msk;
	} itype;
};

/* pinctrl */
struct pin_func_desc {
	const char *name;
	unsigned char pin_id;
	unsigned char func_reg;
	unsigned char func_mask;
	unsigned char en_val;
	unsigned char ha_sub;
	unsigned char sub_reg;
	unsigned char sub_mask;
	unsigned char sube_val;
};

/* pinctrl: match data */
struct pinctrl_match_data {
	int nr_pin_mux;
	const char **pinmux_funcs;
	int nr_pin_fuc_desc;
	const struct pin_func_desc *pinfunc_desc;
	int nr_pin_conf_desc;
	const struct pin_config_desc *pinconf_desc;
	const char *name;
};

struct spm_p1_pctl {
	struct gpio_chip chip;
	struct regmap *regmap;
	struct pinctrl_dev *pctldev;
	struct device *dev;
	struct pinctrl_desc pinctrl_desc;
	int funcdesc_nums;
	int confdesc_nums;
	const struct pin_func_desc *func_desc;
	const struct pin_config_desc *config_desc;
	const char *name;
};

/* gpio set */
const char *spm_p1_pinmux_functions[] = {
	"gpioin", "gpioout", "exten", "pwrctrl",
	"sleep", "nreset", "adcin"
};

#define SPM8XX_DESC_PIN_FUNC_COM(_pin_id, _match, _ereg, _emask,	\
	_enval,	_hsub, _subreg, _submask, _subenval			\
	)								\
	{								\
		.name		= (_match),				\
		.pin_id		= (_pin_id),				\
		.func_reg	= (_ereg),				\
		.func_mask	= (_emask),				\
		.en_val		= (_enval),				\
		.ha_sub		= (_hsub),				\
		.sub_reg	= (_subreg),				\
		.sub_mask	= (_submask),				\
		.sube_val	= (_subenval),				\
	}

static const struct pin_func_desc spm_p1_pinfunc_desc[] = {
	/* PIN0 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(0, "gpioin", 0x8, 0x3, 0, 0, 0, 0, 0),
	/* PIN0 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(0, "gpioout", 0x8, 0x3, 1, 0, 0, 0, 0),
	/* PIN0 exten */
	SPM8XX_DESC_PIN_FUNC_COM(0, "exten", 0x8, 0x3, 0x3, 1, 0xa, 0x7, 0x0),
	/* PIN0 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(0, "pwrctrl", 0x8, 0x3, 0x3, 1, 0xa, 0x7, 0x1),
	/* PIN0 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(0, "sleep", 0x8, 0x3, 0x3, 1, 0xa, 0x7, 0x2),
	/* PIN0 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(0, "nreset", 0x8, 0x3, 0x3, 1, 0xa, 0x7, 0x3),
	/* PIN0 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(0, "adcin", 0x8, 0x3, 0x3, 1, 0xa, 0x7, 0x4),
	/* PIN1 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(1, "gpioin", 0x8, 0xc, 0, 0, 0, 0, 0),
	/* PIN1 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(1, "gpioout", 0x8, 0xc, 1, 0, 0, 0, 0),
	/* PIN1 exten */
	SPM8XX_DESC_PIN_FUNC_COM(1, "exten", 0x8, 0xc, 0x3, 1, 0xa, 0x38, 0x0),
	/* PIN1 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(1, "pwrctrl", 0x8, 0xc, 0x3, 1, 0xa, 0x38, 0x1),
	/* PIN1 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(1, "sleep", 0x8, 0xc, 0x3, 1, 0xa, 0x38, 0x2),
	/* PIN1 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(1, "nreset", 0x8, 0xc, 0x3, 1, 0xa, 0x38, 0x3),
	/* PIN1 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(1, "adcin", 0x8, 0xc, 0x3, 1, 0xa, 0x38, 0x4),
	/* PIN2 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(2, "gpioin", 0x8, 0x30, 0, 0, 0, 0, 0),
	/* PIN2 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(2, "gpioout", 0x8, 0x30, 1, 0, 0, 0, 0),
	/* PIN2 exten */
	SPM8XX_DESC_PIN_FUNC_COM(2, "exten", 0x8, 0x30, 0x3, 1, 0xb, 0x7, 0x0),
	/* PIN2 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(2, "pwrctrl", 0x8, 0x30, 0x3, 1, 0xb, 0x7, 0x1),
	/* PIN2 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(2, "sleep", 0x8, 0x30, 0x3, 1, 0xb, 0x7, 0x2),
	/* PIN2 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(2, "nreset", 0x8, 0x30, 0x3, 1, 0xb, 0x7, 0x3),
	/* PIN2 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(2, "adcin", 0x8, 0x30, 0x3, 1, 0xb, 0x7, 0x4),
	/* PIN3 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(3, "gpioin", 0x9, 0x3, 0, 0, 0, 0, 0),
	/* PIN3 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(3, "gpioout", 0x9, 0x3, 1, 0, 0, 0, 0),
	/* PIN3 exten */
	SPM8XX_DESC_PIN_FUNC_COM(3, "exten", 0x9, 0x3, 0x3, 1, 0xb, 0x38, 0x0),
	/* PIN3 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(3, "pwrctrl", 0x9, 0x3, 0x3, 1, 0xb, 0x38, 0x1),
	/* PIN3 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(3, "sleep", 0x9, 0x3, 0x3, 1, 0xb, 0x38, 0x2),
	/* PIN3 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(3, "nreset", 0x9, 0x3, 0x3, 1, 0xb, 0x38, 0x3),
	/* PIN3 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(3, "adcin", 0x9, 0x3, 0x3, 1, 0xb, 0x38, 0x4),
	/* PIN4 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(4, "gpioin", 0x9, 0xc, 0, 0, 0, 0, 0),
	/* PIN4 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(4, "gpioout", 0x9, 0xc, 1, 0, 0, 0, 0),
	/* PIN4 exten */
	SPM8XX_DESC_PIN_FUNC_COM(4, "exten", 0x9, 0xc, 0x3, 1, 0xc, 0x7, 0x0),
	/* PIN4 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(4, "pwrctrl", 0x9, 0xc, 0x3, 1, 0xc, 0x7, 0x1),
	/* PIN4 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(4, "sleep", 0x9, 0xc, 0x3, 1, 0xc, 0x7, 0x2),
	/* PIN4 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(4, "nreset", 0x9, 0xc, 0x3, 1, 0xc, 0x7, 0x3),
	/* PIN4 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(4, "adcin", 0x9, 0xc, 0x3, 1, 0xc, 0x7, 0x4),
	/* PIN5 gpioin */
	SPM8XX_DESC_PIN_FUNC_COM(5, "gpioin", 0x9, 0x30, 0, 0, 0, 0, 0),
	/* PIN5 gpioout*/
	SPM8XX_DESC_PIN_FUNC_COM(5, "gpioout", 0x9, 0x30, 1, 0, 0, 0, 0),
	/* PIN5 exten */
	SPM8XX_DESC_PIN_FUNC_COM(5, "exten", 0x9, 0x30, 0x3, 1, 0xc, 0x38, 0x0),
	/* PIN5 pwrctrl */
	SPM8XX_DESC_PIN_FUNC_COM(5, "pwrctrl", 0x9, 0x30, 0x3, 1, 0xc, 0x38, 0x1),
	/* PIN5 sleep */
	SPM8XX_DESC_PIN_FUNC_COM(5, "sleep", 0x9, 0x30, 0x3, 1, 0xc, 0x38, 0x2),
	/* PIN5 nreset */
	SPM8XX_DESC_PIN_FUNC_COM(5, "nreset", 0x9, 0x30, 0x3, 1, 0xc, 0x38, 0x3),
	/* PIN5 adcin */
	SPM8XX_DESC_PIN_FUNC_COM(5, "adcin", 0x9, 0x30, 0x3, 1, 0xc, 0x38, 0x4),
};

#define SPM8XX_DESC_PIN_CONFIG_COM(_pin_id, _ireg, _imsk, _oreg, _omsk,		\
	_pureg, _pumsk, _debreg, _debtmsk, _debemsk, _odreg, _odmsk,		\
	_itypereg, _itypemsk							\
	)							\
	{							\
		.pin_id = (_pin_id),				\
		.input = {					\
			.reg = (_ireg),				\
			.msk = (_imsk),				\
		},						\
		.output = {					\
			.reg = (_oreg),				\
			.msk = (_omsk),				\
		},						\
		.pup = {					\
			.reg = (_pureg),			\
			.msk = (_pumsk),			\
		},						\
		.deb = {					\
			.reg = (_debreg),			\
			.timemsk = (_debtmsk),			\
			.en.msk = (_debemsk)			\
		},						\
		.od = {						\
			.reg = (_odreg),			\
			.msk = (_odmsk),			\
		},						\
		.itype = {					\
			.reg = (_itypereg),			\
			.msk = (_itypemsk),			\
		},						\
	}

static const struct pin_config_desc spm_p1_pinconfig_desc[] = {
	SPM8XX_DESC_PIN_CONFIG_COM(0, 0x0, 0x1, 0x1, 0x1, 0x2, 0x3, 0x4,
				   0xc0, 0x1, 0x5, 0x1, 0x6, 0x3),
	SPM8XX_DESC_PIN_CONFIG_COM(1, 0x0, 0x2, 0x1, 0x2, 0x2, 0xC, 0x4,
				   0xc0, 0x2, 0x5, 0x2, 0x6, 0xC),
	SPM8XX_DESC_PIN_CONFIG_COM(2, 0x0, 0x4, 0x1, 0x4, 0x2, 0x30, 0x4,
				   0xc0, 0x4, 0x5, 0x4, 0x6, 0x30),
	SPM8XX_DESC_PIN_CONFIG_COM(3, 0x0, 0x8, 0x1, 0x8, 0x3, 0x3, 0x4,
				   0xc0, 0x8, 0x5, 0x8, 0x7, 0x3),
	SPM8XX_DESC_PIN_CONFIG_COM(4, 0x0, 0x10, 0x1, 0x10, 0x3, 0xc, 0x4,
				   0xc0, 0x10, 0x5, 0x10, 0x7, 0xc),
	SPM8XX_DESC_PIN_CONFIG_COM(5, 0x0, 0x20, 0x1, 0x20, 0x3, 0x30, 0x4,
				   0xc0, 0x20, 0x5, 0x20, 0x7, 0x30),
};

static struct pinctrl_match_data spm_p1_pinctrl_match_data = {
	.nr_pin_mux = ARRAY_SIZE(spm_p1_pinmux_functions),
	.pinmux_funcs = spm_p1_pinmux_functions,
	.nr_pin_fuc_desc = ARRAY_SIZE(spm_p1_pinfunc_desc),
	.pinfunc_desc = spm_p1_pinfunc_desc,
	.nr_pin_conf_desc = ARRAY_SIZE(spm_p1_pinconfig_desc),
	.pinconf_desc = spm_p1_pinconfig_desc,
	.name = "spm_p1",
};

static const struct pinctrl_ops spm_p1_gpio_pinctrl_ops = {
	.get_groups_count = pinctrl_generic_get_group_count,
	.get_group_name = pinctrl_generic_get_group_name,
	.get_group_pins = pinctrl_generic_get_group_pins,
	.dt_node_to_map = pinconf_generic_dt_node_to_map_group,
	.dt_free_map = pinconf_generic_dt_free_map,
};

static int spm_p1_gpio_pinmux_set(struct pinctrl_dev *pctldev, unsigned int function,
				  unsigned int group)
{
	int i, ret;
	struct spm_p1_pctl *pctl = pinctrl_dev_get_drvdata(pctldev);
	const char *funcname = pinmux_generic_get_function_name(pctldev, function);

	/* get the target desc */
	for (i = 0; i < pctl->funcdesc_nums; ++i) {
		if (strcmp(funcname, pctl->func_desc[i].name) == 0 && group ==
				pctl->func_desc[i].pin_id) {
			/* set the first */
			ret = regmap_update_bits(pctl->regmap,
					pctl->func_desc[i].func_reg,
					pctl->func_desc[i].func_mask,
					pctl->func_desc[i].en_val
					<< (ffs(pctl->func_desc[i].func_mask) - 1));
			if (ret) {
				dev_err(pctl->dev, "set PIN%d, function:%s, failed\n",
					group, funcname);
				return ret;
			}

			/* set the next if it have */
			if (pctl->func_desc[i].ha_sub) {
				ret = regmap_update_bits(pctl->regmap,
					pctl->func_desc[i].sub_reg,
					pctl->func_desc[i].sub_mask,
					pctl->func_desc[i].sube_val
					<< (ffs(pctl->func_desc[i].sub_mask) - 1));
				if (ret) {
					dev_err(pctl->dev,
						"set PIN%d, function:%s, failed\n",
						group, funcname);
					return ret;
				}
			}

			break;
		}
	}

	if (i >= pctl->funcdesc_nums) {
		dev_err(pctl->dev, "Unsupported PIN%d, function:%s\n", group, funcname);
		return -EINVAL;
	}

	return 0;
}

static int spm_p1_pmx_gpio_set_direction(struct pinctrl_dev *pctldev,
					 struct pinctrl_gpio_range *range,
					 unsigned int offset, bool input)
{
	int ret;
	struct spm_p1_pctl *pctl = pinctrl_dev_get_drvdata(pctldev);

	if (strcmp(pctl->name, "spm_p1") == 0)
		/* when input == true, it means that we should set this pin
		 * as gpioin, so we should pass function(0) to set_mux
		 */
		ret = spm_p1_gpio_pinmux_set(pctldev, !input, offset);
	else
		return -EINVAL;

	return ret;
}

static const struct pinmux_ops spm_p1_gpio_pinmux_ops = {
	.get_functions_count = pinmux_generic_get_function_count,
	.get_function_name = pinmux_generic_get_function_name,
	.get_function_groups = pinmux_generic_get_function_groups,
	.set_mux = spm_p1_gpio_pinmux_set,
	.gpio_set_direction = spm_p1_pmx_gpio_set_direction,
	.strict = true,
};

static int spm_p1_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	int ret;
	unsigned int val;
	struct spm_p1_pctl *pctl = gpiochip_get_data(chip);

	ret = regmap_read(pctl->regmap, pctl->config_desc[offset].input.reg, &val);
	if (ret) {
		dev_err(pctl->dev, "get PIN%d, direction failed\n", offset);
		return ret;
	}

	val = val & pctl->config_desc[offset].input.msk;
	val >>= ffs(pctl->config_desc[offset].input.msk) - 1;

	return val;
}

static int spm_p1_gpio_get_direction(struct gpio_chip *chip,
				     unsigned int offset)
{
	int i, ret;
	unsigned int val, direction = 0;
	struct spm_p1_pctl *pctl = gpiochip_get_data(chip);

	/* read the function set register */
	for (i = 0; i < pctl->funcdesc_nums; ++i) {
		if (offset == pctl->func_desc[i].pin_id) {
			ret = regmap_read(pctl->regmap, pctl->func_desc[i].func_reg, &val);
			if (ret) {
				dev_err(pctl->dev, "get PIN%d, direction failed\n", offset);
				return ret;
			}

			direction = val & pctl->func_desc[i].func_mask;
			direction >>= ffs(pctl->func_desc[i].func_mask) - 1;

			break;
		}
	}

	if (strcmp(pctl->name, "spm_p1") == 0)
		return !direction;
	else
		return -EINVAL;
}

static void spm_p1_gpio_set(struct gpio_chip *chip, unsigned int offset,
			    int value)
{
	int ret;
	struct spm_p1_pctl *pctl = gpiochip_get_data(chip);

	ret = regmap_update_bits(pctl->regmap,
			pctl->config_desc[offset].output.reg,
			pctl->config_desc[offset].output.msk,
			value ? pctl->config_desc[offset].output.msk : 0);
	if (ret)
		dev_err(pctl->dev, "set PIN%d, val:%d, failed\n", offset, value);
}

static int spm_p1_gpio_input(struct gpio_chip *chip, unsigned int offset)
{
	/* set the gpio input */
	return pinctrl_gpio_direction_input(chip->base + offset);
}

static int spm_p1_gpio_output(struct gpio_chip *chip, unsigned int offset,
			      int value)
{
	/* set the gpio output */
	return pinctrl_gpio_direction_input(chip->base + offset);
}

static int spm_p1_pin_pconf_get(struct pinctrl_dev *pctldev, unsigned int pin,
			       unsigned long *config)
{
	/* Do nothing by now */
	return 0;
}

static int spm_p1_pin_pconf_set(struct pinctrl_dev *pctldev, unsigned int pin,
			       unsigned long *configs, unsigned int num_configs)
{
	unsigned int reg, msk, ret;
	struct spm_p1_pctl *pctl = pinctrl_dev_get_drvdata(pctldev);

	while (num_configs) {
		switch (pinconf_to_config_param(*configs)) {
		case PIN_CONFIG_BIAS_DISABLE:
		case PIN_CONFIG_BIAS_PULL_DOWN:
		case PIN_CONFIG_BIAS_PULL_UP:
			reg = pctl->config_desc[pin].pup.reg;
			msk = pctl->config_desc[pin].pup.msk;
			break;
		case PIN_CONFIG_DRIVE_OPEN_DRAIN:
		case PIN_CONFIG_DRIVE_PUSH_PULL:
		case PIN_CONFIG_DRIVE_OPEN_SOURCE:
			reg = pctl->config_desc[pin].od.reg;
			msk = pctl->config_desc[pin].od.msk;
			break;
		case PIN_CONFIG_INPUT_DEBOUNCE:
			reg = pctl->config_desc[pin].deb.reg;
			msk = pctl->config_desc[pin].deb.timemsk;
			break;
		case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
			reg = pctl->config_desc[pin].deb.reg;
			msk = pctl->config_desc[pin].deb.en.msk;
			break;
		case PIN_CONFIG_OUTPUT:
			reg = pctl->config_desc[pin].output.reg;
			msk = pctl->config_desc[pin].output.msk;
			break;
		default:
			return -EOPNOTSUPP;
		}

		ret = regmap_update_bits(pctl->regmap, reg, msk,
				pinconf_to_config_argument(*configs)
				<< (ffs(msk) - 1));
		if (ret) {
			dev_err(pctl->dev, "set reg:%x, msk:%x failed\n", reg, msk);
			return -EINVAL;
		}
		++configs;
		--num_configs;
	}

	return 0;
}

static int spm_p1_pconf_group_set(struct pinctrl_dev *pctldev, unsigned int group,
				 unsigned long *configs, unsigned int num_configs)
{
	return spm_p1_pin_pconf_set(pctldev, group, configs, num_configs);
}

static int spm_p1_pconf_group_get(struct pinctrl_dev *pctldev, unsigned int group,
				  unsigned long *config)
{
	return spm_p1_pin_pconf_get(pctldev, group, config);
}

static const struct pinconf_ops spm_p1_gpio_pinconf_ops = {
	.is_generic = true,
	.pin_config_get = spm_p1_pin_pconf_get,
	.pin_config_set = spm_p1_pin_pconf_set,
	.pin_config_group_get = spm_p1_pconf_group_get,
	.pin_config_group_set = spm_p1_pconf_group_set,
};

static const struct of_device_id spm_p1_pinctrl_of_match[] = {
	{ .compatible = "spacemit,p1,pinctrl", .data = (void *)&spm_p1_pinctrl_match_data },
	{ },
};
MODULE_DEVICE_TABLE(of, spm_p1_pinctrl_of_match);

static int spm_p1_pinctrl_probe(struct platform_device *pdev)
{
	int i, res;
	struct spm_p1_pctl *pctl;
	unsigned int npins;
	const char **pin_names;
	unsigned int *pin_nums;
	struct pinctrl_pin_desc *pins;
	const struct of_device_id *of_id;
	struct spacemit_pmic *pmic = dev_get_drvdata(pdev->dev.parent);
	struct pinctrl_match_data *match_data;

	of_id = of_match_device(spm_p1_pinctrl_of_match, &pdev->dev);
	if (!of_id) {
		pr_err("Unable to match OF ID\n");
		return -ENODEV;
	}

	match_data = (struct pinctrl_match_data *)of_id->data;

	pctl = devm_kzalloc(&pdev->dev, sizeof(*pctl), GFP_KERNEL);
	if (!pctl)
		return -ENOMEM;

	pctl->name = match_data->name;
	pctl->dev = &pdev->dev;
	pctl->regmap = pmic->regmap;
	pctl->func_desc = match_data->pinfunc_desc;
	pctl->funcdesc_nums = match_data->nr_pin_fuc_desc;
	pctl->config_desc = match_data->pinconf_desc;
	pctl->confdesc_nums = match_data->nr_pin_conf_desc;
	dev_set_drvdata(&pdev->dev, pctl);

	if (of_property_read_u32(pdev->dev.of_node, "spm_pmic,npins", &npins))
		return dev_err_probe(&pdev->dev, -EINVAL, "spm_pmic,npins property not found\n");

	pins = devm_kmalloc_array(&pdev->dev, npins, sizeof(pins[0]),
				  GFP_KERNEL);
	pin_names = devm_kmalloc_array(&pdev->dev, npins, sizeof(pin_names[0]),
				       GFP_KERNEL);
	pin_nums = devm_kmalloc_array(&pdev->dev, npins, sizeof(pin_nums[0]),
				      GFP_KERNEL);
	if (!pins || !pin_names || !pin_nums)
		return -ENOMEM;

	for (i = 0; i < npins; i++) {
		pins[i].number = i;
		pins[i].name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "PIN%u", i);
		pins[i].drv_data = pctl;
		pin_names[i] = pins[i].name;
		pin_nums[i] = i;
	}

	pctl->pinctrl_desc.name = dev_name(pctl->dev);
	pctl->pinctrl_desc.pins = pins;
	pctl->pinctrl_desc.npins = npins;
	pctl->pinctrl_desc.pctlops = &spm_p1_gpio_pinctrl_ops;
	pctl->pinctrl_desc.pmxops = &spm_p1_gpio_pinmux_ops;
	pctl->pinctrl_desc.confops = &spm_p1_gpio_pinconf_ops;

	pctl->pctldev =	devm_pinctrl_register(&pdev->dev, &pctl->pinctrl_desc, pctl);
	if (IS_ERR(pctl->pctldev))
		return dev_err_probe(&pdev->dev, PTR_ERR(pctl->pctldev),
				     "Failed to register pinctrl device.\n");

	for (i = 0; i < npins; i++) {
		res = pinctrl_generic_add_group(pctl->pctldev, pins[i].name,
						pin_nums + i, 1, pctl);
		if (res < 0)
			return dev_err_probe(pctl->dev, res,
					     "Failed to register group");
	}

	for (i = 0; i < match_data->nr_pin_mux; ++i) {
		res = pinmux_generic_add_function(pctl->pctldev, match_data->pinmux_funcs[i],
						  pin_names, npins, pctl);
		if (res < 0)
			return dev_err_probe(pctl->dev, res, "Failed to register function.");
	}

	pctl->chip.base = -1;
	pctl->chip.can_sleep = true;
	pctl->chip.request = gpiochip_generic_request;
	pctl->chip.free = gpiochip_generic_free;
	pctl->chip.parent = &pdev->dev;
	pctl->chip.label = dev_name(&pdev->dev);
	pctl->chip.owner = THIS_MODULE;
	pctl->chip.get = spm_p1_gpio_get;
	pctl->chip.get_direction = spm_p1_gpio_get_direction;
	pctl->chip.set = spm_p1_gpio_set;
	pctl->chip.direction_input = spm_p1_gpio_input;
	pctl->chip.direction_output = spm_p1_gpio_output;

	pctl->chip.ngpio = pctl->pinctrl_desc.npins;

	res = devm_gpiochip_add_data(&pdev->dev, &pctl->chip, pctl);
	if (res) {
		dev_err(&pdev->dev, "Failed to register GPIO chip\n");
		return res;
	}

	return 0;
}

static struct platform_driver spm_p1_pinctrl_driver = {
	.probe = spm_p1_pinctrl_probe,
	.driver = {
		.name = "spm-p1-pinctrl",
		.of_match_table = spm_p1_pinctrl_of_match,
	},
};
module_platform_driver(spm_p1_pinctrl_driver);

MODULE_DESCRIPTION("Spacemit P1 pinctrl driver");
MODULE_LICENSE("GPL");
