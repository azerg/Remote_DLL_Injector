/* pelib.h --

   This file is part of the "PE Maker".

   Copyright (C) 2005-2006 Ashkbiz Danehkar
   All Rights Reserved.

   "PE Maker" library are free software; you can redistribute them
   and/or modify them under the terms of the GNU General Public License as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYRIGHT.TXT.
   If not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   yodap's Forum:
   http://yodap.sourceforge.net/forum/

   yodap's Site:
   http://yodap.has.it
   http://yodap.cjb.net
   http://yodap.sourceforge.net

   Ashkbiz Danehkar
   <ashkbiz@yahoo.com>
*/
#pragma once
#define MAX_SECTION_NUM         20
//----------------------------------------------------------------
class CPELibrary 
{
private:
  //-----------------------------------------
  PCHAR          pMem;
  DWORD          dwFileSize;
  //-----------------------------------------
protected:
  //-----------------------------------------
  PIMAGE_DOS_HEADER    image_dos_header;
  PCHAR          pDosStub;
  DWORD          dwDosStubSize, dwDosStubOffset;
  PIMAGE_NT_HEADERS    image_nt_headers;
  PIMAGE_SECTION_HEADER  image_section_header[MAX_SECTION_NUM];
  PCHAR          image_section[MAX_SECTION_NUM];
  //-----------------------------------------
public:
  //-----------------------------------------
  DWORD PEAlign(DWORD dwTarNum,DWORD dwAlignTo);
  void AlignmentSections();
  //-----------------------------------------
  DWORD Offset2RVA(DWORD dwRO);
  DWORD RVA2Offset(DWORD dwRVA);
  //-----------------------------------------
  PIMAGE_SECTION_HEADER ImageRVA2Section(DWORD dwRVA);
  PIMAGE_SECTION_HEADER ImageOffset2Section(DWORD dwRO);
  //-----------------------------------------
  DWORD ImageOffset2SectionNum(DWORD dwRVA);
  PIMAGE_SECTION_HEADER AddNewSection(char* szName,DWORD dwSize);
  //-----------------------------------------
public:
  //-----------------------------------------
  CPELibrary();
  ~CPELibrary();
  //-----------------------------------------
  int OpenFile(const char* FileName);
  int SaveFile(const char* FileName);  
  //-----------------------------------------
};
//----------------------------------------------------------------