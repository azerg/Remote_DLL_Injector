// remInjectTest.cpp : Defines the entry point for the console application.
//

#include "StealthInject.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
  std::string strParams( "paramm1111" );

  boost::filesystem::path dllToInject{boost::filesystem::current_path()};
  dllToInject /= "SampleDll.dll";

  // read file contents
  size_t fileSize = 0;
  std::ifstream testFile(dllToInject.generic_string().c_str(), std::ios::binary);
  std::vector<char> fileContents;
  fileContents.reserve(fileSize);
  fileContents.assign(
    std::istreambuf_iterator<char>(testFile),
    std::istreambuf_iterator<char>());

  // inject
  static StealthParamsIn in;
  static StealthParamsOut out;
  memset(&in, 0, sizeof(in));
  memset(&out, 0, sizeof(out));
  in.dll = &fileContents.at(0); //(char*)dllToInject.c_str();
  in.dllSize = fileContents.size();
  in.process = "TargetApp.exe";
  in.params = (char*)strParams.c_str();
  in.paramLength = strParams.length();
  in.removeExtraSections = false;
  in.removePEHeader = false;
  in.randomHead = false;
  in.randomTail = false;
  in.randomMax = 1024*5;
  in.injectWithLocalDll = true;
  in.localDllPath = (boost::filesystem::current_path() /= "dummyLocal.dll").generic_string();

  SIError err = StealthInjector().Inject(&in, &out);

  std::cout << "SIError: " << err << std::endl;
  system("pause");

  return 0;
}

