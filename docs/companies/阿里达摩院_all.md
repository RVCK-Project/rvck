# 阿里达摩院 贡献详情

<div style="background-color: #F4433620; padding: 15px; border-radius: 8px; border-left: 5px solid #F44336;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 409</li>
<li><strong>统计时间</strong>: 2026-11-12 15:39:57</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.161</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @linux.alibaba.com

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [bf0f24ca](https://github.com/RVCK-Project/rvck/commit/bf0f24cac5d6c8be3235fe371a772ef7b467f1be) | 2026-11-12 | Xie Bo | [DROPPED] RISC-V: KVM: Serialize IMSIC attributes with vCPU migration |
| [6c6a89bc](https://github.com/RVCK-Project/rvck/commit/6c6a89bc84813d58ab3372921a28da915c279314) | 2026-11-12 | Jiakai Xu | [DROPPED] RISC-V: KVM: Fix null pointer dereference in kvm_riscv_aia_imsic_has_a... |
| [ecf6a13f](https://github.com/RVCK-Project/rvck/commit/ecf6a13faa011e705dc537a9502d81a4e895f746) | 2026-11-12 | Jiakai Xu | [DROPPED] RISC-V: KVM: Fix null pointer dereference in kvm_riscv_aia_imsic_rw_at... |
| [d8c386c7](https://github.com/RVCK-Project/rvck/commit/d8c386c77b9f00c25d2597b17967d45045f38ea3) | 2026-07-18 | Pengpeng Hou | RISC-V: KVM: Reject overlapping AIA IMSIC address fields |
| [a6aa65e0](https://github.com/RVCK-Project/rvck/commit/a6aa65e038fb03abbd7b6db1a60fd0e1dd6fe28d) | 2026-07-28 | Yicong Yang | RISC-V: KVM: Use try_cmpxchg for IMSIC MRIF RMW |
| [26ea757b](https://github.com/RVCK-Project/rvck/commit/26ea757b037965fa50ebec00ab304f23fac588c6) | 2026-01-27 | Jiakai Xu | RISC-V: KVM: Skip IMSIC update if vCPU IMSIC state is not initialized |
| [2e1cbe84](https://github.com/RVCK-Project/rvck/commit/2e1cbe840d6f9491a189bdd7e293f9b7ad2d1b77) | 2026-02-26 | Jiakai Xu | RISC-V: KVM: Fix null pointer dereference in kvm_riscv_vcpu_aia_rmw_topei() |
| [c4b739e5](https://github.com/RVCK-Project/rvck/commit/c4b739e5357ee0704a51c32d3fdd76565210aa1a) | 2026-03-04 | Jiakai Xu | RISC-V: KVM: Fix potential UAF in kvm_riscv_aia_imsic_has_attr() |
| [67208bf1](https://github.com/RVCK-Project/rvck/commit/67208bf112dc41efcb04b25bc8b917d2390fb9b7) | 2026-03-02 | Jiakai Xu | RISC-V: KVM: Fix use-after-free in kvm_riscv_aia_aplic_has_attr() |
| [dd4dd97e](https://github.com/RVCK-Project/rvck/commit/dd4dd97e67db3baff068c2949e6deadf6438b111) | 2026-07-13 | Vivian Wang | mm/sparse-vmemmap: flush_cache_vmap() after hotplugging vmemmap |
| [9a055e39](https://github.com/RVCK-Project/rvck/commit/9a055e39ad5717fa4d6577cf28040611cc881d9b) | 2026-10-09 | ZhenXing Zhu | riscv: mm: Make mark_new_valid_map() stuff depend on 64BIT && MMU |
| [7b9372d5](https://github.com/RVCK-Project/rvck/commit/7b9372d5349e9212f90a36a7308b9e438eec650a) | 2026-03-03 | Vivian Wang | riscv: mm: Unconditionally sfence.vma for spurious fault |
| [550cc989](https://github.com/RVCK-Project/rvck/commit/550cc98961bfd001954ee72167c05c0af05ef4dd) | 2026-10-09 | ZhenXing Zhu | riscv: mm: Use the bitmap API for new_valid_map_cpus |
| [e679254b](https://github.com/RVCK-Project/rvck/commit/e679254be6f62be0f9f66349335b3c12b9526743) | 2026-10-09 | ZhenXing Zhu | riscv: mm: Rename new_vmalloc into new_valid_map_cpus |
| [49d947b5](https://github.com/RVCK-Project/rvck/commit/49d947b51c9f229d10f0666433fc52d311bb217b) | 2026-03-03 | Vivian Wang | riscv: kfence: Call mark_new_valid_map() for kfence_unprotect() |
| [786b65b1](https://github.com/RVCK-Project/rvck/commit/786b65b11395665efd8c153c906482461ba91094) | 2026-03-03 | Vivian Wang | riscv: mm: Extract helper mark_new_valid_map() |
| [a00633da](https://github.com/RVCK-Project/rvck/commit/a00633da5c5c51b2203c8d7816e4f8fe6a16be53) | 2025-01-03 | Clément Léger | riscv: use local label names instead of global ones in assembly |
| [b75d8412](https://github.com/RVCK-Project/rvck/commit/b75d841237d7057bda3b619492faffc4c5918428) | 2026-09-17 | ZhenXing Zhu | irqchip: thead-c900-aclint-sswi: Fixup riscv_ipi_set_virq_range() conflict |
| [e36ace89](https://github.com/RVCK-Project/rvck/commit/e36ace8923b5243f08049c026d6332c342e06bc8) | 2024-05-22 | Palmer Dabbelt | irqchip: riscv-imsic: Fixup riscv_ipi_set_virq_range() conflict |
| [2e1c6f71](https://github.com/RVCK-Project/rvck/commit/2e1c6f719d41d53ce31e6ab3ca6603b7c8a8b951) | 2024-03-26 | Samuel Holland | riscv: mm: Always use an ASID to flush mm contexts |
| [28aae774](https://github.com/RVCK-Project/rvck/commit/28aae774430026959605095e73d5a42bc4c4e4ca) | 2024-03-26 | Samuel Holland | riscv: mm: Preserve global TLB entries when switching contexts |
| [ca96592e](https://github.com/RVCK-Project/rvck/commit/ca96592ee7ef11648b94c9458335de32c57bf2b7) | 2024-03-26 | Samuel Holland | riscv: mm: Make asid_bits a local variable |
| [2ad666a8](https://github.com/RVCK-Project/rvck/commit/2ad666a86257b3c3d9b5bc3097c2b39f27bb8ca1) | 2024-03-26 | Samuel Holland | riscv: mm: Use a fixed layout for the MM context ID |
| [7d6d9d57](https://github.com/RVCK-Project/rvck/commit/7d6d9d57f164ec160f4880325e8841664a494d7e) | 2024-03-26 | Samuel Holland | riscv: mm: Introduce cntx2asid/cntx2version helper macros |
| [054c7c37](https://github.com/RVCK-Project/rvck/commit/054c7c37de63a1ef50327ac0cd3d7be23eb1c7c3) | 2024-03-26 | Samuel Holland | riscv: Avoid TLB flush loops when affected by SiFive CIP-1200 |
| [6e235c2b](https://github.com/RVCK-Project/rvck/commit/6e235c2bc15af27ef2a70bc451fd70b7d4e3fdaa) | 2024-03-26 | Samuel Holland | riscv: mm: Combine the SMP and UP TLB flush code |
| [dfeaac20](https://github.com/RVCK-Project/rvck/commit/dfeaac205c5d9b0983d6576eff00c4892286c087) | 2024-03-26 | Samuel Holland | riscv: Only send remote fences when some other CPU is online |
| [fd86b47a](https://github.com/RVCK-Project/rvck/commit/fd86b47a4183968dbe6faf8c859be74c89d40b88) | 2024-03-26 | Samuel Holland | riscv: mm: Broadcast kernel TLB flushes only when needed |
| [76b7f126](https://github.com/RVCK-Project/rvck/commit/76b7f12603d4e9c0059dcb7c08056becebe1f019) | 2024-03-26 | Samuel Holland | riscv: Use IPIs for remote cache/TLB flushes by default |
| [2dad2ae8](https://github.com/RVCK-Project/rvck/commit/2dad2ae82d26d750e6bb5c42228f5e2ea333f54f) | 2024-03-26 | Samuel Holland | riscv: Factor out page table TLB synchronization |
| [7d74ad1b](https://github.com/RVCK-Project/rvck/commit/7d74ad1bc828e8a0be6df0f5578ca9a78abe2772) | 2024-03-26 | Samuel Holland | riscv: Flush the instruction cache during SMP bringup |
| [b6c0cd3d](https://github.com/RVCK-Project/rvck/commit/b6c0cd3dd1619b98c5be67ce7678528056ef899c) | 2026-08-11 | ZhenXing Zhu | riscv: dts: thead: add TH1520 ACLINT SSWI interrupt-controller node |
| [1d0abe62](https://github.com/RVCK-Project/rvck/commit/1d0abe628bce4be6aae38220b0144c41dbeb8615) | 2026-08-11 | ZhenXing Zhu | Revert "riscv: Add ACLINT SSWI support" |
| [d927b615](https://github.com/RVCK-Project/rvck/commit/d927b615f0f573f8b7716e469dcd8916d4028412) | 2025-10-20 | Vivian Wang | riscv: tests: Make RISCV_KPROBES_KUNIT tristate |
| [3ff7230d](https://github.com/RVCK-Project/rvck/commit/3ff7230dd0ce19e0a76fb254a61c167f4c8a7304) | 2025-05-13 | Nam Cao | riscv: Add kprobes KUnit test |
| [289866f9](https://github.com/RVCK-Project/rvck/commit/289866f9709384679f3fac08996f380adb2aacff) | 2023-11-01 | Charlie Jenkins | riscv: Add tests for riscv module loading |
| [6f7ab613](https://github.com/RVCK-Project/rvck/commit/6f7ab613137286ff7a8db23712269b0b6952553b) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_ITYPE_IMM |
| [31b6cf1f](https://github.com/RVCK-Project/rvck/commit/31b6cf1f46eb0c6ef59f53f69ffe1fe7cefb6dab) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_UTYPE_IMM |
| [b3ec7273](https://github.com/RVCK-Project/rvck/commit/b3ec727339c70a69fafddf6dd3936575bd33508a) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_RD_REG |
| [63b4bb4f](https://github.com/RVCK-Project/rvck/commit/63b4bb4f4b2f49e8d5c6a8e132afffae5425a511) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_BTYPE_IMM |
| [18120dbc](https://github.com/RVCK-Project/rvck/commit/18120dbc09d3ac2fbb1c33566eca2cfcca376b5e) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_C2_RS1_REG |
| [37e4b111](https://github.com/RVCK-Project/rvck/commit/37e4b111248c7fda746943881e90f8e1711d836b) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_JTYPE_IMM |
| [649781ae](https://github.com/RVCK-Project/rvck/commit/649781aef941493707575f46d307fdb41c28986a) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_BTYPE_IMM |
| [b5862b41](https://github.com/RVCK-Project/rvck/commit/b5862b41155bf533fe36b8c95216e3eb52dc94e4) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_RS1_REG |
| [7b3aae56](https://github.com/RVCK-Project/rvck/commit/7b3aae56299ef16d6d13111d554db4a1316c83cf) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_JTYPE_IMM |
| [d359f911](https://github.com/RVCK-Project/rvck/commit/d359f9119e8e65783e3e93f4634ab5eeca7be6f2) | 2025-05-11 | Nam Cao | riscv: kprobes: Move branch_funct3 to insn.h |
| [13a46fd1](https://github.com/RVCK-Project/rvck/commit/13a46fd1808d36f427f6b41584558e0abc7316be) | 2025-05-11 | Nam Cao | riscv: kprobes: Move branch_rs2_idx to insn.h |
| [d96d7bf1](https://github.com/RVCK-Project/rvck/commit/d96d7bf11d991a4436fdf76e6525b4ed3ab4f29f) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: disable unused A210 UARTs by default |
| [030ed077](https://github.com/RVCK-Project/rvck/commit/030ed0778849cae95caee0bc53986cc5be656076) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: enable GPIO controllers on A210 boards |
| [c0a83f96](https://github.com/RVCK-Project/rvck/commit/c0a83f96be57e7f202b718a153f5734f35dfd4b3) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 SPI controllers |
| [96cce2ed](https://github.com/RVCK-Project/rvck/commit/96cce2ed2d5596baeb1811b8b8e855c303c5677c) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 I2C controllers |
| [c591e30f](https://github.com/RVCK-Project/rvck/commit/c591e30f685038354987fd41d39b5a8c50fa1e54) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 DW APB timers |
| [71872b79](https://github.com/RVCK-Project/rvck/commit/71872b79a428236b2cbd05962026b4b0b70c9a24) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 GPIO controllers |
| [1268a564](https://github.com/RVCK-Project/rvck/commit/1268a5641d7e8acae5c1df4080e28c4eff5a41f2) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 DMA controllers |
| [1e452118](https://github.com/RVCK-Project/rvck/commit/1e4521186ea504c54c4ed3d104ca99371e2d1fa2) | 2026-09-21 | Zhiguo Zhu | dmaengine: dw-axi-dmac: add ZhiHe A210 support |
| [ebca8f0b](https://github.com/RVCK-Project/rvck/commit/ebca8f0bd631278fd37c7754330eb10a22777c1c) | 2026-09-21 | Zhiguo Zhu | dt-bindings: dma: snps,dw-axi-dmac: add ZhiHe A210 |
| [1fce9a6c](https://github.com/RVCK-Project/rvck/commit/1fce9a6cba641d8df51324ab3934f9d6fbb9064e) | 2026-09-30 | Zhiguo Zhu | riscv: dts: zhihe: rename A210 board to Melon Pi |
| [7b411457](https://github.com/RVCK-Project/rvck/commit/7b41145778514ca176add31938db3d08a10ef3c5) | 2026-09-30 | Zhiguo Zhu | dt-bindings: riscv: zhihe: rename A210 board to Melon Pi |
| [bbb8af22](https://github.com/RVCK-Project/rvck/commit/bbb8af228bde282ad639cb5a86102f7456c24e3f) | 2026-09-12 | ZhenXing Zhu | firmware: thead: th1520_event: make stubs static inline |
| [bf0edd71](https://github.com/RVCK-Project/rvck/commit/bf0edd710499b1d13b9c2b4650133047009a8271) | 2026-08-31 | Zhiguo Zhu | riscv: rvck_defconfig: enable ZhiHe A210 drivers as modules |
| [69204386](https://github.com/RVCK-Project/rvck/commit/6920438641f76c1f084c246f86cf2a62bcabe9a6) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 power domains |
| [94f97641](https://github.com/RVCK-Project/rvck/commit/94f97641a45494ab528618ad0675436a01aa0c4d) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 AON subsystem |
| [4f29ba6f](https://github.com/RVCK-Project/rvck/commit/4f29ba6f4db8dc361b77755477f9ff37da7cf112) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 pinctrl |
| [e325def6](https://github.com/RVCK-Project/rvck/commit/e325def609f851e87b05d774e69b216d181b9faa) | 2026-08-18 | Zhiguo Zhu | pmdomain: zhihe: add A210 power domain support |
| [50f6baa8](https://github.com/RVCK-Project/rvck/commit/50f6baa8b9cf2be270ec75f8727f64d17fa6c585) | 2026-08-27 | Zhiguo Zhu | dt-bindings: power: zhihe: add A210 power domains |
| [6f79297b](https://github.com/RVCK-Project/rvck/commit/6f79297bc96eac72d26907757f7808b5364eec13) | 2026-08-27 | Zhiguo Zhu | dt-bindings: clock: zhihe: add A210 CCU |
| [adb357e3](https://github.com/RVCK-Project/rvck/commit/adb357e3de50819078552e5cbd44df124693689d) | 2026-08-18 | Zhiguo Zhu | regulator: zhihe: add A210 AON regulator driver |
| [dcb12cae](https://github.com/RVCK-Project/rvck/commit/dcb12cae5a8f7719c89c3ea1404c9a68f9957c32) | 2026-08-18 | Zhiguo Zhu | firmware: zhihe: add A210 AON subsystem driver |
| [726af24e](https://github.com/RVCK-Project/rvck/commit/726af24e800f2697b1f9c4c69be3b37473c4d408) | 2026-08-18 | Zhiguo Zhu | mailbox: zhihe: add A210 mailbox controller driver |
| [87926a69](https://github.com/RVCK-Project/rvck/commit/87926a69e5491d0193fcad37e75c9a4a979d1840) | 2026-08-27 | Zhiguo Zhu | dt-bindings: regulator: zhihe: add A210 AON regulator |
| [9e91f432](https://github.com/RVCK-Project/rvck/commit/9e91f4322277e70fcf1b30f21f6cb06f5a97e1b5) | 2026-08-27 | Zhiguo Zhu | dt-bindings: mailbox: zhihe: add A210 mailbox controller |
| [5f78cf4e](https://github.com/RVCK-Project/rvck/commit/5f78cf4e058dc28186592cbefdb4fdb907c502a2) | 2026-08-27 | Zhiguo Zhu | dt-bindings: firmware: zhihe: add A210 AON subsystem |
| [3ba7ec45](https://github.com/RVCK-Project/rvck/commit/3ba7ec45b2c510b2ac4378ef93aaad26d7ccebac) | 2026-08-18 | Zhiguo Zhu | pinctrl: zhihe: add A210 pinctrl driver |
| [bd754e86](https://github.com/RVCK-Project/rvck/commit/bd754e8694086ad8ff0cc6bdeede9964d550473c) | 2026-08-18 | Zhiguo Zhu | dt-bindings: pinctrl: zhihe: add A210 pinctrl |
| [2831b2dc](https://github.com/RVCK-Project/rvck/commit/2831b2dced416e983096430d92e4b78dfeab63b3) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 Ethernet controllers |
| [2b21db32](https://github.com/RVCK-Project/rvck/commit/2b21db327cfdc3a8a131c681a69effd2c63752ed) | 2026-08-18 | Zhiguo Zhu | net: stmmac: add ZhiHe A210 DWMAC glue layer |
| [4c58d8d3](https://github.com/RVCK-Project/rvck/commit/4c58d8d37b79062539c8f5f864cb29c8452b7f77) | 2026-08-18 | Zhiguo Zhu | dt-bindings: net: zhihe: add A210 DWMAC |
| [2810fa8a](https://github.com/RVCK-Project/rvck/commit/2810fa8a289e685c0d8e4c6a1a899c3d9a278aaf) | 2026-08-20 | Zhiguo Zhu | dt-bindings: mfd: syscon: add ZhiHe A210 GMAC syscon |
| [c9ac798d](https://github.com/RVCK-Project/rvck/commit/c9ac798d4eeed1051095e9b54bf43d9a17483070) | 2026-08-13 | Zhiguo Zhu | riscv: rvck_defconfig: enable ZhiHe A210 support |
| [76cc45f6](https://github.com/RVCK-Project/rvck/commit/76cc45f66037286258b6f25daf64a814933456a4) | 2026-08-13 | Zhiguo Zhu | riscv: dts: add ZhiHe A210 device tree support |
| [b8bc0116](https://github.com/RVCK-Project/rvck/commit/b8bc011680dab9d49c1910c02eeeb6f2d5700868) | 2026-08-13 | Zhiguo Zhu | irqchip/sifive-plic: add ZhiHe A210 PLIC support |
| [8ff6ccc6](https://github.com/RVCK-Project/rvck/commit/8ff6ccc66ef3094bd33ef4a6d8dbb891b4061f3a) | 2026-08-11 | Zhiguo Zhu | mmc: sdhci-of-dwcmshc: add ZhiHe A210 support |
| [5d886e9f](https://github.com/RVCK-Project/rvck/commit/5d886e9f1420af49c93cbb1cb488402af3b0411a) | 2026-08-13 | Zhiguo Zhu | reset: zhihe: add A210 reset controller driver |
| [98fe88ed](https://github.com/RVCK-Project/rvck/commit/98fe88eda0a05470ab22a52a98e1fb8eacda9ebd) | 2026-08-13 | Zhiguo Zhu | clk: zhihe: add A210 clock controller driver |
| [7f3f9e97](https://github.com/RVCK-Project/rvck/commit/7f3f9e97973d423e383977747460a174011228f7) | 2026-08-13 | Zhiguo Zhu | riscv: add Kconfig option for ZHIHE SoC family |
| [bbb69719](https://github.com/RVCK-Project/rvck/commit/bbb69719f16342745752ef3c36218f416ce99655) | 2026-08-13 | Zhiguo Zhu | dt-bindings: interrupt-controller: sifive,plic: add ZhiHe A210 |
| [9635ead5](https://github.com/RVCK-Project/rvck/commit/9635ead5a1e9c97607b7fd22509c8870b7343462) | 2026-08-11 | Zhiguo Zhu | dt-bindings: mmc: snps,dwcmshc: add ZhiHe A210 |
| [57d73d05](https://github.com/RVCK-Project/rvck/commit/57d73d058df0ce6191bfe6e805e9ebb690216e55) | 2026-08-13 | Zhiguo Zhu | dt-bindings: reset: zhihe: add reset controller for A210 |
| [bb843112](https://github.com/RVCK-Project/rvck/commit/bb8431121532138a141d7de312673f0af4edf8af) | 2026-08-13 | Zhiguo Zhu | dt-bindings: clock: zhihe: add clock controller for A210 |
| [c47b1c3f](https://github.com/RVCK-Project/rvck/commit/c47b1c3f844af6facf7ee18a91a66109a0099e0e) | 2026-08-13 | Zhiguo Zhu | dt-bindings: riscv: zhihe: Add A210 evaluation board |
| [b4b91806](https://github.com/RVCK-Project/rvck/commit/b4b91806ffe6168067877813b274c71c922159c1) | 2026-08-13 | Zhiguo Zhu | dt-bindings: vendor-prefixes: add ZhiHe |
| [379a32e0](https://github.com/RVCK-Project/rvck/commit/379a32e0863e8a30f10e2df640aab382a67db9d5) | 2024-05-10 | Andy Chiu | riscv: vector: adjust minimum Vector requirement to ZVE32X |
| [41411817](https://github.com/RVCK-Project/rvck/commit/4141181730a2c6835f2ec3fd5cf68f795cda46d4) | 2025-09-19 | Han Gao | riscv: dts: thead: add xtheadvector to the th1520 devicetree |
| [42b082e2](https://github.com/RVCK-Project/rvck/commit/42b082e2c66d091ebd4b7a90f706df659e6d1309) | 2025-11-19 | Sergey Matyukevich | riscv: dts: allwinner: d1: fix vlenb property |
| [e64d3dea](https://github.com/RVCK-Project/rvck/commit/e64d3dea5ddcaac32e7b5a5a880ce74cdf80f354) | 2025-10-18 | Paul Walmsley | riscv: cpufeature: avoid uninitialized variable in has_thead_homogeneous_vlenb() |
| [5b2de94e](https://github.com/RVCK-Project/rvck/commit/5b2de94ed942a44cfe41f3932d9b954ae17398e6) | 2025-05-23 | Han Gao | riscv: vector: Fix context save/restore with xtheadvector |
| [addacc34](https://github.com/RVCK-Project/rvck/commit/addacc343253fbd76d54bf76a5bfb691663110d2) | 2024-11-13 | Charlie Jenkins | selftests: riscv: Support xtheadvector in vector tests |
| [bb4f93cc](https://github.com/RVCK-Project/rvck/commit/bb4f93cc55639a5691ae95f7fc9a1c1efefecd83) | 2024-11-13 | Charlie Jenkins | selftests: riscv: Fix vector tests |
| [f7c59066](https://github.com/RVCK-Project/rvck/commit/f7c59066e76191df0e58b4ab2382055de038943c) | 2024-11-13 | Charlie Jenkins | riscv: hwprobe: Document thead vendor extensions and xtheadvector extension |
| [dd2cfb98](https://github.com/RVCK-Project/rvck/commit/dd2cfb98d999dca86b2e0f011eeed557ad76794a) | 2024-11-13 | Charlie Jenkins | riscv: vector: Support xtheadvector save/restore |
| [31cce2d2](https://github.com/RVCK-Project/rvck/commit/31cce2d23aabb2571c5e4b00f6bcff7fb72ae19a) | 2024-11-13 | Charlie Jenkins | riscv: Add xtheadvector instruction definitions |
| [8dc041b6](https://github.com/RVCK-Project/rvck/commit/8dc041b6f591c2a5d7025f3d6d56985a39443e9f) | 2024-11-13 | Charlie Jenkins | riscv: csr: Add CSR encodings for CSR_VXRM/CSR_VXSAT |
| [eecaf43e](https://github.com/RVCK-Project/rvck/commit/eecaf43e33eb64075fbe0eab5a307fa8017ba5b5) | 2024-11-13 | Heiko Stuebner | RISC-V: define the elements of the VCSR vector CSR |
| [d955594c](https://github.com/RVCK-Project/rvck/commit/d955594cbbbc9e2ef856595d13a29b3c0559c33c) | 2024-11-13 | Charlie Jenkins | riscv: vector: Use vlenb from DT for thead |
| [07bab969](https://github.com/RVCK-Project/rvck/commit/07bab9692db80885f839decce0cb74e334a13b2a) | 2024-11-13 | Charlie Jenkins | riscv: Add thead and xtheadvector as a vendor extension |
| [69316daf](https://github.com/RVCK-Project/rvck/commit/69316daf8d69c5453f47f138208ba13f0a91d8d6) | 2024-11-13 | Charlie Jenkins | riscv: dts: allwinner: Add xtheadvector to the D1/D1s devicetree |
| [297a5290](https://github.com/RVCK-Project/rvck/commit/297a52905b1a1b43f8f4c13bcb552e3dc36dd1b2) | 2024-11-13 | Charlie Jenkins | dt-bindings: cpus: add a thead vlen register length property |
| [24987ad4](https://github.com/RVCK-Project/rvck/commit/24987ad41ee9873849ef3f436b69f0bb39d6f0e6) | 2024-11-13 | Charlie Jenkins | dt-bindings: riscv: Add xtheadvector ISA extension description |
| [3527bfc8](https://github.com/RVCK-Project/rvck/commit/3527bfc80025eef26e8f92b90a58a30572b028f8) | 2023-10-09 | Conor Dooley | riscv: dts: allwinner: convert isa detection to new properties |
| [8e048586](https://github.com/RVCK-Project/rvck/commit/8e0485865bd7e797859b4f4302caa1b78da03fdd) | 2026-07-29 | ZhenXing Zhu | Revert "T-Head C9xx cores implement an older version (0.7.1) of the vector speci... |
| [73eb58a5](https://github.com/RVCK-Project/rvck/commit/73eb58a5869aa7e289e3721024411fa047fdb6cf) | 2026-07-29 | ZhenXing Zhu | Revert "riscv: xtheadvector: enable vector function" |
| [602b05a4](https://github.com/RVCK-Project/rvck/commit/602b05a4625971a07fe25399c0b165f8f8a15da4) | 2026-07-29 | ZhenXing Zhu | Revert "xtheadvector: fix it used as v-ext when hwprobe is used" |
| [a8899819](https://github.com/RVCK-Project/rvck/commit/a889981979f62319ec487a1f8f2a497d42a86741) | 2026-07-29 | ZhenXing Zhu | Revert "fix: riscv: xtheadvector: fix setup_v_vsize" |
| [c8885cce](https://github.com/RVCK-Project/rvck/commit/c8885cceeaf4e99e67e10a04f5b3f6a468f04333) | 2024-01-03 | Leonardo Bras | riscv: Introduce set_compat_task() in asm/compat.h |
| [73662d67](https://github.com/RVCK-Project/rvck/commit/73662d67eba6f0552b5fa37df8763425c367b0bb) | 2024-01-03 | Leonardo Bras | riscv: Introduce is_compat_thread() into compat.h |
| [d0ed06b2](https://github.com/RVCK-Project/rvck/commit/d0ed06b2f31a4820159f58cc759a1a81170b7096) | 2024-01-03 | Leonardo Bras | riscv: add compile-time test into is_compat_task() |
| [4fd6539f](https://github.com/RVCK-Project/rvck/commit/4fd6539fce9e15844add91bcf8c7c465c8fe18ea) | 2024-01-03 | Leonardo Bras | riscv: Replace direct thread flag check with is_compat_task() |
| [6e2beb28](https://github.com/RVCK-Project/rvck/commit/6e2beb28f7345a0e79bd1113deb6e72b1c508141) | 2024-10-16 | Samuel Holland | KVM: riscv: selftests: Add Smnpm and Ssnpm to get-reg-list test |
| [9764f803](https://github.com/RVCK-Project/rvck/commit/9764f80369614cb5f0b47625ef0b384de7183fc8) | 2024-10-16 | Samuel Holland | riscv: selftests: Add a pointer masking test |
| [3e1b7946](https://github.com/RVCK-Project/rvck/commit/3e1b794601b1ee37bba2f6f5dd8aa588859ccade) | 2024-10-16 | Samuel Holland | riscv: Allow ptrace control of the tagged address ABI |
| [36ec62d3](https://github.com/RVCK-Project/rvck/commit/36ec62d3944223e86b07efd74bebe0439d505cf6) | 2024-10-16 | Samuel Holland | riscv: Add support for the tagged address ABI |
| [ad6faa09](https://github.com/RVCK-Project/rvck/commit/ad6faa097e6c3f5cfdfac7a56ec6a39db32b52ff) | 2024-10-16 | Samuel Holland | riscv: Add support for userspace pointer masking |
| [6e1a936e](https://github.com/RVCK-Project/rvck/commit/6e1a936e826b83d13348b79923ed2cdffbe99289) | 2024-10-16 | Samuel Holland | riscv: Add CSR definitions for pointer masking |
| [bf6376dc](https://github.com/RVCK-Project/rvck/commit/bf6376dcf88cceb26c8436cffd4d241d10ccfea8) | 2026-04-04 | Charlie Jenkins | selftests: riscv: Add license to cfi selftest |
| [a44ee164](https://github.com/RVCK-Project/rvck/commit/a44ee164dfe5a478402125f61b02870bad2de294) | 2026-04-04 | Paul Walmsley | prctl: cfi: change the branch landing pad prctl()s to be more descriptive |
| [b43eb2f6](https://github.com/RVCK-Project/rvck/commit/b43eb2f6b8a569a47d68ab6432394a7865073714) | 2026-04-04 | Zong Li | riscv: cfi: clear CFI lock status in start_thread() |
| [4f548c93](https://github.com/RVCK-Project/rvck/commit/4f548c9395f660c998ea8a1f4a0b9b4565750ece) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: expand "SS" references to "shadow stack" in uapi headers |
| [8ca4b5b1](https://github.com/RVCK-Project/rvck/commit/8ca4b5b1497fbbfaee60cb3499ca0c63a56ae882) | 2026-04-04 | Paul Walmsley | prctl: rename branch landing pad implementation functions to be more explicit |
| [0008f04f](https://github.com/RVCK-Project/rvck/commit/0008f04f4dca1d87ec3fa3457991e8a42abd92fb) | 2026-04-04 | Paul Walmsley | riscv: ptrace: expand "LP" references to "branch landing pads" in uapi headers |
| [f41c38b9](https://github.com/RVCK-Project/rvck/commit/f41c38b9ecd2ae1fdbcec3caf701c9fe89ecac6e) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: fix "PRACE" typo in uapi header |
| [b24ba0f3](https://github.com/RVCK-Project/rvck/commit/b24ba0f3877025918ea514f1237f0a30f1efeb35) | 2026-04-02 | Paul Walmsley | riscv: use _BITUL macro rather than BIT() in ptrace uapi and kselftests |
| [a832e88e](https://github.com/RVCK-Project/rvck/commit/a832e88e5c392e929aa9157a10c9aeecfc210e48) | 2026-01-25 | Deepak Gupta | kselftest/riscv: add kselftest for user mode CFI |
| [c62fdc19](https://github.com/RVCK-Project/rvck/commit/c62fdc1947709e75df3c126ae5d85d95f1f0e5cd) | 2026-01-25 | Deepak Gupta | riscv: add documentation for shadow stack |
| [d3d398ac](https://github.com/RVCK-Project/rvck/commit/d3d398acc080d2cbc6a70375a1cfaaad4815240d) | 2026-01-25 | Deepak Gupta | riscv: add documentation for landing pad / indirect branch tracking |
| [19e86700](https://github.com/RVCK-Project/rvck/commit/19e8670006e5f869b6a316860adc28b0db673c62) | 2026-01-25 | Deepak Gupta | riscv: create a Kconfig fragment for shadow stack and landing pad support |
| [1b328e72](https://github.com/RVCK-Project/rvck/commit/1b328e721f4d655b2b0fcbd1b5727d7dc52b5b6b) | 2026-01-25 | Deepak Gupta | arch/riscv: add dual vdso creation logic and select vdso based on hw |
| [5b08fd45](https://github.com/RVCK-Project/rvck/commit/5b08fd45eaa3a5e7696ac18df785a8a3d66b4fe8) | 2026-01-25 | Jim Shu | arch/riscv: compile vdso with landing pad and shadow stack note |
| [54d77d96](https://github.com/RVCK-Project/rvck/commit/54d77d967230a0c39a1d880d54ed48af8466d10e) | 2024-10-16 | Alexandre Ghiti | riscv: Check that vdso does not contain any dynamic relocations |
| [2807467e](https://github.com/RVCK-Project/rvck/commit/2807467ed1df8fa5fd35f8be8e390e62c519784c) | 2026-01-25 | Deepak Gupta | riscv: enable kernel access to shadow stack memory via the FWFT SBI call |
| [c2a44b95](https://github.com/RVCK-Project/rvck/commit/c2a44b95c85aebee86117cce85d671e5305bf4d7) | 2026-01-25 | Deepak Gupta | riscv: add kernel command line option to opt out of user CFI |
| [ac831700](https://github.com/RVCK-Project/rvck/commit/ac831700ecfe0a67b6e25afe712d25cf4dff439b) | 2026-01-25 | Deepak Gupta | riscv/hwprobe: add zicfilp / zicfiss enumeration in hwprobe |
| [bbb63fe3](https://github.com/RVCK-Project/rvck/commit/bbb63fe32b0e52b328db82b7e0b4ef9139c24e06) | 2026-01-25 | Paul Walmsley | riscv: hwprobe: add support for RISCV_HWPROBE_KEY_IMA_EXT_1 |
| [839d88aa](https://github.com/RVCK-Project/rvck/commit/839d88aac4df4cecbc23b15eaae27888fa7460b9) | 2026-01-25 | Deepak Gupta | riscv/ptrace: expose riscv CFI status and state via ptrace and in core files |
| [a234100c](https://github.com/RVCK-Project/rvck/commit/a234100c605081e900bca1d3832a876f12ea2cff) | 2026-01-25 | Deepak Gupta | riscv/kernel: update __show_regs() to print shadow stack register |
| [32912f04](https://github.com/RVCK-Project/rvck/commit/32912f049690617043693448c31b77a6a7088327) | 2026-01-25 | Deepak Gupta | riscv/signal: save and restore the shadow stack on a signal |
| [68c37e0c](https://github.com/RVCK-Project/rvck/commit/68c37e0c99ab90d1f2f94d608d69bfc67bee1d4c) | 2026-01-25 | Deepak Gupta | riscv/traps: Introduce software check exception and uprobe handling |
| [c375b6d8](https://github.com/RVCK-Project/rvck/commit/c375b6d8fa14c5a7c89924a2391ffcda8940281d) | 2026-01-25 | Deepak Gupta | riscv: Implement indirect branch tracking prctls |
| [2b040465](https://github.com/RVCK-Project/rvck/commit/2b040465069ee8a31cf33dcd950fa0452f91e5b4) | 2026-01-25 | Deepak Gupta | prctl: add arch-agnostic prctl()s for indirect branch tracking |
| [b6093d8a](https://github.com/RVCK-Project/rvck/commit/b6093d8a824296cf83bba5ad5be83c819578a9bc) | 2024-10-01 | Mark Brown | mman: Add map_shadow_stack() flags |
| [f53e9f59](https://github.com/RVCK-Project/rvck/commit/f53e9f59b69f011287516b94bcd13652fc9f09b9) | 2024-10-01 | Mark Brown | prctl: arch-agnostic prctl for shadow stack |
| [5bbfe50f](https://github.com/RVCK-Project/rvck/commit/5bbfe50fc8b2d9465ff93f2c5b83e749c7f1a117) | 2026-01-25 | Deepak Gupta | riscv: Implement arch-agnostic shadow stack prctls |
| [299ef157](https://github.com/RVCK-Project/rvck/commit/299ef1572e663b36b4c1b5677f881773451814d5) | 2026-01-25 | Deepak Gupta | riscv/shstk: If needed allocate a new shadow stack on clone |
| [4ecc589f](https://github.com/RVCK-Project/rvck/commit/4ecc589fad533c458bb91af51d757ca2ee308a5a) | 2026-01-25 | Deepak Gupta | riscv/mm: Implement map_shadow_stack() syscall |
| [822d7e73](https://github.com/RVCK-Project/rvck/commit/822d7e73407b48130aac63ddfd51765bad992fdd) | 2026-01-25 | Deepak Gupta | riscv/mm: update write protect to work on shadow stacks |
| [4d8215a1](https://github.com/RVCK-Project/rvck/commit/4d8215a17da8569bce8ede2ed390efa948968c89) | 2026-01-25 | Deepak Gupta | riscv/mm: teach pte_mkwrite to manufacture shadow stack PTEs |
| [cf461e59](https://github.com/RVCK-Project/rvck/commit/cf461e59148fad5e71a87616a43d82143cd8710d) | 2026-01-25 | Deepak Gupta | riscv/mm: manufacture shadow stack ptes |
| [385275f6](https://github.com/RVCK-Project/rvck/commit/385275f6ed700fe607281ecc69444098db28e2fc) | 2026-01-25 | Deepak Gupta | riscv/mm: ensure PROT_WRITE leads to VM_READ \| VM_WRITE |
| [86de9132](https://github.com/RVCK-Project/rvck/commit/86de9132512a4309256292d52980be43d36065d3) | 2026-01-25 | Deepak Gupta | riscv: Add usercfi state for task and save/restore of CSR_SSP on trap entry/exit |
| [3b4a67ce](https://github.com/RVCK-Project/rvck/commit/3b4a67ce0e0c39674dddc152ff8f427c40c74963) | 2026-01-25 | Deepak Gupta | riscv: add Zicfiss / Zicfilp extension CSR and bit definitions |
| [8fa5b531](https://github.com/RVCK-Project/rvck/commit/8fa5b531d8de52a86c1315359b37fe9d7e821cc7) | 2026-01-25 | Deepak Gupta | riscv: zicfiss / zicfilp enumeration |
| [48849cea](https://github.com/RVCK-Project/rvck/commit/48849cea8e2198f63da4d05d7ff1a872b74c7cf7) | 2026-01-25 | Deepak Gupta | dt-bindings: riscv: document zicfilp and zicfiss in extensions.yaml |
| [a721a1fc](https://github.com/RVCK-Project/rvck/commit/a721a1fcc429b7e6911b7477bdbb19122c63f86d) | 2026-01-25 | Deepak Gupta | mm: add VM_SHADOW_STACK definition for riscv |
| [f24367b2](https://github.com/RVCK-Project/rvck/commit/f24367b2719b927db53a8c322dbc349570b585ff) | 2024-04-27 | Masahiro Yamada | kbuild: use $(obj)/ instead of $(src)/ for common pattern rules |
| [5be628d8](https://github.com/RVCK-Project/rvck/commit/5be628d81ca517deb575683d1d7512f23ec26005) | 2024-03-13 | Vladimir Isaev | riscv: hwprobe: do not produce frtace relocation |
| [17334ad9](https://github.com/RVCK-Project/rvck/commit/17334ad91c0a98301c2f66cd7ae01038e117a000) | 2025-03-20 | Charlie Jenkins | riscv: entry: Split ret_from_fork() into user and kernel |
| [1bce3b9c](https://github.com/RVCK-Project/rvck/commit/1bce3b9c7afbdee79943cf8f90fa8f8e3f6aea5e) | 2025-03-20 | Charlie Jenkins | riscv: entry: Convert ret_from_fork() to C |
| [7c7438ec](https://github.com/RVCK-Project/rvck/commit/7c7438ec4bb3aadb1d2b0a90931bf8e0d139cdba) | 2025-04-11 | Xi Ruoyao | RISC-V: vDSO: Wire up getrandom() vDSO implementation |
| [3f3601d6](https://github.com/RVCK-Project/rvck/commit/3f3601d642ccac119bb3cf61a0ffff6838dda907) | 2024-08-22 | Christophe Leroy | random: vDSO: add missing c-getrandom-y in Makefile |
| [b2d8cd15](https://github.com/RVCK-Project/rvck/commit/b2d8cd15c0d64a3fc8411a0b25a6023f57bf526d) | 2025-11-12 | Andy Chiu | riscv: signal: abstract header saving for setup_sigcontext |
| [b0efd4ec](https://github.com/RVCK-Project/rvck/commit/b0efd4ec1ee9ee698f86de95391e8b2662d6bcb7) | 2023-09-14 | Sohil Mehta | arch: Reserve map_shadow_stack() syscall number for all architectures |
| [0cfa1113](https://github.com/RVCK-Project/rvck/commit/0cfa1113a425cc28bfd1db76b65594e87763f466) | 2026-06-04 | ZhenXing Zhu | soc: thead: adapt AON drivers to new th1520-aon protocol API |
| [9a3c82d3](https://github.com/RVCK-Project/rvck/commit/9a3c82d3ad7ad53f362a38643bf5ba5aa11af1e3) | 2026-06-04 | ZhenXing Zhu | drivers/soc/event: Add THEAD TH1520 event driver |
| [4dd41266](https://github.com/RVCK-Project/rvck/commit/4dd41266c702b70505f8ee3522f534023955fb64) | 2026-06-04 | ZhenXing Zhu | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [eeb52343](https://github.com/RVCK-Project/rvck/commit/eeb5234339fe25e5b43f02a74bf5215625a93495) | 2026-06-03 | ZhenXing Zhu | riscv: dts: th1520: add watchdog nodes for reboot support |
| [fdf3c69d](https://github.com/RVCK-Project/rvck/commit/fdf3c69d1bc038db23bb1d6059bafc97d582ce47) | 2026-06-03 | ZhenXing Zhu | riscv: th1520: lpi4a: fix fan not spinning |
| [d11f34d3](https://github.com/RVCK-Project/rvck/commit/d11f34d3ebedb3ea40cb8680252978105f9d675b) | 2026-06-03 | ZhenXing Zhu | riscv: defconfig: th1520: fix DWMAC config symbol for ethernet |
| [79f52bdb](https://github.com/RVCK-Project/rvck/commit/79f52bdb477ee3ec86358d6da0793640aef6c5f3) | 2024-04-19 | Robin Murphy | dma-mapping: Simplify arch_setup_dma_ops() |
| [0fa825f7](https://github.com/RVCK-Project/rvck/commit/0fa825f71de14aa8e68c2cec506231b9e752d12c) | 2024-04-19 | Robin Murphy | iommu/dma: Centralise iommu_setup_dma_ops() |
| [9ffe6e5e](https://github.com/RVCK-Project/rvck/commit/9ffe6e5e431c7864b2b5d2b2c008d9804b51a4bb) | 2024-04-19 | Robin Murphy | iommu/dma: Make limit checks self-contained |
| [520cd6d9](https://github.com/RVCK-Project/rvck/commit/520cd6d90b33fb5e4573be2441345a44dbcabff9) | 2024-04-19 | Robin Murphy | dma-mapping: Add helpers for dma_range_map bounds |
| [65c88671](https://github.com/RVCK-Project/rvck/commit/65c886713256b6d659554462ee3d06c8064b5d99) | 2024-04-19 | Robin Murphy | ACPI/IORT: Handle memory address size limits as limits |
| [2c62fd42](https://github.com/RVCK-Project/rvck/commit/2c62fd4273a311d25157b7ae75c8b749dec98489) | 2024-04-19 | Robin Murphy | OF: Simplify DMA range calculations |
| [4fa6351b](https://github.com/RVCK-Project/rvck/commit/4fa6351b9aee5ca5a3b3aca291f7720aeac415ad) | 2024-04-19 | Robin Murphy | OF: Retire dma-ranges mask workaround |
| [125c8c72](https://github.com/RVCK-Project/rvck/commit/125c8c72416bc872bbadd307ba79ab7a3d2eb510) | 2024-01-22 | Haibo Xu | KVM: selftests: Add CONFIG_64BIT definition for the build |
| [a4f18f52](https://github.com/RVCK-Project/rvck/commit/a4f18f524996c1fae99fdde333391fba4b6f09f1) | 2024-06-03 | Andrew Jones | KVM: selftests: Fix RISC-V compilation |
| [63568675](https://github.com/RVCK-Project/rvck/commit/63568675a680c99e9a7ef427d20e09a98ee7e378) | 2026-05-29 | ZhenXing Zhu | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [c9b87b21](https://github.com/RVCK-Project/rvck/commit/c9b87b21abb5196ffce358b29240b00f3cff692c) | 2026-05-29 | ZhenXing Zhu | Revert "KVM: riscv: selftests: Move sbi definitions to its own header file" |
| [2ca2bf8f](https://github.com/RVCK-Project/rvck/commit/2ca2bf8f7960077da6fc2fe85abb13ad7f903f90) | 2025-06-26 | Michal Wilczynski | riscv: dts: thead: th1520: Add GPU clkgen reset to AON node |
| [ad80ceba](https://github.com/RVCK-Project/rvck/commit/ad80cebadd42a3a343ec80e3e3444e434de7de11) | 2025-03-03 | Michal Wilczynski | reset: thead: Add TH1520 reset controller driver |
| [6b347e3d](https://github.com/RVCK-Project/rvck/commit/6b347e3daf3932cecb5bfbbade1c5c2477c73cfe) | 2025-03-03 | Michal Wilczynski | dt-bindings: reset: Add T-HEAD TH1520 SoC Reset Controller |
| [2779f758](https://github.com/RVCK-Project/rvck/commit/2779f758cf478cede9908f0341018963966aac9a) | 2026-03-12 | ZhenXing Zhu | Revert "dt-bindings: reset: Document th1520 reset control" |
| [011fab6c](https://github.com/RVCK-Project/rvck/commit/011fab6ca1ad3ef83bcc3203ac0f045c9940fb57) | 2026-03-12 | ZhenXing Zhu | Revert "reset: Add th1520 reset driver support" |
| [5ce82e9d](https://github.com/RVCK-Project/rvck/commit/5ce82e9d7fe8548d15cee3736eedae58f17b6369) | 2026-03-12 | ZhenXing Zhu | Revert "reset: th1520: to support npu/fce reset feature" |
| [42c9d438](https://github.com/RVCK-Project/rvck/commit/42c9d438305e922edb02611be030ede23d7fd273) | 2026-04-20 | ZhenXing Zhu | riscv: dts: thead: Fix aon node for OpenSBI compatibility |
| [52468098](https://github.com/RVCK-Project/rvck/commit/5246809881ed33f5d600840aad86aa1eb50a5e93) | 2026-04-17 | ZhenXing Zhu | clk: Kconfig: Restore thead clock driver Kconfig inclusion |
| [7fa7a52b](https://github.com/RVCK-Project/rvck/commit/7fa7a52bd94c99f516df689cf177a64e461401da) | 2026-03-12 | ZhenXing Zhu | Revert "riscv:dts:thead: Add TH1520 event and watchdog device node" |
| [4a9603cb](https://github.com/RVCK-Project/rvck/commit/4a9603cbefe283007b3ad326f8bcc5e732d38fce) | 2026-03-12 | ZhenXing Zhu | Revert "i2s: add i2s driver for XuanTie TH1520 SoC" |
| [9490b2ec](https://github.com/RVCK-Project/rvck/commit/9490b2ece8e535c328e5849540ac371d9fc7eca7) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: add tdm driver for XuanTie TH1520 SoC" |
| [724d8c7e](https://github.com/RVCK-Project/rvck/commit/724d8c7eecbb23b34be7047c5f6966fc3c950fca) | 2026-03-12 | ZhenXing Zhu | Revert "dts: audio: to support i2s-8ch feature" |
| [bbcd890a](https://github.com/RVCK-Project/rvck/commit/bbcd890ab774319aecfd792eefe4a35f918dbc5a) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: to support tdm/spdif feature" |
| [fe0ffd84](https://github.com/RVCK-Project/rvck/commit/fe0ffd842e9f754586f232d9fd553ceb8c847e68) | 2025-02-19 | Michal Wilczynski | riscv: dts: thead: Introduce power domain nodes with aon firmware |
| [f00c5970](https://github.com/RVCK-Project/rvck/commit/f00c59701fe0b1d80808eded5eae7ca19879d83e) | 2024-02-08 | Krzysztof Kozlowski | pmdomain: core: constify of_phandle_args in xlate |
| [b894cee1](https://github.com/RVCK-Project/rvck/commit/b894cee143cb3e07529163f21b4fb462949f2cf0) | 2025-03-11 | Michal Wilczynski | dt-bindings: power: Add TH1520 SoC power domains |
| [e1b0c756](https://github.com/RVCK-Project/rvck/commit/e1b0c7569244bbada676152de62a303c821cc0d8) | 2023-09-11 | Ulf Hansson | pmdomain: Prepare to move Kconfig files into the pmdomain subsystem |
| [5a71072e](https://github.com/RVCK-Project/rvck/commit/5a71072ecef367555ba840cbcb0fcd7832c68942) | 2025-03-14 | Arnd Bergmann | pmdomain: thead: fix TH1520_AON_PROTOCOL dependency |
| [636fe297](https://github.com/RVCK-Project/rvck/commit/636fe2975dcba2c4f4dfd5ae6ebb42621a8d229a) | 2025-03-11 | Michal Wilczynski | pmdomain: thead: Add power-domain driver for TH1520 |
| [e40eed8b](https://github.com/RVCK-Project/rvck/commit/e40eed8ba5c9dc27fbfa75cd31d0dea112c3e455) | 2025-03-11 | Michal Wilczynski | firmware: thead: Add AON firmware protocol driver |
| [d8776ea6](https://github.com/RVCK-Project/rvck/commit/d8776ea6c01850c970c658583849d03c1ce3a4e2) | 2026-03-11 | ZhenXing Zhu | Revert "regdump:add regdump support for lpi4a and light-a && rename some dts nam... |
| [5298d117](https://github.com/RVCK-Project/rvck/commit/5298d11702a105644fa89ba6fd037b9f39058645) | 2026-03-11 | ZhenXing Zhu | Revert "drivers/soc/event: Add THEAD TH1520 event driver" |
| [c1f17a98](https://github.com/RVCK-Project/rvck/commit/c1f17a9816c6b2d55547ee0feff309eb3d07c73e) | 2026-03-11 | ZhenXing Zhu | Revert "add c906 audio support" |
| [9f19ba4f](https://github.com/RVCK-Project/rvck/commit/9f19ba4f1105fe5aac6c996b6ec831f9f9faed4d) | 2026-03-11 | ZhenXing Zhu | Revert "drivers: regulator: add th1520 AON virtual regulator control support." |
| [fad7b6ed](https://github.com/RVCK-Project/rvck/commit/fad7b6edaec7ba93992b62dca4f7e0d1ace9d137) | 2026-03-10 | ZhenXing Zhu | Revert "drivers/watchdog: Add THEAD TH1520 pmic watchdog driver" |
| [8254e00f](https://github.com/RVCK-Project/rvck/commit/8254e00fc9f83c639b3d18067625b21d1053b23e) | 2026-03-10 | ZhenXing Zhu | Revert "firmware: thead: c910_aon: add th1520 Aon protocol driver" |
| [663c0d70](https://github.com/RVCK-Project/rvck/commit/663c0d70a3e4be96cfdeb37595ab5b59ff1a25ae) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: cpufreq: add cpufreq driver." |
| [13ba43db](https://github.com/RVCK-Project/rvck/commit/13ba43db2717dd74435c7af5c7f2bf34176416a1) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to modify rvbook dts" |
| [96960edd](https://github.com/RVCK-Project/rvck/commit/96960edd2b6dba11fcdd830f6ab0de134f3f1e62) | 2026-03-10 | ZhenXing Zhu | Revert "audio: th1520: add soundcard dts node of th1520-a-val board" |
| [3c016017](https://github.com/RVCK-Project/rvck/commit/3c016017f974b736652b68e5117b7210f01a2984) | 2026-03-10 | ZhenXing Zhu | Revert "dts:th1520-a: add th1520-a-val.dts and th1520-a-val-sec.dts" |
| [120d3e33](https://github.com/RVCK-Project/rvck/commit/120d3e338bf0c3a491e74633964f18913f0be13b) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: pmdomain: support th1520 Power domain control." |
| [98e08ac6](https://github.com/RVCK-Project/rvck/commit/98e08ac6dcce435ee16c163226c19cccb0975ab8) | 2026-03-10 | ZhenXing Zhu | Revert "dts: add GPU device node" |
| [b4e7a59b](https://github.com/RVCK-Project/rvck/commit/b4e7a59bfa6ba324bbb60d4f469aff2fcba12908) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [90c6a1bf](https://github.com/RVCK-Project/rvck/commit/90c6a1bf8bff24fcdbbe4971f65f198e47e3421c) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add npu device node" |
| [570db731](https://github.com/RVCK-Project/rvck/commit/570db7318bac7c21783ead718ec23794da124da6) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to add npu device node" |
| [c5b9f5bb](https://github.com/RVCK-Project/rvck/commit/c5b9f5bb309a46bd6cc7a1397273d73e3d675a7b) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add th1520-a-val-crash.dts and th1520-lpi4a-product-crash.d... |
| [ee3d8e62](https://github.com/RVCK-Project/rvck/commit/ee3d8e62196aea77255810938dca0e8f61848068) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [fe1520eb](https://github.com/RVCK-Project/rvck/commit/fe1520eb5af9f85d345933d2c732a90f056c69fa) | 2026-03-10 | ZhenXing Zhu | Revert "dtb:lipi:enable VI module config" |
| [9a57af18](https://github.com/RVCK-Project/rvck/commit/9a57af1804bf58cb53ff187545980c239a114e47) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add vdec venc and video mem device node" |
| [6a3c9a6f](https://github.com/RVCK-Project/rvck/commit/6a3c9a6f586185953e4867f4fbc37045002ca286) | 2026-03-10 | ZhenXing Zhu | Revert "chore: use xuantie instead of thead" |
| [cfd5ec92](https://github.com/RVCK-Project/rvck/commit/cfd5ec92eac8aa16b4e2ad4db2a146ce1b90d12a) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce NAPOT field to PTDUMP |
| [1fd210c4](https://github.com/RVCK-Project/rvck/commit/1fd210c4e8ebb7e1570cfc33057291f7bd27d1f9) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce PBMT field to PTDUMP |
| [a69933ce](https://github.com/RVCK-Project/rvck/commit/a69933ce825217de40783212aad9c91e465daa2f) | 2023-09-21 | Yu Chien Peter Lin | riscv: Improve PTDUMP to show RSW with non-zero value |
| [ad1d93ff](https://github.com/RVCK-Project/rvck/commit/ad1d93ff68a4995661b7b80573886dd635a41a0e) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add commandline option for SBI PMU test |
| [70da6bc0](https://github.com/RVCK-Project/rvck/commit/70da6bc048a68ed276e962640e8622250033e3a5) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for counter overflow |
| [b8f98dea](https://github.com/RVCK-Project/rvck/commit/b8f98deae533b0765da2fcc196b1b8b2558dd4dd) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for PMU snapshot functionality |
| [55232ea3](https://github.com/RVCK-Project/rvck/commit/55232ea315faef24af8ef124982af471d103c441) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU selftest |
| [35b99727](https://github.com/RVCK-Project/rvck/commit/35b99727756696599780b4a9b2502617e17f4bb7) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU extension definitions |
| [f0e268ca](https://github.com/RVCK-Project/rvck/commit/f0e268cafb40ca0a6f48a6da2f968f65df90ffe9) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add Sscofpmf to get-reg-list test |
| [5ce5c7cc](https://github.com/RVCK-Project/rvck/commit/5ce5c7cce6b39ace4a4e167784a55ac2bb032fbd) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add helper functions for extension checks |
| [891788a1](https://github.com/RVCK-Project/rvck/commit/891788a1f31a74656b7404546e208dfb58323bfe) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [588b2fda](https://github.com/RVCK-Project/rvck/commit/588b2fdaf5d82aec553d3b682a0fd4462b6f128a) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add sstc timer test |
| [2068a4e8](https://github.com/RVCK-Project/rvck/commit/2068a4e8b027a26d08399bf3fb4a18f5b2ce0065) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Change vcpu_has_ext to a common function |
| [377bd016](https://github.com/RVCK-Project/rvck/commit/377bd0167b77a81819cc50bc34a11a26f8137909) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add guest helper to get vcpu id |
| [a43adb5b](https://github.com/RVCK-Project/rvck/commit/a43adb5bbd70cffdc829913c9586270f33f1860d) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add exception handling support |
| [40bef847](https://github.com/RVCK-Project/rvck/commit/40bef847f81295b5487ea4009e7aa36f32abace2) | 2024-01-22 | Haibo Xu | KVM: arm64: selftests: Split arch_timer test code |
| [c9a60f15](https://github.com/RVCK-Project/rvck/commit/c9a60f15fc558526205cb962acf3d69c41d08706) | 2024-01-22 | Paolo Bonzini | selftests/kvm: Fix issues with $(SPLIT_TESTS) |
| [db448c6c](https://github.com/RVCK-Project/rvck/commit/db448c6c72d9b5aefed4eebdf804247eecc157ba) | 2024-04-20 | Atish Patra | RISC-V: KVM: Improve firmware counter read function |
| [55b9f75a](https://github.com/RVCK-Project/rvck/commit/55b9f75a773e3ecaff5216935b52af23a244cf68) | 2024-04-20 | Atish Patra | RISC-V: KVM: Support 64 bit firmware counters on RV32 |
| [49d59957](https://github.com/RVCK-Project/rvck/commit/49d599575b95455d5012f2cd28c6fb5281083df7) | 2024-04-20 | Atish Patra | RISC-V: KVM: Add perf sampling support for guests |
| [563eed61](https://github.com/RVCK-Project/rvck/commit/563eed610633e238ff6b319fd109dd3b56fb61ef) | 2024-04-20 | Atish Patra | RISC-V: KVM: Implement SBI PMU Snapshot feature |
| [b38537a8](https://github.com/RVCK-Project/rvck/commit/b38537a8df7f5e563b72f0ab8ef65426e566051c) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to exit to the user space if perf event failed |
| [8ae0fa22](https://github.com/RVCK-Project/rvck/commit/8ae0fa2235b94e69a55746337e18ea863f27bb63) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to update the counter value during reset |
| [3f065af9](https://github.com/RVCK-Project/rvck/commit/3f065af96ed51bcc0770cc400d960936558e8ec9) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Implement SBI PMU snapshot function |
| [0c75e50f](https://github.com/RVCK-Project/rvck/commit/0c75e50f2251343edb2f5ceb517236b955d113a1) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Fix counter mask iteration for RV32 |
| [ee6faed1](https://github.com/RVCK-Project/rvck/commit/ee6faed10ff1cf35f1efdbac487291246f2ddc1a) | 2024-04-20 | Atish Patra | RISC-V: Use the minor version mask while computing sbi version |
| [eb7e8bbc](https://github.com/RVCK-Project/rvck/commit/eb7e8bbc72053b3c428dbd2385a48ce5d9a2e30e) | 2024-04-20 | Atish Patra | RISC-V: KVM: Rename the SBI_STA_SHMEM_DISABLE to a generic name |
| [1dea6c99](https://github.com/RVCK-Project/rvck/commit/1dea6c995dbc86968c51bba6f8085eb6940cb1bd) | 2024-04-20 | Atish Patra | RISC-V: Add SBI PMU snapshot definitions |
| [ebd5a8ae](https://github.com/RVCK-Project/rvck/commit/ebd5a8aeccfad39e1d35511bd5bc0e5828cf1b56) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Use BIT macro for shifting operations |
| [b150e995](https://github.com/RVCK-Project/rvck/commit/b150e995835ccd0007c361da6f19d8f23908619e) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Read upper bits of a firmware counter |
| [d841b53e](https://github.com/RVCK-Project/rvck/commit/d841b53e64e61dee9053145239d5914199771fa9) | 2024-04-20 | Atish Patra | RISC-V: Add FIRMWARE_READ_HI definition |
| [e4d273c8](https://github.com/RVCK-Project/rvck/commit/e4d273c802df21a62c969ab7f67c42db758eec2e) | 2024-04-20 | Atish Patra | RISC-V: Fix the typo in Scountovf CSR name |
| [a29ea96c](https://github.com/RVCK-Project/rvck/commit/a29ea96ce45de5b5e35c46c6d4bbf1eb9aa896c3) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add get-reg-list test for STA registers |
| [7c145869](https://github.com/RVCK-Project/rvck/commit/7c1458695dc3a83f963ceaeff679827e84fd6207) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add steal_time test support |
| [cae81950](https://github.com/RVCK-Project/rvck/commit/cae81950a2b80b148e489f5eb6c4e56164a3a2e5) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add guest_sbi_probe_extension |
| [0ba9e45b](https://github.com/RVCK-Project/rvck/commit/0ba9e45b7d79d0a71874387c17435cdeaf62f597) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Move sbi_ecall to processor.c |
| [c6dc39ec](https://github.com/RVCK-Project/rvck/commit/c6dc39ecdcbfaa26d9bbd1b79b9b4ce08e0967e7) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Implement SBI STA extension |
| [907e8886](https://github.com/RVCK-Project/rvck/commit/907e88862cc35d090d3e936f6cd22d38b17510bd) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI STA registers |
| [e0bbf4c5](https://github.com/RVCK-Project/rvck/commit/e0bbf4c5a7d1af9509a6b2aff3fb7027a1d6ce82) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI extension registers |
| [c9dc3bd2](https://github.com/RVCK-Project/rvck/commit/c9dc3bd2581a8ea42b91fb9bf6079a3d0f17d261) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA info to vcpu_arch |
| [6a046d90](https://github.com/RVCK-Project/rvck/commit/6a046d90c73a7f79b62a8132ad15c2144e55d307) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add steal-update vcpu request |
| [94d9492b](https://github.com/RVCK-Project/rvck/commit/94d9492bb6993a4982908f0e4804ffb16492eb3d) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA extension skeleton |
| [c4c858ba](https://github.com/RVCK-Project/rvck/commit/c4c858ba93dd0db85960f020009e22d02c210d26) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Implement steal-time support |
| [79a54f4a](https://github.com/RVCK-Project/rvck/commit/79a54f4ac81eec9020c5f584260b0f89ce385af9) | 2023-12-20 | Andrew Jones | RISC-V: Add SBI STA extension definitions |
| [374c8bf3](https://github.com/RVCK-Project/rvck/commit/374c8bf32b4da06f53c8862631f87186d48709d7) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Add skeleton for pv-time support |
| [fd45228f](https://github.com/RVCK-Project/rvck/commit/fd45228fb0a365ada6642671435d13bfb0bd4afd) | 2023-12-13 | Andrew Jones | KVM: riscv: selftests: Add RISCV_SBI_EXT_REG |
| [be470cfb](https://github.com/RVCK-Project/rvck/commit/be470cfba6caa4fb5e549f0ec273b1b56b92b3f8) | 2023-12-13 | Andrew Jones | RISC-V: KVM: Make SBI uapi consistent with ISA uapi |
| [88f01730](https://github.com/RVCK-Project/rvck/commit/88f01730fef08510bfdb717a1706fb1e96245cb0) | 2023-11-28 | Anup Patel | KVM: riscv: selftests: Generate ISA extension reg_list using macros |
| [809f65d8](https://github.com/RVCK-Project/rvck/commit/809f65d8da4738864ee7dfe44165fc9bdc880c07) | 2023-10-19 | Thomas Huth | KVM: selftests: Use TAP in the steal_time test |
| [e499d838](https://github.com/RVCK-Project/rvck/commit/e499d838d20f8e28e494feedc5e58a53e365defb) | 2023-09-20 | Andrew Jones | KVM: riscv: selftests: get-reg-list print_reg should never fail |
| [3e8d0e67](https://github.com/RVCK-Project/rvck/commit/3e8d0e679fd928676ab7bb7802f72a6d93ffeef4) | 2023-08-17 | Andrew Jones | KVM: selftests: Add array order helpers to riscv get-reg-list |
| [95be4472](https://github.com/RVCK-Project/rvck/commit/95be44726e6db4e316f71693d2eae42711c34ee0) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Switch to use macro from csr.h |
| [8e663349](https://github.com/RVCK-Project/rvck/commit/8e663349ac6e067c8021783dec525fb72d637d89) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file vdso/processor.h |
| [0c27522b](https://github.com/RVCK-Project/rvck/commit/0c27522b306dbfa1c0e1fc3eac543661b65852d4) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file csr.h |
| [f4e9f973](https://github.com/RVCK-Project/rvck/commit/f4e9f97308c7c4a03aae610c3bb15f4ef289ad5b) | 2024-11-08 | Charlie Jenkins | riscv: Fix default misaligned access trap |
| [f1b928a4](https://github.com/RVCK-Project/rvck/commit/f1b928a406ff48aa3d459b1ff7cf1e81db1b5e26) | 2025-12-06 | Eric Biggers | lib/crypto: riscv: Depend on RISCV_EFFICIENT_VECTOR_UNALIGNED_ACCESS |
| [fb66e8d1](https://github.com/RVCK-Project/rvck/commit/fb66e8d15e7da93fb38f2761d299a80054dd7c9f) | 2025-03-04 | Andrew Jones | Documentation/kernel-parameters: Add riscv unaligned speed parameters |
| [a7982e94](https://github.com/RVCK-Project/rvck/commit/a7982e9447a0f419da19d1a6b12e329a01cbfa98) | 2025-03-04 | Andrew Jones | riscv: Add parameter for skipping access speed tests |
| [1344dfe9](https://github.com/RVCK-Project/rvck/commit/1344dfe9185fc052c5a595804fff923e7d0c7eff) | 2025-03-04 | Andrew Jones | riscv: Fix set up of vector cpu hotplug callback |
| [58957838](https://github.com/RVCK-Project/rvck/commit/589578386cbda92a7d373e912faa050c064b4c4b) | 2025-03-04 | Andrew Jones | riscv: Fix set up of cpu hotplug callbacks |
| [751be7c0](https://github.com/RVCK-Project/rvck/commit/751be7c0b58873667507612f17d0594134dc5b64) | 2025-03-04 | Andrew Jones | riscv: Change check_unaligned_access_speed_all_cpus to void |
| [aff7b549](https://github.com/RVCK-Project/rvck/commit/aff7b549354777265d7c5c91af71e589c004fdd0) | 2025-03-04 | Andrew Jones | riscv: Fix check_unaligned_access_all_cpus |
| [9844c618](https://github.com/RVCK-Project/rvck/commit/9844c618187e29e3c593ac6398a8a28ef8c041cf) | 2025-03-04 | Andrew Jones | riscv: Fix riscv_online_cpu_vec |
| [edc59fce](https://github.com/RVCK-Project/rvck/commit/edc59fcec4a614bcc1e895a1d62d8436cc165879) | 2025-03-04 | Andrew Jones | riscv: Annotate unaligned access init functions |
| [a13e98b0](https://github.com/RVCK-Project/rvck/commit/a13e98b090ad0b803222aef9959f43268c105d4a) | 2024-10-17 | Jesse Taube | RISC-V: hwprobe: Document unaligned vector perf key |
| [7f2c8f07](https://github.com/RVCK-Project/rvck/commit/7f2c8f07395732f58ef02488ab0f71cc17e07e09) | 2024-10-17 | Jesse Taube | RISC-V: Report vector unaligned access speed hwprobe |
| [096dc32f](https://github.com/RVCK-Project/rvck/commit/096dc32fa7c2b2865229c744128883fc4400413f) | 2024-10-17 | Jesse Taube | RISC-V: Detect unaligned vector accesses supported |
| [46bac190](https://github.com/RVCK-Project/rvck/commit/46bac1904dfd5636e90328f67129ca529e936ad3) | 2024-10-17 | Jesse Taube | RISC-V: Replace RISCV_MISALIGNED with RISCV_SCALAR_MISALIGNED |
| [c4f0c400](https://github.com/RVCK-Project/rvck/commit/c4f0c400d42c6a867b694beed76af5a76b2e59de) | 2024-10-17 | Jesse Taube | RISC-V: Scalar unaligned access emulated on hotplug CPUs |
| [5153997f](https://github.com/RVCK-Project/rvck/commit/5153997f1d0cd052ab75671d820449ef13ec7adc) | 2024-10-17 | Jesse Taube | RISC-V: Check scalar unaligned access on all CPUs |
| [06c27bb5](https://github.com/RVCK-Project/rvck/commit/06c27bb5f6d3950a39ba3725c8c7982c6fc1b800) | 2024-08-14 | Samuel Holland | riscv: misaligned: Restrict user access to kernel memory |
| [2de90f00](https://github.com/RVCK-Project/rvck/commit/2de90f007b82e517d72ab2efd43cc1854109cce4) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add SCALAR to misaligned perf defines |
| [276d67ea](https://github.com/RVCK-Project/rvck/commit/276d67eae4b88c4729e09bcddc2a8886424b47a9) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add MISALIGNED_PERF key |
| [f19ab1a2](https://github.com/RVCK-Project/rvck/commit/f19ab1a2d2d431fa2903453abd3a283c0a56bfd1) | 2024-03-17 | Xingyou Chen | riscv: typo in comment for get_f64_reg |
| [cb3f9f49](https://github.com/RVCK-Project/rvck/commit/cb3f9f498108ea720071c54359a56576c43cd2d1) | 2024-03-08 | Charlie Jenkins | riscv: Set unaligned access speed at compile time |
| [c3b14694](https://github.com/RVCK-Project/rvck/commit/c3b1469464b77dbdb376901b96b222eb47b4f5a3) | 2024-03-08 | Charlie Jenkins | riscv: Decouple emulated unaligned accesses from access speed |
| [801b4f8b](https://github.com/RVCK-Project/rvck/commit/801b4f8b62dbe676f0ad5b83e885754b32da3d5a) | 2024-03-08 | Charlie Jenkins | riscv: Only check online cpus for emulated accesses |
| [35a04067](https://github.com/RVCK-Project/rvck/commit/35a040679bd948a19252d24e9c74232a4bad4d5e) | 2024-03-08 | Charlie Jenkins | riscv: lib: Introduce has_fast_unaligned_access() |
| [4814bdcb](https://github.com/RVCK-Project/rvck/commit/4814bdcb9ab5097e30f5eeb8c1e963eba85454f7) | 2024-02-12 | Eric Biggers | crypto: riscv - add vector crypto accelerated AES-CBC-CTS |
| [f7f7388e](https://github.com/RVCK-Project/rvck/commit/f7f7388e1e698a4d1a2a2f5018df8ebff461b66b) | 2024-02-06 | Clément Léger | riscv: misaligned: remove CONFIG_RISCV_M_MODE specific code |
| [298ee403](https://github.com/RVCK-Project/rvck/commit/298ee403f3c062496d81759432625abe586a32a4) | 2024-01-08 | Charlie Jenkins | kunit: Add tests for csum_ipv6_magic and ip_fast_csum |
| [13a04389](https://github.com/RVCK-Project/rvck/commit/13a0438963b4a835e821a071ff19482f899c6673) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum library |
| [563d0db0](https://github.com/RVCK-Project/rvck/commit/563d0db083bcba4e0c81c8241b1bbea742e78519) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum header |
| [1d3e9e16](https://github.com/RVCK-Project/rvck/commit/1d3e9e16c6c0965580ef1890b020b46d82efe234) | 2024-01-08 | Charlie Jenkins | riscv: Add static key for misaligned accesses |
| [9a7c8669](https://github.com/RVCK-Project/rvck/commit/9a7c86690e28b63c4d1c7a39da951016642b2682) | 2024-01-08 | Charlie Jenkins | asm-generic: Improve csum_fold |
| [0f15f7c5](https://github.com/RVCK-Project/rvck/commit/0f15f7c565e9d240d639b38c907a9d08793babd0) | 2023-12-25 | Jisheng Zhang | riscv: select DCACHE_WORD_ACCESS for efficient unaligned access HW |
| [de3fecfe](https://github.com/RVCK-Project/rvck/commit/de3fecfe745bfe12148646386f7926802bd3733b) | 2023-12-25 | Jisheng Zhang | riscv: introduce RISCV_EFFICIENT_UNALIGNED_ACCESS |
| [cfe4a8ea](https://github.com/RVCK-Project/rvck/commit/cfe4a8eae520d7f8f10a6d98c505a12e6594ad50) | 2023-11-23 | Ben Dooks | riscv; fix __user annotation in save_v_state() |
| [680200b3](https://github.com/RVCK-Project/rvck/commit/680200b3c9a11e6d198d2adf5746a78109cd0c2b) | 2023-11-23 | Ben Dooks | riscv: fix __user annotation in traps_misaligned.c |
| [66be17de](https://github.com/RVCK-Project/rvck/commit/66be17dec2f35300a870fe6e84af5e92aab30fa0) | 2023-11-06 | Evan Green | RISC-V: Show accurate per-hart isa in /proc/cpuinfo |
| [2250e509](https://github.com/RVCK-Project/rvck/commit/2250e509b913702dcdf0cf8fa2f1db901c94867d) | 2026-02-02 | Chen Pei | Revert "riscv:uprobe: fix flush_icache to ensure that instructions are refreshed... |
| [6fd8b07d](https://github.com/RVCK-Project/rvck/commit/6fd8b07dd451ce5cbeecc46a07ed0d30b19c9990) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM4 |
| [e71fefdc](https://github.com/RVCK-Project/rvck/commit/e71fefdc49e0efa59486e203dc9aba37a5d47c54) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM3 |
| [3accb695](https://github.com/RVCK-Project/rvck/commit/3accb69568e23eed4e95d483aa340214af3561fc) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{512,384} |
| [35179420](https://github.com/RVCK-Project/rvck/commit/351794202443e14fb062acf6ffa842bd979958a2) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{256,224} |
| [829267e9](https://github.com/RVCK-Project/rvck/commit/829267e9d825f943442988462782cd2967d709da) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated GHASH |
| [754a7e98](https://github.com/RVCK-Project/rvck/commit/754a7e9899ef7c2e19a7f941edc43830fcaf6b16) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated ChaCha20 |
| [0c7f75e6](https://github.com/RVCK-Project/rvck/commit/0c7f75e67e6bb786360bfa52c1994360b52667f3) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated AES-{ECB,CBC,CTR,XTS} |
| [2099f301](https://github.com/RVCK-Project/rvck/commit/2099f301eb96219ac4d96345e7392297fde4d8c7) | 2024-01-21 | Heiko Stuebner | RISC-V: hook new crypto subdir into build-system |
| [8b319967](https://github.com/RVCK-Project/rvck/commit/8b31996713985a5d4ce3be9f8b089e43a02b3e09) | 2024-01-21 | Eric Biggers | RISC-V: add TOOLCHAIN_HAS_VECTOR_CRYPTO |
| [719807d0](https://github.com/RVCK-Project/rvck/commit/719807d0033f9d5201385ccd92ca0521494fdaaa) | 2024-01-21 | Heiko Stuebner | RISC-V: add helper function to read the vector VLEN |
| [56a712a3](https://github.com/RVCK-Project/rvck/commit/56a712a3ac83fbc446ce932798161073c263e6e8) | 2023-10-12 | Ian Rogers | perf pmu: Lazily compute default config |
| [ccf4a439](https://github.com/RVCK-Project/rvck/commit/ccf4a439af8e79587f423407bab157707cb0a2d8) | 2023-10-12 | Ian Rogers | perf pmu-events: Remember the perf_events_map for a PMU |
| [1e3bf280](https://github.com/RVCK-Project/rvck/commit/1e3bf280cf501c780c09b64ddb22a158216006ee) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify perf_pmu__config_terms |
| [23610c8b](https://github.com/RVCK-Project/rvck/commit/23610c8bb514084a98ec8ab3f148c93c39928d2e) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify file APIs |
| [64dd924a](https://github.com/RVCK-Project/rvck/commit/64dd924a8929493d9e15067e8d2be077bd400657) | 2023-10-12 | Ian Rogers | perf arm-spe: Move PMU initialization from default config code |
| [737a87e4](https://github.com/RVCK-Project/rvck/commit/737a87e40f8c88e7ffab4bcf8cdd7a73ff969917) | 2023-10-12 | Ian Rogers | perf intel-pt: Move PMU initialization from default config code |
| [931f14b0](https://github.com/RVCK-Project/rvck/commit/931f14b00b3db89a2e4c5c9fa92080ebfcafc773) | 2023-10-12 | Ian Rogers | perf pmu: Rename perf_pmu__get_default_config to perf_pmu__arch_init |
| [5431483e](https://github.com/RVCK-Project/rvck/commit/5431483ec836f556d6bfb882d26bf3f1466dfa33) | 2023-09-24 | Ian Rogers | perf pmus: Make PMU alias name loading lazy |
| [17bb7cf6](https://github.com/RVCK-Project/rvck/commit/17bb7cf6cbd868cc331ca153589b49246b94baaf) | 2023-09-01 | Ian Rogers | perf parse-events: Introduce 'struct parse_events_terms' |
| [b1310cba](https://github.com/RVCK-Project/rvck/commit/b1310cba2ed2cccbf3fd066e3fb0089b1156090b) | 2023-09-01 | Ian Rogers | perf parse-events: Copy fewer term lists |
| [b96284f9](https://github.com/RVCK-Project/rvck/commit/b96284f93208fb995f5797d479421ac3a487699f) | 2023-09-01 | Ian Rogers | perf parse-events: Avoid enum casts |
| [35727e33](https://github.com/RVCK-Project/rvck/commit/35727e33a9abad8bacc63ab9a0dcf9d667402e87) | 2023-09-01 | Ian Rogers | perf parse-events: Tidy up str parameter |
| [ce655dd5](https://github.com/RVCK-Project/rvck/commit/ce655dd5935bf0b306167b375f39d510bb11aca9) | 2023-09-01 | Ian Rogers | perf parse-events: Remove unnecessary __maybe_unused |
| [13d1ae81](https://github.com/RVCK-Project/rvck/commit/13d1ae81316f1863a01627ab2a0d8c1cb345cfcc) | 2024-04-22 | Shenlin Liang | perf kvm/riscv: Port perf kvm stat to RISC-V |
| [23dbcebc](https://github.com/RVCK-Project/rvck/commit/23dbcebcd237bb92ffde1075ac35a5e368ef9f69) | 2024-04-22 | Shenlin Liang | RISCV: KVM: add tracepoints for entry and exit events |
| [efa6cbcb](https://github.com/RVCK-Project/rvck/commit/efa6cbcb6b3145879f35fac47cc79e027f05d956) | 2024-06-21 | Huaming | defconfig:th1520: enable cma config |
| [70400a2e](https://github.com/RVCK-Project/rvck/commit/70400a2e56406dc3c072ba8bea58d80af84df23c) | 2024-09-06 | Guo Ren | riscv: mm: Add support for Svinval extension |
| [6ddd0e42](https://github.com/RVCK-Project/rvck/commit/6ddd0e4281ce3b2ea89cb5b1b41a2b7f90837467) | 2024-09-04 | Guo Ren | riscv: Add ACLINT SSWI support |
| [12318968](https://github.com/RVCK-Project/rvck/commit/123189681073462b4971bef1ffb7e4cc44bad8b8) | 2024-07-29 | forain | drm: Fix HDMI hot-plug problem |
| [026a8f84](https://github.com/RVCK-Project/rvck/commit/026a8f8495c0a8a89e97ef20dad2c69e6b59c21f) | 2024-07-25 | Hao Li | gpu/drm: hdmi: Add hdmi debounce to enhance hdmi plugin/out stable |
| [6d7aa7ad](https://github.com/RVCK-Project/rvck/commit/6d7aa7ad1f8caf65498d39e596a7fbceba12bfe2) | 2024-07-21 | David Li | audio: th1520: fixup compile warning of i2s driver |
| [5545f027](https://github.com/RVCK-Project/rvck/commit/5545f02723ccca7285e63575f8952fa29cf2c94d) | 2024-07-12 | David Li | dmaengine: dw-axi-dmac: Add support for Xuantie TH1520 DMA |
| [cd24cf6f](https://github.com/RVCK-Project/rvck/commit/cd24cf6f35ec6920412fb6a48ff9fa34aab9eb94) | 2024-07-04 | Chen Pei | arch:rsicv:select ARCH_HAS_DMA_WRITE_COMBINE |
| [2d9437a2](https://github.com/RVCK-Project/rvck/commit/2d9437a25023f7ebc4ba12ea7ac64549ba604720) | 2024-07-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [e8e09a24](https://github.com/RVCK-Project/rvck/commit/e8e09a241152149bff57624ccbc46b9962fd4fd4) | 2024-06-30 | Huaming | driver:padctrl:correct th1520 gpio_1 24/25 cfg |
| [322a8e82](https://github.com/RVCK-Project/rvck/commit/322a8e829103f2a7ed2b89a0c068148888423b0e) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [9bef5562](https://github.com/RVCK-Project/rvck/commit/9bef5562c03a06918c85951bb494bef7b71ecd1a) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [e4f20517](https://github.com/RVCK-Project/rvck/commit/e4f205170adf38fdd36451ac0c72d4029f23ae49) | 2024-07-04 | Xiangyi Zeng | drivers: event: add macro definition to control SW_PANIC event |
| [76f8f541](https://github.com/RVCK-Project/rvck/commit/76f8f541e26f7ea0759f01a45534d3b3ac202576) | 2024-06-28 | David Li | audio: th1520: enable soundcard feature |
| [3d3d6ed8](https://github.com/RVCK-Project/rvck/commit/3d3d6ed82b8fc14a3761211cc0f9b886ebf5fe1b) | 2024-06-27 | David Li | audio: th1520: support audiosys pinctrl feature |
| [cb00f962](https://github.com/RVCK-Project/rvck/commit/cb00f962c6f87b2e7122f1188ba5cb81172fe43a) | 2024-06-26 | Xiangyi Zeng | dts: th1520: fix interrupt number config error in dts |
| [3b996522](https://github.com/RVCK-Project/rvck/commit/3b996522288bab104323397ad9625de08b6090a9) | 2024-06-24 | forain | DPU: add DPU driver for Lichee-Pi-4A board |
| [173ea821](https://github.com/RVCK-Project/rvck/commit/173ea8210e23260b283e2bc596708ff66f3a027b) | 2024-06-23 | tingming | dts: th1520: add npu device node |
| [6efaaf64](https://github.com/RVCK-Project/rvck/commit/6efaaf6400545197dd19f2d271765d0bdc2af1f2) | 2024-06-21 | David Li | codec: audio: add codec driver for Lichee-Pi-4A board |
| [eee5bd7f](https://github.com/RVCK-Project/rvck/commit/eee5bd7f29e4f7b46af5601505cf96df9af651f2) | 2024-06-20 | Chen Pei | riscv: vector: Fix the boot issue compiled using xuantie-toolchain or upstream-t... |
| [3531cdf3](https://github.com/RVCK-Project/rvck/commit/3531cdf30ee4ae7ae5f0be07f956cd634693b402) | 2024-06-19 | Esther Z | drivers: cpufreq: add cpufreq driver. |
| [a6c9526a](https://github.com/RVCK-Project/rvck/commit/a6c9526a1b9cbf217bd1f2f20d2dcd07ca61cb2d) | 2024-06-18 | Esther Z | riscv: dts: Introduce lichee-pi-4a fixed regulator support. |
| [2e542bb2](https://github.com/RVCK-Project/rvck/commit/2e542bb2ebfc62156b9bbc63a6fc83e5061c9f9b) | 2024-06-17 | zhangye | Enable XUANTIE ISA for memcpy performance |
| [edcb9edb](https://github.com/RVCK-Project/rvck/commit/edcb9edbba7d576333aa0d147ed6fe1e6123f935) | 2024-03-27 | Chen Pei | riscv: build: Support compiling kernel using Xuantie toolchain |
| [442725c6](https://github.com/RVCK-Project/rvck/commit/442725c63e99d0ec6322a2d8fa6d1fae9620be54) | 2024-06-17 | David Li | i2s: remove debug message |
| [e47885f8](https://github.com/RVCK-Project/rvck/commit/e47885f82b494d9c065f9f3cbd0d83eb5b83df8d) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the aon gpio range configuration error |
| [52510caa](https://github.com/RVCK-Project/rvck/commit/52510caa3156bcc071d98e0943a3bc0582cd56ab) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix spi/qspi1 cs pin duplicate configuration error |
| [7791afcf](https://github.com/RVCK-Project/rvck/commit/7791afcf198f182b019a7d8c5cbb835d60700e60) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the gpio range configuration error |
| [b4ba84f0](https://github.com/RVCK-Project/rvck/commit/b4ba84f05746bbde94e122ae2f506ebfc36e69f7) | 2024-06-16 | Esther Z | drivers: regulator: add th1520 AON virtual regulator control support. |
| [8333e86b](https://github.com/RVCK-Project/rvck/commit/8333e86b48f072acd5392f093cc825b692a95d61) | 2024-06-16 | Esther Z | dt-bindings: add AON resource id headfile |
| [ccb4aa58](https://github.com/RVCK-Project/rvck/commit/ccb4aa588b76ed5ad7b02c709a67e182eb3d04c7) | 2024-06-16 | Esther Z | drivers: pmdomain: support th1520 Power domain control. |
| [5aa4bff5](https://github.com/RVCK-Project/rvck/commit/5aa4bff572ac8ca8000e4f14fdbb9bdb40583cd4) | 2024-06-15 | David Li | i2s: add i2s driver for XuanTie TH1520 SoC |
| [ea90098b](https://github.com/RVCK-Project/rvck/commit/ea90098b638d172cec2f2a1af510ab1696d6545d) | 2024-06-15 | David Li | configs: xuantie: correct definition of SoC Architecture |
| [4370b518](https://github.com/RVCK-Project/rvck/commit/4370b5183b11d0dda896b71b8614db17ebd8b781) | 2024-06-11 | lst | i2c: designware: add support for hcnt/lcnt got from dt |
| [6ea7c193](https://github.com/RVCK-Project/rvck/commit/6ea7c193c58fe527d85042f26d61aee6d6d58ee1) | 2024-06-06 | Xiangyi Zeng | riscv:dts:thead: Add TH1520 event and watchdog device node |
| [fff99582](https://github.com/RVCK-Project/rvck/commit/fff995823a4577da74aa44d5750f282be334137b) | 2024-06-06 | Xiangyi Zeng | dt-bindings:wdt: Add Documentation for THEAD TH1520 pmic watchdog |
| [51a3fdd9](https://github.com/RVCK-Project/rvck/commit/51a3fdd97383e507357eaaac38b5368c17db46f4) | 2024-06-06 | Xiangyi Zeng | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [11e7141a](https://github.com/RVCK-Project/rvck/commit/11e7141a0204d37f04892139ecdcd6f8875b7adf) | 2024-06-06 | Xiangyi Zeng | dt-bindings:event: Add Documentation for THEAD TH1520 event driver |
| [bd0149be](https://github.com/RVCK-Project/rvck/commit/bd0149beaf43a111028fe89a3db8d9efabcb3e3c) | 2024-06-06 | Xiangyi Zeng | drivers/soc/event: Add THEAD TH1520 event driver |
| [f690f65b](https://github.com/RVCK-Project/rvck/commit/f690f65bfab485445b5bac5a606bae8aae37d71a) | 2024-06-05 | xianbing Zhu | net:stmmac: increase timeout for dma reset |
| [1c5f00ee](https://github.com/RVCK-Project/rvck/commit/1c5f00eede978dc7c06fefc5840bfb80e5ea02ed) | 2024-06-05 | xianbing Zhu | stmmac:dwmac-thead: add support for suspend/resume feature |
| [68ff26db](https://github.com/RVCK-Project/rvck/commit/68ff26db127cd2309378e72a64f93825fc2ef40a) | 2024-06-04 | xianbing Zhu | net:dwmac-thead: dd ptp clk set and enable |
| [9dc7efec](https://github.com/RVCK-Project/rvck/commit/9dc7efec1f515cc844fece3a9d1e710b35067d46) | 2024-06-05 | Esther Z | configs: Enable th1520 mailbox. |
| [fc91cd65](https://github.com/RVCK-Project/rvck/commit/fc91cd658168cf9cacc3f51547f51a63941d8649) | 2021-08-10 | fugang.duan | firmware: thead: c910_aon: add th1520 Aon protocol driver |
| [0d0d5615](https://github.com/RVCK-Project/rvck/commit/0d0d5615328f45271970636c5063ddd04a80fb0d) | 2024-06-04 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 add delay line in different mode and sdio rxclk del... |
| [6e2b76c7](https://github.com/RVCK-Project/rvck/commit/6e2b76c74b15c443cbe9fd1deba8cfa3bb21e0c3) | 2024-06-03 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 larger tuning max loop count to 128 |
| [7d04d396](https://github.com/RVCK-Project/rvck/commit/7d04d3963ea451b8e69b343bf8d594c9794a889e) | 2024-05-31 | xianbing Zhu | dts: th1520: enable sdio1 for wifi card in lichee-pi-4a |
| [f3f0dd88](https://github.com/RVCK-Project/rvck/commit/f3f0dd884f9fa0bb4d29f6b2483d8b915a85f1e5) | 2024-05-31 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 sdhci add fix io voltage 1v8 |
| [632e0c09](https://github.com/RVCK-Project/rvck/commit/632e0c0977d19393f8cf019f473f8d5338fc86e1) | 2024-05-30 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 resolve accss rpmb error in hs400 |
| [c2ee8431](https://github.com/RVCK-Project/rvck/commit/c2ee843198a6114da401418ee1ae156d9c357eee) | 2024-05-30 | Xiangyi Zeng | drivers/dmac: add pm suspend/resume for dma driver |
| [8885fa99](https://github.com/RVCK-Project/rvck/commit/8885fa9946de27072f52b90e7cf226c9a7b74361) | 2023-08-21 | David Li | audio: th1520: add dma chan str for dmaengine |
| [020ca59b](https://github.com/RVCK-Project/rvck/commit/020ca59b74bf96a67b5e6c8a3fb6c02559114a2e) | 2024-05-30 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 dmac1 and dmac2 device node |
| [545a54e6](https://github.com/RVCK-Project/rvck/commit/545a54e65198195b44e71a1d18af29630b58cf24) | 2023-08-22 | sanyi | STR: fix pca953x resume bug |
| [260acd90](https://github.com/RVCK-Project/rvck/commit/260acd9026722119be41c1c49206c7a05aeb4800) | 2024-05-28 | Xiangyi Zeng | drivers/iio/adc: add sysfs_remove_file when adc driver removed |
| [36e4816b](https://github.com/RVCK-Project/rvck/commit/36e4816bbd22de41b649d7d21b48057d75046f6d) | 2024-05-27 | Xiangyi Zeng | drivers/pvt: add mr75203 driver pm feature and correct temperature coefficient |
| [736997ff](https://github.com/RVCK-Project/rvck/commit/736997ffe58a3bb32c936e60f3c4d61169a9fcef) | 2024-05-27 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 SPI/QSPI device node |
| [f14ed93b](https://github.com/RVCK-Project/rvck/commit/f14ed93b2ee247aa5eaf7057ce9f192ea39ea52b) | 2024-05-27 | Xiangyi Zeng | dt-bindings: spi/qspi: Add Documentation for THEAD TH1520 SPI/QSPI |
| [5d5c7da2](https://github.com/RVCK-Project/rvck/commit/5d5c7da2cd3ebe0308962e4c82aea9dd7779f200) | 2024-05-27 | Xiangyi Zeng | drivers/spi: Add THEAD TH1520 QSPI driver |
| [d9fd01f7](https://github.com/RVCK-Project/rvck/commit/d9fd01f7d2b46a25193b02a54fe64517b7456a66) | 2024-05-19 | Wei Fu | riscv: dts: thead: Add XuanTie TH1520 Mailbox device node |
| [a4da68bd](https://github.com/RVCK-Project/rvck/commit/a4da68bd9e3bd2c6e508dea57007a07736e06ac5) | 2024-05-17 | Fugang Duan | mailbox: add XuanTie TH1520 Mailbox IPC driver |
| [45febcf5](https://github.com/RVCK-Project/rvck/commit/45febcf5dead29f7e74499a1defefc816c4b9f83) | 2024-05-19 | Wei Fu | dt-bindings: mailbox: Add a binding file for XuanTie TH1520 Mailbox |
| [733807e3](https://github.com/RVCK-Project/rvck/commit/733807e3fd58ecfa42d99c3bb8f44529ec881c82) | 2024-05-17 | Xiangyi Zeng | dt-bindings: adc: Add Documentation for THEAD TH1520 ADC |
| [3963eaae](https://github.com/RVCK-Project/rvck/commit/3963eaaef55fabaafd468c1b9fd910d6ce6edc11) | 2024-05-17 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 ADC device node |
| [18c4497b](https://github.com/RVCK-Project/rvck/commit/18c4497bc2cbd0e9638228285447cd9f81ef3884) | 2024-05-17 | Xiangyi Zeng | drivers/iio/adc: Add THEAD TH1520 ADC driver |
| [51c329d2](https://github.com/RVCK-Project/rvck/commit/51c329d24d32248fe4422bca8b36232c5b2f6b62) | 2024-06-29 | Chen Pei | riscv: ptrace: Fix ptrace using uninitialized riscv_v_vsize |
| [ec633acf](https://github.com/RVCK-Project/rvck/commit/ec633acf6cb8c703aa089c9e71eef12fca88079e) | 2024-03-18 | Heiko Stuebner | T-Head C9xx cores implement an older version (0.7.1) of the vector specification... |
---

**共 409 条提交（显示全部）**

[分页显示](阿里达摩院.md) | [纯文本视图](阿里达摩院_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-11-12 15:39:57*
*数据来源: 主分支 rvck-6.6@dc246d68*
