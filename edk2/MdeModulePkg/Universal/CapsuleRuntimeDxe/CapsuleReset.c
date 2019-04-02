/** @file
  Default implementation of architecture specific routines related to
  PersistAcrossReset capsules

  Copyright (c) 2018, Linaro, Ltd. All rights reserved.<BR>
  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "CapsuleService.h"

/**
  Whether the platform supports capsules that persist across reset. Note that
  some platforms only support such capsules at boot time.

  @return TRUE  if a PersistAcrossReset capsule may be passed to UpdateCapsule()
                at this time
          FALSE otherwise
**/
BOOLEAN
IsPersistAcrossResetCapsuleSupported (
  VOID
  )
{
  return FeaturePcdGet (PcdSupportUpdateCapsuleReset);
}

