/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.  


Module Name:

  EdkIIGlueUefiDriverEntryPoint.h
  
Abstract: 

  Public header file for UEFI Driver Entry Point Lib

--*/

#ifndef __EDKII_GLUE_UEFI_DRIVER_ENTRY_POINT_H__
#define __EDKII_GLUE_UEFI_DRIVER_ENTRY_POINT_H__

//
// Declare the EFI/UEFI Specification Revision to which this driver is implemented 
//
extern const UINT32                   _gUefiDriverRevision;

//
// Declare the number of entry points in the image. 
//
extern const UINT8                    _gDriverEntryPointCount;

//
// Declare the number of unload handler in the image. 
//
extern const UINT8                    _gDriverUnloadImageCount;

//
// Declare the arrary of Boot Sevice Exit Event callbacks . 
//
extern const EFI_EVENT_NOTIFY         _gDriverExitBootServicesEvent[];

//
// Declare the arrary of Virtual Address Change Event callbacks . 
//
extern const EFI_EVENT_NOTIFY         _gDriverSetVirtualAddressMapEvent[];

/**
  Enrty point to DXE SMM Driver.

  @param  ImageHandle ImageHandle of the loaded driver.
  @param  SystemTable Pointer to the EFI System Table.

  @retval  EFI_SUCCESS One or more of the drivers returned a success code.
  @retval  !EFI_SUCESS The return status from the last driver entry point in the list.

**/
EFI_STATUS
EFIAPI
_ModuleEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );


/**
  Enrty point wrapper of DXE Driver.

  @param  ImageHandle ImageHandle of the loaded driver.
  @param  SystemTable Pointer to the EFI System Table.

  @retval  EFI_SUCCESS One or more of the drivers returned a success code.
  @retval  !EFI_SUCESS The return status from the last driver entry point in the list.

**/
EFI_STATUS
EFIAPI
EfiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );


/**
  Computes the cummulative return status for the driver entry point and perform
  a long jump back into DriverEntryPoint().

  @param  Status Status returned by the driver that is exiting.

**/
VOID
EFIAPI
ExitDriver (
  IN EFI_STATUS  Status
  );


/**
  Call constructs for all libraries. Automatics Generated by tool.

  @param  ImageHandle ImageHandle of the loaded driver.
  @param  SystemTable Pointer to the EFI System Table.

**/
VOID
EFIAPI
ProcessLibraryConstructorList (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );


/**
  Call destructors for all libraries. Automatics Generated by tool.

  @param  ImageHandle ImageHandle of the loaded driver.
  @param  SystemTable Pointer to the EFI System Table.

**/
VOID
EFIAPI
ProcessLibraryDestructorList (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

/**
  Call the list of driver entry points. Automatics Generated by tool.

  @param  ImageHandle ImageHandle of the loaded driver.
  @param  SystemTable Pointer to the EFI System Table.

  @return Status returned by entry points of drivers.  
 
**/

EFI_STATUS
EFIAPI
ProcessModuleEntryPointList (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );


/**
  Call the unload handlers for all the modules. Automatics Generated by tool.

  @param  ImageHandle ImageHandle of the loaded driver.
 
  @return Status returned by unload handlers of drivers.

**/
EFI_STATUS
EFIAPI
ProcessModuleUnloadList (
  IN EFI_HANDLE  ImageHandle
  );

#endif
