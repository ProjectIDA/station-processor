// WaveformPreview.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "WaveformPreview.h"
#include <math.h>
#include "TimeFunc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveformPreview

CWaveformPreview::CWaveformPreview(CWaveformsDisplay *pDisplay)
	{
	this->pDisplay=pDisplay;
	double amin=-3000., amax=3000.;
	}

CWaveformPreview::~CWaveformPreview()
	{
	}


BEGIN_MESSAGE_MAP(CWaveformPreview, CWnd)
	//{{AFX_MSG_MAP(CWaveformPreview)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWaveformPreview message handlers

void CWaveformPreview::OnPaint() 
	{
	CPaintDC dc(this); // device context for painting
	void DrawBitmap(CDC *DC, int x, int y, CBitmap *Bitmap);
	
	// TODO: Add your message handler code here
	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL)
		{
		DrawBitmap(&dc, 0, 0,&Bitmap);
		}
	ScreenBitmapCriticalSection.Unlock();

    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));

	int xl=(int)((pDisplay->t1 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	int xr=(int)((pDisplay->t2 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	
	CPen *pOldPen=dc.SelectObject(&WhitePen2);
	dc.MoveTo(xl,MaxY-y1);		
	dc.LineTo(xr,MaxY-y1);
	dc.LineTo(xr,MaxY-y2);		
	dc.LineTo(xl,MaxY-y2);		
	dc.LineTo(xl,MaxY-y1);		
	dc.SelectObject(pOldPen);

	// Do not call CWnd::OnPaint() for painting messages
	}

void CWaveformPreview::OnSize(UINT nType, int cx, int cy) 
	{
	CWnd::OnSize(nType, cx, cy);
	DrawPreviewBitmap();
	}
void CWaveformPreview::DrawPreviewBitmap()
	{
	CWaitCursor wc;
	RECT rc;
    CDC MemDC;
    CDC *pDC=GetDC();
	CBitmap Bitmap1;

	GetClientRect(&rc);

	MaxX=rc.right;
	MaxY=rc.bottom;
	


	x1=4;
	x2=MaxX-4;
	y1=4;
	y2=MaxY-4;

	xw1=1;
	xw2=MaxX;
	yw1=0;
	yw2=MaxY;

    Bitmap1.CreateCompatibleBitmap(pDC,rc.right,rc.bottom);
    MemDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

    CBitmap *poldBMP=MemDC.SelectObject(&Bitmap1);

    CBrush WhiteBrush(RGB(255,255,255));
    CBrush GrayBrush(RGB(192,192,192));
    CBrush BlueBrush(RGB(0,0,192));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(0,255,255));
    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));

    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();

//********************************************************************
	MemDC.SelectObject(&BluePen);
	MemDC.FillRect(&rc, &BlueBrush);
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));


//	int xl=(int)((pDisplay->t1 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
//	int xr=(int)((pDisplay->t2 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);

	MemDC.SelectObject(&WhitePen2);

	MemDC.MoveTo(xw1,MaxY-yw2);		
	MemDC.LineTo(xw2,MaxY-yw2);

	MemDC.MoveTo(xw1,MaxY-yw2);		
	MemDC.LineTo(xw1,MaxY-yw1);

	MemDC.SelectObject(&GrayPen2);

	MemDC.MoveTo(xw2,MaxY-yw1);		
	MemDC.LineTo(xw2,MaxY-yw2);

	MemDC.MoveTo(xw2,MaxY-yw1);		
	MemDC.LineTo(xw1,MaxY-yw1);



	DrawWaveform(MemDC);
	
//	MemDC.SelectObject(&WhitePen2);
//	MemDC.MoveTo(xl,MaxY-y1);		
//	MemDC.LineTo(xr,MaxY-y1);
//	MemDC.LineTo(xr,MaxY-y2);		
//	MemDC.LineTo(xl,MaxY-y2);		
//	MemDC.LineTo(xl,MaxY-y1);		


//********************************************************************
    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);

    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}
	HGDIOBJ pBmp=Bitmap1.Detach();
	Bitmap.Attach(pBmp);

	InvalidateRect(NULL,FALSE);

	}
