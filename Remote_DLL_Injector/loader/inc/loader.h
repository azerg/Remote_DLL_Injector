#pragma once
#include "stub_data.h"
#include "Windows.h"

typedef ULONG_PTR(__stdcall *PLoadLibraryA)(char*);
typedef ULONG_PTR(__stdcall *PGetProcAddress)(ULONG_PTR, char*);
typedef ULONG_PTR(__stdcall *PVirtualProtect)(__in   LPVOID lpAddress,
  __in   SIZE_T dwSize,
  __in   DWORD flNewProtect,
  __out  PDWORD lpflOldProtect);
typedef NTSTATUS(__stdcall *PZwQueryInformationProcess)(
  __in       HANDLE ProcessHandle,
  __in       DWORD ProcessInformationClass,
  __out      PDWORD ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
  );

NTSTATUS WINAPI NewZwQInfoProcess(
  __in       HANDLE ProcessHandle,
  __in       DWORD ProcessInformationClass,
  __out      PDWORD ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
);
BOOL InterceptAPI(PStubParams pStubData, ULONG_PTR dwAddressToIntercept, DWORD dwReplaced, DWORD dwTrampoline, BYTE offset);
bool
WINAPI
TrampZwQInfoProcess(
  __in       HANDLE ProcessHandle,
  __in       DWORD ProcessInformationClass,
  __out      PDWORD ProcessInformation,
  __in       ULONG ProcessInformationLength,
  __out_opt  PULONG ReturnLength
);




typedef struct _PEB {
  BOOLEAN                 InheritedAddressSpace;
  BOOLEAN                 ReadImageFileExecOptions;
  BOOLEAN                 BeingDebugged;
  BOOLEAN                 Spare;
  HANDLE                  Mutant;
  ULONG                   ImageBaseAddress;
  PVOID                   param1;
  PVOID                   param2;

  PVOID                   SubSystemData;
  PVOID                   ProcessHeap;
  PVOID                   FastPebLock;
  PVOID                   param3;
  PVOID                   param4;

  ULONG                   EnvironmentUpdateCount;
  PVOID                  *KernelCallbackTable;
  PVOID                   EventLogSection;
  PVOID                   EventLog;
  PVOID                   FreeList;

  ULONG                   TlsExpansionCounter;
  PVOID                   TlsBitmap;
  ULONG                   TlsBitmapBits[0x2];
  PVOID                   ReadOnlySharedMemoryBase;
  PVOID                   ReadOnlySharedMemoryHeap;
  PVOID                  *ReadOnlyStaticServerData;
  PVOID                   AnsiCodePageData;
  PVOID                   OemCodePageData;
  PVOID                   UnicodeCaseTableData;
  ULONG                   NumberOfProcessors;
  ULONG                   NtGlobalFlag;
  BYTE                    Spare2[0x4];
  LARGE_INTEGER           CriticalSectionTimeout;
  ULONG                   HeapSegmentReserve;
  ULONG                   HeapSegmentCommit;
  ULONG                   HeapDeCommitTotalFreeThreshold;
  ULONG                   HeapDeCommitFreeBlockThreshold;
  ULONG                   NumberOfHeaps;
  ULONG                   MaximumNumberOfHeaps;
  PVOID                  *ProcessHeaps;

  PVOID                   GdiSharedHandleTable;
  PVOID                   ProcessStarterHelper;
  PVOID                   GdiDCAttributeList;
  PVOID                   LoaderLock;
  ULONG                   OSMajorVersion;
  ULONG                   OSMinorVersion;
  ULONG                   OSBuildNumber;
  ULONG                   OSPlatformId;
  ULONG                   ImageSubSystem;
  ULONG                   ImageSubSystemMajorVersion;
  ULONG                   ImageSubSystemMinorVersion;
  ULONG                   GdiHandleBuffer[0x22];
  ULONG                   PostProcessInitRoutine;
  ULONG                   TlsExpansionBitmap;
  BYTE                    TlsExpansionBitmapBits[0x80];
  ULONG                   SessionId;

} PEB, *PPEB;