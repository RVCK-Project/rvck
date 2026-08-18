.. SPDX-License-Identifier: GPL-2.0

=====================================
ZhiHe A210 AON RPC protocol
=====================================

The Always-On (AON) subsystem of the ZhiHe A210 SoC runs a dedicated
firmware on a RISC-V E902 core. It provides power, clock and resource
management services for the whole SoC. The application processor
loads the AON firmware at boot and talks to it through a Remote
Procedure Call (RPC) protocol carried on a mailbox channel.

This document describes the AON bring-up sequence and the RPC wire
format as implemented by the ``zhihe_a210_aon`` driver
(``drivers/firmware/zhihe/a210_aon.c``) and its consumers, currently
the AON regulator driver
(``drivers/regulator/zhihe-a210-aon-regulator.c``).

System overview
===============

::

    +---------------------------------------------+   +--------------+
    | Application processor (Linux)               |   | AON subsystem|
    |                                             |   |              |
    |  regulator/pmdomain drivers                 |   |  E902 core   |
    |        |  zhihe_a210_aon_call_rpc()         |   |  running     |
    |        v                                    |   |  AON         |
    |  zhihe_a210_aon driver  ---- mailbox ------>|--->  firmware    |
    |  (drivers/firmware/zhihe)   (INFO0~INFO7)   |   |              |
    +---------------------------------------------+   +--------------+

Each kernel consumer obtains an IPC handle bound to a named mailbox
channel (e.g. ``aon0``) and serialises its transactions through it.
Only one transaction may be in flight per channel; the host driver
holds a mutex for the duration of a request/response pair.

AON bring-up
============

The AON node exposes three register regions:

=============  =======================================================
reg-name       Purpose
=============  =======================================================
``aon-base``   AON SRAM, the firmware image is copied here
``aon-reset``  Reset control register of the E902 core
``aon-sync``   Handshake register written by the firmware once it is
               up and running
=============  =======================================================

The bring-up sequence is:

1. Load the firmware image named by the ``firmware-name`` property
   (e.g. ``a210-aon.bin``) via the kernel firmware loader.
2. Clear ``aon-base`` and copy the image into it.
3. Release the E902 core by writing ``0x3`` to ``aon-reset``.
4. Poll ``aon-sync`` until it reads ``0xfeabdeff``. The poll times out
   after 50 ms; the whole sequence is retried once before giving up.

Once the firmware is alive, the host driver registers the IPC channel
and populates the child nodes of the AON node (regulators, etc.).

Mailbox transport
=================

The RPC layer sits on one mailbox channel of the A210 mailbox
controller. Mailbox specifier cells are:

=====  ================================================================
Cell   Meaning
=====  ================================================================
0      Remote channel index (0..3)
1      Channel type: 0 = data channel with Tx/Rx doorbells,
       1 = doorbell-only channel
=====  ================================================================

The AON channel is a data channel, e.g. ``mboxes = <&mbox_920 1 0>``
with ``mbox-names = "aon0"``.

One mailbox data transfer moves eight 32-bit registers:

=========  ===========================================================
Register   Role
=========  ===========================================================
INFO0      Protocol word of the request; must be non-zero for the
           mailbox layer to treat the transfer as valid data
INFO0~6    The 28-byte RPC message payload
INFO7      Transport-level acknowledge: the receiver writes the magic
           ``0xdeadbeaf`` here to signal completion of the previous
           transfer
=========  ===========================================================

The mailbox layer raises the remote doorbell after writing the data
registers and reports Tx completion when INFO7 carries the magic
value. All of this is transparent to the RPC layer, which only sees
28-byte request and response buffers.

RPC message format
==================

An RPC message is exactly 28 bytes (seven 32-bit words). Every
message starts with a 4-byte header::

    Offset  Size  Field  Description
    ------  ----  -----  ------------------------------------------
    0       1     ver    Protocol version, currently 2
    1       1     size   Message size in 32-bit words, must be 7
    2       1     svc    Service byte, see below
    3       1     func   Function ID within the service
    4..27   24    payload, function specific

The ``svc`` byte is laid out as follows:

* Bit 7: message type, 0 = data (request), 1 = ack (response).
* Bit 6: ack type, 0 = response expected, 1 = no response expected.
* Bits 5 - 0: service ID.

The host driver stamps ``ver``, the message type and the ack type;
the caller provides ``svc``, ``func`` and the payload. Requests must
be built in a zero-initialised buffer because the header fields are
set with read-modify-write macros. All multi-byte payload fields are
big-endian.

Services
========

===========  =========================================================
Service ID   Name
===========  =========================================================
0            UNKNOWN
1            PM (power management)
2            MISC
3            AVFS
4            SYS
5            WDG (watchdog)
6            LPM (low-power mode)
0x3F         MAX
===========  =========================================================

Only the PM service is used by the kernel today. Its function IDs are:

===========  =========================================================
Function ID  Name
===========  =========================================================
0            UNKNOWN
1            SET_RESOURCE_REGULATOR
2            GET_RESOURCE_REGULATOR
3            SET_RESOURCE_POWER_MODE
4            PWR_SET
5            PWR_GET
6            CHECK_FAULT
7            GET_TEMPERATURE
===========  =========================================================

Response and error codes
========================

A response starts with the common ack descriptor: a 4-byte message
header followed by a 1-byte error code at offset 4. The error code is
translated to a Linux errno by the host driver:

