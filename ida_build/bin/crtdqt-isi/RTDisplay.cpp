// RTDisplay.cpp: implementation of the CRTDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include <qapplication.h>
#include <qvariant.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>

#include "RTDisplay.h"
#include "nrtsinfo.h"
#include "CRTDglob.h"
#include "util.h"
#include "MainFrame.h"


THREAD_FUNC GettingDataThread(void* lpV);

#ifndef max
#define max(x,y)((x<y)?y:x)
#endif
#ifndef min
#define min(x,y)((x>y)?y:x)
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRTDisplay::CRTDisplay(QWidget * parent = 0, const char * name = 0, WFlags f = 0)
:QWidget(parent, name , f) 
	{



	QRect rc;
	GetTextRectangle("WgWgWgWgWgWg",rc);
	TimeBarYsize=rc.height()*2+8;
	WindowInfoXsize=rc.width();


	int t=rc.top();
	int b=rc.bottom();
	tmmarks.setAutoDelete(TRUE);
	nActiveScreen = -1;
	bDrawBitmap = FALSE;
	setMouseTracking(TRUE);
	}

CRTDisplay::~CRTDisplay()
	{
	ClearWaveformInfo();
	}
void CRTDisplay::ClearWaveformInfo()
	{
	while(WaveformInfo.count()>0)
		{
		CMWindow *wfi=WaveformInfo.first();
		WaveformInfo.removeFirst();
		delete wfi;
		}
	}

void CRTDisplay::AddWindow(QString Sta, QString Chan, QString LCODE, double dSpS)
	{
	CMWindow *wfi = new CMWindow;
	wfi->StaName  = Sta;
	wfi->ChanName = Chan;
	wfi->LCODE    = LCODE;
	wfi->dSpS     = dSpS;
	WaveformInfo.append(wfi);
	}
void CRTDisplay::RemoveWindow(QString Sta, QString Chan, QString LCODE)
	{
	CMWindow *wfi;
	for(wfi = WaveformInfo.first(); wfi; wfi=WaveformInfo.next())
		{
		if( (Sta.compare(wfi->StaName)==0) && (Chan.compare(wfi->ChanName)==0) && (LCODE.compare(wfi->LCODE)==0))
			{
			WaveformInfo.remove();
			delete wfi;
			return;
			}
		}

	}


void CRTDisplay::Run()
	{
	CNRTSSTAINFO	*si;
	CNRTSCHANINFO	*ci;
	QString			sSta,sChan,sLoc;

	// Make windows control list

	ClearWaveformInfo();
	nLastMarkRightPos=0;
	for(si=NrtsInfo.StaInfo.first(); si; si=NrtsInfo.StaInfo.next())
		{
		sSta=si->Sta;
		for(ci=si->ChanInfo.first();ci; ci=si->ChanInfo.next())
			{
			if(ci->bSelected)
				{
				sChan=ci->Chan;
				sLoc=ci->LCODE;
				double dSpS=ci->dSpS;
				AddWindow(sSta, sChan, sLoc, dSpS);
				}
			}
		}

	nActiveScreen=0;
	CalculateWindowsSize();
	bContinue=TRUE;

	}
void CRTDisplay::Stop()
	{
	ScreenBitmapCriticalSection.lock();
	Critical.lock();

	ClearWaveformInfo();
	bDrawBitmap = FALSE;
	Critical.unlock();
	ScreenBitmapCriticalSection.unlock();
	ClearDisplay();
	repaint(TRUE);
	}

void CRTDisplay::CalculateWindowsSize()
	{
	int i;
	CMWindow *wfi;
	int nWin = WaveformInfo.count();
	if(nWin==0)
		{
		return;
		}
	MaxX=width();
	MaxY= height();
	int yHeight=(MaxY-TimeBarYsize)/nWin;
	i=0;
	for(wfi=WaveformInfo.first();wfi;wfi=WaveformInfo.next(),++i)
		{
		wfi->xw1=2;
		wfi->xw2=MaxX-1;
		wfi->yw1=yHeight*i+TimeBarYsize;
		wfi->yw2=(yHeight*(i+1)+TimeBarYsize)-1;

		wfi->x1=wfi->xw1+WindowInfoXsize+2;
		wfi->x2=wfi->xw2-1;

		wfi->y1=wfi->yw1+1;
		wfi->y2=wfi->yw2-1;
		}

	}
