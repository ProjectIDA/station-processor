// DBPickView.cpp : implementation file
//

#include "stdafx.h"
#include "crtd.h"
#include "DBPickView.h"
#include "TimeFunc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBPickView

IMPLEMENT_DYNCREATE(CDBPickView, CFrameWnd)


int CDBPickView::WindowInfoXsize=100;
int CDBPickView::TimeBarYsize=30;

CDBPickView::CDBPickView()
	{
	}

CDBPickView::CDBPickView(CRTDisplay *pRTD)
	{
	this->pRTDisplay=pRTD;
	nActiveScreen=-1;
	}

CDBPickView::~CDBPickView()
	{
// Clear all WaveformInfo
	ClearWaveformInfo();
	}

void CDBPickView::ClearWaveformInfo()
	{
	while(WaveformInfo.GetSize()>0)
		{
		CMWindowX *wfi=WaveformInfo[0];
		WaveformInfo.RemoveAt(0);
		delete wfi;
		}
	}

void CDBPickView::AddWindow(CString Sta, CString Chan, double dSpS)
	{
	CMWindowX *wfi=new CMWindowX;
	wfi->StaName=Sta;
	wfi->ChanName=Chan;
	wfi->dSpS=dSpS;
	WaveformInfo.Add(wfi);
	}
void CDBPickView::RemoveWindow(CString Sta, CString Chan)
	{
	CMWindowX *wfi;
	for(int i=0; i<WaveformInfo.GetSize(); ++i)
		{
		wfi=WaveformInfo[i];
		if( (Sta.CompareNoCase(wfi->StaName)==0) && (wfi->ChanName==0) )
			{
			WaveformInfo.RemoveAt(i);
			delete wfi;
			return;
			}
		}

	}





BEGIN_MESSAGE_MAP(CDBPickView, CFrameWnd)
	//{{AFX_MSG_MAP(CDBPickView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBPickView message handlers


void CDBPickView::OnPaint() 
	{
	CPaintDC dc(this); // device context for painting
	void DrawBitmap(CDC *DC, int x, int y, CBitmap *Bitmap);
	
	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL)
		{
		DrawBitmap(&dc, 0, 0,&Bitmap);
		DrawFocused(&dc);
		}
	ScreenBitmapCriticalSection.Unlock();
	}
void CDBPickView::Run()
	{
	ClearWaveformInfo();
	nLastMarkRightPos=0;
	int nElem=this->pRTDisplay->WaveformInfo.GetSize();
	for(int i=0; i<nElem; ++i)
		{
		CMWindow *pMWin=pRTDisplay->WaveformInfo[i];
		CString sSta=pMWin->StaName;
		CString sChan=pMWin->ChanName;
		double dSpS=pMWin->dSpS;
		AddWindow(sSta, sChan,dSpS);
		}
	t1=this->pRTDisplay->t1;
	t2=this->pRTDisplay->t2;
	nActiveScreen=0;
	CalculateWindowsSize();
	DrawWaveforms();
	}

void CDBPickView::Stop()
	{
	ScreenBitmapCriticalSection.Lock();
	ClearWaveformInfo();
	ScreenBitmapCriticalSection.Unlock();
	ClearDisplay();
	}

void CDBPickView::CalculateWindowsSize()
	{
	RECT rc;

	GetClientRect(&rc);
	MaxX=rc.right;
	MaxY=rc.bottom;
	int nWin=WaveformInfo.GetSize();
	if(nWin==0) return;
	int yHeight=(MaxY-TimeBarYsize)/nWin;

	for(int i=0; i<nWin; ++i)
		{
		WaveformInfo[i]->xw1=2;
		WaveformInfo[i]->xw2=MaxX-1;
		WaveformInfo[i]->yw1=yHeight*i+TimeBarYsize;
		WaveformInfo[i]->yw2=(yHeight*(i+1)+TimeBarYsize)-1;

		WaveformInfo[i]->x1=WaveformInfo[i]->xw1+WindowInfoXsize+2;
		WaveformInfo[i]->x2=WaveformInfo[i]->xw2-1;

		WaveformInfo[i]->y1=WaveformInfo[i]->yw1+1;
		WaveformInfo[i]->y2=WaveformInfo[i]->yw2-1;

		}

	}

