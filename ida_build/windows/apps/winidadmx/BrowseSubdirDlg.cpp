// BrowseSubdirDlg.cpp : implementation file
//

#include "stdafx.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "winidadmx.h"
#include "BrowseSubdirDlg.h"
#include "util.h"
#include "css/3.0/wfdiscio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ScanWfdLine(struct wfdisc *pwfdisc, char *line);
bool EmptyDirectory(const char * folderPath);
int CALLBACK      BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData);

/////////////////////////////////////////////////////////////////////////////
// CBrowseSubdirDlg dialog


CBrowseSubdirDlg::CBrowseSubdirDlg(LPCSTR pDir, CWnd* pParent /*=NULL*/)
	: CDialogWB(CBrowseSubdirDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CBrowseSubdirDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	sDirectory = pDir;

	sDirectory.TrimLeft();
	sDirectory.TrimRight();
	if((sDirectory.Right(1).Compare("\\"))!=0)
		{
		sDirectory+="\\";
		}

	}


void CBrowseSubdirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowseSubdirDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowseSubdirDlg, CDialog)
	//{{AFX_MSG_MAP(CBrowseSubdirDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SUBDIRLIST, OnItemchangedSubdirlist)
	ON_BN_CLICKED(IDC_DELDIR, OnDeldir)
	ON_BN_CLICKED(IDC_MOVEDIR, OnMovedir)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_SUBDIRLIST, OnBeginlabeleditSubdirlist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SUBDIRLIST, OnEndlabeleditSubdirlist)
	ON_NOTIFY(NM_RCLICK, IDC_SUBDIRLIST, OnRclickSubdirlist)
	ON_BN_CLICKED(IDC_RENAMEDIR, OnRenamedir)
	ON_NOTIFY(NM_DBLCLK, IDC_SUBDIRLIST, OnDblclkSubdirlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseSubdirDlg message handlers

BOOL CBrowseSubdirDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_WORKINGDIRECTORY, (LPCSTR)sDirectory);

	AddFunctionHelpButton(IDC_DELDIR, "Delete output directory");
	AddFunctionHelpButton(IDC_MOVEDIR, "Move directory");
	AddFunctionHelpButton(IDC_RENAMEDIR, "Rename output directory");
	AddFunctionHelpButton(IDCANCEL, "Quit");


	InitTreeImage();
	SetListHeader();
	LoadDirectoryList();
	SetFileListHeader();
	SetPreviewListHeader();

	return TRUE;
	}

void CBrowseSubdirDlg::InitTreeImage()
	{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_SUBDIRLIST);

	ListView_SetExtendedListViewStyle(pList->m_hWnd, LVS_EX_FULLROWSELECT);
/*	CBitmap cb1;
	cb1.LoadBitmap(IDB_TREEIMG);
	m_stateImage.Create(16, 16, TRUE, 2, 0);
	m_stateImage.Add(&cb1,RGB (255,0,0));
	pList->SetImageList(&m_stateImage, LVSIL_SMALL);*/
	ListView_SetExtendedListViewStyle(pList->m_hWnd, LVS_EX_FULLROWSELECT);
	m_stateImage.Create(16, 16, TRUE, 2, 0);
	CWinApp* pApp = AfxGetApp();
//	m_stateImage.Add(pApp->LoadIcon(IDI_FILEICON));
	m_stateImage.Add(pApp->LoadIcon(IDI_FOLDERICON));
	m_stateImage.Add(pApp->LoadIcon(IDI_OPENEDFOLDER));
//	pList->SetImageList(&m_stateImage, LVSIL_STATE);
	pList->SetImageList(&m_stateImage, LVSIL_SMALL);

	}

void CBrowseSubdirDlg::LoadDirectoryList()
	{
	int nIndex=0;
	LV_ITEM lvi;
	long hFile;
	char cMask[MAX_PATH];
	struct _finddata_t all_files;
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_SUBDIRLIST);

	lstrcpy(cMask, (LPCSTR)sDirectory);
	strcat(cMask,"*.*");

	if( (hFile = _findfirst( cMask, &all_files )) == -1L ) return ;

	do 
        {
		if(!(all_files.attrib&_A_SUBDIR))continue;
        if(all_files.name[0] != '.')
			{
			memset(&lvi,0,sizeof(lvi));
			lvi.iItem=nIndex++;
			lvi.iSubItem=0;
			lvi.mask=LVIF_TEXT|LVIF_STATE;
			lvi.iImage=0;
			lvi.state=0x1000;
			lvi.cchTextMax=128;
			lvi.pszText=all_files.name;
			pList->InsertItem(&lvi);

//			pCombo->AddString(all_files.name);
			}
		}
	while( _findnext( hFile, &all_files ) == 0 );

	_findclose(hFile);

	}