void  CRTDisplay::resizeEvent( QResizeEvent *q )
	{
	bRepaint=TRUE;
	}


void CRTDisplay::OnTimer() 
	{
	SetTime();
	RemoveOldData();
	bRepaint=FALSE;
	DrawWaveforms();
	}
void CRTDisplay::DrawWaveforms()
	{
	QPen DarkPen(QColor(0,0,0));
	QBrush BlueBrush(QColor(0,0,128));


    QBrush WhiteBrush(QColor(255,255,255));
    QBrush GrayBrush(QColor(192,192,192));

    QPen WhitePen(QColor(255,255,255));
    QPen BluePen(QColor(0,0,255));
    QPen YellowPen(QColor(255,255,0));

    QPen WhitePen2(QColor(255,255,255),2);
    QPen GrayPen2 (QColor(128,128,128),2);



	if(height()<30 && width()<30) return;

	QMutexLocker ml(&Critical);

	CalculateWindowsSize();
	QRect rc(0,0,MaxX, MaxY);
	QPixmap Bitmap1(MaxX, MaxY);
	QPainter DC(&Bitmap1);
	DC.setBackgroundMode(Qt::TransparentMode);

	DC.fillRect(rc, BlueBrush);

	DC.setPen(DarkPen);
	DC.moveTo(0,0);		
	DC.lineTo(0,MaxY);

	DrawTimePanel(&DC);

	for(unsigned int i=0; i<WaveformInfo.count(); ++i)
		{
		DrawWindow(i,&DC);
		}


//	DrawTimePanel(&DC);


	ScreenBitmapCriticalSection.lock();
	Bitmap = Bitmap1;
	bDrawBitmap = TRUE;
	ScreenBitmapCriticalSection.unlock();

	repaint(FALSE);
	}
void CRTDisplay::SetTime()
	{
	double dnewtime;
	time_t tim;

	tim = time(NULL);
	dnewtime = tim;

	dnewtime-=nDelay;
	Critical.lock();

	if(t2>=tB2)
		{
		t2=dnewtime;
		t1=t2-nDisplayedTime;
		}
	else
		{
		if(tB1>=t1)
			{
			t1=dnewtime-nBufferedTime;
			t2=nDisplayedTime+t1;
			}
		}
	tB2=dnewtime;
	tB1=tB2-nBufferedTime;
	Critical.unlock();

	}
void CRTDisplay::DrawFocused(QPainter *pDC)
	{
	if(nActiveScreen==-1) return;
	if(WaveformInfo.count()==0) return;
	CMWindow *wfi = WaveformInfo.at(nActiveScreen);
	int xw1=wfi->xw1+4;
	int xw2=wfi->xw1+WindowInfoXsize-4;
	int yw1=wfi->yw1+4;
	int yw2=wfi->yw2-4;


    QPen WhitePen(QColor(255,255,255));
    QPen GrayPen(QColor(128,128,128));


	pDC->setPen(GrayPen);

	pDC->moveTo(xw1,MaxY-yw2);		
	pDC->lineTo(xw2,MaxY-yw2);

	pDC->moveTo(xw1,MaxY-yw2);		
	pDC->lineTo(xw1,MaxY-yw1);

	pDC->setPen(WhitePen);

	pDC->moveTo(xw2,MaxY-yw1);		
	pDC->lineTo(xw2,MaxY-yw2);

	pDC->moveTo(xw2+1,MaxY-yw1);		
	pDC->lineTo(xw2+1,MaxY-yw2);


	pDC->moveTo(xw2,MaxY-yw1);		
	pDC->lineTo(xw1,MaxY-yw1);

	pDC->moveTo(xw2,MaxY-yw1+1);		
	pDC->lineTo(xw1,MaxY-yw1+1);



	}

