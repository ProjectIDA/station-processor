// WaveformsDisplay.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "SelectDlg.h"
#include <stdio.h>
#include <math.h>
#include "TimeFunc.h"
#include "MainFrm.h"
#include "SetStartTimeDlg.h"
#include "globals.h"
#include "filter\filter.h"
#include "WaveformsPrint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ScanWfdLine(struct wfdisc *pwfdisc, char *line);
void DrawBitmap(CDC *DC, int x, int y, CBitmap *Bitmap);
void DrawBitmap(CDC *DC, int x, int y, CBitmap *Bitmap, int x1, int y1);

void _close_filter();
void _do_filter(float *data);
void _init_filter(float *data);
int _open_filter(double  dt, int nFilter);


extern struct gather_plot_filter_ gpl_filters_def[];
/////////////////////////////////////////////////////////////////////////////
// CWaveformsDisplay

CWaveformsDisplay::CWaveformsDisplay()
	{
	nAmpFlag=0;
	nActiveScreen=-1;
	t1=t2=tB1=tB2=-1;
	bBox=false;
	bBoxExpandX=false;
	bBoxContractX=false;
	bBoxExpandY=false;
	bBoxContractY=false;
	bMiddleButtonDrag=false;
	pScreen=NULL;
	pScreen=new CScreen();
	bClip=false;
	bPanelSelectionActive=false;
	DisplayMode=mAuto;
	bInvert=false;
	dGain=1.;
	mbuttonx=0;
	mbuttony=0;
	xX1=xX2=0;
	nFilterType=-1;
	}

CWaveformsDisplay::~CWaveformsDisplay()
	{
	ClearWaveformInfo();
	if(pScreen!=NULL) delete pScreen;
	}
void CWaveformsDisplay::ClearWaveformInfo()
	{
	while(WaveformInfo.GetSize()>0)
		{
		CMWindow *wfi=WaveformInfo[0];
		WaveformInfo.RemoveAt(0);
		delete wfi;
		}
	}
void CWaveformsDisplay::DrawFocused(int nScr, CDC *pDC)
	{
	int nWfm=pScreen->GetWaveformN(nScr);
	int xw1=WaveformInfo[nWfm]->xw1+4;
	int xw2=WaveformInfo[nWfm]->xw1+WindowInfoXsize-4;
	int yw1=WaveformInfo[nWfm]->yw1+4;
	int yw2=WaveformInfo[nWfm]->yw2-4;

    CPen RedPen(PS_SOLID,1,RGB(255,0,0));
    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
    CPen GrayPen(PS_SOLID,1,RGB(128,128,128));
	CBrush RedBrush(RGB(255,0,0));

    CPen   *oldPen=pDC->GetCurrentPen();

	pDC->SelectObject(&GrayPen);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw1,MaxY-yw1);

	pDC->SelectObject(&WhitePen);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw1,MaxY-yw1);


	pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	CString cBuff;
	cBuff.Format("%d",pScreen->GetSelectionNumber(nScr)+1);
	RECT rcc;
	GetTextRectangle((LPSTR)(LPCTSTR)cBuff,rcc);
	rcc.left+=xw1;
	rcc.right+=xw1;

	int height=(rcc.top-rcc.bottom);
	rcc.bottom=(MaxY-yw1);
	rcc.top=(MaxY-yw1)+height;

	pDC->SetTextColor(RGB(255,0,0));
	pDC->SetBkMode(TRANSPARENT);

	pDC->DrawText(cBuff,&rcc,DT_LEFT|DT_SINGLELINE);

    pDC->SelectObject(oldPen);

	}


BEGIN_MESSAGE_MAP(CWaveformsDisplay, CWnd)
	//{{AFX_MSG_MAP(CWaveformsDisplay)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_MBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWaveformsDisplay message handlers

void CWaveformsDisplay::OnPaint() 
	{
	CPaintDC dc(this); // device context for painting

	
	ScreenBitmapCriticalSection.Lock();

	if(leftPannel.m_hObject!=NULL)
		{
		DrawBitmap(&dc, 0, 0,&leftPannel);
		dc.SetBkMode(TRANSPARENT);

		int nMaxWin=pScreen->GetMaxWindow();
		for(int i=0; i<nMaxWin; ++i)
			{
			if(pScreen->IsSelected(i))	DrawFocused(i,&dc);
			}

		}

	if(bMiddleButtonDrag)
		{
		if(offscrbmp.m_hObject!=NULL)
			{
			DrawBitmap(&dc, WindowInfoXsize, 0,&offscrbmp);
			}
		}
	else
		{
		if(Bitmap.m_hObject!=NULL)
			{
			DrawBitmap(&dc, WindowInfoXsize, 0,&Bitmap);
			}
		}

	if(timePanel.m_hObject!=NULL)
		{
		DrawBitmap(&dc, WindowInfoXsize, MaxY-TimeBarYsize,&timePanel);
		}
	CPen dotPen(PS_DOT, 1, RGB(255,255,255));
	CPen *oldPen=dc.SelectObject(&dotPen);

	for(int i=0; i<tmmarks.GetSize(); ++i)
		{
		int x=tmmarks[i];
		dc.MoveTo(x,MaxY-TimeBarYsize);		
		dc.LineTo(x,1);
		}
	dc.SelectObject(oldPen);

	ScreenBitmapCriticalSection.Unlock();
	}

void CWaveformsDisplay::OnLButtonUp(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default

	int x=point.x;
	int y=MaxY-point.y;

	bPanelSelectionActive=false;


	CWnd::OnLButtonUp(nFlags, point);
	}
