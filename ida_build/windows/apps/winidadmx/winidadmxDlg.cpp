// winidadmxDlg.cpp : implementation file
//
#include "stdafx.h"
#include <afxmt.h>
#include "winidadmx.h"
#include "winidadmxDlg.h"
#include "devlist.h"
#include "ViewResults.h"
#include "SelectFilesOnCDDlg.h"
#include "win32/zlib.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <dbt.h>
#include "globals.h"
#include <winsock2.h>
#include "ida.h"
#include "ida10.h"
#include "BrowseSubdirDlg.h"
#include "ViewLstDlg.h"


extern "C"
 {
 int dhead_rev7(IDA *db, IDA_DHDR *dest, UINT8 *src);
 }


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct OutputFileTimeThreadParam
	{
	CWinidadmxDlg *pDlg;
	CString sFile;
	int nIndex;
	}
OutputFileTimeThreadParam;

typedef struct 
	{
	char fname[256];
	DWORD dwSize;
	double tbeg;
	double tend;
	int nType;
	}
CDFile;

CArray <CDFile *, CDFile *> CDFileArray;
long *ArrayIndex=NULL;
CMapStringToPtr HashTable;

CRITICAL_SECTION critFS;

int CALLBACK      BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

bool InitASPI();
DevList *ScanSCSI(HWND hwnd);

static char cTmpBuffer[32000000];
static char cOutputFileName[MAX_PATH]={0};

char *pIniFileName=".\\winidadmx.ini";
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

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxDlg dialog

CWinidadmxDlg::CWinidadmxDlg(CWnd* pParent /*=NULL*/)
	: CDialogWB(CWinidadmxDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CWinidadmxDlg)
	m_inputidstring = _T("");
	m_outputdatadirectory = _T("");
	m_startdate = _T("");
	m_enddate = _T("");
	m_discardlist = _T("");
	m_records = _T("");
	m_inputformat = _T("");
	m_outputdirectory = _T("");
	m_workingdirectory = _T("");
	m_writerawheader = FALSE;
	m_swapbyte = FALSE;
	m_unpackall = -1;
	m_inputfrom = -1;
	m_record1 = 0;
	m_record2 = 0;
	m_stacode = _T("");
	m_dbdir = _T("");
	m_timetagsummary = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_swapbyte=TRUE;
	m_inputfrom=0;
	m_unpackall=0;
	m_stacode="pfo";
	m_showfiletime=true;
	m_pDragList = NULL;
	m_CheckWorkingDirectoryFlag = 0;
	}

CWinidadmxDlg::~CWinidadmxDlg()
	{
	ClearCDFileArray();
	if(m_pDragList!=NULL)
		{
		delete m_pDragList; 
		m_pDragList=NULL;
		}
	}

void CWinidadmxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinidadmxDlg)
	DDX_Control(pDX, IDC_DATASOURCE1, m_inputcombo);
	DDX_CBString(pDX, IDC_INPUTFORMAT, m_inputformat);
	DDX_Text(pDX, IDC_OUTPUTDIRECTORY, m_outputdirectory);
	DDX_Text(pDX, IDC_WORKINGDIRECTORY, m_workingdirectory);
	DDX_Check(pDX, IDC_WRITERAWHEADER, m_writerawheader);
	DDX_Check(pDX, IDC_BYTESWAPINTOHOSTORDER, m_swapbyte);
	DDX_Radio(pDX, IDC_UNPACKALL, m_unpackall);
	DDX_Radio(pDX, IDC_INPUTFROMTAPE, m_inputfrom);
	DDX_Text(pDX, IDC_RECORD1, m_record1);
	DDX_Text(pDX, IDC_RECORD2, m_record2);
	DDX_Text(pDX, IDC_STACODE, m_stacode);
	DDX_Text(pDX, IDC_DBDIR, m_dbdir);
	DDX_Check(pDX, IDC_TIMETAGSUMMARY, m_timetagsummary);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinidadmxDlg, CDialog)
	//{{AFX_MSG_MAP(CWinidadmxDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BROWSEOUTPUTDATADIR, OnBrowseoutputdatadir)
	ON_BN_CLICKED(IDC_BROWSECHANMAP, OnBrowsechanmap)
	ON_BN_DOUBLECLICKED(IDC_BROWSE, OnDoubleclickedBrowse)
	ON_BN_CLICKED(IDC_INPUTFROMTAPE, OnInputfromtape)
	ON_BN_CLICKED(IDC_IDCINPUTFROMCD, OnInputfromCD)
	ON_BN_CLICKED(IDC_IDCINPUTFROMDISK, OnIdcinputfromdisk)
	ON_COMMAND( IDC_EXITIDADMX, OnExitIDADMX )
	ON_COMMAND( IDC_EXITIDALST, OnExitIDALST )
	ON_COMMAND( IDCANCEL, OnCancel)
	ON_COMMAND( IDC_BROWSEOUTPUTDATASUBDIR, OnBrowseSubdir)
	ON_BN_CLICKED(IDC_UNPACKALL, OnUnpackall)
	ON_BN_CLICKED(IDC_UNPACKRANGE, OnUnpackrange)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_CBN_SELCHANGE(IDC_INPUTFORMAT, OnSelchangeInputformat)
	ON_BN_CLICKED(IDC_SELALL, OnSelall)
	ON_BN_CLICKED(IDC_UNSELALL, OnUnselall)
	ON_CBN_SELCHANGE(IDC_CDCOMBO, OnSelchangeDriveCombo)
	ON_CBN_SELCHANGE(IDC_ADDRESSCOMBO, OnSelchangeAddresscombo)
	ON_BN_CLICKED(IDC_ENTERADDRESS, OnEnteraddress)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclklist)
	ON_EN_KILLFOCUS(IDC_WORKINGDIRECTORY, CheckWorkingDirectory)
//	ON_BN_CLICKED(ID_RUNIDALST, OnIdalstButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWinidadmxDlg::OnSelchangeDriveCombo() 
	{
//	LoadCDFileNames();
	SetDataDirFromCD();
	UpdateListView();
	LoadInfoFromDisk();
	}


BOOL CWinidadmxDlg::RestoreOriginalDirectory()
	{
	return SetCurrentDirectory((LPCSTR)m_currentDirectory);
	}

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxDlg message handlers

BOOL CWinidadmxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitializeCriticalSection(&critFS);

	m_pDragList = new CDragList();
	m_pDragList->SubclassDlgItem(IDC_LIST,this);

	char cDir[MAX_PATH];
	GetCurrentDirectory(sizeof(cDir), cDir);
	m_currentDirectory = cDir;

	m_dataDirectory = m_currentDirectory;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);


	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetupImages();
	LoadParam();
	SetListHeader();
	LoadMapFileNames();
	LoadCDDeviceNames();
	LoadDriveNames();
	UpdateData(FALSE);

	switch(m_inputfrom)
		{
		case 0:
			OnInputfromtape();
			break;
		case 1:
			OnIdcinputfromdisk();
			break;
		case 2:
			OnInputfromCD();
			break;

		}

	if(bStartFromCD)
		{
		GetDlgItem(IDC_INPUTFROMTAPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_IDCINPUTFROMDISK)->EnableWindow(FALSE);
		}

	OnSelchangeInputformat();


	AddFunctionHelpButton(IDOK, "Run IDADMX utility");
//	AddFunctionHelpButton(ID_RUNIDALST, "Run IDALST utility");
	AddFunctionHelpButton(IDCANCEL, "Cancel");
	AddFunctionHelpButton(IDC_BROWSECHANMAP, "Browse database directory");
	AddFunctionHelpButton(IDC_BROWSEOUTPUTDATADIR, "Browse output directory");
//   AddFunctionHelpButton(IDC_BROWSEOUTPUTDATADIR, "Browse output directory");
//	AddFunctionHelpButton(IDC_BROWSE, "Select input file");
//	AddFunctionHelpButton(IDC_ABOUT, "About",true);
	AddFunctionHelpButton(IDC_UNSELALL, "Unselect all files from the file list");
	AddFunctionHelpButton(IDC_SELALL, "Select all files in the file list");
	AddFunctionHelpButton(IDC_BROWSEOUTPUTDATASUBDIR, "Manage output directories");
	
	CShadeButtonST *pBt = AddFunctionHelpButton(IDC_ENTERADDRESS, "Enter address");
	pBt->SetIcon(IDI_ENTER);
	pBt->SetFlat(true);

	

	SelectMap();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWinidadmxDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWinidadmxDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinidadmxDlg::OnQueryDragIcon()
	{
	return (HCURSOR) m_hIcon;
	}

void CWinidadmxDlg::Process1()
	{
	bool bGZfile=false;
	int nItem;
	int nRev;
	char cSource[MAX_PATH+2];
	char cMapFile[MAX_PATH+2];
	UpdateData(TRUE);
	CWnd *pWnd;
	char cBuff[32000]={0};

	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	nItem = pCombo->GetCurSel();
	pCombo->GetLBText(nItem, cMapFile);


	ExecString=" ";

	if(CheckDir((LPCSTR)(m_workingdirectory+"\\"+m_outputdirectory)))
		{
		MessageBox("Directory already exists","Error", MB_ICONSTOP);
		pWnd=GetDlgItem(IDC_OUTPUTDIRECTORY);
		pWnd->SetFocus();
		return;
		}

	if(m_inputformat.IsEmpty())
		{
		MessageBeep(0);
		GetDlgItem(IDC_INPUTFORMAT)->SetFocus();
		return;
		}
	else
		{
		CString sRev=m_inputformat.Left(2);
		nRev=atoi((LPCSTR)sRev);
		if(nRev!=10)
			{
			ExecString+="rev="+sRev+" ";
			}
		}



	CString sFile;
//	GetDlgItemText(IDC_DATASOURCE, cSource, sizeof(cSource));


	if(cSource[0]==0) 
		{
		MessageBeep(0);
		return;
		}
	sFile  = cSource;
	if(sFile.Right(3).CompareNoCase(".gz")==0)
		{
		if(ProcessInputFromGZfile(sFile))
			{
			return;
			}
		bGZfile=true;
		}

	if(bGZfile)
		{
		ExecString=ExecString+"if=\x22"+cTempFileForDataFromCD+"\x22 ";
		}
	else
		{
		ExecString=ExecString+"if=\x22"+cSource+"\x22 ";
		}

	if(!m_workingdirectory.IsEmpty())
		{
		ExecString=ExecString+"outdir=\x22"+m_workingdirectory+"\\"+m_outputdirectory+"\x22 ";
		}

	if(m_unpackall>0)
		{
		CString srng;
		srng.Format("range=%d,%d",m_record1,m_record2);
		ExecString+=srng;
		}
	if(nRev<10)
	if(m_swapbyte)
		{
		ExecString=ExecString+"+swab"+" ";
		}
	else
		{
		ExecString+=ExecString+"-swab"+" ";
		}
	if(nRev<10)
	if(m_writerawheader)
		{
		ExecString=ExecString+"+headers"+" ";
		}
	else
		{
		ExecString=ExecString+"-headers"+" ";
		}
	if(nRev<10)
	if(!m_dbdir.IsEmpty())
		{
		ExecString+="db=\x22"+m_dbdir+"\x22 ";
		}
	if(nRev>9 && m_timetagsummary)
		{
		ExecString=ExecString+"+ttag"+" ";      
		}

	if(m_stacode.IsEmpty()) 
		{
		MessageBeep(0);
		GetDlgItem(IDC_STACODE)->SetFocus();
		return;
		}
	else
		{
		ExecString=ExecString+"sta="+m_stacode+" ";
		}

	if((*cMapFile)&&(nRev<10))
		{
		ExecString=ExecString+"map="+cMapFile+" ";
		}


	if(nRev<10)
		{
		ExecString="idadmx.exe "+ExecString;
		}
	else
		{
		ExecString="i10dmx.exe "+ExecString;
		}

	SaveParam();

    DWORD WINAPI DMXThread(LPVOID lpV);
    DWORD dwId;

	HANDLE hThread=CreateThread(NULL,0,DMXThread, this,0,&dwId);
    SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

    CloseHandle(hThread);
	WorkingDlg=new CWorkingDlg(this);
	EnableWindow(false);



	}