void CRTDisplay::mousePressEvent ( QMouseEvent *e )  
	{

	QMutexLocker ml(&Critical);

	CMWindow *wfi;	
	int x=e->x();
	int y=MaxY-e->y();
	int i=0;

//
// Change Focus
//
	for(wfi = WaveformInfo.first(); wfi; wfi=WaveformInfo.next(),++i)
		{
		int xw1=wfi->xw1;
		int xw2=wfi->xw2;
		int yw1=wfi->yw1;
		int yw2=wfi->yw2;

		if( y<=yw2 && y>=yw1)
			{
			nActiveScreen=i;
/*			RECT rc;
			rc.top=0;
			rc.bottom=MaxY;
			rc.left=0;
			rc.right=WindowInfoXsize+4;*/
			repaint(FALSE);
//			InvalidateRect(&rc,FALSE);

//   Update Preview Window
			MainFrame *pParent=(MainFrame *)parentWidget();
			QCustomEvent *event;
			event = new QCustomEvent(CPreview::DrawPreview);
			QApplication::postEvent(pParent->pPreview, event);

//			pParent->pPreview->DrawPreviewBitmap();
//			pParent->pPreview->repaint(FALSE);
/*			CWnd *pParent=GetParent();
			pParent->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_UPDATEVIEW,0),(LPARAM)m_hWnd);*/
			return;
			}
		
		}


/*	CWnd::OnLButtonUp(nFlags, point);*/
	}
void CRTDisplay::GetTextRectangle(const char *lpText, QRect &rc)
	{

	const QFont f=font();

	QFontMetrics qfm(f);
	rc.setRect(0,0,0,0);
	rc = qfm.boundingRect(lpText, -1);

	}
//************************************************************
//
// Removes data blocks out of buffered time interval
//
//
void CRTDisplay::RemoveOldData()
	{
	CDataBlock *xfd;
	CMWindow *wfi;
	Critical.lock();
	for(wfi = WaveformInfo.first(); wfi; wfi=WaveformInfo.next())
		{
		for(xfd = wfi->xferData.first();xfd;xfd = wfi->xferData.next())
			{
			double t1packet=xfd->beg;
			double t2packet=xfd->end;

			if(t2packet<tB1)
				{
				wfi->xferData.remove();
				wfi->xferData.prev();
				delete xfd;
				continue;
				}
			}
		}
	Critical.unlock();
	}
void CRTDisplay::DrawTimePanel(QPainter *pDC)
	{
    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));
	QPen RedPen(QColor(255,0,0));
	
	int xw1=WindowInfoXsize+3;
	int xw2=MaxX-1;
	int yw1=1;
	int yw2=TimeBarYsize-1;
	int x, x_old;

	int delta=(t2-t1)/10;
	if(delta==0) delta=1;

	DrawTimeBounds(pDC);

	tmmarks.clear();

	pDC->setPen(WhitePen);


	pDC->moveTo(xw1,MaxY-yw2);		
	pDC->lineTo(xw2,MaxY-yw2);

	pDC->moveTo(xw1,MaxY-yw2);		
	pDC->lineTo(xw1,MaxY-yw1);

	pDC->setPen(DarkPen);

	pDC->moveTo(xw2,MaxY-yw1);		
	pDC->lineTo(xw2,MaxY-yw2);

	pDC->moveTo(xw2,MaxY-yw1);		
	pDC->lineTo(xw1,MaxY-yw1);

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

		if(((int)tt)%delta==0)
			{
			x_old=x;

			DrawTimeMark(x, tt, pDC);
			}


		}


	}


void CRTDisplay::DrawTimeMark(int x, double tt, QPainter *pDC)
	{
    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));

	int yw1=1;
	int yw2=TimeBarYsize-1;

	char cBuff1[64], cBuff2[64];


	pDC->setPen(WhitePen);

	strcpy(cBuff1, util_dttostr(tt, 15));
	QRect rcc1,rcc2;
	GetTextRectangle(cBuff1,rcc1);
	int wdt=rcc1.width()/2+1;
	int hgt=rcc1.height();

	rcc1.setTopLeft(QPoint(x-wdt, (MaxY-(yw1+hgt))));
	rcc1.setBottomRight(QPoint(x+wdt, (MaxY-(yw1))));


	strcpy(cBuff2, util_dttostr(tt, 14));
	GetTextRectangle(cBuff2,rcc2);
	wdt=rcc2.width()/2+1;
	hgt=rcc2.height();
	rcc2.setTopLeft(QPoint(x-wdt, (MaxY-(yw1+hgt+hgt))));
	rcc2.setBottomRight(QPoint(x+wdt, (MaxY-(yw1+hgt))));




	int xminPos=min(rcc2.left(),rcc1.left());
	if( nLastMarkRightPos<xminPos && WindowInfoXsize<xminPos)
		{
		pDC->drawText(rcc1, Qt::AlignLeft, cBuff1);
		pDC->drawText(rcc2, Qt::AlignLeft, cBuff2);
		nLastMarkRightPos=max(rcc1.right(),rcc2.right());
//		nLastMarkRightPos=rcc1.right();
		tmmarks.append(new int(x));
		pDC->setPen(WhitePen);
		pDC->moveTo(x,MaxY-yw2);		
		pDC->lineTo(x,MaxY-(yw2-4));
		pDC->setPen(DarkPen);
		pDC->moveTo(x+1,MaxY-yw2);		
		pDC->lineTo(x+1,MaxY-(yw2-4));
		}
	}

