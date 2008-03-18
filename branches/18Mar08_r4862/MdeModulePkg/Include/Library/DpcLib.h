/** @file

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  DpcLib.h

Abstract:

  Library for Deferred Procedure Calls.

**/

#ifndef _DPC_LIB_H_
#define _DPC_LIB_H_

#include <PiDxe.h>
#include <Protocol/Dpc.h>

/**
  Add a Deferred Procedure Call to the end of the DPC queue.

  @param  DpcTpl        The EFI_TPL that the DPC should be invoked.
  @param  DpcProcedure  Pointer to the DPC's function.
  @param  DpcContext    Pointer to the DPC's context.  Passed to DpcProcedure
                        when DpcProcedure is invoked.

  @retval EFI_SUCCESS            The DPC was queued.
  @retval EFI_INVALID_PARAMETER  DpcTpl is not a valid EFI_TPL.
  @retval EFI_INVALID_PARAMETER  DpcProcedure is NULL.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resources available to
                                 add the DPC to the queue.

**/
EFI_STATUS
QueueDpc (
  IN EFI_TPL            DpcTpl,
  IN EFI_DPC_PROCEDURE  DpcProcedure,
  IN VOID               *DpcContext    OPTIONAL
  );

/**
  Dispatch the queue of DPCs.  ALL DPCs that have been queued with a DpcTpl
  value greater than or equal to the current TPL are invoked in the order that
  they were queued.  DPCs with higher DpcTpl values are invoked before DPCs with
  lower DpcTpl values.

  @retval EFI_SUCCESS    One or more DPCs were invoked.
  @retval EFI_NOT_FOUND  No DPCs were invoked.

**/
EFI_STATUS
DispatchDpc (
  VOID
  );

#endif
