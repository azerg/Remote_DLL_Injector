// stub2rsrc.cpp : Defines the entry point for the console application.
//

#include <memory>
#include <vector>
#include <fstream>
#include <iostream>
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../PELib/MemFile.h"

// using:
// app.exe [source.exe path] [dest bin file path]

int _tmain(int argc, _TCHAR* argv[])
{
  if ( argc != 3 )
  {
    std::cout << "usage: app.exe [source.exe path] [dest bin file path]";
    return -1;
  }

  // we dont want to handle exceptions here coz we want to get notice if
  // smth goes wrong

  // here we will open target exe file ( we require read-only access )
  PE::MemPEFile MemPE( argv[1], PE::MEMFILE_ACCESS::MA_READ );

  if ( !MemPE.IsValidPEFile() )
  {
    throw std::runtime_error( "not a valid PE file passed" );
  }

  PIMAGE_SECTION_HEADER pSecHead = MemPE.GetSectionHead();
  
  std::ofstream fDestBin( argv[2], std::ios::out | std::ios::binary );
  fDestBin.write(
    reinterpret_cast<char*>( MemPE.GetMemPtr() + pSecHead->PointerToRawData ),
    pSecHead->SizeOfRawData );
  fDestBin.close();

  std::cout << "Stub created successfully in: ";
  std::wcout << argv[2];
}