void CDBPickView::OnSize(UINT nType, int cx, int cy) 
	{
	CWnd::OnSize(nType, cx, cy);
	DrawWaveforms();
	// TODO: Add your message handler code here
	}
void CDBPickView::DrawWaveforms()
	{
	CWaitCursor wc;

	this->pRTDisplay->Critical.Lock();

	RECT rc;
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;

	CalculateWindowsSize();


	rc.left=rc.top=0;
	rc.right=MaxX;
	rc.bottom=MaxY;

    Bitmap1.CreateCompatibleBitmap(pDC,rc.right,rc.bottom);
    MemDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

    CBitmap *poldBMP=MemDC.SelectObject(&Bitmap1);

    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));
    CBrush BlueBrush(RGB(0,0,128));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));

    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));


    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();


	MemDC.SetBkMode(TRANSPARENT);

	MemDC.FillRect(&rc, &BlueBrush);
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SelectObject(&DarkPen);
	MemDC.MoveTo(0,0);		
	MemDC.LineTo(0,MaxY);

	DrawTimePanel(&MemDC);

	for(int i=0; i<WaveformInfo.GetSize(); ++i)
		{
		DrawWindow(i,&MemDC);
		}



    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);
	MemDC.DeleteDC();
	this->pRTDisplay->Critical.Unlock();

	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	Bitmap.Attach(pBmp);
	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(NULL,FALSE);

	}
void CDBPickView::DrawFocused(CDC *pDC)
	{
	if(nActiveScreen==-1) return;

	int xw1=WaveformInfo[nActiveScreen]->xw1+4;
	int xw2=WaveformInfo[nActiveScreen]->xw1+WindowInfoXsize-4;
	int yw1=WaveformInfo[nActiveScreen]->yw1+4;
	int yw2=WaveformInfo[nActiveScreen]->yw2-4;


    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
    CPen GrayPen(PS_SOLID,1,RGB(128,128,128));

    CPen   *oldPen=pDC->GetCurrentPen();

	pDC->SelectObject(&GrayPen);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw1,MaxY-yw1);

	pDC->SelectObject(&WhitePen);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw2+1,MaxY-yw1);		
	pDC->LineTo(xw2+1,MaxY-yw2);


	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw1,MaxY-yw1);

	pDC->MoveTo(xw2,MaxY-yw1+1);		
	pDC->LineTo(xw1,MaxY-yw1+1);


    pDC->SelectObject(oldPen);

	}

void CDBPickView::OnLButtonUp(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	
	int x=point.x;
	int y=MaxY-point.y;
	int i;

//
// Change Focus
//
	if(x<WindowInfoXsize)
	for(i=0; i<WaveformInfo.GetSize(); ++i)
		{
		int xw1=WaveformInfo[i]->xw1;
		int xw2=WaveformInfo[i]->xw2;
		int yw1=WaveformInfo[i]->yw1;
		int yw2=WaveformInfo[i]->yw2;

		if( y<=yw2 && y>=yw1)
			{
			nActiveScreen=i;
			RECT rc;
			
			rc.top=0;
			rc.bottom=MaxY;
			rc.left=0;
			rc.right=WindowInfoXsize+4;
			InvalidateRect(&rc,FALSE);

//   Update Preview Window

//			CWnd *pParent=GetParent();
//			pParent->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_UPDATEVIEW,0),(LPARAM)m_hWnd);
			CFrameWnd::OnLButtonUp(nFlags, point);
			return;
			}
		
		}
	int x1=WaveformInfo[0]->x1;
	int x2=WaveformInfo[0]->x2;

	double ttt=(x-x1)*(t2-t1)/(x2-x1)+t1;
	double delta=t1-ttt;
	t1-=delta;
	t2-=delta;
	DrawWaveforms();

	CFrameWnd::OnLButtonUp(nFlags, point);
	}


void CDBPickView::GetTextRectangle(LPSTR lpText, RECT &rc)
	{
	CFont *pFontOld;
	CDC *pdc=GetDC();
		
	pFontOld=pdc->SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	rc.left=0;rc.top=0;rc.right=0; rc.bottom=0;
	pdc->DrawText( lpText, -1,&rc, DT_CALCRECT|DT_LEFT);
	pdc->SelectObject(pFontOld);
	ReleaseDC(pdc);


	}
