/** @file
  AsmReadGs function

  Copyright (c) 2006 - 2007, Intel Corporation<BR>
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//
// Include common header file for this module.
//


/**
  Reads the current value of GS Data Segment Register (GS).

  Reads and returns the current value of GS. This function is only available on
  IA-32 and X64.

  @return The current value of GS.

**/
UINT16
EFIAPI
AsmReadGs (
  VOID
  )
{
  __asm {
    xor     eax, eax
    mov     ax, gs
  }
}

