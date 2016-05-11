#include "StealthInject.h"
#include "common_funcs.h"
#include "stub_data.h"
#include "pelib.h"
#include "RESOURCE_LocalEmptyDll.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/optional.hpp>
#include <map>
#include <limits>
#include <tlhelp32.h>
#include <Shlwapi.h>
using namespace std;

#define HEADER_SIZE 0x1000

LPVOID GetStubCodePtr(const std::string& stubName) noexcept
{
  // load loader data
  static std::vector<uint8_t> loaderData;

  // todo(azerg): add ability to use alternative loader's storage path, instead of current dir
  boost::filesystem::path loaderFullPath{boost::filesystem::current_path()};
  loaderFullPath /= stubName;

  boost::filesystem::ifstream loaderFile;
  loaderFile.open(loaderFullPath, std::ios::in || std::ifstream::binary);

  auto pbuf = loaderFile.rdbuf();
  // get file size
  size_t size = static_cast<size_t>(pbuf->pubseekoff(0, loaderFile.end, loaderFile.in));
  pbuf->pubseekpos(0, loaderFile.in);

  loaderData.reserve(size);

  // get file data
  pbuf->sgetn(reinterpret_cast<char*>(loaderData.data()), size);

  loaderFile.close();

  return reinterpret_cast<LPVOID>(loaderData.data());
}

boost::optional<StubParams> FillStubParams(StealthParamsIn* in, StealthParamsOut* out, int targetPID, PEFile& dllToInjectFile)
{
  StubParams stubData{};
  assert(in->params.size() <= sizeof(stubData.extraData));

  memcpy(stubData.extraData, in->params.data(), in->params.size());
  stubData.extraData[in->params.size()] = 0;
  stubData.dllBase = out->dllBase;
  stubData.entryPoint = (DllMainProc)((ULONG_PTR)out->dllBase + dllToInjectFile.getNtHeaders32()->OptionalHeader.AddressOfEntryPoint);
  out->dllEntryPoint = (DWORD)stubData.entryPoint;
  memcpy(stubData.stub, GetStubCodePtr({"loader_x86.stub"}), sizeof(stubData.stub));

  stubData.pGetModuleHandle = cmn::getModuleHandleEx(targetPID, "GetModuleHandle");
  stubData.pGetProcAddress = cmn::getModuleHandleEx(targetPID, "GetProcAddress");

  return boost::none;
}

