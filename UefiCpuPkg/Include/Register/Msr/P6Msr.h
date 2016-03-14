/** @file
  MSR Definitions for P6 Family Processors.

  Provides defines for Machine Specific Registers(MSR) indexes. Data structures
  are provided for MSRs that contain one or more bit fields.  If the MSR value
  returned is a single 32-bit or 64-bit value, then a data structure is not
  provided for that MSR.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Specification Reference:
  Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Volume 3,
  December 2015, Chapter 35 Model-Specific-Registers (MSR), Section 35-19.

**/

#ifndef __P6_MSR_H__
#define __P6_MSR_H__

#include <Register/ArchitecturalMsr.h>

/**
  See Section 35.20, "MSRs in Pentium Processors.".

  @param  ECX  MSR_P6_P5_MC_ADDR (0x00000000)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_P5_MC_ADDR);
  AsmWriteMsr64 (MSR_P6_P5_MC_ADDR, Msr);
  @endcode
**/
#define MSR_P6_P5_MC_ADDR                        0x00000000


/**
  See Section 35.20, "MSRs in Pentium Processors.".

  @param  ECX  MSR_P6_P5_MC_TYPE (0x00000001)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_P5_MC_TYPE);
  AsmWriteMsr64 (MSR_P6_P5_MC_TYPE, Msr);
  @endcode
**/
#define MSR_P6_P5_MC_TYPE                        0x00000001


/**
  See Section 17.14, "Time-Stamp Counter.".

  @param  ECX  MSR_P6_TSC (0x00000010)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_TSC);
  AsmWriteMsr64 (MSR_P6_TSC, Msr);
  @endcode
**/
#define MSR_P6_TSC                               0x00000010


/**
  Platform ID (R)  The operating system can use this MSR to determine "slot"
  information for the processor and the proper microcode update to load.

  @param  ECX  MSR_P6_IA32_PLATFORM_ID (0x00000017)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_IA32_PLATFORM_ID_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_IA32_PLATFORM_ID_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_IA32_PLATFORM_ID_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_IA32_PLATFORM_ID);
  @endcode
**/
#define MSR_P6_IA32_PLATFORM_ID                  0x00000017

/**
  MSR information returned for MSR index #MSR_P6_IA32_PLATFORM_ID
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:32;
    UINT32  Reserved2:18;
    ///
    /// [Bits 52:50] Platform Id (R) Contains information concerning the
    /// intended platform for the processor.
    ///
    ///  52 51 50
    ///   0  0  0  Processor Flag 0.
    ///   0  0  1  Processor Flag 1
    ///   0  1  0  Processor Flag 2
    ///   0  1  1  Processor Flag 3
    ///   1  0  0  Processor Flag 4
    ///   1  0  1  Processor Flag 5
    ///   1  1  0  Processor Flag 6
    ///   1  1  1  Processor Flag 7
    ///
    UINT32  PlatformId:3;
    ///
    /// [Bits 56:53] L2 Cache Latency Read.
    ///
    UINT32  L2CacheLatencyRead:4;
    UINT32  Reserved3:3;
    ///
    /// [Bit 60] Clock Frequency Ratio Read.
    ///
    UINT32  ClockFrequencyRatioRead:1;
    UINT32  Reserved4:3;
  } Bits;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_IA32_PLATFORM_ID_REGISTER;


/**
  Section 10.4.4, "Local APIC Status and Location.".

  @param  ECX  MSR_P6_APIC_BASE (0x0000001B)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_APIC_BASE_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_APIC_BASE_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_APIC_BASE_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_APIC_BASE);
  AsmWriteMsr64 (MSR_P6_APIC_BASE, Msr.Uint64);
  @endcode
**/
#define MSR_P6_APIC_BASE                         0x0000001B

/**
  MSR information returned for MSR index #MSR_P6_APIC_BASE
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:8;
    ///
    /// [Bit 8] Boot Strap Processor indicator Bit 1 = BSP.
    ///
    UINT32  BSP:1;
    UINT32  Reserved2:2;
    ///
    /// [Bit 11] APIC Global Enable Bit - Permanent till reset 1 = Enabled 0 =
    /// Disabled.
    ///
    UINT32  EN:1;
    ///
    /// [Bits 31:12] APIC Base Address.
    ///
    UINT32  ApicBase:20;
    UINT32  Reserved3:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_APIC_BASE_REGISTER;


/**
  Processor Hard Power-On Configuration (R/W) Enables and disables processor
  features; (R) indicates current processor configuration.

  @param  ECX  MSR_P6_EBL_CR_POWERON (0x0000002A)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_EBL_CR_POWERON_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_EBL_CR_POWERON_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_EBL_CR_POWERON_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_EBL_CR_POWERON);
  AsmWriteMsr64 (MSR_P6_EBL_CR_POWERON, Msr.Uint64);
  @endcode
**/
#define MSR_P6_EBL_CR_POWERON                    0x0000002A