void CRTDisplay::DrawTimeBounds(QPainter *pDC)
	{
	int yw1=1;
	int yw2=TimeBarYsize-1;
	char cBuff1[64];

	QPen WhitePen(QColor(255,255,255));


	pDC->setPen(WhitePen);

	strcpy(cBuff1, util_dttostr(t1,14));
	QRect rcc1,rcc2;
	GetTextRectangle(cBuff1,rcc1);
	rcc1.moveTop(MaxY-yw2);

/*	int n1=(MaxY-rcc1.bottom);
	int n2=(MaxY-rcc1.top);
	rcc1.top=n1;
	rcc1.bottom=n2;



	lstrcpy(cBuff2, dttostr(t2,15));
	GetTextRectangle(cBuff2,rcc2);
	nWtd=rcc2.right-rcc2.left;
	hgt=(rcc2.bottom-rcc2.top);
	rcc2.left+=1;
	rcc2.right+=1;
	rcc2.bottom+=yw1;
	rcc2.top+=yw1;

	n1=(MaxY-rcc2.bottom);
	n2=(MaxY-rcc2.top);
	rcc2.top=n1;
	rcc2.bottom=n2;*/

//	pDC->DrawText(cBuff1,&rcc1,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);
	pDC->drawText(rcc1, Qt::AlignLeft, cBuff1);
//	pDC->DrawText(cBuff2,&rcc2,DT_LEFT|DT_SINGLELINE|DT_BOTTOM);

//	nLastMarkRightPos=max(rcc1.right,rcc2.right);
	nLastMarkRightPos=rcc1.right();
	}

void CRTDisplay::OnAutoScale(int nType)
	{


	ScreenBitmapCriticalSection.lock();
	Critical.lock();

	int sState=bShiftButtonPressed;
	QPixmap Bitmap1=Bitmap;
	Critical.unlock();

	QPainter DC(&Bitmap1);

	if(sState==0)
		{
		ApplyAutoscale(nActiveScreen, nType);
		DrawWindow(nActiveScreen, &DC);
		}
	else
		{
		for(unsigned int i=0; i<WaveformInfo.count(); ++i)
			{
			ApplyAutoscale(i, nType);
			DrawWindow(i, &DC);
			}
		}
	Critical.lock();
	Bitmap=Bitmap1;
	Critical.unlock();
	ScreenBitmapCriticalSection.unlock();

	if(sState==0)
		{
		CMWindow *wfi = WaveformInfo.at(nActiveScreen);
		int xw1=wfi->xw1;
		int xw2=wfi->xw2;
		int yw1=wfi->yw1;
		int yw2=wfi->yw2;

//		QRect rc(QPoint(xw1,MaxY-yw2),QPoint(xw2,MaxY-yw1));

		repaint(FALSE); //InvalidateRect(rc, FALSE);
		}
	else
		{
		repaint(FALSE);
		}

	}
void CRTDisplay::ApplyAutoscale(int nWindow, int nType)
	{
	CDataBlock *xfd;
	BOOL bfirstcount=TRUE;
	CMWindow *wfi = WaveformInfo.at(nWindow);


	long amax=wfi->amax;
	long amin=wfi->amin;

	
	for(xfd = wfi->xferData.first();xfd;xfd = wfi->xferData.next())
		{
		double t1packet=xfd->beg;
		double t2packet=xfd->end;
		int nsamp=xfd->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=xfd->data[k];
			double tt=t1packet+xfd->sint*k;

			if(nType==0)
				{
				if(tt<t1) continue;
				if(tt>t2) break;
				}
			if(!bfirstcount)
				{
				if(a>amax) amax=a;
				if(a<amin) amin=a;
				}
			else
				{
				bfirstcount=FALSE;
				amax=a+1;
				amin=a-1;
				}

			}
		}
	if(amax==amin)
		{
		amax++;
		amin--;
		}

	WaveformInfo.at(nWindow)->amax=amax;
	WaveformInfo.at(nWindow)->amin=amin;
	}