void CWinidadmxDlg::OnOK() 
	{
//	CWaitCursor cw;
	bool bGZfile=false;
	int nItem;
	int nRev;
	char cSource[MAX_PATH+2];
	char cMapFile[MAX_PATH+2];
	UpdateData(TRUE);
	CWnd *pWnd;
	char cBuff[32000]={0};

	HWND h1 = ::GetParent(::GetFocus());
	HWND h2 = ::GetDlgItem(this->m_hWnd, IDC_ADDRESSCOMBO);
	if(h1==h2)
		{
		OnEnteraddress();
		return;
		}
	h1 = ::GetFocus();
	h2 = ::GetDlgItem(this->m_hWnd, IDC_LIST);
	if(h1==h2)
		{
		EnterDirectory();
		return;
		}
		

	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	nItem = pCombo->GetCurSel();
	pCombo->GetLBText(nItem, cMapFile);

	WritePrivateProfileString("MAP FILES",(LPCSTR)m_stacode,cMapFile,pIniFileName);


	ExecString=" ";

	if(CheckDir((LPCSTR)(m_workingdirectory+"\\"+m_outputdirectory)))
		{
		MessageBox("Directory already exists","Error", MB_ICONSTOP);
		pWnd=GetDlgItem(IDC_OUTPUTDIRECTORY);
		pWnd->SetFocus();
		return;
		}

/*	if(m_inputfrom==1)
		{
		Process1();
		return;
		}*/

	if(m_inputfrom==2 || m_inputfrom==1)
		{
		if(ProcessInputFromList()!=0)
			{
			return;
			}
		}
	
	UpdateData(TRUE);


	if(m_inputformat.IsEmpty())
		{
		MessageBeep(0);
		GetDlgItem(IDC_INPUTFORMAT)->SetFocus();
		return;
		}
	else
		{
		CString sRev=m_inputformat.Left(2);
		nRev=atoi((LPCSTR)sRev);
		if(nRev!=10)
			{
			ExecString+="rev="+sRev+" ";
			}
		}

	if(m_inputfrom==2 || m_inputfrom==1)
		{
		ExecString=ExecString+"if=\x22"+cTempFileForDataFromCD+"\x22 ";
		}
	else
		{
		pWnd=GetDlgItem(IDC_DATASOURCE1);
		if(!pWnd->IsWindowEnabled())
			{
			MessageBox("There is no available tape device","Error",MB_ICONSTOP);
			return;
			}
		pWnd->GetWindowText(cSource, sizeof(cSource));

		if(cSource[0]==0) 
			{
			MessageBeep(0);
			pWnd->SetFocus();
			return;
			}
		else
			{
			if(m_inputfrom==0)
				{
				ExecString=ExecString+"if="+cSource+" ";
				}
			else
				{
				ExecString=ExecString+"if=\x22"+cSource+"\x22 ";
				}
			}
		}
	if(!m_workingdirectory.IsEmpty())
		{
		ExecString=ExecString+"outdir=\x22"+m_workingdirectory+"\\"+m_outputdirectory+"\x22 ";
		}

	if(m_unpackall>0 && (m_inputfrom!=2))
		{
		CString srng;
		srng.Format("range=%d,%d",m_record1,m_record2);
		ExecString+=srng;
		}
	if(nRev<10)
	if(m_swapbyte)
		{
		ExecString=ExecString+"+swab"+" ";
		}
	else
		{
		ExecString+=ExecString+"-swab"+" ";
		}
	if(nRev<10)
	if(m_writerawheader)
		{
		ExecString=ExecString+"+headers"+" ";
		}
	else
		{
		ExecString=ExecString+"-headers"+" ";
		}
	if(nRev<10)
	if(!m_dbdir.IsEmpty())
		{
		ExecString+="db=\x22"+m_dbdir+"\x22 ";
		}
	if(nRev>9 && m_timetagsummary)
		{
		ExecString=ExecString+"+ttag"+" ";      
		}

	if(m_stacode.IsEmpty()) 
		{
		MessageBeep(0);
		GetDlgItem(IDC_STACODE)->SetFocus();
		return;
		}
	else
		{
		ExecString=ExecString+"sta="+m_stacode+" ";
		}

	if((*cMapFile)&&(nRev<10))
		{
		ExecString=ExecString+"map="+cMapFile+" ";
		}

	if(nRev<10)
		{
		ExecString=ExecString+"-fixyear"+" ";      
		}


	if(nRev<10)
		{
		ExecString="idadmx.exe "+ExecString;
		}
	else
		{
		ExecString="i10dmx.exe "+ExecString;
		}

	SaveParam();

    DWORD WINAPI DMXThread(LPVOID lpV);
    DWORD dwId;

	HANDLE hThread=CreateThread(NULL,0,DMXThread, this,0,&dwId);
    SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

    CloseHandle(hThread);
	WorkingDlg=new CWorkingDlg(this);
	EnableWindow(false);
	}
BOOL CWinidadmxDlg::ProcessInputFromGZfile(LPCSTR str)
	{
	CStringList FilesList;
	FilesList.AddTail(str);
	return MakeTemporaryFile(FilesList);
	}

void CWinidadmxDlg::ViewResults(LPSTR str)
	{
	CString s(str);
	CViewResults Dlg(s,this);
	Dlg.DoModal();
	}

BOOL CWinidadmxDlg::CheckDirectory( LPCSTR lpDir)
	{
	if(CreateDir(lpDir)!=0)
		{
		CString sErr;
		sErr.Format("Can not create directory '%s'", lpDir);
		MessageBox(sErr,"WinIdaDmx error",MB_ICONSTOP);
		return FALSE;
		}
	return TRUE;
	}

void CWinidadmxDlg::OnBrowseoutputdatadir() 
	{
	LPITEMIDLIST _p;
    BROWSEINFO _browseInfo;
    char       _displayName [MAX_PATH];
    char       _fullPath [MAX_PATH];

	GetDlgItemText(IDC_WORKINGDIRECTORY,_fullPath, sizeof(_fullPath)-2);


	CheckDirectory(_fullPath);


    _displayName [0] = '\0';
    _browseInfo.hwndOwner = m_hWnd;
    _browseInfo.pidlRoot = NULL; 
    _browseInfo.pszDisplayName = _displayName;
    _browseInfo.lpszTitle = "Select working directory";
    _browseInfo.ulFlags = (BIF_RETURNONLYFSDIRS|0x0040); 
    _browseInfo.lpfn = 0;
    _browseInfo.lParam = (long)_fullPath;
    _browseInfo.iImage = 0;
	_browseInfo.lpfn = BrowseCallbackProc;
    // Let the user do the browsing
    _p = SHBrowseForFolder (&_browseInfo);
     if (_p != 0)
		{
        SHGetPathFromIDList (_p, _fullPath);
		SetDlgItemText(IDC_WORKINGDIRECTORY,_fullPath);
		}

	}


void CWinidadmxDlg::OnBrowseSubdir()
	{
    char       _fullPath [MAX_PATH];
	GetDlgItemText(IDC_WORKINGDIRECTORY,_fullPath, sizeof(_fullPath));

	if(!CheckDirectory(_fullPath))
		{
		return;
		}

	CBrowseSubdirDlg Dlg(_fullPath, this);
	Dlg.DoModal();

	}
void CWinidadmxDlg::OnBrowsechanmap() 
	{
	// TODO: Add your control notification handler code here
	LPITEMIDLIST _p;
    BROWSEINFO _browseInfo;
    char       _displayName [MAX_PATH];
    char       _fullPath [MAX_PATH];

	GetDlgItemText(IDC_DBDIR,_fullPath, sizeof(_fullPath)-2);

    _displayName [0] = '\0';
//    _fullPath [0] = '\0';
    _browseInfo.hwndOwner = m_hWnd;
    _browseInfo.pidlRoot = NULL; 
    _browseInfo.pszDisplayName = _displayName;
    _browseInfo.lpszTitle = "Select database directory";
    _browseInfo.ulFlags = BIF_RETURNONLYFSDIRS; 
    _browseInfo.lpfn = 0;
    _browseInfo.lParam = (long)_fullPath;
    _browseInfo.iImage = 0;
	_browseInfo.lpfn = BrowseCallbackProc;

    // Let the user do the browsing
    _p = SHBrowseForFolder (&_browseInfo);
     if (_p != 0)
		{
        SHGetPathFromIDList (_p, _fullPath);
		SetDlgItemText(IDC_DBDIR,_fullPath);
		LoadMapFileNames();
		SelectMap();
		}
	
	}

void CWinidadmxDlg::OnDoubleclickedBrowse() 
	{

	}
