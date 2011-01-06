// GettingDataThread.h: interface for the GettingDataThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_GETTINGDATATHREAD_H_)
#define _GETTINGDATATHREAD_H_

#include <qthread.h>
#include "isiTapResults.h"

class GettingDataThread : public QThread  
{
public:
	GettingDataThread();
	virtual ~GettingDataThread();
	void Init(isiTapResults *p);
	virtual void run();
	isiTapResults *pisiTapResults;

};

#endif // !defined(_GETTINGDATATHREAD_H_)
