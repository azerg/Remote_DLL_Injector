#include "MemFile.h"

namespace PE
{

  MemPEFile::MemPEFile( const wchar_t* szFileName, MEMFILE_ACCESS AccessMask ):
    m_hFile( INVALID_HANDLE_VALUE )
  {
    if ( !LoadFile( szFileName, AccessMask ) )
    {
      throw std::runtime_error( "error loading selected file" );
    }
  }

  MemPEFile::~MemPEFile(void)
  {
    if ( m_hFile != INVALID_HANDLE_VALUE )
    {
      CloseMemFile();
    }
  }

  bool MemPEFile::LoadFile( const wchar_t* szFileName, ULONG cbAdditionalBytes, MEMFILE_ACCESS AccessMask )
  {
        m_Access = AccessMask;
        m_strFileName = szFileName;
    DWORD dwFileAccess = 0;
    DWORD dwPageAccess = 0;
    DWORD dwFileMapAccess = 0;

    switch ( AccessMask )
    {
    case MA_READ:
      dwFileAccess = GENERIC_READ;
      dwPageAccess = PAGE_READONLY;
      dwFileMapAccess = FILE_MAP_READ;
      break;
    case MA_WRITE:
      dwFileAccess = GENERIC_WRITE;
      dwPageAccess = PAGE_READWRITE;
      dwFileMapAccess = FILE_MAP_WRITE;
      break;
    case MA_ALL:
      dwFileAccess = GENERIC_READ | GENERIC_WRITE;
      dwPageAccess = PAGE_READWRITE;
      dwFileMapAccess = FILE_MAP_READ | FILE_MAP_WRITE;
      break;
    default:
      throw std::logic_error( "invalid access mask passed" );
    }


    m_hFile = CreateFile( szFileName, dwFileAccess, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
    if( m_hFile == INVALID_HANDLE_VALUE )
    {
      return false;
    }

    LARGE_INTEGER FileSize = {0,0};

    FileSize.LowPart = GetFileSize( m_hFile, reinterpret_cast<LPDWORD>( &FileSize.HighPart ) );
    if ( FileSize.LowPart == INVALID_FILE_SIZE )
    {
      return false;
    }

    FileSize.QuadPart += cbAdditionalBytes;

    m_hFileMap = CreateFileMapping( m_hFile, NULL, dwPageAccess | SEC_RESERVE, FileSize.HighPart, FileSize.LowPart, 0);
    if( m_hFileMap == NULL )
    {
      CloseHandle(m_hFile);
      return false;
    }

    m_pFileMap = reinterpret_cast<ULONG_PTR>( MapViewOfFile( m_hFileMap, dwFileMapAccess, 0, 0, 0 ) );
    if( m_pFileMap == NULL )
    {
      CloseHandle(m_hFileMap);
      CloseHandle(m_hFile);
      return false;
    }

        Reset( GetMemPtr() );

    return true;
  }

  bool MemPEFile::LoadFile( const wchar_t* szFileName, MEMFILE_ACCESS AccessMask )
  {
    return LoadFile( szFileName, 0, AccessMask );
  }

  void MemPEFile::CloseMemFile()
  {
    UnmapViewOfFile( reinterpret_cast<LPCVOID>( m_pFileMap ) ); 
    CloseHandle( m_hFileMap );
    CloseHandle( m_hFile );
  }

    PIMAGE_SECTION_HEADER MemPEFile::AddPESection( const char* szName, ULONG cbSizeOfRawData, ULONG Characteristics )
    {
        // get file alignment value
        ULONG dwFileAlignment = GetOptionalHead32()->FileAlignment;

        // closing old file;
        CloseMemFile();

        ULONG cbAdditionalBytes = AlignUp( cbSizeOfRawData, dwFileAlignment );

        // reload file using old AccessMask, with allocating additional bytes
        if ( !LoadFile( m_strFileName.c_str(), cbAdditionalBytes, m_Access ) )
        {
            throw std::runtime_error( "error reloading file" );
        }

        return AddSection( szName, cbAdditionalBytes, Characteristics );
    }

    void MemPEFile::CutFile( DWORD dwNewFileSize )
    {
        CloseMemFile();

        {
            HandleGuard hFile = CreateFile( m_strFileName.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0 );
            if ( hFile.Get() == INVALID_HANDLE_VALUE )
            {
                throw std::runtime_error( "Error opening target file" );
            }

            SetFilePointer( hFile.Get(), dwNewFileSize, NULL, FILE_BEGIN );
            SetEndOfFile( hFile.Get() );
        }

        if ( !LoadFile( m_strFileName.c_str(), m_Access ) )
        {
            throw std::runtime_error( "error reloading file" );
        }       

    }

}
