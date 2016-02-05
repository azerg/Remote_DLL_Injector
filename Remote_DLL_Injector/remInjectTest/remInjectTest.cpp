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
#include <boost/program_options.hpp>

namespace po = boost::program_options;


struct CmdOptions
{
  std::string targetProcessName;
  std::string dllToInjectPath;
  bool injectWithLocalDLL; /*true*/
  std::string localDLLPath;
};

template <class Type>
void GetValFromCmd(const po::variables_map& vm, const char* fieldName, Type& outVar)
{
  outVar = vm[fieldName].as<Type>();
}

CmdOptions ParseCmd(int argc, _TCHAR* argv[])
{
  CmdOptions result;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "produce help message")
  ("target_process_name", po::value<std::string>()->default_value(""), "Target process name (e.g. \"target.exe\")")
  ("source_dll_path", po::value<std::string>()->default_value(""), "Full path to dll to inject")
  ("with_local_dll", po::value<bool>()->default_value(false), "Inject with local dll flag (default false)")
  ("local_dll_path", po::value<std::string>()->default_value(""), "Full path to the local dll that will be created during process of injection")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n" << "Sample params str:\n" <<
      "--target_process_name=\"TargetApp.exe\" "
      "--source_dll_path=\"D:\\Work\\Remote_DLL_Injector\\bin\\Debug\\SampleDll.dll\" "
      "--with_local_dll=yes "
      "--local_dll_path=\"D:\\Work\\Remote_DLL_Injector\\bin\\Debug\\dummy_local.dll\"";
    return CmdOptions{};
  }

  GetValFromCmd(vm, "target_process_name", result.targetProcessName);
  GetValFromCmd(vm, "source_dll_path", result.dllToInjectPath);
  GetValFromCmd(vm, "with_local_dll", result.injectWithLocalDLL);
  GetValFromCmd(vm, "local_dll_path", result.localDLLPath);

  return result;
}

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
  auto cmdOptions = ParseCmd(argc, argv);

  if (cmdOptions.dllToInjectPath.empty())
  {
    std::cout << "Type -h for help" << std::endl;
    return -1;
  }

  // inject
  static StealthParamsIn in;
  static StealthParamsOut out;
  memset(&in, 0, sizeof(in));
  memset(&out, 0, sizeof(out));
  in.dllToInject = ReadFileContents(cmdOptions.dllToInjectPath.c_str());
  in.process = cmdOptions.targetProcessName;
  in.params = {1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4,3,2,1}; // sample params arr :D
  in.removeExtraSections = true;
  in.removePEHeader = true;
  in.randomHead = true;
  in.randomTail = true;
  in.randomMax = 1024*5;
  in.injectWithLocalDll = cmdOptions.injectWithLocalDLL;
  in.localDllPath = cmdOptions.localDLLPath;

  StealthInject inj;
  SIError err = inj.Inject(&in, &out);

  std::cout << "SIError: " << err << std::endl;

  return 0;
}

