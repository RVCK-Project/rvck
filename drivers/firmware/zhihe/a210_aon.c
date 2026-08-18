// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/firmware.h>
#include <linux/firmware/zhihe/a210-ipc.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/mailbox_client.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#ifdef CONFIG_ZHIHE_PROC_DEBUG
#include "zhihe_proc_debug.h"
#endif

#define ZHIHE_A210_AON_SYNC_VAL      0xfeabdeff
#define ZHIHE_A210_AON_RESET_VAL     0x3

#define ZHIHE_A210_AON_V1			0x0
#define ZHIHE_A210_AON_V2			0x1

/* wait for response for 3000ms instead of 300ms (fix me pls)*/
#define MAX_RX_TIMEOUT (msecs_to_jiffies(3000))
#define MAX_TX_TIMEOUT (msecs_to_jiffies(500))

struct zhihe_a210_aon_chan {
	struct zhihe_a210_aon_ipc *aon_ipc;
	struct mbox_client cl;
	struct mbox_chan *ch;
	struct completion tx_done;
	/*for log proc*/
	phys_addr_t log_phy;
	size_t log_size;
	void __iomem *log_mem;
	void *log_ctrl;
	struct proc_dir_entry *proc_dir;
};

struct zhihe_a210_aon_ipc {
	struct zhihe_a210_aon_chan chans;
	struct device *dev;
	struct mutex lock; /* Serializes RPC transactions. */
	struct completion done;
	const char *mbox_name;
	u32 *msg;
	size_t msg_size;
};

/*
 * This type is used to indicate error response for most functions.
 */
enum zhihe_a210_aon_error_codes {
	ZHIHE_A210_AON_ERR_NONE = 0, /* Success */
	ZHIHE_A210_AON_ERR_VERSION = 1, /* Incompatible API version */
	ZHIHE_A210_AON_ERR_CONFIG = 2, /* Configuration error */
	ZHIHE_A210_AON_ERR_PARM = 3, /* Bad parameter */
	ZHIHE_A210_AON_ERR_NOACCESS = 4, /* Permission error (no access) */
	ZHIHE_A210_AON_ERR_LOCKED = 5, /* Permission error (locked) */
	ZHIHE_A210_AON_ERR_UNAVAILABLE = 6, /* Unavailable (out of resources) */
	ZHIHE_A210_AON_ERR_NOTFOUND = 7, /* Not found */
	ZHIHE_A210_AON_ERR_NOPOWER = 8, /* No power */
	ZHIHE_A210_AON_ERR_IPC = 9, /* Generic IPC error */
	ZHIHE_A210_AON_ERR_BUSY = 10, /* Resource is currently busy/active */
	ZHIHE_A210_AON_ERR_FAIL = 11, /* General I/O failure */
	ZHIHE_A210_AON_ERR_LAST
};

static int zhihe_a210_aon_linux_errmap[ZHIHE_A210_AON_ERR_LAST] = {
	0, /* ZHIHE_A210_AON_ERR_NONE */
	-EINVAL, /* ZHIHE_A210_AON_ERR_VERSION */
	-EINVAL, /* ZHIHE_A210_AON_ERR_CONFIG */
	-EINVAL, /* ZHIHE_A210_AON_ERR_PARM */
	-EACCES, /* ZHIHE_A210_AON_ERR_NOACCESS */
	-EACCES, /* ZHIHE_A210_AON_ERR_LOCKED */
	-ERANGE, /* ZHIHE_A210_AON_ERR_UNAVAILABLE */
	-EEXIST, /* ZHIHE_A210_AON_ERR_NOTFOUND */
	-EPERM, /* ZHIHE_A210_AON_ERR_NOPOWER */
	-EPIPE, /* ZHIHE_A210_AON_ERR_IPC */
	-EBUSY, /* ZHIHE_A210_AON_ERR_BUSY */
	-EIO, /* ZHIHE_A210_AON_ERR_FAIL */
};

#define ZHIHE_A210_AON_CHN_MAX     (4)
static struct zhihe_a210_aon_ipc *zhihe_a210_aon_ipc_handle[ZHIHE_A210_AON_CHN_MAX];
static u32 g_aon_ipc_handle_num;

static inline int zhihe_a210_aon_to_linux_errno(int errno)
{
	if (errno >= ZHIHE_A210_AON_ERR_NONE && errno < ZHIHE_A210_AON_ERR_LAST)
		return zhihe_a210_aon_linux_errmap[errno];
	return -EIO;
}