void CDBPickView::DrawTimePanel(CDC *pDC)
	{

	char cBuff[64];
    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
	CPen RedPen(PS_SOLID,1,RGB(255,0,0));
	
	int xw1=WindowInfoXsize+3;
	int xw2=MaxX-1;
	int yw1=1;
	int yw2=TimeBarYsize-1;
	int x, x_old;


	CPen   *oldPen=pDC->GetCurrentPen();
	pDC->SelectObject(&WhitePen);


	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw1,MaxY-yw1);

	pDC->SelectObject(&DarkPen);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw1,MaxY-yw1);

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
	nLastMarkRightPos=-100;
	for(long tt=(long)t1; tt<t2; ++tt)
		{
		x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);

		if(tt%3600==0)
			{
			pDC->SelectObject(&WhitePen);
			pDC->MoveTo(x,MaxY-yw2);		
			pDC->LineTo(x,MaxY-(yw2-12));
			pDC->SelectObject(&DarkPen);
			pDC->MoveTo(x+1,MaxY-yw2);		
			pDC->LineTo(x+1,MaxY-(yw2-12));
			x_old=x;


			lstrcpy(cBuff, dttostr(tt, 1));
			RECT rcc;
			GetTextRectangle(cBuff,rcc);
			int nWtd=(rcc.right-rcc.left)/2+1;
			rcc.left=x-nWtd;
			rcc.right=x+nWtd;
			rcc.bottom=(MaxY-yw1);
			rcc.top=(MaxY-yw2);
			if(nLastMarkRightPos<rcc.left)
				{
				pDC->DrawText(cBuff,&rcc,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
				nLastMarkRightPos=rcc.right;
				}

			continue;
			}


		
		if(imode<2)
		if(tt%60==0)
			{
			pDC->SelectObject(&WhitePen);
			pDC->MoveTo(x,MaxY-yw2);		
			pDC->LineTo(x,MaxY-(yw2-8));
			pDC->SelectObject(&DarkPen);
			pDC->MoveTo(x+1,MaxY-yw2);		
			pDC->LineTo(x+1,MaxY-(yw2-8));
			x_old=x;

			pDC->SetTextColor(RGB(255,255,255));

			lstrcpy(cBuff, dttostr(tt, 1));
			RECT rcc;
			GetTextRectangle(cBuff,rcc);
			int nWtd=(rcc.right-rcc.left)/2+1;
			rcc.left=x-nWtd;
			rcc.right=x+nWtd;
			rcc.bottom=(MaxY-yw1);
			rcc.top=(MaxY-yw2);
			if(nLastMarkRightPos<rcc.left)
				{
				pDC->DrawText(cBuff,&rcc,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
				nLastMarkRightPos=rcc.right;
				}
			continue;
			}

		if(imode==0)
			{
			pDC->SelectObject(&WhitePen);
			pDC->MoveTo(x,MaxY-yw2);		
			pDC->LineTo(x,MaxY-(yw2-4));
			pDC->SelectObject(&DarkPen);
			pDC->MoveTo(x+1,MaxY-yw2);		
			pDC->LineTo(x+1,MaxY-(yw2-4));
			x_old=x;
			}

		}

/*	pDC->SetTextColor(RGB(255,255,255));

	lstrcpy(cBuff, dttostr(t1, 1));
	CRect *rcc=new CRect(xw1,(MaxY-yw2),xw2, (MaxY-yw1));
	pDC->DrawText(cBuff,rcc,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
	delete rcc;

	lstrcpy(cBuff, dttostr(t2, 1));
	rcc=new CRect(xw1,(MaxY-yw2),xw2, (MaxY-yw1));
	pDC->DrawText(cBuff,rcc,DT_RIGHT|DT_SINGLELINE|DT_BOTTOM);
	delete rcc;*/

    pDC->SelectObject(oldPen);

	}
