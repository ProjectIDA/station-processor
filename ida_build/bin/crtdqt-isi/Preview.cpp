// Preview.cpp : implementation file
//
#include <qapplication.h>
#include "Preview.h"
#include "CRTDglob.h"
#include <math.h>
#include "MainFrame.h"
#include "util.h"
#include <qpainter.h> 

/////////////////////////////////////////////////////////////////////////////
// CPreview

CPreview::CPreview(CRTDisplay *pDisplay, QWidget * parent = 0, const char * name = 0, WFlags f = 0)
:QWidget(parent, name , f)
	{
	this->pDisplay=pDisplay;
	bDrawBitmap=FALSE;
	setMouseTracking(TRUE);
	}

CPreview::~CPreview()
	{
	}

/////////////////////////////////////////////////////////////////////////////
// CPreview message handlers

void CPreview::paintEvent(QPaintEvent *) 
	{
	if(bContinue)
		{
		ScreenBitmapCriticalSection.lock();
		if(bDrawBitmap)
			{
			QPainter paint( this );
			paint.drawPixmap(0,0, Bitmap);
			}
		ScreenBitmapCriticalSection.unlock();
		}
	}

void CPreview::resizeEvent(QResizeEvent * )
	{
	DrawPreviewBitmap();
	}

void CPreview::DrawPreviewBitmap()
	{
	if(!bContinue) return;


	MaxX=width();
	MaxY=height();
	QPixmap tmpBitmap(MaxX, MaxY);
	QPainter DC(&tmpBitmap);
	QRect rc=rect();

	x1=4;
	x2=MaxX-4;
	y1=4;
	y2=MaxY-4;

	xw1=1;
	xw2=MaxX;
	yw1=0;
	yw2=MaxY;


    QBrush WhiteBrush(QColor(255,255,255));
    QBrush GrayBrush(QColor(192,192,192));
    QBrush BlueBrush(QColor(0,0,255));

    QPen WhitePen(QColor(255,255,255));
	QPen DarkPen(QColor(0,0,0));
    QPen BluePen(QColor(0,0,255));
    QPen YellowPen(QColor(255,255,0));
    QPen WhitePen2(QColor(255,255,255),2);
    QPen GrayPen2(QColor(128,128,128),2);


//********************************************************************
	DC.setPen(BluePen);
	DC.fillRect(rc, BlueBrush);
//	DC.SelectObject(CFont::FromHandle((HFONT)GetStockObject(ANSI_VAR_FONT)));


	int xl=(int)((pDisplay->t1 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
	int xr=(int)((pDisplay->t2 - pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);

	DC.setPen(WhitePen2);

	DC.moveTo(xw1,MaxY-yw2);		
	DC.lineTo(xw2,MaxY-yw2);

	DC.moveTo(xw1,MaxY-yw2);		
	DC.lineTo(xw1,MaxY-yw1);

	DC.setPen(GrayPen2);

	DC.moveTo(xw2,MaxY-yw1);		
	DC.lineTo(xw2,MaxY-yw2);

	DC.moveTo(xw2,MaxY-yw1);		
	DC.lineTo(xw1,MaxY-yw1);

	DrawWaveform(DC);
	
	DC.setPen(WhitePen2);
	DC.moveTo(xl,MaxY-y1);		
	DC.lineTo(xr,MaxY-y1);
	DC.lineTo(xr,MaxY-y2);		
	DC.lineTo(xl,MaxY-y2);		
	DC.lineTo(xl,MaxY-y1);		


//********************************************************************

	ScreenBitmapCriticalSection.lock();
	Bitmap=tmpBitmap;
	bDrawBitmap=TRUE;
	ScreenBitmapCriticalSection.unlock();
	repaint(FALSE);
	}
void CPreview::DrawWaveform(QPainter &DC)
	{

	int nActiveScreen=pDisplay->nActiveScreen;

	if(nActiveScreen==-1) return;


	double amin=-3000., amax=3000.;
	double ttt=-1.;
    QPen YellowPen(QColor(255,255,0));
	CDataBlock *xfd;
	CMWindow *wfi;
	DC.setPen(YellowPen);

	pDisplay->Critical.lock();

	wfi = pDisplay->WaveformInfo.at(nActiveScreen);

// find max and min
	BOOL bfirstcount=TRUE;

	for(xfd = wfi->xferData.first();xfd;xfd = wfi->xferData.next())
		{
		double t1packet=xfd->beg;
		double t2packet=xfd->end;
		int nsamp=xfd->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=xfd->data[k];
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
	for(xfd = wfi->xferData.first();xfd;xfd = wfi->xferData.next())
		{
		double t1packet=xfd->beg;
		double t2packet=xfd->end;
		int nsamp=xfd->nsamp;
		for(int k=0; k<nsamp; ++k)
			{
			long a=xfd->data[k];
			double tt=t1packet+xfd->sint*k;
			int x,y;

			if(tt<pDisplay->tB1) continue;
			if(tt>pDisplay->tB2) break;

			x=(int)((tt-pDisplay->tB1)*(x2-x1)/(pDisplay->tB2-pDisplay->tB1)+x1+0.5);
			y=(int)((a-amin)*(y2-y1)/(amax-amin)+y1+0.5);
			y=MaxY-y;
			if(tt-ttt>1.1*xfd->sint)
				{
				DC.moveTo(x,y);		
				}
			else
				{
				DC.lineTo(x,y);		
				}
			ttt=tt;
			}
		}
	pDisplay->Critical.unlock();
	}

void CPreview::mousePressEvent ( QMouseEvent *e )  
	{
	if(!bContinue) return;
	int x=e->x();
	int y=MaxY-e->y();
	int i=0;

	if(e->button()!= QMouseEvent::LeftButton) return;
	if(pDisplay->nActiveScreen==-1) return;
	pDisplay->Critical.lock();
	
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

	pDisplay->Critical.unlock();


	DrawPreviewBitmap();

	QCustomEvent *event = new QCustomEvent(CRTDisplay::Draw);
	QApplication::postEvent(pDisplay, event);

	}
void CPreview::Stop()
	{
	ScreenBitmapCriticalSection.lock();
	bDrawBitmap=FALSE;
	ScreenBitmapCriticalSection.unlock();
	repaint(TRUE);
	}

void CPreview::mouseMoveEvent ( QMouseEvent * e ) 
	{
	if(!bContinue) return;

	int x=e->x();
	int y=MaxY-e->y();


	if(x>=x1 && x<=x2)
		{
		double ttt=(x-x1)*(pDisplay->tB2-pDisplay->tB1)/(x2-x1)+pDisplay->tB1;
		char cBuff[64];	
		strcpy(cBuff, util_dttostr(ttt, 1));
		MainFrame *pParent=(MainFrame *)parentWidget();
		QCustomEvent *event = new QCustomEvent(MainFrame::SETTEXT);
		event->setData(new QString(cBuff));
		QApplication::postEvent(pParent, event);
		}

	}
void CPreview::customEvent(QCustomEvent *event) {
    switch (event->type()) 
		{
		case DrawPreview:
			DrawPreviewBitmap();
			repaint(FALSE);
			break;
		default:
			{
			qWarning("Unknown custom event type: %d", event->type());
			}
		}
	}
