# 阿里达摩院 贡献详情

<div style="background-color: #F4433620; padding: 15px; border-radius: 8px; border-left: 5px solid #F44336;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 393</li>
<li><strong>统计时间</strong>: 2026-06-26 11:06:09</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.141</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @linux.alibaba.com

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [c1f4a892](https://github.com/RVCK-Project/rvck/commit/c1f4a8929de0cf4e988c05bf4c4e30b60caa9079) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated ChaCha20 |
| [b72d325b](https://github.com/RVCK-Project/rvck/commit/b72d325bc5dd66c63fe6d72f7c717521996340bf) | 2024-01-21 | Jerry Shih | crypto: riscv - add vector crypto accelerated AES-{ECB,CBC,CTR,XTS} |
| [f83d1f51](https://github.com/RVCK-Project/rvck/commit/f83d1f51311bb430d2d213b854c58a4ab90ca10a) | 2024-01-21 | Heiko Stuebner | RISC-V: hook new crypto subdir into build-system |
| [8822790a](https://github.com/RVCK-Project/rvck/commit/8822790a6f469bba17244e0406baa1799224b09f) | 2024-01-21 | Eric Biggers | RISC-V: add TOOLCHAIN_HAS_VECTOR_CRYPTO |
| [cf784b4c](https://github.com/RVCK-Project/rvck/commit/cf784b4c87b96422f964428ab268cc85e27f7437) | 2024-01-21 | Heiko Stuebner | RISC-V: add helper function to read the vector VLEN |
| [ec1bbedf](https://github.com/RVCK-Project/rvck/commit/ec1bbedf924dfc79fc27977e28c89c51a646e31f) | 2023-10-12 | Ian Rogers | perf pmu: Lazily compute default config |
| [4d6bfbc1](https://github.com/RVCK-Project/rvck/commit/4d6bfbc15c59365714f91eeb1d993ce2322bdbd3) | 2023-10-12 | Ian Rogers | perf pmu-events: Remember the perf_events_map for a PMU |
| [8c4c9aed](https://github.com/RVCK-Project/rvck/commit/8c4c9aed0cdab24bb0c0522c0f4094b56da33fb7) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify perf_pmu__config_terms |
| [7f4b08ff](https://github.com/RVCK-Project/rvck/commit/7f4b08ffc752d3d3888e9380b4fcca0d5e413c7b) | 2023-10-12 | Ian Rogers | perf pmu: Const-ify file APIs |
| [88d6a985](https://github.com/RVCK-Project/rvck/commit/88d6a98554a84aa3af167131ab8743404dd4e137) | 2023-10-12 | Ian Rogers | perf arm-spe: Move PMU initialization from default config code |
| [ba227876](https://github.com/RVCK-Project/rvck/commit/ba227876701b38ff76dbeea077e463378f434be5) | 2023-10-12 | Ian Rogers | perf intel-pt: Move PMU initialization from default config code |
| [46eb3c80](https://github.com/RVCK-Project/rvck/commit/46eb3c80e1015ebdd2e28e3d9db9b71270943192) | 2023-10-12 | Ian Rogers | perf pmu: Rename perf_pmu__get_default_config to perf_pmu__arch_init |
| [745da4b1](https://github.com/RVCK-Project/rvck/commit/745da4b1962668ed00d50381e66ffa3738b97ae7) | 2023-09-24 | Ian Rogers | perf pmus: Make PMU alias name loading lazy |
| [7677dd38](https://github.com/RVCK-Project/rvck/commit/7677dd38a45d500372bfc5286ad6601b158c8ffa) | 2023-09-01 | Ian Rogers | perf parse-events: Introduce 'struct parse_events_terms' |
| [146e7589](https://github.com/RVCK-Project/rvck/commit/146e758904d316e7ab341583e02e1c3b26a650a2) | 2023-09-01 | Ian Rogers | perf parse-events: Copy fewer term lists |
| [60eab316](https://github.com/RVCK-Project/rvck/commit/60eab3164ab203ef347d0e23ad4cde4830167cf5) | 2023-09-01 | Ian Rogers | perf parse-events: Avoid enum casts |
| [24eb47e3](https://github.com/RVCK-Project/rvck/commit/24eb47e3874e810fd8e7ee7ae95e1f7b50dd8624) | 2023-09-01 | Ian Rogers | perf parse-events: Tidy up str parameter |
| [92a20dbe](https://github.com/RVCK-Project/rvck/commit/92a20dbe4c97652d45164b7b5699ea2cd8cf78d9) | 2023-09-01 | Ian Rogers | perf parse-events: Remove unnecessary __maybe_unused |
| [4c3ccb4e](https://github.com/RVCK-Project/rvck/commit/4c3ccb4e85628a9384ceb0543906a59461245109) | 2025-04-21 | Guo Ren | riscv: xchg: Prefetch the destination word for sc.w |
| [11e512a8](https://github.com/RVCK-Project/rvck/commit/11e512a82a1c314e636954f0b24bff55fc5720a9) | 2025-04-21 | Guo Ren | riscv: Add ARCH_HAS_PREFETCH[W] support with Zicbop |
| [36e9c467](https://github.com/RVCK-Project/rvck/commit/36e9c4674b6c71c8a0c93b31e557de5c74b5be7c) | 2025-01-02 | Guo Ren | iommu/riscv: Fixup compile warning |
| [62e964fb](https://github.com/RVCK-Project/rvck/commit/62e964fb7a8798858b674b04f5bf3d2b07dc25cd) | 2024-04-22 | Shenlin Liang | perf kvm/riscv: Port perf kvm stat to RISC-V |
| [08d8a88d](https://github.com/RVCK-Project/rvck/commit/08d8a88db9b06e102e9d34f8f93835be20f443b8) | 2024-04-22 | Shenlin Liang | RISCV: KVM: add tracepoints for entry and exit events |
| [eeac07db](https://github.com/RVCK-Project/rvck/commit/eeac07dbad3a7de1698da16c6eb611fb28f684f3) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Add separate ticket-lock.h |
| [9adabe27](https://github.com/RVCK-Project/rvck/commit/9adabe27e02a3ebc94de93767c3298b2f3ad8ffc) | 2024-11-03 | Guo Ren | asm-generic: ticket-lock: Reuse arch_spinlock_t of qspinlock |
| [24bea32a](https://github.com/RVCK-Project/rvck/commit/24bea32a12bc20e60df2f205563874a1860ec778) | 2023-09-08 | Guo Ren | asm-generic: ticket-lock: Optimize arch_spin_value_unlocked() |
| [288defca](https://github.com/RVCK-Project/rvck/commit/288defca9fe1d2b09f31e15d4bdc2d328d3e19ae) | 2023-09-18 | Baolin Wang | mm: add statistics for PUD level pagetable |
| [f0007179](https://github.com/RVCK-Project/rvck/commit/f0007179a67be36e235ff7818521790eda44fa52) | 2024-09-01 | xianbing Zhu | hibernate: reduce logs in nosave_region register/remove |
| [fdaa8df9](https://github.com/RVCK-Project/rvck/commit/fdaa8df9fce2f6e92f733f8b2743ad9e3a9b4914) | 2024-09-01 | Xiangyi Zeng | drivers: pwm_bl: fix adjusting failure of pwm backlight when resume from str |
| [49fdb054](https://github.com/RVCK-Project/rvck/commit/49fdb05427d0305f0602642d080f5167e67dae87) | 2024-09-01 | David Li | audio: th1520: solve misalignment dma addr problem when HDMI playback |
| [d1734179](https://github.com/RVCK-Project/rvck/commit/d173417967cc626bfac08d0e3055a93bb0ab8f8b) | 2024-09-01 | Xiangyi Zeng | drivers: i2c-hid: fix rvbook i2c-hid wake-up error |
| [8af49553](https://github.com/RVCK-Project/rvck/commit/8af495538ba383847f155604e2c94cb96171f682) | 2024-09-01 | xianbing Zhu | hibernate: add blkdev flush op berfore poweroff |
| [2966f061](https://github.com/RVCK-Project/rvck/commit/2966f0617616ab11e5648ecb1a013198d41b8f02) | 2024-09-01 | David Li | audio: light: i2s dma channel acquire change to dynamic |
| [12f1fe66](https://github.com/RVCK-Project/rvck/commit/12f1fe66f56d966cce4069441078e31403431317) | 2024-09-01 | Xiangyi Zeng | dts: crash: enable watchdog0 and watchdog1 in crash kernel |
| [dae5ce12](https://github.com/RVCK-Project/rvck/commit/dae5ce12d44768b7052f6a59138193e6dd03d9e8) | 2024-09-01 | tingming | defconfig: th1520: to fix problem of do_compile_kernelmodules fail |
| [3f11814e](https://github.com/RVCK-Project/rvck/commit/3f11814e75728a5cba4017efce9b923ffc9d6cc8) | 2024-09-01 | tingming | defconfig: th1520: to add configs of media |
| [eeff3358](https://github.com/RVCK-Project/rvck/commit/eeff335853d52d9dff8a5c16cbce2f69281809b6) | 2024-09-01 | Xiangyi Zeng | dts: som: add gpio-keys node |
| [797df4be](https://github.com/RVCK-Project/rvck/commit/797df4be69c6ea58b915b0990b7505273523ce1b) | 2024-09-01 | Xiangyi Zeng | driver: wdt: th1520_wdt: add th1520_wdt driver pm ops |
| [38b0d041](https://github.com/RVCK-Project/rvck/commit/38b0d0413e2c75b99b7cbeec132ffe3ae551b418) | 2024-09-01 | Xiangyi Zeng | drivers: mtd: nand: spi: support new winbond-spi-nand devices |
| [1981d6ca](https://github.com/RVCK-Project/rvck/commit/1981d6cade1c796491733f3d5e6298d23bdd20e1) | 2024-09-01 | tingming | config: th1520: to modify MEMBLOCK&LOCALVERSION_AUTO config |
| [e239345d](https://github.com/RVCK-Project/rvck/commit/e239345d791475d5a43655789f0fab7614a8ec2b) | 2024-09-01 | xianbing Zhu | dts:th1520: fix sd card wprtn issue |
| [a8d05967](https://github.com/RVCK-Project/rvck/commit/a8d05967eb6b580445a03dfce98979f25cfc0cdc) | 2024-09-01 | xianbing Zhu | dts:th1520: add aliases for basic interface |
| [be4d8765](https://github.com/RVCK-Project/rvck/commit/be4d8765cc068bdc2b0ecf6d50a1baa7cb35be8c) | 2024-09-01 | shuofeng.ren | audio: light_fm: change hdmi config for ap i2s hdmi audio |
| [8e6d5d8a](https://github.com/RVCK-Project/rvck/commit/8e6d5d8afaa41a93f9f8d579606f54dacbd62968) | 2024-09-01 | Huaming | driver:i2c:fix rx_over miss re-try |
| [7882df21](https://github.com/RVCK-Project/rvck/commit/7882df2162486845bf30c4149491ed8d0116d2e5) | 2024-09-01 | Esther Z | drivers: cpufreq: fix cpufreq em model regsiter warn. |
| [78d68fef](https://github.com/RVCK-Project/rvck/commit/78d68fefa496ffa90cd989056f78c1396ce635ae) | 2024-09-01 | Xiangyi Zeng | drivers: pwm: fix pwm enable status check error |
| [fe1c6897](https://github.com/RVCK-Project/rvck/commit/fe1c689707c9f65a830ca8bdfc67bcfecaf56e97) | 2024-09-01 | xianbing Zhu | th1520: sdhci: add quirks for th1520-a-val board |
| [9488964d](https://github.com/RVCK-Project/rvck/commit/9488964dabe89563f97d603113d253adc690c86c) | 2024-09-01 | xianbing Zhu | sdhci:th1520: fixup mmc retune infinitely issue |
| [092fb61b](https://github.com/RVCK-Project/rvck/commit/092fb61b0205a59297be3253786b631e82099e30) | 2024-09-01 | David Li | dts: audio: update audio pin configuration |
| [35335afb](https://github.com/RVCK-Project/rvck/commit/35335afb3bdd14997cf010bfd71b4f0a11991dd0) | 2024-09-01 | David Li | audio: th1520: workaround to solve noise at the end |
| [eae85130](https://github.com/RVCK-Project/rvck/commit/eae85130f856f8506eddbda38789472e92c46f25) | 2024-09-01 | tingming | dts: th1520_defconfig: to revert the config CONFIG_USB_GADGET |
| [bd10de6a](https://github.com/RVCK-Project/rvck/commit/bd10de6a5f13cef50048b4852cf8e6db391dade5) | 2024-09-01 | Xiangyi Zeng | dts: gpio: add gpio/0/1/2/3 clk gate |
| [3e5a738b](https://github.com/RVCK-Project/rvck/commit/3e5a738b3ada43357caf57d3c8f67576ec4f6f9a) | 2024-09-01 | xianbing Zhu | driver: pty/clk: bootargs options for pty and clk_debug init |
| [5d85d1da](https://github.com/RVCK-Project/rvck/commit/5d85d1da74347c797c0bb1a277f8429060b83d58) | 2024-09-01 | xianbing Zhu | hibernate: improve crc32 speed by calc compressed data |
| [6c00edde](https://github.com/RVCK-Project/rvck/commit/6c00edde84e9d2d01caae7758136219a69fdc898) | 2024-09-01 | xianbing Zhu | trace: add bootargs options for trace and ftrace for hibernate resume |
| [44a3619b](https://github.com/RVCK-Project/rvck/commit/44a3619b89d8173caa63e23436eb8f1cf41f8f99) | 2024-09-01 | xianbing Zhu | hibernate: restart when resume fail and added resume dev |
| [b48d3687](https://github.com/RVCK-Project/rvck/commit/b48d368782fc13e7a83fa39920cb39cc2858d147) | 2024-09-01 | xianbing Zhu | hibernate: get buffer page speedup with no GPF_ZERO |
| [be05e6e9](https://github.com/RVCK-Project/rvck/commit/be05e6e9bca99b3252eee24385d09d2ecab8abfb) | 2024-09-01 | xianbing Zhu | hibernate: speedup image crc caculation by multi-threads |
| [655f202c](https://github.com/RVCK-Project/rvck/commit/655f202c43cc5616e2500023d5fe47a39e2df8b1) | 2024-09-01 | xianbing Zhu | hibernate: load image retry in some crc mismatch |
| [ab0a8e16](https://github.com/RVCK-Project/rvck/commit/ab0a8e16c26f95baea0bc622ce5295ca57de92f9) | 2024-09-01 | xianbing Zhu | dts: th1520: update mmc sdio clocks |
| [39c874d4](https://github.com/RVCK-Project/rvck/commit/39c874d4475457f3220546b45e35067c1dfbea75) | 2024-09-01 | tingming | dts: rvbook: to modity rvbook dts |
| [f52796d9](https://github.com/RVCK-Project/rvck/commit/f52796d9ffa0bd3e7ca41038b390c87d2c98a1fd) | 2024-09-01 | Xiangyi Zeng | drivers: iopmp: add iopmp driver |
| [d3d0dd02](https://github.com/RVCK-Project/rvck/commit/d3d0dd02c971b74109a009825d25a5f80862df65) | 2024-09-01 | Xiangyi Zeng | drivers: pct: fix NULL ptr error when pvt resume from ram |
| [1e19c5d8](https://github.com/RVCK-Project/rvck/commit/1e19c5d82c5ad46f5d1404db3bb6cd86f0941bba) | 2024-09-01 | Xiangyi Zeng | drivers: pinctrl: add pinctrl driver pm ops |
| [d0bbc7a7](https://github.com/RVCK-Project/rvck/commit/d0bbc7a76fb537c74292d6c514b6e65efe91e143) | 2024-09-01 | Xiangyi Zeng | dts: th1520: modify kernel memory region, [0x0, 0x20000] is reserved for opensbi |
| [6b5729a0](https://github.com/RVCK-Project/rvck/commit/6b5729a07ee2cf2b76a242a481bbcd9ee8ef1bc6) | 2024-09-01 | tingming | dts: th1520: to modify rvbook dts |
| [2c35ef46](https://github.com/RVCK-Project/rvck/commit/2c35ef46dc6b872636e6740e0814fbe13e9971f4) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [c014f29b](https://github.com/RVCK-Project/rvck/commit/c014f29bb4706bfe4b1fafba2966acd32119da55) | 2024-09-01 | David Li | audio: th1520: to support tdm/spdif feature |
| [c48d6656](https://github.com/RVCK-Project/rvck/commit/c48d6656dfbda13697b46746067251c17bb7a5b1) | 2024-09-01 | David Li | dts: rvbook: to adapt compatible 'thead,th1520' of rvbook |
| [15aa3fc2](https://github.com/RVCK-Project/rvck/commit/15aa3fc29cb6cd8c338bae93bad17abc83433c07) | 2024-09-01 | xianbing Zhu | riscv: hibernate: crash dump memory mark as nosave |
| [85d1d59a](https://github.com/RVCK-Project/rvck/commit/85d1d59aef05a385886f934f9a95979ebeeb666f) | 2024-09-01 | xianbing Zhu | hibernate:snaoshot: detail show copied pfn info |
| [bd654f56](https://github.com/RVCK-Project/rvck/commit/bd654f564fe07f77fef7a6801653e40628ae6639) | 2024-09-01 | xianbing Zhu | hibernate: extended platform ops for all mode |
| [ca830162](https://github.com/RVCK-Project/rvck/commit/ca8301622f3f7b0365077386901a32fefe3eb062) | 2024-09-01 | xianbing Zhu | hibernate: add interface for driver to mark no_save region |
| [449fe99b](https://github.com/RVCK-Project/rvck/commit/449fe99be3caa72e61af0b6fc0ec94e38a4aae49) | 2024-09-01 | Xiangyi Zeng | drivers: dma: change dw dma runtime pm to sync |
| [7f5e171d](https://github.com/RVCK-Project/rvck/commit/7f5e171d48b8f406c718f0492f79eb8f5c7f3f58) | 2024-09-01 | Xiangyi Zeng | audio: th1520: fix i2s pause/resume dma fail |
| [794eab72](https://github.com/RVCK-Project/rvck/commit/794eab72723a0f7925cb48ac5d202d4d94684444) | 2024-09-01 | Hao Li | dma: dw-axi-dmac: fix multi-channel dmatest fail |
| [cca91944](https://github.com/RVCK-Project/rvck/commit/cca91944f24493eff2568f8164e87aadb8371580) | 2024-09-01 | Xiangyi Zeng | audio: th1520: resolve the dma error problem when ap i2s audio stop play |
| [075f1204](https://github.com/RVCK-Project/rvck/commit/075f120451ed8f946374b7eccbac58a7d2a84ea6) | 2024-09-01 | Xiangyi Zeng | watchdog: dw_wdt: ap watchdog timeout report different strategies |
| [bd473780](https://github.com/RVCK-Project/rvck/commit/bd473780b057ddef782e0bdb2c99bf6a59ef89b2) | 2024-09-01 | Xiangyi Zeng | drivers: wdt: drop DW wdt restart function for th1520 soc |
| [243c30b9](https://github.com/RVCK-Project/rvck/commit/243c30b94e9be3787d3d083d1b4a1fbafcfd6c3e) | 2024-09-01 | Xiangyi Zeng | drivers: hwmon: bugfix for mr75203 |
| [896dace0](https://github.com/RVCK-Project/rvck/commit/896dace02ab14c8759da9e572b127fedd8122549) | 2024-09-01 | Xiangyi Zeng | spi: spidev: add spidev speed sysfs for debug |
| [9d880871](https://github.com/RVCK-Project/rvck/commit/9d880871ad1b3a39c7fc2c9daf0e1e404b8ccd00) | 2024-09-01 | Huaming | dtb:th15210: correct vi camera2 path cfg |
| [a6a82e1f](https://github.com/RVCK-Project/rvck/commit/a6a82e1f9515c25b395b28b223fa72322e79eaf4) | 2024-09-01 | David Li | dts: update pin/regulator configuration in rvbook dts file |
| [95f5415c](https://github.com/RVCK-Project/rvck/commit/95f5415cc26c8b8419a8ab57a532dac9814a1aeb) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add th1520-a-val-crash.dts and th1520-lpi4a-product-crash.dts for k... |
| [0e144c45](https://github.com/RVCK-Project/rvck/commit/0e144c45c46283b084cc04de799f59692d0df907) | 2024-09-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [20f551cb](https://github.com/RVCK-Project/rvck/commit/20f551cb7240c614f17cdf2978e96c99337018ba) | 2024-09-01 | Huaming | dtb:th1520: LA Board correct cma cfg |
| [c74d5c5c](https://github.com/RVCK-Project/rvck/commit/c74d5c5cf60192ee82b0ffb541ff74cb1fa1a9c9) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [01ef7508](https://github.com/RVCK-Project/rvck/commit/01ef750857e58a523e915e9ebaaf22ea0e29b962) | 2024-09-01 | David Li | perf: sync vendor event path with torvalds/linux |
| [0fd97cbd](https://github.com/RVCK-Project/rvck/commit/0fd97cbd520b20e58b61377a9963a3171dc9990e) | 2024-09-01 | David Li | dts: audio: to support i2s-8ch feature |
| [94dbdda2](https://github.com/RVCK-Project/rvck/commit/94dbdda235548629eda269affd0f27f2ceb21bfb) | 2024-09-01 | tingming | th1520: system_monitor: to add system_monitor driver |
| [6103eec0](https://github.com/RVCK-Project/rvck/commit/6103eec0b229587b55349f4b145f9e79d51b0f19) | 2024-09-01 | David Li | dts: audio: to adapt compatible 'thead,th1520' |
| [4bce2d6b](https://github.com/RVCK-Project/rvck/commit/4bce2d6bc817f4c7fee3a5db1bb63c7cc1276e65) | 2024-09-01 | Huaming | dtb:th1520:light add vi module |
| [2992bd6e](https://github.com/RVCK-Project/rvck/commit/2992bd6e8c8a8bf5104be1c566815cffbc990445) | 2024-09-01 | xiaojin.cxj | regdump:add regdump support for lpi4a and light-a && rename some dts name from "... |
| [5fd4a7cc](https://github.com/RVCK-Project/rvck/commit/5fd4a7cc4b9292d452bc58450f273dd9650b21e0) | 2024-09-01 | Huaming | driver:i2c:add i2c mode for tx transfer |
| [68b0571a](https://github.com/RVCK-Project/rvck/commit/68b0571a6c4dce10f12c78adab0ac9addd65706f) | 2024-09-01 | David Li | dts: audio: correct audiosys pinctrl name |
| [fda64dbe](https://github.com/RVCK-Project/rvck/commit/fda64dbee1267b4adc8153e5d6a935223bbd4205) | 2024-09-01 | David Li | audio: th1520: support audiosys pinctrl feature |
| [07dfec12](https://github.com/RVCK-Project/rvck/commit/07dfec120cf4092ef59c2543a07560bbb8fca261) | 2024-09-01 | Xiangyi Zeng | dts: th1520: remove usb hub node from th1520-a board |
| [0988563a](https://github.com/RVCK-Project/rvck/commit/0988563a2df91ae9d8daf8503637f66c4a51c540) | 2024-09-01 | tingming | hwspinlock: th1520: to add th1520 hwspinlock driver |
| [98463d27](https://github.com/RVCK-Project/rvck/commit/98463d277fe64962d8c0a1f0d657c78134a0f315) | 2024-09-01 | Xiangyi Zeng | dts: th1520: add rambus eip_28 device node |
| [2b952415](https://github.com/RVCK-Project/rvck/commit/2b952415f9b895c7acd72e24f72ce6c9dc8641a7) | 2024-09-01 | Esther Z | dts: th1520: add LA regulator support. |
| [59346bac](https://github.com/RVCK-Project/rvck/commit/59346bacb10660f32f448053c703c094497eea60) | 2024-09-01 | Xiangyi Zeng | drivers:misc:dsmart: add th1520 iso7816 driver |
| [2a1ec22b](https://github.com/RVCK-Project/rvck/commit/2a1ec22bf78e030d4c2835255c59e7c02d6ad15d) | 2024-09-01 | Xiangyi Zeng | drivers:nvmem:efuse: add th1520 efuse driver |
| [46738625](https://github.com/RVCK-Project/rvck/commit/46738625624b97957a4e5e0cccbc9f7f9da6cdac) | 2024-09-01 | David Li | audio: th1520: add soundcard dts node of th1520-a-val board |
| [d755db88](https://github.com/RVCK-Project/rvck/commit/d755db887ef47b4a72473f2f7f9b7a66336e5aba) | 2024-09-01 | tingming | dts: th1520: to add npu device node |
| [23e458ef](https://github.com/RVCK-Project/rvck/commit/23e458ef8210287c5c1dbad675b3a65d11bb4437) | 2024-09-01 | Xiangyi Zeng | riscv:dts:som: add th1520-lpi4a-product and th1520-lpi4a-product-sec dts file |
| [04b53d88](https://github.com/RVCK-Project/rvck/commit/04b53d88aa54d6b2456b8eb15d141a6fface022c) | 2024-09-01 | Xiangyi Zeng | dts:th1520-a: add th1520-a-val.dts and th1520-a-val-sec.dts |
| [9832f291](https://github.com/RVCK-Project/rvck/commit/9832f291008a6c4a9aaa6609f6f7c83c86ac9f5c) | 2024-06-21 | Huaming | defconfig:th1520: enable cma config |
| [f8a663d0](https://github.com/RVCK-Project/rvck/commit/f8a663d0a18ae086753c13742187e894250c6519) | 2024-09-01 | Huaming | dtb:lipi:enable VI module config |
| [47607e7d](https://github.com/RVCK-Project/rvck/commit/47607e7d7ede13b062c441992e5183b74b8d7fe6) | 2024-09-01 | xianbing Zhu | riscv: config: add MAX_ORDER config for riscv and th1520 |
| [060f5abf](https://github.com/RVCK-Project/rvck/commit/060f5abf17ee6d849c80d89bcceee9bccef1f2f7) | 2024-09-01 | xianbing Zhu | dts: th1520: add vdec venc and video mem device node |
| [82d9415e](https://github.com/RVCK-Project/rvck/commit/82d9415e4bd3b380f9858e1bbea0ac8ee4f38f97) | 2024-08-31 | xianbing Zhu | mm: export sysmbol __pte_offset_map_lock for modules outside |
| [4a883921](https://github.com/RVCK-Project/rvck/commit/4a883921d52477e2ddc0ea46c99cd5193453953f) | 2024-08-31 | David Li | audio: th1520: add pa aw87519 driver for XuanTie TH1520 SoC |
| [0322ed42](https://github.com/RVCK-Project/rvck/commit/0322ed421f0ac97d39a71b7c893438fead48d640) | 2024-08-31 | David Li | audio: th1520: add spdif driver for XuanTie TH1520 SoC |
| [eab8b1f9](https://github.com/RVCK-Project/rvck/commit/eab8b1f9886bcc09691cfb7ad9b6dd7d9656ed41) | 2024-08-31 | David Li | audio: th1520: add tdm driver for XuanTie TH1520 SoC |
| [a67780b4](https://github.com/RVCK-Project/rvck/commit/a67780b49defefe53927fbe74a381a485385093b) | 2024-09-06 | Guo Ren | riscv: mm: Add support for Svinval extension |
| [18ed3b4e](https://github.com/RVCK-Project/rvck/commit/18ed3b4ec1d4ab74c9743f20253f0ce652828f11) | 2024-09-04 | Guo Ren | riscv: Add ACLINT SSWI support |
| [64057af0](https://github.com/RVCK-Project/rvck/commit/64057af0a881d5909be8fbbe0853df4802c16536) | 2024-07-29 | forain | drm: Fix HDMI hot-plug problem |
| [1a440ec0](https://github.com/RVCK-Project/rvck/commit/1a440ec05fde0b8d3c3d5a9a37f248590e3a7152) | 2024-07-25 | Hao Li | gpu/drm: hdmi: Add hdmi debounce to enhance hdmi plugin/out stable |
| [2f596afa](https://github.com/RVCK-Project/rvck/commit/2f596afa60320b8b1d586f09261ea417d0dc0337) | 2024-07-21 | David Li | audio: th1520: fixup compile warning of i2s driver |
| [a3fe0ef1](https://github.com/RVCK-Project/rvck/commit/a3fe0ef15b4f502b2b4d3a1ba455a743a5a1f950) | 2024-07-12 | David Li | dmaengine: dw-axi-dmac: Add support for Xuantie TH1520 DMA |
| [7564ef3d](https://github.com/RVCK-Project/rvck/commit/7564ef3d42a7ebcbe99cd7eaf9671d72421c9c34) | 2024-07-04 | Chen Pei | arch:rsicv:select ARCH_HAS_DMA_WRITE_COMBINE |
| [f527454a](https://github.com/RVCK-Project/rvck/commit/f527454a8305877c5f001ebaf22a7dff3d46a3f8) | 2024-07-01 | Xiangyi Zeng | drivers: pinctrl: correct th1520 audio i2c1 bit mapping table |
| [1e8c9456](https://github.com/RVCK-Project/rvck/commit/1e8c9456e5251cb72b31258f459277b784434889) | 2024-06-30 | Huaming | driver:padctrl:correct th1520 gpio_1 24/25 cfg |
| [04689c24](https://github.com/RVCK-Project/rvck/commit/04689c245f5a39b6be6ffb8847e3d430eaa3867d) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add adc vref-supply regulator |
| [9835eb56](https://github.com/RVCK-Project/rvck/commit/9835eb5650ec8e91475e54600a470f3e28e7d633) | 2024-07-04 | Xiangyi Zeng | dts: th1520: add cpu thermal node and device thermal node |
| [1a412a01](https://github.com/RVCK-Project/rvck/commit/1a412a0147162c04cf08c6d2ad285a786edb5a24) | 2024-07-04 | Xiangyi Zeng | drivers: event: add macro definition to control SW_PANIC event |
| [55809fb2](https://github.com/RVCK-Project/rvck/commit/55809fb230c5232ca3cf5596bda5dea4c3bd5343) | 2024-06-28 | David Li | audio: th1520: enable soundcard feature |
| [7416271a](https://github.com/RVCK-Project/rvck/commit/7416271a6643a378c25d93bfc42902067d2d6874) | 2024-06-27 | David Li | audio: th1520: support audiosys pinctrl feature |
| [27d5cf57](https://github.com/RVCK-Project/rvck/commit/27d5cf57ce21b8575af1c1a78f510d0423b05fb7) | 2024-06-26 | Xiangyi Zeng | dts: th1520: fix interrupt number config error in dts |
| [6912afe6](https://github.com/RVCK-Project/rvck/commit/6912afe675a00bc6ad29789b947a78017edc28c2) | 2024-06-24 | forain | DPU: add DPU driver for Lichee-Pi-4A board |
| [4c0c3272](https://github.com/RVCK-Project/rvck/commit/4c0c3272d7cc1489c6a51e2e1d904417da7cfa66) | 2024-06-23 | tingming | dts: th1520: add npu device node |
| [fa3e865b](https://github.com/RVCK-Project/rvck/commit/fa3e865b269f9a0c5781c31a3cce259542ad2b63) | 2024-06-21 | David Li | codec: audio: add codec driver for Lichee-Pi-4A board |
| [8987949f](https://github.com/RVCK-Project/rvck/commit/8987949f1edd4fddfea922e1f215abdd6d6ae468) | 2024-06-20 | Chen Pei | riscv: vector: Fix the boot issue compiled using xuantie-toolchain or upstream-t... |
| [f1a50876](https://github.com/RVCK-Project/rvck/commit/f1a50876ba5fd2b83dbf22c6b39c4e22b61a707c) | 2024-06-19 | Esther Z | drivers: cpufreq: add cpufreq driver. |
| [3abea5c4](https://github.com/RVCK-Project/rvck/commit/3abea5c4b8201bd666a8545bfb10b306a2ce3631) | 2024-06-18 | Esther Z | riscv: dts: Introduce lichee-pi-4a fixed regulator support. |
| [a98d8e74](https://github.com/RVCK-Project/rvck/commit/a98d8e74b961c8d41416e047e8b0566d93ab0e65) | 2024-06-17 | zhangye | Enable XUANTIE ISA for memcpy performance |
| [10280c60](https://github.com/RVCK-Project/rvck/commit/10280c6057002f108cffbb947b424b86763b01a2) | 2024-03-27 | Chen Pei | riscv: build: Support compiling kernel using Xuantie toolchain |
| [c673c28d](https://github.com/RVCK-Project/rvck/commit/c673c28d8b2ed7ef65d0f3589a7a163726d3e1ac) | 2024-06-17 | David Li | i2s: remove debug message |
| [33bd553e](https://github.com/RVCK-Project/rvck/commit/33bd553e33b983ab8716ca0d03ea57879c24d8ba) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the aon gpio range configuration error |
| [6a9a3f90](https://github.com/RVCK-Project/rvck/commit/6a9a3f902c1bf02f9bcc340eee51789789537890) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix spi/qspi1 cs pin duplicate configuration error |
| [6105284d](https://github.com/RVCK-Project/rvck/commit/6105284d6e1c6dae4f566f11adb35581fb79efbd) | 2024-06-17 | Xiangyi Zeng | riscv:dts: fix the gpio range configuration error |
| [ef0cf7e6](https://github.com/RVCK-Project/rvck/commit/ef0cf7e60e745c362a659cf46875d7726bd80c96) | 2024-06-16 | Esther Z | drivers: regulator: add th1520 AON virtual regulator control support. |
| [b8457808](https://github.com/RVCK-Project/rvck/commit/b84578089468bf98e47ef502a92b90baefc807e3) | 2024-06-16 | Esther Z | dt-bindings: add AON resource id headfile |
| [3ea95cbf](https://github.com/RVCK-Project/rvck/commit/3ea95cbfee4414e994059322696758b1991ed401) | 2024-06-16 | Esther Z | drivers: pmdomain: support th1520 Power domain control. |
| [faa81b99](https://github.com/RVCK-Project/rvck/commit/faa81b9938fc35836cc22ef6b70b79ccb8e8e428) | 2024-06-15 | David Li | i2s: add i2s driver for XuanTie TH1520 SoC |
| [4218f831](https://github.com/RVCK-Project/rvck/commit/4218f831c66815edb0bded7b844afc760401fc29) | 2024-06-15 | David Li | configs: xuantie: correct definition of SoC Architecture |
| [d131fa0f](https://github.com/RVCK-Project/rvck/commit/d131fa0f66d7ec6fbb04b419da9923deae0202b1) | 2024-06-11 | lst | i2c: designware: add support for hcnt/lcnt got from dt |
| [015d5f2d](https://github.com/RVCK-Project/rvck/commit/015d5f2d83d1ed51936f01564ab6ebcddd41dff3) | 2024-06-11 | xiaojin.cxj | add 902 share mem log |
| [46acb0a2](https://github.com/RVCK-Project/rvck/commit/46acb0a203a30066536ef8021591c9db3bdfee57) | 2024-06-05 | abing | refine thead,th1520-usb.yaml |
| [3f188b56](https://github.com/RVCK-Project/rvck/commit/3f188b56a193bab12d67acbd137199d5cba61c4f) | 2024-06-05 | abing | drivers: usb: dwc3: add usb_mode usb_speed param for mode speed change when insm... |
| [49f15008](https://github.com/RVCK-Project/rvck/commit/49f1500804e2a3c0d8bb7c9c413cd7d8f703f376) | 2024-06-05 | abing | drivers: usb: add dwc3-thead.c |
| [2eba2c68](https://github.com/RVCK-Project/rvck/commit/2eba2c68128deaa7ff083b7d3e8b15a7131fb95e) | 2024-06-05 | abing | th1520.dtsi: refine usb dts |
| [960f07a5](https://github.com/RVCK-Project/rvck/commit/960f07a5f863ca5628d60376e5381df0c1f12e4d) | 2024-06-06 | Xiangyi Zeng | riscv:dts:thead: Add TH1520 event and watchdog device node |
| [d167848a](https://github.com/RVCK-Project/rvck/commit/d167848a2b6a77a8cca451d92f234ee37b88fbca) | 2024-06-06 | Xiangyi Zeng | dt-bindings:wdt: Add Documentation for THEAD TH1520 pmic watchdog |
| [5a19cac1](https://github.com/RVCK-Project/rvck/commit/5a19cac1da803ae8cdeb8460c5e1d4ce512b0795) | 2024-06-06 | Xiangyi Zeng | drivers/watchdog: Add THEAD TH1520 pmic watchdog driver |
| [81373769](https://github.com/RVCK-Project/rvck/commit/813737696e8e92538a8ac58658ac158d5398cf1d) | 2024-06-06 | Xiangyi Zeng | dt-bindings:event: Add Documentation for THEAD TH1520 event driver |
| [9d515e3e](https://github.com/RVCK-Project/rvck/commit/9d515e3ed2c12e6e1d2171003a2975ab981e6367) | 2024-06-06 | Xiangyi Zeng | drivers/soc/event: Add THEAD TH1520 event driver |
| [391054a5](https://github.com/RVCK-Project/rvck/commit/391054a596b793e38eb0cb1f3aab180caedbae9b) | 2024-06-07 | xiaojin.cxj | fix rpmsg addr cast warning |
| [010d8645](https://github.com/RVCK-Project/rvck/commit/010d86454d53694ea44cb23872c350795f2ac015) | 2024-06-07 | xiaojin.cxj | fix lp32 compile warnoing for rpmsg |
| [82600fb6](https://github.com/RVCK-Project/rvck/commit/82600fb66174ad885aaadd315b2587898ec26683) | 2024-06-07 | xiaojin.cxj | fix proc log warning |
| [329f0b0a](https://github.com/RVCK-Project/rvck/commit/329f0b0aaec905d5d7d8c3eaf1e3e18d4c3d5a10) | 2024-06-06 | xiaojin.cxj | fix rpmsg warning |
| [a7a65cd7](https://github.com/RVCK-Project/rvck/commit/a7a65cd756bad83a56637fb997b5cee1cebd7ac2) | 2024-06-06 | xiaojin.cxj | add c906 audio support |
| [2c01ef3a](https://github.com/RVCK-Project/rvck/commit/2c01ef3aaa0679ad4ff1d074f5f3a4e2969a8f72) | 2024-06-05 | xianbing Zhu | net:stmmac: increase timeout for dma reset |
| [89aae4e6](https://github.com/RVCK-Project/rvck/commit/89aae4e603aa98266dc51cc2586f827d1a57df91) | 2024-06-05 | xianbing Zhu | stmmac:dwmac-thead: add support for suspend/resume feature |
| [069803da](https://github.com/RVCK-Project/rvck/commit/069803da9b0ccede6ec42fcb02fb06b68a667184) | 2024-06-04 | xianbing Zhu | net:dwmac-thead: dd ptp clk set and enable |
| [fd7eecf0](https://github.com/RVCK-Project/rvck/commit/fd7eecf08090e2499bccec0d8a19cc3e6a667d15) | 2024-06-05 | Esther Z | configs: Enable th1520 mailbox. |
| [9ba40fc3](https://github.com/RVCK-Project/rvck/commit/9ba40fc34decd04c6ca644e9c25b3ba499dd9c32) | 2024-06-04 | Esther Z | drivers:ipc: update th1520 rpc msg version 2 |
| [0737371b](https://github.com/RVCK-Project/rvck/commit/0737371b1696a32345ea54847ef166133b7895a7) | 2021-08-10 | fugang.duan | firmware: thead: c910_aon: add th1520 Aon protocol driver |
| [d52faca5](https://github.com/RVCK-Project/rvck/commit/d52faca55ceb4e24171a86a99978065d7a66c9a8) | 2024-06-04 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 add delay line in different mode and sdio rxclk del... |
| [74264d33](https://github.com/RVCK-Project/rvck/commit/74264d334a9cca45fac14eb8ba485459a63fb61b) | 2024-06-03 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 larger tuning max loop count to 128 |
| [1bd0cb0f](https://github.com/RVCK-Project/rvck/commit/1bd0cb0f72961ab257ea29d06400cd785ffb8465) | 2024-05-31 | xianbing Zhu | dts: th1520: enable sdio1 for wifi card in lichee-pi-4a |
| [78a4851f](https://github.com/RVCK-Project/rvck/commit/78a4851f1441feb84997fb54e97f52d231f7151b) | 2024-05-31 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 sdhci add fix io voltage 1v8 |
| [9a5100e3](https://github.com/RVCK-Project/rvck/commit/9a5100e3b89e99e4f326b2584813df8ff72fe753) | 2024-05-30 | xianbing Zhu | mmc:sdhci-of-dwcmshc: th1520 resolve accss rpmb error in hs400 |
| [a11f83be](https://github.com/RVCK-Project/rvck/commit/a11f83bea195968ca84b0f1c9741a7bb3fa4b920) | 2024-05-30 | Xiangyi Zeng | drivers/dmac: add pm suspend/resume for dma driver |
| [d22f6a0b](https://github.com/RVCK-Project/rvck/commit/d22f6a0b83efe44318527d98ddeb01fb756364df) | 2023-08-21 | David Li | audio: th1520: add dma chan str for dmaengine |
| [c645c599](https://github.com/RVCK-Project/rvck/commit/c645c599a9348acf6dd2904cec90d2a8ad720b8b) | 2024-05-30 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 dmac1 and dmac2 device node |
| [6dce75e2](https://github.com/RVCK-Project/rvck/commit/6dce75e2bbf6d752c907291c2a3111182667f9a9) | 2023-08-22 | sanyi | STR: fix pca953x resume bug |
| [4dea520d](https://github.com/RVCK-Project/rvck/commit/4dea520d54934fd6a198c850911510d3643cab52) | 2024-05-28 | Xiangyi Zeng | drivers/iio/adc: add sysfs_remove_file when adc driver removed |
| [71fedd74](https://github.com/RVCK-Project/rvck/commit/71fedd748a8bb5e19efd982072d8b9693b70b251) | 2024-05-27 | Xiangyi Zeng | drivers/pvt: add mr75203 driver pm feature and correct temperature coefficient |
| [f2e92399](https://github.com/RVCK-Project/rvck/commit/f2e92399d4e941057e3c66693d385644e1e57ed7) | 2024-05-27 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 SPI/QSPI device node |
| [1e837a30](https://github.com/RVCK-Project/rvck/commit/1e837a303c35598f8671425c070a310d0d81a129) | 2024-05-27 | Xiangyi Zeng | dt-bindings: spi/qspi: Add Documentation for THEAD TH1520 SPI/QSPI |
| [087bdf2d](https://github.com/RVCK-Project/rvck/commit/087bdf2dfdd89fecf4fe0650cd3252ef62922ab8) | 2024-05-27 | Xiangyi Zeng | drivers/spi: Add THEAD TH1520 QSPI driver |
| [57045791](https://github.com/RVCK-Project/rvck/commit/57045791693881cfab6b4114bcbec9d5f1d43fe8) | 2024-05-27 | tingming | reset: th1520: to support npu/fce reset feature |
| [8d7bf7a4](https://github.com/RVCK-Project/rvck/commit/8d7bf7a4a9c219b1de3faca326097fc17bffda93) | 2024-05-19 | Wei Fu | riscv: dts: thead: Add XuanTie TH1520 Mailbox device node |
| [7c894d72](https://github.com/RVCK-Project/rvck/commit/7c894d72a9ae0de7cd3aa1889a7c711c83f417c0) | 2024-05-17 | Fugang Duan | mailbox: add XuanTie TH1520 Mailbox IPC driver |
| [ee9db853](https://github.com/RVCK-Project/rvck/commit/ee9db8538e128570593eb1f16d353883c9cba6f5) | 2024-05-19 | Wei Fu | dt-bindings: mailbox: Add a binding file for XuanTie TH1520 Mailbox |
| [011f8188](https://github.com/RVCK-Project/rvck/commit/011f8188f80e55df92de8aa1822c93fabada4f97) | 2024-05-22 | tingming | riscv: dts: thead: to add th1520 clk nodes |
| [9a916b52](https://github.com/RVCK-Project/rvck/commit/9a916b523829c2d5b453ddea2b122df1f57b8388) | 2024-05-22 | tingming | drivers: clk: to add thead th1520 clk driver |
| [78f9ab12](https://github.com/RVCK-Project/rvck/commit/78f9ab126d19285cfb12be90ab5f0ca0591af175) | 2024-05-17 | Xiangyi Zeng | dt-bindings: adc: Add Documentation for THEAD TH1520 ADC |
| [57d35457](https://github.com/RVCK-Project/rvck/commit/57d35457977cde0f62b3bdc20ce6553968b42021) | 2024-05-17 | Xiangyi Zeng | riscv: dts: thead: Add THEAD TH1520 ADC device node |
| [eb4280a6](https://github.com/RVCK-Project/rvck/commit/eb4280a6420bce19a9311e03e2fd01b917d901a6) | 2024-05-17 | Xiangyi Zeng | drivers/iio/adc: Add THEAD TH1520 ADC driver |
| [7f9d43a7](https://github.com/RVCK-Project/rvck/commit/7f9d43a761a8092859481ced16d6820ec4244caf) | 2024-06-29 | Chen Pei | riscv: ptrace: Fix ptrace using uninitialized riscv_v_vsize |
| [668bef52](https://github.com/RVCK-Project/rvck/commit/668bef52e4b534462285b938ecd99edbc4bced07) | 2024-03-18 | Heiko Stuebner | T-Head C9xx cores implement an older version (0.7.1) of the vector specification... |
---

**共 393 条提交，显示 201-393**

[1](阿里达摩院.md) **[2]**

[显示全部](阿里达摩院_all.md) | [纯文本视图](阿里达摩院_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-06-26 11:06:09*
*数据来源: 主分支 rvck-6.6@ddd010bd*
