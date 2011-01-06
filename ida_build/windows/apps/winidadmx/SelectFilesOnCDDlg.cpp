// SelectFilesOnCDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "winidadmx.h"
#include "SelectFilesOnCDDlg.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectFilesOnCDDlg dialog


CSelectFilesOnCDDlg::CSelectFilesOnCDDlg(LPCSTR lpcCDDISK, CWnd* pParent /*=NULL*/)
	: CDialogWB(CSelectFilesOnCDDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CSelectFilesOnCDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	cDisk = lpcCDDISK;
	nRelease = -1;
	}

CSelectFilesOnCDDlg::~CSelectFilesOnCDDlg()
	{
//	CString *s;
	FilesList.RemoveAll();
/*	while(!FilesList.IsEmpty())
		{
		s=FilesList.GetTail();
		delete s;
		FilesList.RemoveTail();
		}*/

	}

void CSelectFilesOnCDDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectFilesOnCDDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CSelectFilesOnCDDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectFilesOnCDDlg)
	ON_BN_CLICKED(IDC_SELALL, OnSelall)
	ON_BN_CLICKED(IDC_UNSELALL, OnUnselall)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectFilesOnCDDlg message handlers

BOOL CSelectFilesOnCDDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	
	SetListHeader();
	LoadInfoFromDisk();
	LoadFileNames();

	AddFunctionHelpButton(IDC_UNSELALL, "Unselect all files from the file list");
	AddFunctionHelpButton(IDC_SELALL, "Select all files in the file list");
	AddFunctionHelpButton(IDCANCEL, "Abort data processing");
	AddFunctionHelpButton(IDOK, "Continue data processing");

	return TRUE;
	}

void CSelectFilesOnCDDlg::LoadInfoFromDisk()
	{
	int i=0;
	long lFLength;
	int nHandle;
	char cFileName[MAX_PATH], cSta[8];
	char *Buff;
	

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

	strcpy(cFileName, cDisk);
	strcat(cFileName,"CONTENTS.TXT");

	nHandle = open(cFileName, O_BINARY|O_RDONLY);
	lFLength = _filelength(nHandle);
	Buff = new char[lFLength+1];
	if(read(nHandle, Buff, lFLength)>0)
		{
		Buff[lFLength-1] = 0;
		sscanf(Buff, "%s %d", cSta, &nRelease);
		Station=cSta;
		}

	close(nHandle);
	delete Buff;

	}
void CSelectFilesOnCDDlg::LoadFileNames()
	{
	LV_ITEM lvi;
	long hFile, nIndex=0;
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_LIST);
	char cMask[MAX_PATH], buff[64];

	strcpy(cMask, cDisk);
	strcat(cMask,"*.gz");

	struct _finddata_t all_files;

	if( (hFile = _findfirst( cMask, &all_files )) == -1L ) return ;
	while( _findnext( hFile, &all_files ) == 0 )
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
			pList->SetItemText(nIndex,1,buff);
			++nIndex;
			}
		}
	_findclose(hFile);
	}

void CSelectFilesOnCDDlg::SetListHeader()
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

void CSelectFilesOnCDDlg::OnSelall() 
	{
	SelUnselFiles(TRUE);
	}

void CSelectFilesOnCDDlg::OnUnselall() 
	{
	SelUnselFiles(FALSE);
	}
void CSelectFilesOnCDDlg::SelUnselFiles(BOOL bSel)
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

/*	for(i=0; i<nItems; ++i)
		{
		pList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED );
		}*/

	}

int CSelectFilesOnCDDlg::MakeFilesList()
	{
	CString s, sfile;
	int nItem;
	CListCtrl	*pList;
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
		sfile = cDisk + s;
		FilesList.AddTail((LPCSTR)sfile);
		}

	return FilesList.GetCount();
	}

void CSelectFilesOnCDDlg::OnOK() 
	{
	if(MakeFilesList()>0)
		CDialog::OnOK();
	else
		{
		CString s;
		s.LoadString(IDS_STRING1038);
		MessageBox(s,"", MB_ICONSTOP);
		}
	}

