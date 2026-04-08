// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Zhihe Computing Limited.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_clk.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/pm.h>
#include <linux/pm_domain.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/debugfs.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>
#include <linux/regulator/consumer.h>
#include <dt-bindings/iopmp/a210-iopmp.h>
#include <asm/a210-iopmp.h>

#include "a210-pd.h"

static struct dentry *pd_debugfs_root;
static struct dentry *pd_pde;

static inline struct a210_pm_domain *to_a210_pd(struct generic_pm_domain *domain)
{
	return container_of(domain, struct a210_pm_domain, pd);
}

static void __iomem *a210_ioremap_by_index(struct device *dev, struct device_node *np,
				    int index)
{
	const __be32 *reg;
	struct resource res;
	u64 addr, size;
	int na, ns, len;
	int ret;

	ret = of_address_to_resource(np, index, &res);
	if (!ret)
		return devm_ioremap(dev, res.start, resource_size(&res));

	/*
	 * Power-domain IDs are described in the parent node address space, while
	 * CCU subnodes still contain real SoC MMIO addresses.  There is no useful
	 * address translation between those two spaces, so fall back to mapping the
	 * raw address from the CCU reg property.
	 */
	na = of_n_addr_cells(np);
	ns = of_n_size_cells(np);
	reg = of_get_property(np, "reg", &len);
	if (!reg || len < (index + 1) * (na + ns) * sizeof(__be32)) {
		dev_err(dev, "Failed to get resource from node %pOF at index %d\n",
			np, index);
		return ERR_PTR(ret);
	}

	reg += index * (na + ns);
	addr = of_read_number(reg, na);
	size = of_read_number(reg + na, ns);
	if (!size)
		return ERR_PTR(-EINVAL);

	return devm_ioremap(dev, addr, size);
}

static void __iomem *a210_ioremap_by_name(struct device *dev, struct device_node *np,
				   const char *name)
{
	int idx;

	idx = of_property_match_string(np, "reg-names", name);
	if (idx < 0)
		return ERR_PTR(-ENODEV);

	return a210_ioremap_by_index(dev, np, idx);
}

static bool a210_child_is_available_ccu(struct device_node *np)
{
	if (of_device_is_compatible(np, "zhihe,a210-ccu") && of_device_is_available(np))
		return true;

	return false;
}

static int a210_parse_ccu(struct device *dev, struct device_node *np,
			  struct a210_pm_domain *a210_pd)
{
	struct device_node *child;
	int ret;

	INIT_LIST_HEAD(&a210_pd->ccu_grp);
	for_each_child_of_node(np, child) {
		if (!a210_child_is_available_ccu(child))
			continue;

		int count = of_address_count(child);

		if (count <= 0)
			continue;

		struct a210_ccu_grp *ccu_grp = devm_kzalloc(
			dev, struct_size(ccu_grp, ccu, count), GFP_KERNEL);
		if (!ccu_grp) {
			of_node_put(child);
			return -ENOMEM;
		}

		ccu_grp->num_ccus = count;
		ccu_grp->mode = CCU_MODE_GATING; // gating as the default option
		of_property_read_u32(child, "zhihe,ccu-mode", &ccu_grp->mode);

		for (int i = 0; i < ccu_grp->num_ccus; i++) {
			ccu_grp->ccu[i].base =
				a210_ioremap_by_index(dev, child, i);

			ret = of_property_read_u32_index(child,
				"zhihe,ccu-dly-time", i,
				&ccu_grp->ccu[i].delay);
			if (ret) {
				dev_err(dev,
					"ccu failed to get dly-time for %pOF idx=%d\n",
					child, i);
				of_node_put(child);
				return -EINVAL;
			}

			ret = of_property_read_u32_index(child,
				"zhihe,ccu-dly-time-step",
				i, &ccu_grp->ccu[i].step);
			if (ret) {
				dev_err(dev,
					"ccu failed to get dly-time-step for %pOF idx=%d\n",
					child, i);
				of_node_put(child);
				return -EINVAL;
			}

			if (ccu_grp->mode == CCU_MODE_SCALING) {
				ret = of_property_read_u32_index(child,
					"zhihe,ccu-ratio", i,
					&ccu_grp->ccu[i].ratio);
				if (ret) {
					dev_err(dev,
						"ccu failed to get ratio for %pOF idx=%d\n",
						child, i);
					of_node_put(child);
					return -EINVAL;
				}
			}
		}
		list_add_tail(&ccu_grp->list, &a210_pd->ccu_grp);
	}

	return 0;
}