void CWaveformsDisplay::ExpandX(int x)
	{
	int mouse_x1, mouse_x2;

	if(!bBoxExpandX) return;

	bBoxExpandX=false;


	if(x!=mouse_x)
		{
		if(mouse_x>x)
			{
			mouse_x1=x;
			mouse_x2=mouse_x;
			}
		else
			{
			mouse_x2=x;
			mouse_x1=mouse_x;
			}
		double ttt1=(mouse_x1-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double ttt2=(mouse_x2-xX1)*(t2-t1)/(xX2-xX1)+t1;
		t1=ttt1;
		t2=ttt2;
		DrawWaveforms();
		RedrawPreview();
		}

	}
void CWaveformsDisplay::ContractX(int x)
	{
	int mouse_x1, mouse_x2;
	if(!bBoxContractX) return;
	bBoxContractX=false;

	double ttt0=t1;
	double ttt1=t2;


	if(x!=mouse_x)
		{
		if(mouse_x>x)
			{
			mouse_x1=x;
			mouse_x2=mouse_x;
			}
		else
			{
			mouse_x2=x;
			mouse_x1=mouse_x;
			}
		double ttt3=(mouse_x1-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double ttt4=(mouse_x2-xX1)*(t2-t1)/(xX2-xX1)+t1;


		double delta_s=(ttt1-ttt0)*(ttt1-ttt0)/(ttt4-ttt3);
		t1=ttt0-(ttt3-ttt0)*(ttt1-ttt0)/(ttt4-ttt3);
		t2=t1+delta_s;


		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}

	}
void CWaveformsDisplay::ExpandY(int y)
	{
	if(!bBoxExpandY) return;
	bBoxExpandY=false;
	int mouse_y1,mouse_y2;
	bBoxExpandY=false;

	int nWfm=pScreen->GetWaveformN(nBoxWindow);


	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;

	if(y==mouse_y) { InvalidateRect(NULL, TRUE);return;}

	if(mouse_y>y)
		{
		mouse_y1=y;
		mouse_y2=mouse_y;
		}
	else
		{
		mouse_y2=y;
		mouse_y1=mouse_y;
		}

	long amax=WaveformInfo[nWfm]->amax;
	long amin=WaveformInfo[nWfm]->amin;
	long aaa1=(long)((double)(mouse_y1-y1)*(amax-amin)/(y2-y1)+amin+0.5);
	long aaa2=(long)((double)(mouse_y2-y1)*(amax-amin)/(y2-y1)+amin+0.5);


	WaveformInfo[nWfm]->amax=aaa2;
	WaveformInfo[nWfm]->amin=aaa1;

	RedrawWindow(nBoxWindow);
	DrawLeftPannel();

	}

void CWaveformsDisplay::ContractY(int y)
	{
	if(!bBoxContractY) return;
	bBoxContractY=false;
	int mouse_y1, mouse_y2;

	int nWfm=pScreen->GetWaveformN(nBoxWindow);

	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;


	if(y==mouse_y) { InvalidateRect(NULL, TRUE);return;}

	if(mouse_y>y)
		{
		mouse_y1=y;
		mouse_y2=mouse_y;
		}
	else
		{
		mouse_y2=y;
		mouse_y1=mouse_y;
		}

	long amax=WaveformInfo[nWfm]->amax;
	long amin=WaveformInfo[nWfm]->amin;
	double aaa0=amin;
	double aaa1=amax;
	double aaa3=(mouse_y1-y1)*(amax-amin)/(y2-y1)+amin;
	double aaa4=(mouse_y2-y1)*(amax-amin)/(y2-y1)+amin;


	double delta_a=(aaa1-aaa0)*(aaa1-aaa0)/(aaa4-aaa3);
	long a1=(long)(aaa0-(aaa3-aaa0)*(aaa1-aaa0)/(aaa4-aaa3));
	long a2=(long)(a1+delta_a);


	WaveformInfo[nWfm]->amax=a2;
	WaveformInfo[nWfm]->amin=a1;


	RedrawWindow(nBoxWindow);
	DrawLeftPannel();

	}
void CWaveformsDisplay::LoadWfdiscFileInfo(CString FileName)
	{
	char line[WFDISC_SIZE+2];
	wfdisc wfd;
	wfdisc *pwfdisc;
	pwfdisc=&wfd;
	FILE *fp=fopen((LPCSTR)FileName,"rt");

	nActiveScreen=-1;
	t1=t2=tB1=tB2=-1;

	AllStaChan.Clear();
	ClearWaveformInfo();
	wfdarray.Clear();

	if(fp==NULL)
		{
		MessageBox("Wfdisc file open error","Data View",MB_ICONSTOP);
		return;
		}

	while(1)
		{
		if (fgets(line, WFDISC_SIZE+1, fp) == NULL) break;
		if (strlen(line) == WFDISC_SIZE) 
			{
//            sscanf(line, WFDISC_SCS, WFDISC_RVL(pwfdisc));
			ScanWfdLine(pwfdisc, line);
            WFDISC_TRM(pwfdisc);

			AllStaChan.AddInfo(*pwfdisc);
			wfdarray.Add(*pwfdisc);
			}
		}
	fclose(fp);
	WfdFileName=FileName;
	RunSelectDlg();
	}
void CWaveformsDisplay::RunSelectDlg()
	{
	CSelectDlg SelDlg(this);

	SelDlg.StaInfo=AllStaChan;
	if(SelDlg.DoModal()==IDOK)
		{
		AllStaChan=SelDlg.StaInfo;
		FillWaveformInfo();
		FindMinMaxTimeInWaveformInfo();
		SetStartTime();
		DrawWaveforms();
		RedrawPreview();
		}
	}
void CWaveformsDisplay::SetStartTime()
	{
	int nCounts=GetPrivateProfileInt("START","MAXCOUNTS",6000,cpIniFileName);
	double dDelta=t2-t1;

	for(int i=0; i<WaveformInfo.GetSize(); ++i)
		{
		for(int k=0; k<WaveformInfo[i]->wfdrec.GetSize(); ++k)
			{
			double tt1=WaveformInfo[i]->wfdrec[k]->wfd.time;
			double tt2=WaveformInfo[i]->wfdrec[k]->wfd.endtime;
			float nSpS=WaveformInfo[i]->wfdrec[k]->wfd.smprate;

			if(dDelta*nSpS>nCounts) dDelta=nCounts/nSpS;

			}
		}

	t2=t1+dDelta;
	}
void CWaveformsDisplay::FillWaveformInfo()
	{
	int i,j,k,l;
	CString Sta,Chan;
	double dSpS;
	for(i=0; i<AllStaChan.AllSta.GetSize();++i)
		{
		Sta=AllStaChan.AllSta[i]->Sta;
		for(j=0; j<AllStaChan.AllSta[i]->ChanInfo.GetSize(); ++j)
			{
			Chan=AllStaChan.AllSta[i]->ChanInfo[j]->Chan;
			dSpS=AllStaChan.AllSta[i]->ChanInfo[j]->dSpS;
			if(AllStaChan.AllSta[i]->ChanInfo[j]->bSelected)
				{
				BOOL bFound=FALSE;
				for(l=0; l<WaveformInfo.GetSize(); ++l)
					{
					if( (Sta.Compare(WaveformInfo[l]->StaName)==0) && (Chan.Compare(WaveformInfo[l]->ChanName)==0))
						{
						bFound=TRUE;
						break;
						}
					}

				if(!bFound)
					{
					CMWindow *mw=new CMWindow(Sta, Chan, dSpS);
					int m=WaveformInfo.Add(mw);
					for(k=0; k<wfdarray.GetSize(); ++k)
						{
						if( (Sta.Compare(wfdarray[k]->wfd.sta)==0) && (Chan.Compare(wfdarray[k]->wfd.chan)==0))
							{
							mw->AddWfdRecord(wfdarray[k]->wfd);
							}
						}
//					mw->AutoScale(t1,t2);
					}
				}
			else
				{
				for(l=0; l<WaveformInfo.GetSize(); ++l)
					{
					if( (Sta.Compare(WaveformInfo[l]->StaName)==0) && (Chan.Compare(WaveformInfo[l]->ChanName)==0))
						{
						CMWindow *wfi=WaveformInfo[l];
						WaveformInfo.RemoveAt(l);
						delete wfi;
						}

					}
				}
			}
		}

	pScreen->Clear();
	for(l=0; l<WaveformInfo.GetSize(); ++l)
		{
		pScreen->AddElement(l);
		}

	}
void CWaveformsDisplay::FindMinMaxTimeInWaveformInfo()
	{
	int i,k;

	tB1=-1;
	tB2=-1;
	t1=-1;
	t2=-1;
	for(i=0; i<WaveformInfo.GetSize(); ++i)
		{
		for(k=0; k<WaveformInfo[i]->wfdrec.GetSize(); ++k)
			{
			double tt1=WaveformInfo[i]->wfdrec[k]->wfd.time;
			double tt2=WaveformInfo[i]->wfdrec[k]->wfd.endtime;

			if(tB1==-1)
				{
				tB1=tt1;
				}
			else
				{
				if(tB1>tt1)tB1=tt1;
				}
			if(tB2==-1)
				{
				tB2=tt2;
				}
			else
				{
				if(tB2<tt2) tB2=tt2;
				}
			}
		}

	tB1-=1;
	tB2+=1;

	if( (t1<tB1)||(t1>tB2 ))
		{
		t1=tB1;
		}
	
	if( (t2<tB1)||(t2>tB2 ))
		{
		t2=tB2;
		}

	}
int CWaveformsDisplay::CalculateMWindowsSize()
	{
	RECT rc;
	int i;

	GetClientRect(&rc);
	MaxX=rc.right-1;
	MaxY=rc.bottom;
	int nWin=pScreen->GetMaxWindow();

	if(nWin==0)
		{
		nActiveScreen=-1;
		return nWin;
		}

	int yHeight=(MaxY-TimeBarYsize)/nWin;

	xX1=WindowInfoXsize;
	xX2=MaxX;

	for(int j=nWin-1; j>=0; --j)
		{
		i=pScreen->GetWaveformN(j);
		WaveformInfo[i]->xw1=0;
		WaveformInfo[i]->xw2=MaxX-1;
		WaveformInfo[i]->yw1=yHeight*j+TimeBarYsize;
		WaveformInfo[i]->yw2=(yHeight*(j+1)+TimeBarYsize)-1;

		WaveformInfo[i]->x1=xX1;
		WaveformInfo[i]->x2=xX2;

		WaveformInfo[i]->y1=WaveformInfo[i]->yw1+1;
		WaveformInfo[i]->y2=WaveformInfo[i]->yw2-1;

		}
	if(nWin-1<nActiveScreen) nActiveScreen=0;


	return nWin;
	}
void CWaveformsDisplay::ApplyScale()
	{
	switch(DisplayMode)
		{
		case mFx:
			break;
		case mAuto:
			ApplyAuto();
			break;
		case mAuto0:
			ApplyAuto0();
			break;
		case mAuto1:
			ApplyAuto1();
			break;
		case mAutoA:
			ApplyAutoA();
			break;
		}
	ApplyGain();
	}
void CWaveformsDisplay::DrawWaveforms()
	{
	CWaitCursor wc;

	ApplyScale();


    CBrush BlueBrush(RGB(0,0,192));
	
	RECT rc;
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;

	GetClientRect(&rc);
	rc.bottom-=TimeBarYsize;
	MaxX=rc.right-1;
	MaxY=rc.bottom;

	rc.right-=WindowInfoXsize;
	xcursor=-1;
	ycursor=-1;

    Bitmap1.CreateCompatibleBitmap(pDC,rc.right,rc.bottom);
    MemDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);


    CBitmap *poldBMP=MemDC.SelectObject(&Bitmap1);


	if(CalculateMWindowsSize()==0)
		{
		MemDC.FillRect(&rc, &BlueBrush);
	    MemDC.SelectObject(poldBMP);
		ScreenBitmapCriticalSection.Lock();
		if(Bitmap.m_hObject!=NULL) 
			{
			Bitmap.DeleteObject();
			}
		HGDIOBJ pBmp=Bitmap1.Detach();
		Bitmap.Attach(pBmp);
		ScreenBitmapCriticalSection.Unlock();
		DrawTimePannel(t1,t2);
		InvalidateRect(NULL,TRUE);
		return;
		}

	DrawLeftPannel();
	DrawTimePannel(t1,t2);

    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(0,255,255));

    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));


    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();


