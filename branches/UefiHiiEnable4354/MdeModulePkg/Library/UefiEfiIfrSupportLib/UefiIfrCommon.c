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

#include "UefiIfrLibraryInternal.h"

//
// Hii vendor device path template
//
HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
        (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
      },
      EFI_IFR_TIANO_GUID,
    },
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

//
// Hii relative protocols
//
BOOLEAN  mHiiProtocolsInitialized = FALSE;

EFI_HII_DATABASE_PROTOCOL *gIfrLibHiiDatabase;
EFI_HII_STRING_PROTOCOL   *gIfrLibHiiString;


/**
  This function locate Hii relative protocols for later usage.

  None.

  @return None.

**/
VOID
LocateHiiProtocols (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mHiiProtocolsInitialized) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &gIfrLibHiiDatabase);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, &gIfrLibHiiString);
  ASSERT_EFI_ERROR (Status);

  mHiiProtocolsInitialized = TRUE;
}


/**
  Assemble EFI_HII_PACKAGE_LIST according to the passed in packages.

  @param  NumberOfPackages       Number of packages.
  @param  GuidId                 Package GUID.

  @return Pointer of EFI_HII_PACKAGE_LIST_HEADER.

**/
EFI_HII_PACKAGE_LIST_HEADER *
PreparePackageList (
  IN UINTN                    NumberOfPackages,
  IN EFI_GUID                 *GuidId,
  ...
  )
{
  VA_LIST                     Marker;
  EFI_HII_PACKAGE_LIST_HEADER *PackageListHeader;
  UINT8                       *PackageListData;
  UINT32                      PackageListLength;
  UINT32                      PackageLength;
  EFI_HII_PACKAGE_HEADER      PackageHeader;
  UINT8                       *PackageArray;
  UINTN                       Index;

  PackageListLength = sizeof (EFI_HII_PACKAGE_LIST_HEADER);

  VA_START (Marker, GuidId);
  for (Index = 0; Index < NumberOfPackages; Index++) {
    CopyMem (&PackageLength, VA_ARG (Marker, VOID *), sizeof (UINT32));
    PackageListLength += (PackageLength - sizeof (UINT32));
  }
  VA_END (Marker);

  //
  // Include the lenght of EFI_HII_PACKAGE_END
  //
  PackageListLength += sizeof (EFI_HII_PACKAGE_HEADER);
  PackageListHeader = AllocateZeroPool (PackageListLength);
  ASSERT (PackageListHeader != NULL);
  CopyMem (&PackageListHeader->PackageListGuid, GuidId, sizeof (EFI_GUID));
  PackageListHeader->PackageLength = PackageListLength;

  PackageListData = ((UINT8 *) PackageListHeader) + sizeof (EFI_HII_PACKAGE_LIST_HEADER);

  VA_START (Marker, GuidId);
  for (Index = 0; Index < NumberOfPackages; Index++) {
    PackageArray = (UINT8 *) VA_ARG (Marker, VOID *);
    CopyMem (&PackageLength, PackageArray, sizeof (UINT32));
    PackageLength  -= sizeof (UINT32);
    PackageArray += sizeof (UINT32);
    CopyMem (PackageListData, PackageArray, PackageLength);
    PackageListData += PackageLength;
  }
  VA_END (Marker);

  //
  // Append EFI_HII_PACKAGE_END
  //
  PackageHeader.Type = EFI_HII_PACKAGE_END;
  PackageHeader.Length = sizeof (EFI_HII_PACKAGE_HEADER);
  CopyMem (PackageListData, &PackageHeader, PackageHeader.Length);

  return PackageListHeader;
}