/**
  MSR information returned for MSR index #MSR_P6_EBL_CR_POWERON
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:1;
    ///
    /// [Bit 1] Data Error Checking Enable (R/W) 1 = Enabled 0 = Disabled.
    ///
    UINT32  DataErrorCheckingEnable:1;
    ///
    /// [Bit 2] Response Error Checking Enable FRCERR Observation Enable (R/W)
    /// 1 = Enabled 0 = Disabled.
    ///
    UINT32  ResponseErrorCheckingEnable:1;
    ///
    /// [Bit 3] AERR# Drive Enable (R/W) 1 = Enabled 0 = Disabled.
    ///
    UINT32  AERR_DriveEnable:1;
    ///
    /// [Bit 4] BERR# Enable for Initiator Bus Requests (R/W) 1 = Enabled 0 =
    /// Disabled.
    ///
    UINT32  BERR_Enable:1;
    UINT32  Reserved2:1;
    ///
    /// [Bit 6] BERR# Driver Enable for Initiator Internal Errors (R/W) 1 =
    /// Enabled 0 = Disabled.
    ///
    UINT32  BERR_DriverEnable:1;
    ///
    /// [Bit 7] BINIT# Driver Enable (R/W) 1 = Enabled 0 = Disabled.
    ///
    UINT32  BINIT_DriverEnable:1;
    ///
    /// [Bit 8] Output Tri-state Enabled (R) 1 = Enabled 0 = Disabled.
    ///
    UINT32  OutputTriStateEnable:1;
    ///
    /// [Bit 9] Execute BIST (R) 1 = Enabled 0 = Disabled.
    ///
    UINT32  ExecuteBIST:1;
    ///
    /// [Bit 10] AERR# Observation Enabled (R) 1 = Enabled 0 = Disabled.
    ///
    UINT32  AERR_ObservationEnabled:1;
    UINT32  Reserved3:1;
    ///
    /// [Bit 12] BINIT# Observation Enabled (R) 1 = Enabled 0 = Disabled.
    ///
    UINT32  BINIT_ObservationEnabled:1;
    ///
    /// [Bit 13] In Order Queue Depth (R) 1 = 1 0 = 8.
    ///
    UINT32  InOrderQueueDepth:1;
    ///
    /// [Bit 14] 1-MByte Power on Reset Vector (R) 1 = 1MByte 0 = 4GBytes.
    ///
    UINT32  ResetVector:1;
    ///
    /// [Bit 15] FRC Mode Enable (R) 1 = Enabled 0 = Disabled.
    ///
    UINT32  FRCModeEnable:1;
    ///
    /// [Bits 17:16] APIC Cluster ID (R).
    ///
    UINT32  APICClusterID:2;
    ///
    /// [Bits 19:18] System Bus Frequency (R) 00 = 66MHz 10 = 100Mhz 01 =
    /// 133MHz 11 = Reserved.
    ///
    UINT32  SystemBusFrequency:2;
    ///
    /// [Bits 21:20] Symmetric Arbitration ID (R).
    ///
    UINT32  SymmetricArbitrationID:2;
    ///
    /// [Bits 25:22] Clock Frequency Ratio (R).
    ///
    UINT32  ClockFrequencyRatio:4;
    ///
    /// [Bit 26] Low Power Mode Enable (R/W).
    ///
    UINT32  LowPowerModeEnable:1;
    ///
    /// [Bit 27] Clock Frequency Ratio.
    ///
    UINT32  ClockFrequencyRatio1:1;
    UINT32  Reserved4:4;
    UINT32  Reserved5:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_EBL_CR_POWERON_REGISTER;


/**
  Test Control Register.

  @param  ECX  MSR_P6_TEST_CTL (0x00000033)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_TEST_CTL_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_TEST_CTL_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_TEST_CTL_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_TEST_CTL);
  AsmWriteMsr64 (MSR_P6_TEST_CTL, Msr.Uint64);
  @endcode
**/
#define MSR_P6_TEST_CTL                          0x00000033

