# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 512</li>
<li><strong>统计时间</strong>: 2026-10-08 19:27:23</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.158</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [644bea27](https://github.com/RVCK-Project/rvck/commit/644bea27bf28737b7d40c24eaf150b4dc4e2c91d) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [8d256a39](https://github.com/RVCK-Project/rvck/commit/8d256a3967c56a7fa381cb448e79e017a83a4a2c) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [fc328f99](https://github.com/RVCK-Project/rvck/commit/fc328f997832defd63ac760b62761905c3f90b84) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
| [05a3c3ee](https://github.com/RVCK-Project/rvck/commit/05a3c3eef91f059b82bce3462707850f865ae174) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [09046a07](https://github.com/RVCK-Project/rvck/commit/09046a07860779fd6f250b191c5632707260ea4b) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [f512d209](https://github.com/RVCK-Project/rvck/commit/f512d2091ef20004d886eab0391ce18342da630f) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [8423832a](https://github.com/RVCK-Project/rvck/commit/8423832aa3322e7b65edcc53870179d1543e729e) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [26ab2565](https://github.com/RVCK-Project/rvck/commit/26ab25655e8c7a4c7eff944f6dacafd46d9dc397) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [1cdc34ea](https://github.com/RVCK-Project/rvck/commit/1cdc34eaf9f35c0eb3085a05bdc8b3889d6100c5) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [6af257ab](https://github.com/RVCK-Project/rvck/commit/6af257aba433d481ea83f48c718d3c427facdfcd) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [abcef8ec](https://github.com/RVCK-Project/rvck/commit/abcef8ec418d1f655205f2357df2a045f2454678) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [6c3ba3b1](https://github.com/RVCK-Project/rvck/commit/6c3ba3b104762a5c72e462a55d7558fdce4939ef) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [db6974da](https://github.com/RVCK-Project/rvck/commit/db6974da86c9b41ba1aa5db4e503215b5c02c378) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [88bf9ee8](https://github.com/RVCK-Project/rvck/commit/88bf9ee858912cf47a31ca8528c85b24ac02920f) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [9a4725fc](https://github.com/RVCK-Project/rvck/commit/9a4725fc1be8144caed7a24b20466683c57c7b43) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [18dbf829](https://github.com/RVCK-Project/rvck/commit/18dbf8295ca03b4f9f00fcafc8a3faea79c0b5c4) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [1f4a5bb8](https://github.com/RVCK-Project/rvck/commit/1f4a5bb8d6e6c65151f9d9d66daded99f5c32bc5) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [5094040f](https://github.com/RVCK-Project/rvck/commit/5094040fa6356c4004f83c243c0471b219267dd3) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [39754836](https://github.com/RVCK-Project/rvck/commit/397548367c255126be7ce27742026950bee348ed) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [7a388f0b](https://github.com/RVCK-Project/rvck/commit/7a388f0b2f7f6cc8ea58a0237b96023e81bb80d0) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [2f013191](https://github.com/RVCK-Project/rvck/commit/2f01319117ca945099056e7608c2c8684f2c8773) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [bf0fd9c0](https://github.com/RVCK-Project/rvck/commit/bf0fd9c046553da3c7234fecec723647f340226a) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [f06a0b43](https://github.com/RVCK-Project/rvck/commit/f06a0b43b5d55307ca98661069b71e90f0481e51) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [bdd86844](https://github.com/RVCK-Project/rvck/commit/bdd868441c98824b94cc68f75aa98b2ac72b283d) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [dbd35bff](https://github.com/RVCK-Project/rvck/commit/dbd35bffb2c4ed1244a7a68807d3986913ef305c) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [0109b963](https://github.com/RVCK-Project/rvck/commit/0109b963c41cc795993aad14f4efdfed5989d127) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [0a198576](https://github.com/RVCK-Project/rvck/commit/0a1985768b9553fe957621770666817460d301b4) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Add separate ticket-lock.h |
| [adb47520](https://github.com/RVCK-Project/rvck/commit/adb47520f22a4f8848c9b2213f99008dedfd1b70) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Reuse arch_spinlock_t of qspinlock |
| [f1f9312d](https://github.com/RVCK-Project/rvck/commit/f1f9312d0b7da2da10dc3c40c000f2fbc6b1f4e0) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [4f673e59](https://github.com/RVCK-Project/rvck/commit/4f673e5966d9747d42db819f31fbf0a2c8a35517) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [f7119d7f](https://github.com/RVCK-Project/rvck/commit/f7119d7f91bdac74a38f67fb3af468af751e7ef5) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [9ad464fb](https://github.com/RVCK-Project/rvck/commit/9ad464fb12202b8ec595e71e76630e36fea8c56e) | 2023-09-08 | Guo Ren | asm-generic: ticket-lock: Optimize arch_spin_value_unlocked() |
| [b9fa1809](https://github.com/RVCK-Project/rvck/commit/b9fa18094d5ad25da9cfb0dad86a11516f09b2b3) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Ziccrse extension for Guest/VM |
| [88d32508](https://github.com/RVCK-Project/rvck/commit/88d32508b29631978f6a0e041864207c03f495b1) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Zabha extension for Guest/VM |
| [a61aa3d9](https://github.com/RVCK-Project/rvck/commit/a61aa3d987188dd3ed6436da8064b7d2825a48ce) | 2024-12-02 | Quan Zhou | RISC-V: KVM: Allow Svvptc extension for Guest/VM |
| [2999c3ad](https://github.com/RVCK-Project/rvck/commit/2999c3ad0d636ad5fad55bbc4c9e10f194c4779b) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [a02fe748](https://github.com/RVCK-Project/rvck/commit/a02fe748083df1240226d34470e3174eff553d3d) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [3c9549eb](https://github.com/RVCK-Project/rvck/commit/3c9549eb877c9fa8d6e7f47cc80323c100267f19) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [7fe72adb](https://github.com/RVCK-Project/rvck/commit/7fe72adb103cee1fab8d7c5d6fa1f97d7eeeb578) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [9a527b7a](https://github.com/RVCK-Project/rvck/commit/9a527b7a9f8424543b7744f6436f1a6711b842bc) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [7a679188](https://github.com/RVCK-Project/rvck/commit/7a679188a91307e3e86a3114b55e6f4f4b6bd7d5) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [b8437632](https://github.com/RVCK-Project/rvck/commit/b84376320109692be109092f9d29ee44dc5735a4) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [f72587d5](https://github.com/RVCK-Project/rvck/commit/f72587d549205772f29c8ac132826c8b461ba48b) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [ddb8d48d](https://github.com/RVCK-Project/rvck/commit/ddb8d48dee77b7ae3a26b7a2838f89452b68757d) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [e1a1c31d](https://github.com/RVCK-Project/rvck/commit/e1a1c31dccbfc076be8350ef1276f7fb042e6a6b) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [eaa77eca](https://github.com/RVCK-Project/rvck/commit/eaa77eca44e031dacfa9723dd6c426f80b02d6a4) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [b427d0e3](https://github.com/RVCK-Project/rvck/commit/b427d0e30db628648c2653b590f2614dbb6d15b4) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [6919a826](https://github.com/RVCK-Project/rvck/commit/6919a826751cc6a47ee6962f727492562b4bec0f) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [4c37e6dd](https://github.com/RVCK-Project/rvck/commit/4c37e6dd4ce6d84fd2e1a6373c3103fb7c943836) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [b0541e44](https://github.com/RVCK-Project/rvck/commit/b0541e4456cb7ba82c86cccd2ae7b8b5a58e9525) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [e9a43fcb](https://github.com/RVCK-Project/rvck/commit/e9a43fcb806cb84b1580002d4ddc03cc04e313d5) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [d333db61](https://github.com/RVCK-Project/rvck/commit/d333db6163533f2b37efa511d88419236652c87d) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [005a386e](https://github.com/RVCK-Project/rvck/commit/005a386e0935a53feb0cbb6f8efee1c9212bed54) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [f7bd6b1d](https://github.com/RVCK-Project/rvck/commit/f7bd6b1d49feeb64377cd1d7758f869b45ac2df7) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [535041e8](https://github.com/RVCK-Project/rvck/commit/535041e8512d336c3c3776f9657eb2fc551a258a) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [c350d6aa](https://github.com/RVCK-Project/rvck/commit/c350d6aaad22b04cc001bd0e2af44fce4c7a8e63) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [7139be11](https://github.com/RVCK-Project/rvck/commit/7139be111885b798fe68d7a12e38bade2edb595d) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [4b744a61](https://github.com/RVCK-Project/rvck/commit/4b744a617c2e6382f713ae35d665b9b95afc8c77) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [54c8cb33](https://github.com/RVCK-Project/rvck/commit/54c8cb333031b1e5a9367c509e358becfe542695) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [3410f0bc](https://github.com/RVCK-Project/rvck/commit/3410f0bcbaa8be67efe0d863bb6e81a890a89166) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
| [479c09f8](https://github.com/RVCK-Project/rvck/commit/479c09f82875a60a6895b319fafe0ead6cd9d2f9) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [67cb901b](https://github.com/RVCK-Project/rvck/commit/67cb901b769917c8bb06ad40f753e66fb4b954bd) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [0a39288f](https://github.com/RVCK-Project/rvck/commit/0a39288fb3e026abdd26d950874fbcfec7d83bac) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [2fa1bb7e](https://github.com/RVCK-Project/rvck/commit/2fa1bb7e710504f3838fafe02bf09d4a29d752f8) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [5ce7676f](https://github.com/RVCK-Project/rvck/commit/5ce7676fef678fdb2aade1832fe2938487a40a51) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [cb754be9](https://github.com/RVCK-Project/rvck/commit/cb754be9dd9c9a3919c8188a619b012cf025c758) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [3bf15e82](https://github.com/RVCK-Project/rvck/commit/3bf15e82f18e8237c67e34e588fcf3b010babfd0) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [080c1d8b](https://github.com/RVCK-Project/rvck/commit/080c1d8bf6e26d7f17535ba8983833c0172f4561) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [baa43bb4](https://github.com/RVCK-Project/rvck/commit/baa43bb4627ab23ce4738842cb6f84659d49a959) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [72456e7d](https://github.com/RVCK-Project/rvck/commit/72456e7de97b73e7a23ce3c09c31c4573dde1abc) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [09e941a5](https://github.com/RVCK-Project/rvck/commit/09e941a5aaf2154b2ded1444434295431af5fa4e) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [1499c691](https://github.com/RVCK-Project/rvck/commit/1499c6914bff326503b693ce1c3c988a11d69f4e) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [4a7f62ec](https://github.com/RVCK-Project/rvck/commit/4a7f62ecdfd5f9d39b8d0abea8793b341aea1718) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [06997490](https://github.com/RVCK-Project/rvck/commit/069974908e033b849302654efee5c8542a26a243) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [4ca47d54](https://github.com/RVCK-Project/rvck/commit/4ca47d5458cd7e694f831ed588a97dd41fd29e95) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [1b08a9e0](https://github.com/RVCK-Project/rvck/commit/1b08a9e07d2936c623e2937b133f0398e362e28b) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [586a52d8](https://github.com/RVCK-Project/rvck/commit/586a52d88ee76a3f6273117d04f9b530b95c9284) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [a2753d92](https://github.com/RVCK-Project/rvck/commit/a2753d9242aa6a296e48e4ab3c5d14b2b6fcbd17) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [78fb1158](https://github.com/RVCK-Project/rvck/commit/78fb1158185c36ca67d266b2e46f710241224144) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [2348e667](https://github.com/RVCK-Project/rvck/commit/2348e667230b936f718508f5065aed6e5ce9b3f0) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
| [142e672e](https://github.com/RVCK-Project/rvck/commit/142e672e44cc73c1d4ec67cf44bc66e121d702bd) | 2024-04-09 | Baoquan He | kexec: fix the unexpected kexec_dprintk() macro |
| [12988592](https://github.com/RVCK-Project/rvck/commit/1298859252328fcf28956a36adb39aa9f70595a0) | 2024-07-30 | Sunil V L | kexec_file, parisc: print out debugging message if required |
| [2dbcda74](https://github.com/RVCK-Project/rvck/commit/2dbcda7426a362fd5df93152a454f6f74eeaff67) | 2023-12-13 | Baoquan He | kexec_file, power: print out debugging message if required |
| [d0b3409f](https://github.com/RVCK-Project/rvck/commit/d0b3409f151bac0c1856b6ce82c55c9e349d2583) | 2023-12-13 | Baoquan He | kexec_file, riscv: print out debugging message if required |
| [55c8c884](https://github.com/RVCK-Project/rvck/commit/55c8c8847b978ecfdd183a7241997bd158b012e6) | 2023-12-13 | Baoquan He | kexec_file, arm64: print out debugging message if required |
| [84932230](https://github.com/RVCK-Project/rvck/commit/84932230b608d6fab8b4f7eb4a7cd3e9251e241b) | 2023-12-13 | Baoquan He | kexec_file, x86: print out debugging message if required |
| [83b16f11](https://github.com/RVCK-Project/rvck/commit/83b16f118e24d83ef1a1335be02dcbcce1f69d4f) | 2023-12-13 | Baoquan He | kexec_file: print out debugging message if required |
| [cc4e93fc](https://github.com/RVCK-Project/rvck/commit/cc4e93fc434d3affecb97440673f354f8e5a98e7) | 2023-12-13 | Baoquan He | kexec_file: add kexec_file flag to control debug printing |
| [f44d6008](https://github.com/RVCK-Project/rvck/commit/f44d6008d5b07f8bd0c22f37709a5a97518bc0fd) | 2025-04-03 | Radim Krčmář | KVM: RISC-V: reset smstateen CSRs |
| [ec3e196d](https://github.com/RVCK-Project/rvck/commit/ec3e196d1501a5d79f08631f9da0780195ef4337) | 2023-12-24 | Anup Patel | RISC-V: KVM: Fix indentation in kvm_riscv_vcpu_set_reg_csr() |
| [6e91c406](https://github.com/RVCK-Project/rvck/commit/6e91c40609bf40b968a08f4c62e73b3a4e6318a3) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 to ONE_REG |
| [3bdcbf5d](https://github.com/RVCK-Project/rvck/commit/3bdcbf5d6731636ee50662c464c169bb00b6cb5f) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 context save/restore |
| [ff1e657e](https://github.com/RVCK-Project/rvck/commit/ff1e657e6eb4c70aa96be60a99eae2a79fa44b78) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add senvcfg context save/restore |
| [bf3bc84a](https://github.com/RVCK-Project/rvck/commit/bf3bc84ad79c52f6ae364ad06ed674a700b29ecb) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Enable Smstateen accesses |
| [3ab72afb](https://github.com/RVCK-Project/rvck/commit/3ab72afb807107ee1f4188f2656da4dad4d5ff2d) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Add kvm_vcpu_config |
| [2b740360](https://github.com/RVCK-Project/rvck/commit/2b740360be46f12ecae47ffdbd987ebb8720b0f2) | 2024-07-30 | Sunil V L | serial: 8250_platform: Enable generic 16550A platform devices |
| [252eefb0](https://github.com/RVCK-Project/rvck/commit/252eefb00d69afb3849106ef5c9d7105ce317712) | 2025-04-09 | Song Shuai | riscv: kexec_file: Support loading Image binary file |
| [15102d00](https://github.com/RVCK-Project/rvck/commit/15102d00a99c9f84bce99d82454e51d66e5c4316) | 2025-07-25 | gaorui | riscv: kexec_file: Split the loading of kernel and others |
| [adb68981](https://github.com/RVCK-Project/rvck/commit/adb689815c08470a14eeae685c6b62b38e94b45d) | 2025-07-25 | gaorui | Revert "riscv: kexec: Add image loader for kexec file" |
| [238208c0](https://github.com/RVCK-Project/rvck/commit/238208c0960ba7b0f556dc624dc6de346f0339d0) | 2023-11-30 | Samuel Ortiz | RISC-V: Implement archrandom when Zkr is available |
| [e9e889ed](https://github.com/RVCK-Project/rvck/commit/e9e889ed312799700ef9cea3fbaa6a141d7d9c0b) | 2024-02-08 | Sunil V L | cpufreq: Move CPPC configs to common Kconfig and add RISC-V |
| [0c816098](https://github.com/RVCK-Project/rvck/commit/0c816098fed964c03e46d39be8625d85c3c387bf) | 2024-02-08 | Sunil V L | ACPI: RISC-V: Add CPPC driver |
| [6244e603](https://github.com/RVCK-Project/rvck/commit/6244e603b0bc586a9ba00509b01d904e0471266f) | 2024-06-17 | Yunhui Cui | RISC-V: Select ACPI PPTT drivers |
| [7f780656](https://github.com/RVCK-Project/rvck/commit/7f7806563ff3c01d77bd1e585f82a3fe3af04087) | 2024-05-02 | Sia Jee Heng | RISC-V: ACPI: Enable SPCR table for console output on RISC-V |
| [b3be4543](https://github.com/RVCK-Project/rvck/commit/b3be4543383e6978841ab77349dc05066169899c) | 2024-07-18 | Ryo Takakura | RISC-V: Enable IPI CPU Backtrace |
| [06a97df9](https://github.com/RVCK-Project/rvck/commit/06a97df9058993fd7f15751fe9d1a3544056ce99) | 2024-06-13 | Haibo Xu | riscv: dmi: Add SMBIOS/DMI support |
| [0116b3c8](https://github.com/RVCK-Project/rvck/commit/0116b3c88842feda6f54caf507949fa941749dad) | 2024-06-13 | Haibo Xu | ACPI: NUMA: replace pr_info with pr_debug in arch_acpi_numa_init |
| [d20ff09b](https://github.com/RVCK-Project/rvck/commit/d20ff09b3fa9acccd0fd884f43c729f6377e9228) | 2025-04-25 | gaorui | ACPI: NUMA: change the ACPI_NUMA to a hidden option |
| [5528a049](https://github.com/RVCK-Project/rvck/commit/5528a049711489c3512366bf0ee5943377fede0c) | 2025-04-25 | gaorui | ACPI: NUMA: Make some NUMA-related functions available for RISC-V |
| [e13f3e8b](https://github.com/RVCK-Project/rvck/commit/e13f3e8b466e4723d9461bf65ba3e78b2396c717) | 2024-06-13 | Haibo Xu | ACPI: NUMA: Add handler for SRAT RINTC affinity structure |
| [6cc8e413](https://github.com/RVCK-Project/rvck/commit/6cc8e413951366cf096e69896df44560be7ef7fa) | 2024-06-13 | Haibo Xu | ACPI: RISCV: Add NUMA support based on SRAT and SLIT |
| [6231b91a](https://github.com/RVCK-Project/rvck/commit/6231b91ada62da690799b337eb71cd60825d03b2) | 2024-01-17 | Haibo Xu | ACPICA: SRAT: Add RISC-V RINTC affinity structure |
---

**共 512 条提交，显示 401-512**

[1](中兴通讯.md) [2](中兴通讯_page2.md) **[3]**

[显示全部](中兴通讯_all.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-10-08 19:27:23*
*数据来源: 主分支 rvck-6.6@07927e13*
