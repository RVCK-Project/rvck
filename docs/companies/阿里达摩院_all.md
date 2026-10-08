# 阿里达摩院 贡献详情

<div style="background-color: #F4433620; padding: 15px; border-radius: 8px; border-left: 5px solid #F44336;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 392</li>
<li><strong>统计时间</strong>: 2026-10-08 19:27:23</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.158</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @linux.alibaba.com

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [a5ff5cce](https://github.com/RVCK-Project/rvck/commit/a5ff5cce240c8e29f608576b9887769e2b42fdbc) | 2026-09-17 | ZhenXing Zhu | irqchip: thead-c900-aclint-sswi: Fixup riscv_ipi_set_virq_range() conflict |
| [ecff7318](https://github.com/RVCK-Project/rvck/commit/ecff731849dea47dcb964b1d3d3462b9afc707ab) | 2024-05-22 | Palmer Dabbelt | irqchip: riscv-imsic: Fixup riscv_ipi_set_virq_range() conflict |
| [03872d9f](https://github.com/RVCK-Project/rvck/commit/03872d9f2a34fa5e08d1c0aca011635fe5af6042) | 2024-03-26 | Samuel Holland | riscv: mm: Always use an ASID to flush mm contexts |
| [2c98ed9f](https://github.com/RVCK-Project/rvck/commit/2c98ed9fc1ca896ed33066fc2442ebc8aed22201) | 2024-03-26 | Samuel Holland | riscv: mm: Preserve global TLB entries when switching contexts |
| [b4630885](https://github.com/RVCK-Project/rvck/commit/b4630885d4878c1902fde0df5d6751918c5f339e) | 2024-03-26 | Samuel Holland | riscv: mm: Make asid_bits a local variable |
| [36f075f5](https://github.com/RVCK-Project/rvck/commit/36f075f554e46dfed9d6ef60c2c02a155aace100) | 2024-03-26 | Samuel Holland | riscv: mm: Use a fixed layout for the MM context ID |
| [7f0429d7](https://github.com/RVCK-Project/rvck/commit/7f0429d7408040679d05caaaf4168da8b18cdd10) | 2024-03-26 | Samuel Holland | riscv: mm: Introduce cntx2asid/cntx2version helper macros |
| [15213e40](https://github.com/RVCK-Project/rvck/commit/15213e40373ad160e0bc0c3844caabc3e77a72b2) | 2024-03-26 | Samuel Holland | riscv: Avoid TLB flush loops when affected by SiFive CIP-1200 |
| [296d71fa](https://github.com/RVCK-Project/rvck/commit/296d71fa581b0ff583b22df35b7ac4c6cc8a1e21) | 2024-03-26 | Samuel Holland | riscv: mm: Combine the SMP and UP TLB flush code |
| [b1076d5b](https://github.com/RVCK-Project/rvck/commit/b1076d5bc2e99b7168ef527617d55e6ce072606f) | 2024-03-26 | Samuel Holland | riscv: Only send remote fences when some other CPU is online |
| [6e69bacd](https://github.com/RVCK-Project/rvck/commit/6e69bacd304100464eedaa4535457046ce5497a6) | 2024-03-26 | Samuel Holland | riscv: mm: Broadcast kernel TLB flushes only when needed |
| [8060c903](https://github.com/RVCK-Project/rvck/commit/8060c9036e8d1ce0d937002d8617ccb3c24874ea) | 2024-03-26 | Samuel Holland | riscv: Use IPIs for remote cache/TLB flushes by default |
| [7abcfbd7](https://github.com/RVCK-Project/rvck/commit/7abcfbd78d0114601f15c409ee0671d7b65e3364) | 2024-03-26 | Samuel Holland | riscv: Factor out page table TLB synchronization |
| [1eebb90c](https://github.com/RVCK-Project/rvck/commit/1eebb90c4a0c6f3b72d5cfa24df51c0c9db969ed) | 2024-03-26 | Samuel Holland | riscv: Flush the instruction cache during SMP bringup |
| [a355bb56](https://github.com/RVCK-Project/rvck/commit/a355bb56d08e6e5b618d275a86bff3a0fbc227c8) | 2026-08-11 | ZhenXing Zhu | riscv: dts: thead: add TH1520 ACLINT SSWI interrupt-controller node |
| [1c2c91b2](https://github.com/RVCK-Project/rvck/commit/1c2c91b2458368d6e3e87fe93147c20dd428ecf5) | 2026-08-11 | ZhenXing Zhu | Revert "riscv: Add ACLINT SSWI support" |
| [a6b28c18](https://github.com/RVCK-Project/rvck/commit/a6b28c18a193fd2157c28b03e417abc8ca8280cf) | 2025-10-20 | Vivian Wang | riscv: tests: Make RISCV_KPROBES_KUNIT tristate |
| [f96420c8](https://github.com/RVCK-Project/rvck/commit/f96420c8314f1e0b62ec15bd3ac9dc23fcee9e4c) | 2025-05-13 | Nam Cao | riscv: Add kprobes KUnit test |
| [f3d88e7c](https://github.com/RVCK-Project/rvck/commit/f3d88e7ca69fdd76bae55249853b4548848ff19f) | 2023-11-01 | Charlie Jenkins | riscv: Add tests for riscv module loading |
| [27926c34](https://github.com/RVCK-Project/rvck/commit/27926c3422879cf69b67013522290e8e83f1442d) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_ITYPE_IMM |
| [18214582](https://github.com/RVCK-Project/rvck/commit/18214582735974582d65c59125aaf67b5b89b771) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_UTYPE_IMM |
| [24ea94f9](https://github.com/RVCK-Project/rvck/commit/24ea94f9f8f81041125ff09be86b5e8eccbc03d5) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_RD_REG |
| [72073812](https://github.com/RVCK-Project/rvck/commit/72073812411cd719bc9db0325208d0b812317bda) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_BTYPE_IMM |
| [cb9912ff](https://github.com/RVCK-Project/rvck/commit/cb9912ff41b18e3873738abd180583e6654b9d7b) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_C2_RS1_REG |
| [0c181e2a](https://github.com/RVCK-Project/rvck/commit/0c181e2a3b06343278ce6acd39edea626c1e2478) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RVC_EXTRACT_JTYPE_IMM |
| [6541a019](https://github.com/RVCK-Project/rvck/commit/6541a019d3063263167868430be3de5b112098e0) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_BTYPE_IMM |
| [a110b7e0](https://github.com/RVCK-Project/rvck/commit/a110b7e0cd07d50275311aaadc8378e0760d6aa1) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_RS1_REG |
| [dd9a4d5a](https://github.com/RVCK-Project/rvck/commit/dd9a4d5acbcb98b751822007f1e9bf39691bb1b0) | 2025-05-11 | Nam Cao | riscv: kprobes: Remove duplication of RV_EXTRACT_JTYPE_IMM |
| [54250a86](https://github.com/RVCK-Project/rvck/commit/54250a86ea50f872eb8354934652746d9ef4c543) | 2025-05-11 | Nam Cao | riscv: kprobes: Move branch_funct3 to insn.h |
| [ad21f7b8](https://github.com/RVCK-Project/rvck/commit/ad21f7b87405948377f88391f7e15345261205f5) | 2025-05-11 | Nam Cao | riscv: kprobes: Move branch_rs2_idx to insn.h |
| [fb5157ec](https://github.com/RVCK-Project/rvck/commit/fb5157ecc79f5d6e61de26b7c673c4f1d873949d) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: disable unused A210 UARTs by default |
| [add0e62c](https://github.com/RVCK-Project/rvck/commit/add0e62ccaf982235ee1b9b86254443758e9d986) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: enable GPIO controllers on A210 boards |
| [bbe0a612](https://github.com/RVCK-Project/rvck/commit/bbe0a6128355785a77bfac4ef81de3b13b7c83ff) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 SPI controllers |
| [72f0f183](https://github.com/RVCK-Project/rvck/commit/72f0f1833e3aa52ef5ae63bad1407142c3469562) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 I2C controllers |
| [79d13558](https://github.com/RVCK-Project/rvck/commit/79d13558b7a2a834dbd3581acb5e23bab9663723) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 DW APB timers |
| [fb2dd701](https://github.com/RVCK-Project/rvck/commit/fb2dd701b1b344e1d5ac1f58dd6da38636b36db7) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 GPIO controllers |
| [da33359a](https://github.com/RVCK-Project/rvck/commit/da33359ae1286b02b8c32fee44aef18749107a27) | 2026-09-21 | Zhiguo Zhu | riscv: dts: zhihe: add A210 DMA controllers |
| [fa35fdae](https://github.com/RVCK-Project/rvck/commit/fa35fdae233a69e2b2d003482b4f080202ee354c) | 2026-09-21 | Zhiguo Zhu | dmaengine: dw-axi-dmac: add ZhiHe A210 support |
| [220d3016](https://github.com/RVCK-Project/rvck/commit/220d301664a4e3707b297a69bb047e3d5efe1ec4) | 2026-09-21 | Zhiguo Zhu | dt-bindings: dma: snps,dw-axi-dmac: add ZhiHe A210 |
| [bca0a1ae](https://github.com/RVCK-Project/rvck/commit/bca0a1aeeec038ad76f19cab295a419bdbc22375) | 2026-09-30 | Zhiguo Zhu | riscv: dts: zhihe: rename A210 board to Melon Pi |
| [4b3e4242](https://github.com/RVCK-Project/rvck/commit/4b3e424297bcd8b04244a0c721817b51d3b2abec) | 2026-09-30 | Zhiguo Zhu | dt-bindings: riscv: zhihe: rename A210 board to Melon Pi |
| [4ec0b3f0](https://github.com/RVCK-Project/rvck/commit/4ec0b3f0ca697b74573e34f400ff028ee8df147f) | 2026-09-12 | ZhenXing Zhu | firmware: thead: th1520_event: make stubs static inline |
| [19a69de8](https://github.com/RVCK-Project/rvck/commit/19a69de81870446173633bbc1914ce0a0059242d) | 2026-08-31 | Zhiguo Zhu | riscv: rvck_defconfig: enable ZhiHe A210 drivers as modules |
| [c3b55180](https://github.com/RVCK-Project/rvck/commit/c3b551806956fdc743976cb757ec7a8b37e049d2) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 power domains |
| [f76a687d](https://github.com/RVCK-Project/rvck/commit/f76a687d1e40e88aa6c6551f4808a1c210e22aad) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 AON subsystem |
| [200cbe81](https://github.com/RVCK-Project/rvck/commit/200cbe81841699bf2f6ffacb404e10071899447b) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 pinctrl |
| [cb5735e4](https://github.com/RVCK-Project/rvck/commit/cb5735e42e17a6b304762ffe5dd877546f41f502) | 2026-08-18 | Zhiguo Zhu | pmdomain: zhihe: add A210 power domain support |
| [5435327d](https://github.com/RVCK-Project/rvck/commit/5435327dc65db459bbdb1084dc6a565c43125563) | 2026-08-27 | Zhiguo Zhu | dt-bindings: power: zhihe: add A210 power domains |
| [614279c9](https://github.com/RVCK-Project/rvck/commit/614279c96950dbcff7438e3a790f61e2e01074b6) | 2026-08-27 | Zhiguo Zhu | dt-bindings: clock: zhihe: add A210 CCU |
| [318cee2e](https://github.com/RVCK-Project/rvck/commit/318cee2e3e2e343efb7323482fb8c36775094bf1) | 2026-08-18 | Zhiguo Zhu | regulator: zhihe: add A210 AON regulator driver |
| [0a205db4](https://github.com/RVCK-Project/rvck/commit/0a205db4326e5a0063e76c02d3caa3da4a41ef0b) | 2026-08-18 | Zhiguo Zhu | firmware: zhihe: add A210 AON subsystem driver |
| [7c89f75b](https://github.com/RVCK-Project/rvck/commit/7c89f75b5538c5639879e3a01de46ab9583c3768) | 2026-08-18 | Zhiguo Zhu | mailbox: zhihe: add A210 mailbox controller driver |
| [01e46593](https://github.com/RVCK-Project/rvck/commit/01e46593a5cc06aa43f9849fa69730561236887d) | 2026-08-27 | Zhiguo Zhu | dt-bindings: regulator: zhihe: add A210 AON regulator |
| [4c627a5d](https://github.com/RVCK-Project/rvck/commit/4c627a5d0fc8c54639da7598ef473e04581f5132) | 2026-08-27 | Zhiguo Zhu | dt-bindings: mailbox: zhihe: add A210 mailbox controller |
| [beae0c7d](https://github.com/RVCK-Project/rvck/commit/beae0c7d012ccff2d6f5a8f92a48169247beb09f) | 2026-08-27 | Zhiguo Zhu | dt-bindings: firmware: zhihe: add A210 AON subsystem |
| [d172c12b](https://github.com/RVCK-Project/rvck/commit/d172c12b2685ca641806679efaf4b6bd7ab46fe3) | 2026-08-18 | Zhiguo Zhu | pinctrl: zhihe: add A210 pinctrl driver |
| [12845fb7](https://github.com/RVCK-Project/rvck/commit/12845fb754b1345a4acf28908cf9ff22abcd6ac2) | 2026-08-18 | Zhiguo Zhu | dt-bindings: pinctrl: zhihe: add A210 pinctrl |
| [872d4be4](https://github.com/RVCK-Project/rvck/commit/872d4be4b0308df9f2214080c1972d4a51255804) | 2026-08-18 | Zhiguo Zhu | riscv: dts: zhihe: add A210 Ethernet controllers |
| [88075bdd](https://github.com/RVCK-Project/rvck/commit/88075bdd803349b5d8db8d76cce50112b75c5d28) | 2026-08-18 | Zhiguo Zhu | net: stmmac: add ZhiHe A210 DWMAC glue layer |
| [17935fe7](https://github.com/RVCK-Project/rvck/commit/17935fe76dfc26abd681e9a8faa78ee00805d889) | 2026-08-18 | Zhiguo Zhu | dt-bindings: net: zhihe: add A210 DWMAC |
| [99ecab28](https://github.com/RVCK-Project/rvck/commit/99ecab28ff84228e455ce43eae4ce6eb5e035f50) | 2026-08-20 | Zhiguo Zhu | dt-bindings: mfd: syscon: add ZhiHe A210 GMAC syscon |
| [007e7222](https://github.com/RVCK-Project/rvck/commit/007e72222af6b2f312bb38865f13a6bc85ce7a65) | 2026-08-13 | Zhiguo Zhu | riscv: rvck_defconfig: enable ZhiHe A210 support |
| [1333412e](https://github.com/RVCK-Project/rvck/commit/1333412ea35860039e3cce008c702dd376198260) | 2026-08-13 | Zhiguo Zhu | riscv: dts: add ZhiHe A210 device tree support |
| [46ab3a7f](https://github.com/RVCK-Project/rvck/commit/46ab3a7f21a73aa0a582c94302055fa7825583ba) | 2026-08-13 | Zhiguo Zhu | irqchip/sifive-plic: add ZhiHe A210 PLIC support |
| [6b2feeba](https://github.com/RVCK-Project/rvck/commit/6b2feeba42eb42eb816b982510ed770b5deb87ec) | 2026-08-11 | Zhiguo Zhu | mmc: sdhci-of-dwcmshc: add ZhiHe A210 support |
| [e62b9a73](https://github.com/RVCK-Project/rvck/commit/e62b9a7315cf1f1b7f02af50a0180b28dbc266b9) | 2026-08-13 | Zhiguo Zhu | reset: zhihe: add A210 reset controller driver |
| [d23340aa](https://github.com/RVCK-Project/rvck/commit/d23340aa5f7d935760b78c68ca35aa0ea07dedde) | 2026-08-13 | Zhiguo Zhu | clk: zhihe: add A210 clock controller driver |
| [92f14021](https://github.com/RVCK-Project/rvck/commit/92f1402194eccb9fbcfe48ce3d0204fafa7fc8af) | 2026-08-13 | Zhiguo Zhu | riscv: add Kconfig option for ZHIHE SoC family |
| [ae001640](https://github.com/RVCK-Project/rvck/commit/ae0016407b71fe7e4ece251d61ec82f35065e803) | 2026-08-13 | Zhiguo Zhu | dt-bindings: interrupt-controller: sifive,plic: add ZhiHe A210 |
| [9dc0c7fa](https://github.com/RVCK-Project/rvck/commit/9dc0c7fa7e57b704a3dec14427a83cebc57f4d34) | 2026-08-11 | Zhiguo Zhu | dt-bindings: mmc: snps,dwcmshc: add ZhiHe A210 |
| [bcbc4c79](https://github.com/RVCK-Project/rvck/commit/bcbc4c799fe7bd88658362d731e528d4398a95cf) | 2026-08-13 | Zhiguo Zhu | dt-bindings: reset: zhihe: add reset controller for A210 |
| [a46353c6](https://github.com/RVCK-Project/rvck/commit/a46353c647a1e3f9b2bff25f79fabd16d5b57f9c) | 2026-08-13 | Zhiguo Zhu | dt-bindings: clock: zhihe: add clock controller for A210 |
| [8818fb87](https://github.com/RVCK-Project/rvck/commit/8818fb87b27d2a0c63c6ac0bfb844ef58ebdb4a4) | 2026-08-13 | Zhiguo Zhu | dt-bindings: riscv: zhihe: Add A210 evaluation board |
| [21dab95b](https://github.com/RVCK-Project/rvck/commit/21dab95be105cbdf353777e45fb66da57e0fc17a) | 2026-08-13 | Zhiguo Zhu | dt-bindings: vendor-prefixes: add ZhiHe |
| [c1d5be07](https://github.com/RVCK-Project/rvck/commit/c1d5be07101e38149a0db2f6921ab0c22e641bbc) | 2024-05-10 | Andy Chiu | riscv: vector: adjust minimum Vector requirement to ZVE32X |
| [653d4d74](https://github.com/RVCK-Project/rvck/commit/653d4d747216e66871a4f5c6842c4d00de07b75f) | 2025-09-19 | Han Gao | riscv: dts: thead: add xtheadvector to the th1520 devicetree |
| [1f613eb1](https://github.com/RVCK-Project/rvck/commit/1f613eb1bfb9af4ee336c7d6d79519672578662c) | 2025-11-19 | Sergey Matyukevich | riscv: dts: allwinner: d1: fix vlenb property |
| [d041e8d2](https://github.com/RVCK-Project/rvck/commit/d041e8d20cfa207e4b666e1a0d64da5415f42936) | 2025-10-18 | Paul Walmsley | riscv: cpufeature: avoid uninitialized variable in has_thead_homogeneous_vlenb() |
| [765ba225](https://github.com/RVCK-Project/rvck/commit/765ba225c5f276948cc59ce404a8c0ea272a767d) | 2025-05-23 | Han Gao | riscv: vector: Fix context save/restore with xtheadvector |
| [87e4364a](https://github.com/RVCK-Project/rvck/commit/87e4364a6a1a67202fc4e9751f65106a5f7cc040) | 2024-11-13 | Charlie Jenkins | selftests: riscv: Support xtheadvector in vector tests |
| [5be66d8e](https://github.com/RVCK-Project/rvck/commit/5be66d8e90d425cebea1824b11bcc2344edff08a) | 2024-11-13 | Charlie Jenkins | selftests: riscv: Fix vector tests |
| [5c5eba61](https://github.com/RVCK-Project/rvck/commit/5c5eba619c201daa4de62c5d1e02a990f779eb9a) | 2024-11-13 | Charlie Jenkins | riscv: hwprobe: Document thead vendor extensions and xtheadvector extension |
| [653abf76](https://github.com/RVCK-Project/rvck/commit/653abf7625604623a24f348c0483e70412eb4fd2) | 2024-11-13 | Charlie Jenkins | riscv: vector: Support xtheadvector save/restore |
| [4d5c627a](https://github.com/RVCK-Project/rvck/commit/4d5c627a89a4e7f414e19ce743b4fa740ef56f11) | 2024-11-13 | Charlie Jenkins | riscv: Add xtheadvector instruction definitions |
| [b9fc3705](https://github.com/RVCK-Project/rvck/commit/b9fc370576b8bf48046cc19948402c0a2f4fd66d) | 2024-11-13 | Charlie Jenkins | riscv: csr: Add CSR encodings for CSR_VXRM/CSR_VXSAT |
| [59b4b279](https://github.com/RVCK-Project/rvck/commit/59b4b27960740cc038236d990eaa66692893b1b2) | 2024-11-13 | Heiko Stuebner | RISC-V: define the elements of the VCSR vector CSR |
| [df0cf762](https://github.com/RVCK-Project/rvck/commit/df0cf7622935fe98f5e775abab9dc3de5f0d82ac) | 2024-11-13 | Charlie Jenkins | riscv: vector: Use vlenb from DT for thead |
| [160a466b](https://github.com/RVCK-Project/rvck/commit/160a466b7c3b3e0ceb85a1d0caea9c07bdbedfa0) | 2024-11-13 | Charlie Jenkins | riscv: Add thead and xtheadvector as a vendor extension |
| [8912eb92](https://github.com/RVCK-Project/rvck/commit/8912eb9238c72423df427bb9cae8c715482eaf03) | 2024-11-13 | Charlie Jenkins | riscv: dts: allwinner: Add xtheadvector to the D1/D1s devicetree |
| [9a0cead5](https://github.com/RVCK-Project/rvck/commit/9a0cead57ed62100ff5b1d91c56d877fe3ea5085) | 2024-11-13 | Charlie Jenkins | dt-bindings: cpus: add a thead vlen register length property |
| [79547fab](https://github.com/RVCK-Project/rvck/commit/79547fab979b9086b2338a7d4a0b628eab6ee757) | 2024-11-13 | Charlie Jenkins | dt-bindings: riscv: Add xtheadvector ISA extension description |
| [5d0a33f7](https://github.com/RVCK-Project/rvck/commit/5d0a33f710ad6c883ec8c8a43a602a4e96381251) | 2023-10-09 | Conor Dooley | riscv: dts: allwinner: convert isa detection to new properties |
| [457b5b75](https://github.com/RVCK-Project/rvck/commit/457b5b7535ec0e910af095fcfe12e537d77de34d) | 2026-07-29 | ZhenXing Zhu | Revert "T-Head C9xx cores implement an older version (0.7.1) of the vector speci... |
| [31a7d899](https://github.com/RVCK-Project/rvck/commit/31a7d899bc9a7f549a9c8466b7388e321d5e8331) | 2026-07-29 | ZhenXing Zhu | Revert "riscv: xtheadvector: enable vector function" |
| [7433b63d](https://github.com/RVCK-Project/rvck/commit/7433b63d7e2d09105f29276661041b7f06333120) | 2026-07-29 | ZhenXing Zhu | Revert "xtheadvector: fix it used as v-ext when hwprobe is used" |
| [d64d5eed](https://github.com/RVCK-Project/rvck/commit/d64d5eed96c5313132accda8efc1198e369607e0) | 2026-07-29 | ZhenXing Zhu | Revert "fix: riscv: xtheadvector: fix setup_v_vsize" |
| [dcbc4b99](https://github.com/RVCK-Project/rvck/commit/dcbc4b99d2fa982270b7f0e8928d7d16b832852c) | 2024-01-03 | Leonardo Bras | riscv: Introduce set_compat_task() in asm/compat.h |
| [281afa91](https://github.com/RVCK-Project/rvck/commit/281afa91f3bfc3dac353069a05f3343960cd9fd0) | 2024-01-03 | Leonardo Bras | riscv: Introduce is_compat_thread() into compat.h |
| [4f705870](https://github.com/RVCK-Project/rvck/commit/4f7058708c6e17f05f0b2cb3f6ecacfa4ff93deb) | 2024-01-03 | Leonardo Bras | riscv: add compile-time test into is_compat_task() |
| [a558b763](https://github.com/RVCK-Project/rvck/commit/a558b763185528716d8f3e605b323051b8425fc7) | 2024-01-03 | Leonardo Bras | riscv: Replace direct thread flag check with is_compat_task() |
| [b3e5e42a](https://github.com/RVCK-Project/rvck/commit/b3e5e42aa0ffdabb4944a097e5995a95c6f10b19) | 2024-10-16 | Samuel Holland | KVM: riscv: selftests: Add Smnpm and Ssnpm to get-reg-list test |
| [b3ca609f](https://github.com/RVCK-Project/rvck/commit/b3ca609f7a734c87166ca9e18ae2439fdaf1b1d0) | 2024-10-16 | Samuel Holland | riscv: selftests: Add a pointer masking test |
| [1acd11d7](https://github.com/RVCK-Project/rvck/commit/1acd11d7425d721b32a712bab004dce27986bd54) | 2024-10-16 | Samuel Holland | riscv: Allow ptrace control of the tagged address ABI |
| [724884f4](https://github.com/RVCK-Project/rvck/commit/724884f4a2f9c9f75ddeca92a0935cfcbcf65fe3) | 2024-10-16 | Samuel Holland | riscv: Add support for the tagged address ABI |
| [3e22c7f6](https://github.com/RVCK-Project/rvck/commit/3e22c7f6070624ef5e6293ea078ee6cbac2fd3cd) | 2024-10-16 | Samuel Holland | riscv: Add support for userspace pointer masking |
| [77b3b664](https://github.com/RVCK-Project/rvck/commit/77b3b664d1a6cc491eeea3391ef67628c11af748) | 2024-10-16 | Samuel Holland | riscv: Add CSR definitions for pointer masking |
| [b6156a4a](https://github.com/RVCK-Project/rvck/commit/b6156a4ada3535f4266dbe49fda45307e1c97e99) | 2026-04-04 | Charlie Jenkins | selftests: riscv: Add license to cfi selftest |
| [ad1a7fcb](https://github.com/RVCK-Project/rvck/commit/ad1a7fcb69668d4bc473aff9b9b0c0908ec8409f) | 2026-04-04 | Paul Walmsley | prctl: cfi: change the branch landing pad prctl()s to be more descriptive |
| [ae275cb4](https://github.com/RVCK-Project/rvck/commit/ae275cb4846c1d62f333d1cbae0aff271e0402f9) | 2026-04-04 | Zong Li | riscv: cfi: clear CFI lock status in start_thread() |
| [ffe1bcd2](https://github.com/RVCK-Project/rvck/commit/ffe1bcd23f7f0c32130de04d8e030211ce447eec) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: expand "SS" references to "shadow stack" in uapi headers |
| [8c7a2022](https://github.com/RVCK-Project/rvck/commit/8c7a20220503d3914e26994ab930aab0d2ee70ff) | 2026-04-04 | Paul Walmsley | prctl: rename branch landing pad implementation functions to be more explicit |
| [80c6bc29](https://github.com/RVCK-Project/rvck/commit/80c6bc295acd99c3e70c833c8ac36c720a4be4b2) | 2026-04-04 | Paul Walmsley | riscv: ptrace: expand "LP" references to "branch landing pads" in uapi headers |
| [de1428d9](https://github.com/RVCK-Project/rvck/commit/de1428d97c185f5c3230ee0443122131cdf4f062) | 2026-04-04 | Paul Walmsley | riscv: ptrace: cfi: fix "PRACE" typo in uapi header |
| [e08480de](https://github.com/RVCK-Project/rvck/commit/e08480de1b1646ef994c92758719b2f646ae70ce) | 2026-04-02 | Paul Walmsley | riscv: use _BITUL macro rather than BIT() in ptrace uapi and kselftests |
| [13d67d7b](https://github.com/RVCK-Project/rvck/commit/13d67d7b7bb64a2737aefc9b0fd1ffe82418965c) | 2026-01-25 | Deepak Gupta | kselftest/riscv: add kselftest for user mode CFI |
| [af2d5806](https://github.com/RVCK-Project/rvck/commit/af2d580637c0f3ad03446b95b0744fae64278346) | 2026-01-25 | Deepak Gupta | riscv: add documentation for shadow stack |
| [2ebb98f3](https://github.com/RVCK-Project/rvck/commit/2ebb98f36eb773a0de36ac2299d3d4c3ff51c7ad) | 2026-01-25 | Deepak Gupta | riscv: add documentation for landing pad / indirect branch tracking |
| [efb40614](https://github.com/RVCK-Project/rvck/commit/efb4061456f7755fc6c2282df6b3b137dcbf407f) | 2026-01-25 | Deepak Gupta | riscv: create a Kconfig fragment for shadow stack and landing pad support |
| [fd4eee6c](https://github.com/RVCK-Project/rvck/commit/fd4eee6cc80cc2d6998c8e3e8829a19328fef3f4) | 2026-01-25 | Deepak Gupta | arch/riscv: add dual vdso creation logic and select vdso based on hw |
| [862a3e00](https://github.com/RVCK-Project/rvck/commit/862a3e00b97abb8ef12e3a80d8bbda4e29954d07) | 2026-01-25 | Jim Shu | arch/riscv: compile vdso with landing pad and shadow stack note |
| [b6b0de16](https://github.com/RVCK-Project/rvck/commit/b6b0de16701a46dab2ee7a3d5c13777a8e450e40) | 2024-10-16 | Alexandre Ghiti | riscv: Check that vdso does not contain any dynamic relocations |
| [a71e7d58](https://github.com/RVCK-Project/rvck/commit/a71e7d58444fb0ac96a7d8eb8a4ab4eafd3ce7c3) | 2026-01-25 | Deepak Gupta | riscv: enable kernel access to shadow stack memory via the FWFT SBI call |
| [b461d733](https://github.com/RVCK-Project/rvck/commit/b461d73369190d374b30c0a5c16e9f0d1a374b74) | 2026-01-25 | Deepak Gupta | riscv: add kernel command line option to opt out of user CFI |
| [43de4023](https://github.com/RVCK-Project/rvck/commit/43de4023597d0a0322d50f86ab06c8de0e189c93) | 2026-01-25 | Deepak Gupta | riscv/hwprobe: add zicfilp / zicfiss enumeration in hwprobe |
| [9862f6fc](https://github.com/RVCK-Project/rvck/commit/9862f6fcba4588c1e6a4170962caa105753f036b) | 2026-01-25 | Paul Walmsley | riscv: hwprobe: add support for RISCV_HWPROBE_KEY_IMA_EXT_1 |
| [c679b285](https://github.com/RVCK-Project/rvck/commit/c679b28561dca3fa6011d67c4fe10166ea1e349e) | 2026-01-25 | Deepak Gupta | riscv/ptrace: expose riscv CFI status and state via ptrace and in core files |
| [ff7bb3bf](https://github.com/RVCK-Project/rvck/commit/ff7bb3bf05acdc67005df609252b81b540daacdd) | 2026-01-25 | Deepak Gupta | riscv/kernel: update __show_regs() to print shadow stack register |
| [838bd85f](https://github.com/RVCK-Project/rvck/commit/838bd85faeface017a6d825b1359d50af9810c4f) | 2026-01-25 | Deepak Gupta | riscv/signal: save and restore the shadow stack on a signal |
| [f9f5d3f1](https://github.com/RVCK-Project/rvck/commit/f9f5d3f12c3077bdf4b6c294677d813faa7d6a9f) | 2026-01-25 | Deepak Gupta | riscv/traps: Introduce software check exception and uprobe handling |
| [dc9e15fc](https://github.com/RVCK-Project/rvck/commit/dc9e15fcde4a81cdcdcfd76730479ff37d12a74b) | 2026-01-25 | Deepak Gupta | riscv: Implement indirect branch tracking prctls |
| [dbe5e5eb](https://github.com/RVCK-Project/rvck/commit/dbe5e5eb08709c82264fa6f7514fa3a796fb4c07) | 2026-01-25 | Deepak Gupta | prctl: add arch-agnostic prctl()s for indirect branch tracking |
| [596a0a12](https://github.com/RVCK-Project/rvck/commit/596a0a124247c4a40e1b74908fc50d23eed02d03) | 2024-10-01 | Mark Brown | mman: Add map_shadow_stack() flags |
| [5bbc2e29](https://github.com/RVCK-Project/rvck/commit/5bbc2e2996bec2380503fcf02783e8fccea0429c) | 2024-10-01 | Mark Brown | prctl: arch-agnostic prctl for shadow stack |
| [21307ff9](https://github.com/RVCK-Project/rvck/commit/21307ff9c0d715fbe10e76a7efe116d4dfc5fc4e) | 2026-01-25 | Deepak Gupta | riscv: Implement arch-agnostic shadow stack prctls |
| [41da181d](https://github.com/RVCK-Project/rvck/commit/41da181d0233e854fb3d78e3ae7fb3327dc53463) | 2026-01-25 | Deepak Gupta | riscv/shstk: If needed allocate a new shadow stack on clone |
| [d6ddc96a](https://github.com/RVCK-Project/rvck/commit/d6ddc96af161971791d773c84706f62a0a18f781) | 2026-01-25 | Deepak Gupta | riscv/mm: Implement map_shadow_stack() syscall |
| [e88d444f](https://github.com/RVCK-Project/rvck/commit/e88d444f070eb0c3f2d0cd36703cf4908557f3e6) | 2026-01-25 | Deepak Gupta | riscv/mm: update write protect to work on shadow stacks |
| [71b21cd0](https://github.com/RVCK-Project/rvck/commit/71b21cd02f04461ad1acc61602fb7b6c9b0374c4) | 2026-01-25 | Deepak Gupta | riscv/mm: teach pte_mkwrite to manufacture shadow stack PTEs |
| [e2631678](https://github.com/RVCK-Project/rvck/commit/e2631678f603707480ffe7b54cc3424cb634e5af) | 2026-01-25 | Deepak Gupta | riscv/mm: manufacture shadow stack ptes |
| [4bc17876](https://github.com/RVCK-Project/rvck/commit/4bc178764e20f8d1b7f557c5da4e8bae13b31dc8) | 2026-01-25 | Deepak Gupta | riscv/mm: ensure PROT_WRITE leads to VM_READ \| VM_WRITE |
| [c5603344](https://github.com/RVCK-Project/rvck/commit/c56033442b761375dc5e4ebbe967f1535a5aa875) | 2026-01-25 | Deepak Gupta | riscv: Add usercfi state for task and save/restore of CSR_SSP on trap entry/exit |
| [bc0a21ef](https://github.com/RVCK-Project/rvck/commit/bc0a21efc0b5cfdc6124f52544962529ea640a1c) | 2026-01-25 | Deepak Gupta | riscv: add Zicfiss / Zicfilp extension CSR and bit definitions |
| [05dbea54](https://github.com/RVCK-Project/rvck/commit/05dbea542f5a52b8941422d017fe2a2bbfd46ab2) | 2026-01-25 | Deepak Gupta | riscv: zicfiss / zicfilp enumeration |
| [132bc4d9](https://github.com/RVCK-Project/rvck/commit/132bc4d9a48dd7bb03f187f6e6aed92c3b5ef145) | 2026-01-25 | Deepak Gupta | dt-bindings: riscv: document zicfilp and zicfiss in extensions.yaml |
| [90cafb00](https://github.com/RVCK-Project/rvck/commit/90cafb0027dba0368dbf1c625bafd22d07dfa9e3) | 2026-01-25 | Deepak Gupta | mm: add VM_SHADOW_STACK definition for riscv |
| [3f30340a](https://github.com/RVCK-Project/rvck/commit/3f30340a2ca4035d32a735e52b43136debfc3fb1) | 2024-04-27 | Masahiro Yamada | kbuild: use $(obj)/ instead of $(src)/ for common pattern rules |
| [60af0da6](https://github.com/RVCK-Project/rvck/commit/60af0da6614abf0ffee2e82605c7978cfce16f73) | 2024-03-13 | Vladimir Isaev | riscv: hwprobe: do not produce frtace relocation |
| [eae93e79](https://github.com/RVCK-Project/rvck/commit/eae93e79e20256b68eedf27549724597af0edc52) | 2025-03-20 | Charlie Jenkins | riscv: entry: Split ret_from_fork() into user and kernel |
| [3223ed27](https://github.com/RVCK-Project/rvck/commit/3223ed2789190089c9e1d9974c0e099b760e1345) | 2025-03-20 | Charlie Jenkins | riscv: entry: Convert ret_from_fork() to C |
| [0dbbad77](https://github.com/RVCK-Project/rvck/commit/0dbbad7711571baa48b063e0a3b1ef5f7f29285e) | 2025-04-11 | Xi Ruoyao | RISC-V: vDSO: Wire up getrandom() vDSO implementation |
| [b4431e5b](https://github.com/RVCK-Project/rvck/commit/b4431e5b7088356ea9d2326fc1dd59fe197dcb09) | 2024-08-22 | Christophe Leroy | random: vDSO: add missing c-getrandom-y in Makefile |
| [c47a6451](https://github.com/RVCK-Project/rvck/commit/c47a64512b18dac9b5d4f0464310da5872c2f215) | 2025-11-12 | Andy Chiu | riscv: signal: abstract header saving for setup_sigcontext |
| [ab67c956](https://github.com/RVCK-Project/rvck/commit/ab67c956fe26d3df487746e12f3ca417411e7df7) | 2023-09-14 | Sohil Mehta | arch: Reserve map_shadow_stack() syscall number for all architectures |
| [6846f896](https://github.com/RVCK-Project/rvck/commit/6846f89608d79d9eaf5caaa8e29a25dec69d704c) | 2026-06-04 | ZhenXing Zhu | soc: thead: adapt AON drivers to new th1520-aon protocol API |
| [5efe184d](https://github.com/RVCK-Project/rvck/commit/5efe184d95344ca6c9ec8da6f67937f4cf3215ef) | 2026-06-04 | ZhenXing Zhu | drivers/soc/event: Add THEAD TH1520 event driver |
| [501c72c1](https://github.com/RVCK-Project/rvck/commit/501c72c17128bfd5865a6ca551d1bbc2230f18ac) | 2026-06-04 | ZhenXing Zhu | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [a2ad3e31](https://github.com/RVCK-Project/rvck/commit/a2ad3e312dc88ece566171333a46c6d3ef5f3c66) | 2026-06-03 | ZhenXing Zhu | riscv: dts: th1520: add watchdog nodes for reboot support |
| [431ea72b](https://github.com/RVCK-Project/rvck/commit/431ea72b80ecff344e2b76e2ee0fd0ba899d311b) | 2026-06-03 | ZhenXing Zhu | riscv: th1520: lpi4a: fix fan not spinning |
| [d9171b6c](https://github.com/RVCK-Project/rvck/commit/d9171b6c088699dec16aac29a45d9173381fdd7c) | 2026-06-03 | ZhenXing Zhu | riscv: defconfig: th1520: fix DWMAC config symbol for ethernet |
| [da36c362](https://github.com/RVCK-Project/rvck/commit/da36c3629c970935545f4c118f19354889f8e26b) | 2024-04-19 | Robin Murphy | dma-mapping: Simplify arch_setup_dma_ops() |
| [c5042f92](https://github.com/RVCK-Project/rvck/commit/c5042f92ef1ec403a88b5344b2692841f95bc814) | 2024-04-19 | Robin Murphy | iommu/dma: Centralise iommu_setup_dma_ops() |
| [64e39a54](https://github.com/RVCK-Project/rvck/commit/64e39a54a549c58527aa26042bd65b73adedb797) | 2024-04-19 | Robin Murphy | iommu/dma: Make limit checks self-contained |
| [dd5b57bc](https://github.com/RVCK-Project/rvck/commit/dd5b57bc3c90879d80c6b69602f2169d266cf680) | 2024-04-19 | Robin Murphy | dma-mapping: Add helpers for dma_range_map bounds |
| [3eb63233](https://github.com/RVCK-Project/rvck/commit/3eb63233791e1f882cd5ddb0ea507ddd7edf90ac) | 2024-04-19 | Robin Murphy | ACPI/IORT: Handle memory address size limits as limits |
| [a239338d](https://github.com/RVCK-Project/rvck/commit/a239338dcfbd9b946dadf0c6cae166eb54cc8a55) | 2024-04-19 | Robin Murphy | OF: Simplify DMA range calculations |
| [8e26227c](https://github.com/RVCK-Project/rvck/commit/8e26227ccdd23b318c30d8dc83e1bf558669948e) | 2024-04-19 | Robin Murphy | OF: Retire dma-ranges mask workaround |
| [6736261f](https://github.com/RVCK-Project/rvck/commit/6736261f7331e28177d2bf5316fcd5d43cb629d4) | 2024-01-22 | Haibo Xu | KVM: selftests: Add CONFIG_64BIT definition for the build |
| [ba780955](https://github.com/RVCK-Project/rvck/commit/ba7809557bbb0001da7c2a4568a051ce512489e7) | 2024-06-03 | Andrew Jones | KVM: selftests: Fix RISC-V compilation |
| [6c60cae4](https://github.com/RVCK-Project/rvck/commit/6c60cae4ffff69cd4a31f4730fc1b3854285c36f) | 2026-05-29 | ZhenXing Zhu | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [ed618326](https://github.com/RVCK-Project/rvck/commit/ed61832625ab175e8732ed091dff6df379dabe66) | 2026-05-29 | ZhenXing Zhu | Revert "KVM: riscv: selftests: Move sbi definitions to its own header file" |
| [65ecff54](https://github.com/RVCK-Project/rvck/commit/65ecff542f4b605ce27d7de45f5943e2226cd455) | 2025-06-26 | Michal Wilczynski | riscv: dts: thead: th1520: Add GPU clkgen reset to AON node |
| [fc818a8f](https://github.com/RVCK-Project/rvck/commit/fc818a8f18d48ad48c05f408b1b8d0ccbd62dd3e) | 2025-03-03 | Michal Wilczynski | reset: thead: Add TH1520 reset controller driver |
| [252b7f7b](https://github.com/RVCK-Project/rvck/commit/252b7f7b7f14b242271c4dd5cb8ba709ddf6c199) | 2025-03-03 | Michal Wilczynski | dt-bindings: reset: Add T-HEAD TH1520 SoC Reset Controller |
| [8b256800](https://github.com/RVCK-Project/rvck/commit/8b2568006d9f90131c0d7d37012dd770ec7f14ad) | 2026-03-12 | ZhenXing Zhu | Revert "dt-bindings: reset: Document th1520 reset control" |
| [e2b87919](https://github.com/RVCK-Project/rvck/commit/e2b87919e0a1bca6a78707072c57b9aeb23d8c52) | 2026-03-12 | ZhenXing Zhu | Revert "reset: Add th1520 reset driver support" |
| [00a65e85](https://github.com/RVCK-Project/rvck/commit/00a65e8569e7191c131efbf22ebfbf0d2bee47b1) | 2026-03-12 | ZhenXing Zhu | Revert "reset: th1520: to support npu/fce reset feature" |
| [860ad353](https://github.com/RVCK-Project/rvck/commit/860ad35393102291c2498c4cf08da782794646a2) | 2026-04-20 | ZhenXing Zhu | riscv: dts: thead: Fix aon node for OpenSBI compatibility |
| [2d505979](https://github.com/RVCK-Project/rvck/commit/2d505979a62baa1e679b17ba788b880101920ee1) | 2026-04-17 | ZhenXing Zhu | clk: Kconfig: Restore thead clock driver Kconfig inclusion |
| [15610ac7](https://github.com/RVCK-Project/rvck/commit/15610ac7e19876312139ede3665d65e4c48ec990) | 2026-03-12 | ZhenXing Zhu | Revert "riscv:dts:thead: Add TH1520 event and watchdog device node" |
| [22308bb9](https://github.com/RVCK-Project/rvck/commit/22308bb93506797a8b4e99f49afe9b124180f42a) | 2026-03-12 | ZhenXing Zhu | Revert "i2s: add i2s driver for XuanTie TH1520 SoC" |
| [f702c94d](https://github.com/RVCK-Project/rvck/commit/f702c94dc4d4d21bdf59c60d9ccc028c05d27453) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: add tdm driver for XuanTie TH1520 SoC" |
| [eab93bc6](https://github.com/RVCK-Project/rvck/commit/eab93bc64b33d7f6f2d9baf6c640915c26bd30fe) | 2026-03-12 | ZhenXing Zhu | Revert "dts: audio: to support i2s-8ch feature" |
| [abfa43c2](https://github.com/RVCK-Project/rvck/commit/abfa43c2afd95249cb28517af32ab1a7c14643e8) | 2026-03-12 | ZhenXing Zhu | Revert "audio: th1520: to support tdm/spdif feature" |
| [fa24233f](https://github.com/RVCK-Project/rvck/commit/fa24233fd860db1540b193f55154392321af9291) | 2025-02-19 | Michal Wilczynski | riscv: dts: thead: Introduce power domain nodes with aon firmware |
| [befba4aa](https://github.com/RVCK-Project/rvck/commit/befba4aac5da8bf38b1aedef6768f36288a4d328) | 2024-02-08 | Krzysztof Kozlowski | pmdomain: core: constify of_phandle_args in xlate |
| [13d941ee](https://github.com/RVCK-Project/rvck/commit/13d941eeee5de3dd0fec65e73c78bf4b397f29c2) | 2025-03-11 | Michal Wilczynski | dt-bindings: power: Add TH1520 SoC power domains |
| [bea43d41](https://github.com/RVCK-Project/rvck/commit/bea43d41a3941ecadddf6b76487e508d75f0b626) | 2023-09-11 | Ulf Hansson | pmdomain: Prepare to move Kconfig files into the pmdomain subsystem |
| [e0ea75a3](https://github.com/RVCK-Project/rvck/commit/e0ea75a3816e557cdd3dc2715fb3ab43271f7ccc) | 2025-03-14 | Arnd Bergmann | pmdomain: thead: fix TH1520_AON_PROTOCOL dependency |
| [fd8a8d50](https://github.com/RVCK-Project/rvck/commit/fd8a8d50ff1c67ed6cd0b2b03cd207b6fa75b6f6) | 2025-03-11 | Michal Wilczynski | pmdomain: thead: Add power-domain driver for TH1520 |
| [9064f714](https://github.com/RVCK-Project/rvck/commit/9064f714626e58ed07d2960644e7d319967678c8) | 2025-03-11 | Michal Wilczynski | firmware: thead: Add AON firmware protocol driver |
| [912c7958](https://github.com/RVCK-Project/rvck/commit/912c7958e17cddbeed8e3b1611091757d33c4100) | 2026-03-11 | ZhenXing Zhu | Revert "regdump:add regdump support for lpi4a and light-a && rename some dts nam... |
| [c3103fb3](https://github.com/RVCK-Project/rvck/commit/c3103fb34938cf790a8e9901725bd2a0b1ea63aa) | 2026-03-11 | ZhenXing Zhu | Revert "drivers/soc/event: Add THEAD TH1520 event driver" |
| [2a1b0ae3](https://github.com/RVCK-Project/rvck/commit/2a1b0ae3efac51c96b4315a701080a6e084e3f1f) | 2026-03-11 | ZhenXing Zhu | Revert "add c906 audio support" |
| [3725773d](https://github.com/RVCK-Project/rvck/commit/3725773dbb49ae360833fa5edb498824d95bbb92) | 2026-03-11 | ZhenXing Zhu | Revert "drivers: regulator: add th1520 AON virtual regulator control support." |
| [3e10e548](https://github.com/RVCK-Project/rvck/commit/3e10e5487cf695da0fee2632b472c5c6b375477c) | 2026-03-10 | ZhenXing Zhu | Revert "drivers/watchdog: Add THEAD TH1520 pmic watchdog driver" |
| [bfc2b170](https://github.com/RVCK-Project/rvck/commit/bfc2b17063201f99b2a815a9bfb3fc78f279ecb2) | 2026-03-10 | ZhenXing Zhu | Revert "firmware: thead: c910_aon: add th1520 Aon protocol driver" |
| [ff8a5a9a](https://github.com/RVCK-Project/rvck/commit/ff8a5a9aefabf0597c78007fd568353043087f28) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: cpufreq: add cpufreq driver." |
| [ab94b0a5](https://github.com/RVCK-Project/rvck/commit/ab94b0a5c6addddf264acf9a33815acb6a489b3b) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to modify rvbook dts" |
| [889cb9a2](https://github.com/RVCK-Project/rvck/commit/889cb9a259ec4d03d5ea3ec2197aba02697e2b12) | 2026-03-10 | ZhenXing Zhu | Revert "audio: th1520: add soundcard dts node of th1520-a-val board" |
| [c385bb07](https://github.com/RVCK-Project/rvck/commit/c385bb07f10bb9a4edad4faf58c72765f64919ae) | 2026-03-10 | ZhenXing Zhu | Revert "dts:th1520-a: add th1520-a-val.dts and th1520-a-val-sec.dts" |
| [0265d4c1](https://github.com/RVCK-Project/rvck/commit/0265d4c19d1ba28760e6bd8faf7b9a6d2a19d41c) | 2026-03-10 | ZhenXing Zhu | Revert "drivers: pmdomain: support th1520 Power domain control." |
| [1324405f](https://github.com/RVCK-Project/rvck/commit/1324405f37fa09bfe9b61e7afb348cc868ff9770) | 2026-03-10 | ZhenXing Zhu | Revert "dts: add GPU device node" |
| [d88fde9a](https://github.com/RVCK-Project/rvck/commit/d88fde9a0535d5832caf8a1eb04e2a8cc3acc9db) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [e5289bbe](https://github.com/RVCK-Project/rvck/commit/e5289bbe45fc2ec9ea09e123f0c25f1a7802a42e) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add npu device node" |
| [36c24251](https://github.com/RVCK-Project/rvck/commit/36c24251c3d70548f62e8b8b6414a74a074f25e2) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: to add npu device node" |
| [346ff48d](https://github.com/RVCK-Project/rvck/commit/346ff48d83c463af483e696c102b117011ecb800) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add th1520-a-val-crash.dts and th1520-lpi4a-product-crash.d... |
| [5b772695](https://github.com/RVCK-Project/rvck/commit/5b77269530c59480605001432d3873fdc7526ede) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add cpu thermal node and device thermal node" |
| [1694012d](https://github.com/RVCK-Project/rvck/commit/1694012d74a79c1b3878d8955c222359eb0e6c5e) | 2026-03-10 | ZhenXing Zhu | Revert "dtb:lipi:enable VI module config" |
| [7c508039](https://github.com/RVCK-Project/rvck/commit/7c5080391c1cc71b63e0a96764364fe7af1f873f) | 2026-03-10 | ZhenXing Zhu | Revert "dts: th1520: add vdec venc and video mem device node" |
| [33d7c7b3](https://github.com/RVCK-Project/rvck/commit/33d7c7b3577473cf6bc0c0c94c4974708637f1d7) | 2026-03-10 | ZhenXing Zhu | Revert "chore: use xuantie instead of thead" |
| [afe1dc19](https://github.com/RVCK-Project/rvck/commit/afe1dc1958dd547da4e6a64e288042bb5425be33) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce NAPOT field to PTDUMP |
| [9c4cf898](https://github.com/RVCK-Project/rvck/commit/9c4cf8989a1dd08d3dc424bbd3d42e5ee19c202c) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce PBMT field to PTDUMP |
| [dc1e9ab3](https://github.com/RVCK-Project/rvck/commit/dc1e9ab3b86f13e60c8395c52acfaf6017fd5b9e) | 2023-09-21 | Yu Chien Peter Lin | riscv: Improve PTDUMP to show RSW with non-zero value |
| [11471071](https://github.com/RVCK-Project/rvck/commit/11471071da42b8ec238f59c381a248826c97187e) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add commandline option for SBI PMU test |
| [02529c38](https://github.com/RVCK-Project/rvck/commit/02529c38d52a825395b2750134f0b99d26595f5a) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for counter overflow |
| [50bd54db](https://github.com/RVCK-Project/rvck/commit/50bd54db27faf317bce6ff6183dd99c4712df40d) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for PMU snapshot functionality |
| [2b22b293](https://github.com/RVCK-Project/rvck/commit/2b22b2936279d58eb21cef2187b52a4a1bde3d74) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU selftest |
| [c8bca2ce](https://github.com/RVCK-Project/rvck/commit/c8bca2ce76ea06d968afba1c33ca996cbc677444) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU extension definitions |
| [53ba04c8](https://github.com/RVCK-Project/rvck/commit/53ba04c8e9233d3ef20c520488a61317cb826a84) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add Sscofpmf to get-reg-list test |
| [c34f8d3c](https://github.com/RVCK-Project/rvck/commit/c34f8d3c4dfec1063f8067dbae2fa4087ab1449c) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add helper functions for extension checks |
| [3d0b723f](https://github.com/RVCK-Project/rvck/commit/3d0b723fb173ae54fa9e0197bc58d20567170c58) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [e3370c6a](https://github.com/RVCK-Project/rvck/commit/e3370c6abf11838f612a648fad68948d5ded1444) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add sstc timer test |
| [39035a30](https://github.com/RVCK-Project/rvck/commit/39035a3083356c511a145420ec2b67396503e74a) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Change vcpu_has_ext to a common function |
| [d303eddb](https://github.com/RVCK-Project/rvck/commit/d303eddbe08ed8f8660e70bb484399aa54029e68) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add guest helper to get vcpu id |
| [c428267e](https://github.com/RVCK-Project/rvck/commit/c428267e6d1e8feec65f83d7473a9b26f0de0ed7) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add exception handling support |
| [5e445613](https://github.com/RVCK-Project/rvck/commit/5e4456139d0a0854b4bdd1a50ab451f00c44ffec) | 2024-01-22 | Haibo Xu | KVM: arm64: selftests: Split arch_timer test code |
| [1c968ed0](https://github.com/RVCK-Project/rvck/commit/1c968ed0f1ad4d839db7cfa6afe2448ce701f4db) | 2024-01-22 | Paolo Bonzini | selftests/kvm: Fix issues with $(SPLIT_TESTS) |
| [8da65445](https://github.com/RVCK-Project/rvck/commit/8da654456f1db484ffbbdb503fe9ead235fbc7dd) | 2024-04-20 | Atish Patra | RISC-V: KVM: Improve firmware counter read function |
| [12959bd7](https://github.com/RVCK-Project/rvck/commit/12959bd78ebad31e368d1487051e1729e82629e4) | 2024-04-20 | Atish Patra | RISC-V: KVM: Support 64 bit firmware counters on RV32 |
| [65ab7407](https://github.com/RVCK-Project/rvck/commit/65ab74076aa17fc4c664a627894edcf3d5b7b465) | 2024-04-20 | Atish Patra | RISC-V: KVM: Add perf sampling support for guests |
| [b3523ab0](https://github.com/RVCK-Project/rvck/commit/b3523ab0d33a65149752e66bf9afa5d1c6f1197d) | 2024-04-20 | Atish Patra | RISC-V: KVM: Implement SBI PMU Snapshot feature |
| [f57ff154](https://github.com/RVCK-Project/rvck/commit/f57ff154bc37555309bea2c8502460c4b54edaa0) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to exit to the user space if perf event failed |
| [75509349](https://github.com/RVCK-Project/rvck/commit/75509349cceecc52f8621dee1797b23f24d9711f) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to update the counter value during reset |
| [1cbac2cf](https://github.com/RVCK-Project/rvck/commit/1cbac2cf70bde2dfd4d8901c7bb8d8f62b4711cb) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Implement SBI PMU snapshot function |
| [12eaeea1](https://github.com/RVCK-Project/rvck/commit/12eaeea1becece835abb6bef1d36ac4022cc5874) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Fix counter mask iteration for RV32 |
| [d54c9858](https://github.com/RVCK-Project/rvck/commit/d54c9858ba1bc55972f22fd3eecc55a57bee6047) | 2024-04-20 | Atish Patra | RISC-V: Use the minor version mask while computing sbi version |
| [f8985844](https://github.com/RVCK-Project/rvck/commit/f8985844fc094f41de79dff46e0a744eac618070) | 2024-04-20 | Atish Patra | RISC-V: KVM: Rename the SBI_STA_SHMEM_DISABLE to a generic name |
| [83f5234d](https://github.com/RVCK-Project/rvck/commit/83f5234d64a5cbb18dfef9642c0a0c23c70829bb) | 2024-04-20 | Atish Patra | RISC-V: Add SBI PMU snapshot definitions |
| [8f294cf3](https://github.com/RVCK-Project/rvck/commit/8f294cf3bb33e4db4cffcd65e9179b0e04d1f371) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Use BIT macro for shifting operations |
| [c7b637c5](https://github.com/RVCK-Project/rvck/commit/c7b637c59ab8de0444183f0b2c42f1a79c537822) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Read upper bits of a firmware counter |
| [328cbbb5](https://github.com/RVCK-Project/rvck/commit/328cbbb5f08615f42c03135f3ab7982b2e0ff4ce) | 2024-04-20 | Atish Patra | RISC-V: Add FIRMWARE_READ_HI definition |
| [5f6d65fe](https://github.com/RVCK-Project/rvck/commit/5f6d65fe9083dee6d8752f6233ced5efdbe6bc7a) | 2024-04-20 | Atish Patra | RISC-V: Fix the typo in Scountovf CSR name |
| [45955d81](https://github.com/RVCK-Project/rvck/commit/45955d8115600091f7cd1349303fadc1b76cc464) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add get-reg-list test for STA registers |
| [0555966a](https://github.com/RVCK-Project/rvck/commit/0555966a3d7044d0ef812139f0160ba58ee22767) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add steal_time test support |
| [5620df53](https://github.com/RVCK-Project/rvck/commit/5620df53584315d5488d6e389fd92b589eae0da7) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add guest_sbi_probe_extension |
| [12a3556e](https://github.com/RVCK-Project/rvck/commit/12a3556e01ab0e0599d78761a8cf0494f1480a16) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Move sbi_ecall to processor.c |
| [3a74da4e](https://github.com/RVCK-Project/rvck/commit/3a74da4e7a52782d0c620de0172f073e3de8824a) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Implement SBI STA extension |
| [cf86788e](https://github.com/RVCK-Project/rvck/commit/cf86788ea734e757fdcbd15cf0e0ea469378b89a) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI STA registers |
| [97a50342](https://github.com/RVCK-Project/rvck/commit/97a503420fee65fc2a695643d7310a5da4c9db2c) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI extension registers |
| [25a80197](https://github.com/RVCK-Project/rvck/commit/25a8019734fd3c00529403f3e4a255626f6921b2) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA info to vcpu_arch |
| [6dded146](https://github.com/RVCK-Project/rvck/commit/6dded1461fe336530a0d3bd02f8888b2ce62fa28) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add steal-update vcpu request |
| [d2ecbb44](https://github.com/RVCK-Project/rvck/commit/d2ecbb44c629f9eec1d7fa5cf1e863fb9f813039) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA extension skeleton |
| [cc4c3626](https://github.com/RVCK-Project/rvck/commit/cc4c362618e9ee4ffcf857ea8a1d8d935607b306) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Implement steal-time support |
| [f1dffa8a](https://github.com/RVCK-Project/rvck/commit/f1dffa8a92177ab85961c379cc7966eb688315ce) | 2023-12-20 | Andrew Jones | RISC-V: Add SBI STA extension definitions |
| [100f7087](https://github.com/RVCK-Project/rvck/commit/100f70875f7cc0c032c615c55a13b9f8b8dc3f04) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Add skeleton for pv-time support |
| [0588fea2](https://github.com/RVCK-Project/rvck/commit/0588fea21e8a51058064901ef5d79bc3d25a31d9) | 2023-12-13 | Andrew Jones | KVM: riscv: selftests: Add RISCV_SBI_EXT_REG |
| [99e034d3](https://github.com/RVCK-Project/rvck/commit/99e034d39a20a60f22ecdf238da73e23c01257b1) | 2023-12-13 | Andrew Jones | RISC-V: KVM: Make SBI uapi consistent with ISA uapi |
| [0ad311bc](https://github.com/RVCK-Project/rvck/commit/0ad311bca44bb0444c5b284befa350055aead421) | 2023-11-28 | Anup Patel | KVM: riscv: selftests: Generate ISA extension reg_list using macros |
| [f6df18f3](https://github.com/RVCK-Project/rvck/commit/f6df18f350384fa9996986ce0a85b85187b65958) | 2023-10-19 | Thomas Huth | KVM: selftests: Use TAP in the steal_time test |
| [eabca2c2](https://github.com/RVCK-Project/rvck/commit/eabca2c2007f592df49e2c198ebbbc8e79ecf121) | 2023-09-20 | Andrew Jones | KVM: riscv: selftests: get-reg-list print_reg should never fail |
| [9276f4db](https://github.com/RVCK-Project/rvck/commit/9276f4dbb38f14dd8b6a12977b557c3d47f8bd10) | 2023-08-17 | Andrew Jones | KVM: selftests: Add array order helpers to riscv get-reg-list |
| [ac3a510e](https://github.com/RVCK-Project/rvck/commit/ac3a510e32cf6cdb2eee1766d12270a009177a91) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Switch to use macro from csr.h |
| [fbd35a9f](https://github.com/RVCK-Project/rvck/commit/fbd35a9f3844e573a57af2a1c8e0b9aa2f678486) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file vdso/processor.h |
| [eb5a8009](https://github.com/RVCK-Project/rvck/commit/eb5a8009d77efe0e7f502e345fc08c4fd4bb493b) | 2024-01-22 | Haibo Xu | tools: riscv: Add header file csr.h |
| [f38e0d7c](https://github.com/RVCK-Project/rvck/commit/f38e0d7c48b647814bcd10489aa2140e8c8dad4b) | 2024-11-08 | Charlie Jenkins | riscv: Fix default misaligned access trap |
| [0f542113](https://github.com/RVCK-Project/rvck/commit/0f542113fafbc24f75fef8e7cd571fbf98445fd9) | 2025-12-06 | Eric Biggers | lib/crypto: riscv: Depend on RISCV_EFFICIENT_VECTOR_UNALIGNED_ACCESS |
| [c9768c0c](https://github.com/RVCK-Project/rvck/commit/c9768c0c7bd443392ade72503a974a193b4bfeac) | 2025-03-04 | Andrew Jones | Documentation/kernel-parameters: Add riscv unaligned speed parameters |
| [fcc79b25](https://github.com/RVCK-Project/rvck/commit/fcc79b253230bd45194ae86a5484314f9e807d88) | 2025-03-04 | Andrew Jones | riscv: Add parameter for skipping access speed tests |
| [eed9cbd7](https://github.com/RVCK-Project/rvck/commit/eed9cbd7a3de5e655e1ebc2b9168dc9ce0a897c7) | 2025-03-04 | Andrew Jones | riscv: Fix set up of vector cpu hotplug callback |
| [6e3e3a02](https://github.com/RVCK-Project/rvck/commit/6e3e3a02136e6329ce24fdb0880027d7980aa1af) | 2025-03-04 | Andrew Jones | riscv: Fix set up of cpu hotplug callbacks |
| [2afe0e51](https://github.com/RVCK-Project/rvck/commit/2afe0e51a9b465ffa0acb8e8b43ccff9462a2a6b) | 2025-03-04 | Andrew Jones | riscv: Change check_unaligned_access_speed_all_cpus to void |
| [aa06ccca](https://github.com/RVCK-Project/rvck/commit/aa06ccca9c4112ee2b4f745fef8373d563577643) | 2025-03-04 | Andrew Jones | riscv: Fix check_unaligned_access_all_cpus |
| [c1047190](https://github.com/RVCK-Project/rvck/commit/c10471900d5d00b239f07c8931f4764525a9b717) | 2025-03-04 | Andrew Jones | riscv: Fix riscv_online_cpu_vec |
| [0a53f771](https://github.com/RVCK-Project/rvck/commit/0a53f7712c8332d94e9050b1ec104daa8e90e6cf) | 2025-03-04 | Andrew Jones | riscv: Annotate unaligned access init functions |
| [d4e0db4c](https://github.com/RVCK-Project/rvck/commit/d4e0db4c75dedc996679d9205e2ffc971d52f4a1) | 2024-10-17 | Jesse Taube | RISC-V: hwprobe: Document unaligned vector perf key |
| [5b3f54be](https://github.com/RVCK-Project/rvck/commit/5b3f54bee1352c2829645de29cd22b7dfce93efe) | 2024-10-17 | Jesse Taube | RISC-V: Report vector unaligned access speed hwprobe |
| [652912c0](https://github.com/RVCK-Project/rvck/commit/652912c006ce00e2be678955abf57752566f0ccf) | 2024-10-17 | Jesse Taube | RISC-V: Detect unaligned vector accesses supported |
| [5d20efc6](https://github.com/RVCK-Project/rvck/commit/5d20efc6f0f6ea5331da4d1900f91d7dec6c2cc7) | 2024-10-17 | Jesse Taube | RISC-V: Replace RISCV_MISALIGNED with RISCV_SCALAR_MISALIGNED |
| [cda40a32](https://github.com/RVCK-Project/rvck/commit/cda40a32ca2a367807091c23cbd4a9d348273236) | 2024-10-17 | Jesse Taube | RISC-V: Scalar unaligned access emulated on hotplug CPUs |
| [ed69d4e9](https://github.com/RVCK-Project/rvck/commit/ed69d4e9ab774085c3b6b60426f9634f66164c90) | 2024-10-17 | Jesse Taube | RISC-V: Check scalar unaligned access on all CPUs |
| [9a80450d](https://github.com/RVCK-Project/rvck/commit/9a80450d8d944c928e7499f55da8b6fe0863b82e) | 2024-08-14 | Samuel Holland | riscv: misaligned: Restrict user access to kernel memory |
| [d6e3d13a](https://github.com/RVCK-Project/rvck/commit/d6e3d13a090bbd869efece0ee29b15f8e46910db) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add SCALAR to misaligned perf defines |
| [b2068f6e](https://github.com/RVCK-Project/rvck/commit/b2068f6e81bb228dc146d42ad84ad12753fece14) | 2024-08-09 | Evan Green | RISC-V: hwprobe: Add MISALIGNED_PERF key |
| [f845902b](https://github.com/RVCK-Project/rvck/commit/f845902ba50d490a44e4368e5a5cc9388ca48164) | 2024-03-17 | Xingyou Chen | riscv: typo in comment for get_f64_reg |
| [207d2079](https://github.com/RVCK-Project/rvck/commit/207d2079cfea5b983394dd8b5934971ba832e43b) | 2024-03-08 | Charlie Jenkins | riscv: Set unaligned access speed at compile time |
| [a9f9c422](https://github.com/RVCK-Project/rvck/commit/a9f9c4224f5532a322dcc2e8ab84466c6a8ce782) | 2024-03-08 | Charlie Jenkins | riscv: Decouple emulated unaligned accesses from access speed |
| [601a6fb6](https://github.com/RVCK-Project/rvck/commit/601a6fb61ec1cbbd8156cc552ec5f0ff78a13103) | 2024-03-08 | Charlie Jenkins | riscv: Only check online cpus for emulated accesses |
| [f8d6c1eb](https://github.com/RVCK-Project/rvck/commit/f8d6c1eb87dced2bc33f415534062c447ccf37df) | 2024-03-08 | Charlie Jenkins | riscv: lib: Introduce has_fast_unaligned_access() |
| [e6085bc9](https://github.com/RVCK-Project/rvck/commit/e6085bc9fb37c307f1ac32482c93c112093a3376) | 2024-02-12 | Eric Biggers | crypto: riscv - add vector crypto accelerated AES-CBC-CTS |
| [82e3724d](https://github.com/RVCK-Project/rvck/commit/82e3724db350e5f0c23b75805354db1839a24f5a) | 2024-02-06 | Clément Léger | riscv: misaligned: remove CONFIG_RISCV_M_MODE specific code |
| [3ee91725](https://github.com/RVCK-Project/rvck/commit/3ee917256cab97ca7282607088bcee912663ee3f) | 2024-01-08 | Charlie Jenkins | kunit: Add tests for csum_ipv6_magic and ip_fast_csum |
| [a883517f](https://github.com/RVCK-Project/rvck/commit/a883517f6b5aefdfcc9ba4f2552822e22b86f81c) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum library |
| [d89f9eea](https://github.com/RVCK-Project/rvck/commit/d89f9eea204badecb8bbf48b71761d07d917cc0e) | 2024-01-08 | Charlie Jenkins | riscv: Add checksum header |
| [98a3488f](https://github.com/RVCK-Project/rvck/commit/98a3488fdd7c0d34a16ceb57438d1f9546c398f2) | 2024-01-08 | Charlie Jenkins | riscv: Add static key for misaligned accesses |
| [439fc04e](https://github.com/RVCK-Project/rvck/commit/439fc04ec58464cca4fa7bc06c44d92bb6d0b110) | 2024-01-08 | Charlie Jenkins | asm-generic: Improve csum_fold |
| [a8bd550f](https://github.com/RVCK-Project/rvck/commit/a8bd550fa9582abce18cdc15ed1c5502172426dc) | 2023-12-25 | Jisheng Zhang | riscv: select DCACHE_WORD_ACCESS for efficient unaligned access HW |
| [659c85cc](https://github.com/RVCK-Project/rvck/commit/659c85cccc4f79e9b686baad7dd595cf8b456c89) | 2023-12-25 | Jisheng Zhang | riscv: introduce RISCV_EFFICIENT_UNALIGNED_ACCESS |
| [4c688530](https://github.com/RVCK-Project/rvck/commit/4c688530c1c7fb0a214adf691a595a3077f4132c) | 2023-11-23 | Ben Dooks | riscv; fix __user annotation in save_v_state() |
| [0126a957](https://github.com/RVCK-Project/rvck/commit/0126a957c1d0649654f12c8519d359f2fbcd3cb2) | 2023-11-23 | Ben Dooks | riscv: fix __user annotation in traps_misaligned.c |
| [803c80d1](https://github.com/RVCK-Project/rvck/commit/803c80d1251d234ff9fe58b649d1995007998483) | 2023-11-06 | Evan Green | RISC-V: Show accurate per-hart isa in /proc/cpuinfo |
| [24dcaca6](https://github.com/RVCK-Project/rvck/commit/24dcaca6e769dbe8a2417333df9792ba006703eb) | 2026-02-02 | Chen Pei | Revert "riscv:uprobe: fix flush_icache to ensure that instructions are refreshed... |
| [7f6fa412](https://github.com/RVCK-Project/rvck/commit/7f6fa4120363d5c4ed7c1ccab7ff115ed1872db4) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM4 |
| [643a0330](https://github.com/RVCK-Project/rvck/commit/643a033028d8e17c9e694074b9fa97aa3f66886c) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SM3 |
| [62b355e0](https://github.com/RVCK-Project/rvck/commit/62b355e09252dc62d627c05fe22b717f3b80ba5d) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{512,384} |
| [a871bdac](https://github.com/RVCK-Project/rvck/commit/a871bdace697ec0b9480253540c45e86a6530642) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated SHA-{256,224} |
| [eb8be685](https://github.com/RVCK-Project/rvck/commit/eb8be685cd866340a2c42c36a2cd428bd9ba46ea) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated GHASH |
| [f0996326](https://github.com/RVCK-Project/rvck/commit/f099632699bec121d04065468f98b6d443aaadb0) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated ChaCha20 |
| [4b5dd53c](https://github.com/RVCK-Project/rvck/commit/4b5dd53c0060b74cdae9f1bc1a526fa39d9e8ce5) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated AES-{ECB,CBC,CTR,XTS} |
| [8980a2a9](https://github.com/RVCK-Project/rvck/commit/8980a2a983bec9f98cef018eab07d95cfa0b89ee) | 2024-01-21 | Heiko Stuebner | RISC-V: hook new crypto subdir into build-system |
| [5c29ea00](https://github.com/RVCK-Project/rvck/commit/5c29ea00520085fa4b120e89ae117a4686cadf11) | 2024-01-21 | Eric Biggers | RISC-V: add TOOLCHAIN_HAS_VECTOR_CRYPTO |
| [9a907359](https://github.com/RVCK-Project/rvck/commit/9a90735934adf6e9a44da48f3035d9a95c946298) | 2024-01-21 | Heiko Stuebner | RISC-V: add helper function to read the vector VLEN |
| [c611cacb](https://github.com/RVCK-Project/rvck/commit/c611cacb7aec4dc81b8bfcdf498e6a4960af2e50) | 2023-10-12 | Ian Rogers | perf pmu: Lazily compute default config |
| [5c576909](https://github.com/RVCK-Project/rvck/commit/5c576909620c96b634b8917838fd34f17bf38b67) | 2023-10-12 | Ian Rogers | perf pmu-events: Remember the perf_events_map for a PMU |
| [4e735763](https://github.com/RVCK-Project/rvck/commit/4e735763d1a6aaa97d25324a6432d937f07f0796) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify perf_pmu__config_terms |
| [f4dc9d6b](https://github.com/RVCK-Project/rvck/commit/f4dc9d6b9299d4ae18854900734af3fc62196491) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify file APIs |
| [bfdb5bf1](https://github.com/RVCK-Project/rvck/commit/bfdb5bf1803c6cf1b4067bbf9b8f2f079d1ac630) | 2023-10-12 | Ian Rogers | perf arm-spe: Move PMU initialization from default config code |
| [c03695c0](https://github.com/RVCK-Project/rvck/commit/c03695c0f0b2814587186e31601b5b6547e9dbc3) | 2023-10-12 | Ian Rogers | perf intel-pt: Move PMU initialization from default config code |
| [fde133a4](https://github.com/RVCK-Project/rvck/commit/fde133a46b10ec035e86a0c0c0be334cd1d55945) | 2023-10-12 | Ian Rogers | perf pmu: Rename perf_pmu__get_default_config to perf_pmu__arch_init |
| [5bb7f7e2](https://github.com/RVCK-Project/rvck/commit/5bb7f7e2c23f0abd797b2daf125f0fd66012043e) | 2023-09-24 | Ian Rogers | perf pmus: Make PMU alias name loading lazy |
| [26081f58](https://github.com/RVCK-Project/rvck/commit/26081f58286049faa5372c19cbe6f0e46bd0e39e) | 2023-09-01 | Ian Rogers | perf parse-events: Introduce 'struct parse_events_terms' |
| [9265ac41](https://github.com/RVCK-Project/rvck/commit/9265ac413cf47bdde7269d02fbb0930ca08ce66c) | 2023-09-01 | Ian Rogers | perf parse-events: Copy fewer term lists |
| [7f984843](https://github.com/RVCK-Project/rvck/commit/7f984843110d182da02a16fad91b2600d054fbdb) | 2023-09-01 | Ian Rogers | perf parse-events: Avoid enum casts |
| [82551ec2](https://github.com/RVCK-Project/rvck/commit/82551ec25b0cd340588ad689b0a453f5958bbbc2) | 2023-09-01 | Ian Rogers | perf parse-events: Tidy up str parameter |
| [28b6b8f1](https://github.com/RVCK-Project/rvck/commit/28b6b8f11e8f11bce7689495f925be6bb54dc60b) | 2023-09-01 | Ian Rogers | perf parse-events: Remove unnecessary __maybe_unused |
| [d459e520](https://github.com/RVCK-Project/rvck/commit/d459e520083f384af3d4e1886c20724a755bb83f) | 2024-04-22 | Shenlin Liang | perf kvm/riscv: Port perf kvm stat to RISC-V |
| [d8188138](https://github.com/RVCK-Project/rvck/commit/d81881383c537264c638a64a3e3753bb918a05fd) | 2024-04-22 | Shenlin Liang | RISCV: KVM: add tracepoints for entry and exit events |
| [9d007f38](https://github.com/RVCK-Project/rvck/commit/9d007f3804da3c51219c52f73a209c9882016abd) | 2024-06-21 | Huaming | defconfig:th1520: enable cma config |
| [bbe96410](https://github.com/RVCK-Project/rvck/commit/bbe9641007d7b75ffb24ac17c8c3a96f77faf3c7) | 2024-09-06 | Guo Ren | riscv: mm: Add support for Svinval extension |
| [ad81d523](https://github.com/RVCK-Project/rvck/commit/ad81d52300f682771403dbf3929eebc83c43442f) | 2024-09-04 | Guo Ren | riscv: Add ACLINT SSWI support |
| [e974c07f](https://github.com/RVCK-Project/rvck/commit/e974c07f54f1362a82067ff6d160607d6dd20037) | 2024-07-29 | forain | drm: Fix HDMI hot-plug problem |
| [52cc4ac6](https://github.com/RVCK-Project/rvck/commit/52cc4ac608035d6e0a544b38b0b33b892c16f61f) | 2024-07-25 | Hao Li | gpu/drm: hdmi: Add hdmi debounce to enhance hdmi plugin/out stable |
| [c9a571de](https://github.com/RVCK-Project/rvck/commit/c9a571de0aff6bf3bc1333809418208653360bd1) | 2024-07-21 | David Li | audio: th1520: fixup compile warning of i2s driver |
| [473b147b](https://github.com/RVCK-Project/rvck/commit/473b147b3e12a4b90741a626551e5a6a493875af) | 2024-07-12 | David Li | dmaengine: dw-axi-dmac: Add support for Xuantie TH1520 DMA |
| [a208a2e7](https://github.com/RVCK-Project/rvck/commit/a208a2e7ff3d1aab9766042129f211eda07570d4) | 2024-07-04 | Chen Pei | arch:rsicv:select ARCH_HAS_DMA_WRITE_COMBINE |
| [cbb33753](https://github.com/RVCK-Project/rvck/commit/cbb33753153918ac69b3cf949472455d6cde8f4b) | 2024-07-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [068734ce](https://github.com/RVCK-Project/rvck/commit/068734ce54f9a8078468ddb242dcd8a29b9fe278) | 2024-06-30 | Huaming | driver:padctrl:correct th1520 gpio_1 24/25 cfg |
| [38adb5fd](https://github.com/RVCK-Project/rvck/commit/38adb5fdc5a48fc476b2f25b48ca4d455d3d38eb) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [21b432c4](https://github.com/RVCK-Project/rvck/commit/21b432c4dd45547af70382caf553ac54330a139c) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [db6953eb](https://github.com/RVCK-Project/rvck/commit/db6953eb7cdcf7ab6cf906baa2df270b8b9d63c4) | 2024-07-04 | Xiangyi Zeng | drivers: event: add macro definition to control SW_PANIC event |
| [d802d93e](https://github.com/RVCK-Project/rvck/commit/d802d93e9abeaef140da4bda9f36adc5554b403a) | 2024-06-28 | David Li | audio: th1520: enable soundcard feature |
| [2c7baf8b](https://github.com/RVCK-Project/rvck/commit/2c7baf8b744444ec42dfdf2aaa205db1373dcb4b) | 2024-06-27 | David Li | audio: th1520: support audiosys pinctrl feature |
| [2e2891e5](https://github.com/RVCK-Project/rvck/commit/2e2891e503358c39e4da2fb474455a3bb1e0aaad) | 2024-06-26 | Xiangyi Zeng | dts: th1520: fix interrupt number config error in dts |
| [54294dd6](https://github.com/RVCK-Project/rvck/commit/54294dd648748beeedbd7bfcff8c615bf4e92be0) | 2024-06-24 | forain | DPU: add DPU driver for Lichee-Pi-4A board |
| [d2ee0cad](https://github.com/RVCK-Project/rvck/commit/d2ee0cad8a1f20e21c4e8c9ed7a6b5dd9b26ae54) | 2024-06-23 | tingming | dts: th1520: add npu device node |
| [734c044b](https://github.com/RVCK-Project/rvck/commit/734c044be09020de5a6394916029927783d3f0a3) | 2024-06-21 | David Li | codec: audio: add codec driver for Lichee-Pi-4A board |
| [377591fc](https://github.com/RVCK-Project/rvck/commit/377591fc4055685da7624e440f9e8674d18b7797) | 2024-06-20 | Chen Pei | riscv: vector: Fix the boot issue compiled using xuantie-toolchain or upstream-t... |
| [cba7f21c](https://github.com/RVCK-Project/rvck/commit/cba7f21c910ea359d60cab5d4bdc43c4b4f7bd60) | 2024-06-19 | Esther Z | drivers: cpufreq: add cpufreq driver. |
| [a2817add](https://github.com/RVCK-Project/rvck/commit/a2817addb940a5b31ded925f10ca44af05d862fb) | 2024-06-18 | Esther Z | riscv: dts: Introduce lichee-pi-4a fixed regulator support. |
| [fbd65d00](https://github.com/RVCK-Project/rvck/commit/fbd65d001fbd6f5b833149d0cbf13519aab46483) | 2024-06-17 | zhangye | Enable XUANTIE ISA for memcpy performance |
| [2e39eb70](https://github.com/RVCK-Project/rvck/commit/2e39eb7028570bda1a74f2c78789333bd2a0d381) | 2024-03-27 | Chen Pei | riscv: build: Support compiling kernel using Xuantie toolchain |
| [bb2ba12f](https://github.com/RVCK-Project/rvck/commit/bb2ba12f6e2c5e5c4a80fff9342796cc5920610e) | 2024-06-17 | David Li | i2s: remove debug message |
| [a1e5cb81](https://github.com/RVCK-Project/rvck/commit/a1e5cb81d9fbeeba00146647b0c59843ce04d8de) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the aon gpio range configuration error |
| [b8f60ce1](https://github.com/RVCK-Project/rvck/commit/b8f60ce13736dfa6e9b30e85ff32f60bbc9ca80b) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix spi/qspi1 cs pin duplicate configuration error |
| [77f15cad](https://github.com/RVCK-Project/rvck/commit/77f15cad725db3afad96fde021a9ea7272d477b0) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the gpio range configuration error |
| [c4cc88b0](https://github.com/RVCK-Project/rvck/commit/c4cc88b030a3245e6b8f0199ecd2e21181eb760a) | 2024-06-16 | Esther Z | drivers: regulator: add th1520 AON virtual regulator control support. |
| [b4175222](https://github.com/RVCK-Project/rvck/commit/b4175222587ab0f284572ef8259ec0d6d1a4e647) | 2024-06-16 | Esther Z | dt-bindings: add AON resource id headfile |
| [bb2514e2](https://github.com/RVCK-Project/rvck/commit/bb2514e2ca9d476aba2f0531700f115f1446469f) | 2024-06-16 | Esther Z | drivers: pmdomain: support th1520 Power domain control. |
| [84b13681](https://github.com/RVCK-Project/rvck/commit/84b13681cae158a563d80e8e9a218cead6f8805c) | 2024-06-15 | David Li | i2s: add i2s driver for XuanTie TH1520 SoC |
| [3e05e739](https://github.com/RVCK-Project/rvck/commit/3e05e73930ffd846bf964e19b9e4cad80d175d8b) | 2024-06-15 | David Li | configs: xuantie: correct definition of SoC Architecture |
| [62546cc7](https://github.com/RVCK-Project/rvck/commit/62546cc781f48cd63cce487e24d83ec2e6e61bfa) | 2024-06-11 | lst | i2c: designware: add support for hcnt/lcnt got from dt |
| [da06b5f1](https://github.com/RVCK-Project/rvck/commit/da06b5f1e0edf5dae70a9d9bb18ed03609d2e52f) | 2024-06-06 | Xiangyi Zeng | riscv:dts:thead: Add TH1520 event and watchdog device node |
| [191ab32e](https://github.com/RVCK-Project/rvck/commit/191ab32e1b0c0f1bf805b0d71c00149b1e89f746) | 2024-06-06 | Xiangyi Zeng | dt-bindings:wdt: Add Documentation for THEAD TH1520 pmic watchdog |
| [cb5a7826](https://github.com/RVCK-Project/rvck/commit/cb5a78268e75b5e73a23a092bd1ec9bafd13c044) | 2024-06-06 | Xiangyi Zeng | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [2d1e300a](https://github.com/RVCK-Project/rvck/commit/2d1e300a76e59e800fa85b9dd66c6287ff9fb3c2) | 2024-06-06 | Xiangyi Zeng | dt-bindings:event: Add Documentation for THEAD TH1520 event driver |
| [3d60de99](https://github.com/RVCK-Project/rvck/commit/3d60de99d69140bb1270912e41d144977cb81380) | 2024-06-06 | Xiangyi Zeng | drivers/soc/event: Add THEAD TH1520 event driver |
| [aa9ad7ba](https://github.com/RVCK-Project/rvck/commit/aa9ad7ba5c48e8fc71b55ba0bc277d9904c8b021) | 2024-06-05 | xianbing Zhu | net:stmmac: increase timeout for dma reset |
| [eba89425](https://github.com/RVCK-Project/rvck/commit/eba89425565e5f8f3cd44cf73b15fec0688f19cb) | 2024-06-05 | xianbing Zhu | stmmac:dwmac-thead: add support for suspend/resume feature |
| [cd3d4740](https://github.com/RVCK-Project/rvck/commit/cd3d4740bc6faa484f18bdc4b5faba33819edb76) | 2024-06-04 | xianbing Zhu | net:dwmac-thead: dd ptp clk set and enable |
| [ce70a272](https://github.com/RVCK-Project/rvck/commit/ce70a272b8ec57614c8768082947be48fcc267f0) | 2024-06-05 | Esther Z | configs: Enable th1520 mailbox. |
| [6b3dbd67](https://github.com/RVCK-Project/rvck/commit/6b3dbd671b1717a18f334e9e6fe82b7d73a01782) | 2021-08-10 | fugang.duan | firmware: thead: c910_aon: add th1520 Aon protocol driver |
| [2ed5ab44](https://github.com/RVCK-Project/rvck/commit/2ed5ab4476a253dd672ccf45ecb00c959dfb9aa0) | 2024-06-04 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 add delay line in different mode and sdio rxclk del... |
| [ddc30689](https://github.com/RVCK-Project/rvck/commit/ddc306899fddff9838d0d2ffc198262d70c1d061) | 2024-06-03 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 larger tuning max loop count to 128 |
| [60f109b4](https://github.com/RVCK-Project/rvck/commit/60f109b405ec275f6238c9700978b4fbc731ee18) | 2024-05-31 | xianbing Zhu | dts: th1520: enable sdio1 for wifi card in lichee-pi-4a |
| [c1426a62](https://github.com/RVCK-Project/rvck/commit/c1426a625f9dca7725ba152ce9cc0342b39c688d) | 2024-05-31 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 sdhci add fix io voltage 1v8 |
| [39fb9ed3](https://github.com/RVCK-Project/rvck/commit/39fb9ed3dbb976251ce24367877c4da6e9d7607a) | 2024-05-30 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 resolve accss rpmb error in hs400 |
| [52b6a7d1](https://github.com/RVCK-Project/rvck/commit/52b6a7d19bf910344d9215033555a8439315e587) | 2024-05-30 | Xiangyi Zeng | drivers/dmac: add pm suspend/resume for dma driver |
| [801c8c3f](https://github.com/RVCK-Project/rvck/commit/801c8c3fd858cd560fdf3c2ff3fedc64a63475d5) | 2023-08-21 | David Li | audio: th1520: add dma chan str for dmaengine |
| [a9d28ba1](https://github.com/RVCK-Project/rvck/commit/a9d28ba10cec5fd1c5a8519c56fc4ff1d1ab3633) | 2024-05-30 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 dmac1 and dmac2 device node |
| [7e226153](https://github.com/RVCK-Project/rvck/commit/7e226153574a0bf5bca6fad0cd2506880105d496) | 2023-08-22 | sanyi | STR: fix pca953x resume bug |
| [6855f6b4](https://github.com/RVCK-Project/rvck/commit/6855f6b459d09dffd3e0b106fb8bb37394fa2c5d) | 2024-05-28 | Xiangyi Zeng | drivers/iio/adc: add sysfs_remove_file when adc driver removed |
| [6333edd6](https://github.com/RVCK-Project/rvck/commit/6333edd6f4d4c3f0b69656dc8e0c3ac64b151e5e) | 2024-05-27 | Xiangyi Zeng | drivers/pvt: add mr75203 driver pm feature and correct temperature coefficient |
| [204a1df5](https://github.com/RVCK-Project/rvck/commit/204a1df5cd58171534fe260616cda1d8574162c7) | 2024-05-27 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 SPI/QSPI device node |
| [981d8d24](https://github.com/RVCK-Project/rvck/commit/981d8d24889aa0efe5d323fa8c410008e390beee) | 2024-05-27 | Xiangyi Zeng | dt-bindings: spi/qspi: Add Documentation for THEAD TH1520 SPI/QSPI |
| [73ab117b](https://github.com/RVCK-Project/rvck/commit/73ab117b5440ff6e20c3c2b7e845bf57a56d0bbc) | 2024-05-27 | Xiangyi Zeng | drivers/spi: Add THEAD TH1520 QSPI driver |
| [e7a9b73e](https://github.com/RVCK-Project/rvck/commit/e7a9b73ecb6e20949fdd83017fa158c0e5ad4501) | 2024-05-19 | Wei Fu | riscv: dts: thead: Add XuanTie TH1520 Mailbox device node |
| [68156170](https://github.com/RVCK-Project/rvck/commit/68156170838fc0b053fa33ad00dc664da8ae60ea) | 2024-05-17 | Fugang Duan | mailbox: add XuanTie TH1520 Mailbox IPC driver |
| [ef7b6321](https://github.com/RVCK-Project/rvck/commit/ef7b632185e2fd33bc16fca06830c6efa39f10de) | 2024-05-19 | Wei Fu | dt-bindings: mailbox: Add a binding file for XuanTie TH1520 Mailbox |
| [f166893d](https://github.com/RVCK-Project/rvck/commit/f166893d0573731b52d2f424910257225d966e4f) | 2024-05-17 | Xiangyi Zeng | dt-bindings: adc: Add Documentation for THEAD TH1520 ADC |
| [3d539ad7](https://github.com/RVCK-Project/rvck/commit/3d539ad721185058e22e58623e28b189a96b6967) | 2024-05-17 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 ADC device node |
| [62817e1e](https://github.com/RVCK-Project/rvck/commit/62817e1e5ec2e426ee19d4396cf4af3bb41656ec) | 2024-05-17 | Xiangyi Zeng | drivers/iio/adc: Add THEAD TH1520 ADC driver |
| [f6e40be4](https://github.com/RVCK-Project/rvck/commit/f6e40be4d5b8a476694cefa1ba33677e2e69499c) | 2024-06-29 | Chen Pei | riscv: ptrace: Fix ptrace using uninitialized riscv_v_vsize |
| [65b7d98e](https://github.com/RVCK-Project/rvck/commit/65b7d98e4a6256d60f7b5f0944ebbaab3be9b0dc) | 2024-03-18 | Heiko Stuebner | T-Head C9xx cores implement an older version (0.7.1) of the vector specification... |
---

**共 392 条提交（显示全部）**

[分页显示](阿里达摩院.md) | [纯文本视图](阿里达摩院_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-10-08 19:27:23*
*数据来源: 主分支 rvck-6.6@07927e13*