void CWinidadmxDlg::SaveParam()
	{
	CButton *pButton;
	CComboBox *pCombo;

	int nInputType, nItem;
	char cBuff[MAX_PATH+2];
	WritePrivateProfileString("PARAM","WORKING DIRECTORY",m_workingdirectory,pIniFileName);
	WritePrivateProfileString("PARAM","OUTPUT DIRECTORY",m_outputdirectory,pIniFileName);
	WritePrivateProfileString("PARAM","STATION",m_stacode,pIniFileName);
//	sprintf(cBuff,"%d",m_unpackall);
//	WritePrivateProfileString("PARAM","UNPUCK",cBuff,pIniFileName);

	pButton = (CButton *)GetDlgItem(IDC_INPUTFROMTAPE);
	if(pButton->GetCheck()==1)
		{
		nInputType = 0;
		}
	else
		{
		pButton = (CButton *)GetDlgItem(IDC_IDCINPUTFROMDISK);
		if(pButton->GetCheck()==1)
			{
			nInputType = 1;
			}
		else
			{
			nInputType = 2;
			}

		}


	sprintf(cBuff,"%d", nInputType);
	WritePrivateProfileString("PARAM","INPUTTYPE",cBuff,pIniFileName);

	if(nInputType==0)
		{
		GetDlgItemText(IDC_DATASOURCE1,cBuff,sizeof(cBuff));
		}
	if(nInputType==1)
		{
//		GetDlgItemText(IDC_DATASOURCE,cBuff,sizeof(cBuff));
		}
	if(nInputType==2)
		{
		pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
		nItem = pCombo->GetCurSel();
		pCombo->GetLBText(nItem, cBuff);
		}

	WritePrivateProfileString("PARAM","INPUT SOURCE NAME",cBuff,pIniFileName);
	GetDlgItemText(IDC_RECORD1,cBuff,sizeof(cBuff));
	WritePrivateProfileString("PARAM","RECORD1",cBuff,pIniFileName);
	GetDlgItemText(IDC_RECORD2,cBuff,sizeof(cBuff));
	WritePrivateProfileString("PARAM","RECORD2",cBuff,pIniFileName);
	WritePrivateProfileString("PARAM","DBDIR",m_dbdir,pIniFileName);
	sprintf(cBuff,"%d",m_writerawheader);
	WritePrivateProfileString("PARAM","WRITERAWHEADER",cBuff,pIniFileName);
	pCombo=(CComboBox *)GetDlgItem(IDC_INPUTFORMAT);
	int nFormat=pCombo->GetCurSel();
	sprintf(cBuff,"%d",nFormat);
	WritePrivateProfileString("PARAM","FORMAT",cBuff,pIniFileName);
	}
void CWinidadmxDlg::LoadParam()
	{
	int nInputType=0;
	char cBuff[MAX_PATH+2];
	CComboBox *pCombo;

	GetPrivateProfileString("PARAM","WORKING DIRECTORY","",cBuff,sizeof(cBuff),pIniFileName);
	if(*cBuff) m_workingdirectory=cBuff;
	GetPrivateProfileString("PARAM","OUTPUT DIRECTORY","",cBuff,sizeof(cBuff),pIniFileName);
	if(*cBuff) m_outputdirectory=cBuff;
	GetPrivateProfileString("PARAM","STATION","pfo",cBuff,sizeof(cBuff),pIniFileName);
	if(*cBuff) m_stacode=cBuff;
	m_unpackall=0;
	OnUnpackall();

	if(bStartFromCD)
		{
		m_inputfrom=2;
		}
	else
		{
		m_inputfrom=GetPrivateProfileInt("PARAM","INPUTTYPE",0,pIniFileName);
		}

	GetPrivateProfileString("PARAM","INPUT SOURCE NAME","",cBuff,sizeof(cBuff),pIniFileName);
	if(m_inputfrom<=0)
		{
		SetDlgItemText(IDC_DATASOURCE1,cBuff);
		}
	if(m_inputfrom==1)
		{
		SetDlgItemText(IDC_DATASOURCE,cBuff);
		}
	if(!bStartFromCD)
	if(m_inputfrom>=2)
		{
		m_inputfrom=2;
		pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
		int nItem = pCombo->FindStringExact(-1, cBuff);
		if(nItem!=-1)
			{
			pCombo->SetCurSel(nItem);
			}
		}

	m_record1=GetPrivateProfileInt("PARAM","RECORD1",1,pIniFileName);
	m_record2=GetPrivateProfileInt("PARAM","RECORD2",100,pIniFileName);

	GetPrivateProfileString("PARAM","DBDIR","",cBuff,sizeof(cBuff),pIniFileName);
	m_dbdir=cBuff;
	SetDlgItemText(IDC_DBDIR, cBuff);
	m_writerawheader=GetPrivateProfileInt("PARAM","WRITERAWHEADER",0,pIniFileName);
	int nFormat=GetPrivateProfileInt("PARAM","FORMAT",6,pIniFileName);
	pCombo=(CComboBox *)GetDlgItem(IDC_INPUTFORMAT);
	pCombo->SetCurSel(nFormat);
	}
void CWinidadmxDlg::OnCancel() 
	{
	SaveParam();
	CDialog::OnCancel();
	}

void CWinidadmxDlg::OnInputfromtape() 
	{
	m_inputfrom = 0;

//	GetDlgItem(IDC_DATASOURCE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DATASOURCE1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CDCOMBO)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_ADDRESSCOMBO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ENTERADDRESS)->ShowWindow(SW_HIDE);
	

	GetDlgItem(IDC_UNPACKRANGE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD2)->EnableWindow(TRUE);

	GetDlgItem(IDC_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_UNSELALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADDRESSCOMBO)->EnableWindow(FALSE);
	LoadTapesList();
	ClearCDInfo();
	}
void CWinidadmxDlg::OnInputfromCD() 
	{
	m_inputfrom = 2;

//	GetDlgItem(IDC_DATASOURCE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DATASOURCE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CDCOMBO)->ShowWindow(SW_SHOW);
//	GetDlgItem(IDC_DRIVECOMBO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADDRESSCOMBO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ENTERADDRESS)->ShowWindow(SW_HIDE);

	((CButton *)GetDlgItem(IDC_UNPACKALL))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_UNPACKRANGE))->SetCheck(0);


	GetDlgItem(IDC_UNPACKRANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD1)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD2)->EnableWindow(FALSE);

	GetDlgItem(IDC_LIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_UNSELALL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELALL)->EnableWindow(TRUE);

	LoadCDDeviceNames();
	LoadStaAndFormat();

//	LoadCDFileNames();
	SetDataDirFromCD();
	UpdateListView();

	LoadInfoFromDisk();

	}

void CWinidadmxDlg::OnIdcinputfromdisk() 
	{
	m_inputfrom = 1;

//	GetDlgItem(IDC_DATASOURCE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DATASOURCE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CDCOMBO)->ShowWindow(SW_HIDE);
//	GetDlgItem(IDC_DRIVECOMBO)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_ADDRESSCOMBO)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_ENTERADDRESS)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_UNPACKRANGE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD2)->EnableWindow(TRUE);
	GetDlgItem(IDC_ADDRESSCOMBO)->EnableWindow(TRUE);

	GetDlgItem(IDC_LIST)->EnableWindow(true);
	GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_UNSELALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELALL)->EnableWindow(FALSE);

	ClearCDInfo();
	LoadDriveNames();
	UpdateListView();

	}



void CWinidadmxDlg::OnExitIDALST()
	{
	WorkingDlg->Stop();
	delete WorkingDlg;
	EnableWindow();
	if(m_inputfrom==2||m_inputfrom==1)
		{
		DeleteFile(cTempFileForDataFromCD);
		}
//	ViewResults(cTmpBuffer);

	CViewLstDlg Dlg(cTmpBuffer,this);
	Dlg.DoModal();


	}


void CWinidadmxDlg::OnExitIDADMX()
	{
	WorkingDlg->Stop();
	delete WorkingDlg;
	EnableWindow();
	if(m_inputfrom==2||m_inputfrom==1)
		{
		unlink(cTempFileForDataFromCD);
		}
	ViewResults(cTmpBuffer);
	}
void CWinidadmxDlg::LoadTapesList()
	{

	if(!InitASPI())
		{
		m_inputcombo.ResetContent();
		m_inputcombo.AddString("No available tape device");
		m_inputcombo.SetCurSel(0);
		m_inputcombo.EnableWindow(FALSE);
		return;
		}
	m_inputcombo.EnableWindow(TRUE);
	m_inputcombo.ResetContent();

	DevList *dv=ScanSCSI(m_hWnd), *pNext;
	if(dv==NULL)
		{
		m_inputcombo.AddString("No available tape device");
		m_inputcombo.SetCurSel(0);
		m_inputcombo.EnableWindow(FALSE);
		return;
		}
	else
		{
		while(true)
			{
			char cBuff[120];
			pNext=(DevList *)dv->pNext;
			sprintf(cBuff,"/dev/%d:%d:%d ", dv->adapter, dv->target, dv->lun); 
			m_inputcombo.AddString(cBuff);
			delete dv;
			dv=pNext;
			if(pNext==NULL) break;
			}
		}
	m_inputcombo.SetCurSel(0);
	}

DWORD WINAPI DMXThread(LPVOID lpV)
	{
	PROCESS_INFORMATION pi;
	STARTUPINFO stinfo;

	BOOL bRes;
	CWinidadmxDlg *pDlg=(CWinidadmxDlg *)lpV;
    HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
    HANDLE hInputWriteTmp,hInputRead,hInputWrite;
    HANDLE hErrorWrite;
	SECURITY_ATTRIBUTES sa;

    sa.nLength= sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

      // Create the child output pipe.
    bRes=CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0);


    bRes=CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0);



    bRes=DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                           GetCurrentProcess(),
                           &hOutputRead, // Address of new handle.
                           0,FALSE, // Make it uninheritable.
                           DUPLICATE_SAME_ACCESS);

    bRes=DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
                           GetCurrentProcess(),
                           &hInputWrite, // Address of new handle.
                           0,FALSE, // Make it uninheritable.
                           DUPLICATE_SAME_ACCESS);
      
	DuplicateHandle(GetCurrentProcess(),hOutputWrite,
                           GetCurrentProcess(),&hErrorWrite,0,
                           TRUE,DUPLICATE_SAME_ACCESS);

	CloseHandle(hOutputReadTmp);
	CloseHandle(hInputWriteTmp); 

	memset(&stinfo,0,sizeof(stinfo));

	stinfo.dwFlags=STARTF_USECOUNTCHARS|STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	stinfo.hStdOutput=hOutputWrite;
	stinfo.hStdError=hErrorWrite;
//	stinfo.wShowWindow = SW_HIDE;

	stinfo.dwXCountChars=80;
	stinfo.dwYCountChars=25; 

	

	stinfo.cb=sizeof(stinfo);
	memset(&pi,0,sizeof(pi));

	BOOL bStart=CreateProcess(NULL,
    (LPSTR)(LPCSTR)pDlg->ExecString,  // pointer to command line string
    NULL,  // process security attributes
    NULL,   // thread security attributes
    TRUE,  // handle inheritance flag
	CREATE_NEW_CONSOLE , // creation flags
    NULL,  // pointer to new environment block
    NULL,   // pointer to current directory name
  &stinfo,  // pointer to STARTUPINFO
  &pi  // pointer to PROCESS_INFORMATION
);
	*cTmpBuffer=0;
	if(bStart)
		{

//		Dlg.DoModal();
		WaitForSingleObject(pi.hProcess,60000);
		DWORD dwBytes;
		memset(&cTmpBuffer,0,sizeof(cTmpBuffer));
		bRes=ReadFile(hErrorWrite,cTmpBuffer,sizeof(cTmpBuffer),&dwBytes,NULL);
		bRes=ReadFile(hOutputRead,cTmpBuffer,sizeof(cTmpBuffer),&dwBytes,NULL);

		CWinidadmxDlg::ParserForOutputFileName();

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(hInputRead);
		CloseHandle(hInputWrite);
		CloseHandle(hOutputRead);
   		CloseHandle(hOutputWrite);
   		CloseHandle(hErrorWrite);
//		Dlg.Stop();
		}
	SendMessage(pDlg->m_hWnd, WM_COMMAND, IDC_EXITIDADMX,0);

	return 0;
	}