/**
  MSR information returned for MSR index #MSR_P6_TEST_CTL
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:30;
    ///
    /// [Bit 30] Streaming Buffer Disable.
    ///
    UINT32  StreamingBufferDisable:1;
    ///
    /// [Bit 31] Disable LOCK# Assertion for split locked access.
    ///
    UINT32  Disable_LOCK:1;
    UINT32  Reserved2:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_TEST_CTL_REGISTER;


/**
  BIOS Update Trigger Register.

  @param  ECX  MSR_P6_BIOS_UPDT_TRIG (0x00000079)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BIOS_UPDT_TRIG);
  AsmWriteMsr64 (MSR_P6_BIOS_UPDT_TRIG, Msr);
  @endcode
**/
#define MSR_P6_BIOS_UPDT_TRIG                    0x00000079


/**
  Chunk n data register D[63:0]: used to write to and read from the L2.

  @param  ECX  MSR_P6_BBL_CR_Dn
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BBL_CR_D0);
  AsmWriteMsr64 (MSR_P6_BBL_CR_D0, Msr);
  @endcode
  @{
**/
#define MSR_P6_BBL_CR_D0                         0x00000088
#define MSR_P6_BBL_CR_D1                         0x00000089
#define MSR_P6_BBL_CR_D2                         0x0000008A
/// @}


/**
  BIOS Update Signature Register or Chunk 3 data register D[63:0] Used to
  write to and read from the L2 depending on the usage model.

  @param  ECX  MSR_P6_BIOS_SIGN (0x0000008B)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BIOS_SIGN);
  AsmWriteMsr64 (MSR_P6_BIOS_SIGN, Msr);
  @endcode
**/
#define MSR_P6_BIOS_SIGN                         0x0000008B


/**


  @param  ECX  MSR_P6_PERFCTR0 (0x000000C1)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_PERFCTR0);
  AsmWriteMsr64 (MSR_P6_PERFCTR0, Msr);
  @endcode
  @{
**/
#define MSR_P6_PERFCTR0                          0x000000C1
#define MSR_P6_PERFCTR1                          0x000000C2
/// @}


/**


  @param  ECX  MSR_P6_MTRRCAP (0x000000FE)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRCAP);
  AsmWriteMsr64 (MSR_P6_MTRRCAP, Msr);
  @endcode
**/
#define MSR_P6_MTRRCAP                           0x000000FE


/**
  Address register: used to send specified address (A31-A3) to L2 during cache
  initialization accesses.

  @param  ECX  MSR_P6_BBL_CR_ADDR (0x00000116)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_ADDR_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_ADDR_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_BBL_CR_ADDR_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_BBL_CR_ADDR);
  AsmWriteMsr64 (MSR_P6_BBL_CR_ADDR, Msr.Uint64);
  @endcode
**/
#define MSR_P6_BBL_CR_ADDR                       0x00000116

/**
  MSR information returned for MSR index #MSR_P6_BBL_CR_ADDR
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:3;
    ///
    /// [Bits 31:3] Address bits
    ///
    UINT32  Address:29;
    UINT32  Reserved2:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_BBL_CR_ADDR_REGISTER;


/**
  Data ECC register D[7:0]: used to write ECC and read ECC to/from L2.

  @param  ECX  MSR_P6_BBL_CR_DECC (0x00000118)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BBL_CR_DECC);
  AsmWriteMsr64 (MSR_P6_BBL_CR_DECC, Msr);
  @endcode
**/
#define MSR_P6_BBL_CR_DECC                       0x00000118


/**
  Control register: used to program L2 commands to be issued via cache
  configuration accesses mechanism. Also receives L2 lookup response.

  @param  ECX  MSR_P6_BBL_CR_CTL (0x00000119)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_CTL_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_CTL_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_BBL_CR_CTL_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_BBL_CR_CTL);
  AsmWriteMsr64 (MSR_P6_BBL_CR_CTL, Msr.Uint64);
  @endcode
**/
#define MSR_P6_BBL_CR_CTL                        0x00000119