static void a210_config_ccu(struct a210_pm_domain *a210_pd)
{
	struct a210_ccu_grp *ccu_grp;

	list_for_each_entry(ccu_grp, &a210_pd->ccu_grp, list) {
		for (int i = 0; i < ccu_grp->num_ccus; i++) {
			u32 val = ccu_grp->ccu[i].delay |
				  (ccu_grp->ccu[i].step << 8);

			writel(val, ccu_grp->ccu[i].base);
			if (ccu_grp->mode == CCU_MODE_GATING)
				writel(0x4, ccu_grp->ccu[i].base + 0x4);
			else {
				writel(0x2, ccu_grp->ccu[i].base + 0x4);
				writel(ccu_grp->ccu[i].ratio,
				       ccu_grp->ccu[i].base + 0x18);
			}
		}
	}
}

static void bpc_config(struct device *dev, const char *str,
		       void __iomem *base_addr, u32 bpc_ctrl)
{
	if ((bpc_ctrl & BIT(0)) != 0) {
		dev_dbg(dev, "Enter %s sw model...\n", str);
		writel(0x1, base_addr + 0x000); // 0x1 bypass
		writel(0x18, base_addr + 0x13c); // bpc 9000 ocgen &rset
	} else {
		dev_dbg(dev, "Enter %s hw model...\n", str);
		writel(0x0, base_addr + 0x000);
	}
	writel(0x10101, base_addr + 0x004); // pwr venc bpc 3000| fence
}

static void pcu_intr(struct device *dev, const char *str,
		     void __iomem *base_addr)
{
	u32 data;

	udelay(1);
	data = readl(base_addr + 0x2c); // read pcu intr
	while (data == 0) {
		udelay(1);
		data = readl(base_addr + 0x2c); // read pcu intr
	}
	if (((data & BIT(0)) != 0) || ((data & BIT(3)) != 0))
		dev_dbg(dev, "%s accept\n", str);
	if (((data & BIT(1)) != 0) || ((data & BIT(4)) != 0))
		dev_err(dev, "%s deny\n", str);

	if (((data & BIT(2)) != 0) || ((data & BIT(5)) != 0))
		dev_err(dev, "%s timeout\n", str);
	writel(data, base_addr + 0x28); // clr cpu intr
}

static void pcu_config(struct device *dev, const char *str,
		       void __iomem *base_addr, u32 pcu_ctrl, u32 state)
{
	writel(0x3f, base_addr + 0x24); // interrupt enable
	if ((pcu_ctrl & BIT(0)) != 0) {
		dev_dbg(dev,
			"Enter %s: pcu reg trigger...state=0x%x\n",
			str, state);
		writel((state & 0x1f), base_addr + 0x0c); // lpstate = power on
		writel(0x1, base_addr + 0x08); // lqreq
		pcu_intr(dev, str, base_addr); // wait for accept
	} else {
		dev_dbg(dev, "Enter %s: wait r2p trigger...\n", str);
	}
}

static int a210_pd_power_switch(struct generic_pm_domain *domain,
				enum power_mode mode)
{
	struct a210_pm_domain *a210_pd = to_a210_pd(domain);
	struct a210_pd_soc *soc = a210_pd->soc;

	struct device *dev = soc->dev;
	struct regulator *regulator = soc->regulators[a210_pd->index];
	const char *name = domain->name;
	int ret;

	if (mode == ON && regulator != NULL) {
		ret = regulator_enable(regulator);
		if (ret) {
			dev_err(dev, "failed to regulator_enable for %s", name);
			return ret;
		}
	}

	/* config pca if needed */
	if (mode == ON && !IS_ERR(a210_pd->pca_base))
		writel(0x0, a210_pd->pca_base + 0x20);

	if (!IS_ERR(a210_pd->bpc_base))
		bpc_config(dev, name, a210_pd->bpc_base, BPC_HW_MODEL);
	if (!IS_ERR(a210_pd->pcu_base))
		pcu_config(dev, name, a210_pd->pcu_base, PCU_REG_TRIGGER, mode);

	if (mode == OFF && regulator != NULL) {
		ret = regulator_disable(regulator);
		if (ret) {
			dev_err(dev, "failed to regulator_disable for %s", name);
			return ret;
		}
	}

	return 0;
}

static int a210_pd_power_off(struct generic_pm_domain *domain)
{
	struct a210_pm_domain *a210_pd = to_a210_pd(domain);
	int ret;

#ifdef CONFIG_A210_IOPMP
	if (a210_pd->device_ids_count > 0)
		iopmp_disable(a210_pd->device_ids, a210_pd->device_ids_count);
#endif

	if (a210_pd->num_clks)
		clk_bulk_disable(a210_pd->num_clks, a210_pd->clks);

	ret = reset_control_assert(a210_pd->reset);
	if (ret)
		return ret;

	return a210_pd_power_switch(domain, OFF);
}

