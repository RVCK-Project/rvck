# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 354</li>
<li><strong>统计时间</strong>: 2026-05-19 22:52:14</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.137</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [0d61cc41](https://github.com/RVCK-Project/rvck/commit/0d61cc4183e12ec32c6c320d4adbdbd6712aa9e4) | 2025-06-02 | Cyril Bur | riscv: uaccess: Only restore the CSR_STATUS SUM bit |
| [b18ee1aa](https://github.com/RVCK-Project/rvck/commit/b18ee1aa507d9937f395ea0cc01621a608b90a9d) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning in __get_user_error() |
| [0fcaa7b1](https://github.com/RVCK-Project/rvck/commit/0fcaa7b11396801eb2bcc55d2360d48d902aa231) | 2025-06-10 | Palmer Dabbelt | RISC-V: uaccess: Wrap the get_user_8 uaccess macro |
| [7d1e89e6](https://github.com/RVCK-Project/rvck/commit/7d1e89e647813d27ef3d1ce1d1ee62ca66051016) | 2025-07-25 | Aurelien Jarno | riscv: uaccess: fix __put_user_nocheck for unaligned accesses |
| [ebff58ec](https://github.com/RVCK-Project/rvck/commit/ebff58ec1d661fe30101bdb5ba918524dd0a8c87) | 2025-07-15 | Nathan Chancellor | riscv: uaccess: Fix -Wuninitialized and -Wshadow in __put_user_nocheck |
| [37b33f86](https://github.com/RVCK-Project/rvck/commit/37b33f86ef7fb40450103fb07e0534299c61e1a9) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning about different address spaces |
| [cb2a9c3d](https://github.com/RVCK-Project/rvck/commit/cb2a9c3d49635f8f440b11a55c32a08f43ad0fc1) | 2025-08-05 | Radim Krčmář | RISC-V: KVM: fix stack overrun when loading vlenb |
| [7594581c](https://github.com/RVCK-Project/rvck/commit/7594581cd2552b5dda28a6fab87479f8a27ee611) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: add 'vlenb' Vector CSR |
| [8485afb4](https://github.com/RVCK-Project/rvck/commit/8485afb45e256c46a4901a0549007c9528e7f490) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: set 'vlenb' in kvm_riscv_vcpu_alloc_vector_context() |
| [f62dc7fb](https://github.com/RVCK-Project/rvck/commit/f62dc7fbae97b0179ad0ff3bda5c74f9344e4908) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use NACL HFENCEs for KVM request based HFENCEs |
| [c8034db8](https://github.com/RVCK-Project/rvck/commit/c8034db86a2dd4e03237bff6572b3c2fb3f8e777) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save trap CSRs in kvm_riscv_vcpu_enter_exit() |
| [8f54dc9a](https://github.com/RVCK-Project/rvck/commit/8f54dc9ab817d88e192692def0c7c5685cb2b2dd) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use SBI sync SRET call when available |
| [3789f069](https://github.com/RVCK-Project/rvck/commit/3789f0698b9851d18a8f188e5c4ae23dcf7827f8) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing AIA CSRs |
| [b71f61e9](https://github.com/RVCK-Project/rvck/commit/b71f61e9df3901e4742848f130caf0938072c0b7) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing H-extension CSRs |
| [565dc78d](https://github.com/RVCK-Project/rvck/commit/565dc78d15e2c2082c7060a7c04cc9aad6647268) | 2024-10-21 | Anup Patel | RISC-V: KVM: Add common nested acceleration support |
| [0389c882](https://github.com/RVCK-Project/rvck/commit/0389c88247d39ad096261938c89644600029d201) | 2024-10-21 | Anup Patel | RISC-V: Add defines for the SBI nested acceleration extension |
| [15596fc5](https://github.com/RVCK-Project/rvck/commit/15596fc57a77e728cecfa5998f335e1ddc403052) | 2024-10-21 | Anup Patel | RISC-V: KVM: Don't setup SGEI for zero guest external interrupts |
| [d68ba0cc](https://github.com/RVCK-Project/rvck/commit/d68ba0cc8636c89dfda7cd01559c0fc178677018) | 2024-10-21 | Anup Patel | RISC-V: KVM: Replace aia_set_hvictl() with aia_hvictl_value() |
| [ac541553](https://github.com/RVCK-Project/rvck/commit/ac541553accac4ff087ae2ccefc9cf4c176e2a64) | 2024-10-21 | Anup Patel | RISC-V: KVM: Break down the __kvm_riscv_switch_to() into macros |
| [a9cc9857](https://github.com/RVCK-Project/rvck/commit/a9cc98571cb7eb3c0f71bf7f350f641841f49388) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore SCOUNTEREN in C source |
| [e8f9dd70](https://github.com/RVCK-Project/rvck/commit/e8f9dd708c76e70a32b91b29a0d6a5b7bf9887c5) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore HSTATUS in C source |
| [dd9fea00](https://github.com/RVCK-Project/rvck/commit/dd9fea00eec1108d570404da2963579a58d5331c) | 2024-10-21 | Anup Patel | RISC-V: KVM: Order the object files alphabetically |
| [6f2a0f2a](https://github.com/RVCK-Project/rvck/commit/6f2a0f2a86bfa26c839eba87eb47cc494ca752ef) | 2024-07-08 | Jinjie Ruan | riscv: stacktrace: Add USER_STACKTRACE support |
| [2028fcb2](https://github.com/RVCK-Project/rvck/commit/2028fcb23b5614c8d8f6c1f7ce764f123b1fcfed) | 2023-10-24 | Clément Léger | riscv: kvm: use ".L" local labels in assembly when applicable |
| [c0e6d0ce](https://github.com/RVCK-Project/rvck/commit/c0e6d0cee796a526846c083477ad53349142cfd8) | 2023-10-24 | Clément Léger | riscv: kvm: Use SYM_*() assembly macros instead of deprecated ones |
| [933e4a9c](https://github.com/RVCK-Project/rvck/commit/933e4a9cb7b18d70e155ffee09aab6ef521fd5e5) | 2025-08-18 | Anup Patel | MAINTAINERS: Add entry for RISC-V RPMI and MPXY drivers |
| [a5d9a135](https://github.com/RVCK-Project/rvck/commit/a5d9a135a18ff6fbf14eabdf54ea8c4834f18ec9) | 2025-08-18 | Anup Patel | RISC-V: Enable GPIO keyboard and event device in RV64 defconfig |
| [c76f43e3](https://github.com/RVCK-Project/rvck/commit/c76f43e338716b673293579d96da75fbb0519f72) | 2025-08-18 | Sunil V L | irqchip/riscv-rpmi-sysmsi: Add ACPI support |
| [16d85c74](https://github.com/RVCK-Project/rvck/commit/16d85c74497780109dce1b4933b8dd8f64eecd9f) | 2025-08-18 | Sunil V L | mailbox/riscv-sbi-mpxy: Add ACPI support |
| [2e75a7db](https://github.com/RVCK-Project/rvck/commit/2e75a7dbc418edab617b18f3a9bd37f3921a4dfa) | 2025-08-18 | Sunil V L | irqchip/irq-riscv-imsic-early: Export imsic_acpi_get_fwnode() |
| [83be1294](https://github.com/RVCK-Project/rvck/commit/83be12940a51fc5378d785fc366d68699938d679) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add RPMI System MSI to GSI mapping |
| [7378a6d9](https://github.com/RVCK-Project/rvck/commit/7378a6d9f10260ffd956d79d646d463cc74e8f0d) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add support to update gsi range |
| [eab9e5cd](https://github.com/RVCK-Project/rvck/commit/eab9e5cd338544e3dbfa838df0142c51909bb13e) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Create interrupt controller list in sorted order |
| [6630d45a](https://github.com/RVCK-Project/rvck/commit/6630d45adbb43fa29460f7edd079da66f8ff4ac0) | 2025-08-18 | Sunil V L | ACPI: scan: Update honor list for RPMI System MSI |
| [da3e0291](https://github.com/RVCK-Project/rvck/commit/da3e0291b46eff02feea4b2be0f00ab7a539795e) | 2025-08-18 | Sunil V L | ACPI: Add support for nargs_prop in acpi_fwnode_get_reference_args() |
| [8399433e](https://github.com/RVCK-Project/rvck/commit/8399433e7e357e5c4e5ddc965f7152d8aa8bcec4) | 2025-08-18 | Sunil V L | ACPI: property: Refactor acpi_fwnode_get_reference_args() to support nargs_prop |
| [a4f18aec](https://github.com/RVCK-Project/rvck/commit/a4f18aeca21f2ec739f45dbc64bb133373c63e30) | 2025-08-18 | Anup Patel | irqchip: Add driver for the RPMI system MSI service group |
| [3dac755c](https://github.com/RVCK-Project/rvck/commit/3dac755c88b648e7c01a64a498a28e06f89b6e72) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI interrupt controller bindings |
| [136bcb27](https://github.com/RVCK-Project/rvck/commit/136bcb2734c93591bac30b0c0109ed7b076481f2) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI message proxy bindings |
| [b56b39b8](https://github.com/RVCK-Project/rvck/commit/b56b39b895412905d444f35ce96a39c8ce5a87fe) | 2025-08-18 | Rahul Pathak | clk: Add clock driver for the RISC-V RPMI clock service group |
| [917c8910](https://github.com/RVCK-Project/rvck/commit/917c8910409c68197660e821a79644f5b814d7df) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service controller bindings |
| [1c7a0b32](https://github.com/RVCK-Project/rvck/commit/1c7a0b32e974b567e956d22f3708fe23800bb938) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service message proxy bindings |
| [8e47f6cc](https://github.com/RVCK-Project/rvck/commit/8e47f6cc0385bf560d93718970dc1d4d43785f1e) | 2025-08-18 | Anup Patel | mailbox: Add RISC-V SBI message proxy (MPXY) based mailbox driver |
| [e194b862](https://github.com/RVCK-Project/rvck/commit/e194b862c9742458965273aeab8c8fa5935eac11) | 2025-08-18 | Anup Patel | byteorder: Add memcpy_to_le32() and memcpy_from_le32() |
| [d2a6a3c3](https://github.com/RVCK-Project/rvck/commit/d2a6a3c3b551c639f4488c3020283ad2970e9752) | 2025-08-18 | Anup Patel | mailbox: Add common header for RPMI messages sent via mailbox |
| [afae9654](https://github.com/RVCK-Project/rvck/commit/afae9654c354ee27cdc16e04ce380dd5a5d0ca9f) | 2025-08-18 | Anup Patel | RISC-V: Add defines for the SBI message proxy extension |
| [984112d1](https://github.com/RVCK-Project/rvck/commit/984112d1f3ce373f0d2dc625132523993cd6d01c) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RISC-V SBI MPXY extension |
| [cbf2316c](https://github.com/RVCK-Project/rvck/commit/cbf2316cd8e90d3b7a1c7ff10f2caeb9b87b0285) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RPMI shared memory transport |
| [b02bdc46](https://github.com/RVCK-Project/rvck/commit/b02bdc462ca7620f6ad86658c5fb7f164dc94143) | 2025-02-12 | Andy Shevchenko | driver core: Split devres APIs to device/devres.h |
| [47dd4bf3](https://github.com/RVCK-Project/rvck/commit/47dd4bf3cdf043fb82d535ebe86b08caa9f4f589) | 2023-12-26 | Tanzir Hasan | lib/string: shrink lib/string.i via IWYU |
| [53d5cd6a](https://github.com/RVCK-Project/rvck/commit/53d5cd6aef5517b3501904868538816f74c8dc09) | 2023-12-26 | Tanzir Hasan | kernel.h: removed REPEAT_BYTE from kernel.h |
| [56461356](https://github.com/RVCK-Project/rvck/commit/56461356fdb5defd9701dda3a1f0f22acb2fa504) | 2024-06-06 | Nuno Sa | dev_printk: add new dev_err_probe() helpers |
| [cd5e0b4e](https://github.com/RVCK-Project/rvck/commit/cd5e0b4ee604f3e83322c2309014f51773013355) | 2025-04-09 | Sakari Ailus | Documentation: ACPI: Use all-string data node references |
| [24ca848c](https://github.com/RVCK-Project/rvck/commit/24ca848c25159ae001312c756800c73f2801b420) | 2023-11-06 | Rafael J. Wysocki | ACPI: property: Support using strings in reference properties |
| [adf790e3](https://github.com/RVCK-Project/rvck/commit/adf790e3ca3a2f12b62894dcac71c77770d938d8) | 2025-06-20 | Nam Cao | Revert "riscv: misaligned: fix sleeping function called during misaligned access... |
| [c0b12076](https://github.com/RVCK-Project/rvck/commit/c0b120766f750f1783d1ff4a357d9757b1326675) | 2025-06-20 | Alexandre Ghiti | riscv: Move all duplicate insn parsing macros into asm/insn.h |
| [0a3da80b](https://github.com/RVCK-Project/rvck/commit/0a3da80ba2b50842ab63d4de5e44705dab4e57b7) | 2025-06-20 | Alexandre Ghiti | riscv: Strengthen duplicate and inconsistent definition of RV_X() |
| [efbfb4bd](https://github.com/RVCK-Project/rvck/commit/efbfb4bd1f73b12382c5808c1bf8a3331b77c6e9) | 2025-06-20 | Alexandre Ghiti | riscv: Fix typo EXRACT -\> EXTRACT |
| [6ab31c31](https://github.com/RVCK-Project/rvck/commit/6ab31c312dadca151ed43a219b6094330fa8012b) | 2025-07-10 | Andreas Schwab | riscv: traps_misaligned: properly sign extend value in misaligned load handler |
| [b1096006](https://github.com/RVCK-Project/rvck/commit/b10960064ba4a951e7fc756e00ee77be2a7766e3) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for SBI_FWFT_MISALIGNED_DELEG |
| [51dcb004](https://github.com/RVCK-Project/rvck/commit/51dcb00495710c1cd50d1f6589bf5718a4d865e7) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for FWFT SBI extension |
| [71a12492](https://github.com/RVCK-Project/rvck/commit/71a12492c24203daf23a840e4bba028c35304bc0) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension reset callback |
| [50a7da91](https://github.com/RVCK-Project/rvck/commit/50a7da911a4dc72897497116d74928e9b08d0d74) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension init()/deinit() functions |
| [e0cc1501](https://github.com/RVCK-Project/rvck/commit/e0cc1501a790e049c374d9235609143b2e616f4a) | 2025-05-23 | Clément Léger | riscv: misaligned: add a function to check misalign trap delegability |
| [6575dea5](https://github.com/RVCK-Project/rvck/commit/6575dea5b57ff4a359e6096267979d9314f19fa7) | 2025-05-23 | Clément Léger | riscv: misaligned: move emulated access uniformity check in a function |
| [65a13367](https://github.com/RVCK-Project/rvck/commit/65a13367e0f7a18332d156ec3e2142654d00aca8) | 2025-05-23 | Clément Léger | riscv: misaligned: declare misaligned_access_speed under CONFIG_RISCV_MISALIGNED |
| [0af94e92](https://github.com/RVCK-Project/rvck/commit/0af94e92afb01257a7fec14b040dafb3e66d898a) | 2025-05-23 | Clément Léger | riscv: misaligned: use on_each_cpu() for scalar misaligned access probing |
| [06029835](https://github.com/RVCK-Project/rvck/commit/0602983543628ab9991b44ffe163d756e620c136) | 2025-05-23 | Clément Léger | riscv: misaligned: request misaligned exception from SBI |
| [85c0252e](https://github.com/RVCK-Project/rvck/commit/85c0252ef7b499c74717c4ca860e1eb34b4eb055) | 2025-05-23 | Clément Léger | riscv: sbi: add SBI FWFT extension calls |
| [6bed28b2](https://github.com/RVCK-Project/rvck/commit/6bed28b2512b93c19dc8a34d7be2f78a6963a7e5) | 2025-05-23 | Clément Léger | riscv: sbi: add FWFT extension interface |
| [cf91bb37](https://github.com/RVCK-Project/rvck/commit/cf91bb373d3602dc329141cc5d4d633bae2394ae) | 2025-05-23 | Clément Léger | riscv: sbi: add new SBI error mappings |
| [c3a76d6a](https://github.com/RVCK-Project/rvck/commit/c3a76d6aee72b78ae6134775c79ffb4ceec06a72) | 2025-05-23 | Clément Léger | riscv: sbi: remove useless parenthesis |
| [77f3b970](https://github.com/RVCK-Project/rvck/commit/77f3b9704556f3fecb99203926c49e1003ff7d01) | 2025-05-23 | Clément Léger | riscv: sbi: add Firmware Feature (FWFT) SBI extensions definitions |
| [f58b6e0a](https://github.com/RVCK-Project/rvck/commit/f58b6e0a4512b407bf298c0faa0a5d379c33551f) | 2024-04-02 | Chao Du | RISC-V: KVM: selftests: Add ebreak test support |
| [9c429968](https://github.com/RVCK-Project/rvck/commit/9c4299683cbd7254907717f4b5009fe1afadacc0) | 2024-04-02 | Chao Du | RISC-V: KVM: Handle breakpoint exits for VCPU |
| [1f346814](https://github.com/RVCK-Project/rvck/commit/1f346814df0d097a2ba829103f273885cd7de780) | 2024-04-02 | Chao Du | RISC-V: KVM: Implement kvm_arch_vcpu_ioctl_set_guest_debug() |
| [63d91530](https://github.com/RVCK-Project/rvck/commit/63d9153093169513e0756c05cd0cee25128b6b18) | 2024-01-11 | Paolo Bonzini | KVM: define __KVM_HAVE_GUEST_DEBUG unconditionally |
| [7ef4be74](https://github.com/RVCK-Project/rvck/commit/7ef4be7404cb2d25175d510909d458df9fff6faf) | 2025-04-09 | Andrew Jones | riscv: Fix unaligned access info messages |
| [5d6fe55d](https://github.com/RVCK-Project/rvck/commit/5d6fe55d71f27e06f60be836c6916a72876de89e) | 2024-08-29 | Alexandre Ghiti | riscv: Fix RISCV_ALTERNATIVE_EARLY |
| [b23cd9e7](https://github.com/RVCK-Project/rvck/commit/b23cd9e76c468c863e0c7d9715c2f700950a561c) | 2024-03-22 | Alexandre Ghiti | riscv: Improve sbi_ecall() code generation by reordering arguments |
| [49b9cabc](https://github.com/RVCK-Project/rvck/commit/49b9cabc04c563ecac3f301815597660a00ebb9d) | 2024-03-21 | Samuel Holland | riscv: Add tracepoints for SBI calls and returns |
| [18991d00](https://github.com/RVCK-Project/rvck/commit/18991d00b1317d002faf07e4d04ce403926d234f) | 2025-04-22 | Clément Léger | riscv: misaligned: use get_user() instead of __get_user() |
| [33611e4b](https://github.com/RVCK-Project/rvck/commit/33611e4bddd032bc210a8b2128f6affbc4371d5f) | 2025-04-22 | Clément Léger | riscv: misaligned: enable IRQs while handling misaligned accesses |
| [fa8b0794](https://github.com/RVCK-Project/rvck/commit/fa8b079416782b34d39f2c04a64ba30e1e4ef39c) | 2025-04-22 | Clément Léger | riscv: misaligned: factorize trap handling |
| [ac91d940](https://github.com/RVCK-Project/rvck/commit/ac91d94048862e03d2b80c3a65f288254543f925) | 2025-04-11 | Nylon Chen | riscv: misaligned: Add handling for ZCB instructions |
| [66a72b84](https://github.com/RVCK-Project/rvck/commit/66a72b841621218a2540198a87550e5bede3bed8) | 2025-04-11 | Nylon Chen | riscv: misaligned: fix sleeping function called during misaligned access handlin... |
| [9039662d](https://github.com/RVCK-Project/rvck/commit/9039662dc7d1cdcff405182381b868cc58b09783) | 2025-02-28 | Alexandre Ghiti | riscv: Fix missing __free_pages() in check_vector_unaligned_access() |
| [d4b6faab](https://github.com/RVCK-Project/rvck/commit/d4b6faab1e69808b507058695b16f92185b1ef9b) | 2025-02-28 | Tingbo Liao | riscv: Fix the __riscv_copy_vec_words_unaligned implementation |
| [5c932c87](https://github.com/RVCK-Project/rvck/commit/5c932c87d9a8410c73b89a1b70bcc7dbe461f4ec) | 2025-10-20 | Xu Lu | RISC-V: KVM: Allow Zalasr extensions for Guest/VM |
| [b311d8b2](https://github.com/RVCK-Project/rvck/commit/b311d8b298ce1d24651e46712e2c794c611218de) | 2025-07-24 | Aleksa Paunovic | riscv: hwprobe: Add MIPS vendor extension probing |
| [016698c5](https://github.com/RVCK-Project/rvck/commit/016698c5329a57a6544439a00da2d91ea8942a94) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Add SiFive vendor extension support and probe for xsfqmaccdod an... |
| [274a1c3b](https://github.com/RVCK-Project/rvck/commit/274a1c3bc4ec1f34a90403d06b93b056fb68a092) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Document SiFive xsfvqmaccdod and xsfvqmaccqoq vendor extensions |
| [5e501b5a](https://github.com/RVCK-Project/rvck/commit/5e501b5a81ccced560a675f5b19c3b7173c1a879) | 2024-11-13 | Charlie Jenkins | riscv: hwprobe: Add thead vendor extension probing |
| [aca9fc58](https://github.com/RVCK-Project/rvck/commit/aca9fc580a54367e86ec97d652aaffae4afa8932) | 2025-10-20 | Xu Lu | riscv: Introduce Zalasr instructions |
| [cb54b1f7](https://github.com/RVCK-Project/rvck/commit/cb54b1f73b1c5928398aa6a11d5348955643dbfb) | 2025-10-20 | Xu Lu | riscv: hwprobe: Export Zalasr extension |
| [71697602](https://github.com/RVCK-Project/rvck/commit/71697602fcee31370592e9e98032bb6ca2ece685) | 2025-10-20 | Xu Lu | dt-bindings: riscv: Add Zalasr ISA extension description |
| [962ecab6](https://github.com/RVCK-Project/rvck/commit/962ecab636fa305edebb147a1e63efa2b13261a7) | 2025-10-20 | Xu Lu | riscv: Add ISA extension parsing for Zalasr |
| [79dbd1ca](https://github.com/RVCK-Project/rvck/commit/79dbd1cab8e805131ebe88aaa8ce6331abb57372) | 2025-04-21 | Alexandre Ghiti | riscv: hwprobe: export Zabha extension |
| [bf5d008d](https://github.com/RVCK-Project/rvck/commit/bf5d008d49254c34cc35b71873a6f398996bb864) | 2024-03-01 | Samuel Holland | riscv: mm: Fix prototype to avoid discarding const |
| [e08114fa](https://github.com/RVCK-Project/rvck/commit/e08114faa62d14ebc908ca528dd83162ce9f849e) | 2024-01-30 | Alexandre Ghiti | riscv: Fix arch_tlbbatch_flush() by clearing the batch cpumask |
| [1a54196c](https://github.com/RVCK-Project/rvck/commit/1a54196c7525e78485c5afa7cc881e5c39e25f2c) | 2024-01-08 | Alexandre Ghiti | riscv: Add support for BATCHED_UNMAP_TLB_FLUSH |
| [0c1b1c22](https://github.com/RVCK-Project/rvck/commit/0c1b1c229fe45e30adfade648ceb0646290052d8) | 2023-10-30 | Alexandre Ghiti | riscv: Improve flush_tlb_range() for hugetlb pages |
| [9ec9ccd5](https://github.com/RVCK-Project/rvck/commit/9ec9ccd5e4f5b5f38399a625548057bb60ccfee5) | 2026-02-05 | shenlin | perf vendor events riscv: fix lrw core PMU event mapping |
| [e341c76b](https://github.com/RVCK-Project/rvck/commit/e341c76bf553bff7c20b3aa6d935fc435109d29b) | 2024-06-05 | Björn Töpel | riscv: Enable DAX VMEMMAP optimization |
| [3de689ee](https://github.com/RVCK-Project/rvck/commit/3de689ee326afa265289f24248c47666797d8190) | 2024-06-05 | Björn Töpel | riscv: mm: Add support for ZONE_DEVICE |
| [a87dba9d](https://github.com/RVCK-Project/rvck/commit/a87dba9db905d947e5475def349f7ef866516cb7) | 2024-06-05 | Björn Töpel | virtio-mem: Enable virtio-mem for RISC-V |
| [57387a49](https://github.com/RVCK-Project/rvck/commit/57387a49cf4d99dc9f7c4a164c2e7c3b14a7df63) | 2024-06-05 | Björn Töpel | riscv: Enable memory hotplugging for RISC-V |
| [0befad6b](https://github.com/RVCK-Project/rvck/commit/0befad6b98fa32f31a5d8e0a08f0299f56e08dc3) | 2024-06-05 | Björn Töpel | riscv: mm: Take memory hotplug read-lock during kernel page table dump |
| [41dc690c](https://github.com/RVCK-Project/rvck/commit/41dc690cc5399e7ff2c9beb8791f467e31bbb546) | 2024-06-05 | Björn Töpel | riscv: mm: Add memory hotplugging support |
| [697e7de5](https://github.com/RVCK-Project/rvck/commit/697e7de56352200c7ba3f0e0c04fea001ff81d06) | 2024-06-05 | Björn Töpel | riscv: mm: Add pfn_to_kaddr() implementation |
| [cd4f49c6](https://github.com/RVCK-Project/rvck/commit/cd4f49c6ae0610d0f814e1a9b2fc74521b95d90a) | 2024-06-05 | Björn Töpel | riscv: mm: Refactor create_linear_mapping_range() for memory hot add |
| [c80aa643](https://github.com/RVCK-Project/rvck/commit/c80aa643f570f20d6a86fc5ca7f0aec1b8745fb6) | 2024-06-05 | Björn Töpel | riscv: mm: Change attribute from __init to __meminit for page functions |
| [b81a2e60](https://github.com/RVCK-Project/rvck/commit/b81a2e608c1e12c516c0c7fc0c948965a70dadcc) | 2024-06-05 | Björn Töpel | riscv: mm: Pre-allocate vmemmap/direct map/kasan PGD entries |
| [9142fcae](https://github.com/RVCK-Project/rvck/commit/9142fcaed0c64bb67e2fc54931e2e676c7d04c18) | 2024-06-05 | Björn Töpel | riscv: mm: Properly forward vmemmap_populate() altmap parameter |
| [5835995b](https://github.com/RVCK-Project/rvck/commit/5835995b4744bb9270df30498f1f2a4dfc6533d7) | 2023-12-14 | Alexandre Ghiti | riscv: Use hugepage mappings for vmemmap |
| [061f1788](https://github.com/RVCK-Project/rvck/commit/061f17887189690207cf25c455568ded2cf13245) | 2023-11-06 | Evan Green | RISC-V: Probe misaligned access speed in parallel |
| [04747dbe](https://github.com/RVCK-Project/rvck/commit/04747dbeb001ce186ee83570130afda1fb1d7c1e) | 2023-11-06 | Evan Green | RISC-V: Remove __init on unaligned_emulation_finish() |
| [eaa7a89c](https://github.com/RVCK-Project/rvck/commit/eaa7a89c02eadb96ebfc8457e57b82e81cbdee2e) | 2023-10-04 | Clément Léger | riscv: add support for PR_SET_UNALIGN and PR_GET_UNALIGN |
| [d5f5fda0](https://github.com/RVCK-Project/rvck/commit/d5f5fda0cdf7dd075a43932f26dfdedfb4c33d09) | 2023-10-04 | Clément Léger | riscv: report misaligned accesses emulation to hwprobe |
| [044a7ae3](https://github.com/RVCK-Project/rvck/commit/044a7ae35c296c28d49cab329a2bdb3f9b0b2e3f) | 2023-10-04 | Clément Léger | riscv: add support for sysctl unaligned_enabled control |
| [f2ef5550](https://github.com/RVCK-Project/rvck/commit/f2ef55502e3963c1574c3291d7b01e57944eb505) | 2023-10-04 | Clément Léger | riscv: add floating point insn support to misaligned access emulation |
| [bf0ce92e](https://github.com/RVCK-Project/rvck/commit/bf0ce92ef0832446d9c2e198d0ba0a65f81704cf) | 2023-10-04 | Clément Léger | riscv: report perf event for misaligned fault |
| [d4ea122e](https://github.com/RVCK-Project/rvck/commit/d4ea122e40b7e330625703902d35ec1617c3db69) | 2023-10-04 | Clément Léger | riscv: add support for misaligned trap handling in S-mode |
| [ee788547](https://github.com/RVCK-Project/rvck/commit/ee7885478604f66d99a47bd73d3360d5b145f3dc) | 2026-01-17 | Lu Peng | riscv: defconfig: Enable more ACPI_APEI configs |
| [563fe63a](https://github.com/RVCK-Project/rvck/commit/563fe63a87be0a9f5421d3cb3a7394c088a9c171) | 2025-07-23 | Ignacio Encinas | riscv: introduce asm/swab.h |
| [1a87ce1c](https://github.com/RVCK-Project/rvck/commit/1a87ce1c8429c82a8a660b1a679bba1d254cac7f) | 2026-01-07 | hu.yuye | riscv:defconfig:Enable PCIE_EDR |
| [5ffef145](https://github.com/RVCK-Project/rvck/commit/5ffef14553a584bf96d5917bd63045db7085e47e) | 2025-12-31 | Yunhui Cui | arch_topology: move parse_acpi_topology() to common code |
| [d61034e9](https://github.com/RVCK-Project/rvck/commit/d61034e90defebb0d0880a46f851579de39a957e) | 2025-12-31 | Yicong Yang | arm64: topology: Support SMT control on ACPI based system |
| [d94ec4ae](https://github.com/RVCK-Project/rvck/commit/d94ec4aedf5e5dbed3e9b1e84135c58d686938bf) | 2025-12-31 | Yicong Yang | arch_topology: Support SMT control for OF based system |
| [a8ab66c4](https://github.com/RVCK-Project/rvck/commit/a8ab66c4f3566c77d78965a488cae0902abac123) | 2025-12-31 | Yicong Yang | cpu/SMT: Provide a default topology_is_primary_thread() |
| [6f3bec19](https://github.com/RVCK-Project/rvck/commit/6f3bec19e067c44035e56b7fc30af996aea86ff9) | 2025-01-24 | Andy Shevchenko | serial: 8250_core: Remove unneeded -\>iotype assignment |
| [9f19701b](https://github.com/RVCK-Project/rvck/commit/9f19701b75f172ed5fb91ff770baae0904fb9885) | 2025-12-30 | hu.yuye | Revert "mango pci hack:broadcast when no MSI source known" |
| [5d17c0c0](https://github.com/RVCK-Project/rvck/commit/5d17c0c00b36b7df88683f45517c5d5935398251) | 2025-12-22 | Clément Léger | riscv: uaccess: do not do misaligned accesses in get/put_user() |
| [a4f91b2d](https://github.com/RVCK-Project/rvck/commit/a4f91b2db352efd6f1dac249ca020118eb210e12) | 2025-12-22 | Alexandre Ghiti | riscv: make unsafe user copy routines use existing assembly routines |
| [10947361](https://github.com/RVCK-Project/rvck/commit/109473618de26afdde39691aeafdc26105027212) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm_goto_output' for get_user() |
| [0145277b](https://github.com/RVCK-Project/rvck/commit/0145277ba12d1b48b5ee73cb0dd06144f9779e35) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm goto' for put_user() |
| [821e2497](https://github.com/RVCK-Project/rvck/commit/821e24975a82b4f49ad3ab31281383b88fc6dab0) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use input constraints for ptr of __put_user() |
| [299ce27a](https://github.com/RVCK-Project/rvck/commit/299ce27ac488939ffa697f97b2cca8574631de8a) | 2025-12-22 | Jisheng Zhang | riscv: implement user_access_begin() and families |
| [87db66db](https://github.com/RVCK-Project/rvck/commit/87db66dbd3aabc3322f11d0ef9c70412fe16c136) | 2025-12-22 | Ben Dooks | riscv: save the SR_SUM status over switches |
| [0570c5e5](https://github.com/RVCK-Project/rvck/commit/0570c5e58f309c9065276421e72dc48077ef9bd1) | 2011-12-08 | Tejun Heo | Revert "mm: Modify __find_max_addr for memory hole" |
| [4f31a1d5](https://github.com/RVCK-Project/rvck/commit/4f31a1d5af6560f8b61b01ffe570cbc01f646497) | 2021-12-06 | Alexandre Ghiti | Revert "riscv: mm: Clear compilation warning about last_cpupid" |
| [de1a03c1](https://github.com/RVCK-Project/rvck/commit/de1a03c158c92d3d92cd7588cdf7129c508b9653) | 2025-11-12 | shenlin | perf vendor events riscv: add lrw core JSON file with metric support |
| [b8746a45](https://github.com/RVCK-Project/rvck/commit/b8746a4571b529abd0ea609c914ca7a1b17f55c7) | 2025-11-11 | Fei Liu | i2c: Add driver for the LRW I2C |
| [49dd7737](https://github.com/RVCK-Project/rvck/commit/49dd7737f5be862e9c8b322ff80e5ca8868fe133) | 2025-09-29 | Fei Liu | dt-bindings: i2c: Add binding for LRW I2C |
| [bb0e485e](https://github.com/RVCK-Project/rvck/commit/bb0e485e24db182b227ea751418576c50bde7c75) | 2025-11-12 | Jie Feng | drivers/perf: add LRW DDR PMU support |
| [a8872a55](https://github.com/RVCK-Project/rvck/commit/a8872a55ca04c4cfffdde04f100fa60ff6422e06) | 2025-09-04 | Wenhong Liu | serial: Add driver for the LRW UART |
| [70ca715f](https://github.com/RVCK-Project/rvck/commit/70ca715fd6f0e1a410e7ada6f1324b484690e791) | 2025-09-04 | Wenhong Liu | dt-bindings: serial: Add binding for LRW UART |
| [b807b1f2](https://github.com/RVCK-Project/rvck/commit/b807b1f2d24703672a61a405a918367380bbfe14) | 2025-10-16 | Wenhong Liu | riscv: defconfig: remove CONFIG_CMDLINE and CONFIG_CMDLINE_EXTEND as mainline do... |
| [f67e33a0](https://github.com/RVCK-Project/rvck/commit/f67e33a00d5b82e02d00175982bd9aa74772d43d) | 2025-08-27 | Himanshu Chauhan | riscv: Enable APEI and NMI safe cmpxchg options required for RAS |
| [de4813bb](https://github.com/RVCK-Project/rvck/commit/de4813bb4ccf3cd1e072da9ac8a277a9668eb35f) | 2025-08-27 | Himanshu Chauhan | riscv: Add config option to enable APEI SSE handler |
| [283c9584](https://github.com/RVCK-Project/rvck/commit/283c958429c07f12df8f528b9862b27476528456) | 2025-08-27 | Himanshu Chauhan | riscv: Introduce HEST SSE notification handlers |
| [19b58782](https://github.com/RVCK-Project/rvck/commit/19b587828b00368f595678b9c0c1b2de9fe0b770) | 2025-08-27 | Himanshu Chauhan | riscv: Add RISC-V entries in processor type and ISA strings |
| [d9182568](https://github.com/RVCK-Project/rvck/commit/d9182568afdbda36982194db7f60dde22003f746) | 2025-08-27 | Himanshu Chauhan | riscv: Add functions to register ghes having SSE notification |
| [4616beb1](https://github.com/RVCK-Project/rvck/commit/4616beb1cb69938490d15df841dec92d3774af1e) | 2025-08-27 | Himanshu Chauhan | riscv: conditionally compile GHES NMI spool function |
| [cc4f9fbf](https://github.com/RVCK-Project/rvck/commit/cc4f9fbfc8fdbfbeea533aadb521d88e023b8d9d) | 2025-08-27 | Himanshu Chauhan | riscv: Add fixmap indices for GHES IRQ and SSE contexts |
| [2fc0a73f](https://github.com/RVCK-Project/rvck/commit/2fc0a73ff751d7ab3082b637270caa012a7ea72d) | 2025-08-27 | Himanshu Chauhan | acpi: Introduce SSE in HEST notification types |
| [9e27c5dc](https://github.com/RVCK-Project/rvck/commit/9e27c5dcf6d99f749ae9c958e6b232436ede20b4) | 2025-08-27 | Himanshu Chauhan | riscv: Define arch_apei_get_mem_attribute for RISC-V |
| [d4a396c1](https://github.com/RVCK-Project/rvck/commit/d4a396c182c163b29006850f5fbf57c4f8847a94) | 2025-08-27 | Himanshu Chauhan | riscv: Define ioremap_cache for RISC-V |
| [59f7a9ec](https://github.com/RVCK-Project/rvck/commit/59f7a9ec2f507a46fc366cc765a4dca37365b08a) | 2025-08-27 | Clément Léger | selftests/riscv: add SSE test module |
| [141f4c80](https://github.com/RVCK-Project/rvck/commit/141f4c807462160dfc6ae92d8ba3ca75b9e7eaa9) | 2025-08-27 | Clément Léger | perf: RISC-V: add support for SSE event |
| [ad5632a1](https://github.com/RVCK-Project/rvck/commit/ad5632a111bb051054a329a18338f3f7debf8242) | 2025-08-27 | Clément Léger | drivers: firmware: add riscv SSE support |
| [d29e89c9](https://github.com/RVCK-Project/rvck/commit/d29e89c990d3f4670982fce79fa1ffd4649c7d46) | 2025-08-27 | Clément Léger | riscv: add support for SBI Supervisor Software Events extension |
| [44361dc4](https://github.com/RVCK-Project/rvck/commit/44361dc4d171c664a84f33561223d736c4c22c2e) | 2025-08-08 | Clément Léger | riscv: add SBI SSE extension definitions |
| [6a9347c5](https://github.com/RVCK-Project/rvck/commit/6a9347c5eac0c1e12d0cd4e245ee344b65407da6) | 2025-10-14 | Sunil V L | iommu/riscv: Add ACPI support |
| [db635f51](https://github.com/RVCK-Project/rvck/commit/db635f51f5a111f58c1021287b6da189d05d1877) | 2025-10-14 | Sunil V L | ACPI: scan: Add support for RISC-V in acpi_iommu_configure_id() |
| [177bf292](https://github.com/RVCK-Project/rvck/commit/177bf292945bb6ee3de199ce1c57bc158c3a0152) | 2025-10-14 | Sunil V L | ACPI: RISC-V: Add support for RIMT |
| [9d305c38](https://github.com/RVCK-Project/rvck/commit/9d305c38ebd661e4787edfe5b037ba6ed812cc8b) | 2025-10-13 | Sunil V L | ACPICA: actbl2: Add definitions for RIMT |
| [748d30eb](https://github.com/RVCK-Project/rvck/commit/748d30eb11f35763bd28293f1e554ed03be5eb67) | 2025-09-20 | shenlin | perf vendor events riscv: add lrw core JSON file |
| [c3e234e2](https://github.com/RVCK-Project/rvck/commit/c3e234e2c8632f72ee569545fbf4399dfd1f56fc) | 2025-04-21 | Alexandre Ghiti | riscv: Add support for Zicbop |
| [fa15f003](https://github.com/RVCK-Project/rvck/commit/fa15f0030917a9e40ad93d81e779d148c384d77d) | 2025-04-21 | Alexandre Ghiti | riscv: Introduce Zicbop instructions |
| [d808742c](https://github.com/RVCK-Project/rvck/commit/d808742c0a94bbae1cb843b0052d55715f4cc48a) | 2025-02-26 | Yunhui Cui | RISC-V: Enable cbo.clean/flush in usermode |
| [47208b6d](https://github.com/RVCK-Project/rvck/commit/47208b6db32536defae4b50151d42e90e58ba4d8) | 2024-08-14 | Samuel Holland | riscv: Add support for per-thread envcfg CSR values |
| [bbdf0e47](https://github.com/RVCK-Project/rvck/commit/bbdf0e479abaa84cf2a94548f2e30eee17b895cb) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zaamo/Zalrsc extensions for Guest/VM |
| [c2bc66ad](https://github.com/RVCK-Project/rvck/commit/c2bc66ad38b43cb07a1b4958bf8852d519d4dc0b) | 2024-06-19 | Clément Léger | riscv: hwprobe: export Zaamo and Zalrsc extensions |
| [ac4af8db](https://github.com/RVCK-Project/rvck/commit/ac4af8dbbd83e71a38ef206a803db2eca4c10c68) | 2024-06-19 | Clément Léger | riscv: add parsing for Zaamo and Zalrsc extensions |
| [326682c5](https://github.com/RVCK-Project/rvck/commit/326682c5d502db271bbe93b5f8d1c5b256178f77) | 2025-02-13 | Inochi Amaoto | riscv: hwprobe: export bfloat16 ISA extension |
| [99a82208](https://github.com/RVCK-Project/rvck/commit/99a82208dcfdaf6a62b9323d2ef4ffd6bcc097b9) | 2025-02-13 | Inochi Amaoto | riscv: add ISA extension parsing for bfloat16 ISA extension |
| [4f306687](https://github.com/RVCK-Project/rvck/commit/4f306687884655dd68f4e3a71e4e552ad20632f0) | 2024-05-24 | Xiao Wang | riscv, bpf: Introduce shift add helper with Zba optimization |
| [81958ac3](https://github.com/RVCK-Project/rvck/commit/81958ac36a62eed13fd597f02f1918b000eaba9a) | 2024-05-16 | Xiao Wang | riscv, bpf: Optimize zextw insn with Zba extension |
| [b64bb09f](https://github.com/RVCK-Project/rvck/commit/b64bb09f7a7b63bd693e811af484acf2a54c053b) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize bswap insns with Zbb support |
| [0f671a96](https://github.com/RVCK-Project/rvck/commit/0f671a96eeb586c2b4581dacc0335b88edbca11d) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize sign-extention mov insns with Zbb support |
| [ccc6bc0c](https://github.com/RVCK-Project/rvck/commit/ccc6bc0cac1f644421344721400af55ad9c4fbb2) | 2024-01-15 | Pu Lehui | riscv, bpf: Add necessary Zbb instructions |
| [9d83d620](https://github.com/RVCK-Project/rvck/commit/9d83d6202783c1aca35d71b8e43111282ae2c3b2) | 2024-01-15 | Pu Lehui | riscv, bpf: Simplify sext and zext logics in branch instructions |
| [9d99253f](https://github.com/RVCK-Project/rvck/commit/9d99253fa7180ea49e7ef407e651bb45f0f7494d) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit zero-extension to emit_zextw |
| [4e8ecf0d](https://github.com/RVCK-Project/rvck/commit/4e8ecf0d6e2d1f27580f900bbae62f6dfadc507f) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit sign-extension to emit_sextw |
| [5cc12bc0](https://github.com/RVCK-Project/rvck/commit/5cc12bc0aea2a98e81f68f661a900c343bf6c658) | 2025-03-12 | Robin Murphy | iommu: Don't warn prematurely about dodgy probes |
| [a8e6b5f6](https://github.com/RVCK-Project/rvck/commit/a8e6b5f602b1a9dcf5f8aa0ec3b5a9c6e72d6868) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_domain_alloc() |
| [9763d022](https://github.com/RVCK-Project/rvck/commit/9763d02203ec2eabc5e4073bc91d8f2f4068ee04) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_present() |
| [e7cf4807](https://github.com/RVCK-Project/rvck/commit/e7cf480730a39a495609be974e946b9b33312e44) | 2024-09-02 | Lu Baolu | drm/tegra: Use iommu_paging_domain_alloc() |
| [4e626a55](https://github.com/RVCK-Project/rvck/commit/4e626a552a6ef2100c9e52dd06b8f3815a620834) | 2024-09-02 | Lu Baolu | drm/rockchip: Use iommu_paging_domain_alloc() |
| [6733bcce](https://github.com/RVCK-Project/rvck/commit/6733bccea42325e7b57863836c1359152b209c35) | 2024-06-10 | Lu Baolu | RDMA/usnic: Use iommu_paging_domain_alloc() |
| [e38238ba](https://github.com/RVCK-Project/rvck/commit/e38238ba723723cca8788729898d9e655f59898d) | 2024-08-12 | Lu Baolu | soc: fsl: qbman: Use iommu_paging_domain_alloc() |
| [b6d6ddfb](https://github.com/RVCK-Project/rvck/commit/b6d6ddfb23132f5c723e0ee376328bcb630b7573) | 2024-08-12 | Lu Baolu | remoteproc: Use iommu_paging_domain_alloc() |
| [52555b87](https://github.com/RVCK-Project/rvck/commit/52555b874168af87be5fc8fef7fa95513b2a7313) | 2024-08-12 | Lu Baolu | media: venus: firmware: Use iommu_paging_domain_alloc() |
| [821e852d](https://github.com/RVCK-Project/rvck/commit/821e852d9b1fe5cd842161cd4c32d3c49b10936f) | 2024-08-12 | Lu Baolu | media: nvidia: tegra: Use iommu_paging_domain_alloc() |
| [41623077](https://github.com/RVCK-Project/rvck/commit/41623077c7ed334bfd183592e3f921c67480b41a) | 2024-08-12 | Lu Baolu | gpu: host1x: Use iommu_paging_domain_alloc() |
| [9eddc688](https://github.com/RVCK-Project/rvck/commit/9eddc6881810bee38053e60b5f9c624a722c4ecd) | 2024-09-02 | Lu Baolu | drm/nouveau/tegra: Use iommu_paging_domain_alloc() |
| [98925e3b](https://github.com/RVCK-Project/rvck/commit/98925e3b1028ccddf4b9babf617ae3433d0e60e8) | 2024-06-10 | Lu Baolu | wifi: ath11k: Use iommu_paging_domain_alloc() |
| [1ecce27d](https://github.com/RVCK-Project/rvck/commit/1ecce27d24ed11509d0a4e9f5472a8dce9d106b4) | 2024-06-10 | Lu Baolu | wifi: ath10k: Use iommu_paging_domain_alloc() |
| [9fc450f4](https://github.com/RVCK-Project/rvck/commit/9fc450f457a17c593979a8c3e216d14915cca941) | 2024-06-10 | Lu Baolu | drm/msm: Use iommu_paging_domain_alloc() |
| [61d5a5e2](https://github.com/RVCK-Project/rvck/commit/61d5a5e2f86cd71a683b2458662f05f65c13afc7) | 2024-06-10 | Lu Baolu | vhost-vdpa: Use iommu_paging_domain_alloc() |
| [ec9c2d57](https://github.com/RVCK-Project/rvck/commit/ec9c2d57a2bb2a12bb8594c43f0b8f7f4c738106) | 2024-06-10 | Lu Baolu | vfio/type1: Use iommu_paging_domain_alloc() |
| [e640cce2](https://github.com/RVCK-Project/rvck/commit/e640cce2dbe168907b700902ad987c1c21368c9a) | 2024-06-10 | Lu Baolu | iommufd: Use iommu_paging_domain_alloc() |
| [1734a13c](https://github.com/RVCK-Project/rvck/commit/1734a13c19fcda02f03309d7543b3f1b77ec451f) | 2024-06-10 | Lu Baolu | iommu: Add iommu_paging_domain_alloc() interface |
| [4c792d9f](https://github.com/RVCK-Project/rvck/commit/4c792d9f29fc784980a326cac1dff54706cef50e) | 2025-02-28 | Robin Murphy | iommu: Get DT/ACPI parsing into the proper probe path |
| [b0d43520](https://github.com/RVCK-Project/rvck/commit/b0d43520b5ec064d19ae649092c73aeb93def82c) | 2025-02-28 | Robin Murphy | iommu: Keep dev-\>iommu state consistent |
| [e12824b2](https://github.com/RVCK-Project/rvck/commit/e12824b221881b07cf03febbec78f6430b2c90e3) | 2025-02-28 | Robin Murphy | iommu: Resolve ops in iommu_init_device() |
| [7e1886f8](https://github.com/RVCK-Project/rvck/commit/7e1886f87bb7dbc081fa365d0e75dd3bacfa4d1b) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [912e39b2](https://github.com/RVCK-Project/rvck/commit/912e39b28fc2a580709d316ff733e4d54f4409be) | 2025-02-27 | Robin Murphy | iommu: Unexport iommu_fwspec_free() |
| [e9e76d80](https://github.com/RVCK-Project/rvck/commit/e9e76d804e87a608e5a56abb40c345753d86ecbf) | 2024-07-02 | Robin Murphy | iommu: Remove iommu_fwspec ops |
| [ebf3e3cb](https://github.com/RVCK-Project/rvck/commit/ebf3e3cb03827460f4026389480cfd3183317e24) | 2024-07-02 | Robin Murphy | OF: Simplify of_iommu_configure() |
| [7bddb00b](https://github.com/RVCK-Project/rvck/commit/7bddb00bbdef7e40577fe9f4e12df4569ec6f7b3) | 2024-07-02 | Robin Murphy | ACPI: Retire acpi_iommu_fwspec_ops() |
| [8158deb5](https://github.com/RVCK-Project/rvck/commit/8158deb53c6bb23f7c27e7c493a48c820f42012e) | 2024-07-02 | Robin Murphy | iommu: Resolve fwspec ops automatically |
| [42e134ee](https://github.com/RVCK-Project/rvck/commit/42e134ee202aa593e2cb50d3fa13997d7e4adcc0) | 2023-12-07 | Jason Gunthorpe | acpi: Do not return struct iommu_ops from acpi_iommu_configure_id() |
| [4fb6943c](https://github.com/RVCK-Project/rvck/commit/4fb6943c39c9ea3742134af9eb6a1eeb723e6d90) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_priv_set() with a lockdep |
| [df254acf](https://github.com/RVCK-Project/rvck/commit/df254acf545e1798c18e2bad424e44215bae414a) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_get() with lockdep |
| [1c9f76f1](https://github.com/RVCK-Project/rvck/commit/1c9f76f1dfb6f5c121f3b54c0eef1e18e66723f0) | 2023-12-07 | Jason Gunthorpe | iommu/of: Use -ENODEV consistently in of_iommu_configure() |
| [bf993728](https://github.com/RVCK-Project/rvck/commit/bf993728b2e3f2030cfcafcc26e42cdb996f3201) | 2023-12-07 | Jason Gunthorpe | iommmu/of: Do not return struct iommu_ops from of_iommu_configure() |
| [c85b63e8](https://github.com/RVCK-Project/rvck/commit/c85b63e8ee76f24c9b3edc760048c7806d04bada) | 2023-12-07 | Jason Gunthorpe | iommu: Remove struct iommu_ops *iommu from arch_setup_dma_ops() |
| [8cd77873](https://github.com/RVCK-Project/rvck/commit/8cd778733f034692a0c7db9cbacf3f9ec57df057) | 2023-11-21 | Robin Murphy | iommu: Clean up open-coded ownership checks |
| [82dfbf43](https://github.com/RVCK-Project/rvck/commit/82dfbf436c1fe6c32377cb02552e61421deb2f79) | 2023-11-21 | Robin Murphy | iommu: Retire bus ops |
| [6c229f80](https://github.com/RVCK-Project/rvck/commit/6c229f80226700bdb61782fb647de0d46c90808d) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_domain_alloc() from bus ops |
| [ce769cbe](https://github.com/RVCK-Project/rvck/commit/ce769cbedcc3d602867b76b9b2e31f16b60671f4) | 2023-11-21 | Robin Murphy | iommu: Validate that devices match domains |
| [239c296d](https://github.com/RVCK-Project/rvck/commit/239c296d493cf81eb8b69a9d8f0cf7a7adb038ca) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_present() from bus ops |
| [055a12c8](https://github.com/RVCK-Project/rvck/commit/055a12c83e6441424aee112c3a5dcc1c39145b5f) | 2023-11-21 | Robin Murphy | iommu: Factor out some helpers |
| [6633255b](https://github.com/RVCK-Project/rvck/commit/6633255b67e895e83bcbbaf66cbf040111a13380) | 2024-03-13 | Xiao Wang | riscv: uaccess: Relax the threshold for fast path |
| [ce4628d8](https://github.com/RVCK-Project/rvck/commit/ce4628d87b6d561d58740575e2e9fb4cedb9d6a0) | 2024-03-13 | Xiao Wang | riscv: uaccess: Allow the last potential unrolled copy |
| [9b098ecf](https://github.com/RVCK-Project/rvck/commit/9b098ecf0693d1d2d5ae8a5679e8975bd3a938b7) | 2024-12-24 | Atish Patra | RISC-V: KVM: Add new exit statstics for redirected traps |
| [6990fab3](https://github.com/RVCK-Project/rvck/commit/6990fab3ddf65823e3d7fa823a3f93bf0894b52d) | 2024-12-24 | Atish Patra | RISC-V: KVM: Update firmware counters for various events |
| [f2cef74c](https://github.com/RVCK-Project/rvck/commit/f2cef74cdd85a4454294d25d59d05fa812fd15f4) | 2024-04-29 | Yu-Wei Hsu | RISC-V: KVM: Redirect AMO load/store access fault traps to guest |
| [5afcc7b4](https://github.com/RVCK-Project/rvck/commit/5afcc7b40b9001019b7cb191fed80a035f665487) | 2025-08-20 | XianLiang Huang | iommu/riscv: prevent NULL deref in iova_to_phys |
| [d410ec3c](https://github.com/RVCK-Project/rvck/commit/d410ec3cfe9e5f33e4ba248000c4e45410c8cf81) | 2025-01-03 | Xu Lu | iommu/riscv: Add shutdown function for iommu driver |
| [d5e646e8](https://github.com/RVCK-Project/rvck/commit/d5e646e8bc1d1b4cc127c1236d144ade66b3a64e) | 2025-01-03 | Xu Lu | iommu/riscv: Empty iommu queue before enabling it |
| [b7304a50](https://github.com/RVCK-Project/rvck/commit/b7304a504507edaba146bc2696e0530e02b0e1f9) | 2024-11-12 | Andrew Jones | iommu/riscv: Add support for platform msi |
| [572b69be](https://github.com/RVCK-Project/rvck/commit/572b69be717934597fb6744c8ea3f94168fa4874) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Paging domain support |
| [43c6f8a2](https://github.com/RVCK-Project/rvck/commit/43c6f8a2f57fd820529773c9e74288df91f23572) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Command and fault queue support |
| [a74001f2](https://github.com/RVCK-Project/rvck/commit/a74001f277d988c7748051f7b04e440b75c62c76) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Device directory management. |
| [570312b0](https://github.com/RVCK-Project/rvck/commit/570312b091f799cf82251f2b2b4bcf44990c5fb0) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Enable IOMMU registration and device probe. |
| [08703d48](https://github.com/RVCK-Project/rvck/commit/08703d4898f5f9de21344c764b52046fe386ab42) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU PCIe device driver |
| [e6713f5e](https://github.com/RVCK-Project/rvck/commit/e6713f5ee61f367414c44f8f6aa5d764a93e9cb7) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU platform device driver |
| [25a83261](https://github.com/RVCK-Project/rvck/commit/25a83261ce3cbe927c93a69b18676fb9a3723309) | 2024-10-15 | Tomasz Jeznach | dt-bindings: iommu: riscv: Add bindings for RISC-V IOMMU |
| [3850237e](https://github.com/RVCK-Project/rvck/commit/3850237e6e2b4e7ebe9af74b8e454c63e594d646) | 2024-04-13 | Pasha Tatashin | iommu/vt-d: add wrapper functions for page allocations |
| [8851b8e5](https://github.com/RVCK-Project/rvck/commit/8851b8e53e0f79164e75f04f0677a865da7fdbbe) | 2024-02-16 | Krzysztof Kozlowski | iommu: constify of_phandle_args in xlate |
| [b41ed47e](https://github.com/RVCK-Project/rvck/commit/b41ed47ef14546199dad50a49eacb3769a15218f) | 2023-11-22 | Matt Coster | sizes.h: Add entries between SZ_32G and SZ_64T |
| [f4dc9a34](https://github.com/RVCK-Project/rvck/commit/f4dc9a346623f3d3f77093cc5399259fb11b6dd3) | 2024-03-05 | Lu Baolu | iommu: Add static iommu_ops-\>release_domain |
| [c93c43f2](https://github.com/RVCK-Project/rvck/commit/c93c43f2c7c6a806dedcb9eda66668e9ec38c0f0) | 2023-09-27 | Jason Gunthorpe | iommufd: Convert to alloc_domain_paging() |
| [d75b2445](https://github.com/RVCK-Project/rvck/commit/d75b24453af0ba6f5c384ab43d2e70b68ee6fb88) | 2024-04-13 | Pasha Tatashin | iommu: Move IOMMU_DOMAIN_BLOCKED global statics to ops-\>blocked_domain |
| [7a85db36](https://github.com/RVCK-Project/rvck/commit/7a85db36f92c73cf260b8c09a40dcbbfaa47776d) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new userspace mappings with Svvpt... |
| [ebce2213](https://github.com/RVCK-Project/rvck/commit/ebce2213fc3d590e1380078b1b7b503ff64fe22b) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [dd27524a](https://github.com/RVCK-Project/rvck/commit/dd27524a3e17ceadaa77bf83d457ce7cdecb435c) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [d8fcff50](https://github.com/RVCK-Project/rvck/commit/d8fcff501ef20b666b2de70bc7644c38e1a6d285) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
| [7892664b](https://github.com/RVCK-Project/rvck/commit/7892664bdc99367bb760005599f5219363ddbbbd) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [51aa9151](https://github.com/RVCK-Project/rvck/commit/51aa915165f72e501cad2fe8372b9ebb06f1f88d) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [f9214d3a](https://github.com/RVCK-Project/rvck/commit/f9214d3aebb040ec0138f5479538cda9f85d032e) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [5d857154](https://github.com/RVCK-Project/rvck/commit/5d857154a52ed9a9d6be0417ad90003c47a2c0fb) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [f89046be](https://github.com/RVCK-Project/rvck/commit/f89046be1237037ff0430fc82cc0341174ba81fe) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [177bd9dc](https://github.com/RVCK-Project/rvck/commit/177bd9dc02b993946dec40bf7cd61f41c7e52612) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [e1375536](https://github.com/RVCK-Project/rvck/commit/e137553694e87d9eb0622ff11670cb03c4c0872c) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [a9f267c4](https://github.com/RVCK-Project/rvck/commit/a9f267c48d0bdfd5f758a5d4797f6e1c15a639bf) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [e5b8dbb2](https://github.com/RVCK-Project/rvck/commit/e5b8dbb263f9984938bd62f1615cdb3a51832ce3) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [2caeffd2](https://github.com/RVCK-Project/rvck/commit/2caeffd2c75f6254a4b49dd024ae9cd1370d684b) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [23a1d073](https://github.com/RVCK-Project/rvck/commit/23a1d073e51125bbb9754c5a2af49e13b50082c8) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [0368d999](https://github.com/RVCK-Project/rvck/commit/0368d999954772bfb4533dd80f05983e1c44395f) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [06a6339a](https://github.com/RVCK-Project/rvck/commit/06a6339a97a6b573c92ca14ee1e2d51d2e16a2e4) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [417b7330](https://github.com/RVCK-Project/rvck/commit/417b7330dab55ff380cd7fd2367972652274b024) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [19fe5fac](https://github.com/RVCK-Project/rvck/commit/19fe5fac2c6e37b779584944069256c450636c40) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [2e2c3740](https://github.com/RVCK-Project/rvck/commit/2e2c3740794ab4b712f6d306583e0cddf8c8c240) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [22dd91ee](https://github.com/RVCK-Project/rvck/commit/22dd91ee246e650c1ad37252ab0a88aca67b8bed) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [83bab694](https://github.com/RVCK-Project/rvck/commit/83bab694c614862e7cc4ffdeac5ecca5a412bb53) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [955fb7dd](https://github.com/RVCK-Project/rvck/commit/955fb7dd8a96952571c6f92d9c0fdfa94093d90b) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [2ccf5ec0](https://github.com/RVCK-Project/rvck/commit/2ccf5ec0eac9c7349afeaa5b34a354021aabee20) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [252db2aa](https://github.com/RVCK-Project/rvck/commit/252db2aa751f6b911001b7658faf455708aee876) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [98a2ceb5](https://github.com/RVCK-Project/rvck/commit/98a2ceb50256a61769976b8b7aa614d1d200725c) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [e0b14fda](https://github.com/RVCK-Project/rvck/commit/e0b14fda0924ad4f929fdc295f8d087c2fdd4be9) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [29b644b9](https://github.com/RVCK-Project/rvck/commit/29b644b948a1de77d9cc0faff712b07b0ff2097a) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [c27c38d1](https://github.com/RVCK-Project/rvck/commit/c27c38d1b5ec3fdeae342c87ae128ba58ddefbc1) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [a769deb2](https://github.com/RVCK-Project/rvck/commit/a769deb2026416d34f0f5124eaa2e1e322f578d4) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [90f02d59](https://github.com/RVCK-Project/rvck/commit/90f02d597f4cbb9ee89a358b8301a366d96538c8) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [4d263026](https://github.com/RVCK-Project/rvck/commit/4d2630269897086b24ed5ecb463972a4c1644ccf) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [585c9d7f](https://github.com/RVCK-Project/rvck/commit/585c9d7f5c8ae539ede88e98909e47032e5abb7c) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [bce2752f](https://github.com/RVCK-Project/rvck/commit/bce2752ff30bdfad8f4078a6d7e906b99f55023e) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [3a44069d](https://github.com/RVCK-Project/rvck/commit/3a44069d2fb5d5a2dce9229e8173f6045297f20f) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [07458a01](https://github.com/RVCK-Project/rvck/commit/07458a011ebd2798234907cf5337b01bc5609c49) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [01db57cc](https://github.com/RVCK-Project/rvck/commit/01db57ccc8bbc036337ca270594f1f4664bf04c6) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [c35cb8cc](https://github.com/RVCK-Project/rvck/commit/c35cb8cc573f69fe7984e81f5daab590d16eb79c) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [e586c5f2](https://github.com/RVCK-Project/rvck/commit/e586c5f22d1d679cb15c801426d4892a754f5d79) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [a431a6b4](https://github.com/RVCK-Project/rvck/commit/a431a6b4668c3ea8fa44b5e922385727720ad5c4) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [4e937278](https://github.com/RVCK-Project/rvck/commit/4e9372781c4daebe192f25e0b34f20338e46add8) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [c82c16ac](https://github.com/RVCK-Project/rvck/commit/c82c16ac571fe4541816ed46ae337ee3db80a7fb) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [762b07c0](https://github.com/RVCK-Project/rvck/commit/762b07c0d27ea3f82e3dd567e7f605f95a230078) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [51c199bf](https://github.com/RVCK-Project/rvck/commit/51c199bf1962dc2030c211557a1eb6bcf49830c9) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [fdc35f10](https://github.com/RVCK-Project/rvck/commit/fdc35f10d22b1f046eab6fcf4271a0984f6b8602) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [1ae9fc6e](https://github.com/RVCK-Project/rvck/commit/1ae9fc6e402f584b3fff1b9e9dca04eacc045be5) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [dfefc9bf](https://github.com/RVCK-Project/rvck/commit/dfefc9bf2ef20bec81e8fce5756c58dcf4ba6d7b) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [831869fa](https://github.com/RVCK-Project/rvck/commit/831869fafb31ab566f3a12792882829c13dba9fe) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [575bc6c9](https://github.com/RVCK-Project/rvck/commit/575bc6c9c916e0d47ea1e6a5a7a40cd23d1fc6d8) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [3ff7a2ac](https://github.com/RVCK-Project/rvck/commit/3ff7a2ac294ae1b83cf29e412d2d52ad3f251ad2) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [f645182b](https://github.com/RVCK-Project/rvck/commit/f645182b943f9d353a83523114b5e2aae3da5e6a) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [897097be](https://github.com/RVCK-Project/rvck/commit/897097bec4b6a6385ecbc4b49e44780e7b65e472) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [3d718bdb](https://github.com/RVCK-Project/rvck/commit/3d718bdb4024af61dd8f5ddea9c61d5343744334) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [d07f4e19](https://github.com/RVCK-Project/rvck/commit/d07f4e197345f5eb922f5471ba93cf694a2531be) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [dcc3ee1a](https://github.com/RVCK-Project/rvck/commit/dcc3ee1a7e59887d384d6ed1b4b1bd51a016b877) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
| [c70a028d](https://github.com/RVCK-Project/rvck/commit/c70a028d4ac0a2d6e6e0a41a3921a96539e148df) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [b892f934](https://github.com/RVCK-Project/rvck/commit/b892f934a06f00b52bc1440682ee20723a503692) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [892b1b65](https://github.com/RVCK-Project/rvck/commit/892b1b65c2852b3b3757357ab591ea02d24d2eab) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [7b0f613a](https://github.com/RVCK-Project/rvck/commit/7b0f613aebff133e91e984742a41152260b6be48) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [80d97b2a](https://github.com/RVCK-Project/rvck/commit/80d97b2ab2f858d2f9f5d0a0892abfae54cb5716) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [9627dca2](https://github.com/RVCK-Project/rvck/commit/9627dca23f76ed816039f889a711df18a4bc3146) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [9cf52371](https://github.com/RVCK-Project/rvck/commit/9cf5237164f07b2742d51e4082962975b5ee9908) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [10c03afc](https://github.com/RVCK-Project/rvck/commit/10c03afcd23b55aa586a03fa3301fe2ff347f08a) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [ab401c1e](https://github.com/RVCK-Project/rvck/commit/ab401c1e13ed5cec207ff75b1fe38aafd3db386e) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [13479b1b](https://github.com/RVCK-Project/rvck/commit/13479b1bab4ba2865690fcc61a1fae819d955915) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [f38805bb](https://github.com/RVCK-Project/rvck/commit/f38805bb80d7c5454ac0db45868f8eaa830b8642) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [8e6e8376](https://github.com/RVCK-Project/rvck/commit/8e6e8376967f673c011d0cbca7111052296b79c7) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [d7430770](https://github.com/RVCK-Project/rvck/commit/d743077097fc8781c5bbaf4653a2196f6d7c9721) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [3025312e](https://github.com/RVCK-Project/rvck/commit/3025312e7acaecab8c3a988871fcace1e15d5468) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [15d70b2d](https://github.com/RVCK-Project/rvck/commit/15d70b2d930c2fb5f1f492ecc21df5c5ade0473d) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [c4f0d586](https://github.com/RVCK-Project/rvck/commit/c4f0d586277cca4d1fbaa72c2023c6e2f116d0e5) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [2c791d2c](https://github.com/RVCK-Project/rvck/commit/2c791d2c42dbcc0f690a869697c8741c8079acca) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [1fe5c4dd](https://github.com/RVCK-Project/rvck/commit/1fe5c4dd3f9c8bc6b5d7ac3c0513848870485a11) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [8607ba7e](https://github.com/RVCK-Project/rvck/commit/8607ba7e4a3141270b6e588db20e330f95031843) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [f3290c9c](https://github.com/RVCK-Project/rvck/commit/f3290c9c1380dbad46020b54a10c49b75b2736dc) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
| [3ab7933a](https://github.com/RVCK-Project/rvck/commit/3ab7933a6d71b786b9db79ff044a6b9c1e7701df) | 2024-04-09 | Baoquan He | kexec: fix the unexpected kexec_dprintk() macro |
| [a240221c](https://github.com/RVCK-Project/rvck/commit/a240221c73eb1e7701b1558a9ee73a5b2446ec80) | 2024-07-30 | Sunil V L | kexec_file, parisc: print out debugging message if required |
| [8b7b9fc4](https://github.com/RVCK-Project/rvck/commit/8b7b9fc4341300ebb24adb743d21d7dab28e642f) | 2023-12-13 | Baoquan He | kexec_file, power: print out debugging message if required |
| [a86a5e85](https://github.com/RVCK-Project/rvck/commit/a86a5e85d426109f67798dc4817cea01ece77bb5) | 2023-12-13 | Baoquan He | kexec_file, riscv: print out debugging message if required |
| [5f97d158](https://github.com/RVCK-Project/rvck/commit/5f97d158074d15fef53b4f979210e3c91b860215) | 2023-12-13 | Baoquan He | kexec_file, arm64: print out debugging message if required |
| [cf143fa2](https://github.com/RVCK-Project/rvck/commit/cf143fa28f3f22ba13755f094b8ebb593d78cd93) | 2023-12-13 | Baoquan He | kexec_file, x86: print out debugging message if required |
| [dda08671](https://github.com/RVCK-Project/rvck/commit/dda086715381f053fd02758fcfd1a3c5e7398033) | 2023-12-13 | Baoquan He | kexec_file: print out debugging message if required |
| [d1cbf776](https://github.com/RVCK-Project/rvck/commit/d1cbf77641086c36cc28321073c77b55ed92dc19) | 2023-12-13 | Baoquan He | kexec_file: add kexec_file flag to control debug printing |
| [ab1b780b](https://github.com/RVCK-Project/rvck/commit/ab1b780b5ea2a7105834ffc5bc184c02293d37d9) | 2025-04-03 | Radim Krčmář | KVM: RISC-V: reset smstateen CSRs |
| [5672cf32](https://github.com/RVCK-Project/rvck/commit/5672cf3224252a07b0b92bf5af83c0887c6da87a) | 2023-12-24 | Anup Patel | RISC-V: KVM: Fix indentation in kvm_riscv_vcpu_set_reg_csr() |
| [c3c4b211](https://github.com/RVCK-Project/rvck/commit/c3c4b211c1934b5c9c54b2ab7b09cb277d61135d) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 to ONE_REG |
| [ca66078f](https://github.com/RVCK-Project/rvck/commit/ca66078f7add83337ef6cd5e0b35f32dbb3df73b) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 context save/restore |
| [bf4b0243](https://github.com/RVCK-Project/rvck/commit/bf4b0243bed9e97a068d81480b115b9d5e39aeba) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add senvcfg context save/restore |
| [f863e063](https://github.com/RVCK-Project/rvck/commit/f863e063fecef00b13836e71c4e8b6ac0ba3d256) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Enable Smstateen accesses |
| [bc47f401](https://github.com/RVCK-Project/rvck/commit/bc47f4013827f266d87c445a8229e7964e454c59) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Add kvm_vcpu_config |
| [27868302](https://github.com/RVCK-Project/rvck/commit/2786830212d810e873d6dac72a2a2f577f3bf419) | 2024-07-30 | Sunil V L | serial: 8250_platform: Enable generic 16550A platform devices |
| [c37a72d0](https://github.com/RVCK-Project/rvck/commit/c37a72d0aa5330d3409f1da9cd514c79d5d06544) | 2025-04-09 | Song Shuai | riscv: kexec_file: Support loading Image binary file |
| [77144e6b](https://github.com/RVCK-Project/rvck/commit/77144e6b7ada711efe6704507a13cc45c44b9462) | 2025-07-25 | gaorui | riscv: kexec_file: Split the loading of kernel and others |
| [1273df02](https://github.com/RVCK-Project/rvck/commit/1273df028cee32cde9ff53261c77cbcb482e2ca3) | 2025-07-25 | gaorui | Revert "riscv: kexec: Add image loader for kexec file" |
| [5b05066d](https://github.com/RVCK-Project/rvck/commit/5b05066d1761e81937ebc54305a35ace6f4d3fa1) | 2023-11-30 | Samuel Ortiz | RISC-V: Implement archrandom when Zkr is available |
| [11587c4d](https://github.com/RVCK-Project/rvck/commit/11587c4d5d6f36f515f109b49500b51e1f2b675c) | 2024-02-08 | Sunil V L | cpufreq: Move CPPC configs to common Kconfig and add RISC-V |
| [361da4ca](https://github.com/RVCK-Project/rvck/commit/361da4ca7fd181ab00199dc44d387d3038be200c) | 2024-02-08 | Sunil V L | ACPI: RISC-V: Add CPPC driver |
| [94b1d85f](https://github.com/RVCK-Project/rvck/commit/94b1d85f4d6ba66c617a4d36d68eb98e91452e9e) | 2024-06-17 | Yunhui Cui | RISC-V: Select ACPI PPTT drivers |
| [d8ce6f65](https://github.com/RVCK-Project/rvck/commit/d8ce6f65dc3c47524d3f0962bdefe25a595554a5) | 2024-05-02 | Sia Jee Heng | RISC-V: ACPI: Enable SPCR table for console output on RISC-V |
| [66d29302](https://github.com/RVCK-Project/rvck/commit/66d2930250dd72fcd83f42168aa931b5dd29c7df) | 2024-07-18 | Ryo Takakura | RISC-V: Enable IPI CPU Backtrace |
| [643c7d78](https://github.com/RVCK-Project/rvck/commit/643c7d783657d53e3d00306331f5dc211e87fbb4) | 2024-06-13 | Haibo Xu | riscv: dmi: Add SMBIOS/DMI support |
| [fd53f4c0](https://github.com/RVCK-Project/rvck/commit/fd53f4c00f6eec667d454d2296e8accc121aef78) | 2024-06-13 | Haibo Xu | ACPI: NUMA: replace pr_info with pr_debug in arch_acpi_numa_init |
| [4a28a0c5](https://github.com/RVCK-Project/rvck/commit/4a28a0c568b1efd7d7252df1094d8d8708315d59) | 2025-04-25 | gaorui | ACPI: NUMA: change the ACPI_NUMA to a hidden option |
| [33e8aca8](https://github.com/RVCK-Project/rvck/commit/33e8aca8225802de447abd2de0b3f2a5751aa261) | 2025-04-25 | gaorui | ACPI: NUMA: Make some NUMA-related functions available for RISC-V |
| [f8cf83d7](https://github.com/RVCK-Project/rvck/commit/f8cf83d767bb1bc8646f0c71536eb4a1da700b23) | 2024-06-13 | Haibo Xu | ACPI: NUMA: Add handler for SRAT RINTC affinity structure |
| [31648e7f](https://github.com/RVCK-Project/rvck/commit/31648e7f81cd3bfd6eee9fdab1db6c5b4224b08b) | 2024-06-13 | Haibo Xu | ACPI: RISCV: Add NUMA support based on SRAT and SLIT |
| [09153d5d](https://github.com/RVCK-Project/rvck/commit/09153d5d8415f7ed9b908c48e1417239d7b39af8) | 2024-01-17 | Haibo Xu | ACPICA: SRAT: Add RISC-V RINTC affinity structure |
---

**共 354 条提交（显示全部）**

[分页显示](中兴通讯.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-05-19 22:52:14*
*数据来源: 主分支 rvck-6.6@998334e6*