/**
  MSR information returned for MSR index #MSR_P6_BBL_CR_CTL
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 4:0] L2 Command
    ///   Data Read w/ LRU update (RLU)
    ///   Tag Read w/ Data Read (TRR)
    ///   Tag Inquire (TI)
    ///   L2 Control Register Read (CR)
    ///   L2 Control Register Write (CW)
    ///   Tag Write w/ Data Read (TWR)
    ///   Tag Write w/ Data Write (TWW)
    ///   Tag Write (TW).
    ///
    UINT32  L2Command:5;
    ///
    /// [Bits 6:5] State to L2
    ///
    UINT32  StateToL2:2;
    UINT32  Reserved:1;
    ///
    /// [Bits 9:8] Way to L2.
    ///
    UINT32  WayToL2:2;
    ///
    /// [Bits 11:10] Way 0 - 00, Way 1 - 01, Way 2 - 10, Way 3 - 11.
    ///
    UINT32  Way:2;
    ///
    /// [Bits 13:12] Modified - 11,Exclusive - 10, Shared - 01, Invalid - 00.
    ///
    UINT32  MESI:2;
    ///
    /// [Bits 15:14] State from L2.
    ///
    UINT32  StateFromL2:2;
    UINT32  Reserved2:1;
    ///
    /// [Bit 17] L2 Hit.
    ///
    UINT32  L2Hit:1;
    UINT32  Reserved3:1;
    ///
    /// [Bits 20:19] User supplied ECC.
    ///
    UINT32  UserEcc:2;
    ///
    /// [Bit 21] Processor number Disable = 1 Enable = 0 Reserved.
    ///
    UINT32  ProcessorNumber:1;
    UINT32  Reserved4:10;
    UINT32  Reserved5:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_BBL_CR_CTL_REGISTER;


/**
  Trigger register: used to initiate a cache configuration accesses access,
  Write only with Data = 0.

  @param  ECX  MSR_P6_BBL_CR_TRIG (0x0000011A)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BBL_CR_TRIG);
  AsmWriteMsr64 (MSR_P6_BBL_CR_TRIG, Msr);
  @endcode
**/
#define MSR_P6_BBL_CR_TRIG                       0x0000011A


/**
  Busy register: indicates when a cache configuration accesses L2 command is
  in progress. D[0] = 1 = BUSY.

  @param  ECX  MSR_P6_BBL_CR_BUSY (0x0000011B)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_BBL_CR_BUSY);
  AsmWriteMsr64 (MSR_P6_BBL_CR_BUSY, Msr);
  @endcode
**/
#define MSR_P6_BBL_CR_BUSY                       0x0000011B


/**
  Control register 3: used to configure the L2 Cache.

  @param  ECX  MSR_P6_BBL_CR_CTL3 (0x0000011E)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_CTL3_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_BBL_CR_CTL3_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_BBL_CR_CTL3_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_BBL_CR_CTL3);
  AsmWriteMsr64 (MSR_P6_BBL_CR_CTL3, Msr.Uint64);
  @endcode
**/
#define MSR_P6_BBL_CR_CTL3                       0x0000011E

/**
  MSR information returned for MSR index #MSR_P6_BBL_CR_CTL3
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] L2 Configured (read/write ).
    ///
    UINT32  L2Configured:1;
    ///
    /// [Bits 4:1] L2 Cache Latency (read/write).
    ///
    UINT32  L2CacheLatency:4;
    ///
    /// [Bit 5] ECC Check Enable (read/write).
    ///
    UINT32  ECCCheckEnable:1;
    ///
    /// [Bit 6] Address Parity Check Enable (read/write).
    ///
    UINT32  AddressParityCheckEnable:1;
    ///
    /// [Bit 7] CRTN Parity Check Enable (read/write).
    ///
    UINT32  CRTNParityCheckEnable:1;
    ///
    /// [Bit 8] L2 Enabled (read/write).
    ///
    UINT32  L2Enabled:1;
    ///
    /// [Bits 10:9] L2 Associativity (read only) Direct Mapped 2 Way 4 Way
    /// Reserved.
    ///
    UINT32  L2Associativity:2;
    ///
    /// [Bits 12:11] Number of L2 banks (read only).
    ///
    UINT32  L2Banks:2;
    ///
    /// [Bits 17:13] Cache size per bank (read/write) 256KBytes 512KBytes
    /// 1MByte 2MByte 4MBytes.
    ///
    UINT32  CacheSizePerBank:5;
    ///
    /// [Bit 18] Cache State error checking enable (read/write).
    ///
    UINT32  CacheStateErrorEnable:1;
    UINT32  Reserved1:1;
    ///
    /// [Bits 22:20] L2 Physical Address Range support 64GBytes 32GBytes
    /// 16GBytes 8GBytes 4GBytes 2GBytes 1GBytes 512MBytes.
    ///
    UINT32  L2AddressRange:3;
    ///
    /// [Bit 23] L2 Hardware Disable (read only).
    ///
    UINT32  L2HardwareDisable:1;
    UINT32  Reserved2:1;
    ///
    /// [Bit 25] Cache bus fraction (read only).
    ///
    UINT32  CacheBusFraction:1;
    UINT32  Reserved3:6;
    UINT32  Reserved4:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_BBL_CR_CTL3_REGISTER;


/**
  CS register target for CPL 0 code.

  @param  ECX  MSR_P6_SYSENTER_CS_MSR (0x00000174)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_SYSENTER_CS_MSR);
  AsmWriteMsr64 (MSR_P6_SYSENTER_CS_MSR, Msr);
  @endcode
**/
#define MSR_P6_SYSENTER_CS_MSR                   0x00000174