void CDBPickView::OnAutoScale()
	{


	ScreenBitmapCriticalSection.Lock();
	this->pRTDisplay->Critical.Lock();


	SHORT sState=GetAsyncKeyState(VK_SHIFT)&0x0001;
  



	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	if(sState==0)
		{
		ApplyAutoscale(nActiveScreen);
		DrawWindow(nActiveScreen, &MemDC);
		}
	else
		{
		for(int i=0; i<WaveformInfo.GetSize(); ++i)
			{
			ApplyAutoscale(i);
			DrawWindow(i, &MemDC);
			}
		}


/*	BOOL bfirstcount=TRUE;

	double dSpS=WaveformInfo[nActiveScreen]->dSpS;
	int nXferBlocks=WaveformInfo[nActiveScreen]->xferData.GetSize();

	long amax=WaveformInfo[nActiveScreen]->amax;
	long amin=WaveformInfo[nActiveScreen]->amin;

	
	for(int j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=WaveformInfo[nActiveScreen]->xferData[j];
		double t1packet=WaveformInfo[nActiveScreen]->xferData[j]->beg;
		double t2packet=WaveformInfo[nActiveScreen]->xferData[j]->end;
		int nsamp=WaveformInfo[nActiveScreen]->xferData[j]->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=WaveformInfo[nActiveScreen]->xferData[j]->data[k];
			double tt=t1packet+(1./dSpS)*k;

			if(tt<t1) continue;
			if(tt>t2) break;
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

	WaveformInfo[nActiveScreen]->amax=amax;
	WaveformInfo[nActiveScreen]->amin=amin;*/

//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	this->pRTDisplay->Critical.Unlock();
	ScreenBitmapCriticalSection.Unlock();

	if(sState==0)
		{
	
		int xw1=WaveformInfo[nActiveScreen]->xw1;
		int xw2=WaveformInfo[nActiveScreen]->xw2;
		int yw1=WaveformInfo[nActiveScreen]->yw1;
		int yw2=WaveformInfo[nActiveScreen]->yw2;

		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);

		InvalidateRect(rc, FALSE);
		}
	else
		{
		InvalidateRect(NULL, FALSE);
		}

	}
void CDBPickView::ApplyAutoscale(int nWindow)
	{
	BOOL bfirstcount=TRUE;
	double dSpS=WaveformInfo[nWindow]->dSpS;
	int nXferBlocks=pRTDisplay->WaveformInfo[nWindow]->xferData.GetSize();


	long amax=WaveformInfo[nWindow]->amax;
	long amin=WaveformInfo[nWindow]->amin;

	
	for(int j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j];
		double t1packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->beg;
		double t2packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->end;
		int nsamp=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->data[k];
			double tt=t1packet+(1./dSpS)*k;

			if(tt<t1) continue;
			if(tt>t2) break;
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

	WaveformInfo[nWindow]->amax=amax;
	WaveformInfo[nWindow]->amin=amin;
	}

void CDBPickView::DrawWindow(int nWindow, CDC *pDC)
	{

    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));
    CBrush BlueBrush(RGB(0,0,128));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));


	int xw1=WaveformInfo[nWindow]->xw1;
	int xw2=WaveformInfo[nWindow]->xw2;
	int yw1=WaveformInfo[nWindow]->yw1;
	int yw2=WaveformInfo[nWindow]->yw2;

	int x1=WaveformInfo[nWindow]->x1;
	int x2=WaveformInfo[nWindow]->x2;
	int y1=WaveformInfo[nWindow]->y1;
	int y2=WaveformInfo[nWindow]->y2;



	long amin=WaveformInfo[nWindow]->amin;
	long amax=WaveformInfo[nWindow]->amax;



    CPen   *oldPen=pDC->GetCurrentPen();
    CFont  *oldFont=pDC->GetCurrentFont();
    CBrush *oldBrush=pDC->GetCurrentBrush();



	CRect rc(xw1, MaxY-yw2, xw2, MaxY-yw1);
	pDC->FillRect(&rc, &BlueBrush);

	pDC->SelectObject(&DarkPen);