/********************************************************************/
	MemDC.SetBkMode(TRANSPARENT);

	MemDC.FillRect(&rc, &BlueBrush);
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SelectObject(&DarkPen);
	MemDC.MoveTo(0,0);		
	MemDC.LineTo(0,MaxY);



    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);

	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	Bitmap.Attach(pBmp);
	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(NULL,FALSE);



	int nMaxWin=pScreen->GetMaxWindow();
	for(int i=0; i<nMaxWin ; ++i)
		{

		int nWf=pScreen->GetWaveformN(i);

		int xw1=WaveformInfo[nWf]->xw1;
		int xw2=WaveformInfo[nWf]->xw2;
		int yw1=WaveformInfo[nWf]->yw1;
		int yw2=WaveformInfo[nWf]->yw2;

		int x1=WaveformInfo[nWf]->x1;
		int x2=WaveformInfo[nWf]->x2;
		int y1=WaveformInfo[nWf]->y1;
		int y2=WaveformInfo[nWf]->y2;


		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);

		ScreenBitmapCriticalSection.Lock();

		CDC MemDC;
		MemDC.CreateCompatibleDC(NULL);
		CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

		CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

		MemDC.SetBkMode(TRANSPARENT);
		MemDC.SetViewportOrg(-WindowInfoXsize,0);



//		DrawWindow(i, &MemDC);
		DrawWindow(i, &MemDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
//*******************************************************
		MemDC.SelectObject(CBitmapOld);
		MemDC.SelectObject(pFontOld);

		ScreenBitmapCriticalSection.Unlock();

		InvalidateRect(rc, FALSE);

		}



/********************************************************************/


	}
void CWaveformsDisplay::DrawTimePanel(CDC *pDC)
	{

/*	lstrcpy(cBuff, dttostr(t1, 1));
	CRect *rcc=new CRect(xw1,(MaxY-yw2),xw2, (MaxY-yw1));
	pDC->DrawText(cBuff,rcc,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
	delete rcc;

	lstrcpy(cBuff, dttostr(t2, 1));
	rcc=new CRect(xw1,(MaxY-yw2),xw2, (MaxY-yw1));
	pDC->DrawText(cBuff,rcc,DT_RIGHT|DT_SINGLELINE|DT_BOTTOM);
	delete rcc;
*/
	}
void CWaveformsDisplay::DrawTimeMark(int x, double tt, CDC *pDC)
	{
	int yw1=1;
	int yw2=TimeBarYsize-1;

	char cBuff1[64], cBuff2[64];

	pDC->SetTextColor(RGB(255,255,255));

	lstrcpy(cBuff1, dttostr(tt, 15));
	RECT rcc1,rcc2;
	GetTextRectangle(cBuff1,rcc1);
	int nWtd=(rcc1.right-rcc1.left)/2+1;
	int hgt=(rcc1.bottom-rcc1.top);
	rcc1.left=x-nWtd;
	rcc1.right=x+nWtd;
	rcc1.bottom+=yw1+hgt;
	rcc1.top+=yw1+hgt;

	int n1=(MaxY-rcc1.bottom);
	int n2=(MaxY-rcc1.top);
	rcc1.top=n1;
	rcc1.bottom=n2;



	lstrcpy(cBuff2, dttostr(tt, 4));
	GetTextRectangle(cBuff2,rcc2);
	nWtd=(rcc2.right-rcc2.left)/2+1;
	hgt=(rcc2.bottom-rcc2.top);
	rcc2.left=x-nWtd;
	rcc2.right=x+nWtd;
	rcc2.bottom+=yw1;
	rcc2.top+=yw1;

	n1=(MaxY-rcc2.bottom);
	n2=(MaxY-rcc2.top);
	rcc2.top=n1;
	rcc2.bottom=n2;


	if( nLastMarkRightPos<min(rcc2.left,rcc1.left))
		{
		pDC->DrawText(cBuff1,&rcc1,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
		pDC->DrawText(cBuff2,&rcc2,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
		nLastMarkRightPos=max(rcc1.right,rcc2.right);
		tmmarks.Add(x);
		}

	}

void CWaveformsDisplay::DrawWindow(int nWin, CDC *pDC, int TimeBarYsize,int x1, int x2, int y1, int y2,
   int xw1, int xw2, int yw1, int yw2, int MaxY, int MaxX, bool bPrint)
	{
	bool bfirstcount=true;
	int xxold=-1, yymin, yymax;
	int nWf=pScreen->GetWaveformN(nWin);


    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));
    CBrush BlueBrush(RGB(0,0,192));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));
	CPen *pOldPen;



	long amin=WaveformInfo[nWf]->amin;
	long amax=WaveformInfo[nWf]->amax;

	pOldPen=pDC->GetCurrentPen();


	if(!bPrint)
	if(!bClip)
		{
		CRect rc(xw1, MaxY-yw2, xw2, MaxY-yw1);
		pDC->FillRect(&rc, &BlueBrush);
		}


	int nDataBlocks=WaveformInfo[nWf]->wfdrec.GetSize();
	double ttt=-1.;
	double dSpS=WaveformInfo[nWf]->dSpS;

	if(bPrint) 
		pDC->SelectObject(&DarkPen);
	else
		pDC->SelectObject(&YellowPen);

	if(nFilterType>=0)
		{
		_open_filter((1./dSpS), nFilterType);
		}

	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=WaveformInfo[nWf]->wfdrec[j]->wfd.time;
		int nsamp=WaveformInfo[nWf]->wfdrec[j]->wfd.nsamp;
		double t2packet=t1packet+nsamp/dSpS;
		int nStartSamp,nCnts;
		double tbeg1,tend1;

		if( !( (t1>=t1packet && t1<=t2packet) || (t1<t1packet && t2>=t1packet)) ) continue;

		tbeg1=max(t1,t1packet);
		tend1=min(t2,t2packet);

		if( t1>t1packet)
			{
			nStartSamp=(int)((t1-t1packet)*dSpS);
			tbeg1=t1;
			}
		else
			{
			nStartSamp=0;
			tbeg1=t1packet;
			}

		
		if(t2>t2packet)
			{
			nCnts=(int)((t2packet-tbeg1)*dSpS);
			}
		else
			{
			nCnts=(int)((t2-tbeg1)*dSpS);
			}

		CWfdiscRecord *wfdr=WaveformInfo[nWf]->wfdrec[j];
		wfdr->OpenDataSet();
		double a;
		int nSamples=0;

		bool bContinue=true;
		while(bContinue)
			{
			if(nCnts==0)
				{
				break;
				}
			nStartSamp+=nSamples;
			nSamples=wfdr->LoadData(nStartSamp,nCnts);
			nCnts-=nSamples;
			if(nSamples==0) break;
			long *lData=wfdr->lData;



			for(int ii=0; ii<nSamples; ++ii,++lData)
				{
				a=*lData;
				if(bInvert) a=-a;
				double tt=t1packet+(1./dSpS)*(ii+nStartSamp);
				int x,y;

				if(tt<t1) continue;
				if(tt>t2)
					{
					bContinue=false;
					break;
					}

				if(nFilterType>=0)
					{
					float aaa=(float)a;
					if(bfirstcount) {_init_filter(&aaa);bfirstcount=false;}
					_do_filter(&aaa);
					a=aaa;
					}

				x=(int)((tt-t1)*(x2-x1)/(t2-t1)+x1+0.5);

				if(!bClip)
					{
					if(a>amax) a=amax;
					if(a<amin) a=amin;
					}

				y=(int)((double)((a-amin)*(y2-y1)/(amax-amin)+y1+0.5));
				y=MaxY-y;
				if(bClip)
					{
					if(y<0)y=0;
					if(y>MaxY-TimeBarYsize)y=MaxY-TimeBarYsize;
					}
				if(x==xxold)
					{
					if(yymax<y) yymax=y;
					if(yymin>y) yymin=y;
					}
				else
					{
					if(xxold>0)
						{

						pDC->LineTo(xxold,yymin);
						pDC->LineTo(xxold,yymax);

						if( (tt-ttt)>1.1/dSpS || (tt-ttt)<0)
							pDC->MoveTo(x,yymin);
						
						}
					else
						{
						pDC->MoveTo(x,y);
						}
					xxold=x;
					yymin=y;
					yymax=y;
					}

					ttt=tt;
					}
			}
		wfdr->CloseDataSet();
		}
	_close_filter();
	pDC->SelectObject(pOldPen);
	}



void CWaveformsDisplay::GetTextRectangle(LPSTR lpText, RECT &rc)
	{
	CFont *pFontOld;
	CDC *pdc=GetDC();
		
	pFontOld=pdc->SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	rc.left=0;rc.top=0;rc.right=0; rc.bottom=0;
	pdc->DrawText( lpText, -1,&rc, DT_CALCRECT|DT_SINGLELINE);
	pdc->SelectObject(pFontOld);
	ReleaseDC(pdc);
	}


void CWaveformsDisplay::RedrawPreview()
	{
	CWnd *pParent=GetParent();
	pParent->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_UPDATEVIEW,0),(LPARAM)m_hWnd);
	}
