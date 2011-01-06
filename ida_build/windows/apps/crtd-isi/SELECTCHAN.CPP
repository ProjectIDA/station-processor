// SelectChan.cpp : implementation file
//

#include "stdafx.h"
#include "crtd.h"
#include "SelectChan.h"
#include "DrawListView.h"
#include "crtdglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectChan property page


CSelectChan::CSelectChan(CNRTSINFO &NrtsInfo) : CPropertyPage(CSelectChan::IDD), NrtsInfoX(NrtsInfo)
{
	//{{AFX_DATA_INIT(CSelectChan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
ActiveSta="";
}

CSelectChan::~CSelectChan()
{
}

void CSelectChan::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectChan)
	DDX_Control(pDX, IDC_STA, m_sta);
	DDX_Control(pDX, IDC_CHAN, m_chan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectChan, CPropertyPage)
	//{{AFX_MSG_MAP(CSelectChan)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_LOADLIST, OnLoadlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectChan message handlers

void CSelectChan::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
	{
	// TODO: Add your message handler code here and/or call default
	switch(nIDCtl)
		{	
		case IDC_STA:
			DrawListViewItem(lpDrawItemStruct, 1);
			break;
		case IDC_CHAN:
			DrawListViewItem(lpDrawItemStruct, 3);
			break;
		default:
			CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;

		}

	}

BOOL CSelectChan::OnInitDialog() 
	{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	SetStaHeader();
	SetChanHeader();
	LoadStaList();

	CBitmap cb1;

	cb1.LoadBitmap("CHECKUNCHECK");


	StateImage.Create(16, 16, TRUE, 2, 0);
	StateImage.Add(&cb1,RGB (255,0,0));

	m_chan.SetImageList(&StateImage, LVSIL_STATE);
	m_sta.SetImageList(&StateImage, LVSIL_STATE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}
void CSelectChan::SetStaHeader()
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

void CSelectChan::SetChanHeader()
	{
	LV_COLUMN lvc;
	int i;
	char *colums[] = { "Channels","Loc.code","Samp./sec."};
	RECT rc;
	m_chan.GetClientRect(&rc);


	memset(&lvc,0,sizeof(lvc));
	lvc.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_LEFT;
	lvc.cx=rc.right/3;
	lvc.cchTextMax=64;
	for(i=0;i<3;++i)
		{
		lvc.iSubItem=i;
		lvc.pszText=colums[i];
		m_chan.InsertColumn(i,&lvc);
		}

			
	}	
void CSelectChan::LoadStaList()
	{
	char cSta[16];

	m_sta.DeleteAllItems();
	int nSta=NrtsInfoX.StaInfo.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		lstrcpy(cSta,NrtsInfoX.StaInfo[i]->Sta);

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
		if(bHasSelectedChan(cSta))
			{
			m_sta.SetItemState(i,0x2000,LVIS_STATEIMAGEMASK);
			}
		else
			{
			m_sta.SetItemState(i,0x1000,LVIS_STATEIMAGEMASK);
			}

		}
	}

BOOL CSelectChan::bHasSelectedChan(CString sSta)
	{
	int nSta=NrtsInfoX.StaInfo.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.CompareNoCase(NrtsInfoX.StaInfo[i]->Sta)==0)
			{
			int nChan=NrtsInfoX.StaInfo[i]->ChanInfo.GetSize();
			for(int j=0; j<nChan; ++j)
				{
				CNRTSCHANINFO *ci = NrtsInfoX.StaInfo[i]->ChanInfo[j];
				if(ci->bSelected) return TRUE;
				}
			}
		}
	return FALSE;
	}
