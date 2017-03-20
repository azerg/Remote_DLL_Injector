// SimplePackPE.cpp : Defines the entry point for the console application.
//

#include "loader.h"
#include "stub_data.h"

void StartOriginalPE(PStubParams params);

///---------------------------------------------------------------------------------

DWORD WINAPI StubEP(PStubParams params)
{
  __asm
  {
  __looop:
     jmp __looop
  }


  const BYTE TrampLen = 0x16;//0x0F;

  // get addresses ( rebase func addresses )
  DWORD dwNewProcAddr = reinterpret_cast<DWORD>(params) + params->NewZwQInfoProcess;
  DWORD dwTampProcAddr = reinterpret_cast<DWORD>(params) + params->TrampZwQInfoProcess;
  auto pOriginalApiAddr = params->pZwQueryInformationProcess;
  params->pZwQueryInformationProcess = dwTampProcAddr;

  InterceptAPI(
    params,
    pOriginalApiAddr,
    dwNewProcAddr,
    dwTampProcAddr,
    TrampLen);

  StartOriginalPE(params);
  
  return 0;
}

void StartOriginalPE(PStubParams params)
{
  params->entryPoint(params->dllBase, DLL_PROCESS_ATTACH, params->extraData);
}

///---------------------------------------------------------------------------------

#pragma optimize( "", off )


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


BOOL InterceptAPI(PStubParams pStubData, ULONG_PTR dwAddressToIntercept, DWORD dwReplaced, DWORD dwTrampoline, BYTE offset)
{
  int i;
  DWORD dwOldProtect;

  BYTE *pbTargetCode = (BYTE *)dwAddressToIntercept;
  BYTE *pbReplaced = (BYTE *)dwReplaced;
  BYTE *pbTrampoline = (BYTE *)dwTrampoline;

  for (i = 0; i < offset; i++)

    *pbTrampoline++ = *pbTargetCode++;

  pbTargetCode = (BYTE *)dwAddressToIntercept;

  *pbTrampoline++ = 0xE9;        // jump rel32 

  *((signed int *)(pbTrampoline)) = (pbTargetCode + offset) - (pbTrampoline + 4);

  // Overwrite the first 5 bytes of the target function 

  reinterpret_cast<PVirtualProtect>(pStubData->pVirtualProtect)((void *)dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);

  *pbTargetCode++ = 0xE9;        // jump rel32 

  *((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode + 4);

  reinterpret_cast<PVirtualProtect>(pStubData->pVirtualProtect)((void *)dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);

  return (TRUE);
}

//////////////////////////////////////////////////////////////////////////
// Dummy Tramplines


bool
WINAPI
TrampZwQInfoProcess(
  __in       HANDLE ProcessHandle,
  __in       DWORD ProcessInformationClass,
  __out      PDWORD ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
)
{
  __asm {
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
  }
}