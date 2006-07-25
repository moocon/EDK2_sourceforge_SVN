;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation
; All rights reserved. This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
; Module Name:
;
;   EnablePaging64.Asm
;
; Abstract:
;
;   AsmEnablePaging64 function
;
; Notes:
;
;------------------------------------------------------------------------------

    .code

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; InternalX86EnablePaging64 (
;   IN      UINT16                    Cs,
;   IN      UINT64                    EntryPoint,
;   IN      UINT64                    Context1,  OPTIONAL
;   IN      UINT64                    Context2,  OPTIONAL
;   IN      UINT64                    NewStack
;   );
;------------------------------------------------------------------------------
InternalX86EnablePaging64 PROC
    cli
    mov     dword ptr [rsp], @F         ; offset for far retf, seg is the 1st arg
    mov     rax, cr4                    ; mov eax, cr4
    or      al, (1 SHL 5)
    mov     cr4, rax                    ; enable PAE
    mov     ecx, 0c0000080h
    rdmsr
    or      ah, 1                       ; set LME
    wrmsr
    mov     rax, cr0                    ; mov eax, cr0
    bts     eax, 31
    mov     cr0, rax                    ; enable paging
    retf
@@:                                     ; long mode starts here
    mov     rbx, [esp]
    mov     rcx, [esp + 8]
    mov     rdx, [esp + 10h]
    mov     rsp, [esp + 18h]
    add     rsp, -20h
    call    rbx
    jmp     $                           ; halt processor if EntryPoint() returned
InternalX86EnablePaging64 ENDP

    END
