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
#include <CLI/CLI.hpp>


CLI::App app("remInject TestConsole");


struct CmdOptions
{
  std::string targetProcessName;
  uint32_t pid;
  std::string dllToInjectPath;
  bool injectWithLocalDLL; /*true*/
  std::string localDLLPath;
};

std::vector<uint8_t> ReadFileContents(std::string filePath)
{
  std::ifstream testFile(filePath.c_str(), std::ios::binary);
  std::vector<uint8_t> fileContents;
  fileContents.assign(
    std::istreambuf_iterator<char>(testFile),
    std::istreambuf_iterator<char>());
  return fileContents;
}

int doJob(const CmdOptions& cmdOptions) {
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
  in.params = { 1,2,3,4,5,6,7,8,9,0,9,8,7,6,5,4,3,2,1 }; // sample params arr :D
  in.removeExtraSections = true;
  in.removePEHeader = true;
  in.randomHead = true;
  in.randomTail = true;
  in.randomMax = 1024 * 5;
  in.injectWithLocalDll = cmdOptions.injectWithLocalDLL;
  in.localDllPath = cmdOptions.localDLLPath;

  StealthInject inj;
  SIError err = inj.Inject(&in, &out);

  std::cout << "SIError: " << err << std::endl;
  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
  CmdOptions cmdOptions;

  app.add_option("target_process_name,--target_process_name,-target_process_name,--t,-t", cmdOptions.targetProcessName, "Target process name (e.g. \"target.exe\")");

  app.add_option("pid,--pid,-pid", cmdOptions.pid, "Target process name (e.g. \"target.exe\")");

  app.add_option("source_dll_path,--source_dll_path,-source_dll_path,--s,-s", cmdOptions.dllToInjectPath, "Full path to dll to inject");

  app.add_option("with_local_dll,--with_local_dll,-with_local_dll", cmdOptions.injectWithLocalDLL, "Inject with local dll flag (default false)");

  app.add_option("local_dll_path,--local_dll_path,-local_dll_path", cmdOptions.localDLLPath, "Full path to the local dll that will be created during process of injection ( default empty ). Use with --with_local_dll_opt flag");

  CLI11_PARSE(app, argc, argv);

  return doJob(cmdOptions);
}