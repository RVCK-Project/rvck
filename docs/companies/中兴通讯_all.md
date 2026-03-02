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
| [98ae7cdd](https://github.com/RVCK-Project/rvck/commit/98ae7cddc7906c91a830c2097951861832129c42) | 2026-02-05 | shenlin | perf vendor events riscv: fix lrw core PMU event mapping |
| [5b98c7e7](https://github.com/RVCK-Project/rvck/commit/5b98c7e7086477af8e1d2bbf0b12c01480f9d699) | 2024-06-05 | Björn Töpel | riscv: Enable DAX VMEMMAP optimization |
| [c42a387c](https://github.com/RVCK-Project/rvck/commit/c42a387c55a53389fea99401794358ef48c4e09f) | 2024-06-05 | Björn Töpel | riscv: mm: Add support for ZONE_DEVICE |
| [d2ce625d](https://github.com/RVCK-Project/rvck/commit/d2ce625d1a4b9b10e79ca93c9885c16080cd3393) | 2024-06-05 | Björn Töpel | virtio-mem: Enable virtio-mem for RISC-V |
| [2b493c6e](https://github.com/RVCK-Project/rvck/commit/2b493c6e1d070eac72d9646dd679a2dc31c04896) | 2024-06-05 | Björn Töpel | riscv: Enable memory hotplugging for RISC-V |
| [e731fe09](https://github.com/RVCK-Project/rvck/commit/e731fe098339f4772f7e082b001d8ac1b2f317ae) | 2024-06-05 | Björn Töpel | riscv: mm: Take memory hotplug read-lock during kernel page table dump |
| [1a909829](https://github.com/RVCK-Project/rvck/commit/1a909829246ea0fd0607d2853b294e2e5905689f) | 2024-06-05 | Björn Töpel | riscv: mm: Add memory hotplugging support |
| [d33b6226](https://github.com/RVCK-Project/rvck/commit/d33b6226031b82ceeb59e9e67be095b3663c908b) | 2024-06-05 | Björn Töpel | riscv: mm: Add pfn_to_kaddr() implementation |
| [27931db8](https://github.com/RVCK-Project/rvck/commit/27931db8ae67ebd5be8a585fdb3f166978505f6e) | 2024-06-05 | Björn Töpel | riscv: mm: Refactor create_linear_mapping_range() for memory hot add |
| [13cccade](https://github.com/RVCK-Project/rvck/commit/13cccade1fd2b545579fb6b1cd61df204c1d5d7c) | 2024-06-05 | Björn Töpel | riscv: mm: Change attribute from __init to __meminit for page functions |
| [b51b59ad](https://github.com/RVCK-Project/rvck/commit/b51b59ad045dc53ceb9f107db41bc4b243f22949) | 2024-06-05 | Björn Töpel | riscv: mm: Pre-allocate vmemmap/direct map/kasan PGD entries |
| [babb3b09](https://github.com/RVCK-Project/rvck/commit/babb3b09f154c2420e985ca838470a528d2dcff4) | 2024-06-05 | Björn Töpel | riscv: mm: Properly forward vmemmap_populate() altmap parameter |
| [8952bd61](https://github.com/RVCK-Project/rvck/commit/8952bd6146b980113bcd2804be8ec7c4fa7abf41) | 2023-12-14 | Alexandre Ghiti | riscv: Use hugepage mappings for vmemmap |
| [3b9ecb96](https://github.com/RVCK-Project/rvck/commit/3b9ecb9635863d6c529395190450787982efefd6) | 2023-11-06 | Evan Green | RISC-V: Probe misaligned access speed in parallel |
| [b2cd299a](https://github.com/RVCK-Project/rvck/commit/b2cd299a2c6e67c3c8dcb39f75a3236aee76cbe9) | 2023-11-06 | Evan Green | RISC-V: Remove __init on unaligned_emulation_finish() |
| [04890143](https://github.com/RVCK-Project/rvck/commit/048901433d8cbc8e6ee6bcca1cece555edc3d1f2) | 2023-10-04 | Clément Léger | riscv: add support for PR_SET_UNALIGN and PR_GET_UNALIGN |
| [889f368f](https://github.com/RVCK-Project/rvck/commit/889f368ff30f807df0cfd00787627b77f8713a8e) | 2023-10-04 | Clément Léger | riscv: report misaligned accesses emulation to hwprobe |
| [e4744708](https://github.com/RVCK-Project/rvck/commit/e4744708bd36f975531dcacaa21448cc9765a255) | 2023-10-04 | Clément Léger | riscv: add support for sysctl unaligned_enabled control |
| [f417fc10](https://github.com/RVCK-Project/rvck/commit/f417fc10317ebfcdce2d1b2d672b64456b59894d) | 2023-10-04 | Clément Léger | riscv: add floating point insn support to misaligned access emulation |
| [60f18ee1](https://github.com/RVCK-Project/rvck/commit/60f18ee1aff2b0475de5df38c9b14c8c2ebfe867) | 2023-10-04 | Clément Léger | riscv: report perf event for misaligned fault |
| [b5d7026e](https://github.com/RVCK-Project/rvck/commit/b5d7026ea384d46a03378743a4a6b1f9983401f5) | 2023-10-04 | Clément Léger | riscv: add support for misaligned trap handling in S-mode |
| [8678e566](https://github.com/RVCK-Project/rvck/commit/8678e566257187ef9c509275dde75c1fa969fc5f) | 2026-01-17 | Lu Peng | riscv: defconfig: Enable more ACPI_APEI configs |
| [66973183](https://github.com/RVCK-Project/rvck/commit/669731830aaae1d69aaa044f1ea0ca4b22ebd541) | 2025-07-23 | Ignacio Encinas | riscv: introduce asm/swab.h |
| [e6d03b34](https://github.com/RVCK-Project/rvck/commit/e6d03b3418ac53e73d4bcf43232a8de593ebc532) | 2026-01-07 | hu.yuye | riscv:defconfig:Enable PCIE_EDR |
| [b1d1ba92](https://github.com/RVCK-Project/rvck/commit/b1d1ba925a13209c3ae8a07a5666141caf4731ee) | 2025-12-31 | Yunhui Cui | arch_topology: move parse_acpi_topology() to common code |
| [51b2a13a](https://github.com/RVCK-Project/rvck/commit/51b2a13a69e903114b775b41238261e79eb266d6) | 2025-12-31 | Yicong Yang | arm64: topology: Support SMT control on ACPI based system |
| [05bec16e](https://github.com/RVCK-Project/rvck/commit/05bec16e0e14dddfe9751a1bf47c7180a0cab6b4) | 2025-12-31 | Yicong Yang | arch_topology: Support SMT control for OF based system |
| [d6c837e6](https://github.com/RVCK-Project/rvck/commit/d6c837e6726f95926ffdb53e54fd9602ba957b70) | 2025-12-31 | Yicong Yang | cpu/SMT: Provide a default topology_is_primary_thread() |
| [11215102](https://github.com/RVCK-Project/rvck/commit/112151026266e142df8ed2d7c20213b9e613c59b) | 2025-01-24 | Andy Shevchenko | serial: 8250_core: Remove unneeded -\>iotype assignment |
| [232181ca](https://github.com/RVCK-Project/rvck/commit/232181ca25e592be3acd2464b6f7bf95f747ab5c) | 2025-12-30 | hu.yuye | Revert "mango pci hack:broadcast when no MSI source known" |
| [1e737b00](https://github.com/RVCK-Project/rvck/commit/1e737b0010da798db06a8eccb419689fc5ca541b) | 2025-12-22 | Clément Léger | riscv: uaccess: do not do misaligned accesses in get/put_user() |
| [89179daa](https://github.com/RVCK-Project/rvck/commit/89179daa7378cac652a5f9e007e02525b1fb1158) | 2025-12-22 | Alexandre Ghiti | riscv: make unsafe user copy routines use existing assembly routines |
| [0225dcfa](https://github.com/RVCK-Project/rvck/commit/0225dcfa3923e700561ee16a206755af945ed811) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm_goto_output' for get_user() |
| [8fc83c04](https://github.com/RVCK-Project/rvck/commit/8fc83c046a16cf60824b6b8d9d618b493685e1b7) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm goto' for put_user() |
| [1aa1da36](https://github.com/RVCK-Project/rvck/commit/1aa1da36532b75062802772ee3711e44ea1733ca) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use input constraints for ptr of __put_user() |
| [d129224f](https://github.com/RVCK-Project/rvck/commit/d129224f2075094bffcc237d9bf312369ae59e6e) | 2025-12-22 | Jisheng Zhang | riscv: implement user_access_begin() and families |
| [bd384d1d](https://github.com/RVCK-Project/rvck/commit/bd384d1de9f2873f4f09a96af210b8dfdd27f5e1) | 2025-12-22 | Ben Dooks | riscv: save the SR_SUM status over switches |
| [5dc5d226](https://github.com/RVCK-Project/rvck/commit/5dc5d226f5ef0d7d990fd7b38fa653955029ceac) | 2011-12-08 | Tejun Heo | Revert "mm: Modify __find_max_addr for memory hole" |
| [3319d463](https://github.com/RVCK-Project/rvck/commit/3319d4633cb5ed1aaaf813ee56cd8f9439f2aa0b) | 2021-12-06 | Alexandre Ghiti | Revert "riscv: mm: Clear compilation warning about last_cpupid" |
| [380317a5](https://github.com/RVCK-Project/rvck/commit/380317a51512070ae7cee5fd26d2dc9a7226e2f4) | 2025-11-12 | shenlin | perf vendor events riscv: add lrw core JSON file with metric support |
| [63ff7f10](https://github.com/RVCK-Project/rvck/commit/63ff7f10d267c759db2c2ac9c137117268f01973) | 2025-11-11 | Fei Liu | i2c: Add driver for the LRW I2C |
| [f75e7230](https://github.com/RVCK-Project/rvck/commit/f75e7230830413ac96bdb16e41954d75e0a963d4) | 2025-09-29 | Fei Liu | dt-bindings: i2c: Add binding for LRW I2C |
| [d8df9f46](https://github.com/RVCK-Project/rvck/commit/d8df9f46a7d5f728db537fb51bb93e9577345762) | 2025-11-12 | Jie Feng | drivers/perf: add LRW DDR PMU support |
| [5ae950f2](https://github.com/RVCK-Project/rvck/commit/5ae950f2a9949759e038dc3075f92aca9e80a8a6) | 2025-09-04 | Wenhong Liu | serial: Add driver for the LRW UART |
| [3f51ab5f](https://github.com/RVCK-Project/rvck/commit/3f51ab5ff489f882af369fed48069566b8533942) | 2025-09-04 | Wenhong Liu | dt-bindings: serial: Add binding for LRW UART |
| [f79a03cf](https://github.com/RVCK-Project/rvck/commit/f79a03cfff064eed7c7fd03ec934dee054d7e145) | 2025-10-16 | Wenhong Liu | riscv: defconfig: remove CONFIG_CMDLINE and CONFIG_CMDLINE_EXTEND as mainline do... |
| [84ef8d24](https://github.com/RVCK-Project/rvck/commit/84ef8d2412e8edfd8a5f223ce3be8d2afa055bd6) | 2025-08-27 | Himanshu Chauhan | riscv: Enable APEI and NMI safe cmpxchg options required for RAS |
| [60d94a04](https://github.com/RVCK-Project/rvck/commit/60d94a04a2c5dd6edaa043888c28af1d87486f1f) | 2025-08-27 | Himanshu Chauhan | riscv: Add config option to enable APEI SSE handler |
| [99b8d4f5](https://github.com/RVCK-Project/rvck/commit/99b8d4f558e014dc1664aba607e622199f39e8f8) | 2025-08-27 | Himanshu Chauhan | riscv: Introduce HEST SSE notification handlers |
| [bf875227](https://github.com/RVCK-Project/rvck/commit/bf875227638b55ca84da98fd49c732d1352415e5) | 2025-08-27 | Himanshu Chauhan | riscv: Add RISC-V entries in processor type and ISA strings |
| [c79363c8](https://github.com/RVCK-Project/rvck/commit/c79363c8b12f92b570df0a087d9884314e07ee1c) | 2025-08-27 | Himanshu Chauhan | riscv: Add functions to register ghes having SSE notification |
| [65c83ba9](https://github.com/RVCK-Project/rvck/commit/65c83ba914478794b7ac3981d8a5e95280e615e1) | 2025-08-27 | Himanshu Chauhan | riscv: conditionally compile GHES NMI spool function |
| [739260c0](https://github.com/RVCK-Project/rvck/commit/739260c046a61abfc5939ac1212e9d6424345589) | 2025-08-27 | Himanshu Chauhan | riscv: Add fixmap indices for GHES IRQ and SSE contexts |
| [4a57875a](https://github.com/RVCK-Project/rvck/commit/4a57875a5daf52d8a3a563dd033798abbfd4c31f) | 2025-08-27 | Himanshu Chauhan | acpi: Introduce SSE in HEST notification types |
| [d366af83](https://github.com/RVCK-Project/rvck/commit/d366af8315316aaaf7c6f29f39f96542b6e6c3cd) | 2025-08-27 | Himanshu Chauhan | riscv: Define arch_apei_get_mem_attribute for RISC-V |
| [53dd5dd9](https://github.com/RVCK-Project/rvck/commit/53dd5dd93045478dde38d8f513a96fe603f6fbc4) | 2025-08-27 | Himanshu Chauhan | riscv: Define ioremap_cache for RISC-V |
| [22adfeda](https://github.com/RVCK-Project/rvck/commit/22adfeda3d2c217a088b64f935d2b9036c0bcd60) | 2025-08-27 | Clément Léger | selftests/riscv: add SSE test module |
| [9765754b](https://github.com/RVCK-Project/rvck/commit/9765754bc47f7f2f3d4265b037d2f586d4f2e2fb) | 2025-08-27 | Clément Léger | perf: RISC-V: add support for SSE event |
| [ab1aede8](https://github.com/RVCK-Project/rvck/commit/ab1aede8e171613baffcd2ce4799b5bc8e88f193) | 2025-08-27 | Clément Léger | drivers: firmware: add riscv SSE support |
| [f3b4f420](https://github.com/RVCK-Project/rvck/commit/f3b4f42036b5ebafad973f566d16fbc97cae3a2d) | 2025-08-27 | Clément Léger | riscv: add support for SBI Supervisor Software Events extension |
| [12921892](https://github.com/RVCK-Project/rvck/commit/12921892b363e53d273088d5f8226f414410264d) | 2025-08-08 | Clément Léger | riscv: add SBI SSE extension definitions |
| [226acf66](https://github.com/RVCK-Project/rvck/commit/226acf6605b42bdd088f73fd979d629b98dba83b) | 2025-10-14 | Sunil V L | iommu/riscv: Add ACPI support |
| [cdb3b6db](https://github.com/RVCK-Project/rvck/commit/cdb3b6db3f206af733b5124875b6b3035fc15163) | 2025-10-14 | Sunil V L | ACPI: scan: Add support for RISC-V in acpi_iommu_configure_id() |
| [6cda109e](https://github.com/RVCK-Project/rvck/commit/6cda109ea67328d73690330aa39d0d4711ef4a1b) | 2025-10-14 | Sunil V L | ACPI: RISC-V: Add support for RIMT |
| [8e1a0034](https://github.com/RVCK-Project/rvck/commit/8e1a003403e393393a243caef69c2c0d55ed390a) | 2025-10-13 | Sunil V L | ACPICA: actbl2: Add definitions for RIMT |
| [e4e79ca2](https://github.com/RVCK-Project/rvck/commit/e4e79ca25b64a3e793061972a5d2a2de5ac20a8f) | 2025-09-20 | shenlin | perf vendor events riscv: add lrw core JSON file |
| [b17db4ee](https://github.com/RVCK-Project/rvck/commit/b17db4ee274a3beeb9673bba38026ee389bd6fbe) | 2025-04-21 | Alexandre Ghiti | riscv: Add support for Zicbop |
| [2610e498](https://github.com/RVCK-Project/rvck/commit/2610e4982ff1e95ec68f589a7c4c560f42e4db3c) | 2025-04-21 | Alexandre Ghiti | riscv: Introduce Zicbop instructions |
| [aa53cd30](https://github.com/RVCK-Project/rvck/commit/aa53cd30a77f18c3c83268fa1c4bfa500abee2e5) | 2025-02-26 | Yunhui Cui | RISC-V: Enable cbo.clean/flush in usermode |
| [d6849be7](https://github.com/RVCK-Project/rvck/commit/d6849be757ec20fd1bdf81349945a1d26810b87a) | 2024-08-14 | Samuel Holland | riscv: Add support for per-thread envcfg CSR values |
| [34a8cd76](https://github.com/RVCK-Project/rvck/commit/34a8cd76a0acfe390e5a35e349e64a644b6e0f73) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zaamo/Zalrsc extensions for Guest/VM |
| [59ef5dce](https://github.com/RVCK-Project/rvck/commit/59ef5dce31bbb72839501db690c838bf4fe4b8fc) | 2024-06-19 | Clément Léger | riscv: hwprobe: export Zaamo and Zalrsc extensions |
| [48d57e17](https://github.com/RVCK-Project/rvck/commit/48d57e1778801c5d02bd509811bac9f4767ade65) | 2024-06-19 | Clément Léger | riscv: add parsing for Zaamo and Zalrsc extensions |
| [9efca695](https://github.com/RVCK-Project/rvck/commit/9efca6959b1c569eec70675cb3a4291810400aab) | 2025-02-13 | Inochi Amaoto | riscv: hwprobe: export bfloat16 ISA extension |
| [b5f37811](https://github.com/RVCK-Project/rvck/commit/b5f378117161d1608fa49e85037557167ac86079) | 2025-02-13 | Inochi Amaoto | riscv: add ISA extension parsing for bfloat16 ISA extension |
| [05f84823](https://github.com/RVCK-Project/rvck/commit/05f8482327f84d12a3c6d3aed27a25bf3a0f9ba8) | 2024-05-24 | Xiao Wang | riscv, bpf: Introduce shift add helper with Zba optimization |
| [d39a5d9b](https://github.com/RVCK-Project/rvck/commit/d39a5d9b7e66be775391e6669758fd9bb4719259) | 2024-05-16 | Xiao Wang | riscv, bpf: Optimize zextw insn with Zba extension |
| [6b3cb73d](https://github.com/RVCK-Project/rvck/commit/6b3cb73d4163792165f1caf205a810dc080b7d8a) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize bswap insns with Zbb support |
| [3fa28c6a](https://github.com/RVCK-Project/rvck/commit/3fa28c6a695d3a289d7b1f8848a0c0d52fc4d397) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize sign-extention mov insns with Zbb support |
| [44ea2a16](https://github.com/RVCK-Project/rvck/commit/44ea2a160df4570e358fbf501d7b0cd9794122d7) | 2024-01-15 | Pu Lehui | riscv, bpf: Add necessary Zbb instructions |
| [7f463909](https://github.com/RVCK-Project/rvck/commit/7f463909c1b582b01b8198135a774bfef01c11e2) | 2024-01-15 | Pu Lehui | riscv, bpf: Simplify sext and zext logics in branch instructions |
| [c54c0158](https://github.com/RVCK-Project/rvck/commit/c54c01585f7774a4a8e2c725964101ef00c1e296) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit zero-extension to emit_zextw |
| [5f753c0c](https://github.com/RVCK-Project/rvck/commit/5f753c0c9bc1438815f22143db2ba9363a876d09) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit sign-extension to emit_sextw |
| [8d02e4a1](https://github.com/RVCK-Project/rvck/commit/8d02e4a100890ea31956272854ac830372d89ecd) | 2025-03-12 | Robin Murphy | iommu: Don't warn prematurely about dodgy probes |
| [a9a45e95](https://github.com/RVCK-Project/rvck/commit/a9a45e956a6a75d792a044eb6111864712ffb654) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_domain_alloc() |
| [b8c19d4d](https://github.com/RVCK-Project/rvck/commit/b8c19d4d7747cacd14112a84bf9406ce5d267ab6) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_present() |
| [26e7943a](https://github.com/RVCK-Project/rvck/commit/26e7943a24611fedffd6b762131998a4439cb18b) | 2024-09-02 | Lu Baolu | drm/tegra: Use iommu_paging_domain_alloc() |
| [feb87595](https://github.com/RVCK-Project/rvck/commit/feb87595648363260e7efb9c36efd9169ad35252) | 2024-09-02 | Lu Baolu | drm/rockchip: Use iommu_paging_domain_alloc() |
| [9848be0d](https://github.com/RVCK-Project/rvck/commit/9848be0dce473b45a5e09b2eafbdfad424fbd85e) | 2024-06-10 | Lu Baolu | RDMA/usnic: Use iommu_paging_domain_alloc() |
| [fb43d331](https://github.com/RVCK-Project/rvck/commit/fb43d331696a66145b8ac07796408047f9bb7cbc) | 2024-08-12 | Lu Baolu | soc: fsl: qbman: Use iommu_paging_domain_alloc() |
| [06339cfd](https://github.com/RVCK-Project/rvck/commit/06339cfd065b4eb1463de624202e1700ec06a70e) | 2024-08-12 | Lu Baolu | remoteproc: Use iommu_paging_domain_alloc() |
| [53d4a4f5](https://github.com/RVCK-Project/rvck/commit/53d4a4f50b37169d58088ef437275da9238e9228) | 2024-08-12 | Lu Baolu | media: venus: firmware: Use iommu_paging_domain_alloc() |
| [0e8b8a86](https://github.com/RVCK-Project/rvck/commit/0e8b8a864a55c260df24c27613101c564aaddf1e) | 2024-08-12 | Lu Baolu | media: nvidia: tegra: Use iommu_paging_domain_alloc() |
| [468ba6f0](https://github.com/RVCK-Project/rvck/commit/468ba6f0037a2703b651e2cb744d4019e9c5e1e3) | 2024-08-12 | Lu Baolu | gpu: host1x: Use iommu_paging_domain_alloc() |
| [2bb018d8](https://github.com/RVCK-Project/rvck/commit/2bb018d81e57140c344ef2d21e88356999ae5bfa) | 2024-09-02 | Lu Baolu | drm/nouveau/tegra: Use iommu_paging_domain_alloc() |
| [670e4880](https://github.com/RVCK-Project/rvck/commit/670e4880fcf9b045e28ad3f6b78412b4d4765d7e) | 2024-06-10 | Lu Baolu | wifi: ath11k: Use iommu_paging_domain_alloc() |
| [00510d32](https://github.com/RVCK-Project/rvck/commit/00510d32b6d8f3e9af19e2f16fd31bddb83c6448) | 2024-06-10 | Lu Baolu | wifi: ath10k: Use iommu_paging_domain_alloc() |
| [af964266](https://github.com/RVCK-Project/rvck/commit/af96426621624cd0bafdbf7e8eb52d03316c3a91) | 2024-06-10 | Lu Baolu | drm/msm: Use iommu_paging_domain_alloc() |
| [dd5160ac](https://github.com/RVCK-Project/rvck/commit/dd5160ac3bb165325c3c3a9b863f5848296e8e68) | 2024-06-10 | Lu Baolu | vhost-vdpa: Use iommu_paging_domain_alloc() |
| [0fd38336](https://github.com/RVCK-Project/rvck/commit/0fd38336ed2bde4950413f8be3a819a598b342b6) | 2024-06-10 | Lu Baolu | vfio/type1: Use iommu_paging_domain_alloc() |
| [ec049eb5](https://github.com/RVCK-Project/rvck/commit/ec049eb56882f5aa35924b5ec208c325c979d043) | 2024-06-10 | Lu Baolu | iommufd: Use iommu_paging_domain_alloc() |
| [9ba9d4e4](https://github.com/RVCK-Project/rvck/commit/9ba9d4e40518a1e8c50e4c9e46ae1c69f5be0dee) | 2024-06-10 | Lu Baolu | iommu: Add iommu_paging_domain_alloc() interface |
| [b00c4f7c](https://github.com/RVCK-Project/rvck/commit/b00c4f7cfdf7eade482ab2a511ea7bc8142d567f) | 2025-02-28 | Robin Murphy | iommu: Get DT/ACPI parsing into the proper probe path |
| [0c2ddb38](https://github.com/RVCK-Project/rvck/commit/0c2ddb3854f2d9edf49e71bec6cdf730315beebb) | 2025-02-28 | Robin Murphy | iommu: Keep dev-\>iommu state consistent |
| [7e938292](https://github.com/RVCK-Project/rvck/commit/7e9382920aecdc8f512ac12baf4cdff2fe98c0d4) | 2025-02-28 | Robin Murphy | iommu: Resolve ops in iommu_init_device() |
| [32ffe55f](https://github.com/RVCK-Project/rvck/commit/32ffe55f649dd50be33e401e344ef3e7d13c3987) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [90dbeb72](https://github.com/RVCK-Project/rvck/commit/90dbeb72b5d9f44b972df2c2b13cf2c3235b76be) | 2025-02-27 | Robin Murphy | iommu: Unexport iommu_fwspec_free() |
| [9d983572](https://github.com/RVCK-Project/rvck/commit/9d983572f219dd84ae1793f58e870999eab1d9be) | 2024-07-02 | Robin Murphy | iommu: Remove iommu_fwspec ops |
| [bb149fad](https://github.com/RVCK-Project/rvck/commit/bb149fada5a36f25f46039f267341d312befc44e) | 2024-07-02 | Robin Murphy | OF: Simplify of_iommu_configure() |
| [7d6dd3c4](https://github.com/RVCK-Project/rvck/commit/7d6dd3c4771a3327da1dbf0b0425098881b5d33a) | 2024-07-02 | Robin Murphy | ACPI: Retire acpi_iommu_fwspec_ops() |
| [471255f9](https://github.com/RVCK-Project/rvck/commit/471255f9f8b9b2c341faf91ff5c273f65febce71) | 2024-07-02 | Robin Murphy | iommu: Resolve fwspec ops automatically |
| [4b3cb3fb](https://github.com/RVCK-Project/rvck/commit/4b3cb3fb542bc42fc5d9901a87dec4831773a922) | 2023-12-07 | Jason Gunthorpe | acpi: Do not return struct iommu_ops from acpi_iommu_configure_id() |
| [db837928](https://github.com/RVCK-Project/rvck/commit/db837928994bff933e57602dfc4e7bc59cb0861c) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_priv_set() with a lockdep |
| [77f5966a](https://github.com/RVCK-Project/rvck/commit/77f5966aab25df7e68b3518c52e886a0fa50932d) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_get() with lockdep |
| [fefa830d](https://github.com/RVCK-Project/rvck/commit/fefa830dc2832104b96be4f2151fd56ac75eceba) | 2023-12-07 | Jason Gunthorpe | iommu/of: Use -ENODEV consistently in of_iommu_configure() |
| [02654774](https://github.com/RVCK-Project/rvck/commit/02654774f72279d5854ce2e15f78d0a978995170) | 2023-12-07 | Jason Gunthorpe | iommmu/of: Do not return struct iommu_ops from of_iommu_configure() |
| [a29741ca](https://github.com/RVCK-Project/rvck/commit/a29741ca7fe570ae4f5be14070e00d9a6925cf42) | 2023-12-07 | Jason Gunthorpe | iommu: Remove struct iommu_ops *iommu from arch_setup_dma_ops() |
| [33173506](https://github.com/RVCK-Project/rvck/commit/33173506bd02ec1837d088e75c8893b1082cae3e) | 2023-11-21 | Robin Murphy | iommu: Clean up open-coded ownership checks |
| [dec05a7a](https://github.com/RVCK-Project/rvck/commit/dec05a7afce59a010d9847d100c92311e7b01038) | 2023-11-21 | Robin Murphy | iommu: Retire bus ops |
| [2bd34254](https://github.com/RVCK-Project/rvck/commit/2bd34254dfdebff00bbb8b2685489fd973d515f8) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_domain_alloc() from bus ops |
| [9d84d332](https://github.com/RVCK-Project/rvck/commit/9d84d33262e29f075ac37e576b1517a41b2c901e) | 2023-11-21 | Robin Murphy | iommu: Validate that devices match domains |
| [44a428f7](https://github.com/RVCK-Project/rvck/commit/44a428f707b8532e5ac4ac0ccc565773c44658e4) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_present() from bus ops |
| [76d810e0](https://github.com/RVCK-Project/rvck/commit/76d810e08745828e1d9c79af88ee2565a7460751) | 2023-11-21 | Robin Murphy | iommu: Factor out some helpers |
| [70c74625](https://github.com/RVCK-Project/rvck/commit/70c746251caa6190faaed09a4d08b8909fc07ae7) | 2024-03-13 | Xiao Wang | riscv: uaccess: Relax the threshold for fast path |
| [3d68c05a](https://github.com/RVCK-Project/rvck/commit/3d68c05aed37bba26a8c8046f67cdc4bdc4d05ca) | 2024-03-13 | Xiao Wang | riscv: uaccess: Allow the last potential unrolled copy |
| [e04ae582](https://github.com/RVCK-Project/rvck/commit/e04ae58205d59687ca26aea7f16328cb55c886a0) | 2024-12-24 | Atish Patra | RISC-V: KVM: Add new exit statstics for redirected traps |
| [80eb8f70](https://github.com/RVCK-Project/rvck/commit/80eb8f70b8a357972162f3ad5b1314caa9fc6b2a) | 2024-12-24 | Atish Patra | RISC-V: KVM: Update firmware counters for various events |
| [1e64aba4](https://github.com/RVCK-Project/rvck/commit/1e64aba465ff6c993875d4c2c8789bf330b35684) | 2024-04-29 | Yu-Wei Hsu | RISC-V: KVM: Redirect AMO load/store access fault traps to guest |
| [38eb67d7](https://github.com/RVCK-Project/rvck/commit/38eb67d76e8fbaf78e4bc2bd246d02c1da41a7fb) | 2025-08-20 | XianLiang Huang | iommu/riscv: prevent NULL deref in iova_to_phys |
| [dc4fc4b9](https://github.com/RVCK-Project/rvck/commit/dc4fc4b9933e2df27cd902d9e518609c9e960caa) | 2025-01-03 | Xu Lu | iommu/riscv: Add shutdown function for iommu driver |
| [1f66d428](https://github.com/RVCK-Project/rvck/commit/1f66d428682e6c0205bdebe7adc3cb0a5b1de48a) | 2025-01-03 | Xu Lu | iommu/riscv: Empty iommu queue before enabling it |
| [1a5e2671](https://github.com/RVCK-Project/rvck/commit/1a5e267138b58360ba08af26af6b373209d10d4e) | 2024-11-12 | Andrew Jones | iommu/riscv: Add support for platform msi |
| [1f17c8c6](https://github.com/RVCK-Project/rvck/commit/1f17c8c6a1c9436b84b6f1e4d26cb4fe1a760b25) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Paging domain support |
| [e1002f54](https://github.com/RVCK-Project/rvck/commit/e1002f541fdbebdb9caab6144b933a208c9e9232) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Command and fault queue support |
| [c2195fd9](https://github.com/RVCK-Project/rvck/commit/c2195fd91ad386234d142ce173fc2fe2540fe6ea) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Device directory management. |
| [457994e4](https://github.com/RVCK-Project/rvck/commit/457994e42195f6e5124ba0cbbd02466f2ad3af73) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Enable IOMMU registration and device probe. |
| [1084fab8](https://github.com/RVCK-Project/rvck/commit/1084fab81843ad17538d64ee739f365bf60819fa) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU PCIe device driver |
| [c5b5bc8a](https://github.com/RVCK-Project/rvck/commit/c5b5bc8a497c894405d201b39ca78196d101d9af) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU platform device driver |
| [91bde4fd](https://github.com/RVCK-Project/rvck/commit/91bde4fde0504e22e6f82f96d5f2ec39b012f7b4) | 2024-10-15 | Tomasz Jeznach | dt-bindings: iommu: riscv: Add bindings for RISC-V IOMMU |
| [6de61714](https://github.com/RVCK-Project/rvck/commit/6de61714fadb7c49bf56a21fb3e9ce4b2f50af99) | 2024-04-13 | Pasha Tatashin | iommu/vt-d: add wrapper functions for page allocations |
| [850a35fc](https://github.com/RVCK-Project/rvck/commit/850a35fc6bbce7ae9eab9c4d18482d3fc89022dc) | 2024-02-16 | Krzysztof Kozlowski | iommu: constify of_phandle_args in xlate |
| [4886b311](https://github.com/RVCK-Project/rvck/commit/4886b3116a3e634a4976aacbd7a9bab9261b192f) | 2023-11-22 | Matt Coster | sizes.h: Add entries between SZ_32G and SZ_64T |
| [3e5e7205](https://github.com/RVCK-Project/rvck/commit/3e5e7205a5f72aa560096bd98776354e9ccc2555) | 2024-03-05 | Lu Baolu | iommu: Add static iommu_ops-\>release_domain |
| [7be70fa6](https://github.com/RVCK-Project/rvck/commit/7be70fa6892442c298a7b8bac8cabc185beb227a) | 2023-09-27 | Jason Gunthorpe | iommufd: Convert to alloc_domain_paging() |
| [65704158](https://github.com/RVCK-Project/rvck/commit/6570415870c300ef23d93996a6ad0f5bb313f03e) | 2024-04-13 | Pasha Tatashin | iommu: Move IOMMU_DOMAIN_BLOCKED global statics to ops-\>blocked_domain |
| [6f287f80](https://github.com/RVCK-Project/rvck/commit/6f287f80510405c3954f7918888084776f9f0a6d) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new userspace mappings with Svvpt... |
| [99434229](https://github.com/RVCK-Project/rvck/commit/994342298b20164677bbe4a4c7bab92c355dd6b3) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [dfe176c7](https://github.com/RVCK-Project/rvck/commit/dfe176c7f0140c8aa9bdd64c4b751c86a5c97e26) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [f456e3d0](https://github.com/RVCK-Project/rvck/commit/f456e3d0cf08bea9c83189c239761f02ef4fceb4) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
| [426397a8](https://github.com/RVCK-Project/rvck/commit/426397a8d5a9115553b66e20895347f6e1899902) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [9ae64ace](https://github.com/RVCK-Project/rvck/commit/9ae64ace747137f6ddbb7d30661d1ad77e37916b) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [ae68f52c](https://github.com/RVCK-Project/rvck/commit/ae68f52c331a9c2140f5245dd38c45933658c453) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [71cb211a](https://github.com/RVCK-Project/rvck/commit/71cb211a5fb963a499b36b201a0fa5eeb6da9658) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [a3159fba](https://github.com/RVCK-Project/rvck/commit/a3159fbae61e504f25a9f159c73a32e409f34760) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [520426c1](https://github.com/RVCK-Project/rvck/commit/520426c15c598f49bac6548de6525dc52e339a0b) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [795e40f0](https://github.com/RVCK-Project/rvck/commit/795e40f073b146384bb8c8d6874fe1a829ad13b6) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [c5e35fb1](https://github.com/RVCK-Project/rvck/commit/c5e35fb1fbf16a0b126058c3b74a1f1c64947f70) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [9f6bf02f](https://github.com/RVCK-Project/rvck/commit/9f6bf02ff51348fac90320899d3f5d4d02cf62fc) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [826da7c8](https://github.com/RVCK-Project/rvck/commit/826da7c87e5b16e757e49505a742311cda334280) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [9f426232](https://github.com/RVCK-Project/rvck/commit/9f426232029e5343b3384674773772917e8c3f24) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [62cb230d](https://github.com/RVCK-Project/rvck/commit/62cb230d1193638f69c4f38c5448214751b49cea) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [daa89bfb](https://github.com/RVCK-Project/rvck/commit/daa89bfbde98b9b484b039bc393e0ca3d251815f) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [673550af](https://github.com/RVCK-Project/rvck/commit/673550af59edaf55fb6e07500401564c0f338479) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [ca438ea7](https://github.com/RVCK-Project/rvck/commit/ca438ea748644c4565c0b2cc7ff73a70811d5c47) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [1ca0be68](https://github.com/RVCK-Project/rvck/commit/1ca0be68176d55b4cb58e7da3ef52ffd1db114aa) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [608b7445](https://github.com/RVCK-Project/rvck/commit/608b74451a00279e77dc800c154dab85f4c2fc4c) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [5303cd6e](https://github.com/RVCK-Project/rvck/commit/5303cd6e307c19933618d95bf64d15c8eb9288f5) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [9d5f8306](https://github.com/RVCK-Project/rvck/commit/9d5f830630ba4c0f8e8990715100e7f558418950) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [82d4957f](https://github.com/RVCK-Project/rvck/commit/82d4957f563a357ea357b76cdcc21d6c2226e9b9) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [8c3e5ef3](https://github.com/RVCK-Project/rvck/commit/8c3e5ef3698e302817d05f4f15a27c71ea403407) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [0a62a207](https://github.com/RVCK-Project/rvck/commit/0a62a207998613274faea65ccc9f94f54036738f) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [28315882](https://github.com/RVCK-Project/rvck/commit/28315882df62fc2657992284e1ecb291fe456818) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [57c71d4b](https://github.com/RVCK-Project/rvck/commit/57c71d4bacd473eb8cc33acf0b3dde44a10e60eb) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [670dda19](https://github.com/RVCK-Project/rvck/commit/670dda191279fb38164a6ccca54a28a62b3fbc6a) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [45a1be8b](https://github.com/RVCK-Project/rvck/commit/45a1be8b5a826e205968756865631ee9243736cc) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [9612072e](https://github.com/RVCK-Project/rvck/commit/9612072e80ee19fb970098a43edeb8d2d21fe3b1) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [81a26967](https://github.com/RVCK-Project/rvck/commit/81a2696763e29f4728e83c2dc8b73c260a67c2ab) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [b76318af](https://github.com/RVCK-Project/rvck/commit/b76318aff95068a2f5ea878c6f775dfdd32d4363) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [395fda11](https://github.com/RVCK-Project/rvck/commit/395fda115c42ac6b83fcbe999a0d7222b2fcbe2e) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [626177ea](https://github.com/RVCK-Project/rvck/commit/626177ea61404aa185991445bc8f3c509b997e82) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [dbacec2e](https://github.com/RVCK-Project/rvck/commit/dbacec2e062273997e870b8204257209687960af) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [284f4f0a](https://github.com/RVCK-Project/rvck/commit/284f4f0a015cbc29c21c5b50ee4f67d23b3f868d) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [fca8d5e2](https://github.com/RVCK-Project/rvck/commit/fca8d5e27e703f3f180a639d4a6e1bdfe381cb9d) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [15d09069](https://github.com/RVCK-Project/rvck/commit/15d09069860dcf592223ea610c78fa3c3462a637) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [a77356b7](https://github.com/RVCK-Project/rvck/commit/a77356b7f7c0b22e6764cd77973c30dc834b0dd3) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [e4c199d1](https://github.com/RVCK-Project/rvck/commit/e4c199d1cc65e418f3f08f6ee4a32e20d7aadcbf) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [ef98d903](https://github.com/RVCK-Project/rvck/commit/ef98d903e31a2a94ef30f0dc8dfa6903206c086c) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [0a1c7678](https://github.com/RVCK-Project/rvck/commit/0a1c76782511bc0547cc609c85a92298b655483a) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [f5b495cb](https://github.com/RVCK-Project/rvck/commit/f5b495cbf4be1cf28273a890540b4a88b44a63fb) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [af260bd2](https://github.com/RVCK-Project/rvck/commit/af260bd27f7f931df82716746639514c957cf66a) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [d9b25527](https://github.com/RVCK-Project/rvck/commit/d9b255271fe5e882271e8e1d6f1cb2a9f9788b7f) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [1a5d87d5](https://github.com/RVCK-Project/rvck/commit/1a5d87d559d1951a3d15fc7598a30eddd4674374) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [07dcae62](https://github.com/RVCK-Project/rvck/commit/07dcae62866fb7596b8b48c0192b137f8dfafda6) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [8c2aaf95](https://github.com/RVCK-Project/rvck/commit/8c2aaf95581187697405a8ddab6a94ac34f55543) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [882c1e4f](https://github.com/RVCK-Project/rvck/commit/882c1e4f637dd0cfe197ca26b926e56e5d150f08) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [5bcb23f6](https://github.com/RVCK-Project/rvck/commit/5bcb23f6d0ca65e87e50828f46f30ad52cc54c42) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [b2024a6c](https://github.com/RVCK-Project/rvck/commit/b2024a6ce42a0fff58f983a8139d808089bf49fc) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [415caad5](https://github.com/RVCK-Project/rvck/commit/415caad579396ef251436b57e51c9aa87d4657ef) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [3e94048e](https://github.com/RVCK-Project/rvck/commit/3e94048e6281053089b8a67348f2073f4b6a0058) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [bcca9a5f](https://github.com/RVCK-Project/rvck/commit/bcca9a5fcb562f3d6d61f7ba568f14890d8f0f59) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
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

**共 252 条提交（显示全部）**

[分页显示](中兴通讯.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-03-02 22:17:57*
*数据来源: 主分支 rvck-6.6@d415e668*
