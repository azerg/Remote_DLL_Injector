
// InjectorUIDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CInjectorUIDlg dialog
class CInjectorUIDlg : public CDialog
{
// Construction
public:
	CInjectorUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECTORUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  CButton m_btnAbout;
  CButton m_btnExit;
  afx_msg void OnBnClickedAbout();
  afx_msg LRESULT OnNcHitTest(CPoint point);
};
