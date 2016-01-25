// remInjectTest.cpp : Defines the entry point for the console application.
//

#include "StealthInject.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <tchar.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

std::vector<uint8_t> ReadFileContents(std::string filePath)
{
  std::ifstream testFile(filePath.c_str(), std::ios::binary);
  std::vector<uint8_t> fileContents;
  fileContents.assign(
    std::istreambuf_iterator<char>(testFile),
    std::istreambuf_iterator<char>());
  return fileContents;
}

int _tmain(int argc, _TCHAR* argv[])
{
  boost::filesystem::path dllToInject{boost::filesystem::current_path()};
  dllToInject /= "SampleDll.dll";

  // inject
  static StealthParamsIn in;
  static StealthParamsOut out;
  memset(&in, 0, sizeof(in));
  memset(&out, 0, sizeof(out));
  in.dllToInject = ReadFileContents(dllToInject.generic_string());
  in.process = "TargetApp.exe";
  in.params = {1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4,3,2,1}; // sample params arr :D
  in.removeExtraSections = true;
  in.removePEHeader = true;
  in.randomHead = true;
  in.randomTail = true;
  in.randomMax = 1024*5;
  in.injectWithLocalDll = true;
  in.localDllPath = (boost::filesystem::current_path() /= "dummyLocal.dll").generic_string();

  StealthInject inj;
  SIError err = inj.Inject(&in, &out);

  std::cout << "SIError: " << err << std::endl;
  system("pause");

  return 0;
}

