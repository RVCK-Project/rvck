// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Zhihe Group Holding Limited.
 */

#include <linux/debugfs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/regulator/machine.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/firmware/zhihe/a210-ipc.h>

#define MBOX_MAX_MSG_LEN	28

/*
 * struct rpc_msg_regu_vol_set - regulator voltage set request descriptor
 *
 * @regu_id: Virtual regulator id
 * @is_daul_rail: Specify if this virtual id contains dual rails
 * @dc1: Voltage uint in uv for single rail or the first rail
 * @dc2: Voltage uint in uv for the second rail, ignore it if it's not daul rail
 */
struct rpc_msg_regu_vol_set {
	u16 regu_id;
	u16 is_dual_rail;
	u32 dc1;
	u32 dc2;
	u16 reserved[6];
} __packed __aligned(1);

/*
 * struct rpc_msg_regu_vol_get - regulator voltage get request descriptor
 *
 * @regu_id: Virtual regulator id
 * @is_daul_rail: Specify if this virtual id contains dual rails
 * @dc1: Voltage uint in uv for single rail or the first rail
 * @dc2: Voltage uint in uv for the second rail, ignore it if it's not daul rail
 */
struct rpc_msg_regu_vol_get {
	u16 regu_id;
	u16 is_dual_rail;
	u32 dc1;
	u32 dc2;
	u16 reserved[6];
} __packed __aligned(1);

/*
 * struct rpc_msg_regu_vol_get_ack - regulator voltage get resp descriptor
 *
 * Dedicated for rpc rx message with transform status and regulator voltage.
 *
 * @th1520_aon_rpc_ack_common: RPC message ack descriptor
 * @regu_id: Virtual regulator id
 * @is_daul_rail: Specify if this virtual id contains dual rails
 * @dc1: Voltage uint in uv for single rail or the first rail
 * @dc2: Voltage uint in uv for the second rail, ignore it if it's not daul rail
 */
struct rpc_msg_regu_vol_get_ack {
	struct th1520_aon_rpc_ack_common ack_hdr;
	u16 regu_id;
	u16 is_dual_rail;
	u32 dc1;
	u32 dc2;
	u16 reserved[6];
} __packed __aligned(1);

/*
 * struct rpc_msg_regu_pwr_set - regulator power status set request descriptor
 *
 * @regu_id: Virtual regulator id
 * @status: Regulator on\off status
 */
struct rpc_msg_regu_pwr_set {
	u16 regu_id;
	u16 status;
	u32 reserved[5];
} __packed __aligned(1);

/*
 * struct rpc_msg_regu_pwr_get - regulator power status get request descriptor
 *
 * @regu_id: Virtual regulator id
 */
struct rpc_msg_regu_pwr_get {
	u16 regu_id;
	u32 reserved[5];
} __packed __aligned(1);

/*
 * struct rpc_msg_regu_pwr_get_ack - regulator power status get resp descriptor
 *
 * @th1520_aon_rpc_ack_common: RPC message ack descriptor
 * @regu_id: Virtual regulator id
 * @status: Regulator on\off status
 */
struct rpc_msg_regu_pwr_get_ack {
	struct th1520_aon_rpc_ack_common ack_hdr;
	u16 regu_id;
	u16 status;
	u32 reserved[5];
} __packed __aligned(1);

struct zhihe_aon_msg_regulator_ctrl {
	struct th1520_aon_rpc_msg_hdr hdr;
	union rpc_func_t {
		struct rpc_msg_regu_vol_set regu_vol_set;
		struct rpc_msg_regu_vol_get regu_vol_get;
		struct rpc_msg_regu_pwr_set regu_pwr_set;
		struct rpc_msg_regu_pwr_get regu_pwr_get;
	} __packed __aligned(1) rpc;
} __packed __aligned(1);

enum a210_pm_resource {
	A210_AVDD33_EMMC,       /* sy70209: ldo1  */
	A210_AVDD33_USB2,       /* sy70209: ldo2  */
	A210_DVDD08_AON,        /* sy70209: ldo3  */
	A210_AVDD18_AON,        /* sy70209: ldo4  */
	A210_AVDD18_EMMC_USB2,  /* sy70209: ldo5  */
	A210_AVDD18_EMMC_PERI,  /* sy70209: ldo7  */
	A210_AVDD18_TOP,        /* sy70209: ldo8  */
	A210_AVDD18_PLL,        /* sy70209: ldo9  */
	A210_AVDD18,            /* sy70209: ldo10 */
	A210_DVDD18_DDR_VAA,    /* sy70209: vout1 */
	A210_P3V3,              /* sy70209: vout2 */
	A210_DVDD08_TOP,        /* sy70209: vout3 */
	A210_DVDD06_DDR_VDDQLP, /* sy70209: vout4 */
	A210_DVDD08_DDR,        /* sy70209: vout5 */
	A210_DVDD_CPU,          /* sy70209: vout6 */
	A210_DVDDM_CPU,         /* sy70209: vout7 */
	A210_DVDD_VP,           /* sy70209: vout8 */
	A210_DVDD_NPU_VIP,      /* sy70300: vout1 */
	A210_DVDD_CPU_P,        /* sy70300: vout2 */
	A210_DVDD_GPU,          /* sy70300: vout3 */
	A210_REGU_MAX
};

