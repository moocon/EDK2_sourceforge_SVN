/** @file
  Implementation for PlatformBdsLib library class interfaces.

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available
  under the terms and conditions of the BSD License which accompanies this
  distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, WITHOUT
  WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "IntelBdsPlatform.h"

#include <Library/QemuBootOrderLib.h>

//
// BDS Platform Functions
//
/**
  Platform Bds init. Include the platform firmware vendor, revision
  and so crc check.

**/
VOID
EFIAPI
PlatformBdsInit (
  VOID
  )
{
}

STATIC
EFI_STATUS
GetConsoleDevicePathFromVariable (
  IN  CHAR16*             ConsoleVarName,
  IN  CHAR16*             DefaultConsolePaths,
  OUT EFI_DEVICE_PATH**   DevicePaths
  )
{
  EFI_STATUS                Status;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL* DevicePathInstances;
  EFI_DEVICE_PATH_PROTOCOL* DevicePathInstance;
  CHAR16*                   DevicePathStr;
  CHAR16*                   NextDevicePathStr;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL  *EfiDevicePathFromTextProtocol;

  Status = GetGlobalEnvironmentVariable (ConsoleVarName, NULL, NULL, (VOID**)&DevicePathInstances);
  if (EFI_ERROR (Status)) {
    // In case no default console device path has been defined we assume a driver handles the console (eg: SimpleTextInOutSerial)
    if ((DefaultConsolePaths == NULL) || (DefaultConsolePaths[0] == L'\0')) {
      *DevicePaths = NULL;
      return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (&gEfiDevicePathFromTextProtocolGuid, NULL, (VOID **)&EfiDevicePathFromTextProtocol);
    ASSERT_EFI_ERROR (Status);

    DevicePathInstances = NULL;

    // Extract the Device Path instances from the multi-device path string
    while ((DefaultConsolePaths != NULL) && (DefaultConsolePaths[0] != L'\0')) {
      NextDevicePathStr = StrStr (DefaultConsolePaths, L";");
      if (NextDevicePathStr == NULL) {
        DevicePathStr = DefaultConsolePaths;
        DefaultConsolePaths = NULL;
      } else {
        DevicePathStr = (CHAR16*)AllocateCopyPool ((NextDevicePathStr - DefaultConsolePaths + 1) * sizeof (CHAR16), DefaultConsolePaths);
        *(DevicePathStr + (NextDevicePathStr - DefaultConsolePaths)) = L'\0';
        DefaultConsolePaths = NextDevicePathStr;
        if (DefaultConsolePaths[0] == L';') {
          DefaultConsolePaths++;
        }
      }

      DevicePathInstance = EfiDevicePathFromTextProtocol->ConvertTextToDevicePath (DevicePathStr);
      ASSERT (DevicePathInstance != NULL);
      DevicePathInstances = AppendDevicePathInstance (DevicePathInstances, DevicePathInstance);

      if (NextDevicePathStr != NULL) {
        FreePool (DevicePathStr);
      }
      FreePool (DevicePathInstance);
    }

    // Set the environment variable with this device path multi-instances
    Size = GetDevicePathSize (DevicePathInstances);
    if (Size > 0) {
      gRT->SetVariable (
          ConsoleVarName,
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          Size,
          DevicePathInstances
          );
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }

  if (!EFI_ERROR (Status)) {
    *DevicePaths = DevicePathInstances;
  }
  return Status;
}

STATIC
VOID
SetConsoleVariables (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH*          ConOutDevicePaths;
  EFI_DEVICE_PATH*          ConInDevicePaths;
  EFI_DEVICE_PATH*          ConErrDevicePaths;

  // By getting the Console Device Paths from the environment variables before initializing the console pipe, we
  // create the 3 environment variables (ConIn, ConOut, ConErr) that allows to initialize all the console interface
  // of newly installed console drivers
  Status = GetConsoleDevicePathFromVariable (L"ConOut", (CHAR16*)PcdGetPtr (PcdDefaultConOutPaths), &ConOutDevicePaths);
  ASSERT_EFI_ERROR (Status);
  Status = GetConsoleDevicePathFromVariable (L"ConIn", (CHAR16*)PcdGetPtr (PcdDefaultConInPaths), &ConInDevicePaths);
  ASSERT_EFI_ERROR (Status);
  Status = GetConsoleDevicePathFromVariable (L"ErrOut", (CHAR16*)PcdGetPtr (PcdDefaultConOutPaths), &ConErrDevicePaths);
  ASSERT_EFI_ERROR (Status);
}

/**
  Connect with predefined platform connect sequence,
  the OEM/IBV can customize with their own connect sequence.
**/
VOID
PlatformBdsConnectSequence (
  VOID
  )
{
}

/**
  Load the predefined driver option, OEM/IBV can customize this
  to load their own drivers

  @param BdsDriverLists  - The header of the driver option link list.

**/
VOID
PlatformBdsGetDriverOption (
  IN OUT LIST_ENTRY              *BdsDriverLists
  )
{
}

/**
  Perform the platform diagnostic, such like test memory. OEM/IBV also
  can customize this function to support specific platform diagnostic.

  @param MemoryTestLevel  The memory test intensive level
  @param QuietBoot        Indicate if need to enable the quiet boot
  @param BaseMemoryTest   A pointer to BdsMemoryTest()

**/
VOID
PlatformBdsDiagnostics (
  IN EXTENDMEM_COVERAGE_LEVEL    MemoryTestLevel,
  IN BOOLEAN                     QuietBoot,
  IN BASEM_MEMORY_TEST           BaseMemoryTest
  )
{
}

/**
  The function will execute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.

  @param  DriverOptionList        The header of the driver option link list
  @param  BootOptionList          The header of the boot option link list
  @param  ProcessCapsules         A pointer to ProcessCapsules()
  @param  BaseMemoryTest          A pointer to BaseMemoryTest()

**/
VOID
EFIAPI
PlatformBdsPolicyBehavior (
  IN LIST_ENTRY                      *DriverOptionList,
  IN LIST_ENTRY                      *BootOptionList,
  IN PROCESS_CAPSULES                ProcessCapsules,
  IN BASEM_MEMORY_TEST               BaseMemoryTest
  )
{
  SetConsoleVariables ();
  BdsLibConnectAll ();

  //
  // Process QEMU's -kernel command line option. Note that the kernel booted
  // this way should receive ACPI tables, which is why we connect all devices
  // first (see above) -- PCI enumeration blocks ACPI table installation, if
  // there is a PCI host.
  //
  TryRunningQemuKernel ();

  BdsLibEnumerateAllBootOption (BootOptionList);
  SetBootOrderFromQemu (BootOptionList);
  //
  // The BootOrder variable may have changed, reload the in-memory list with
  // it.
  //
  BdsLibBuildOptionFromVar (BootOptionList, L"BootOrder");

  PlatformBdsEnterFrontPage (GetFrontPageTimeoutFromQemu(), TRUE);
}

/**
  Hook point after a boot attempt succeeds. We don't expect a boot option to
  return, so the UEFI 2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in this case. This
  is also a platform implementation and can be customized by IBV/OEM.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
PlatformBdsBootSuccess (
  IN  BDS_COMMON_OPTION *Option
  )
{
}

/**
  Hook point after a boot attempt fails.

  @param  Option                  Pointer to Boot Option that failed to boot.
  @param  Status                  Status returned from failed boot.
  @param  ExitData                Exit data returned from failed boot.
  @param  ExitDataSize            Exit data size returned from failed boot.

**/
VOID
EFIAPI
PlatformBdsBootFail (
  IN  BDS_COMMON_OPTION  *Option,
  IN  EFI_STATUS         Status,
  IN  CHAR16             *ExitData,
  IN  UINTN              ExitDataSize
  )
{
}

/**
  This function locks platform flash that is not allowed to be updated during normal boot path.
  The flash layout is platform specific.
**/
VOID
EFIAPI
PlatformBdsLockNonUpdatableFlash (
  VOID
  )
{
  return;
}


/**
  Lock the ConsoleIn device in system table. All key
  presses will be ignored until the Password is typed in. The only way to
  disable the password is to type it in to a ConIn device.

  @param  Password        Password used to lock ConIn device.

  @retval EFI_SUCCESS     lock the Console In Spliter virtual handle successfully.
  @retval EFI_UNSUPPORTED Password not found

**/
EFI_STATUS
EFIAPI
LockKeyboards (
  IN  CHAR16    *Password
  )
{
    return EFI_UNSUPPORTED;
}
