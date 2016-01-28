#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

struct StealthParamsIn{
  std::string process;
  std::vector<uint8_t> dllToInject;
  std::vector<uint8_t> params;

  bool removePEHeader;
  bool removeExtraSections;
  bool randomHead;
  bool randomTail;
  SIZE_T randomMax;

  bool injectWithLocalDll;
  std::string localDllPath;
};

struct StealthParamsOut {
  LPVOID prepDllAlloc;
  SIZE_T prepDllSize;
  LPVOID prepDllBase;

  SIZE_T allocationSize;
  DWORD allocationBase;
  DWORD dllBase;
  DWORD dllEntryPoint;

  SIZE_T randomHead;
  SIZE_T randomTail;
};

enum SIError {
  SI_Success,
  SI_ProcessNotFound,
  SI_CantOpenProcess,
  DI_InvalidFile,
  SI_FailedToAllocate,
  SI_UnableToResolveIAT,
  SI_UnableToResolveRelocs,
  SI_FailedToWriteDll,
  SI_FailedToCreateThread
};

class PEFile
{
public:
  PEFile(LPVOID dll) { pefile_dll = dll; };

  bool isValidPEFile();

  PIMAGE_DOS_HEADER getDosHeader()
  { return PIMAGE_DOS_HEADER(pefile_dll); };

  PIMAGE_NT_HEADERS32 getNtHeaders32()
  { return (PIMAGE_NT_HEADERS32)((DWORD)pefile_dll + getDosHeader()->e_lfanew); };

  PIMAGE_FILE_HEADER getFileHead()
  { return (PIMAGE_FILE_HEADER)(&getNtHeaders32()->FileHeader); };

  PIMAGE_OPTIONAL_HEADER32 getOptionalHead32()
  { return (PIMAGE_OPTIONAL_HEADER32)(&getNtHeaders32()->OptionalHeader); };

  PIMAGE_SECTION_HEADER getSectionHead()
  { return (PIMAGE_SECTION_HEADER)((DWORD)getOptionalHead32() + getFileHead()->SizeOfOptionalHeader); };

  PIMAGE_IMPORT_DESCRIPTOR getImportDescriptor()
  { return (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pefile_dll + getOptionalHead32()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress); };

  PIMAGE_IMPORT_BY_NAME getImportByName(PIMAGE_THUNK_DATA pThunkData)
  { return (PIMAGE_IMPORT_BY_NAME)((DWORD)pefile_dll + pThunkData->u1.AddressOfData); };
private:
  LPVOID pefile_dll;
};

// redirecting logs out of this obj if needed
class ConRedirect
{
public:
  ConRedirect(std::ostream& srcStream, std::ostream& destStream) :
    srcStream_(srcStream),
    oldBuff_(srcStream.rdbuf(destStream.rdbuf()))
  {}

  ~ConRedirect()
  {
    srcStream_.rdbuf(oldBuff_);
  }

private:
  ConRedirect(const ConRedirect&) = delete;

  std::ostream& srcStream_;
  std::streambuf* oldBuff_;
};

class StealthInject
{
public:
  // redirectedStream - allows us to redirect injector messages to any stream instead of console
  StealthInject(std::ostream& redirectedStream = std::cout):
    conRecirect_(std::cout, redirectedStream)
  {}
  SIError Inject(StealthParamsIn* in, StealthParamsOut* out);

  DWORD LoadLibrary_Ex(HANDLE process, const char* moduleName, const char* modulePath);
  DWORD GetProcAddress_Ex(HANDLE process, const char* moduleName, const char* functionName);

private:
  bool AllocateDll(HANDLE process, StealthParamsIn* in, StealthParamsOut* out, PEFile* peFile);
  bool ResolveIAT(HANDLE process, StealthParamsOut* out);
  bool ResolveRelocs(StealthParamsOut* out);
  HANDLE CreateThread(HANDLE process, DWORD startRoutine, DWORD params);
  bool LoadImportedDlls(HANDLE process, PIMAGE_IMPORT_DESCRIPTOR importDescriptor, StealthParamsOut* out);

private:
  ConRedirect conRecirect_;
  LPVOID preparatoryDll;
};