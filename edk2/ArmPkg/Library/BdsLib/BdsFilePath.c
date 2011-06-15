/** @file
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*  
*  This program and the accompanying materials                          
*  are licensed and made available under the terms and conditions of the BSD License         
*  which accompanies this distribution.  The full text of the license may be found at        
*  http://opensource.org/licenses/bsd-license.php                                            
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             
*
**/

#include "BdsInternal.h"

#include <Protocol/UsbIo.h>
#include <Protocol/DiskIo.h>

#define IS_DEVICE_PATH_NODE(node,type,subtype) (((node)->Type == (type)) && ((node)->SubType == (subtype)))

// Extract the FilePath from the Device Path
CHAR16*
BdsExtractFilePathFromDevicePath (
  IN  CONST CHAR16    *StrDevicePath,
  IN  UINTN           NumberDevicePathNode
  )
{
  UINTN       Node;
  CHAR16      *Str;

  Str = (CHAR16*)StrDevicePath;
  Node = 0;
  while ((Str != NULL) && (*Str != L'\0') && (Node < NumberDevicePathNode)) {
    if ((*Str == L'/') || (*Str == L'\\')) {
        Node++;
    }
    Str++;
  }

  if (*Str == L'\0') {
    return NULL;
  } else {
    return Str;
  }
}

BOOLEAN
BdsIsRemovableUsb (
  IN  EFI_DEVICE_PATH*  DevicePath
  )
{
  return ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
          ((DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP) ||
           (DevicePathSubType (DevicePath) == MSG_USB_WWID_DP)));
}

EFI_STATUS
BdsGetDeviceUsb (
  IN  EFI_DEVICE_PATH*  RemovableDevicePath,
  OUT EFI_HANDLE*       DeviceHandle,
  OUT EFI_DEVICE_PATH** NewDevicePath
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         UsbIoHandleCount;
  EFI_HANDLE                    *UsbIoBuffer;
  EFI_DEVICE_PATH*              UsbIoDevicePath;
  EFI_DEVICE_PATH*              TmpDevicePath;
  USB_WWID_DEVICE_PATH*         WwidDevicePath1;
  USB_WWID_DEVICE_PATH*         WwidDevicePath2;
  USB_CLASS_DEVICE_PATH*        UsbClassDevicePath1;
  USB_CLASS_DEVICE_PATH*        UsbClassDevicePath2;

  // Get all the UsbIo handles
  UsbIoHandleCount = 0;
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiUsbIoProtocolGuid, NULL, &UsbIoHandleCount, &UsbIoBuffer);
  if (EFI_ERROR(Status) || (UsbIoHandleCount == 0)) {
    return Status;
  }

  // Check if one of the handles matches the USB description
  for (Index = 0; Index < UsbIoHandleCount; Index++) {
    Status = gBS->HandleProtocol (UsbIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &UsbIoDevicePath);
    if (!EFI_ERROR(Status)) {
      TmpDevicePath = UsbIoDevicePath;
      while (!IsDevicePathEnd (TmpDevicePath)) {
        // Check if the Device Path node is a USB Removable device Path node
        if (BdsIsRemovableUsb (TmpDevicePath)) {
          if (TmpDevicePath->SubType == MSG_USB_WWID_DP) {
            WwidDevicePath1 = (USB_WWID_DEVICE_PATH*)RemovableDevicePath;
            WwidDevicePath2 = (USB_WWID_DEVICE_PATH*)TmpDevicePath;
            if ((WwidDevicePath1->VendorId == WwidDevicePath2->VendorId) &&
                (WwidDevicePath1->ProductId == WwidDevicePath2->ProductId) &&
                (CompareMem (WwidDevicePath1+1, WwidDevicePath2+1, DevicePathNodeLength(WwidDevicePath1)-sizeof(USB_WWID_DEVICE_PATH)) == 0))
            {
              *DeviceHandle = UsbIoBuffer[Index];
              // Add the additional original Device Path Nodes (eg: FilePath Device Path Node) to the new Device Path
              *NewDevicePath = AppendDevicePath (UsbIoDevicePath, NextDevicePathNode(RemovableDevicePath));
              return EFI_SUCCESS;
            }
          } else {
            UsbClassDevicePath1 = (USB_CLASS_DEVICE_PATH*)RemovableDevicePath;
            UsbClassDevicePath2 = (USB_CLASS_DEVICE_PATH*)TmpDevicePath;
            if ((UsbClassDevicePath1->VendorId != 0xFFFF) && (UsbClassDevicePath1->VendorId == UsbClassDevicePath2->VendorId) &&
                (UsbClassDevicePath1->ProductId != 0xFFFF) && (UsbClassDevicePath1->ProductId == UsbClassDevicePath2->ProductId) &&
                (UsbClassDevicePath1->DeviceClass != 0xFF) && (UsbClassDevicePath1->DeviceClass == UsbClassDevicePath2->DeviceClass) &&
                (UsbClassDevicePath1->DeviceSubClass != 0xFF) && (UsbClassDevicePath1->DeviceSubClass == UsbClassDevicePath2->DeviceSubClass) &&
                (UsbClassDevicePath1->DeviceProtocol != 0xFF) && (UsbClassDevicePath1->DeviceProtocol == UsbClassDevicePath2->DeviceProtocol))
            {
              *DeviceHandle = UsbIoBuffer[Index];
              // Add the additional original Device Path Nodes (eg: FilePath Device Path Node) to the new Device Path
              *NewDevicePath = AppendDevicePath (UsbIoDevicePath, NextDevicePathNode(RemovableDevicePath));
              return EFI_SUCCESS;
            }
          }
        }
        TmpDevicePath = NextDevicePathNode (TmpDevicePath);
      }

    }
  }

  return EFI_NOT_FOUND;
}

