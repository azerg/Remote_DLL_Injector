#pragma once
#include <Windows.h>

struct StealthParamsIn{
	char* process;
	LPVOID dll;
	SIZE_T dllSize;

	char* params;
	SIZE_T paramLength;

	bool removePEHeader;
	bool removeExtraSections;
	bool randomHead;
	bool randomTail;
	SIZE_T randomMax;

	bool injectWithLocalDll;
	const char* localDllPath;
};

struct StealthParamsOut {
	LPVOID prepDllAlloc;
	SIZE_T prepDllSize;
	LPVOID prepDllBase;

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

class StealthInject
{
public:
	SIError inject(StealthParamsIn* in, StealthParamsOut* out);

	DWORD LoadLibrary_Ex(HANDLE process, const char* moduleName, const char* modulePath);
	DWORD GetProcAddress_Ex(HANDLE process, const char* moduleName, const char* functionName);

private:
	bool allocateDll(HANDLE process, StealthParamsIn* in, StealthParamsOut* out, PEFile* peFile);
	bool resolveIAT(HANDLE process, StealthParamsOut* out);
	bool resolveRelocs(StealthParamsOut* out);
	HANDLE createThread(HANDLE process, DWORD startRoutine, DWORD params);
	bool loadImportedDlls(HANDLE process, PIMAGE_IMPORT_DESCRIPTOR importDescriptor, StealthParamsOut* out);

private:
	LPVOID preparatoryDll;
};

inline StealthInject& StealthInjector()
{
	// This is Stub.bin converted to an array! (not stub.exe!)
	//static reminjector::RemoteInjector inj(reinterpret_cast<char*>( &Stub[0] ), Stub_size);
	static StealthInject inj;
	return inj;
}