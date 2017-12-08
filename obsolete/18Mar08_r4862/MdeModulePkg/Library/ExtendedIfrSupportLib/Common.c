/** @file

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  UefiIfrCommon.c

Abstract:

  Common Library Routines to assist handle HII elements.


**/

#include "LibraryInternal.h"


//
// Hii relative protocols
//

EFI_HII_DATABASE_PROTOCOL *gIfrLibHiiDatabase;
EFI_HII_STRING_PROTOCOL   *gIfrLibHiiString;

EFI_STATUS
EFIAPI
ExtendedIfrSupportLibConstructor (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &gIfrLibHiiDatabase);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **) &gIfrLibHiiString);
  ASSERT_EFI_ERROR (Status);
  
  return EFI_SUCCESS;
}



STATIC EFI_GUID mIfrVendorGuid = EFI_IFR_TIANO_GUID;

EFI_STATUS
EFIAPI
IfrLibExtractClassFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     UINT16              *Class,
  OUT     EFI_STRING_ID       *FormSetTitle,
  OUT     EFI_STRING_ID       *FormSetHelp
  )
/*++

Routine Description:
  Extract formset class for given HII handle.

Arguments:
  HiiHandle       - Hii handle
  Class           - Class of the formset
  FormSetTitle    - Formset title string
  FormSetHelp     - Formset help string

Returns:
  EFI_SUCCESS     - Successfully extract Class for specified Hii handle.

--*/
{
  EFI_STATUS                   Status;
  UINTN                        BufferSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;

  ASSERT (Handle != NULL);
  ASSERT (Class != NULL);  
  ASSERT (FormSetTitle != NULL);
  ASSERT (FormSetHelp != NULL);

  *Class = EFI_NON_DEVICE_CLASS;
  *FormSetTitle = 0;
  *FormSetHelp = 0;

  //
  // Get HII PackageList
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = gIfrLibHiiDatabase->ExportPackageLists (gIfrLibHiiDatabase, Handle, &BufferSize, HiiPackageList);
  ASSERT (Status != EFI_NOT_FOUND);
  
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HiiPackageList = AllocatePool (BufferSize);
    ASSERT (HiiPackageList != NULL);

    Status = gIfrLibHiiDatabase->ExportPackageLists (gIfrLibHiiDatabase, Handle, &BufferSize, HiiPackageList);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Form package from this HII package List
  //
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  Offset2 = 0;
  CopyMem (&PackageListLength, &HiiPackageList->PackageLength, sizeof (UINT32));

  while (Offset < PackageListLength) {
    Package = ((UINT8 *) HiiPackageList) + Offset;
    CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

    if (PackageHeader.Type == EFI_HII_PACKAGE_FORM) {
      //
      // Search Class Opcode in this Form Package
      //
      Offset2 = sizeof (EFI_HII_PACKAGE_HEADER);
      while (Offset2 < PackageHeader.Length) {
        OpCodeData = Package + Offset2;

        if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP) {
          //
          // Find FormSet OpCode
          //
          CopyMem (FormSetTitle, &((EFI_IFR_FORM_SET *) OpCodeData)->FormSetTitle, sizeof (EFI_STRING_ID));
          CopyMem (FormSetHelp, &((EFI_IFR_FORM_SET *) OpCodeData)->Help, sizeof (EFI_STRING_ID));
        }

        if ((((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_GUID_OP) &&
             CompareGuid (&mIfrVendorGuid, (EFI_GUID *)(OpCodeData + sizeof (EFI_IFR_OP_HEADER))) &&
            (((EFI_IFR_GUID_CLASS *) OpCodeData)->ExtendOpCode == EFI_IFR_EXTEND_OP_CLASS)
           ) {
          //
          // Find GUIDed Class OpCode
          //
          CopyMem (Class, &((EFI_IFR_GUID_CLASS *) OpCodeData)->Class, sizeof (UINT16));

          //
          // Till now, we ought to have found the formset Opcode
          //
          break;
        }

        Offset2 += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
      }

      if (Offset2 < PackageHeader.Length) {
        //
        // Target formset found
        //
        break;
      }
    }

    Offset += PackageHeader.Length;
  }

  gBS->FreePool (HiiPackageList);

  return EFI_SUCCESS;
}