BOOLEAN
BdsIsRemovableHd (
  IN  EFI_DEVICE_PATH*  DevicePath
  )
{
  return IS_DEVICE_PATH_NODE(DevicePath, MEDIA_DEVICE_PATH, MEDIA_HARDDRIVE_DP);
}

EFI_STATUS
BdsGetDeviceHd (
  IN  EFI_DEVICE_PATH*  RemovableDevicePath,
  OUT EFI_HANDLE*       DeviceHandle,
  OUT EFI_DEVICE_PATH** NewDevicePath
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         PartitionHandleCount;
  EFI_HANDLE                    *PartitionBuffer;
  EFI_DEVICE_PATH*              PartitionDevicePath;
  EFI_DEVICE_PATH*              TmpDevicePath;
  HARDDRIVE_DEVICE_PATH*        HardDriveDevicePath1;
  HARDDRIVE_DEVICE_PATH*        HardDriveDevicePath2;

  // Get all the DiskIo handles
  PartitionHandleCount = 0;
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiDiskIoProtocolGuid, NULL, &PartitionHandleCount, &PartitionBuffer);
  if (EFI_ERROR(Status) || (PartitionHandleCount == 0)) {
    return Status;
  }

  // Check if one of the handles matches the Hard Disk Description
  for (Index = 0; Index < PartitionHandleCount; Index++) {
    Status = gBS->HandleProtocol (PartitionBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &PartitionDevicePath);
    if (!EFI_ERROR(Status)) {
      TmpDevicePath = PartitionDevicePath;
      while (!IsDevicePathEnd (TmpDevicePath)) {
        // Check if the Device Path node is a HD Removable device Path node
        if (BdsIsRemovableHd (TmpDevicePath)) {
          HardDriveDevicePath1 = (HARDDRIVE_DEVICE_PATH*)RemovableDevicePath;
          HardDriveDevicePath2 = (HARDDRIVE_DEVICE_PATH*)TmpDevicePath;
          if ((HardDriveDevicePath1->SignatureType == HardDriveDevicePath2->SignatureType) &&
              (CompareGuid ((EFI_GUID *)HardDriveDevicePath1->Signature,(EFI_GUID *)HardDriveDevicePath2->Signature) == TRUE) &&
              (HardDriveDevicePath1->PartitionNumber == HardDriveDevicePath2->PartitionNumber))
          {
            *DeviceHandle = PartitionBuffer[Index];
            // Add the additional original Device Path Nodes (eg: FilePath Device Path Node) to the new Device Path
            *NewDevicePath = AppendDevicePath (PartitionDevicePath, NextDevicePathNode(RemovableDevicePath));
            return EFI_SUCCESS;
          }
        }
        TmpDevicePath = NextDevicePathNode (TmpDevicePath);
      }

    }
  }

  return EFI_NOT_FOUND;
}

