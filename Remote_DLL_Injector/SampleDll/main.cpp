#include <Windows.h>
#include <iostream>
using namespace std;

#define CONSOLE(x) cout << x << endl;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  if (ul_reason_for_call == DLL_PROCESS_ATTACH)
  {

    DisableThreadLibraryCalls(hModule);

    MessageBox(0, "Hello from DLL", nullptr, 0);

    //__asm {int 3};
    FILE *stream;
    // create console
    if (!AttachConsole(ATTACH_PARENT_PROCESS))
      AllocConsole();
    SetConsoleTitle("Sample Dll");
    freopen_s(&stream, "CONOUT$", "w", stdout);

    //CONSOLE("param: " << (char*)lpReserved);

    try
    {
      CONSOLE("Throwing runtime error...");
      throw std::runtime_error("eek");
    }
    catch (const std::exception& ex)
    {
      MessageBox(0, "Catched", nullptr, 0);
      CONSOLE("Catched! Exception msg: " << ex.what());
    }
  }
  return TRUE;
}