struct apcpu_vol_set {
	u32 vdd;               ///< cpu core voltage
	u32 vddm;              ///< cpu core-mem voltage
};

struct aon_regu_desc {
	struct regulator_desc *regu_desc;
	u32                    regu_num;
};

struct aon_regu_info {
	struct device              *dev;
	const struct apcpu_vol_set *cpu_vol;            ///< signed-off voltage of cpu
	u32                        vddm;                ///< cpu-mem voltage
	uint8_t                    cpu_dual_rail_flag;  ///< cpu dual rail flag
	uint8_t                    vddm_dual_rail_flag; ///< cpu-mem dual rail flag
	struct aon_regu_desc       *regu_desc;          ///< regu-desc set
	struct zhihe_aon_ipc       *ipc_handle;         ///< handle of mail-box
};

static struct aon_regu_info zhihe_aon_pmic_info;

/* dc2 is valid when is_dual_rail is true
 *
 * Dual-rail regulator means that a virtual regulator involves two hardware
 * regulators.
 */
static int aon_set_regulator(struct zhihe_aon_ipc *ipc, u16 regu_id, u32 dc)
{
	struct zhihe_aon_msg_regulator_ctrl msg = { 0 };
	struct th1520_aon_rpc_ack_common ack_msg = { 0 };
	struct th1520_aon_rpc_msg_hdr *hdr = &msg.hdr;

	hdr->svc = (uint8_t)TH1520_AON_RPC_SVC_PM;
	hdr->func = (uint8_t)TH1520_AON_PM_FUNC_SET_RESOURCE_REGULATOR;
	hdr->size = TH1520_AON_RPC_MSG_NUM;

	RPC_SET_BE16(&msg.rpc.regu_vol_set.regu_id, 0, regu_id);
	RPC_SET_BE32(&msg.rpc.regu_vol_set.regu_id, 4, dc);

	return zhihe_aon_call_rpc(ipc, &msg, &ack_msg, true);
}

/* dc2 is valid when is_dual_rail is true
 *
 * Dual-rail regulator means that a virtual regulator involves two hardware
 * regulators.
 */
static int aon_get_regulator(struct zhihe_aon_ipc *ipc, u16 regu_id, u32 *dc)
{
	struct zhihe_aon_msg_regulator_ctrl msg = { 0 };
	struct rpc_msg_regu_vol_get_ack ack_msg = { 0 };
	struct th1520_aon_rpc_msg_hdr *hdr = &msg.hdr;
	int ret;

	hdr->svc = (uint8_t)TH1520_AON_RPC_SVC_PM;
	hdr->func = (uint8_t)TH1520_AON_PM_FUNC_GET_RESOURCE_REGULATOR;
	hdr->size = TH1520_AON_RPC_MSG_NUM;

	RPC_SET_BE16(&msg.rpc.regu_vol_get.regu_id, 0, regu_id);

	ret = zhihe_aon_call_rpc(ipc, &msg, &ack_msg, true);
	if (ret)
		return ret;
	/*fix me:set local */
	ack_msg.regu_id = regu_id;

	RPC_GET_BE32(&ack_msg.regu_id, 4, &ack_msg.dc1);

	if (dc != NULL)
		*dc = ack_msg.dc1;

	return 0;
}

static int aon_regu_power_ctrl(struct zhihe_aon_ipc *ipc, u32 regu_id, u16 pwr_on)
{
	struct zhihe_aon_msg_regulator_ctrl msg = { 0 };
	struct th1520_aon_rpc_ack_common ack_msg = { 0 };
	struct th1520_aon_rpc_msg_hdr *hdr = &msg.hdr;

	hdr->svc = (uint8_t)TH1520_AON_RPC_SVC_PM;
	hdr->func = (uint8_t)TH1520_AON_PM_FUNC_PWR_SET;
	hdr->size = TH1520_AON_RPC_MSG_NUM;

	RPC_SET_BE16(&msg.rpc.regu_pwr_set.regu_id, 0, regu_id);
	RPC_SET_BE16(&msg.rpc.regu_pwr_set.regu_id, 2, pwr_on);

	return zhihe_aon_call_rpc(ipc, &msg, &ack_msg, true);
}

