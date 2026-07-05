# 阿里达摩院 贡献详情

<div style="background-color: #F4433620; padding: 15px; border-radius: 8px; border-left: 5px solid #F44336;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 393</li>
<li><strong>统计时间</strong>: 2026-07-06 01:41:51</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.143</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @linux.alibaba.com

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [3f1ce50b](https://github.com/RVCK-Project/rvck/commit/3f1ce50b4bbd386755a9a8a48166c96012615707) | 2026-04-04 | Charlie Jenkins | selftests: riscv: Add license to cfi selftest |
| [d85c3625](https://github.com/RVCK-Project/rvck/commit/d85c36258603480648604b12d2db46e535c8bf83) | 2026-04-04 | Paul Walmsley | prctl: cfi: change the branch landing pad prctl()s to be more descriptive |
| [5641255f](https://github.com/RVCK-Project/rvck/commit/5641255f8275665d0e6f09c5148e2502f253bb99) | 2026-04-04 | Zong Li | riscv: cfi: clear CFI lock status in start_thread() |
| [73632115](https://github.com/RVCK-Project/rvck/commit/736321157ab62060f8d1199063001ef6f58fc56e) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: expand "SS" references to "shadow stack" in uapi headers |
| [bdefe9b7](https://github.com/RVCK-Project/rvck/commit/bdefe9b73d1ff3d7772488b60081932164c14596) | 2026-04-04 | Paul Walmsley | prctl: rename branch landing pad implementation functions to be more explicit |
| [b62663a2](https://github.com/RVCK-Project/rvck/commit/b62663a238b92370c22a0807fcf2c6f1bde12f17) | 2026-04-04 | Paul Walmsley | riscv: ptrace: expand "LP" references to "branch landing pads" in uapi headers |
| [93526058](https://github.com/RVCK-Project/rvck/commit/93526058d4f5f007764eae93f3537b3f4ca29550) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: fix "PRACE" typo in uapi header |
| [9d1f61f6](https://github.com/RVCK-Project/rvck/commit/9d1f61f69dcccb291082d692c62baa31a4055874) | 2026-04-02 | Paul Walmsley | riscv: use _BITUL macro rather than BIT() in ptrace uapi and kselftests |
| [05cf77ef](https://github.com/RVCK-Project/rvck/commit/05cf77ef22be76ed14f1aeda7cae5f054e495138) | 2026-01-25 | Deepak Gupta | kselftest/riscv: add kselftest for user mode CFI |
| [b73ff8b9](https://github.com/RVCK-Project/rvck/commit/b73ff8b9a64af8520c102deba4643adad4f42897) | 2026-01-25 | Deepak Gupta | riscv: add documentation for shadow stack |
| [76f94b7f](https://github.com/RVCK-Project/rvck/commit/76f94b7fb978499fbf4cd7e5a39cd1f0d64b219f) | 2026-01-25 | Deepak Gupta | riscv: add documentation for landing pad / indirect branch tracking |
| [df7b7101](https://github.com/RVCK-Project/rvck/commit/df7b71016fe503b9868d9b5176dce237bee6e074) | 2026-01-25 | Deepak Gupta | riscv: create a Kconfig fragment for shadow stack and landing pad support |
| [9b25986a](https://github.com/RVCK-Project/rvck/commit/9b25986a31de45ab931e25f4b217525e8a42d28f) | 2026-01-25 | Deepak Gupta | arch/riscv: add dual vdso creation logic and select vdso based on hw |
| [c1fcb4ef](https://github.com/RVCK-Project/rvck/commit/c1fcb4ef5567498d8576dd7701e76fdd0e5d8ba7) | 2026-01-25 | Jim Shu | arch/riscv: compile vdso with landing pad and shadow stack note |
| [c9ea7a8b](https://github.com/RVCK-Project/rvck/commit/c9ea7a8b78537eb8bfb3b1d416cd13a6f5fd4e7c) | 2024-10-16 | Alexandre Ghiti | riscv: Check that vdso does not contain any dynamic relocations |
| [ad4c317b](https://github.com/RVCK-Project/rvck/commit/ad4c317b49c44c70c90d5f4b2cce9d92bdb2150e) | 2026-01-25 | Deepak Gupta | riscv: enable kernel access to shadow stack memory via the FWFT SBI call |
| [6b03f44e](https://github.com/RVCK-Project/rvck/commit/6b03f44edf3b642150755e48c030f0741ee83d25) | 2026-01-25 | Deepak Gupta | riscv: add kernel command line option to opt out of user CFI |
| [98b5f5d0](https://github.com/RVCK-Project/rvck/commit/98b5f5d04805bda26045333a483a4205993f646b) | 2026-01-25 | Deepak Gupta | riscv/hwprobe: add zicfilp / zicfiss enumeration in hwprobe |
| [602c69dc](https://github.com/RVCK-Project/rvck/commit/602c69dc1c98c1e103a5b8359fde585ed42cfedd) | 2026-01-25 | Paul Walmsley | riscv: hwprobe: add support for RISCV_HWPROBE_KEY_IMA_EXT_1 |
| [0afd6b1f](https://github.com/RVCK-Project/rvck/commit/0afd6b1fb0c715e236601934723d56260bf6d382) | 2026-01-25 | Deepak Gupta | riscv/ptrace: expose riscv CFI status and state via ptrace and in core files |
| [f783bf21](https://github.com/RVCK-Project/rvck/commit/f783bf213b15d421d88251d7d643ef6a162ca7f3) | 2026-01-25 | Deepak Gupta | riscv/kernel: update __show_regs() to print shadow stack register |
| [78702bed](https://github.com/RVCK-Project/rvck/commit/78702bed622bc08d6be1ee53584e8aff2410592b) | 2026-01-25 | Deepak Gupta | riscv/signal: save and restore the shadow stack on a signal |
| [062eefdf](https://github.com/RVCK-Project/rvck/commit/062eefdfa4983a0642b69a7875ad0fd946fe388c) | 2026-01-25 | Deepak Gupta | riscv/traps: Introduce software check exception and uprobe handling |
| [e8c71e4e](https://github.com/RVCK-Project/rvck/commit/e8c71e4ee56c430e9fa13bf324fde5885f281ddc) | 2026-01-25 | Deepak Gupta | riscv: Implement indirect branch tracking prctls |
| [04d28dc4](https://github.com/RVCK-Project/rvck/commit/04d28dc4ca0d3573e1353d1b013946f48aab55d9) | 2026-01-25 | Deepak Gupta | prctl: add arch-agnostic prctl()s for indirect branch tracking |
| [d1092a90](https://github.com/RVCK-Project/rvck/commit/d1092a90afa25268f96b8eb5840311a77b4dc4be) | 2024-10-01 | Mark Brown | mman: Add map_shadow_stack() flags |
| [1f56fdd8](https://github.com/RVCK-Project/rvck/commit/1f56fdd8a44695609c5feb48cdd213bcd41891e6) | 2024-10-01 | Mark Brown | prctl: arch-agnostic prctl for shadow stack |
| [71212ce5](https://github.com/RVCK-Project/rvck/commit/71212ce514d5a04996c5e9df59e845c922f78f5e) | 2026-01-25 | Deepak Gupta | riscv: Implement arch-agnostic shadow stack prctls |
| [97644be6](https://github.com/RVCK-Project/rvck/commit/97644be6352af8709579b5e72b7429d3217aa466) | 2026-01-25 | Deepak Gupta | riscv/shstk: If needed allocate a new shadow stack on clone |
| [287e6028](https://github.com/RVCK-Project/rvck/commit/287e602849918bce56ad99e09d94bab1f645a0cf) | 2026-01-25 | Deepak Gupta | riscv/mm: Implement map_shadow_stack() syscall |
| [65dee71a](https://github.com/RVCK-Project/rvck/commit/65dee71ac4059f5df40190bb8d7f7508e24802e2) | 2026-01-25 | Deepak Gupta | riscv/mm: update write protect to work on shadow stacks |
| [c15ffd50](https://github.com/RVCK-Project/rvck/commit/c15ffd501b3949e9258e90275b73605935074545) | 2026-01-25 | Deepak Gupta | riscv/mm: teach pte_mkwrite to manufacture shadow stack PTEs |
| [748726ad](https://github.com/RVCK-Project/rvck/commit/748726ad0ad42fd9539384367202f0ff9d8672f5) | 2026-01-25 | Deepak Gupta | riscv/mm: manufacture shadow stack ptes |
| [aaa73512](https://github.com/RVCK-Project/rvck/commit/aaa735123dcbce6fbcc6e6d2568d06b732596e9c) | 2026-01-25 | Deepak Gupta | riscv/mm: ensure PROT_WRITE leads to VM_READ \| VM_WRITE |
| [6ec8858a](https://github.com/RVCK-Project/rvck/commit/6ec8858a6b83b001d0c6c2b8c15b7ee8203b79bb) | 2026-01-25 | Deepak Gupta | riscv: Add usercfi state for task and save/restore of CSR_SSP on trap entry/exit |
| [5ef3ce94](https://github.com/RVCK-Project/rvck/commit/5ef3ce94f8192356d915e9c6a534f1474d097e4c) | 2026-01-25 | Deepak Gupta | riscv: add Zicfiss / Zicfilp extension CSR and bit definitions |
| [2915a3b6](https://github.com/RVCK-Project/rvck/commit/2915a3b673c7f7fbe484259e87ceb39a77072cba) | 2026-01-25 | Deepak Gupta | riscv: zicfiss / zicfilp enumeration |
| [d43be12c](https://github.com/RVCK-Project/rvck/commit/d43be12ce9aedbab80a09ef7fbb159c14713bf91) | 2026-01-25 | Deepak Gupta | dt-bindings: riscv: document zicfilp and zicfiss in extensions.yaml |
| [3c92924b](https://github.com/RVCK-Project/rvck/commit/3c92924ba7d3269915a9f9a47a130b07c4e1d40e) | 2026-01-25 | Deepak Gupta | mm: add VM_SHADOW_STACK definition for riscv |
| [2956cf70](https://github.com/RVCK-Project/rvck/commit/2956cf70b6886a0edd29035e938b10841bcf982f) | 2024-04-27 | Masahiro Yamada | kbuild: use $(obj)/ instead of $(src)/ for common pattern rules |
| [b7689679](https://github.com/RVCK-Project/rvck/commit/b768967952cbbd689d5901279f3bee79a6af22df) | 2024-03-13 | Vladimir Isaev | riscv: hwprobe: do not produce frtace relocation |
| [fa144759](https://github.com/RVCK-Project/rvck/commit/fa144759c0682c171d9962c0cb845962c6643b8b) | 2025-03-20 | Charlie Jenkins | riscv: entry: Split ret_from_fork() into user and kernel |
| [2198ae23](https://github.com/RVCK-Project/rvck/commit/2198ae239e0ce0ffcd8b0951be115974a0fe4d5b) | 2025-03-20 | Charlie Jenkins | riscv: entry: Convert ret_from_fork() to C |
| [44b89eef](https://github.com/RVCK-Project/rvck/commit/44b89eef92aea825fe8e41517ce1aff8ee4bded7) | 2025-04-11 | Xi Ruoyao | RISC-V: vDSO: Wire up getrandom() vDSO implementation |
| [a4503ced](https://github.com/RVCK-Project/rvck/commit/a4503ced238114ebe85f72c28e24823f43f90860) | 2024-08-22 | Christophe Leroy | random: vDSO: add missing c-getrandom-y in Makefile |
| [03202997](https://github.com/RVCK-Project/rvck/commit/032029974ee481da79e0080d754567fea4491ec6) | 2025-11-12 | Andy Chiu | riscv: signal: abstract header saving for setup_sigcontext |
| [61959d36](https://github.com/RVCK-Project/rvck/commit/61959d36118853178dfd1a89759ea4911caa22c1) | 2023-09-14 | Sohil Mehta | arch: Reserve map_shadow_stack() syscall number for all architectures |
| [b68b2b32](https://github.com/RVCK-Project/rvck/commit/b68b2b329dfe14225ba590d0583d318bc697a303) | 2026-06-04 | ZhenXing Zhu | soc: thead: adapt AON drivers to new th1520-aon protocol API |
| [84f2f672](https://github.com/RVCK-Project/rvck/commit/84f2f672c1a2190ac73cc9d43a08fe360780f90a) | 2026-06-04 | ZhenXing Zhu | drivers/soc/event: Add THEAD TH1520 event driver |
| [2061aedc](https://github.com/RVCK-Project/rvck/commit/2061aedc9a54c541f163c827b6a3838af0adae01) | 2026-06-04 | ZhenXing Zhu | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [d152c092](https://github.com/RVCK-Project/rvck/commit/d152c092bd54d7e7eb63ecf89d23ac354b0f0c0d) | 2026-06-03 | ZhenXing Zhu | riscv: dts: th1520: add watchdog nodes for reboot support |
| [70225593](https://github.com/RVCK-Project/rvck/commit/70225593a5b3ee3cc63c6d29582c9b8dacb8355a) | 2026-06-03 | ZhenXing Zhu | riscv: th1520: lpi4a: fix fan not spinning |
| [321f997b](https://github.com/RVCK-Project/rvck/commit/321f997b39b5e7be502b54e14f92307813b2d40c) | 2026-06-03 | ZhenXing Zhu | riscv: defconfig: th1520: fix DWMAC config symbol for ethernet |
| [33eba004](https://github.com/RVCK-Project/rvck/commit/33eba004af2d5d457821e24bf721a016dc37164e) | 2024-04-19 | Robin Murphy | dma-mapping: Simplify arch_setup_dma_ops() |
| [05ba57da](https://github.com/RVCK-Project/rvck/commit/05ba57da2b5c4feaac961beac5e1935afd6911ea) | 2024-04-19 | Robin Murphy | iommu/dma: Centralise iommu_setup_dma_ops() |
| [f2889544](https://github.com/RVCK-Project/rvck/commit/f2889544ecbb9ffc1b0f5376244a744b87410a74) | 2024-04-19 | Robin Murphy | iommu/dma: Make limit checks self-contained |
| [fe627ef2](https://github.com/RVCK-Project/rvck/commit/fe627ef243951b7067ed54d0e111443ed340e6f8) | 2024-04-19 | Robin Murphy | dma-mapping: Add helpers for dma_range_map bounds |
| [1e56e998](https://github.com/RVCK-Project/rvck/commit/1e56e99819302b5348f855617cc65b5b5639ebc5) | 2024-04-19 | Robin Murphy | ACPI/IORT: Handle memory address size limits as limits |
| [76164b85](https://github.com/RVCK-Project/rvck/commit/76164b85c0cbe272fd666a32c82f9a7560eedbc1) | 2024-04-19 | Robin Murphy | OF: Simplify DMA range calculations |
| [dd1e28c8](https://github.com/RVCK-Project/rvck/commit/dd1e28c85697e75f254fe4cea0906f284cecb5fa) | 2024-04-19 | Robin Murphy | OF: Retire dma-ranges mask workaround |
| [cad8104f](https://github.com/RVCK-Project/rvck/commit/cad8104fd29ab1944f2a190e96c991e0d50eb3cb) | 2024-01-22 | Haibo Xu | KVM: selftests: Add CONFIG_64BIT definition for the build |
| [3b9f6efe](https://github.com/RVCK-Project/rvck/commit/3b9f6efe77de1a9a162f9a232a5063df72324ec4) | 2024-06-03 | Andrew Jones | KVM: selftests: Fix RISC-V compilation |
| [ee308db4](https://github.com/RVCK-Project/rvck/commit/ee308db4882a6ee463d2b189b693a8a4600b9b05) | 2026-05-29 | ZhenXing Zhu | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [96fea4fe](https://github.com/RVCK-Project/rvck/commit/96fea4fe6c571e7391b1a7e5cfd1d98f132b4b51) | 2026-05-29 | ZhenXing Zhu | Revert "KVM: riscv: selftests: Move sbi definitions to its own header file" |
| [afdaefb4](https://github.com/RVCK-Project/rvck/commit/afdaefb4d30935c89852c063e4f4e88f27b7c31b) | 2025-06-26 | Michal Wilczynski | riscv: dts: thead: th1520: Add GPU clkgen reset to AON node |
| [c0ee622f](https://github.com/RVCK-Project/rvck/commit/c0ee622f58657550f8b5d1c5099c7d7676ab1888) | 2025-03-03 | Michal Wilczynski | reset: thead: Add TH1520 reset controller driver |
| [a009114a](https://github.com/RVCK-Project/rvck/commit/a009114abea0c0a3c6a092e12c1c3dd02418aacf) | 2025-03-03 | Michal Wilczynski | dt-bindings: reset: Add T-HEAD TH1520 SoC Reset Controller |
| [571f7a65](https://github.com/RVCK-Project/rvck/commit/571f7a6507d377445d33177a6a7fa557eeb97ad1) | 2026-03-12 | ZhenXing Zhu | Revert "dt-bindings: reset: Document th1520 reset control" |
| [b47ccccb](https://github.com/RVCK-Project/rvck/commit/b47ccccbe99b7578e726b702be95e55559709cbb) | 2026-03-12 | ZhenXing Zhu | Revert "reset: Add th1520 reset driver support" |
| [0f45d395](https://github.com/RVCK-Project/rvck/commit/0f45d395a4fd44f500cf65538ec050d5e8d2d8bf) | 2026-03-12 | ZhenXing Zhu | Revert "reset: th1520: to support npu/fce reset feature" |
| [c3c6665b](https://github.com/RVCK-Project/rvck/commit/c3c6665b002115c18005a4827b4ef4c9a25ac6b8) | 2026-04-20 | ZhenXing Zhu | riscv: dts: thead: Fix aon node for OpenSBI compatibility |
| [f01f49d0](https://github.com/RVCK-Project/rvck/commit/f01f49d0843f20bbe739690ffaf83f6ddaad0c3b) | 2026-04-17 | ZhenXing Zhu | clk: Kconfig: Restore thead clock driver Kconfig inclusion |
| [f9f1af26](https://github.com/RVCK-Project/rvck/commit/f9f1af26be5d1a246ff9857e6583aefa9edd648b) | 2026-03-12 | ZhenXing Zhu | Revert "riscv:dts:thead: Add TH1520 event and watchdog device node" |
| [fdb252b2](https://github.com/RVCK-Project/rvck/commit/fdb252b20e4834b4362810d1960a18fd75fabe4f) | 2026-03-12 | ZhenXing Zhu | Revert "i2s: add i2s driver for XuanTie TH1520 SoC" |
| [d05c5d3b](https://github.com/RVCK-Project/rvck/commit/d05c5d3b38ce010dfe3fa391bca71d1520824576) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: add tdm driver for XuanTie TH1520 SoC" |
| [b5c42443](https://github.com/RVCK-Project/rvck/commit/b5c424435ef1b3c53a31fe410cbdac3c89f1a3c1) | 2026-03-12 | ZhenXing Zhu | Revert "dts: audio: to support i2s-8ch feature" |
| [66c5c97e](https://github.com/RVCK-Project/rvck/commit/66c5c97e13d71dc7d2f5db0f5c45ba23a361a258) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: to support tdm/spdif feature" |
| [49eaeb27](https://github.com/RVCK-Project/rvck/commit/49eaeb278e8fed8054160a8b3dd9d0e7d38a3bca) | 2025-02-19 | Michal Wilczynski | riscv: dts: thead: Introduce power domain nodes with aon firmware |
| [3924e538](https://github.com/RVCK-Project/rvck/commit/3924e5386f75bbe72d05740c029b6729443cfd23) | 2024-02-08 | Krzysztof Kozlowski | pmdomain: core: constify of_phandle_args in xlate |
| [d0db66f0](https://github.com/RVCK-Project/rvck/commit/d0db66f0f203786e3a29886ebf47f5db6d1361ad) | 2025-03-11 | Michal Wilczynski | dt-bindings: power: Add TH1520 SoC power domains |
| [0bc06a5d](https://github.com/RVCK-Project/rvck/commit/0bc06a5d27143f508a07a78eadcd5ea4a0c68c2d) | 2023-09-11 | Ulf Hansson | pmdomain: Prepare to move Kconfig files into the pmdomain subsystem |
| [b38dfd74](https://github.com/RVCK-Project/rvck/commit/b38dfd740fd699e80604754b4deb6b68a749fc09) | 2025-03-14 | Arnd Bergmann | pmdomain: thead: fix TH1520_AON_PROTOCOL dependency |
| [4d4605b3](https://github.com/RVCK-Project/rvck/commit/4d4605b3c173f0756c0c54eaada3ee0d41b68672) | 2025-03-11 | Michal Wilczynski | pmdomain: thead: Add power-domain driver for TH1520 |
| [299158c0](https://github.com/RVCK-Project/rvck/commit/299158c09fe950c20de2274b14025d0d90e4ab29) | 2025-03-11 | Michal Wilczynski | firmware: thead: Add AON firmware protocol driver |
| [3ca85c32](https://github.com/RVCK-Project/rvck/commit/3ca85c32b9e620b069c6e86608e72ab0fec0f3df) | 2026-03-11 | ZhenXing Zhu | Revert "regdump:add regdump support for lpi4a and light-a && rename some dts nam... |
| [0e5c6307](https://github.com/RVCK-Project/rvck/commit/0e5c63071561670121260c799b6bbeda968b5d13) | 2026-03-11 | ZhenXing Zhu | Revert "drivers/soc/event: Add THEAD TH1520 event driver" |
| [4dbc5496](https://github.com/RVCK-Project/rvck/commit/4dbc54962631ee42705c69e4964ed484f26d42ab) | 2026-03-11 | ZhenXing Zhu | Revert "add c906 audio support" |
| [df019728](https://github.com/RVCK-Project/rvck/commit/df0197285ff9a9e35fd1eb8ffba0d4c09045b673) | 2026-03-11 | ZhenXing Zhu | Revert "drivers: regulator: add th1520 AON virtual regulator control support." |
| [56c42c50](https://github.com/RVCK-Project/rvck/commit/56c42c502a0a601267235ce6bb9327044905d6c2) | 2026-03-10 | ZhenXing Zhu | Revert "drivers/watchdog: Add THEAD TH1520 pmic watchdog driver" |
| [a8467904](https://github.com/RVCK-Project/rvck/commit/a8467904f123ff4884b578c11c7da2f9afc70bd8) | 2026-03-10 | ZhenXing Zhu | Revert "firmware: thead: c910_aon: add th1520 Aon protocol driver" |
| [3a33d954](https://github.com/RVCK-Project/rvck/commit/3a33d9540e59841301e7839592c7397d2151e42d) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: cpufreq: add cpufreq driver." |
| [50cc73fb](https://github.com/RVCK-Project/rvck/commit/50cc73fbd77df25faa7d186cafd5751383d77db1) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to modify rvbook dts" |
| [0e19847a](https://github.com/RVCK-Project/rvck/commit/0e19847a5b4a2596599ad1916ee7bd4f13ed6f76) | 2026-03-10 | ZhenXing Zhu | Revert "audio: th1520: add soundcard dts node of th1520-a-val board" |
| [14bb72d0](https://github.com/RVCK-Project/rvck/commit/14bb72d097eaeada62f153ce1537e8a5990e452b) | 2026-03-10 | ZhenXing Zhu | Revert "dts:th1520-a: add th1520-a-val.dts and th1520-a-val-sec.dts" |
| [d1a4c8db](https://github.com/RVCK-Project/rvck/commit/d1a4c8db781df602e5aab459a2b1c00eef65dbf3) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: pmdomain: support th1520 Power domain control." |
| [b651ac3c](https://github.com/RVCK-Project/rvck/commit/b651ac3cc55576fea50c3ff9c06afbc28892c5c0) | 2026-03-10 | ZhenXing Zhu | Revert "dts: add GPU device node" |
| [c9eeaafc](https://github.com/RVCK-Project/rvck/commit/c9eeaafc7bc6514ef5753fa057a125fb3c33aedc) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [69edec49](https://github.com/RVCK-Project/rvck/commit/69edec499ff47c8c01e48aac2e853acfb2cf5149) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add npu device node" |
| [eab7202f](https://github.com/RVCK-Project/rvck/commit/eab7202fdc38f22f9f2fbed5e37a427048b92c66) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to add npu device node" |
| [11d822de](https://github.com/RVCK-Project/rvck/commit/11d822de8a4653a11eb0ac46789bcc8d7599ef03) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add th1520-a-val-crash.dts and th1520-lpi4a-product-crash.d... |
| [90207b71](https://github.com/RVCK-Project/rvck/commit/90207b71718cb377bd8c6b693a89e17bec39ccf9) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [1c7eb155](https://github.com/RVCK-Project/rvck/commit/1c7eb155328b4e0743a63d8329a5ef15e4dd82ad) | 2026-03-10 | ZhenXing Zhu | Revert "dtb:lipi:enable VI module config" |
| [0d64fcf0](https://github.com/RVCK-Project/rvck/commit/0d64fcf02501eb318e0e0faf6341d13cc3990f0c) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add vdec venc and video mem device node" |
| [b2340311](https://github.com/RVCK-Project/rvck/commit/b2340311174c43c6ebb2d99861fa69ca3c995b9f) | 2026-03-10 | ZhenXing Zhu | Revert "chore: use xuantie instead of thead" |
| [5d50e30a](https://github.com/RVCK-Project/rvck/commit/5d50e30a4d74d4eadc39904677689a7d1bb07c36) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce NAPOT field to PTDUMP |
| [8b91d89d](https://github.com/RVCK-Project/rvck/commit/8b91d89d75632b6e766c2e3c5e1c3c6fb46e2527) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce PBMT field to PTDUMP |
| [470044c6](https://github.com/RVCK-Project/rvck/commit/470044c6d06d7842de07121b3306f46fa6303970) | 2023-09-21 | Yu Chien Peter Lin | riscv: Improve PTDUMP to show RSW with non-zero value |
| [b346c682](https://github.com/RVCK-Project/rvck/commit/b346c682b2bf7705ff0a150d1d559d5a8cce3cf6) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add commandline option for SBI PMU test |
| [62a0c7ed](https://github.com/RVCK-Project/rvck/commit/62a0c7edaefdbd1b5f74543704d5034166c7b6dd) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for counter overflow |
| [35ff5f52](https://github.com/RVCK-Project/rvck/commit/35ff5f52c1252222a242b3c7f967ec5d467c5f20) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for PMU snapshot functionality |
| [810d57c4](https://github.com/RVCK-Project/rvck/commit/810d57c485bf08d3c82408c3799fcd7cf3cc6739) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU selftest |
| [0d088977](https://github.com/RVCK-Project/rvck/commit/0d08897718d6287d61e32de8778a50b748fdafd6) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU extension definitions |
| [b7681564](https://github.com/RVCK-Project/rvck/commit/b7681564618bc949f02b7a92ccc953241ee987b0) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add Sscofpmf to get-reg-list test |
| [cc03ef4a](https://github.com/RVCK-Project/rvck/commit/cc03ef4ad0768c4379ac4cf2bea6357e9faada2f) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add helper functions for extension checks |
| [4bc4cc36](https://github.com/RVCK-Project/rvck/commit/4bc4cc36154950cb3fb6481e4711556dc081ceee) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [97375ba4](https://github.com/RVCK-Project/rvck/commit/97375ba4990db6a7bacf2cfc1c604aa39f94968e) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add sstc timer test |
| [aa0feaf6](https://github.com/RVCK-Project/rvck/commit/aa0feaf6e3edd91076734e1e8feb54bba9c4e819) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Change vcpu_has_ext to a common function |
| [2ba8101b](https://github.com/RVCK-Project/rvck/commit/2ba8101b36252f9da6eb1809859d3fcb990e83d4) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add guest helper to get vcpu id |
| [ac40add9](https://github.com/RVCK-Project/rvck/commit/ac40add9881f150c3eaa659b3ba71c9cd06723c0) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add exception handling support |
| [61e668dd](https://github.com/RVCK-Project/rvck/commit/61e668dd7a61cd2cb0c0752c83d20b798d473010) | 2024-01-22 | Haibo Xu | KVM: arm64: selftests: Split arch_timer test code |
| [567e6fdc](https://github.com/RVCK-Project/rvck/commit/567e6fdc79991cdf7250be9a4a3b2d1647933c3c) | 2024-01-22 | Paolo Bonzini | selftests/kvm: Fix issues with $(SPLIT_TESTS) |
| [0422bea7](https://github.com/RVCK-Project/rvck/commit/0422bea70ab3b4783c6b9cd1c3d79e164f8c7849) | 2024-04-20 | Atish Patra | RISC-V: KVM: Improve firmware counter read function |
| [e0dc9ff1](https://github.com/RVCK-Project/rvck/commit/e0dc9ff1c12da7805f93a72a3ef327d64d16a3f1) | 2024-04-20 | Atish Patra | RISC-V: KVM: Support 64 bit firmware counters on RV32 |
| [70b46eee](https://github.com/RVCK-Project/rvck/commit/70b46eee16b9f470e39bc46d096a893ca9fa2ab3) | 2024-04-20 | Atish Patra | RISC-V: KVM: Add perf sampling support for guests |
| [dca6f83d](https://github.com/RVCK-Project/rvck/commit/dca6f83d71778d7befb2229d9bd723e76b398e9b) | 2024-04-20 | Atish Patra | RISC-V: KVM: Implement SBI PMU Snapshot feature |
| [23e8e814](https://github.com/RVCK-Project/rvck/commit/23e8e8149c0211280e146cf0fa89fa1ead621f24) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to exit to the user space if perf event failed |
| [5506b2bf](https://github.com/RVCK-Project/rvck/commit/5506b2bf31e09b3f91263e05e0f4558b1982aab8) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to update the counter value during reset |
| [d4f71f3f](https://github.com/RVCK-Project/rvck/commit/d4f71f3f8091c341c62d971a70907a49d10c24b9) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Implement SBI PMU snapshot function |
| [32532cc8](https://github.com/RVCK-Project/rvck/commit/32532cc815044e8e9438a3bd738ce3166e6f3a82) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Fix counter mask iteration for RV32 |
| [40e9dd4a](https://github.com/RVCK-Project/rvck/commit/40e9dd4a14cb48865b128531044b65fd25d7af96) | 2024-04-20 | Atish Patra | RISC-V: Use the minor version mask while computing sbi version |
| [b45b648e](https://github.com/RVCK-Project/rvck/commit/b45b648ecb0b14b8f89c9d217f56fde7e7ded1a4) | 2024-04-20 | Atish Patra | RISC-V: KVM: Rename the SBI_STA_SHMEM_DISABLE to a generic name |
| [8e5dc896](https://github.com/RVCK-Project/rvck/commit/8e5dc896e7fd69bb0b139052d8cc86e7e0089380) | 2024-04-20 | Atish Patra | RISC-V: Add SBI PMU snapshot definitions |
| [d2e9995f](https://github.com/RVCK-Project/rvck/commit/d2e9995fb0c08b198bfce0795b77819ce0f679a4) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Use BIT macro for shifting operations |
| [5dedb9ef](https://github.com/RVCK-Project/rvck/commit/5dedb9ef47dd244192dd78c45574b7443e7437c9) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Read upper bits of a firmware counter |
| [d617d590](https://github.com/RVCK-Project/rvck/commit/d617d590b5667ad79d609043a073b7e977ddcafb) | 2024-04-20 | Atish Patra | RISC-V: Add FIRMWARE_READ_HI definition |
| [72a47e1a](https://github.com/RVCK-Project/rvck/commit/72a47e1a9d3faf040bcf7436ba15d0ac119c28d3) | 2024-04-20 | Atish Patra | RISC-V: Fix the typo in Scountovf CSR name |
| [d65ac0fe](https://github.com/RVCK-Project/rvck/commit/d65ac0fe31909c0568741b72f12036ad0491d414) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add get-reg-list test for STA registers |
| [89fcd2b2](https://github.com/RVCK-Project/rvck/commit/89fcd2b2bc060d2f47940715f69d194dd5d6aeaf) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add steal_time test support |
| [3e8832c9](https://github.com/RVCK-Project/rvck/commit/3e8832c9d6b63c9064f684ff13a4b51d25e19e41) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add guest_sbi_probe_extension |
| [0a3bfa49](https://github.com/RVCK-Project/rvck/commit/0a3bfa49d9dfe6494d8b7d56244ed81ad4b30524) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Move sbi_ecall to processor.c |
| [5410c5b1](https://github.com/RVCK-Project/rvck/commit/5410c5b1bcf8fe49152d834c3ab810cbe660286e) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Implement SBI STA extension |
| [73c87ade](https://github.com/RVCK-Project/rvck/commit/73c87adeff1af2a1f19bfb608b8d20c434ffa324) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI STA registers |
| [c4b00059](https://github.com/RVCK-Project/rvck/commit/c4b00059738cd3db03fd3bcbc852e6df61a218bc) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI extension registers |
| [ac016487](https://github.com/RVCK-Project/rvck/commit/ac0164878966e390b2d9196b54d5566c0579a024) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA info to vcpu_arch |
| [87e16ebd](https://github.com/RVCK-Project/rvck/commit/87e16ebd3cfffe651bbb8350719904c92b37dbb1) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add steal-update vcpu request |
| [ccbfd2e6](https://github.com/RVCK-Project/rvck/commit/ccbfd2e652e642424319b563486a23fc9c604a5a) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA extension skeleton |
| [1ee1d145](https://github.com/RVCK-Project/rvck/commit/1ee1d145b93d49dbf2fb1cb2d9252cce49755e81) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Implement steal-time support |
| [ec6e4e96](https://github.com/RVCK-Project/rvck/commit/ec6e4e96f05fa5732ab3a4799fe669e2198624e1) | 2023-12-20 | Andrew Jones | RISC-V: Add SBI STA extension definitions |
| [c2ea67f9](https://github.com/RVCK-Project/rvck/commit/c2ea67f9e758cdd668ae0f204dabba4894a530c7) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Add skeleton for pv-time support |
| [1285f8fa](https://github.com/RVCK-Project/rvck/commit/1285f8fa89a530c709cdb142f55334cc2826943b) | 2023-12-13 | Andrew Jones | KVM: riscv: selftests: Add RISCV_SBI_EXT_REG |
| [ce048b35](https://github.com/RVCK-Project/rvck/commit/ce048b3507e55a74535d43d599ae617d41b751f4) | 2023-12-13 | Andrew Jones | RISC-V: KVM: Make SBI uapi consistent with ISA uapi |
| [f034df9c](https://github.com/RVCK-Project/rvck/commit/f034df9c232b5c950043451e407754830926a803) | 2023-11-28 | Anup Patel | KVM: riscv: selftests: Generate ISA extension reg_list using macros |
| [d2487663](https://github.com/RVCK-Project/rvck/commit/d2487663be0b75c52af92197e6f5cef85ca3973f) | 2023-10-19 | Thomas Huth | KVM: selftests: Use TAP in the steal_time test |
| [fcdffaaa](https://github.com/RVCK-Project/rvck/commit/fcdffaaa43864ad6cd97aea2329e064acdb68c84) | 2023-09-20 | Andrew Jones | KVM: riscv: selftests: get-reg-list print_reg should never fail |
| [288a219b](https://github.com/RVCK-Project/rvck/commit/288a219b59087db6ba8ac7cbd1b75750e4366755) | 2023-08-17 | Andrew Jones | KVM: selftests: Add array order helpers to riscv get-reg-list |
| [ad920589](https://github.com/RVCK-Project/rvck/commit/ad920589f69d11fd8bf8339710c95b08f29cb164) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Switch to use macro from csr.h |
| [1c4cd487](https://github.com/RVCK-Project/rvck/commit/1c4cd4874480b1aee8f0fa89fadeb0d726be6764) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file vdso/processor.h |
| [d4fcf3b5](https://github.com/RVCK-Project/rvck/commit/d4fcf3b590c8f782f9328cb2ca620987955fbf40) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file csr.h |
| [4c946674](https://github.com/RVCK-Project/rvck/commit/4c9466741544f8d086d31c219380979de177577b) | 2024-11-08 | Charlie Jenkins | riscv: Fix default misaligned access trap |
| [3c056d03](https://github.com/RVCK-Project/rvck/commit/3c056d036dbca120af2279d13319bf025418a58e) | 2025-12-06 | Eric Biggers | lib/crypto: riscv: Depend on RISCV_EFFICIENT_VECTOR_UNALIGNED_ACCESS |
| [a48dd38d](https://github.com/RVCK-Project/rvck/commit/a48dd38dfd490ec4ef6998fa190778e33a05d5ab) | 2025-03-04 | Andrew Jones | Documentation/kernel-parameters: Add riscv unaligned speed parameters |
| [0356deeb](https://github.com/RVCK-Project/rvck/commit/0356deeb4090e0394eea477c1acdf64699891b5e) | 2025-03-04 | Andrew Jones | riscv: Add parameter for skipping access speed tests |
| [96475d98](https://github.com/RVCK-Project/rvck/commit/96475d98533928def517fe3374d90da36df75aa9) | 2025-03-04 | Andrew Jones | riscv: Fix set up of vector cpu hotplug callback |
| [2be82d63](https://github.com/RVCK-Project/rvck/commit/2be82d63ba7fa3ffcb00ac3d375af4704015912e) | 2025-03-04 | Andrew Jones | riscv: Fix set up of cpu hotplug callbacks |
| [a3e21cdf](https://github.com/RVCK-Project/rvck/commit/a3e21cdfa08ca625b9416fb2902c691d2274eff9) | 2025-03-04 | Andrew Jones | riscv: Change check_unaligned_access_speed_all_cpus to void |
| [4ee3aed5](https://github.com/RVCK-Project/rvck/commit/4ee3aed5e1595ba7185c8ea2669bc5afc213eb4f) | 2025-03-04 | Andrew Jones | riscv: Fix check_unaligned_access_all_cpus |
| [6d3be83a](https://github.com/RVCK-Project/rvck/commit/6d3be83a6efdc14c378300ca4725198418ec708e) | 2025-03-04 | Andrew Jones | riscv: Fix riscv_online_cpu_vec |
| [3e614fe4](https://github.com/RVCK-Project/rvck/commit/3e614fe4e7092d998be323d3f0556936d5b44465) | 2025-03-04 | Andrew Jones | riscv: Annotate unaligned access init functions |
| [b15b0d35](https://github.com/RVCK-Project/rvck/commit/b15b0d35f23ce891b56f6e67c4c18bfa7a4354b1) | 2024-10-17 | Jesse Taube | RISC-V: hwprobe: Document unaligned vector perf key |
| [fb7881d1](https://github.com/RVCK-Project/rvck/commit/fb7881d134649a54fd85be2ac64c61fe27a0f38e) | 2024-10-17 | Jesse Taube | RISC-V: Report vector unaligned access speed hwprobe |
| [84f08a50](https://github.com/RVCK-Project/rvck/commit/84f08a50ff1c791f797414047f5777106577e7a1) | 2024-10-17 | Jesse Taube | RISC-V: Detect unaligned vector accesses supported |
| [54bd81f5](https://github.com/RVCK-Project/rvck/commit/54bd81f5c349c446f421a7280a8a12084208a450) | 2024-10-17 | Jesse Taube | RISC-V: Replace RISCV_MISALIGNED with RISCV_SCALAR_MISALIGNED |
| [709dd0f4](https://github.com/RVCK-Project/rvck/commit/709dd0f48d2c117e4bb3b9190bc352c3a59e87c0) | 2024-10-17 | Jesse Taube | RISC-V: Scalar unaligned access emulated on hotplug CPUs |
| [5dfe2ade](https://github.com/RVCK-Project/rvck/commit/5dfe2ade6f253ba8672061414dd9f900d8e616c1) | 2024-10-17 | Jesse Taube | RISC-V: Check scalar unaligned access on all CPUs |
| [92bbe4e8](https://github.com/RVCK-Project/rvck/commit/92bbe4e812a424e1f84d7395ee16d4e06c9c7e01) | 2024-08-14 | Samuel Holland | riscv: misaligned: Restrict user access to kernel memory |
| [f450677d](https://github.com/RVCK-Project/rvck/commit/f450677dd0283a750341eb29b06c3cb9ea215bcf) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add SCALAR to misaligned perf defines |
| [2c11e094](https://github.com/RVCK-Project/rvck/commit/2c11e09431f8d06fd9b79ca698baedf4cdab3da8) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add MISALIGNED_PERF key |
| [e579bcb7](https://github.com/RVCK-Project/rvck/commit/e579bcb7dcddcb2e5af9b03864297826225dc9e9) | 2024-03-17 | Xingyou Chen | riscv: typo in comment for get_f64_reg |
| [24e5d743](https://github.com/RVCK-Project/rvck/commit/24e5d7436245f472c3901e5da970045d34959130) | 2024-03-08 | Charlie Jenkins | riscv: Set unaligned access speed at compile time |
| [b0479f81](https://github.com/RVCK-Project/rvck/commit/b0479f81fb2d6f8974b3cf35df9f357d5173272c) | 2024-03-08 | Charlie Jenkins | riscv: Decouple emulated unaligned accesses from access speed |
| [deb468f0](https://github.com/RVCK-Project/rvck/commit/deb468f03cbe6fdbbb2c15fa9dc1b0c2bb6c93ad) | 2024-03-08 | Charlie Jenkins | riscv: Only check online cpus for emulated accesses |
| [95cd9939](https://github.com/RVCK-Project/rvck/commit/95cd99392e79a3b090174ba35e19024c04e87ad4) | 2024-03-08 | Charlie Jenkins | riscv: lib: Introduce has_fast_unaligned_access() |
| [7083a26e](https://github.com/RVCK-Project/rvck/commit/7083a26eb57263253ac0aec6b02dce26c5e9c34f) | 2024-02-12 | Eric Biggers | crypto: riscv - add vector crypto accelerated AES-CBC-CTS |
| [ed645eec](https://github.com/RVCK-Project/rvck/commit/ed645eec33d4f6734d22c6f31a26aff2ad9b9a48) | 2024-02-06 | Clément Léger | riscv: misaligned: remove CONFIG_RISCV_M_MODE specific code |
| [0b7e0ca8](https://github.com/RVCK-Project/rvck/commit/0b7e0ca8f0c484c80feef454f26b6eb9d9d0fa6e) | 2024-01-08 | Charlie Jenkins | kunit: Add tests for csum_ipv6_magic and ip_fast_csum |
| [f2aface1](https://github.com/RVCK-Project/rvck/commit/f2aface1bc6cbaa6277901f1196355d69ac76e07) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum library |
| [c927bdde](https://github.com/RVCK-Project/rvck/commit/c927bdde043829e4f1dd7582c192ff97c3820d91) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum header |
| [f2fe50c4](https://github.com/RVCK-Project/rvck/commit/f2fe50c436e969197c74a0439b8cc555ddfa3700) | 2024-01-08 | Charlie Jenkins | riscv: Add static key for misaligned accesses |
| [b89300eb](https://github.com/RVCK-Project/rvck/commit/b89300ebabca076a281124ca1b1cd00ec355833a) | 2024-01-08 | Charlie Jenkins | asm-generic: Improve csum_fold |
| [3736086c](https://github.com/RVCK-Project/rvck/commit/3736086ce23646e0b5f7bcf77655b672ad6666b0) | 2023-12-25 | Jisheng Zhang | riscv: select DCACHE_WORD_ACCESS for efficient unaligned access HW |
| [1461f0ff](https://github.com/RVCK-Project/rvck/commit/1461f0ff96cf3923026a639f8bcc6a5130cd1b39) | 2023-12-25 | Jisheng Zhang | riscv: introduce RISCV_EFFICIENT_UNALIGNED_ACCESS |
| [b36662d1](https://github.com/RVCK-Project/rvck/commit/b36662d178cd4332bfeae8cc1e5735578bdfd6c0) | 2023-11-23 | Ben Dooks | riscv; fix __user annotation in save_v_state() |
| [a4605078](https://github.com/RVCK-Project/rvck/commit/a4605078951da9ca91e7b970b9d99cfa71464fe0) | 2023-11-23 | Ben Dooks | riscv: fix __user annotation in traps_misaligned.c |
| [561378f8](https://github.com/RVCK-Project/rvck/commit/561378f8e5aab08b7f4c8a59c45e6a9c2fb33899) | 2023-11-06 | Evan Green | RISC-V: Show accurate per-hart isa in /proc/cpuinfo |
| [aa5670a3](https://github.com/RVCK-Project/rvck/commit/aa5670a37a36a1cd3cc1af5fac287f9bf6cefa95) | 2026-02-02 | Chen Pei | Revert "riscv:uprobe: fix flush_icache to ensure that instructions are refreshed... |
| [321714e0](https://github.com/RVCK-Project/rvck/commit/321714e014150314ea48ef0c42e83c45675e26a8) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM4 |
| [6b1514d7](https://github.com/RVCK-Project/rvck/commit/6b1514d7230d39d593188a2e2a980fdb6e64ad7c) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM3 |
| [acf0aeca](https://github.com/RVCK-Project/rvck/commit/acf0aeca07f399ca4aebede48d0843f10a11c855) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{512,384} |
| [395edcd3](https://github.com/RVCK-Project/rvck/commit/395edcd386d45e55fc008fd02c8aa29dde693c38) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{256,224} |
| [84b70c42](https://github.com/RVCK-Project/rvck/commit/84b70c42823d5777f207b436d2769f1f35e4b109) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated GHASH |
| [a7e48d1d](https://github.com/RVCK-Project/rvck/commit/a7e48d1d52cb2141ade7d81643aed579a7c59447) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated ChaCha20 |
| [34c64d2c](https://github.com/RVCK-Project/rvck/commit/34c64d2c2c7a198aa548c358301d5e7a44ad7306) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated AES-{ECB,CBC,CTR,XTS} |
| [9e757a6d](https://github.com/RVCK-Project/rvck/commit/9e757a6d273813d7e6712b00362755c5429a763d) | 2024-01-21 | Heiko Stuebner | RISC-V: hook new crypto subdir into build-system |
| [70122b1f](https://github.com/RVCK-Project/rvck/commit/70122b1fc7960601ee6903cdb462efa958e842cd) | 2024-01-21 | Eric Biggers | RISC-V: add TOOLCHAIN_HAS_VECTOR_CRYPTO |
| [d18b4da5](https://github.com/RVCK-Project/rvck/commit/d18b4da55aa2af4870d0f0b267a59ca2cd1e9fe3) | 2024-01-21 | Heiko Stuebner | RISC-V: add helper function to read the vector VLEN |
| [e41d37b6](https://github.com/RVCK-Project/rvck/commit/e41d37b6a3b1c5a8c778847f42e618a62ada5e94) | 2023-10-12 | Ian Rogers | perf pmu: Lazily compute default config |
| [6a47f701](https://github.com/RVCK-Project/rvck/commit/6a47f7012642267644d6edb0c42486661ece8d6e) | 2023-10-12 | Ian Rogers | perf pmu-events: Remember the perf_events_map for a PMU |
| [ca4ca1ec](https://github.com/RVCK-Project/rvck/commit/ca4ca1ecaf7ed71544eb2d6ecea68397206fcdb4) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify perf_pmu__config_terms |
| [5d8e2ba6](https://github.com/RVCK-Project/rvck/commit/5d8e2ba6f49676ac3b9233f24f6b673976c99fc2) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify file APIs |
| [e886a860](https://github.com/RVCK-Project/rvck/commit/e886a8604981c60880cd31ce92c9d6a7fdd044f6) | 2023-10-12 | Ian Rogers | perf arm-spe: Move PMU initialization from default config code |
| [cc0895e4](https://github.com/RVCK-Project/rvck/commit/cc0895e4bf741909b82b5e9a4186c6998f74669e) | 2023-10-12 | Ian Rogers | perf intel-pt: Move PMU initialization from default config code |
| [8f5b8881](https://github.com/RVCK-Project/rvck/commit/8f5b88814d6c650dfe9ec54f21eda159533b5165) | 2023-10-12 | Ian Rogers | perf pmu: Rename perf_pmu__get_default_config to perf_pmu__arch_init |
| [0d3c6987](https://github.com/RVCK-Project/rvck/commit/0d3c6987ace717338ea920cf5639ff7a3233ee14) | 2023-09-24 | Ian Rogers | perf pmus: Make PMU alias name loading lazy |
| [4293643c](https://github.com/RVCK-Project/rvck/commit/4293643c13381ade5ec2438fdf482b8a1735380f) | 2023-09-01 | Ian Rogers | perf parse-events: Introduce 'struct parse_events_terms' |
| [ae624c55](https://github.com/RVCK-Project/rvck/commit/ae624c551206d1e52b73aa7f061efb69e75025c5) | 2023-09-01 | Ian Rogers | perf parse-events: Copy fewer term lists |
| [84da7495](https://github.com/RVCK-Project/rvck/commit/84da7495f3fdae459649bd0fb1c8707fab3d0ae9) | 2023-09-01 | Ian Rogers | perf parse-events: Avoid enum casts |
| [935c28d5](https://github.com/RVCK-Project/rvck/commit/935c28d5bfa618ea4ad2ee9620c3ccd4572377e5) | 2023-09-01 | Ian Rogers | perf parse-events: Tidy up str parameter |
| [c4faef28](https://github.com/RVCK-Project/rvck/commit/c4faef289e8f0174fc565a48fe992d1e6e0ff123) | 2023-09-01 | Ian Rogers | perf parse-events: Remove unnecessary __maybe_unused |
| [eeeec364](https://github.com/RVCK-Project/rvck/commit/eeeec364fde0ac6b71793c33850129f82266390e) | 2025-04-21 | Guo Ren | riscv: xchg: Prefetch the destination word for sc.w |
| [03aacb4f](https://github.com/RVCK-Project/rvck/commit/03aacb4f84db44c7d3f4baf97dd1ee3aca878759) | 2025-04-21 | Guo Ren | riscv: Add ARCH_HAS_PREFETCH[W] support with Zicbop |
| [ed367c19](https://github.com/RVCK-Project/rvck/commit/ed367c193419b23ad4f075dd2b6c5e70343eddd1) | 2025-01-02 | Guo Ren | iommu/riscv: Fixup compile warning |
| [09666946](https://github.com/RVCK-Project/rvck/commit/09666946a4b9c66d99f49efbd06ea508fcdfc6b0) | 2024-04-22 | Shenlin Liang | perf kvm/riscv: Port perf kvm stat to RISC-V |
| [6a2682d0](https://github.com/RVCK-Project/rvck/commit/6a2682d00d2cadae365cd2f136feba109219e1b8) | 2024-04-22 | Shenlin Liang | RISCV: KVM: add tracepoints for entry and exit events |
| [10396772](https://github.com/RVCK-Project/rvck/commit/10396772a4bf1ea3db99239223956a85d6565a1c) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Add separate ticket-lock.h |
| [2d024dbc](https://github.com/RVCK-Project/rvck/commit/2d024dbc94d8ed46c12a5eed02a9e906ddf307f4) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Reuse arch_spinlock_t of qspinlock |
| [5eb63c70](https://github.com/RVCK-Project/rvck/commit/5eb63c70b80d44a257b48188867a0c9fafa6ed1a) | 2023-09-08 | Guo Ren | asm-generic: ticket-lock: Optimize arch_spin_value_unlocked() |
| [501c678f](https://github.com/RVCK-Project/rvck/commit/501c678f163513f3e8623caa99b4b278f19177bb) | 2023-09-18 | Baolin Wang | mm: add statistics for PUD level pagetable |
| [7fc500d0](https://github.com/RVCK-Project/rvck/commit/7fc500d0368956e4c74f027b0286c94e2c63a637) | 2024-09-01 | xianbing Zhu | hibernate: reduce logs in nosave_region register/remove |
| [49e54bb3](https://github.com/RVCK-Project/rvck/commit/49e54bb39739c7b8a3ecf1c1e137ded9e6296b4d) | 2024-09-01 | Xiangyi Zeng | drivers: pwm_bl: fix adjusting failure of pwm backlight when resume from str |
| [1efeaeb2](https://github.com/RVCK-Project/rvck/commit/1efeaeb2a642ee24dc3965acf8dff4a859e5a205) | 2024-09-01 | David Li | audio: th1520: solve misalignment dma addr problem when HDMI playback |
| [7a13989b](https://github.com/RVCK-Project/rvck/commit/7a13989bae1546a3648f31e5f573e73f16c8ace4) | 2024-09-01 | Xiangyi Zeng | drivers: i2c-hid: fix rvbook i2c-hid wake-up error |
| [7efacf73](https://github.com/RVCK-Project/rvck/commit/7efacf73d5c59b9278c3e072ed9ab23ff36c6c52) | 2024-09-01 | xianbing Zhu | hibernate: add blkdev flush op berfore poweroff |
| [35d485e2](https://github.com/RVCK-Project/rvck/commit/35d485e2b3b87b836c056899d431daf119ee6221) | 2024-09-01 | David Li | audio: light: i2s dma channel acquire change to dynamic |
| [b1a8014f](https://github.com/RVCK-Project/rvck/commit/b1a8014f0e8da2795ffe46b63f8df3810a64409e) | 2024-09-01 | Xiangyi Zeng | dts: crash: enable watchdog0 and watchdog1 in crash kernel |
| [9ea94f6b](https://github.com/RVCK-Project/rvck/commit/9ea94f6bf6ae5914accf76b5da9d12c29b1ca2bc) | 2024-09-01 | tingming | defconfig: th1520: to fix problem of do_compile_kernelmodules fail |
| [4bcb382a](https://github.com/RVCK-Project/rvck/commit/4bcb382ac1d67db8232b23911bebb454c55a8795) | 2024-09-01 | tingming | defconfig: th1520: to add configs of media |
| [c0f07bae](https://github.com/RVCK-Project/rvck/commit/c0f07baec2ea7cb634d690f7c04cb415eb1deffc) | 2024-09-01 | Xiangyi Zeng | dts: som: add gpio-keys node |
| [66725388](https://github.com/RVCK-Project/rvck/commit/667253880d14943ec738fe4efb5f42bd7579a04f) | 2024-09-01 | Xiangyi Zeng | driver: wdt: th1520_wdt: add th1520_wdt driver pm ops |
| [9aa5449c](https://github.com/RVCK-Project/rvck/commit/9aa5449c022bb0eeddb9d65507f7c4300222779e) | 2024-09-01 | Xiangyi Zeng | drivers: mtd: nand: spi: support new winbond-spi-nand devices |
| [ed1ed6c4](https://github.com/RVCK-Project/rvck/commit/ed1ed6c4ade838a165ee97f27cb1f8991ecfd729) | 2024-09-01 | tingming | config: th1520: to modify MEMBLOCK&LOCALVERSION_AUTO config |
| [b7a76954](https://github.com/RVCK-Project/rvck/commit/b7a76954f4ec1cf1e1eb5772afa8336602fb4f85) | 2024-09-01 | xianbing Zhu | dts:th1520: fix sd card wprtn issue |
| [a58e8681](https://github.com/RVCK-Project/rvck/commit/a58e86816082bff70c13c7a23dab74e17159bb97) | 2024-09-01 | xianbing Zhu | dts:th1520: add aliases for basic interface |
| [78f3fd23](https://github.com/RVCK-Project/rvck/commit/78f3fd2354df3d41d1226c9c8f4ef4cbac98dea4) | 2024-09-01 | shuofeng.ren | audio: light_fm: change hdmi config for ap i2s hdmi audio |
| [54c3c8f4](https://github.com/RVCK-Project/rvck/commit/54c3c8f49d854818c368e8437fd1da9b7f5fa39c) | 2024-09-01 | Huaming | driver:i2c:fix rx_over miss re-try |
| [68f271e8](https://github.com/RVCK-Project/rvck/commit/68f271e8f0ee59ac60f5a48af5ef98fa5c79d7cc) | 2024-09-01 | Esther Z | drivers: cpufreq: fix cpufreq em model regsiter warn. |
| [efa50c3a](https://github.com/RVCK-Project/rvck/commit/efa50c3a8a2145ef8f20223d96cef66a386b6b01) | 2024-09-01 | Xiangyi Zeng | drivers: pwm: fix pwm enable status check error |
| [5cd35e62](https://github.com/RVCK-Project/rvck/commit/5cd35e6276bd0c73c7d555e37e6006810639b081) | 2024-09-01 | xianbing Zhu | th1520: sdhci: add quirks for th1520-a-val board |
| [8e0dfe33](https://github.com/RVCK-Project/rvck/commit/8e0dfe3365b567151d3e217546ffb7e1b1f1717d) | 2024-09-01 | xianbing Zhu | sdhci:th1520: fixup mmc retune infinitely issue |
| [306fb320](https://github.com/RVCK-Project/rvck/commit/306fb32070e5233e79a9bbf5bf41d2ac065cc20f) | 2024-09-01 | David Li | dts: audio: update audio pin configuration |
| [ddff0f4c](https://github.com/RVCK-Project/rvck/commit/ddff0f4c7a93a96a8bed857658d640a72c176a3c) | 2024-09-01 | David Li | audio: th1520: workaround to solve noise at the end |
| [98d34450](https://github.com/RVCK-Project/rvck/commit/98d344505d6522f4463c95da09872fe6ad56d292) | 2024-09-01 | tingming | dts: th1520_defconfig: to revert the config CONFIG_USB_GADGET |
| [f205bd8d](https://github.com/RVCK-Project/rvck/commit/f205bd8d06e6a725f2218a40a05f95adad316fbb) | 2024-09-01 | Xiangyi Zeng | dts: gpio: add gpio/0/1/2/3 clk gate |
| [2dbfa59c](https://github.com/RVCK-Project/rvck/commit/2dbfa59c1f21f6620e0f6e59b273c73528b6e204) | 2024-09-01 | xianbing Zhu | driver: pty/clk: bootargs options for pty and clk_debug init |
| [39d7c88e](https://github.com/RVCK-Project/rvck/commit/39d7c88e95aeab907c23bbc660cef78a49db0779) | 2024-09-01 | xianbing Zhu | hibernate: improve crc32 speed by calc compressed data |
| [7ceac106](https://github.com/RVCK-Project/rvck/commit/7ceac106414d9dc7ab3313f09524cc965df56fd0) | 2024-09-01 | xianbing Zhu | trace: add bootargs options for trace and ftrace for hibernate resume |
| [845c8854](https://github.com/RVCK-Project/rvck/commit/845c885475f2ab46a230c09cac3014eca598111e) | 2024-09-01 | xianbing Zhu | hibernate: restart when resume fail and added resume dev |
| [57d6140a](https://github.com/RVCK-Project/rvck/commit/57d6140af0494a6ab0b4479001003aa9b3e91ff3) | 2024-09-01 | xianbing Zhu | hibernate: get buffer page speedup with no GPF_ZERO |
| [7bed0390](https://github.com/RVCK-Project/rvck/commit/7bed0390837270f5a171000db0dc8d3c01ac2c4f) | 2024-09-01 | xianbing Zhu | hibernate: speedup image crc caculation by multi-threads |
| [81e7f594](https://github.com/RVCK-Project/rvck/commit/81e7f59425193ca18c9b550733ed927d5ca17011) | 2024-09-01 | xianbing Zhu | hibernate: load image retry in some crc mismatch |
| [4d3b3beb](https://github.com/RVCK-Project/rvck/commit/4d3b3bebd3eab06cce36ef917b913de182f3bef3) | 2024-09-01 | xianbing Zhu | dts: th1520: update mmc sdio clocks |
| [2ca27856](https://github.com/RVCK-Project/rvck/commit/2ca278567a54c38ba3e4abf68921b3d53ecb8968) | 2024-09-01 | tingming | dts: rvbook: to modity rvbook dts |
| [bf4552e2](https://github.com/RVCK-Project/rvck/commit/bf4552e22d57cecd46cc26c28f241a11d93d500f) | 2024-09-01 | Xiangyi Zeng | drivers: iopmp: add iopmp driver |
| [82b76390](https://github.com/RVCK-Project/rvck/commit/82b763901d043e6d9519fa6fea9e51e34fdd0681) | 2024-09-01 | Xiangyi Zeng | drivers: pct: fix NULL ptr error when pvt resume from ram |
| [4c27b81d](https://github.com/RVCK-Project/rvck/commit/4c27b81d23b0d7042ee55d9c8795f7fdb3ae99ce) | 2024-09-01 | Xiangyi Zeng | drivers: pinctrl: add pinctrl driver pm ops |
| [d47a13f6](https://github.com/RVCK-Project/rvck/commit/d47a13f61006cc657776bd219387abbd282b7cac) | 2024-09-01 | Xiangyi Zeng | dts: th1520: modify kernel memory region, [0x0, 0x20000] is reserved for opensbi |
| [d244b79b](https://github.com/RVCK-Project/rvck/commit/d244b79bc12eef5c76201828453d428b720774ea) | 2024-09-01 | tingming | dts: th1520: to modify rvbook dts |
| [b455e683](https://github.com/RVCK-Project/rvck/commit/b455e6833763e85b22c6bbafc9fb3b6c36d8913e) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [f8360bb0](https://github.com/RVCK-Project/rvck/commit/f8360bb04344e97508ec2d1c51d0de74fe3a69c4) | 2024-09-01 | David Li | audio: th1520: to support tdm/spdif feature |
| [05adab4d](https://github.com/RVCK-Project/rvck/commit/05adab4d34c178ca7fb50bbbed9247dc2a45d726) | 2024-09-01 | David Li | dts: rvbook: to adapt compatible 'thead,th1520' of rvbook |
| [ae051076](https://github.com/RVCK-Project/rvck/commit/ae051076b10a50a73d2d71b08b501eedce710094) | 2024-09-01 | xianbing Zhu | riscv: hibernate: crash dump memory mark as nosave |
| [8ccd535c](https://github.com/RVCK-Project/rvck/commit/8ccd535c0e4bf86588ef2263a5b7de8dcd1f34d2) | 2024-09-01 | xianbing Zhu | hibernate:snaoshot: detail show copied pfn info |
| [084cc1ab](https://github.com/RVCK-Project/rvck/commit/084cc1ab43130d2ed2c1046e1a32fcb8ce408c3f) | 2024-09-01 | xianbing Zhu | hibernate: extended platform ops for all mode |
| [62261e92](https://github.com/RVCK-Project/rvck/commit/62261e927d5c6778c22039b23300896c3ca34daa) | 2024-09-01 | xianbing Zhu | hibernate: add interface for driver to mark no_save region |
| [b4a9efa2](https://github.com/RVCK-Project/rvck/commit/b4a9efa2cf3d4ec6caf753fa989f114a3dc8e4f3) | 2024-09-01 | Xiangyi Zeng | drivers: dma: change dw dma runtime pm to sync |
| [b5b99583](https://github.com/RVCK-Project/rvck/commit/b5b99583f699ab20064411f9d9e06a569f2a9965) | 2024-09-01 | Xiangyi Zeng | audio: th1520: fix i2s pause/resume dma fail |
| [0aae62d0](https://github.com/RVCK-Project/rvck/commit/0aae62d0af294b5cde6586730a3c15a29dc2aa3d) | 2024-09-01 | Hao Li | dma: dw-axi-dmac: fix multi-channel dmatest fail |
| [26ff2b2a](https://github.com/RVCK-Project/rvck/commit/26ff2b2a61a0eb777a9b165dfef882daf8b0c0c4) | 2024-09-01 | Xiangyi Zeng | audio: th1520: resolve the dma error problem when ap i2s audio stop play |
| [1ea19652](https://github.com/RVCK-Project/rvck/commit/1ea19652d7be1d64780513173c345b563e287fce) | 2024-09-01 | Xiangyi Zeng | watchdog: dw_wdt: ap watchdog timeout report different strategies |
| [00dc2d7e](https://github.com/RVCK-Project/rvck/commit/00dc2d7eb285ca0931e3e075b63583a415bad767) | 2024-09-01 | Xiangyi Zeng | drivers: wdt: drop DW wdt restart function for th1520 soc |
| [f58ffb2f](https://github.com/RVCK-Project/rvck/commit/f58ffb2f19a09d8bf70534e669d2b186ed2d0652) | 2024-09-01 | Xiangyi Zeng | drivers: hwmon: bugfix for mr75203 |
| [4c21d97a](https://github.com/RVCK-Project/rvck/commit/4c21d97afd8f5e28259014b6d394fe95102c7f1f) | 2024-09-01 | Xiangyi Zeng | spi: spidev: add spidev speed sysfs for debug |
| [782111ae](https://github.com/RVCK-Project/rvck/commit/782111aeaabf308ca40136aa7bbdd2ed370d1d6d) | 2024-09-01 | Huaming | dtb:th15210: correct vi camera2 path cfg |
| [3876fb19](https://github.com/RVCK-Project/rvck/commit/3876fb197c6c7fee68492f4309606012702917ad) | 2024-09-01 | David Li | dts: update pin/regulator configuration in rvbook dts file |
| [d4ceae38](https://github.com/RVCK-Project/rvck/commit/d4ceae38d13429f0ee9a363da904db151c5a83d1) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add th1520-a-val-crash.dts and th1520-lpi4a-product-crash.dts for k... |
| [3f9c7b32](https://github.com/RVCK-Project/rvck/commit/3f9c7b32554b4fd0ff77a1bf779d4ccb477cef5d) | 2024-09-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [ef411bef](https://github.com/RVCK-Project/rvck/commit/ef411bef4e62f7df49f0613505e24a46aaefe6a9) | 2024-09-01 | Huaming | dtb:th1520: LA Board correct cma cfg |
| [6e4defbd](https://github.com/RVCK-Project/rvck/commit/6e4defbde5119d235dc05928a9ef3921ea9d6ded) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [7c7c44d7](https://github.com/RVCK-Project/rvck/commit/7c7c44d71befc9cd575d0c0d0796a9ec167f7497) | 2024-09-01 | David Li | perf: sync vendor event path with torvalds/linux |
| [11e7e13a](https://github.com/RVCK-Project/rvck/commit/11e7e13abf54152d342cdf7985e686f6c6b61701) | 2024-09-01 | David Li | dts: audio: to support i2s-8ch feature |
| [6c3d2e9a](https://github.com/RVCK-Project/rvck/commit/6c3d2e9ae0b8bcd227ed4d6b7c4f94cadcc66b3f) | 2024-09-01 | tingming | th1520: system_monitor: to add system_monitor driver |
| [a55be692](https://github.com/RVCK-Project/rvck/commit/a55be69253b13ed8de857ee11ad16b826ec3b9f2) | 2024-09-01 | David Li | dts: audio: to adapt compatible 'thead,th1520' |
| [6aa3ef68](https://github.com/RVCK-Project/rvck/commit/6aa3ef685474817949534b63c08d0d203b2420e9) | 2024-09-01 | Huaming | dtb:th1520:light add vi module |
| [8cdb4227](https://github.com/RVCK-Project/rvck/commit/8cdb42273626614d2873573927b8af967aed9cad) | 2024-09-01 | xiaojin.cxj | regdump:add regdump support for lpi4a and light-a && rename some dts name from "... |
| [16e50e61](https://github.com/RVCK-Project/rvck/commit/16e50e6156dad72fb47f32238af3926d94dcaed3) | 2024-09-01 | Huaming | driver:i2c:add i2c mode for tx transfer |
| [4967f796](https://github.com/RVCK-Project/rvck/commit/4967f796d7d9261a23538db60397b7e2db74dfeb) | 2024-09-01 | David Li | dts: audio: correct audiosys pinctrl name |
| [9b61330b](https://github.com/RVCK-Project/rvck/commit/9b61330b21063b876d3c1d4e51058272172682b6) | 2024-09-01 | David Li | audio: th1520: support audiosys pinctrl feature |
| [79ed0d9c](https://github.com/RVCK-Project/rvck/commit/79ed0d9cad084dbd21adc62aab811e07480650df) | 2024-09-01 | Xiangyi Zeng | dts: th1520: remove usb hub node from th1520-a board |
| [33377591](https://github.com/RVCK-Project/rvck/commit/33377591bd3e9dd7148ae1ff3781ac6727ccd851) | 2024-09-01 | tingming | hwspinlock: th1520: to add th1520 hwspinlock driver |
| [2146166b](https://github.com/RVCK-Project/rvck/commit/2146166b9697d97a25944e03fb9749bcb65176d2) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add rambus eip_28 device node |
| [b7b95d21](https://github.com/RVCK-Project/rvck/commit/b7b95d2110c6adf8801132a13f388e26e51f38fe) | 2024-09-01 | Esther Z | dts: th1520: add LA regulator support. |
| [37913208](https://github.com/RVCK-Project/rvck/commit/37913208a08f0f9532d9fb16c96e6a1bb36eb5a5) | 2024-09-01 | Xiangyi Zeng | drivers:misc:dsmart: add th1520 iso7816 driver |
| [fcaaa23f](https://github.com/RVCK-Project/rvck/commit/fcaaa23fa5c64c247865f628f4d0115c02d29c47) | 2024-09-01 | Xiangyi Zeng | drivers:nvmem:efuse: add th1520 efuse driver |
| [baac7ccb](https://github.com/RVCK-Project/rvck/commit/baac7ccb283881b296f26c394cd0b5b41a824e7e) | 2024-09-01 | David Li | audio: th1520: add soundcard dts node of th1520-a-val board |
| [487fd99a](https://github.com/RVCK-Project/rvck/commit/487fd99a91b1b43df69c1da261d0c6c00575231c) | 2024-09-01 | tingming | dts: th1520: to add npu device node |
| [3b24a3b2](https://github.com/RVCK-Project/rvck/commit/3b24a3b2676ee09725dc4717daa6b1ae10c31ca3) | 2024-09-01 | Xiangyi Zeng | riscv:dts:som: add th1520-lpi4a-product and th1520-lpi4a-product-sec dts file |
| [47ce0901](https://github.com/RVCK-Project/rvck/commit/47ce0901e7fcafc95c7952718270c45d75a23e60) | 2024-09-01 | Xiangyi Zeng | dts:th1520-a: add th1520-a-val.dts and th1520-a-val-sec.dts |
| [8bb2cd67](https://github.com/RVCK-Project/rvck/commit/8bb2cd6719e88853f985699ddabc94d4c90fa2b9) | 2024-06-21 | Huaming | defconfig:th1520: enable cma config |
| [cc3706b4](https://github.com/RVCK-Project/rvck/commit/cc3706b4cbcfbe73afac26e962922861f780454f) | 2024-09-01 | Huaming | dtb:lipi:enable VI module config |
| [640b21df](https://github.com/RVCK-Project/rvck/commit/640b21dfc479746799553a7cab24eb5198408834) | 2024-09-01 | xianbing Zhu | riscv: config: add MAX_ORDER config for riscv and th1520 |
| [86a34a6b](https://github.com/RVCK-Project/rvck/commit/86a34a6b6bafe51072c9d45c5a0f1930364f698c) | 2024-09-01 | xianbing Zhu | dts: th1520: add vdec venc and video mem device node |
| [4c5d4777](https://github.com/RVCK-Project/rvck/commit/4c5d4777a7e3733739927f5098598d5308a6e34e) | 2024-08-31 | xianbing Zhu | mm: export sysmbol __pte_offset_map_lock for modules outside |
| [2f296195](https://github.com/RVCK-Project/rvck/commit/2f296195e62b1bcc0e53372169f363e8737a4e56) | 2024-08-31 | David Li | audio: th1520: add pa aw87519 driver for XuanTie TH1520 SoC |
| [8aec74c0](https://github.com/RVCK-Project/rvck/commit/8aec74c005b97a5e0ea35e2eda0fb7979e42de99) | 2024-08-31 | David Li | audio: th1520: add spdif driver for XuanTie TH1520 SoC |
| [e02f0dca](https://github.com/RVCK-Project/rvck/commit/e02f0dcad2eb4564a4cdcf38d226c2c6d1787931) | 2024-08-31 | David Li | audio: th1520: add tdm driver for XuanTie TH1520 SoC |
| [b897850e](https://github.com/RVCK-Project/rvck/commit/b897850ee06d362a5ff18cd03437bc4f69d61b9e) | 2024-09-06 | Guo Ren | riscv: mm: Add support for Svinval extension |
| [e8d1a934](https://github.com/RVCK-Project/rvck/commit/e8d1a934dc98911c820202df02e606a1c48c37d2) | 2024-09-04 | Guo Ren | riscv: Add ACLINT SSWI support |
| [4f31a41f](https://github.com/RVCK-Project/rvck/commit/4f31a41f94a23c5e5d2c5a1cc77e87f38f35c114) | 2024-07-29 | forain | drm: Fix HDMI hot-plug problem |
| [642642b0](https://github.com/RVCK-Project/rvck/commit/642642b07285792526d573fc44817f61debee4be) | 2024-07-25 | Hao Li | gpu/drm: hdmi: Add hdmi debounce to enhance hdmi plugin/out stable |
| [c24b9578](https://github.com/RVCK-Project/rvck/commit/c24b95782bc4fe1935b0c3ca68d122bef68f760a) | 2024-07-21 | David Li | audio: th1520: fixup compile warning of i2s driver |
| [35927bca](https://github.com/RVCK-Project/rvck/commit/35927bcab625ac0f6d59bf38bb49d8cda48340e0) | 2024-07-12 | David Li | dmaengine: dw-axi-dmac: Add support for Xuantie TH1520 DMA |
| [e56083e7](https://github.com/RVCK-Project/rvck/commit/e56083e730509c3b23e2d72178ececa3f16c6eaa) | 2024-07-04 | Chen Pei | arch:rsicv:select ARCH_HAS_DMA_WRITE_COMBINE |
| [1775090e](https://github.com/RVCK-Project/rvck/commit/1775090ee94d59332429bf5b7f2db3a931022d9a) | 2024-07-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [f033fcbe](https://github.com/RVCK-Project/rvck/commit/f033fcbec5819e8bcd4c43efc03d0692ef7d7774) | 2024-06-30 | Huaming | driver:padctrl:correct th1520 gpio_1 24/25 cfg |
| [8e4cbbe7](https://github.com/RVCK-Project/rvck/commit/8e4cbbe7487b750c8f8c289c75840464457d5750) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [3cdc83e0](https://github.com/RVCK-Project/rvck/commit/3cdc83e0e6ca45713c95b185a1d992f521a088b5) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [530785a9](https://github.com/RVCK-Project/rvck/commit/530785a9f594a37abe27617926bf10d433bd4a69) | 2024-07-04 | Xiangyi Zeng | drivers: event: add macro definition to control SW_PANIC event |
| [377b76c2](https://github.com/RVCK-Project/rvck/commit/377b76c2e8f39942e0eec61dc2d4a2e473effe6c) | 2024-06-28 | David Li | audio: th1520: enable soundcard feature |
| [dbfdbf4b](https://github.com/RVCK-Project/rvck/commit/dbfdbf4b41e02c2ed2fe4ae565ebc4fc69c9f8b2) | 2024-06-27 | David Li | audio: th1520: support audiosys pinctrl feature |
| [cb340f29](https://github.com/RVCK-Project/rvck/commit/cb340f29cdbb69c771981759b5aee8af75ea2c3b) | 2024-06-26 | Xiangyi Zeng | dts: th1520: fix interrupt number config error in dts |
| [f3d99048](https://github.com/RVCK-Project/rvck/commit/f3d99048203d5f7b7fbb5c4bd1088da6aa902548) | 2024-06-24 | forain | DPU: add DPU driver for Lichee-Pi-4A board |
| [d7ab70a5](https://github.com/RVCK-Project/rvck/commit/d7ab70a5c8c9b802bccfffa4a3d1a60ebc502788) | 2024-06-23 | tingming | dts: th1520: add npu device node |
| [cad39eed](https://github.com/RVCK-Project/rvck/commit/cad39eedc71588d7ff685d80e9a423a4a3627c82) | 2024-06-21 | David Li | codec: audio: add codec driver for Lichee-Pi-4A board |
| [8f415a09](https://github.com/RVCK-Project/rvck/commit/8f415a09998f3b12ab46a9535f6ace77e6ae0606) | 2024-06-20 | Chen Pei | riscv: vector: Fix the boot issue compiled using xuantie-toolchain or upstream-t... |
| [526c5cc0](https://github.com/RVCK-Project/rvck/commit/526c5cc0eb3511680054d160dac4aa6dbb404ffa) | 2024-06-19 | Esther Z | drivers: cpufreq: add cpufreq driver. |
| [f659a8e6](https://github.com/RVCK-Project/rvck/commit/f659a8e6b981f52ade211cb7df16057dad01a748) | 2024-06-18 | Esther Z | riscv: dts: Introduce lichee-pi-4a fixed regulator support. |
| [42cfb735](https://github.com/RVCK-Project/rvck/commit/42cfb73532bc3e6694401436d96449f70f3d09b0) | 2024-06-17 | zhangye | Enable XUANTIE ISA for memcpy performance |
| [24152a94](https://github.com/RVCK-Project/rvck/commit/24152a948ba797ae59dad1e3ce868a313df62090) | 2024-03-27 | Chen Pei | riscv: build: Support compiling kernel using Xuantie toolchain |
| [b822c4cd](https://github.com/RVCK-Project/rvck/commit/b822c4cd0505147f2c707de2964fba4fd80b3c1a) | 2024-06-17 | David Li | i2s: remove debug message |
| [7f78f065](https://github.com/RVCK-Project/rvck/commit/7f78f06524f752c3acae0eec3d976630962bdc9d) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the aon gpio range configuration error |
| [29f6b01b](https://github.com/RVCK-Project/rvck/commit/29f6b01bce3e347481f4d7872c293a66870ec79a) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix spi/qspi1 cs pin duplicate configuration error |
| [38568df1](https://github.com/RVCK-Project/rvck/commit/38568df10430fe8cb832a9e15ddd0bf7b7add2aa) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the gpio range configuration error |
| [7452e16f](https://github.com/RVCK-Project/rvck/commit/7452e16f2dfbef49d61f6b43f30f0dcc17325958) | 2024-06-16 | Esther Z | drivers: regulator: add th1520 AON virtual regulator control support. |
| [7c0c49bb](https://github.com/RVCK-Project/rvck/commit/7c0c49bb08dea224246abe0ed521f5d57d6070ba) | 2024-06-16 | Esther Z | dt-bindings: add AON resource id headfile |
| [ed771d79](https://github.com/RVCK-Project/rvck/commit/ed771d79b218f62f9a3a57589b72f317a37375a5) | 2024-06-16 | Esther Z | drivers: pmdomain: support th1520 Power domain control. |
| [556c946c](https://github.com/RVCK-Project/rvck/commit/556c946cca6b40f99c4017cb9f3bafbf8b71317b) | 2024-06-15 | David Li | i2s: add i2s driver for XuanTie TH1520 SoC |
| [0da9067e](https://github.com/RVCK-Project/rvck/commit/0da9067e50c709c9acd441fd4abcb6fb9ab67554) | 2024-06-15 | David Li | configs: xuantie: correct definition of SoC Architecture |
| [d1f1debc](https://github.com/RVCK-Project/rvck/commit/d1f1debc9bf06b909e3fd082905ab91704be6e3b) | 2024-06-11 | lst | i2c: designware: add support for hcnt/lcnt got from dt |
| [8fc31db9](https://github.com/RVCK-Project/rvck/commit/8fc31db9fb44aa886218ec7209e8426b13bb8d44) | 2024-06-11 | xiaojin.cxj | add 902 share mem log |
| [9d86b872](https://github.com/RVCK-Project/rvck/commit/9d86b872197f9cf1b4a4384adf72a409929ccc34) | 2024-06-05 | abing | refine thead,th1520-usb.yaml |
| [6f226e67](https://github.com/RVCK-Project/rvck/commit/6f226e67bc83b945c91d6b92005f3d68f6d056af) | 2024-06-05 | abing | drivers: usb: dwc3: add usb_mode usb_speed param for mode speed change when insm... |
| [6ed231c3](https://github.com/RVCK-Project/rvck/commit/6ed231c34d3b0c741256cb91a4bbf3f2f0711d46) | 2024-06-05 | abing | drivers: usb: add dwc3-thead.c |
| [9b5ab579](https://github.com/RVCK-Project/rvck/commit/9b5ab5796478a13b50b064a0dd3caffda346ff31) | 2024-06-05 | abing | th1520.dtsi: refine usb dts |
| [2b4f0e7e](https://github.com/RVCK-Project/rvck/commit/2b4f0e7e96b9a8f70a8da12fad2d9ce9f6c9be1e) | 2024-06-06 | Xiangyi Zeng | riscv:dts:thead: Add TH1520 event and watchdog device node |
| [706ff4dd](https://github.com/RVCK-Project/rvck/commit/706ff4dd9928773537f332907fd4c11d1b5794cc) | 2024-06-06 | Xiangyi Zeng | dt-bindings:wdt: Add Documentation for THEAD TH1520 pmic watchdog |
| [3a10a72d](https://github.com/RVCK-Project/rvck/commit/3a10a72d61031ba94a6197958b5f3e77a5da24a3) | 2024-06-06 | Xiangyi Zeng | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [7b791854](https://github.com/RVCK-Project/rvck/commit/7b7918545779438cbae59f958e481d9a4f4028b7) | 2024-06-06 | Xiangyi Zeng | dt-bindings:event: Add Documentation for THEAD TH1520 event driver |
| [0aabdf36](https://github.com/RVCK-Project/rvck/commit/0aabdf36549dc8cdff7b44e66e785d5a6f410a24) | 2024-06-06 | Xiangyi Zeng | drivers/soc/event: Add THEAD TH1520 event driver |
| [1eebdea2](https://github.com/RVCK-Project/rvck/commit/1eebdea24217d4227140ffe8f1be2c7a41adf90a) | 2024-06-07 | xiaojin.cxj | fix rpmsg addr cast warning |
| [4680cc95](https://github.com/RVCK-Project/rvck/commit/4680cc95d4d9ba4e92b41d6f64dff4f7c2040cd1) | 2024-06-07 | xiaojin.cxj | fix lp32 compile warnoing for rpmsg |
| [ada58884](https://github.com/RVCK-Project/rvck/commit/ada58884a550949ac478a5597e5b0d814d187f19) | 2024-06-07 | xiaojin.cxj | fix proc log warning |
| [044956fd](https://github.com/RVCK-Project/rvck/commit/044956fd9440571782e55021bb198f391b34a7d1) | 2024-06-06 | xiaojin.cxj | fix rpmsg warning |
| [39e63e9b](https://github.com/RVCK-Project/rvck/commit/39e63e9b0c76c7d61063656f801e8db34c65689d) | 2024-06-06 | xiaojin.cxj | add c906 audio support |
| [0173de1a](https://github.com/RVCK-Project/rvck/commit/0173de1a8cf232db0fada29197202f397eed58e5) | 2024-06-05 | xianbing Zhu | net:stmmac: increase timeout for dma reset |
| [ae79db4b](https://github.com/RVCK-Project/rvck/commit/ae79db4b782be0bc978d6b82fcb91aac16833c52) | 2024-06-05 | xianbing Zhu | stmmac:dwmac-thead: add support for suspend/resume feature |
| [27b95c70](https://github.com/RVCK-Project/rvck/commit/27b95c70b3cf4c2e85f44fb4a6f622be64b8ef2c) | 2024-06-04 | xianbing Zhu | net:dwmac-thead: dd ptp clk set and enable |
| [3966e935](https://github.com/RVCK-Project/rvck/commit/3966e9350f805877c7005b0baa3ce106b0eb0926) | 2024-06-05 | Esther Z | configs: Enable th1520 mailbox. |
| [e8025993](https://github.com/RVCK-Project/rvck/commit/e8025993d3cd489cfa28a5863cdee6b6f50d476b) | 2024-06-04 | Esther Z | drivers:ipc: update th1520 rpc msg version 2 |
| [61a1ed4c](https://github.com/RVCK-Project/rvck/commit/61a1ed4c6cf08aca02f5700c0254a8e44037a07f) | 2021-08-10 | fugang.duan | firmware: thead: c910_aon: add th1520 Aon protocol driver |
| [6969fe05](https://github.com/RVCK-Project/rvck/commit/6969fe056be0e7cd5aa1f72f4882eea74c608778) | 2024-06-04 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 add delay line in different mode and sdio rxclk del... |
| [d009212d](https://github.com/RVCK-Project/rvck/commit/d009212db33350fcb54db5d42201a4097a101118) | 2024-06-03 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 larger tuning max loop count to 128 |
| [c81c6010](https://github.com/RVCK-Project/rvck/commit/c81c6010ddb3c9bc55694e8217536b662b397e8f) | 2024-05-31 | xianbing Zhu | dts: th1520: enable sdio1 for wifi card in lichee-pi-4a |
| [2344c5bb](https://github.com/RVCK-Project/rvck/commit/2344c5bb5f09704129a51ece38e9c8ee8e284c6e) | 2024-05-31 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 sdhci add fix io voltage 1v8 |
| [1bfb6a71](https://github.com/RVCK-Project/rvck/commit/1bfb6a71475450cad5219d68883f915cf7b10544) | 2024-05-30 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 resolve accss rpmb error in hs400 |
| [b78bb3d8](https://github.com/RVCK-Project/rvck/commit/b78bb3d874bbfe4a70e6d5acbf5e8df1bd463c9c) | 2024-05-30 | Xiangyi Zeng | drivers/dmac: add pm suspend/resume for dma driver |
| [7dbead9e](https://github.com/RVCK-Project/rvck/commit/7dbead9e8aa47e8a25621923a5fc1798675bd72c) | 2023-08-21 | David Li | audio: th1520: add dma chan str for dmaengine |
| [d156cd12](https://github.com/RVCK-Project/rvck/commit/d156cd12aeff93c0c210b5ba256fdcc772051c5a) | 2024-05-30 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 dmac1 and dmac2 device node |
| [5882a98e](https://github.com/RVCK-Project/rvck/commit/5882a98e6d608c2fe9e55610c5a2050503ee00bc) | 2023-08-22 | sanyi | STR: fix pca953x resume bug |
| [65bffcee](https://github.com/RVCK-Project/rvck/commit/65bffceed99aac0acf121f498001c7060ba8630f) | 2024-05-28 | Xiangyi Zeng | drivers/iio/adc: add sysfs_remove_file when adc driver removed |
| [8dcd61ab](https://github.com/RVCK-Project/rvck/commit/8dcd61ab4252b45ef1de45f8d18527b5f3b4f889) | 2024-05-27 | Xiangyi Zeng | drivers/pvt: add mr75203 driver pm feature and correct temperature coefficient |
| [206db659](https://github.com/RVCK-Project/rvck/commit/206db659167f9f46f51996df34e97c9b5c299680) | 2024-05-27 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 SPI/QSPI device node |
| [75a3c4e6](https://github.com/RVCK-Project/rvck/commit/75a3c4e6d12354582003bf442f3bbc10e8ecfc5c) | 2024-05-27 | Xiangyi Zeng | dt-bindings: spi/qspi: Add Documentation for THEAD TH1520 SPI/QSPI |
| [5f388175](https://github.com/RVCK-Project/rvck/commit/5f3881751bc83289aeb462259c5a04c77bab28a4) | 2024-05-27 | Xiangyi Zeng | drivers/spi: Add THEAD TH1520 QSPI driver |
| [177aac9a](https://github.com/RVCK-Project/rvck/commit/177aac9a86ea1875f2928000bc82f79172b4f667) | 2024-05-27 | tingming | reset: th1520: to support npu/fce reset feature |
| [8bc5af6c](https://github.com/RVCK-Project/rvck/commit/8bc5af6c64500a89aeeba600c893b02e5a45dda7) | 2024-05-19 | Wei Fu | riscv: dts: thead: Add XuanTie TH1520 Mailbox device node |
| [ee4b2d96](https://github.com/RVCK-Project/rvck/commit/ee4b2d96797af1350e95e26e0befd9143ddb36b5) | 2024-05-17 | Fugang Duan | mailbox: add XuanTie TH1520 Mailbox IPC driver |
| [c447b29c](https://github.com/RVCK-Project/rvck/commit/c447b29c1fe81d028615d8b2d9ddf4ca4d2fb962) | 2024-05-19 | Wei Fu | dt-bindings: mailbox: Add a binding file for XuanTie TH1520 Mailbox |
| [0b275ee8](https://github.com/RVCK-Project/rvck/commit/0b275ee8bca4ef2c0bade225db6626c7630d138a) | 2024-05-22 | tingming | riscv: dts: thead: to add th1520 clk nodes |
| [01d1f788](https://github.com/RVCK-Project/rvck/commit/01d1f788d62f10c92078dfbecd802ac6c746c4e6) | 2024-05-22 | tingming | drivers: clk: to add thead th1520 clk driver |
| [ea03d852](https://github.com/RVCK-Project/rvck/commit/ea03d852376ade1016ecf3fc0b7934df98ae9b74) | 2024-05-17 | Xiangyi Zeng | dt-bindings: adc: Add Documentation for THEAD TH1520 ADC |
| [aad85e79](https://github.com/RVCK-Project/rvck/commit/aad85e79c36b2cc86ffd7479467a963c11a3de19) | 2024-05-17 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 ADC device node |
| [633ee5e4](https://github.com/RVCK-Project/rvck/commit/633ee5e4b84e11e7ae587e88ba649ae28d5eca06) | 2024-05-17 | Xiangyi Zeng | drivers/iio/adc: Add THEAD TH1520 ADC driver |
| [ceaa2c45](https://github.com/RVCK-Project/rvck/commit/ceaa2c45076194f93b2dabbe2194d275455d7b0a) | 2024-06-29 | Chen Pei | riscv: ptrace: Fix ptrace using uninitialized riscv_v_vsize |
| [d701970c](https://github.com/RVCK-Project/rvck/commit/d701970c3dd4d4b0bf294822ea6a565f0177aca3) | 2024-03-18 | Heiko Stuebner | T-Head C9xx cores implement an older version (0.7.1) of the vector specification... |
---

**共 393 条提交（显示全部）**

[分页显示](阿里达摩院.md) | [纯文本视图](阿里达摩院_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-07-06 01:41:51*
*数据来源: 主分支 rvck-6.6@3eb78001*
