// GettingDataThread.h: interface for the GettingDataThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GETTINGDATATHREAD_H_)
#define _GETTINGDATATHREAD_H_

#include <qthread.h>
#include "MainFrame.h"
#include "DrawWaveformThread.h"
#include "IsiInterruptThread.h"


class GettingDataThread : public QThread  
{
public:
	GettingDataThread();
	virtual ~GettingDataThread();
	void Init(MainFrame *p);
	virtual void run();
	MainFrame *pMF;

private:
	IsiInterruptThread    IsiInterruptT;
	DrawWaveformThread    DrawT;

private:
	void StopDatavisualization();

};

#endif // !defined(_GETTINGDATATHREAD_H_)
