/** @file
Library instance for ExtendedHiiLib.
  
This library instance implements the common HII routines which is
related to HII but reference data structures that are not defined in 
UEFI specification, for example HII_VENDOR_DEVICE_PATH.


Copyright (c) 2006 - 2008, Intel Corporation. <BR>
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Guid\MdeModuleHii.h>


//
// Hii vendor device path template
//
GLOBAL_REMOVE_IF_UNREFERENCED CONST HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      EFI_IFR_TIANO_GUID
    },
    0,
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      END_DEVICE_PATH_LENGTH
    }
  }
};

/**
  The HII driver handle passed in for HiiDatabase.NewPackageList() requires
  that there should be DevicePath Protocol installed on it.
  This routine create a virtual Driver Handle by installing a vendor device
  path on it, so as to use it to invoke HiiDatabase.NewPackageList().
  The Device Path created is a Vendor Device Path specific to Intel's implemenation
  and it is defined as HII_VENDOR_DEVICE_PATH_NODE.
  

  @param  DriverHandle           Handle to be returned

  @retval EFI_SUCCESS            Handle destroy success.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
EFIAPI
HiiLibCreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  )
{
  EFI_STATUS                   Status;
  HII_VENDOR_DEVICE_PATH_NODE  *VendorDevicePath;

  VendorDevicePath = AllocateCopyPool (sizeof (HII_VENDOR_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
  if (VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use memory address as unique ID to distinguish from different device paths
  //
  VendorDevicePath->UniqueId = (UINT64) ((UINTN) VendorDevicePath);

  *DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  VendorDevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/**
  Destroy the Driver Handle created by CreateHiiDriverHandle().

  If no Device Path protocol is installed on the DriverHandle, then ASSERT.
  If this Device Path protocol is failed to be uninstalled, then ASSERT.

  @param  DriverHandle           Handle returned by CreateHiiDriverHandle()


**/
VOID
EFIAPI
HiiLibDestroyHiiDriverHandle (
  IN EFI_HANDLE               DriverHandle
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  DevicePath
                  );
  ASSERT_EFI_ERROR (Status);

  FreePool (DevicePath);

}



