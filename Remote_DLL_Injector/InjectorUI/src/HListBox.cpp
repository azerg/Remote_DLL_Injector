// HListBox.cpp : implementation file
//

#include "stdafx.h"
#include "HListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CHListBox, CListBox)
END_MESSAGE_MAP()

void CHListBox::updateWidth(LPCTSTR s)
{
  CClientDC dc(this);
  CFont * f = CListBox::GetFont();
  dc.SelectObject(f);
  CSize sz = dc.GetTextExtent(s, _tcslen(s));
  sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
  if (sz.cx > width)
  {
    width = sz.cx;
    CListBox::SetHorizontalExtent(width);
  }
}

int CHListBox::AddString(LPCTSTR s)
{
  int result = CListBox::AddString(s);
  if (result < 0)
    return result;
  updateWidth(s);

  CListBox::SetCaretIndex(result);

  return result;
}

int CHListBox::InsertString(int i, LPCTSTR s)
{
  int result = CListBox::InsertString(i, s);
  if (result < 0)
    return result;
  updateWidth(s);
  return result;
}

void CHListBox::ResetContent()
{
  CListBox::ResetContent();
  width = 0;
}

int CHListBox::DeleteString(int n)
{
  int result = CListBox::DeleteString(n);
  if (result < 0)
    return result;
  CClientDC dc(this);

  CFont * f = CListBox::GetFont();
  dc.SelectObject(f);

  width = 0;
  for (int i = 0; i < CListBox::GetCount(); i++)
  {
    CString s;
    CListBox::GetText(i, s);
    CSize sz = dc.GetTextExtent(s);
    sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER);
    if (sz.cx > width)
      width = sz.cx;
  }
  CListBox::SetHorizontalExtent(width);
  return result;
}