DWORD WINAPI LSTThread(LPVOID lpV)
	{
	PROCESS_INFORMATION pi;
	STARTUPINFO stinfo;

	BOOL bRes;
	CWinidadmxDlg *pDlg=(CWinidadmxDlg *)lpV;
    HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
    HANDLE hInputWriteTmp,hInputRead,hInputWrite;
    HANDLE hErrorWrite;
	SECURITY_ATTRIBUTES sa;


	char cTempDir[4096];
	char cTempFileName[4096];

	GetTempPath(sizeof(cTempDir),cTempDir);
	GetTempFileName(cTempDir,"dmx",0,cTempFileName);

	HANDLE hFtemp=CreateFile(cTempFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);






    sa.nLength= sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

      // Create the child output pipe.
//    bRes=CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0);


//    bRes=CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0);



	bRes=DuplicateHandle(GetCurrentProcess(),hFtemp,
                           GetCurrentProcess(),
                           &hOutputWrite, // Address of new handle.
                           0,TRUE, 
                           DUPLICATE_SAME_ACCESS);

/*    bRes=DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
                           GetCurrentProcess(),
                           &hOutputRead, // Address of new handle.
                           0,FALSE, // Make it uninheritable.
                           DUPLICATE_SAME_ACCESS);

    bRes=DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
                           GetCurrentProcess(),
                           &hInputWrite, // Address of new handle.
                           0,FALSE, // Make it uninheritable.
                           DUPLICATE_SAME_ACCESS);*/
      

//	CloseHandle(hOutputReadTmp);
//	CloseHandle(hInputWriteTmp); 

	memset(&stinfo,0,sizeof(stinfo));

	stinfo.dwFlags=STARTF_USECOUNTCHARS|STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	stinfo.hStdOutput= hOutputWrite;
	stinfo.hStdError = hOutputWrite;
	stinfo.wShowWindow = SW_HIDE;

	stinfo.dwXCountChars=80;
	stinfo.dwYCountChars=25; 

	

	stinfo.cb=sizeof(stinfo);
	memset(&pi,0,sizeof(pi));

	BOOL bStart=CreateProcess(NULL,
    (LPSTR)(LPCSTR)pDlg->ExecString,  // pointer to command line string
    NULL,  // process security attributes
    NULL,   // thread security attributes
    TRUE,  // handle inheritance flag
	CREATE_NEW_CONSOLE , // creation flags
    NULL,  // pointer to new environment block
    NULL,   // pointer to current directory name
  &stinfo,  // pointer to STARTUPINFO
  &pi  // pointer to PROCESS_INFORMATION
);
	*cTmpBuffer=0;
	if(bStart)
		{

//		Dlg.DoModal();
		memset(&cTmpBuffer,0,sizeof(cTmpBuffer));
		WaitForSingleObject(pi.hProcess,600000);
		DWORD dwBytes;

		CloseHandle(hOutputWrite);
		SetFilePointer(hFtemp, 0,0, FILE_BEGIN); 

		bRes=ReadFile(hFtemp,cTmpBuffer,sizeof(cTmpBuffer)-1,&dwBytes,NULL);
		CloseHandle(hFtemp);
		DeleteFile(cTempFileName);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
//		CloseHandle(hInputRead);
//		CloseHandle(hInputWrite);
//		CloseHandle(hOutputRead);
//   		CloseHandle(hErrorWrite);
//		Dlg.Stop();
		}
	SendMessage(pDlg->m_hWnd, WM_COMMAND, IDC_EXITIDALST,0);

	return 0;
	}





void CWinidadmxDlg::OnUnpackall() 
	{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_RECORD1)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD2)->EnableWindow(FALSE);
	}

void CWinidadmxDlg::OnUnpackrange() 
	{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_RECORD1)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD2)->EnableWindow(TRUE);
	}
void CWinidadmxDlg::ParserForOutputFileName()
	{
	static char *cpPattern="Output file name:" ;
	char *cp=strstr(cTmpBuffer, cpPattern);
	*cOutputFileName=0;
	int i=0;
	if(cp!=NULL)
		{
		for(i=0; i<sizeof(cOutputFileName); ++i, ++cp)
			{
			if(*cp==0 || *cp==0xd || *cp==0xa)
				{
				cOutputFileName[i]=0;
				}
			else
				{
				cOutputFileName[i]=*cp;
				}
			}
		}
	}
bool CWinidadmxDlg::CheckDir(LPCSTR lpDir)
	{
	char cCurDir[MAX_PATH];
	DWORD dw=GetCurrentDirectory(sizeof(cCurDir),cCurDir);
	if(SetCurrentDirectory(lpDir)!=0)
		{
		SetCurrentDirectory(cCurDir);
		return true;
		}
	SetCurrentDirectory(cCurDir);
	return false;
	}
BOOL CAboutDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	m_essw.SetLink(TRUE)
		.SetTextColor(RGB(0,0,255))
		.SetFontUnderline(TRUE)
		.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_HAND));


	CString s;
	s.Format(IDS_STRING1042,(LPCSTR)GetFileVersion(), __DATE__,__TIME__);

	SetDlgItemText(IDC_REALVERSIONINFO, (LPCSTR)s);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CWinidadmxDlg::OnAbout() 
	{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
	}

void CWinidadmxDlg::OnSelchangeInputformat() 
	{
	// TODO: Add your control notification handler code here
	char cRelease[128];
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_INPUTFORMAT);
	int nFormat=pCombo->GetCurSel();

	pCombo->GetLBText( nFormat, cRelease);
	if(strncmp(cRelease,"10",2)==0)
		{
		GetDlgItem(IDC_DBDIR)->EnableWindow(true);
		GetDlgItem(IDC_WRITERAWHEADER)->EnableWindow(false);
		GetDlgItem(IDC_TIMETAGSUMMARY)->EnableWindow(true);
		GetDlgItem(IDC_BYTESWAPINTOHOSTORDER)->EnableWindow(false);
		GetDlgItem(IDC_BROWSECHANMAP)->EnableWindow(true);
		GetDlgItem(IDC_MAPFILENAME)->EnableWindow(false);
		}
	else
		{
		GetDlgItem(IDC_DBDIR)->EnableWindow(true);
		GetDlgItem(IDC_WRITERAWHEADER)->EnableWindow(true);
		GetDlgItem(IDC_TIMETAGSUMMARY)->EnableWindow(false);
		GetDlgItem(IDC_BYTESWAPINTOHOSTORDER)->EnableWindow(true);
		GetDlgItem(IDC_BROWSECHANMAP)->EnableWindow(true);
		GetDlgItem(IDC_MAPFILENAME)->EnableWindow(true);
		}
	}
void CWinidadmxDlg::LoadCDDeviceNames()
	{
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
    LPSTR drive_names;
    LPSTR p;
    DWORD  size;
    UINT   type;
	BOOL bCDavailable=FALSE;

	pCombo->ResetContent();

	if(bStartFromCD)
		{
		pCombo->AddString(csCD);
		bCDavailable = TRUE;
		pCombo->SetCurSel(0);
		return;
		}

    size = GetLogicalDriveStrings(0, NULL);
    drive_names = (LPTSTR)malloc(sizeof(char) * (size + 1));
    p = drive_names;

    GetLogicalDriveStrings(size, drive_names);
    while(*p){
		type = GetDriveType(p);
		if(type==DRIVE_CDROM )
			{
			pCombo->AddString(p);
			bCDavailable = TRUE;
			}
	while(*p)p++;
        p++;
    
	}

	free(drive_names);
	if(bCDavailable)
		{
		pCombo->SetCurSel(0);
		}
	else
		{
		pCombo->EnableWindow(FALSE);
		}
	}


void CWinidadmxDlg::LoadDriveNames()
	{
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_ADDRESSCOMBO);
    LPSTR drive_names;
    LPSTR p;
    DWORD  size;
    UINT   type;
	BOOL bCDavailable=FALSE;


	pCombo->ResetContent();

    size = GetLogicalDriveStrings(0, NULL);
    drive_names = (LPTSTR)malloc(sizeof(char) * (size + 1));
    p = drive_names;

    GetLogicalDriveStrings(size, drive_names);
    while(*p){
//		type = GetDriveType(p);
//		if(type==DRIVE_CDROM )
//			{
			pCombo->AddString(p);
			bCDavailable = TRUE;
//			}
	while(*p)p++;
        p++;
    
	}

	free(drive_names);
	if(bCDavailable)
		{
//		pCombo->SetCurSel(0);
		}
	else
		{
		pCombo->EnableWindow(FALSE);
		}
	}


int CWinidadmxDlg::ProcessInputFromList()
	{
	CStringList FilesList;

	if(MakeFilesList(FilesList)<=0)
		{
		CString s;
		s.LoadString(IDS_STRING1038);
		MessageBox(s,"Error", MB_ICONSTOP);
		return 1;
		}

	return MakeTemporaryFile(FilesList);
	}
void CWinidadmxDlg::SelectRelease(int nRelease)
	{
	CComboBox *pCombo;
	int nItem;
	char buff[8];

	if(nRelease>0)
		{
		sprintf(buff,"%d", nRelease);
		pCombo=(CComboBox *)GetDlgItem(IDC_INPUTFORMAT);
		nItem = pCombo->FindString(-1, buff);
		if(nItem>0)
			{
			pCombo->SetCurSel(nItem); 
			OnSelchangeInputformat();
			}
		}
	}
