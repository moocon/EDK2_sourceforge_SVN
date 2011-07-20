/** @file
  Global Configuration macros for configuring how LibC is built.

  This file must be included at the beginning of every C file in the
  library, and before any of the Standard C header files are included.

  The configuration, as distributed in this file, is the only configuration
  these libraries have been tested with.  Changing this configuration may
  produce code that will not build or may not run.  Change at your own risk.

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License that accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#define _LIBC         1
#define NLS           1
#define NO_FENV_H     1
#define NO_HEX_FP     1
#define No_Hex_NaN    1
#define ALL_STATE     1
#define USG_COMPAT    1
#define _IEEE_LIBM    1

#undef  WITH_RUNE