SIError StealthInject::Inject(StealthParamsIn* in, StealthParamsOut* out)
{
  CONSOLE("Started injecting: " << in->localDllPath);

  int pid = cmn::getProcessID(in->process, false);
  if (!pid)
  {
    CONSOLE("Error: ProcessNotFound (" << in->process << ")");
    return SI_ProcessNotFound;
  }

  HANDLE targetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!targetProcess)
  {
    CONSOLE("Error: CantOpenProcess");
    return SI_CantOpenProcess;
  }

  PEFile peFile(in->dllToInject.data());
  if (!peFile.isValidPEFile())
  {
    CONSOLE("Error: InvalidPEFile");
    return DI_InvalidFile;
  }

  if (!AllocateDll(targetProcess, in, out, &peFile))
  {
    CONSOLE("Error: FailedToAllocate dll in target process");
    return SI_FailedToAllocate;
  }

  // copy stub
  const auto& stubData = FillStubParams(in, out, pid, peFile);
  memcpy((LPVOID)((DWORD)out->prepDllAlloc + out->randomHead), stubData.get_ptr(), sizeof(StubParams));

  // copy pe header, real header size is in IMAGE_FIRST_SECTION (peFile.getNtHeaders32())->PointerToRawData), but 0x1000 works well too
  memcpy(out->prepDllBase, in->dllToInject.data(), HEADER_SIZE);

  // copy sections
  char sectionName[10]{};
  for (int i = 0; i < peFile.getFileHead()->NumberOfSections; i++)
  {
    PIMAGE_SECTION_HEADER section = &(IMAGE_FIRST_SECTION(peFile.getNtHeaders32()))[i];

    memcpy((PUCHAR)((DWORD)out->prepDllBase) + section->VirtualAddress, (PUCHAR)((DWORD)in->dllToInject.data()) + section->PointerToRawData, section->SizeOfRawData);

    sprintf_s(sectionName, sizeof(sectionName), "%.8s", (char*)section->Name);
    CONSOLE("Section " << i << ") " << sectionName << " :: VirtualAddress " << (DWORD*)section->VirtualAddress << ", SizeOfRawData " << (DWORD*)section->SizeOfRawData << ",  VirtualSize " << (DWORD*)section->Misc.VirtualSize);
  }

  if (!ResolveIAT(targetProcess, out))
  {
    CONSOLE("Error: UnableToResolveIAT");
    return SI_UnableToResolveIAT;
  }

  if (!ResolveRelocs(out))
  {
    CONSOLE("Error: UnableToResolveRelocs");
    return SI_UnableToResolveRelocs;
  }

  if (in->removePEHeader)
    memset(out->prepDllBase, 0, HEADER_SIZE);

  if (in->removeExtraSections)
  {
    for (int i = 0; i < peFile.getFileHead()->NumberOfSections; i++)
    {
      sprintf_s(sectionName, sizeof(sectionName), "%.8s", (char*)(IMAGE_FIRST_SECTION(peFile.getNtHeaders32()))[i].Name);
      if (!strcmp(sectionName, ".reloc")
          || !strcmp(sectionName, ".rsrc"))
      {
        CONSOLE("Nulling section: " << sectionName);
        PIMAGE_SECTION_HEADER section = &(IMAGE_FIRST_SECTION(peFile.getNtHeaders32()))[i];
        memset((PUCHAR)((DWORD)out->prepDllBase) + section->VirtualAddress, 0, section->SizeOfRawData);
      }
    }
  }

  // write the fully prepared dll into target process
  if (!WriteProcessMemory(targetProcess, (LPVOID)out->allocationBase, out->prepDllAlloc, out->prepDllSize, NULL))
  {
    CONSOLE("Error: FailedToWriteDll in target process");
    return SI_FailedToWriteDll;
  }

  // create the stub thread, stub is located at allocationBase+randomHead, this is the start of StubParams, and the first
  // param is the stub itself!
  auto pRemoteAddr = out->allocationBase + out->randomHead;
  if (!CreateThread(targetProcess, pRemoteAddr, out->allocationBase + out->randomHead))
  {
    CONSOLE("Error: FailedToCreateThread in target process");
    return SI_FailedToCreateThread;
  }

  // we cant null the sub here because the thread might not have finished executing and hence this will crash the target process!
  //WriteProcessMemory(targetProcess, (LPVOID)(out->allocationBase+out->randomHead), out->prepDllAlloc, sizeof(stubData.stub), NULL);

  return SI_Success;
}

bool StealthInject::AllocateDll(HANDLE process, StealthParamsIn* in, StealthParamsOut* out, PEFile* peFile)
{
  out->randomHead = 0;
  if (in->randomHead)
  {
    out->randomHead = cmn::randomNumber() % in->randomMax;
    out->randomHead += (out->randomHead % sizeof(int));  // round up to a number divisible by 4 for alignments sake
  }

  out->randomTail = 0;
  if (in->randomTail)
  {
    out->randomTail = cmn::randomNumber() % in->randomMax;
    out->randomTail += (out->randomTail % sizeof(int));  // round up to a number divisible by 4 for alignments sake
  }

  // prepare a local dll which we can write to the target process in one go, makes things easier
  auto imageSize = out->randomHead + sizeof(StubParams) + peFile->getOptionalHead32()->SizeOfImage + out->randomTail;
  out->prepDllSize = imageSize;
  out->allocationSize = imageSize;
  out->prepDllAlloc = new char[imageSize];
  out->prepDllBase = (LPVOID)(DWORD(out->prepDllAlloc) + out->randomHead + sizeof(StubParams));
  memset(out->prepDllAlloc, 0, imageSize);

  // randomize head
  for (UINT i = 0; i < out->randomHead; i++)
    ((PUCHAR)out->prepDllAlloc)[i] = cmn::randomNumber() % 255;

  // randomize tail
  for (UINT i = 0; i < out->randomTail; i++)
    ((PUCHAR)out->prepDllAlloc)[out->randomHead + sizeof(StubParams) + peFile->getOptionalHead32()->SizeOfImage + i] = cmn::randomNumber() % 255;

  if (in->injectWithLocalDll)
  {
    /*
    TODO: Need to fix this!
    */
    cmn::writeFile(in->localDllPath.c_str(), RESOURCE_LocalEmptyDll, sizeof(RESOURCE_LocalEmptyDll));
    CPELibrary peLib;
    peLib.OpenFile(in->localDllPath.c_str());
    peLib.AddNewSection(".obj", imageSize + (cmn::randomNumber() % imageSize));
    peLib.SaveFile(in->localDllPath.c_str());

    static char dllName[256]{};
    strcpy_s(dllName, sizeof(dllName), in->localDllPath.c_str());
    PathStripPath(dllName);
    out->allocationBase = LoadLibrary_Ex(process, dllName, in->localDllPath.c_str());

    DWORD oldProt = NULL;
    VirtualProtectEx(process, (LPVOID)out->allocationBase, imageSize, PAGE_EXECUTE_READWRITE, &oldProt);
    CONSOLE("Allocated in target process cave at " << (DWORD*)out->allocationBase << " size is " << (DWORD*)out->allocationSize);
    CONSOLE("Loaded " << in->localDllPath << " at " << (DWORD*)out->allocationBase);
  }
  else
  {
    out->allocationBase = (DWORD)VirtualAllocEx(process, NULL, out->allocationSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  }

  out->dllBase = out->allocationBase + out->randomHead + sizeof(StubParams);
  return out->allocationBase != 0;
}

bool StealthInject::LoadImportedDlls(HANDLE process, PIMAGE_IMPORT_DESCRIPTOR importDescriptor, StealthParamsOut* out)
{
  static std::map<string, bool> loadedDlls;
  loadedDlls.clear();
  static string moduleName;
  while (importDescriptor->Name)
  {
    moduleName = (char*)((DWORD)(out->prepDllBase) + importDescriptor->Name);
    CONSOLE("Name: " << moduleName);
    cmn::to_lowercase(&moduleName);
    loadedDlls[moduleName] = false;
    importDescriptor++;
  }

  SetLastError(0);
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(process));
  CONSOLE("CreateToolhelp32Snapshot: " << GetLastError());
  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  MODULEENTRY32 mod;
  mod.dwSize = sizeof(MODULEENTRY32);
  if (Module32First(snapshot, &mod))
  {
    moduleName = mod.szModule;
    cmn::to_lowercase(&moduleName);

    if (loadedDlls.find(moduleName) != loadedDlls.end())
      loadedDlls[moduleName] = true;

    while (Module32Next(snapshot, &mod))
    {
      moduleName = mod.szModule;
      cmn::to_lowercase(&moduleName);

      if (loadedDlls.find(moduleName) != loadedDlls.end())
        loadedDlls[moduleName] = true;
    }
  }

  for (auto& it : loadedDlls)
  {
    if (!it.second)
    {
      CONSOLE("Loading " << it.first << " into target process...");
      if (!LoadLibrary_Ex(process, it.first.c_str(), it.first.c_str()))
      {
        CONSOLE("Failed to load: " << it.first);
        return false;
      }
    }
  }

  return true;
}

