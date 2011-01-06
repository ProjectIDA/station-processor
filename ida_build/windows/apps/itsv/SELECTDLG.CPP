// SelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "SelectDlg.h"
#include "DrawListView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog


CSelectDlg::CSelectDlg(CWnd* pParent)
	: CDialog(CSelectDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT


	ActiveSta="";

	}


void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDlg)
	DDX_Control(pDX, IDC_STA, m_sta);
	DDX_Control(pDX, IDC_CHAN, m_chan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDlg)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_UNSELECT_ALL, OnUnselectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg message handlers

BOOL CSelectDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	

	SetStaHeader();
	SetChanHeader();
	LoadStaList();

	CBitmap cb1;

	cb1.LoadBitmap("CHECKUNCHECK");


	StateImage.Create(16, 16, TRUE, 2, 0);
	StateImage.Add(&cb1,RGB (255,0,0));

	m_chan.SetImageList(&StateImage, LVSIL_STATE);

	return TRUE;  
	}

void CSelectDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CSelectDlg::SetStaHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = { "Station"};
	RECT rc;
	m_sta.GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right;
	lvc.cchTextMax=64;
	for(i=0;i<1;++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		m_sta.InsertColumn(i,&lvc);
		}

			
	}	

void CSelectDlg::SetChanHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = { "Channels","Samp./sec."};
	RECT rc;
	m_chan.GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right/2;
	lvc.cchTextMax=64;
	for(i=0;i<2;++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		m_chan.InsertColumn(i,&lvc);
		}

			
	}	
void CSelectDlg::LoadStaList()
	{
	char cSta[16];

	m_sta.DeleteAllItems();
	int nSta=StaInfo.AllSta.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		lstrcpy(cSta,StaInfo.AllSta[i]->Sta);

		LV_ITEM lvi;

		memset(&lvi,0,sizeof(lvi));
		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.mask=LVIF_TEXT;
		lvi.iImage=0;
		lvi.cchTextMax=sizeof(cSta);
		lvi.pszText=cSta;

		if(i==0)
			{
			lvi.state=LVIS_SELECTED|LVIS_FOCUSED;
			lvi.mask=LVIF_TEXT|LVIF_STATE;
			LoadChanList(cSta);
			}

		m_sta.InsertItem(&lvi);

		}
	}

void CSelectDlg::LoadChanList(CString sSta)
	{
	char cChan[16],cSpS[16];
	LV_ITEM lvi;

	m_chan.DeleteAllItems();
	int nSta=StaInfo.AllSta.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.CompareNoCase(StaInfo.AllSta[i]->Sta)==0)
			{
			int nChan=StaInfo.AllSta[i]->ChanInfo.GetSize();
			for(int j=0; j<nChan; ++j)
				{
				lstrcpy(cChan, StaInfo.AllSta[i]->ChanInfo[j]->Chan);
				sprintf(cSpS,"%.3f",StaInfo.AllSta[i]->ChanInfo[j]->dSpS);

				memset(&lvi,0,sizeof(lvi));
				lvi.iItem=j;
				lvi.iSubItem=0;
				lvi.mask=LVIF_TEXT;
				lvi.iImage=0;
				lvi.cchTextMax=sizeof(cChan);
				lvi.pszText=cChan;


				m_chan.InsertItem(&lvi);

				m_chan.SetItemText(j,1,cSpS);
				if(StaInfo.AllSta[i]->ChanInfo[j]->bSelected)
					{
					m_chan.SetItemState(j,0x2000,LVIS_STATEIMAGEMASK);
					}
				else
					{
					m_chan.SetItemState(j,0x1000,LVIS_STATEIMAGEMASK);
					}

				}
			return;
			}
		}
	}


BOOL CSelectDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
	// TODO: Add your specialized code here and/or call the base class
    NM_LISTVIEW *pNm = (NM_LISTVIEW *)lParam;
	NMHDR		*pnmhdr=(LPNMHDR )lParam;
	UINT           state;

	if(pnmhdr->idFrom==IDC_CHAN)	return OnChanListNotify(wParam, lParam, pResult);


	switch (pnmhdr->code)
		{
		case LVN_ITEMCHANGED:
			{
			int nItem=pNm->iItem;
            state = m_sta.GetItemState(nItem,LVIS_FOCUSED );
			if(state==1)
				{
				CString Sta=m_sta.GetItemText(nItem,0);
				if(ActiveSta.CompareNoCase(Sta)!=0)
					{
					LoadChanList(Sta);
					ActiveSta=Sta;
					}
				}
			break;
			}
		}	
	
	return CDialog::OnNotify(wParam, lParam, pResult);
	}

