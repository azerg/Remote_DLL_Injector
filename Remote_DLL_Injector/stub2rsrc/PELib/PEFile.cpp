#include "PEFile.h"
#include <Dbghelp.h>

namespace PE
{

  PEFile::PEFile( ULONG_PTR pFileBuff )
  {
    Reset( pFileBuff );
  }

  PEFile::~PEFile(void)
  {
  }

  PIMAGE_DOS_HEADER PEFile::GetDOSHead()
  {
    return reinterpret_cast<PIMAGE_DOS_HEADER>( m_pFileBuff );
  }

  PIMAGE_NT_HEADERS32 PEFile::GetNtHeaders32()
  {
    return reinterpret_cast<PIMAGE_NT_HEADERS32>( m_pFileBuff + GetDOSHead()->e_lfanew );
  }

  PIMAGE_FILE_HEADER PEFile::GetFileHead()
  {
    return reinterpret_cast<PIMAGE_FILE_HEADER> ( &GetNtHeaders32()->FileHeader );
  }

  PIMAGE_OPTIONAL_HEADER32 PEFile::GetOptionalHead32()
  {
    return reinterpret_cast<PIMAGE_OPTIONAL_HEADER32> ( &GetNtHeaders32()->OptionalHeader );
  }

  PIMAGE_SECTION_HEADER PEFile::GetSectionHead()
  {
    return reinterpret_cast<PIMAGE_SECTION_HEADER> ( (DWORD)GetOptionalHead32() + GetFileHead()->SizeOfOptionalHeader);
  }

  bool PEFile::IsValidPEFile()
  {
    PIMAGE_DOS_HEADER DOSHead = GetDOSHead();

    if ( DOSHead->e_magic != IMAGE_DOS_SIGNATURE )
    {
      return false;
    }

    PIMAGE_OPTIONAL_HEADER32 OptHead32 = GetOptionalHead32();

    if ( OptHead32->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC )
    {
      return false;
    }

    // more checks will be here ;)

    return true;
  }

  PIMAGE_SECTION_HEADER PEFile::AddSection( const char* szName, ULONG cbSizeOfRawData, ULONG Characteristics )
  {
    DWORD sectionIndex = GetFileHead()->NumberOfSections++;
    PIMAGE_SECTION_HEADER pSectionHead = GetSectionHead();
    PIMAGE_SECTION_HEADER pNewSection = &pSectionHead[sectionIndex];
    PIMAGE_SECTION_HEADER pLastSection = &pSectionHead[sectionIndex - 1];

    PIMAGE_OPTIONAL_HEADER32 pOptHead = GetOptionalHead32();

    pNewSection->VirtualAddress = AlignUp( pLastSection->VirtualAddress + pLastSection->Misc.VirtualSize, pOptHead->SectionAlignment );
    pNewSection->PointerToRawData = AlignUp( pLastSection->PointerToRawData + pLastSection->SizeOfRawData, pOptHead->FileAlignment );
    pNewSection->Misc.VirtualSize = AlignUp( cbSizeOfRawData, pOptHead->SectionAlignment );
    pNewSection->SizeOfRawData = AlignUp( cbSizeOfRawData, pOptHead->FileAlignment );
    pNewSection->PointerToRelocations = 0;
    pNewSection->PointerToLinenumbers = 0;
    pNewSection->NumberOfRelocations = 0;
    pNewSection->NumberOfLinenumbers = 0;
    pNewSection->Characteristics = Characteristics;
    strcpy_s( reinterpret_cast<char*>( pNewSection->Name ), IMAGE_SIZEOF_SHORT_NAME, szName );
    GetOptionalHead32()->SizeOfImage = AlignUp( pNewSection->VirtualAddress + pNewSection->Misc.VirtualSize, pOptHead->SectionAlignment );
    return pNewSection;
  }

  DWORD PEFile::AlignDown(DWORD val, DWORD align) const
  {
    return ( val & ~( align - 1 ) );
  }


  DWORD PEFile::AlignUp(DWORD val, DWORD align) const
  {
    return ( ( val & ( align - 1 ) ) ? AlignDown( val, align ) + align : val );
  }

  DWORD PEFile::RvaToFileOffset(DWORD rva)
  {
    PIMAGE_FILE_HEADER pFileHead = GetFileHead();
    PIMAGE_SECTION_HEADER pSectionHeader = GetSectionHead();
    for (int sectionIndex = 0; sectionIndex < pFileHead->NumberOfSections; ++sectionIndex)
    {
      IMAGE_SECTION_HEADER curSection = pSectionHeader[sectionIndex];

      if (rva >= curSection.VirtualAddress &&
        rva < curSection.VirtualAddress + curSection.Misc.VirtualSize)
      {
        return (rva - curSection.VirtualAddress + curSection.PointerToRawData);
      }
    }
    return rva;
  }

  DWORD PEFile::FileOffsetToRva(DWORD fileOffset)
  {
    PIMAGE_FILE_HEADER pFileHead = GetFileHead();
    PIMAGE_SECTION_HEADER pSectionHeader = GetSectionHead();
    for (int sectionIndex = 0; sectionIndex < pFileHead->NumberOfSections; ++ sectionIndex)
    {
      IMAGE_SECTION_HEADER curSection = GetSectionHead()[sectionIndex];
      if (fileOffset >= curSection.PointerToRawData &&
        fileOffset < curSection.PointerToRawData + curSection.SizeOfRawData)
      {
        return (fileOffset + curSection.VirtualAddress - curSection.PointerToRawData);
      }
    }
    return fileOffset;
  }

  PIMAGE_IMPORT_DESCRIPTOR PEFile::GetImportDescriptor()
  {
    return ( PIMAGE_IMPORT_DESCRIPTOR )( m_pFileBuff + RvaToFileOffset( GetOptionalHead32()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ) );
  }

  PIMAGE_THUNK_DATA PEFile::GetThunkData( PIMAGE_IMPORT_DESCRIPTOR pImportDescription )
  {
    return ( PIMAGE_THUNK_DATA ) ( m_pFileBuff + RvaToFileOffset( pImportDescription->Characteristics ) );
  }

  PIMAGE_IMPORT_BY_NAME PEFile::GetImportByName( PIMAGE_THUNK_DATA pThunkData )
  {
    return ( PIMAGE_IMPORT_BY_NAME ) ( m_pFileBuff + RvaToFileOffset( pThunkData->u1.AddressOfData ) );
  }

  WORD PEFile::GetNumberOfImportedDlls( )
  {
    PIMAGE_IMPORT_DESCRIPTOR pFirstImportDescriptor = GetImportDescriptor();
    WORD numberOfImportedDlls = 0;
    while( pFirstImportDescriptor->Name )
    {
      numberOfImportedDlls++;
      pFirstImportDescriptor++;
    }
    return numberOfImportedDlls;
  }

}