/**
  Stack pointer for CPL 0 stack.

  @param  ECX  MSR_P6_SYSENTER_ESP_MSR (0x00000175)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_SYSENTER_ESP_MSR);
  AsmWriteMsr64 (MSR_P6_SYSENTER_ESP_MSR, Msr);
  @endcode
**/
#define MSR_P6_SYSENTER_ESP_MSR                  0x00000175


/**
  CPL 0 code entry point.

  @param  ECX  MSR_P6_SYSENTER_EIP_MSR (0x00000176)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_SYSENTER_EIP_MSR);
  AsmWriteMsr64 (MSR_P6_SYSENTER_EIP_MSR, Msr);
  @endcode
**/
#define MSR_P6_SYSENTER_EIP_MSR                  0x00000176


/**


  @param  ECX  MSR_P6_MCG_CAP (0x00000179)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MCG_CAP);
  AsmWriteMsr64 (MSR_P6_MCG_CAP, Msr);
  @endcode
**/
#define MSR_P6_MCG_CAP                           0x00000179


/**


  @param  ECX  MSR_P6_MCG_STATUS (0x0000017A)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MCG_STATUS);
  AsmWriteMsr64 (MSR_P6_MCG_STATUS, Msr);
  @endcode
**/
#define MSR_P6_MCG_STATUS                        0x0000017A


/**


  @param  ECX  MSR_P6_MCG_CTL (0x0000017B)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MCG_CTL);
  AsmWriteMsr64 (MSR_P6_MCG_CTL, Msr);
  @endcode
**/
#define MSR_P6_MCG_CTL                           0x0000017B


/**


  @param  ECX  MSR_P6_PERFEVTSELn
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_PERFEVTSEL_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_PERFEVTSEL_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_PERFEVTSEL_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_PERFEVTSEL0);
  AsmWriteMsr64 (MSR_P6_PERFEVTSEL0, Msr.Uint64);
  @endcode
  @{
**/
#define MSR_P6_PERFEVTSEL0                       0x00000186
#define MSR_P6_PERFEVTSEL1                       0x00000187
/// @}

/**
  MSR information returned for MSR indexes #MSR_P6_PERFEVTSEL0 and
  #MSR_P6_PERFEVTSEL1.
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 7:0] Event Select Refer to Performance Counter section for a
    /// list of event encodings.
    ///
    UINT32  EventSelect:8;
    ///
    /// [Bits 15:8] UMASK (Unit Mask) Unit mask register set to 0 to enable
    /// all count options.
    ///
    UINT32  UMASK:8;
    ///
    /// [Bit 16] USER Controls the counting of events at Privilege levels of
    /// 1, 2, and 3.
    ///
    UINT32  USR:1;
    ///
    /// [Bit 17] OS Controls the counting of events at Privilege level of 0.
    ///
    UINT32  OS:1;
    ///
    /// [Bit 18] E Occurrence/Duration Mode Select 1 = Occurrence 0 = Duration.
    ///
    UINT32  E:1;
    ///
    /// [Bit 19] PC Enabled the signaling of performance counter overflow via
    /// BP0 pin.
    ///
    UINT32  PC:1;
    ///
    /// [Bit 20] INT Enables the signaling of counter overflow via input to
    /// APIC 1 = Enable 0 = Disable.
    ///
    UINT32  INT:1;
    UINT32  Reserved1:1;
    ///
    /// [Bit 22] ENABLE Enables the counting of performance events in both
    /// counters 1 = Enable 0 = Disable.
    ///
    UINT32  EN:1;
    ///
    /// [Bit 23] INV Inverts the result of the CMASK condition 1 = Inverted 0
    /// = Non-Inverted.
    ///
    UINT32  INV:1;
    ///
    /// [Bits 31:24] CMASK (Counter Mask).
    ///
    UINT32  CMASK:8;
    UINT32  Reserved2:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_PERFEVTSEL_REGISTER;


/**


  @param  ECX  MSR_P6_DEBUGCTLMSR (0x000001D9)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_DEBUGCTLMSR_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_DEBUGCTLMSR_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_DEBUGCTLMSR_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_DEBUGCTLMSR);
  AsmWriteMsr64 (MSR_P6_DEBUGCTLMSR, Msr.Uint64);
  @endcode
**/
#define MSR_P6_DEBUGCTLMSR                       0x000001D9

