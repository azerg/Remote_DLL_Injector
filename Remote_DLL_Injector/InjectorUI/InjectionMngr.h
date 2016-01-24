#pragma once

#include <cstdint>
#include <vector>
#include "Settings.h"

class InjectionMngr
{
public:
  bool DoInject(const char* targetProcessName, const char* dllToInjectPath, InjectionOptions options) const;
};

