#pragma once
#include "Windows.h"

typedef BOOL(APIENTRY* DllMain) (DWORD hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
struct StubParams
{
  char stub[0x200];  // 0x100 is approximate size of the stub_startDll routine. NOTE: this MUST be the first parameter of StubParams!!
  DWORD dllBase;
  DllMain entryPoint;
  char extraData[0x1000];
};