BOOL CSelectDlg::OnChanListNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
    NM_LISTVIEW *pNm = (NM_LISTVIEW *)lParam;
	LV_DISPINFO *ptvd= (LV_DISPINFO *)lParam;
	NMHDR		*pnmhdr=(LPNMHDR )lParam;
	UINT           state;
	DWORD          dwpos;
	LV_HITTESTINFO lvhti;
	int            iItemClicked;

	switch (pnmhdr->code)
		{
		case NM_CLICK:
		case NM_RCLICK:
			{
			CImageList 	*himl = m_chan.GetImageList(LVSIL_STATE);
			int cxImage = 0, cyImage = 0;
			if (himl)
				{
				IMAGEINFO im;

				himl->GetImageInfo(0,&im);
				cxImage=im.rcImage.right;
				cyImage=im.rcImage.bottom;

				}


            // Find out where the cursor was
			dwpos = GetMessagePos();
			lvhti.pt.x = LOWORD(dwpos);
			lvhti.pt.y = HIWORD(dwpos);

			m_chan.ScreenToClient(&lvhti.pt);

			if(lvhti.pt.x>cxImage) break;
            // Now do a hittest with this point.
            iItemClicked = m_chan.HitTest(&lvhti);

            if (lvhti.flags & LVHT_ONITEMSTATEICON)
				{
                // Now lets get the state from the item and toggle it.
                state = m_chan.GetItemState(iItemClicked,LVIS_STATEIMAGEMASK);

                state = (state == 0x1000) ? 0x2000 : 0x1000;


				CString Chan=m_chan.GetItemText(iItemClicked,0);

				if(state==0x2000)
					{
					SetChanDrawState(ActiveSta, Chan, TRUE);
					}
				else
					{
					SetChanDrawState(ActiveSta, Chan, FALSE);
					}

				}


                m_chan.SetItemState(iItemClicked, state, LVIS_STATEIMAGEMASK);
            }
		}
		return CDialog::OnNotify(wParam, lParam, pResult);
	}

void CSelectDlg::SetChanDrawState(CString sSta, CString sChan, BOOL bSelected)
	{
	int nSta=StaInfo.AllSta.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.CompareNoCase(StaInfo.AllSta[i]->Sta)==0)
			{
			int nChan=StaInfo.AllSta[i]->ChanInfo.GetSize();
			for(int j=0; j<nChan; ++j)
				{
				if(sChan.CompareNoCase(StaInfo.AllSta[i]->ChanInfo[j]->Chan)==0)
					{
					StaInfo.AllSta[i]->ChanInfo[j]->bSelected=bSelected;
					break;
					}
				}
			return;
			}
		}
	}

void CSelectDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDCtl)
		{	
		case IDC_STA:
			DrawListViewItem(lpDrawItemStruct, 1);
			break;
		case IDC_CHAN:
			DrawListViewItem(lpDrawItemStruct, 2);
			break;
		default:
			CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;

		}
	
}

void CSelectDlg::OnSelectAll() 
	{
	// TODO: Add your control notification handler code here
	int nSta=StaInfo.AllSta.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		int nChan=StaInfo.AllSta[i]->ChanInfo.GetSize();
		for(int j=0; j<nChan; ++j)
			{
			StaInfo.AllSta[i]->ChanInfo[j]->bSelected=true;
			}
		}

	LoadChanList(ActiveSta);	

	}

void CSelectDlg::OnUnselectAll() 
	{
	// TODO: Add your control notification handler code here
	
	int nSta=StaInfo.AllSta.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		int nChan=StaInfo.AllSta[i]->ChanInfo.GetSize();
		for(int j=0; j<nChan; ++j)
			{
			StaInfo.AllSta[i]->ChanInfo[j]->bSelected=false;
			}
		}

	LoadChanList(ActiveSta);	
	}
