/** @file
SMM profile internal header file.

Copyright (c) 2012 - 2015, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SMM_PROFILE_INTERNAL_H_
#define _SMM_PROFILE_INTERNAL_H_

#include <Guid/GlobalVariable.h>
#include <Guid/Acpi.h>
#include <Protocol/SmmReadyToLock.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/CpuLib.h>
#include <IndustryStandard/Acpi.h>

#include "SmmProfileArch.h"

//
// Configure the SMM_PROFILE DTS region size
//
#define SMM_PROFILE_DTS_SIZE       (4 * 1024 * 1024) // 4M

#define MAX_PF_PAGE_COUNT           0x2

#define PEBS_RECORD_NUMBER          0x2

#define MAX_PF_ENTRY_COUNT          10

//
// This MACRO just enable unit test for the profile
// Please disable it.
//

#define IA32_PF_EC_P                (1u << 0)
#define IA32_PF_EC_WR               (1u << 1)
#define IA32_PF_EC_US               (1u << 2)
#define IA32_PF_EC_RSVD             (1u << 3)
#define IA32_PF_EC_ID               (1u << 4)

#define SMM_PROFILE_NAME            L"SmmProfileData"

//
// CPU generic definition
//
#define   CPUID1_EDX_XD_SUPPORT      0x100000
#define   MSR_EFER                   0xc0000080
#define   MSR_EFER_XD                0x800

#define   CPUID1_EDX_BTS_AVAILABLE   0x200000

#define   DR6_SINGLE_STEP            0x4000
#define   RFLAG_TF                   0x100

#define MSR_DEBUG_CTL                0x1D9
#define   MSR_DEBUG_CTL_LBR          0x1
#define   MSR_DEBUG_CTL_TR           0x40
#define   MSR_DEBUG_CTL_BTS          0x80
#define   MSR_DEBUG_CTL_BTINT        0x100
#define MSR_LASTBRANCH_TOS           0x1C9
#define MSR_LER_FROM_LIP             0x1DD
#define MSR_LER_TO_LIP               0x1DE
#define MSR_DS_AREA                  0x600

typedef struct {
  EFI_PHYSICAL_ADDRESS   Base;
  EFI_PHYSICAL_ADDRESS   Top;
} MEMORY_RANGE;

typedef struct {
  MEMORY_RANGE   Range;
  BOOLEAN        Present;
  BOOLEAN        Nx;
} MEMORY_PROTECTION_RANGE;

typedef struct {
  UINT64  HeaderSize;
  UINT64  MaxDataEntries;
  UINT64  MaxDataSize;
  UINT64  CurDataEntries;
  UINT64  CurDataSize;
  UINT64  TsegStart;
  UINT64  TsegSize;
  UINT64  NumSmis;
  UINT64  NumCpus;
} SMM_PROFILE_HEADER;

typedef struct {
  UINT64  SmiNum;
  UINT64  CpuNum;
  UINT64  ApicId;
  UINT64  ErrorCode;
  UINT64  Instruction;
  UINT64  Address;
  UINT64  SmiCmd;
} SMM_PROFILE_ENTRY;

extern SMM_S3_RESUME_STATE       *mSmmS3ResumeState;
extern UINTN                     gSmiExceptionHandlers[];
extern BOOLEAN                   mXdSupported;
extern UINTN                     *mPFEntryCount;
extern UINT64                    (*mLastPFEntryValue)[MAX_PF_ENTRY_COUNT];
extern UINT64                    *(*mLastPFEntryPointer)[MAX_PF_ENTRY_COUNT];

//
// Internal functions
//

/**
  Update IDT table to replace page fault handler and INT 1 handler.

**/
VOID
InitIdtr (
  VOID
  );

/**
  Check if the memory address will be mapped by 4KB-page.

  @param  Address  The address of Memory.

**/
BOOLEAN
IsAddressSplit (
  IN EFI_PHYSICAL_ADDRESS   Address
  );

/**
  Check if the memory address will be mapped by 4KB-page.

  @param  Address  The address of Memory.
  @param  Nx       The flag indicates if the memory is execute-disable.

**/
BOOLEAN
IsAddressValid (
  IN EFI_PHYSICAL_ADDRESS   Address,
  IN BOOLEAN                *Nx
  );

/**
  Page Fault handler for SMM use.

**/
VOID
SmiDefaultPFHandler (
  VOID
  );

/**
  Clear TF in FLAGS.

  @param  SystemContext    A pointer to the processor context when
                           the interrupt occurred on the processor.

**/
VOID
ClearTrapFlag (
  IN OUT EFI_SYSTEM_CONTEXT   SystemContext
  );

#endif // _SMM_PROFILE_H_
