#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Settings.h"

class InjectionMngr
{
public:
  InjectionMngr(CListBox& lbLogOutput);
  bool DoInject(const char* targetProcessName, const char* dllToInjectPath, InjectionOptions options) const;

private:
  InjectionMngr(const InjectionMngr&) = delete;
  InjectionMngr& operator=(const InjectionMngr&) = delete;

  class LogBuff;
  std::shared_ptr<LogBuff> logBuff_;
};

