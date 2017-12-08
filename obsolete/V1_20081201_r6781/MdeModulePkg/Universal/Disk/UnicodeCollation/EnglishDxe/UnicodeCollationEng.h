/** @file
  Head file for Unicode Collation Protocol (English)

Copyright (c) 2006 - 2008, Intel Corporation. <BR>
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _UNICODE_COLLATION_ENG_H_
#define _UNICODE_COLLATION_ENG_H_



#include <Uefi.h>


#include <Protocol/UnicodeCollation.h>

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>

//
// Defines
//
#define CHAR_FAT_VALID  0x01

#define TO_UPPER(a)      (CHAR16) (a <= 0xFF ? mEngUpperMap[a] : a)
#define TO_LOWER(a)      (CHAR16) (a <= 0xFF ? mEngLowerMap[a] : a)

//
// Prototypes
//
/**
  Performs a case-insensitive comparison of two Null-terminated Unicode 
  strings.

  @param  This Protocol instance pointer.
  @param  Str1 A pointer to a Null-terminated Unicode string.
  @param  Str2 A pointer to a Null-terminated Unicode string.

  @retval 0   Str1 is equivalent to Str2
  @retval > 0 Str1 is lexically greater than Str2
  @retval < 0 Str1 is lexically less than Str2

**/
INTN
EFIAPI
EngStriColl (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *Str1,
  IN CHAR16                           *Str2
  );

/**
  Performs a case-insensitive comparison of a Null-terminated Unicode 
  pattern string and a Null-terminated Unicode string.

  @param  This    Protocol instance pointer.
  @param  String  A pointer to a Null-terminated Unicode string.
  @param  Pattern A pointer to a Null-terminated Unicode pattern string.

  @retval TRUE    Pattern was found in String.
  @retval FALSE   Pattern was not found in String.

**/
BOOLEAN
EFIAPI
EngMetaiMatch (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *String,
  IN CHAR16                           *Pattern
  );

/**
  Converts all the Unicode characters in a Null-terminated Unicode string to 
  lower case Unicode characters.

  @param  This   Protocol instance pointer.
  @param  Str    A pointer to a Null-terminated Unicode string.

**/
VOID
EFIAPI
EngStrLwr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN OUT CHAR16                       *Str
  );

/**
  Converts all the Unicode characters in a Null-terminated Unicode string to upper
  case Unicode characters.

  @param  This   Protocol instance pointer.
  @param  Str    A pointer to a Null-terminated Unicode string.

**/
VOID
EFIAPI
EngStrUpr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN OUT CHAR16                       *Str
  );

/**
  Converts an 8.3 FAT file name in an OEM character set to a Null-terminated 
  Unicode string.

  @param  This    Protocol instance pointer.
  @param  FatSize The size of the string Fat in bytes.
  @param  Fat     A pointer to a Null-terminated string that contains an 8.3 file
                  name using an OEM character set.
  @param  String  A pointer to a Null-terminated Unicode string. The string must
                  be preallocated to hold FatSize Unicode characters.

**/
VOID
EFIAPI
EngFatToStr (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN UINTN                            FatSize,
  IN CHAR8                            *Fat,
  OUT CHAR16                          *String
  );

/**
  Converts a Null-terminated Unicode string to legal characters in a FAT 
  filename using an OEM character set. 

  @param  This    Protocol instance pointer.
  @param  String  A pointer to a Null-terminated Unicode string. The string must
                  be preallocated to hold FatSize Unicode characters.
  @param  FatSize The size of the string Fat in bytes.
  @param  Fat     A pointer to a Null-terminated string that contains an 8.3 file
                  name using an OEM character set.

  @retval TRUE    Fat is a Long File Name
  @retval FALSE   Fat is an 8.3 file name

**/
BOOLEAN
EFIAPI
EngStrToFat (
  IN EFI_UNICODE_COLLATION_PROTOCOL   *This,
  IN CHAR16                           *String,
  IN UINTN                            FatSize,
  OUT CHAR8                           *Fat
  );

/**
  The user Entry Point for English module.
 
  This function initializes unicode character mapping and then installs Unicode
  Collation & Unicode Collation 2 Protocols based on the feature flags.  

  @param  ImageHandle    The firmware allocated handle for the EFI image.  
  @param  SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeUnicodeCollationEng (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif
