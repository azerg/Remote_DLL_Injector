#pragma once

#include "Settings.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

class InjectionMngr
{
public:
  InjectionMngr(CListBox& lbLogOutput);
  boost::optional<int64_t> DoInject(const char* targetProcessName, const char* dllToInjectPath, InjectionOptions options) const;

private:
  InjectionMngr(const InjectionMngr&) = delete;
  InjectionMngr& operator=(const InjectionMngr&) = delete;

  class LogBuff;
  std::shared_ptr<LogBuff> logBuff_;
};