/**
  MSR information returned for MSR index #MSR_P6_DEBUGCTLMSR
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bit 0] Enable/Disable Last Branch Records.
    ///
    UINT32  LBR:1;
    ///
    /// [Bit 1] Branch Trap Flag.
    ///
    UINT32  BTF:1;
    ///
    /// [Bit 2] Performance Monitoring/Break Point Pins.
    ///
    UINT32  PB0:1;
    ///
    /// [Bit 3] Performance Monitoring/Break Point Pins.
    ///
    UINT32  PB1:1;
    ///
    /// [Bit 4] Performance Monitoring/Break Point Pins.
    ///
    UINT32  PB2:1;
    ///
    /// [Bit 5] Performance Monitoring/Break Point Pins.
    ///
    UINT32  PB3:1;
    ///
    /// [Bit 6] Enable/Disable Execution Trace Messages.
    ///
    UINT32  TR:1;
    UINT32  Reserved1:25;
    UINT32  Reserved2:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_DEBUGCTLMSR_REGISTER;


/**


  @param  ECX  MSR_P6_LASTBRANCHFROMIP (0x000001DB)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_LASTBRANCHFROMIP);
  AsmWriteMsr64 (MSR_P6_LASTBRANCHFROMIP, Msr);
  @endcode
**/
#define MSR_P6_LASTBRANCHFROMIP                  0x000001DB


/**


  @param  ECX  MSR_P6_LASTBRANCHTOIP (0x000001DC)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_LASTBRANCHTOIP);
  AsmWriteMsr64 (MSR_P6_LASTBRANCHTOIP, Msr);
  @endcode
**/
#define MSR_P6_LASTBRANCHTOIP                    0x000001DC


/**


  @param  ECX  MSR_P6_LASTINTFROMIP (0x000001DD)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_LASTINTFROMIP);
  AsmWriteMsr64 (MSR_P6_LASTINTFROMIP, Msr);
  @endcode
**/
#define MSR_P6_LASTINTFROMIP                     0x000001DD


/**


  @param  ECX  MSR_P6_LASTINTTOIP (0x000001DE)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_LASTINTTOIP);
  AsmWriteMsr64 (MSR_P6_LASTINTTOIP, Msr);
  @endcode
**/
#define MSR_P6_LASTINTTOIP                       0x000001DE


/**


  @param  ECX  MSR_P6_ROB_CR_BKUPTMPDR6 (0x000001E0)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_ROB_CR_BKUPTMPDR6_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_ROB_CR_BKUPTMPDR6_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_ROB_CR_BKUPTMPDR6_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_ROB_CR_BKUPTMPDR6);
  AsmWriteMsr64 (MSR_P6_ROB_CR_BKUPTMPDR6, Msr.Uint64);
  @endcode
**/
#define MSR_P6_ROB_CR_BKUPTMPDR6                 0x000001E0

