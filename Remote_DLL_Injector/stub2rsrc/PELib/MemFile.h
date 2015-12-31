#pragma once

#include "PEFile.h"
#include <string>
#include <Windows.h>

namespace PE
{

  enum MEMFILE_ACCESS
  {
    MA_UNUSED,
    MA_READ,
    MA_WRITE,
    MA_ALL
  };

    class HandleGuard
    {
    public:
        HandleGuard( const HANDLE& Handle ){ m_Handle = Handle; };
        ~HandleGuard(){ CloseHandle( m_Handle ); };
        HANDLE Get(){ return m_Handle; };
    private:
        HANDLE m_Handle;
    };

  class MemPEFile: 
    public PEFile
  {
  public:
    MemPEFile( const wchar_t* szFileName, MEMFILE_ACCESS AccessMask );
    ~MemPEFile(void);
    ULONG_PTR GetMemPtr() const { return m_pFileMap; };
    PIMAGE_SECTION_HEADER AddPESection( const char* szName, ULONG cbSizeOfRawData, ULONG Characteristics );
        void CutFile( DWORD dwNewFileSize );

     void CloseMemFile();
  private:
    bool LoadFile( const wchar_t* szFileName, MEMFILE_ACCESS AccessMask ); // GENERIC_READ | GENERIC_WRITE
    bool LoadFile( const wchar_t* szFileName, ULONG cbAdditionalBytes, MEMFILE_ACCESS AccessMask );

    HANDLE m_hFile;
    HANDLE m_hFileMap;
    ULONG_PTR m_pFileMap;
        MEMFILE_ACCESS m_Access;
        std::wstring m_strFileName;
  };

}
