#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <time.h>
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shlwapi.lib")
using namespace std;

#define CONSOLE(x) cout << x << endl;

////////////////////////////

void to_lowercase(std::string* s);

////////////////////////////

int getProcessID(string processName, bool caseSensitive)
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;

  if(!caseSensitive)
    to_lowercase(&processName);

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if(hProcessSnap == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }
  else
  {  
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(hProcessSnap, &pe32) == NULL)
    {
      CloseHandle(hProcessSnap);
      return FALSE;
    }
    else
    {
      do
      {
        string exeName = pe32.szExeFile;

        if(!caseSensitive)
          to_lowercase(&exeName);

        if(processName == exeName)
        {
          CloseHandle(hProcessSnap);
          return pe32.th32ProcessID;  //** Return Process ID  
        }

      } while(Process32Next(hProcessSnap, &pe32));
    }
  }

  CloseHandle(hProcessSnap);

  return FALSE; //** If we make it here, error process not found
}




int getModuleHandleEx(int processID, string name, bool caseSensitive)
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
  if(snapshot == INVALID_HANDLE_VALUE)
    return NULL;

  if(!caseSensitive)
    to_lowercase(&name);

  MODULEENTRY32 mod;
  mod.dwSize = sizeof(MODULEENTRY32);

  if(Module32First(snapshot, &mod))
  {
    string moduleName = mod.szModule;
    if(!caseSensitive)
      to_lowercase(&moduleName);

    if(moduleName == name)
      return (int)mod.modBaseAddr;

    while(Module32Next(snapshot, &mod))
    {
      moduleName = mod.szModule;
      if(!caseSensitive)
        to_lowercase(&moduleName);

      if(moduleName == name)
        return (int)mod.modBaseAddr;
    }
  }

  /* Failed to find the module */
  return NULL;
}


int getProcAddressEx(int processID, const char* moduleName, const char* funcName)
{
  DWORD mod = (DWORD)LoadLibrary(moduleName);
  DWORD offsetInCurrentProcess = (DWORD)GetProcAddress((HMODULE)mod, funcName)-mod;
  if(int modInTarget = getModuleHandleEx(processID, moduleName, false))
    return modInTarget + offsetInCurrentProcess;

  // module not loaded
  return NULL;
}

unsigned int randomNumber()
{
  // our initial starting seed is 5323
  static unsigned int nSeed = time(NULL);
 
  // Take the current seed and generate a new value from it
  // Due to our use of large constants and overflow, it would be
  // very hard for someone to predict what the next number is
  // going to be from the previous one.
  nSeed = (8253729 * nSeed + 2396403);
 
  // Take the seed and return a value between 0 and 32767
  return nSeed  % 32767;
}

void writeFile(const char* file, unsigned char* block, unsigned int size)
{
  ofstream ofs;
  ofs.open(file, ios::binary);

  // read data as a block:
  ofs.write((char*)block, size);

  ofs.close();
}

void to_lowercase(std::string* s)
{
  std::string temp = s->c_str();
  for (std::string::iterator i = temp.begin(); i != temp.end(); ++i)
    *i = tolower(*i);

  *s = temp.c_str();
}