void CWaveformsDisplay::RepaintPreview()
	{
	CWnd *pParent=GetParent();
	pParent->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_REPAINTPREVIEW,0),(LPARAM)m_hWnd);
	}
void CWaveformsDisplay::OnFitToWindow()
	{
	}
void CWaveformsDisplay::OnFitToWindow(int nWindow)
	{

	int nWf=pScreen->GetWaveformN(nWindow);

	int xw1=WaveformInfo[nWf]->xw1;
	int xw2=WaveformInfo[nWf]->xw2;
	int yw1=WaveformInfo[nWf]->yw1;
	int yw2=WaveformInfo[nWf]->yw2;

	int x1=WaveformInfo[nWf]->x1;
	int x2=WaveformInfo[nWf]->x2;
	int y1=WaveformInfo[nWf]->y1;
	int y2=WaveformInfo[nWf]->y2;


	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);

	ScreenBitmapCriticalSection.Lock();

	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);


	SetFitMaxMin(nWf);

	MemDC.SetViewportOrg(-WindowInfoXsize,0);



	DrawWindow(nWindow, &MemDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(rc, FALSE);

	}
void CWaveformsDisplay::SetFitMaxMin(int nWf)
	{
	BOOL bfirstcount=TRUE;

	double dSpS=WaveformInfo[nWf]->dSpS;
	int nDataBlocks=WaveformInfo[nWf]->wfdrec.GetSize();

	long amax=WaveformInfo[nWf]->amax;
	long amin=WaveformInfo[nWf]->amin;
	GetMinMaxForTrace(nWf,amax, amin);


	WaveformInfo[nWf]->amax=amax;
	WaveformInfo[nWf]->amin=amin;

	}
void CWaveformsDisplay::OnExpandHorizontal()
	{
	if(t1<0 || t2<0 || tB1<0 || tB2<0) return;

	double tc=(t1+t2)/2.;
	double td=(t2-t1)/2.;
	td/=1.5;

	t1=tc-td;
	t2=tc+td;

	if(t1<tB1) tB1=t1;
	if(t2>tB2) tB2=t2;


	DrawWaveforms();
	RepaintPreview();

	}
void CWaveformsDisplay::OnContractHorizontal()
	{

	if(t1<0 || t2<0 || tB1<0 || tB2<0) return;

	double tc=(t1+t2)/2.;
	double td=(t2-t1)/2.;
	td*=1.5;

	t1=tc-td;
	t2=tc+td;

	if(t1<tB1) tB1=t1;
	if(t2>tB2) tB2=t2;

	DrawWaveforms();
	RepaintPreview();

	}

void CWaveformsDisplay::OnExpandVertical()
	{

	ScreenBitmapCriticalSection.Lock();


	int nScrns=pScreen->GetMaxWindow();

	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	MemDC.SetViewportOrg(-WindowInfoXsize,0);

//*******************************************************

	for(int i=0; i<nScrns; ++i)
		{
		if(!pScreen->IsSelected(i)) continue;
		int nWin=pScreen->GetWaveformN(i);
		int xw1=WaveformInfo[nWin]->xw1;
		int xw2=WaveformInfo[nWin]->xw2;
		int yw1=WaveformInfo[nWin]->yw1;
		int yw2=WaveformInfo[nWin]->yw2;


		int x1=WaveformInfo[nWin]->x1;
		int x2=WaveformInfo[nWin]->x2;
		int y1=WaveformInfo[nWin]->y1;
		int y2=WaveformInfo[nWin]->y2;



		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


		MemDC.SetBkMode(TRANSPARENT);

		long amax=WaveformInfo[nWin]->amax;
		long amin=WaveformInfo[nWin]->amin;

		long amid, adelta;

		amid=(amax+amin)/2;
		adelta=(amax-amin)/2;

		adelta=(long)((double)adelta/1.2+0.5);
		if(adelta<2) adelta=2;

		amax=amid+adelta;
		amin=amid-adelta;
	
		WaveformInfo[nWin]->amax=amax;
		WaveformInfo[nWin]->amin=amin;

//		DrawWindow(i, &MemDC);
		DrawWindow(i, &MemDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
//*******************************************************
		}

	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);


	ScreenBitmapCriticalSection.Unlock();

//	CRect rc(0,0,WindowInfoXsize, MaxY);
	InvalidateRect(NULL, FALSE);

	}