int CWinidadmxDlg::MakeTemporaryFile(const CStringList & FileList)
	{
	CWaitCursor cw;
	POSITION pos;

// Make a temporary file name
	char cTempDir[4096];
	char cTempFileName[4096];

	GetTempPath(sizeof(cTempDir),cTempDir);
	GetTempFileName(cTempDir,"dmx",0,cTempFileName);

	HANDLE hF=CreateFile(cTempFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

	if(INVALID_HANDLE_VALUE==hF)
		{
		CString sErr;
		sErr.LoadString(IDS_STRING1039);
		MessageBox(sErr,"",MB_ICONSTOP);
		return 1;
		}

	cTempFileForDataFromCD = cTempFileName;

	for( pos = FileList.GetHeadPosition(); pos != NULL; )
		{
		CString s = FileList.GetNext( pos );
		if(UnpackFileToTemp(hF, s)!=0)
			{
//			CloseHandle(hF);
//			return 1;
			}
		}
	CloseHandle(hF);
	return 0;
	}
int CWinidadmxDlg::UnpackFileToTemp(HANDLE hF, const CString &s)
	{
	DWORD dwNumberOfBytesWritten;
	long nRead;
	char DataBuff[1024*256];
	gzFile file;

	file = gzopen(s,"rb");

	if(file==NULL) 
		{
		CString sErr;
		sErr.Format(IDS_STRING1041, s); 
		MessageBox(sErr,"",MB_ICONSTOP);
		return 1;
		}

	while(1)
		{
		nRead = gzread(file, DataBuff, sizeof(DataBuff));
		if(nRead == 0)
			{
			gzclose(file);
			return 0;
			}
		if(nRead <= -1)
			{
			gzclose(file);

			CString sErr;
			sErr.Format(IDS_STRING1041, s); 
			MessageBox(sErr,"",MB_ICONSTOP);
			return 1;
			}
		if(nRead%1024!=0)
			{
			nRead = (nRead/1024)*1024;
			}
		if(!WriteFile(hF,DataBuff, nRead, &dwNumberOfBytesWritten, NULL))
			{
			gzclose(file);

			CString sErr;
			sErr.LoadString(IDS_STRING1040);
			MessageBox(sErr,"",MB_ICONSTOP);
			return 1;
			}
		}
	}

void CWinidadmxDlg::LoadStaAndFormat()
	{
	long lFLength;
	int nHandle;
	char cFileName[MAX_PATH]= {0}, cSta[32]={0}, *Buff;
	int nRelease, nItem;
	CComboBox *pCombo;
	CString cDisk, TemporaryFileName;
	pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return;
	pCombo->GetLBText(nItem, cDisk);


	strcpy(cFileName, cDisk);
	strcat(cFileName,"CONTENTS.TXT");

	nHandle = open(cFileName, O_BINARY|O_RDONLY);
	lFLength = _filelength(nHandle);
	Buff = new char[lFLength+1];
	if(read(nHandle, Buff, lFLength)>0)
		{
		Buff[lFLength-1] = 0;
		sscanf(Buff, "%s %d", cSta, &nRelease);
		if(*cSta)
			{
			GetDlgItem(IDC_STACODE)->SetWindowText(cSta);
//			SelectMap(cSta, nRelease);
			SelectMap();
			}
		SelectRelease(nRelease);
		}

	close(nHandle);
	delete Buff;
	}
void CWinidadmxDlg::SelectMap()
	{
	char cSta[32];
	GetDlgItem(IDC_STACODE)->GetWindowText(cSta, sizeof(cSta)-2);
	CString sMap;
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	char cMapFile[64];
	if(!*cSta) return;

	GetPrivateProfileString("MAP FILES",cSta,"",cMapFile,sizeof(cMapFile),pIniFileName);
	if(*cMapFile)
		{
		if(bSelectMapInCombobox(cMapFile)) return;
		}
	if(bSelectMapInCombobox(cSta)) return;		
	if(SelectMapFromAssignments(cSta)) return;
	}
BOOL CWinidadmxDlg::SelectMapFromAssignments(LPCSTR lpSta)
	{
	FILE *f;
	HANDLE hFF;
	WIN32_FIND_DATA ff;
	int i,j;
	char buff[256],sta[8], map[256];
    CString       fullPath, sName;
	GetDlgItemText(IDC_DBDIR,fullPath);

	fullPath.TrimLeft();
	fullPath.TrimRight();
	if((fullPath.Right(1).Compare("\\"))!=0)
		{
		fullPath+="\\";
		}
	fullPath+="etc\\maps\\Assignments";
	f=fopen(fullPath,"r");
	if(f==NULL) return FALSE;
	while(fgets(buff,sizeof(buff)-2,f)!=NULL)
		{
		if(*buff=='#') continue;
		if(strlen(buff)<7) continue;
		for(i=0; i<strlen(buff); ++i) if( buff[i]==0xd || buff[i]==0xa || buff[i]==0x9) buff[i]=' ';
		sscanf(buff,"%s %s",sta,map);
		if(*sta==0) continue;
		if(*map==0) continue;
		if(stricmp(sta, lpSta)==0)
			{
			fclose(f);
			if(bSelectMapInCombobox(map))
				{
				return TRUE;
				}
			else
				{
				return FALSE;
				}
			}
		}
	fclose(f);
	return FALSE;
	}


BOOL CWinidadmxDlg::bSelectMapInCombobox(LPCSTR lpName)
	{
	CString sMap;
	int i;
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	int nItems = pCombo->GetCount();

	for(i=0; i<nItems; ++i)
		{
		pCombo->GetLBText(i, sMap);
		if(sMap.CompareNoCase(lpName)==0)
			{
			pCombo->SetCurSel(i);
			return TRUE;
			}
		}
	return FALSE;
	}

void CWinidadmxDlg::SelectMap(LPCSTR lpSta, int nRelease)
	{
	int i;
	CString sMap;
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	char cMapFile[64];
	GetPrivateProfileString("MAP FILES",lpSta,"",cMapFile,sizeof(cMapFile),pIniFileName);
	if(*cMapFile)
		{
		int nItems = pCombo->GetCount();
		for(i=0; i<nItems; ++i)
			{
			pCombo->GetLBText(i, sMap);
			if(sMap.Compare(cMapFile)==0)
				{
				pCombo->SetCurSel(i);
				break;
				}
			}

		if((i==nItems)&&(nRelease>-1))
			{
			char s[32];
			sprintf(s,"Std.%02d", nRelease);
			i=pCombo->AddString(s);
			if(i>=0) pCombo->SetCurSel(i);
			}


		}
	}

char FirstDriveFromMask (ULONG unitmask)
   {
      char i;

      for (i = 0; i < 26; ++i)
      {
         if (unitmask & 0x1)
            break;
         unitmask = unitmask >> 1;
      }

      return (i + 'A');
   }

BOOL CWinidadmxDlg::OnDeviceChange( UINT nEventType, DWORD dwData )
	{
	char szMsg[80];
	char cDisk[8];

	if(m_inputfrom!=2) return TRUE;

	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	int nItem = pCombo->GetCurSel();
	if(nItem<0) return TRUE;
	pCombo->GetLBText(nItem, cDisk);



	if(nEventType == DBT_DEVICEARRIVAL)
		{
		DEV_BROADCAST_HDR *d = (DEV_BROADCAST_HDR *)dwData;

		if(((CButton *)GetDlgItem(IDC_IDCINPUTFROMCD))->GetCheck()!=1) return TRUE;

         if (d->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
            PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)d;

            if (lpdbv -> dbcv_flags & DBTF_MEDIA)
				{
				if(cDisk[0]==FirstDriveFromMask(lpdbv ->dbcv_unitmask))
					{
					wsprintf (szMsg, "Drive %c: arrived\n",
                         FirstDriveFromMask(lpdbv ->dbcv_unitmask));

//					MessageBox (szMsg, "WM_DEVICECHANGE", MB_OK);
//					LoadCDFileNames();
					SetDataDirFromCD();
					UpdateListView();
					LoadStaAndFormat();
					LoadInfoFromDisk();
					}
				}
			}
        


		}
	if(nEventType == DBT_DEVICEREMOVECOMPLETE)
		{
        // See if a CD-ROM was removed from a drive.
		DEV_BROADCAST_HDR *d = (DEV_BROADCAST_HDR *)dwData;
        if (d -> dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
			PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)d;

			if (lpdbv -> dbcv_flags & DBTF_MEDIA)
				{
				CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
				pList->DeleteAllItems();
				ClearCDFileArray();
				GetDlgItem(IDC_EDIT)->SetWindowText("");

//				CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
//				int nItem = pCombo->GetCurSel();
//				if(nItem<0) return TRUE;
//				pCombo->GetLBText(nItem, cDisk);
				if(cDisk[0]==FirstDriveFromMask(lpdbv ->dbcv_unitmask))
					{
					if(bStartFromCD) EndDialog(0);
					}
				}
			}
		}
	return TRUE;
	}


void CWinidadmxDlg::SetListHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = {"File","Size"};
	RECT rc;
	CListCtrl	*pList;

	pList = (CListCtrl	*)(GetDlgItem(IDC_LIST));

	pList->GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right/2;
	lvc.cchTextMax=128;
	for(i=0;i<sizeof(colums)/sizeof(char *);++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		pList->InsertColumn(i,&lvc);
		}
			
	}	
void CWinidadmxDlg::LoadMapFileNames()
	{
	CString bdDir;
	CComboBox *pCombo;
	pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	long hFile;
	char cMask[MAX_PATH], buff[64];
	struct _finddata_t all_files;


	pCombo->ResetContent( );


	GetDlgItemText(IDC_DBDIR, cMask, sizeof(cMask)-7);
	if(cMask[strlen(cMask)-1]!='\\') strcat(cMask, "\\");
	strcat(cMask,"etc\\maps\\*.*");

	if( (hFile = _findfirst( cMask, &all_files )) == -1L ) return ;

	do 
        {
		if(stricmp("Assignments",all_files.name)==0) continue;

        if(all_files.name[0] != '.')
			{
			pCombo->AddString(all_files.name);
			}
		}
	while( _findnext( hFile, &all_files ) == 0 );

	_findclose(hFile);



	}
void CWinidadmxDlg::ClearCDFileArray()
	{
	CDFile *cdf;
	delete [] ArrayIndex; ArrayIndex = NULL;
	while(CDFileArray.GetSize()>0)
		{
		cdf = CDFileArray[0];
		delete cdf;
		CDFileArray.RemoveAt(0);
		}
	HashTable.RemoveAll();
	}
void CWinidadmxDlg::SetDataDirFromCD()
	{
	CString cDisk;
	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	int nItem = pCombo->GetCurSel();
	if(nItem<0) return;
	pCombo->GetLBText(nItem, cDisk);
	m_dataDirectory = cDisk;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);
	}
void CWinidadmxDlg::LoadCDFileNames()
	{
	int nItem;
	LV_ITEM lvi;
	long hFile, nIndex=0;
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
	char cMask[MAX_PATH], buff[64];
	CComboBox *pCombo;
	CDFile *cdf;

	CString cDisk, TemporaryFileName;
	pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return;
	pCombo->GetLBText(nItem, cDisk);


	strcpy(cMask, (LPCSTR)cDisk);
	strcat(cMask,"*.gz");

	struct _finddata_t all_files;


	pList->DeleteAllItems();
	ClearCDFileArray();

	if( (hFile = _findfirst( cMask, &all_files )) == -1L ) 
		if( (hFile = _findfirst( cMask, &all_files )) == -1L ) return ;

	do 
        {
        if(all_files.name[0] != '.')
			{
			memset(&lvi,0,sizeof(lvi));
			lvi.iItem=nIndex;
			lvi.iSubItem=0;
			lvi.mask=LVIF_TEXT;
			lvi.iImage=0;
			lvi.cchTextMax=128;
			lvi.pszText=all_files.name;
			pList->InsertItem(&lvi);
			sprintf(buff,"%ld", all_files.size);
//			pList->SetItemText(nIndex,1,buff);

			cdf = new CDFile;
			cdf->dwSize = all_files.size;
			cdf->tbeg= 0;
			cdf->tend= 0;
			lstrcpy(cdf->fname, all_files.name);

			CDFileArray.Add(cdf);


			++nIndex;
			}
		}
	while( _findnext( hFile, &all_files ) == 0 );

	_findclose(hFile);
	}