/*BOOLEAN
BdsIsRemovableCdrom (
  IN  EFI_DEVICE_PATH*  DevicePath
  )
{
  return IS_DEVICE_PATH_NODE(DevicePath, MEDIA_DEVICE_PATH, MEDIA_CDROM_DP);
}

EFI_STATUS
BdsGetDeviceCdrom (
  IN  EFI_DEVICE_PATH*  RemovableDevicePath,
  OUT EFI_HANDLE*       DeviceHandle,
  OUT EFI_DEVICE_PATH** DevicePath
  )
{
  ASSERT(0);
  return EFI_UNSUPPORTED;
}*/

typedef BOOLEAN
(*BDS_IS_REMOVABLE) (
  IN  EFI_DEVICE_PATH*  DevicePath
  );

typedef EFI_STATUS
(*BDS_GET_DEVICE) (
  IN  EFI_DEVICE_PATH*  RemovableDevicePath,
  OUT EFI_HANDLE*       DeviceHandle,
  OUT EFI_DEVICE_PATH** DevicePath
  );

typedef struct {
  BDS_IS_REMOVABLE    IsRemovable;
  BDS_GET_DEVICE      GetDevice;
} BDS_REMOVABLE_DEVICE_SUPPORT;

BDS_REMOVABLE_DEVICE_SUPPORT  RemovableDeviceSupport[] = {
  { BdsIsRemovableUsb, BdsGetDeviceUsb },
  { BdsIsRemovableHd, BdsGetDeviceHd },
  //{ BdsIsRemovableCdrom, BdsGetDeviceCdrom }
};

