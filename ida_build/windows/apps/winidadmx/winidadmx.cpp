// winidadmx.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "winidadmx.h"
#include "winidadmxDlg.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxApp

BEGIN_MESSAGE_MAP(CWinidadmxApp, CWinApp)
	//{{AFX_MSG_MAP(CWinidadmxApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxApp construction

CWinidadmxApp::CWinidadmxApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWinidadmxApp object

CWinidadmxApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxApp initialization

BOOL CWinidadmxApp::InitInstance()
	{
	AfxEnableControlContainer();

	if (!AfxOleInit())
	{
		AfxMessageBox("AfxOleInit Error!");
		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	ParseCmd();

	CWinidadmxDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
		{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		}
	else if (nResponse == IDCANCEL)
		{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
		}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
	}
void CWinidadmxApp::ParseCmd()
	{
    UINT type;
	CString s;
	char const *pCD = "/CD=";
	char cDisk[8];
	for(int i=0; i<__argc; ++i)
		{
		s=__argv[i];
		s.MakeUpper();

		if(strncmp(s,pCD,4)==0)
			{
			csCD = (LPCSTR)s+4;

			cDisk[0]=((LPCSTR)csCD)[0];
			cDisk[1]=':';
			cDisk[2]='\\';
			cDisk[3]=0;

			type = GetDriveType(cDisk);
			if(type==DRIVE_CDROM)
				{
				csCD=cDisk;
				bStartFromCD=TRUE;
				}
			}
		}
	}