static int aon_regu_enable(struct regulator_dev *reg)
{
	u16 regu_id = (u16)rdev_get_id(reg);

	return aon_regu_power_ctrl(zhihe_aon_pmic_info.ipc_handle, regu_id, 1);
}

static int aon_regu_disable(struct regulator_dev *reg)
{
	u16 regu_id = (u16)rdev_get_id(reg);

	return aon_regu_power_ctrl(zhihe_aon_pmic_info.ipc_handle, regu_id, 0);
}

static int aon_regu_is_enabled(struct regulator_dev *reg)
{
	struct zhihe_aon_msg_regulator_ctrl msg = { 0 };
	struct rpc_msg_regu_pwr_get_ack ack_msg = { 0 };
	struct th1520_aon_rpc_msg_hdr *hdr = &msg.hdr;
	u16 regu_id = (u16)rdev_get_id(reg);
	int ret;

	hdr->svc = (uint8_t)TH1520_AON_RPC_SVC_PM;
	hdr->func = (uint8_t)TH1520_AON_PM_FUNC_PWR_GET;
	hdr->size = TH1520_AON_RPC_MSG_NUM;

	RPC_SET_BE16(&msg.rpc.regu_pwr_get.regu_id, 0, regu_id);

	ret = zhihe_aon_call_rpc(zhihe_aon_pmic_info.ipc_handle, &msg, &ack_msg, true);
	if (ret < 0)
		return ret;

	RPC_GET_BE16(&ack_msg.regu_id, 2, &ack_msg.status);
	return (int)ack_msg.status;
}

static int aon_regu_set_voltage(struct regulator_dev *reg, int minuV,
				int max_uV, unsigned int *selector)
{
	const struct regulator_desc *desc = reg->desc;
	u16 regu_id = (u16)rdev_get_id(reg);

	*selector = DIV_ROUND_UP(minuV - desc->min_uV, desc->uV_step);

	return aon_set_regulator(zhihe_aon_pmic_info.ipc_handle, regu_id, minuV);
}

static int aon_regu_get_voltage(struct regulator_dev *reg)
{
	u16 regu_id = (u16)rdev_get_id(reg);
	int voltage, ret;

	ret = aon_get_regulator(zhihe_aon_pmic_info.ipc_handle, regu_id, &voltage);
	if (ret) {
		pr_err("failed to get voltage\n");
		return -EINVAL;
	}

	return voltage;
}

static const struct regulator_ops regu_common_ops = {
	.enable =        aon_regu_enable,
	.disable =       aon_regu_disable,
	.is_enabled =    aon_regu_is_enabled,
	.list_voltage =  regulator_list_voltage_linear,
	.set_voltage =   aon_regu_set_voltage,
	.get_voltage =   aon_regu_get_voltage,
};

/* Macros for voltage DC/DC converters (BUCKs) for cpu */
#define REGU_DSC_DEF(regu_id, of_math_name)       \
	{ .id = regu_id,                          \
	  .name = of_match_ptr(of_math_name),     \
	  .of_match = of_match_ptr(of_math_name), \
	  .type = REGULATOR_VOLTAGE,              \
	  .owner = THIS_MODULE }

#define REGU_DYN_DSC_DEF(regu_id, of_math_name, min, step, max) \
	{ .id = regu_id,                                        \
	  .name = of_match_ptr(of_math_name),                   \
	  .of_match = of_match_ptr(of_math_name),               \
	  .min_uV = (min),                                      \
	  .uV_step = (step),                                    \
	  .n_voltages = ((max) - (min)) / (step) + 1,           \
	  .type = REGULATOR_VOLTAGE,                            \
	  .owner = THIS_MODULE }

static struct regulator_desc zhihe_a210_aon_regu_desc[] = {
	/* Common regulators do not need dynamic voltage adjustment. */
	REGU_DSC_DEF(A210_AVDD33_EMMC, "avdd33_emmc"),
	REGU_DSC_DEF(A210_AVDD33_USB2, "avdd33_usb2"),
	REGU_DSC_DEF(A210_DVDD08_AON, "dvdd08_aon"),
	REGU_DSC_DEF(A210_AVDD18_AON, "avdd18_aon"),
	REGU_DSC_DEF(A210_AVDD18_EMMC_USB2, "avdd18_emmc_usb2"),
	REGU_DSC_DEF(A210_AVDD18_EMMC_PERI, "avdd18_emmc_peri"),
	REGU_DSC_DEF(A210_AVDD18_TOP, "avdd18_top"),
	REGU_DSC_DEF(A210_AVDD18_PLL, "avdd18_pll"),
	REGU_DSC_DEF(A210_AVDD18, "avdd18"),
	REGU_DSC_DEF(A210_DVDD18_DDR_VAA, "dvdd18_ddr_vaa"),
	REGU_DSC_DEF(A210_P3V3, "p3v3"),
	REGU_DSC_DEF(A210_DVDD08_TOP, "dvdd08_top"),
	REGU_DSC_DEF(A210_DVDD06_DDR_VDDQLP, "dvdd06_ddr_vddqlp"),
	REGU_DYN_DSC_DEF(A210_DVDD08_DDR, "dvdd08_ddr", 750000, 5000, 800000),
	REGU_DYN_DSC_DEF(A210_DVDD_CPU, "dvdd_cpu", 600000, 12500, 1000000),
	REGU_DYN_DSC_DEF(A210_DVDDM_CPU, "dvddm_cpu", 800000, 12500, 1000000),
	REGU_DYN_DSC_DEF(A210_DVDD_VP, "dvdd_vp", 750000, 12500, 800000),
	REGU_DYN_DSC_DEF(A210_DVDD_NPU_VIP, "dvdd_npu_vip", 750000, 5000, 1000000),
	REGU_DYN_DSC_DEF(A210_DVDD_CPU_P, "dvdd_cpu_p", 800000, 5000, 1000000),
	REGU_DYN_DSC_DEF(A210_DVDD_GPU, "dvdd_gpu", 750000, 5000, 800000),
};

