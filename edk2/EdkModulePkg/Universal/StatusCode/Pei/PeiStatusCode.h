
/** @file
  Heade file of status code PEIM

  Copyright (c) 2006, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            
                                                                                            
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  Module Name:  PeiStatusCode.h

**/

#ifndef __PEI_STATUS_CODE_H__
#define __PEI_STATUS_CODE_H__


/**
  Convert status code value and extended data to readable ASCII string, send string to serial I/O device.
 
  @param  CodeType      Indicates the type of status code being reported.  Type EFI_STATUS_CODE_TYPE is defined in "Related Definitions" below.
 
  @param  Value         Describes the current status of a hardware or software entity.  
                        This included information about the class and subclass that is used to classify the entity 
                        as well as an operation.  For progress codes, the operation is the current activity. 
                        For error codes, it is the exception.  For debug codes, it is not defined at this time. 
                        Type EFI_STATUS_CODE_VALUE is defined in "Related Definitions" below.  
                        Specific values are discussed in the Intel? Platform Innovation Framework for EFI Status Code Specification.
 
  @param  Instance      The enumeration of a hardware or software entity within the system.  
                        A system may contain multiple entities that match a class/subclass pairing. 
                        The instance differentiates between them.  An instance of 0 indicates that instance information is unavailable, 
                        not meaningful, or not relevant.  Valid instance numbers start with 1.


  @param  CallerId      This optional parameter may be used to identify the caller. 
                        This parameter allows the status code driver to apply different rules to different callers. 
                        Type EFI_GUID is defined in InstallProtocolInterface() in the EFI 1.10 Specification.


  @param  Data          This optional parameter may be used to pass additional data
 
  @return               The function always return EFI_SUCCESS.

**/
EFI_STATUS
SerialStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  );


/**
  Initialize memory status code.
  Create one GUID'ed HOB with PCD defined size. If create required size 
  GUID'ed HOB failed, then ASSERT().
 
  @return           The function always return EFI_SUCCESS

**/
EFI_STATUS
MemoryStatusCodeInitializeWorker (
  VOID
  );

/**
  Report status code into GUID'ed HOB.
 
  @param  CodeType      Indicates the type of status code being reported.  Type EFI_STATUS_CODE_TYPE is defined in "Related Definitions" below.
 
  @param  Value         Describes the current status of a hardware or software entity.  
                        This included information about the class and subclass that is used to classify the entity 
                        as well as an operation.  For progress codes, the operation is the current activity. 
                        For error codes, it is the exception.  For debug codes, it is not defined at this time. 
                        Type EFI_STATUS_CODE_VALUE is defined in "Related Definitions" below.  
                        Specific values are discussed in the Intel? Platform Innovation Framework for EFI Status Code Specification.
 
  @param  Instance      The enumeration of a hardware or software entity within the system.  
                        A system may contain multiple entities that match a class/subclass pairing. 
                        The instance differentiates between them.  An instance of 0 indicates that instance information is unavailable, 
                        not meaningful, or not relevant.  Valid instance numbers start with 1.
 
  @return               The function always return EFI_SUCCESS.

**/
EFI_STATUS
MemoryStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance
  );

#endif
