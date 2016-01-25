#pragma once

#include "Settings.h"
#include "afxwin.h"

// CDlgSettings dialog

class CDlgSettings : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSettings)

public:
	CDlgSettings(UIInternalSettings& uiInternalSettings, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSettings();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SETTINGS };
#endif

protected:
  virtual BOOL OnInitDialog();
  UIInternalSettings& m_uiInternalSettings;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  CButton m_cbRemoveExtraSections;
  CButton m_cbRemovePEHeader;
  CButton m_cbRandomHead;
  CButton m_cbRandomTail;
  CButton m_cbInjectWithLocalDLL;

	DECLARE_MESSAGE_MAP()
public:
  CButton m_btnSave;
  afx_msg void OnBnClickedBtnSave();
};
