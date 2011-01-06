// DrawWaveformThread.h: interface for the DrawWaveformThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DRAWWAVEFORMTHREAD_H_)
#define _DRAWWAVEFORMTHREAD_H_

#include <qthread.h>
#include "MainFrame.h"

class DrawWaveformThread : public QThread  
{
public:
	DrawWaveformThread(unsigned int stackSize = 0);
	virtual ~DrawWaveformThread();
	void Init(MainFrame *p);
	virtual void run();
	MainFrame *pMF;

};

#endif // !defined(_DRAWWAVEFORMTHREAD_H_)
