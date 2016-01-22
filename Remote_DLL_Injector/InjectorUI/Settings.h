#pragma once

#include <string>
#include <unordered_set>

struct Settings
{
  Settings() {}
  Settings(Settings&) = default;
  int width;
  int height;
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