/*
 * Get the default handle used by SCU
 */
int zhihe_a210_aon_get_handle(struct zhihe_a210_aon_ipc **ipc, char *name)
{
	u32 i = 0;

	if (!name)
		return -EINVAL;
	for (i = 0; i < g_aon_ipc_handle_num; i++) {
		if (!zhihe_a210_aon_ipc_handle[i])
			return -EPROBE_DEFER;
		if (!strcmp(name, zhihe_a210_aon_ipc_handle[i]->mbox_name)) {
			*ipc = zhihe_a210_aon_ipc_handle[i];
			return 0;
		}
	}
	return -EPROBE_DEFER;
}
EXPORT_SYMBOL(zhihe_a210_aon_get_handle);

static void zhihe_a210_aon_tx_done(struct mbox_client *cl, void *mssg, int r)
{
	struct zhihe_a210_aon_chan *aon_chan =
		container_of(cl, struct zhihe_a210_aon_chan, cl);

	complete(&aon_chan->tx_done);
}

static void zhihe_a210_aon_rx_callback(struct mbox_client *c, void *msg)
{
	struct zhihe_a210_aon_chan *aon_chan = container_of(c, struct zhihe_a210_aon_chan, cl);
	struct zhihe_a210_aon_ipc *aon_ipc = aon_chan->aon_ipc;
	size_t recv_size = ZHIHE_A210_AON_RPC_MSG_NUM * sizeof(u32);
	size_t copy_size;

	if (!msg)
		return;

	copy_size = min(recv_size, aon_ipc->msg_size);
	if (copy_size)
		memcpy(aon_ipc->msg, msg, copy_size);
	dev_dbg(aon_ipc->dev, "msg head: 0x%x, size:%zu\n", *((u32 *)msg),
		recv_size);
	complete(&aon_ipc->done);
}

static int zhihe_a210_aon_ipc_write(struct zhihe_a210_aon_ipc *aon_ipc, void *msg)
{
	struct zhihe_a210_aon_rpc_msg_hdr *hdr = msg;
	struct zhihe_a210_aon_chan *aon_chan;
	u32 *data = msg;
	int ret;
	/* check size, currently it requires 7 MSG in one transfer */
	if (hdr->size != ZHIHE_A210_AON_RPC_MSG_NUM)
		return -EINVAL;

	dev_dbg(aon_ipc->dev, "RPC SVC %u FUNC %u SIZE %u\n", hdr->svc,
		hdr->func, hdr->size);

	aon_chan = &aon_ipc->chans;

	if (!wait_for_completion_timeout(&aon_chan->tx_done, MAX_TX_TIMEOUT)) {
		dev_err(aon_ipc->dev, "tx_done timeout\n");
		return -ETIMEDOUT;
	}
	reinit_completion(&aon_chan->tx_done);
	ret = mbox_send_message(aon_chan->ch, data);
	if (ret < 0)
		return ret;

	return 0;
}

/*
 * RPC command/response
 */
int zhihe_a210_aon_call_rpc(struct zhihe_a210_aon_ipc *aon_ipc,
			    void *msg, void *ack_msg,
		       size_t ack_size, bool have_resp)
{
	struct zhihe_a210_aon_rpc_msg_hdr *hdr = msg;
	struct zhihe_a210_aon_rpc_ack_common *ack;
	int ret = 0;

	if (WARN_ON(!aon_ipc || !msg))
		return -EINVAL;
	if (have_resp && WARN_ON(!ack_msg))
		return -EINVAL;
	if (have_resp && WARN_ON(!ack_size))
		return -EINVAL;
	mutex_lock(&aon_ipc->lock);
	reinit_completion(&aon_ipc->done);
	aon_ipc->msg = ack_msg;
	aon_ipc->msg_size = ack_size;
	ZHIHE_A210_RPC_SET_VER(hdr, ZHIHE_A210_AON_RPC_VERSION);
	/*svc id use 6bit for version 2*/
	ZHIHE_A210_RPC_SET_SVC_ID(hdr, hdr->svc);
	ZHIHE_A210_RPC_SET_SVC_FLAG_MSG_TYPE(hdr, ZHIHE_A210_RPC_SVC_MSG_TYPE_DATA);
	if (have_resp)
		ZHIHE_A210_RPC_SET_SVC_FLAG_ACK_TYPE(hdr, ZHIHE_A210_RPC_SVC_MSG_NEED_ACK);
	else
		ZHIHE_A210_RPC_SET_SVC_FLAG_ACK_TYPE(hdr, ZHIHE_A210_RPC_SVC_MSG_NO_NEED_ACK);
	ret = zhihe_a210_aon_ipc_write(aon_ipc, msg);
	if (ret < 0) {
		dev_err(aon_ipc->dev, "RPC send msg failed: %d\n", ret);
		goto out;
	}
	if (have_resp) {
		if (!wait_for_completion_timeout(&aon_ipc->done, MAX_RX_TIMEOUT)) {
			dev_err(aon_ipc->dev, "RPC send msg timeout\n");
			aon_ipc->msg = NULL;
			aon_ipc->msg_size = 0;
			mutex_unlock(&aon_ipc->lock);
			return -ETIMEDOUT;
		}

		/* response status is stored in msg data[0] field */
		ack = ack_msg;
		ret = ack->err_code;
	}
out:
	aon_ipc->msg = NULL;
	aon_ipc->msg_size = 0;
	mutex_unlock(&aon_ipc->lock);

	dev_dbg(aon_ipc->dev, "RPC SVC done\n");

	return zhihe_a210_aon_to_linux_errno(ret);
}
EXPORT_SYMBOL(zhihe_a210_aon_call_rpc);