void CBrowseSubdirDlg::SetListHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = {"Dir","Date"};
	RECT rc;
	CListCtrl	*pList;

	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

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


void CBrowseSubdirDlg::SetFileListHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = {"Files"};
	RECT rc;
	CListCtrl	*pList;

	pList = (CListCtrl	*)(GetDlgItem(IDC_FILELIST));

	pList->GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right;
	lvc.cchTextMax=128;
	for(i=0;i<sizeof(colums)/sizeof(char *);++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		pList->InsertColumn(i,&lvc);
		}
			
	}	


BOOL CBrowseSubdirDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
	NMHDR		*pnmhdr=(LPNMHDR )lParam;


	return CDialog::OnNotify(wParam, lParam, pResult);
	}

void CBrowseSubdirDlg::OnItemchangedSubdirlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int nItem = pNMListView->iItem;
	UINT state, stateold;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

   if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
        return;    // No change
	
	if (pNMListView->uOldState == pNMListView->uNewState )
        return;    // No change

	if(pNMListView->uNewState>=0x1000) return;

	if(pNMListView->uChanged & LVIF_STATE)
		{

		if(!(pNMListView->uNewState & LVIS_SELECTED) &&
				(pNMListView->uOldState & LVIS_SELECTED))
			{
			CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_FILELIST);
			ClearFileList();
			m_subdir = "";
			ClearPreview();
			}
		else
			{
			if((pNMListView->uNewState & LVIS_SELECTED) &&
				!(pNMListView->uOldState & LVIS_SELECTED))
				{
				LoadDirectoryInfo(nItem);
				}
			}


		if((pNMListView->uNewState) & LVIS_SELECTED)
			{
			state = 0x2000;
			pList->SetItem(nItem,0, LVIF_IMAGE, NULL, 1, 0,0,0);
			}
		else
			{
			state = 0x1000/*INDEXTOSTATEIMAGEMASK(2)*/;
			pList->SetItem(nItem,0, LVIF_IMAGE, NULL, 0, 0,0,0);
			}
//		stateold = ListView_GetItemState (pList->m_hWnd, nItem,	LVIS_STATEIMAGEMASK);
//		pList->SetItemState(nItem, (state), LVIS_STATEIMAGEMASK);


/*	ListView_SetItemState (pList->m_hWnd, nItem,
		UINT((int(state) + 1) << 12), LVIS_STATEIMAGEMASK);*/

		}
	*pResult = 0;
	}

void CBrowseSubdirDlg::LoadDirectoryInfo(int nItem)
	{
	CListCtrl	*pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

	CString s;
	s = pList->GetItemText(nItem,0);
	LoadFileList( (LPCSTR)s);
	m_subdir = s;
	CString sWfd = FindFirstWfdisc(s);
	if(!sWfd.IsEmpty())
		{
		LoadPreview( (LPCSTR)sWfd);
		}

	}

void CBrowseSubdirDlg::ClearFileList()
	{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_FILELIST);
	pList->DeleteAllItems();
	}

void CBrowseSubdirDlg::LoadFileList(LPCSTR lpDir)
	{
	int nIndex=0;
	LV_ITEM lvi;
	long hFile;
	char cMask[MAX_PATH];
	struct _finddata_t all_files;

	ClearFileList();

	lstrcpy(cMask, (LPCSTR)sDirectory);
	strcat(cMask, lpDir);
	strcat(cMask,"\\*.*");

	if( (hFile = _findfirst( cMask, &all_files )) == -1L ) return ;
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_FILELIST);

	do 
        {
        if(all_files.name[0] != '.')
			{
			memset(&lvi,0,sizeof(lvi));
			lvi.iItem=nIndex++;
			lvi.iSubItem=0;
			lvi.mask=LVIF_TEXT;
			lvi.iImage=0;
			lvi.state=0x1000;
			lvi.cchTextMax=0;
			lvi.pszText=all_files.name;
			pList->InsertItem(&lvi);
			}
		}
	while( _findnext( hFile, &all_files ) == 0 );

	_findclose(hFile);
	}

void CBrowseSubdirDlg::ClearPreview()
	{
	CListCtrl *pList = (CListCtrl *)GetDlgItem(IDC_FILELIST);
	pList->DeleteAllItems();

	}