void CWaveformsDisplay::RedrawWindow(int nWindow)
	{

	if(bClip)
		{
		DrawWaveforms();
		return;
		}

	ScreenBitmapCriticalSection.Lock();
	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	int nWfm=pScreen->GetWaveformN(nWindow);

	MemDC.SetViewportOrg(-WindowInfoXsize,0);

	int xw1=WaveformInfo[nWfm]->xw1;
	int xw2=WaveformInfo[nWfm]->xw2;
	int yw1=WaveformInfo[nWfm]->yw1;
	int yw2=WaveformInfo[nWfm]->yw2;

	int x1=WaveformInfo[nWfm]->x1;
	int x2=WaveformInfo[nWfm]->x2;
	int y1=WaveformInfo[nWfm]->y1;
	int y2=WaveformInfo[nWfm]->y2;

//	DrawWindow(nWindow, &MemDC);
	DrawWindow(nWindow, &MemDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
	ScreenBitmapCriticalSection.Unlock();


//	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);
	InvalidateRect(NULL, FALSE);
	}
void CWaveformsDisplay::OnContractVertical()
	{

	if(nActiveScreen<0) return;

	ScreenBitmapCriticalSection.Lock();


	int nScrns=pScreen->GetMaxWindow();

	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	MemDC.SetViewportOrg(-WindowInfoXsize,0);

	for(int i=0; i<nScrns; ++i)
		{

		if(!pScreen->IsSelected(i)) continue;
		int nWin=pScreen->GetWaveformN(i);

		int xw1=WaveformInfo[nWin]->xw1;
		int xw2=WaveformInfo[nWin]->xw2;
		int yw1=WaveformInfo[nWin]->yw1;
		int yw2=WaveformInfo[nWin]->yw2;


		int x1=WaveformInfo[nWin]->x1;
		int x2=WaveformInfo[nWin]->x2;
		int y1=WaveformInfo[nWin]->y1;
		int y2=WaveformInfo[nWin]->y2;


		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


//*******************************************************


		MemDC.SetBkMode(TRANSPARENT);

		long amax=WaveformInfo[nWin]->amax;
		long amin=WaveformInfo[nWin]->amin;

		long amid, adelta;

		amid=(amax+amin)/2;
		adelta=(amax-amin)/2;

		adelta=(long)((double)adelta*1.2+0.5);


		amax=amid+adelta;
		amin=amid-adelta;

	
		WaveformInfo[nWin]->amax=amax;
		WaveformInfo[nWin]->amin=amin;

//		DrawWindow(i, &MemDC);
		DrawWindow(i, &MemDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX);
		}
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(NULL, FALSE);

	}
void CWaveformsDisplay::OnShiftToLeft()
	{

	if(t1<0 || t2<0 || tB1<0 || tB2<0) return;

	double tc=(t1+t2)/2.;
	double td=(t2-t1);
	double td1=td*0.2;

	t1=t1-td1;
	t2=t1+td;

	if(t1<tB1) tB1=t1;
	if(t2>tB2) tB2=t2;


	DrawWaveforms();
	RepaintPreview();

	}
void CWaveformsDisplay::OnShiftToRight()
	{

	if(t1<0 || t2<0 || tB1<0 || tB2<0) return;

	double tc=(t1+t2)/2.;
	double td=(t2-t1);
	double td1=td*0.2;

	t2=t2+td1;
	t1=t2-td;

	if(t1<tB1) tB1=t1;
	if(t2>tB2) tB2=t2;


	DrawWaveforms();
	RepaintPreview();

	}
void CWaveformsDisplay::OnShiftToDown()
	{
	if(nActiveScreen<0) return;

	ScreenBitmapCriticalSection.Lock();

	int xw1=WaveformInfo[nActiveScreen]->xw1;
	int xw2=WaveformInfo[nActiveScreen]->xw2;
	int yw1=WaveformInfo[nActiveScreen]->yw1;
	int yw2=WaveformInfo[nActiveScreen]->yw2;

/*	int x1=WaveformInfo[nWf]->x1;
	int x2=WaveformInfo[nWf]->x2;
	int y1=WaveformInfo[nWf]->y1;
	int y2=WaveformInfo[nWf]->y2;*/


	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	long amax=WaveformInfo[nActiveScreen]->amax;
	long amin=WaveformInfo[nActiveScreen]->amin;

	long amid, adelta, ad;

	amid=(amax+amin)/2;
	adelta=(amax-amin);

	ad=(long)((double)adelta*0.1+0.5);


	amin=amin-ad;
	amax=amin+adelta;
	
	WaveformInfo[nActiveScreen]->amax=amax;
	WaveformInfo[nActiveScreen]->amin=amin;
	MemDC.SetViewportOrg(-WindowInfoXsize,0);

//	DrawWindow(nActiveScreen, &MemDC);

//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(rc, FALSE);
	}
void CWaveformsDisplay::OnShiftToUp()
	{

	if(nActiveScreen<0) return;


	ScreenBitmapCriticalSection.Lock();

	int xw1=WaveformInfo[nActiveScreen]->xw1;
	int xw2=WaveformInfo[nActiveScreen]->xw2;
	int yw1=WaveformInfo[nActiveScreen]->yw1;
	int yw2=WaveformInfo[nActiveScreen]->yw2;


	int x1=WaveformInfo[nActiveScreen]->x1;
	int x2=WaveformInfo[nActiveScreen]->x2;
	int y1=WaveformInfo[nActiveScreen]->y1;
	int y2=WaveformInfo[nActiveScreen]->y2;


	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	MemDC.SetViewportOrg(-WindowInfoXsize,0);

	MemDC.SetBkMode(TRANSPARENT);

	long amax=WaveformInfo[nActiveScreen]->amax;
	long amin=WaveformInfo[nActiveScreen]->amin;

	long amid, adelta, ad;

	amid=(amax+amin)/2;
	adelta=(amax-amin);

	ad=(long)((double)adelta*0.1+0.5);


	amax=amax+ad;
	amin=amax-adelta;
	
	WaveformInfo[nActiveScreen]->amax=amax;
	WaveformInfo[nActiveScreen]->amin=amin;

//	DrawWindow(nActiveScreen, &MemDC);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(rc, FALSE);
	}
void CWaveformsDisplay::DrawShiftedScreenBmp(int delta)
	{

    CBrush BlueBrush(RGB(128,0,192));
	
    CDC MemDC;

	BITMAP B;
	offscrbmp.GetBitmap(&B);
	int yM=B.bmHeight;
	int xM=B.bmWidth;

	xcursor=-1;
	ycursor=-1;

	MemDC.CreateCompatibleDC(NULL);

	ScreenBitmapCriticalSection.Lock();

	CBitmap *CBitmapOld=MemDC.SelectObject(&offscrbmp);

	CRect rc(0,0,xM,yM);
	MemDC.FillRect(&rc, &BlueBrush);

	if(delta>=0)
		{
		DrawBitmap(&MemDC, delta, 0,&Bitmap);
		}
	else
		{
		DrawBitmap(&MemDC, 0, 0, &Bitmap, abs(delta), 0);
		}

	MemDC.SelectObject(CBitmapOld);

	ScreenBitmapCriticalSection.Unlock();


	double dttt=delta*(t2-t1)/(xX2-xX1);

	DrawTimePannel(t1-dttt,t2-dttt);

	InvalidateRect(NULL,false);
	}

void CWaveformsDisplay::MakeShiftedScreenBmp()
	{
    CBrush BlueBrush(RGB(128,0,192));
	
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;

	BITMAP B;
	Bitmap.GetBitmap(&B);
	int yM=B.bmHeight;
	int xM=B.bmWidth;

    Bitmap1.CreateCompatibleBitmap(pDC,xM,yM);
	MemDC.CreateCompatibleDC(NULL);
	ReleaseDC(pDC);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap1);

	CRect rc(0,0,xM,yM);
	MemDC.FillRect(&rc, &BlueBrush);
	DrawBitmap(&MemDC, 0, 0,&Bitmap);

	MemDC.SelectObject(CBitmapOld);

	ScreenBitmapCriticalSection.Lock();
	if(offscrbmp.m_hObject!=NULL) 
		{
		offscrbmp.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	offscrbmp.Attach(pBmp);
	ScreenBitmapCriticalSection.Unlock();
	}
void CWaveformsDisplay::OnMouseMove(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	UINT uState=GetKeyState(VK_SCROLL);
	char cBuff[64]={0};	
	int x=point.x;
	int y=MaxY-point.y;

	if(nFlags&MK_MBUTTON)
		{
		bMiddleButtonDrag=true;
		int delta=x-mbuttonx;
		DrawShiftedScreenBmp(delta);
		}

	if(bPanelSelectionActive)
		{
		int nScr=GetLeftPanelNumberFromPoint(x,y);
		if(nScr!=oldPanelAct)
			{
			if(nScr!=-1)
				{
				RECT rc;
				rc.top=0;
				rc.bottom=MaxY;
				rc.left=0;
				rc.right=WindowInfoXsize+4;
				pScreen->SelectUnselectElement(nScr);
				InvalidateRect(&rc,FALSE);
				}
			}
		oldPanelAct=nScr;
		return;
		}


	int nWindow=nWindowFromPoint(x,y);

	if(nWindow!=-1)
		{
		int nWfm=pScreen->GetWaveformN(nWindow);
		long amax=WaveformInfo[nWfm]->amax;
		long amin=WaveformInfo[nWfm]->amin;
		int y1=WaveformInfo[nWfm]->y1;
		int y2=WaveformInfo[nWfm]->y2;


		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		long aaa=(long)((double)(y-y1)*(amax-amin)/(y2-y1)+amin+0.5);

		sprintf(cBuff,"A:%d T:%s",(long)aaa, dttostr(ttt, 1));
		CMainFrame *pParent=(CMainFrame *)GetParent();

		pParent->OnSetTextToStatusbar1(cBuff);
		}

	if( (uState&0x0001) && (!(bBoxContractX || bBoxExpandX || bBoxExpandY|| bBoxContractY)))
		{
		ProcessCursorAction(x, y, nFlags);
		}
	else
		{
		if(amplinfwnd.IsWindowVisible())amplinfwnd.ShowWindow(SW_HIDE);
		if(infwnd.IsWindowVisible())infwnd.ShowWindow(SW_HIDE);
		}

	if(bBoxExpandY||bBoxContractY)
		{
		int x1=WaveformInfo[nBoxWindow]->x1;
		int x2=WaveformInfo[nBoxWindow]->x2;

		int y1;
		int y2;

		if(mouse_y_old>y)
			{
			y1=y;y2=mouse_y_old;
			}
		else
			{
			y1=mouse_y_old; y2=y;
			}

		CDC *DC=GetDC();

		CRect rect(x1,(MaxY-y1), x2,(MaxY-y2));
		DC->InvertRect(rect);
		ReleaseDC(DC);

		mouse_y_old=y;
		}

    if(bBoxExpandX || bBoxContractX)
		{
		int x1=mouse_x_old;
		int x2=x;

		int y1=0;
		int y2=MaxY;

		if(mouse_x_old>x)
			{
			x1=x;x2=mouse_x_old;
			}
		else
			{
			x1=mouse_x_old; x2=x;
			}

		CDC *DC=GetDC();

		CRect rect(x1,(MaxY-y1), x2,(MaxY-y2));
		DC->InvertRect(rect);
		ReleaseDC(DC);

		mouse_x_old=x;
		}

	CWnd::OnMouseMove(nFlags, point);
	}

void CWaveformsDisplay::OnLButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	int x=point.x;
	int y=MaxY-point.y;
	int nWindow=-1;

//
// Change Focus
//
	int nMaxWindow=pScreen->GetMaxWindow();


	int nScr=GetLeftPanelNumberFromPoint( x, y);
	if(nScr>=0)
		{
		bPanelSelectionActive=true;
		oldPanelAct=nScr;


		RECT rc;
		rc.top=0;
		rc.bottom=MaxY;
		rc.left=0;
		rc.right=WindowInfoXsize+4;
		pScreen->SelectUnselectElement(nScr);
		InvalidateRect(&rc,FALSE);
//		RedrawPreview();
		return;
		}



    if( !(bBoxContractX || bBoxExpandX || bBoxExpandY || bBoxContractY))
		{
		if(nFlags&MK_SHIFT)
			{
		    mouse_x_old=mouse_x1=mouse_x=x;

			bBoxExpandX=true;
//			nBoxWindow=i;
			}
		if(nFlags&MK_CONTROL)
			{
			nBoxWindow=GetWindowFromPoint(x, y);
			if(nBoxWindow>=0)
				{
				bBoxExpandY=true;
				mouse_y_old=mouse_y=y;
				}
			}

		}
	else
		{
		if(bBoxContractY) {ContractY(y);return;	}
		if(bBoxContractX) {ContractX(x);return;	}
		if(bBoxExpandX)	{ExpandX(x);return;	}
		if(bBoxExpandY) {ExpandY(y);return;	}
		}




	if( (WaveformInfo.GetSize()>0) && ((nFlags&MK_SHIFT)==0) && ((nFlags&MK_CONTROL)==0))
		{

		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double delta=ttt-t1;
		double dt=t2-t1;
		t2+=delta;
		t1+=delta;

		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}

	}


