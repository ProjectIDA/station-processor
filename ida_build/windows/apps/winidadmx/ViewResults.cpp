// ViewResults.cpp : implementation file
//

#include "stdafx.h"
#include "winidadmx.h"
#include "ViewResults.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewResults dialog


CViewResults::CViewResults(LPCSTR csText, CWnd* pParent /*=NULL*/)
	: CDialog(CViewResults::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CViewResults)
	//}}AFX_DATA_INIT
	text=csText;
	ParserForOutputFileName();
	}

void CViewResults::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewResults)
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CViewResults, CDialog)
	//{{AFX_MSG_MAP(CViewResults)
	ON_BN_CLICKED(IDC_RUNVIEWDATA, OnRunviewdata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewResults message handlers

void CViewResults::OnOK() 
	{
	// TODO: Add extra validation here
	CDialog::OnOK();
	}
void CViewResults::ParserForOutputFileName()
	{
	char cOutputFileName[MAX_PATH];
	static char *cpPattern="Output file name:";
	char *cp=strstr(text, cpPattern);
	*cOutputFileName=0;
	int i=0;
	if(cp!=NULL)
		{
		cp+=lstrlen(cpPattern);
		for(i=0; i<sizeof(cOutputFileName); ++i, ++cp)
			{
			if(*cp==0 || *cp==0xd || *cp==0xa)
				{
				cOutputFileName[i]=0;
				break;
				}
			else
				{
				cOutputFileName[i]=*cp;
				}
			}
		}
	CFileName=cOutputFileName;
	}


void CViewResults::OnRunviewdata() 
	{
	// TODO: Add your control notification handler code here
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    ZeroMemory( &pi, sizeof(pi) );
	
    si.cb = sizeof(si);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_NORMAL;


	CString ExecString="itsv.exe \x22"+CFileName+"\x22";


	BOOL bStart=CreateProcess(NULL,
    (LPSTR)(LPCSTR)ExecString,  // pointer to command line string
    NULL,  // process security attributes
    NULL,   // thread security attributes
    FALSE,  // handle inheritance flag
	NULL, //CREATE_NEW_CONSOLE , // creation flags
    NULL,  // pointer to new environment block
    NULL,   // pointer to current directory name
	&si,  // pointer to STARTUPINFO
	&pi  // pointer to PROCESS_INFORMATION
	);
	if(bStart)
		{
//		::WaitForSingleObject(pi.hProcess, -1);

		}
	else
		{
		MessageBox("Can not start itsv.exe",NULL, MB_ICONSTOP);
		}


	}

BOOL CViewResults::OnInitDialog() 
	{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_RESULTS, text);
	
	if(CFileName.IsEmpty())
		{
		GetDlgItem(IDC_RUNVIEWDATA)->EnableWindow(FALSE);
		}
	
	return TRUE;  
	              
	}