/**
  The HII driver handle passed in for HiiDatabase.NewPackageList() requires
  that there should be DevicePath Protocol installed on it.
  This routine create a virtual Driver Handle by installing a vendor device
  path on it, so as to use it to invoke HiiDatabase.NewPackageList().

  @param  DriverHandle           Handle to be returned

  @retval EFI_SUCCESS            Handle destroy success.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  )
{
  EFI_STATUS                   Status;
  HII_VENDOR_DEVICE_PATH_NODE  *VendorDevicePath;
  UINT64                       MonotonicCount;

  VendorDevicePath = AllocateCopyPool (sizeof (HII_VENDOR_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
  if (VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  gBS->GetNextMonotonicCount (&MonotonicCount);
  VendorDevicePath->MonotonicCount = (UINT32) MonotonicCount;

  *DriverHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  VendorDevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Destroy the Driver Handle created by CreateHiiDriverHandle().

  @param  DriverHandle           Handle returned by CreateHiiDriverHandle()

  @retval EFI_SUCCESS            Handle destroy success.
  @retval other                  Handle destroy fail.

**/
EFI_STATUS
DestroyHiiDriverHandle (
  IN EFI_HANDLE                 DriverHandle
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  DevicePath
                  );

  return Status;
}


/**
  Find HII Handle associated with given Device Path.

  @param  HiiDatabase            Point to EFI_HII_DATABASE_PROTOCOL instance.
  @param  DevicePath             Device Path associated with the HII package list
                                 handle.

  @retval Handle                 HII package list Handle associated with the Device
                                 Path.
  @retval NULL                   Hii Package list handle is not found.

**/
EFI_HII_HANDLE
DevicePathToHiiHandle (
  IN EFI_HII_DATABASE_PROTOCOL  *HiiDatabase,
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *TmpDevicePath;
  UINTN                       BufferSize;
  UINTN                       HandleCount;
  UINTN                       Index;
  EFI_HANDLE                  *Handles;
  EFI_HANDLE                  Handle;
  UINTN                       Size;
  EFI_HANDLE                  DriverHandle;
  EFI_HII_HANDLE              *HiiHandles;
  EFI_HII_HANDLE              HiiHandle;

  //
  // Locate Device Path Protocol handle buffer
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // Search Driver Handle by Device Path
  //
  DriverHandle = NULL;
  BufferSize = GetDevicePathSize (DevicePath);
  for(Index = 0; Index < HandleCount; Index++) {
    Handle = Handles[Index];
    gBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, &TmpDevicePath);

    //
    // Check whether DevicePath match
    //
    Size = GetDevicePathSize (TmpDevicePath);
    if ((Size == BufferSize) && CompareMem (DevicePath, TmpDevicePath, Size) == 0) {
      DriverHandle = Handle;
      break;
    }
  }
  gBS->FreePool (Handles);

  if (DriverHandle == NULL) {
    return NULL;
  }

  //
  // Retrieve all Hii Handles from HII database
  //
  BufferSize = 0x1000;
  HiiHandles = AllocatePool (BufferSize);
  ASSERT (HiiHandles != NULL);
  Status = HiiDatabase->ListPackageLists (
                          HiiDatabase,
                          EFI_HII_PACKAGE_TYPE_ALL,
                          NULL,
                          &BufferSize,
                          HiiHandles
                          );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->FreePool (HiiHandles);
    HiiHandles = AllocatePool (BufferSize);
    ASSERT (HiiHandles != NULL);

    Status = HiiDatabase->ListPackageLists (
                            HiiDatabase,
                            EFI_HII_PACKAGE_TYPE_ALL,
                            NULL,
                            &BufferSize,
                            HiiHandles
                            );
  }

  if (EFI_ERROR (Status)) {
    gBS->FreePool (HiiHandles);
    return NULL;
  }

  //
  // Search Hii Handle by Driver Handle
  //
  HiiHandle = NULL;
  HandleCount = BufferSize / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = HiiDatabase->GetPackageListHandle (
                            HiiDatabase,
                            HiiHandles[Index],
                            &Handle
                            );
    if (!EFI_ERROR (Status) && (Handle == DriverHandle)) {
      HiiHandle = HiiHandles[Index];
      break;
    }
  }

  gBS->FreePool (HiiHandles);
  return HiiHandle;
}


