/** @file  NorFlashBlockIoDxe.c

  Copyright (c) 2011, ARM Ltd. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "NorFlashDxe.h"

EFI_STATUS
EFIAPI
NorFlashBlkIoInitialize (
  IN NOR_FLASH_INSTANCE* Instance
  )
{
  UINT32                Reply;
  EFI_STATUS            Status = EFI_SUCCESS;

  DEBUG((DEBUG_BLKIO,"NorFlashBlkIoInitialize()\n"));

  //
  // Verify that there is a physical hardware device where we expect it to be.
  //

  // Read a specific CFI query that returns back "QRY"
  // This ensures that there is really a device present there
  SEND_NOR_COMMAND (Instance->BaseAddress, 0, P30_CMD_READ_CFI_QUERY);

  // Read CFI 'QRY' data
  Status = NorFlashReadCfiData (Instance->BaseAddress, P30_CFI_ADDR_QUERY_UNIQUE_QRY, 3, &Reply);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Reply != CFI_QRY) {
    DEBUG((EFI_D_ERROR, "NorFlashBlkIoInitialize: CFI QRY=0x%x (expected 0x595251)\n", Reply));
    return EFI_DEVICE_ERROR;
  }

  // Reset the device
  Status = NorFlashBlockIoReset (&Instance->BlockIoProtocol, FALSE);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Instance->Initialized = TRUE;
  return EFI_SUCCESS;
}


//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.Reset
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReset (
  IN EFI_BLOCK_IO_PROTOCOL  *This,
  IN BOOLEAN                ExtendedVerification
  )
{
  NOR_FLASH_INSTANCE *Instance;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);

  DEBUG ((DEBUG_BLKIO, "NorFlashBlockIoReset(MediaId=0x%x)\n", This->Media->MediaId));

  return NorFlashReset(Instance);
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.ReadBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoReadBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSizeInBytes,
  OUT VOID                    *Buffer
  )
{
  NOR_FLASH_INSTANCE  *Instance;
  EFI_STATUS          Status;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);

  DEBUG ((DEBUG_BLKIO, "NorFlashBlockIoReadBlocks(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%08x)\n", MediaId, Lba, BufferSizeInBytes, Buffer));

  if( !This->Media->MediaPresent ) {
    Status = EFI_NO_MEDIA;
  } else if( This->Media->MediaId != MediaId ) {
    Status = EFI_MEDIA_CHANGED;
  } else {
    Status = NorFlashReadBlocks(Instance,Lba,BufferSizeInBytes,Buffer);
  }

  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.WriteBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoWriteBlocks (
  IN  EFI_BLOCK_IO_PROTOCOL   *This,
  IN  UINT32                  MediaId,
  IN  EFI_LBA                 Lba,
  IN  UINTN                   BufferSizeInBytes,
  IN  VOID                    *Buffer
  )
{
  NOR_FLASH_INSTANCE  *Instance;
  EFI_STATUS          Status;

  Instance = INSTANCE_FROM_BLKIO_THIS(This);

  DEBUG ((DEBUG_BLKIO, "NorFlashBlockIoWriteBlocks(MediaId=0x%x, Lba=%ld, BufferSize=0x%x bytes (%d kB), BufferPtr @ 0x%08x)\n", MediaId, Lba, BufferSizeInBytes, Buffer));

  if( !This->Media->MediaPresent ) {
    Status = EFI_NO_MEDIA;
  } else if( This->Media->MediaId != MediaId ) {
    Status = EFI_MEDIA_CHANGED;
  } else if( This->Media->ReadOnly ) {
    Status = EFI_WRITE_PROTECTED;
  } else {
    Status = NorFlashWriteBlocks(Instance,Lba,BufferSizeInBytes,Buffer);
  }

  return Status;
}

//
// BlockIO Protocol function EFI_BLOCK_IO_PROTOCOL.FlushBlocks
//
EFI_STATUS
EFIAPI
NorFlashBlockIoFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  // No Flush required for the NOR Flash driver
  // because cache operations are not permitted.

  DEBUG ((DEBUG_BLKIO, "NorFlashBlockIoFlushBlocks: Function NOT IMPLEMENTED (not required).\n"));

  // Nothing to do so just return without error
  return EFI_SUCCESS;
}
