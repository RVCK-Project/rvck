.. SPDX-License-Identifier: GPL-2.0

Xuange CPM PMU
==============

The v1 CHI performance monitor exposes transaction counters, latency
statistics and OSTD/LCRD flow-control statistics through perf stat.
Latency event units are CPM input bus-clock cycles, not CPU cycles.

Firmware interface
------------------

Device Tree uses ``xuange,cpm-pmu-v1``; see the corresponding binding in
``Documentation/devicetree/bindings/perf/xuange,cpm-pmu.yaml``.

ACPI uses HID ``BABA5004``. The first memory resource in _CRS describes
the CPM register block. CPU subsystem instances provide a second memory
resource for the selector register; DDR instances omit it and only accept
``cpm_sel=0``. One interrupt resource is required. _PXM should describe
the device's NUMA proximity domain when available.

The standard device-properties _DSD UUID
``daffd814-6eba-4d8c-8a91-bc9bbf4aa301`` carries these optional properties:

* ``xuange,cpm-name``: unique instance suffix, for example
  ``die0_complex8``. The PMU is named ``cpm_<suffix>``. Without the
  property, an address-derived name is used.
* ``xuange,fpga``: presence selects the 7.6 MHz HAPS clock and period
  exponent 16. For ACPI use ``Package () { "xuange,fpga", One }``.
  On silicon omit the property entirely; Zero does not express false
  for this presence-based boolean. Absence selects 1.9 GHz and exponent 24.

The driver derives the stop timeout from the selected clock and period.
Firmware must accurately describe the hardware; these properties do not
change its clock. Management CPU migration prefers the device NUMA node
and falls back to another online CPU if necessary.

Event configuration
-------------------

config1 bits 0-47 specify addr_lo, 48-54 opcode_mask, 55-61 opcode_tgt,
and bit 62 cpm_sel. Bit 63 is reserved and must be zero. config2 bits
0-47 specify addr_hi; all higher bits must be zero.

The six read/write average, minimum and maximum latency events expose
``.unit=cycles``. Real OSTD/LCRD events expose ``.snapshot=1`` so perf
can report instantaneous values without interval subtraction.