/**
  Determines the handles that are currently active in the database.
  It's the caller's responsibility to free handle buffer.

  @param  HiiDatabase            A pointer to the EFI_HII_DATABASE_PROTOCOL
                                 instance.
  @param  HandleBufferLength     On input, a pointer to the length of the handle
                                 buffer. On output, the length of the handle buffer
                                 that is required for the handles found.
  @param  HiiHandleBuffer        Pointer to an array of Hii Handles returned.

  @retval EFI_SUCCESS            Get an array of Hii Handles successfully.
  @retval EFI_INVALID_PARAMETER  Hii is NULL.
  @retval EFI_NOT_FOUND          Database not found.

**/
EFI_STATUS
GetHiiHandles (
  IN OUT UINTN                     *HandleBufferLength,
  OUT    EFI_HII_HANDLE            **HiiHandleBuffer
  )
{
  UINTN       BufferLength;
  EFI_STATUS  Status;

  BufferLength = 0;

  LocateHiiProtocols ();

  //
  // Try to find the actual buffer size for HiiHandle Buffer.
  //
  Status = gIfrLibHiiDatabase->ListPackageLists (
                                 gIfrLibHiiDatabase,
                                 EFI_HII_PACKAGE_TYPE_ALL,
                                 NULL,
                                 &BufferLength,
                                 *HiiHandleBuffer
                                 );

  if (Status == EFI_BUFFER_TOO_SMALL) {
      *HiiHandleBuffer = AllocateZeroPool (BufferLength);
      Status = gIfrLibHiiDatabase->ListPackageLists (
                                     gIfrLibHiiDatabase,
                                     EFI_HII_PACKAGE_TYPE_ALL,
                                     NULL,
                                     &BufferLength,
                                     *HiiHandleBuffer
                                     );
      //
      // we should not fail here.
      //
      ASSERT_EFI_ERROR (Status);
  }

  *HandleBufferLength = BufferLength;

  return Status;
}


/**
  Extract Hii package list GUID for given HII handle.

  @param  HiiHandle              Hii handle
  @param  Guid                   Package list GUID

  @retval EFI_SUCCESS            Successfully extract GUID from Hii database.

**/
EFI_STATUS
ExtractGuidFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     EFI_GUID            *Guid
  )
{
  EFI_STATUS                   Status;
  UINTN                        BufferSize;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;

  //
  // Locate HII Database protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  &HiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get HII PackageList
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HiiPackageList = AllocatePool (BufferSize);
    ASSERT (HiiPackageList != NULL);

    Status = HiiDatabase->ExportPackageLists (HiiDatabase, Handle, &BufferSize, HiiPackageList);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Extract GUID
  //
  CopyMem (Guid, &HiiPackageList->PackageListGuid, sizeof (EFI_GUID));

  gBS->FreePool (HiiPackageList);

  return EFI_SUCCESS;
}


/**
  Extract formset class for given HII handle.

  @param  HiiHandle              Hii handle
  @param  Class                  Class of the formset
  @param  FormSetTitle           Formset title string
  @param  FormSetHelp            Formset help string

  @retval EFI_SUCCESS            Successfully extract Class for specified Hii
                                 handle.

**/
EFI_STATUS
ExtractClassFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle,
  OUT     UINT16              *Class,
  OUT     EFI_STRING_ID       *FormSetTitle,
  OUT     EFI_STRING_ID       *FormSetHelp
  )
{
  EFI_STATUS                   Status;
  UINTN                        BufferSize;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  UINT8                        *FormSet;
  UINT8                        *OpCodeData;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;

  *Class = EFI_NON_DEVICE_CLASS;
  *FormSetTitle = 0;
  *FormSetHelp = 0;

  //
  // Locate HII Database protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  &HiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get HII PackageList
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HiiPackageList = AllocatePool (BufferSize);
    ASSERT (HiiPackageList != NULL);

    Status = HiiDatabase->ExportPackageLists (HiiDatabase, Handle, &BufferSize, HiiPackageList);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Form package from this HII package List
  //
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  Offset2 = 0;
  FormSet = NULL;
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
            (CompareGuid (&mIfrVendorGuid, &((EFI_IFR_GUID *) OpCodeData)->Guid)) &&
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
