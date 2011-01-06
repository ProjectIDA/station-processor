#include <qapplication.h>
#include "StopAcquisitionThread.h"
#include "RTDisplay.h"
#include "platform.h"
#include "CRTDglob.h"
#include "isi.h"
#include "util.h"
#include "MainFrame.h"
#include "GettingDataThread.h"



static StopAcquisitionThread StopT;
static GettingDataThread GettingDataT;

void StopDatavisualization(MainFrame *pM);



void StopAcquisition(MainFrame *p)
	{
	StopT.Init(p);
	StopT.start(QThread::NormalPriority);
	}
void StartAcquisition(MainFrame *p)
	{
	GettingDataT.Init(p);
	GettingDataT.start(QThread::NormalPriority);
	}
void WaitForStop()
	{
	GettingDataT.wait();
	}