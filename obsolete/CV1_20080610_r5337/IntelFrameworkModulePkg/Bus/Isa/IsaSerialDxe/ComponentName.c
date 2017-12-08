/**@file

  
Copyright (c) 2006 - 2007, Intel Corporation.<BR>
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Serial.h"

//
// EFI Component Name Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME_PROTOCOL  gIsaSerialComponentName = {
  IsaSerialComponentNameGetDriverName,
  IsaSerialComponentNameGetControllerName,
  "eng"
};

//
// EFI Component Name 2 Protocol
//
GLOBAL_REMOVE_IF_UNREFERENCED EFI_COMPONENT_NAME2_PROTOCOL gIsaSerialComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME) IsaSerialComponentNameGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) IsaSerialComponentNameGetControllerName,
  "en"
};


GLOBAL_REMOVE_IF_UNREFERENCED EFI_UNICODE_STRING_TABLE mIsaSerialDriverNameTable[] = {
  {
    "eng;en",
    L"ISA Serial Driver"
  },
  {
    NULL,
    NULL
  }
};

/**
  Retrieves a Unicode string that is the user readable name of the driver.

  This function retrieves the user readable name of a driver in the form of a
  Unicode string. If the driver specified by This has a user readable name in
  the language specified by Language, then a pointer to the driver name is
  returned in DriverName, and EFI_SUCCESS is returned. If the driver specified
  by This does not support the language specified by Language,
  then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language. This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified
                                in RFC 3066 or ISO 639-2 language code format.

  @param  DriverName[out]       A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                driver specified by This in the language
                                specified by Language.

  @retval EFI_SUCCESS           The Unicode string for the Driver specified by
                                This and the language specified by Language was
                                returned in DriverName.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER DriverName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
IsaSerialComponentNameGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
  )
{
  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mIsaSerialDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gIsaSerialComponentName)
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by a driver.

  This function retrieves the user readable name of the controller specified by
  ControllerHandle and ChildHandle in the form of a Unicode string. If the
  driver specified by This has a user readable name in the language specified by
  Language, then a pointer to the controller name is returned in ControllerName,
  and EFI_SUCCESS is returned.  If the driver specified by This is not currently
  managing the controller specified by ControllerHandle and ChildHandle,
  then EFI_UNSUPPORTED is returned.  If the driver specified by This does not
  support the language specified by Language, then EFI_UNSUPPORTED is returned.

  @param  This[in]              A pointer to the EFI_COMPONENT_NAME2_PROTOCOL or
                                EFI_COMPONENT_NAME_PROTOCOL instance.

  @param  ControllerHandle[in]  The handle of a controller that the driver
                                specified by This is managing.  This handle
                                specifies the controller whose name is to be
                                returned.

  @param  ChildHandle[in]       The handle of the child controller to retrieve
                                the name of.  This is an optional parameter that
                                may be NULL.  It will be NULL for device
                                drivers.  It will also be NULL for a bus drivers
                                that wish to retrieve the name of the bus
                                controller.  It will not be NULL for a bus
                                driver that wishes to retrieve the name of a
                                child controller.

  @param  Language[in]          A pointer to a Null-terminated ASCII string
                                array indicating the language.  This is the
                                language of the driver name that the caller is
                                requesting, and it must match one of the
                                languages specified in SupportedLanguages. The
                                number of languages supported by a driver is up
                                to the driver writer. Language is specified in
                                RFC 3066 or ISO 639-2 language code format.

  @param  ControllerName[out]   A pointer to the Unicode string to return.
                                This Unicode string is the name of the
                                controller specified by ControllerHandle and
                                ChildHandle in the language specified by
                                Language from the point of view of the driver
                                specified by This.

  @retval EFI_SUCCESS           The Unicode string for the user readable name in
                                the language specified by Language for the
                                driver specified by This was returned in
                                DriverName.

  @retval EFI_INVALID_PARAMETER ControllerHandle is not a valid EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER ChildHandle is not NULL and it is not a valid
                                EFI_HANDLE.

  @retval EFI_INVALID_PARAMETER Language is NULL.

  @retval EFI_INVALID_PARAMETER ControllerName is NULL.

  @retval EFI_UNSUPPORTED       The driver specified by This is not currently
                                managing the controller specified by
                                ControllerHandle and ChildHandle.

  @retval EFI_UNSUPPORTED       The driver specified by This does not support
                                the language specified by Language.

**/
EFI_STATUS
EFIAPI
IsaSerialComponentNameGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL                     *This,
  IN  EFI_HANDLE                                      ControllerHandle,
  IN  EFI_HANDLE                                      ChildHandle        OPTIONAL,
  IN  CHAR8                                           *Language,
  OUT CHAR16                                          **ControllerName
  )
{
  EFI_STATUS              Status;
  EFI_SERIAL_IO_PROTOCOL  *SerialIo;
  SERIAL_DEV              *SerialDevice;

  //
  // This is a device driver, so ChildHandle must be NULL.
  //
  if (ChildHandle != NULL) {
    return EFI_UNSUPPORTED;
  }
  //
  // Make sure this driver is currently managing ControllerHandle
  //
  Status = EfiTestManagedDevice (
             ControllerHandle,
             gSerialControllerDriver.DriverBindingHandle,
             &gEfiIsaIoProtocolGuid
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Block I/O Protocol on Controller
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSerialIoProtocolGuid,
                  (VOID **) &SerialIo,
                  gSerialControllerDriver.DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Serial Controller's Device structure
  //
  SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           SerialDevice->ControllerNameTable,
           ControllerName,
           (BOOLEAN)(This == &gIsaSerialComponentName)
           );
}

VOID
AddName (
  IN  SERIAL_DEV                               *SerialDevice,
  IN  EFI_ISA_IO_PROTOCOL                      *IsaIo
  )
/**

  Routine Description:
  
    Add the component name for the serial io device

  Arguments:
  
    SerialDevice              - A pointer to the SERIAL_DEV instance.
    IsaIo                     - A pointer to the EFI_ISA_IO_PROTOCOL or EFI_LIGHT_ISA_IO_PROTOCOL instance.
    
  Returns:

    None
    
**/
{
  CHAR16  SerialPortName[sizeof (SERIAL_PORT_NAME)];

  StrCpy (SerialPortName, L"ISA Serial Port # ");
  SerialPortName[sizeof (SERIAL_PORT_NAME) - 2] = (CHAR16) (L'0' + (UINT8) IsaIo->ResourceList->Device.UID);
  AddUnicodeString2 (
    "eng",
    gIsaSerialComponentName.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    (CHAR16 *) SerialPortName,
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gIsaSerialComponentName2.SupportedLanguages,
    &SerialDevice->ControllerNameTable,
    (CHAR16 *) SerialPortName,
    FALSE
    );

}