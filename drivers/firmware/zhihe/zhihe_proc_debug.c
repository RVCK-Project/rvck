// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/cacheflush.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#define GET_PAGE_NUM(size, offset) \
	((((size) + ((offset) & ~PAGE_MASK)) + PAGE_SIZE - 1) >> PAGE_SHIFT)

struct zhihe_log_ring_buffer {
	__u32 read;
	__u32 write;
	__u32 size;
	__u32 reserved[1];
	__u8 data[];
};

struct zhihe_hw_log {
	__u32 panic;
	__u32 reserved[2];
	struct zhihe_log_ring_buffer rb;
};

struct zhihe_proc_log_ctrl {
	struct zhihe_hw_log __iomem *log;
	struct proc_dir_entry *log_proc_file;
	phys_addr_t log_phy;
};

static void dump_regs(const char *fn, void *hw_arg)
{
	struct zhihe_proc_log_ctrl *log_ctrl = hw_arg;

	if (!log_ctrl->log)
		return;

	pr_debug("%s: panic = 0x%08x\n", fn,
		 __raw_readl(&log_ctrl->log->panic));
	pr_debug("%s: read = 0x%08x, write = 0x%08x, size = 0x%08x\n", fn,
		 __raw_readl(&log_ctrl->log->rb.read),
		 __raw_readl(&log_ctrl->log->rb.write),
		 __raw_readl(&log_ctrl->log->rb.size));
}

static int log_proc_show(struct seq_file *file, void *v)
{
	struct zhihe_proc_log_ctrl *log_ctrl = file->private;
	u8 last_frame_size;
	int log_patch_1 = -1;
	int log_patch_2 = -1;
	size_t log_size;
	int page_num;
	u32 write;
	u32 read;
	u32 size;
	size_t i;
	char *buf;

	/* dcache clean and invalidate */
	ALT_CMO_OP(flush, phys_to_virt(log_ctrl->log_phy),
		   sizeof(struct zhihe_hw_log), riscv_cbom_block_size);

	write = __raw_readl(&log_ctrl->log->rb.write);
	read = __raw_readl(&log_ctrl->log->rb.read);
	size = __raw_readl(&log_ctrl->log->rb.size);
	log_size = write >= read ? write - read : size + write - read;

	seq_puts(file, "****************** device log >>>>>>>>>>>>>>>>>\n");
	dump_regs(__func__, log_ctrl);
	if (!log_size) {
		seq_puts(file, "****************** end device log <<<<<<<<<<<<<<<<<\n");
		return 0;
	}

	page_num = GET_PAGE_NUM(log_size, 0);
	buf = kmalloc(PAGE_SIZE * page_num, GFP_KERNEL);
	if (!buf) {
		pr_debug("Fail to alloc buf\n");
		return -ENOMEM;
	}

	if (read + log_size >= size) {
		log_patch_2 = read + log_size - size + 1;
		log_patch_1 = log_size - log_patch_2;
	} else {
		log_patch_1 = log_size;
	}

	memcpy_fromio(buf, &log_ctrl->log->rb.data[read], log_patch_1);
	if (log_patch_2 > 0)
		memcpy_fromio(buf, &log_ctrl->log->rb.data[0], log_patch_2);

	last_frame_size = log_size % 64;
	for (i = 0; i < log_size - last_frame_size; i += 64)
		seq_printf(file, " %*pEp", 64, buf + i);

	if (last_frame_size)
		seq_printf(file, " %*pEp", last_frame_size,
			   buf + log_size - last_frame_size);

	__raw_writel(write, &log_ctrl->log->rb.read);
	kfree(buf);

	/* dcache clean */
	ALT_CMO_OP(clean, phys_to_virt(log_ctrl->log_phy),
		   sizeof(struct zhihe_hw_log), riscv_cbom_block_size);
	seq_puts(file, "\n****************** end device log <<<<<<<<<<<<<<<<<\n");

	return 0;
}

static bool zhihe_panic_init(struct zhihe_hw_log *hw_log, size_t size)
{
	if (size < sizeof(struct zhihe_hw_log))
		return false;

	hw_log->rb.read = 0;
	hw_log->rb.size = size - sizeof(struct zhihe_hw_log);
	return true;
}

void *zhihe_create_panic_log_proc(phys_addr_t log_phy, void *dir,
				  void *log_info_addr, size_t size)
{
	struct zhihe_proc_log_ctrl *log_ctrl;

	log_ctrl = kmalloc(sizeof(*log_ctrl), GFP_KERNEL);

	if (!log_ctrl)
		return NULL;

	log_ctrl->log = log_info_addr;

	zhihe_panic_init(log_ctrl->log, size);

	log_ctrl->log_proc_file = proc_create_single_data("proc_log", 0644, dir,
							  &log_proc_show,
							  log_ctrl);
	if (!log_ctrl->log_proc_file) {
		pr_debug("Error: Could not initialize %s\n", "dsp_log");
		kfree(log_ctrl);
		return NULL;
	}

	pr_debug("%s create Success!\n", "dsp_log");

	log_ctrl->log_phy = log_phy;
	return log_ctrl;
}
EXPORT_SYMBOL_GPL(zhihe_create_panic_log_proc);
