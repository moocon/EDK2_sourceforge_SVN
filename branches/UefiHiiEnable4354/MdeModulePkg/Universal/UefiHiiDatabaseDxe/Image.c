/** @file

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

    Image.c

Abstract:

    Implementation for EFI_HII_IMAGE_PROTOCOL.

Revision History


**/


#include "HiiDatabase.h"

#ifndef DISABLE_UNUSED_HII_PROTOCOLS

STATIC
UINT8*
GetImageIdOrAddress (
  IN  UINT8           *ImageBlock,
  IN OUT EFI_IMAGE_ID *ImageId
  )
/*++

  Routine Description:
    Get the imageid of last image block: EFI_HII_IIBT_END_BLOCK when input
    ImageId is zero, otherwise return the address of the
    corresponding image block with identifier specified by ImageId.

  Arguments:
    ImageBlock     - Points to the beginning of a series of image blocks stored in order.
    ImageId        - If input ImageId is 0, output the image id of the EFI_HII_IIBT_END_BLOCK;
                     else use this id to find its corresponding image block address.

  Returns:
    The image block address when input ImageId is not zero; otherwise return NULL.

--*/
{
  EFI_IMAGE_ID                   ImageIdCurrent;
  UINT8                          *ImageBlockHdr;
  UINT8                          Length8;
  UINT16                         Length16;
  UINT32                         Length32;
  EFI_HII_IIBT_IMAGE_1BIT_BLOCK  Iibt1bit;
  EFI_HII_IIBT_IMAGE_4BIT_BLOCK  Iibt4bit;
  EFI_HII_IIBT_IMAGE_8BIT_BLOCK  Iibt8bit;
  UINT16                         Width;
  UINT16                         Height;

  ASSERT (ImageBlock != NULL && ImageId != NULL);

  ImageBlockHdr  = ImageBlock;
  ImageIdCurrent = 1;

  while (((EFI_HII_IMAGE_BLOCK *) ImageBlock)->BlockType != EFI_HII_IIBT_END) {
    if (*ImageId > 0) {
      if (*ImageId == ImageIdCurrent) {
        //
        // If the found image block is a duplicate block, update the ImageId to
        // find the previous defined image block.
        //
        if (((EFI_HII_IMAGE_BLOCK *) ImageBlock)->BlockType == EFI_HII_IIBT_DUPLICATE) {
          CopyMem (ImageId, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (EFI_IMAGE_ID));
          ASSERT (*ImageId != ImageIdCurrent);
          ImageBlock = ImageBlockHdr;
          ImageIdCurrent = 1;
          continue;
        }

        return ImageBlock;
      }
      if (*ImageId < ImageIdCurrent) {
        //
        // Can not find the specified image block in this image.
        //
        return NULL;
      }
    }
    switch (((EFI_HII_IMAGE_BLOCK *) ImageBlock)->BlockType) {
    case EFI_HII_IIBT_EXT1:
      Length8 = *(ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT8));
      ImageBlock += Length8;
      ImageIdCurrent++;
      break;
    case EFI_HII_IIBT_EXT2:
      CopyMem (
        &Length16,
        ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT8),
        sizeof (UINT16)
        );
      ImageBlock += Length16;
      ImageIdCurrent++;
      break;
    case EFI_HII_IIBT_EXT4:
      CopyMem (
        &Length32,
        ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT8),
        sizeof (UINT32)
        );
      ImageBlock += Length32;
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_IMAGE_1BIT:
    case EFI_HII_IIBT_IMAGE_1BIT_TRANS:
      CopyMem (&Iibt1bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK));
      ImageBlock += sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK) - sizeof (UINT8) +
                    BITMAP_LEN_1_BIT (Iibt1bit.Bitmap.Width, Iibt1bit.Bitmap.Height);
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_IMAGE_4BIT:
    case EFI_HII_IIBT_IMAGE_4BIT_TRANS:
      CopyMem (&Iibt4bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_4BIT_BLOCK));
      ImageBlock += sizeof (EFI_HII_IIBT_IMAGE_4BIT_BLOCK) - sizeof (UINT8) +
                    BITMAP_LEN_4_BIT (Iibt4bit.Bitmap.Width, Iibt4bit.Bitmap.Height);
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_IMAGE_8BIT:
    case EFI_HII_IIBT_IMAGE_8BIT_TRANS:
      CopyMem (&Iibt8bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_8BIT_BLOCK));
      ImageBlock += sizeof (EFI_HII_IIBT_IMAGE_8BIT_BLOCK) - sizeof (UINT8) +
                    BITMAP_LEN_8_BIT (Iibt8bit.Bitmap.Width, Iibt8bit.Bitmap.Height);
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_IMAGE_24BIT:
    case EFI_HII_IIBT_IMAGE_24BIT_TRANS:
      CopyMem (&Width, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (UINT16));
      CopyMem (
        &Height,
        ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT16),
        sizeof (UINT16)
        );
      ImageBlock += sizeof (EFI_HII_IIBT_IMAGE_24BIT_BLOCK) - sizeof (EFI_HII_RGB_PIXEL) +
                    BITMAP_LEN_24_BIT (Width, Height);
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_DUPLICATE:
      ImageBlock += sizeof (EFI_HII_IIBT_DUPLICATE_BLOCK);
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_IMAGE_JPEG:
      CopyMem (&Length32, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (UINT32));
      ImageBlock += Length32;
      ImageIdCurrent++;
      break;

    case EFI_HII_IIBT_SKIP1:
      Length8 = *(ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK));
      ImageBlock += sizeof (EFI_HII_IIBT_SKIP1_BLOCK);
      ImageIdCurrent = (UINT16) (ImageIdCurrent + Length8);
      break;

    case EFI_HII_IIBT_SKIP2:
      CopyMem (&Length16, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (UINT16));
      ImageBlock += sizeof (EFI_HII_IIBT_SKIP2_BLOCK);
      ImageIdCurrent = (UINT16) (ImageIdCurrent + Length16);
      break;

    default:
      //
      // Unknown image blocks can not be skipped, processing halts.
      //
      ASSERT (FALSE);
    }
  }

  //
  // When ImageId is zero, return the imageid of last image block: EFI_HII_IIBT_END_BLOCK.
  //
  if (*ImageId == 0) {
    *ImageId = ImageIdCurrent;
    return ImageBlock;
  }

  return NULL;
}



