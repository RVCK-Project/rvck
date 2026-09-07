# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 432</li>
<li><strong>统计时间</strong>: 2026-09-07 16:23:15</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.148</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [b9d04e67](https://github.com/RVCK-Project/rvck/commit/b9d04e670f2b7fd1d2e165d84e4e691c48ba10fe) | 2023-10-04 | Clément Léger | riscv: report perf event for misaligned fault |
| [0a9586f1](https://github.com/RVCK-Project/rvck/commit/0a9586f114cef55d610912b04578dfa508d080ed) | 2023-10-04 | Clément Léger | riscv: add support for misaligned trap handling in S-mode |
| [648cbf0f](https://github.com/RVCK-Project/rvck/commit/648cbf0fb2fe1d6c915ec80004ac88c08363a2d6) | 2026-01-17 | Lu Peng | riscv: defconfig: Enable more ACPI_APEI configs |
| [989e1d47](https://github.com/RVCK-Project/rvck/commit/989e1d47a31fe9870fe70ba3a281e0a23cabcca0) | 2025-07-23 | Ignacio Encinas | riscv: introduce asm/swab.h |
| [3f8cbebc](https://github.com/RVCK-Project/rvck/commit/3f8cbebceaddcfef848448096347e67d7e096dac) | 2026-01-07 | hu.yuye | riscv:defconfig:Enable PCIE_EDR |
| [b89225bc](https://github.com/RVCK-Project/rvck/commit/b89225bc24a51dced7b905647a21589f8b0babca) | 2025-12-31 | Yunhui Cui | arch_topology: move parse_acpi_topology() to common code |
| [98915277](https://github.com/RVCK-Project/rvck/commit/98915277d2f1c4713bcc3cb08bc43ca5c955f736) | 2025-12-31 | Yicong Yang | arm64: topology: Support SMT control on ACPI based system |
| [0b86ba70](https://github.com/RVCK-Project/rvck/commit/0b86ba7076878a7a586fbfcab2528433f8ca872e) | 2025-12-31 | Yicong Yang | arch_topology: Support SMT control for OF based system |
| [7dfc4ed4](https://github.com/RVCK-Project/rvck/commit/7dfc4ed488fceb75e9960b12d29b7ecf09ff6ccf) | 2025-12-31 | Yicong Yang | cpu/SMT: Provide a default topology_is_primary_thread() |
| [6f25c1f6](https://github.com/RVCK-Project/rvck/commit/6f25c1f64663b8d88cee5e04092599790d2ad68c) | 2025-01-24 | Andy Shevchenko | serial: 8250_core: Remove unneeded -\>iotype assignment |
| [ad018464](https://github.com/RVCK-Project/rvck/commit/ad0184646f0cec4146d019cb504889ea3094e6a2) | 2025-12-30 | hu.yuye | Revert "mango pci hack:broadcast when no MSI source known" |
| [031a2194](https://github.com/RVCK-Project/rvck/commit/031a219405652cc74120409e1251a31b80683cfb) | 2025-12-22 | Clément Léger | riscv: uaccess: do not do misaligned accesses in get/put_user() |
| [e804dead](https://github.com/RVCK-Project/rvck/commit/e804dead4256f99d5b313a114941ea27203aa662) | 2025-12-22 | Alexandre Ghiti | riscv: make unsafe user copy routines use existing assembly routines |
| [81c7e8d8](https://github.com/RVCK-Project/rvck/commit/81c7e8d8f69403b9513befad93cd4105c10bf2f4) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm_goto_output' for get_user() |
| [ad411c68](https://github.com/RVCK-Project/rvck/commit/ad411c68ce5500aa2c0e43d793a09953c64e3f0f) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm goto' for put_user() |
| [6a166b46](https://github.com/RVCK-Project/rvck/commit/6a166b46ae4b57a353e17724d39b6c0554df5709) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use input constraints for ptr of __put_user() |
| [1cf91b87](https://github.com/RVCK-Project/rvck/commit/1cf91b8715390b2d8ed08c24119cffaa0eb33313) | 2025-12-22 | Jisheng Zhang | riscv: implement user_access_begin() and families |
| [40030969](https://github.com/RVCK-Project/rvck/commit/400309696b9be4ba6db3fa985d1987c1444cc993) | 2025-12-22 | Ben Dooks | riscv: save the SR_SUM status over switches |
| [913d927f](https://github.com/RVCK-Project/rvck/commit/913d927f1fe8c1d4025ce1a24ed66184f0ca78cf) | 2011-12-08 | Tejun Heo | Revert "mm: Modify __find_max_addr for memory hole" |
| [8baed5b7](https://github.com/RVCK-Project/rvck/commit/8baed5b753084a2c50999147079b3fc87df6a3c3) | 2021-12-06 | Alexandre Ghiti | Revert "riscv: mm: Clear compilation warning about last_cpupid" |
| [bb77e911](https://github.com/RVCK-Project/rvck/commit/bb77e911aea3def5364ee8debd0de6c01ba2125d) | 2025-11-12 | shenlin | perf vendor events riscv: add lrw core JSON file with metric support |
| [81cfc9aa](https://github.com/RVCK-Project/rvck/commit/81cfc9aa93ae9498eb428a2f565f2efadd25a2f4) | 2025-11-11 | Fei Liu | i2c: Add driver for the LRW I2C |
| [db8e354e](https://github.com/RVCK-Project/rvck/commit/db8e354e1dada7609a6f7d95d0f55ddfff19c619) | 2025-09-29 | Fei Liu | dt-bindings: i2c: Add binding for LRW I2C |
| [fdcc4001](https://github.com/RVCK-Project/rvck/commit/fdcc4001e5271fa0922c9cc5fb138bf197dc71a1) | 2025-11-12 | Jie Feng | drivers/perf: add LRW DDR PMU support |
| [d3a0b1f5](https://github.com/RVCK-Project/rvck/commit/d3a0b1f51db56f881632aedb7b711310eb3c74ad) | 2025-09-04 | Wenhong Liu | serial: Add driver for the LRW UART |
| [992f63e7](https://github.com/RVCK-Project/rvck/commit/992f63e724eab74c2a696398d834807ba7506212) | 2025-09-04 | Wenhong Liu | dt-bindings: serial: Add binding for LRW UART |
| [f1f3b690](https://github.com/RVCK-Project/rvck/commit/f1f3b6901ab530ca33f22ed4a3423edf9301e3c5) | 2025-10-16 | Wenhong Liu | riscv: defconfig: remove CONFIG_CMDLINE and CONFIG_CMDLINE_EXTEND as mainline do... |
| [0efc44af](https://github.com/RVCK-Project/rvck/commit/0efc44af17923933bdc284b391fcd56b70bb4879) | 2025-08-27 | Himanshu Chauhan | riscv: Enable APEI and NMI safe cmpxchg options required for RAS |
| [ba8b3c84](https://github.com/RVCK-Project/rvck/commit/ba8b3c84e649bdf93c2788611dedaa2e9c39102e) | 2025-08-27 | Himanshu Chauhan | riscv: Add config option to enable APEI SSE handler |
| [4d32578e](https://github.com/RVCK-Project/rvck/commit/4d32578ea63adb3af0dd200c2e3db08a1aadca38) | 2025-08-27 | Himanshu Chauhan | riscv: Introduce HEST SSE notification handlers |
| [5b3e7581](https://github.com/RVCK-Project/rvck/commit/5b3e758106189feaebe8c6be3682fd0307b52415) | 2025-08-27 | Himanshu Chauhan | riscv: Add RISC-V entries in processor type and ISA strings |
| [82c340f8](https://github.com/RVCK-Project/rvck/commit/82c340f8d4e16cfef38399e8203b7a73e048594c) | 2025-08-27 | Himanshu Chauhan | riscv: Add functions to register ghes having SSE notification |
| [a79ae759](https://github.com/RVCK-Project/rvck/commit/a79ae75930f85ac41337818148182207cb1e80f1) | 2025-08-27 | Himanshu Chauhan | riscv: conditionally compile GHES NMI spool function |
| [1e74705a](https://github.com/RVCK-Project/rvck/commit/1e74705a33d819421c11f77f41abc9123125d15d) | 2025-08-27 | Himanshu Chauhan | riscv: Add fixmap indices for GHES IRQ and SSE contexts |
| [5d7d2dd0](https://github.com/RVCK-Project/rvck/commit/5d7d2dd0c27b68c0185d443202f4052352d908e7) | 2025-08-27 | Himanshu Chauhan | acpi: Introduce SSE in HEST notification types |
| [25eb9f2e](https://github.com/RVCK-Project/rvck/commit/25eb9f2eb4eba8e8b68c2fbfea573359c3f5ba7f) | 2025-08-27 | Himanshu Chauhan | riscv: Define arch_apei_get_mem_attribute for RISC-V |
| [c56c762b](https://github.com/RVCK-Project/rvck/commit/c56c762ba5a7e8be42e8621258e692dc7b88a671) | 2025-08-27 | Himanshu Chauhan | riscv: Define ioremap_cache for RISC-V |
| [5f0aad5a](https://github.com/RVCK-Project/rvck/commit/5f0aad5adf10f232eb36f745dd2625f065dc26cb) | 2025-08-27 | Clément Léger | selftests/riscv: add SSE test module |
| [b21320fa](https://github.com/RVCK-Project/rvck/commit/b21320fa87415b7efae3815244249036e4f63b46) | 2025-08-27 | Clément Léger | perf: RISC-V: add support for SSE event |
| [d0409839](https://github.com/RVCK-Project/rvck/commit/d040983982a090465123c3a1a495891e76b9a14f) | 2025-08-27 | Clément Léger | drivers: firmware: add riscv SSE support |
| [1900448d](https://github.com/RVCK-Project/rvck/commit/1900448d177473c29404d7d3bc2a0f27efff66af) | 2025-08-27 | Clément Léger | riscv: add support for SBI Supervisor Software Events extension |
| [ec1132ea](https://github.com/RVCK-Project/rvck/commit/ec1132ea8da738eebd1a0b5d1bfe86794c096076) | 2025-08-08 | Clément Léger | riscv: add SBI SSE extension definitions |
| [64810910](https://github.com/RVCK-Project/rvck/commit/64810910c4fae50323d6525104974b3f81d97045) | 2025-10-14 | Sunil V L | iommu/riscv: Add ACPI support |
| [b5d95fc5](https://github.com/RVCK-Project/rvck/commit/b5d95fc557786ead81202256785674219abefc66) | 2025-10-14 | Sunil V L | ACPI: scan: Add support for RISC-V in acpi_iommu_configure_id() |
| [1e210453](https://github.com/RVCK-Project/rvck/commit/1e210453bfa30af3e1eba38f452cbffdf421dc84) | 2025-10-14 | Sunil V L | ACPI: RISC-V: Add support for RIMT |
| [8340c3b3](https://github.com/RVCK-Project/rvck/commit/8340c3b3fff916f157d23700313d3be0d0c8a951) | 2025-10-13 | Sunil V L | ACPICA: actbl2: Add definitions for RIMT |
| [bf311a32](https://github.com/RVCK-Project/rvck/commit/bf311a326abe4fb62522c024c536c83d73dd56bd) | 2025-09-20 | shenlin | perf vendor events riscv: add lrw core JSON file |
| [fbcba60f](https://github.com/RVCK-Project/rvck/commit/fbcba60f62609ca549a09fcc8f2139ee71571948) | 2025-04-21 | Alexandre Ghiti | riscv: Add support for Zicbop |
| [eb757a93](https://github.com/RVCK-Project/rvck/commit/eb757a931e8cd8244e99c155ce9de57cb17ec900) | 2025-04-21 | Alexandre Ghiti | riscv: Introduce Zicbop instructions |
| [6c55e184](https://github.com/RVCK-Project/rvck/commit/6c55e1843e5021337b06475d8358a44dc68c7ca2) | 2025-02-26 | Yunhui Cui | RISC-V: Enable cbo.clean/flush in usermode |
| [9e36e5cc](https://github.com/RVCK-Project/rvck/commit/9e36e5cce59f169e03ce7290593c7bc5a7ee9f05) | 2024-08-14 | Samuel Holland | riscv: Add support for per-thread envcfg CSR values |
| [1f0b454b](https://github.com/RVCK-Project/rvck/commit/1f0b454b4dbe78aba56a8ada9edad787718e93c2) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zaamo/Zalrsc extensions for Guest/VM |
| [26975721](https://github.com/RVCK-Project/rvck/commit/26975721c51555e1bb3c315bba26c340c58af892) | 2024-06-19 | Clément Léger | riscv: hwprobe: export Zaamo and Zalrsc extensions |
| [2bf0abb7](https://github.com/RVCK-Project/rvck/commit/2bf0abb7bb74f223ac504d130d6a3e5fcd98b30c) | 2024-06-19 | Clément Léger | riscv: add parsing for Zaamo and Zalrsc extensions |
| [20aefbbb](https://github.com/RVCK-Project/rvck/commit/20aefbbbb62876f33751d60f9f35e6e346f4c2d7) | 2025-02-13 | Inochi Amaoto | riscv: hwprobe: export bfloat16 ISA extension |
| [27f80a3e](https://github.com/RVCK-Project/rvck/commit/27f80a3ef64cdd5fe9dc55d45b6dd2a656e07f0f) | 2025-02-13 | Inochi Amaoto | riscv: add ISA extension parsing for bfloat16 ISA extension |
| [bbb460c1](https://github.com/RVCK-Project/rvck/commit/bbb460c1b2ad7cb113ac5de0e0226793d0a2a383) | 2024-05-24 | Xiao Wang | riscv, bpf: Introduce shift add helper with Zba optimization |
| [b8d7a0bc](https://github.com/RVCK-Project/rvck/commit/b8d7a0bccd43d3a4519c452df2788b4f7ea4ef71) | 2024-05-16 | Xiao Wang | riscv, bpf: Optimize zextw insn with Zba extension |
| [4b49e2ea](https://github.com/RVCK-Project/rvck/commit/4b49e2ea143be262122080fcfd5a15211a1c65a1) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize bswap insns with Zbb support |
| [c817e186](https://github.com/RVCK-Project/rvck/commit/c817e186ac7926a9a6e27bd346592bdf28b14df9) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize sign-extention mov insns with Zbb support |
| [a7680b8f](https://github.com/RVCK-Project/rvck/commit/a7680b8f9f24a6240e8be6f6bb98f25d93d7f006) | 2024-01-15 | Pu Lehui | riscv, bpf: Add necessary Zbb instructions |
| [b8942eff](https://github.com/RVCK-Project/rvck/commit/b8942eff5de2df308cd24bbd79c578fcd8df038f) | 2024-01-15 | Pu Lehui | riscv, bpf: Simplify sext and zext logics in branch instructions |
| [5d694d2a](https://github.com/RVCK-Project/rvck/commit/5d694d2a6e20bdffa526c43e0bfb18c15f82a32c) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit zero-extension to emit_zextw |
| [b76aaf9b](https://github.com/RVCK-Project/rvck/commit/b76aaf9b3d1767718bd22a58a66d3b111ba6d49e) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit sign-extension to emit_sextw |
| [cd6039ec](https://github.com/RVCK-Project/rvck/commit/cd6039ecf4af4e25045425c520fb5936aa0d2a4a) | 2025-03-12 | Robin Murphy | iommu: Don't warn prematurely about dodgy probes |
| [839611f4](https://github.com/RVCK-Project/rvck/commit/839611f40f31eec09de87a721f3e452b7600f340) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_domain_alloc() |
| [6301d796](https://github.com/RVCK-Project/rvck/commit/6301d796080e245a7ccb9e2503d1971cbccf3204) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_present() |
| [71efdcc8](https://github.com/RVCK-Project/rvck/commit/71efdcc8957881b42e862e63adfd29beffa44a22) | 2024-09-02 | Lu Baolu | drm/tegra: Use iommu_paging_domain_alloc() |
| [99ac413d](https://github.com/RVCK-Project/rvck/commit/99ac413d788e8108bda3a75957b7a48372f05f70) | 2024-09-02 | Lu Baolu | drm/rockchip: Use iommu_paging_domain_alloc() |
| [69485253](https://github.com/RVCK-Project/rvck/commit/6948525331425e554cb33263fe3a5b42550f89ad) | 2024-06-10 | Lu Baolu | RDMA/usnic: Use iommu_paging_domain_alloc() |
| [cc539205](https://github.com/RVCK-Project/rvck/commit/cc539205f3104c78b05b4653a4bae3901b06ac44) | 2024-08-12 | Lu Baolu | soc: fsl: qbman: Use iommu_paging_domain_alloc() |
| [9571a62c](https://github.com/RVCK-Project/rvck/commit/9571a62c98854705df53e773f4af349c0658ef5d) | 2024-08-12 | Lu Baolu | remoteproc: Use iommu_paging_domain_alloc() |
| [c2cea553](https://github.com/RVCK-Project/rvck/commit/c2cea55386f92d7836688138ff138a39bfaac968) | 2024-08-12 | Lu Baolu | media: venus: firmware: Use iommu_paging_domain_alloc() |
| [cf9c7827](https://github.com/RVCK-Project/rvck/commit/cf9c78277ecdbb09d094356a0a56c9e428459342) | 2024-08-12 | Lu Baolu | media: nvidia: tegra: Use iommu_paging_domain_alloc() |
| [6e087105](https://github.com/RVCK-Project/rvck/commit/6e08710533caaf6c5484fb01b15be2d4a44321ec) | 2024-08-12 | Lu Baolu | gpu: host1x: Use iommu_paging_domain_alloc() |
| [c8a8cabf](https://github.com/RVCK-Project/rvck/commit/c8a8cabf440393bb2b0706d6c672a397231a2f2b) | 2024-09-02 | Lu Baolu | drm/nouveau/tegra: Use iommu_paging_domain_alloc() |
| [beab98bd](https://github.com/RVCK-Project/rvck/commit/beab98bdbe0bbb468bac10409e87e0e18692983b) | 2024-06-10 | Lu Baolu | wifi: ath11k: Use iommu_paging_domain_alloc() |
| [7e77f939](https://github.com/RVCK-Project/rvck/commit/7e77f939a5fa40337245e507a80de499611709bd) | 2024-06-10 | Lu Baolu | wifi: ath10k: Use iommu_paging_domain_alloc() |
| [7906b8b5](https://github.com/RVCK-Project/rvck/commit/7906b8b5074d7f332114898be24901b9c3e579a9) | 2024-06-10 | Lu Baolu | drm/msm: Use iommu_paging_domain_alloc() |
| [ac3c7bde](https://github.com/RVCK-Project/rvck/commit/ac3c7bde3235946ec340e183c4f5fbe5cd6e97bb) | 2024-06-10 | Lu Baolu | vhost-vdpa: Use iommu_paging_domain_alloc() |
| [bc408eb4](https://github.com/RVCK-Project/rvck/commit/bc408eb4f7b53904d049893830097c6df5940e16) | 2024-06-10 | Lu Baolu | vfio/type1: Use iommu_paging_domain_alloc() |
| [e9596b5c](https://github.com/RVCK-Project/rvck/commit/e9596b5c220fb1e298f6564f48c51267eeb14309) | 2024-06-10 | Lu Baolu | iommufd: Use iommu_paging_domain_alloc() |
| [0f33e98f](https://github.com/RVCK-Project/rvck/commit/0f33e98f2129a7ce3e8660ce63a77afa16026571) | 2024-06-10 | Lu Baolu | iommu: Add iommu_paging_domain_alloc() interface |
| [f8423f5e](https://github.com/RVCK-Project/rvck/commit/f8423f5ea9a0a4b7d425669957c7b399453a515b) | 2025-02-28 | Robin Murphy | iommu: Get DT/ACPI parsing into the proper probe path |
| [0ce2c3ec](https://github.com/RVCK-Project/rvck/commit/0ce2c3ec5840be6f765fb8c9ccff8dd6379d9252) | 2025-02-28 | Robin Murphy | iommu: Keep dev-\>iommu state consistent |
| [9aa94dab](https://github.com/RVCK-Project/rvck/commit/9aa94dab9a8792d9801f98212b191829d5313a9b) | 2025-02-28 | Robin Murphy | iommu: Resolve ops in iommu_init_device() |
| [9095eb9b](https://github.com/RVCK-Project/rvck/commit/9095eb9b7187ae3437b80457c917d068240e11c1) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [897b572e](https://github.com/RVCK-Project/rvck/commit/897b572e74a9facb73e95cc7263aaf38331e1e0f) | 2025-02-27 | Robin Murphy | iommu: Unexport iommu_fwspec_free() |
| [fdd5f0a9](https://github.com/RVCK-Project/rvck/commit/fdd5f0a994432eb2ae870d5110d1a16d397c9830) | 2024-07-02 | Robin Murphy | iommu: Remove iommu_fwspec ops |
| [751bed38](https://github.com/RVCK-Project/rvck/commit/751bed38f09ec2b0b80d8b16e962104b90b196fb) | 2024-07-02 | Robin Murphy | OF: Simplify of_iommu_configure() |
| [6ad41c4a](https://github.com/RVCK-Project/rvck/commit/6ad41c4a83adfcb13a5f06074ce106498ae5e19d) | 2024-07-02 | Robin Murphy | ACPI: Retire acpi_iommu_fwspec_ops() |
| [94367b42](https://github.com/RVCK-Project/rvck/commit/94367b42fd48ddee1af101b18691ad82f82d016f) | 2024-07-02 | Robin Murphy | iommu: Resolve fwspec ops automatically |
| [dfff8792](https://github.com/RVCK-Project/rvck/commit/dfff8792a732b1e901e7232635a855f24eba8e71) | 2023-12-07 | Jason Gunthorpe | acpi: Do not return struct iommu_ops from acpi_iommu_configure_id() |
| [ff8e26f2](https://github.com/RVCK-Project/rvck/commit/ff8e26f26d496486ffeffd2815606570c4d40489) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_priv_set() with a lockdep |
| [122ddecd](https://github.com/RVCK-Project/rvck/commit/122ddecdb07d17a6d5a8446396d0feb05ed21ee1) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_get() with lockdep |
| [ee8a5634](https://github.com/RVCK-Project/rvck/commit/ee8a5634632ee15bafd6b8b83268f4de56e08206) | 2023-12-07 | Jason Gunthorpe | iommu/of: Use -ENODEV consistently in of_iommu_configure() |
| [32ececed](https://github.com/RVCK-Project/rvck/commit/32ececed24d000c6b735cbbad0039cc987ff7626) | 2023-12-07 | Jason Gunthorpe | iommmu/of: Do not return struct iommu_ops from of_iommu_configure() |
| [7385c321](https://github.com/RVCK-Project/rvck/commit/7385c3216e1d180c50b21dc6a70b9ed5550cbe4d) | 2023-12-07 | Jason Gunthorpe | iommu: Remove struct iommu_ops *iommu from arch_setup_dma_ops() |
| [eece2c5c](https://github.com/RVCK-Project/rvck/commit/eece2c5c1632f2d883ba5cb2279ec23d5bc8f6c3) | 2023-11-21 | Robin Murphy | iommu: Clean up open-coded ownership checks |
| [dfa4c1cf](https://github.com/RVCK-Project/rvck/commit/dfa4c1cfad810c2ab6665383983f7f7356056d4f) | 2023-11-21 | Robin Murphy | iommu: Retire bus ops |
| [82d440e3](https://github.com/RVCK-Project/rvck/commit/82d440e336da4f3c284cabdf2e4a9000621ef1ea) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_domain_alloc() from bus ops |
| [f2157bae](https://github.com/RVCK-Project/rvck/commit/f2157bae879b55e7099adf3a12f1d925dd749636) | 2023-11-21 | Robin Murphy | iommu: Validate that devices match domains |
| [182f942b](https://github.com/RVCK-Project/rvck/commit/182f942b8bfe785ee655d4ceb8aecd639dbe5c67) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_present() from bus ops |
| [2b997d09](https://github.com/RVCK-Project/rvck/commit/2b997d095bce7117a1f4ceee30d22be7591a7b13) | 2023-11-21 | Robin Murphy | iommu: Factor out some helpers |
| [377e6bbe](https://github.com/RVCK-Project/rvck/commit/377e6bbeff531d53b3717c157e921dd042abe5af) | 2024-03-13 | Xiao Wang | riscv: uaccess: Relax the threshold for fast path |
| [9ab60fd9](https://github.com/RVCK-Project/rvck/commit/9ab60fd9e12be40a88d70c2d9e70f0d1d5ce218e) | 2024-03-13 | Xiao Wang | riscv: uaccess: Allow the last potential unrolled copy |
| [190f9eb4](https://github.com/RVCK-Project/rvck/commit/190f9eb4c34c31d09c2ac8e84d29d4948e4dbc88) | 2024-12-24 | Atish Patra | RISC-V: KVM: Add new exit statstics for redirected traps |
| [ca142333](https://github.com/RVCK-Project/rvck/commit/ca142333422542d38cb126e3eabce021ca13aec4) | 2024-12-24 | Atish Patra | RISC-V: KVM: Update firmware counters for various events |
| [25a673c2](https://github.com/RVCK-Project/rvck/commit/25a673c269cd2dea126ce3b80f686238da625eb4) | 2024-04-29 | Yu-Wei Hsu | RISC-V: KVM: Redirect AMO load/store access fault traps to guest |
| [4c66d558](https://github.com/RVCK-Project/rvck/commit/4c66d55885695e35f5d0901856cbf3c9a9aea08b) | 2025-01-03 | Xu Lu | iommu/riscv: Add shutdown function for iommu driver |
| [3b462e45](https://github.com/RVCK-Project/rvck/commit/3b462e4517bd023d8f115cf67b47435fe6daa73e) | 2025-01-03 | Xu Lu | iommu/riscv: Empty iommu queue before enabling it |
| [ee93628d](https://github.com/RVCK-Project/rvck/commit/ee93628da06f2c8d8a08ea669f5857e587fea6db) | 2024-11-12 | Andrew Jones | iommu/riscv: Add support for platform msi |
| [bd20eab6](https://github.com/RVCK-Project/rvck/commit/bd20eab6b259c9322aacb9fe4d3b18f8d3951da9) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Paging domain support |
| [2732dfc9](https://github.com/RVCK-Project/rvck/commit/2732dfc9becf8187bfed1bc2b972375aff11cfdc) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Command and fault queue support |
| [b71b85b9](https://github.com/RVCK-Project/rvck/commit/b71b85b98c99bfa3215863f4180a07e4d16e154a) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Device directory management. |
| [cfd28c5b](https://github.com/RVCK-Project/rvck/commit/cfd28c5b10204fda84b966f4cb5f6ec1d8470a13) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Enable IOMMU registration and device probe. |
| [91a49b75](https://github.com/RVCK-Project/rvck/commit/91a49b7551470b3b0e2fa8a93c476b602bd3baf1) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU PCIe device driver |
| [44192805](https://github.com/RVCK-Project/rvck/commit/44192805e3d351fe497578bebe7bc702494217bf) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU platform device driver |
| [93783ef1](https://github.com/RVCK-Project/rvck/commit/93783ef17eff2e25dc8fa1f258820be1bb509a3f) | 2024-10-15 | Tomasz Jeznach | dt-bindings: iommu: riscv: Add bindings for RISC-V IOMMU |
| [b995fa9d](https://github.com/RVCK-Project/rvck/commit/b995fa9d2178b7f2e7b52c6efe03c4dca3008eb7) | 2024-04-13 | Pasha Tatashin | iommu/vt-d: add wrapper functions for page allocations |
| [41510883](https://github.com/RVCK-Project/rvck/commit/415108836fa66454f2d1194582e79cdc637173c5) | 2024-02-16 | Krzysztof Kozlowski | iommu: constify of_phandle_args in xlate |
| [0c6c0489](https://github.com/RVCK-Project/rvck/commit/0c6c04893c31f9fb7e6f6cc42be85331d6b36d4d) | 2023-11-22 | Matt Coster | sizes.h: Add entries between SZ_32G and SZ_64T |
| [f1cde8b3](https://github.com/RVCK-Project/rvck/commit/f1cde8b3f5a69d27b4dac322a2a258e1217b8e3a) | 2024-03-05 | Lu Baolu | iommu: Add static iommu_ops-\>release_domain |
| [49cef219](https://github.com/RVCK-Project/rvck/commit/49cef219ad1f456c9784056ae244fab7f7a6407e) | 2023-09-27 | Jason Gunthorpe | iommufd: Convert to alloc_domain_paging() |
| [f0389ffb](https://github.com/RVCK-Project/rvck/commit/f0389ffb064654291c4c88731dcee7a461bc5dc5) | 2024-04-13 | Pasha Tatashin | iommu: Move IOMMU_DOMAIN_BLOCKED global statics to ops-\>blocked_domain |
| [c2bb831c](https://github.com/RVCK-Project/rvck/commit/c2bb831c1529979aaa41c9c4e6ec3e65685f7466) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new userspace mappings with Svvpt... |
| [efd7f449](https://github.com/RVCK-Project/rvck/commit/efd7f4494af1d536d0d39094cb1af533b63784f2) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [52da8434](https://github.com/RVCK-Project/rvck/commit/52da843483f7d084ced0a74a5af788aaf6388a12) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [46936216](https://github.com/RVCK-Project/rvck/commit/46936216913856743fe8304a321c3cb00f71c62f) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
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
| [ed9fb072](https://github.com/RVCK-Project/rvck/commit/ed9fb0721c3e0203cfdb9021a82b767c67fe1ea6) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [05e5658c](https://github.com/RVCK-Project/rvck/commit/05e5658c626ee972e2e0f5e44fa15de3edf21a65) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [d7100d75](https://github.com/RVCK-Project/rvck/commit/d7100d7553b434e74f745a04eb7fa0bb309819a8) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
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
---

**共 432 条提交，显示 201-400**

[1](中兴通讯.md) **[2]** [3](中兴通讯_page3.md)

[显示全部](中兴通讯_all.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-09-07 16:23:15*
*数据来源: 主分支 rvck-6.6@863dbf9d*
