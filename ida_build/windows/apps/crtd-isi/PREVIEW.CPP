// Preview.cpp : implementation file
//
#include "stdafx.h"
#include "crtd.h"
#include "Preview.h"
#include "crtdglob.h"
#include <math.h>
#include "MainFrame.h"
#include "TimeFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreview

CPreview::CPreview(CRTDisplay *pDisplay)
	{
	this->pDisplay=pDisplay;
	}

CPreview::~CPreview()
	{
	}


BEGIN_MESSAGE_MAP(CPreview, CWnd)
	//{{AFX_MSG_MAP(CPreview)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreview message handlers

void CPreview::OnPaint() 
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
	// Do not call CWnd::OnPaint() for painting messages
	}

void CPreview::OnSize(UINT nType, int cx, int cy) 
	{
	CWnd::OnSize(nType, cx, cy);
	DrawPreviewBitmap();
	
	// TODO: Add your message handler code here
	
	}

void CPreview::DrawPreviewBitmap()
	{
	if(!bContinue) return;
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
    CBrush BlueBrush(RGB(0,0,255));

    CPen WhitePen(PS_SOLID,1,RGB(255,255,255));
	CPen DarkPen(PS_SOLID,1,RGB(0,0,0));
    CPen BluePen(PS_SOLID,1,RGB(0,0,255));
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));
    CPen WhitePen2(PS_SOLID,2,RGB(255,255,255));
    CPen GrayPen2(PS_SOLID,2,RGB(128,128,128));

    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();

//********************************************************************
	MemDC.SelectObject(&BluePen);
	MemDC.FillRect(&rc, &BlueBrush);
	MemDC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));


	int xl=(int)((pDisplay->t1 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	int xr=(int)((pDisplay->t2 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);

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
	
	MemDC.SelectObject(&WhitePen2);
	MemDC.MoveTo(xl,MaxY-y1);		
	MemDC.LineTo(xr,MaxY-y1);
	MemDC.LineTo(xr,MaxY-y2);		
	MemDC.LineTo(xl,MaxY-y2);		
	MemDC.LineTo(xl,MaxY-y1);		


//********************************************************************
    MemDC.SelectObject(poldBMP);
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);
	MemDC.DeleteDC();

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
void CPreview::DrawWaveform(CDC &MemDC)
	{

	int nActiveScreen=pDisplay->nActiveScreen;

	if(nActiveScreen==-1) return;

    CPen   *oldPen=MemDC.GetCurrentPen();
    CFont  *oldFont=MemDC.GetCurrentFont();
    CBrush *oldBrush=MemDC.GetCurrentBrush();



	double amin=-3000., amax=3000.;
	int nXferBlocks=pDisplay->WaveformInfo[nActiveScreen]->xferData.GetSize();
	double ttt=-1.;
    CPen YellowPen(PS_SOLID,1,RGB(255,255,0));

	MemDC.SelectObject(&YellowPen);

	pDisplay->Critical.Lock();

// find max and min
	BOOL bfirstcount=TRUE;

	for(int j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=pDisplay->WaveformInfo[nActiveScreen]->xferData[j];
		double t1packet=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->beg;
		double t2packet=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->end;
		int nsamp=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->data[k];
			double tt=t1packet+xfd->sint*k;

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
	for(j=0; j<nXferBlocks; ++j)
		{
		CDataBlock *xfd=pDisplay->WaveformInfo[nActiveScreen]->xferData[j];
		double t1packet=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->beg;
		double t2packet=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->end;
		int nsamp=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=pDisplay->WaveformInfo[nActiveScreen]->xferData[j]->data[k];
			double tt=t1packet+xfd->sint*k;
			int x,y;

			if(tt<pDisplay->tB1) continue;
			if(tt>pDisplay->tB2) break;

			x=(int)((tt-pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
			y=(int)((a-amin)*(y2-y1)/(amax-amin)+y1+0.5);
			y=MaxY-y;
			if(tt-ttt>1.1*xfd->sint)
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
	pDisplay->Critical.Unlock();
    MemDC.SelectObject(oldPen);
    MemDC.SelectObject(oldBrush);
    MemDC.SelectObject(oldFont);

	}

void CPreview::OnLButtonUp(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default

	pDisplay->Critical.Lock();
	
	int x=point.x;
	int y=point.y;
	int nActiveScreen=pDisplay->nActiveScreen;
	double tB1=pDisplay->tB1;
	double tB2=pDisplay->tB2;
	double t1=pDisplay->t1;
	double t2=pDisplay->t2;
	double half=nDisplayedTime/2.;

	double ttt=(x-x1)*(tB2-tB1)/(x2-x1)+tB1;

	pDisplay->t1=floor(ttt-half);
	pDisplay->t2=pDisplay->t1+nDisplayedTime;

	if(pDisplay->t2>tB2)
		{
		pDisplay->t2=tB2;
		pDisplay->t1=tB2-nDisplayedTime;
		}
	if(pDisplay->t1<tB1)
		{
		pDisplay->t1=tB1;
		pDisplay->t2=tB1+nDisplayedTime;
		}

	pDisplay->Critical.Unlock();

	DrawPreviewBitmap();
	pDisplay->DrawWaveforms();


	CWnd::OnLButtonUp(nFlags, point);
	}
void CPreview::Stop()
	{
	ScreenBitmapCriticalSection.Lock();
    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}
	ScreenBitmapCriticalSection.Unlock();

	InvalidateRect(NULL,TRUE);
	}

void CPreview::OnMouseMove(UINT nFlags, CPoint point) 
	{
	// TODO: Add your message handler code here and/or call default

	if(!bContinue) return;
	int x=point.x;

	if(x>=x1 && x<=x2)
		{
		double ttt=(x-x1)*(pDisplay->tB2-pDisplay->tB1)/(x2-x1)+pDisplay->tB1;
		char cBuff[64];	
		lstrcpy(cBuff, dttostr(ttt, 1));
		MainFrame *pParent=(MainFrame *)GetParent();

		pParent->OnSetTextToStatusbar1(cBuff);
		}

//	CWnd::OnMouseMove(nFlags, point);
	}
