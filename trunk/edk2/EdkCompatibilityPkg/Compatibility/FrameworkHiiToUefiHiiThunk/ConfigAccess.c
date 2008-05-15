/**@file
  This file contains functions related to Config Access Protocols installed by
  by HII Thunk Modules which is used to thunk UEFI Config Access Callback to 
  Framework HII Callback.
  
Copyright (c) 2008, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "HiiDatabase.h"

HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE ConfigAccessProtocolInstanceTempate = {
  HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE_SIGNATURE,
  {
    ThunkExtractConfig,
    ThunkRouteConfig,
    ThunkCallback
  }, //ConfigAccessProtocol
  NULL, //FrameworkFormCallbackProtocol
  {NULL, NULL} //ConfigAccessStorageListHead
};

/**
   Find and return the pointer to Package Header of the Form package
   in the Framework Package List. The Framework Package List is created
   by a module calling the Framework HII interface.
   The Framwork Package List contains package data 
   generated by Intel's UEFI VFR Compiler and String gather tool. The data format
   of the package data is defined by  TIANO_AUTOGEN_PACKAGES_HEADER.

   If the package list contains other type of packages such as KEYBOARD_LAYOUT,
   FONTS and IMAGES, the ASSERT. This is to make sure the caller is a 
   Framework Module which does not include packages introduced by UEFI Specification
   or packages that is not supported by Thunk layer.
    
   @param Packages    The Framework Package List
   
   @retval EFI_HII_PACKAGE_HEADER* Return the Package Header of
                                                          Form Package.
   @retval NULL                                    If no Form Package is found.
**/
EFI_HII_PACKAGE_HEADER *
GetIfrFormSet (
  IN  CONST EFI_HII_PACKAGES  *Packages
  )
{
  TIANO_AUTOGEN_PACKAGES_HEADER **TianoAutogenPackageHdrArray;
  EFI_HII_PACKAGE_HEADER        *IfrPackage;
  UINTN                         Index;

  ASSERT (Packages != NULL);

  IfrPackage = NULL;

  TianoAutogenPackageHdrArray = (TIANO_AUTOGEN_PACKAGES_HEADER **) (((UINT8 *) &Packages->GuidId) + sizeof (Packages->GuidId));
  for (Index = 0; Index < Packages->NumberOfPackages; Index++) {
    //
    // BugBug: The current UEFI HII build tool generate a binary in the format defined in: 
    // TIANO_AUTOGEN_PACKAGES_HEADER. We assume that all packages generated in
    // this binary is with same package type. So the returned IfrPackNum and StringPackNum
    // may not be the exact number of valid package number in the binary generated 
    // by HII Build tool.
    //
    switch (TianoAutogenPackageHdrArray[Index]->PackageHeader.Type) {
      case EFI_HII_PACKAGE_FORM:
        return &TianoAutogenPackageHdrArray[Index]->PackageHeader;
        break;

      case EFI_HII_PACKAGE_STRINGS:
      case EFI_HII_PACKAGE_SIMPLE_FONTS:
        break;

      //
      // The following fonts are invalid for a module that using Framework to UEFI thunk layer.
      //
      case EFI_HII_PACKAGE_KEYBOARD_LAYOUT:
      case EFI_HII_PACKAGE_FONTS:
      case EFI_HII_PACKAGE_IMAGES:
      default:
        ASSERT (FALSE);
        break;
    }
  }

  return (EFI_HII_PACKAGE_HEADER *) NULL;
}

