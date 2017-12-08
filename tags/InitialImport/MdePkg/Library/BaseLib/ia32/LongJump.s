#------------------------------------------------------------------------------
#
# Copyright (c) 2006, Intel Corporation
# All rights reserved. This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
# Module Name:
#
#   LongJump.Asm
#
# Abstract:
#
#   Implementation of LongJump() on IA-32.
#
#------------------------------------------------------------------------------



     

.global _LongJump
_LongJump: 
    popl    %eax
    popl    %edx
    popl    %eax
    movl    (%edx),%ebx
    movl    4(%edx),%esi
    movl    8(%edx),%edi
    movl    12(%edx),%ebp
    movl    16(%edx),%esp
    jmpl    *20(%edx)