static const struct aon_regu_desc zhihe_a210_aon_regus = {
	.regu_desc = (struct regulator_desc *)&zhihe_a210_aon_regu_desc,
	.regu_num  = ARRAY_SIZE(zhihe_a210_aon_regu_desc),
};

static int zhihe_match_regulator_id(struct aon_regu_desc *regus_set, u32 id)
{
	int index;

	for (index = 0; index < regus_set->regu_num; index++) {
		if (regus_set->regu_desc[index].id == id)
			return index;
	}

	return -EINVAL;
}

static int zhihe_aon_regulator_probe(struct platform_device *pdev)
{
	int ret;
	int index;
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	struct device_node *child;
	struct regulator_config config = {};
	struct regulator_dev *rdev;
	struct regulator_desc *desc;
	struct aon_regu_desc *regus_set = NULL;
	u32 regulator_id;

	if (!np)
		return -ENODEV;

	regus_set = (struct aon_regu_desc *)of_device_get_match_data(&pdev->dev);
	if (!regus_set)
		return -ENODEV;

	/* Get IPC handle. */
	ret = zhihe_aon_get_handle(&(zhihe_aon_pmic_info.ipc_handle), "aon0");
	if (ret) {
		dev_err(dev, "failed to get ipc_handle\n");
		return ret;
	}

	/* Register regulators. */
	zhihe_aon_pmic_info.dev = dev;
	config.dev = dev;
	config.driver_data = &zhihe_aon_pmic_info;

	for_each_child_of_node(np, child) {
		ret = of_property_read_u32(child, "zhihe,regulator-id", &regulator_id);
		if (ret) {
			dev_err(dev, "failed to get zhihe,regulator-id for %pOFn\n", child);
			goto put_child;
		}

		index = zhihe_match_regulator_id(regus_set, regulator_id);
		if (index < 0) {
			dev_err(dev, "no regulator matches id %u\n", regulator_id);
			ret = -EINVAL;
			goto put_child;
		}

		desc = &regus_set->regu_desc[index];
		desc->ops = &regu_common_ops;

		config.of_node = child;
		rdev = devm_regulator_register(dev, desc, &config);
		if (IS_ERR(rdev)) {
			dev_err(dev, "Failed to register regulator %s\n", desc->name);
			ret = PTR_ERR(rdev);
			goto put_child;
		}
	}

	platform_set_drvdata(pdev, &zhihe_aon_pmic_info);
	return 0;

put_child:
	of_node_put(child);
	return ret;
}

static const struct of_device_id zhihe_pmic_dev_id[] = {
	{ .compatible = "zhihe,a210-aon-regulator", .data = &zhihe_a210_aon_regus},
	{},
};
MODULE_DEVICE_TABLE(of, zhihe_pmic_dev_id);

static struct platform_driver zhihe_aon_regulator_driver = {
	.driver = {
		   .name = "zhihe-aon-reg",
		   .owner = THIS_MODULE,
		   .of_match_table = zhihe_pmic_dev_id,
	},
	.probe = zhihe_aon_regulator_probe,
};

static int __init zhihe_aon_regulator_init(void)
{
	return platform_driver_register(&zhihe_aon_regulator_driver);
}
postcore_initcall(zhihe_aon_regulator_init);

static void __exit zhihe_aon_regulator_exit(void)
{
	platform_driver_unregister(&zhihe_aon_regulator_driver);
}
module_exit(zhihe_aon_regulator_exit);

MODULE_AUTHOR("hongkun.xu <xuhongkun@zhcomputing.com>");
MODULE_DESCRIPTION("Zhihe Aon regulator driver");
MODULE_LICENSE("GPL");
