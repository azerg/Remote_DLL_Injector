
// InjectorUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InjectorUI.h"
#include "InjectorUIDlg.h"
#include "DlgSettings.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SETTINGS_FILENAME "settings.log"

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CInjectorUIDlg dialog

CInjectorUIDlg::CInjectorUIDlg(CWnd* pParent /*=NULL*/)
  : CDialog(IDD_INJECTORUI_DIALOG, pParent),
  m_injectionManager{std::make_unique<InjectionMngr>(m_lbLogOutput)},
  m_uiInternalSettings{}
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectorUIDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDOK2, m_btnAbout);
  DDX_Control(pDX, IDCANCEL, m_btnExit);
  DDX_Control(pDX, IDC_EDIT_TARGET_PROCESS, m_editTargetProcess);
  DDX_Control(pDX, IDC_EDIT_DLL2INJECT, m_sourceDLLPath);
  DDX_Control(pDX, ID_BTN_PICK_DLL, m_btnPickDLL);
  DDX_Control(pDX, IDC_BUTTON_SETTIGNS, m_btnSettings);
  DDX_Control(pDX, IDC_LIST_OUTPUT, m_lbLogOutput);
}

BEGIN_MESSAGE_MAP(CInjectorUIDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDOK2, &CInjectorUIDlg::OnBnClickedAbout)
  ON_WM_NCHITTEST()
  ON_BN_CLICKED(IDCANCEL, &CInjectorUIDlg::OnBnClickedCancel)
  ON_BN_CLICKED(ID_BTN_DO_INJECT, &CInjectorUIDlg::OnBnClickedBtnDoInject)
  ON_BN_CLICKED(ID_BTN_PICK_DLL, &CInjectorUIDlg::OnBnClickedBtnPickDll)
  ON_BN_CLICKED(IDC_BUTTON_SETTIGNS, &CInjectorUIDlg::OnBnClickedButtonSettigns)
END_MESSAGE_MAP()


// CInjectorUIDlg message handlers

BOOL CInjectorUIDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);      // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  // TODO: Add extra initialization here

  SettingsMngr settingsMngr(SETTINGS_FILENAME);
  // load & apply saved settings
  ApplySettings(settingsMngr.Read());

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInjectorUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInjectorUIDlg::OnPaint()
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInjectorUIDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CInjectorUIDlg::OnBnClickedAbout()
{
  //CAboutDlg dlgAbout;
  //dlgAbout.DoModal();
  LoadLibrary("SampleDll.dll"); // todo(azerg): remove this for test only!!
}

LRESULT CInjectorUIDlg::OnNcHitTest(CPoint point)
{
  return HTCAPTION;
}

void CInjectorUIDlg::OnBnClickedCancel()
{
  auto settings = GetSettingsFromControls();

  SettingsMngr settingsMngr(SETTINGS_FILENAME);
  settingsMngr.Save(std::move(settings));

  CDialog::OnCancel();
}

std::string GetStringFromEdit(const CEdit& edit)
{
  std::string result;
  CString mfcStr;
  edit.GetWindowTextA(mfcStr);
  if (!mfcStr.IsEmpty())
  {
    result = mfcStr.GetBuffer();
  }
  return result;
}

Settings CInjectorUIDlg::GetSettingsFromControls() const
{
  Settings settings;
  // fill settings here

  // save last wnd pos
  RECT curRect;
  CDialog::GetWindowRect(&curRect);
  settings.topLeftX = curRect.left;
  settings.topLeftY = curRect.top;

  settings.lastDLLPath = GetStringFromEdit(m_sourceDLLPath);
  settings.lastTargetProcess = GetStringFromEdit(m_editTargetProcess);

  settings.injOpts = m_uiInternalSettings.injOpts;

  return settings;
}

void SetEditTextString(CEdit& srcEdit, std::string string)
{
  if (!string.empty())
  {
    srcEdit.SetWindowTextA(string.c_str());
  }
}

void CInjectorUIDlg::ApplySettings(const Settings& settings)
{
  if (settings.topLeftX && settings.topLeftY)
  {
    RECT curRect{};
    CDialog::GetWindowRect(&curRect);
    CDialog::SetWindowPos(
      nullptr,
      settings.topLeftX,
      settings.topLeftY,
      curRect.right - curRect.left,
      curRect.bottom - curRect.top,
      0);
  }

  SetEditTextString(m_sourceDLLPath, settings.lastDLLPath);
  SetEditTextString(m_editTargetProcess, settings.lastTargetProcess);

  m_uiInternalSettings.injOpts = settings.injOpts;
}


void CInjectorUIDlg::OnBnClickedBtnDoInject()
{
  auto settings = GetSettingsFromControls();

  auto reslt = m_injectionManager->DoInject(
    settings.lastTargetProcess.c_str(),
    settings.lastDLLPath.c_str(),
    {
      settings.injOpts.removeExtraSections,
      settings.injOpts.removePEHeader,
      settings.injOpts.randomHead,
      settings.injOpts.randomTail,
      settings.injOpts.injectWithLocalDll,
      settings.injOpts.randomMax
    }
  );

  if (!reslt)
  {
    m_lbLogOutput.AddString("Success.");
  }
  else
  {
    auto msg = "Error! Code: " + std::to_string(*reslt);
    m_lbLogOutput.AddString(msg.c_str());
  }
}


void CInjectorUIDlg::OnBnClickedBtnPickDll()
{
  OPENFILENAME ofn{};       // common dialog box structure
  char szFile[MAX_PATH]{};  // buffer for file name

  LPCSTR lpstrInitialDir = nullptr;
  auto sourceDLL = GetStringFromEdit(m_sourceDLLPath);
  if (!sourceDLL.empty())
  {
    lpstrInitialDir = sourceDLL.c_str();
  }

  // Initialize OPENFILENAME
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = m_hWnd;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "All\0*.*\0DLL to inject\0*.dll\0";
  ofn.nFilterIndex = 2;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = lpstrInitialDir;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  // Display the Open dialog box.

  if (GetOpenFileName(&ofn) == TRUE)
  {
    m_sourceDLLPath.SetWindowTextA(ofn.lpstrFile);
  }
}

void CInjectorUIDlg::OnBnClickedButtonSettigns()
{
  CDlgSettings dlgSettings(m_uiInternalSettings);
  dlgSettings.DoModal();
}
