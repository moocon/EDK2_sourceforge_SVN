/** @file
  This module update S3 Suspend Performance Record in ACPI Firmware Performance Data Table.

  This module register report status code listener to collect performance data
  for S3 Suspend Performance Record.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiSmm.h>

#include <IndustryStandard/Acpi50.h>

#include <Protocol/SmmReportStatusCodeHandler.h>

#include <Guid/FirmwarePerformance.h>

#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/LockBoxLib.h>
#include <Library/PcdLib.h>

EFI_SMM_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol    = NULL;
UINT64                        mSuspendStartTime       = 0;
BOOLEAN                       mS3SuspendLockBoxSaved  = FALSE;

/**
  Report status code listener for SMM. This is used to record the performance
  data for S3 Suspend Start and S3 Suspend End in FPDT.

  @param[in]  CodeType            Indicates the type of status code being reported.
  @param[in]  Value               Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is used to
                                  classify the entity as well as an operation.
  @param[in]  Instance            The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in]  CallerId            This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different rules to
                                  different callers.
  @param[in]  Data                This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             Status code is what we expected.
  @retval EFI_UNSUPPORTED         Status code not supported.

**/
EFI_STATUS
EFIAPI
FpdtStatusCodeListenerSmm (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data
  )
{
  EFI_STATUS                           Status;
  UINT64                               CurrentTime;
  EFI_ACPI_5_0_FPDT_S3_SUSPEND_RECORD  S3SuspendRecord;

  //
  // Check whether status code is what we are interested in.
  //
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE) {
    return EFI_UNSUPPORTED;
  }
  if ((Value != PcdGet32 (PcdProgressCodeS3SuspendStart)) &&
      (Value != PcdGet32 (PcdProgressCodeS3SuspendEnd))) {
    return EFI_UNSUPPORTED;
  }

  //
  // Retrieve current time.
  //
  CurrentTime = GetTimeInNanoSecond (GetPerformanceCounter ());

  if (Value == PcdGet32 (PcdProgressCodeS3SuspendStart)) {
    //
    // S3 Suspend started, record the performance data and return.
    //
    mSuspendStartTime = CurrentTime;
    return EFI_SUCCESS;
  }

  //
  // We are going to S3 sleep, record S3 Suspend End performance data.
  //
  S3SuspendRecord.SuspendStart = mSuspendStartTime;
  S3SuspendRecord.SuspendEnd   = CurrentTime;

  //
  // Save S3 suspend performance data to lock box, it will be used by Firmware Performance PEIM.
  //
  if (!mS3SuspendLockBoxSaved) {
    Status = SaveLockBox (
               &gEfiFirmwarePerformanceGuid,
               &S3SuspendRecord,
               sizeof (EFI_ACPI_5_0_FPDT_S3_SUSPEND_RECORD)
               );
    ASSERT_EFI_ERROR (Status);

    mS3SuspendLockBoxSaved = TRUE;
  } else {
    Status = UpdateLockBox (
               &gEfiFirmwarePerformanceGuid,
               0,
               &S3SuspendRecord,
               sizeof (EFI_ACPI_5_0_FPDT_S3_SUSPEND_RECORD)
               );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

/**
  The module Entry Point of the Firmware Performance Data Table SMM driver.

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
FirmwarePerformanceSmmEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  if (FeaturePcdGet (PcdFirmwarePerformanceDataTableS3Support)) {
    EFI_STATUS  Status;

    //
    // Get SMM Report Status Code Handler Protocol.
    //
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmRscHandlerProtocolGuid,
                      NULL,
                      (VOID **) &mRscHandlerProtocol
                      );
    ASSERT_EFI_ERROR (Status);

    //
    // Register report status code listener for S3 Suspend Start and End.
    //
    Status = mRscHandlerProtocol->Register (FpdtStatusCodeListenerSmm);
    ASSERT_EFI_ERROR (Status);

    return Status;
  } else {
    return EFI_UNSUPPORTED;
  }
}
