/** @file
  SMM General Purpose Input (GPI) Dispatch2 Protocol as defined in PI 1.1 Specification
  Volume 4 System Management Mode Core Interface.

  This protocol provides the parent dispatch service for the General Purpose Input 
  (GPI) SMI source generator.

  The EFI_SMM_GPI_DISPATCH2_PROTOCOL provides the ability to install child handlers for the 
  given event types.  Several inputs can be enabled.  This purpose of this interface is to generate an 
  SMI in response to any of these inputs having a true value provided.

  Copyright (c) 2009, Intel Corporation
  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  @par Revision Reference:
  This protocol is from PI Version 1.1.

**/

#ifndef _SMM_GPI_DISPATCH2_H_
#define _SMM_GPI_DISPATCH2_H_

#include <Pi/PiSmmCis.h>

///
/// Note:
///   To avoid name conflict between PI and Framework SMM spec, the following names defined
///   in PI 1.2 SMM spec are renamed. These renamings are not yet in a public PI spec and errta.
///
///   EFI_SMM_GPI_REGISTER   -> EFI_SMM_GPI_REGISTER2
///   EFI_SMM_GPI_UNREGISTER -> EFI_SMM_GPI_UNREGISTER2
///

#define EFI_SMM_GPI_DISPATCH2_PROTOCOL_GUID \
  { \
    0x25566b03, 0xb577, 0x4cbf, {0x95, 0x8c, 0xed, 0x66, 0x3e, 0xa2, 0x43, 0x80 } \
  }

///
/// The dispatch function's context.
///
typedef struct {
  ///
  /// A bit mask of 64 possible GPIs that can generate an SMI.  Bit 0 corresponds to logical 
  /// GPI[0], 1 corresponds to logical GPI[1], and so on.
  ///
  UINT64 GpiNum;
} EFI_SMM_GPI_REGISTER_CONTEXT;

typedef struct _EFI_SMM_GPI_DISPATCH2_PROTOCOL EFI_SMM_GPI_DISPATCH2_PROTOCOL;

/**
  Registers a child SMI source dispatch function with a parent SMM driver.

  This service registers a function (DispatchFunction) which will be called when an SMI is 
  generated because of one or more of the GPIs specified by RegisterContext. On return, 
  DispatchHandle contains a unique handle which may be used later to unregister the function 
  using UnRegister().
  The DispatchFunction will be called with Context set to the same value as was passed into 
  this function in RegisterContext and with CommBuffer pointing to another instance of 
  EFI_SMM_GPI_REGISTER_CONTEXT describing the GPIs which actually caused the SMI and 
  CommBufferSize pointing to the size of the structure.

  @param[in]  This               Pointer to the EFI_SMM_GPI_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchFunction   Function to register for handler when the specified GPI causes an SMI.
  @param[in]  RegisterContext    Pointer to the dispatch function's context.
                                 The caller fills this context in before calling
                                 the register function to indicate to the register
                                 function the GPI(s) for which the dispatch function
                                 should be invoked.
  @param[out] DispatchHandle     Handle generated by the dispatcher to track the
                                 function instance.

  @retval EFI_SUCCESS            The dispatch function has been successfully
                                 registered and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR       The driver was unable to enable the SMI source.
  @retval EFI_INVALID_PARAMETER  RegisterContext is invalid. The GPI input value
                                 is not within valid range.
  @retval EFI_OUT_OF_RESOURCES   There is not enough memory (system or SMM) to manage this child.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_GPI_REGISTER2)(
  IN CONST EFI_SMM_GPI_DISPATCH2_PROTOCOL  *This,
  IN       EFI_SMM_HANDLER_ENTRY_POINT2    DispatchFunction,
  IN CONST EFI_SMM_GPI_REGISTER_CONTEXT    *RegisterContext,
  OUT      EFI_HANDLE                      *DispatchHandle
  );

/**
  Unregisters a General Purpose Input (GPI) service.

  This service removes the handler associated with DispatchHandle so that it will no longer be 
  called when the GPI triggers an SMI. 

  @param[in]  This               Pointer to the EFI_SMM_GPI_DISPATCH2_PROTOCOL instance.
  @param[in]  DispatchHandle     Handle of the service to remove.

  @retval EFI_SUCCESS            Handle of the service to remove.
  @retval EFI_INVALID_PARAMETER  The DispatchHandle was not valid.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_GPI_UNREGISTER2)(
  IN CONST EFI_SMM_GPI_DISPATCH2_PROTOCOL  *This,
  IN       EFI_HANDLE                      DispatchHandle
  );

///
/// Interface structure for the SMM GPI SMI Dispatch Protocol
///
/// The SMM GPI SMI Dispatch Protocol provides the parent dispatch service
/// for the General Purpose Input (GPI) SMI source generator.
///
struct _EFI_SMM_GPI_DISPATCH2_PROTOCOL {
  EFI_SMM_GPI_REGISTER2    Register;
  EFI_SMM_GPI_UNREGISTER2  UnRegister;
  ///
  /// Denotes the maximum value of inputs that can have handlers attached.
  ///
  UINTN                   NumSupportedGpis;
};

extern EFI_GUID gEfiSmmGpiDispatch2ProtocolGuid;

#endif