STATIC
BOOLEAN
IsRemovableDevice (
  IN  EFI_DEVICE_PATH*  DevicePath
  )
{
  UINTN             Index;
  EFI_DEVICE_PATH*  TmpDevicePath;

  TmpDevicePath = DevicePath;
  while (!IsDevicePathEnd (TmpDevicePath)) {
    for (Index = 0; Index < sizeof(RemovableDeviceSupport) / sizeof(BDS_REMOVABLE_DEVICE_SUPPORT); Index++) {
      if (RemovableDeviceSupport[Index].IsRemovable(TmpDevicePath)) {
        return TRUE;
      }
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
  }

  return FALSE;
}

STATIC
EFI_STATUS
TryRemovableDevice (
  IN  EFI_DEVICE_PATH*  DevicePath,
  OUT EFI_HANDLE*       DeviceHandle,
  OUT EFI_DEVICE_PATH** NewDevicePath
  )
{
  EFI_STATUS        Status;
  UINTN             Index;
  EFI_DEVICE_PATH*  TmpDevicePath;
  BDS_REMOVABLE_DEVICE_SUPPORT* RemovableDevice;
  EFI_DEVICE_PATH* RemovableDevicePath;
  BOOLEAN         RemovableFound;

  RemovableDevice     = NULL;
  RemovableDevicePath = NULL;
  RemovableFound      = FALSE;
  TmpDevicePath       = DevicePath;

  while (!IsDevicePathEnd (TmpDevicePath) && !RemovableFound) {
    for (Index = 0; Index < sizeof(RemovableDeviceSupport) / sizeof(BDS_REMOVABLE_DEVICE_SUPPORT); Index++) {
      RemovableDevice = &RemovableDeviceSupport[Index];
      if (RemovableDevice->IsRemovable(TmpDevicePath)) {
        RemovableDevicePath = TmpDevicePath;
        RemovableFound = TRUE;
        break;
      }
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
  }

  if (!RemovableFound) {
    return EFI_NOT_FOUND;
  }

  // Search into the current started drivers
  Status = RemovableDevice->GetDevice (RemovableDevicePath, DeviceHandle, NewDevicePath);
  if (Status == EFI_NOT_FOUND) {
    // Connect all the drivers
    BdsConnectAllDrivers ();

    // Search again into all the drivers
    Status = RemovableDevice->GetDevice (RemovableDevicePath, DeviceHandle, NewDevicePath);
  }

  return Status;
}

EFI_STATUS
BdsConnectDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL* DevicePath,
  OUT EFI_HANDLE                *Handle,
  OUT EFI_DEVICE_PATH_PROTOCOL  **RemainingDevicePath
  )
{
  EFI_DEVICE_PATH*            Remaining;
  EFI_DEVICE_PATH*            NewDevicePath;
  EFI_STATUS                  Status;

  if ((DevicePath == NULL) || (Handle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  do {
    Remaining = DevicePath;
    // The LocateDevicePath() function locates all devices on DevicePath that support Protocol and returns
    // the handle to the device that is closest to DevicePath. On output, the device path pointer is modified
    // to point to the remaining part of the device path
    Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &Remaining, Handle);
    if (!EFI_ERROR (Status)) {
      // Recursive = FALSE: We do not want to start all the device tree
      Status = gBS->ConnectController (*Handle, NULL, Remaining, FALSE);
    }

    /*// We need to check if RemainingDevicePath does not point on the last node. Otherwise, calling
    // NextDevicePathNode() will return an undetermined Device Path Node
    if (!IsDevicePathEnd (RemainingDevicePath)) {
      RemainingDevicePath = NextDevicePathNode (RemainingDevicePath);
    }*/
  } while (!EFI_ERROR (Status) && !IsDevicePathEnd (Remaining));

  if (!EFI_ERROR (Status)) {
    // Now, we have got the whole Device Path connected, call again ConnectController to ensure all the supported Driver
    // Binding Protocol are connected (such as DiskIo and SimpleFileSystem)
    Remaining = DevicePath;
    Status = gBS->LocateDevicePath(&gEfiDevicePathProtocolGuid,&Remaining,Handle);
    if (!EFI_ERROR (Status)) {
      Status = gBS->ConnectController (*Handle, NULL, Remaining, FALSE);
      if (EFI_ERROR (Status)) {
        // If the last node is a Memory Map Device Path just return EFI_SUCCESS.
        if ((Remaining->Type == HARDWARE_DEVICE_PATH) && (Remaining->SubType == HW_MEMMAP_DP)) {
            Status = EFI_SUCCESS;
        }
      }
    }
  } else if (!IsDevicePathEnd (Remaining) && !IsRemovableDevice (Remaining)) {

    /*// If the remaining Device Path is a FilePath or MemoryMap then we consider the Device Path has been loaded correctly
    if ((Remaining->Type == MEDIA_DEVICE_PATH) && (Remaining->SubType == MEDIA_FILEPATH_DP)) {
      Status = EFI_SUCCESS;
    } else if ((Remaining->Type == HARDWARE_DEVICE_PATH) && (Remaining->SubType == HW_MEMMAP_DP)) {
      Status = EFI_SUCCESS;
    }*/

    //TODO: Should we just return success and leave the caller decide if it is the expected RemainingPath
    Status = EFI_SUCCESS;
  } else {
    Status = TryRemovableDevice (DevicePath, Handle, &NewDevicePath);
    if (!EFI_ERROR (Status)) {
      return BdsConnectDevicePath (NewDevicePath, Handle, RemainingDevicePath);
    }
  }

  if (RemainingDevicePath) {
    *RemainingDevicePath = Remaining;
  }

  return Status;
}

BOOLEAN
BdsFileSystemSupport (
  IN EFI_DEVICE_PATH *DevicePath,
  IN EFI_HANDLE Handle,
  IN EFI_DEVICE_PATH *RemainingDevicePath
  )
{
  EFI_STATUS  Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *FsProtocol;

  Status = gBS->HandleProtocol (Handle,&gEfiSimpleFileSystemProtocolGuid, (VOID **)&FsProtocol);

  return (!EFI_ERROR(Status) && IS_DEVICE_PATH_NODE(RemainingDevicePath,MEDIA_DEVICE_PATH,MEDIA_FILEPATH_DP));
}

EFI_STATUS
BdsFileSystemLoadImage (
  IN     EFI_DEVICE_PATH *DevicePath,
  IN     EFI_HANDLE Handle,
  IN     EFI_DEVICE_PATH *RemainingDevicePath,
  IN     EFI_ALLOCATE_TYPE     Type,
  IN OUT EFI_PHYSICAL_ADDRESS* Image,
  OUT    UINTN                 *ImageSize
  )
{
  FILEPATH_DEVICE_PATH*             FilePathDevicePath;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL     *FsProtocol;
  EFI_FILE_PROTOCOL                   *Fs;
  EFI_STATUS Status;
  EFI_FILE_INFO       *FileInfo;
  EFI_FILE_PROTOCOL   *File;
  UINTN               Size;

  ASSERT (IS_DEVICE_PATH_NODE(RemainingDevicePath,MEDIA_DEVICE_PATH,MEDIA_FILEPATH_DP));

  FilePathDevicePath = (FILEPATH_DEVICE_PATH*)RemainingDevicePath;

  Status = gBS->HandleProtocol(Handle,&gEfiSimpleFileSystemProtocolGuid, (VOID **)&FsProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //Try to Open the volume and get root directory
  Status = FsProtocol->OpenVolume (FsProtocol, &Fs);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  File = NULL;
  Status = Fs->Open(Fs, &File, FilePathDevicePath->PathName, EFI_FILE_MODE_READ, 0);

  Size = 0;
  File->GetInfo(File, &gEfiFileInfoGuid, &Size, NULL);
  FileInfo = AllocatePool (Size);
  Status = File->GetInfo(File, &gEfiFileInfoGuid, &Size, FileInfo);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Get the file size
  Size = FileInfo->FileSize;
  if (ImageSize) {
    *ImageSize = Size;
  }
  FreePool(FileInfo);

  Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(Size),Image);
  if (!EFI_ERROR(Status)) {
    Status = File->Read (File, &Size, (VOID*)(UINTN)(*Image));
  }

  return Status;
}

BOOLEAN
BdsMemoryMapSupport (
  IN EFI_DEVICE_PATH *DevicePath,
  IN EFI_HANDLE Handle,
  IN EFI_DEVICE_PATH *RemainingDevicePath
  )
{
  return IS_DEVICE_PATH_NODE(RemainingDevicePath,HARDWARE_DEVICE_PATH,HW_MEMMAP_DP);
}

EFI_STATUS
BdsMemoryMapLoadImage (
  IN     EFI_DEVICE_PATH *DevicePath,
  IN     EFI_HANDLE Handle,
  IN     EFI_DEVICE_PATH *RemainingDevicePath,
  IN     EFI_ALLOCATE_TYPE     Type,
  IN OUT EFI_PHYSICAL_ADDRESS* Image,
  OUT    UINTN                 *ImageSize
  )
{
  EFI_STATUS            Status;
  MEMMAP_DEVICE_PATH*   MemMapPathDevicePath;
  UINTN                 Size;

  ASSERT (IS_DEVICE_PATH_NODE(RemainingDevicePath,HARDWARE_DEVICE_PATH,HW_MEMMAP_DP));

  MemMapPathDevicePath = (MEMMAP_DEVICE_PATH*)RemainingDevicePath;

  Size = MemMapPathDevicePath->EndingAddress - MemMapPathDevicePath->StartingAddress;
  if (Size == 0) {
      return EFI_INVALID_PARAMETER;
  }

  Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(Size),Image);
  if (!EFI_ERROR(Status)) {
    CopyMem ((VOID*)(UINTN)(*Image), (CONST VOID*)(UINTN)MemMapPathDevicePath->StartingAddress, Size);

    if (ImageSize != NULL) {
        *ImageSize = Size;
    }
  }

  return Status;
}

BOOLEAN
BdsFirmwareVolumeSupport (
  IN EFI_DEVICE_PATH *DevicePath,
  IN EFI_HANDLE Handle,
  IN EFI_DEVICE_PATH *RemainingDevicePath
  )
{
  return IS_DEVICE_PATH_NODE(RemainingDevicePath, MEDIA_DEVICE_PATH, MEDIA_PIWG_FW_FILE_DP);
}

EFI_STATUS
BdsFirmwareVolumeLoadImage (
  IN     EFI_DEVICE_PATH *DevicePath,
  IN     EFI_HANDLE Handle,
  IN     EFI_DEVICE_PATH *RemainingDevicePath,
  IN     EFI_ALLOCATE_TYPE     Type,
  IN OUT EFI_PHYSICAL_ADDRESS* Image,
  OUT    UINTN                 *ImageSize
  )
{
  EFI_STATUS            Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL     *FwVol;
  EFI_GUID                          *FvNameGuid;
  EFI_SECTION_TYPE                  SectionType;
  EFI_FV_FILETYPE                   FvType;
  EFI_FV_FILE_ATTRIBUTES            Attrib;
  UINT32                            AuthenticationStatus;
  VOID* ImageBuffer;

  ASSERT (IS_DEVICE_PATH_NODE(RemainingDevicePath, MEDIA_DEVICE_PATH, MEDIA_PIWG_FW_FILE_DP));

  Status = gBS->HandleProtocol(Handle,&gEfiFirmwareVolume2ProtocolGuid, (VOID **)&FwVol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  FvNameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)RemainingDevicePath);
  if (FvNameGuid == NULL) {
    Status = EFI_INVALID_PARAMETER;
  }

  SectionType = EFI_SECTION_PE32;
  AuthenticationStatus = 0;
  //Note: ReadSection at the opposite of ReadFile does not allow to pass ImageBuffer == NULL to get the size of the file.
  ImageBuffer = NULL;
  Status = FwVol->ReadSection (
                    FwVol,
                    FvNameGuid,
                    SectionType,
                    0,
                    &ImageBuffer,
                    ImageSize,
                    &AuthenticationStatus
                    );
  if (!EFI_ERROR (Status)) {
#if 0
    // In case the buffer has some address requirements, we must copy the buffer to a buffer following the requirements
    if (Type != AllocateAnyPages) {
      Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(*ImageSize),Image);
      if (!EFI_ERROR(Status)) {
        CopyMem ((VOID*)(UINTN)(*Image), ImageBuffer, *ImageSize);
        FreePool (ImageBuffer);
      }
    }
#else
    // We must copy the buffer into a page allocations. Otherwise, the caller could call gBS->FreePages() on the pool allocation
    Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(*ImageSize), Image);
    if (!EFI_ERROR(Status)) {
      CopyMem ((VOID*)(UINTN)(*Image), ImageBuffer, *ImageSize);
      FreePool (ImageBuffer);
    }
