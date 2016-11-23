/*++

Copyright (c) 2007, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  EdbCommand.h

Abstract:


--*/

#ifndef _EFI_EDB_COMMAND_H_
#define _EFI_EDB_COMMAND_H_

typedef enum {
  EdbWidthUint8,
  EdbWidthUint16,
  EdbWidthUint32,
  EdbWidthUint64,
  EdbWidthMax
} EDB_DATA_WIDTH;

EFI_DEBUGGER_COMMAND
MatchDebuggerCommand (
  IN CHAR16    *CommandName,
  IN CHAR16    **CommandArg
  );

CHAR16 *
GetCommandNameByKey (
  IN EFI_INPUT_KEY CommandKey
  );

//
// Definition for Command Table
//
#define EDB_COMMAND_DEFINE(func) \
EFI_DEBUG_STATUS \
func ( \
  IN     CHAR16                    *CommandArg, \
  IN     EFI_DEBUGGER_PRIVATE_DATA *DebuggerPrivate, \
  IN     EFI_EXCEPTION_TYPE        ExceptionType, \
  IN OUT EFI_SYSTEM_CONTEXT        SystemContext \
  )

EDB_COMMAND_DEFINE (DebuggerCallStack);
EDB_COMMAND_DEFINE (DebuggerInstructionBranch);
EDB_COMMAND_DEFINE (DebuggerBreakOnCALL);
EDB_COMMAND_DEFINE (DebuggerBreakOnCALLEX);
EDB_COMMAND_DEFINE (DebuggerBreakOnRET);
EDB_COMMAND_DEFINE (DebuggerBreakOnEntrypoint);
EDB_COMMAND_DEFINE (DebuggerBreakOnThunk);
EDB_COMMAND_DEFINE (DebuggerBreakOnKey);
EDB_COMMAND_DEFINE (DebuggerBreakpointList);
EDB_COMMAND_DEFINE (DebuggerBreakpointSet);
EDB_COMMAND_DEFINE (DebuggerBreakpointClear);
EDB_COMMAND_DEFINE (DebuggerBreakpointDisable);
EDB_COMMAND_DEFINE (DebuggerBreakpointEnable);
EDB_COMMAND_DEFINE (DebuggerGo);
EDB_COMMAND_DEFINE (DebuggerHelp);
EDB_COMMAND_DEFINE (DebuggerMemoryDB);
EDB_COMMAND_DEFINE (DebuggerMemoryDW);
EDB_COMMAND_DEFINE (DebuggerMemoryDD);
EDB_COMMAND_DEFINE (DebuggerMemoryDQ);
EDB_COMMAND_DEFINE (DebuggerMemoryEB);
EDB_COMMAND_DEFINE (DebuggerMemoryEW);
EDB_COMMAND_DEFINE (DebuggerMemoryED);
EDB_COMMAND_DEFINE (DebuggerMemoryEQ);
EDB_COMMAND_DEFINE (DebuggerQuit);
EDB_COMMAND_DEFINE (DebuggerRegister);
EDB_COMMAND_DEFINE (DebuggerScope);
EDB_COMMAND_DEFINE (DebuggerList);
EDB_COMMAND_DEFINE (DebuggerStepInto);
EDB_COMMAND_DEFINE (DebuggerStepOver);
EDB_COMMAND_DEFINE (DebuggerStepOut);
EDB_COMMAND_DEFINE (DebuggerListSymbol);
EDB_COMMAND_DEFINE (DebuggerLoadSymbol);
EDB_COMMAND_DEFINE (DebuggerUnloadSymbol);
EDB_COMMAND_DEFINE (DebuggerDisplaySymbol);
EDB_COMMAND_DEFINE (DebuggerLoadCode);
EDB_COMMAND_DEFINE (DebuggerUnloadCode);
EDB_COMMAND_DEFINE (DebuggerDisplayCode);
EDB_COMMAND_DEFINE (DebuggerExtIoIB);
EDB_COMMAND_DEFINE (DebuggerExtIoIW);
EDB_COMMAND_DEFINE (DebuggerExtIoID);
EDB_COMMAND_DEFINE (DebuggerExtIoOB);
EDB_COMMAND_DEFINE (DebuggerExtIoOW);
EDB_COMMAND_DEFINE (DebuggerExtIoOD);
EDB_COMMAND_DEFINE (DebuggerExtPciPCIL);
EDB_COMMAND_DEFINE (DebuggerExtPciPCID);
EDB_COMMAND_DEFINE (DebuggerExtPciCFGB);
EDB_COMMAND_DEFINE (DebuggerExtPciCFGW);
EDB_COMMAND_DEFINE (DebuggerExtPciCFGD);

extern EFI_DEBUGGER_COMMAND_SET  mDebuggerCommandSet[];

#endif