void CBrowseSubdirDlg::LoadPreview(LPCSTR lpFile)
	{
	CString sFileName;
	int nRecord=0;
	CListCtrl	*pList;
	char cChan[32],cSpS[32],cSta[32],cTimeBeg[32],cTimeEnd[32];
	LV_ITEM lvi;
	static char line[WFDISC_SIZE+2];
	static wfdisc wfd;
	struct wfdisc *pwfdisc;
	pwfdisc=&wfd;

	pList = (CListCtrl *)GetDlgItem(IDC_PREVIEW);

	pList->DeleteAllItems();

	sFileName = sDirectory + m_subdir + "\\";
	sFileName += lpFile;

	int result;
	struct _stat buffer;
	result = _stat((LPCSTR)sFileName, &buffer);
	if( result == 0 )
		{
		int file_type=_S_IFREG;
		int dir_type=_S_IFDIR ;
		
		if( (buffer.st_mode & _S_IFMT) == _S_IFMT)
			{
			return;
			}
		}
	else
		{
		return;
		}

	FILE *fp=fopen((LPCSTR)sFileName,"rt");

	if(fp==NULL)
		{
		return;
		}

	while(1)
		{
		if (fgets(line, WFDISC_SIZE+1, fp) == NULL) break;
		if (lstrlen(line) == WFDISC_SIZE) 
			{
//        sscanf(line, WFDISC_SCS, WFDISC_RVL(pwfdisc));

			ScanWfdLine(pwfdisc, line);
            WFDISC_TRM(pwfdisc);

			lstrcpy(cSta,	pwfdisc->sta);
			lstrcpy(cChan,	pwfdisc->chan);
			sprintf(cSpS,"%.3f", pwfdisc->smprate);
			sprintf(cTimeBeg,"%s",util_dttostr(pwfdisc->time, 1));
			sprintf(cTimeEnd,"%s",util_dttostr(pwfdisc->endtime, 1));


			memset(&lvi,0,sizeof(lvi));
			lvi.iItem=nRecord;
			lvi.iSubItem=0;
			lvi.mask=LVIF_TEXT;
			lvi.iImage=0;
			lvi.cchTextMax=sizeof(cSta);
			lvi.pszText=cSta;

			pList->InsertItem(&lvi);
			pList->SetItemText(nRecord,1,cChan);
			pList->SetItemText(nRecord,2,cSpS);
			pList->SetItemText(nRecord,3,cTimeBeg);
			pList->SetItemText(nRecord,4,cTimeEnd);
			++nRecord;
			}
		}
	fclose(fp);

	}

void CBrowseSubdirDlg::SetPreviewListHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = { "Station","Channel","SpS","T beg", "T end"};

	RECT rc;
	CListCtrl	*pList;

	pList = (CListCtrl	*)(GetDlgItem(IDC_PREVIEW));

	pList->GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right/5;
	lvc.cchTextMax=128;
	for(i=0;i<sizeof(colums)/sizeof(char *);++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		pList->InsertColumn(i,&lvc);
		}
			
		
	}	

CString CBrowseSubdirDlg::FindFirstWfdisc(LPCSTR sSub)
	{
	CString       fullPath, sName;
	HANDLE hFF;
	WIN32_FIND_DATA ff;

	fullPath = sDirectory+sSub+"\\*.wfdisc";

	hFF = FindFirstFile((LPCSTR)fullPath, &ff);

	if(hFF!=INVALID_HANDLE_VALUE)
		{
		sName = ff.cFileName;
		FindClose(hFF);
		}
	return sName;
	}

void CBrowseSubdirDlg::OnDeldir() 
	{
	CString s;
	int nItem;
	CArray <int, int> Index;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
		{
		return ;
		}

	while (pos)
		{
		nItem = pList->GetNextSelectedItem(pos);
		s = pList->GetItemText(nItem,0);
		if(DeleteDirectory(s))
			{
			Index.Add(nItem);
			}
		}
	for(int i=Index.GetSize()-1; i>-1; --i)
		{
		pList->DeleteItem(Index[i]);
		}
	if(Index.GetSize()>0)
		{
		nItem = Index[0];
		if(nItem<0) nItem=0;
		if(nItem>=pList->GetItemCount())
			{
			nItem=pList->GetItemCount()-1;
			}
//		pList->SetSelectionMark(nItem);
		pList->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED );
		LoadDirectoryInfo(nItem);
		}
	else
		{
		ClearPreview();
		ClearFileList();
		}
	}
BOOL CBrowseSubdirDlg::DeleteDirectory(LPCSTR lpDir) 
	{
	CString sDir;
	sDir = sDirectory+lpDir;
	return EmptyDirectory((LPCSTR)sDir);
	}

void CBrowseSubdirDlg::OnMovedir() 
	{
	LPITEMIDLIST _p;
    BROWSEINFO _browseInfo;
    char       _displayName [MAX_PATH];
    char       _fullPath [MAX_PATH];

	lstrcpy(_fullPath, (LPCSTR)sDirectory);

    _displayName [0] = '\0';
    _browseInfo.hwndOwner = m_hWnd;
    _browseInfo.pidlRoot = NULL; 
    _browseInfo.pszDisplayName = _displayName;
    _browseInfo.lpszTitle = "Move to";
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
		MoveSelectedDirTo(_fullPath);
		}
	
	}
