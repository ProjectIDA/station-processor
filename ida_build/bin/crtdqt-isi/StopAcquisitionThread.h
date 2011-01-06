// StopAcquisitionThread.h: interface for the StopAcquisitionThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_STOPACQUISITIONTHREAD_H_)
#define _STOPACQUISITIONTHREAD_H_

#include <qthread.h>
#include "MainFrame.h"

class StopAcquisitionThread : public QThread  
{
public:
	StopAcquisitionThread(unsigned int stackSize = 0);
	void Init(MainFrame *p);
	virtual ~StopAcquisitionThread();
	virtual void run();
	MainFrame *pMF;

};

#endif // !defined(_STOPACQUISITIONTHREAD_H_)