static int a210_pd_power_on(struct generic_pm_domain *domain)
{
	struct a210_pm_domain *a210_pd = to_a210_pd(domain);
	int ret;

	ret = a210_pd_power_switch(domain, ON);
	if (ret)
		return ret;

	ret = reset_control_deassert(a210_pd->reset);
	if (ret)
		return ret;

	if (a210_pd->num_clks) {
		ret = clk_bulk_enable(a210_pd->num_clks, a210_pd->clks);
		if (ret)
			return ret;
	}

#ifdef CONFIG_A210_IOPMP
	if (a210_pd->device_ids_count > 0)
		iopmp_enable(a210_pd->device_ids, a210_pd->device_ids_count);
#endif

	a210_config_ccu(a210_pd);

	return 0;
}

static char *a210_pd_get_user_string(const char __user *userbuf, size_t userlen)
{
	char *buffer;

	buffer = vmalloc(userlen + 1);
	if (!buffer)
		return ERR_PTR(-ENOMEM);

	if (copy_from_user(buffer, userbuf, userlen) != 0) {
		vfree(buffer);
		return ERR_PTR(-EFAULT);
	}

	pr_debug("buffer before strip linefeed = %s\n", buffer);
	/* got the string, now strip linefeed. */
	if (buffer[userlen - 1] == '\n')
		buffer[userlen - 1] = '\0';
	else
		buffer[userlen] = '\0';

	pr_debug("buffer after strip linefeed = %s\n", buffer);

	return buffer;
}

static ssize_t a210_power_domain_write(struct file *file,
				       const char __user *userbuf,
				       size_t userlen, loff_t *ppos)
{
	char *buffer, *start, *end;
	struct seq_file *m = (struct seq_file *)file->private_data;
	struct a210_pd_soc *soc = m->private;
	struct device *dev = soc->dev;
	struct generic_pm_domain *domain;
	char pd_name[A210_PD_NAME_SIZE];
	char pd_state[A210_PD_STATE_NAME_SIZE];
	int idx, ret;

	buffer = a210_pd_get_user_string(userbuf, userlen);
	if (IS_ERR(buffer))
		return PTR_ERR(buffer);

	start = skip_spaces(buffer); // skip leading space if any
	end = start;
	while (!isspace(*end) && *end != '\0')
		end++;

	*end = '\0';
	strscpy(pd_name, start, sizeof(pd_name));

	/* find the target power domain */
	for (idx = 0; idx < soc->num_domains; idx++) {
		domain = &soc->domains[idx]->pd;
		if (strcmp(pd_name, domain->name))
			continue;
		break;
	}

	if (idx == soc->num_domains) {
		dev_err(dev,
			"no taget power domain-%s found, idx = %d, total pd numbers = %d\n",
			pd_name, idx, soc->num_domains);
		userlen = -EINVAL;
		goto out;
	}

	end = end + 1; // end is the new start
	start = skip_spaces(end); // skip leading space if any
	end = start;
	while (!isspace(*end) && *end != '\0')
		end++;

	*end = '\0';
	strscpy(pd_state, start, sizeof(pd_state));

	if (!strcmp(pd_state, "on")) {
		ret = domain->power_on(domain);
		if (ret) {
			userlen = ret;
			goto out;
		}
	} else if (!strcmp(pd_state, "off")) {
		ret = domain->power_off(domain);
		if (ret) {
			userlen = ret;
			goto out;
		}
	} else {
		dev_err(dev,
			"invalid power domain target state, not 'on' or 'off'\n");
		userlen = -EINVAL;
		goto out;
	}

out:
	vfree(buffer);

	return userlen;
}

static int a210_power_domain_show(struct seq_file *m, void *v)
{
	struct a210_pd_soc *soc = m->private;
	u32 count = soc->num_domains;
	int idx;

	seq_puts(m, "[Power domain name list]: ");
	for (idx = 0; idx < count; idx++)
		seq_printf(m, "%s ", soc->domains[idx]->pd.name);
	seq_puts(m, "\n");
	seq_puts(m, "[Power on  domain usage]: echo power_name on  > domain\n");
	seq_puts(m, "[Power off domain usage]: echo power_name off > domain\n");

	return 0;
}

static int a210_power_domain_open(struct inode *inode, struct file *file)
{
	struct a210_pd_soc *soc = inode->i_private;

	return single_open(file, a210_power_domain_show, soc);
}

