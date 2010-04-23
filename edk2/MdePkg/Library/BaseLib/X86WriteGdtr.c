/** @file
  IA-32/x64 AsmWriteGdtr()

  Copyright (c) 2006 - 2008, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/




#include "BaseLibInternals.h"

/**
  Writes the current Global Descriptor Table Register (GDTR) descriptor.

  Writes and the current GDTR descriptor specified by Gdtr. This function is
  only available on IA-32 and x64.

  If Gdtr is NULL, then ASSERT().

  @param  Gdtr  Pointer to a GDTR descriptor.

**/
VOID
EFIAPI
AsmWriteGdtr (
  IN      CONST IA32_DESCRIPTOR     *Gdtr
  )
{
  ASSERT (Gdtr != NULL);
  InternalX86WriteGdtr (Gdtr);
}