void CBrowseSubdirDlg::MoveSelectedDirTo(LPCSTR lpPath)
	{
	CString sSource, sTarget,s, sPath=lpPath;
	int nItem;
	CArray <int, int> Index;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

	sPath.TrimLeft();
	sPath.TrimRight();
	if((sPath.Right(1).Compare("\\"))!=0)
		{
		sPath+="\\";
		}



	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
		{
		return ;
		}

	while (pos)
		{
		nItem = pList->GetNextSelectedItem(pos);
		s = pList->GetItemText(nItem,0);
		sSource = sDirectory + s; 
		sTarget = sPath+s;
		if(MoveFile(sSource, sTarget))
			{
			Index.Add(nItem);
			}
		}
	for(int i=Index.GetSize()-1; i>-1; --i)
		{
		pList->DeleteItem(Index[i]);
		}
	if(Index.GetSize()>0)
		{
		nItem = Index[0];
		if(nItem<0) nItem=0;
		if(nItem>=pList->GetItemCount())
			{
			nItem=pList->GetItemCount()-1;
			}
//		pList->SetSelectionMark(nItem);
		pList->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED );
		LoadDirectoryInfo(nItem);
		}
	else
		{
		ClearPreview();
		ClearFileList();
		}

	}

void CBrowseSubdirDlg::OnBeginlabeleditSubdirlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
//	*pResult = 0;
//	return;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));
	LV_DISPINFO *lpLvdi=(LV_DISPINFO *)pNMHDR;

	m_editedItem = lpLvdi->item.iItem;

/*	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_DISPINFO *lpLvdi=(LV_DISPINFO *)pNMHDR;
	// TODO: Add your control notification handler code here
	CRect	subrect;
	pList->GetSubItemRect( lpLvdi->item.iItem, 0, LVIR_BOUNDS , subrect );

		//Get edit control and subclass
	HWND hWnd=(HWND)pList->SendMessage(LVM_GETEDITCONTROL);
	ASSERT(hWnd!=NULL);
	VERIFY(m_editWnd.SubclassWindow(hWnd));

		//Move edit control text 1 pixel to the right of org label, as Windows does it...
	m_editWnd.m_x=subrect.left + 6; 
	m_editWnd.SetWindowText(pList->GetItemText(lpLvdi->item.iItem,0));

		//Hide subitem text so it don't show if we delete some text in the editcontrol.
		//OnPaint handles other issues also regarding this.
			CRect	rect;
//			m_editWnd.GetWindowRect(rect);
	pList->GetSubItemRect(lpLvdi->item.iItem, 0,LVIR_LABEL ,rect);
	CClientDC	dc(this);
	m_editWnd.m_y  = rect.top;
	m_editWnd.m_cy = rect.bottom - rect.top;

	dc.FillRect(rect,&CBrush(::GetSysColor(COLOR_WINDOW)));*/
	
	*pResult = 0;
	}

void CBrowseSubdirDlg::OnEndlabeleditSubdirlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	if(pDispInfo->item.pszText!=0)
		{
		CListCtrl	*pList;
		pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));

		CString s1 = pDispInfo->item.pszText;
		CString s2 = pList->GetItemText(m_editedItem,0); 
		CString sDir1, sDir2;

		sDir1 = sDir2 = sDirectory;
		sDir1 += s1;
		sDir2 += s2;

		if(MoveFile(sDir2, sDir1))
			{
			pList->SetItemText(m_editedItem,0,s1); 
			}
		}
	*pResult = 0;
	}

void CBrowseSubdirDlg::OnRclickSubdirlist(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	DWORD          dwpos;
	LV_HITTESTINFO lvhti;
	int nItem;
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));
	dwpos = GetMessagePos();
	lvhti.pt.x = LOWORD(dwpos);
	lvhti.pt.y = HIWORD(dwpos);
	lvhti.flags=LVHT_NOWHERE;
	pList->ScreenToClient(&lvhti.pt);
	nItem = pList->SubItemHitTest(&lvhti);

//			nItem = PhasesList.HitTest(&lvhti);


	if(nItem>=0)
		{
		pList->EditLabel(nItem);
		}
	*pResult = 0;
	}

void CBrowseSubdirDlg::OnRenamedir() 
	{
	CListCtrl	*pList;
	pList = (CListCtrl	*)(GetDlgItem(IDC_SUBDIRLIST));
	int nS = pList->GetSelectedCount();

	if(nS>1 || nS==0)
		{
		MessageBeep(0);
		return;
		}

	POSITION pos = pList->GetFirstSelectedItemPosition();
	if (pos == NULL)
		{
		MessageBeep(0);
		return ;
		}

	int nItem = pList->GetNextSelectedItem(pos);
	if(nItem>=0)
		{
		pList->SetFocus();
		pList->EditLabel(nItem);
		}
		


	}


void CBrowseSubdirDlg::OnDblclkSubdirlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
