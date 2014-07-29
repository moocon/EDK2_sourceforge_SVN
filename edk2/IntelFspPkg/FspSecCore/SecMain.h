/** @file

  Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SEC_CORE_H_
#define _SEC_CORE_H_


#include <PiPei.h>
#include <Ppi/TemporaryRamSupport.h>

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/SerialPortLib.h>
#include <Library/FspSwitchStackLib.h>
#include <Library/FspCommonLib.h>
#include <FspApi.h>

#define SEC_IDT_ENTRY_COUNT    34

typedef VOID (*PEI_CORE_ENTRY) ( \
  IN CONST  EFI_SEC_PEI_HAND_OFF    *SecCoreData, \
  IN CONST  EFI_PEI_PPI_DESCRIPTOR  *PpiList \
);

typedef struct _SEC_IDT_TABLE {
  EFI_PEI_SERVICES  *PeiService;
  UINT64            IdtTable[SEC_IDT_ENTRY_COUNT];
} SEC_IDT_TABLE;

/**
  Switch the stack in the temporary memory to the one in the permanent memory.

  This function must be invoked after the memory migration immediately. The relative
  position of the stack in the temporary and permanent memory is same.

  @param[in] TemporaryMemoryBase  Base address of the temporary memory.
  @param[in] PermenentMemoryBase  Base address of the permanent memory.
**/
VOID
EFIAPI
SecSwitchStack (
  IN UINT32   TemporaryMemoryBase,
  IN UINT32   PermenentMemoryBase
  );

/**
  This service of the TEMPORARY_RAM_SUPPORT_PPI that migrates temporary RAM into
  permanent memory.

  @param[in] PeiServices            Pointer to the PEI Services Table.
  @param[in] TemporaryMemoryBase    Source Address in temporary memory from which the SEC or PEIM will copy the
                                Temporary RAM contents.
  @param[in] PermanentMemoryBase    Destination Address in permanent memory into which the SEC or PEIM will copy the
                                Temporary RAM contents.
  @param[in] CopySize               Amount of memory to migrate from temporary to permanent memory.

  @retval EFI_SUCCESS           The data was successfully returned.
  @retval EFI_INVALID_PARAMETER PermanentMemoryBase + CopySize > TemporaryMemoryBase when
                                TemporaryMemoryBase > PermanentMemoryBase.

**/
EFI_STATUS
EFIAPI
SecTemporaryRamSupport (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PHYSICAL_ADDRESS     TemporaryMemoryBase,
  IN EFI_PHYSICAL_ADDRESS     PermanentMemoryBase,
  IN UINTN                    CopySize
  );

/**
  Initializes floating point units for requirement of UEFI specification.

  This function initializes floating-point control word to 0x027F (all exceptions
  masked,double-precision, round-to-nearest) and multimedia-extensions control word
  (if supported) to 0x1F80 (all exceptions masked, round-to-nearest, flush to zero
  for masked underflow).

**/
VOID
EFIAPI
InitializeFloatingPointUnits (
  VOID
  );

/**

  Entry point to the C language phase of SEC. After the SEC assembly
  code has initialized some temporary memory and set up the stack,
  the control is transferred to this function.


  @param[in] SizeOfRam          Size of the temporary memory available for use.
  @param[in] TempRamBase        Base address of tempory ram
  @param[in] BootFirmwareVolume Base address of the Boot Firmware Volume.

  @return This function never returns.

**/
VOID
EFIAPI
SecStartup (
  IN UINT32                   SizeOfRam,
  IN UINT32                   TempRamBase,
  IN VOID                    *BootFirmwareVolume
  );

/**
  Autogenerated function that calls the library constructors for all of the module's
  dependent libraries.  This function must be called by the SEC Core once a stack has
  been established.

**/
VOID
EFIAPI
ProcessLibraryConstructorList (
  VOID
  );

#endif