/**
  This function scan EFI_IFR_VARSTORE_OP in the Form Package.
  It create entries for these VARSTORE found and append the entry
  to a Link List.

  If FormSetPackage is not EFI_HII_PACKAGE_FORM, then ASSERT.
  If there is no linear buffer storage in this formset, then ASSERT.

   @param FormSetPackage            The Form Package header.
   @param BufferStorageListHead    The link list for the VARSTORE found in the form package.
   
   @retval  EFI_SUCCESS                     The function scan the form set and find one or more 
                                                          VARSTOREs.
   @retval  EFI_OUT_OF_RESOURCES    There is not enough memory to complete the function.
**/
EFI_STATUS
GetBufferStorage  (
  IN  CONST EFI_HII_PACKAGE_HEADER *FormSetPackage,
  OUT       LIST_ENTRY             *BufferStorageListHead
  )
{
  UINTN                   OpCodeOffset;
  UINTN                   OpCodeLength;
  UINT8                   *OpCodeData;
  UINT8                   Operand;
  EFI_IFR_VARSTORE        *VarStoreOpCode;
  HII_TRHUNK_BUFFER_STORAGE_KEY *BufferStorageKey;

  ASSERT (FormSetPackage->Type == EFI_HII_PACKAGE_FORM);

  OpCodeOffset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (OpCodeOffset < FormSetPackage->Length) {
    OpCodeData = (UINT8 *) FormSetPackage + OpCodeOffset;

    OpCodeLength = ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
    OpCodeOffset += OpCodeLength;
    Operand = ((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode;

    if (Operand == EFI_IFR_VARSTORE_OP) {
      VarStoreOpCode = (EFI_IFR_VARSTORE *)OpCodeData;
      BufferStorageKey = AllocateZeroPool (sizeof (*BufferStorageKey));
      if (BufferStorageKey == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (&BufferStorageKey->Guid, &VarStoreOpCode->Guid, sizeof (EFI_GUID));
      
      BufferStorageKey->Name = AllocateZeroPool (AsciiStrSize (VarStoreOpCode->Name) * 2);
      AsciiStrToUnicodeStr (VarStoreOpCode->Name, BufferStorageKey->Name);

      BufferStorageKey->VarStoreId = VarStoreOpCode->VarStoreId;

      BufferStorageKey->Size = VarStoreOpCode->Size;
      BufferStorageKey->Signature = HII_TRHUNK_BUFFER_STORAGE_KEY_SIGNATURE;

      InsertTailList (BufferStorageListHead, &BufferStorageKey->List);
    }
  }

  return EFI_SUCCESS;
}
  
/**
  This function installs a EFI_CONFIG_ACCESS_PROTOCOL instance for a form package registered
  by a module using Framework HII Protocol Interfaces.

   UEFI HII require EFI_HII_CONFIG_ACCESS_PROTOCOL to be installed on a EFI_HANDLE, so
   that Setup Utility can load the Buffer Storage using this protocol.
   
   @param Packages          The framework package list.
   @param MapEntry          The Thunk Layer Handle Mapping Database Entry.
   
   @retval  EFI_SUCCESS             The Config Access Protocol is installed successfully.
   @retval  EFI_OUT_RESOURCE    There is not enough memory.
   
**/
EFI_STATUS
InstallDefaultUefiConfigAccessProtocol (
  IN  CONST EFI_HII_PACKAGES                         *Packages,
  IN  OUT   HII_TRHUNK_HANDLE_MAPPING_DATABASE_ENTRY *MapEntry
  )
{
  EFI_HII_PACKAGE_HEADER                      *FormSetPackage;
  EFI_STATUS                                  Status;
  HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE  *ConfigAccessInstance;

  Status = HiiLibCreateHiiDriverHandle (&MapEntry->UefiHiiDriverHandle);
  ConfigAccessInstance = AllocateCopyPool (
                           sizeof (HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE), 
                           &ConfigAccessProtocolInstanceTempate
                           );
  ASSERT (ConfigAccessInstance != NULL);
  InitializeListHead (&ConfigAccessInstance->ConfigAccessBufferStorageListHead);

  //
  // We assume there is only one formset package in each Forms Package
  //
  FormSetPackage = GetIfrFormSet (Packages);
  ASSERT (FormSetPackage != NULL);
  
  Status = GetBufferStorage (FormSetPackage, &ConfigAccessInstance->ConfigAccessBufferStorageListHead);
  if (EFI_ERROR (Status)) {
    FreePool (ConfigAccessInstance);
    ASSERT (FALSE);
    return Status;
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
          &MapEntry->UefiHiiDriverHandle,
          &gEfiHiiConfigAccessProtocolGuid,
          &ConfigAccessInstance->ConfigAccessProtocol,
          NULL
          );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    FreePool  (ConfigAccessInstance);
    return Status;
  }
  
  return EFI_SUCCESS;
}

/**

   Wrap EFI_HII_CONFIG_ACCESS_PROTOCOL.RouteConfig to a call to EFI_FORM_CALLBACK_PROTOCOL.NvWrite.
   
   @param BufferStorageKey               The key with all attributes needed to call EFI_FORM_CALLBACK_PROTOCOL.NvWrite.
   @param FrameworkFormCallBack     The EFI_FORM_CALLBACK_PROTOCOL registered by Framework HII module.
   @param Data                                  The data to be saved.
   @param DataSize                            The size of data.
   
   @retval EFI_STATUS                       The status returned by the EFI_FORM_CALLBACK_PROTOCOL.NvWrite.
 **/
EFI_STATUS
RouteConfigToFrameworkFormCallBack (
  IN       HII_TRHUNK_BUFFER_STORAGE_KEY              *BufferStorageKey,
  IN       EFI_FORM_CALLBACK_PROTOCOL                 *FrameworkFormCallBack,
  IN       VOID                                       *Data,
  IN       UINTN                                      DataSize
  )
{
  EFI_STATUS          Status;
  BOOLEAN             ResetRequired;

  Status = FrameworkFormCallBack->NvWrite (
              FrameworkFormCallBack,  
              BufferStorageKey->Name,
              &BufferStorageKey->Guid,
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
              DataSize,
              Data,
              &ResetRequired
              );
  return Status;
}

/**
   Wrap the EFI_HII_CONFIG_ACCESS_PROTOCOL.ExtractConfig to a call to EFI_FORM_CALLBACK_PROTOCOL.NvRead.
   
   @param BufferStorageKey               The key with all attributes needed to call EFI_FORM_CALLBACK_PROTOCOL.NvRead.
   @param FrameworkFormCallBack     The EFI_FORM_CALLBACK_PROTOCOL registered by Framework HII module.
   @param Data                                  The data read.
   @param DataSize                            The size of data.
   
   @retval EFI_STATUS                       The status returned by the EFI_FORM_CALLBACK_PROTOCOL.NvWrite.
   @retval EFI_INVALID_PARAMETER   If the EFI_FORM_CALLBACK_PROTOCOL.NvRead return the size information of the data
                                                        does not match what has been recorded early in he HII_TRHUNK_BUFFER_STORAGE_KEY.
 **/
EFI_STATUS
ExtractConfigFromFrameworkFormCallBack (
  IN       HII_TRHUNK_BUFFER_STORAGE_KEY              *BufferStorageKey,
  IN       EFI_FORM_CALLBACK_PROTOCOL                 *FrameworkFormCallBack,
  OUT      VOID                                       **Data,
  OUT      UINTN                                      *DataSize
  )
{
  EFI_STATUS          Status;

  *DataSize = 0;
  *Data     = NULL;
  
  Status = FrameworkFormCallBack->NvRead (
              FrameworkFormCallBack,  
              BufferStorageKey->Name,
              &BufferStorageKey->Guid,
              NULL,
              DataSize,
              *Data
              );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    if (BufferStorageKey->Size != *DataSize) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    *Data = AllocateZeroPool (*DataSize);
    if (Data == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    FrameworkFormCallBack->NvRead (
                  FrameworkFormCallBack,  
                  BufferStorageKey->Name,
                  &BufferStorageKey->Guid,
                  NULL,
                  DataSize,
                  *Data
                  );
  }

  return Status;
}

/**
   Wrap the EFI_HII_CONFIG_ACCESS_PROTOCOL.ExtractConfig to a call to UEFI Variable Set Service.
   
   @param BufferStorageKey               The key with all attributes needed to call a UEFI Variable Get Service.
   @param Data                                  The data read.
   @param DataSize                            The size of data.

   @retval EFI_STATUS                       The status returned by the UEFI Variable Set Service.
   
 **/
EFI_STATUS
RouteConfigToUefiVariable (
  IN       HII_TRHUNK_BUFFER_STORAGE_KEY              *BufferStorageKey,
  IN       VOID                                       *Data,
  IN       UINTN                                      DataSize
  )
{
  return gRT->SetVariable (
                  BufferStorageKey->Name,
                  &BufferStorageKey->Guid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  DataSize,
                  Data
                  );
  
}

/**
   Wrap the EFI_HII_CONFIG_ACCESS_PROTOCOL.ExtractConfig to a call to UEFI Variable Get Service.
   
   @param BufferStorageKey               The key with all attributes needed to call a UEFI Variable Get Service.
   @param Data                                  The data read.
   @param DataSize                            The size of data.

   If the UEFI Variable Get Service return the size information of the data
   does not match what has been recorded early in he HII_TRHUNK_BUFFER_STORAGE_KEY.
   then ASSERT.
                                                        
   @retval EFI_STATUS                       The status returned by the UEFI Variable Get Service.
   @retval EFI_INVALID_PARAMETER   If the UEFI Variable Get Service return the size information of the data
                                                        does not match what has been recorded early in he HII_TRHUNK_BUFFER_STORAGE_KEY.
 **/

EFI_STATUS
ExtractConfigFromUefiVariable (
  IN       HII_TRHUNK_BUFFER_STORAGE_KEY              *BufferStorageKey,
  OUT      VOID                                       **Data,
  OUT      UINTN                                      *DataSize
  )
{
  EFI_STATUS          Status;

  *DataSize = 0;
  *Data = NULL;
  Status = gRT->GetVariable (
              BufferStorageKey->Name,
              &BufferStorageKey->Guid,
              NULL,
              DataSize,
              *Data
              );
  if (Status == EFI_BUFFER_TOO_SMALL) {

    if (BufferStorageKey->Size != *DataSize) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }

    *Data = AllocateZeroPool (*DataSize);
    if (Data == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = gRT->GetVariable (
                BufferStorageKey->Name,
                &BufferStorageKey->Guid,
                NULL,
                DataSize,
                *Data
                );
  }

  return Status;
}

/**

  This function implement the EFI_HII_CONFIG_ACCESS_PROTOCOL.ExtractConfig
  so that data can be read from the data storage such as UEFI Variable or module's
  customized storage exposed by EFI_FRAMEWORK_CALLBACK.

   @param This          Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL
   @param Request     A null-terminated Unicode string in <ConfigRequest> format. Note that this
                                includes the routing information as well as the configurable name / value pairs. It is
                                invalid for this string to be in <MultiConfigRequest> format.

   @param Progress   On return, points to a character in the Request string. Points to the string��s null
                               terminator if request was successful. Points to the most recent ��&�� before the first
                                failing name / value pair (or the beginning of the string if the failure is in the first
                                name / value pair) if the request was not successful
   @param Results     A null-terminated Unicode string in <ConfigAltResp> format which has all
                              values filled in for the names in the Request string. String to be allocated by the called
                              function.
   
   @retval EFI_INVALID_PARAMETER   If there is no Buffer Storage for this Config Access instance.
   @retval EFI_SUCCESS                    The setting is retrived successfully.
   @retval !EFI_SUCCESS                  The error returned by UEFI Get Variable or Framework Form Callback Nvread.
 **/
EFI_STATUS
EFIAPI
ThunkExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                                  Status;
  HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE  *ConfigaAccessInstance;
  LIST_ENTRY                                  *ListEntry;
  HII_TRHUNK_BUFFER_STORAGE_KEY               *BufferStorageKey;
  VOID                                        *Data;
  UINTN                                       DataSize;

  Data = NULL;
  ConfigaAccessInstance = HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE_FROM_PROTOCOL (This);

  //
  // For now, only one var varstore is supported so that we don't need to parse the Configuration string.
  //
  ListEntry = GetFirstNode (&ConfigaAccessInstance->ConfigAccessBufferStorageListHead);
  if (ListEntry == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  
  BufferStorageKey = HII_TRHUNK_BUFFER_STORAGE_KEY_FROM_LIST_ENTRY (ListEntry);

  if (ConfigaAccessInstance->FrameworkFormCallbackProtocol == NULL ||
      ConfigaAccessInstance->FrameworkFormCallbackProtocol->NvRead == NULL) {
    Status = ExtractConfigFromUefiVariable (
               BufferStorageKey,
               &Data,
               &DataSize
               );
  } else {
    Status = ExtractConfigFromFrameworkFormCallBack (
               BufferStorageKey,
               ConfigaAccessInstance->FrameworkFormCallbackProtocol,
                &Data,
                &DataSize
               );
  }
  
  if (!EFI_ERROR (Status)) {
    Status = mHiiConfigRoutingProtocol->BlockToConfig (
                                            mHiiConfigRoutingProtocol,
                                            Request,
                                            Data,
                                            DataSize,
                                            Results,
                                            Progress
                                            );
  }

  SafeFreePool (Data);
  return Status;
}

/**

  This function implement the EFI_HII_CONFIG_ACCESS_PROTOCOL.RouteConfig
  so that data can be written to the data storage such as UEFI Variable or module's
  customized storage exposed by EFI_FRAMEWORK_CALLBACK.
   
   @param This          Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL
   @param Configuration A null-terminated Unicode string in <ConfigResp> format.
   @param a pointer to a string filled in with the offset of the most recent ��&�� before the first
                failing name / value pair (or the beginning of the string if the failure is in the first
                name / value pair) or the terminating NULL if all was successful.
   
   @retval EFI_INVALID_PARAMETER   If there is no Buffer Storage for this Config Access instance.
   @retval EFI_SUCCESS                    The setting is saved successfully.
   @retval !EFI_SUCCESS                  The error returned by UEFI Set Variable or Framework Form Callback Nvwrite.
**/   
EFI_STATUS
EFIAPI
ThunkRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                                  Status;
  HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE  *ConfigaAccessInstance;
  LIST_ENTRY                                  *ListEntry;
  HII_TRHUNK_BUFFER_STORAGE_KEY               *BufferStorageKey;
  VOID                                        *Data;
  UINTN                                       DataSize;
  UINTN                                       LastModifiedByteIndex;

  Data = NULL;
  ConfigaAccessInstance = HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE_FROM_PROTOCOL (This);

  //
  // For now, only one var varstore is supported so that we don't need to parse the Configuration string.
  //
  ListEntry = GetFirstNode (&ConfigaAccessInstance->ConfigAccessBufferStorageListHead);
  if (ListEntry == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  BufferStorageKey = HII_TRHUNK_BUFFER_STORAGE_KEY_FROM_LIST_ENTRY (ListEntry);

  Data = AllocateZeroPool (BufferStorageKey->Size);
  if (Data == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = mHiiConfigRoutingProtocol->ConfigToBlock (
                                          mHiiConfigRoutingProtocol,
                                          Configuration,
                                          Data,
                                          &LastModifiedByteIndex,
                                          Progress
                                          );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

  DataSize = BufferStorageKey->Size;
  if (ConfigaAccessInstance->FrameworkFormCallbackProtocol == NULL ||
      ConfigaAccessInstance->FrameworkFormCallbackProtocol->NvRead == NULL) {
    Status = RouteConfigToUefiVariable (
               BufferStorageKey,
               Data,
               DataSize
               );
  } else {
    Status = RouteConfigToFrameworkFormCallBack (
               BufferStorageKey,
               ConfigaAccessInstance->FrameworkFormCallbackProtocol,
               Data,
               DataSize
               );
  }

Done:  
  SafeFreePool (Data);
  return Status;
}

/**
  Wrap the EFI_HII_CONFIG_ACCESS_PROTOCOL.CallBack to EFI_FORM_CALLBACK_PROTOCOL.Callback. Therefor,
  the framework HII module willl do no porting (except some porting works needed for callback for EFI_ONE_OF_OPTION opcode)
  and still work with a UEFI HII SetupBrowser.
   
   @param This                          Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
   @param Action                       Specifies the type of action taken by the browser. See EFI_BROWSER_ACTION_x.
   @param QuestionId                A unique value which is sent to the original exporting driver so that it can identify the
                                                type of data to expect. The format of the data tends to vary based on the opcode that
                                                generated the callback.
   @param Type                         The type of value for the question. See EFI_IFR_TYPE_x in
                                                EFI_IFR_ONE_OF_OPTION.
   @param Value                        A pointer to the data being sent to the original exporting driver. The type is specified
                                                by Type. Type EFI_IFR_TYPE_VALUE is defined in
                                                EFI_IFR_ONE_OF_OPTION.
   @param ActionRequest             On return, points to the action requested by the callback function. Type
                                                EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form
                                                Browser Protocol.
   
   @retval EFI_UNSUPPORTED      If the Framework HII module does not register Callback although it specify the opcode under
                                                focuse to be INTERRACTIVE.
   @retval EFI_SUCCESS            The callback complete successfully.
   @retval !EFI_SUCCESS           The error code returned by EFI_FORM_CALLBACK_PROTOCOL.Callback.
   
 **/
EFI_STATUS
EFIAPI
ThunkCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                  Status;
  HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE  *ConfigaAccessInstance;
  EFI_FORM_CALLBACK_PROTOCOL                  *FrameworkFormCallbackProtocol;
  EFI_HII_CALLBACK_PACKET                     *Packet;
  FRAMEWORK_EFI_IFR_DATA_ARRAY                Data;
  FRAMEWORK_EFI_IFR_DATA_ENTRY                *DataEntry;
  EFI_FORM_CALLBACK                           Callback; 

  ASSERT (This != NULL);
  ASSERT (Value != NULL);
  ASSERT (ActionRequest != NULL);

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

  ConfigaAccessInstance = HII_TRHUNK_CONFIG_ACCESS_PROTOCOL_INSTANCE_FROM_PROTOCOL (This);

  FrameworkFormCallbackProtocol = ConfigaAccessInstance->FrameworkFormCallbackProtocol;
  if (FrameworkFormCallbackProtocol == NULL) {
    return EFI_UNSUPPORTED;
  }
  Callback = FrameworkFormCallbackProtocol->Callback;

  Status = Callback (
              FrameworkFormCallbackProtocol,
              QuestionId,
              &Data,
              &Packet
              );

  //
  // Callback require browser to perform action
  //
  if (Packet != NULL) {
    if (Packet->DataArray.EntryCount  == 1 && Packet->DataArray.NvRamMap == NULL) {
      DataEntry = (FRAMEWORK_EFI_IFR_DATA_ENTRY *) ((UINT8 *) Packet + sizeof (FRAMEWORK_EFI_IFR_DATA_ARRAY));
      switch (DataEntry->Flags) {
        case EXIT_REQUIRED:
          *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
          break;
        case SAVE_REQUIRED:
          *ActionRequest = EFI_BROWSER_ACTION_REQUEST_SUBMIT;
          break;
        case RESET_REQUIRED:
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            break;
        default:
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
            break;  
      }
    }
  }
  
  return Status;
}