//***********************************************************

	pDC->SelectObject(&WhitePen);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw1,MaxY-yw2);		
	pDC->LineTo(xw1,MaxY-yw1);

	pDC->MoveTo(xw1+1,MaxY-yw2);		
	pDC->LineTo(xw1+1,MaxY-yw1);


	pDC->SelectObject(&DarkPen);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw2,MaxY-yw2);

	pDC->MoveTo(xw2,MaxY-yw1);		
	pDC->LineTo(xw1,MaxY-yw1);


	pDC->SelectObject(&DarkPen);

	pDC->MoveTo(xw1+WindowInfoXsize,MaxY-yw1);		
	pDC->LineTo(xw1+WindowInfoXsize,MaxY-yw2);

	pDC->SelectObject(&WhitePen);

	pDC->MoveTo(xw1+WindowInfoXsize+1,MaxY-yw1);		
	pDC->LineTo(xw1+WindowInfoXsize+1,MaxY-yw2);

	pDC->MoveTo(xw1+WindowInfoXsize+2,MaxY-yw1);		
	pDC->LineTo(xw1+WindowInfoXsize+2,MaxY-yw2);

//*************************************************************
//      Draw Ampl (Y) Axis
//

	CRect rc1(xw1, MaxY-yw2, xw1+WindowInfoXsize-1, MaxY-yw1);
	pDC->FillRect(&rc1, &GrayBrush);


	pDC->SelectObject(&DarkPen);
	pDC->SetTextColor(RGB(0,0,0));

	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));		
	pDC->LineTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));

	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));
	pDC->LineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw1+8));

	pDC->MoveTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));
	pDC->LineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw2-8));

	pDC->SelectObject(&WhitePen);

	{
	RECT rcText;
	char cText[64];
	sprintf(cText,"%d",amax);

	CRect rcc1(xw1,MaxY-(yw2-8),xw1+WindowInfoXsize-8-2, MaxY-(yw2-8-13));

	pDC->DrawText(cText,rcc1,DT_RIGHT);

	sprintf(cText,"%d",amin);
	GetTextRectangle(cText, rcText);

	CRect rcc2(xw1,MaxY-(yw1+8+13),xw1+WindowInfoXsize-8-2, MaxY-(yw1-8));
	pDC->DrawText(cText,rcc2,DT_RIGHT);

	CRect rcc3(xw1+5,(MaxY-yw2),xw1+WindowInfoXsize-8-2, (MaxY-yw1));
	sprintf(cText,"%s::%s",WaveformInfo[nWindow]->StaName, WaveformInfo[nWindow]->ChanName);
	pDC->DrawText(cText,rcc3,	DT_VCENTER|DT_SINGLELINE );

	}

//**************************************************************

	int nXferBlocks=this->pRTDisplay->WaveformInfo[nWindow]->xferData.GetSize();
	double ttt=-1.;
	double dSpS=WaveformInfo[nWindow]->dSpS;

	pDC->SelectObject(&YellowPen);

	for(int j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j];
		double t1packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->beg;
		double t2packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->end;
		int nsamp=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->nsamp;


		for(int k=0; k<nsamp; ++k)
			{
			long a=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->data[k];
			double tt=t1packet+(1./dSpS)*k;
			int x,y;

			if(tt<t1) continue;
			if(tt>t2) break;

			x=(int)((tt-t1)*(x2-x1)/(t2-t1)+x1);

			if(a>amax) a=amax;
			if(a<amin) a=amin;

			y=(a-amin)*(y2-y1)/(amax-amin)+y1;
			y=MaxY-y;
			if(tt-ttt>1.1/dSpS)
				{
				pDC->MoveTo(x,y);		
				}
			else
				{
				pDC->LineTo(x,y);		
				}
			ttt=tt;
			}
		}
	DrawGaps(nWindow,pDC);
    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldFont);

	}