/**
  MSR information returned for MSR index #MSR_P6_ROB_CR_BKUPTMPDR6
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    UINT32  Reserved1:2;
    ///
    /// [Bit 2] Fast Strings Enable bit. Default is enabled.
    ///
    UINT32  FastStrings:1;
    UINT32  Reserved2:29;
    UINT32  Reserved3:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_ROB_CR_BKUPTMPDR6_REGISTER;


/**


  @param  ECX  MSR_P6_MTRRPHYSBASEn
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRPHYSBASE0);
  AsmWriteMsr64 (MSR_P6_MTRRPHYSBASE0, Msr);
  @endcode
  @{
**/
#define MSR_P6_MTRRPHYSBASE0                     0x00000200
#define MSR_P6_MTRRPHYSBASE1                     0x00000202
#define MSR_P6_MTRRPHYSBASE2                     0x00000204
#define MSR_P6_MTRRPHYSBASE3                     0x00000206
#define MSR_P6_MTRRPHYSBASE4                     0x00000208
#define MSR_P6_MTRRPHYSBASE5                     0x0000020A
#define MSR_P6_MTRRPHYSBASE6                     0x0000020C
#define MSR_P6_MTRRPHYSBASE7                     0x0000020E
/// @}


/**


  @param  ECX  MSR_P6_MTRRPHYSMASKn
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRPHYSMASK0);
  AsmWriteMsr64 (MSR_P6_MTRRPHYSMASK0, Msr);
  @endcode
  @{
**/
#define MSR_P6_MTRRPHYSMASK0                     0x00000201
#define MSR_P6_MTRRPHYSMASK1                     0x00000203
#define MSR_P6_MTRRPHYSMASK2                     0x00000205
#define MSR_P6_MTRRPHYSMASK3                     0x00000207
#define MSR_P6_MTRRPHYSMASK4                     0x00000209
#define MSR_P6_MTRRPHYSMASK5                     0x0000020B
#define MSR_P6_MTRRPHYSMASK6                     0x0000020D
#define MSR_P6_MTRRPHYSMASK7                     0x0000020F
/// @}


/**


  @param  ECX  MSR_P6_MTRRFIX64K_00000 (0x00000250)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX64K_00000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX64K_00000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX64K_00000                  0x00000250


/**


  @param  ECX  MSR_P6_MTRRFIX16K_80000 (0x00000258)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX16K_80000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX16K_80000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX16K_80000                  0x00000258


/**


  @param  ECX  MSR_P6_MTRRFIX16K_A0000 (0x00000259)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX16K_A0000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX16K_A0000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX16K_A0000                  0x00000259


/**


  @param  ECX  MSR_P6_MTRRFIX4K_C0000 (0x00000268)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_C0000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_C0000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_C0000                   0x00000268


/**


  @param  ECX  MSR_P6_MTRRFIX4K_C8000 (0x00000269)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_C8000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_C8000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_C8000                   0x00000269


/**


  @param  ECX  MSR_P6_MTRRFIX4K_D0000 (0x0000026A)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_D0000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_D0000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_D0000                   0x0000026A


/**


  @param  ECX  MSR_P6_MTRRFIX4K_D8000 (0x0000026B)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_D8000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_D8000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_D8000                   0x0000026B


/**


  @param  ECX  MSR_P6_MTRRFIX4K_E0000 (0x0000026C)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_E0000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_E0000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_E0000                   0x0000026C


/**


  @param  ECX  MSR_P6_MTRRFIX4K_E8000 (0x0000026D)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_E8000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_E8000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_E8000                   0x0000026D


/**


  @param  ECX  MSR_P6_MTRRFIX4K_F0000 (0x0000026E)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_F0000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_F0000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_F0000                   0x0000026E


/**


  @param  ECX  MSR_P6_MTRRFIX4K_F8000 (0x0000026F)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MTRRFIX4K_F8000);
  AsmWriteMsr64 (MSR_P6_MTRRFIX4K_F8000, Msr);
  @endcode
**/
#define MSR_P6_MTRRFIX4K_F8000                   0x0000026F


/**


  @param  ECX  MSR_P6_MTRRDEFTYPE (0x000002FF)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_MTRRDEFTYPE_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_MTRRDEFTYPE_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_MTRRDEFTYPE_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_MTRRDEFTYPE);
  AsmWriteMsr64 (MSR_P6_MTRRDEFTYPE, Msr.Uint64);
  @endcode
**/
#define MSR_P6_MTRRDEFTYPE                       0x000002FF

