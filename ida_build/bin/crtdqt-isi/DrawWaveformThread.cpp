// DrawWaveformThread.cpp: implementation of the DrawWaveformThread class.
//
//////////////////////////////////////////////////////////////////////
#include <qapplication.h> 
#include "DrawWaveformThread.h"
#include "CRTDglob.h"
#include "Preview.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DrawWaveformThread::DrawWaveformThread(unsigned int stackSize):QThread(stackSize)
	{
	}

DrawWaveformThread::~DrawWaveformThread()
	{
	}

void DrawWaveformThread::Init(MainFrame *p)
	{
	pMF = p;
	}

void DrawWaveformThread::run()
	{
	QCustomEvent *event;
	int nTimerCount=nRefreshTime;

	CRTDisplay *pRTDisplay = pMF->pDisplay;
	CPreview *pPreview      = pMF->pPreview;

	while(bContinue)
		{
		sleep(1);
		if(nTimerCount++==nRefreshTime)
			{
			event = new QCustomEvent(CRTDisplay::Update);
			QApplication::postEvent(pRTDisplay, event);
			event = new QCustomEvent(CPreview::DrawPreview);
			QApplication::postEvent(pPreview, event);
			nTimerCount=0;
			}

		}
	return;
	}
