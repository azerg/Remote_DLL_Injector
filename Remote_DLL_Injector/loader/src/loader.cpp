// SimplePackPE.cpp : Defines the entry point for the console application.
//

#include "stub_data.h"

DWORD WINAPI StubEP(StubParams* params)
{
  _asm int 3
  params->entryPoint(params->dllBase, DLL_PROCESS_ATTACH, params->extraData);
  return 0;
}