/**
  Convert pixels from EFI_GRAPHICS_OUTPUT_BLT_PIXEL to EFI_HII_RGB_PIXEL style.

  @param  BitMapOut              Pixels in EFI_HII_RGB_PIXEL format.
  @param  BitMapIn               Pixels in EFI_GRAPHICS_OUTPUT_BLT_PIXEL format.
  @param  PixelNum               The number of pixels to be converted.


**/
STATIC
VOID
CopyGopToRgbPixel (
  OUT EFI_HII_RGB_PIXEL              *BitMapOut,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BitMapIn,
  IN  UINTN                          PixelNum
  )
{
  UINTN Index;

  ASSERT (BitMapOut != NULL && BitMapIn != NULL);

  for (Index = 0; Index < PixelNum; Index++) {
    CopyMem (BitMapOut + Index, BitMapIn + Index, sizeof (EFI_HII_RGB_PIXEL));
  }
}


/**
  Convert pixels from EFI_HII_RGB_PIXEL to EFI_GRAPHICS_OUTPUT_BLT_PIXEL style.

  @param  BitMapOut              Pixels in EFI_GRAPHICS_OUTPUT_BLT_PIXEL format.
  @param  BitMapIn               Pixels in EFI_HII_RGB_PIXEL format.
  @param  PixelNum               The number of pixels to be converted.


**/
STATIC
VOID
CopyRgbToGopPixel (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BitMapOut,
  IN  EFI_HII_RGB_PIXEL              *BitMapIn,
  IN  UINTN                          PixelNum
  )
{
  UINTN Index;

  ASSERT (BitMapOut != NULL && BitMapIn != NULL);

  for (Index = 0; Index < PixelNum; Index++) {
    CopyMem (BitMapOut + Index, BitMapIn + Index, sizeof (EFI_HII_RGB_PIXEL));
  }
}


/**
  Output pixels in "1 bit per pixel" format to an image.

  @param  Image                  Points to the image which will store the pixels.
  @param  Data                   Stores the value of output pixels, 0 or 1.
  @param  PaletteInfo            PaletteInfo which stores the color of the output
                                 pixels. First entry corresponds to color 0 and
                                 second one to color 1.


**/
STATIC
VOID
Output1bitPixel (
  IN OUT EFI_IMAGE_INPUT             *Image,
  IN UINT8                           *Data,
  IN EFI_HII_IMAGE_PALETTE_INFO      *PaletteInfo
  )
{
  UINT16                             X;
  UINT16                             Y;
  UINTN                              OffsetY;
  UINT8                              Index;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BitMapPtr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      PaletteValue[2];
  EFI_HII_IMAGE_PALETTE_INFO         *Palette;
  UINT16                             PaletteSize;
  UINT8                              Byte;

  ASSERT (Image != NULL && Data != NULL && PaletteInfo != NULL);

  BitMapPtr = Image->Bitmap;

  //
  // First entry corresponds to color 0 and second entry corresponds to color 1.
  //
  CopyMem (&PaletteSize, PaletteInfo, sizeof (UINT16));
  PaletteSize += sizeof (UINT16);
  Palette = AllocateZeroPool (PaletteSize);
  ASSERT (Palette != NULL);
  CopyMem (Palette, PaletteInfo, PaletteSize);

  ZeroMem (PaletteValue, sizeof (PaletteValue));
  CopyRgbToGopPixel (&PaletteValue[0], &Palette->PaletteValue[0], 1);
  CopyRgbToGopPixel (&PaletteValue[1], &Palette->PaletteValue[1], 1);
  SafeFreePool (Palette);

  //
  // Convert the pixel from one bit to corresponding color.
  //
  for (Y = 0; Y < Image->Height; Y++) {
    OffsetY = BITMAP_LEN_1_BIT (Image->Width, Y);
    //
    // All bits in these bytes are meaningful
    //
    for (X = 0; X < Image->Width / 8; X++) {
      Byte = *(Data + OffsetY + X);
      for (Index = 0; Index < 8; Index++) {
        if ((Byte & (1 << Index)) != 0) {
          BitMapPtr[Y * Image->Width + X * 8 + (8 - Index - 1)] = PaletteValue[1];
        } else {
          BitMapPtr[Y * Image->Width + X * 8 + (8 - Index - 1)] = PaletteValue[0];
        }
      }
    }

    if (Image->Width % 8 != 0) {
      //
      // Padding bits in this byte should be ignored.
      //
      Byte = *(Data + OffsetY + X);
      for (Index = 0; Index < Image->Width % 8; Index++) {
        if ((Byte & (1 << (8 - Index - 1))) != 0) {
          BitMapPtr[Y * Image->Width + X * 8 + Index] = PaletteValue[1];
        } else {
          BitMapPtr[Y * Image->Width + X * 8 + Index] = PaletteValue[0];
        }
      }
    }
  }
}


/**
  Output pixels in "4 bit per pixel" format to an image.

  @param  Image                  Points to the image which will store the pixels.
  @param  Data                   Stores the value of output pixels, 0 ~ 15.
  @param  PaletteInfo            PaletteInfo which stores the color of the output
                                 pixels. Each entry corresponds to a color within
                                 [0, 15].


**/
STATIC
VOID
Output4bitPixel (
  IN OUT EFI_IMAGE_INPUT             *Image,
  IN UINT8                           *Data,
  IN EFI_HII_IMAGE_PALETTE_INFO      *PaletteInfo
  )
{
  UINT16                             X;
  UINT16                             Y;
  UINTN                              OffsetY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BitMapPtr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      PaletteValue[16];
  EFI_HII_IMAGE_PALETTE_INFO         *Palette;
  UINT16                             PaletteSize;
  UINT16                             PaletteNum;
  UINT8                              Byte;

  ASSERT (Image != NULL && Data != NULL && PaletteInfo != NULL);

  BitMapPtr = Image->Bitmap;

  //
  // The bitmap should allocate each color index starting from 0.
  //
  CopyMem (&PaletteSize, PaletteInfo, sizeof (UINT16));
  PaletteSize += sizeof (UINT16);
  Palette = AllocateZeroPool (PaletteSize);
  ASSERT (Palette != NULL);
  CopyMem (Palette, PaletteInfo, PaletteSize);
  PaletteNum = Palette->PaletteSize / sizeof (EFI_HII_RGB_PIXEL);

  ZeroMem (PaletteValue, sizeof (PaletteValue));
  CopyRgbToGopPixel (PaletteValue, Palette->PaletteValue, PaletteNum);
  SafeFreePool (Palette);

  //
  // Convert the pixel from 4 bit to corresponding color.
  //
  for (Y = 0; Y < Image->Height; Y++) {
    OffsetY = BITMAP_LEN_4_BIT (Image->Width, Y);
    //
    // All bits in these bytes are meaningful
    //
    for (X = 0; X < Image->Width / 2; X++) {
      Byte = *(Data + OffsetY + X);
      BitMapPtr[Y * Image->Width + X * 2]     = PaletteValue[Byte >> 4];
      BitMapPtr[Y * Image->Width + X * 2 + 1] = PaletteValue[Byte & 0x0F];
    }

    if (Image->Width % 2 != 0) {
      //
      // Padding bits in this byte should be ignored.
      //
      Byte = *(Data + OffsetY + X);
      BitMapPtr[Y * Image->Width + X * 2]     = PaletteValue[Byte >> 4];
    }
  }
}


