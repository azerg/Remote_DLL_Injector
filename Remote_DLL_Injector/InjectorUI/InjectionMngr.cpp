#include "stdafx.h"
#include "InjectionMngr.h"
#include "StealthInject.h"
#include <vector>
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

bool InjectionMngr::DoInject(const char* targetProcessName, const char * dllToInjectPath, InjectionOptions options) const
{
  static StealthParamsIn in{};
  static StealthParamsOut out{};
  in.dllToInject = ReadFileContents(dllToInjectPath);
  in.process = targetProcessName;
  // unussed yet. todo(azerg): add to options
  //in.params = {1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4,3,2,1}; // sample params arr :D
  in.removeExtraSections = options.removeExtraSections;
  in.removePEHeader = options.removePEHeader;
  in.randomHead = options.randomHead;
  in.randomTail = options.randomTail;
  in.randomMax = options.randomMax;
  in.injectWithLocalDll = false;
  in.localDllPath = (boost::filesystem::current_path() /= "dummyLocal.dll").generic_string();

  StealthInject inj;
  SIError err = inj.Inject(&in, &out);
  return err == SI_Success;
}
