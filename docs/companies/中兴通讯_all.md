# 中兴通讯 贡献详情

<div style="background-color: #FF980020; padding: 15px; border-radius: 8px; border-left: 5px solid #FF9800;">
<p><strong>📊 统计信息</strong></p>
<ul>
<li><strong>贡献提交数</strong>: 407</li>
<li><strong>统计时间</strong>: 2026-07-10 00:58:34</li>
<li><strong>主分支</strong>: rvck-6.6</li>
<li><strong>起始标签</strong>: v6.6.143</li>
</ul>
</div>

## 📧 识别规则

- **邮箱后缀**: @zte.com.cn

## 📋 提交列表

| 提交哈希 | 日期 | 原始作者 | 标题 |
|----------|------|----------|------|
| [a71ca51d](https://github.com/RVCK-Project/rvck/commit/a71ca51d719bf614f2847efc496e59ea1510265a) | 2026-06-17 | bailu | iommu/riscv: add dma_wmb before cmpxchg_relaxed |
| [9b1a0804](https://github.com/RVCK-Project/rvck/commit/9b1a08044c0459625cc06d59096e90617c3bd269) | 2026-06-30 | heyilin | PERF: Update LRW PMU Event Encoding |
| [2920b176](https://github.com/RVCK-Project/rvck/commit/2920b1765855bb4cc1c6489840c785a0ef5e098d) | 2026-01-28 | Yicong Yang | ACPI: scan: Use async schedule function in acpi_scan_clear_dep_fn() |
| [d2bbc0fd](https://github.com/RVCK-Project/rvck/commit/d2bbc0fd98c87f70febb4318485e843d8763b393) | 2025-11-17 | Anup Patel | RISC-V: Enable HOTPLUG_PARALLEL for secondary CPUs |
| [ef38af4d](https://github.com/RVCK-Project/rvck/commit/ef38af4df3e7f964339d8417d68f555d959897ad) | 2024-07-16 | Jiaxun Yang | cpu/hotplug: Provide weak fallback for arch_cpuhp_init_parallel_bringup() |
| [2af6734c](https://github.com/RVCK-Project/rvck/commit/2af6734c9c1fad983c6591aa681a66f549c7cef8) | 2024-07-16 | Jiaxun Yang | cpu/hotplug: Make HOTPLUG_PARALLEL independent of HOTPLUG_SMT |
| [de5e3d5c](https://github.com/RVCK-Project/rvck/commit/de5e3d5cb12e89a77f47ec7d69c8653f01d8d3c5) | 2026-05-29 | Lu Peng | riscv: Fix incorrect use of REG_L for 32-bit types in sse_entry.S |
| [af801f22](https://github.com/RVCK-Project/rvck/commit/af801f229bb7fd71754c32ab3f91f04c29084872) | 2026-04-03 | Feng Jiang | riscv: lib: add strrchr() implementation |
| [67e2b459](https://github.com/RVCK-Project/rvck/commit/67e2b459be1ff02f01c230d5af12b49a7682dda6) | 2026-04-03 | Feng Jiang | riscv: lib: add strchr() implementation |
| [80ddadf4](https://github.com/RVCK-Project/rvck/commit/80ddadf4a6aa6fdf304a36f76d429bbf5d5e1f77) | 2026-04-03 | Feng Jiang | riscv: lib: add strnlen() implementation |
| [10421c84](https://github.com/RVCK-Project/rvck/commit/10421c84fe6aec4543c7b9563ab8f35265f41c53) | 2026-04-03 | Feng Jiang | lib/string_kunit: extend benchmarks to strnlen() and chr searches |
| [1c24a6c8](https://github.com/RVCK-Project/rvck/commit/1c24a6c8a53525583c6d9ffb1c6ccfe373748631) | 2026-04-03 | Feng Jiang | lib/string_kunit: add performance benchmark for strlen() |
| [67017502](https://github.com/RVCK-Project/rvck/commit/67017502940303211bbe6c406a73540938fc7ab8) | 2026-04-03 | Feng Jiang | lib/string_kunit: add correctness test for strrchr() |
| [d2f69b99](https://github.com/RVCK-Project/rvck/commit/d2f69b99f247744eedc84776f40c188ceb2887c2) | 2026-04-03 | Feng Jiang | lib/string_kunit: add correctness test for strnlen() |
| [e35ca199](https://github.com/RVCK-Project/rvck/commit/e35ca199375e35a6574038685562e08ff35dfa73) | 2026-04-03 | Feng Jiang | lib/string_kunit: add correctness test for strlen() |
| [ab571725](https://github.com/RVCK-Project/rvck/commit/ab57172544678db46c7666c1f6993047058d9945) | 2024-04-24 | Kees Cook | string_kunit: Move strtomem KUnit test to string_kunit.c |
| [69a4b8f8](https://github.com/RVCK-Project/rvck/commit/69a4b8f87efcafa3894bac3e6d9bab8847fb8df6) | 2024-04-09 | Kees Cook | string.h: Introduce memtostr() and memtostr_pad() |
| [2470c06d](https://github.com/RVCK-Project/rvck/commit/2470c06d2d4469e3123c4f76a22056b4c0a74a5e) | 2024-04-19 | Kees Cook | string: Convert KUnit test names to standard convention |
| [75fd153d](https://github.com/RVCK-Project/rvck/commit/75fd153dee244b496723c554be507530a1ee908e) | 2024-04-19 | Kees Cook | string: Merge strcat KUnit tests into string_kunit.c |
| [73491b79](https://github.com/RVCK-Project/rvck/commit/73491b79726b065a6944faa131f68b48b4fcdc2b) | 2024-04-19 | Kees Cook | string: Prepare to merge strcat KUnit tests into string_kunit.c |
| [fe2f421d](https://github.com/RVCK-Project/rvck/commit/fe2f421d5262ceff8ede42bfd957eda5d94b2ff9) | 2024-04-19 | Kees Cook | string: Merge strscpy KUnit tests into string_kunit.c |
| [eb09b610](https://github.com/RVCK-Project/rvck/commit/eb09b610b5415a73febf3c1367d9ee9c94cba555) | 2024-04-19 | Kees Cook | string: Prepare to merge strscpy_kunit.c into string_kunit.c |
| [e0ea2ce4](https://github.com/RVCK-Project/rvck/commit/e0ea2ce4dce9019cf5e0db0f3143281722e8f7d7) | 2024-04-18 | Ivan Orlov | string_kunit: Add test cases for str*cmp functions |
| [9ac194dc](https://github.com/RVCK-Project/rvck/commit/9ac194dcfb66c93372d0606c4d3d550345a1de3d) | 2024-03-01 | Kees Cook | string: Convert helpers selftest to KUnit |
| [b0f0f611](https://github.com/RVCK-Project/rvck/commit/b0f0f611261d1c8a2e1003911a108d89a56840ed) | 2024-03-01 | Kees Cook | string: Convert selftest to KUnit |
| [36db68a1](https://github.com/RVCK-Project/rvck/commit/36db68a106436a2f2d263522fe699021fc2f5eb2) | 2024-02-29 | Andy Shevchenko | lib/string_helpers: Add flags param to string_get_size() |
| [8588ec6d](https://github.com/RVCK-Project/rvck/commit/8588ec6d77c611c7e0441652e284cc0ef81b5c85) | 2022-04-25 | Kent Overstreet | lib/string_helpers: string_get_size() now returns characters wrote |
| [def205bc](https://github.com/RVCK-Project/rvck/commit/def205bcf73e5d561754f103895ebd174f91face) | 2026-05-18 | Liu Qingtao | tmpfs: don't enable large folios if not supported |
| [ca7648bd](https://github.com/RVCK-Project/rvck/commit/ca7648bd283b62dcb46e233dd606bb29fc650b07) | 2026-05-20 | guowu | add LRW PCIE PMU support |
| [cef7fa11](https://github.com/RVCK-Project/rvck/commit/cef7fa11996f8c69d327c7283c8741254991f119) | 2026-04-16 | Lu Peng | riscv: add hardware error trap handler support |
| [78446c3c](https://github.com/RVCK-Project/rvck/commit/78446c3c40c379205fbe82fb7a12865dd360f062) | 2026-05-12 | Lu Peng | riscv: defconfig: Enable EDAC & EDAC_GHES config |
| [eb07ef0d](https://github.com/RVCK-Project/rvck/commit/eb07ef0d64806e2d83a2ed6988a9d71f6c3c413d) | 2025-02-17 | Andrew Jones | riscv: KVM: Fix SBI sleep_type use |
| [fca61999](https://github.com/RVCK-Project/rvck/commit/fca619995365d614f934d26ef9042e9065e9609e) | 2024-10-17 | Andrew Jones | KVM: riscv: selftests: Add SBI SUSP to get-reg-list test |
| [8890f08e](https://github.com/RVCK-Project/rvck/commit/8890f08e42c4e761faa3abb94f53c5a9d4de9b38) | 2024-10-17 | Andrew Jones | RISC-V: KVM: Add SBI system suspend support |
| [2d73024d](https://github.com/RVCK-Project/rvck/commit/2d73024db7320880b8e7dd7377ab38618886f0f7) | 2024-04-17 | Yong-Xuan Wang | RISCV: KVM: Introduce vcpu-\>reset_cntx_lock |
| [394ad59f](https://github.com/RVCK-Project/rvck/commit/394ad59f593e95a4e26378a214d1541c91489bcc) | 2025-03-17 | Atish Patra | RISC-V: KVM: Teardown riscv specific bits after kvm_exit |
| [6bc1f0b5](https://github.com/RVCK-Project/rvck/commit/6bc1f0b5f7ed53c6b805c25d9910d991c7aaf0e7) | 2025-02-17 | Andrew Jones | riscv: KVM: Fix hart suspend_type use |
| [fb9dc770](https://github.com/RVCK-Project/rvck/commit/fb9dc770d4919a4ec39f0f759b281ace6e960302) | 2024-08-15 | Anup Patel | RISC-V: KVM: Don't zero-out PMU snapshot area before freeing data |
| [bedb75ea](https://github.com/RVCK-Project/rvck/commit/bedb75eaa1871435e12df10d79ece6503a06619f) | 2026-03-30 | Wang Yechao | RISC-V: KVM: Split huge pages during fault handling for dirty logging |
| [92243da3](https://github.com/RVCK-Project/rvck/commit/92243da314e55d98085c3d27930dffb325d89579) | 2026-03-30 | Wang Yechao | RISC-V: KVM: Fix lost write protection on huge pages during dirty logging |
| [269a663b](https://github.com/RVCK-Project/rvck/commit/269a663b349ee13181c7a5ef2520cc9dd135e7b5) | 2026-02-26 | Wang Yechao | RISC-V: KVM: Skip THP support check during dirty logging |
| [07b226e6](https://github.com/RVCK-Project/rvck/commit/07b226e63f303c4dda2700db39c52003bc924ccd) | 2025-11-27 | Jessica Liu | RISC-V: KVM: Transparent huge page support |
| [b8ce0f19](https://github.com/RVCK-Project/rvck/commit/b8ce0f192771b45eda7955a1a279c3fb7aeb9353) | 2025-01-08 | Andrew Bresticker | riscv: Support huge pfnmaps |
| [7030422e](https://github.com/RVCK-Project/rvck/commit/7030422e58f0a81d91fd4d99109412f675b7c24d) | 2025-06-18 | Anup Patel | RISC-V: KVM: Pass VMID as parameter to kvm_riscv_hfence_xyz() APIs |
| [29c25ae2](https://github.com/RVCK-Project/rvck/commit/29c25ae266905ca8f3b177430320a3df5621e47e) | 2025-06-18 | Anup Patel | RISC-V: KVM: Factor-out g-stage page table management |
| [505d3102](https://github.com/RVCK-Project/rvck/commit/505d3102cc7219dd053fa0d03519da4624b5cb86) | 2025-06-18 | Anup Patel | RISC-V: KVM: Add vmid field to struct kvm_riscv_hfence |
| [3db93bf2](https://github.com/RVCK-Project/rvck/commit/3db93bf27c448a6c62368899b96168021d1420d9) | 2025-06-18 | Anup Patel | RISC-V: KVM: Introduce struct kvm_gstage_mapping |
| [273ce843](https://github.com/RVCK-Project/rvck/commit/273ce8434b77a425eaa3e63174d8182b1f64db15) | 2025-06-18 | Anup Patel | RISC-V: KVM: Factor-out MMU related declarations into separate headers |
| [06944f70](https://github.com/RVCK-Project/rvck/commit/06944f70b631c32e3ad34d82cbfe564a0ac1f834) | 2025-06-18 | Anup Patel | RISC-V: KVM: Use ncsr_xyz() in kvm_riscv_vcpu_trap_redirect() |
| [e4bfd939](https://github.com/RVCK-Project/rvck/commit/e4bfd93962e212b2a92fa00f6b2eef37901a5347) | 2025-06-18 | Anup Patel | RISC-V: KVM: Implement kvm_arch_flush_remote_tlbs_range() |
| [0784f764](https://github.com/RVCK-Project/rvck/commit/0784f764043a2279c70c41230215dee20e679a0e) | 2025-06-18 | Anup Patel | RISC-V: KVM: Don't flush TLB when PTE is unchanged |
| [af1e1af5](https://github.com/RVCK-Project/rvck/commit/af1e1af5ee0694374c82c441c4d71c79d4ae04de) | 2025-06-18 | Anup Patel | RISC-V: KVM: Replace KVM_REQ_HFENCE_GVMA_VMID_ALL with KVM_REQ_TLB_FLUSH |
| [78f16495](https://github.com/RVCK-Project/rvck/commit/78f16495dd9cabf550e32505d1835e93e79cd4af) | 2025-06-18 | Anup Patel | RISC-V: KVM: Rename and move kvm_riscv_local_tlb_sanitize() |
| [fdb93a38](https://github.com/RVCK-Project/rvck/commit/fdb93a38bcd7ec9137a229b12b566df9e18099c5) | 2025-06-18 | Anup Patel | RISC-V: KVM: Drop the return value of kvm_riscv_vcpu_aia_init() |
| [93f52fee](https://github.com/RVCK-Project/rvck/commit/93f52fee16b03a4f8a258301dbb37f2f453fb541) | 2025-06-18 | Anup Patel | RISC-V: KVM: Check kvm_riscv_vcpu_alloc_vector_context() return value |
| [6256677e](https://github.com/RVCK-Project/rvck/commit/6256677e8f7f9a52d59af7db3b099278f9a6a760) | 2025-06-02 | Cyril Bur | riscv: uaccess: Only restore the CSR_STATUS SUM bit |
| [929b5d0e](https://github.com/RVCK-Project/rvck/commit/929b5d0efcbe37c34517e78f1cd6a89acc56d612) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning in __get_user_error() |
| [a87581c1](https://github.com/RVCK-Project/rvck/commit/a87581c120837270bbcf83e47dd4ca5f27bc18a3) | 2025-06-10 | Palmer Dabbelt | RISC-V: uaccess: Wrap the get_user_8 uaccess macro |
| [37f179cd](https://github.com/RVCK-Project/rvck/commit/37f179cdeb2aca42f44d0b27a292f8d87c53862e) | 2025-07-25 | Aurelien Jarno | riscv: uaccess: fix __put_user_nocheck for unaligned accesses |
| [1daf17de](https://github.com/RVCK-Project/rvck/commit/1daf17de14d2e95bb0830fb4ed3a9314f3af502a) | 2025-07-15 | Nathan Chancellor | riscv: uaccess: Fix -Wuninitialized and -Wshadow in __put_user_nocheck |
| [5ee0a992](https://github.com/RVCK-Project/rvck/commit/5ee0a9927c4d776e3f5ab226561d332cee9f9a0d) | 2025-09-03 | Alexandre Ghiti | riscv: Fix sparse warning about different address spaces |
| [283921b5](https://github.com/RVCK-Project/rvck/commit/283921b5e2636a21a7a5fe6b420b71313bdb027b) | 2025-08-05 | Radim Krčmář | RISC-V: KVM: fix stack overrun when loading vlenb |
| [4a7ed6f9](https://github.com/RVCK-Project/rvck/commit/4a7ed6f99ce7d1548438f93319de35a23b85e30f) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: add 'vlenb' Vector CSR |
| [0c693f5c](https://github.com/RVCK-Project/rvck/commit/0c693f5c226f2def511cc5edfa0af5d51fa87264) | 2023-12-05 | Daniel Henrique Barboza | RISC-V: KVM: set 'vlenb' in kvm_riscv_vcpu_alloc_vector_context() |
| [d4520d49](https://github.com/RVCK-Project/rvck/commit/d4520d496ebf1a1a170cea1913b11322ede3b251) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use NACL HFENCEs for KVM request based HFENCEs |
| [af845bda](https://github.com/RVCK-Project/rvck/commit/af845bda803d9f183a945c08c02225a14faffd1e) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save trap CSRs in kvm_riscv_vcpu_enter_exit() |
| [f36906c8](https://github.com/RVCK-Project/rvck/commit/f36906c8c4bd64a5a03b9c2e8274ddc36b3f2c2b) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use SBI sync SRET call when available |
| [3c8a40e3](https://github.com/RVCK-Project/rvck/commit/3c8a40e3de07f9877588c0a62f8d2e67f60cd21e) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing AIA CSRs |
| [9cf0da4f](https://github.com/RVCK-Project/rvck/commit/9cf0da4f731bd1a8fe8f06bd661251d1baaef03e) | 2024-10-21 | Anup Patel | RISC-V: KVM: Use nacl_csr_xyz() for accessing H-extension CSRs |
| [eac1a6dd](https://github.com/RVCK-Project/rvck/commit/eac1a6ddfae24b949f43b0594b02a103902a129c) | 2024-10-21 | Anup Patel | RISC-V: KVM: Add common nested acceleration support |
| [9766caf2](https://github.com/RVCK-Project/rvck/commit/9766caf29e50f30d32cbc33e64df9d61639453d1) | 2024-10-21 | Anup Patel | RISC-V: Add defines for the SBI nested acceleration extension |
| [fdf90fbe](https://github.com/RVCK-Project/rvck/commit/fdf90fbe1f824420d0dee62b1ecea31ceae445ac) | 2024-10-21 | Anup Patel | RISC-V: KVM: Don't setup SGEI for zero guest external interrupts |
| [feda1a02](https://github.com/RVCK-Project/rvck/commit/feda1a02077f9fac3831853fd69a7851b05e42a1) | 2024-10-21 | Anup Patel | RISC-V: KVM: Replace aia_set_hvictl() with aia_hvictl_value() |
| [3852825a](https://github.com/RVCK-Project/rvck/commit/3852825afc715442d163888ac29a175f3f98fd64) | 2024-10-21 | Anup Patel | RISC-V: KVM: Break down the __kvm_riscv_switch_to() into macros |
| [a9e398f3](https://github.com/RVCK-Project/rvck/commit/a9e398f3dccd5b90629e0b9dbab8ed90b067a1c4) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore SCOUNTEREN in C source |
| [764100c0](https://github.com/RVCK-Project/rvck/commit/764100c0e527b35a44f99fe087fbaa5d301b2dac) | 2024-10-21 | Anup Patel | RISC-V: KVM: Save/restore HSTATUS in C source |
| [4d2a1c9c](https://github.com/RVCK-Project/rvck/commit/4d2a1c9c62ab828374d5b9a33fc235ef7b7bf4b7) | 2024-10-21 | Anup Patel | RISC-V: KVM: Order the object files alphabetically |
| [11d0e9bf](https://github.com/RVCK-Project/rvck/commit/11d0e9bf69c176c6c2f5e4fa402b4d85d1de38df) | 2024-07-08 | Jinjie Ruan | riscv: stacktrace: Add USER_STACKTRACE support |
| [0ca1e1b5](https://github.com/RVCK-Project/rvck/commit/0ca1e1b5e68e9854134bf16c0b203ffa8c812644) | 2023-10-24 | Clément Léger | riscv: kvm: use ".L" local labels in assembly when applicable |
| [f07c2cfe](https://github.com/RVCK-Project/rvck/commit/f07c2cfe8764e1baaf21cf163d8ba25812ac972e) | 2023-10-24 | Clément Léger | riscv: kvm: Use SYM_*() assembly macros instead of deprecated ones |
| [372853be](https://github.com/RVCK-Project/rvck/commit/372853beed9b711fbcfa449f2bec1ec4884b11a1) | 2025-08-18 | Anup Patel | MAINTAINERS: Add entry for RISC-V RPMI and MPXY drivers |
| [e418cc1c](https://github.com/RVCK-Project/rvck/commit/e418cc1c1bf22f37f453686ce6061d03489ba172) | 2025-08-18 | Anup Patel | RISC-V: Enable GPIO keyboard and event device in RV64 defconfig |
| [1c6ad275](https://github.com/RVCK-Project/rvck/commit/1c6ad27551a4d64fd8198ce1745a0dd8e62aab57) | 2025-08-18 | Sunil V L | irqchip/riscv-rpmi-sysmsi: Add ACPI support |
| [b5c8b3b5](https://github.com/RVCK-Project/rvck/commit/b5c8b3b52419e171fcf58c1d9dbc8968d6eb3843) | 2025-08-18 | Sunil V L | mailbox/riscv-sbi-mpxy: Add ACPI support |
| [cdda57c6](https://github.com/RVCK-Project/rvck/commit/cdda57c6b1b881a6f3a47d95c91d257223d81af9) | 2025-08-18 | Sunil V L | irqchip/irq-riscv-imsic-early: Export imsic_acpi_get_fwnode() |
| [2b1a54f1](https://github.com/RVCK-Project/rvck/commit/2b1a54f12d0f57441e61fa073cac4135d61d6351) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add RPMI System MSI to GSI mapping |
| [04aaee96](https://github.com/RVCK-Project/rvck/commit/04aaee96f54058fa9da6784f76e6976a15050592) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Add support to update gsi range |
| [62fb9c8e](https://github.com/RVCK-Project/rvck/commit/62fb9c8e277e23528ce5d4cef37de06753bc6f24) | 2025-08-18 | Sunil V L | ACPI: RISC-V: Create interrupt controller list in sorted order |
| [18004c91](https://github.com/RVCK-Project/rvck/commit/18004c91fbc7eb99bb734c72cc8ff14736f32c7b) | 2025-08-18 | Sunil V L | ACPI: scan: Update honor list for RPMI System MSI |
| [b62ba479](https://github.com/RVCK-Project/rvck/commit/b62ba479a99acfb971fa55d29378bb3dc967e71b) | 2025-08-18 | Sunil V L | ACPI: Add support for nargs_prop in acpi_fwnode_get_reference_args() |
| [3b05b982](https://github.com/RVCK-Project/rvck/commit/3b05b98257defb5ed4998fc03f1712e32af4aa29) | 2025-08-18 | Sunil V L | ACPI: property: Refactor acpi_fwnode_get_reference_args() to support nargs_prop |
| [4905dc12](https://github.com/RVCK-Project/rvck/commit/4905dc12be49cc71910fa9576bffae27f8e5afb0) | 2025-08-18 | Anup Patel | irqchip: Add driver for the RPMI system MSI service group |
| [717f0179](https://github.com/RVCK-Project/rvck/commit/717f01790e22d6b8edc36e6fbc8f14f4178b5f4a) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI interrupt controller bindings |
| [389243c2](https://github.com/RVCK-Project/rvck/commit/389243c2f07ce05f75f7c58a48685b8dbe8839ee) | 2025-08-18 | Anup Patel | dt-bindings: Add RPMI system MSI message proxy bindings |
| [a99bb57f](https://github.com/RVCK-Project/rvck/commit/a99bb57f2b68cb3ffa0114b106d9c8ed10d42cc5) | 2025-08-18 | Rahul Pathak | clk: Add clock driver for the RISC-V RPMI clock service group |
| [eab5d364](https://github.com/RVCK-Project/rvck/commit/eab5d3643a018ff0fe0fd4cd4adeb3dd09faffd1) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service controller bindings |
| [3db06e97](https://github.com/RVCK-Project/rvck/commit/3db06e976253fefa48eee7d6cd91d66264ded0b0) | 2025-08-18 | Anup Patel | dt-bindings: clock: Add RPMI clock service message proxy bindings |
| [5c41e7d3](https://github.com/RVCK-Project/rvck/commit/5c41e7d3b7f9aef0b927646ecead92f8c2df3113) | 2025-08-18 | Anup Patel | mailbox: Add RISC-V SBI message proxy (MPXY) based mailbox driver |
| [488d86b9](https://github.com/RVCK-Project/rvck/commit/488d86b9c5b67e3f7c2be8fe773ec9a01750553a) | 2025-08-18 | Anup Patel | byteorder: Add memcpy_to_le32() and memcpy_from_le32() |
| [561c3183](https://github.com/RVCK-Project/rvck/commit/561c3183a548217298b21c332b5a7e5fbd06dec1) | 2025-08-18 | Anup Patel | mailbox: Add common header for RPMI messages sent via mailbox |
| [f1590676](https://github.com/RVCK-Project/rvck/commit/f1590676a31670f22d2681aa58f85d068976dd8b) | 2025-08-18 | Anup Patel | RISC-V: Add defines for the SBI message proxy extension |
| [97c1f223](https://github.com/RVCK-Project/rvck/commit/97c1f2231b84889a41d1f59203743d1b94c5e067) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RISC-V SBI MPXY extension |
| [c3768486](https://github.com/RVCK-Project/rvck/commit/c3768486181b454464ee719a28472b873f8a4d4f) | 2025-08-18 | Anup Patel | dt-bindings: mailbox: Add bindings for RPMI shared memory transport |
| [1e679963](https://github.com/RVCK-Project/rvck/commit/1e6799637017d28646ecd04fb37b3eb0ccec5808) | 2025-02-12 | Andy Shevchenko | driver core: Split devres APIs to device/devres.h |
| [2d645e0a](https://github.com/RVCK-Project/rvck/commit/2d645e0a81f0f1ee39363de3d0972d659cb3a499) | 2023-12-26 | Tanzir Hasan | lib/string: shrink lib/string.i via IWYU |
| [60cfe85d](https://github.com/RVCK-Project/rvck/commit/60cfe85df444d445ffd89ef8452b9952c2aed8a8) | 2023-12-26 | Tanzir Hasan | kernel.h: removed REPEAT_BYTE from kernel.h |
| [3838707c](https://github.com/RVCK-Project/rvck/commit/3838707c3a65d512dd1dcbc4b9f223033c35349a) | 2025-04-09 | Sakari Ailus | Documentation: ACPI: Use all-string data node references |
| [dfd5c8ab](https://github.com/RVCK-Project/rvck/commit/dfd5c8ab0f287a98d9d45f54eb5de407c1bb2f92) | 2023-11-06 | Rafael J. Wysocki | ACPI: property: Support using strings in reference properties |
| [50871a15](https://github.com/RVCK-Project/rvck/commit/50871a1571b5197d54873d87fbf1e74246275e2c) | 2025-06-20 | Nam Cao | Revert "riscv: misaligned: fix sleeping function called during misaligned access... |
| [3c0a19cb](https://github.com/RVCK-Project/rvck/commit/3c0a19cbdf8d9f84b0bfcf61dc9d844bdce89e92) | 2025-06-20 | Alexandre Ghiti | riscv: Move all duplicate insn parsing macros into asm/insn.h |
| [cd3159d5](https://github.com/RVCK-Project/rvck/commit/cd3159d5c546b643e33c80f633cd2061a6f48630) | 2025-06-20 | Alexandre Ghiti | riscv: Strengthen duplicate and inconsistent definition of RV_X() |
| [e7a667b6](https://github.com/RVCK-Project/rvck/commit/e7a667b6a3c4daa72e533db9661353b028180578) | 2025-06-20 | Alexandre Ghiti | riscv: Fix typo EXRACT -\> EXTRACT |
| [0884cfd9](https://github.com/RVCK-Project/rvck/commit/0884cfd9a9a59b5a007ad3e697b7623e22396750) | 2025-07-10 | Andreas Schwab | riscv: traps_misaligned: properly sign extend value in misaligned load handler |
| [1b827953](https://github.com/RVCK-Project/rvck/commit/1b82795375ec6097e10332fbc456c0e97e12facc) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for SBI_FWFT_MISALIGNED_DELEG |
| [4e3ee40a](https://github.com/RVCK-Project/rvck/commit/4e3ee40a4bb95e4c64255952a2c8fc0159f1c239) | 2025-05-23 | Clément Léger | RISC-V: KVM: add support for FWFT SBI extension |
| [bb714355](https://github.com/RVCK-Project/rvck/commit/bb714355f529a2fbf24904a65741ddaef2fa431b) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension reset callback |
| [a171bc65](https://github.com/RVCK-Project/rvck/commit/a171bc65687b10c9e6d0e36434c1ae8e65ce51e5) | 2025-05-23 | Clément Léger | RISC-V: KVM: add SBI extension init()/deinit() functions |
| [8fd901a1](https://github.com/RVCK-Project/rvck/commit/8fd901a1cae7b8816cf14a81647f35aa2a66a202) | 2025-05-23 | Clément Léger | riscv: misaligned: add a function to check misalign trap delegability |
| [eb6accd0](https://github.com/RVCK-Project/rvck/commit/eb6accd06bbe9ea0d92b96ef0cbd1ef608673d33) | 2025-05-23 | Clément Léger | riscv: misaligned: move emulated access uniformity check in a function |
| [3a90f53b](https://github.com/RVCK-Project/rvck/commit/3a90f53b28ca58f00b77f7d8f668370a0612c69a) | 2025-05-23 | Clément Léger | riscv: misaligned: declare misaligned_access_speed under CONFIG_RISCV_MISALIGNED |
| [5b0426ac](https://github.com/RVCK-Project/rvck/commit/5b0426acf1249e09cdbc9d152bf2a1135d5f71f7) | 2025-05-23 | Clément Léger | riscv: misaligned: use on_each_cpu() for scalar misaligned access probing |
| [2d5df411](https://github.com/RVCK-Project/rvck/commit/2d5df4112e8a2cfce1c765cd3a1bdac533998760) | 2025-05-23 | Clément Léger | riscv: misaligned: request misaligned exception from SBI |
| [aa7ed7d4](https://github.com/RVCK-Project/rvck/commit/aa7ed7d4135cca58e98c96cf116d97bedc805dac) | 2025-05-23 | Clément Léger | riscv: sbi: add SBI FWFT extension calls |
| [e3d9d280](https://github.com/RVCK-Project/rvck/commit/e3d9d2803e66326a61012863e8ad1ecc0a608503) | 2025-05-23 | Clément Léger | riscv: sbi: add FWFT extension interface |
| [bd0c2b62](https://github.com/RVCK-Project/rvck/commit/bd0c2b6285820e921292101c8c9bbf8808b54c88) | 2025-05-23 | Clément Léger | riscv: sbi: add new SBI error mappings |
| [e780446e](https://github.com/RVCK-Project/rvck/commit/e780446e0304d5d810b10dc75941b1b1be9d6319) | 2025-05-23 | Clément Léger | riscv: sbi: remove useless parenthesis |
| [32723aec](https://github.com/RVCK-Project/rvck/commit/32723aec550e4ca6e3dbb9a0296df57c9aaacb42) | 2025-05-23 | Clément Léger | riscv: sbi: add Firmware Feature (FWFT) SBI extensions definitions |
| [757b28ec](https://github.com/RVCK-Project/rvck/commit/757b28ec60031fc83aa7ec8a78be9a81026fc2c3) | 2024-04-02 | Chao Du | RISC-V: KVM: selftests: Add ebreak test support |
| [03ccd35e](https://github.com/RVCK-Project/rvck/commit/03ccd35e007872a3876db0db25675ceea5d05ab2) | 2024-04-02 | Chao Du | RISC-V: KVM: Handle breakpoint exits for VCPU |
| [b53e49ae](https://github.com/RVCK-Project/rvck/commit/b53e49aeb0475baba137465c4a27750ba87c2e0f) | 2024-04-02 | Chao Du | RISC-V: KVM: Implement kvm_arch_vcpu_ioctl_set_guest_debug() |
| [377e23c6](https://github.com/RVCK-Project/rvck/commit/377e23c67915c717223256851dfbc6d5a913791d) | 2024-01-11 | Paolo Bonzini | KVM: define __KVM_HAVE_GUEST_DEBUG unconditionally |
| [ec667067](https://github.com/RVCK-Project/rvck/commit/ec66706720fca5619e7819dd3890e6688f16529d) | 2025-04-09 | Andrew Jones | riscv: Fix unaligned access info messages |
| [65b8d648](https://github.com/RVCK-Project/rvck/commit/65b8d648c596d98bc08b056e78ee650045ac5393) | 2024-08-29 | Alexandre Ghiti | riscv: Fix RISCV_ALTERNATIVE_EARLY |
| [c2cd2feb](https://github.com/RVCK-Project/rvck/commit/c2cd2feb42bb19a2fc2600b5eabfaddf5a3d84f4) | 2024-03-22 | Alexandre Ghiti | riscv: Improve sbi_ecall() code generation by reordering arguments |
| [26004313](https://github.com/RVCK-Project/rvck/commit/260043131a44db7e19bca78618a061865e6963e4) | 2024-03-21 | Samuel Holland | riscv: Add tracepoints for SBI calls and returns |
| [f9a72a51](https://github.com/RVCK-Project/rvck/commit/f9a72a51b16231f586b9c1f5d19ee3449cd1dc5f) | 2025-04-22 | Clément Léger | riscv: misaligned: use get_user() instead of __get_user() |
| [d62ed64b](https://github.com/RVCK-Project/rvck/commit/d62ed64be21373ecd91d5210fe2f94eebe42e4c2) | 2025-04-22 | Clément Léger | riscv: misaligned: enable IRQs while handling misaligned accesses |
| [807df416](https://github.com/RVCK-Project/rvck/commit/807df4167df0fc9e5f52954288f2deeb6d38aec8) | 2025-04-22 | Clément Léger | riscv: misaligned: factorize trap handling |
| [c8e67648](https://github.com/RVCK-Project/rvck/commit/c8e67648fd64e3698a354f17c63e541450cd2670) | 2025-04-11 | Nylon Chen | riscv: misaligned: Add handling for ZCB instructions |
| [e4844b42](https://github.com/RVCK-Project/rvck/commit/e4844b42ff4169291ded0a3cf50a5a5cee92f2ad) | 2025-04-11 | Nylon Chen | riscv: misaligned: fix sleeping function called during misaligned access handlin... |
| [fd097d16](https://github.com/RVCK-Project/rvck/commit/fd097d16b49852bb7874fdd467205a119de97dad) | 2025-02-28 | Alexandre Ghiti | riscv: Fix missing __free_pages() in check_vector_unaligned_access() |
| [98ba7c59](https://github.com/RVCK-Project/rvck/commit/98ba7c59d173d1dbfef872a40ab44943693bf415) | 2025-02-28 | Tingbo Liao | riscv: Fix the __riscv_copy_vec_words_unaligned implementation |
| [8e5892a8](https://github.com/RVCK-Project/rvck/commit/8e5892a8b82b70c32114c50ae17bc720edad2760) | 2025-10-20 | Xu Lu | RISC-V: KVM: Allow Zalasr extensions for Guest/VM |
| [0b0ca32d](https://github.com/RVCK-Project/rvck/commit/0b0ca32d3031cafe34a29b1dff607f09715ba36b) | 2025-07-24 | Aleksa Paunovic | riscv: hwprobe: Add MIPS vendor extension probing |
| [4d94c76c](https://github.com/RVCK-Project/rvck/commit/4d94c76c851a49dacd929be971cfb0c20fede063) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Add SiFive vendor extension support and probe for xsfqmaccdod an... |
| [5fad0d3c](https://github.com/RVCK-Project/rvck/commit/5fad0d3cb5e85a1fd5123b42b0b7ac8dc98468e6) | 2025-04-18 | Cyan Yang | riscv: hwprobe: Document SiFive xsfvqmaccdod and xsfvqmaccqoq vendor extensions |
| [21e49e64](https://github.com/RVCK-Project/rvck/commit/21e49e64f307d3df93198ec54f1a23c6a9c7c1ff) | 2024-11-13 | Charlie Jenkins | riscv: hwprobe: Add thead vendor extension probing |
| [4e8027f9](https://github.com/RVCK-Project/rvck/commit/4e8027f93b1f4d9d2257fe395fa8d7aea8d441e6) | 2025-10-20 | Xu Lu | riscv: Introduce Zalasr instructions |
| [3eadca06](https://github.com/RVCK-Project/rvck/commit/3eadca060881feef1674086865cf714fa1a92a6c) | 2025-10-20 | Xu Lu | riscv: hwprobe: Export Zalasr extension |
| [bbca422c](https://github.com/RVCK-Project/rvck/commit/bbca422c74efa35a31f2662a7618008702126ec5) | 2025-10-20 | Xu Lu | dt-bindings: riscv: Add Zalasr ISA extension description |
| [27296e5a](https://github.com/RVCK-Project/rvck/commit/27296e5a2a48eef16df87a9290f7a79516b6d8b5) | 2025-10-20 | Xu Lu | riscv: Add ISA extension parsing for Zalasr |
| [88327869](https://github.com/RVCK-Project/rvck/commit/88327869df3e31ed814918f7ef71d911f910fe45) | 2025-04-21 | Alexandre Ghiti | riscv: hwprobe: export Zabha extension |
| [cfe0db59](https://github.com/RVCK-Project/rvck/commit/cfe0db59ab7537ab81ab7beba43691b9e63ac32b) | 2024-03-01 | Samuel Holland | riscv: mm: Fix prototype to avoid discarding const |
| [21a49de1](https://github.com/RVCK-Project/rvck/commit/21a49de190706c737cefddfdbf8a037874aa0cf9) | 2024-01-30 | Alexandre Ghiti | riscv: Fix arch_tlbbatch_flush() by clearing the batch cpumask |
| [a81aa965](https://github.com/RVCK-Project/rvck/commit/a81aa965207b8869545dad7300256bb41f8d5d86) | 2024-01-08 | Alexandre Ghiti | riscv: Add support for BATCHED_UNMAP_TLB_FLUSH |
| [eb56badf](https://github.com/RVCK-Project/rvck/commit/eb56badfff96f8cdf00dba63be416ad82f076cbd) | 2023-10-30 | Alexandre Ghiti | riscv: Improve flush_tlb_range() for hugetlb pages |
| [19e1fa98](https://github.com/RVCK-Project/rvck/commit/19e1fa9861cd66bc0350d35dfc29d2075a83071a) | 2026-02-05 | shenlin | perf vendor events riscv: fix lrw core PMU event mapping |
| [eed4f321](https://github.com/RVCK-Project/rvck/commit/eed4f32111b61455508e48414940320edb7e04e2) | 2024-06-05 | Björn Töpel | riscv: Enable DAX VMEMMAP optimization |
| [77034074](https://github.com/RVCK-Project/rvck/commit/77034074e0b7dac4a665b778f77c2159b25e6d38) | 2024-06-05 | Björn Töpel | riscv: mm: Add support for ZONE_DEVICE |
| [fa6c4cc5](https://github.com/RVCK-Project/rvck/commit/fa6c4cc5275db6f5d0e6169ee091074da84b2ef8) | 2024-06-05 | Björn Töpel | virtio-mem: Enable virtio-mem for RISC-V |
| [d31d504b](https://github.com/RVCK-Project/rvck/commit/d31d504b3d062e5134576fb781aadbdf59b0e4a7) | 2024-06-05 | Björn Töpel | riscv: Enable memory hotplugging for RISC-V |
| [4017ebb7](https://github.com/RVCK-Project/rvck/commit/4017ebb7fe80dc948b6b63b9480b9bf4419593fd) | 2024-06-05 | Björn Töpel | riscv: mm: Take memory hotplug read-lock during kernel page table dump |
| [f3016abf](https://github.com/RVCK-Project/rvck/commit/f3016abfa06b04f5ebddfe612d3aac8256bb34e4) | 2024-06-05 | Björn Töpel | riscv: mm: Add memory hotplugging support |
| [2158f54d](https://github.com/RVCK-Project/rvck/commit/2158f54d2f1bb3e3ed719608b65e97697e88990d) | 2024-06-05 | Björn Töpel | riscv: mm: Add pfn_to_kaddr() implementation |
| [4cb30863](https://github.com/RVCK-Project/rvck/commit/4cb30863cfe87d467015028de8530afe30359e78) | 2024-06-05 | Björn Töpel | riscv: mm: Refactor create_linear_mapping_range() for memory hot add |
| [5bc6da0d](https://github.com/RVCK-Project/rvck/commit/5bc6da0d7902744e200eb5d770641c6716d49e5b) | 2024-06-05 | Björn Töpel | riscv: mm: Change attribute from __init to __meminit for page functions |
| [1b736b89](https://github.com/RVCK-Project/rvck/commit/1b736b8904144a59aa30bb7c5343d877dfef7bb5) | 2024-06-05 | Björn Töpel | riscv: mm: Pre-allocate vmemmap/direct map/kasan PGD entries |
| [1e932578](https://github.com/RVCK-Project/rvck/commit/1e932578b9a200ebbe0714da597fe7ecac0901fa) | 2024-06-05 | Björn Töpel | riscv: mm: Properly forward vmemmap_populate() altmap parameter |
| [548f7277](https://github.com/RVCK-Project/rvck/commit/548f7277bb3977fb939cd0b0c14e579f9f1eca8b) | 2023-12-14 | Alexandre Ghiti | riscv: Use hugepage mappings for vmemmap |
| [084530e8](https://github.com/RVCK-Project/rvck/commit/084530e86cdbc94d2685dfba527e4d79201e17ce) | 2023-11-06 | Evan Green | RISC-V: Probe misaligned access speed in parallel |
| [2352c28a](https://github.com/RVCK-Project/rvck/commit/2352c28aa72975b20131d25e4db0ca5c1df6d003) | 2023-11-06 | Evan Green | RISC-V: Remove __init on unaligned_emulation_finish() |
| [9ba8ab99](https://github.com/RVCK-Project/rvck/commit/9ba8ab99d480de3af33e84b0bc78fe57a1d48f09) | 2023-10-04 | Clément Léger | riscv: add support for PR_SET_UNALIGN and PR_GET_UNALIGN |
| [c401b16f](https://github.com/RVCK-Project/rvck/commit/c401b16ff5f2767929451c2fe6b3da4cf46d1028) | 2023-10-04 | Clément Léger | riscv: report misaligned accesses emulation to hwprobe |
| [9e7173dd](https://github.com/RVCK-Project/rvck/commit/9e7173dd0a192ac981c3233af124617b3a7c78db) | 2023-10-04 | Clément Léger | riscv: add support for sysctl unaligned_enabled control |
| [d506da7f](https://github.com/RVCK-Project/rvck/commit/d506da7f2ac90db188d4bbf57d09d6ae79f241b9) | 2023-10-04 | Clément Léger | riscv: add floating point insn support to misaligned access emulation |
| [e8c47156](https://github.com/RVCK-Project/rvck/commit/e8c4715677fb7fdde9a27fbe52f75d874ec1cd02) | 2023-10-04 | Clément Léger | riscv: report perf event for misaligned fault |
| [1e102db3](https://github.com/RVCK-Project/rvck/commit/1e102db38d56102dea3468ea690388783a96f40c) | 2023-10-04 | Clément Léger | riscv: add support for misaligned trap handling in S-mode |
| [ed0625dd](https://github.com/RVCK-Project/rvck/commit/ed0625dde47f6e330ce4b5b64ee43050ef305350) | 2026-01-17 | Lu Peng | riscv: defconfig: Enable more ACPI_APEI configs |
| [3440e3e8](https://github.com/RVCK-Project/rvck/commit/3440e3e8814c000a732625844f6a42e1eb01833f) | 2025-07-23 | Ignacio Encinas | riscv: introduce asm/swab.h |
| [de57fe60](https://github.com/RVCK-Project/rvck/commit/de57fe605485303deb60fd862a70275784d102be) | 2026-01-07 | hu.yuye | riscv:defconfig:Enable PCIE_EDR |
| [5371d97a](https://github.com/RVCK-Project/rvck/commit/5371d97a5364e341b190dd997814b1ef76c529db) | 2025-12-31 | Yunhui Cui | arch_topology: move parse_acpi_topology() to common code |
| [bbe4fc4f](https://github.com/RVCK-Project/rvck/commit/bbe4fc4f1381db38c979f775410b97aa60689d55) | 2025-12-31 | Yicong Yang | arm64: topology: Support SMT control on ACPI based system |
| [7675bcab](https://github.com/RVCK-Project/rvck/commit/7675bcab5f1586aebd54b304276277a924fd0849) | 2025-12-31 | Yicong Yang | arch_topology: Support SMT control for OF based system |
| [dfa7a7b6](https://github.com/RVCK-Project/rvck/commit/dfa7a7b65566e1bbc99798a9bf7b55a5be4ea3f6) | 2025-12-31 | Yicong Yang | cpu/SMT: Provide a default topology_is_primary_thread() |
| [8f405c7e](https://github.com/RVCK-Project/rvck/commit/8f405c7ef4165050cd0f73be3e5ac886fb0d627a) | 2025-01-24 | Andy Shevchenko | serial: 8250_core: Remove unneeded -\>iotype assignment |
| [3d8bb8d5](https://github.com/RVCK-Project/rvck/commit/3d8bb8d5e415b69f0ac4d40ecf9d4c7e5fadd990) | 2025-12-30 | hu.yuye | Revert "mango pci hack:broadcast when no MSI source known" |
| [7d768a80](https://github.com/RVCK-Project/rvck/commit/7d768a803973c8fd20a17d41c3cf0b0a1301c8f8) | 2025-12-22 | Clément Léger | riscv: uaccess: do not do misaligned accesses in get/put_user() |
| [90490f08](https://github.com/RVCK-Project/rvck/commit/90490f08e78cc968fdd441b840e004d1e8c788c0) | 2025-12-22 | Alexandre Ghiti | riscv: make unsafe user copy routines use existing assembly routines |
| [2f85887d](https://github.com/RVCK-Project/rvck/commit/2f85887d2045bc973a5ddfae93251de619175ddc) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm_goto_output' for get_user() |
| [e4248887](https://github.com/RVCK-Project/rvck/commit/e4248887e694cc742ed79ed5bf10fffd741a0c07) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use 'asm goto' for put_user() |
| [fdf030f7](https://github.com/RVCK-Project/rvck/commit/fdf030f741358a120504127aaa9bb0ea543b64dc) | 2025-12-22 | Jisheng Zhang | riscv: uaccess: use input constraints for ptr of __put_user() |
| [68b46029](https://github.com/RVCK-Project/rvck/commit/68b460293d01fa82b5e1567b1649d0a77d4f38ac) | 2025-12-22 | Jisheng Zhang | riscv: implement user_access_begin() and families |
| [b4e444b1](https://github.com/RVCK-Project/rvck/commit/b4e444b12b378f38f856b7697dc0640639253ade) | 2025-12-22 | Ben Dooks | riscv: save the SR_SUM status over switches |
| [3e11453f](https://github.com/RVCK-Project/rvck/commit/3e11453fdf8c7f4507e76ec967b8832e823edf15) | 2011-12-08 | Tejun Heo | Revert "mm: Modify __find_max_addr for memory hole" |
| [d53590a8](https://github.com/RVCK-Project/rvck/commit/d53590a81a6d77d08e6929d4926ffe8a73f691d7) | 2021-12-06 | Alexandre Ghiti | Revert "riscv: mm: Clear compilation warning about last_cpupid" |
| [dcf3885e](https://github.com/RVCK-Project/rvck/commit/dcf3885e68dbd05a16629dcf4db3862377596530) | 2025-11-12 | shenlin | perf vendor events riscv: add lrw core JSON file with metric support |
| [dc9de32e](https://github.com/RVCK-Project/rvck/commit/dc9de32e5080fc10f384c3069973b345e35f2258) | 2025-11-11 | Fei Liu | i2c: Add driver for the LRW I2C |
| [d27df592](https://github.com/RVCK-Project/rvck/commit/d27df592a14c626285bc29fbae024fc024343bc0) | 2025-09-29 | Fei Liu | dt-bindings: i2c: Add binding for LRW I2C |
| [e9d7b7d7](https://github.com/RVCK-Project/rvck/commit/e9d7b7d755eae5a503a50b0f7e38b7c6f5a86c1b) | 2025-11-12 | Jie Feng | drivers/perf: add LRW DDR PMU support |
| [913d3668](https://github.com/RVCK-Project/rvck/commit/913d3668105858b00ee7962a5a3299fac987f507) | 2025-09-04 | Wenhong Liu | serial: Add driver for the LRW UART |
| [6dc2d517](https://github.com/RVCK-Project/rvck/commit/6dc2d5177e7919fab6a8233e75844b96e1956d72) | 2025-09-04 | Wenhong Liu | dt-bindings: serial: Add binding for LRW UART |
| [cd7c9c25](https://github.com/RVCK-Project/rvck/commit/cd7c9c253cebb73c336d50970b903fc27b9bedcb) | 2025-10-16 | Wenhong Liu | riscv: defconfig: remove CONFIG_CMDLINE and CONFIG_CMDLINE_EXTEND as mainline do... |
| [544d2f93](https://github.com/RVCK-Project/rvck/commit/544d2f937d238706ac6b8cb9b1c344f91e811d6e) | 2025-08-27 | Himanshu Chauhan | riscv: Enable APEI and NMI safe cmpxchg options required for RAS |
| [1eb4fea7](https://github.com/RVCK-Project/rvck/commit/1eb4fea7008eb50e6550eeaf9b3590348c2a1452) | 2025-08-27 | Himanshu Chauhan | riscv: Add config option to enable APEI SSE handler |
| [ecc53ede](https://github.com/RVCK-Project/rvck/commit/ecc53ede4249628f2e5fb84cc60e4becdc291681) | 2025-08-27 | Himanshu Chauhan | riscv: Introduce HEST SSE notification handlers |
| [3a6e5705](https://github.com/RVCK-Project/rvck/commit/3a6e5705739c053951dc0f4b484a0244fe64a479) | 2025-08-27 | Himanshu Chauhan | riscv: Add RISC-V entries in processor type and ISA strings |
| [467d75dd](https://github.com/RVCK-Project/rvck/commit/467d75dd4b3986cd74ddbe299c83059587f5a86c) | 2025-08-27 | Himanshu Chauhan | riscv: Add functions to register ghes having SSE notification |
| [aaba5988](https://github.com/RVCK-Project/rvck/commit/aaba59887454f23027179beca6f160800351b373) | 2025-08-27 | Himanshu Chauhan | riscv: conditionally compile GHES NMI spool function |
| [b36bfdb1](https://github.com/RVCK-Project/rvck/commit/b36bfdb15940d641a42ad2785180e3c2d19d0b0c) | 2025-08-27 | Himanshu Chauhan | riscv: Add fixmap indices for GHES IRQ and SSE contexts |
| [c37057b4](https://github.com/RVCK-Project/rvck/commit/c37057b41709bc11096b53ae567448cf3b9b4194) | 2025-08-27 | Himanshu Chauhan | acpi: Introduce SSE in HEST notification types |
| [98b016c7](https://github.com/RVCK-Project/rvck/commit/98b016c7c1918acb6548f48d6a06ec3ba1f1a3c2) | 2025-08-27 | Himanshu Chauhan | riscv: Define arch_apei_get_mem_attribute for RISC-V |
| [70683122](https://github.com/RVCK-Project/rvck/commit/706831226cccc205dd48151415b0557bdb39852e) | 2025-08-27 | Himanshu Chauhan | riscv: Define ioremap_cache for RISC-V |
| [c1bc05a0](https://github.com/RVCK-Project/rvck/commit/c1bc05a0dc84fe1d15e23e95fa289817e3b95172) | 2025-08-27 | Clément Léger | selftests/riscv: add SSE test module |
| [f8c41f01](https://github.com/RVCK-Project/rvck/commit/f8c41f0176f51bd19179d4165c2eca840aa30710) | 2025-08-27 | Clément Léger | perf: RISC-V: add support for SSE event |
| [fe0f00d8](https://github.com/RVCK-Project/rvck/commit/fe0f00d87c2d9a272b18a323a2ffb39619b7d1f0) | 2025-08-27 | Clément Léger | drivers: firmware: add riscv SSE support |
| [e1153fd9](https://github.com/RVCK-Project/rvck/commit/e1153fd9a9cfa1053421444b12fd8c31a1ea39e0) | 2025-08-27 | Clément Léger | riscv: add support for SBI Supervisor Software Events extension |
| [284420e1](https://github.com/RVCK-Project/rvck/commit/284420e12abcb74cb3bdd3b1078f1aa31c43d321) | 2025-08-08 | Clément Léger | riscv: add SBI SSE extension definitions |
| [dacf3e33](https://github.com/RVCK-Project/rvck/commit/dacf3e3362bf2a07c865990320a1bb228ecb955a) | 2025-10-14 | Sunil V L | iommu/riscv: Add ACPI support |
| [874e9e63](https://github.com/RVCK-Project/rvck/commit/874e9e6309a32f0261ea79b59e62fcd2fb85bbf9) | 2025-10-14 | Sunil V L | ACPI: scan: Add support for RISC-V in acpi_iommu_configure_id() |
| [5fd4097b](https://github.com/RVCK-Project/rvck/commit/5fd4097b0461485e6d68d6889ce685054e71372d) | 2025-10-14 | Sunil V L | ACPI: RISC-V: Add support for RIMT |
| [48a16ccb](https://github.com/RVCK-Project/rvck/commit/48a16ccbde9dcd03407df09171603931376dc8be) | 2025-10-13 | Sunil V L | ACPICA: actbl2: Add definitions for RIMT |
| [f696bab6](https://github.com/RVCK-Project/rvck/commit/f696bab687687e0de1fcd99249d3ea98b898209e) | 2025-09-20 | shenlin | perf vendor events riscv: add lrw core JSON file |
| [bb0a9a1a](https://github.com/RVCK-Project/rvck/commit/bb0a9a1a4a5b4941c63d0362bdb734b7b3d1f484) | 2025-04-21 | Alexandre Ghiti | riscv: Add support for Zicbop |
| [23d7b523](https://github.com/RVCK-Project/rvck/commit/23d7b52312fa8b3b4e30b6c5c30827fa93222ad4) | 2025-04-21 | Alexandre Ghiti | riscv: Introduce Zicbop instructions |
| [04ba9b49](https://github.com/RVCK-Project/rvck/commit/04ba9b4967e1f56da5b59a4feb5ea3182aea24ec) | 2025-02-26 | Yunhui Cui | RISC-V: Enable cbo.clean/flush in usermode |
| [8cf1ee23](https://github.com/RVCK-Project/rvck/commit/8cf1ee2385036412652fcf2775e3108f5b37daa9) | 2024-08-14 | Samuel Holland | riscv: Add support for per-thread envcfg CSR values |
| [530632b6](https://github.com/RVCK-Project/rvck/commit/530632b69c2774554f1be0998e469f8da2cc5d6e) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zaamo/Zalrsc extensions for Guest/VM |
| [d4c316f4](https://github.com/RVCK-Project/rvck/commit/d4c316f45065a1744654e2c88cee8287c0dff2e1) | 2024-06-19 | Clément Léger | riscv: hwprobe: export Zaamo and Zalrsc extensions |
| [1bb40926](https://github.com/RVCK-Project/rvck/commit/1bb40926fb20857e1a87f1c3c4f9cddf7d88a0e8) | 2024-06-19 | Clément Léger | riscv: add parsing for Zaamo and Zalrsc extensions |
| [0d2140e4](https://github.com/RVCK-Project/rvck/commit/0d2140e42d2fb98fdc75506c03d1f3c9cfe98b6d) | 2025-02-13 | Inochi Amaoto | riscv: hwprobe: export bfloat16 ISA extension |
| [4d564e1a](https://github.com/RVCK-Project/rvck/commit/4d564e1a9dc923522acaab74f703e914825f97ca) | 2025-02-13 | Inochi Amaoto | riscv: add ISA extension parsing for bfloat16 ISA extension |
| [9a9a0a18](https://github.com/RVCK-Project/rvck/commit/9a9a0a183faed1adee607787ea6367aaafaa8d94) | 2024-05-24 | Xiao Wang | riscv, bpf: Introduce shift add helper with Zba optimization |
| [8797a7c5](https://github.com/RVCK-Project/rvck/commit/8797a7c5fdf327f06f59d0f30ef5b924f673e30c) | 2024-05-16 | Xiao Wang | riscv, bpf: Optimize zextw insn with Zba extension |
| [b9572cbb](https://github.com/RVCK-Project/rvck/commit/b9572cbb5e76a486e46c68411532780114097a2c) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize bswap insns with Zbb support |
| [3d8a1bbb](https://github.com/RVCK-Project/rvck/commit/3d8a1bbb88cea0e1a290663a7a0130785601a567) | 2024-01-15 | Pu Lehui | riscv, bpf: Optimize sign-extention mov insns with Zbb support |
| [3385d717](https://github.com/RVCK-Project/rvck/commit/3385d717ca44dea03b2521870bdd5c281259ba53) | 2024-01-15 | Pu Lehui | riscv, bpf: Add necessary Zbb instructions |
| [9ef4bcf9](https://github.com/RVCK-Project/rvck/commit/9ef4bcf9d0698a4b989f4142cb3db2d690dabd1e) | 2024-01-15 | Pu Lehui | riscv, bpf: Simplify sext and zext logics in branch instructions |
| [912f9b2f](https://github.com/RVCK-Project/rvck/commit/912f9b2fa5486ea1de699ac3c0beb5cf32cb4983) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit zero-extension to emit_zextw |
| [fb5770e9](https://github.com/RVCK-Project/rvck/commit/fb5770e97653bbebdae8f6de339fa2382406aa15) | 2024-01-15 | Pu Lehui | riscv, bpf: Unify 32-bit sign-extension to emit_sextw |
| [16a10344](https://github.com/RVCK-Project/rvck/commit/16a103441cf1c7120fef1778d8586bdbdb052b32) | 2025-03-12 | Robin Murphy | iommu: Don't warn prematurely about dodgy probes |
| [af92a430](https://github.com/RVCK-Project/rvck/commit/af92a4300aafd58e54a6196afb1b589a3402a44a) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_domain_alloc() |
| [da39d4a3](https://github.com/RVCK-Project/rvck/commit/da39d4a3c6a0539da20a370ff19b0a55e4f3d28f) | 2024-10-09 | Lu Baolu | iommu: Remove iommu_present() |
| [a336afe2](https://github.com/RVCK-Project/rvck/commit/a336afe2a1448c5a0a7438832c608d47d0a2b6ab) | 2024-09-02 | Lu Baolu | drm/tegra: Use iommu_paging_domain_alloc() |
| [7e045085](https://github.com/RVCK-Project/rvck/commit/7e0450850e106c50d2b065f175f8dc6afd52d1a0) | 2024-09-02 | Lu Baolu | drm/rockchip: Use iommu_paging_domain_alloc() |
| [bd6ec5f7](https://github.com/RVCK-Project/rvck/commit/bd6ec5f7f35cbeb0d0e822115891c9ec16260c28) | 2024-06-10 | Lu Baolu | RDMA/usnic: Use iommu_paging_domain_alloc() |
| [6ee614b9](https://github.com/RVCK-Project/rvck/commit/6ee614b9439ddbd2715f545e9e66e1a4b5c32a22) | 2024-08-12 | Lu Baolu | soc: fsl: qbman: Use iommu_paging_domain_alloc() |
| [14043e86](https://github.com/RVCK-Project/rvck/commit/14043e864011ee1315cbaf088629032c69989a07) | 2024-08-12 | Lu Baolu | remoteproc: Use iommu_paging_domain_alloc() |
| [986f43aa](https://github.com/RVCK-Project/rvck/commit/986f43aaf300d635cca0bedf6d17cddbe9c69d1c) | 2024-08-12 | Lu Baolu | media: venus: firmware: Use iommu_paging_domain_alloc() |
| [830d983e](https://github.com/RVCK-Project/rvck/commit/830d983ea44634c69edb59effaca2e35a98fe815) | 2024-08-12 | Lu Baolu | media: nvidia: tegra: Use iommu_paging_domain_alloc() |
| [cbc75436](https://github.com/RVCK-Project/rvck/commit/cbc75436593534ab2a41bf565252ab061a557213) | 2024-08-12 | Lu Baolu | gpu: host1x: Use iommu_paging_domain_alloc() |
| [b1d402c5](https://github.com/RVCK-Project/rvck/commit/b1d402c51cf0551b95a293f216d6375fcaf5b463) | 2024-09-02 | Lu Baolu | drm/nouveau/tegra: Use iommu_paging_domain_alloc() |
| [78f05eb7](https://github.com/RVCK-Project/rvck/commit/78f05eb715e636e4765c55a377890f109580f654) | 2024-06-10 | Lu Baolu | wifi: ath11k: Use iommu_paging_domain_alloc() |
| [715ea2bd](https://github.com/RVCK-Project/rvck/commit/715ea2bd145b75d9fe451d14d5d09958c86afe30) | 2024-06-10 | Lu Baolu | wifi: ath10k: Use iommu_paging_domain_alloc() |
| [e9a34ba0](https://github.com/RVCK-Project/rvck/commit/e9a34ba0e666690fc1b793d363e5c86479c62e10) | 2024-06-10 | Lu Baolu | drm/msm: Use iommu_paging_domain_alloc() |
| [b46253ae](https://github.com/RVCK-Project/rvck/commit/b46253aef91c2c012100dd8d2c920764d8a7c19e) | 2024-06-10 | Lu Baolu | vhost-vdpa: Use iommu_paging_domain_alloc() |
| [edb2fbe8](https://github.com/RVCK-Project/rvck/commit/edb2fbe8acb3df190ffe25a7ded068bfe12d3254) | 2024-06-10 | Lu Baolu | vfio/type1: Use iommu_paging_domain_alloc() |
| [2180b0c7](https://github.com/RVCK-Project/rvck/commit/2180b0c7ef9104bf7b5d0d74b8839c54ba84bb8e) | 2024-06-10 | Lu Baolu | iommufd: Use iommu_paging_domain_alloc() |
| [3922fd14](https://github.com/RVCK-Project/rvck/commit/3922fd147fcf3165350997446db1745433c8e1e5) | 2024-06-10 | Lu Baolu | iommu: Add iommu_paging_domain_alloc() interface |
| [3cfd97db](https://github.com/RVCK-Project/rvck/commit/3cfd97db1e72b2903bf50fe1b88cf13c5e6c672b) | 2025-02-28 | Robin Murphy | iommu: Get DT/ACPI parsing into the proper probe path |
| [eee981f6](https://github.com/RVCK-Project/rvck/commit/eee981f6dcdd6525836dc6cf746ac67579207611) | 2025-02-28 | Robin Murphy | iommu: Keep dev-\>iommu state consistent |
| [2601849b](https://github.com/RVCK-Project/rvck/commit/2601849b4d7d5d568cdf92e49b52ba6432ed7a67) | 2025-02-28 | Robin Murphy | iommu: Resolve ops in iommu_init_device() |
| [68c54beb](https://github.com/RVCK-Project/rvck/commit/68c54bebd3219846005ecfe5220fb1327b6c2f56) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [4a4be9c4](https://github.com/RVCK-Project/rvck/commit/4a4be9c44bc5d8b99b5e41c419f93fd58fd671cc) | 2025-02-27 | Robin Murphy | iommu: Unexport iommu_fwspec_free() |
| [1e63686a](https://github.com/RVCK-Project/rvck/commit/1e63686ae72191cacebd0c266a9a62a3a0bb2890) | 2024-07-02 | Robin Murphy | iommu: Remove iommu_fwspec ops |
| [b9d9c0d7](https://github.com/RVCK-Project/rvck/commit/b9d9c0d7a8adc544704f4f9f3f88b2c9d26c3d5c) | 2024-07-02 | Robin Murphy | OF: Simplify of_iommu_configure() |
| [ee9bcb88](https://github.com/RVCK-Project/rvck/commit/ee9bcb88c8f7622964c6b2d522c78adf84e13a36) | 2024-07-02 | Robin Murphy | ACPI: Retire acpi_iommu_fwspec_ops() |
| [199e767a](https://github.com/RVCK-Project/rvck/commit/199e767aa3267da4c401e54c65b80bcb99c43478) | 2024-07-02 | Robin Murphy | iommu: Resolve fwspec ops automatically |
| [e425983f](https://github.com/RVCK-Project/rvck/commit/e425983f3cabb46096883d76aedaf09a6764ae63) | 2023-12-07 | Jason Gunthorpe | acpi: Do not return struct iommu_ops from acpi_iommu_configure_id() |
| [d499a197](https://github.com/RVCK-Project/rvck/commit/d499a197ceff4800c5ab61658259ddc277f270ba) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_priv_set() with a lockdep |
| [c6e6dbf7](https://github.com/RVCK-Project/rvck/commit/c6e6dbf7c46a082a36185fb75886accfd65abfc0) | 2023-12-07 | Jason Gunthorpe | iommu: Mark dev_iommu_get() with lockdep |
| [38f7dc2d](https://github.com/RVCK-Project/rvck/commit/38f7dc2d9af8cb50b3a73daa9a913d750878d9ce) | 2023-12-07 | Jason Gunthorpe | iommu/of: Use -ENODEV consistently in of_iommu_configure() |
| [bd4292ac](https://github.com/RVCK-Project/rvck/commit/bd4292ac1aade7dabe00c0d312894712ab0a9e36) | 2023-12-07 | Jason Gunthorpe | iommmu/of: Do not return struct iommu_ops from of_iommu_configure() |
| [d4f57492](https://github.com/RVCK-Project/rvck/commit/d4f574922b497da87c8966816c0b3ec7f8593767) | 2023-12-07 | Jason Gunthorpe | iommu: Remove struct iommu_ops *iommu from arch_setup_dma_ops() |
| [94305e83](https://github.com/RVCK-Project/rvck/commit/94305e83bc34c4f8cd67d8e7caec20f2207225f7) | 2023-11-21 | Robin Murphy | iommu: Clean up open-coded ownership checks |
| [2a961c9a](https://github.com/RVCK-Project/rvck/commit/2a961c9ab043434f2d9b1e51adcfb2ec201e9248) | 2023-11-21 | Robin Murphy | iommu: Retire bus ops |
| [f16c2af7](https://github.com/RVCK-Project/rvck/commit/f16c2af7b74969700c3426edde3becff9b3a0c39) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_domain_alloc() from bus ops |
| [39c98742](https://github.com/RVCK-Project/rvck/commit/39c9874211b4fd6559651f492f6e1c1774d79c49) | 2023-11-21 | Robin Murphy | iommu: Validate that devices match domains |
| [d3e1fcf4](https://github.com/RVCK-Project/rvck/commit/d3e1fcf462861085acb4cb6530268b3d4ef385a5) | 2023-11-21 | Robin Murphy | iommu: Decouple iommu_present() from bus ops |
| [c2a2d5b1](https://github.com/RVCK-Project/rvck/commit/c2a2d5b1335bec93b7bf4efeb8966bc16d8ee643) | 2023-11-21 | Robin Murphy | iommu: Factor out some helpers |
| [3477a9aa](https://github.com/RVCK-Project/rvck/commit/3477a9aa8c1d335eead45411e233d719913b1af0) | 2024-03-13 | Xiao Wang | riscv: uaccess: Relax the threshold for fast path |
| [3f30162a](https://github.com/RVCK-Project/rvck/commit/3f30162afb5c5cacc4caf497828feee0285ee7e4) | 2024-03-13 | Xiao Wang | riscv: uaccess: Allow the last potential unrolled copy |
| [1cefc899](https://github.com/RVCK-Project/rvck/commit/1cefc8990373b7d09101c77c742ada7b470e9381) | 2024-12-24 | Atish Patra | RISC-V: KVM: Add new exit statstics for redirected traps |
| [4a24b990](https://github.com/RVCK-Project/rvck/commit/4a24b99051a91f90e97484949db9e70e81ca2e9b) | 2024-12-24 | Atish Patra | RISC-V: KVM: Update firmware counters for various events |
| [cd4a9b39](https://github.com/RVCK-Project/rvck/commit/cd4a9b39f2622becdfe89a43f0336b77ae5048b8) | 2024-04-29 | Yu-Wei Hsu | RISC-V: KVM: Redirect AMO load/store access fault traps to guest |
| [ab102091](https://github.com/RVCK-Project/rvck/commit/ab102091b1e146766e40e499ba3f0660e70ea710) | 2025-01-03 | Xu Lu | iommu/riscv: Add shutdown function for iommu driver |
| [47207ba5](https://github.com/RVCK-Project/rvck/commit/47207ba59c08254e823d3e935f500a78fc249958) | 2025-01-03 | Xu Lu | iommu/riscv: Empty iommu queue before enabling it |
| [f5e7eb6e](https://github.com/RVCK-Project/rvck/commit/f5e7eb6e1084d4a5084a29528231f4e48f998c7a) | 2024-11-12 | Andrew Jones | iommu/riscv: Add support for platform msi |
| [71eb24ff](https://github.com/RVCK-Project/rvck/commit/71eb24ff92aab14fb2299ff41b10a4670454a42e) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Paging domain support |
| [489a2012](https://github.com/RVCK-Project/rvck/commit/489a2012efa071f583ad0d1b0fde030154b749c0) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Command and fault queue support |
| [e3b9d7b5](https://github.com/RVCK-Project/rvck/commit/e3b9d7b5fefe683dc365f9b7c724db18a8807105) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Device directory management. |
| [15db8e59](https://github.com/RVCK-Project/rvck/commit/15db8e59e2d100cd05c41c6ec6b3f47b1d3775a0) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Enable IOMMU registration and device probe. |
| [c01b1122](https://github.com/RVCK-Project/rvck/commit/c01b112204ca049f0eb23bc60ca10032992586dc) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU PCIe device driver |
| [4619c530](https://github.com/RVCK-Project/rvck/commit/4619c530921cbfef817bdce78a58383df110f3fb) | 2024-10-15 | Tomasz Jeznach | iommu/riscv: Add RISC-V IOMMU platform device driver |
| [9613a9d2](https://github.com/RVCK-Project/rvck/commit/9613a9d24c08a7af66d65bb10416512599bdedba) | 2024-10-15 | Tomasz Jeznach | dt-bindings: iommu: riscv: Add bindings for RISC-V IOMMU |
| [3395d6c1](https://github.com/RVCK-Project/rvck/commit/3395d6c1a8b9047ec367d37549c19a860e20786d) | 2024-04-13 | Pasha Tatashin | iommu/vt-d: add wrapper functions for page allocations |
| [14c37b36](https://github.com/RVCK-Project/rvck/commit/14c37b36b861d00a102f020161637d706a3d6a99) | 2024-02-16 | Krzysztof Kozlowski | iommu: constify of_phandle_args in xlate |
| [9b0590ab](https://github.com/RVCK-Project/rvck/commit/9b0590abc5ab4593f8ef3874e80d64e37c10ca9d) | 2023-11-22 | Matt Coster | sizes.h: Add entries between SZ_32G and SZ_64T |
| [66e761d4](https://github.com/RVCK-Project/rvck/commit/66e761d4dc3ba68c1189f90da25d4271c3197e0f) | 2024-03-05 | Lu Baolu | iommu: Add static iommu_ops-\>release_domain |
| [cfe54fd4](https://github.com/RVCK-Project/rvck/commit/cfe54fd4384b7f737b1d97b713d8baf2eb393785) | 2023-09-27 | Jason Gunthorpe | iommufd: Convert to alloc_domain_paging() |
| [2eceb630](https://github.com/RVCK-Project/rvck/commit/2eceb630244c94d588826357524df49219cd54cd) | 2024-04-13 | Pasha Tatashin | iommu: Move IOMMU_DOMAIN_BLOCKED global statics to ops-\>blocked_domain |
| [c5e0f737](https://github.com/RVCK-Project/rvck/commit/c5e0f7373a4ba67de7b1061ff322a3a46f698db1) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new userspace mappings with Svvpt... |
| [22ff621d](https://github.com/RVCK-Project/rvck/commit/22ff621d2bd2a1adb1d6d69f92123674449dcd0f) | 2024-07-17 | Alexandre Ghiti | riscv: Stop emitting preventive sfence.vma for new vmalloc mappings |
| [62b37fea](https://github.com/RVCK-Project/rvck/commit/62b37fea24f8494a1c374801114e9c8968e4475c) | 2023-10-20 | Anup Patel | KVM: riscv: selftests: Add SBI DBCN extension to get-reg-list test |
| [6f6f4957](https://github.com/RVCK-Project/rvck/commit/6f6f49577cc7d88ce8ed8ef2682461913f020be1) | 2022-07-22 | Anup Patel | RISC-V: KVM: Forward SBI DBCN extension to user-space |
| [9219d4cd](https://github.com/RVCK-Project/rvck/commit/9219d4cde459525c9a520adec1088001c07be0c4) | 2023-10-11 | Anup Patel | RISC-V: KVM: Allow some SBI extensions to be disabled by default |
| [909800ff](https://github.com/RVCK-Project/rvck/commit/909800ff832e0b502477986b2df5278ab5f91c22) | 2023-10-10 | Anup Patel | RISC-V: KVM: Change the SBI specification version to v2.0 |
| [f981eaca](https://github.com/RVCK-Project/rvck/commit/f981eaca5df4a3d9592ef89e4472bd038a0344a1) | 2022-07-22 | Anup Patel | RISC-V: Add defines for SBI debug console extension |
| [17ea4eff](https://github.com/RVCK-Project/rvck/commit/17ea4eff279493e4d776d83742069a75d0db3d14) | 2023-11-24 | Anup Patel | RISC-V: Enable SBI based earlycon support |
| [bff5ca95](https://github.com/RVCK-Project/rvck/commit/bff5ca9592ec2d8404f9307ebdabe4f376f42615) | 2023-11-24 | Atish Patra | tty: Add SBI debug console support to HVC SBI driver |
| [a0159afd](https://github.com/RVCK-Project/rvck/commit/a0159afdd1a4199175e3316e170bd934ffacef28) | 2023-11-24 | Anup Patel | tty/serial: Add RISC-V SBI debug console based earlycon |
| [51531281](https://github.com/RVCK-Project/rvck/commit/515312819fe2cf27b33a539f197b4c006cf9b230) | 2023-11-24 | Anup Patel | RISC-V: Add SBI debug console helper routines |
| [51679ea2](https://github.com/RVCK-Project/rvck/commit/51679ea26e9303fbfb508a459d4b363cca9df1ec) | 2023-11-24 | Anup Patel | RISC-V: Add stubs for sbi_console_putchar/getchar() |
| [901ca5ab](https://github.com/RVCK-Project/rvck/commit/901ca5ab17d1d0e29ec517bd02e9ccddb636605f) | 2024-04-03 | Björn Töpel | riscv: Fix vector state restore in rt_sigreturn() |
| [2c4400b3](https://github.com/RVCK-Project/rvck/commit/2c4400b3fd639bc771b25490f46826f08cca217f) | 2024-01-15 | Andy Chiu | riscv: vector: allow kernel-mode Vector with preemption |
| [a64b7783](https://github.com/RVCK-Project/rvck/commit/a64b77832c7084dcd4d09d64cbb05ca22b60542a) | 2024-01-15 | Andy Chiu | riscv: vector: use kmem_cache to manage vector context |
| [60801aab](https://github.com/RVCK-Project/rvck/commit/60801aab3792bd4eba0b8b4458d029cf50fe268e) | 2024-01-15 | Andy Chiu | riscv: vector: use a mask to write vstate_ctrl |
| [a40f7ded](https://github.com/RVCK-Project/rvck/commit/a40f7ded535c75f66b9399e6b5d2a952ebbb2d27) | 2024-01-15 | Andy Chiu | riscv: vector: do not pass task_struct into riscv_v_vstate_{save,restore}() |
| [d26e1c36](https://github.com/RVCK-Project/rvck/commit/d26e1c3650b9e690f8e022690aa19e598a6232d8) | 2024-01-15 | Andy Chiu | riscv: fpu: drop SR_SD bit checking |
| [b842b8b9](https://github.com/RVCK-Project/rvck/commit/b842b8b935d7370a23a015cf2ac5a310fc11698a) | 2024-01-15 | Andy Chiu | riscv: lib: vectorize copy_to_user/copy_from_user |
| [d4c0b66b](https://github.com/RVCK-Project/rvck/commit/d4c0b66bf88c4b600322d8d4e3357edf23765c49) | 2024-01-15 | Andy Chiu | riscv: sched: defer restoring Vector context for user |
| [75885764](https://github.com/RVCK-Project/rvck/commit/758857643c278b5379f58db081b49a92253c8512) | 2024-01-15 | Greentime Hu | riscv: Add vector extension XOR implementation |
| [ba75a467](https://github.com/RVCK-Project/rvck/commit/ba75a467f6092ee488932dd49f003e3d44710b7a) | 2024-01-15 | Andy Chiu | riscv: vector: make Vector always available for softirq context |
| [9c4753fc](https://github.com/RVCK-Project/rvck/commit/9c4753fc6b91d58f2be87fe5d9aad838995fe899) | 2024-01-15 | Greentime Hu | riscv: Add support for kernel mode vector |
| [47007221](https://github.com/RVCK-Project/rvck/commit/47007221711aa0a5365452f7afb6251db2507b08) | 2023-10-24 | Clément Léger | riscv: kernel: Use correct SYM_DATA_*() macro for data |
| [6b01f71f](https://github.com/RVCK-Project/rvck/commit/6b01f71f4bb99863b2265daad51e626643fd5eaf) | 2023-10-24 | Clément Léger | riscv: Use SYM_*() assembly macros instead of deprecated ones |
| [a2f5bfe6](https://github.com/RVCK-Project/rvck/commit/a2f5bfe6dc2cc04e5c27a85a734d0b9503e84000) | 2023-10-24 | Clément Léger | riscv: use ".L" local labels in assembly when applicable |
| [b09f7be3](https://github.com/RVCK-Project/rvck/commit/b09f7be37bb6f5fa1bea9e13c5c8cc1625e396db) | 2024-11-03 | Alexandre Ghiti | riscv: Add qspinlock support |
| [0804deab](https://github.com/RVCK-Project/rvck/commit/0804deab201fe4ed673fa59a6aa5cf4724b39ba9) | 2024-11-03 | Alexandre Ghiti | riscv: Implement xchg8/16() using Zabha |
| [e8f64c1d](https://github.com/RVCK-Project/rvck/commit/e8f64c1d4024c63ddc2833fd0c0582be045e068c) | 2024-11-03 | Alexandre Ghiti | riscv: Implement arch_cmpxchg128() using Zacas |
| [1b58a8f7](https://github.com/RVCK-Project/rvck/commit/1b58a8f7e81d0e472be388ee310caf235a792fdc) | 2024-11-03 | Alexandre Ghiti | riscv: Improve zacas fully-ordered cmpxchg() |
| [78696281](https://github.com/RVCK-Project/rvck/commit/786962812630726d189b8d91bf781a32f51fbb1c) | 2024-07-26 | Yong-Xuan Wang | RISC-V: KVM: Add Svade and Svadu Extensions Support for Guest/VM |
| [f811000c](https://github.com/RVCK-Project/rvck/commit/f811000c3c1add3f4c1319cbf6492f19ca8785fb) | 2024-10-16 | Samuel Holland | RISC-V: KVM: Allow Smnpm and Ssnpm extensions for guests |
| [7fb5fb98](https://github.com/RVCK-Project/rvck/commit/7fb5fb989fae2b3b51e952a64eb5bf264bc97890) | 2024-04-26 | Andrew Jones | KVM: riscv: Support guest wrs.nto |
| [28be7a86](https://github.com/RVCK-Project/rvck/commit/28be7a86cb36bcfefb873b464ca1633c97f01740) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zcmop extension for Guest/VM |
| [7df57b2b](https://github.com/RVCK-Project/rvck/commit/7df57b2b44da44e9eb7d14bb7b4d6f0929398c6d) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zca, Zcf, Zcd and Zcb extensions for Guest/VM |
| [02efbe06](https://github.com/RVCK-Project/rvck/commit/02efbe065eca30250f2d2d71faf90e64d4f605d1) | 2024-06-19 | Clément Léger | RISC-V: KVM: Allow Zimop extension for Guest/VM |
| [2a16dfdc](https://github.com/RVCK-Project/rvck/commit/2a16dfdc2754f3d1cbaa96f715d42d84846470c6) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Zacas extension for Guest/VM |
| [3563df5b](https://github.com/RVCK-Project/rvck/commit/3563df5bfbebbb7c741f2b1e871d385b30ca68f5) | 2024-02-13 | Anup Patel | RISC-V: KVM: Allow Ztso extension for Guest/VM |
| [79203c18](https://github.com/RVCK-Project/rvck/commit/79203c18850263b52c5aad73a1022bc5abad7921) | 2024-02-13 | Anup Patel | RISC-V: KVM: Forward SEED CSR access to user space |
| [a3fe0788](https://github.com/RVCK-Project/rvck/commit/a3fe0788abb3f1242bffe58465502a0ad8cbf49a) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfa extension for Guest/VM |
| [a721bb90](https://github.com/RVCK-Project/rvck/commit/a721bb906b4a5619004b4dba5bec0d9d623a3ca3) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zvfh[min] extensions for Guest/VM |
| [47b297c5](https://github.com/RVCK-Project/rvck/commit/47b297c5636a4cf7d9d9e1368bab5ddefecdc245) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zihintntl extension for Guest/VM |
| [eab451af](https://github.com/RVCK-Project/rvck/commit/eab451af01576629d80520418f683512cf2eff8f) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zfh[min] extensions for Guest/VM |
| [b4c38d86](https://github.com/RVCK-Project/rvck/commit/b4c38d86f25f0b7f0b19a06be88965824d2016d1) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow vector crypto extensions for Guest/VM |
| [d649b0db](https://github.com/RVCK-Project/rvck/commit/d649b0dbf48ec97199c62fe27c1f3f86b5fdf688) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow scalar crypto extensions for Guest/VM |
| [ef5112aa](https://github.com/RVCK-Project/rvck/commit/ef5112aa1af49566eb2c77cbcc6dc42f21bd4e39) | 2023-11-27 | Anup Patel | RISC-V: KVM: Allow Zbc extension for Guest/VM |
| [e24f062b](https://github.com/RVCK-Project/rvck/commit/e24f062ba996d7876c7cacb0911dc9fb8513e846) | 2023-09-15 | Anup Patel | RISC-V: KVM: Allow Zicond extension for Guest/VM |
| [65833075](https://github.com/RVCK-Project/rvck/commit/65833075bbc127cf362b4c42499c59b17a5b2bb2) | 2023-11-12 | Xiao Wang | riscv: Optimize hweight API with Zbb extension |
| [fe48ad91](https://github.com/RVCK-Project/rvck/commit/fe48ad9146d0db70e108a21119f358401c6eeb84) | 2023-10-31 | Xiao Wang | riscv: Optimize bitops with Zbb extension |
| [0cebdf67](https://github.com/RVCK-Project/rvck/commit/0cebdf67582c79add475d67d97e32ae99a591495) | 2024-06-21 | Xiao Wang | riscv: Optimize crc32 with Zbc extension |
| [fef5dce1](https://github.com/RVCK-Project/rvck/commit/fef5dce1aefc8782a68a29e9ab5125b34c4e2cac) | 2025-02-28 | Robin Murphy | iommu: Handle race with default domain setup |
| [8fd50003](https://github.com/RVCK-Project/rvck/commit/8fd50003dd103fd9088bc953f6262b7efce1ef77) | 2023-10-03 | Jason Gunthorpe | iommu: Do not use IOMMU_DOMAIN_DMA if CONFIG_IOMMU_DMA is not enabled |
| [9918e0c7](https://github.com/RVCK-Project/rvck/commit/9918e0c781973cb029f895cca06906caba0c3e9f) | 2023-09-13 | Jason Gunthorpe | iommu: Convert remaining simple drivers to domain_alloc_paging() |
| [3aa7ff87](https://github.com/RVCK-Project/rvck/commit/3aa7ff87c20d0b7fff3551558d786d87854aa0ac) | 2023-09-13 | Jason Gunthorpe | iommu: Convert simple drivers with DOMAIN_DMA to domain_alloc_paging() |
| [117922f6](https://github.com/RVCK-Project/rvck/commit/117922f69c8a045087ce6463a6d47af7e6a19d34) | 2023-09-13 | Jason Gunthorpe | iommu: Add ops-\>domain_alloc_paging() |
| [4ae956cc](https://github.com/RVCK-Project/rvck/commit/4ae956cc95fef5c1e273c8829ff33752e5746c1d) | 2023-09-13 | Jason Gunthorpe | iommu: Add __iommu_group_domain_alloc() |
| [dab2f682](https://github.com/RVCK-Project/rvck/commit/dab2f682c878b757111c11be4816f2f310a969d0) | 2023-09-13 | Jason Gunthorpe | iommu: Require a default_domain for all iommu drivers |
| [93c1139e](https://github.com/RVCK-Project/rvck/commit/93c1139edd36358ca1e80020d9e91b5f7c7cc0d4) | 2023-09-13 | Jason Gunthorpe | iommu/sun50i: Add an IOMMU_IDENTITIY_DOMAIN |
| [a72a2274](https://github.com/RVCK-Project/rvck/commit/a72a227453331e7da0e871f113754553bd437400) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [bbd0dfdc](https://github.com/RVCK-Project/rvck/commit/bbd0dfdcf911b469498ce97b8e62ffdb8bd4118f) | 2023-09-13 | Jason Gunthorpe | iommu/ipmmu: Add an IOMMU_IDENTITIY_DOMAIN |
| [33bd59fd](https://github.com/RVCK-Project/rvck/commit/33bd59fd42b5e2251d9b7b1eb86bf39db3ee66dc) | 2023-09-13 | Jason Gunthorpe | iommu/qcom_iommu: Add an IOMMU_IDENTITIY_DOMAIN |
| [2b974eef](https://github.com/RVCK-Project/rvck/commit/2b974eefedc417ba6a72faaedc2b5a1de67dbf89) | 2023-09-13 | Jason Gunthorpe | iommu: Remove ops-\>set_platform_dma_ops() |
| [a4b6c5ba](https://github.com/RVCK-Project/rvck/commit/a4b6c5ba2bb225f5863dc9cc65907e30a4cbf8ad) | 2023-09-13 | Jason Gunthorpe | iommu/msm: Implement an IDENTITY domain |
| [081fc6b2](https://github.com/RVCK-Project/rvck/commit/081fc6b2ddb2f1117554f930fd227cf2240e31d8) | 2023-09-13 | Jason Gunthorpe | iommu/omap: Implement an IDENTITY domain |
| [85dc6ce2](https://github.com/RVCK-Project/rvck/commit/85dc6ce2ddb259152ad61645cb394ab55ec072e9) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Support DMA domains in tegra |
| [93d73499](https://github.com/RVCK-Project/rvck/commit/93d7349999357c5cac40af2c2e1dcbf384deba07) | 2023-09-13 | Jason Gunthorpe | iommu/tegra-smmu: Implement an IDENTITY domain |
| [86d0ed13](https://github.com/RVCK-Project/rvck/commit/86d0ed138c9a49b8da78ea3848efa9eda03f4870) | 2023-09-13 | Jason Gunthorpe | iommu/exynos: Implement an IDENTITY domain |
| [557cef25](https://github.com/RVCK-Project/rvck/commit/557cef25e2f3ed631d6b653b2e6d10b9c72b5b3b) | 2023-09-13 | Jason Gunthorpe | iommu: Allow an IDENTITY domain as the default_domain in ARM32 |
| [aa922d39](https://github.com/RVCK-Project/rvck/commit/aa922d395b8b25b7b579a3015fe2792eff7264de) | 2023-09-13 | Jason Gunthorpe | iommu: Reorganize iommu_get_default_domain_type() to respect def_domain_type() |
| [de42b7be](https://github.com/RVCK-Project/rvck/commit/de42b7be063b99037074f483015a90c5d0e82cd9) | 2023-09-13 | Jason Gunthorpe | iommu/mtk_iommu_v1: Implement an IDENTITY domain |
| [7f31b568](https://github.com/RVCK-Project/rvck/commit/7f31b568f915e2c3d7a0d6cc093a5e7c006c243f) | 2023-09-13 | Jason Gunthorpe | iommu/fsl_pamu: Implement a PLATFORM domain |
| [2a080b66](https://github.com/RVCK-Project/rvck/commit/2a080b6679e8cb149ad2cc4297e3518a4085fb80) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM for S390 |
| [3e18eaa1](https://github.com/RVCK-Project/rvck/commit/3e18eaa14394d219a5dd84a696cc255fa30dcefb) | 2023-09-13 | Jason Gunthorpe | iommu: Add IOMMU_DOMAIN_PLATFORM |
| [174c285c](https://github.com/RVCK-Project/rvck/commit/174c285c3070069fcdd5100267fb1019a263e955) | 2023-09-13 | Jason Gunthorpe | iommu: Add iommu_ops-\>identity_domain |
| [6119d5fc](https://github.com/RVCK-Project/rvck/commit/6119d5fcaac51046d0a5aa3191d8b3eaa802fb84) | 2025-07-29 | gaorui | Revert "iommu: Handle race with default domain setup" |
| [3bb4186d](https://github.com/RVCK-Project/rvck/commit/3bb4186d8fb4a43fd337bc2f318c78ec4d391a08) | 2024-04-09 | Baoquan He | kexec: fix the unexpected kexec_dprintk() macro |
| [0e910c86](https://github.com/RVCK-Project/rvck/commit/0e910c8610e6c9aa08de1b69246bccf3920152c8) | 2024-07-30 | Sunil V L | kexec_file, parisc: print out debugging message if required |
| [9536dfb6](https://github.com/RVCK-Project/rvck/commit/9536dfb6d9917d8c1b856816f1be461142eea3c6) | 2023-12-13 | Baoquan He | kexec_file, power: print out debugging message if required |
| [7cd30931](https://github.com/RVCK-Project/rvck/commit/7cd30931ac7c048496f986afab208b71c5fbcaa6) | 2023-12-13 | Baoquan He | kexec_file, riscv: print out debugging message if required |
| [36af9aed](https://github.com/RVCK-Project/rvck/commit/36af9aed25941d80144920b8d0a9f9a6aa290940) | 2023-12-13 | Baoquan He | kexec_file, arm64: print out debugging message if required |
| [7153e847](https://github.com/RVCK-Project/rvck/commit/7153e8470cec85cfbf3090853c90f61d7002cb83) | 2023-12-13 | Baoquan He | kexec_file, x86: print out debugging message if required |
| [2a653ab4](https://github.com/RVCK-Project/rvck/commit/2a653ab48cd5e965cfa07e3ac1d1542f15a61038) | 2023-12-13 | Baoquan He | kexec_file: print out debugging message if required |
| [b0b4d536](https://github.com/RVCK-Project/rvck/commit/b0b4d5361e417d5889afc42ea2eb935175c26458) | 2023-12-13 | Baoquan He | kexec_file: add kexec_file flag to control debug printing |
| [bc38e549](https://github.com/RVCK-Project/rvck/commit/bc38e54959de797d9b1033aab5cf4eddb780a809) | 2025-04-03 | Radim Krčmář | KVM: RISC-V: reset smstateen CSRs |
| [86e2ce15](https://github.com/RVCK-Project/rvck/commit/86e2ce15ca40f41037fed54080b1528e6fef0518) | 2023-12-24 | Anup Patel | RISC-V: KVM: Fix indentation in kvm_riscv_vcpu_set_reg_csr() |
| [592d9de9](https://github.com/RVCK-Project/rvck/commit/592d9de95f6470ded03e92494d340461ce632265) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 to ONE_REG |
| [7f7504f9](https://github.com/RVCK-Project/rvck/commit/7f7504f9fefc97b024f7c99fbfc828b83b1f3487) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add sstateen0 context save/restore |
| [ca54b6b8](https://github.com/RVCK-Project/rvck/commit/ca54b6b805e0b39a149cb5ac0643bab8d3607f65) | 2023-09-13 | Mayuresh Chitale | RISCV: KVM: Add senvcfg context save/restore |
| [e79de7a9](https://github.com/RVCK-Project/rvck/commit/e79de7a9009b43ceb7b4b6d53879ba941c8b054f) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Enable Smstateen accesses |
| [6b6ca3fc](https://github.com/RVCK-Project/rvck/commit/6b6ca3fc37d5e2ce1ad98892cb9af7716f57fa04) | 2023-09-13 | Mayuresh Chitale | RISC-V: KVM: Add kvm_vcpu_config |
| [6d8933f6](https://github.com/RVCK-Project/rvck/commit/6d8933f651036c5b4fb18c77d01f8666ce726572) | 2024-07-30 | Sunil V L | serial: 8250_platform: Enable generic 16550A platform devices |
| [5a304ad2](https://github.com/RVCK-Project/rvck/commit/5a304ad20fa5471639afcfee6f3c9594b946af21) | 2025-04-09 | Song Shuai | riscv: kexec_file: Support loading Image binary file |
| [2eec7a59](https://github.com/RVCK-Project/rvck/commit/2eec7a596ae77f96fab66f5d085e67221aaee1cf) | 2025-07-25 | gaorui | riscv: kexec_file: Split the loading of kernel and others |
| [c75c90f5](https://github.com/RVCK-Project/rvck/commit/c75c90f50b49d879b7518348e8d6c3010166940b) | 2025-07-25 | gaorui | Revert "riscv: kexec: Add image loader for kexec file" |
| [2b867302](https://github.com/RVCK-Project/rvck/commit/2b867302a94c60b279e50c9ff65bcf0d6a3cac44) | 2023-11-30 | Samuel Ortiz | RISC-V: Implement archrandom when Zkr is available |
| [dda68c38](https://github.com/RVCK-Project/rvck/commit/dda68c38f67433d6f179a7be0c2374dcd06228a8) | 2024-02-08 | Sunil V L | cpufreq: Move CPPC configs to common Kconfig and add RISC-V |
| [44b0e706](https://github.com/RVCK-Project/rvck/commit/44b0e70633db190d4a4fa9aa0bfcd9bf9bd6f689) | 2024-02-08 | Sunil V L | ACPI: RISC-V: Add CPPC driver |
| [6693b71c](https://github.com/RVCK-Project/rvck/commit/6693b71c48085dfa71253954a9cb7f26077a04bf) | 2024-06-17 | Yunhui Cui | RISC-V: Select ACPI PPTT drivers |
| [81d1d1fb](https://github.com/RVCK-Project/rvck/commit/81d1d1fbf1a438408ebdefb80eb55f54bbdd4a4a) | 2024-05-02 | Sia Jee Heng | RISC-V: ACPI: Enable SPCR table for console output on RISC-V |
| [31454310](https://github.com/RVCK-Project/rvck/commit/31454310333e1b8b7256313e2ac5c7c9dec990c0) | 2024-07-18 | Ryo Takakura | RISC-V: Enable IPI CPU Backtrace |
| [879bbe34](https://github.com/RVCK-Project/rvck/commit/879bbe34ab077914a3cd947a8eda986cf935bf28) | 2024-06-13 | Haibo Xu | riscv: dmi: Add SMBIOS/DMI support |
| [4b6bcbab](https://github.com/RVCK-Project/rvck/commit/4b6bcbab3b542e98780847d5404b8f1b596de09c) | 2024-06-13 | Haibo Xu | ACPI: NUMA: replace pr_info with pr_debug in arch_acpi_numa_init |
| [c364d950](https://github.com/RVCK-Project/rvck/commit/c364d950f567770594d739cc0b57d1a0392a8107) | 2025-04-25 | gaorui | ACPI: NUMA: change the ACPI_NUMA to a hidden option |
| [7d5b12e4](https://github.com/RVCK-Project/rvck/commit/7d5b12e43ebab1c8527c71f931283df02e224cc3) | 2025-04-25 | gaorui | ACPI: NUMA: Make some NUMA-related functions available for RISC-V |
| [754c0289](https://github.com/RVCK-Project/rvck/commit/754c02892e1ed6fb38ab640dc5d0b1440a2ac585) | 2024-06-13 | Haibo Xu | ACPI: NUMA: Add handler for SRAT RINTC affinity structure |
| [6f2873ab](https://github.com/RVCK-Project/rvck/commit/6f2873ab06f2f9c068b2da959e0ad25a3485ee8a) | 2024-06-13 | Haibo Xu | ACPI: RISCV: Add NUMA support based on SRAT and SLIT |
| [b71d0d6d](https://github.com/RVCK-Project/rvck/commit/b71d0d6dcdb69249122ee286819ebe99ec8857fe) | 2024-01-17 | Haibo Xu | ACPICA: SRAT: Add RISC-V RINTC affinity structure |
---

**共 407 条提交（显示全部）**

[分页显示](中兴通讯.md) | [纯文本视图](中兴通讯_commits.txt)
## 🔙 返回

[← 返回统计主页](../index.md)

---

*本页面最后更新于 2026-07-10 00:58:34*
*数据来源: 主分支 rvck-6.6@6e2f72b4*
