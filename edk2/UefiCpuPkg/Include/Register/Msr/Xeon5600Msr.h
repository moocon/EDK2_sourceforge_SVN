/** @file
  MSR Definitions for Intel(R) Xeon(R) Processor Series 5600.

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
  December 2015, Chapter 35 Model-Specific-Registers (MSR), Section 35-6.

**/

#ifndef __XEON_5600_MSR_H__
#define __XEON_5600_MSR_H__

#include <Register/ArchitecturalMsr.h>

/**
  Core. AES Configuration (RW-L) Privileged post-BIOS agent must provide a #GP
  handler to handle unsuccessful read of this MSR.

  @param  ECX  MSR_XEON_5600_FEATURE_CONFIG (0x0000013C)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_XEON_5600_FEATURE_CONFIG_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_XEON_5600_FEATURE_CONFIG_REGISTER.

  <b>Example usage</b>
  @code
  MSR_XEON_5600_FEATURE_CONFIG_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_XEON_5600_FEATURE_CONFIG);
  AsmWriteMsr64 (MSR_XEON_5600_FEATURE_CONFIG, Msr.Uint64);
  @endcode
  @note MSR_XEON_5600_FEATURE_CONFIG is defined as MSR_FEATURE_CONFIG in SDM.
**/
#define MSR_XEON_5600_FEATURE_CONFIG             0x0000013C

/**
  MSR information returned for MSR index #MSR_XEON_5600_FEATURE_CONFIG
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 1:0] AES Configuration (RW-L)  Upon a successful read of this
    /// MSR, the configuration of AES instruction set availability is as
    /// follows: 11b: AES instructions are not available until next RESET.
    /// otherwise, AES instructions are available. Note, AES instruction set
    /// is not available if read is unsuccessful. If the configuration is not
    /// 01b, AES instruction can be mis-configured if a privileged agent
    /// unintentionally writes 11b.
    ///
    UINT32  AESConfiguration:2;
    UINT32  Reserved1:30;
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
} MSR_XEON_5600_FEATURE_CONFIG_REGISTER;


/**
  Thread. Offcore Response Event Select Register (R/W).

  @param  ECX  MSR_XEON_5600_OFFCORE_RSP_1 (0x000001A7)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_XEON_5600_OFFCORE_RSP_1);
  AsmWriteMsr64 (MSR_XEON_5600_OFFCORE_RSP_1, Msr);
  @endcode
  @note MSR_XEON_5600_OFFCORE_RSP_1 is defined as MSR_OFFCORE_RSP_1 in SDM.
**/
#define MSR_XEON_5600_OFFCORE_RSP_1              0x000001A7


/**
  Package. Maximum Ratio Limit of Turbo Mode RO if MSR_PLATFORM_INFO.[28] = 0,
  RW if MSR_PLATFORM_INFO.[28] = 1.

  @param  ECX  MSR_XEON_5600_TURBO_RATIO_LIMIT (0x000001AD)
  @param  EAX  Lower 32-bits of MSR value.
               Described by the type MSR_XEON_5600_TURBO_RATIO_LIMIT_REGISTER.
  @param  EDX  Upper 32-bits of MSR value.
               Described by the type MSR_XEON_5600_TURBO_RATIO_LIMIT_REGISTER.

  <b>Example usage</b>
  @code
  MSR_XEON_5600_TURBO_RATIO_LIMIT_REGISTER  Msr;

  Msr.Uint64 = AsmReadMsr64 (MSR_XEON_5600_TURBO_RATIO_LIMIT);
  @endcode
  @note MSR_XEON_5600_TURBO_RATIO_LIMIT is defined as MSR_TURBO_RATIO_LIMIT in SDM.
**/
#define MSR_XEON_5600_TURBO_RATIO_LIMIT          0x000001AD

/**
  MSR information returned for MSR index #MSR_XEON_5600_TURBO_RATIO_LIMIT_REGISTER
**/
typedef union {
  ///
  /// Individual bit fields
  ///
  struct {
    ///
    /// [Bits 7:0] Package. Maximum Ratio Limit for 1C Maximum turbo ratio
    /// limit of 1 core active.
    ///
    UINT32  Maximum1C:8;
    ///
    /// [Bits 15:8] Package. Maximum Ratio Limit for 2C Maximum turbo ratio
    /// limit of 2 core active.
    ///
    UINT32  Maximum2C:8;
    ///
    /// [Bits 23:16] Package. Maximum Ratio Limit for 3C Maximum turbo ratio
    /// limit of 3 core active.
    ///
    UINT32  Maximum3C:8;
    ///
    /// [Bits 31:24] Package. Maximum Ratio Limit for 4C Maximum turbo ratio
    /// limit of 4 core active.
    ///
    UINT32  Maximum4C:8;
    ///
    /// [Bits 39:32] Package. Maximum Ratio Limit for 5C Maximum turbo ratio
    /// limit of 5 core active.
    ///
    UINT32  Maximum5C:8;
    ///
    /// [Bits 47:40] Package. Maximum Ratio Limit for 6C Maximum turbo ratio
    /// limit of 6 core active.
    ///
    UINT32  Maximum6C:8;
    UINT32  Reserved:16;
  } Bits;
  ///
  /// All bit fields as a 64-bit value
  ///
  UINT64  Uint64;
} MSR_XEON_5600_TURBO_RATIO_LIMIT_REGISTER;


/**
  Package. See Table 35-2.

  @param  ECX  MSR_XEON_5600_IA32_ENERGY_PERF_BIAS (0x000001B0)
  @param  EAX  Lower 32-bits of MSR value.
  @param  EDX  Upper 32-bits of MSR value.

  <b>Example usage</b>
  @code
  UINT64  Msr;

  Msr = AsmReadMsr64 (MSR_XEON_5600_IA32_ENERGY_PERF_BIAS);
  AsmWriteMsr64 (MSR_XEON_5600_IA32_ENERGY_PERF_BIAS, Msr);
  @endcode
  @note MSR_XEON_5600_IA32_ENERGY_PERF_BIAS is defined as IA32_ENERGY_PERF_BIAS in SDM.
**/
#define MSR_XEON_5600_IA32_ENERGY_PERF_BIAS      0x000001B0

#endif