bool StealthInject::ResolveIAT(HANDLE process, StealthParamsOut* out)
{
  PEFile peFileForPrepDll(out->prepDllBase);
  PIMAGE_IMPORT_DESCRIPTOR importDescriptor = peFileForPrepDll.getImportDescriptor();

  // load all required dlls into target process
  if (!LoadImportedDlls(process, peFileForPrepDll.getImportDescriptor(), out))
    return false;

  while (importDescriptor->Name)
  {
    char* moduleName = (char*)((DWORD)(out->prepDllBase) + importDescriptor->Name);
    PDWORD firstThunk = (PDWORD)((DWORD)(out->prepDllBase) + importDescriptor->FirstThunk);
    PIMAGE_THUNK_DATA thunksChain = (PIMAGE_THUNK_DATA)((DWORD)(out->prepDllBase) + importDescriptor->OriginalFirstThunk);

    if (!importDescriptor->OriginalFirstThunk)
      thunksChain = (PIMAGE_THUNK_DATA)(firstThunk);

    // load all imported apis from definite dll
    while (thunksChain->u1.AddressOfData)
    {
      // by ordinal
      if (thunksChain->u1.AddressOfData & IMAGE_ORDINAL_FLAG32)
      {
        LoadLibrary(moduleName);
        DWORD ordinalNumber = IMAGE_ORDINAL32(thunksChain->u1.Ordinal);
        *firstThunk = GetProcAddress_Ex(process, moduleName, (char*)ordinalNumber);
        CONSOLE("Import by Ordinal: " << (DWORD*)ordinalNumber << " from " << moduleName << " at " << (DWORD*)*firstThunk);
      }
      else // by name
      {
        char* functionName = (char*)((DWORD)peFileForPrepDll.getImportByName(thunksChain) + 2);
        *firstThunk = GetProcAddress_Ex(process, moduleName, functionName);
        CONSOLE("Import by Name: " << functionName << " from " << moduleName << " at " << (DWORD*)GetProcAddress_Ex(process, moduleName, functionName));
      }

      ++thunksChain;
      ++firstThunk;
    }

    importDescriptor++;
  }

  return true;
}