void CRTDisplay::DrawWindow(int nWindow, QPainter *pDC)
	{
	QString qText;
    QBrush WhiteBrush(QColor(255,255,255));
    QBrush GrayBrush(QColor(192,192,192));
    QBrush BlueBrush(QColor(0,0,128));

    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));
    QPen BluePen(QColor(0,0,255));
    QPen YellowPen(QColor(255,255,0));

	CMWindow *wfi = WaveformInfo.at(nWindow);

	int xw1=wfi->xw1;
	int xw2=wfi->xw2;
	int yw1=wfi->yw1;
	int yw2=wfi->yw2;

	int x1=wfi->x1;
	int x2=wfi->x2;
	int y1=wfi->y1;
	int y2=wfi->y2;

	if(wfi->nBlocksRec<5 )
		{
		ApplyAutoscale(nWindow,1);	
		}


	long amin=wfi->amin;
	long amax=wfi->amax;
	

	QRect rc(QPoint(xw1-1, MaxY-yw2-1), QPoint(xw2+1, MaxY-yw1+1));
	pDC->fillRect(rc, BlueBrush);

//	pDC->SelectObject(&DarkPen);


//*************************************************************
//      Draw Ampl (Y) Axis
//

	QRect rc1(QPoint(xw1, MaxY-yw2), QPoint(xw1+WindowInfoXsize-1, MaxY-yw1));
	pDC->fillRect(rc1, GrayBrush);


	pDC->setPen(DarkPen);
//	pDC->SetTextColor(QColor(0,0,0));

	pDC->moveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));		
	pDC->lineTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));

	pDC->moveTo(xw1+WindowInfoXsize-8,MaxY-(yw1+8));
	pDC->lineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw1+8));

	pDC->moveTo(xw1+WindowInfoXsize-8,MaxY-(yw2-8));
	pDC->lineTo(xw1+WindowInfoXsize-8+4,MaxY-(yw2-8));

	pDC->setPen(WhitePen);

	{
	QPoint qp;
	QRect rcText, rcTextAmax, rcTextAmin;
	char cTextAmin[64], cTextAmax[64];

	sprintf(cTextAmax,"%d",amax);
	GetTextRectangle(cTextAmax, rcTextAmax);

	qp=QPoint(xw1+WindowInfoXsize-rcTextAmax.width()-10,MaxY-yw2);
	rcTextAmax.moveTopLeft(qp);

//	QRect rcc1(QPoint(xw1,MaxY-(yw2-8)),QPoint(xw1+WindowInfoXsize-8-2, MaxY-(yw2-8-13)));


	sprintf(cTextAmin,"%d",amin);
	GetTextRectangle(cTextAmin, rcTextAmin);

	qp=QPoint(xw1+WindowInfoXsize-rcTextAmin.width()-10,MaxY-yw1);
	rcTextAmin.moveBottomLeft(qp);

//	QRect rcc2(QPoint(xw1,MaxY-(yw1+8+13)),QPoint(xw1+WindowInfoXsize-8-2, MaxY-(yw1-8)));

	if(rcTextAmax.bottom()<rcTextAmin.top()+1)
		{
		pDC->drawText(rcTextAmax,Qt::AlignRight, cTextAmin);
		pDC->drawText(rcTextAmin, Qt::AlignRight, cTextAmin);
		}




	QRect rcc3(QPoint(xw1+5,(MaxY-yw2)),QPoint(xw1+WindowInfoXsize-8-2, (MaxY-yw1)));
	qText = wfi->StaName +"."+ wfi->ChanName+"."+ wfi->LCODE;

	pDC->drawText(rcc3,Qt::AlignVCenter,qText);

	}

//**************************************************************

	int nXferBlocks=wfi->xferData.count();
	double ttt=-1.;

	pDC->setPen(YellowPen);
	CDataBlock *xfd;
	for(xfd=wfi->xferData.first();xfd; xfd=wfi->xferData.next())
		{
		double t1packet=xfd->beg;
		double t2packet=xfd->end;
		int nsamp=xfd->nsamp;


		for(int k=0; k<nsamp; ++k)
			{
			long a=xfd->data[k];
			double tt=t1packet+(xfd->sint)*k;
			int x,y;

			if(tt<t1) continue;
			if(tt>t2) break;

			x=(int)((tt-t1)*(x2-x1)/(t2-t1)+x1);

			if(a>amax) a=amax;
			if(a<amin) a=amin;

			y=(a-amin)*(y2-y1)/(amax-amin)+y1;
			y=MaxY-y;
			if(tt-ttt>1.1*xfd->sint)
				{
				pDC->moveTo(x,y);		
				}
			else
				{
				pDC->lineTo(x,y);		
				}
			ttt=tt;
			}
		}
	DrawGaps(nWindow,pDC);

	}
