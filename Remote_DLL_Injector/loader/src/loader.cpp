// SimplePackPE.cpp : Defines the entry point for the console application.
//

#include "loader.h"
#include "stub_data.h"

DWORD WINAPI StubEP(PStubParams params)
{
  __asm
  {
  __looop:
     jmp __looop
  }


  const BYTE TrampLen = 0x20;

  /*
  // get addresses ( rebase func addresses )
  DWORD dwNewProcAddr = (DWORD)&NewZwQInfoProcess;
  DWORD dwTampProcAddr = (DWORD)&TrampZwQInfoProcess;

  // TODO: replace with MACRO
  dwNewProcAddr -= 0x401000;
  dwTampProcAddr -= 0x401000;
  dwNewProcAddr += reinterpret_cast<DWORD>( pStubData ) + sizeof(STUB_DATA);
  dwTampProcAddr += reinterpret_cast<DWORD>( pStubData ) + sizeof(STUB_DATA);
  pStubData->pZwQueryInformationProcess = dwTampProcAddr;
  */

  StartOriginalPE(params);
  
  return 0;
}

void StartOriginalPE(PStubParams params)
{
  params->entryPoint(params->dllBase, DLL_PROCESS_ATTACH, params->extraData);
}

NTSTATUS WINAPI NewZwQInfoProcess(
  __in       HANDLE ProcessHandle,
  __in       DWORD ProcessInformationClass,
  __out      PDWORD ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
)
{
  PStubParams pStubData;

  // get address of STUB_DATA
  // we know that it's located straight before our code.
  __asm {
    call __getmyaddr
    __getmyaddr :
    pop eax
      and eax, 0xFFFFF000 // We know that address of structure is the address of section.
                          // section addr is aligned to 0x1000
      mov pStubData, eax
  }

  PZwQueryInformationProcess pZwQInfoProcess = reinterpret_cast<PZwQueryInformationProcess>(pStubData->pZwQueryInformationProcess);
  NTSTATUS Result = pZwQInfoProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

  // our rude patch
  if (ProcessInformationClass == 0x22 &&
    (ProcessInformationLength == sizeof(ULONG_PTR)) &&
    (*ProcessInformation == 0x4D))
  {
    *ProcessInformation = 0x74;
  }

  return Result;
}