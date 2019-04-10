/** @file
  Provides the parent dispatch service for the periodical timer SMI source generator.

Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  This Protocol is defined in Framework of EFI SMM Core Interface Spec
  Version 0.9.

**/

#ifndef _EFI_SMM_PERIODIC_TIMER_DISPATCH_H_
#define _EFI_SMM_PERIODIC_TIMER_DISPATCH_H_


//
// Global ID for the Periodic Timer SMI Protocol
//
#define EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL_GUID \
  { \
    0x9cca03fc, 0x4c9e, 0x4a19, {0x9b, 0x6, 0xed, 0x7b, 0x47, 0x9b, 0xde, 0x55 } \
  }

typedef struct _EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL  EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL;

//
// Related Definitions
//

typedef struct {
  ///
  /// The minimum period of time that the child gets called, in 100 nanosecond units.
  /// The child will be called back after a time greater than the time Period.
  ///
  UINT64  Period;
  ///
  /// The period of time interval between SMIs.  Children of this interface
  /// should use this field when registering for periodic timer intervals when a finer
  /// granularity periodic SMI is desired.  Valid values for this field are those returned
  /// by GetNextInterval.  A value of 0 indicates the parent is allowed to use any SMI
  /// interval period to satisfy the requested period.
  ///
  UINT64  SmiTickInterval;
  ///
  /// The actual time in 100 nanosecond units elapsed since last called. A
  /// value of 0 indicates an unknown amount of time.
  ///
  UINT64  ElapsedTime;
} EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT;

//
// Member functions
//
/**
  Dispatch function for a Periodic Timer SMI handler.

  @param  DispatchHandle        The handle of this dispatch function.
  @param  DispatchContext       The pointer to the dispatch function's context.
                                The DispatchContext fields are filled in
                                by the dispatching driver prior to
                                invoking this dispatch function.

  @return None

**/
typedef
VOID
(EFIAPI *EFI_SMM_PERIODIC_TIMER_DISPATCH)(
  IN EFI_HANDLE                                DispatchHandle,
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT   *DispatchContext
  );

/**
  Returns the next SMI tick period supported by the chipset.  The order
  returned is from longest to shortest interval period.

  @param  This                  The protocol instance pointer.
  @param  SmiTickInterval       The pointer to pointer of next shorter SMI interval
                                period supported by the child. This parameter works as a get-first,
                                get-next field. The first time this function is called, *SmiTickInterval
                                should be set to NULL to get the longest SMI interval. The returned
                                *SmiTickInterval should be passed in on subsequent calls to get the
                                next shorter interval period until *SmiTickInterval = NULL.

  @retval EFI_SUCCESS           The service returned successfully.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_INTERVAL)(
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL           *This,
  IN OUT UINT64                                         **SmiTickInterval
  );

/**
  Register a child SMI source dispatch function with a parent SMM driver

  @param  This                  The pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param  DispatchFunction      The function to install.
  @param  DispatchContext       The pointer to the dispatch function's context.
                                Indicates to the register
                                function the period at which the dispatch function
                                should be invoked.
  @param  DispatchHandle        The handle generated by the dispatcher to track the function instance.

  @retval EFI_SUCCESS           The dispatch function has been successfully
                                registered, and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR      The driver was unable to enable the SMI source.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory (system or SMM) to manage this
                                child.
  @retval EFI_INVALID_PARAMETER DispatchContext is invalid. The period input value
                                is not within valid range.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_REGISTER)(
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL          *This,
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH                   DispatchFunction,
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_CONTEXT           *DispatchContext,
  OUT EFI_HANDLE                                       *DispatchHandle
  );

/**
  Unregisters a periodic timer service.

  @param  This                  The pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param  DispatchHandle        The handle of the service to remove.

  @retval EFI_SUCCESS           The dispatch function has been successfully
                                unregistered, and the SMI source has been disabled
                                if there are no other registered child dispatch
                                functions for this SMI source.
  @retval EFI_INVALID_PARAMETER The handle is invalid.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_SMM_PERIODIC_TIMER_UNREGISTER)(
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL          *This,
  IN EFI_HANDLE                                        DispatchHandle
  );

//
// Interface structure for the SMM Periodic Timer Dispatch Protocol
//
/**
  Provides the parent dispatch service for the periodical timer SMI source generator.
**/
struct _EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL {
  ///
  /// Installs a child service to be dispatched by this protocol.
  ///
  EFI_SMM_PERIODIC_TIMER_REGISTER   Register;

  ///
  /// Removes a child service dispatched by this protocol.
  ///
  EFI_SMM_PERIODIC_TIMER_UNREGISTER UnRegister;

  ///
  /// Returns the next SMI tick period that is supported by the chipset.
  ///
  EFI_SMM_PERIODIC_TIMER_INTERVAL   GetNextShorterInterval;
};

extern EFI_GUID gEfiSmmPeriodicTimerDispatchProtocolGuid;

#endif