/**
  Output pixels in "8 bit per pixel" format to an image.

  @param  Image                  Points to the image which will store the pixels.
  @param  Data                   Stores the value of output pixels, 0 ~ 255.
  @param  PaletteInfo            PaletteInfo which stores the color of the output
                                 pixels. Each entry corresponds to a color within
                                 [0, 255].


**/
STATIC
VOID
Output8bitPixel (
  IN OUT EFI_IMAGE_INPUT             *Image,
  IN UINT8                           *Data,
  IN EFI_HII_IMAGE_PALETTE_INFO      *PaletteInfo
  )
{
  UINT16                             X;
  UINT16                             Y;
  UINTN                              OffsetY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BitMapPtr;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      PaletteValue[256];
  EFI_HII_IMAGE_PALETTE_INFO         *Palette;
  UINT16                             PaletteSize;
  UINT16                             PaletteNum;
  UINT8                              Byte;

  ASSERT (Image != NULL && Data != NULL && PaletteInfo != NULL);

  BitMapPtr = Image->Bitmap;

  //
  // The bitmap should allocate each color index starting from 0.
  //
  CopyMem (&PaletteSize, PaletteInfo, sizeof (UINT16));
  PaletteSize += sizeof (UINT16);
  Palette = AllocateZeroPool (PaletteSize);
  ASSERT (Palette != NULL);
  CopyMem (Palette, PaletteInfo, PaletteSize);
  PaletteNum = Palette->PaletteSize / sizeof (EFI_HII_RGB_PIXEL);
  ZeroMem (PaletteValue, sizeof (PaletteValue));
  CopyRgbToGopPixel (PaletteValue, Palette->PaletteValue, PaletteNum);
  SafeFreePool (Palette);

  //
  // Convert the pixel from 8 bits to corresponding color.
  //
  for (Y = 0; Y < Image->Height; Y++) {
    OffsetY = BITMAP_LEN_8_BIT (Image->Width, Y);
    //
    // All bits are meaningful since the bitmap is 8 bits per pixel.
    //
    for (X = 0; X < Image->Width; X++) {
      Byte = *(Data + OffsetY + X);
      BitMapPtr[OffsetY + X] = PaletteValue[Byte];
    }
  }

}


/**
  Output pixels in "24 bit per pixel" format to an image.

  @param  Image                  Points to the image which will store the pixels.
  @param  Data                   Stores the color of output pixels, allowing 16.8
                                 millions colors.


**/
STATIC
VOID
Output24bitPixel (
  IN OUT EFI_IMAGE_INPUT             *Image,
  IN EFI_HII_RGB_PIXEL               *Data
  )
{
  UINT16                             Y;
  UINTN                              OffsetY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      *BitMapPtr;

  ASSERT (Image != NULL && Data != NULL);

  BitMapPtr = Image->Bitmap;

  for (Y = 0; Y < Image->Height; Y++) {
    OffsetY = BITMAP_LEN_8_BIT (Image->Width, Y);
    CopyRgbToGopPixel (&BitMapPtr[OffsetY], &Data[OffsetY], Image->Width);
  }

}