static const struct file_operations a210_power_domain_fops = {
	.owner = THIS_MODULE,
	.write = a210_power_domain_write,
	.read = seq_read,
	.open = a210_power_domain_open,
	.llseek = generic_file_llseek,
};

static void pd_debugfs_init(struct a210_pd_soc *soc)
{
	pd_debugfs_root = debugfs_create_dir("power_domain", NULL);
	if (IS_ERR_OR_NULL(pd_debugfs_root))
		return;

	pd_pde = debugfs_create_file("domain", 0600, pd_debugfs_root, soc,
				     &a210_power_domain_fops);
}

static int a210_domain_lookup(struct device_node *np)
{
	u32 id;

	if (!of_property_read_u32_index(np, "reg", 1, &id))
		return id;

	if (!of_property_read_u32(np, "reg", &id))
		return id;

	if (!of_property_read_u32(np, "id", &id))
		return id;

	return -ENODEV;
}

static int a210_add_one_domain(struct platform_device *pdev,
			       struct device_node *np)
{
	struct device *dev = &pdev->dev;
	struct a210_pd_soc *pd_soc = dev_get_drvdata(dev);
	struct a210_pm_domain *a210_pd;
	const char *name;
	int ret;

	a210_pd = devm_kzalloc(dev, sizeof(*a210_pd), GFP_KERNEL);
	if (!a210_pd)
		return -ENOMEM;

	int id = a210_domain_lookup(np);

	if (id < 0)
		return -ENODEV;

	name = strrchr(of_node_full_name(np), '/');
	a210_pd->pd.name = devm_kstrdup(dev, name ? name + 1 : of_node_full_name(np),
					 GFP_KERNEL);
	if (!a210_pd->pd.name)
		return -ENOMEM;

	a210_pd->index = id;
	a210_pd->pd.power_off = a210_pd_power_off;
	a210_pd->pd.power_on = a210_pd_power_on;
	a210_pd->soc = pd_soc;

	a210_pd->pca_base = a210_ioremap_by_name(dev, np, "pca");
	a210_pd->bpc_base = a210_ioremap_by_name(dev, np, "bpc");
	a210_pd->pcu_base = a210_ioremap_by_name(dev, np, "pcu");

	ret = pm_genpd_init(&a210_pd->pd, NULL, true);
	if (ret) {
		dev_err(dev, "failed to init power domain %s index %d",
			a210_pd->pd.name, a210_pd->index);
		devm_kfree(dev, a210_pd);
		return -ENODEV;
	}

	ret = of_genpd_add_provider_simple(np, &a210_pd->pd);
	if (ret) {
		dev_err(dev, "failed to add PM domain provider for %pOFn: %d\n",
			np, ret);
		goto remove_genpd;
	}

	a210_pd->reset = of_reset_control_array_get_optional_shared(np);
	if (IS_ERR(a210_pd->reset)) {
		ret = PTR_ERR(a210_pd->reset);
		dev_err(dev, "failed to get device resets for domain:%s\n", np->name);
		goto reset_fail;
	}

	a210_pd->num_clks = of_clk_get_parent_count(np);
	if (a210_pd->num_clks) {
		a210_pd->clks = devm_kcalloc(dev, a210_pd->num_clks,
					     sizeof(*a210_pd->clks), GFP_KERNEL);
		if (!a210_pd->clks) {
			ret = -ENOMEM;
			goto reset_fail;
		}

		for (int i = 0; i < a210_pd->num_clks; i++) {
			a210_pd->clks[i].clk = of_clk_get(np, i);
			if (IS_ERR(a210_pd->clks[i].clk)) {
				ret = PTR_ERR(a210_pd->clks[i].clk);
				dev_err(dev,
					"failed to get clk at index %d: err:%d for domain:%s\n",
					i, ret, np->name);
				goto clk_fail;
			}
		}

		ret = clk_bulk_prepare(a210_pd->num_clks, a210_pd->clks);
		if (ret) {
			clk_bulk_put(a210_pd->num_clks, a210_pd->clks);
			goto clk_fail;
		}
	}

	ret = a210_parse_ccu(dev, np, a210_pd);
	if (ret)
		goto clk_fail;

	pd_soc->domains[pd_soc->num_domains++] = a210_pd;

	dev_dbg(dev, "added PM domain %s\n", a210_pd->pd.name);

#ifdef CONFIG_A210_IOPMP
	/* get iopmps config node */
	int device_id_count = 0;
	int count = of_count_phandle_with_args(np, "iopmps", NULL);