void CWinidadmxDlg::OnSelall() 
	{
	m_showfiletime=false;
	SelUnselFiles(TRUE);
	m_showfiletime=true;
	FillSizeInfo();
	}

void CWinidadmxDlg::OnUnselall() 
	{
	m_showfiletime=false;
	SelUnselFiles(FALSE);
	m_showfiletime=true;
	FillSizeInfo();
	}
void CWinidadmxDlg::SelUnselFiles(BOOL bSel)
	{
	int state;
	int nItems;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_LIST));

	nItems = pList->GetItemCount();

	if(bSel)
		state=LVIS_SELECTED;
	else
		state=0;
	pList->SetItemState(-1, state, LVIS_SELECTED );


	}
void CWinidadmxDlg::LoadInfoFromDisk()
	{
	int i=0, nItem;
	long lFLength;
	int nHandle;
	char cFileName[MAX_PATH], cDisk[8];
	char *Buff;
	CComboBox *pCombo;

	pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return;
	pCombo->GetLBText(nItem, cDisk);
	

	strcpy(cFileName, cDisk);
	strcat(cFileName,"README.TXT");

	nHandle = open(cFileName, O_BINARY|O_RDONLY);
	lFLength = _filelength(nHandle);
	Buff = new char[lFLength+1];
	if(read(nHandle, Buff, lFLength)>0)
		{
		Buff[lFLength-1] = 0;
		GetDlgItem(IDC_EDIT)->SetWindowText(Buff);
		}
	delete Buff;
	close(nHandle);
	}

void CWinidadmxDlg::ClearCDInfo()
	{
	GetDlgItem(IDC_EDIT)->SetWindowText("");
	((CListCtrl *)GetDlgItem(IDC_LIST))->DeleteAllItems();
	}
int CWinidadmxDlg::MakeFilesList(CStringList &FilesList)
	{
	CString sDir = m_dataDirectory;
	CString s, sfile;;
	int nItem;
	CListCtrl	*pList;
	CComboBox *pCombo;

//	pCombo=(CComboBox *)GetDlgItem(IDC_ADDRESSCOMBO);
//	nItem = pCombo->GetCurSel();
//	if(nItem<0) return 0;
//	pCombo->GetLBText(nItem, cDisk);

	sDir.TrimRight();
	sDir.TrimLeft();
	if(sDir.Right(1)!="\\") sDir+="\\";


	pList = (CListCtrl	*)(GetDlgItem(IDC_LIST));
	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
		{
		return 0;
		}

	while (pos)
		{
		nItem = pList->GetNextSelectedItem(pos);
		s = pList->GetItemText(nItem,0);
		sfile = sDir + s;
		FilesList.AddTail((LPCSTR)sfile);
		}

	return FilesList.GetCount();
	}
/*void CWinidadmxDlg::ShowHideFileList()
	{
	RECT rc;
	GetClientRect(&rc);
	}*/

BOOL CWinidadmxDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
	char cbuff[256];
	CString s, sfile;

	NM_LISTVIEW *pNm = (NM_LISTVIEW *)lParam;
	NMHDR		*pnmhdr=(LPNMHDR )lParam;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItemIndx= pItem->iItem;
	LPNMLVODSTATECHANGE lpStateChange = (LPNMLVODSTATECHANGE) lParam;


	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_LIST));


	switch(pnmhdr->code)
		{
		case LVN_GETDISPINFO:
			if (pItem->mask & LVIF_TEXT) //valid text buffer?
				{
				switch(pItem->iSubItem){
					case 0: 
						lstrcpy(pItem->pszText, CDFileArray[ArrayIndex[iItemIndx]]->fname);
						break;
					case 1: 
						if(CDFileArray[ArrayIndex[iItemIndx]]->nType==0)
							sprintf(pItem->pszText, "%ld", CDFileArray[ArrayIndex[iItemIndx]]->dwSize);
						else
							sprintf(pItem->pszText, "<DIR>");
						break;
				}
			}
			if (pItem->mask & LVIF_IMAGE ) //valid text buffer?
				{
				pItem->iImage=CDFileArray[ArrayIndex[iItemIndx]]->nType;
				}
			if (pItem->mask & LVIF_PARAM ) //valid text buffer?
				{
				pItem->lParam=2;
				}

			break;
		case LVN_ODSTATECHANGED:
			if(m_showfiletime)FillSizeInfo();
			return TRUE;
			break;

		case LVN_ITEMCHANGED:
			{
			int nItem=pNm->iItem;


			if ((pNm->uChanged & LVIF_STATE) &&
				((pNm->uNewState & LVIS_SELECTED) &&
				!(pNm->uOldState & LVIS_SELECTED)) || ( !(pNm->uNewState & LVIS_SELECTED) && (pNm->uOldState & LVIS_SELECTED)))  
				{
/*				s = pList->GetItemText(nItem,0);

				DWORD WINAPI OutputFileTimeThread(LPVOID lpV);

				DWORD dwId;

				OutputFileTimeThreadParam *param=new OutputFileTimeThreadParam;

				param->pDlg   = this;
				param->sFile  = s;
				param->nIndex = nItem;

				HANDLE hThread=CreateThread(NULL,0,OutputFileTimeThread, param,0,&dwId);
				if(hThread!=NULL)
					{
					SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
					CloseHandle(hThread);
					}
				else
					{
					delete param;
					}
*/
				POSITION pos = pList->GetFirstSelectedItemPosition();
				if (pos != NULL)
					{
					pList->GetNextSelectedItem(pos);
					if(pos==NULL)
						{
						if(m_showfiletime) FillSizeInfo();
						}
					}

//				if(m_showfiletime)FillSizeInfo();


				}
			break;
			}

		}
	return CDialog::OnNotify(wParam, lParam, pResult);
	}


int rdrec10(gzFile fp, IDA10_TS *ts, struct counter *count);

struct counter {
    UINT32 rec;       /* number of records which have been read  */
    UINT32 stream[IDA10_MAXSTREAMS]; /* per stream count        */
};


DWORD WINAPI OutputFileTimeThread(LPVOID lpV)
	{
	long nRead;
//	struct data_rec dr;
	double tmin=0, tmax=0;
	char cTBuff1[64], cTBuff2[64], cTime[100];
	int verbose, nIndex ;
	struct counter count;
	IDA10_TS ts;
	IDA_DHDR dhdr;

	OutputFileTimeThreadParam *param = (OutputFileTimeThreadParam *)lpV;
	CString s = param->sFile;
	CString sfile;
	CWinidadmxDlg *pDlg = param->pDlg;
	char DataBuff[1024];
	char buffer[2 * IDA10_MAXRECLEN];
	nIndex = param->nIndex;

	delete param;


	EnterCriticalSection(&critFS); 
	

	char cDisk[8];
	int nItem;


	CDFile *cdf=CDFileArray[nIndex];

	tmin = cdf->tbeg;
	tmax = cdf->tend;

	if(tmin==0 || tmax==0)
		{


	CComboBox *pCombo;

	pCombo=(CComboBox *)pDlg->GetDlgItem(IDC_ADDRESSCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return 0;
	pCombo->GetLBText(nItem, cDisk);

	char cRelease[128];
	pCombo=(CComboBox *)pDlg->GetDlgItem(IDC_INPUTFORMAT);
	int nFormat=pCombo->GetCurSel();

	pCombo->GetLBText( nFormat, cRelease);


	sfile = cDisk + s;

	gzFile file;

	file = gzopen(sfile,"rb");

	if(file==NULL) 
		{
		LeaveCriticalSection(&critFS);
		return 1;
		}

	if(strncmp(cRelease,"10",2)==0)
		{
		while (rdrec10(file, &ts, &count))
			{
			if(tmin==0.) tmin = ts.hdr.tofs;
			tmax = ts.hdr.tols;
			}
		}
	else
		{
		while(1)
			{
			nRead = gzread(file, DataBuff, sizeof(DataBuff));
			if(nRead == 0)
				{
				gzclose(file);
				break;
				}
			if(nRead <= -1)
				{
				gzclose(file);
				break;
				}
//			dbdrec_rev7(NULL, &dr, DataBuff, 1);

			dhead_rev7(NULL, &dhdr, (UINT8 *)DataBuff);

			if(tmin==0.) tmin = dhdr.beg.tru;
			nRead = (nRead/1024-1)*1024;
//			dbdrec_rev7(NULL, &dr, &DataBuff[nRead], 1);
			dhead_rev7(NULL, &dhdr, (UINT8 *)&DataBuff[nRead]);
			tmax = dhdr.beg.tru;

			}
		CDFileArray[nIndex]->tbeg=tmin;
		CDFileArray[nIndex]->tend=tmax;

		}
	}
	utilLttostr((long)tmin, 0, cTBuff1);
	utilLttostr((long)tmax, 0, cTBuff2);

	strcpy(cTime, cTBuff1);
	strcat(cTime, " ");
	strcat(cTime, cTBuff2);
	pDlg->SetDlgItemText(IDC_CDFILETIME, cTime);

	LeaveCriticalSection(&critFS);
	return 0;
	}



static int ReadIspLogRec(gzFile fp, UINT8 *buf, int buflen)
{
int ReadLen;

    if (buflen < IDA10_ISPLOGLEN) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }
    ReadLen = IDA10_ISPLOGLEN - IDA10_PREAMBLE_LEN;
    if (gzread(fp, buf, ReadLen) != ReadLen) return IDA10_IOERR;
    return IDA10_OK;
}

static int ida10ReadRecord(gzFile fp, UINT8 *buffer, int buflen, int *pType)
{
UINT8 *ptr;
IDA10_CMNHDR cmn;
int ReadLen, CmnHdrLen;

    if (buffer == NULL || pType == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    if (buflen < IDA10_PREAMBLE_LEN) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

/* Read the preamble and determine the type */

    ptr = buffer;
    ReadLen = IDA10_PREAMBLE_LEN;
    if (gzread(fp, ptr, ReadLen) != ReadLen) {
        return gzeof(fp) ? IDA10_EOF : IDA10_IOERR;
    }

    if ((*pType = ida10Type(ptr)) == IDA10_TYPE_UNKNOWN) {
        return IDA10_DATAERR;
    }
    ptr += ReadLen;

/* Special handling for the ISP logs */

    if (*pType == IDA10_TYPE_ISPLOG) return ReadIspLogRec(fp, ptr, buflen);

/* Remaining records must follow IDA10 format rules */

    switch (ida10SubFormatCode(buffer)) {
      case IDA10_SUBFORMAT_0:
        CmnHdrLen = IDA10_SUBFORMAT_0_HDRLEN;
        break;
      case IDA10_SUBFORMAT_1:
        CmnHdrLen = IDA10_SUBFORMAT_1_HDRLEN;
        break;
      case IDA10_SUBFORMAT_2:
        CmnHdrLen = IDA10_SUBFORMAT_2_HDRLEN;
        break;
      default:
        return IDA10_DATAERR;
    }

/* Read and decode common header */

    if (buflen < CmnHdrLen) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

    ReadLen = CmnHdrLen - IDA10_PREAMBLE_LEN;

    if (gzread(fp, ptr, ReadLen) != ReadLen) return IDA10_IOERR;
    ptr += ReadLen;

    if (ida10UnpackCmnHdr(buffer, &cmn) == 0) return IDA10_DATAERR;

/* Read the rest of the record */

    if (buflen < ida10PacketLength(&cmn)) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }
    ReadLen = cmn.nbytes;
    if (gzread(fp, ptr, ReadLen) != ReadLen) return IDA10_IOERR;

    return IDA10_OK;
}

