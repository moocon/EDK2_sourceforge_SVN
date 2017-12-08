/**@file
  PS/2 Mouse driver header file
  
Copyright (c) 2006 - 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _PS2MOUSE_H
#define _PS2MOUSE_H

#include <PiDxe.h>
#include <Framework/StatusCode.h>

#include <Protocol/SimplePointer.h>
#include <Protocol/IsaIo.h>
#include <Protocol/DevicePath.h>

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>

//
// PS/2 mouse sample rate
//
typedef enum {
  SSR_10,
  SSR_20,
  SSR_40,
  SSR_60,
  SSR_80,
  SSR_100,
  SSR_200,
  MAX_SR
} MOUSE_SR;

//
// PS/2 mouse resolution
//
typedef enum {
  CMR1,
  CMR2,
  CMR4,
  CMR8,
  MAX_CMR
} MOUSE_RE;

//
// PS/2 mouse scaling
//
typedef enum {
  SF1,
  SF2
} MOUSE_SF;

//
// Driver Private Data
//
#define PS2_MOUSE_DEV_SIGNATURE EFI_SIGNATURE_32 ('p', 's', '2', 'm')

typedef struct {
  UINTN                               Signature;

  EFI_HANDLE                          Handle;
  EFI_SIMPLE_POINTER_PROTOCOL         SimplePointerProtocol;
  EFI_SIMPLE_POINTER_STATE            State;
  EFI_SIMPLE_POINTER_MODE             Mode;
  BOOLEAN                             StateChanged;

  //
  // PS2 Mouse device specific information
  //
  MOUSE_SR                            SampleRate;
  MOUSE_RE                            Resolution;
  MOUSE_SF                            Scaling;
  UINT8                               DataPackageSize;

  EFI_ISA_IO_PROTOCOL                 *IsaIo;

  EFI_EVENT                           TimerEvent;

  EFI_UNICODE_STRING_TABLE            *ControllerNameTable;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
} PS2_MOUSE_DEV;

#define PS2_MOUSE_DEV_FROM_THIS(a)  CR (a, PS2_MOUSE_DEV, SimplePointerProtocol, PS2_MOUSE_DEV_SIGNATURE)

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL  gPS2MouseDriver;
extern EFI_COMPONENT_NAME_PROTOCOL  gPs2MouseComponentName;

//
// Function prototypes
//
EFI_STATUS
EFIAPI
PS2MouseDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PS2MouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
PS2MouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  );

//
// EFI Component Name Functions
//
EFI_STATUS
EFIAPI
Ps2MouseComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  );

EFI_STATUS
EFIAPI
Ps2MouseComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  );

EFI_STATUS
EFIAPI
MouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

EFI_STATUS
EFIAPI
MouseGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN OUT EFI_SIMPLE_POINTER_STATE   *State
  );

VOID
EFIAPI
MouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

VOID
EFIAPI
PollMouse (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

EFI_STATUS
In8042Data (
  IN EFI_ISA_IO_PROTOCOL                  *IsaIo,
  IN OUT UINT8                            *Data
  );
BOOLEAN
CheckMouseConnect (
  IN  PS2_MOUSE_DEV     *MouseDev
  );

#endif