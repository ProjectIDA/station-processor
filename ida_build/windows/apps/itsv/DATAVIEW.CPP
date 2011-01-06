// DataView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DataView.h"
#include "MainFrm.h"
#include <locale.h>
#include "Splash.h"
#include "globals.h"
#include "Label.h"
#include "filter\filter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern struct gather_plot_filter_ gpl_filters_def[];
extern int GPL_NFILTERS_DEF;

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp

BEGIN_MESSAGE_MAP(CDataViewApp, CWinApp)
	//{{AFX_MSG_MAP(CDataViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp construction

CDataViewApp::CDataViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDataViewApp object

CDataViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp initialization

BOOL CDataViewApp::InitInstance()
	{
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
//	ProcessShellCommand(cmdInfo);

	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
		//Set up date and time defaults so they're the same as system defaults
	setlocale(LC_ALL, "usa");

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	AfxEnableControlContainer();

	HBRUSH hBrush=(HBRUSH)GetStockObject(WHITE_BRUSH);
	HCURSOR hCur=(HCURSOR)::LoadCursor(NULL,IDC_ARROW);
	HICON hIco=LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	LPCTSTR lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);

	CMainFrame *pMainFrame = new CMainFrame;
	RECT rcM = { 0,0,100,100 };
	::GetWindowRect(::GetDesktopWindow(),&rcM);
	if (!pMainFrame->Create(lpClass,"Interactive Time Series Viewer", WS_VISIBLE | WS_OVERLAPPEDWINDOW,rcM,NULL,NULL))	return FALSE;

	m_pMainWnd = pMainFrame;
	pMainFrame->ShowWindow(m_nCmdShow|SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

	pMainFrame->MaiFrameMenu.LoadMenu(IDR_MAINFRAME);

	pMainFrame->SetMenu(&pMainFrame->MaiFrameMenu);
	pMainFrame->SendMessage(WM_COMMAND,IDC_AUTOAMP);
	LoadGainMenu();
	LoadFilterMenu();
	return TRUE;

	}


void CDataViewApp::LoadGainMenu()
	{
	CMainFrame *pMainFrame =(CMainFrame *)m_pMainWnd;
	char cBuff[128],cRes[64],cGain[32];
	CMenu *sub1=pMainFrame->MaiFrameMenu.GetSubMenu(2);

	for(int i=0; i<20; ++i)
		{
		sprintf(cGain,"gain%d",i);
		GetPrivateProfileString("GAIN",cGain,"",cRes,sizeof(cRes),cpIniFileName);
		if(*cRes==0) break;
		lstrcpy(cBuff,"GAIN");lstrcat(cBuff,cRes);
		sub1->AppendMenu(MF_STRING, IDC_GAIN1+i,cBuff); 
		}
//	pMainFrame->PostMessage(WM_COMMAND,ID_FILE_OPEN);
	}
void CDataViewApp::LoadFilterMenu()
	{
	CMainFrame *pMainFrame =(CMainFrame *)m_pMainWnd;
	char cBuff[128],cRes[64],cGain[32];
	CMenu *sub1=pMainFrame->MaiFrameMenu.GetSubMenu(4);
	sub1=pMainFrame->MaiFrameMenu.GetSubMenu(2);
	sub1=pMainFrame->MaiFrameMenu.GetSubMenu(3);

	for(int i=0; i<GPL_NFILTERS_DEF; ++i)
		{
		lstrcpy(cBuff,gpl_filters_def[i].label);
		sub1->AppendMenu(MF_STRING, IDC_FILTER1+i,cBuff); 
		}

	}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CLabel	m_essw;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_ESSWSTATIC, m_essw);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDataViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp commands

BOOL CDataViewApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_essw.SetLink(TRUE)
		.SetTextColor(RGB(0,0,255))
		.SetFontUnderline(TRUE)
		.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_HAND));

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
