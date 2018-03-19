;------------------------------------------------------------------------------
;
; GetInterruptState() function for AArch64
;
; Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
; Portions copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
; Portions copyright (c) 2011 - 2013, ARM Ltd. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
;------------------------------------------------------------------------------

  EXPORT GetInterruptState
  AREA BaseLib_LowLevel, CODE, READONLY

DAIF_RD_IRQ_BIT     EQU     (1 << 7)

;/**
;  Retrieves the current CPU interrupt state.
;
;  Returns TRUE is interrupts are currently enabled. Otherwise
;  returns FALSE.
;
;  @retval TRUE  CPU interrupts are enabled.
;  @retval FALSE CPU interrupts are disabled.
;
;**/
;
;BOOLEAN
;EFIAPI
;GetInterruptState (
;  VOID
; );
;
GetInterruptState
    mrs    x0, daif
    mov    w0, wzr
    tst    x0, #DAIF_RD_IRQ_BIT   // Check IRQ mask; set Z=1 if clear/unmasked
    bne    exit                   // if Z=1 (eq) return 1, else 0
    mov    w0, #1
exit
    ret

  END
