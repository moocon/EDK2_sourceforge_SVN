/** @file
  Formset guids, form id and VarStore data structure for Boot Maintenance Manager.

Copyright (c) 2004 - 2008, Intel Corporation. <BR>
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _FORM_GUID_H
#define _FORM_GUID_H

#define BOOT_MAINT_FORMSET_GUID \
  { \
  0x642237c7, 0x35d4, 0x472d, {0x83, 0x65, 0x12, 0xe0, 0xcc, 0xf2, 0x7a, 0x22} \
  }

#define FILE_EXPLORE_FORMSET_GUID \
  { \
  0x1f2d63e1, 0xfebd, 0x4dc7, {0x9c, 0xc5, 0xba, 0x2b, 0x1c, 0xef, 0x9c, 0x5b} \
  }

#define FORM_MAIN_ID                         0x1001
#define FORM_BOOT_ADD_ID                     0x1002
#define FORM_BOOT_DEL_ID                     0x1003
#define FORM_BOOT_CHG_ID                     0x1004
#define FORM_DRV_ADD_ID                      0x1005
#define FORM_DRV_DEL_ID                      0x1006
#define FORM_DRV_CHG_ID                      0x1007
#define FORM_CON_MAIN_ID                     0x1008
#define FORM_CON_IN_ID                       0x1009
#define FORM_CON_OUT_ID                      0x100A
#define FORM_CON_ERR_ID                      0x100B
#define FORM_FILE_SEEK_ID                    0x100C
#define FORM_FILE_NEW_SEEK_ID                0x100D
#define FORM_DRV_ADD_FILE_ID                 0x100E
#define FORM_DRV_ADD_HANDLE_ID               0x100F
#define FORM_DRV_ADD_HANDLE_DESC_ID          0x1010
#define FORM_BOOT_NEXT_ID                    0x1011
#define FORM_TIME_OUT_ID                     0x1012
#define FORM_RESET                           0x1013
#define FORM_BOOT_SETUP_ID                   0x1014
#define FORM_DRIVER_SETUP_ID                 0x1015
#define FORM_BOOT_LEGACY_DEVICE_ID           0x1016
#define FORM_CON_COM_ID                      0x1017
#define FORM_CON_COM_SETUP_ID                0x1018
#define FORM_SET_FD_ORDER_ID                 0x1019
#define FORM_SET_HD_ORDER_ID                 0x101A
#define FORM_SET_CD_ORDER_ID                 0x101B
#define FORM_SET_NET_ORDER_ID                0x101C
#define FORM_SET_BEV_ORDER_ID                0x101D
#define FORM_FILE_EXPLORER_ID                0x101E
#define FORM_BOOT_ADD_DESCRIPTION_ID         0x101F
#define FORM_DRIVER_ADD_FILE_DESCRIPTION_ID  0x1020
#define FORM_CON_MODE_ID                     0x1021

#define MAXIMUM_FORM_ID                      0x10FF

#define KEY_VALUE_COM_SET_BAUD_RATE          0x1101
#define KEY_VALUE_COM_SET_DATA_BITS          0x1102
#define KEY_VALUE_COM_SET_STOP_BITS          0x1103
#define KEY_VALUE_COM_SET_PARITY             0x1104
#define KEY_VALUE_COM_SET_TERMI_TYPE         0x1105
#define KEY_VALUE_MAIN_BOOT_NEXT             0x1106
#define KEY_VALUE_BOOT_ADD_DESC_DATA         0x1107
#define KEY_VALUE_BOOT_ADD_OPT_DATA          0x1108
#define KEY_VALUE_DRIVER_ADD_DESC_DATA       0x1109
#define KEY_VALUE_DRIVER_ADD_OPT_DATA        0x110A
#define KEY_VALUE_SAVE_AND_EXIT              0x110B
#define KEY_VALUE_NO_SAVE_AND_EXIT           0x110C
#define KEY_VALUE_BOOT_FROM_FILE             0x110D

#define MAXIMUM_NORMAL_KEY_VALUE             0x11FF

//
// Varstore ID defined for Buffer Stoarge
//
#define VARSTORE_ID_BOOT_MAINT               0x1000
#define VARSTORE_ID_FILE_EXPLORER            0x1001

//
// This is the structure that will be used to store the
// question's current value. Use it at initialize time to
// set default value for each question. When using at run
// time, this map is returned by the callback function,
// so dynamically changing the question's value will be
// possible through this mechanism
//
typedef struct {
  //
  // Three questions displayed at the main page
  // for Timeout, BootNext Variables respectively
  //
  UINT16  BootTimeOut;
  UINT16  BootNext;

  //
  // This is the COM1 Attributes value storage
  //
  UINT8   COM1BaudRate;
  UINT8   COM1DataRate;
  UINT8   COM1StopBits;
  UINT8   COM1Parity;
  UINT8   COM1TerminalType;

  //
  // This is the COM2 Attributes value storage
  //
  UINT8   COM2BaudRate;
  UINT8   COM2DataRate;
  UINT8   COM2StopBits;
  UINT8   COM2Parity;
  UINT8   COM2TerminalType;

  //
  // Driver Option Add Handle page storage
  //
  UINT16  DriverAddHandleDesc[100];
  UINT16  DriverAddHandleOptionalData[100];
  UINT8   DriverAddActive;
  UINT8   DriverAddForceReconnect;

  //
  // Console Input/Output/Errorout using COM port check storage
  //
  UINT8   ConsoleInputCOM1;
  UINT8   ConsoleInputCOM2;
  UINT8   ConsoleOutputCOM1;
  UINT8   ConsoleOutputCOM2;
  UINT8   ConsoleErrorCOM1;
  UINT8   ConsoleErrorCOM2;

  //
  // At most 100 input/output/errorout device for console storage
  //
  UINT8   ConsoleCheck[100];

  //
  // Boot or Driver Option Order storage
  //
  UINT8   OptionOrder[100];
  UINT8   DriverOptionToBeDeleted[100];

  //
  // Boot Option Delete storage
  //
  UINT8   BootOptionDel[100];
  UINT8   DriverOptionDel[100];

  //
  // This is the Terminal Attributes value storage
  //
  UINT8   COMBaudRate;
  UINT8   COMDataRate;
  UINT8   COMStopBits;
  UINT8   COMParity;
  UINT8   COMTerminalType;

  //
  // Legacy Device Order Selection Storage
  //
  UINT8   LegacyFD[100];
  UINT8   LegacyHD[100];
  UINT8   LegacyCD[100];
  UINT8   LegacyNET[100];
  UINT8   LegacyBEV[100];

  //
  // We use DisableMap array to record the enable/disable state of each boot device
  // It should be taken as a bit array, from left to right there are totally 256 bits
  // the most left one stands for BBS table item 0, and the most right one stands for item 256
  // If the bit is 1, it means the boot device has been disabled.
  //
  UINT8   DisableMap[32];

  //
  // Console Output Text Mode
  //
  UINT16  ConsoleOutMode;

  //
  //  UINT16                    PadArea[10];
  //
} BMM_FAKE_NV_DATA;

//
// Key used by File Explorer forms
//
#define KEY_VALUE_SAVE_AND_EXIT_BOOT           0x1000
#define KEY_VALUE_NO_SAVE_AND_EXIT_BOOT        0x1001
#define KEY_VALUE_SAVE_AND_EXIT_DRIVER         0x1002
#define KEY_VALUE_NO_SAVE_AND_EXIT_DRIVER      0x1003

//
// This is the data structure used by File Explorer formset
//
typedef struct {
  UINT16  DescriptionData[75];
  UINT16  OptionalData[127];
  UINT8   Active;
  UINT8   ForceReconnect;
} FILE_EXPLORER_NV_DATA;

#endif