void CRTDisplay::DrawGaps(int nWindow, QPainter *pDC)
	{
	}

void CRTDisplay::OnExpandVert()
	{
	ScreenBitmapCriticalSection.lock();
	Critical.lock();


	short sState=bShiftButtonPressed;//GetAsyncKeyState(VK_SHIFT)&0x0001;


	QPainter DC(&Bitmap);
//*******************************************************

	if(sState==0)
		{
		ApplyExpand(nActiveScreen);
		DrawWindow(nActiveScreen, &DC);
		}
	else
		{
		for(unsigned int i=0; i<WaveformInfo.count(); ++i)
			{
			ApplyExpand(i);
			DrawWindow(i, &DC);
			}
		}
//	DrawWindow(nActiveScreen, &DC);
//*******************************************************
	Critical.unlock();
	ScreenBitmapCriticalSection.unlock();


	if(sState==0)
		{
		CMWindow *wfi = WaveformInfo.at(nActiveScreen);	
		int xw1=wfi->xw1;
		int xw2=wfi->xw2;
		int yw1=wfi->yw1;
		int yw2=wfi->yw2;

		QRect rc(QPoint(xw1,MaxY-yw2),QPoint(xw2,MaxY-yw1));

		repaint(FALSE); //InvalidateRect(rc, FALSE);
		}
	else
		{
		repaint(FALSE);
		}

	}

void CRTDisplay::ApplyExpand(int nWindow)
	{
	CMWindow *wfi = WaveformInfo.at(nWindow);	

	long amax=wfi->amax;
	long amin=wfi->amin;

	long amid, adelta;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;

	adelta/=2;

	amax=amid+adelta;
	amin=amid-adelta;

	if(amax==amin)
		{
		amax++;
		amin--;
		}
	
	wfi->amax=amax;
	wfi->amin=amin;
	}

void CRTDisplay::OnContractVert()
	{
	ScreenBitmapCriticalSection.lock();
	Critical.lock();


	short sState=bShiftButtonPressed;//GetAsyncKeyState(VK_SHIFT)&0x0001;


	QPainter DC(&Bitmap);
//*******************************************************

	if(sState==0)
		{
		ApplyContract(nActiveScreen);
		DrawWindow(nActiveScreen, &DC);
		}
	else
		{
		for(unsigned int i=0; i<WaveformInfo.count(); ++i)
			{
			ApplyContract(i);
			DrawWindow(i, &DC);
			}
		}
//	DrawWindow(nActiveScreen, &DC);
//*******************************************************
	Critical.unlock();
	ScreenBitmapCriticalSection.unlock();


	if(sState==0)
		{
		CMWindow *wfi = WaveformInfo.at(nActiveScreen);	
		int xw1=wfi->xw1;
		int xw2=wfi->xw2;
		int yw1=wfi->yw1;
		int yw2=wfi->yw2;

		QRect rc(QPoint(xw1,MaxY-yw2),QPoint(xw2,MaxY-yw1));

		repaint(FALSE); //InvalidateRect(rc, FALSE);
		}
	else
		{
		repaint(FALSE);
		}

	}

void CRTDisplay::ApplyContract(int nWindow)
	{
	BOOL bfirstcount=TRUE;


	long amax=WaveformInfo.at(nWindow)->amax;
	long amin=WaveformInfo.at(nWindow)->amin;

	long amid, adelta;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;

	adelta*=2;

	if(adelta==0) adelta=1;

	amax=amid+adelta;
	amin=amid-adelta;

	
	WaveformInfo.at(nWindow)->amax=amax;
	WaveformInfo.at(nWindow)->amin=amin;

	}
