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

  pt::ptree tree;
  pt::read_json(filename_, tree);

  result.width = tree.get("UI.Width", 0);
  result.height = tree.get("UI.Height", 0);

  BOOST_FOREACH(pt::ptree::value_type &v, tree.get_child("UI.Recent"))
  {
    result.recentFiles.insert(v.second.data());
  }

  return result;
}

void SettingsMngr::Save(Settings settings)
{
  pt::ptree tree;
  tree.put("UI.Width", settings.width);
  tree.put("UI.Height", settings.height);

  BOOST_FOREACH(const std::string &name, settings.recentFiles)
  {
    tree.add("UI.Recent", name);
  }

  pt::write_json(filename_, tree);
}
