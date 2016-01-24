#include "stdafx.h"
#include "Settings.h"
#include <vector>
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

namespace pt = boost::property_tree;

Settings SettingsMngr::Read()
{
  Settings result;

  try
  {
    pt::ptree tree;
    pt::read_json(filename_, tree);

    //---------------------------------------------
    // UI
    result.topLeftX = tree.get("UI.topLeftX", 0);
    result.topLeftY = tree.get("UI.topLeftY", 0);
    
    result.lastDLLPath = tree.get<std::string>("UI.lastDLLPath", "");
    result.lastTargetProcess = tree.get<std::string>("UI.lastTargetProcess", "");

    BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("UI.RecentFiles"))
    {
      result.recentFiles.insert(v.second.data());
    }

    result.injOpts.randomHead = tree.get("InjectionOpts.randomHead", false);
    result.injOpts.randomTail = tree.get("InjectionOpts.randomTail", false);
    result.injOpts.removeExtraSections = tree.get("InjectionOpts.removeExtraSections", false);
    result.injOpts.removePEHeader = tree.get("InjectionOpts.removePEHeader", false);
    result.injOpts.injectWithLocalDll = tree.get("InjectionOpts.injectWithLocalDll", false);
    result.injOpts.randomMax = tree.get("InjectionOpts.randomMax", 1024 * 4);

    //---------------------------------------------
    // Injection Options
  }
  catch (const boost::exception&)
  {} // noting to do here. We just cant find error log. (or not? :D)

  return result;
}

void SettingsMngr::Save(Settings settings)
{
  pt::ptree tree;

  //---------------------------------------------
  // UI

  tree.put("UI.topLeftX", settings.topLeftX);
  tree.put("UI.topLeftY", settings.topLeftY);

  tree.put("UI.lastDLLPath", settings.lastDLLPath);
  tree.put("UI.lastTargetProcess", settings.lastTargetProcess);

  BOOST_FOREACH(const std::string &name, settings.recentFiles)
  {
    tree.add("UI.RecentFiles", name);
  }

  //---------------------------------------------
  // Injection Options

  tree.put("InjectionOpts.randomHead", settings.injOpts.randomHead);
  tree.put("InjectionOpts.randomTail", settings.injOpts.randomTail);
  tree.put("InjectionOpts.removeExtraSections", settings.injOpts.removeExtraSections);
  tree.put("InjectionOpts.removePEHeader", settings.injOpts.removePEHeader);
  tree.put("InjectionOpts.injectWithLocalDll", settings.injOpts.injectWithLocalDll);
  tree.put("InjectionOpts.randomMax", settings.injOpts.randomMax);

  pt::write_json(filename_, tree);
}