void CDBPickView::DrawGaps(int nWindow, CDC *pDC)
	{
    CBrush LightBlueBrush(RGB(0,0,192));

    CPen   *oldPen=pDC->GetCurrentPen();
    CFont  *oldFont=pDC->GetCurrentFont();
    CBrush *oldBrush=pDC->GetCurrentBrush();

	int xw1=WaveformInfo[nWindow]->xw1;
	int xw2=WaveformInfo[nWindow]->xw2;
	int yw1=WaveformInfo[nWindow]->yw1;
	int yw2=WaveformInfo[nWindow]->yw2;

	int x1=WaveformInfo[nWindow]->x1;
	int x2=WaveformInfo[nWindow]->x2;
	int y1=WaveformInfo[nWindow]->y1;
	int y2=WaveformInfo[nWindow]->y2;


	int nXferBlocks=this->pRTDisplay->WaveformInfo[nWindow]->xferData.GetSize();
	double ttt=-1.;
	double dSpS=WaveformInfo[nWindow]->dSpS;


	double tend=t1;
	for(int j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j];
		double t1packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->beg;
		double t2packet=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->end;
		int nsamp=this->pRTDisplay->WaveformInfo[nWindow]->xferData[j]->nsamp;
		if(tend>t2packet) continue;
		if(tend>t1packet)
			{
			tend=t2packet;
			continue;
			}

		if((fabs(tend-t1packet)>1/dSpS) && (tend!=-1))
			{
			int xx1=(int)((tend-t1)*(x2-x1)/(t2-t1)+x1);
			int xx2=(int)((t1packet-t1)*(x2-x1)/(t2-t1)+x1);
			CRect rc(xx1,(MaxY-y1),xx2,(MaxY-y2));
			pDC->FillRect(&rc, &LightBlueBrush);
			}
		tend=t1packet+nsamp/dSpS;

		}
	if(fabs(tend-t2)>1/dSpS)
		{
		int xx1=(int)((tend-t1)*(x2-x1)/(t2-t1)+x1);
		int xx2=x2;
		CRect rc(xx1,(MaxY-y1),xx2,(MaxY-y2));
		pDC->FillRect(&rc, &LightBlueBrush);
		}

    pDC->SelectObject(oldPen);
    pDC->SelectObject(oldBrush);
    pDC->SelectObject(oldFont);

	}

void CDBPickView::OnExpandVert()
	{
	ScreenBitmapCriticalSection.Lock();
	this->pRTDisplay->Critical.Lock();

	SHORT sState=GetAsyncKeyState(VK_SHIFT)&0x0001;


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	if(sState==0)
		{
		ApplyExpand(nActiveScreen);
		DrawWindow(nActiveScreen, &MemDC);
		}
	else
		{
		for(int i=0; i<WaveformInfo.GetSize(); ++i)
			{
			ApplyExpand(i);
			DrawWindow(i, &MemDC);
			}
		}





	DrawWindow(nActiveScreen, &MemDC);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	this->pRTDisplay->Critical.Unlock();
	ScreenBitmapCriticalSection.Unlock();


	if(sState==0)
		{
	
		int xw1=WaveformInfo[nActiveScreen]->xw1;
		int xw2=WaveformInfo[nActiveScreen]->xw2;
		int yw1=WaveformInfo[nActiveScreen]->yw1;
		int yw2=WaveformInfo[nActiveScreen]->yw2;

		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);

		InvalidateRect(rc, FALSE);
		}
	else
		{
		InvalidateRect(NULL, FALSE);
		}


	}

void CDBPickView::ApplyExpand(int nWindow)
	{


	long amax=WaveformInfo[nWindow]->amax;
	long amin=WaveformInfo[nWindow]->amin;

	long amid, adelta;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;

	adelta/=2;

	amax=amid+adelta;
	amin=amid-adelta;

	
	WaveformInfo[nWindow]->amax=amax;
	WaveformInfo[nWindow]->amin=amin;

	}

void CDBPickView::OnContractVert()
	{
	ScreenBitmapCriticalSection.Lock();
	this->pRTDisplay->Critical.Lock();

	SHORT sState=GetAsyncKeyState(VK_SHIFT)&0x0001;


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	if(sState==0)
		{
		ApplyContract(nActiveScreen);
		DrawWindow(nActiveScreen, &MemDC);
		}
	else
		{
		for(int i=0; i<WaveformInfo.GetSize(); ++i)
			{
			ApplyContract(i);
			DrawWindow(i, &MemDC);
			}
		}





	DrawWindow(nActiveScreen, &MemDC);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	this->pRTDisplay->Critical.Unlock();
	ScreenBitmapCriticalSection.Unlock();


	if(sState==0)
		{
	
		int xw1=WaveformInfo[nActiveScreen]->xw1;
		int xw2=WaveformInfo[nActiveScreen]->xw2;
		int yw1=WaveformInfo[nActiveScreen]->yw1;
		int yw2=WaveformInfo[nActiveScreen]->yw2;

		CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);

		InvalidateRect(rc, FALSE);
		}
	else
		{
		InvalidateRect(NULL, FALSE);
		}

	}