/**
  Convert the image from EFI_IMAGE_INPUT to EFI_IMAGE_OUTPUT format.

  @param  BltBuffer              Buffer points to bitmap data of incoming image.
  @param  BltY                   Specifies the offset from the left and top edge of
                                  the output image of the first pixel in the image.
  @param  Width                  Width of the incoming image, in pixels.
  @param  Height                 Height of the incoming image, in pixels.
  @param  Transparent            If TRUE, all "off" pixels in the image will be
                                 drawn using the pixel value from blt and all other
                                 pixels will be copied.
  @param  Blt                    Buffer points to bitmap data of output image.

  @retval EFI_SUCCESS            The image was successfully converted.
  @retval EFI_INVALID_PARAMETER  Any incoming parameter is invalid.

**/
STATIC
EFI_STATUS
ImageToBlt (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer,
  IN UINTN                           BltX,
  IN UINTN                           BltY,
  IN UINTN                           Width,
  IN UINTN                           Height,
  IN BOOLEAN                         Transparent,
  IN OUT EFI_IMAGE_OUTPUT            **Blt
  )
{
  EFI_IMAGE_OUTPUT                   *ImageOut;
  UINTN                              X;
  UINTN                              Y;
  UINTN                              OffsetY1; // src buffer
  UINTN                              OffsetY2; // dest buffer
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      SrcPixel;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL      ZeroPixel;

  if (BltBuffer == NULL || Blt == NULL || *Blt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ImageOut = *Blt;

  if (Width + BltX > ImageOut->Width) {
    return EFI_INVALID_PARAMETER;
  }
  if (Height + BltY > ImageOut->Height) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&ZeroPixel, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  for (Y = 0; Y < Height; Y++) {
    OffsetY1 = Width * Y;
    OffsetY2 = ImageOut->Width * (BltY + Y);
    for (X = 0; X < Width; X++) {
      SrcPixel = BltBuffer[OffsetY1 + X];
      if (Transparent) {
        if (CompareMem (&SrcPixel, &ZeroPixel, 3) != 0) {
          ImageOut->Image.Bitmap[OffsetY2 + BltX + X] = SrcPixel;
        }
      } else {
        ImageOut->Image.Bitmap[OffsetY2 + BltX + X] = SrcPixel;
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  This function adds the image Image to the group of images owned by PackageList, and returns
  a new image identifier (ImageId).

  @param  This                   A pointer to the EFI_HII_IMAGE_PROTOCOL instance.
  @param  PackageList            Handle of the package list where this image will
                                 be added.
  @param  ImageId                On return, contains the new image id, which is
                                 unique within PackageList.
  @param  Image                  Points to the image.

  @retval EFI_SUCCESS            The new image was added successfully.
  @retval EFI_NOT_FOUND          The specified PackageList could not be found in
                                 database.
  @retval EFI_OUT_OF_RESOURCES   Could not add the image due to lack of resources.
  @retval EFI_INVALID_PARAMETER  Image is NULL or ImageId is NULL.

**/
EFI_STATUS
EFIAPI
HiiNewImage (
  IN  CONST EFI_HII_IMAGE_PROTOCOL   *This,
  IN  EFI_HII_HANDLE                 PackageList,
  OUT EFI_IMAGE_ID                   *ImageId,
  IN  CONST EFI_IMAGE_INPUT          *Image
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *DatabaseRecord;
  HII_DATABASE_PACKAGE_LIST_INSTANCE  *PackageListNode;
  HII_IMAGE_PACKAGE_INSTANCE          *ImagePackage;
  UINT8                               *ImageBlock;
  UINTN                               BlockSize;
  UINT8                               *NewBlock;
  UINT8                               *NewBlockPtr;
  UINTN                               NewBlockSize;
  EFI_IMAGE_INPUT                     *ImageIn;

  if (This == NULL || ImageId == NULL || Image == NULL || PackageList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsHiiHandleValid (PackageList)) {
    return EFI_NOT_FOUND;
  }

  Private = HII_IMAGE_DATABASE_PRIVATE_DATA_FROM_THIS (This);

  //
  // Get the specified package list
  //

  PackageListNode = NULL;

  for (Link = Private->DatabaseList.ForwardLink;
       Link != &Private->DatabaseList;
       Link = Link->ForwardLink
      ) {
    DatabaseRecord = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);
    if (DatabaseRecord->Handle == PackageList) {
      PackageListNode = DatabaseRecord->PackageList;
      break;
    }
  }

  if (PackageListNode == NULL) {
    return EFI_NOT_FOUND;
  }

  ImageIn = (EFI_IMAGE_INPUT *) Image;

  NewBlockSize = sizeof (EFI_HII_IIBT_IMAGE_24BIT_BLOCK) - sizeof (EFI_HII_RGB_PIXEL) +
                 BITMAP_LEN_24_BIT (ImageIn->Width, ImageIn->Height);

  //
  // Get the image package in the package list,
  // or create a new image package if image package does not exist.
  //
  if (PackageListNode->ImagePkg != NULL) {
    ImagePackage = PackageListNode->ImagePkg;

    //
    // Output the image id of the incoming image being inserted, which is the
    // image id of the EFI_HII_IIBT_END block of old image package.
    //
    *ImageId = 0;
    GetImageIdOrAddress (ImagePackage->ImageBlock, ImageId);

    //
    // Update the package's image block by appending the new block to the end.
    //
    BlockSize  = ImagePackage->ImageBlockSize + NewBlockSize;
    ImageBlock = (UINT8 *) AllocateZeroPool (BlockSize);
    if (ImageBlock == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Copy the original content.
    //
    CopyMem (
      ImageBlock,
      ImagePackage->ImageBlock,
      ImagePackage->ImageBlockSize - sizeof (EFI_HII_IIBT_END_BLOCK)
      );
    SafeFreePool (ImagePackage->ImageBlock);
    ImagePackage->ImageBlock = ImageBlock;
    ImageBlock += ImagePackage->ImageBlockSize - sizeof (EFI_HII_IIBT_END_BLOCK);
    //
    // Temp memory to store new block.
    //
    NewBlock = AllocateZeroPool (NewBlockSize);
    if (NewBlock == NULL) {
      SafeFreePool (ImagePackage->ImageBlock);
      ImagePackage->ImageBlock = NULL;
      return EFI_OUT_OF_RESOURCES;
    }
    NewBlockPtr = NewBlock;

    //
    // Update the length record.
    //
    ImagePackage->ImageBlockSize = (UINT32) BlockSize;
    ImagePackage->ImagePkgHdr.Header.Length += (UINT32) NewBlockSize;
    PackageListNode->PackageListHdr.PackageLength += (UINT32) NewBlockSize;

  } else {
    //
    // The specified package list does not contain image package.
    // Create one to add this image block.
    //
    ImagePackage = (HII_IMAGE_PACKAGE_INSTANCE *) AllocateZeroPool (sizeof (HII_IMAGE_PACKAGE_INSTANCE));
    if (ImagePackage == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Output the image id of the incoming image being inserted, which is the
    // first image block so that id is initially to one.
    //
    *ImageId = 1;
    BlockSize    = sizeof (EFI_HII_IIBT_END_BLOCK) + NewBlockSize;
    //
    // Fill in image package header.
    //
    ImagePackage->ImagePkgHdr.Header.Length     = (UINT32) BlockSize + sizeof (EFI_HII_IMAGE_PACKAGE_HDR);
    ImagePackage->ImagePkgHdr.Header.Type       = EFI_HII_PACKAGE_IMAGES;
    ImagePackage->ImagePkgHdr.ImageInfoOffset   = sizeof (EFI_HII_IMAGE_PACKAGE_HDR);
    ImagePackage->ImagePkgHdr.PaletteInfoOffset = 0;

    //
    // Fill in palette info.
    //
    ImagePackage->PaletteBlock    = NULL;
    ImagePackage->PaletteInfoSize = 0;

    //
    // Fill in image blocks.
    //
    ImagePackage->ImageBlockSize = (UINT32) BlockSize;
    ImagePackage->ImageBlock = (UINT8 *) AllocateZeroPool (BlockSize);
    if (ImagePackage->ImageBlock == NULL) {
      SafeFreePool (ImagePackage);
      return EFI_OUT_OF_RESOURCES;
    }
    ImageBlock = ImagePackage->ImageBlock;

    //
    // Temp memory to store new block.
    //
    NewBlock = AllocateZeroPool (NewBlockSize);
    if (NewBlock == NULL) {
      SafeFreePool (ImagePackage->ImageBlock);
      SafeFreePool (ImagePackage);
      return EFI_OUT_OF_RESOURCES;
    }
    NewBlockPtr = NewBlock;

    //
    // Insert this image package.
    //
    PackageListNode->ImagePkg = ImagePackage;
    PackageListNode->PackageListHdr.PackageLength += ImagePackage->ImagePkgHdr.Header.Length;
  }

  //
  // Append the new block here
  //
  if (ImageIn->Flags == EFI_IMAGE_TRANSPARENT) {
    *NewBlock = EFI_HII_IIBT_IMAGE_24BIT_TRANS;
  } else {
    *NewBlock = EFI_HII_IIBT_IMAGE_24BIT;
  }
  NewBlock++;
  CopyMem (NewBlock, &ImageIn->Width, sizeof (UINT16));
  NewBlock += sizeof (UINT16);
  CopyMem (NewBlock, &ImageIn->Height, sizeof (UINT16));
  NewBlock += sizeof (UINT16);
  CopyGopToRgbPixel ((EFI_HII_RGB_PIXEL *) NewBlock, ImageIn->Bitmap, ImageIn->Width * ImageIn->Height);

  CopyMem (ImageBlock, NewBlockPtr, NewBlockSize);
  SafeFreePool (NewBlockPtr);

  //
  // Append the block end
  //
  ImageBlock += NewBlockSize;
  ((EFI_HII_IIBT_END_BLOCK *) (ImageBlock))->Header.BlockType = EFI_HII_IIBT_END;

  return EFI_SUCCESS;
}


/**
  This function retrieves the image specified by ImageId which is associated with
  the specified PackageList and copies it into the buffer specified by Image.

  @param  This                   A pointer to the EFI_HII_IMAGE_PROTOCOL instance.
  @param  PackageList            Handle of the package list where this image will
                                 be searched.
  @param  ImageId                The image��s id,, which is unique within
                                 PackageList.
  @param  Image                  Points to the image.
  @param  ImageSize              On entry, points to the size of the buffer pointed
                                 to by Image, in bytes.  On return, points to the
                                 length of the image, in bytes.

  @retval EFI_SUCCESS            The new image was returned successfully.
  @retval EFI_NOT_FOUND          The image specified by ImageId is not available.
  @retval EFI_BUFFER_TOO_SMALL   The buffer specified by ImageSize is too small to
                                 hold the image.
  @retval EFI_INVALID_PARAMETER  The Image or ImageSize was NULL.

**/
EFI_STATUS
EFIAPI
HiiGetImage (
  IN  CONST EFI_HII_IMAGE_PROTOCOL   *This,
  IN  EFI_HII_HANDLE                 PackageList,
  IN  EFI_IMAGE_ID                   ImageId,
  OUT EFI_IMAGE_INPUT                *Image,
  OUT UINTN                          *ImageSize
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *DatabaseRecord;
  HII_DATABASE_PACKAGE_LIST_INSTANCE  *PackageListNode;
  HII_IMAGE_PACKAGE_INSTANCE          *ImagePackage;
  UINT8                               *ImageBlock;
  EFI_IMAGE_ID                        LocalImageId;
  UINT8                               BlockType;
  EFI_HII_IIBT_IMAGE_1BIT_BLOCK       Iibt1bit;
  UINT16                              Width;
  UINT16                              Height;
  UINTN                               ImageLength;
  BOOLEAN                             Flag;
  UINT8                               *PaletteInfo;
  UINT8                               PaletteIndex;
  UINT16                              PaletteSize;

  if (This == NULL || ImageSize == NULL || Image == NULL || ImageId < 1 || PackageList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsHiiHandleValid (PackageList)) {
    return EFI_NOT_FOUND;
  }

  Private = HII_IMAGE_DATABASE_PRIVATE_DATA_FROM_THIS (This);

  //
  // Get the specified package list and image package.
  //
  PackageListNode = NULL;
  for (Link = Private->DatabaseList.ForwardLink;
       Link != &Private->DatabaseList;
       Link = Link->ForwardLink
      ) {
    DatabaseRecord = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);
    if (DatabaseRecord->Handle == PackageList) {
      PackageListNode = DatabaseRecord->PackageList;
      break;
    }
  }
  if (PackageListNode == NULL) {
    return EFI_NOT_FOUND;
  }
  ImagePackage = PackageListNode->ImagePkg;
  if (ImagePackage == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Find the image block specified by ImageId
  //
  LocalImageId = ImageId;
  ImageBlock = GetImageIdOrAddress (ImagePackage->ImageBlock, &LocalImageId);
  if (ImageBlock == NULL) {
    return EFI_NOT_FOUND;
  }

  Flag      = FALSE;
  BlockType = *ImageBlock;

  switch (BlockType) {
  case EFI_HII_IIBT_IMAGE_JPEG:
    //
    // BUGBUG: need to be supported as soon as image tool is designed.
    //
    return EFI_UNSUPPORTED;
    break;

  case EFI_HII_IIBT_IMAGE_1BIT_TRANS:
  case EFI_HII_IIBT_IMAGE_4BIT_TRANS:
  case EFI_HII_IIBT_IMAGE_8BIT_TRANS:
    Flag = TRUE;
    //
    // fall through
    //
  case EFI_HII_IIBT_IMAGE_1BIT:
  case EFI_HII_IIBT_IMAGE_4BIT:
  case EFI_HII_IIBT_IMAGE_8BIT:
    //
    // Use the common block code since the definition of these structures is the same.
    //
    CopyMem (&Iibt1bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK));
    ImageLength = sizeof (EFI_IMAGE_INPUT) + sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) *
                  (Iibt1bit.Bitmap.Width * Iibt1bit.Bitmap.Height - 1);
    if (*ImageSize < ImageLength) {
      *ImageSize = ImageLength;
      return EFI_BUFFER_TOO_SMALL;
    }
    ZeroMem (Image, ImageLength);

    if (Flag) {
      Image->Flags = EFI_IMAGE_TRANSPARENT;
    }
    Image->Width  = Iibt1bit.Bitmap.Width;
    Image->Height = Iibt1bit.Bitmap.Height;

    PaletteInfo = ImagePackage->PaletteBlock + sizeof (EFI_HII_IMAGE_PALETTE_INFO_HEADER);
    for (PaletteIndex = 1; PaletteIndex < Iibt1bit.PaletteIndex; PaletteIndex++) {
      CopyMem (&PaletteSize, PaletteInfo, sizeof (UINT16));
      PaletteInfo += PaletteSize + sizeof (UINT16);
    }
    ASSERT (PaletteIndex == Iibt1bit.PaletteIndex);

    //
    // Output bitmap data
    //
    if (BlockType == EFI_HII_IIBT_IMAGE_1BIT || BlockType == EFI_HII_IIBT_IMAGE_1BIT_TRANS) {
      Output1bitPixel (
        Image,
        (UINT8 *) (ImageBlock + sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK) - sizeof (UINT8)),
        (EFI_HII_IMAGE_PALETTE_INFO *) PaletteInfo
        );
    } else if (BlockType == EFI_HII_IIBT_IMAGE_4BIT || BlockType == EFI_HII_IIBT_IMAGE_4BIT_TRANS) {
      Output4bitPixel (
        Image,
        (UINT8 *) (ImageBlock + sizeof (EFI_HII_IIBT_IMAGE_4BIT_BLOCK) - sizeof (UINT8)),
        (EFI_HII_IMAGE_PALETTE_INFO *) PaletteInfo
        );
    } else {
      Output8bitPixel (
        Image,
        (UINT8 *) (ImageBlock + sizeof (EFI_HII_IIBT_IMAGE_8BIT_BLOCK) - sizeof (UINT8)),
        (EFI_HII_IMAGE_PALETTE_INFO *) PaletteInfo
        );
    }

    return EFI_SUCCESS;
    break;

  case EFI_HII_IIBT_IMAGE_24BIT_TRANS:
    Flag = TRUE;
    //
    // fall through
    //
  case EFI_HII_IIBT_IMAGE_24BIT:
    CopyMem (&Width, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (UINT16));
    CopyMem (
      &Height,
      ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT16),
      sizeof (UINT16)
      );
    ImageLength = sizeof (EFI_IMAGE_INPUT) +
                  sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * (Width * Height - 1);
    if (*ImageSize < ImageLength) {
      *ImageSize = ImageLength;
      return EFI_BUFFER_TOO_SMALL;
    }
    ZeroMem (Image, ImageLength);

    if (Flag) {
      Image->Flags = EFI_IMAGE_TRANSPARENT;
    }
    Image->Width  = Width;
    Image->Height = Height;

    //
    // Output the bimap data directly.
    //
    Output24bitPixel (
      Image,
      (EFI_HII_RGB_PIXEL *) (ImageBlock + sizeof (EFI_HII_IIBT_IMAGE_24BIT_BLOCK) - sizeof (EFI_HII_RGB_PIXEL))
      );
    return EFI_SUCCESS;
    break;

  default:
    return EFI_NOT_FOUND;
    break;
  }
}


/**
  This function updates the image specified by ImageId in the specified PackageListHandle to
  the image specified by Image.

  @param  This                   A pointer to the EFI_HII_IMAGE_PROTOCOL instance.
  @param  PackageList            The package list containing the images.
  @param  ImageId                The image��s id,, which is unique within
                                 PackageList.
  @param  Image                  Points to the image.

  @retval EFI_SUCCESS            The new image was updated successfully.
  @retval EFI_NOT_FOUND          The image specified by ImageId is not in the
                                 database.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.

**/
EFI_STATUS
EFIAPI
HiiSetImage (
  IN CONST EFI_HII_IMAGE_PROTOCOL    *This,
  IN EFI_HII_HANDLE                  PackageList,
  IN EFI_IMAGE_ID                    ImageId,
  IN CONST EFI_IMAGE_INPUT           *Image
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *DatabaseRecord;
  HII_DATABASE_PACKAGE_LIST_INSTANCE  *PackageListNode;
  HII_IMAGE_PACKAGE_INSTANCE          *ImagePackage;
  UINT8                               *ImageBlock;
  EFI_IMAGE_ID                        LocalImageId;
  UINT8                               BlockType;
  EFI_HII_IIBT_IMAGE_1BIT_BLOCK       Iibt1bit;
  EFI_HII_IIBT_IMAGE_4BIT_BLOCK       Iibt4bit;
  EFI_HII_IIBT_IMAGE_8BIT_BLOCK       Iibt8bit;
  UINT16                              Width;
  UINT16                              Height;
  UINT32                              BlockSize;
  UINT32                              NewBlockSize;
  UINT32                              OldBlockSize;
  EFI_IMAGE_INPUT                     *ImageIn;
  UINT8                               *NewBlock;
  UINT8                               *NewBlockPtr;
  UINT8                               *Block;
  UINT8                               *BlockPtr;
  UINT32                               Part1Size;
  UINT32                               Part2Size;

  if (This == NULL || Image == NULL || ImageId < 1 || PackageList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsHiiHandleValid (PackageList)) {
    return EFI_NOT_FOUND;
  }

  Private = HII_IMAGE_DATABASE_PRIVATE_DATA_FROM_THIS (This);

  //
  // Get the specified package list and image package.
  //
  PackageListNode = NULL;
  for (Link = Private->DatabaseList.ForwardLink;
       Link != &Private->DatabaseList;
       Link = Link->ForwardLink
      ) {
    DatabaseRecord = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);
    if (DatabaseRecord->Handle == PackageList) {
      PackageListNode = DatabaseRecord->PackageList;
      break;
    }
  }
  if (PackageListNode == NULL) {
    return EFI_NOT_FOUND;
  }
  ImagePackage = PackageListNode->ImagePkg;
  if (ImagePackage == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Find the image block specified by ImageId
  //
  LocalImageId = ImageId;
  ImageBlock = GetImageIdOrAddress (ImagePackage->ImageBlock, &LocalImageId);
  if (ImageBlock == NULL) {
    return EFI_NOT_FOUND;
  }

  BlockType = *ImageBlock;

  //
  // Get the size of original image block. Use some common block code here
  // since the definition of some structures is the same.
  //
  switch (BlockType) {
  case EFI_HII_IIBT_IMAGE_JPEG:
    //
    // BUGBUG: need to be supported as soon as image tool is designed.
    //
    return EFI_UNSUPPORTED;
    break;

  case EFI_HII_IIBT_IMAGE_1BIT:
  case EFI_HII_IIBT_IMAGE_1BIT_TRANS:
    CopyMem (&Iibt1bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK));
    OldBlockSize = sizeof (EFI_HII_IIBT_IMAGE_1BIT_BLOCK) - sizeof (UINT8) +
                   BITMAP_LEN_1_BIT (Iibt1bit.Bitmap.Width, Iibt1bit.Bitmap.Height);
    break;
  case EFI_HII_IIBT_IMAGE_4BIT:
  case EFI_HII_IIBT_IMAGE_4BIT_TRANS:
    CopyMem (&Iibt4bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_4BIT_BLOCK));
    OldBlockSize = sizeof (EFI_HII_IIBT_IMAGE_4BIT_BLOCK) - sizeof (UINT8) +
                   BITMAP_LEN_4_BIT (Iibt4bit.Bitmap.Width, Iibt4bit.Bitmap.Height);
    break;
  case EFI_HII_IIBT_IMAGE_8BIT:
  case EFI_HII_IIBT_IMAGE_8BIT_TRANS:
    CopyMem (&Iibt8bit, ImageBlock, sizeof (EFI_HII_IIBT_IMAGE_8BIT_BLOCK));
    OldBlockSize = sizeof (EFI_HII_IIBT_IMAGE_8BIT_BLOCK) - sizeof (UINT8) +
                   BITMAP_LEN_8_BIT (Iibt8bit.Bitmap.Width, Iibt8bit.Bitmap.Height);
    break;
  case EFI_HII_IIBT_IMAGE_24BIT:
  case EFI_HII_IIBT_IMAGE_24BIT_TRANS:
    CopyMem (&Width, ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK), sizeof (UINT16));
    CopyMem (
      &Height,
      ImageBlock + sizeof (EFI_HII_IMAGE_BLOCK) + sizeof (UINT16),
      sizeof (UINT16)
      );
    OldBlockSize = sizeof (EFI_HII_IIBT_IMAGE_24BIT_BLOCK) - sizeof (EFI_HII_RGB_PIXEL) +
                   BITMAP_LEN_24_BIT (Width , Height);
    break;
  default:
    return EFI_NOT_FOUND;
    break;
  }

  //
  // Create the new image block according to input image.
  //
  ImageIn = (EFI_IMAGE_INPUT *) Image;
  NewBlockSize = sizeof (EFI_HII_IIBT_IMAGE_24BIT_BLOCK) - sizeof (EFI_HII_RGB_PIXEL) +
                 BITMAP_LEN_24_BIT (ImageIn->Width, ImageIn->Height);
  NewBlock = (UINT8 *) AllocateZeroPool (NewBlockSize);
  if (NewBlock == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewBlockPtr = NewBlock;
  if ((ImageIn->Flags & EFI_IMAGE_TRANSPARENT) == EFI_IMAGE_TRANSPARENT) {
    *NewBlockPtr = EFI_HII_IIBT_IMAGE_24BIT_TRANS;
  } else {
    *NewBlockPtr = EFI_HII_IIBT_IMAGE_24BIT;
  }
  NewBlockPtr++;

  CopyMem (NewBlockPtr, &ImageIn->Width, sizeof (UINT16));
  NewBlockPtr += sizeof (UINT16);
  CopyMem (NewBlockPtr, &ImageIn->Height, sizeof (UINT16));
  NewBlockPtr += sizeof (UINT16);

  CopyGopToRgbPixel ((EFI_HII_RGB_PIXEL *) NewBlockPtr, ImageIn->Bitmap, ImageIn->Width * ImageIn->Height);

  //
  // Adjust the image package to remove the original block firstly then add the new block.
  //
  BlockSize = ImagePackage->ImageBlockSize + NewBlockSize - OldBlockSize;
  Block = (UINT8 *) AllocateZeroPool (BlockSize);
  if (Block == NULL) {
    SafeFreePool (NewBlock);
    return EFI_OUT_OF_RESOURCES;
  }

  BlockPtr  = Block;
  Part1Size = (UINT32) (ImageBlock - ImagePackage->ImageBlock);
  Part2Size = ImagePackage->ImageBlockSize - Part1Size - OldBlockSize;
  CopyMem (BlockPtr, ImagePackage->ImageBlock, Part1Size);
  BlockPtr += Part1Size;
  CopyMem (BlockPtr, NewBlock, NewBlockSize);
  BlockPtr += NewBlockSize;
  CopyMem (BlockPtr, ImageBlock + OldBlockSize, Part2Size);

  SafeFreePool (ImagePackage->ImageBlock);
  SafeFreePool (NewBlock);
  ImagePackage->ImageBlock     = Block;
  ImagePackage->ImageBlockSize = BlockSize;
  ImagePackage->ImagePkgHdr.Header.Length += NewBlockSize - OldBlockSize;
  PackageListNode->PackageListHdr.PackageLength += NewBlockSize - OldBlockSize;

  return EFI_SUCCESS;

}