#ifdef CONFIG_ZHIHE_PROC_DEBUG
static int get_aon_log_mem(struct device *dev, phys_addr_t *mem, size_t *mem_size)
{
	struct resource r;
	struct device_node *node;
	int ret;

	*mem = 0;
	*mem_size = 0;

	node = of_parse_phandle(dev->of_node, "log-memory-region", 0);
	if (!node)
		return -ENOENT;

	ret = of_address_to_resource(node, 0, &r);
	of_node_put(node);
	if (ret)
		return ret;

	*mem = r.start;
	*mem_size = resource_size(&r);
	return 0;
}
#endif

static int zhihe_a210_aon_boot_img_by_name(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = pdev->dev.of_node;
	const struct firmware *zhihe_a210_aon_fw = NULL;
	void __iomem *base_addr;
	void __iomem *reset_reg;
	void __iomem *sync_reg;
	unsigned int reset_val = ZHIHE_A210_AON_RESET_VAL;
	unsigned int sync_val = ZHIHE_A210_AON_SYNC_VAL;
	unsigned int sync_read = 0;
	const char *bin_name = NULL;
	unsigned int timeout_ms = 50;
	unsigned long timeout;
	int ret;

	base_addr = devm_platform_ioremap_resource_byname(pdev, "aon-base");
	if (IS_ERR(base_addr)) {
		dev_err(dev, "Failed to map base_addr\n");
		return PTR_ERR(base_addr);
	}

	reset_reg = devm_platform_ioremap_resource_byname(pdev, "aon-reset");
	if (IS_ERR(reset_reg)) {
		dev_err(dev, "Failed to map aon_reset_reg\n");
		return PTR_ERR(reset_reg);
	}

	sync_reg = devm_platform_ioremap_resource_byname(pdev, "aon-sync");
	if (IS_ERR(sync_reg)) {
		dev_err(dev, "Failed to map aon_sync_reg\n");
		return PTR_ERR(sync_reg);
	}

	ret = of_property_read_string(node, "firmware-name", &bin_name);
	if (ret) {
		dev_err(dev, "Failed to get aon-bin-names property: %d\n", ret);
		return ret;
	}

	ret = request_firmware(&zhihe_a210_aon_fw, bin_name, dev);
	if (ret) {
		dev_err(dev, "Failed to load firmware: %s (ret=%d)\n",
			bin_name, ret);
		return ret;
	}

	dev_info(dev, "firmware loaded, size = %zu bytes\n", zhihe_a210_aon_fw->size);

	memset(base_addr, 0, zhihe_a210_aon_fw->size);
	memcpy(base_addr, zhihe_a210_aon_fw->data, zhihe_a210_aon_fw->size);

	iowrite32(reset_val, reset_reg);

	timeout = jiffies + msecs_to_jiffies(timeout_ms);

	while (ioread32(sync_reg) != sync_val) {
		if (time_after(jiffies, timeout)) {
			dev_err(dev, "AON bringup timeout! sync_reg=0x%x, expected=0x%x\n",
				sync_read, sync_val);
			release_firmware(zhihe_a210_aon_fw);
			return -ETIMEDOUT;
		}

		usleep_range(1000, 2000);
	}

	release_firmware(zhihe_a210_aon_fw);

	return 0;
}

