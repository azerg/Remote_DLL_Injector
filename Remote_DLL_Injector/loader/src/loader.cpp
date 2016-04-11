// SimplePackPE.cpp : Defines the entry point for the console application.
//

#include "stub_data.h"

DWORD WINAPI StubEP(StubParams* params)
{
  params->entryPoint(params->dllBase, DLL_PROCESS_ATTACH, params->extraData);
  return 0;
}




