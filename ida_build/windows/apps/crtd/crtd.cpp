// crtd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "crtd.h"

#include "MainFrame.h"
#include <locale.h>
#include "crtdglob.h"
#include "Splash.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrtdApp

BEGIN_MESSAGE_MAP(CCrtdApp, CWinApp)
	//{{AFX_MSG_MAP(CCrtdApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrtdApp construction

CCrtdApp::CCrtdApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCrtdApp object

CCrtdApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCrtdApp initialization

BOOL CCrtdApp::InitInstance()
	{
	// CG: The following block was added by the Splash Screen component.
\
	{
\
		CCommandLineInfo cmdInfo;
\
		ParseCommandLine(cmdInfo);
\

\
//		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
\
	}
	// CG: The following block was inserted by 'Status Bar' component.
	{
		//Set up date and time defaults so they're the same as system defaults
		setlocale(LC_ALL, "");
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

//	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	LoadGlobalParamFromIni();


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

/*	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCrtdDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCrtdView));
	AddDocTemplate(pDocTemplate);*/

	// Parse command line for standard shell commands, DDE, file open
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	HBRUSH hBrush=(HBRUSH)GetStockObject(WHITE_BRUSH);
	HCURSOR hCur=(HCURSOR)::LoadCursor(NULL,IDC_ARROW);
	HICON hIco=LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	LPCTSTR lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);

	MainFrame *pMainFrame = new MainFrame;
	RECT rcM = { 0,0,100,100 };
	::GetWindowRect(::GetDesktopWindow(),&rcM);
	if (!pMainFrame->Create(lpClass,"Continuous Real-Time Display", WS_VISIBLE | WS_OVERLAPPEDWINDOW,rcM,NULL,NULL))	return FALSE;

	m_pMainWnd = pMainFrame;
	pMainFrame->ShowWindow(m_nCmdShow|SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();


	return TRUE;
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
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
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
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCrtdApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCrtdApp commands

BOOL CCrtdApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
/*	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;*/

	return CWinApp::PreTranslateMessage(pMsg);
}
