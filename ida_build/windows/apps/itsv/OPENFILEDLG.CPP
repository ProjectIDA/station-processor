// OpenFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "OpenFileDlg.h"
#include "wfdiscio.h"
#include "TimeFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <sys/types.h>
#include <sys/stat.h>


void ScanWfdLine(struct wfdisc *pwfdisc, char *line);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OpenFileDlg

IMPLEMENT_DYNAMIC(OpenFileDlg, CFileDialog)

OpenFileDlg::OpenFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

void OpenFileDlg::OnFileNameChange( )
	{
	char cFileName[4096];
	CommDlg_OpenSave_GetFilePath(GetParent()->m_hWnd, cFileName, sizeof(cFileName)); 
	LoadWfdiscRecords(cFileName);
	return;
	}
void OpenFileDlg::OnInitDone()
	{
	RECT rc1,rc;
	HWND hMainDlg=GetParent()->m_hWnd;

	::GetClientRect(hMainDlg,&rc1);


	GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.left=0;
	rc.right=rc1.right-rc1.left;
	MoveWindow(&rc);

	SetListHeader();
	

	return;
	}
void OpenFileDlg::SetListHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = { "Station","Channel","SpS","T beg", "T end"};
	RECT rc,rc1;

	HWND hList=GetDlgItem(IDC_LIST)->m_hWnd;
	::GetWindowRect(hList,&rc);
	ScreenToClient(&rc);

	HWND hMainList=GetParent()->GetDlgItem(0x0460)->m_hWnd; 
	::GetWindowRect(hMainList,&rc1);
	ScreenToClient(&rc1);

	rc.left=rc1.left;
	rc.right=rc1.right;

	GetDlgItem(IDC_LIST)->MoveWindow(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right/4;
	lvc.cchTextMax=32;
	for(i=0;i<5;++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		::SendMessage(hList, LVM_INSERTCOLUMN, i, (LPARAM)&lvc);
		}

			
	}	


BEGIN_MESSAGE_MAP(OpenFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(OpenFileDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void OpenFileDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
	// TODO: Add your message handler code here and/or call default
	void DrawListViewItem(LPDRAWITEMSTRUCT lpDrawItem, int nColumns);

	switch(nIDCtl)
		{	
		case IDC_LIST:
			DrawListViewItem(lpDrawItemStruct, 5);
			return;
		}
	CFileDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
void OpenFileDlg::LoadWfdiscRecords(LPSTR lpFileName)
	{
	int nRecord=0;
	CListCtrl	m_list;
	char cChan[32],cSpS[32],cSta[32],cTimeBeg[32],cTimeEnd[32];
	LV_ITEM lvi;
	static char line[WFDISC_SIZE+2];
	static wfdisc wfd;
	struct wfdisc *pwfdisc;
	pwfdisc=&wfd;

	m_list.Attach(GetDlgItem(IDC_LIST)->m_hWnd);

	m_list.DeleteAllItems();


	int result;
	struct _stat buffer;
	result = _stat(lpFileName, &buffer);
	if( result == 0 )
		{
		int file_type=_S_IFREG;
		int dir_type=_S_IFDIR ;
		
		if( (buffer.st_mode & _S_IFMT) == _S_IFMT)
			{
			m_list.Detach();
			return;
			}
		}
	else
		{
		m_list.Detach();
		return;
		}




	FILE *fp=fopen(lpFileName,"rt");

	if(fp==NULL)
		{
		m_list.Detach();
		return;
		}

	_fpreset();
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
			sprintf(cTimeBeg,"%s",dttostr(pwfdisc->time, 1));
			sprintf(cTimeEnd,"%s",dttostr(pwfdisc->endtime, 1));


			memset(&lvi,0,sizeof(lvi));
			lvi.iItem=nRecord;
			lvi.iSubItem=0;
			lvi.mask=LVIF_TEXT;
			lvi.iImage=0;
			lvi.cchTextMax=sizeof(cSta);
			lvi.pszText=cSta;

			m_list.InsertItem(&lvi);
			m_list.SetItemText(nRecord,1,cChan);
			m_list.SetItemText(nRecord,2,cSpS);
			m_list.SetItemText(nRecord,3,cTimeBeg);
			m_list.SetItemText(nRecord,4,cTimeEnd);
			++nRecord;
			}
		}
	fclose(fp);
	m_list.Detach();
	}