int rdrec10(gzFile fp, IDA10_TS *ts, struct counter *count)
{
int status, RecordType;
static UINT8 buf[IDA10_MAXRECLEN];
static char tmpbuf[256];

    while (1) {
        status = ida10ReadRecord(fp, buf, IDA10_MAXRECLEN, &RecordType);
        if (status != IDA10_OK) {
            if (status == IDA10_EOF) return FALSE;
            return TRUE;
        }
        ++count->rec;
        if (RecordType == IDA10_TYPE_LM) {
//            unpackLM(buf, count->rec);
            continue;
        } else if (RecordType == IDA10_TYPE_CF) {
            continue;
        } else if (RecordType == IDA10_TYPE_ISPLOG) {
            continue;
        } else if (RecordType != IDA10_TYPE_TS) {
            continue;
        }

        if (ida10UnpackTS(buf, ts)) {
//            if (saveTtags) store_ttag(ts, count->rec);
            return TRUE;
        } else {
/*            sprintf(Buffer, "rdrec: can't decode TS record no. %ld\n",
                count->rec
            );*/
        }
    }
}

void CWinidadmxDlg::LoadCDFileTime(int nIndex)
	{
	IDA_DHDR dhdr;
//	struct data_rec dr;
	long nRead;
	struct counter count;
	IDA10_TS ts;
	double tmin, tmax;
	gzFile file;
	char cDisk[8];
	int nItem;
	char DataBuff[1024];
	char buffer[2 * IDA10_MAXRECLEN];
	CString sfile;
	CComboBox *pCombo;

	pCombo=(CComboBox *)GetDlgItem(IDC_CDCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return ;
	pCombo->GetLBText(nItem, cDisk);

	char cRelease[128];
	pCombo=(CComboBox *)GetDlgItem(IDC_INPUTFORMAT);
	int nFormat=pCombo->GetCurSel();

	pCombo->GetLBText( nFormat, cRelease);

	CDFile *cdf=CDFileArray[nIndex];
	CString s = cdf->fname;

	tmin = cdf->tbeg;
	tmax = cdf->tend;

	if(tmin==0 || tmax==0)
		{
		sfile = cDisk + s;
		file = gzopen(sfile,"rb");
		if(file==NULL) 
			{
			return;
			}

		if(strncmp(cRelease,"10",2)==0)
			{
			while (rdrec10(file, &ts, &count))
				{
				if(tmin==0.) tmin = ts.hdr.tofs;
				tmax = ts.hdr.tols;
				}
			}
		else
			{
			while(1)
				{
				nRead = gzread(file, DataBuff, sizeof(DataBuff));
				if(nRead == 0)
					{
					gzclose(file);
					break;
					}
				if(nRead <= -1)
					{
					gzclose(file);
					break;
					}
//				dbdrec_rev7(NULL, &dr, DataBuff, 1);
				dhead_rev7(NULL, &dhdr, (UINT8 *)DataBuff);
				if(tmin==0.) tmin = dhdr.beg.tru;
				nRead = (nRead/1024-1)*1024;
//				dbdrec_rev7(NULL, &dr, &DataBuff[nRead], 1);
				dhead_rev7(NULL, &dhdr, (UINT8 *)&DataBuff[nRead]);
				tmax = dhdr.beg.tru;
				}
			}
		CDFileArray[nIndex]->tbeg=tmin;
		CDFileArray[nIndex]->tend=tmax;
		}
	}

void CWinidadmxDlg::FillSizeInfo()
	{
	if(m_inputfrom!=2) return;
	CWaitCursor wc;
	char cTBuff1[64], cTBuff2[64], cTime[100];
	int nIndex;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_LIST));
	int i;
	double t1=0., t2=0.;
	CDFile *pCDF;
	int nItems=CDFileArray.GetSize();
	int nFirst =-1;
	int nLast  =-1;

	for(nIndex=0; nIndex<CDFileArray.GetSize(); ++nIndex)
		{
		int state = pList->GetItemState(nIndex, LVIS_SELECTED);
		if(state)
			{
			if(nFirst==-1) nFirst = nIndex;
			nLast = nIndex;
			}
		}


	if(nFirst>-1)
		{
		LoadCDFileTime(nFirst);
		pCDF = CDFileArray[nFirst];
		t1 = pCDF->tbeg;
		t2 = pCDF->tend;
		}
	if(nLast>nFirst)
		{
		LoadCDFileTime(nLast);
		pCDF = CDFileArray[nLast];
		if(t1 > pCDF->tbeg) 
			{
			t1 = pCDF->tbeg;
			}
		if(t2 < pCDF->tend) 
			{
			t2 = pCDF->tend;
			}
		}


	if(t1>0 || t2>0)
		{
		if(t1>0)
			{
			utilLttostr((long)t1, 0, cTBuff1);
			strcpy(cTime, cTBuff1);
			}
		else
			{
			*cTime=0;
			}

		if(t2>0)
			{
			utilLttostr((long)t2, 0, cTBuff2);
			}
		else
			{
			lstrcpy(cTBuff2,"      ");
			}
		strcat(cTime, " ");
		strcat(cTime, cTBuff2);
		SetDlgItemText(IDC_CDFILETIME, cTime);
		}
	else
		{
		SetDlgItemText(IDC_CDFILETIME, "");
		}

	}


DWORD WINAPI MainOutputFileTimeThread(LPVOID lpV)
	{
	long nRead;
//	struct data_rec dr;
	IDA_DHDR dhdr;
	double tmin=0, tmax=0;
	char cTBuff1[64], cTBuff2[64], cTime[100];
	int verbose, nIndex ;
	struct counter count;
	IDA10_TS ts;

	OutputFileTimeThreadParam *param = (OutputFileTimeThreadParam *)lpV;
	CString s = param->sFile;
	CString sfile;
	CWinidadmxDlg *pDlg = param->pDlg;
	char DataBuff[1024];
	char buffer[2 * IDA10_MAXRECLEN];
	nIndex = param->nIndex;

	delete param;


//	EnterCriticalSection(&critFS); 
	

	char cDisk[8];
	int nItem;



	for(nIndex=0; nIndex<CDFileArray.GetSize(); ++nIndex)
		{
	CDFile *cdf=CDFileArray[nIndex];

	tmin = cdf->tbeg;
	tmax = cdf->tend;

	if(tmin==0 || tmax==0)
		{


	CComboBox *pCombo;

	pCombo=(CComboBox *)pDlg->GetDlgItem(IDC_ADDRESSCOMBO);
	nItem = pCombo->GetCurSel();
	if(nItem<0) return 0;
	pCombo->GetLBText(nItem, cDisk);

	char cRelease[128];
	pCombo=(CComboBox *)pDlg->GetDlgItem(IDC_INPUTFORMAT);
	int nFormat=pCombo->GetCurSel();

	pCombo->GetLBText( nFormat, cRelease);


	sfile = cDisk + s;

	gzFile file;

	file = gzopen(sfile,"rb");

	if(file==NULL) 
		{
		LeaveCriticalSection(&critFS);
		return 1;
		}

	if(strncmp(cRelease,"10",2)==0)
		{
		while (rdrec10(file, &ts, &count))
			{
			if(tmin==0.) tmin = ts.hdr.tofs;
			tmax = ts.hdr.tols;
			}
		}
	else
		{
		while(1)
			{
			nRead = gzread(file, DataBuff, sizeof(DataBuff));
			if(nRead == 0)
				{
				gzclose(file);
				break;
				}
			if(nRead <= -1)
				{
				gzclose(file);
				break;
				}
//			dbdrec_rev7(NULL, &dr, DataBuff, 1);
			dhead_rev7(NULL, &dhdr, (UINT8 *)DataBuff);
			if(tmin==0.) tmin = dhdr.beg.tru;
			nRead = (nRead/1024-1)*1024;
//			dbdrec_rev7(NULL, &dr, &DataBuff[nRead], 1);
			dhead_rev7(NULL, &dhdr, (UINT8 *)DataBuff);
			tmax = dhdr.beg.tru;

			}
		CDFileArray[nIndex]->tbeg=tmin;
		CDFileArray[nIndex]->tend=tmax;

		}
	}
	}
	LeaveCriticalSection(&critFS);
	return 0;
	}
CString CWinidadmxDlg::GetDefaultMapFile(LPCSTR lpSta)
	{
	HANDLE hFF;
	WIN32_FIND_DATA ff;
	int i,j;
	char buff[256];
    CString       fullPath, sName;
	GetDlgItemText(IDC_DBDIR,fullPath);

	fullPath.TrimLeft();
	fullPath.TrimRight();
	if(!(fullPath.Right(1).Compare("\\")))
		{
		fullPath+="\\";
		}
	fullPath+="etc\\maps";
	fullPath+=lpSta;

	hFF = FindFirstFile((LPCSTR)fullPath, &ff);

	if(hFF!=INVALID_HANDLE_VALUE)
		{
		FindClose(hFF);
		sName = lpSta;
		return sName;
		}
	sName = "Std";
	return sName;
	}

void CWinidadmxDlg::AddToArray(WIN32_FIND_DATA* fd , int iImage)
	{
	CDFile *cdf;

	cdf = new CDFile;
	cdf->dwSize = fd->nFileSizeLow;
	cdf->tbeg= 0;
	cdf->tend= 0;
	cdf->nType = iImage;
	lstrcpy(cdf->fname, fd->cFileName);
	int nIndex = CDFileArray.Add(cdf);
	}
	


void CWinidadmxDlg::AddArrayToListView()
	{
	CDFile *cdf;
	LV_ITEM	lvitem;
	char sNumBuff[100];
	int iActualItem;
	CString sText;
	int nItems = CDFileArray.GetSize();

	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);

	ArrayIndex=new long[nItems];
	for(int i=0; i<nItems; ++i) ArrayIndex[i]=i;

	if(m_inputfrom==1)qsort( ArrayIndex, nItems, sizeof(long), SortFileArray);

	pList->SetItemCount(nItems);

//	for(int i=0; i<nItems; ++i)
//		{


