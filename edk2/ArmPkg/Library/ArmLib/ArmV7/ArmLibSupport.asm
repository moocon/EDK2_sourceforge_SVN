//------------------------------------------------------------------------------ 
//
// Copyright (c) 2008-2009 Apple Inc. All rights reserved.
//
// All rights reserved. This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
//
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//------------------------------------------------------------------------------


    EXPORT  Cp15IdCode
    EXPORT  Cp15CacheInfo
    EXPORT  ArmEnableInterrupts
    EXPORT  ArmDisableInterrupts
    EXPORT  ArmGetInterruptState
    EXPORT  ArmInvalidateTlb
    EXPORT  ArmSetTranslationTableBaseAddress
    EXPORT  ArmGetTranslationTableBaseAddress
    EXPORT  ArmSetDomainAccessControl
    EXPORT  CPSRMaskInsert
    EXPORT  CPSRRead

    AREA ArmLibSupport, CODE, READONLY

Cp15IdCode
  DSB
  ISB
  mrc     p15,0,R0,c0,c0,0
  DSB
  ISB
  bx      LR

Cp15CacheInfo
  DSB
  ISB
  mrc     p15,0,R0,c0,c0,1
  DSB
  ISB
  bx      LR

ArmEnableInterrupts
  DSB
  ISB
	mrs     R0,CPSR
	bic     R0,R0,#0x80		;Enable IRQ interrupts
	msr     CPSR_c,R0
  DSB
  ISB
	bx      LR

ArmDisableInterrupts
  DSB
  ISB
	mrs     R0,CPSR
	orr     R1,R0,#0x80		;Disable IRQ interrupts
	msr     CPSR_c,R1
  tst     R0,#0x80
  moveq   R0,#1
  movne   R0,#0
  DSB
  ISB
	bx      LR

ArmGetInterruptState
  DSB
  ISB
	mrs     R0,CPSR
	tst     R0,#0x80	    ;Check if IRQ is enabled.
	moveq   R0,#1
	movne   R0,#0
  DSB
  ISB
	bx      LR
  
ArmInvalidateTlb
  DSB
  ISB
  mov     r0,#0
  mcr     p15,0,r0,c8,c7,0
  DSB
  ISB
  bx      lr

ArmSetTranslationTableBaseAddress
  DSB
  ISB
  mcr     p15,0,r0,c2,c0,0
  DSB
  ISB
  bx      lr

ArmGetTranslationTableBaseAddress
  DSB
  ISB
  mrc     p15,0,r0,c2,c0,0
  DSB
  ISB
  bx      lr

ArmSetDomainAccessControl
  DSB
  ISB
  mcr     p15,0,r0,c3,c0,0
  DSB
  ISB
  bx      lr

CPSRMaskInsert              ; on entry, r0 is the mask and r1 is the field to insert
  DSB
  ISB
  stmfd   sp!, {r4-r12, lr} ; save all the banked registers
  mov     r3, sp            ; copy the stack pointer into a non-banked register
  mrs     r2, cpsr          ; read the cpsr
  bic     r2, r2, r0        ; clear mask in the cpsr
  and     r1, r1, r0        ; clear bits outside the mask in the input
  orr     r2, r2, r1        ; set field
  msr     cpsr_cxsf, r2     ; write back cpsr (may have caused a mode switch)
  mov     sp, r3            ; restore stack pointer
  ldmfd   sp!, {r4-r12, lr} ; restore registers
  DSB
  ISB
  bx      lr                ; return (hopefully thumb-safe!)

CPSRRead
  DSB
  ISB
  mrs     r0, cpsr
  DSB
  ISB
  bx      lr
  
  END


