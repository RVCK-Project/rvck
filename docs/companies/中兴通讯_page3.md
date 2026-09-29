# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 509</li>
<li><strong>统计时间</strong>: 2026-09-29 10:23:47</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.155</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [ba498668](https://github.com/RVCK-Project/rvck/commit/ba4986685fc80df55e209f12ac1b9fff795b100c) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [bec9526b](https://github.com/RVCK-Project/rvck/commit/bec9526b5db45ab2afb8e9a4b2c47e5f2daed8cb) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [5be9d152](https://github.com/RVCK-Project/rvck/commit/5be9d152f500eaf6f5448729efe8c63fce08178b) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [c9b05fb7](https://github.com/RVCK-Project/rvck/commit/c9b05fb7097bd832d2ca0b437d3ab9a32ed32217) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [c358d86f](https://github.com/RVCK-Project/rvck/commit/c358d86ffccdcd69bdc05e48bf3a7daf91ee97b8) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [b559acc2](https://github.com/RVCK-Project/rvck/commit/b559acc28f6f61d47d52595c7023c508c338a4f3) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [859491b1](https://github.com/RVCK-Project/rvck/commit/859491b1af069c1e5a20ee9858e8afa954673a10) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [f8277a94](https://github.com/RVCK-Project/rvck/commit/f8277a949e860ef002cf8a150177659fd25d3b55) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [6932dbac](https://github.com/RVCK-Project/rvck/commit/6932dbac6c916e5645420b8e6b0759c53eb3f4b9) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [9d5c63fb](https://github.com/RVCK-Project/rvck/commit/9d5c63fbff85777b7e3774efd1f7195f97af4c5b) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [7cbb7bf9](https://github.com/RVCK-Project/rvck/commit/7cbb7bf9f2b91e0981f400a6d9c76214c3a92e67) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [b637198b](https://github.com/RVCK-Project/rvck/commit/b637198b2efa1618c68dbe044d5b9427b19eedf6) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [cc805193](https://github.com/RVCK-Project/rvck/commit/cc805193c3dddf54c44e581d3988cdd294dbe209) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [5284a6e2](https://github.com/RVCK-Project/rvck/commit/5284a6e2e5f63674692205814481f545d7f85971) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [bc336f8b](https://github.com/RVCK-Project/rvck/commit/bc336f8b278ec1c98fb9275e24ad7a94d6b6f961) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [fed40dc4](https://github.com/RVCK-Project/rvck/commit/fed40dc489ae696bf0564000b6919b3f1b4ac544) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [91cfef79](https://github.com/RVCK-Project/rvck/commit/91cfef79f0959cdb26dedfe250e3217b67dfab03) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [3135e21e](https://github.com/RVCK-Project/rvck/commit/3135e21eb23b786a7c6997a3be9121b37f3eae72) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [28b72514](https://github.com/RVCK-Project/rvck/commit/28b7251477c008e7369de24224833ecca05ebda5) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [b66619c2](https://github.com/RVCK-Project/rvck/commit/b66619c2b33e41601d1ea7aea1128388d83258f3) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [96528d7f](https://github.com/RVCK-Project/rvck/commit/96528d7f175b15fe70a4f317f46d354a3e028089) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [0948d6ce](https://github.com/RVCK-Project/rvck/commit/0948d6ce904b416e65eb259da64e337384780280) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [f2bde542](https://github.com/RVCK-Project/rvck/commit/f2bde542e3fd16ed44a33b1cfb78bcde1f5d4eb9) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [5e6e1a3d](https://github.com/RVCK-Project/rvck/commit/5e6e1a3d57cd8c699337909a8ec735340075d388) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Add separate ticket-lock.h |
| [06154718](https://github.com/RVCK-Project/rvck/commit/06154718a93b58dd6a0faf62bec15fb715b3272d) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Reuse arch_spinlock_t of qspinlock |
| [ed9fb072](https://github.com/RVCK-Project/rvck/commit/ed9fb0721c3e0203cfdb9021a82b767c67fe1ea6) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [05e5658c](https://github.com/RVCK-Project/rvck/commit/05e5658c626ee972e2e0f5e44fa15de3edf21a65) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [d7100d75](https://github.com/RVCK-Project/rvck/commit/d7100d7553b434e74f745a04eb7fa0bb309819a8) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [bbd18a34](https://github.com/RVCK-Project/rvck/commit/bbd18a34ce1e1254629a4c0180e82e13167cdcd8) | 2023-09-08 | Guo Ren | asm-generic: ticket-lock: Optimize arch_spin_value_unlocked() |
| [30580d90](https://github.com/RVCK-Project/rvck/commit/30580d9090f0ef3a50d059fc92343329e7faad16) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Ziccrse extension for Guest/VM |
| [7ffb8346](https://github.com/RVCK-Project/rvck/commit/7ffb8346f39d3fcfa1eab821a8892fef5aa6036d) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Zabha extension for Guest/VM |
| [89b37fdf](https://github.com/RVCK-Project/rvck/commit/89b37fdf85f9b249d0c3aef19935e6ac1501d4a2) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Svvptc extension for Guest/VM |
| [4cbf25c0](https://github.com/RVCK-Project/rvck/commit/4cbf25c06285f9f5531306bd167df3cd2333485a) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [29a8b020](https://github.com/RVCK-Project/rvck/commit/29a8b02084c5f1b13cab569564453c605a1e19c9) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [b0733330](https://github.com/RVCK-Project/rvck/commit/b0733330382a2a91574651d9dc2d6aaf311e2634) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [fdd7073e](https://github.com/RVCK-Project/rvck/commit/fdd7073e0f225c8a0bd00100933477fe0c5b8e68) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [25b3b104](https://github.com/RVCK-Project/rvck/commit/25b3b10451cb8a83e8a943168fa6f30f65ab5d5d) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [a74af32b](https://github.com/RVCK-Project/rvck/commit/a74af32be6672edd9630772338a0fd411e5d7acb) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [8e14270e](https://github.com/RVCK-Project/rvck/commit/8e14270e8397410367f4bd9404eac93626bc4689) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [a2ecc96b](https://github.com/RVCK-Project/rvck/commit/a2ecc96bea87a66493a1eb5885e96f507a4e6fe7) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [6c53d8fc](https://github.com/RVCK-Project/rvck/commit/6c53d8fc4709897d19777b7bb1db23da0d201f0d) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [360d8356](https://github.com/RVCK-Project/rvck/commit/360d835644010f0597655ef5892af955db30ce6b) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [e7d2a825](https://github.com/RVCK-Project/rvck/commit/e7d2a8258592631843cbc0f80ec747d040c6535d) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [1dd3dbb7](https://github.com/RVCK-Project/rvck/commit/1dd3dbb70d26b2cb1f927c0bd99341e6a11d179e) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [dc7e2080](https://github.com/RVCK-Project/rvck/commit/dc7e2080ed48adebcb360738d99ffb658127b061) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [66ba13c3](https://github.com/RVCK-Project/rvck/commit/66ba13c3b43ac9a3dbca7d565c3c971e9b044f31) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [24e940b0](https://github.com/RVCK-Project/rvck/commit/24e940b053d9c262141ddf1e1da0823e604e0467) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [16c8fa4c](https://github.com/RVCK-Project/rvck/commit/16c8fa4c029bfdc621818b9885f96b443fda3987) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [3eba211c](https://github.com/RVCK-Project/rvck/commit/3eba211ccf5c8dd651159fb7b363a755f3537f06) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [bbc1b36c](https://github.com/RVCK-Project/rvck/commit/bbc1b36c31ffd700cd31f171ed377843c2108298) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [fc9f031c](https://github.com/RVCK-Project/rvck/commit/fc9f031c9e425e5b0618c4fdf54ec6aaa88005f7) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [934de2f1](https://github.com/RVCK-Project/rvck/commit/934de2f14780da8a80bfd4faf268345381fd1672) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [84dbe015](https://github.com/RVCK-Project/rvck/commit/84dbe015ae605f339585d40626a7c3ba8168e679) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [ee0b74e8](https://github.com/RVCK-Project/rvck/commit/ee0b74e831b00ccb281183e7dbae032d73b0666d) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [924bb2a4](https://github.com/RVCK-Project/rvck/commit/924bb2a44eac4ec5885986ab11bea04d8d1ab778) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [1ecfbb9f](https://github.com/RVCK-Project/rvck/commit/1ecfbb9f79386665cd32ef12665efdfd3c385ecb) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [f822742d](https://github.com/RVCK-Project/rvck/commit/f822742d671c335d2daba6985e267e9e4e1427ed) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
| [0924fe0f](https://github.com/RVCK-Project/rvck/commit/0924fe0f8a949457c63be31d283015cfed09cfab) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [3b46893b](https://github.com/RVCK-Project/rvck/commit/3b46893b846c0264290462731900e003fa2accf4) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [86cb3c48](https://github.com/RVCK-Project/rvck/commit/86cb3c4857cd4f1ddf60febb8faa43283b8bb72a) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [0613c123](https://github.com/RVCK-Project/rvck/commit/0613c123daa9f2aa7569f98ea315390c4f12e99a) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [1e7475d8](https://github.com/RVCK-Project/rvck/commit/1e7475d80e0100cee3d7f5931ab678e8c1c407e8) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [5c2572d7](https://github.com/RVCK-Project/rvck/commit/5c2572d77f157b736b1eba1e4fb8068012d34076) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [13d6d92f](https://github.com/RVCK-Project/rvck/commit/13d6d92f477a282e0d33cbeb79032f4e4284d81b) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [ef811cac](https://github.com/RVCK-Project/rvck/commit/ef811cacfcbf5d32c94b2ff4a8800d279713f178) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [59788e1d](https://github.com/RVCK-Project/rvck/commit/59788e1dc8688856bb4e6e4348b1b8c9f5abe34f) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [fd222ae8](https://github.com/RVCK-Project/rvck/commit/fd222ae851a108f5dde4b5efe9291b65497f5ff0) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [2ce0a668](https://github.com/RVCK-Project/rvck/commit/2ce0a668b83a2f994c42e44f81dbb0fc01a4faf9) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [d7da524d](https://github.com/RVCK-Project/rvck/commit/d7da524db26afd441d30e3ed82281a30d913eb8f) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [ebd14022](https://github.com/RVCK-Project/rvck/commit/ebd1402207686079d5bcaeeb1d219b9bb58734c4) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [00300f24](https://github.com/RVCK-Project/rvck/commit/00300f2436e2271fe83f4ff8400208333d68bade) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [4939da99](https://github.com/RVCK-Project/rvck/commit/4939da996f1d6ce2bcd2a90f0c7a01070e4b4f7d) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [679a1244](https://github.com/RVCK-Project/rvck/commit/679a124412f9dcbab54aebf2688a62b7ca06d9f9) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [7c83e76b](https://github.com/RVCK-Project/rvck/commit/7c83e76b4ff7b7479f41cc33a24bf551a4a4ce19) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [dc1b2410](https://github.com/RVCK-Project/rvck/commit/dc1b2410ab7cf8f5ab1c803aa5a06db47520fa17) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [84703f58](https://github.com/RVCK-Project/rvck/commit/84703f586777c80ea6053839359f4cf0c40bcae6) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [0d45c54f](https://github.com/RVCK-Project/rvck/commit/0d45c54ff32315b4ce15411390f9f24c4591bdc7) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
| [8a32574d](https://github.com/RVCK-Project/rvck/commit/8a32574d9456d0eefe0dad3282ecfe8d1d86374e) | 2024-04-09 | Baoquan He | kexec: fix the unexpected kexec_dprintk() macro |
| [cc29b0eb](https://github.com/RVCK-Project/rvck/commit/cc29b0eba2171b6dfd8fe7d664180d397dc1802d) | 2024-07-30 | Sunil V L | kexec_file, parisc: print out debugging message if required |
| [eba5ed9d](https://github.com/RVCK-Project/rvck/commit/eba5ed9d5ec4a506ce8b9068c9de80144efdc0a4) | 2023-12-13 | Baoquan He | kexec_file, power: print out debugging message if required |
| [b93850b2](https://github.com/RVCK-Project/rvck/commit/b93850b22e3c6940a0be684b16b89f0cf6d87021) | 2023-12-13 | Baoquan He | kexec_file, riscv: print out debugging message if required |
| [14a9e50c](https://github.com/RVCK-Project/rvck/commit/14a9e50c3d8339eda9eac6562e918772c622112b) | 2023-12-13 | Baoquan He | kexec_file, arm64: print out debugging message if required |
| [3c368919](https://github.com/RVCK-Project/rvck/commit/3c3689191781f7d230fa196500251d68e8b749a7) | 2023-12-13 | Baoquan He | kexec_file, x86: print out debugging message if required |
| [42a77eaf](https://github.com/RVCK-Project/rvck/commit/42a77eaf2f11c081283d74411e9b8cf120038ef4) | 2023-12-13 | Baoquan He | kexec_file: print out debugging message if required |
| [c60c5f03](https://github.com/RVCK-Project/rvck/commit/c60c5f03d80e3cc8082b20213ecbbb8cc3ae7276) | 2023-12-13 | Baoquan He | kexec_file: add kexec_file flag to control debug printing |
| [73d6aa7f](https://github.com/RVCK-Project/rvck/commit/73d6aa7f4aa35825fa0724fe5b57b5c5a848d0f8) | 2025-04-03 | Radim Krčmář | KVM: RISC-V: reset smstateen CSRs |
| [34cf9013](https://github.com/RVCK-Project/rvck/commit/34cf9013a567e8e2ee97a7d93bf882d1e1ad68a3) | 2023-12-24 | Anup Patel | RISC-V: KVM: Fix indentation in kvm_riscv_vcpu_set_reg_csr() |
| [28b08a85](https://github.com/RVCK-Project/rvck/commit/28b08a85cad71ad17f47368577cd93db1891970b) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 to ONE_REG |
| [d6595ad9](https://github.com/RVCK-Project/rvck/commit/d6595ad9a22e035679b228498ba7df1bb9344abd) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 context save/restore |
| [d8f58da4](https://github.com/RVCK-Project/rvck/commit/d8f58da4870d5e0085d6a898bdb60a1da2f7933f) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add senvcfg context save/restore |
| [f76ccc13](https://github.com/RVCK-Project/rvck/commit/f76ccc138571d8ea1f1340c1fd75736023b01b89) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Enable Smstateen accesses |
| [7237ca11](https://github.com/RVCK-Project/rvck/commit/7237ca11aecc65b6554cf0ccb0e39078e98fadb8) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Add kvm_vcpu_config |
| [45a14a58](https://github.com/RVCK-Project/rvck/commit/45a14a584bfc2fff207758ca257dc6ca379b4ad3) | 2024-07-30 | Sunil V L | serial: 8250_platform: Enable generic 16550A platform devices |
| [28c255bc](https://github.com/RVCK-Project/rvck/commit/28c255bcf0d4d676e2f1d1c66c1b777c7b41183c) | 2025-04-09 | Song Shuai | riscv: kexec_file: Support loading Image binary file |
| [801d3d3c](https://github.com/RVCK-Project/rvck/commit/801d3d3c72ce9024c48bf79ae9dd01ac88799332) | 2025-07-25 | gaorui | riscv: kexec_file: Split the loading of kernel and others |
| [57bba7c7](https://github.com/RVCK-Project/rvck/commit/57bba7c7f5067f03d29da03860e0618284573aab) | 2025-07-25 | gaorui | Revert "riscv: kexec: Add image loader for kexec file" |
| [2e2ccc0e](https://github.com/RVCK-Project/rvck/commit/2e2ccc0ee9d6012a2af0139e37d7b17fb72a850d) | 2023-11-30 | Samuel Ortiz | RISC-V: Implement archrandom when Zkr is available |
| [73e6c9f9](https://github.com/RVCK-Project/rvck/commit/73e6c9f9a2bcb5733e40c7e5e5cf26eee2c6cbf9) | 2024-02-08 | Sunil V L | cpufreq: Move CPPC configs to common Kconfig and add RISC-V |
| [aadc78bd](https://github.com/RVCK-Project/rvck/commit/aadc78bd2e7291045b67e3328ece21d35e5e6812) | 2024-02-08 | Sunil V L | ACPI: RISC-V: Add CPPC driver |
| [2bc9557b](https://github.com/RVCK-Project/rvck/commit/2bc9557bfb576b1e7f4fa08bfa40997127a96d0a) | 2024-06-17 | Yunhui Cui | RISC-V: Select ACPI PPTT drivers |
| [264d78cd](https://github.com/RVCK-Project/rvck/commit/264d78cdfa1a887330baad3f62c667fc49312a18) | 2024-05-02 | Sia Jee Heng | RISC-V: ACPI: Enable SPCR table for console output on RISC-V |
| [3ebaecbb](https://github.com/RVCK-Project/rvck/commit/3ebaecbb7d5ab02d8f9cbc298ffc83aefa20724c) | 2024-07-18 | Ryo Takakura | RISC-V: Enable IPI CPU Backtrace |
| [41873c93](https://github.com/RVCK-Project/rvck/commit/41873c9362dcb1cb25df596575a8c4c47d4e355a) | 2024-06-13 | Haibo Xu | riscv: dmi: Add SMBIOS/DMI support |
| [658ba5a2](https://github.com/RVCK-Project/rvck/commit/658ba5a275ad7985507d6551ba4069232484da21) | 2024-06-13 | Haibo Xu | ACPI: NUMA: replace pr_info with pr_debug in arch_acpi_numa_init |
| [d095cc12](https://github.com/RVCK-Project/rvck/commit/d095cc1229a74e5cee7e589c5538a77707accb8e) | 2025-04-25 | gaorui | ACPI: NUMA: change the ACPI_NUMA to a hidden option |
| [9fe5013d](https://github.com/RVCK-Project/rvck/commit/9fe5013d22f2fb49e9f6185d09787352b0103b39) | 2025-04-25 | gaorui | ACPI: NUMA: Make some NUMA-related functions available for RISC-V |
| [2150a23e](https://github.com/RVCK-Project/rvck/commit/2150a23e52038c0b20a516e957d3111b65826e41) | 2024-06-13 | Haibo Xu | ACPI: NUMA: Add handler for SRAT RINTC affinity structure |
| [e0eacf9f](https://github.com/RVCK-Project/rvck/commit/e0eacf9f6a01a1840a956be2da0783cf6b4b1156) | 2024-06-13 | Haibo Xu | ACPI: RISCV: Add NUMA support based on SRAT and SLIT |
| [48e1d183](https://github.com/RVCK-Project/rvck/commit/48e1d183bed68f477ce4663b59390a9542ccde7b) | 2024-01-17 | Haibo Xu | ACPICA: SRAT: Add RISC-V RINTC affinity structure |
---

**共 509 条提交，显示 401-509**

[1](中兴通讯.md) [2](中兴通讯_page2.md) **[3]**

[显示全部](中兴通讯_all.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-09-29 10:23:47*
*数据来源: 主分支 rvck-6.6@0bee6ebb*
