/** @file
  The file defines the protocol to obtain input from the
  ConsoleIn device. The EFI specification requires that the
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL supports the same languages as
  the corresponding

  Copyright (c) 2006 - 2007, Intel Corporation
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#ifndef __SIMPLE_TEXT_IN_EX_H__
#define __SIMPLE_TEXT_IN_EX_H__

#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID \
  {0xdd9e7534, 0x7762, 0x4698, { 0x8c, 0x14, 0xf5, 0x85, 0x17, 0xa6, 0x25, 0xaa } }


typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

/**
  The Reset() function resets the input device hardware. As part
  of initialization process, the firmware/device will make a quick
  but reasonable attempt to verify that the device is functioning.
  If the ExtendedVerification flag is TRUE the firmware may take
  an extended amount of time to verify the device is operating on
  reset. Otherwise the reset operation is to occur as quickly as
  possible. The hardware verification process is not defined by
  this specification and is left up to the platform firmware or
  driver to implement.

  @param This A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              instance. Type EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              is defined in this section.

  @param ExtendedVerification Indicates that the driver may
                              perform a more exhaustive
                              verification operation of the
                              device during reset.


  @retval EFI_SUCCESS The device was reset.
  
  @retval EFI_DEVICE_ERROR  The device is not functioning
                            correctly and could not be reset.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_RESET_EX) (
  IN CONST  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  IN CONST  BOOLEAN                           ExtendedVerification
);


//
// EFI_KEY_TOGGLE_STATE
//
typedef UINT8   EFI_KEY_TOGGLE_STATE;
#define TOGGLE_STATE_VALID    0x80
#define SCROLL_LOCK_ACTIVE    0x01
#define NUM_LOCK_ACTIVE       0x02
#define CAPS_LOCK_ACTIVE      0x04

/**
  Definition of EFI_KEY_STATE

  @param KeyShiftState  Reflects the currently pressed shift
                        modifiers for the input device. The
                        returned value is valid only if the high
                        order bit has been set.

  @param KeyToggleState Reflects the current internal state of
                        various toggled attributes. The returned
                        value is valid only if the high order
                        bit has been set.

**/
typedef struct _EFI_KEY_STATE {
  UINT32                KeyShiftState;
  EFI_KEY_TOGGLE_STATE  KeyToggleState;
} EFI_KEY_STATE;

/**
  Definition of EFI_KEY_DATA.

  @param Key  The EFI scan code and Unicode value returned from
              the input device.

  @param KeyState The current state of various toggled
                  attributes as well as input modifier values.

**/
typedef struct {
  EFI_INPUT_KEY   Key;
  EFI_KEY_STATE   KeyState;
} EFI_KEY_DATA;

//
// Shift State.
//
// Any Shift or Toggle State that is valid should have
// high order bit set.
//
#define SHIFT_STATE_VALID     0x80000000
#define RIGHT_SHIFT_PRESSED   0x00000001
#define LEFT_SHIFT_PRESSED    0x00000002
#define RIGHT_CONTROL_PRESSED 0x00000004
#define LEFT_CONTROL_PRESSED  0x00000008
#define RIGHT_ALT_PRESSED     0x00000010
#define LEFT_ALT_PRESSED      0x00000020
#define RIGHT_LOGO_PRESSED    0x00000040
#define LEFT_LOGO_PRESSED     0x00000080
#define MENU_KEY_PRESSED      0x00000100
#define SYS_REQ_PRESSED       0x00000200

/**
  The function reads the next keystroke from the input device. If
  there is no pending keystroke the function returns
  EFI_NOT_READY. If there is a pending keystroke, then
  KeyData.Key.ScanCode is the EFI scan code defined in Error!
  Reference source not found.. The KeyData.Key.UnicodeChar is the
  actual printable character or is zero if the key does not
  represent a printable character (control key, function key,
  etc.). The KeyData.KeyState is shift state for the character
  reflected in KeyData.Key.UnicodeChar or KeyData.Key.ScanCode .
  When interpreting the data from this function, it should be
  noted that if a class of printable characters that are
  normally adjusted by shift modifiers (e.g. Shift Key + "f"
  key) would be presented solely as a KeyData.Key.UnicodeChar
  without the associated shift state. So in the previous example
  of a Shift Key + "f" key being pressed, the only pertinent
  data returned would be KeyData.Key.UnicodeChar with the value
  of "F". This of course would not typically be the case for
  non-printable characters such as the pressing of the Right
  Shift Key + F10 key since the corresponding returned data
  would be reflected both in the KeyData.KeyState.KeyShiftState
  and KeyData.Key.ScanCode values. UEFI drivers which implement
  the EFI_SIMPLE_TEXT_INPUT_EX protocol are required to return
  KeyData.Key and KeyData.KeyState values. These drivers must
  always return the most current state of
  KeyData.KeyState.KeyShiftState and
  KeyData.KeyState.KeyToggleState. It should also be noted that
  certain input devices may not be able to produce shift or toggle
  state information, and in those cases the high order bit in the
  respective Toggle and Shift state fields should not be active.

  
  @param This   A pointer to the
                EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX instance.

  @param KeyData  A pointer to a buffer that is filled in with
                  the keystroke state data for the key that was
                  pressed.

  
  @retval EFI_SUCCESS     The keystroke information was
                          returned.
  
  @retval EFI_NOT_READY   There was no keystroke data available.
                          EFI_DEVICE_ERROR The keystroke
                          information was not returned due to
                          hardware errors.


**/
typedef
EFI_STATUS
(EFIAPI *EFI_INPUT_READ_KEY_EX) (
  IN CONST  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT       EFI_KEY_DATA                      *KeyData
);

