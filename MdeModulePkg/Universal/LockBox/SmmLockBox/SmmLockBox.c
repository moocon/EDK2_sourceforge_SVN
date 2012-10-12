/** @file
  LockBox SMM driver.
  
  Caution: This module requires additional review when modified.
  This driver will have external input - communicate buffer in SMM mode.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.
  
  SmmLockBoxHandler(), SmmLockBoxRestore(), SmmLockBoxUpdate(), SmmLockBoxSave()
  will receive untrusted input and do basic validation.

Copyright (c) 2010 - 2012, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/LockBoxLib.h>
#include <Protocol/SmmReadyToLock.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/LockBox.h>
#include <Guid/SmmLockBox.h>

BOOLEAN              mLocked = FALSE;

EFI_SMRAM_DESCRIPTOR *mSmramRanges;
UINTN                mSmramRangeCount;

/**
  This function check if the address is in SMRAM.

  @param Buffer  the buffer address to be checked.
  @param Length  the buffer length to be checked.

  @retval TRUE  this address is in SMRAM.
  @retval FALSE this address is NOT in SMRAM.
**/
BOOLEAN
IsAddressInSmram (
  IN EFI_PHYSICAL_ADDRESS  Buffer,
  IN UINT64                Length
  )
{
  UINTN  Index;

  for (Index = 0; Index < mSmramRangeCount; Index ++) {
    if (((Buffer >= mSmramRanges[Index].CpuStart) && (Buffer < mSmramRanges[Index].CpuStart + mSmramRanges[Index].PhysicalSize)) ||
        ((mSmramRanges[Index].CpuStart >= Buffer) && (mSmramRanges[Index].CpuStart < Buffer + Length))) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Dispatch function for SMM lock box save.

  @param LockBoxParameterSave  parameter of lock box save 
**/
VOID
SmmLockBoxSave (
  IN EFI_SMM_LOCK_BOX_PARAMETER_SAVE *LockBoxParameterSave
  )
{
  EFI_STATUS                  Status;

  //
  // Sanity check
  //
  if (mLocked) {
    DEBUG ((EFI_D_ERROR, "SmmLockBox Locked!\n"));
    LockBoxParameterSave->Header.ReturnStatus = (UINT64)EFI_ACCESS_DENIED;
    return ;
  }

  //
  // Save data
  //
  Status = SaveLockBox (
             &LockBoxParameterSave->Guid,
             (VOID *)(UINTN)LockBoxParameterSave->Buffer,
             (UINTN)LockBoxParameterSave->Length
             );
  LockBoxParameterSave->Header.ReturnStatus = (UINT64)Status;
  return ;
}

/**
  Dispatch function for SMM lock box set attributes.

  @param LockBoxParameterSetAttributes  parameter of lock box set attributes
**/
VOID
SmmLockBoxSetAttributes (
  IN EFI_SMM_LOCK_BOX_PARAMETER_SET_ATTRIBUTES *LockBoxParameterSetAttributes
  )
{
  EFI_STATUS                    Status;

  //
  // Sanity check
  //
  if (mLocked) {
    DEBUG ((EFI_D_ERROR, "SmmLockBox Locked!\n"));
    LockBoxParameterSetAttributes->Header.ReturnStatus = (UINT64)EFI_ACCESS_DENIED;
    return ;
  }

  //
  // Update data
  //
  Status = SetLockBoxAttributes (
             &LockBoxParameterSetAttributes->Guid,
             LockBoxParameterSetAttributes->Attributes
             );
  LockBoxParameterSetAttributes->Header.ReturnStatus = (UINT64)Status;
  return ;
}

/**
  Dispatch function for SMM lock box update.

  @param LockBoxParameterUpdate  parameter of lock box update 
**/
VOID
SmmLockBoxUpdate (
  IN EFI_SMM_LOCK_BOX_PARAMETER_UPDATE *LockBoxParameterUpdate
  )
{
  EFI_STATUS                    Status;

  //
  // Sanity check
  //
  if (mLocked) {
    DEBUG ((EFI_D_ERROR, "SmmLockBox Locked!\n"));
    LockBoxParameterUpdate->Header.ReturnStatus = (UINT64)EFI_ACCESS_DENIED;
    return ;
  }

  //
  // Update data
  //
  Status = UpdateLockBox (
             &LockBoxParameterUpdate->Guid,
             (UINTN)LockBoxParameterUpdate->Offset,
             (VOID *)(UINTN)LockBoxParameterUpdate->Buffer,
             (UINTN)LockBoxParameterUpdate->Length
             );
  LockBoxParameterUpdate->Header.ReturnStatus = (UINT64)Status;
  return ;
}

/**
  Dispatch function for SMM lock box restore.

  @param LockBoxParameterRestore  parameter of lock box restore 
**/
VOID
SmmLockBoxRestore (
  IN EFI_SMM_LOCK_BOX_PARAMETER_RESTORE *LockBoxParameterRestore
  )
{
  EFI_STATUS                     Status;

  //
  // Sanity check
  //
  if (IsAddressInSmram (LockBoxParameterRestore->Buffer, LockBoxParameterRestore->Length)) {
    DEBUG ((EFI_D_ERROR, "SmmLockBox Restore address in SMRAM!\n"));
    LockBoxParameterRestore->Header.ReturnStatus = (UINT64)EFI_ACCESS_DENIED;
    return ;
  }

  //
  // Restore data
  //
  if ((LockBoxParameterRestore->Length == 0) && (LockBoxParameterRestore->Buffer == 0)) {
    Status = RestoreLockBox (
               &LockBoxParameterRestore->Guid,
               NULL,
               NULL
               );
  } else {
    Status = RestoreLockBox (
               &LockBoxParameterRestore->Guid,
               (VOID *)(UINTN)LockBoxParameterRestore->Buffer,
               (UINTN *)&LockBoxParameterRestore->Length
               );
  }
  LockBoxParameterRestore->Header.ReturnStatus = (UINT64)Status;
  return ;
}

/**
  Dispatch function for SMM lock box restore all in place.

  @param LockBoxParameterRestoreAllInPlace  parameter of lock box restore all in place
**/
VOID
SmmLockBoxRestoreAllInPlace (
  IN EFI_SMM_LOCK_BOX_PARAMETER_RESTORE_ALL_IN_PLACE *LockBoxParameterRestoreAllInPlace
  )
{
  EFI_STATUS                     Status;

  Status = RestoreAllLockBoxInPlace ();
  LockBoxParameterRestoreAllInPlace->Header.ReturnStatus = (UINT64)Status;
  return ;
}

/**
  Dispatch function for a Software SMI handler.

  @param DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param Context         Points to an optional handler context which was specified when the
                         handler was registered.
  @param CommBuffer      A pointer to a collection of data in memory that will
                         be conveyed from a non-SMM environment into an SMM environment.
  @param CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS Command is handled successfully.

**/
EFI_STATUS
EFIAPI
SmmLockBoxHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  EFI_SMM_LOCK_BOX_PARAMETER_HEADER *LockBoxParameterHeader;

  DEBUG ((EFI_D_ERROR, "SmmLockBox SmmLockBoxHandler Enter\n"));

  LockBoxParameterHeader = (EFI_SMM_LOCK_BOX_PARAMETER_HEADER *)((UINTN)CommBuffer);

  LockBoxParameterHeader->ReturnStatus = (UINT64)-1;

  DEBUG ((EFI_D_ERROR, "SmmLockBox LockBoxParameterHeader - %x\n", (UINTN)LockBoxParameterHeader));

  DEBUG ((EFI_D_ERROR, "SmmLockBox Command - %x\n", (UINTN)LockBoxParameterHeader->Command));

  switch (LockBoxParameterHeader->Command) {
  case EFI_SMM_LOCK_BOX_COMMAND_SAVE:
    SmmLockBoxSave ((EFI_SMM_LOCK_BOX_PARAMETER_SAVE *)(UINTN)LockBoxParameterHeader);
    break;
  case EFI_SMM_LOCK_BOX_COMMAND_UPDATE:
    SmmLockBoxUpdate ((EFI_SMM_LOCK_BOX_PARAMETER_UPDATE *)(UINTN)LockBoxParameterHeader);
    break;
  case EFI_SMM_LOCK_BOX_COMMAND_RESTORE:
    SmmLockBoxRestore ((EFI_SMM_LOCK_BOX_PARAMETER_RESTORE *)(UINTN)LockBoxParameterHeader);
    break;
  case EFI_SMM_LOCK_BOX_COMMAND_SET_ATTRIBUTES:
    SmmLockBoxSetAttributes ((EFI_SMM_LOCK_BOX_PARAMETER_SET_ATTRIBUTES *)(UINTN)LockBoxParameterHeader);
    break;
  case EFI_SMM_LOCK_BOX_COMMAND_RESTORE_ALL_IN_PLACE:
    SmmLockBoxRestoreAllInPlace ((EFI_SMM_LOCK_BOX_PARAMETER_RESTORE_ALL_IN_PLACE *)(UINTN)LockBoxParameterHeader);
    break;
  default:
    break;
  }

  LockBoxParameterHeader->Command = (UINT32)-1;

  DEBUG ((EFI_D_ERROR, "SmmLockBox SmmLockBoxHandler Exit\n"));

  return EFI_SUCCESS;
}

/**
  Smm Ready To Lock event notification handler.

  It sets a flag indicating that SMRAM has been locked.
  
  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification handler runs successfully.
 **/
EFI_STATUS
EFIAPI
SmmReadyToLockEventNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  mLocked = TRUE;
  return EFI_SUCCESS;
}

/**
  Entry Point for LockBox SMM driver.

  @param[in] ImageHandle  Image handle of this driver.
  @param[in] SystemTable  A Pointer to the EFI System Table.

  @retval EFI_SUCEESS     
  @return Others          Some error occurs.
**/
EFI_STATUS
EFIAPI
SmmLockBoxEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    DispatchHandle;
  VOID                          *Registration;
  EFI_SMM_ACCESS2_PROTOCOL      *SmmAccess;
  UINTN                         Size;

  //
  // Get SMRAM information
  //
  Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
  ASSERT_EFI_ERROR (Status);

  Size = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    Size,
                    (VOID **)&mSmramRanges
                    );
  ASSERT_EFI_ERROR (Status);

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
  ASSERT_EFI_ERROR (Status);

  mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);

  //
  // Register LockBox communication handler
  //
  Status = gSmst->SmiHandlerRegister (
                    SmmLockBoxHandler,
                    &gEfiSmmLockBoxCommunicationGuid,
                    &DispatchHandle
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register SMM Ready To Lock Protocol notification
  //
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmReadyToLockProtocolGuid,
                    SmmReadyToLockEventNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Install NULL to DXE data base as notify
  //
  ImageHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiLockBoxProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
