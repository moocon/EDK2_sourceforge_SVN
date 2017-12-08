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
#   WriteMm7.Asm
#
# Abstract:
#
#   AsmWriteMm7 function
#
# Notes:
#
#------------------------------------------------------------------------------



     
     

#------------------------------------------------------------------------------
# UINT64
# EFIAPI
# AsmWriteMm7 (
#   IN UINT64   Value
#   );
#------------------------------------------------------------------------------
.global _AsmWriteMm7
_AsmWriteMm7: 
    movq    4(%esp),%mm7
    ret


