/*++

Copyright (c)  1999  - 2014, Intel Corporation. All rights reserved

  SPDX-License-Identifier: BSD-2-Clause-Patent



Module Name:

  Capsule.h

Abstract:

  Capsule PPI definitions.

--*/
//
//
#ifndef _PEI_CAPSULE_PPI_H_
#define _PEI_CAPSULE_PPI_H_

#define PEI_CAPSULE_PPI_GUID \
  { \
    0x3acf33ee, 0xd892, 0x40f4, 0xa2, 0xfc, 0x38, 0x54, 0xd2, 0xe1, 0x32, 0x3d \
  }

EFI_FORWARD_DECLARATION (PEI_CAPSULE_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_CAPSULE_COALESCE) (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN OUT VOID                        **MemoryBase,
  IN OUT UINTN                       *MemSize
  );

typedef
EFI_STATUS
(EFIAPI *PEI_CAPSULE_CREATE_STATE) (
  IN EFI_PEI_SERVICES                                   **PeiServices,
  IN VOID                                               *CapsuleBase, // returned from coalesce
  IN UINTN                              CapsuleSize                   // returned from coalesce
  );

typedef
EFI_STATUS
(EFIAPI *PEI_CAPSULE_CHECK_CAPSULE_UPDATE) (
  IN EFI_PEI_SERVICES           **PeiServices
  );

typedef struct _PEI_CAPSULE_PPI {
  PEI_CAPSULE_COALESCE              Coalesce;
  PEI_CAPSULE_CHECK_CAPSULE_UPDATE  CheckCapsuleUpdate;
  PEI_CAPSULE_CREATE_STATE          CreateState;
} PEI_CAPSULE_PPI;

extern EFI_GUID gPeiCapsulePpiGuid;

#endif // #ifndef _PEI_CAPSULE_PPI_H_
