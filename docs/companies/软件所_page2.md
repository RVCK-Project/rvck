# 软件所 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 1127</li>
<li><strong>统计时间</strong>: 2026-09-04 11:16:31</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.148</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @iscas.ac.cn, @isrc.iscas.ac.cn
- **特定签名**: Weihao Li <ieiao@outlook.com>

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [375e687f](https://github.com/RVCK-Project/rvck/commit/375e687f1e2d9ca1910cd14953eb923ab1990ae5) | 2025-02-17 | Andrew Jones | riscv: KVM: Fix hart suspend_type use |
| [f0581dbf](https://github.com/RVCK-Project/rvck/commit/f0581dbf5aafa1f3aa7572c8999a16ac6cacfeae) | 2024-11-27 | Michael Neuling | RISC-V: KVM: Fix csr_write -\> csr_set for HVIEN PMU overflow bit |
| [ab197f8e](https://github.com/RVCK-Project/rvck/commit/ab197f8e1112a02a61f2318394997bc8cdd6f81c) | 2024-08-15 | Anup Patel | RISC-V: KVM: Don't zero-out PMU snapshot area before freeing data |
| [8eadf7e3](https://github.com/RVCK-Project/rvck/commit/8eadf7e3e0980f9691f5b16df46da88920a0eaa4) | 2026-02-02 | Jiakai Xu | RISC-V: KVM: Fix use-after-free in kvm_riscv_gstage_get_leaf() |
| [c83fc6d1](https://github.com/RVCK-Project/rvck/commit/c83fc6d15e259e2e6c223016ff6854f2c7398626) | 2025-01-08 | Andrew Bresticker | riscv: Support huge pfnmaps |
| [b26d8dc2](https://github.com/RVCK-Project/rvck/commit/b26d8dc26b142d3bcb69501079053c13c1bf1ea8) | 2025-06-18 | Anup Patel | RISC-V: KVM: Pass VMID as parameter to kvm_riscv_hfence_xyz() APIs |
| [38fafb3c](https://github.com/RVCK-Project/rvck/commit/38fafb3c82c79902b10a6576ff0b5ddfce5b439d) | 2025-06-18 | Anup Patel | RISC-V: KVM: Factor-out g-stage page table management |
| [d7c3d16c](https://github.com/RVCK-Project/rvck/commit/d7c3d16c7fdd2532444cfe1ba86a82f2984a6233) | 2025-06-18 | Anup Patel | RISC-V: KVM: Add vmid field to struct kvm_riscv_hfence |
| [404a0f23](https://github.com/RVCK-Project/rvck/commit/404a0f23f8a7a490410efddc9772fe8332e69249) | 2025-06-18 | Anup Patel | RISC-V: KVM: Introduce struct kvm_gstage_mapping |
| [57f6f34d](https://github.com/RVCK-Project/rvck/commit/57f6f34d5df73bf71e47b8106854367e4783515f) | 2025-06-18 | Anup Patel | RISC-V: KVM: Factor-out MMU related declarations into separate headers |
| [4d4794f7](https://github.com/RVCK-Project/rvck/commit/4d4794f7053b8dbda68a28872b76607594b2b27b) | 2025-06-18 | Anup Patel | RISC-V: KVM: Use ncsr_xyz() in kvm_riscv_vcpu_trap_redirect() |
| [ce49c6ac](https://github.com/RVCK-Project/rvck/commit/ce49c6ac2a85ddaa97ab5bd445afe259b99961e7) | 2025-06-18 | Anup Patel | RISC-V: KVM: Implement kvm_arch_flush_remote_tlbs_range() |
| [63ecb8c7](https://github.com/RVCK-Project/rvck/commit/63ecb8c750f63b80156634df8e14d95b4c9f3e5f) | 2025-06-18 | Anup Patel | RISC-V: KVM: Don't flush TLB when PTE is unchanged |
| [c6db40c5](https://github.com/RVCK-Project/rvck/commit/c6db40c54ac8cb295f5b71837489e2ca80954244) | 2025-06-18 | Anup Patel | RISC-V: KVM: Replace KVM_REQ_HFENCE_GVMA_VMID_ALL with KVM_REQ_TLB_FLUSH |
| [752560bb](https://github.com/RVCK-Project/rvck/commit/752560bb05b6e7d44999cc495d659f3ebcc9686c) | 2025-06-18 | Anup Patel | RISC-V: KVM: Rename and move kvm_riscv_local_tlb_sanitize() |
| [84e3ddb2](https://github.com/RVCK-Project/rvck/commit/84e3ddb295c3b10846f28d394f58d5e2f4b2f4f8) | 2025-06-18 | Anup Patel | RISC-V: KVM: Drop the return value of kvm_riscv_vcpu_aia_init() |
| [bf740451](https://github.com/RVCK-Project/rvck/commit/bf74045134118d34da6f2bc8446bc6db28365964) | 2025-06-18 | Anup Patel | RISC-V: KVM: Check kvm_riscv_vcpu_alloc_vector_context() return value |
| [50012ed5](https://github.com/RVCK-Project/rvck/commit/50012ed5c5ca5f44e49c2c7bc02a986bf1375b06) | 2025-06-02 | Cyril Bur | riscv: uaccess: Only restore the CSR_STATUS SUM bit |
| [96717947](https://github.com/RVCK-Project/rvck/commit/96717947822b702fb14f035401dc4d476842718b) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning in __get_user_error() |
| [4effa8f5](https://github.com/RVCK-Project/rvck/commit/4effa8f5a95ad7aecb8d0420a3a6340d964bbbf7) | 2025-06-10 | Palmer Dabbelt | RISC-V: uaccess: Wrap the get_user_8 uaccess macro |
| [59c10166](https://github.com/RVCK-Project/rvck/commit/59c10166b826285b60e09762ff547f9d5894e357) | 2025-07-25 | Aurelien Jarno | riscv: uaccess: fix __put_user_nocheck for unaligned accesses |
| [73e79733](https://github.com/RVCK-Project/rvck/commit/73e7973389446dad321844fbe8c7caafdd21d0cc) | 2025-07-15 | Nathan Chancellor | riscv: uaccess: Fix -Wuninitialized and -Wshadow in __put_user_nocheck |
| [0fde35ec](https://github.com/RVCK-Project/rvck/commit/0fde35ecf664642532d35508a7ae86046f320a83) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning about different address spaces |
| [12deaa1a](https://github.com/RVCK-Project/rvck/commit/12deaa1a3e31d9b348ca8368b6481bbcc8cfedc5) | 2025-08-05 | Radim Krčmář | RISC-V: KVM: fix stack overrun when loading vlenb |
| [b42bde75](https://github.com/RVCK-Project/rvck/commit/b42bde75ed915b19524ff47ae15d25014ce26f9e) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: add 'vlenb' Vector CSR |
| [cf8fe298](https://github.com/RVCK-Project/rvck/commit/cf8fe29874963bae8880af4d2e27b088648065da) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: set 'vlenb' in kvm_riscv_vcpu_alloc_vector_context() |
| [08f38b52](https://github.com/RVCK-Project/rvck/commit/08f38b522e4fc3ed807be69dfc4da0e2915e5280) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use NACL HFENCEs for KVM request based HFENCEs |
| [2ccc4c88](https://github.com/RVCK-Project/rvck/commit/2ccc4c88257f3896bab6e4e6a1d6c84769dc895d) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save trap CSRs in kvm_riscv_vcpu_enter_exit() |
| [1697c923](https://github.com/RVCK-Project/rvck/commit/1697c9239e7f51e018dcaa5c4b3c2f0418d21c2e) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use SBI sync SRET call when available |
| [a7737658](https://github.com/RVCK-Project/rvck/commit/a7737658fd5c798e2dc6785a094446b27b8f8064) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing AIA CSRs |
| [f3785ebe](https://github.com/RVCK-Project/rvck/commit/f3785ebedeb1bd065e7526bca75838622a7c5c45) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing H-extension CSRs |
| [05a37c8e](https://github.com/RVCK-Project/rvck/commit/05a37c8eee8651b537a9b6c5798e2360cd420506) | 2024-10-21 | Anup Patel | RISC-V: KVM: Add common nested acceleration support |
| [a180f1f5](https://github.com/RVCK-Project/rvck/commit/a180f1f5688aa29a0c65154759dd51ac327e9099) | 2024-10-21 | Anup Patel | RISC-V: Add defines for the SBI nested acceleration extension |
| [a167a8b9](https://github.com/RVCK-Project/rvck/commit/a167a8b98df8d5d300288d6b830296f49070603c) | 2024-10-21 | Anup Patel | RISC-V: KVM: Don't setup SGEI for zero guest external interrupts |
| [71256434](https://github.com/RVCK-Project/rvck/commit/71256434b266b128bf155aab55882d92372839f7) | 2024-10-21 | Anup Patel | RISC-V: KVM: Replace aia_set_hvictl() with aia_hvictl_value() |
| [f96ce9fd](https://github.com/RVCK-Project/rvck/commit/f96ce9fd22b83486f8f08596ab2267d7ae555ae2) | 2024-10-21 | Anup Patel | RISC-V: KVM: Break down the __kvm_riscv_switch_to() into macros |
| [9e9d0138](https://github.com/RVCK-Project/rvck/commit/9e9d0138d97a49fbade53aeecae2709418bb3f6b) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore SCOUNTEREN in C source |
| [805c3956](https://github.com/RVCK-Project/rvck/commit/805c3956ae12957b3c46b7e634197cce29116da3) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore HSTATUS in C source |
| [9d4fa73d](https://github.com/RVCK-Project/rvck/commit/9d4fa73d5c8862076fc99b71285ac5c8ae5b2f51) | 2024-10-21 | Anup Patel | RISC-V: KVM: Order the object files alphabetically |
| [4fbf34bd](https://github.com/RVCK-Project/rvck/commit/4fbf34bd780bbe58c5af6796f18c9efa73bdd134) | 2024-10-15 | Quan Zhou | riscv: KVM: add basic support for host vs guest profiling |
| [b3447555](https://github.com/RVCK-Project/rvck/commit/b34475553f4baab043a49acb3e047cdc9968cb15) | 2024-10-15 | Quan Zhou | riscv: perf: add guest vs host distinction |
| [d4b887c0](https://github.com/RVCK-Project/rvck/commit/d4b887c0bfa6b267b86edd5e05098dd592b810f1) | 2024-07-08 | Jinjie Ruan | riscv: stacktrace: Add USER_STACKTRACE support |
| [c9864cbf](https://github.com/RVCK-Project/rvck/commit/c9864cbfe2a9057dbafe1ee1bef3c68af3ac1fda) | 2023-10-24 | Clément Léger | riscv: kvm: use ".L" local labels in assembly when applicable |
| [770698bf](https://github.com/RVCK-Project/rvck/commit/770698bf331afc3ad9e1a9b4abd75b0a8343ed2a) | 2023-10-24 | Clément Léger | riscv: kvm: Use SYM_*() assembly macros instead of deprecated ones |
| [916f7904](https://github.com/RVCK-Project/rvck/commit/916f790469f7322172b67044cd715d69320ee882) | 2025-06-26 | Michal Wilczynski | riscv: dts: thead: th1520: Add GPU clkgen reset to AON node |
| [c6a67673](https://github.com/RVCK-Project/rvck/commit/c6a67673b72eccb14d0ea854ffc83ff992c31b25) | 2025-03-03 | Michal Wilczynski | reset: thead: Add TH1520 reset controller driver |
| [c3eb79a7](https://github.com/RVCK-Project/rvck/commit/c3eb79a718d417b97715ddabcb9ec01271a8df39) | 2025-03-03 | Michal Wilczynski | dt-bindings: reset: Add T-HEAD TH1520 SoC Reset Controller |
| [e2448004](https://github.com/RVCK-Project/rvck/commit/e244800422dded8a9e04d43e856373a1f868f6da) | 2025-08-18 | Anup Patel | MAINTAINERS: Add entry for RISC-V RPMI and MPXY drivers |
| [00386e9c](https://github.com/RVCK-Project/rvck/commit/00386e9cd590dd5c37748d3a526f5d9b22052179) | 2025-08-18 | Anup Patel | RISC-V: Enable GPIO keyboard and event device in RV64 defconfig |
| [a6b6cfba](https://github.com/RVCK-Project/rvck/commit/a6b6cfbaaaa19109295102ab360be03264678544) | 2025-08-18 | Sunil V L | irqchip/riscv-rpmi-sysmsi: Add ACPI support |
| [22ee536f](https://github.com/RVCK-Project/rvck/commit/22ee536f6e1b9756ea50da26e4631eeb34ccd6e7) | 2025-08-18 | Sunil V L | mailbox/riscv-sbi-mpxy: Add ACPI support |
| [fab5569c](https://github.com/RVCK-Project/rvck/commit/fab5569cc0550af801676f0c3b9eaeb64f080da1) | 2025-08-18 | Sunil V L | irqchip/irq-riscv-imsic-early: Export imsic_acpi_get_fwnode() |
| [5454b09b](https://github.com/RVCK-Project/rvck/commit/5454b09b8c0fa4fabdb729d1af55e5c9583fa178) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add RPMI System MSI to GSI mapping |
| [1251f083](https://github.com/RVCK-Project/rvck/commit/1251f083b3fcfe692017b0ca4be10320a38c1e1a) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add support to update gsi range |
| [791dc981](https://github.com/RVCK-Project/rvck/commit/791dc9819b87295665deb3eb37426d9512520ef9) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Create interrupt controller list in sorted order |
| [eee5e4bc](https://github.com/RVCK-Project/rvck/commit/eee5e4bc91cce7930273c4a5a011681b071478e3) | 2025-08-18 | Sunil V L | ACPI: scan: Update honor list for RPMI System MSI |
| [8d51c7f0](https://github.com/RVCK-Project/rvck/commit/8d51c7f04ff88e99bb992de3afe6feecda43b3f2) | 2025-08-18 | Sunil V L | ACPI: Add support for nargs_prop in acpi_fwnode_get_reference_args() |
| [88072fca](https://github.com/RVCK-Project/rvck/commit/88072fca21d1f6e6f89713704178113a3f08d629) | 2025-08-18 | Sunil V L | ACPI: property: Refactor acpi_fwnode_get_reference_args() to support nargs_prop |
| [cdbdb33f](https://github.com/RVCK-Project/rvck/commit/cdbdb33f42ec0c2458cde4d4ec592aa90325f436) | 2025-08-18 | Anup Patel | irqchip: Add driver for the RPMI system MSI service group |
| [eb9447a6](https://github.com/RVCK-Project/rvck/commit/eb9447a6a3f6f422ce68e5a5f373d5916d1163c3) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI interrupt controller bindings |
| [70fee9ad](https://github.com/RVCK-Project/rvck/commit/70fee9ad3f7248d15b81eb0422d7fde8dd2864c2) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI message proxy bindings |
| [d9e03267](https://github.com/RVCK-Project/rvck/commit/d9e03267581ab6ccf6e591ddf912f180e25c8934) | 2025-08-18 | Rahul Pathak | clk: Add clock driver for the RISC-V RPMI clock service group |
| [01f8c0a6](https://github.com/RVCK-Project/rvck/commit/01f8c0a68cbc89a68346997fdcb4859b9735ea74) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service controller bindings |
| [6ed60836](https://github.com/RVCK-Project/rvck/commit/6ed608364e14e0a85c359bd96d4316d9bc8d1633) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service message proxy bindings |
| [e48a01cd](https://github.com/RVCK-Project/rvck/commit/e48a01cda894f6ee692be9da2a5218a64308ca41) | 2025-08-18 | Anup Patel | mailbox: Add RISC-V SBI message proxy (MPXY) based mailbox driver |
| [2a69b173](https://github.com/RVCK-Project/rvck/commit/2a69b173ba1cdbd2aa556a6d6eb01b2689155123) | 2025-08-18 | Anup Patel | byteorder: Add memcpy_to_le32() and memcpy_from_le32() |
| [9dea8d02](https://github.com/RVCK-Project/rvck/commit/9dea8d02f8a35a75d54d3d986288f5e925287040) | 2025-08-18 | Anup Patel | mailbox: Add common header for RPMI messages sent via mailbox |
| [655b0809](https://github.com/RVCK-Project/rvck/commit/655b0809ab2a55f8d1ed833361d91d80213c0238) | 2025-08-18 | Anup Patel | RISC-V: Add defines for the SBI message proxy extension |
| [e04c6023](https://github.com/RVCK-Project/rvck/commit/e04c60234954a85543407f8db6681393614c5785) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RISC-V SBI MPXY extension |
| [874a07e8](https://github.com/RVCK-Project/rvck/commit/874a07e81d6d877955fe1508d0b4e3b1817a4fff) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RPMI shared memory transport |
| [317b717c](https://github.com/RVCK-Project/rvck/commit/317b717c1c977a2452ce77ec10cefdddb880624c) | 2025-02-12 | Andy Shevchenko | driver core: Split devres APIs to device/devres.h |
| [76a1ce44](https://github.com/RVCK-Project/rvck/commit/76a1ce44b9c7022f44f33bcf29bb0c27c8aad8f2) | 2023-12-26 | Tanzir Hasan | lib/string: shrink lib/string.i via IWYU |
| [c905b309](https://github.com/RVCK-Project/rvck/commit/c905b30956140ca6b8848c5a99ea8259a5c630ac) | 2023-12-26 | Tanzir Hasan | kernel.h: removed REPEAT_BYTE from kernel.h |
| [707d632c](https://github.com/RVCK-Project/rvck/commit/707d632c5fe61061792574038eadee786b2804b4) | 2025-04-09 | Sakari Ailus | Documentation: ACPI: Use all-string data node references |
| [d88a3c02](https://github.com/RVCK-Project/rvck/commit/d88a3c02d4fd8dca7ffabd1adf5328bbc195730d) | 2023-11-06 | Rafael J. Wysocki | ACPI: property: Support using strings in reference properties |
| [e47a77eb](https://github.com/RVCK-Project/rvck/commit/e47a77eb81359fed3e841bc7c21382843e90cfd3) | 2026-03-30 | jichuan Feng | riscv: k1: add OrangePi RV2 device tree |
| [85ba41f8](https://github.com/RVCK-Project/rvck/commit/85ba41f819dbc750d917112964cb125bd3238d80) | 2026-03-30 | jichuan Feng | riscv: k1: add Milk-V Jupiter device tree |
| [f86f1fa0](https://github.com/RVCK-Project/rvck/commit/f86f1fa0c002cded7bd76c331ac23f0023609fdf) | 2026-03-30 | jichuan Feng | riscv: k1: add MUSE Pi device tree |
| [5495f095](https://github.com/RVCK-Project/rvck/commit/5495f0959dd8835c00d58ee6e4e87fd30da4e8f6) | 2026-03-24 | jichuan Feng | riscv: k1: add MUSE Pi Pro device tree |
| [2fb3ac9c](https://github.com/RVCK-Project/rvck/commit/2fb3ac9c25df14f7f1bf315ab15a5b2d99dd54c5) | 2025-06-20 | Nam Cao | Revert "riscv: misaligned: fix sleeping function called during misaligned access... |
| [01ba7979](https://github.com/RVCK-Project/rvck/commit/01ba797999c63577f24630c99db4986a63013e88) | 2025-06-20 | Alexandre Ghiti | riscv: Move all duplicate insn parsing macros into asm/insn.h |
| [cb52fc20](https://github.com/RVCK-Project/rvck/commit/cb52fc20917243581c4b925960e14d6024b8fb0f) | 2025-06-20 | Alexandre Ghiti | riscv: Strengthen duplicate and inconsistent definition of RV_X() |
| [11c85ba1](https://github.com/RVCK-Project/rvck/commit/11c85ba14c86d0d9efb2f6c47d4362bc6d14d68d) | 2025-06-20 | Alexandre Ghiti | riscv: Fix typo EXRACT -\> EXTRACT |
| [52a161ba](https://github.com/RVCK-Project/rvck/commit/52a161ba062cf2f2d8d12b767ae2f33afc2ebb61) | 2025-07-10 | Andreas Schwab | riscv: traps_misaligned: properly sign extend value in misaligned load handler |
| [a221db21](https://github.com/RVCK-Project/rvck/commit/a221db2198ef5936db4a575f6d13abd7846fab98) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for SBI_FWFT_MISALIGNED_DELEG |
| [8749e0db](https://github.com/RVCK-Project/rvck/commit/8749e0db27a66b814281ecf529a1a6e0f050dd0d) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for FWFT SBI extension |
| [c3b9c1c9](https://github.com/RVCK-Project/rvck/commit/c3b9c1c97919510bb106915544a8cae4fb7b8790) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension reset callback |
| [d2df8f5a](https://github.com/RVCK-Project/rvck/commit/d2df8f5a3f7aa83859a78b718e04f25e7c1b10a7) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension init()/deinit() functions |
| [625ee647](https://github.com/RVCK-Project/rvck/commit/625ee64755cc3bc307c02b764a59b5b10297144c) | 2025-05-23 | Clément Léger | riscv: misaligned: add a function to check misalign trap delegability |
| [b8f9c3be](https://github.com/RVCK-Project/rvck/commit/b8f9c3bee9baa58e963a5d53d696bc08df0d1220) | 2025-05-23 | Clément Léger | riscv: misaligned: move emulated access uniformity check in a function |
| [338a929d](https://github.com/RVCK-Project/rvck/commit/338a929d210fab7d6b01b78fb0dab6f699436f2d) | 2025-05-23 | Clément Léger | riscv: misaligned: declare misaligned_access_speed under CONFIG_RISCV_MISALIGNED |
| [610dd008](https://github.com/RVCK-Project/rvck/commit/610dd00837b391aaeb50deb00936750ccc99494c) | 2025-05-23 | Clément Léger | riscv: misaligned: use on_each_cpu() for scalar misaligned access probing |
| [8a90b988](https://github.com/RVCK-Project/rvck/commit/8a90b988810c59ef781a3a88411cc80108f0a249) | 2025-05-23 | Clément Léger | riscv: misaligned: request misaligned exception from SBI |
| [1f78aab1](https://github.com/RVCK-Project/rvck/commit/1f78aab18123e62680bb6654a60eb55506437b07) | 2025-05-23 | Clément Léger | riscv: sbi: add SBI FWFT extension calls |
| [766e0232](https://github.com/RVCK-Project/rvck/commit/766e02323ae13d6a1c0d782b2c61b9e0fbfbe0f6) | 2025-05-23 | Clément Léger | riscv: sbi: add FWFT extension interface |
| [6c3d6bbd](https://github.com/RVCK-Project/rvck/commit/6c3d6bbd91afdf692e2fcbec986f38719a1df731) | 2025-05-23 | Clément Léger | riscv: sbi: add new SBI error mappings |
| [7c2d3a1b](https://github.com/RVCK-Project/rvck/commit/7c2d3a1b0dc4258c5cb6cafbcd307b7ff2b92e80) | 2025-05-23 | Clément Léger | riscv: sbi: remove useless parenthesis |
| [017558b5](https://github.com/RVCK-Project/rvck/commit/017558b51f4f5b1f9813d183b1f52c934cb40ccb) | 2025-05-23 | Clément Léger | riscv: sbi: add Firmware Feature (FWFT) SBI extensions definitions |
| [2e489f60](https://github.com/RVCK-Project/rvck/commit/2e489f6089d940ed16d6f1e499c629922136ef81) | 2024-04-02 | Chao Du | RISC-V: KVM: selftests: Add ebreak test support |
| [1c34124b](https://github.com/RVCK-Project/rvck/commit/1c34124b297eda4bf00e13eecc1d6d1e29e08b3e) | 2024-04-02 | Chao Du | RISC-V: KVM: Handle breakpoint exits for VCPU |
| [92d4b46d](https://github.com/RVCK-Project/rvck/commit/92d4b46db63a6481932787cc34898baac3af1748) | 2024-04-02 | Chao Du | RISC-V: KVM: Implement kvm_arch_vcpu_ioctl_set_guest_debug() |
| [ab06941e](https://github.com/RVCK-Project/rvck/commit/ab06941ec2c2b3791be42c62ecb45123d9e466c9) | 2024-01-11 | Paolo Bonzini | KVM: define __KVM_HAVE_GUEST_DEBUG unconditionally |
| [9fce1f58](https://github.com/RVCK-Project/rvck/commit/9fce1f58842d9fdc6dd7506a8954d7c471ada97b) | 2025-04-09 | Andrew Jones | riscv: Fix unaligned access info messages |
| [9655c898](https://github.com/RVCK-Project/rvck/commit/9655c898c759e66d421dab2a156f1991a2f1aa9e) | 2024-08-29 | Alexandre Ghiti | riscv: Fix RISCV_ALTERNATIVE_EARLY |
| [0288c5cb](https://github.com/RVCK-Project/rvck/commit/0288c5cb48a6da63765cffbbd2e4a34d47b9eaae) | 2024-03-22 | Alexandre Ghiti | riscv: Improve sbi_ecall() code generation by reordering arguments |
| [1647cb4c](https://github.com/RVCK-Project/rvck/commit/1647cb4cf600c72aac7e165114988fa83fd3e764) | 2024-03-21 | Samuel Holland | riscv: Add tracepoints for SBI calls and returns |
| [2a240981](https://github.com/RVCK-Project/rvck/commit/2a240981de449c7d71f5e3b70ce36508e5ce9de7) | 2025-04-22 | Clément Léger | riscv: misaligned: use get_user() instead of __get_user() |
| [055bcc29](https://github.com/RVCK-Project/rvck/commit/055bcc2949a33b8fa1df7e4c5f09eeb51bbb03e0) | 2025-04-22 | Clément Léger | riscv: misaligned: enable IRQs while handling misaligned accesses |
| [3ae13094](https://github.com/RVCK-Project/rvck/commit/3ae13094f9e642e9960f4fe02a14a637f858dfa2) | 2025-04-22 | Clément Léger | riscv: misaligned: factorize trap handling |
| [82cc10b9](https://github.com/RVCK-Project/rvck/commit/82cc10b9803825ce91b5902e2c52fb1afde8e860) | 2025-04-11 | Nylon Chen | riscv: misaligned: Add handling for ZCB instructions |
| [64dcec2e](https://github.com/RVCK-Project/rvck/commit/64dcec2ee6193b0fdc3c2827c4eaa0d71a27a19d) | 2025-04-11 | Nylon Chen | riscv: misaligned: fix sleeping function called during misaligned access handlin... |
| [1a0a10c9](https://github.com/RVCK-Project/rvck/commit/1a0a10c98b5c2af41b5dcdb3a7ce070ff0bc20cd) | 2025-02-28 | Alexandre Ghiti | riscv: Fix missing __free_pages() in check_vector_unaligned_access() |
| [f6129836](https://github.com/RVCK-Project/rvck/commit/f6129836ba62aa2749c0a3aed8adf4ad55690562) | 2025-02-28 | Tingbo Liao | riscv: Fix the __riscv_copy_vec_words_unaligned implementation |
| [fddb012f](https://github.com/RVCK-Project/rvck/commit/fddb012fb92a5eee994b5ffa36a81f43fa728315) | 2025-02-19 | Michal Wilczynski | riscv: dts: thead: Introduce power domain nodes with aon firmware |
| [ef2ec69d](https://github.com/RVCK-Project/rvck/commit/ef2ec69dfb2dc49674c63ffcdf515c89dcc80b62) | 2024-02-08 | Krzysztof Kozlowski | pmdomain: core: constify of_phandle_args in xlate |
| [c137dbb1](https://github.com/RVCK-Project/rvck/commit/c137dbb148e08406b629466dd9c77c76881c97d7) | 2025-03-11 | Michal Wilczynski | dt-bindings: power: Add TH1520 SoC power domains |
| [8f305488](https://github.com/RVCK-Project/rvck/commit/8f3054884373142881bfb4c081dcb6d9303f9bb8) | 2023-09-11 | Ulf Hansson | pmdomain: Prepare to move Kconfig files into the pmdomain subsystem |
| [96b96c70](https://github.com/RVCK-Project/rvck/commit/96b96c70d23c31d3b9568d6a74fb489a6006dac9) | 2025-03-14 | Arnd Bergmann | pmdomain: thead: fix TH1520_AON_PROTOCOL dependency |
| [96ec591e](https://github.com/RVCK-Project/rvck/commit/96ec591e0181a74887deaf307e9c9ddc7d77add2) | 2025-03-11 | Michal Wilczynski | pmdomain: thead: Add power-domain driver for TH1520 |
| [6d3ee292](https://github.com/RVCK-Project/rvck/commit/6d3ee292bba6a1d871825d9ca83e3b99d54b522f) | 2025-03-11 | Michal Wilczynski | firmware: thead: Add AON firmware protocol driver |
| [954b77cb](https://github.com/RVCK-Project/rvck/commit/954b77cb1dd3be9b77a0996be206614433017343) | 2025-10-20 | Xu Lu | RISC-V: KVM: Allow Zalasr extensions for Guest/VM |
| [4b986e08](https://github.com/RVCK-Project/rvck/commit/4b986e0873b3ef8c098df1a5d9a886ce7215c653) | 2025-08-08 | Quan Zhou | KVM: riscv: selftests: Add Zicbop extension to get-reg-list test |
| [1ad1220e](https://github.com/RVCK-Project/rvck/commit/1ad1220e7bf592df5a3f33ced1c57542e8b47999) | 2025-08-08 | Quan Zhou | RISC-V: KVM: Allow bfloat16 extension for Guest/VM |
| [8bed7a44](https://github.com/RVCK-Project/rvck/commit/8bed7a445828501ff5fbdf89cea67d1b07ebb604) | 2025-08-08 | Quan Zhou | RISC-V: KVM: Allow Zicbop extension for Guest/VM |
| [fc798ece](https://github.com/RVCK-Project/rvck/commit/fc798ece2c1b8f4372c96fbfc57bec802c11340a) | 2025-08-08 | Quan Zhou | RISC-V: KVM: Provide UAPI for Zicbop block size |
| [198f5c9d](https://github.com/RVCK-Project/rvck/commit/198f5c9d8e1cb24e251da0ed07de18a64192f940) | 2025-08-08 | Quan Zhou | RISC-V: KVM: Change zicbom/zicboz block size to depend on the host isa |
| [2d238382](https://github.com/RVCK-Project/rvck/commit/2d23838236cde3ab94f42577f59c1edf54d7d54f) | 2025-11-19 | Yao Zihong | riscv: hwprobe: Expose Zicbop extension and its block size |
| [5c37f806](https://github.com/RVCK-Project/rvck/commit/5c37f806b7f3679b07d4c7618a26ceb0201bcff2) | 2025-07-24 | Aleksa Paunovic | riscv: hwprobe: Add MIPS vendor extension probing |
| [893a4c19](https://github.com/RVCK-Project/rvck/commit/893a4c198ae0adae2ef8d3ebd94c6f20d3c62925) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Add SiFive vendor extension support and probe for xsfqmaccdod an... |
| [5439608a](https://github.com/RVCK-Project/rvck/commit/5439608a1075dc4ea8242a1bbfff87edd8216a6c) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Document SiFive xsfvqmaccdod and xsfvqmaccqoq vendor extensions |
| [fdbda8d0](https://github.com/RVCK-Project/rvck/commit/fdbda8d0712bfab41d1272ec0d5a6ba965085ad0) | 2024-11-13 | Charlie Jenkins | riscv: hwprobe: Add thead vendor extension probing |
| [a7760e73](https://github.com/RVCK-Project/rvck/commit/a7760e736511846ee515484f0325c71f7d00ca72) | 2025-10-20 | Xu Lu | riscv: Introduce Zalasr instructions |
| [5ddca5a0](https://github.com/RVCK-Project/rvck/commit/5ddca5a0d1e7e67dddcc105849af30b1eb77c498) | 2025-10-20 | Xu Lu | riscv: hwprobe: Export Zalasr extension |
| [148c42a0](https://github.com/RVCK-Project/rvck/commit/148c42a05dbb3c7a8929a0911f6207030545298b) | 2025-10-20 | Xu Lu | dt-bindings: riscv: Add Zalasr ISA extension description |
| [ffb89f4b](https://github.com/RVCK-Project/rvck/commit/ffb89f4bf48404a7c96b73ecdae5c5d2bf577563) | 2025-10-20 | Xu Lu | riscv: Add ISA extension parsing for Zalasr |
| [d03c91dc](https://github.com/RVCK-Project/rvck/commit/d03c91dc4e8cdbeb53f2f32311ba7d97289cbc14) | 2025-04-21 | Alexandre Ghiti | riscv: hwprobe: export Zabha extension |
| [65a60b68](https://github.com/RVCK-Project/rvck/commit/65a60b68cff9cffb3679ebcfee22f18c87a63931) | 2024-01-06 | Sun Haiyong | perf tools: Fix calloc() arguments to address error introduced in gcc-14 |
| [20ebf52d](https://github.com/RVCK-Project/rvck/commit/20ebf52db5d52e2c83e09e9dd81261ffb632b330) | 2023-12-04 | Sun Haiyong | perf top: Remove needless malloc(0) call that triggers -Walloc-size |
| [32643fc2](https://github.com/RVCK-Project/rvck/commit/32643fc2212391e8423d12108ab1bbb767ca7e07) | 2024-07-19 | Eric Lin | perf arch events: Fix duplicate RISC-V SBI firmware event name |
| [013f5a0f](https://github.com/RVCK-Project/rvck/commit/013f5a0fe98a0e166324b4f5b1234302bbe38689) | 2023-09-12 | Benjamin Gray | perf tools: Address python 3.6 DeprecationWarning for string scapes |
| [87e50f0c](https://github.com/RVCK-Project/rvck/commit/87e50f0c173c592704324fb61206c68944142857) | 2024-03-01 | Samuel Holland | riscv: mm: Fix prototype to avoid discarding const |
| [6784baa3](https://github.com/RVCK-Project/rvck/commit/6784baa32a30e4f1b0be565e7c295a05c7888460) | 2024-01-30 | Alexandre Ghiti | riscv: Fix arch_tlbbatch_flush() by clearing the batch cpumask |
| [3708c5b8](https://github.com/RVCK-Project/rvck/commit/3708c5b8392e22287d87ffcc04a1cfbc6f0ba4a3) | 2024-01-08 | Alexandre Ghiti | riscv: Add support for BATCHED_UNMAP_TLB_FLUSH |
| [6ce49edc](https://github.com/RVCK-Project/rvck/commit/6ce49edc070e63a58d87bb1bdb41b2a56695c6e9) | 2023-10-30 | Alexandre Ghiti | riscv: Improve flush_tlb_range() for hugetlb pages |
| [27e284c4](https://github.com/RVCK-Project/rvck/commit/27e284c4a9731ac37b89e2a4dabb80645ee5991c) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce NAPOT field to PTDUMP |
| [2c4c1440](https://github.com/RVCK-Project/rvck/commit/2c4c144002d70749d672263d36f27383bcbc4906) | 2023-09-21 | Yu Chien Peter Lin | riscv: Introduce PBMT field to PTDUMP |
| [29f99b6a](https://github.com/RVCK-Project/rvck/commit/29f99b6a2c8ca3a690438cf88ffc230dfac26b72) | 2023-09-21 | Yu Chien Peter Lin | riscv: Improve PTDUMP to show RSW with non-zero value |
| [dc926035](https://github.com/RVCK-Project/rvck/commit/dc92603594ab2e61cc22c7a324883513b80dc764) | 2024-06-05 | Björn Töpel | riscv: Enable DAX VMEMMAP optimization |
| [59a8a552](https://github.com/RVCK-Project/rvck/commit/59a8a552d631bb94aa127ccdb1fbff00332b99d8) | 2024-06-05 | Björn Töpel | riscv: mm: Add support for ZONE_DEVICE |
| [da1734f3](https://github.com/RVCK-Project/rvck/commit/da1734f3b297097f6bbf7c92f4e9072e0c629164) | 2024-06-05 | Björn Töpel | virtio-mem: Enable virtio-mem for RISC-V |
| [c77889d5](https://github.com/RVCK-Project/rvck/commit/c77889d59ebe3070813b0c5bfe5e2e3eacef79e8) | 2024-06-05 | Björn Töpel | riscv: Enable memory hotplugging for RISC-V |
| [3be99213](https://github.com/RVCK-Project/rvck/commit/3be99213b9167c0683bab831065c4b94f9dba245) | 2024-06-05 | Björn Töpel | riscv: mm: Take memory hotplug read-lock during kernel page table dump |
| [b8219760](https://github.com/RVCK-Project/rvck/commit/b8219760a17e4cf0408f47713345336514829683) | 2024-06-05 | Björn Töpel | riscv: mm: Add memory hotplugging support |
| [10c8fb41](https://github.com/RVCK-Project/rvck/commit/10c8fb41b1f4c42c9dacfc7b6dcf726edab7c1bc) | 2024-06-05 | Björn Töpel | riscv: mm: Add pfn_to_kaddr() implementation |
| [c54c7d1d](https://github.com/RVCK-Project/rvck/commit/c54c7d1d7acd076006d61e40462a904a7afb6297) | 2024-06-05 | Björn Töpel | riscv: mm: Refactor create_linear_mapping_range() for memory hot add |
| [1c63d4ef](https://github.com/RVCK-Project/rvck/commit/1c63d4ef292fea5157539e565b6d45cbcbca5905) | 2024-06-05 | Björn Töpel | riscv: mm: Change attribute from __init to __meminit for page functions |
| [43550492](https://github.com/RVCK-Project/rvck/commit/435504924eac931b54d349651cad57367458fcdb) | 2024-06-05 | Björn Töpel | riscv: mm: Pre-allocate vmemmap/direct map/kasan PGD entries |
| [b14b58ec](https://github.com/RVCK-Project/rvck/commit/b14b58ec7d575d1eca52e7fd2cc9e21f50792fad) | 2024-06-05 | Björn Töpel | riscv: mm: Properly forward vmemmap_populate() altmap parameter |
| [2ce6df7c](https://github.com/RVCK-Project/rvck/commit/2ce6df7c4899e3d707943f8218a12bc326ee1efe) | 2023-12-14 | Alexandre Ghiti | riscv: Use hugepage mappings for vmemmap |
| [d042103f](https://github.com/RVCK-Project/rvck/commit/d042103f790affb596c812d786fa8644c0bef7c5) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add commandline option for SBI PMU test |
| [ea1c755d](https://github.com/RVCK-Project/rvck/commit/ea1c755d276e431a4c294ba52a4492dafa072652) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for counter overflow |
| [cac4119d](https://github.com/RVCK-Project/rvck/commit/cac4119dad68b4df040c10656643ff119d550171) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add a test for PMU snapshot functionality |
| [faff089c](https://github.com/RVCK-Project/rvck/commit/faff089cc91ee97e4c76c975d17b81800b135b26) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU selftest |
| [a4d8ceac](https://github.com/RVCK-Project/rvck/commit/a4d8ceac0969dfad69f8891a35c8b32cbc9504b8) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add SBI PMU extension definitions |
| [a13fb217](https://github.com/RVCK-Project/rvck/commit/a13fb2174710bd06dad320676aad35a01519d573) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add Sscofpmf to get-reg-list test |
| [6c4314e0](https://github.com/RVCK-Project/rvck/commit/6c4314e0b3bf61d6fc7e3d2dba0644d392c021fc) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Add helper functions for extension checks |
| [4c713332](https://github.com/RVCK-Project/rvck/commit/4c7133323d13bdc63a6c077441a0b2c38bb2abe2) | 2024-04-20 | Atish Patra | KVM: riscv: selftests: Move sbi definitions to its own header file |
| [b1085331](https://github.com/RVCK-Project/rvck/commit/b108533106e56daf856d7656619ca18f4a670add) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add sstc timer test |
| [f7e9dbbc](https://github.com/RVCK-Project/rvck/commit/f7e9dbbcfbf2e627341c6cfcbfa0c13a29531b5c) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Change vcpu_has_ext to a common function |
| [4b08b137](https://github.com/RVCK-Project/rvck/commit/4b08b137cd42b54e50d44cead24907ba13ebab51) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add guest helper to get vcpu id |
| [d56230e4](https://github.com/RVCK-Project/rvck/commit/d56230e475666dc86b2a7f655fb5293a6b900dd3) | 2024-01-22 | Haibo Xu | KVM: riscv: selftests: Add exception handling support |
| [7407f836](https://github.com/RVCK-Project/rvck/commit/7407f836c1a4569ac7ccd3dfad36cba5945b0615) | 2024-01-22 | Haibo Xu | KVM: arm64: selftests: Split arch_timer test code |
| [98806dbc](https://github.com/RVCK-Project/rvck/commit/98806dbc91b39d7dca3b24b1b0df8018d53b5d1c) | 2024-01-22 | Paolo Bonzini | selftests/kvm: Fix issues with $(SPLIT_TESTS) |
| [ef5b29d9](https://github.com/RVCK-Project/rvck/commit/ef5b29d9a539b1c814a7d40ceb58b03219b85c6a) | 2024-04-20 | Atish Patra | RISC-V: KVM: Improve firmware counter read function |
| [2c07f0d7](https://github.com/RVCK-Project/rvck/commit/2c07f0d76505c26f567efbb29b1dafb396e54377) | 2024-04-20 | Atish Patra | RISC-V: KVM: Support 64 bit firmware counters on RV32 |
| [b639a556](https://github.com/RVCK-Project/rvck/commit/b639a5561bc979ade4cdd285a2667d78c2e4f6b1) | 2024-04-20 | Atish Patra | RISC-V: KVM: Add perf sampling support for guests |
| [fb755544](https://github.com/RVCK-Project/rvck/commit/fb7555446c8380573f44cb2bfb2f7d03b927c0bd) | 2024-04-20 | Atish Patra | RISC-V: KVM: Implement SBI PMU Snapshot feature |
| [72c59d43](https://github.com/RVCK-Project/rvck/commit/72c59d43492a4da7a97c2b406951dac1798021fe) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to exit to the user space if perf event failed |
| [8bcecc27](https://github.com/RVCK-Project/rvck/commit/8bcecc27a7ac3db31c28bee4d45daa4fe7c3580a) | 2024-04-20 | Atish Patra | RISC-V: KVM: No need to update the counter value during reset |
| [dd078689](https://github.com/RVCK-Project/rvck/commit/dd078689eff64dbce2793800c3f5735dcd5ce544) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Implement SBI PMU snapshot function |
| [ca7504ae](https://github.com/RVCK-Project/rvck/commit/ca7504ae79be1db30c351fc817cf93f2e6f7cbfc) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Fix counter mask iteration for RV32 |
| [77f8772f](https://github.com/RVCK-Project/rvck/commit/77f8772f18c1939b8f838ae83bcbc10a49922ae0) | 2024-04-20 | Atish Patra | RISC-V: Use the minor version mask while computing sbi version |
| [122df8ef](https://github.com/RVCK-Project/rvck/commit/122df8ef593f820ca28a17edfb05c677772eeeb5) | 2024-04-20 | Atish Patra | RISC-V: KVM: Rename the SBI_STA_SHMEM_DISABLE to a generic name |
| [aeb2742f](https://github.com/RVCK-Project/rvck/commit/aeb2742f18b4e9e0bf9bed286741a2c120512181) | 2024-04-20 | Atish Patra | RISC-V: Add SBI PMU snapshot definitions |
| [35e6e642](https://github.com/RVCK-Project/rvck/commit/35e6e64236017318f38abb9011f7ec32de4aac47) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Use BIT macro for shifting operations |
| [64f63400](https://github.com/RVCK-Project/rvck/commit/64f6340065b4e7bcca9f3dfe108a7c40c71ad741) | 2024-04-20 | Atish Patra | drivers/perf: riscv: Read upper bits of a firmware counter |
| [dc7513b9](https://github.com/RVCK-Project/rvck/commit/dc7513b9fa3336d39e8f4662e4a57b8e90da9fb6) | 2024-04-20 | Atish Patra | RISC-V: Add FIRMWARE_READ_HI definition |
| [48bc3a74](https://github.com/RVCK-Project/rvck/commit/48bc3a745a4653a957b19a4b433888a1b0d835ec) | 2024-04-20 | Atish Patra | RISC-V: Fix the typo in Scountovf CSR name |
| [7cb0909a](https://github.com/RVCK-Project/rvck/commit/7cb0909a2ca45d3950be53e56b33039895887901) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add get-reg-list test for STA registers |
| [9170ea74](https://github.com/RVCK-Project/rvck/commit/9170ea7408698ef6303dc873ae11f6a9fc1c9c6f) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add steal_time test support |
| [c3e4d91f](https://github.com/RVCK-Project/rvck/commit/c3e4d91fbc5ce746507a21876b63f672de8367a1) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Add guest_sbi_probe_extension |
| [a811945f](https://github.com/RVCK-Project/rvck/commit/a811945f62d912bd7f264f0cfb295df8047307a2) | 2023-12-20 | Andrew Jones | RISC-V: KVM: selftests: Move sbi_ecall to processor.c |
| [3330176a](https://github.com/RVCK-Project/rvck/commit/3330176ad6269af36df8db3bd8c338ae943b4885) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Implement SBI STA extension |
| [534cb89c](https://github.com/RVCK-Project/rvck/commit/534cb89c4e3d61c576f6a4cb2170531fa531b831) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI STA registers |
| [699f6401](https://github.com/RVCK-Project/rvck/commit/699f64019a48f9b4f7c4cc786ca097d3035a800c) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add support for SBI extension registers |
| [a7727a28](https://github.com/RVCK-Project/rvck/commit/a7727a285d8a7539f75cb0bc94fa404f6a6d1aee) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA info to vcpu_arch |
| [503a8148](https://github.com/RVCK-Project/rvck/commit/503a814801ab3e3a427bc00ef34694a3366d214b) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add steal-update vcpu request |
| [bff6669e](https://github.com/RVCK-Project/rvck/commit/bff6669eadcecef751f5d3c10b935e1ab71364af) | 2023-12-20 | Andrew Jones | RISC-V: KVM: Add SBI STA extension skeleton |
| [aa457853](https://github.com/RVCK-Project/rvck/commit/aa4578537d7257aa9135766543cb5cd587277fdf) | 2023-12-20 | Andrew Jones | RISC-V: paravirt: Implement steal-time support |
| [29f23bb7](https://github.com/RVCK-Project/rvck/commit/29f23bb7bff2c864fd32a15a31be7cf1d5061163) | 2023-12-20 | Andrew Jones | RISC-V: Add SBI STA extension definitions |
---

**共 1127 条提交，显示 201-400**

[1](软件所.md) **[2]** [3](软件所_page3.md) [4](软件所_page4.md) [5](软件所_page5.md) [6](软件所_page6.md)

[显示全部](软件所_all.md) | [纯文本视图](软件所_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-09-04 11:16:31*
*数据来源: 主分支 rvck-6.6@863dbf9d*