int CWaveformsDisplay::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	HBRUSH hBrush=(HBRUSH)GetStockObject(LTGRAY_BRUSH);
	HCURSOR hCur=(HCURSOR)::LoadCursor(NULL,IDC_ARROW);
	HICON hIco=NULL;
	LPCTSTR lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);

	RECT rcM = { 0,0,100,100 };
	if(!infwnd.CreateEx(0,lpClass,"", WS_CHILD ,rcM, this,0,NULL)) return -1;
	if(!amplinfwnd.CreateEx(0,lpClass,"", WS_CHILD ,rcM, this,0,NULL)) return -1;
	
	RECT rc;
	GetTextRectangle("WWW",rc);
	TimeBarYsize=(rc.bottom-rc.top)*2+8;
	return 0;
	}
int CWaveformsDisplay::nWindowFromPoint(int x, int y)
	{
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWindow=0; nWindow<nMaxWindow; ++nWindow)
		{
		int nWfm=pScreen->GetWaveformN(nWindow);
		int x1=WaveformInfo[nWfm]->x1;
		int x2=WaveformInfo[nWfm]->x2;
		int y1=WaveformInfo[nWfm]->y1;
		int y2=WaveformInfo[nWfm]->y2;
		if( (y<=y2) && (y>=y1) && (x>=x1) && (x<=x2)) return nWindow;
		}
	return -1;
	}
void CWaveformsDisplay::ProcessCursorAction(int x, int y, int nFlags)
	{
	if( !((x>WindowInfoXsize)&&(x<MaxX))) return;
	int nWindow=nWindowFromPoint(x,y);
	int yTextHeight;
	char cBuff[128];
	RECT rc,rc_old;
	CPen *oldPen;
	double ttt,aaa;
	CPen SelectPen(PS_DOT,1,RGB(128+64,128+64,128+64));
	CDC *DC=GetDC();
	oldPen=DC->SelectObject(&SelectPen);
	DC->SetROP2(R2_XORPEN);

	if(xcursor!=-1)
		{
		DC->MoveTo(xcursor,MaxY-TimeBarYsize-2);
		DC->LineTo(xcursor,3);
		DC->MoveTo(WindowInfoXsize+2,MaxY-ycursor);
		DC->LineTo(MaxX-2,MaxY-ycursor);
		}
	DC->MoveTo(x,MaxY-TimeBarYsize-2);
	DC->LineTo(x,3);
	DC->MoveTo(WindowInfoXsize+2,MaxY-y);
	DC->LineTo(MaxX-2,MaxY-y);

	DC->SetROP2(R2_COPYPEN);
	ReleaseDC(DC);

	xcursor=x;
	ycursor=y;
	infwnd.GetWindowRect(&rc_old);
	ScreenToClient(&rc_old);
	int x1=WindowInfoXsize+2;
	int x2=MaxX-2;
	if(MK_LBUTTON&nFlags )
		{
		ttt=(x-mouse_x)*(t2-t1)/(x2-x1);
		sprintf(cBuff,"%.3f",ttt);
		}
	else
		{
		ttt=(x-x1)*(t2-t1)/(x2-x1)+t1;
		sprintf(cBuff,"%s",dttostr(ttt, 1));
		}

	infwnd.SetText(cBuff);
	infwnd.GetWindowRect(&rc);
	ScreenToClient(&rc);
	int xTextLen=(-rc.left+rc.right);
	int xx1=x-xTextLen/2;
	int xx2=xx1+xTextLen;
	int yy1=MaxY-(rc.bottom-rc.top)-2;
	int yy2=MaxY-2;

	if(xx2>x2)
		{
		xx2=x2;
		xx1=xx2-xTextLen;
		}
	if(xx1<x1)
		{
		xx1=x1;
		xx2=xx1+xTextLen;
		}
	rc.left=xx1; rc.right=xx2; rc.top=yy1; rc.bottom=yy2;
	infwnd.MoveWindow(&rc,FALSE);

	if(infwnd.IsWindowVisible())
		{
	    InvalidateRect(&rc_old,FALSE);
		}
	else
		{
		infwnd.ShowWindow(SW_SHOW);
		}

// Filling Ampl tooltip

	if(nWindow==-1)
		{
		amplinfwnd.ShowWindow(SW_HIDE);
		return;
		}
	long amax=WaveformInfo[nWindow]->amax;
	long amin=WaveformInfo[nWindow]->amin;
	int y1=WaveformInfo[nWindow]->y1;
	int y2=WaveformInfo[nWindow]->y2;

	aaa=(long)((double)(y-y1)*(amax-amin)/(y2-y1)+amin+0.5);
	sprintf(cBuff,"%d",(long)aaa);
	
	amplinfwnd.GetWindowRect(&rc_old);
	ScreenToClient(&rc_old);
	amplinfwnd.SetText(cBuff);
	amplinfwnd.GetWindowRect(&rc);
	ScreenToClient(&rc);
	xTextLen=rc.right-rc.left;
	yTextHeight=rc.bottom-rc.top;
	xx2=WindowInfoXsize;
	xx1=xx2-xTextLen;
	yy1=MaxY-(y+yTextHeight/2);
	yy2=yy1+yTextHeight;

	rc.left=xx1; rc.right=xx2; rc.top=yy1; rc.bottom=yy2;
	
	amplinfwnd.MoveWindow(&rc,FALSE);
	if(amplinfwnd.IsWindowVisible())
		{
	    InvalidateRect(&rc_old,FALSE);
		}
	else
		{
		amplinfwnd.ShowWindow(SW_SHOW);
		}


	}

void CWaveformsDisplay::OnRButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	int x=point.x;
	int y=MaxY-point.y;
	
	if(bBoxContractY) {ContractY(y);return;	}
	if(bBoxContractX) {ContractX(x);return;	}
	if(bBoxExpandX)	{ExpandX(x);return;	}
	if(bBoxExpandY) {ExpandY(y);return;	}



	if( (WaveformInfo.GetSize()>0) && ((nFlags&MK_SHIFT)==0) && ((nFlags&MK_CONTROL)==0))
		{

		double ttt=(x-xX1)*(t2-t1)/(xX2-xX1)+t1;
		double delta=ttt-t1;
		double dt=t2-t1;

		t1-=delta;
		t2-=delta;

		if(t1<tB1) tB1=t1;
		if(t2>tB2) tB2=t2;

		DrawWaveforms();
		RedrawPreview();
		}


    if( !(bBoxContractX || bBoxExpandX || bBoxContractY || bBoxExpandY))
		{
		if(nFlags&MK_SHIFT)
			{
		    mouse_x_old=mouse_x1=mouse_x=x;
			bBoxContractX=true;
			nBoxWindow=GetWindowFromPoint(x, y);
			}
		else
		if(nFlags&MK_CONTROL)
			{
			bBoxContractY=true;
			nBoxWindow=GetWindowFromPoint(x, y);
		    mouse_y_old=mouse_y1=mouse_y=y;
			}
		}


	CWnd::OnRButtonDown(nFlags, point);
	}

void CWaveformsDisplay::OnRButtonUp(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	int x=point.x;
	int y=MaxY-point.y;
	

	CWnd::OnRButtonUp(nFlags, point);
	}
int CWaveformsDisplay::GetWindowFromPoint(int x, int y)
	{
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWin=0; nWin<nMaxWindow; ++nWin)
		{
		int nW=pScreen->GetWaveformN(nWin);
		int x1=WaveformInfo[nW]->x1;
		int x2=WaveformInfo[nW]->x2;
		int y1=WaveformInfo[nW]->y1;
		int y2=WaveformInfo[nW]->y2;

		if( y<=y2 && y>=y1 && x>=x1 && x<=x2) return nWin;
		}
	return -1;
	}
int CWaveformsDisplay::GetLeftPanelNumberFromPoint(int x, int y)
	{
	if(x>WindowInfoXsize) return -1;
	int nMaxWindow=pScreen->GetMaxWindow();
	for(int nWin=0; nWin<nMaxWindow; ++nWin)
		{
		int nW=pScreen->GetWaveformN(nWin);
		int x1=WaveformInfo[nW]->x1;
		int x2=WaveformInfo[nW]->x2;
		int y1=WaveformInfo[nW]->y1;
		int y2=WaveformInfo[nW]->y2;

		if( y<=y2 && y>=y1 && x>0 && x<=WindowInfoXsize) return nWin;
		}
	return -1;
	}
void CWaveformsDisplay::OnMButtonDown(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default

    if( bBoxContractX || bBoxExpandX || bBoxExpandY|| bBoxContractY)
		{
		bBoxContractX=false;
		bBoxExpandX=false;
		bBoxExpandY=false;
		bBoxContractY=false;
		InvalidateRect(NULL,TRUE);
		}

	SetCapture();
	mbuttonx=point.x;
	mbuttony=point.y;
	MakeShiftedScreenBmp();
	bMiddleButtonDrag=true;
	SetCursor(AfxGetApp()->LoadCursor(IDC_HMOVECURSOR));
	CWnd::OnMButtonDown(nFlags, point);
	}

void CWaveformsDisplay::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// TODO: Add your message handler code here and/or call default
	
	if(nChar==0x1b)
		{
		if( bBoxContractX || bBoxExpandX || bBoxExpandY)
			{
			bBoxContractX=false;
			bBoxExpandX=false;
			bBoxExpandY=false;
			InvalidateRect(NULL,TRUE);
			}
		}
	CWnd::OnChar(nChar, nRepCnt, nFlags);
	}

