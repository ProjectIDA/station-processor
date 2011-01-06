// IsiInterruptThread.h: interface for the IsiInterruptThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISIINTERRUPTTHREAD_H__3A80311E_2E1F_4895_BFDF_C206CC389295__INCLUDED_)
#define AFX_ISIINTERRUPTTHREAD_H__3A80311E_2E1F_4895_BFDF_C206CC389295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qthread.h>
#include "CRTDglob.h"
#include "isi.h"
#include "util.h"


class IsiInterruptThread : public QThread  
{
public:
	IsiInterruptThread(unsigned int stackSize = 0);
	virtual ~IsiInterruptThread();
	void Init(ISI **p);

protected:
	ISI **pisi;
	void run();

};

#endif // !defined(AFX_ISIINTERRUPTTHREAD_H__3A80311E_2E1F_4895_BFDF_C206CC389295__INCLUDED_)
