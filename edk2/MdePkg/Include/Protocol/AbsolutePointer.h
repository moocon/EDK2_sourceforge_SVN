/** @file
  The file provides services that allow information about a
  absolute pointer device to be retrieved.
  
  Copyright (c) 2006 - 2008, Intel Corporation
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __ABSOLUTE_POINTER_H__
#define __ABSOLUTE_POINTER_H__


#define EFI_ABSOLUTE_POINTER_PROTOCOL_GUID \
  { 0x8D59D32B, 0xC655, 0x4AE9, { 0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43 } }


typedef struct _EFI_ABSOLUTE_POINTER_PROTOCOL EFI_ABSOLUTE_POINTER_PROTOCOL;


//*******************************************************
// EFI_ABSOLUTE_POINTER_MODE
//*******************************************************


/**
  The following data values in the EFI_ABSOLUTE_POINTER_MODE
  interface are read-only and are changed by using the appropriate
  interface functions.
  
  @param AbsoluteMinX The Absolute Minimum of the device on the
                      x-axis.

  @param AbsoluteMinY The Absolute Minimum of the device on the
                      y axis.

  @param AbsoluteMinZ The Absolute Minimum of the device on the
                      z-axis.

  @param AbsoluteMaxX The Absolute Maximum of the device on the
                      x-axis. If 0, and the AbsoluteMinX is 0,
                      then the pointer device does not support a
                      xaxis.

  @param AbsoluteMaxY The Absolute Maximum of the device on the
                      y -axis. If 0,, and the AbsoluteMinX is 0,
                      then the pointer device does not support a
                      yaxis.

  @param AbsoluteMaxZ The Absolute Maximum of the device on the
                      z-axis. If 0 , and the AbsoluteMinX is 0,
                      then the pointer device does not support a
                      zaxis. 

  @param Attributes   The following bits are set as needed (or'd
                      together) to indicate the capabilities of the device
                      supported. The remaining bits are undefined and should be 0.

**/
typedef struct {
  UINT64 AbsoluteMinX;
  UINT64 AbsoluteMinY;
  UINT64 AbsoluteMinZ;
  UINT64 AbsoluteMaxX;
  UINT64 AbsoluteMaxY;
  UINT64 AbsoluteMaxZ;
  UINT32 Attributes;
} EFI_ABSOLUTE_POINTER_MODE;

//
// If set, indicates this device supports an alternate button input. 
// 
#define EFI_ABSP_SupportsAltActive    0x00000001

//
// If set, indicates this device returns pressure data in parameter CurrentZ.
// 
#define EFI_ABSP_SupportsPressureAsZ  0x00000002


/**
  This function resets the pointer device hardware. As part of
  initialization process, the firmware/device will make a quick
  but reasonable attempt to verify that the device is
  functioning. If the ExtendedVerification flag is TRUE the
  firmware may take an extended amount of time to verify the
  device is operating on reset. Otherwise the reset operation is
  to occur as quickly as possible. The hardware verification
  process is not defined by this specification and is left up to
  the platform firmware or driver to implement.

  @param This                 A pointer to the EFI_ABSOLUTE_POINTER_PROTOCOL
                              instance.

  @param ExtendedVerification Indicates that the driver may
                              perform a more exhaustive
                              verification operation of the
                              device during reset.

  @retval EFI_SUCCESS       The device was reset.
  
  @retval EFI_DEVICE_ERROR  The device is not functioning
                            correctly and could not be reset.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_RESET)(
  IN EFI_ABSOLUTE_POINTER_PROTOCOL *This,
  IN BOOLEAN                       ExtendedVerification
);




/**
  Definitions of bits within ActiveButtons.
  
  @param EFI_ABSP_TouchActive This bit is set if the touch
                              sensor is active.
  
  @param EFI_ABS_AltActive    This bit is set if the alt sensor,
                              such as pen-side button, is
                              active.
**/
#define EFI_ABSP_TouchActive  0x00000001
#define EFI_ABS_AltActive     0x00000002


/**
  Definition of EFI_ABSOLUTE_POINTER_STATE.
  
  @param CurrentX The unsigned position of the activation on the
                  x axis If the AboluteMinX and the AboluteMaxX
                  fields of the EFI_ABSOLUTE_POINTER_MODE
                  structure are both 0, then this pointer device
                  does not support an x-axis, and this field
                  must be ignored.

  @param CurrentY The unsigned position of the activation on the
                  x axis If the AboluteMinY and the AboluteMaxY
                  fields of the EFI_ABSOLUTE_POINTER_MODE
                  structure are both 0, then this pointer device
                  does not support an y-axis, and this field
                  must be ignored.

  @param CurrentZ The unsigned position of the activation on the
                  x axis, or the pressure measurement. If the
                  AboluteMinZ and the AboluteMaxZ fields of the
                  EFI_ABSOLUTE_POINTER_MODE structure are
                  both 0, then this pointer device does not
                  support an z-axis, and this field must be
                  ignored.

  @param ActiveButtons Bits are set to 1 in this structure item
                       to indicate that device buttons are
                       active.

**/
typedef struct {
  UINT64 CurrentX;
  UINT64 CurrentY;
  UINT64 CurrentZ;
  UINT32 ActiveButtons;
} EFI_ABSOLUTE_POINTER_STATE;

/**
  The GetState() function retrieves the current state of a pointer
  device. This includes information on the active state associated
  with the pointer device and the current position of the axes
  associated with the pointer device. If the state of the pointer
  device has not changed since the last call to GetState(), then
  EFI_NOT_READY is returned. If the state of the pointer device
  has changed since the last call to GetState(), then the state
  information is placed in State, and EFI_SUCCESS is returned. If
  a device error occurs while attempting to retrieve the state
  information, then EFI_DEVICE_ERROR is returned.


  @param This   A pointer to the EFI_ABSOLUTE_POINTER_PROTOCOL
                instance.

  @param State  A pointer to the state information on the
                pointer device.

  @retval EFI_SUCCESS       The state of the pointer device was
                            returned in State.

  @retval EFI_NOT_READY     The state of the pointer device has not
                            changed since the last call to GetState().

  @retval EFI_DEVICE_ERROR  A device error occurred while
                            attempting to retrieve the pointer
                            device's current state.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_ABSOLUTE_POINTER_GET_STATE)(
  IN 		EFI_ABSOLUTE_POINTER_PROTOCOL  *This,
  IN OUT    EFI_ABSOLUTE_POINTER_STATE *State
);


/**
  @par Protocol Description:
  The EFI_ABSOLUTE_POINTER_PROTOCOL provides a set of services
  for a pointer device that can be used as an input device from an
  application written to this specification. The services include
  the ability to reset the pointer device, retrieve the state of
  the pointer device, and retrieve the capabilities of the pointer
  device. In addition certain data items describing the device are
  provided.


  @param Reset        Resets the pointer device.

  @param GetState     Retrieves the current state of the pointer
                      device.

  @param WaitForInput Event to use with WaitForEvent() to wait
                      for input from the pointer device.
  @param Mode         Pointer to EFI_ABSOLUTE_POINTER_MODE data.

**/
struct _EFI_ABSOLUTE_POINTER_PROTOCOL {
  EFI_ABSOLUTE_POINTER_RESET      Reset;
  EFI_ABSOLUTE_POINTER_GET_STATE  GetState;
  EFI_EVENT                       WaitForInput;
  EFI_ABSOLUTE_POINTER_MODE       *Mode;
};


extern EFI_GUID gEfiAbsolutePointerProtocolGuid;


#endif