void CWaveformsDisplay::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	}
void CWaveformsDisplay::SelectTraces()
	{
	pScreen->SelectTraces();
	DrawWaveforms();
	}
void CWaveformsDisplay::Original()
	{
	pScreen->Clear();
	for(int l=0; l<WaveformInfo.GetSize(); ++l)
		{
		pScreen->AddElement(l);
		}
	DrawWaveforms();
	}
void CWaveformsDisplay::SelectAll()
	{
	pScreen->SelectAll();
	InvalidateRect(NULL,FALSE);
	}
void CWaveformsDisplay::DeleteAll()
	{
	pScreen->UnSelectAll();
	InvalidateRect(NULL,FALSE);
	}
void CWaveformsDisplay::SetAmpFlagFixed()
	{
	DisplayMode=mFx;
	}
void CWaveformsDisplay::SetAmpFlagAuto()
	{
	DisplayMode=mAuto;
	DrawWaveforms();
	}
void CWaveformsDisplay::SetAmpFlagAuto0()
	{
	DisplayMode=mAuto0;
	DrawWaveforms();
	}
void CWaveformsDisplay::SetAmpFlagAuto1()
	{
	DisplayMode=mAuto1;
	DrawWaveforms();
	}
void CWaveformsDisplay::SetAmpFlagAutoA()
	{
	DisplayMode=mAutoA;
	DrawWaveforms();
	}

void CWaveformsDisplay::SetAmpFlagInvert()
	{
	nAmpFlag=2;
	DrawWaveforms();
	}
BOOL CWaveformsDisplay::ClipOnOff()
	{
	if(bClip)
		{
		bClip=false;
		}
	else
		{
		bClip=true;
		}
	DrawWaveforms();
	return bClip;
	}

void CWaveformsDisplay::ApplyAuto()
	{
	long amax, amin;

	int nMaxWin=pScreen->GetMaxWindow();

	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		GetMinMaxForTrace(nWf, amax, amin);
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin=amin;
		}


	}

void CWaveformsDisplay::ApplyAutoA()
	{
	long amax, amin, ampl=0;

	int nMaxWin=pScreen->GetMaxWindow();

	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		GetMinMaxForTrace(nWf, amax, amin);
		ampl=max(ampl,abs(amin));
		ampl=max(ampl,abs(amax));
		}
	for(i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);
		WaveformInfo[nWf]->amax=ampl;
		WaveformInfo[nWf]->amin=-ampl;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;
		}


	}
void CWaveformsDisplay::ApplyAuto0()
	{
	long amax, amin;
	int nMaxWin=pScreen->GetMaxWindow();

	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		GetMinMaxForTrace(nWf, amax, amin);

		long ampl=max(abs(amax),abs(amin));

		WaveformInfo[nWf]->amax=ampl;
		WaveformInfo[nWf]->amin=-ampl;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;

		}


	}
void CWaveformsDisplay::ApplyAuto1()
	{
	long amax, amin;


	int nMaxWin=pScreen->GetMaxWindow();
	if(nMaxWin<1) return;
	int nWf=pScreen->GetWaveformN(0);
	GetMinMaxForTrace(nWf, amax, amin);

	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin=amin;
		WaveformInfo[nWf]->amax_1=WaveformInfo[nWf]->amax;
		WaveformInfo[nWf]->amin_1=WaveformInfo[nWf]->amin;
		}


	}
void CWaveformsDisplay::GetMinMaxForTrace(int nWf, long &amax, long &amin)
	{

	double dSpS=WaveformInfo[nWf]->dSpS;
	int nDataBlocks=WaveformInfo[nWf]->wfdrec.GetSize();
	bool bfirstcount=TRUE;
	amax=1;
	amin=-1;

	if(nFilterType>=0)
		{
		_open_filter((1./dSpS), nFilterType);
		}


	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=WaveformInfo[nWf]->wfdrec[j]->wfd.time;
		int nsamp=WaveformInfo[nWf]->wfdrec[j]->wfd.nsamp;
		double t2packet=t1packet+nsamp/dSpS;
		int nStartSamp,nCnts;
		double tbeg1,tend1;

		if( !( (t1>=t1packet && t1<=t2packet) || (t1<t1packet && t2>=t1packet)) ) continue;

		tbeg1=max(t1,t1packet);
		tend1=min(t2,t2packet);

		if( t1>t1packet)
			{
			nStartSamp=(int)((t1-t1packet)*dSpS);
			tbeg1=t1;
			}
		else
			{
			nStartSamp=0;
			tbeg1=t1packet;
			}

		
		if(t2>t2packet)
			{
			nCnts=(int)((t2packet-tbeg1)*dSpS);
			}
		else
			{
			nCnts=(int)((t2-tbeg1)*dSpS);
			}

		CWfdiscRecord *wfdr=WaveformInfo[nWf]->wfdrec[j];
		wfdr->OpenDataSet();
		long a;
		int nSamples=0;

		bool bContinue=true;
		while(bContinue)
			{
			if(nCnts==0)
				{
				break;
				}
			nStartSamp+=nSamples;
			nSamples=wfdr->LoadData(nStartSamp,nCnts);
			nCnts-=nSamples;
			if(nSamples==0) break;
			long *lData=wfdr->lData;


			for(int ii=0; ii<nSamples; ++ii,++lData)
				{
				a=*lData;
				if(bInvert) a=-a;
				double tt=t1packet+(1./dSpS)*(ii+nStartSamp);

				if(tt<t1) continue;
				if(tt>t2)
					{
					bContinue=false;
					break;
					}
				if(nFilterType>=0)
					{
					float aaa=(float)a;
					if(bfirstcount) _init_filter(&aaa);
					_do_filter(&aaa);
					a=aaa;
					}


				if(!bfirstcount)
					{
					if(a>amax) amax=a;
					if(a<amin) amin=a;
					}
				else
					{
					bfirstcount=FALSE;
					amax=a;
					amin=a;
					}

				}
			}
		wfdr->CloseDataSet();
		}
	if(nFilterType>=0) _close_filter();
	}
void CWaveformsDisplay::FlipInvertFlag()
	{
	bInvert=!bInvert;
	DrawWaveforms();
	}
void CWaveformsDisplay::ChangeGain(double nGain, int flag)
	{
	switch(flag)
		{
		case 0:
			dGain=1.;
			break;
		case 1:
			dGain*=nGain;
			break;
		default:;
						
		}

	DrawWaveforms();
	}


void CWaveformsDisplay::RunSetTimeDlg()
	{
	CSetStartTimeDlg Dlg((int)t1, (int)(t2-t1),this);
	if(Dlg.DoModal()==IDOK)
		{
		t1=Dlg.dStartTime;
		t2=Dlg.dStartTime+Dlg.dDuration;
		DrawWaveforms();
		}

	}
void CWaveformsDisplay::ApplyGain()
	{
	int nMaxWin=pScreen->GetMaxWindow();
	if(nMaxWin<1) return;
	long amax, amin, delta, mid;
	if(dGain==1.) return;
	for(int i=0; i<nMaxWin; ++i)
		{
		int nWf=pScreen->GetWaveformN(i);

		amax=WaveformInfo[nWf]->amax_1;
		amin=WaveformInfo[nWf]->amin_1;
		mid=(long)((amax+amin)/2.+0.5);
		delta=(long)((amax-amin)/2.+0.5);

		delta=(long)(delta/dGain);

		amax=mid+delta;
		amin=mid-delta;

		WaveformInfo[nWf]->amax=amax;
		WaveformInfo[nWf]->amin=amin;
		}
	}
long CWaveformsDisplay::GetDeltaInterval()
	{
	static int deltas[]={1,5,10,20,30,60,300,600,1200,1800,3600};
	float delta=(float)((t2-t1)/10.);

	for(int i=0;i<sizeof(deltas)/sizeof(int);++i)
		{
		if(delta<=deltas[i]) return deltas[i];
		}
	return ((long)delta/3600)*3600;
	}
void CWaveformsDisplay::Fit()
	{
	FindMinMaxTimeInWaveformInfo();
	DrawWaveforms();
	RedrawPreview();
	}
void CWaveformsDisplay::DeleteTraces()
	{
	pScreen->DeleteTraces();
	DrawWaveforms();
	}
void CWaveformsDisplay::DrawLeftPannelWindow(int nScreen, CDC *pDC )
	{

	RECT rcText;
	char cText[64];

    CBrush BlueBrush(RGB(0,0,192));
    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0, 192));
    CPen YellowPen(PS_SOLID,1,RGB(0,255,255));

    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));

	int nWf=pScreen->GetWaveformN(nScreen);

	int xw1=WaveformInfo[nWf]->xw1;
	int xw2=WaveformInfo[nWf]->xw2;
	int yw1=WaveformInfo[nWf]->yw1;
	int yw2=WaveformInfo[nWf]->yw2;

	int x1=WaveformInfo[nWf]->x1;
	int x2=WaveformInfo[nWf]->x2;
	int y1=WaveformInfo[nWf]->y1;
	int y2=WaveformInfo[nWf]->y2;

	long amin=WaveformInfo[nWf]->amin;
	long amax=WaveformInfo[nWf]->amax;

    CPen   *oldPen=pDC->GetCurrentPen();

	pDC->SelectObject(&BluePen);

	CRect rc(0, MaxY-yw2, xw1+WindowInfoXsize, MaxY-yw1);
	pDC->FillRect(&rc, &BlueBrush);

	pDC->SelectObject(&DarkPen);


