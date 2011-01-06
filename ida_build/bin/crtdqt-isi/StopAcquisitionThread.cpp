// StopAcquisitionThread.cpp: implementation of the StopAcquisitionThread class.
//
//////////////////////////////////////////////////////////////////////
#include <qapplication.h>
#include "StopAcquisitionThread.h"
#include "CRTDglob.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void StopAcquisition(MainFrame *p);


StopAcquisitionThread::StopAcquisitionThread(unsigned int stackSize ):QThread(stackSize)
	{
	}
void StopAcquisitionThread::Init(MainFrame *p)
	{
	pMF = p;
	}

StopAcquisitionThread::~StopAcquisitionThread()
	{
	}

void StopAcquisitionThread::run()
	{
	void WaitForStop();
//	pMF->toolBar->SetToolbarState(2);
	pMF->toolBar->setDisabled(true);
	bContinue = FALSE;
	WaitForStop();
	pMF->toolBar->setDisabled(false);
	QCustomEvent *event = new QCustomEvent(MainFrame::STOP);
	QApplication::postEvent(pMF, event);
	}