void CDBPickView::ApplyContract(int nWindow)
	{
	BOOL bfirstcount=TRUE;

	double dSpS=WaveformInfo[nWindow]->dSpS;

	long amax=WaveformInfo[nWindow]->amax;
	long amin=WaveformInfo[nWindow]->amin;

	long amid, adelta;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;

	adelta*=2;

	if(adelta==0) adelta=1;

	amax=amid+adelta;
	amin=amid-adelta;

	
	WaveformInfo[nWindow]->amax=amax;
	WaveformInfo[nWindow]->amin=amin;

	}
void CDBPickView::OnShiftUp()
	{
	ScreenBitmapCriticalSection.Lock();
	this->pRTDisplay->Critical.Lock();

	int xw1=WaveformInfo[nActiveScreen]->xw1;
	int xw2=WaveformInfo[nActiveScreen]->xw2;
	int yw1=WaveformInfo[nActiveScreen]->yw1;
	int yw2=WaveformInfo[nActiveScreen]->yw2;

	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	BOOL bfirstcount=TRUE;

	double dSpS=WaveformInfo[nActiveScreen]->dSpS;

	long amax=WaveformInfo[nActiveScreen]->amax;
	long amin=WaveformInfo[nActiveScreen]->amin;

	long amid, adelta, adelta1;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;
	adelta1=adelta/5;

	if(adelta1==0) adelta1=1;
	amid=amid-adelta1;

	amax=amid+adelta;
	amin=amid-adelta;

	
	WaveformInfo[nActiveScreen]->amax=amax;
	WaveformInfo[nActiveScreen]->amin=amin;

	DrawWindow(nActiveScreen, &MemDC);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	this->pRTDisplay->Critical.Unlock();
	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(rc, FALSE);

	}

void CDBPickView::OnShiftDown()
	{
	ScreenBitmapCriticalSection.Lock();
	this->pRTDisplay->Critical.Lock();

	int xw1=WaveformInfo[nActiveScreen]->xw1;
	int xw2=WaveformInfo[nActiveScreen]->xw2;
	int yw1=WaveformInfo[nActiveScreen]->yw1;
	int yw2=WaveformInfo[nActiveScreen]->yw2;

	CRect rc(xw1,MaxY-yw2,xw2,MaxY-yw1);


	CDC MemDC;
	MemDC.CreateCompatibleDC(NULL);
	CBitmap *CBitmapOld=MemDC.SelectObject(&Bitmap);
//*******************************************************

	CFont *pFontOld=MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));

	MemDC.SetBkMode(TRANSPARENT);

	BOOL bfirstcount=TRUE;

	double dSpS=WaveformInfo[nActiveScreen]->dSpS;
	int nXferBlocks=this->pRTDisplay->WaveformInfo[nActiveScreen]->xferData.GetSize();

	long amax=WaveformInfo[nActiveScreen]->amax;
	long amin=WaveformInfo[nActiveScreen]->amin;

	long amid, adelta, adelta1;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;
	adelta1=adelta/5;

	if(adelta1==0) adelta1=1;
	amid=amid+adelta1;

	amax=amid+adelta;
	amin=amid-adelta;

	
	WaveformInfo[nActiveScreen]->amax=amax;
	WaveformInfo[nActiveScreen]->amin=amin;

	DrawWindow(nActiveScreen, &MemDC);
//*******************************************************
	MemDC.SelectObject(CBitmapOld);
	MemDC.SelectObject(pFontOld);

	this->pRTDisplay->Critical.Unlock();
	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(rc, FALSE);

	}
void CDBPickView::ClearDisplay()
	{
	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}
	ScreenBitmapCriticalSection.Unlock();
	InvalidateRect(NULL,TRUE);
	}


void CDBPickView::OnRButtonUp(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default
	int x=point.x;
	int x1=WaveformInfo[0]->x1;
	int x2=WaveformInfo[0]->x2;

	double ttt=(x-x1)*(t2-t1)/(x2-x1)+t1;
	double delta=t1-ttt;
	t1+=delta;
	t2+=delta;
	DrawWaveforms();
	CFrameWnd::OnRButtonUp(nFlags, point);
	}