//	pDC->MoveTo(xw1+WindowInfoXsize,MaxY-yw1);		
//	pDC->LineTo(xw1+WindowInfoXsize,MaxY-yw2);
	pDC->SelectObject(&DarkPen);

	CRect rc1(xw1, MaxY-yw2, xw1+WindowInfoXsize-1, MaxY-yw1);
	pDC->FillRect(&rc1, &GrayBrush);



	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));		
	pDC->LineTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));

	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));
	pDC->LineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw1+8));

	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));
	pDC->LineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw2-8));

	pDC->SetTextColor(RGB(0,0,0));

	sprintf(cText,"%d",amax);
	CRect rcc1(xw1,MaxY-(yw2-8),xw1+WindowInfoXsize-8-2, MaxY-(yw2-8-13));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(cText,rcc1,DT_RIGHT);

	sprintf(cText,"%d",amin);
	GetTextRectangle(cText, rcText);

	CRect rcc2(xw1,MaxY-(yw1+8+13),xw1+WindowInfoXsize-8-2, MaxY-(yw1-8));
	pDC->DrawText(cText,rcc2,DT_RIGHT);

	CRect rcc3(xw1+5,(MaxY-yw2),xw1+WindowInfoXsize-8-2, (MaxY-yw1));
	sprintf(cText,"%s::%s",WaveformInfo[nWf]->StaName, WaveformInfo[nWf]->ChanName);
	pDC->DrawText(cText,rcc3,	DT_VCENTER|DT_SINGLELINE );


    pDC->SelectObject(oldPen);
	}
void CWaveformsDisplay::DrawLeftPannel()
	{
	CWaitCursor wc;

	
	RECT rc;
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;
    CBrush BlueBrush(RGB(0,0,192));


    Bitmap1.CreateCompatibleBitmap(pDC, WindowInfoXsize,MaxY);
    MemDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

    CBitmap *poldBMP=MemDC.SelectObject(&Bitmap1);


    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(0,255,255));

    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));


    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();


/********************************************************************/
	MemDC.SetBkMode(TRANSPARENT);

	rc.top=0;
	rc.bottom=MaxY;
	rc.left=0;
	rc.right=WindowInfoXsize;
	MemDC.FillRect(&rc, &BlueBrush);
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));


	int nMaxWin=pScreen->GetMaxWindow();
	for(int i=0; i<nMaxWin ; ++i)
		{
		DrawLeftPannelWindow(i,&MemDC);
		}

    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);


	ScreenBitmapCriticalSection.Lock();
	if(leftPannel.m_hObject!=NULL) 
		{
		leftPannel.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	leftPannel.Attach(pBmp);
	ScreenBitmapCriticalSection.Unlock();
	}
void CWaveformsDisplay::DrawTimeMarks(CDC *pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY)
	{
	int x;
	CBrush brush(RGB(255,0,0));

	int delta=GetDeltaInterval();
	if(delta==0) delta=1;

	nLastMarkRightPos=-1;
	long ttstart=(t1/3600)*3600;



	tmmarks.RemoveAll();

	for(long tt=ttstart; tt<t2; tt++)
		{
		if(tt<t1) continue;
		if(((int)tt)%delta==0)
			{
			x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);
			{
			char cBuff1[64], cBuff2[64];

			pDC->SetTextColor(RGB(255,255,255));

			lstrcpy(cBuff1, dttostr(tt, 15));
			RECT rcc1,rcc2;
			GetTextRectangle(cBuff1,rcc1);
			int nWtd=(rcc1.right-rcc1.left)/2+1;
			int hgt1=(rcc1.bottom-rcc1.top);
			rcc1.left=x-nWtd;
			rcc1.right=x+nWtd;
			rcc1.top=yw2-1;
			rcc1.bottom=rcc1.top-hgt1;

			int n1=(MaxY-rcc1.bottom);
			int n2=(MaxY-rcc1.top);
			rcc1.top=n2;
			rcc1.bottom=n1;

			lstrcpy(cBuff2, dttostr(tt, 4));

			GetTextRectangle(cBuff2,rcc2);

			nWtd=(rcc2.right-rcc2.left)/2+1;
			int hgt2=(rcc2.bottom-rcc2.top);
			rcc2.left=x-nWtd;
			rcc2.right=x+nWtd;
			rcc2.top=yw2-2-hgt1;
			rcc2.bottom=rcc2.top-hgt2;

			n1=(MaxY-rcc2.bottom);
			n2=(MaxY-rcc2.top);
			rcc2.top=n2;
			rcc2.bottom=n1;


			if( nLastMarkRightPos<min(rcc2.left,rcc1.left))
				{
				pDC->DrawText(cBuff1,&rcc1,DT_CENTER|DT_WORD_ELLIPSIS|DT_NOCLIP);
				pDC->DrawText(cBuff2,&rcc2,DT_CENTER|DT_WORD_ELLIPSIS|DT_NOCLIP);
//				pDC->FillRect(&rcc1, &brush);
				nLastMarkRightPos=max(rcc1.right,rcc2.right);
				tmmarks.Add(x+WindowInfoXsize);
				}

			}

			}


		}
	}
void CWaveformsDisplay::DrawTimePannel(double t1, double t2)
	{

	int MaxY=TimeBarYsize;
	RECT rc;
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;
    CBrush BlueBrush(RGB(0,0,192));

    Bitmap1.CreateCompatibleBitmap(pDC, MaxX-WindowInfoXsize,TimeBarYsize);
    MemDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

    CBitmap *poldBMP=MemDC.SelectObject(&Bitmap1);

//	MemDC.SetViewportOrg(-WindowInfoXsize,0);


    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();


    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	MemDC.SetBkMode(TRANSPARENT);

	MemDC.SetTextColor(RGB(255,255,255));


	int xw1=0;
	int xw2=MaxX-WindowInfoXsize-1;
	int yw1=0;
	int yw2=TimeBarYsize-1;
	int x, x_old;

	rc.top=0;
	rc.bottom=TimeBarYsize-1;
	rc.left=0;
	rc.right=MaxX-WindowInfoXsize-1;

	MemDC.FillRect(&rc, &BlueBrush);

	MemDC.SelectObject(&WhitePen);

	MemDC.MoveTo(xw1,MaxY-yw2);		
	MemDC.LineTo(xw2,MaxY-yw2);

	MemDC.MoveTo(xw1,MaxY-yw2);		
	MemDC.LineTo(xw1,MaxY-yw1);

	MemDC.SelectObject(&DarkPen);

	MemDC.MoveTo(xw2,MaxY-yw1);		
	MemDC.LineTo(xw2,MaxY-yw2);

	MemDC.MoveTo(xw2,MaxY-yw1);		
	MemDC.LineTo(xw1,MaxY-yw1);

	x_old=-1;

	int imode;

	x=(int)((xw2-xw1)/(t2-t1));

	if(x<3)
		{
		x=(int)(60*(xw2-xw1)/(t2-t1));
		if(x==0)
			{
			imode=2;
			}
		else
			{
			imode=1;
			}
		}
	else
		{
		imode=0;
		}

	DrawTimeMarks(&MemDC, t1, t2, xw1, xw2, yw1, yw2, MaxY);

    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);

	ScreenBitmapCriticalSection.Lock();
	if(timePanel.m_hObject!=NULL) 
		{
		timePanel.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	timePanel.Attach(pBmp);
	ScreenBitmapCriticalSection.Unlock();
	}

void CWaveformsDisplay::OnMButtonUp(UINT nFlags, CPoint point) 
	{
	if(bMiddleButtonDrag)
		{
		SetCursor(::LoadCursor(NULL,IDC_ARROW));
		bMiddleButtonDrag=false;
		int x=point.x;
		int x1=WindowInfoXsize+2;
		int x2=MaxX-2;

		double dttt=(x-mbuttonx)*(t2-t1)/(x2-x1);
		t1-=dttt;
		t2-=dttt;
		if(dttt!=0.)
			{
			DrawWaveforms();
			RedrawPreview();
			ReleaseCapture();
			}
		else
			{
			InvalidateRect(NULL,false);
			}
		}
	CWnd::OnMButtonUp(nFlags, point);
	}
void CWaveformsDisplay::SetFilterNumber(int nFilt)
	{
	nFilterType=nFilt;
	DrawWaveforms();
	}
void CWaveformsDisplay::Print()
	{
	CWaveformsPrint PRN(this);
	PRN.Print();
	}
