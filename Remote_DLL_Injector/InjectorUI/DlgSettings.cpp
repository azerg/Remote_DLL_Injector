// DlgSettings.cpp : implementation file
//

#include "stdafx.h"
#include "InjectorUI.h"
#include "DlgSettings.h"
#include "afxdialogex.h"


// CDlgSettings dialog

IMPLEMENT_DYNAMIC(CDlgSettings, CDialogEx)

CDlgSettings::CDlgSettings(UIInternalSettings& uiInternalSettings, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_SETTINGS, pParent),
  m_uiInternalSettings(uiInternalSettings)
{

}

CDlgSettings::~CDlgSettings()
{
}

void CDlgSettings::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHECK_REMOVE_EXTRA_SECTIONS, m_cbRemoveExtraSections);
  DDX_Control(pDX, IDC_CHECK_REMOVE_PE_HEADER, m_cbRemovePEHeader);
  DDX_Control(pDX, IDC_CHECK_RANDOM_HEAD, m_cbRandomHead);
  DDX_Control(pDX, IDC_CHECK_RANDOM_TAIL, m_cbRandomTail);
  DDX_Control(pDX, IDC_CHECK_INJECT_WITH_LOCAL_DLL, m_cbInjectWithLocalDLL);
  DDX_Control(pDX, ID_BTN_SAVE, m_btnSave);
}


BEGIN_MESSAGE_MAP(CDlgSettings, CDialogEx)
  ON_BN_CLICKED(ID_BTN_SAVE, &CDlgSettings::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CDlgSettings message handlers

BOOL CDlgSettings::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  m_cbRemoveExtraSections.SetCheck( m_uiInternalSettings.injOpts.removeExtraSections  ? BST_CHECKED : BST_UNCHECKED);
  m_cbRemovePEHeader.SetCheck(      m_uiInternalSettings.injOpts.removePEHeader       ? BST_CHECKED : BST_UNCHECKED);
  m_cbRandomHead.SetCheck(          m_uiInternalSettings.injOpts.randomHead           ? BST_CHECKED : BST_UNCHECKED);
  m_cbRandomTail.SetCheck(          m_uiInternalSettings.injOpts.randomTail           ? BST_CHECKED : BST_UNCHECKED);
  m_cbInjectWithLocalDLL.SetCheck(  m_uiInternalSettings.injOpts.injectWithLocalDll   ? BST_CHECKED : BST_UNCHECKED);

  return TRUE;
}

void CDlgSettings::OnBnClickedBtnSave()
{
  m_uiInternalSettings.injOpts.removeExtraSections =  m_cbRemoveExtraSections.GetCheck() == BST_CHECKED;
  m_uiInternalSettings.injOpts.removePEHeader =       m_cbRemovePEHeader.GetCheck() == BST_CHECKED;
  m_uiInternalSettings.injOpts.randomHead =           m_cbRandomHead.GetCheck() == BST_CHECKED;
  m_uiInternalSettings.injOpts.randomTail =           m_cbRandomTail.GetCheck() == BST_CHECKED;
  m_uiInternalSettings.injOpts.injectWithLocalDll =   m_cbInjectWithLocalDLL.GetCheck() == BST_CHECKED;

  CDialog::OnCancel();
}
