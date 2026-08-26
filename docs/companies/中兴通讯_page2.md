# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 432</li>
<li><strong>统计时间</strong>: 2026-08-26 13:07:02</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.148</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [b2a3625e](https://github.com/RVCK-Project/rvck/commit/b2a3625e37fe9d78e7155646de5c3c845ddfc21f) | 2023-10-04 | Clément Léger | riscv: report perf event for misaligned fault |
| [90e05858](https://github.com/RVCK-Project/rvck/commit/90e058581aac845083b8fe87ee9a537a6830705e) | 2023-10-04 | Clément Léger | riscv: add support for misaligned trap handling in S-mode |
| [3a7e7f90](https://github.com/RVCK-Project/rvck/commit/3a7e7f90ef44642ab1cd069080c35e1bc79ec6a7) | 2026-01-17 | Lu Peng | riscv: defconfig: Enable more ACPI_APEI configs |
| [f84b0f0f](https://github.com/RVCK-Project/rvck/commit/f84b0f0f2ec952b2d6b662f890d5e2a419a9aa3d) | 2025-07-23 | Ignacio Encinas | riscv: introduce asm/swab.h |
| [51083c76](https://github.com/RVCK-Project/rvck/commit/51083c767ddbd1c074fab20714139e79a162be99) | 2026-01-07 | hu.yuye | riscv:defconfig:Enable PCIE_EDR |
| [bc758cab](https://github.com/RVCK-Project/rvck/commit/bc758cabe9d9b4055c77bca21cb0324a2ddcacc6) | 2025-12-31 | Yunhui Cui | arch_topology: move parse_acpi_topology() to common code |
| [a662b7c5](https://github.com/RVCK-Project/rvck/commit/a662b7c52ced7a65f7cb01a4b4b35e2c32752b70) | 2025-12-31 | Yicong Yang | arm64: topology: Support SMT control on ACPI based system |
| [36f1f4bc](https://github.com/RVCK-Project/rvck/commit/36f1f4bcad2eb8ebe3cbaf952f558550bbf73b9a) | 2025-12-31 | Yicong Yang | arch_topology: Support SMT control for OF based system |
| [051ce298](https://github.com/RVCK-Project/rvck/commit/051ce2986899050e12aca0e8433cc1fcbcd4953f) | 2025-12-31 | Yicong Yang | cpu/SMT: Provide a default topology_is_primary_thread() |
| [e90f62a4](https://github.com/RVCK-Project/rvck/commit/e90f62a433937febe434cadc03cdb9e546d1d28a) | 2025-01-24 | Andy Shevchenko | serial: 8250_core: Remove unneeded -\>iotype assignment |
| [f460f094](https://github.com/RVCK-Project/rvck/commit/f460f094095d92167b5a8e3a8702927486c61c91) | 2025-12-30 | hu.yuye | Revert "mango pci hack:broadcast when no MSI source known" |
| [34322718](https://github.com/RVCK-Project/rvck/commit/3432271880def58ffed200c68315b65b2cd8c87c) | 2025-12-22 | Clément Léger | riscv: uaccess: do not do misaligned accesses in get/put_user() |
| [e8611785](https://github.com/RVCK-Project/rvck/commit/e86117850729e07c1e16c9a29a36b2f4d5e0ddf5) | 2025-12-22 | Alexandre Ghiti | riscv: make unsafe user copy routines use existing assembly routines |
| [87795d62](https://github.com/RVCK-Project/rvck/commit/87795d62334ff1d0420881ad4d2c4df711044062) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm_goto_output' for get_user() |
| [f0b78bc3](https://github.com/RVCK-Project/rvck/commit/f0b78bc3e8665904ad0a9aadccad05d3c1152722) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm goto' for put_user() |
| [2bd13946](https://github.com/RVCK-Project/rvck/commit/2bd13946fbd758a49351937076df79d042d7b2ec) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use input constraints for ptr of __put_user() |
| [599e6210](https://github.com/RVCK-Project/rvck/commit/599e621077b830094e21990ca4a6c8a20a233588) | 2025-12-22 | Jisheng Zhang | riscv: implement user_access_begin() and families |
| [31f00279](https://github.com/RVCK-Project/rvck/commit/31f002797a365cf0068fa595f88601c93f6c7478) | 2025-12-22 | Ben Dooks | riscv: save the SR_SUM status over switches |
| [1a2cdca8](https://github.com/RVCK-Project/rvck/commit/1a2cdca8efe59df17a2582e0cd44c082443bb676) | 2011-12-08 | Tejun Heo | Revert "mm: Modify __find_max_addr for memory hole" |
| [6185e7dd](https://github.com/RVCK-Project/rvck/commit/6185e7dda00561745fbc81c879eaff99336e68e9) | 2021-12-06 | Alexandre Ghiti | Revert "riscv: mm: Clear compilation warning about last_cpupid" |
| [82e0ac31](https://github.com/RVCK-Project/rvck/commit/82e0ac31c1325982f40bd8f8f8477ede51217770) | 2025-11-12 | shenlin | perf vendor events riscv: add lrw core JSON file with metric support |
| [d9b21c3d](https://github.com/RVCK-Project/rvck/commit/d9b21c3de155768c8085bd29c3b7304ba283b8ea) | 2025-11-11 | Fei Liu | i2c: Add driver for the LRW I2C |
| [c5779ba8](https://github.com/RVCK-Project/rvck/commit/c5779ba8137b120e3e62c91df9afaa02a09dab96) | 2025-09-29 | Fei Liu | dt-bindings: i2c: Add binding for LRW I2C |
| [70fe6fdc](https://github.com/RVCK-Project/rvck/commit/70fe6fdc3e9620a85f391622d8d5b9ea311d71ef) | 2025-11-12 | Jie Feng | drivers/perf: add LRW DDR PMU support |
| [6853a5c8](https://github.com/RVCK-Project/rvck/commit/6853a5c824a82f95f43e1a4d3eea9812cb7a2243) | 2025-09-04 | Wenhong Liu | serial: Add driver for the LRW UART |
| [21c231b0](https://github.com/RVCK-Project/rvck/commit/21c231b02f2b066ee63f5e6e28a6b5ad8d56e46b) | 2025-09-04 | Wenhong Liu | dt-bindings: serial: Add binding for LRW UART |
| [35d77459](https://github.com/RVCK-Project/rvck/commit/35d77459f6a5abd85e71a9fd458a9fce2e1d2b1a) | 2025-10-16 | Wenhong Liu | riscv: defconfig: remove CONFIG_CMDLINE and CONFIG_CMDLINE_EXTEND as mainline do... |
| [9a5093ea](https://github.com/RVCK-Project/rvck/commit/9a5093ea474f07d602f9c2a8050aad1c557f349f) | 2025-08-27 | Himanshu Chauhan | riscv: Enable APEI and NMI safe cmpxchg options required for RAS |
| [eb05dea0](https://github.com/RVCK-Project/rvck/commit/eb05dea04dfbbea81261301622e4328a49586995) | 2025-08-27 | Himanshu Chauhan | riscv: Add config option to enable APEI SSE handler |
| [ff3cb07b](https://github.com/RVCK-Project/rvck/commit/ff3cb07b83e8c04390be99b6e62782ce743a2b11) | 2025-08-27 | Himanshu Chauhan | riscv: Introduce HEST SSE notification handlers |
| [4eb2accb](https://github.com/RVCK-Project/rvck/commit/4eb2accb9c4c0b68398f746e6e0506f9abf3d71d) | 2025-08-27 | Himanshu Chauhan | riscv: Add RISC-V entries in processor type and ISA strings |
| [164106d5](https://github.com/RVCK-Project/rvck/commit/164106d5bc4e8f74b196ccf78a3dec54da1482be) | 2025-08-27 | Himanshu Chauhan | riscv: Add functions to register ghes having SSE notification |
| [c34d853f](https://github.com/RVCK-Project/rvck/commit/c34d853fe839cfb7c79ed68d5c157966ac43b367) | 2025-08-27 | Himanshu Chauhan | riscv: conditionally compile GHES NMI spool function |
| [ddf6488d](https://github.com/RVCK-Project/rvck/commit/ddf6488dc97ec30bcec0aec97e3192dd396b7a54) | 2025-08-27 | Himanshu Chauhan | riscv: Add fixmap indices for GHES IRQ and SSE contexts |
| [77626872](https://github.com/RVCK-Project/rvck/commit/7762687294254cf0f64b476009b44bb7d36a245e) | 2025-08-27 | Himanshu Chauhan | acpi: Introduce SSE in HEST notification types |
| [3045c1ba](https://github.com/RVCK-Project/rvck/commit/3045c1ba177871b1040fee84ae62f9e60e8495fe) | 2025-08-27 | Himanshu Chauhan | riscv: Define arch_apei_get_mem_attribute for RISC-V |
| [692bc0cf](https://github.com/RVCK-Project/rvck/commit/692bc0cfd2fcb6fd94ea1256a63ac0b0efde27a3) | 2025-08-27 | Himanshu Chauhan | riscv: Define ioremap_cache for RISC-V |
| [6e720689](https://github.com/RVCK-Project/rvck/commit/6e72068904b84d1b54a7f5486390e4340a17c0a6) | 2025-08-27 | Clément Léger | selftests/riscv: add SSE test module |
| [8579fb20](https://github.com/RVCK-Project/rvck/commit/8579fb200eb9ee4317289601c13947c5455f33be) | 2025-08-27 | Clément Léger | perf: RISC-V: add support for SSE event |
| [c7bd7543](https://github.com/RVCK-Project/rvck/commit/c7bd75434f6b75dedc1a6e95079d6c51deb82d0c) | 2025-08-27 | Clément Léger | drivers: firmware: add riscv SSE support |
| [5fa672c2](https://github.com/RVCK-Project/rvck/commit/5fa672c2bef77c212e00e61604bdbac806eb5fd6) | 2025-08-27 | Clément Léger | riscv: add support for SBI Supervisor Software Events extension |
| [e3ecea66](https://github.com/RVCK-Project/rvck/commit/e3ecea66cee88c5cd392a21636713ac016ccd579) | 2025-08-08 | Clément Léger | riscv: add SBI SSE extension definitions |
| [db14ed6a](https://github.com/RVCK-Project/rvck/commit/db14ed6a500f17699c57f106b7fa434d989edcb1) | 2025-10-14 | Sunil V L | iommu/riscv: Add ACPI support |
| [ce0b200d](https://github.com/RVCK-Project/rvck/commit/ce0b200d9697804e0ca851ae80baf0db9d07570f) | 2025-10-14 | Sunil V L | ACPI: scan: Add support for RISC-V in acpi_iommu_configure_id() |
| [a31d7ca0](https://github.com/RVCK-Project/rvck/commit/a31d7ca0026b69af50cebdcde3c06a07ed4552cb) | 2025-10-14 | Sunil V L | ACPI: RISC-V: Add support for RIMT |
| [d5583c77](https://github.com/RVCK-Project/rvck/commit/d5583c7717b5d847b08a45278b5f9f14e8e62e90) | 2025-10-13 | Sunil V L | ACPICA: actbl2: Add definitions for RIMT |
| [f2c35982](https://github.com/RVCK-Project/rvck/commit/f2c359824dedae48f4fd4e2f9f7ef3d7ad64a7d0) | 2025-09-20 | shenlin | perf vendor events riscv: add lrw core JSON file |
| [0bf5700f](https://github.com/RVCK-Project/rvck/commit/0bf5700ffccaba5a45a7dde8357889355c077b05) | 2025-04-21 | Alexandre Ghiti | riscv: Add support for Zicbop |
| [6e0815f3](https://github.com/RVCK-Project/rvck/commit/6e0815f32c79d799a4a220c948dbf6886325cedd) | 2025-04-21 | Alexandre Ghiti | riscv: Introduce Zicbop instructions |
| [1a1090de](https://github.com/RVCK-Project/rvck/commit/1a1090ded4ece95a2b38aa029969439c8d4464e0) | 2025-02-26 | Yunhui Cui | RISC-V: Enable cbo.clean/flush in usermode |
| [6da49de1](https://github.com/RVCK-Project/rvck/commit/6da49de159f19969d270cd4b2452e996e1c11a32) | 2024-08-14 | Samuel Holland | riscv: Add support for per-thread envcfg CSR values |
| [da300fa8](https://github.com/RVCK-Project/rvck/commit/da300fa86a7e0086dce7481bd25ee67e59cc6a68) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zaamo/Zalrsc extensions for Guest/VM |
| [25fe07e4](https://github.com/RVCK-Project/rvck/commit/25fe07e49acb771a78348f7162223bad49a395d9) | 2024-06-19 | Clément Léger | riscv: hwprobe: export Zaamo and Zalrsc extensions |
| [d6ce74af](https://github.com/RVCK-Project/rvck/commit/d6ce74af35d695c34e64f03bd56d96601f6ce6e1) | 2024-06-19 | Clément Léger | riscv: add parsing for Zaamo and Zalrsc extensions |
| [2668b7fc](https://github.com/RVCK-Project/rvck/commit/2668b7fcc171c8960378c6ac4c5326eb2dd007c2) | 2025-02-13 | Inochi Amaoto | riscv: hwprobe: export bfloat16 ISA extension |
| [0de4f11f](https://github.com/RVCK-Project/rvck/commit/0de4f11f42a3663bcfafde4c4f4ed85d22a0d341) | 2025-02-13 | Inochi Amaoto | riscv: add ISA extension parsing for bfloat16 ISA extension |
| [3262fa69](https://github.com/RVCK-Project/rvck/commit/3262fa69c0db370dac9bc10de707c1d3395fbb12) | 2024-05-24 | Xiao Wang | riscv, bpf: Introduce shift add helper with Zba optimization |
| [bb576f0f](https://github.com/RVCK-Project/rvck/commit/bb576f0f9bb1801ec6cf96d7569bd2f9b98a8da9) | 2024-05-16 | Xiao Wang | riscv, bpf: Optimize zextw insn with Zba extension |
| [19dfa4c8](https://github.com/RVCK-Project/rvck/commit/19dfa4c80751147f6b21541d8fbfab054ccaa43a) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize bswap insns with Zbb support |
| [70274b9e](https://github.com/RVCK-Project/rvck/commit/70274b9ed34ba4861811bcf3027662928f5649cf) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize sign-extention mov insns with Zbb support |
| [02245c70](https://github.com/RVCK-Project/rvck/commit/02245c70fba0aba7c2eb55061231782ecb58b3c7) | 2024-01-15 | Pu Lehui | riscv, bpf: Add necessary Zbb instructions |
| [8d6be801](https://github.com/RVCK-Project/rvck/commit/8d6be8010fab37f219928b96eb1c58b0d8df6efa) | 2024-01-15 | Pu Lehui | riscv, bpf: Simplify sext and zext logics in branch instructions |
| [8b8f5309](https://github.com/RVCK-Project/rvck/commit/8b8f53093925e637d7f969a69da5a6329146be49) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit zero-extension to emit_zextw |
| [54846d2d](https://github.com/RVCK-Project/rvck/commit/54846d2db89858644448af0f1f448cac73e19f9e) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit sign-extension to emit_sextw |
| [5d8adffd](https://github.com/RVCK-Project/rvck/commit/5d8adffd49b53c3404c351f3c44506a15cd24876) | 2025-03-12 | Robin Murphy | iommu: Don't warn prematurely about dodgy probes |
| [a107d30e](https://github.com/RVCK-Project/rvck/commit/a107d30edab28ee16eeead22820c2346a4fc1e0c) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_domain_alloc() |
| [68f994dc](https://github.com/RVCK-Project/rvck/commit/68f994dc20de439da898f0c58e02b892937fd8a6) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_present() |
| [1d4ee3ac](https://github.com/RVCK-Project/rvck/commit/1d4ee3acbd6957cfd2f107402019938453aae4c8) | 2024-09-02 | Lu Baolu | drm/tegra: Use iommu_paging_domain_alloc() |
| [a56415fd](https://github.com/RVCK-Project/rvck/commit/a56415fd3610c1204ebfb54e383bf77aa6d04813) | 2024-09-02 | Lu Baolu | drm/rockchip: Use iommu_paging_domain_alloc() |
| [c04b577a](https://github.com/RVCK-Project/rvck/commit/c04b577abc519986aa6febc856f99540af4d340d) | 2024-06-10 | Lu Baolu | RDMA/usnic: Use iommu_paging_domain_alloc() |
| [bb0b57d5](https://github.com/RVCK-Project/rvck/commit/bb0b57d562d19eaa1dbf7ebd11b3f8c1005b704a) | 2024-08-12 | Lu Baolu | soc: fsl: qbman: Use iommu_paging_domain_alloc() |
| [e7541a2a](https://github.com/RVCK-Project/rvck/commit/e7541a2a01fdc917ca2099496eb9f19e5d7128a8) | 2024-08-12 | Lu Baolu | remoteproc: Use iommu_paging_domain_alloc() |
| [284b4a7d](https://github.com/RVCK-Project/rvck/commit/284b4a7d3430f462617f66f55fcce0b8a8375ddd) | 2024-08-12 | Lu Baolu | media: venus: firmware: Use iommu_paging_domain_alloc() |
| [36167560](https://github.com/RVCK-Project/rvck/commit/3616756089d5f924d28f73206647d632fdf55c2d) | 2024-08-12 | Lu Baolu | media: nvidia: tegra: Use iommu_paging_domain_alloc() |
| [d8c5e065](https://github.com/RVCK-Project/rvck/commit/d8c5e06564ad8a4bfdb17996ebd782525e70d909) | 2024-08-12 | Lu Baolu | gpu: host1x: Use iommu_paging_domain_alloc() |
| [daf284fb](https://github.com/RVCK-Project/rvck/commit/daf284fbe67141fc6651d12c54e076c95e9127fd) | 2024-09-02 | Lu Baolu | drm/nouveau/tegra: Use iommu_paging_domain_alloc() |
| [195b1ab2](https://github.com/RVCK-Project/rvck/commit/195b1ab25f4bdce7838bdac896694a1338f6641b) | 2024-06-10 | Lu Baolu | wifi: ath11k: Use iommu_paging_domain_alloc() |
| [09d24eed](https://github.com/RVCK-Project/rvck/commit/09d24eed113c5ca0fe0d8cabd3939cc7e8cc3e51) | 2024-06-10 | Lu Baolu | wifi: ath10k: Use iommu_paging_domain_alloc() |
| [abef5c35](https://github.com/RVCK-Project/rvck/commit/abef5c3540a1660b0843c75eb736e92dea7182f3) | 2024-06-10 | Lu Baolu | drm/msm: Use iommu_paging_domain_alloc() |
| [5ee6affd](https://github.com/RVCK-Project/rvck/commit/5ee6affd5433c9ae23e850c1e460e58247aa31e5) | 2024-06-10 | Lu Baolu | vhost-vdpa: Use iommu_paging_domain_alloc() |
| [4c5cd712](https://github.com/RVCK-Project/rvck/commit/4c5cd71228f9a462f697d76e6599afaa7478dcb0) | 2024-06-10 | Lu Baolu | vfio/type1: Use iommu_paging_domain_alloc() |
| [29a9791c](https://github.com/RVCK-Project/rvck/commit/29a9791cf6c6a419ccba72fbbf00ad087b8cc509) | 2024-06-10 | Lu Baolu | iommufd: Use iommu_paging_domain_alloc() |
| [308f2e02](https://github.com/RVCK-Project/rvck/commit/308f2e02eab54f4c5419559eb188c85ae11cf717) | 2024-06-10 | Lu Baolu | iommu: Add iommu_paging_domain_alloc() interface |
| [240827dd](https://github.com/RVCK-Project/rvck/commit/240827dd06bc6f5a90dd1a0fabbda27cecebd438) | 2025-02-28 | Robin Murphy | iommu: Get DT/ACPI parsing into the proper probe path |
| [dea68812](https://github.com/RVCK-Project/rvck/commit/dea6881243bdd36aec3f32e900fcd43e6c59c149) | 2025-02-28 | Robin Murphy | iommu: Keep dev-\>iommu state consistent |
| [902a2ea5](https://github.com/RVCK-Project/rvck/commit/902a2ea52ca0229c1db7d1261c44ad3c28ddd44a) | 2025-02-28 | Robin Murphy | iommu: Resolve ops in iommu_init_device() |
| [781720c3](https://github.com/RVCK-Project/rvck/commit/781720c37fe9ea3c26658e947737441298f6b2c5) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [afe9c9b3](https://github.com/RVCK-Project/rvck/commit/afe9c9b3ea76e129c52d57db80fc603f57211b0c) | 2025-02-27 | Robin Murphy | iommu: Unexport iommu_fwspec_free() |
| [5336d787](https://github.com/RVCK-Project/rvck/commit/5336d78707fc8803db619f1bcd8b01daa0445cd3) | 2024-07-02 | Robin Murphy | iommu: Remove iommu_fwspec ops |
| [89128469](https://github.com/RVCK-Project/rvck/commit/89128469241f279a5b5b478dce5ad4b5305f0841) | 2024-07-02 | Robin Murphy | OF: Simplify of_iommu_configure() |
| [9f39fe5e](https://github.com/RVCK-Project/rvck/commit/9f39fe5e42b6ba190ac31420d379dd5425b58218) | 2024-07-02 | Robin Murphy | ACPI: Retire acpi_iommu_fwspec_ops() |
| [4ccd2933](https://github.com/RVCK-Project/rvck/commit/4ccd29331e64b666994e75930a4a680b5d3a0cf2) | 2024-07-02 | Robin Murphy | iommu: Resolve fwspec ops automatically |
| [d2313d39](https://github.com/RVCK-Project/rvck/commit/d2313d39f591c7151274f6c6477930bfec3d31ef) | 2023-12-07 | Jason Gunthorpe | acpi: Do not return struct iommu_ops from acpi_iommu_configure_id() |
| [ede1a175](https://github.com/RVCK-Project/rvck/commit/ede1a175d4fabde2881ecc5837156028678b9e78) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_priv_set() with a lockdep |
| [9619038c](https://github.com/RVCK-Project/rvck/commit/9619038cdfe1d8e52e722f5b78efcdec7e42b1f5) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_get() with lockdep |
| [84589df0](https://github.com/RVCK-Project/rvck/commit/84589df07262ac2445c95ca07e24e295c257e39c) | 2023-12-07 | Jason Gunthorpe | iommu/of: Use -ENODEV consistently in of_iommu_configure() |
| [a40f1040](https://github.com/RVCK-Project/rvck/commit/a40f10409a3ed17e01a8ea82b775053ea56571b1) | 2023-12-07 | Jason Gunthorpe | iommmu/of: Do not return struct iommu_ops from of_iommu_configure() |
| [d22cbaa6](https://github.com/RVCK-Project/rvck/commit/d22cbaa612e90869f7a5bce3860b1872b331f336) | 2023-12-07 | Jason Gunthorpe | iommu: Remove struct iommu_ops *iommu from arch_setup_dma_ops() |
| [b6e7b74a](https://github.com/RVCK-Project/rvck/commit/b6e7b74adff63682b4bf385263fa51df7792d7cb) | 2023-11-21 | Robin Murphy | iommu: Clean up open-coded ownership checks |
| [b348e443](https://github.com/RVCK-Project/rvck/commit/b348e4432ed8362e98a7a01e8e5b141e482572c8) | 2023-11-21 | Robin Murphy | iommu: Retire bus ops |
| [1963af0e](https://github.com/RVCK-Project/rvck/commit/1963af0e98060b87688432c0eab349851bf0aa38) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_domain_alloc() from bus ops |
| [1a18bee6](https://github.com/RVCK-Project/rvck/commit/1a18bee6876997970f49289e95bb80e98606d296) | 2023-11-21 | Robin Murphy | iommu: Validate that devices match domains |
| [fcdea3c4](https://github.com/RVCK-Project/rvck/commit/fcdea3c40ca9d5b109b4783d43508b37794b16bf) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_present() from bus ops |
| [63c370c9](https://github.com/RVCK-Project/rvck/commit/63c370c9df080bb3ec4529de51c4176a136c5124) | 2023-11-21 | Robin Murphy | iommu: Factor out some helpers |
| [1c0b768c](https://github.com/RVCK-Project/rvck/commit/1c0b768c0a0cf4ac7d567506611e173effecf08b) | 2024-03-13 | Xiao Wang | riscv: uaccess: Relax the threshold for fast path |
| [56f65498](https://github.com/RVCK-Project/rvck/commit/56f6549881e9668cea76441f887f688c3d7c8b4a) | 2024-03-13 | Xiao Wang | riscv: uaccess: Allow the last potential unrolled copy |
| [56391ea0](https://github.com/RVCK-Project/rvck/commit/56391ea0ca3ab9e1a1eed3a9e74302dca09ac704) | 2024-12-24 | Atish Patra | RISC-V: KVM: Add new exit statstics for redirected traps |
| [58e82bd0](https://github.com/RVCK-Project/rvck/commit/58e82bd06e15c897d059daeca1fd0ae448969282) | 2024-12-24 | Atish Patra | RISC-V: KVM: Update firmware counters for various events |
| [e379398e](https://github.com/RVCK-Project/rvck/commit/e379398e15a64b4b2334d9cdab72eeae296b72f5) | 2024-04-29 | Yu-Wei Hsu | RISC-V: KVM: Redirect AMO load/store access fault traps to guest |
| [127e556c](https://github.com/RVCK-Project/rvck/commit/127e556cd6a1415f8fecdd872cd6b65677a9f4fc) | 2025-01-03 | Xu Lu | iommu/riscv: Add shutdown function for iommu driver |
| [89c0b50f](https://github.com/RVCK-Project/rvck/commit/89c0b50faf005cec00ea4e0581256ad44583c607) | 2025-01-03 | Xu Lu | iommu/riscv: Empty iommu queue before enabling it |
| [46e76426](https://github.com/RVCK-Project/rvck/commit/46e764268a813c080963dee70e6cf8690623b039) | 2024-11-12 | Andrew Jones | iommu/riscv: Add support for platform msi |
| [686282f7](https://github.com/RVCK-Project/rvck/commit/686282f72216991241a68fa4e7a29e21887f9ec1) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Paging domain support |
| [5c0f0caf](https://github.com/RVCK-Project/rvck/commit/5c0f0caf807e49cc8ef53dcb1bb994c4ddf8d9f0) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Command and fault queue support |
| [474b27d2](https://github.com/RVCK-Project/rvck/commit/474b27d2f8ae19f7b0a7d8c3fbd3e3a26bf6fe8a) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Device directory management. |
| [9eff902f](https://github.com/RVCK-Project/rvck/commit/9eff902f0af17c2a3e5f236668acda7f54b25e47) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Enable IOMMU registration and device probe. |
| [5f6441cd](https://github.com/RVCK-Project/rvck/commit/5f6441cd969d1842b19d64cdb9abd434a13d151e) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU PCIe device driver |
| [1124580e](https://github.com/RVCK-Project/rvck/commit/1124580ee7c8d8793a80108625f3894f24f16867) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU platform device driver |
| [0f7eb0fe](https://github.com/RVCK-Project/rvck/commit/0f7eb0fe7292fcc8536bb7bfff6cc74370ab4db6) | 2024-10-15 | Tomasz Jeznach | dt-bindings: iommu: riscv: Add bindings for RISC-V IOMMU |
| [e75bfde6](https://github.com/RVCK-Project/rvck/commit/e75bfde6537ea690211aa4fea7756ee5e10bf2c6) | 2024-04-13 | Pasha Tatashin | iommu/vt-d: add wrapper functions for page allocations |
| [0fab6a95](https://github.com/RVCK-Project/rvck/commit/0fab6a95cfbd66d02223ce01e117888489e2bc80) | 2024-02-16 | Krzysztof Kozlowski | iommu: constify of_phandle_args in xlate |
| [ccdb6362](https://github.com/RVCK-Project/rvck/commit/ccdb6362dd7706161c620e1080f968c2568eff1c) | 2023-11-22 | Matt Coster | sizes.h: Add entries between SZ_32G and SZ_64T |
| [ba4f00ea](https://github.com/RVCK-Project/rvck/commit/ba4f00ea4c481dbaad6174593d18a9e233629354) | 2024-03-05 | Lu Baolu | iommu: Add static iommu_ops-\>release_domain |
| [5f700119](https://github.com/RVCK-Project/rvck/commit/5f700119c35f073c5f74773450bd4e3b29277dd5) | 2023-09-27 | Jason Gunthorpe | iommufd: Convert to alloc_domain_paging() |
| [f61dc880](https://github.com/RVCK-Project/rvck/commit/f61dc8808b574cc74be1adf6ea19b61fef82e872) | 2024-04-13 | Pasha Tatashin | iommu: Move IOMMU_DOMAIN_BLOCKED global statics to ops-\>blocked_domain |
| [6ecfdf7a](https://github.com/RVCK-Project/rvck/commit/6ecfdf7a3b1d4ac193142445e99285f5a5e8059a) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new userspace mappings with Svvpt... |
| [6c0987bc](https://github.com/RVCK-Project/rvck/commit/6c0987bcef550074b2a460b4d45bb6c5bd7ffbab) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [77f3cac0](https://github.com/RVCK-Project/rvck/commit/77f3cac0e95ef5c9b1d2b10013887fabd9dcda94) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [7f280e8d](https://github.com/RVCK-Project/rvck/commit/7f280e8d2d552348ba12abb17a0dcc3e98365e91) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
| [93ae59ea](https://github.com/RVCK-Project/rvck/commit/93ae59ea2d03c197a3ffe3ea69beceb4793aa56d) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [0ef86b27](https://github.com/RVCK-Project/rvck/commit/0ef86b27d7624751671bcd47665f87dff1f1ab11) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [166fcee4](https://github.com/RVCK-Project/rvck/commit/166fcee4b1cc624a1c3beb8089372bd53b8fdb44) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [d9f176ca](https://github.com/RVCK-Project/rvck/commit/d9f176cad3b1d5633ef79e7e7b207fbefd63d3da) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [38cf24f9](https://github.com/RVCK-Project/rvck/commit/38cf24f94fc1e0c34ad09b0be888e46d20b94eb2) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [0b5ce852](https://github.com/RVCK-Project/rvck/commit/0b5ce852167de34b0da13318327d97807cda9a46) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [d2d6eee1](https://github.com/RVCK-Project/rvck/commit/d2d6eee106032eadea5cd527e2fa62b37a2f21c8) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [4821885a](https://github.com/RVCK-Project/rvck/commit/4821885a560b181c5b45eac26fc3cd7259d2dab3) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [2b594a30](https://github.com/RVCK-Project/rvck/commit/2b594a30840a4e0d3a222c7fdfd25617aaf2b084) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [5ffac9ca](https://github.com/RVCK-Project/rvck/commit/5ffac9ca7fa3da713c4dece93cef86d8eeb29c69) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [fba28844](https://github.com/RVCK-Project/rvck/commit/fba288445cd5724f91733b370c71527e5d2d53ef) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [885bc445](https://github.com/RVCK-Project/rvck/commit/885bc4455b98ff2f0cde36c3ad1aa3cbd17b8b27) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [17d16056](https://github.com/RVCK-Project/rvck/commit/17d16056ffd988eb18f353fc6bacc2731c39df51) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [d4d7bb0d](https://github.com/RVCK-Project/rvck/commit/d4d7bb0d07bbe30f9b67a873e9c35374aac18b84) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [e6353e7d](https://github.com/RVCK-Project/rvck/commit/e6353e7dbd954e63c49397bb7aa23ec2dd9b13d4) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [918881d5](https://github.com/RVCK-Project/rvck/commit/918881d54883a1aea80d1a16764526cafc1a6bd6) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [3c119df7](https://github.com/RVCK-Project/rvck/commit/3c119df7a54fe4801e2ed55c78807a4d665fdd1c) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [eb095b77](https://github.com/RVCK-Project/rvck/commit/eb095b774b29e31e57aa73a26bdf6d2fd26bf1d0) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [13c4e454](https://github.com/RVCK-Project/rvck/commit/13c4e454e67b0892ec788714ee2703f957af0ee5) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [e5e80093](https://github.com/RVCK-Project/rvck/commit/e5e80093fe278f294db33f7dcabb505fd7cfdb8b) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [e98ffeb4](https://github.com/RVCK-Project/rvck/commit/e98ffeb4f0185ad93cebe14d5e2d2dd15cb81568) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [4985b459](https://github.com/RVCK-Project/rvck/commit/4985b459d7f7ab22d9db804bbd786047f84ebab0) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [112fb750](https://github.com/RVCK-Project/rvck/commit/112fb750a310ac421267bc1974ad865b17773cf7) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [23cddd81](https://github.com/RVCK-Project/rvck/commit/23cddd814387956cc2a085b6a63d04834d3d60e6) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [c31de8bb](https://github.com/RVCK-Project/rvck/commit/c31de8bb711cbaf0beaad595fc4026d982b60ea1) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [a572dec0](https://github.com/RVCK-Project/rvck/commit/a572dec077f3b01b48287fe9335bee04f8dbef54) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [426451d8](https://github.com/RVCK-Project/rvck/commit/426451d8789184bf4b727c997a71ba90d584fd55) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [12c2e4a9](https://github.com/RVCK-Project/rvck/commit/12c2e4a9fc72489df89c1b4661290db18537b7ce) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [e2af09be](https://github.com/RVCK-Project/rvck/commit/e2af09becb1c6750ddc5970f8c80a1d66f39b571) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [0b61c6ce](https://github.com/RVCK-Project/rvck/commit/0b61c6ce0bb30c0dd340839b587829113bc8703b) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [d32fb3f6](https://github.com/RVCK-Project/rvck/commit/d32fb3f603919ddbfdbc87ff5b562362edf45758) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [282ed525](https://github.com/RVCK-Project/rvck/commit/282ed525096b595bdd1dd4d0ab7adf12aadfa2b1) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [2ff25bca](https://github.com/RVCK-Project/rvck/commit/2ff25bcab87b7c46006e0e9d287b78061a7e3dbd) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [2654d16b](https://github.com/RVCK-Project/rvck/commit/2654d16bd854aa1693b9a5a80a6df8aa4c9b7731) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [42e2b187](https://github.com/RVCK-Project/rvck/commit/42e2b187ef6a0420c3c07a8f64c37d10a159aaf1) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [c4e19672](https://github.com/RVCK-Project/rvck/commit/c4e19672f75166a5b97fc2c2ad0974650cdd66a2) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [4000739c](https://github.com/RVCK-Project/rvck/commit/4000739c64a8d1cf80e15bdd201a8123658626be) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [87be3f1c](https://github.com/RVCK-Project/rvck/commit/87be3f1c22ca2c2947b281937709556eb1b0ceff) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [ee3a2bc0](https://github.com/RVCK-Project/rvck/commit/ee3a2bc03fc606f3137d0de64cc955b910310acd) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [a833d091](https://github.com/RVCK-Project/rvck/commit/a833d09170e2dbdbd679fbfae396c57064cec799) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [d3c9a5be](https://github.com/RVCK-Project/rvck/commit/d3c9a5beac50ac412a67241758d2e8ed4777d193) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [7789fa79](https://github.com/RVCK-Project/rvck/commit/7789fa798d4c3e51cb231d45b7caea4d60bab754) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [a8fd12df](https://github.com/RVCK-Project/rvck/commit/a8fd12dfadf15ac9675cc1a6f374f848eee1201d) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [be1af223](https://github.com/RVCK-Project/rvck/commit/be1af2238d53fd3265a030f2ef1ca02d9b531fdf) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [f0561162](https://github.com/RVCK-Project/rvck/commit/f0561162043799c5da506c85c04ae9c0c0e60542) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [2a2adcf2](https://github.com/RVCK-Project/rvck/commit/2a2adcf2c0a2abb2ec957f00001efe8497548f58) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [bc444731](https://github.com/RVCK-Project/rvck/commit/bc444731e9d3e9348f6b4b4a9209d32daea970da) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [72f8c986](https://github.com/RVCK-Project/rvck/commit/72f8c98603411857a45bb373164a619945dc3464) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [b3fd3e49](https://github.com/RVCK-Project/rvck/commit/b3fd3e49cf8a9116ab04d1df20550bfed3cc52a1) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [2b7dd9a1](https://github.com/RVCK-Project/rvck/commit/2b7dd9a1cc3b0637f64d96ce90f9489a724f14a5) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [a7e8111a](https://github.com/RVCK-Project/rvck/commit/a7e8111aa33501c7eecd8b5e59afb0976754d952) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
| [c7096476](https://github.com/RVCK-Project/rvck/commit/c709647618eda430f1110961217e7d2d94fa3880) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [73b30c55](https://github.com/RVCK-Project/rvck/commit/73b30c552e5ee48d0457f62837cdf2630e06bca4) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [66afe9cd](https://github.com/RVCK-Project/rvck/commit/66afe9cd158f34ef274489e0b10e10eb2090cac1) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [0fcfca2c](https://github.com/RVCK-Project/rvck/commit/0fcfca2c7ed1401aaf90560448a94317f439b319) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [534df3e3](https://github.com/RVCK-Project/rvck/commit/534df3e32e6223e80f32248ed1a17c57f3f6ba33) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [d73cfa7c](https://github.com/RVCK-Project/rvck/commit/d73cfa7c10ce1c69a88d8556e2faf5f9c5d09dc6) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [3bbde9af](https://github.com/RVCK-Project/rvck/commit/3bbde9af722e56decfd91c609843ec3f852fc48d) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [126c7616](https://github.com/RVCK-Project/rvck/commit/126c76169d5af221dce469daa68076db3f4d49d2) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [238f07d2](https://github.com/RVCK-Project/rvck/commit/238f07d2c3ad846f828cb63ba2c5f742c2ca8eb3) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [3d4da6eb](https://github.com/RVCK-Project/rvck/commit/3d4da6ebd1dd47f92f58eb53e8fe595a8b4a0982) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [a38cab0e](https://github.com/RVCK-Project/rvck/commit/a38cab0e1133f9e438b3f2d01a75964b033388c9) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [8c08923a](https://github.com/RVCK-Project/rvck/commit/8c08923a21a70f4d2e0815ffcf8d5e6875d988a7) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [2b72a435](https://github.com/RVCK-Project/rvck/commit/2b72a435a450e515caf0570ee7eb5ff8eadd1569) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [5d97b974](https://github.com/RVCK-Project/rvck/commit/5d97b974ac7f286b73e99462130ab28711074fd6) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [8ddd2306](https://github.com/RVCK-Project/rvck/commit/8ddd2306d73871ac75ff9ac885ff51e025741b7e) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [385d495a](https://github.com/RVCK-Project/rvck/commit/385d495a545b25a6cf0a8c3935f8e18956b42744) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [d07c48f8](https://github.com/RVCK-Project/rvck/commit/d07c48f8ee896361d4117b0d3fdab0f1f32bb8f3) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [778abe04](https://github.com/RVCK-Project/rvck/commit/778abe04f2147f1e7b6175765ea6cc751399402c) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [8f310e00](https://github.com/RVCK-Project/rvck/commit/8f310e00bbbb0c8dafd20ddae37d3267674dd7a6) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [946babcd](https://github.com/RVCK-Project/rvck/commit/946babcd6a814eebd821864364ac9e4543e53757) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
---

**共 432 条提交，显示 201-400**

[1](中兴通讯.md) **[2]** [3](中兴通讯_page3.md)

[显示全部](中兴通讯_all.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-08-26 13:07:02*
*数据来源: 主分支 rvck-6.6@3cba41cd*
