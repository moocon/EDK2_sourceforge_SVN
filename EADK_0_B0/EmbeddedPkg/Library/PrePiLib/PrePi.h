/** @file
  Library that helps implement monolithic PEI (i.e. PEI part of SEC)

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PI_PEI_H_
#define _PI_PEI_H_

#include <PiPei.h>

#include <Library/BaseLib.h>
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDecompressLib.h>
#include <Library/PeCoffLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/TimerLib.h>
#include <Library/PerformanceLib.h>

#include <Guid/MemoryAllocationHob.h>


#define GET_HOB_TYPE(Hob)     ((Hob).Header->HobType)
#define GET_HOB_LENGTH(Hob)   ((Hob).Header->HobLength)
#define GET_NEXT_HOB(Hob)     ((Hob).Raw + GET_HOB_LENGTH (Hob))
#define END_OF_HOB_LIST(Hob)  (GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_END_OF_HOB_LIST)

//
// Get the data and data size field of GUID 
//
#define GET_GUID_HOB_DATA(GuidHob)      ((VOID *) (((UINT8 *) &((GuidHob)->Name)) + sizeof (EFI_GUID)))
#define GET_GUID_HOB_DATA_SIZE(GuidHob) (((GuidHob)->Header).HobLength - sizeof (EFI_HOB_GUID_TYPE))

#endif
