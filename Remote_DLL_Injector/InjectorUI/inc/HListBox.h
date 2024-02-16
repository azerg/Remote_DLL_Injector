#pragma once

class CHListBox : public CListBox
{
public:
  CHListBox() : width{}
  {}

  int AddString(LPCTSTR s);
  int InsertString(int i, LPCTSTR s);
  void ResetContent();
  int DeleteString(int i);

private:
  void updateWidth(LPCTSTR s);
  int width;

  DECLARE_MESSAGE_MAP()
};
