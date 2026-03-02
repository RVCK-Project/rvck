# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 252</li>
<li><strong>统计时间</strong>: 2026-03-02 22:17:57</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.127</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [b98351e8](https://github.com/RVCK-Project/rvck/commit/b98351e885d1e4193cadb24bc7094003c8912769) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [874a6329](https://github.com/RVCK-Project/rvck/commit/874a6329ff6e0aed1dbab1335f47475fb25d424e) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [1a9d2259](https://github.com/RVCK-Project/rvck/commit/1a9d22595988d8e516b9dde318d91067c48f6c61) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [81fc6ba9](https://github.com/RVCK-Project/rvck/commit/81fc6ba90df2b6e08a3763edecc667fa6b1c7594) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [53870595](https://github.com/RVCK-Project/rvck/commit/5387059550597e4038e59c5099b0375db3299a5c) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [8f70a405](https://github.com/RVCK-Project/rvck/commit/8f70a405d640ce42e86189324e9e0a626b0b7d74) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [4d4d68d8](https://github.com/RVCK-Project/rvck/commit/4d4d68d8bd6b01f8c19bd07215382dd87004bcb6) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [d89c219f](https://github.com/RVCK-Project/rvck/commit/d89c219f4edcf721483f0a4ecd1b5f50bc73f875) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [bcfb8923](https://github.com/RVCK-Project/rvck/commit/bcfb8923717cd553a78a04dd6be4e576ee1c4bbc) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [744a2c68](https://github.com/RVCK-Project/rvck/commit/744a2c688b837e43454a3609f25abdce6f514292) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [61c1476d](https://github.com/RVCK-Project/rvck/commit/61c1476df1d6313f294636c88d254818fbe07cc2) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [14b950bf](https://github.com/RVCK-Project/rvck/commit/14b950bf0dac795f05ce8904b92bec68f617272e) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [4926ca94](https://github.com/RVCK-Project/rvck/commit/4926ca94b50ff88b2bd67f02dd3075c8a9900f17) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [db996b7e](https://github.com/RVCK-Project/rvck/commit/db996b7ec55caeace0b6751dc016ce018957c6d0) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [7c255552](https://github.com/RVCK-Project/rvck/commit/7c255552095987a6e3b346bc9f3b0a4eafd862b3) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [0189c0f7](https://github.com/RVCK-Project/rvck/commit/0189c0f7164c547a2630908a2e42f7357f904a02) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [58e9bf4b](https://github.com/RVCK-Project/rvck/commit/58e9bf4ba3971edefe9982002e6ce5e1c920b2bd) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [0f9ebb13](https://github.com/RVCK-Project/rvck/commit/0f9ebb13dcd14d29f97cb03920022894a4eb1efd) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [374eb435](https://github.com/RVCK-Project/rvck/commit/374eb435d6860aa21887b0da43211f57288491bd) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [3d5dc24b](https://github.com/RVCK-Project/rvck/commit/3d5dc24bfcd253cf67e23f42884fafc2b5ad419d) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
| [78e23726](https://github.com/RVCK-Project/rvck/commit/78e23726016a4ab058c660da14a861226d12f84e) | 2024-04-09 | Baoquan He | kexec: fix the unexpected kexec_dprintk() macro |
| [29b9ec31](https://github.com/RVCK-Project/rvck/commit/29b9ec3162c269b9cda5e02174584d27eac411a4) | 2024-07-30 | Sunil V L | kexec_file, parisc: print out debugging message if required |
| [0bdccd03](https://github.com/RVCK-Project/rvck/commit/0bdccd0385fa9607ea94d936baa1c0f2805735f6) | 2023-12-13 | Baoquan He | kexec_file, power: print out debugging message if required |
| [7a1d88a0](https://github.com/RVCK-Project/rvck/commit/7a1d88a0e4697f7ecf8fe256ce8b5f577d574db4) | 2023-12-13 | Baoquan He | kexec_file, riscv: print out debugging message if required |
| [7586d9c6](https://github.com/RVCK-Project/rvck/commit/7586d9c6c1f8ac8554604a951d85e864e4a781b6) | 2023-12-13 | Baoquan He | kexec_file, arm64: print out debugging message if required |
| [a0c8fbdc](https://github.com/RVCK-Project/rvck/commit/a0c8fbdc8479842b7acab767a1a95c9859c76d4b) | 2023-12-13 | Baoquan He | kexec_file, x86: print out debugging message if required |
| [9fe2c4ba](https://github.com/RVCK-Project/rvck/commit/9fe2c4ba04981a7ff2b99f5d7e8554372538284b) | 2023-12-13 | Baoquan He | kexec_file: print out debugging message if required |
| [7c9f1578](https://github.com/RVCK-Project/rvck/commit/7c9f15789bbc1edbd371249e37ee8f17c28f44bc) | 2023-12-13 | Baoquan He | kexec_file: add kexec_file flag to control debug printing |
| [3ee78516](https://github.com/RVCK-Project/rvck/commit/3ee7851664399e11fcb3c97209745206f5b70eaf) | 2025-04-03 | Radim Krčmář | KVM: RISC-V: reset smstateen CSRs |
| [16502575](https://github.com/RVCK-Project/rvck/commit/165025756d9c1491962432f5c5bc9e89ebceab94) | 2023-12-24 | Anup Patel | RISC-V: KVM: Fix indentation in kvm_riscv_vcpu_set_reg_csr() |
| [7032b88f](https://github.com/RVCK-Project/rvck/commit/7032b88f56ce8aaf4396e8770ad624447ad83698) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 to ONE_REG |
| [09e918f3](https://github.com/RVCK-Project/rvck/commit/09e918f39d486f778bfb534ee9cb055f16a6b15c) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 context save/restore |
| [8fd5d62d](https://github.com/RVCK-Project/rvck/commit/8fd5d62d352ee4a1315e7f613abe6dd34fb8baad) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add senvcfg context save/restore |
| [bbb0c8b7](https://github.com/RVCK-Project/rvck/commit/bbb0c8b7321d4df930cb5525349e14e33a9d5657) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Enable Smstateen accesses |
| [d44b7070](https://github.com/RVCK-Project/rvck/commit/d44b70709d4bcaeaf21cef361e4d6adb61aa0439) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Add kvm_vcpu_config |
| [ae43c8b6](https://github.com/RVCK-Project/rvck/commit/ae43c8b6c6b7c01406aff60cc14b6e1dd6c442d7) | 2024-07-30 | Sunil V L | serial: 8250_platform: Enable generic 16550A platform devices |
| [d2e021c5](https://github.com/RVCK-Project/rvck/commit/d2e021c581118cf4c17fbf4caa263661ffc93724) | 2025-04-09 | Song Shuai | riscv: kexec_file: Support loading Image binary file |
| [67e69586](https://github.com/RVCK-Project/rvck/commit/67e6958607d279c1179955843b6a5f4378e08d54) | 2025-07-25 | gaorui | riscv: kexec_file: Split the loading of kernel and others |
| [c2fc0700](https://github.com/RVCK-Project/rvck/commit/c2fc07008e6638684a2eb8a1177ad662b6f874d4) | 2025-07-25 | gaorui | Revert "riscv: kexec: Add image loader for kexec file" |
| [2b830b6c](https://github.com/RVCK-Project/rvck/commit/2b830b6ccd995a2b29269d9b42d2f0b2e083845d) | 2023-11-30 | Samuel Ortiz | RISC-V: Implement archrandom when Zkr is available |
| [c0c6808b](https://github.com/RVCK-Project/rvck/commit/c0c6808b2685f640bf591a7096de15d47cbe4a12) | 2024-02-08 | Sunil V L | cpufreq: Move CPPC configs to common Kconfig and add RISC-V |
| [8f470c8d](https://github.com/RVCK-Project/rvck/commit/8f470c8d283562df7119eead9a6a140dc02eb752) | 2024-02-08 | Sunil V L | ACPI: RISC-V: Add CPPC driver |
| [4a1e8b4a](https://github.com/RVCK-Project/rvck/commit/4a1e8b4af6a14e0d5ebf4aaf2ceb7ce0b1d28497) | 2024-06-17 | Yunhui Cui | RISC-V: Select ACPI PPTT drivers |
| [e4fb33a9](https://github.com/RVCK-Project/rvck/commit/e4fb33a9de83f5b1c02d89dc869f432565bdc1d1) | 2024-05-02 | Sia Jee Heng | RISC-V: ACPI: Enable SPCR table for console output on RISC-V |
| [147ae51f](https://github.com/RVCK-Project/rvck/commit/147ae51f6ae294d986c07ed5be2921e8d305a89e) | 2024-07-18 | Ryo Takakura | RISC-V: Enable IPI CPU Backtrace |
| [ff079d7e](https://github.com/RVCK-Project/rvck/commit/ff079d7ea246809623252419270a44c9eaa30b0d) | 2024-06-13 | Haibo Xu | riscv: dmi: Add SMBIOS/DMI support |
| [a2610fb8](https://github.com/RVCK-Project/rvck/commit/a2610fb822a5c38271d04e3b3288534f31c9480c) | 2024-06-13 | Haibo Xu | ACPI: NUMA: replace pr_info with pr_debug in arch_acpi_numa_init |
| [6d55220c](https://github.com/RVCK-Project/rvck/commit/6d55220c1ed681cf21580c00a0b5ae44b0a3ea4e) | 2025-04-25 | gaorui | ACPI: NUMA: change the ACPI_NUMA to a hidden option |
| [63bbfa17](https://github.com/RVCK-Project/rvck/commit/63bbfa1728449c7a7c20eb99618d37c0a5d43a6d) | 2025-04-25 | gaorui | ACPI: NUMA: Make some NUMA-related functions available for RISC-V |
| [4d6a665f](https://github.com/RVCK-Project/rvck/commit/4d6a665fa59fbe5ee6e9d7e73aa2791a8a0e87c0) | 2024-06-13 | Haibo Xu | ACPI: NUMA: Add handler for SRAT RINTC affinity structure |
| [5aa26722](https://github.com/RVCK-Project/rvck/commit/5aa267220ce42ba9b9fa8d224427ac9abaa59b58) | 2024-06-13 | Haibo Xu | ACPI: RISCV: Add NUMA support based on SRAT and SLIT |
| [d1658d9a](https://github.com/RVCK-Project/rvck/commit/d1658d9a67bcd8173ab7aecea4981166db078122) | 2024-01-17 | Haibo Xu | ACPICA: SRAT: Add RISC-V RINTC affinity structure |
---

**共 252 条提交，显示 201-252**

[1](中兴通讯.md) **[2]**

[显示全部](中兴通讯_all.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-03-02 22:17:57*
*数据来源: 主分支 rvck-6.6@d415e668*
