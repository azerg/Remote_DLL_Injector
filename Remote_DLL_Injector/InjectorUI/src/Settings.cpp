#include "stdafx.h"
#include "Settings.h"
#include <vector>
#include <fstream>
#include <filesystem>
#include <toml++/toml.hpp>

void dumpConfig(const char* configName, Settings base) {
  std::fstream defaultConfig(configName, std::ios::binary + std::ios::out);
  defaultConfig << R"(
[UI]
  topLeftX = )" << base.topLeftX << R"(
  topLeftY = )" << base.topLeftY << R"(
  lastDLLPath = ")" << base.lastDLLPath << R"("
  lastTargetProcess = ")" << base.lastTargetProcess << R"("

[InjectionOptions]
  randomHead = )" << base.injOpts.randomHead << R"(
  randomTail = )" << base.injOpts.randomTail << R"(
  removeExtraSections = )" << base.injOpts.removeExtraSections << R"(
  removePEHeader = )" << base.injOpts.removePEHeader << R"(
  injectWithLocalDll = )" << base.injOpts.injectWithLocalDll << R"(
  randomMax = )" << base.injOpts.randomMax << R"(
)";

  defaultConfig.flush();
}

Settings SettingsMngr::Read()
{
  Settings result;

  if (!std::filesystem::exists(filename_)) {
    dumpConfig(filename_.c_str(), std::move(result));
    return Settings();
  }

  toml::table tbl;
  try
  {
    tbl = toml::parse_file(filename_);

    //---------------------------------------------
    // UI
    result.topLeftX = tbl["UI"]["topLeftX"].value_or(0);
    result.topLeftY = tbl["UI"]["topLeftY"].value_or(0);

    result.lastDLLPath = tbl["UI"]["lastDLLPath"].value_or("");
    result.lastTargetProcess = tbl["UI"]["lastTargetProcess"].value_or("");

    //BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("UI.RecentFiles"))
    //{
    //  result.recentFiles.insert(v.second.data());
    //}

    //---------------------------------------------
    // Injection Options
    result.injOpts.randomHead = tbl["InjectionOptions"]["randomHead"].value_or(false);
    result.injOpts.randomTail = tbl["InjectionOptions"]["randomTail"].value_or(false);
    result.injOpts.removeExtraSections = tbl["InjectionOptions"]["removeExtraSections"].value_or(false);
    result.injOpts.removePEHeader = tbl["InjectionOptions"]["removePEHeader"].value_or(false);
    result.injOpts.injectWithLocalDll = tbl["InjectionOptions"]["injectWithLocalDll"].value_or(false);
    result.injOpts.randomMax = tbl["InjectionOptions"]["randomMax"].value_or(1024 * 4);
  }
  catch (const std::runtime_error& err)
  {} // noting to do here. We just cant find error log. (or not? :D)

  return result;
}

void SettingsMngr::Save(Settings settings)
{
  dumpConfig(filename_.c_str(), std::move(settings));
}
