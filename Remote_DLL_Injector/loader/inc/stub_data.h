#pragma once
#include "Windows.h"

typedef BOOL(APIENTRY* DllMainProc) (DWORD hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
typedef struct StubParams
{
  char stub[0x200];  // 0x100 is approximate size of the stub_startDll routine. NOTE: this MUST be the first parameter of StubParams!!
  DWORD dllBase;
  DllMainProc entryPoint;
  ULONG_PTR pGetModuleHandle;
  ULONG_PTR pGetProcAddress;
  ULONG_PTR pVirtualProtect;
  char extraData[0x1000];

  // internal Loader Function addressed
  ULONG_PTR NewZwQInfoProcess;
  ULONG_PTR TrampZwQInfoProcess;

  // runtime data
  ULONG_PTR pZwQueryInformationProcess;
}StubParams, *PStubParams;