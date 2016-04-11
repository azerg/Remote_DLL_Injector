#pragma once

#include <Windows.h>

typedef ULONG_PTR PtrType;

class FixExceptions
{
public:
  FixExceptions(HANDLE hTargetProcess) noexcept :
    hTargetProcess_(hTargetProcess)
  {}

private:
  PtrType GetRemoteFunctionAddr();

  HANDLE hTargetProcess_;
};