void CRTDisplay::OnShiftUpDown(int nDirection)
	{
	CMWindow *wfi = WaveformInfo.at(nActiveScreen);	

	ScreenBitmapCriticalSection.lock();
	Critical.lock();

	QPainter DC(&Bitmap);

	int xw1=wfi->xw1;
	int xw2=wfi->xw2;
	int yw1=wfi->yw1;
	int yw2=wfi->yw2;

	QRect rc( QPoint(xw1,MaxY-yw2),QPoint(xw2,MaxY-yw1));



	BOOL bfirstcount=TRUE;


	long amax=wfi->amax;
	long amin=wfi->amin;

	long amid, adelta, adelta1;

	amid=(amax+amin)/2;
	adelta=(amax-amin)/2;
	adelta1=adelta/5;

	if(adelta1==0) adelta1=1;
	if(nDirection>0)
		{
		amid=amid+adelta1;
		}
	else
		{
		amid=amid-adelta1;
		}

	amax=amid+adelta;
	amin=amid-adelta;

	
	wfi->amax=amax;
	wfi->amin=amin;

	DrawWindow(nActiveScreen, &DC);

	Critical.unlock();
	ScreenBitmapCriticalSection.unlock();

	repaint(FALSE);
	}

void CRTDisplay::ClearDisplay()
	{
	ScreenBitmapCriticalSection.lock();
/*    if(Bitmap.m_hObject!=NULL) 
		{
		Bitmap.DeleteObject();
		}*/
	ScreenBitmapCriticalSection.unlock();
/*	InvalidateRect(NULL,TRUE);*/
	repaint(FALSE);
	}



/*void CRTDisplay::OnMouseMove(UINT nFlags, CPoint point) */
void CRTDisplay::mouseMoveEvent ( QMouseEvent * e ) 
	{
	if(!bContinue) return;

	int x=e->x();
	int y=MaxY-e->y();

	
	int nWindow=nWindowFromPoint(x,y);

	if(nWindow!=-1)
		{
		char cBuff[128];
		long amax=WaveformInfo.at(nWindow)->amax;
		long amin=WaveformInfo.at(nWindow)->amin;
		int x1=WaveformInfo.at(nWindow)->x1;
		int x2=WaveformInfo.at(nWindow)->x2;
		int y1=WaveformInfo.at(nWindow)->y1;
		int y2=WaveformInfo.at(nWindow)->y2;


		double ttt=(x-x1)*(t2-t1)/(x2-x1)+t1;
		long aaa=(long)((double)(y-y1)*(amax-amin)/(y2-y1)+amin+0.5);

		sprintf(cBuff,"A:%d T:%s",(long)aaa, util_dttostr(ttt, 1));

		MainFrame *pParent=(MainFrame *)parentWidget();
		QCustomEvent *event = new QCustomEvent(MainFrame::SETTEXT);
		event->setData(new QString(cBuff));
		QApplication::postEvent(pParent, event);
		}

	}
int CRTDisplay::nWindowFromPoint(int x, int y)
	{
	CMWindow *wfi;
	int nWindow=0;
	int x1, x2, y1, y2;

	for(wfi = WaveformInfo.first(); wfi; wfi=WaveformInfo.next())
		{
		x1=wfi->x1;
		x2=wfi->x2;
		y1=wfi->y1;
		y2=wfi->y2;
		if( (y<=y2) && (y>=y1) && (x>=x1) && (x<=x2)) return nWindow;
		++nWindow;
		}
	return -1;
	}
void CRTDisplay::paintEvent(QPaintEvent *)
	{
	QMutexLocker ml(&ScreenBitmapCriticalSection);

	if(bDrawBitmap)
		{
		QPainter paint( this );
		paint.drawPixmap(0,0, Bitmap);
		DrawFocused(&paint);
		QPen dotPen(QColor(0,0,0),1, Qt::DotLine);

		paint.setPen(dotPen);
		for(unsigned int i=0; i<tmmarks.count(); ++i)
			{
			int x=*tmmarks.at(i);
			paint.moveTo(x,MaxY-TimeBarYsize);		
			paint.lineTo(x,1);
			}
		}

	}
void CRTDisplay::customEvent(QCustomEvent *event) {
    switch (event->type()) 
		{
		case AutoScale0:
			OnAutoScale(0);
			break;
		case AutoScale1:
			OnAutoScale(1);
			break;
		case ExpandVert:
			OnExpandVert();
			break;
		case ContractVert:
			OnContractVert();
			break;
		case ShiftUp:
			OnShiftUpDown(1);
			break;
		case ShiftDown:
			OnShiftUpDown(0);
			break;
		case Update:
			OnTimer();
			break;
		case Draw:
			DrawWaveforms();
			break;
		default:
			{
			qWarning("Unknown custom event type: %d", event->type());
			}
		}
	}
