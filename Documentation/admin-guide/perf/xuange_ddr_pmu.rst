.. SPDX-License-Identifier: GPL-2.0

Xuange DDR PMU
==============

The Xuange DDR controller PMU measures host-interface requests, controller
commands and queue/credit minimum and maximum values. Each controller is
exposed as xuange_ddr_<address>, where address is the MMIO base shifted right
by 12 bits. The cpumask sysfs attribute identifies the management CPU.
Only system-wide counting is supported; sampling and per-task counting are
not supported.

For example::

  perf stat -a -e xuange_ddr_593400/hif_rd/ -- sleep 1
  perf stat -a --no-scale -e '{xuange_ddr_593400/hif_hpr_crd_cnt_min/,xuange_ddr_593400/hif_hpr_crd_cnt_max/}' -- sleep 1

Ordinary events with the same event ID divided by four share one counter
and cannot run together. Debug min/max partners may share their slot only
within the same perf group. They share a hardware enable bit: the first
member stopped ends the measurement and caches both results. Independent
stop/restart of one member while the other remains active is unsupported.

Debug results are snapshots, published when the measurement stops. The
hardware does not refresh them during a software-stop measurement, so
perf stat -I cannot provide live debug values. The events/*.snapshot sysfs
attributes describe this non-cumulative data; they do not trigger a refresh.

Use --no-scale when measuring debug min/max events. The snapshot attribute
prevents delta calculation but does not disable perf time-based scaling.
When multiplexed, scaling by time_enabled/time_running would incorrectly
change the raw minimum or maximum. Avoid multiplexing where possible:
--no-scale preserves the measured value, but cannot recover extrema from
periods when the event was not running.

Ordinary internal counters restart from zero after STAT_EN is cleared.
Their result registers retain the completed result until the hardware
updates them for the next measurement.

Debug snapshots describe only the most recently completed hardware window;
they are not combined across scheduling windows. Multiplexing or management
CPU migration can split a measurement, discarding extrema from earlier
windows. --no-scale does not prevent this loss. Avoid resource contention
and management CPU offlining when a continuous debug window is required.
