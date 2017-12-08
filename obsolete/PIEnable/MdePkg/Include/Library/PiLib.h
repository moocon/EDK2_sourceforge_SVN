/** @file
  MDE PI library functions and macros

  Copyright (c) 2007, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __PI_LIB_H__
#define __PI_LIB_H__

#include <Pi/PiFirmwareFile.h>

/**
  Allocate and fill a buffer with an image identified by a Firmware File GUID name and a Firmware Section type. 
  The Firmware Volumes to search for the Firmware File can be specified to be either all Firmware Volumes 
  in the system, or the Firmware Volume which contains the Firmware File specified by an image handle.

  If ImageHandle is NULL, all Firmware Volumes in the system will be searched. If ImageHandle is not NULL, 
  ImageHandle is interpreted as EFI_PEI_FILE_HANDLE for the implementation of this function for PEI phase. 
  The input parameter ImageHandle is interpreted as EFI_HANDLE, on which an EFI_LOADED_IMAGE_PROTOCOL 
  is installed, for the implementation of this function for DXE phase. The search always starts from the FV 
  identified by ImageHandle. If WithinImageFv is TRUE, search will only be performed on the first FV. If WithinImageFv 
  is FALSE, search will continue on other FVs if it fails on the first FV. The search order of Firmware Volumes is 
  deterministic but arbitrary if no new firmware volume is added into the system between each search. 
  
  The search order for the section type specified by SectionType in the Firmware File is using a depth-first 
  and left-to-right algorithm through all sections. The first section found to match SectionType will be returned. 
  
  If SectionType is EFI_SECTION_PE32, EFI_SECTION_PE32 will be used as Firmware Section type 
  to read Firmware Section data from the Firmware File. If no such section exists, the function will try 
  to read a Firmware File named with NameGuid. If no such file exists, EFI_NOT_FOUND is returned.
  
  If SectionType is EFI_SECTION_TE, EFI_SECTION_TE will be used as Firmware Section type to read Firmware Section 
  data from the Firmware File. If no such section exists, EFI_SECTION_PE32 will be used as Firmware Section type to 
  read Firmware Section data from the Firmware File. If no such section exists, the function will try to read a Firmware 
  File named with NameGuid. If no such file exists, EFI_NOT_FOUND is returned.
  
  The data and size is returned by Buffer and Size. The caller is responsible to free the Buffer allocated 
  by this function. This function can only be called at TPL_NOTIFY and below.
  
  If ImageHandle is NULL and WithinImage is TRUE, then ASSERT ();
  If NameGuid is NULL, then ASSERT();
  If Buffer is NULL, then ASSERT();
  If Size is NULL, then ASSERT().

  @param  NameGuid             The GUID name of a Firmware File.
  @param  SectionType         The Firmware Section type.
  @param  Buffer                  On output, Buffer contains the the data read from the section in the Firmware File found.
  @param  Size                    On output, the size of Buffer.

  @retval  EFI_SUCCESS	      The image is found and data and size is returned.
  @retval  EFI_NOT_FOUND	    The image specified by NameGuid and SectionType can't be found.
  @retval  EFI_OUT_OF_RESOURCES	There were not enough resources to allocate the output data buffer or complete the operations.
  @retval  EFI_DEVICE_ERROR	A hardware error occurs during reading from the Firmware Volume.
  @retval  EFI_ACCESS_DENIED	The firmware volume containing the searched Firmware File is configured to disallow reads.

**/

EFI_STATUS
EFIAPI
GetSectionFromFvFile (
  IN  CONST VOID*        ImageHandle OPTIONAL,
  IN  CONST EFI_GUID     *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size,
  IN  BOOLEAN            WithImageFv
  )
;


#endif