/**
  This function renders an image to a bitmap or the screen using the specified
  color and options. It draws the image on an existing bitmap, allocates a new
  bitmap or uses the screen. The images can be clipped.

  @param  This                   A pointer to the EFI_HII_IMAGE_PROTOCOL instance.
  @param  Flags                  Describes how the image is to be drawn.
  @param  Image                  Points to the image to be displayed.
  @param  Blt                    If this points to a non-NULL on entry, this points
                                 to the image, which is Width pixels wide and
                                 Height pixels high.  The image will be drawn onto
                                 this image and  EFI_HII_DRAW_FLAG_CLIP is implied.
                                 If this points to a  NULL on entry, then a buffer
                                 will be allocated to hold  the generated image and
                                 the pointer updated on exit. It is the caller��s
                                 responsibility to free this buffer.
  @param  BltY                   Specifies the offset from the left and top edge of
                                 the  output image of the first pixel in the image.

  @retval EFI_SUCCESS            The image was successfully drawn.
  @retval EFI_OUT_OF_RESOURCES   Unable to allocate an output buffer for Blt.
  @retval EFI_INVALID_PARAMETER  The Image or Blt was NULL.
  @retval EFI_INVALID_PARAMETER  Any combination of Flags is invalid.

**/
EFI_STATUS
EFIAPI
HiiDrawImage (
  IN CONST EFI_HII_IMAGE_PROTOCOL    *This,
  IN EFI_HII_DRAW_FLAGS              Flags,
  IN CONST EFI_IMAGE_INPUT           *Image,
  IN OUT EFI_IMAGE_OUTPUT            **Blt,
  IN UINTN                           BltX,
  IN UINTN                           BltY
  )
{
  EFI_STATUS                          Status;
  HII_DATABASE_PRIVATE_DATA           *Private;
  BOOLEAN                             Transparent;
  EFI_IMAGE_INPUT                     *ImageIn;
  EFI_IMAGE_OUTPUT                    *ImageOut;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *BltBuffer;
  UINTN                               BufferLen;
  UINTN                               Width;
  UINTN                               Height;
  UINTN                               X;
  UINTN                               Y;
  UINTN                               OffsetY1;
  UINTN                               OffsetY2;
  EFI_FONT_DISPLAY_INFO               *FontInfo;
  UINTN                               Index;
  EFI_CONSOLE_CONTROL_PROTOCOL        *Console;

  if (This == NULL || Image == NULL || Blt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Flags & EFI_HII_DRAW_FLAG_CLIP) == EFI_HII_DRAW_FLAG_CLIP && *Blt == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Flags & EFI_HII_DRAW_FLAG_TRANSPARENT) == EFI_HII_DRAW_FLAG_TRANSPARENT) {
    return EFI_INVALID_PARAMETER;
  }

  ImageIn = (EFI_IMAGE_INPUT *) Image;

  //
  // Check whether the image will be drawn transparently or opaquely.
  //
  Transparent = FALSE;
  if ((Flags & EFI_HII_DRAW_FLAG_TRANSPARENT) == EFI_HII_DRAW_FLAG_FORCE_TRANS) {
    Transparent = TRUE;
  } else if ((Flags & EFI_HII_DRAW_FLAG_TRANSPARENT) == EFI_HII_DRAW_FLAG_FORCE_OPAQUE){
    Transparent = FALSE;
  } else {
    //
    // Now EFI_HII_DRAW_FLAG_DEFAULT is set, whether image will be drawn depending
    // on the image's transparency setting.
    //
    if ((ImageIn->Flags & EFI_IMAGE_TRANSPARENT) == EFI_IMAGE_TRANSPARENT) {
      Transparent = TRUE;
    }
  }

  //
  // Image cannot be drawn transparently if Blt points to NULL on entry.
  // Currently output to Screen transparently is not supported, either.
  //
  if (Transparent) {
    if (*Blt == NULL) {
      return EFI_INVALID_PARAMETER;
    } else if ((Flags & EFI_HII_DIRECT_TO_SCREEN) == EFI_HII_DIRECT_TO_SCREEN) {
      return EFI_INVALID_PARAMETER;
    }
  }

  Private = HII_IMAGE_DATABASE_PRIVATE_DATA_FROM_THIS (This);

  //
  // When Blt points to a non-NULL on entry, this image will be drawn onto
  // this bitmap or screen pointed by "*Blt" and EFI_HII_DRAW_FLAG_CLIP is implied.
  // Otherwise a new bitmap will be allocated to hold this image.
  //
  if (*Blt != NULL) {
    //
    // Clip the image by (Width, Height)
    //

    Width  = ImageIn->Width;
    Height = ImageIn->Height;

    if (Width > (*Blt)->Width - BltX) {
      Width = (*Blt)->Width - BltX;
    }
    if (Height > (*Blt)->Height - BltY) {
      Height = (*Blt)->Height - BltY;
    }

    BufferLen = Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) AllocateZeroPool (BufferLen);
    if (BltBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (Width == ImageIn->Width && Height == ImageIn->Height) {
      CopyMem (BltBuffer, ImageIn->Bitmap, BufferLen);
    } else {
      for (Y = 0; Y < Height; Y++) {
        OffsetY1 = ImageIn->Width * Y;
        OffsetY2 = Width * Y;
        for (X = 0; X < Width; X++) {
          BltBuffer[OffsetY2 + X] = ImageIn->Bitmap[OffsetY1 + X];
        }
      }
    }

    //
    // Draw the image to existing bitmap or screen depending on flag.
    //
    if ((Flags & EFI_HII_DIRECT_TO_SCREEN) == EFI_HII_DIRECT_TO_SCREEN) {
      Status = gBS->LocateProtocol (
                      &gEfiConsoleControlProtocolGuid,
                      NULL,
                      (VOID **) &Console
                      );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      Console->SetMode (Console, EfiConsoleControlScreenGraphics);
      //
      // Write the image directly to the output device specified by Screen.
      //
      Status = (*Blt)->Image.Screen->Blt (
                                       (*Blt)->Image.Screen,
                                       BltBuffer,
                                       EfiBltBufferToVideo,
                                       0,
                                       0,
                                       BltX,
                                       BltY,
                                       Width,
                                       Height,
                                       0
                                       );
    } else {
      //
      // Draw the image onto the existing bitmap specified by Bitmap.
      //
      Status = ImageToBlt (
                 BltBuffer,
                 BltX,
                 BltY,
                 Width,
                 Height,
                 Transparent,
                 Blt
                 );

    }

    SafeFreePool (BltBuffer);
    return Status;

  } else {
    //
    // Allocate a new bitmap to hold the incoming image.
    //
    Width  = ImageIn->Width  + BltX;
    Height = ImageIn->Height + BltY;

    BufferLen = Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) AllocateZeroPool (BufferLen);
    if (BltBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    ImageOut = (EFI_IMAGE_OUTPUT *) AllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT));
    if (ImageOut == NULL) {
      SafeFreePool (BltBuffer);
      return EFI_OUT_OF_RESOURCES;
    }
    ImageOut->Width        = (UINT16) Width;
    ImageOut->Height       = (UINT16) Height;
    ImageOut->Image.Bitmap = BltBuffer;

    //
    // BUGBUG: Now all the "blank" pixels are filled with system default background
    // color. Not sure if it need to be updated or not.
    //
    Status = GetSystemFont (Private, &FontInfo, NULL);
    if (EFI_ERROR (Status)) {
      SafeFreePool (BltBuffer);
      SafeFreePool (ImageOut);
      return Status;
    }
    for (Index = 0; Index < Width * Height; Index++) {
      BltBuffer[Index] = FontInfo->BackgroundColor;
    }
    SafeFreePool (FontInfo);

    //
    // Draw the incoming image to the new created image.
    //
    *Blt = ImageOut;
    return ImageToBlt (
             ImageIn->Bitmap,
             BltX,
             BltY,
             ImageIn->Width,
             ImageIn->Height,
             Transparent,
             Blt
             );

  }
}