/**
  The SetState() function allows the input device hardware to
  have state settings adjusted.
  
  @param This A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              instance. Type EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              is defined in this section.
  
  @param KeyToggleState Pointer to the EFI_KEY_TOGGLE_STATE to
                        set the state for the input device.
  
  
  @retval EFI_SUCCESS The device state was set appropriately.

  @retval EFI_DEVICE_ERROR  The device is not functioning
                            correctly and could not have the
                            setting adjusted.

  @retval EFI_UNSUPPORTED   The device does not support the
                            ability to have its state set.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SET_STATE) (
  IN CONST  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  IN CONST  EFI_KEY_TOGGLE_STATE              *KeyToggleState
);

//
// EFI_KEY_NOTIFY
//
typedef
EFI_STATUS
(EFIAPI *EFI_KEY_NOTIFY_FUNCTION) (
  IN CONST  EFI_KEY_DATA *KeyData
);

/**
  The RegisterKeystrokeNotify() function registers a function
  which will be called when a specified keystroke will occur.
  
  @param This A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              instance. Type EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              is defined in this section.
  
  @param KeyData  A pointer to a buffer that is filled in with
                  the keystroke information for the key that was
                  pressed.
  
  @param KeyNotificationFunction  Points to the function to be
                                  called when the key sequence
                                  is typed specified by KeyData.
  
  
  @param NotifyHandle Points to the unique handle assigned to
                      the registered notification.
  
  @retval EFI_SUCCESS           The device state was set
                                appropriately.

  @retval EFI_OUT_OF_RESOURCES  Unable to allocate necessary
                                data structures.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_REGISTER_KEYSTROKE_NOTIFY) (
  IN CONST  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  IN CONST  EFI_KEY_DATA            *KeyData,
  IN CONST  EFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction,
  OUT       EFI_HANDLE              *NotifyHandle
);

/**
  The UnregisterKeystrokeNotify() function removes the
  notification which was previously registered.
  
  @param This A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL_EX
              instance.
  
  @param NotificationHandle The handle of the notification
                            function being unregistered.
  
  @retval EFI_SUCCESS The device state was set appropriately.
  
  @retval EFI_INVALID_PARAMETER The NotificationHandle is
                                invalid.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_UNREGISTER_KEYSTROKE_NOTIFY) (
IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
IN EFI_HANDLE NotificationHandle
);


/**
  The EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL is used on the ConsoleIn
  device. It is an extension to the Simple Text Input protocol
  which allows a variety of extended shift state information to be
  returned.

  @param Reset  Reset the ConsoleIn device. See Reset().

  @param ReadKeyStrokeEx  Returns the next input character. See
                          ReadKeyStrokeEx().

  @param WaitForKeyEx   Event to use with WaitForEvent() to wait
                        for a key to be available.

  @param SetState   Set the EFI_KEY_TOGGLE_STATE state settings
                    for the input device.

  @param RegisterKeyNotify  Register a notification function to
                            be called when a given key sequence
                            is hit.

  @param UnregisterKeyNotifyRemoves   A specific notification
                                      function.

**/
struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL{
  EFI_INPUT_RESET_EX              Reset;
  EFI_INPUT_READ_KEY_EX           ReadKeyStrokeEx;
  EFI_EVENT                       WaitForKeyEx;
  EFI_SET_STATE                   SetState;
  EFI_REGISTER_KEYSTROKE_NOTIFY   RegisterKeyNotify;
  EFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
};


extern EFI_GUID gEfiSimpleTextInputExProtocolGuid;

#endif