void CWaveformPreview::DrawWaveform(CDC &MemDC)
	{

	int nActiveScreen=pDisplay->nActiveScreen;

	if(nActiveScreen==-1) return;

	int nDataBlocks=pDisplay->WaveformInfo[nActiveScreen]->wfdrec.GetSize();
	double ttt=-1.;
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));

	MemDC.SelectObject(&YellowPen);

// find max and min
	BOOL bfirstcount=TRUE;
	double dSpS=pDisplay->WaveformInfo[nActiveScreen]->dSpS;

	for(int j=0; j<nDataBlocks; ++j)
		{
		double t1packet=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.time;
		double t2packet=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.endtime;
		int nsamp=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->lData[k];
			double tt=t1packet+(1./dSpS)*k;

			if(tt<pDisplay->tB1) continue;
			if(tt>pDisplay->tB2) break;
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


//*************************************
	for(j=0; j<nDataBlocks; ++j)
		{
//		CDataBlock *xfd=pDisplay->WaveformInfo[nActiveScreen]->waveData[j];
		double t1packet=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.time;
		double t2packet=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.endtime;
		int nsamp=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->wfd.nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=pDisplay->WaveformInfo[nActiveScreen]->wfdrec[j]->lData[k];
			double tt=t1packet+(1./dSpS)*k;
			int x,y;

			if(tt<pDisplay->tB1) continue;
			if(tt>pDisplay->tB2) break;

			x=(int)((tt-pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
			y=(int)((a-amin)*(y2-y1)/(amax-amin)+y1+0.5);
			y=MaxY-y;
			if(tt-ttt>1.1/dSpS)
				{
				MemDC.MoveTo(x,y);		
				}
			else
				{
				MemDC.LineTo(x,y);		
				}
			ttt=tt;
			}
		}
	}

void CWaveformPreview::OnLButtonUp(UINT nFlags, CPoint point) 
	{

	int x=point.x;
	int y=point.y;
	int nActiveScreen=pDisplay->nActiveScreen;
	double tB1=pDisplay->tB1;
	double tB2=pDisplay->tB2;
	double t1=pDisplay->t1;
	double t2=pDisplay->t2;
	double half=(t2-t1)/2.;

	double ttt=(x-x1)*(tB2-tB1)/(x2-x1)+tB1;

	pDisplay->t1=ttt-half;
	pDisplay->t2=pDisplay->t1+2.*half;

	if(pDisplay->t2>tB2)
		{
		pDisplay->t2=tB2;
		pDisplay->t1=tB2-2.*half;
		}
	if(pDisplay->t1<tB1)
		{
		pDisplay->t1=tB1;
		pDisplay->t2=tB1+2.*half;
		}

	InvalidateRect(NULL, FALSE);
	pDisplay->DrawWaveforms();
	

	CWnd::OnLButtonUp(nFlags, point);
	}


void CWaveformPreview::OnMouseMove(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default

	int x=point.x;
	int y=MaxY-point.y;

	if(pDisplay->t1<0 || pDisplay->t2<0 || pDisplay->tB1<0 || pDisplay->tB2<0) return;


	double ttt=(x-x1)*(pDisplay->tB2-pDisplay->tB1)/(x2-x1)+pDisplay->tB1;
	long aaa=(long)((double)(y-y1)*(amax-amin)/(y2-y1)+amin+0.5);

	char cBuff[64];	
	sprintf(cBuff,"A:%d T:%s",(long)aaa, dttostr(ttt, 1));
	CMainFrame *pParent=(CMainFrame *)GetParent();

	pParent->OnSetTextToStatusbar1(cBuff);

	CWnd::OnMouseMove(nFlags, point);
	}


BOOL CWaveformPreview::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR		*pnmhdr=(LPNMHDR )lParam;
	int i;
	switch (pnmhdr->code)
		{
		case TTN_NEEDTEXT:
			i=0;
			break;
		}	

	return CWnd::OnNotify(wParam, lParam, pResult);
	}