static int zhihe_a210_aon_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct zhihe_a210_aon_ipc *aon_ipc;
	struct zhihe_a210_aon_chan *aon_chan;
	struct mbox_client *cl;
	struct device_node *np;
	int ret;

	aon_ipc = devm_kzalloc(dev, sizeof(*aon_ipc), GFP_KERNEL);
	if (!aon_ipc)
		return -ENOMEM;
	aon_chan = &aon_ipc->chans;
	cl = &aon_chan->cl;
	cl->dev = dev;
	cl->tx_block = false;
	cl->knows_txdone = true;
	cl->rx_callback = zhihe_a210_aon_rx_callback;

	/* Initial tx_done completion as "done" */
	cl->tx_done = zhihe_a210_aon_tx_done;
	init_completion(&aon_chan->tx_done);
	complete(&aon_chan->tx_done);

	aon_chan->aon_ipc = aon_ipc;
	np = cl->dev->of_node;
	aon_ipc->mbox_name = of_get_property(np, "mbox-names", NULL);
	aon_chan->ch = mbox_request_channel_byname(cl, aon_ipc->mbox_name);
	if (IS_ERR(aon_chan->ch)) {
		ret = PTR_ERR(aon_chan->ch);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Failed to request aon mbox chan ret %d\n",
				ret);
		return ret;
	}
	dev_dbg(dev, "request zhihe mbox chan: %s\n", aon_ipc->mbox_name);

	aon_ipc->dev = dev;
	mutex_init(&aon_ipc->lock);
	init_completion(&aon_ipc->done);
	aon_chan->log_ctrl = NULL;
	if (g_aon_ipc_handle_num >= ZHIHE_A210_AON_CHN_MAX) {
		dev_err(dev, "aon_ipc:%s handle num overflow\n", aon_ipc->mbox_name);
		return -ENOSPC;
	}

#ifdef CONFIG_ZHIHE_PROC_DEBUG
	ret = get_aon_log_mem(dev, &aon_chan->log_phy, &aon_chan->log_size);
	if (!ret) {
		aon_chan->log_mem = ioremap(aon_chan->log_phy, aon_chan->log_size);
		if (!aon_chan->log_mem) {
			dev_err(dev, "%s:get aon log region fail\n", __func__);
			return -ENOMEM;
		}

		aon_chan->proc_dir = proc_mkdir("aon_proc", NULL);
		if (!aon_chan->proc_dir) {
			dev_err(dev, "create aon_proc fail\n");
			return -ENOMEM;
		}

		aon_chan->log_ctrl =
			zhihe_create_panic_log_proc(aon_chan->log_phy,
						    aon_chan->proc_dir,
						    aon_chan->log_mem,
						    aon_chan->log_size);
	} else if (ret != -ENOENT) {
		dev_warn(dev, "failed to get AON log memory: %d\n", ret);
	}
#endif

	zhihe_a210_aon_ipc_handle[g_aon_ipc_handle_num] = aon_ipc;
	g_aon_ipc_handle_num++;

	ret = zhihe_a210_aon_boot_img_by_name(pdev);
	if (ret == -ETIMEDOUT) {
		dev_warn(dev, "AON sync timeout, retrying...\n");
		ret = zhihe_a210_aon_boot_img_by_name(pdev);
		if (ret) {
			dev_err(dev, "AON bringup failed after retry (ret=%d)\n", ret);
			return ret;
		}
	}

	return devm_of_platform_populate(dev);
}

static const struct of_device_id zhihe_a210_aon_match[] = {
	{
		.compatible = "zhihe,a210-aon",
	},
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, zhihe_a210_aon_match);

static int __maybe_unused zhihe_a210_aon_resume_noirq(struct device *dev)
{
	return 0;
}

static const struct dev_pm_ops zhihe_a210_aon_pm_ops = {
	SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(NULL, zhihe_a210_aon_resume_noirq)
};

static struct platform_driver zhihe_a210_aon_driver = {
	.driver = {
		.name = "zhihe_a210_aon",
		.of_match_table = zhihe_a210_aon_match,
		.pm = &zhihe_a210_aon_pm_ops,
	},
	.probe = zhihe_a210_aon_probe,
};
module_platform_driver(zhihe_a210_aon_driver);

MODULE_AUTHOR("Hongkun Xu <xuhongkun@zhcomputing.com>");
MODULE_AUTHOR("Xionglue Huang <huangxionglue@zhcomputing.com>");
MODULE_DESCRIPTION("ZhiHe A210 AON firmware driver");
MODULE_LICENSE("GPL");