/**
  This function renders an image to a bitmap or the screen using the specified
  color and options. It draws the image on an existing bitmap, allocates a new
  bitmap or uses the screen. The images can be clipped.

  @param  This                   A pointer to the EFI_HII_IMAGE_PROTOCOL instance.
  @param  Flags                  Describes how the image is to be drawn.
  @param  PackageList            The package list in the HII database to search for
                                 the  specified image.
  @param  ImageId                The image's id, which is unique within
                                 PackageList.
  @param  Blt                    If this points to a non-NULL on entry, this points
                                 to the image, which is Width pixels wide and
                                 Height pixels high. The image will be drawn onto
                                 this image and
                                 EFI_HII_DRAW_FLAG_CLIP is implied. If this points
                                 to a  NULL on entry, then a buffer will be
                                 allocated to hold  the generated image and the
                                 pointer updated on exit. It is the caller��s
                                 responsibility to free this buffer.
  @param  BltY                   Specifies the offset from the left and top edge of
                                 the  output image of the first pixel in the image.

  @retval EFI_SUCCESS            The image was successfully drawn.
  @retval EFI_OUT_OF_RESOURCES   Unable to allocate an output buffer for Blt.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_NOT_FOUND          The specified packagelist could not be found in
                                 current database.

**/
EFI_STATUS
EFIAPI
HiiDrawImageId (
  IN CONST EFI_HII_IMAGE_PROTOCOL    *This,
  IN EFI_HII_DRAW_FLAGS              Flags,
  IN EFI_HII_HANDLE                  PackageList,
  IN EFI_IMAGE_ID                    ImageId,
  IN OUT EFI_IMAGE_OUTPUT            **Blt,
  IN UINTN                           BltX,
  IN UINTN                           BltY
  )
{
  EFI_STATUS                          Status;
  EFI_IMAGE_INPUT                     ImageTemp;
  EFI_IMAGE_INPUT                     *Image;
  UINTN                               ImageSize;

  //
  // Check input parameter.
  //
  if (This == NULL || PackageList == NULL || Blt == NULL || PackageList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsHiiHandleValid (PackageList)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get the specified Image.
  //
  ImageSize = 0;
  Status = HiiGetImage (This, PackageList, ImageId, &ImageTemp, &ImageSize);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  Image = (EFI_IMAGE_INPUT *) AllocateZeroPool (ImageSize);
  if (Image == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = HiiGetImage (This, PackageList, ImageId, Image, &ImageSize);
  ASSERT_EFI_ERROR (Status);

  //
  // Draw this image.
  //
  Status = HiiDrawImage (This, Flags, Image, Blt, BltX, BltY);
  SafeFreePool (Image);
  return Status;
}

#endif

