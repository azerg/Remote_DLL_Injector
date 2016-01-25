#pragma once

#include <string>
#include <unordered_set>

struct InjectionOptions
{
  bool removeExtraSections;
  bool removePEHeader;
  bool randomHead;
  bool randomTail;
  bool injectWithLocalDll;
  uint32_t randomMax; // = 1024 * 5;
};

// Options used to share between "Settings" dlg & main window
struct UIInternalSettings
{
  UIInternalSettings() = default;
  UIInternalSettings(UIInternalSettings&&) = default;

  InjectionOptions injOpts;
};

struct Settings
{
  Settings():
    topLeftX{},
    topLeftY{},
    injOpts{}
  {}
  Settings(Settings&&) = default;
  int topLeftX;
  int topLeftY;
  std::string lastDLLPath;
  std::string lastTargetProcess;
  InjectionOptions injOpts; // injection options
  std::unordered_set<std::string> recentFiles;
};

class SettingsMngr
{
public:
  SettingsMngr(const char* filename) :
    filename_(filename)
  {}
  SettingsMngr(const SettingsMngr&) = delete;
  Settings Read();
  void Save(Settings settings);

private:
  std::string filename_;
};