bool StealthInject::ResolveRelocs(StealthParamsOut* out)
{
  PEFile peFileForPrepDll(out->prepDllBase);
  if (peFileForPrepDll.getFileHead()->Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
    return false;

  PIMAGE_DATA_DIRECTORY relocsDir = &peFileForPrepDll.getOptionalHead32()->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
  if (!relocsDir->Size)
    return true;

  DWORD baseDelta = out->dllBase - peFileForPrepDll.getNtHeaders32()->OptionalHeader.ImageBase;
  // r: current position inside base relocation data.
  PIMAGE_BASE_RELOCATION r = (IMAGE_BASE_RELOCATION*)((DWORD)out->prepDllBase + relocsDir->VirtualAddress);
  // reloc: We should not start another block if (r >= reloc)
  PIMAGE_BASE_RELOCATION reloc = (IMAGE_BASE_RELOCATION*)((DWORD)r + relocsDir->Size - sizeof(IMAGE_BASE_RELOCATION));

  for (; r < reloc; r = (IMAGE_BASE_RELOCATION*)((DWORD)r + r->SizeOfBlock))
  {
    WORD *relocItem = (WORD*)(r + 1);

    for (DWORD i = 0, totalItems = (r->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) >> 1;
    i < totalItems;
      i++, relocItem++)
    {
      switch (*relocItem >> 12)
      {
      case IMAGE_REL_BASED_ABSOLUTE:
        break;

      case IMAGE_REL_BASED_HIGHLOW:
        *(DWORD*)((DWORD)out->prepDllBase + r->VirtualAddress + (*relocItem & 0xFFF)) += baseDelta;
        break;

      default:
        return false;
      }
    }
  }

  return true;
}

typedef struct _CLIENT_ID
{
  HANDLE UniqueProcess;
  HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
typedef NTSTATUS(WINAPI * _RtlCreateUserThread)
(
  __in     HANDLE                ProcessHandle,
  __in     PSECURITY_DESCRIPTOR  SecurityDescriptor OPTIONAL,
  __in     BOOLEAN               CreateSuspended,
  __in     ULONG                 StackZeroBits,
  __inout  PULONG                StackReserved,
  __inout  PULONG                StackCommit,
  __in     PVOID                 StartAddress,
  __in     PVOID                 StartParameter OPTIONAL,
  __out    PHANDLE               ThreadHandle,
  __out    PCLIENT_ID            ClientID
  );

HANDLE StealthInject::CreateThread(HANDLE process, DWORD startRoutine, DWORD params)
{
  HANDLE remoteThread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)startRoutine, (LPVOID)params, 0, NULL);
  if (!remoteThread)
  {
    _RtlCreateUserThread RtlCreateUserThread = (_RtlCreateUserThread)GetProcAddress(GetModuleHandleW(L"ntdll"), "RtlCreateUserThread");
    if (RtlCreateUserThread)
    {
      CLIENT_ID ClientId;
      if (!NT_SUCCESS(RtlCreateUserThread(process, NULL, FALSE, 0, 0, 0, (PVOID)startRoutine, NULL, &remoteThread, &ClientId)))
        return NULL;
    }
  }
  return remoteThread;
}

DWORD StealthInject::LoadLibrary_Ex(HANDLE process, const char* moduleName, const char* modulePath)
{
  if (!cmn::getModuleHandleEx(GetProcessId(process), moduleName, false))
  {
    // load the dll into target process
    PVOID remoteAddr = VirtualAllocEx(process, NULL, strlen(modulePath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (remoteAddr == NULL)
    {
      std::runtime_error("VirtualAlloc failed...");
    }
    WriteProcessMemory(process, remoteAddr, modulePath, strlen(modulePath), NULL);
    DWORD loadLibAddr = GetProcAddress_Ex(process, "kernel32.dll", "LoadLibraryA");
    HANDLE thread = CreateThread(process, loadLibAddr, DWORD(remoteAddr));
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    VirtualFreeEx(process, remoteAddr, 0, MEM_RELEASE);
  }
  return cmn::getModuleHandleEx(GetProcessId(process), moduleName, false);
}

DWORD StealthInject::GetProcAddress_Ex(HANDLE process, const char* moduleName, const char* functionName)
{
  return cmn::getProcAddressEx(GetProcessId(process), moduleName, functionName);
}

bool PEFile::isValidPEFile()
{
  PIMAGE_DOS_HEADER DOSHead = (PIMAGE_DOS_HEADER)pefile_dll;
  if (DOSHead->e_magic != IMAGE_DOS_SIGNATURE)
    return false;

  PIMAGE_OPTIONAL_HEADER32 OptHead32 = getOptionalHead32();
  if (OptHead32->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    return false;

  return true;
}