===========  ================  ============
Error code   Meaning           Linux errno
===========  ================  ============
0            NONE (success)    0
1            VERSION           -EINVAL
2            CONFIG            -EINVAL
3            PARM              -EINVAL
4            NOACCESS          -EACCES
5            LOCKED            -EACCES
6            UNAVAILABLE       -ERANGE
7            NOTFOUND          -EEXIST
8            NOPOWER           -EPERM
9            IPC               -EPIPE
10           BUSY              -EBUSY
11           FAIL              -EIO
===========  ================  ============

Regulator resource IDs
======================

The PM regulator functions address *virtual regulators* managed by
the AON firmware through a 16-bit resource ID. On the A210 the IDs
map to physical PMIC rails as follows:

===  =====================  =======================
ID   Virtual regulator      Physical rail
===  =====================  =======================
0    AVDD33_EMMC            sy70209 LDO1
1    AVDD33_USB2            sy70209 LDO2
2    DVDD08_AON             sy70209 LDO3
3    AVDD18_AON             sy70209 LDO4
4    AVDD18_EMMC_USB2       sy70209 LDO5
5    AVDD18_EMMC_PERI       sy70209 LDO7
6    AVDD18_TOP             sy70209 LDO8
7    AVDD18_PLL             sy70209 LDO9
8    AVDD18                 sy70209 LDO10
9    DVDD18_DDR_VAA         sy70209 VOUT1
10   P3V3                   sy70209 VOUT2
11   DVDD08_TOP             sy70209 VOUT3
12   DVDD06_DDR_VDDQLP      sy70209 VOUT4
13   DVDD08_DDR             sy70209 VOUT5
14   DVDD_CPU               sy70209 VOUT6
15   DVDDM_CPU              sy70209 VOUT7
16   DVDD_VP                sy70209 VOUT8
17   DVDD_NPU_VIP           sy70300 VOUT1
18   DVDD_CPU_P             sy70300 VOUT2
19   DVDD_GPU               sy70300 VOUT3
===  =====================  =======================

The same IDs are referenced from the device tree by the
``zhihe,regulator-id`` property of the AON regulator child nodes.

PM service messages
===================

Offsets are given from the start of the 28-byte message; multi-byte
fields are big-endian.

SET_RESOURCE_REGULATOR (svc 1, func 1)
--------------------------------------

Set the voltage of a virtual regulator. A *dual-rail* virtual
regulator groups two physical rails that must be moved together
(e.g. the CPU core rails); for single-rail regulators ``dc2`` is
ignored.

======  ======  ================================================
Offset  Size    Field
======  ======  ================================================
4       2       regu_id: virtual regulator ID
6       2       is_dual_rail: non-zero if two rails are involved
8       4       dc1: voltage in uV (single rail or first rail)
12      4       dc2: voltage in uV of the second rail
16      12      reserved
======  ======  ================================================

GET_RESOURCE_REGULATOR (svc 1, func 2)
--------------------------------------

Get the voltage of a virtual regulator. The request carries only
``regu_id`` (big-endian u16 at offset 4). The response is:

======  ======  ================================================
Offset  Size    Field
======  ======  ================================================
0       4       message header
4       1       err_code
5       2       regu_id
7       2       is_dual_rail
9       4       dc1: voltage in uV
13      4       dc2: voltage in uV
17      11      reserved
======  ======  ================================================

PWR_SET (svc 1, func 4)
-----------------------

Enable or disable a virtual regulator.

======  ======  ================================================
Offset  Size    Field
======  ======  ================================================
4       2       regu_id: virtual regulator ID
6       2       status: 1 = on, 0 = off
8       20      reserved
======  ======  ================================================

PWR_GET (svc 1, func 5)
-----------------------

Query the enable state of a virtual regulator. The request carries
only ``regu_id`` (big-endian u16 at offset 4). The response is:

======  ======  ================================================
Offset  Size    Field
======  ======  ================================================
0       4       message header
4       1       err_code
5       2       regu_id
7       2       status: 1 = on, 0 = off
9       19      reserved
======  ======  ================================================

Transaction flow
================

1. The caller builds the request in a zero-initialised 28-byte buffer
   and sets ``svc``, ``func`` and the payload.
2. The host driver stamps ``ver``, forces the message type to data
   and sets the ack type according to whether a response is expected,
   then submits the buffer to the mailbox. Submission fails with
   ``-ETIMEDOUT`` if the previous transfer on the channel has not
   completed within 500 ms.
3. If a response is expected, the driver waits up to 3000 ms for the
   AON firmware to fill the caller's response buffer (at most 28
   bytes are copied), then returns the translated error code. A
   missing response yields ``-ETIMEDOUT``.
4. The channel mutex is held for the whole request/response pair, so
   transactions on one channel are strictly serialised.

Device tree binding
===================

The host side is described by the ``zhihe,a210-aon`` node, see
``Documentation/devicetree/bindings/firmware/zhihe,a210-aon.yaml``:

- ``reg``/``reg-names``: the three register regions listed in
  `AON bring-up`_.
- ``firmware-name``: name of the AON firmware image.
- ``mboxes``/``mbox-names``: the mailbox channel used for RPC; the
  name becomes the IPC handle name that kernel consumers request.
- ``regulators``: child node compatible with
  ``zhihe,a210-aon-regulator``, whose children map virtual regulator
  IDs to consumer-visible regulator names.
- ``log-memory-region`` (optional): reserved memory holding the AON
  firmware log, exposed under ``/proc/aon_proc`` when
  ``CONFIG_ZHIHE_PROC_DEBUG`` is enabled.