/**
  MSR information returned for MSR index #MSR_P6_MTRRDEFTYPE
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 2:0] Default memory type.
    ///
    UINT32  Type:3;
    UINT32  Reserved1:7;
    ///
    /// [Bit 10] Fixed MTRR enable.
    ///
    UINT32  FE:1;
    ///
    /// [Bit 11] MTRR Enable.
    ///
    UINT32  E:1;
    UINT32  Reserved2:20;
    UINT32  Reserved3:32;
  } Bits;
  ///
  /// All bit fields as a 32-bit value
  ///
  UINT32  Uint32;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_MTRRDEFTYPE_REGISTER;


/**


  @param  ECX  MSR_P6_MC0_CTL (0x00000400)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MC0_CTL);
  AsmWriteMsr64 (MSR_P6_MC0_CTL, Msr);
  @endcode
  @{
**/
#define MSR_P6_MC0_CTL                           0x00000400
#define MSR_P6_MC1_CTL                           0x00000404
#define MSR_P6_MC2_CTL                           0x00000408
#define MSR_P6_MC3_CTL                           0x00000410
#define MSR_P6_MC4_CTL                           0x0000040C
/// @}


/**

  Bit definitions for MSR_P6_MC4_STATUS are the same as MSR_P6_MC0_STATUS,
  except bits 0, 4, 57, and 61 are hardcoded to 1.

  @param  ECX  MSR_P6_MCn_STATUS
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_P6_MC_STATUS_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_P6_MC_STATUS_REGISTER.

  <b>Example usage</b>
  @code
  MSR_P6_MC_STATUS_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_P6_MC0_STATUS);
  AsmWriteMsr64 (MSR_P6_MC0_STATUS, Msr.Uint64);
  @endcode
  @{
**/
#define MSR_P6_MC0_STATUS                        0x00000401
#define MSR_P6_MC1_STATUS                        0x00000405
#define MSR_P6_MC2_STATUS                        0x00000409
#define MSR_P6_MC3_STATUS                        0x00000411
#define MSR_P6_MC4_STATUS                        0x0000040D
/// @}

/**
  MSR information returned for MSR index #MSR_P6_MC0_STATUS to
  #MSR_P6_MC4_STATUS
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 15:0] MC_STATUS_MCACOD.
    ///
    UINT32  MC_STATUS_MCACOD:16;
    ///
    /// [Bits 31:16] MC_STATUS_MSCOD.
    ///
    UINT32  MC_STATUS_MSCOD:16;
    UINT32  Reserved:25;
    ///
    /// [Bit 57] MC_STATUS_DAM.
    ///
    UINT32  MC_STATUS_DAM:1;
    ///
    /// [Bit 58] MC_STATUS_ADDRV.
    ///
    UINT32  MC_STATUS_ADDRV:1;
    ///
    /// [Bit 59] MC_STATUS_MISCV.
    ///
    UINT32  MC_STATUS_MISCV:1;
    ///
    /// [Bit 60] MC_STATUS_EN. (Note: For MC0_STATUS only, this bit is
    /// hardcoded to 1.).
    ///
    UINT32  MC_STATUS_EN:1;
    ///
    /// [Bit 61] MC_STATUS_UC.
    ///
    UINT32  MC_STATUS_UC:1;
    ///
    /// [Bit 62] MC_STATUS_O.
    ///
    UINT32  MC_STATUS_O:1;
    ///
    /// [Bit 63] MC_STATUS_V.
    ///
    UINT32  MC_STATUS_V:1;
  } Bits;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_P6_MC_STATUS_REGISTER;


/**

  MSR_P6_MC4_ADDR is defined in MCA architecture but not implemented in P6 Family processors.

  @param  ECX  MSR_P6_MC0_ADDR (0x00000402)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MC0_ADDR);
  AsmWriteMsr64 (MSR_P6_MC0_ADDR, Msr);
  @endcode
  @{
**/
#define MSR_P6_MC0_ADDR                          0x00000402
#define MSR_P6_MC1_ADDR                          0x00000406
#define MSR_P6_MC2_ADDR                          0x0000040A
#define MSR_P6_MC3_ADDR                          0x00000412
#define MSR_P6_MC4_ADDR                          0x0000040E
/// @}


/**
  Defined in MCA architecture but not implemented in the P6 family processors.

  @param  ECX  MSR_P6_MC0_MISC (0x00000403)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_P6_MC0_MISC);
  AsmWriteMsr64 (MSR_P6_MC0_MISC, Msr);
  @endcode
  @{
**/
#define MSR_P6_MC0_MISC                          0x00000403
#define MSR_P6_MC1_MISC                          0x00000407
#define MSR_P6_MC2_MISC                          0x0000040B
#define MSR_P6_MC3_MISC                          0x00000413
#define MSR_P6_MC4_MISC                          0x0000040F
/// @}

#endif
