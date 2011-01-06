// InfoWindow.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "InfoWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoWindow

CInfoWindow::CInfoWindow()
	{
	}

CInfoWindow::~CInfoWindow()
	{
	}

void CInfoWindow::SetText(CString cs)
	{
	sText=cs;

	CFont *pFontOld;
	CDC *pdc=GetDC();
		
	pFontOld=pdc->SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	rcc.left=0;rcc.top=0;rcc.right=0; rcc.bottom=0;
	pdc->DrawText( (LPCSTR)sText, -1,&rcc, DT_CALCRECT|DT_LEFT);
	pdc->SelectObject(pFontOld);
	ReleaseDC(pdc);
	
	RECT rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	MoveWindow(rc.left, rc.top, rcc.right, rcc.bottom, FALSE);
	InvalidateRect(NULL,FALSE);
	}


BEGIN_MESSAGE_MAP(CInfoWindow, CWnd)
	//{{AFX_MSG_MAP(CInfoWindow)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoWindow message handlers

void CInfoWindow::OnPaint() 
	{
    CBrush YellowBrush(RGB(255,255,0));
	CPaintDC dc(this); // device context for painting
	GetClientRect(&rcc);
	CFont *pOldFont=dc.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));
	dc.FillRect(&rcc, &YellowBrush);
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(sText,&rcc,DT_LEFT|DT_SINGLELINE);
	dc.SelectObject(pOldFont);

	}
