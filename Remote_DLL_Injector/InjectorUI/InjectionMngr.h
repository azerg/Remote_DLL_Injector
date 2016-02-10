#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "HListBox.h"
#include "Settings.h"

class InjectionMngr
{
public:
  InjectionMngr(CHListBox& lbLogOutput);
  bool DoInject(const char* targetProcessName, const char* dllToInjectPath, InjectionOptions options) const;

private:
  InjectionMngr(const InjectionMngr&) = delete;
  InjectionMngr& operator=(const InjectionMngr&) = delete;

  class LogBuff;
  std::shared_ptr<LogBuff> logBuff_;
  CHListBox& lbLogOutput_;
};

