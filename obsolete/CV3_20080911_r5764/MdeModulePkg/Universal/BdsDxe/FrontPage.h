/** @file
  FrontPage routines to handle the callbacks and browser calls

Copyright (c) 2004 - 2008, Intel Corporation. <BR>
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _FRONT_PAGE_H_
#define _FRONT_PAGE_H_

#include "DeviceMngr/DeviceManager.h"
#include "BootMaint/BootMaint.h"
#include "BootMngr/BootManager.h"
#include "String.h"

#define ONE_SECOND  10000000

//
// This is the VFR compiler generated header file which defines the
// string identifiers.
//
#define PRINTABLE_LANGUAGE_NAME_STRING_ID     0x0001

//
// These are defined as the same with vfr file
//
#define FRONT_PAGE_FORM_ID             0x1000

#define FRONT_PAGE_KEY_CONTINUE        0x1000
#define FRONT_PAGE_KEY_LANGUAGE        0x1234
#define FRONT_PAGE_KEY_BOOT_MANAGER    0x1064
#define FRONT_PAGE_KEY_DEVICE_MANAGER  0x8567
#define FRONT_PAGE_KEY_BOOT_MAINTAIN   0x9876

#define LABEL_SELECT_LANGUAGE          0x1000

#define FRONT_PAGE_FORMSET_GUID \
  { \
    0x9e0c30bc, 0x3f06, 0x4ba6, 0x82, 0x88, 0x9, 0x17, 0x9b, 0x85, 0x5d, 0xbe \
  }

#define FRONT_PAGE_CALLBACK_DATA_SIGNATURE  EFI_SIGNATURE_32 ('F', 'P', 'C', 'B')

typedef struct {
  UINTN                           Signature;

  //
  // HII relative handles
  //
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HANDLE                      DriverHandle;
  EFI_STRING_ID                   *LanguageToken;

  //
  // Produced protocols
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL  ConfigAccess;
} FRONT_PAGE_CALLBACK_DATA;

#define EFI_FP_CALLBACK_DATA_FROM_THIS(a) \
  CR (a, \
      FRONT_PAGE_CALLBACK_DATA, \
      ConfigAccess, \
      FRONT_PAGE_CALLBACK_DATA_SIGNATURE \
      )

//
// These are the VFR compiler generated data representing our VFR data.
//
extern UINT8  FrontPageVfrBin[];

extern EFI_HII_DATABASE_PROTOCOL       *gHiiDatabase;
extern EFI_HII_STRING_PROTOCOL         *gHiiString;
extern EFI_FORM_BROWSER2_PROTOCOL      *gFormBrowser2;
extern EFI_HII_CONFIG_ROUTING_PROTOCOL *gHiiConfigRouting;

extern UINTN    gCallbackKey;
extern BOOLEAN  gConnectAllHappened;

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         - A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        - On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         - A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
FakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

/**
  This function processes the results of changes in configuration.


  @param This            - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   - A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        - A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
FakeRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

/**
  This function processes the results of changes in configuration.


  @param This            - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          - Specifies the type of action taken by the browser.
  @param QuestionId      - A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            - The type of value for the question.
  @param Value           - A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   - On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
  @retval  EFI_DEVICE_ERROR      The variable could not be saved.
  @retval  EFI_UNSUPPORTED       The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
FrontPageCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

/**
  Initialize HII information for the FrontPage


  @param InitializeHiiData    TRUE if HII elements need to be initialized.

  @retval  EFI_SUCCESS        The operation is successful.
  @retval  EFI_DEVICE_ERROR   If the dynamic opcode creation failed.

**/
EFI_STATUS
InitializeFrontPage (
  IN BOOLEAN    InitializeHiiData
  );

/**
  Acquire the string associated with the ProducerGuid and return it.


  @param ProducerGuid    - The Guid to search the HII database for
  @param Token           - The token value of the string to extract
  @param String          - The string that is extracted

  @retval  EFI_SUCCESS  The function returns EFI_SUCCESS always.

**/
EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  );

/**
  Compare two EFI_TIME data.


  @param FirstTime       - A pointer to the first EFI_TIME data.
  @param SecondTime      - A pointer to the second EFI_TIME data.

  @retval  TRUE              The FirstTime is not later than the SecondTime.
  @retval  FALSE             The FirstTime is later than the SecondTime.

**/
BOOLEAN
TimeCompare (
  IN EFI_TIME               *FirstTime,
  IN EFI_TIME               *SecondTime
  );

/**
  This function is the main entry of the platform setup entry.
  The function will present the main menu of the system setup,
  this is the platform reference part and can be customize.


  @param TimeoutDefault  - The fault time out value before the system
                         continue to boot.
  @param ConnectAllHappened - The indicater to check if the connect all have
                         already happended.

**/
VOID
PlatformBdsEnterFrontPage (
  IN UINT16                 TimeoutDefault,
  IN BOOLEAN                ConnectAllHappened
  );

#endif // _FRONT_PAGE_H_
