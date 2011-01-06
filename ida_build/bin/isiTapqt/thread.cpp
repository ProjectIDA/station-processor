#include "isiTapResults.h"
#include "GettingDataThread.h"


static GettingDataThread GettingDataT;

void StartGettingData(isiTapResults *p)
	{
	GettingDataT.Init(p);
	GettingDataT.start(QThread::NormalPriority);
	}
