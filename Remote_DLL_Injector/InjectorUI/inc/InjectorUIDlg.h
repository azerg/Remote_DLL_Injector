
// InjectorUIDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Settings.h"
#include "InjectionMngr.h"
#include "HListBox.h"
#include <memory>

// CInjectorUIDlg dialog
class CInjectorUIDlg : public CDialog
{
// Construction
public:
  CInjectorUIDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_INJECTORUI_DIALOG };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

private:
  Settings GetSettingsFromControls() const;
  void ApplySettings(const Settings& settings);

// Implementation
protected:
  HICON m_hIcon;
  std::unique_ptr<InjectionMngr> m_injectionManager;
  UIInternalSettings m_uiInternalSettings;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
public:
  CButton m_btnAbout;
  CButton m_btnExit;
  CEdit m_editTargetProcess;
  // Path to DLL that will be injected in  @target_process
  CEdit m_sourceDLLPath;
  afx_msg void OnBnClickedAbout();
  afx_msg LRESULT OnNcHitTest(CPoint point);
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedBtnDoInject();
  CButton m_btnPickDLL;
  CButton m_btnSettings;
  afx_msg void OnBnClickedBtnPickDll();
  afx_msg void OnBnClickedButtonSettigns();
  CEdit m_edtStatusLogs;
  CHListBox m_lbLogOutput;
};
