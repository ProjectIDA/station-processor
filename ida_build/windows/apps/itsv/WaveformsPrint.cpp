// WaveformsPrint.cpp: implementation of the CWaveformsPrint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "WaveformsPrint.h"
#include <winspool.h>
#include "filter\filter.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern struct gather_plot_filter_ gpl_filters_def[];


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CWaveformsPrint::CWaveformsPrint(CWaveformsDisplay *pWaveformsDisplay)
	{	
	pWD=pWaveformsDisplay;
	}

CWaveformsPrint::~CWaveformsPrint()
	{
	}
void CWaveformsPrint::Print()
	{
	PRINTDLG pd;
	memset(&pd,0,sizeof(PRINTDLG));
	pd.lStructSize=(DWORD)sizeof(PRINTDLG);

	CPrintDlg dlg(FALSE);


    dlg.m_pd.hInstance = AfxGetInstanceHandle();
    dlg.m_pd.lpPrintTemplateName = MAKEINTRESOURCE(1538);
    dlg.m_pd.Flags |= PD_ENABLEPRINTTEMPLATE;



	if(dlg.DoModal() == IDOK)
		{
		dlg.GetDefaults();

		CString DriverName=dlg.GetDriverName();
		CString DeviceName=dlg.GetDeviceName();
		CString Output=dlg.GetPortName();
		DEVMODE dvmode;
		LPDEVMODE lpdvmode=dlg.GetDevMode();


		// Open printer and obtain PRINTER_INFO_2 structure.
		HANDLE hPrinter;
		if (OpenPrinter((LPSTR)(LPCSTR)DeviceName, &hPrinter, NULL) == FALSE)return;
		DWORD dwBytesReturned, dwBytesNeeded;
		GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);
		PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR,
		dwBytesNeeded);
		if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded,&dwBytesReturned) == 0) 
			{
			GlobalFree(p2);
			ClosePrinter(hPrinter);
			return;
			}
		ClosePrinter(hPrinter);

		memcpy(&dvmode, p2->pDevMode, sizeof(dvmode));
		dvmode.dmOrientation=DMORIENT_LANDSCAPE;
		GlobalFree(p2);
		HDC	hdc=CreateDC(DriverName,DeviceName,Output,&dvmode);

		CDC *pDC=new CDC();

		pDC->Attach(hdc);

		CString sHead=dlg.GetHeaderText();

		MaxX = pDC->GetDeviceCaps(HORZRES);
		MaxY = pDC->GetDeviceCaps(VERTRES);
		int nMMX = pDC->GetDeviceCaps(HORZSIZE);
		int nMMY = pDC->GetDeviceCaps(VERTSIZE);

		p_mm_x=((float)MaxX/nMMX);
		p_mm_y=((float)MaxY/nMMY);

		topMargin=(int)(p_mm_y*20);

		PrintWaveforms(pDC,sHead);

		pDC->Detach();
		delete pDC;
		CDC::FromHandle(hdc)->DeleteDC();
		}

	}