#endif
  } else {
    // Try a raw file, since a PE32 SECTION does not exist
    Status = FwVol->ReadFile (
                        FwVol,
                        FvNameGuid,
                        NULL,
                        ImageSize,
                        &FvType,
                        &Attrib,
                        &AuthenticationStatus
                        );
    if (!EFI_ERROR(Status)) {
      Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(*ImageSize),Image);
      if (!EFI_ERROR(Status)) {
        Status = FwVol->ReadFile (
                                FwVol,
                                FvNameGuid,
                                (VOID*)(UINTN)(*Image),
                                ImageSize,
                                &FvType,
                                &Attrib,
                                &AuthenticationStatus
                                );
      }
    }
  }
  return Status;
}

BOOLEAN
BdsPxeSupport (
  IN EFI_DEVICE_PATH*           DevicePath,
  IN EFI_HANDLE                 Handle,
  IN EFI_DEVICE_PATH*           RemainingDevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_PXE_BASE_CODE_PROTOCOL* PxeBcProtocol;

  if (!IsDevicePathEnd(RemainingDevicePath)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiPxeBaseCodeProtocolGuid, (VOID **)&PxeBcProtocol);
  if (EFI_ERROR (Status)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

EFI_STATUS
BdsPxeLoadImage (
  IN     EFI_DEVICE_PATH*       DevicePath,
  IN     EFI_HANDLE             Handle,
  IN     EFI_DEVICE_PATH*       RemainingDevicePath,
  IN     EFI_ALLOCATE_TYPE      Type,
  IN OUT EFI_PHYSICAL_ADDRESS   *Image,
  OUT    UINTN                  *ImageSize
  )
{
  EFI_STATUS              Status;
  EFI_LOAD_FILE_PROTOCOL  *LoadFileProtocol;
  UINTN                   BufferSize;

  // Get Load File Protocol attached to the PXE protocol
  Status = gBS->HandleProtocol (Handle, &gEfiLoadFileProtocolGuid, (VOID **)&LoadFileProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LoadFileProtocol->LoadFile (LoadFileProtocol, DevicePath, TRUE, &BufferSize, NULL);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gBS->AllocatePages (Type, EfiBootServicesCode, EFI_SIZE_TO_PAGES(BufferSize), Image);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = LoadFileProtocol->LoadFile (LoadFileProtocol, DevicePath, TRUE, &BufferSize, (VOID*)(UINTN)(*Image));
    if (!EFI_ERROR(Status) && (ImageSize != NULL)) {
      *ImageSize = BufferSize;
    }
  }

  return Status;
}

BOOLEAN
BdsTftpSupport (
  IN EFI_DEVICE_PATH*           DevicePath,
  IN EFI_HANDLE                 Handle,
  IN EFI_DEVICE_PATH*           RemainingDevicePath
  )
{
  EFI_STATUS  Status;
  EFI_DEVICE_PATH  *NextDevicePath;
  EFI_PXE_BASE_CODE_PROTOCOL  *PxeBcProtocol;

  // Validate the Remaining Device Path
  if (IsDevicePathEnd(RemainingDevicePath)) {
    return FALSE;
  }
  if (!IS_DEVICE_PATH_NODE(RemainingDevicePath,MESSAGING_DEVICE_PATH,MSG_IPv4_DP) &&
      !IS_DEVICE_PATH_NODE(RemainingDevicePath,MESSAGING_DEVICE_PATH,MSG_IPv6_DP)) {
    return FALSE;
  }
  NextDevicePath = NextDevicePathNode (RemainingDevicePath);
  if (IsDevicePathEnd(NextDevicePath)) {
    return FALSE;
  }
  if (!IS_DEVICE_PATH_NODE(NextDevicePath,MEDIA_DEVICE_PATH,MEDIA_FILEPATH_DP)) {
    return FALSE;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiPxeBaseCodeProtocolGuid, (VOID **)&PxeBcProtocol);
  if (EFI_ERROR (Status)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

EFI_STATUS
BdsTftpLoadImage (
  IN     EFI_DEVICE_PATH*       DevicePath,
  IN     EFI_HANDLE             Handle,
  IN     EFI_DEVICE_PATH*       RemainingDevicePath,
  IN     EFI_ALLOCATE_TYPE      Type,
  IN OUT EFI_PHYSICAL_ADDRESS   *Image,
  OUT    UINTN                  *ImageSize
  )
{
  EFI_STATUS                  Status;
  EFI_PXE_BASE_CODE_PROTOCOL  *Pxe;
  UINT64                      TftpBufferSize;
  VOID*                       TftpBuffer;
  EFI_IP_ADDRESS              ServerIp;
  IPv4_DEVICE_PATH*           IPv4DevicePathNode;
  FILEPATH_DEVICE_PATH*       FilePathDevicePath;
  EFI_IP_ADDRESS              LocalIp;

  ASSERT(IS_DEVICE_PATH_NODE(RemainingDevicePath,MESSAGING_DEVICE_PATH,MSG_IPv4_DP));

  IPv4DevicePathNode = (IPv4_DEVICE_PATH*)RemainingDevicePath;
  FilePathDevicePath = (FILEPATH_DEVICE_PATH*)(IPv4DevicePathNode + 1);

  Status = gBS->LocateProtocol (&gEfiPxeBaseCodeProtocolGuid, NULL, (VOID **)&Pxe);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = Pxe->Start (Pxe, FALSE);
  if (EFI_ERROR(Status) && (Status != EFI_ALREADY_STARTED)) {
    return Status;
  }

  if (!IPv4DevicePathNode->StaticIpAddress) {
    Status = Pxe->Dhcp(Pxe, TRUE);
  } else {
    CopyMem (&LocalIp.v4, &IPv4DevicePathNode->LocalIpAddress, sizeof (EFI_IPv4_ADDRESS));
    Status = Pxe->SetStationIp (Pxe, &LocalIp, NULL);
  }
  if (EFI_ERROR(Status)) {
    return Status;
  }

  CopyMem (&ServerIp.v4, &IPv4DevicePathNode->RemoteIpAddress, sizeof (EFI_IPv4_ADDRESS));

  Status = Pxe->Mtftp (
                  Pxe,
                  EFI_PXE_BASE_CODE_TFTP_GET_FILE_SIZE,
                  NULL,
                  FALSE,
                  &TftpBufferSize,
                  NULL,
                  &ServerIp,
                  (UINT8 *)FilePathDevicePath->PathName,
                  NULL,
                  TRUE
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Allocate a buffer to hold the whole file.
  TftpBuffer = AllocatePool(TftpBufferSize);
  if (TftpBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = Pxe->Mtftp (
                  Pxe,
                  EFI_PXE_BASE_CODE_TFTP_READ_FILE,
                  TftpBuffer,
                  FALSE,
                  &TftpBufferSize,
                  NULL,
                  &ServerIp,
                  (UINT8 *)FilePathDevicePath->PathName,
                  NULL,
                  FALSE
                  );
  if (EFI_ERROR(Status)) {
    FreePool(TftpBuffer);
  } else if (ImageSize != NULL) {
    *ImageSize = (UINTN)TftpBufferSize;
  }

  return Status;
}

BDS_FILE_LOADER FileLoaders[] = {
    { BdsFileSystemSupport, BdsFileSystemLoadImage },
    { BdsFirmwareVolumeSupport, BdsFirmwareVolumeLoadImage },
    //{ BdsLoadFileSupport, BdsLoadFileLoadImage },
    { BdsMemoryMapSupport, BdsMemoryMapLoadImage },
    { BdsPxeSupport, BdsPxeLoadImage },
    { BdsTftpSupport, BdsTftpLoadImage },
    { NULL, NULL }
};

EFI_STATUS
BdsLoadImage (
  IN     EFI_DEVICE_PATH       *DevicePath,
  IN     EFI_ALLOCATE_TYPE     Type,
  IN OUT EFI_PHYSICAL_ADDRESS* Image,
  OUT    UINTN                 *FileSize
  )
{
  EFI_STATUS      Status;
  EFI_HANDLE      Handle;
  EFI_DEVICE_PATH *RemainingDevicePath;
  BDS_FILE_LOADER*  FileLoader;

  Status = BdsConnectDevicePath (DevicePath, &Handle, &RemainingDevicePath);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FileLoader = FileLoaders;
  while (FileLoader->Support != NULL) {
    if (FileLoader->Support (DevicePath, Handle, RemainingDevicePath)) {
      return FileLoader->LoadImage (DevicePath, Handle, RemainingDevicePath, Type, Image, FileSize);
    }
    FileLoader++;
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
BdsStartEfiApplication (
  IN EFI_HANDLE                  ParentImageHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   ImageHandle;
  EFI_PHYSICAL_ADDRESS         BinaryBuffer;
  UINTN                        BinarySize;

  // Find the nearest supported file loader
  Status = BdsLoadImage (DevicePath, AllocateAnyPages,&BinaryBuffer,&BinarySize);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Load the image from the Buffer with Boot Services function
  Status = gBS->LoadImage (TRUE, ParentImageHandle, DevicePath, (VOID*)(UINTN)BinaryBuffer, BinarySize, &ImageHandle);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Before calling the image, enable the Watchdog Timer for  the 5 Minute period
  gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);
  // Start the image
  Status = gBS->StartImage (ImageHandle, NULL, NULL);
  // Clear the Watchdog Timer after the image returns
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  return Status;
}