	for (int i = 0; i < count; i++) {
		struct device_node *iopmp_node;

		iopmp_node = of_parse_phandle(np, "iopmps", i);
		if (!iopmp_node) {
			dev_err(dev,
				"failed to get iopmps at index %d for domain:%s\n",
				i, np->name);
			ret = -EINVAL;
			goto clk_fail;
		}

		u32 device_id;

		if (of_property_read_u32(iopmp_node, "device-id",
					 &device_id) == 0) {
			a210_pd->device_ids[device_id_count] = device_id;
			device_id_count++;
			dev_dbg(dev,
				"domain %pOFn iopmp %pOFn: device id: %d\n",
				np, iopmp_node, device_id);
		}
	}
	a210_pd->device_ids_count = device_id_count;
#endif

	return 0;

clk_fail:
	devm_kfree(dev, a210_pd->clks);
reset_fail:
	reset_control_put(a210_pd->reset);
remove_genpd:
	pm_genpd_remove(&a210_pd->pd);
	devm_kfree(dev, a210_pd);
	return ret;
}

static int a210_init_pm_domains(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct device_node *child;
	struct of_phandle_args child_args, parent_args;
	int ret;

	for_each_child_of_node(np, child) {
		if (!of_device_is_available(child))
			continue;

		ret = a210_add_one_domain(pdev, child);
		if (ret) {
			dev_err(dev, "failed to handle node %pOFn: %d\n",
				child, ret);
			of_node_put(child);
			return ret;
		}

		if (of_parse_phandle_with_args(child, "power-domains",
					       "#power-domain-cells", 0, &parent_args))
			continue;

		child_args.np = child;
		child_args.args_count = 0;

		ret = of_genpd_add_subdomain(&parent_args, &child_args);
		of_node_put(parent_args.np);
		if (ret) {
			dev_err(dev, "failed to handle subdomain node %pOFn: %d\n",
				child, ret);
			of_node_put(child);
			return ret;
		}
	}

	of_node_put(np);

	return ret;
}

static int a210_pd_parse_regulators(struct device *dev)
{
	struct a210_pd_soc *pd_soc = dev_get_drvdata(dev);
	struct device_node *np = dev->of_node;
	struct device_node *child, *child_regulator;

	for_each_child_of_node(np, child) {
		if (!of_device_is_available(child))
			continue;

		int id = a210_domain_lookup(child);

		if (id < 0)
			return -ENODEV;

		if (of_property_present(child, "pmic-supply")) {
			child_regulator = of_parse_phandle(child,
							   "pmic-supply", 0);
			pd_soc->regulators[id] = regulator_get_optional(
				dev, child_regulator->name);
			if (IS_ERR(pd_soc->regulators[id])) {
				dev_dbg(dev, "Regulator for %s deferred %ld\n",
					child->name,
					PTR_ERR(pd_soc->regulators[id]));
				return -EPROBE_DEFER;
			}

			u32 max_uV = 0;

			if (of_property_read_u32(child_regulator,
				    "regulator-max-microvolt",
				    &max_uV) == 0) {
				regulator_set_voltage(
					pd_soc->regulators[id],
					max_uV, max_uV);
				dev_info(dev, "Set %s voltage target %duV\n",
					 child->name, max_uV);
				regulator_put(pd_soc->regulators[id]);
				pd_soc->regulators[id] =
					devm_regulator_get_optional(
						dev, child_regulator->name);
			}
		}
	}
	of_node_put(np);

	return 0;
}

static int a210_pd_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct a210_pd_soc *pd_soc;
	int ret;

	pd_soc = devm_kzalloc(dev, sizeof(*pd_soc), GFP_KERNEL);
	if (!pd_soc)
		return -ENOMEM;
	pd_soc->dev = dev;

	dev_set_drvdata(dev, pd_soc);

	ret = a210_pd_parse_regulators(dev);
	if (ret)
		return ret;

	ret = a210_init_pm_domains(pdev);
	if (ret)
		return ret;

	pd_debugfs_init(pd_soc);

	dev_info(dev, "Registered a210 power domain\n");

	return ret;
}

static const struct of_device_id a210_pd_of_match[] = {
	{ .compatible = "zhihe,a210-power-domain" },
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, a210_pd_of_match);

static struct platform_driver a210_pd_driver = {
	.probe = a210_pd_probe,
	.driver = {
		.name = "a210-power-domain",
		.of_match_table = of_match_ptr(a210_pd_of_match),
	},
};

builtin_platform_driver(a210_pd_driver);

MODULE_AUTHOR("dong.yan <yand@zhcomputing.com>");
MODULE_DESCRIPTION("Zhihe A210 power domain driver");
MODULE_LICENSE("GPL");