void CWaveformsPrint::PrintWaveforms(CDC *pDC, CString sHead)
	{
	CPen Pen(PS_SOLID,1, RGB(0,0,0));
	CFont fStaNameFont, fAmplFont, fHeadFont;
	CFont *pOldFont=pDC->GetCurrentFont();


	RECT rcText;
	char cText[128];


	DOCINFO di;
	memset(&di,0,sizeof(DOCINFO));
	di.cbSize=sizeof(DOCINFO);
	di.lpszDocName="ITSV Output";

	pDC->StartDoc(&di);
	pDC->StartPage();



	CRect rc(0,0,MaxX-1,MaxY-1);
	int i;


	pDC->SetTextColor(RGB(0,0,0));



	int nWin=pWD->pScreen->GetMaxWindow();
	TimeBarYsize=(int)(30*p_mm_y);
	WindowInfoXsize=(int)(30*p_mm_x);

	int x1,x2,y1,y2,xw1,xw2,yw1,yw2;
	int xX1=WindowInfoXsize;
	int xX2=MaxX;

	fHeadFont.CreateFont(8*p_mm_y,0,0,0,FW_REGULAR,true,false,0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Helv");

	pDC->SelectObject(&fHeadFont);
	GetTextRectangle(pDC,(LPSTR)(LPCSTR)sHead,rcText);

	nHeadHeight=rcText.bottom;

	rcText.right=MaxX;
	rcText.bottom+=topMargin;
	rcText.top+=topMargin;
	pDC->DrawText((LPSTR)(LPCSTR)sHead,&rcText,DT_VCENTER|DT_SINGLELINE|DT_CENTER);


	CPen BoldPen(PS_SOLID,(int)p_mm_x, RGB(0,0,0));
	fStaNameFont.CreateFont(5*p_mm_y,0,0,0,FW_BOLD,false,false,0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Helv");
	fAmplFont.CreateFont(2*p_mm_y,0,0,0,FW_THIN,false,false,0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Helv");

	int yHeight=(MaxY-TimeBarYsize-nHeadHeight-topMargin)/nWin;
	for(int j=nWin-1; j>=0; --j)
		{
		i=pWD->pScreen->GetWaveformN(j);

		long amin=pWD->WaveformInfo[i]->amin;
		long amax=pWD->WaveformInfo[i]->amax;

		xw1=0;
		xw2=MaxX-1;
		yw1=yHeight*j+TimeBarYsize;
		yw2=(yHeight*(j+1)+TimeBarYsize)-5*p_mm_y;

		x1=WindowInfoXsize;
		x2=MaxX;

		y1=yw1+1;
		y2=yw2-1;

		pWD->DrawWindow(j, pDC, TimeBarYsize, x1,  x2,  y1,  y2, xw1, xw2, yw1, yw2, MaxY, MaxX, true);

//      Draw left side


		pDC->SelectObject(&BoldPen);
		pDC->MoveTo(xw1+WindowInfoXsize,MaxY-(yw1+8));		
		pDC->LineTo(xw1+WindowInfoXsize,MaxY-(yw2-8));
		pDC->SelectObject(&Pen);

		pDC->MoveTo(xw1+WindowInfoXsize,MaxY-(yw1+8));
		pDC->LineTo(xw1+WindowInfoXsize+4*p_mm_x,MaxY-(yw1+8));

		pDC->MoveTo(xw1+WindowInfoXsize,MaxY-(yw2-8));
		pDC->LineTo(xw1+WindowInfoXsize+4*p_mm_x,MaxY-(yw2-8));

		pDC->SetTextColor(RGB(0,0,0));

		CRect rcc3(xw1+5,(MaxY-yw2),xw1+WindowInfoXsize-2*p_mm_x, (MaxY-yw1));
		CRect rcc2(xw1,MaxY-(yw1+8+13),xw1+WindowInfoXsize-2*p_mm_x, MaxY-(yw1-8));
		CRect rcc1(xw1,MaxY-(yw2-8),xw1+WindowInfoXsize-2*p_mm_x, MaxY-(yw2-8-13));


		pDC->SetBkMode(TRANSPARENT);

		pDC->SelectObject(&fAmplFont);

		sprintf(cText,"%d",amax);
		PrintLabelStringX(pDC,xw1+WindowInfoXsize-2*p_mm_x, yw2, cText);

		sprintf(cText,"%d",amin);
		PrintLabelStringX(pDC,xw1+WindowInfoXsize-2*p_mm_x, yw1, cText);

		sprintf(cText,"%s::%s",pWD->WaveformInfo[i]->StaName, pWD->WaveformInfo[i]->ChanName);
		pDC->SelectObject(fStaNameFont);
		pDC->DrawText(cText,rcc3, DT_VCENTER|DT_SINGLELINE);
		}

	xw1 = WindowInfoXsize;
	xw2 = MaxX;
	yw1 = 0;
	yw2 = TimeBarYsize;
	DrawTimeMarks(pDC, pWD->t1, pWD->t2, xw1, xw2, yw1, yw2);
	pDC->SelectObject(pOldFont);

	PrintStatus(pDC);
//	pDC->AbortDoc();
	pDC->EndPage();
	pDC->EndDoc();
	}
void CWaveformsPrint::PrintStatus(CDC *pDC)
	{
	RECT rcText;
	CFont fStatusFont;

	fStatusFont.CreateFont(3*p_mm_y,0,0,0,FW_NORMAL,false,false,0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Times New Roman");

	CFont *pOldFont=pDC->GetCurrentFont();
	pDC->SelectObject(&fStatusFont);

	CString sPrint;
	sPrint="Filter:";
	if(pWD->nFilterType==-1)
		{
		sPrint+="none";
		}
	else
		{
		char cBuff[128];
		lstrcpy(cBuff,gpl_filters_def[pWD->nFilterType].label);
		sPrint=sPrint+gpl_filters_def[pWD->nFilterType].label;
		}

	sPrint+=", Amp:";
	switch(pWD->DisplayMode)
		{
		case pWD->mFx:
			sPrint+="Fixed";
			break;
		case pWD->mAuto:
			sPrint+="Auto";
			break;
		case pWD->mAuto0:
			sPrint+="Auto0";
			break;
		case pWD->mAuto1:
			sPrint+="Auto1";
			break;
		case pWD->mAutoA:
			sPrint+="AutoA";
			break;
		}
	sPrint+="\n";

	sPrint+="File:"+pWD->WfdFileName;

	CTime t = CTime::GetCurrentTime();
	
	sPrint+=" "+t.Format("%H:%M:%S  %A, %B %d, %Y");

	GetTextRectangle(pDC, (LPSTR)(LPCSTR)sPrint, rcText, DT_LEFT);

	int hgt=(rcText.bottom-rcText.top);
	rcText.top=hgt;
	rcText.bottom=0;

	int n1=(MaxY-rcText.bottom);
	int n2=(MaxY-rcText.top);
	rcText.top=n2;
	rcText.bottom=n1;

	pDC->DrawText((LPSTR)(LPCSTR)sPrint,&rcText,DT_VCENTER|DT_LEFT);
		
	pDC->SelectObject(pOldFont);
	}
void CWaveformsPrint::PrintLabelStringX(CDC *pDC, int x, int y, LPSTR s)
	{
	RECT rcc;
	GetTextRectangle(pDC,s,rcc);
	int nWtd=(rcc.right-rcc.left);
	int hgt=(rcc.bottom-rcc.top);
	rcc.left=x-nWtd;
	rcc.right=x;
	rcc.top=y+hgt/2.+0.5;
	rcc.bottom=y-hgt/2.+0.5;
	int n1=(MaxY-rcc.bottom);
	int n2=(MaxY-rcc.top);
	rcc.top=n2;
	rcc.bottom=n1;
	pDC->DrawText(s,&rcc,DT_VCENTER|DT_SINGLELINE );
	}
void CWaveformsPrint::GetTextRectangle(CDC *pdc, LPSTR lpText, RECT &rc, UINT nFormat)
	{
	nFormat|=DT_CALCRECT;
	rc.left=0;rc.top=0;rc.right=0; rc.bottom=0;
	pdc->DrawText( lpText, -1,&rc, nFormat);
	}
void CWaveformsPrint::DrawTimeMarks(CDC *pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2)
	{
	int x;

	int delta=pWD->GetDeltaInterval();
	if(delta==0) delta=1;

	int	nLastMarkRightPos=-1;
	long ttstart=(t1/3600)*3600;

	CFont *pOldFont;
	CFont sbf;
	sbf.CreateFont(3*p_mm_y,0,0,0,FW_BOLD,false,false,0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
	CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,"Helv");
	pOldFont=pDC->SelectObject(&sbf);



	for(long tt=ttstart; tt<t2; tt++)
		{
		if(tt<t1) continue;
		if(((int)tt)%delta==0)
			{
			x=(int)((tt-t1)*(xw2-xw1)/(t2-t1)+xw1);
			{
			char cBuff1[64], cBuff2[64];


			lstrcpy(cBuff1, dttostr(tt, 15));
			RECT rcc1,rcc2;
			GetTextRectangle(pDC,cBuff1,rcc1);
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
			GetTextRectangle(pDC, cBuff2,rcc2);

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
				pDC->MoveTo(x,nHeadHeight+topMargin);
				pDC->LineTo(x,MaxY-TimeBarYsize);
				}

			}

			}


		}
	pDC->SelectObject(pOldFont);
	}
