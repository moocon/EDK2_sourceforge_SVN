/*++

Copyright (c) 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


Module Name:

  Ps2Policy.h
    
Abstract:

  Protocol used for PS/2 Policy definition.

--*/

#ifndef _PS2_POLICY_PROTOCOL_H_
#define _PS2_POLICY_PROTOCOL_H_

EFI_FORWARD_DECLARATION (EFI_PS2_POLICY_PROTOCOL);

#define EFI_PS2_POLICY_PROTOCOL_GUID \
  { \
    0x4df19259, 0xdc71, 0x4d46, 0xbe, 0xf1, 0x35, 0x7b, 0xb5, 0x78, 0xc4, 0x18 \
  }

#define EFI_KEYBOARD_CAPSLOCK   0x0004
#define EFI_KEYBOARD_NUMLOCK    0x0002
#define EFI_KEYBOARD_SCROLLLOCK 0x0001

typedef
EFI_STATUS
(EFIAPI *EFI_PS2_INIT_HARDWARE) (
  IN  EFI_HANDLE              Handle
  );

typedef struct _EFI_PS2_POLICY_PROTOCOL {
  UINT8                 KeyboardLight;
  EFI_PS2_INIT_HARDWARE Ps2InitHardware;
} EFI_PS2_POLICY_PROTOCOL;

extern EFI_GUID gEfiPs2PolicyProtocolGuid;

#endif