/*

	char filename[MAX_PATH];
	lstrcpy(filename , fd->cFileName);

	cdf = new CDFile;
	cdf->dwSize = fd->nFileSizeLow;
	cdf->tbeg= 0;
	cdf->tend= 0;
	cdf->nType = iImage;
	lstrcpy(cdf->fname, fd->cFileName);

	int nIndex = CDFileArray.Add(cdf);
	HashTable.SetAt(fd->cFileName, (void *)nIndex);



	lvitem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iItem = 0;
	lvitem.iSubItem = 0;
	lvitem.pszText =  filename; 
	lvitem.iImage = iImage; 
	lvitem.lParam = nIndex;
	iActualItem = pList->InsertItem(&lvitem);


	// Add Size column
	if(iImage==0)
		{
		if(fd->nFileSizeLow != 0)
			ltoa((long)fd->nFileSizeLow,sNumBuff,10);
		else
			strcpy(sNumBuff,"");
		}
	else
		{
		strcpy(sNumBuff,"<dir>");
		}

	lvitem.mask = LVIF_TEXT;
	lvitem.iItem = iActualItem;
	lvitem.iSubItem = 1;
	lvitem.pszText = sNumBuff;
	pList->SetItem(&lvitem);
					
	DWORD dwData = pList->GetItemData(iActualItem);
*/			
	}

void CWinidadmxDlg::UpdateListView()
	{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
	pList->DeleteAllItems();
	ClearCDFileArray();

    HANDLE hFind;
    WIN32_FIND_DATA fd;
    BOOL bResult = FALSE;


    CString strFileSpec = m_dataDirectory;
    if (strFileSpec.Right (1) != "\\")
        strFileSpec += "\\";

	if(m_inputfrom==2)
		strFileSpec += "*.gz";
	else
		strFileSpec += "*.*";

	SetCurrentDirectory(m_dataDirectory);

    if ((hFind = ::FindFirstFile ((LPCTSTR) strFileSpec, &fd)) != INVALID_HANDLE_VALUE)
    {

        do 
		{
            CString strFileName = (LPCTSTR) &fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
//                if ((strFileName != ".") && (strFileName != ".."))
				{
					AddToArray(&fd , 1);
				}
			}
			else
			{
				AddToArray(&fd , 0);
			}
		} while (::FindNextFile (hFind, &fd) && !bResult);

       ::FindClose (hFind);
	}
	AddArrayToListView();
//    GetListCtrl().InsertItem(0, "", ICI_CDUP);

//	UpdateFileDirectoryCount(m_iFileCount , m_iDirectoryCount);
//	SortColumn(0,"C");
	SetCurrentDirectory(m_currentDirectory);
	}


void CWinidadmxDlg::SetupImages()
	{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
	ListView_SetExtendedListViewStyle(pList->m_hWnd, LVS_EX_FULLROWSELECT);
	m_stateImage.Create(16, 16, TRUE, 2, 0);
	CWinApp* pApp = AfxGetApp();
	m_stateImage.Add(pApp->LoadIcon(IDI_FILEICON));
	m_stateImage.Add(pApp->LoadIcon(IDI_FOLDERICON));
//	m_stateImage.Add(pApp->LoadIcon(IDI_OPENEDFOLDER));
//	pList->SetImageList(&m_stateImage, LVSIL_STATE);
	pList->SetImageList(&m_stateImage, LVSIL_SMALL);
	}

void CWinidadmxDlg::OnSelchangeAddresscombo() 
	{
	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_ADDRESSCOMBO);
	int nItem = pCB->GetCurSel();
	CString s; 
	pCB->GetLBText( nItem, s);  
	SetCurrentDirectory(s);
	char cDir[MAX_PATH];
	GetCurrentDirectory(sizeof(cDir), cDir);
	m_dataDirectory = cDir;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);
	SetCurrentDirectory(m_currentDirectory);
	UpdateListView();
	pCB->SetWindowText("");
	}

void CWinidadmxDlg::OnEnteraddress() 
	{
	CComboBox *pCB = (CComboBox *)GetDlgItem(IDC_ADDRESSCOMBO);
	CString s; 
	pCB->GetWindowText(s);
	if(s.IsEmpty())
		{
		MessageBeep(0);
		return;
		}
	SetCurrentDirectory(s);
	char cDir[MAX_PATH];
	GetCurrentDirectory(sizeof(cDir), cDir);
	m_dataDirectory = cDir;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);
	SetCurrentDirectory(m_currentDirectory);
	UpdateListView();
	pCB->SetWindowText("");
	}
void CWinidadmxDlg::OnDblclklist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	CString s;
	NMLISTVIEW *pNM = (NMLISTVIEW *)pNMHDR;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItemIndx= pNM->iItem;
	char cDir[MAX_PATH];
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
	s = pList->GetItemText(iItemIndx,0);
	SetCurrentDirectory(m_dataDirectory);
	if(s.CompareNoCase(".")==0)
		{
		s="\\";
		}
	SetCurrentDirectory(s);
	GetCurrentDirectory(sizeof(cDir), cDir);
	m_dataDirectory = cDir;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);
	UpdateListView();

	GetDlgItem(IDC_ADDRESSCOMBO)->SetWindowText("");

	*pResult = 0;
	}
int CWinidadmxDlg::SortFileArray(const void *p1, const void *p2)
	{
	CDFile *pCD1=CDFileArray[*(long *)p1];
	CDFile *pCD2=CDFileArray[*(long *)p2];

	if(pCD1->nType==pCD2->nType)
		{
		return strcmp(pCD1->fname,  pCD2->fname);
		}
	if(pCD1->nType==0) return 1;
	return -1;
	}
void CWinidadmxDlg::EnterDirectory()
	{
	char cDir[MAX_PATH];
	int iItemIndx;
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);

	iItemIndx = pList->GetNextItem(-1, LVNI_FOCUSED);
	if(iItemIndx<0) return;
	CString s = pList->GetItemText(iItemIndx,0);
	SetCurrentDirectory(m_dataDirectory);
	if(s.CompareNoCase(".")==0)
		{
		s="\\";
		}
	SetCurrentDirectory(s);
	GetCurrentDirectory(sizeof(cDir), cDir);
	m_dataDirectory = cDir;
	m_pDragList->SetDataDirectory(m_dataDirectory);
	SetDlgItemText(IDC_CURRENTDATADIR, m_dataDirectory);
	UpdateListView();
	GetDlgItem(IDC_ADDRESSCOMBO)->SetWindowText("");

	}		
void CWinidadmxDlg::CheckWorkingDirectory()
	{
	if(m_CheckWorkingDirectoryFlag>0) return;
	
	m_CheckWorkingDirectoryFlag=1;

	CString s, sf; 
	GetDlgItemText(IDC_WORKINGDIRECTORY, s);
	if(!CheckDir(s))
		{
		sf.Format(IDS_STRING1043,s);
		if(MessageBox(sf,AfxGetAppName(),MB_ICONEXCLAMATION|MB_OKCANCEL)==IDOK)
			{
			if(!CheckDirectory(s))
				{
				GetDlgItem(IDC_WORKINGDIRECTORY)->SetFocus();
				}
			}
		else
			{
			GetDlgItem(IDC_WORKINGDIRECTORY)->SetFocus();
			}
		}

	m_CheckWorkingDirectoryFlag=0;
	}



void CWinidadmxDlg::OnIdalstButton() 
	{
//	CWaitCursor cw;
	bool bGZfile=false;
	int nItem;
	int nRev;
	char cSource[MAX_PATH+2];
	char cMapFile[MAX_PATH+2];
	UpdateData(TRUE);
	CWnd *pWnd;
	char cBuff[65000]={0};

	CString sRev=m_inputformat.Left(2);
	nRev=atoi((LPCSTR)sRev);
	if(nRev>9) return;

	HWND h1 = ::GetParent(::GetFocus());
	HWND h2 = ::GetDlgItem(this->m_hWnd, IDC_ADDRESSCOMBO);
	if(h1==h2)
		{
		OnEnteraddress();
		return;
		}
	h1 = ::GetFocus();
	h2 = ::GetDlgItem(this->m_hWnd, IDC_LIST);
	if(h1==h2)
		{
		EnterDirectory();
		return;
		}
		

	CComboBox *pCombo=(CComboBox *)GetDlgItem(IDC_MAPFILENAME);
	nItem = pCombo->GetCurSel();
	pCombo->GetLBText(nItem, cMapFile);

	WritePrivateProfileString("MAP FILES",(LPCSTR)m_stacode,cMapFile,pIniFileName);


	ExecString=" ";

/*	if(CheckDir((LPCSTR)(m_workingdirectory+"\\"+m_outputdirectory)))
		{
		MessageBox("Directory already exists","Error", MB_ICONSTOP);
		pWnd=GetDlgItem(IDC_OUTPUTDIRECTORY);
		pWnd->SetFocus();
		return;
		}*/

/*	if(m_inputfrom==1)
		{
		Process1();
		return;
		}*/

	if(m_inputfrom==2 || m_inputfrom==1)
		{
		if(ProcessInputFromList()!=0)
			{
			return;
			}
		}
	
	UpdateData(TRUE);


	if(m_inputformat.IsEmpty())
		{
		MessageBeep(0);
		GetDlgItem(IDC_INPUTFORMAT)->SetFocus();
		return;
		}
	else
		{
		CString sRev=m_inputformat.Left(2);
		nRev=atoi((LPCSTR)sRev);
		if(nRev!=10)
			{
			ExecString+="rev="+sRev+" ";
			}
		}




	if(m_inputfrom==2 || m_inputfrom==1)
		{
		ExecString=ExecString+"if=\x22"+cTempFileForDataFromCD+"\x22 ";
		}
	else
		{
		pWnd=GetDlgItem(IDC_DATASOURCE1);
		if(!pWnd->IsWindowEnabled())
			{
			MessageBox("There is no available tape device","Error",MB_ICONSTOP);
			return;
			}
		pWnd->GetWindowText(cSource, sizeof(cSource));

		if(cSource[0]==0) 
			{
			MessageBeep(0);
			pWnd->SetFocus();
			return;
			}
		else
			{
			if(m_inputfrom==0)
				{
				ExecString=ExecString+"if="+cSource+" ";
				}
			else
				{
				ExecString=ExecString+"if=\x22"+cSource+"\x22 ";
				}
			}
		}

	if(!m_dbdir.IsEmpty())
		{
		ExecString+="db=\x22"+m_dbdir+"\x22 ";
		}

/*	if(m_stacode.IsEmpty()) 
		{
		MessageBeep(0);
		GetDlgItem(IDC_STACODE)->SetFocus();
		return;
		}
	else
		{
		ExecString=ExecString+"sta="+m_stacode+" ";
		}*/

	if((*cMapFile)&&(nRev<10))
		{
		ExecString=ExecString+"map="+cMapFile+" ";
		}


//	ExecString="idalst.exe +call +pum +pcr +ptq +pbt +ps +pi "+ExecString;
	ExecString="idalst.exe   -ps -pk -pi -v -ptt -ci +pl=0 +ctg"+ExecString;
	SaveParam();

    DWORD WINAPI LSTThread(LPVOID lpV);
    DWORD dwId;

	HANDLE hThread=CreateThread(NULL,0,LSTThread, this,0,&dwId);
    SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

    CloseHandle(hThread);
	WorkingDlg=new CWorkingDlg(this);
	EnableWindow(false);
	}