void CSelectChan::LoadChanList(CString sSta)
	{
	char cChan[16],cSpS[16], cLoc[16];
	LV_ITEM lvi;

	m_chan.DeleteAllItems();
	int nSta=NrtsInfoX.StaInfo.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.CompareNoCase(NrtsInfoX.StaInfo[i]->Sta)==0)
			{
			int nChan=NrtsInfoX.StaInfo[i]->ChanInfo.GetSize();
			for(int j=0; j<nChan; ++j)
				{
				lstrcpy(cChan, NrtsInfoX.StaInfo[i]->ChanInfo[j]->Chan);
				if(NrtsInfoX.StaInfo[i]->ChanInfo[j]->dSpS>0)
					{
					sprintf(cSpS,"%.2f",NrtsInfoX.StaInfo[i]->ChanInfo[j]->dSpS);
					}
				else
					{
					sprintf(cSpS,"-");
					}
				lstrcpy(cLoc, NrtsInfoX.StaInfo[i]->ChanInfo[j]->LCODE);
				memset(&lvi,0,sizeof(lvi));
				lvi.iItem=j;
				lvi.iSubItem=0;
				lvi.mask=LVIF_TEXT;
				lvi.iImage=0;
				lvi.cchTextMax=sizeof(cChan);
				lvi.pszText=cChan;


				m_chan.InsertItem(&lvi);
				
				m_chan.SetItemText(j,1,cLoc);
				m_chan.SetItemText(j,2,cSpS);
				if(NrtsInfoX.StaInfo[i]->ChanInfo[j]->bSelected)
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

BOOL CSelectChan::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{

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
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
	}


/**************************************************/
BOOL CSelectChan::OnChanListNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

/*				{
				LV_ITEM lvi;
                int i,j,n;
                union 
					{ 
					struct LISTINDX lst;
					LPARAM lParam;
					} LL;

				memset(&lvi,0,sizeof(lvi));
				lvi.mask=LVIF_PARAM;
				lvi.iItem=iItemClicked;
				lvi.iSubItem=0;
				PhasesList.GetItem(&lvi);

				LL.lParam=lvi.lParam;		
				i=LL.lst.sta;
				j=LL.lst.chan;
				n=LL.lst.phase;

				SEISMIC_PHASE *pPhase;

				pPhase=allWSGPhases.GetPhase(i,j,n);*/

				CString Chan  = m_chan.GetItemText(iItemClicked,0);
				CString LCODE = m_chan.GetItemText(iItemClicked,1);
				if(state==0x2000)
					{
					SetChanDrawState(ActiveSta, Chan, LCODE, TRUE);
					}
				else
					{
					SetChanDrawState(ActiveSta, Chan, LCODE, FALSE);
					}

				}


                m_chan.SetItemState(iItemClicked, state, LVIS_STATEIMAGEMASK);
				SetModified(TRUE);
            }
		}
		return CPropertyPage::OnNotify(wParam, lParam, pResult);
	}

/**************************************************/
void CSelectChan::SetChanDrawState(const CString &sSta, const CString &sChan, const CString &LCODE, BOOL bSelected)
	{
	int nSta=NrtsInfoX.StaInfo.GetSize();
	for(int i=0; i<nSta; ++i)
		{
		if(sSta.CompareNoCase(NrtsInfoX.StaInfo[i]->Sta)==0)
			{
			int nChan=NrtsInfoX.StaInfo[i]->ChanInfo.GetSize();
			for(int j=0; j<nChan; ++j)
				{
				if(sChan.CompareNoCase(NrtsInfoX.StaInfo[i]->ChanInfo[j]->Chan)==0)
					{
					if(LCODE.IsEmpty()&&NrtsInfoX.StaInfo[i]->ChanInfo[j]->LCODE.IsEmpty())
						{
						NrtsInfoX.StaInfo[i]->ChanInfo[j]->bSelected=bSelected;
						SetStationCheck();
						return;
						}
					else
						{
						if( (LCODE.Compare(NrtsInfoX.StaInfo[i]->ChanInfo[j]->LCODE)==0) &&
							(!NrtsInfoX.StaInfo[i]->ChanInfo[j]->LCODE.IsEmpty()))
							{
							NrtsInfoX.StaInfo[i]->ChanInfo[j]->bSelected=bSelected;
							SetStationCheck();
							return;
							}
						}
					}
				}
			return;
			}
		}
	}
void CSelectChan::SetStationCheck()
	{
	char cSta[16];
	int nItem = m_sta.GetNextItem(-1, LVNI_SELECTED );
	if(nItem==-1) return;
	m_sta.GetItemText(nItem, 0, cSta, sizeof(cSta)-1);
	if(bHasSelectedChan(cSta))
		{
		m_sta.SetItemState(nItem,0x2000,LVIS_STATEIMAGEMASK);
		}
	else
		{
		m_sta.SetItemState(nItem,0x1000,LVIS_STATEIMAGEMASK);
		}
	}
BOOL CSelectChan::OnApply() 
	{
	// TODO: Add your specialized code here and/or call the base class
	NrtsInfoX.SaveConfigToIni();
	::NrtsInfo = NrtsInfoX;
	return CPropertyPage::OnApply();
	}

void CSelectChan::OnLoadlist() 
	{
	// TODO: Add your control notification handler code here

	if(NrtsInfoX.LoadConfigFromServer()==1)
		{
		LoadStaList();
		SetModified(TRUE);
		}
	else
		{
		::MessageBox(m_hWnd,"Load Config from server error","ERROR",MB_ICONSTOP);
		}

	}
