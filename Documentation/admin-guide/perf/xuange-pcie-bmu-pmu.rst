.. SPDX-License-Identifier: GPL-2.0-only

Xuange PCIe BMU PMU
===================

The Xuange PCIe Bus Monitor Unit (BMU) driver exposes AXI and A4S
monitoring events through the perf subsystem. It supports system-wide
counting, not per-task events or sampling.

Firmware discovery
------------------

The driver scans ACPI PCIe host bridges (``PNP0A08``) and reads the
``bmu-entries`` package from their ``_DSD``. Each entry describes a type,
instance count, start address and total register length. AXI entries also
require the first instance's GSI; subsequent instances use consecutive
GSIs. The register stride is the total length divided by the count.
Firmware types 0, 1 and 4 describe AXI DMA, MMIO and LTIDU monitors;
type 3 describes A4S monitors. Type 2 is not supported.

Instances are named ``pcie_bmu_rc<RC>_<type>_<instance>``. Availability
depends on the firmware description and hardware capabilities.

Events and usage
----------------

Use ``perf list`` and the PMU's ``events`` and ``format`` sysfs directories
to inspect available events and attributes. For example, if the named
instance exists::

  perf stat -a -e pcie_bmu_rc0_axi_dma_0/read_trans/ -- sleep 5
  perf stat -a -e pcie_bmu_rc0_a4s_0/rxa4s_total/ -- sleep 5

AXI monitors expose read/write transaction, byte, duration, threshold,
variable and address-handshake latency events. Variable events available
on an instance depend on its hardware capabilities. AXI results are
collected through interrupts at a fixed period of 2^18 APB cycles. The
hardware window must be serviced before the next window overwrites it.
Duration threshold tracking samples one in four read/write commands.
The ``duration_threshold`` sysfs attribute defaults to 256 cycles and
may be changed only when no events are active.

AXI variable events expose ``size_flt``, ``len_flt``, ``addr_aln`` and
``filter_en`` in the PMU's ``format`` directory. Address-range bounds do
not yet have a userspace interface, so enabling that filter is rejected.
Consult the hardware definition for supported filter encodings.

A4S exposes four free-running message counters: ``rxa4s_total``,
``txa4s_total``, ``rxa4s_type12`` and ``txa4s_type12``. The driver
periodically collects active counters in software. A4S filtered counters
and channel latency minimum/maximum events are not exposed.
