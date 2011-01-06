// IsiInterruptThread.cpp: implementation of the IsiInterruptThread class.
//
//////////////////////////////////////////////////////////////////////

#include "IsiInterruptThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IsiInterruptThread::IsiInterruptThread(unsigned int stackSize):QThread(stackSize)
	{

	}

IsiInterruptThread::~IsiInterruptThread()
	{

	}
void IsiInterruptThread::Init(ISI **p)
	{
	pisi = p;;
	}

void IsiInterruptThread::run()
	{
	while(bContinue)
		{
		sleep(2);
		}
	if(pisi!=NULL)
		{
		isiSetFlag(*pisi, ISI_FLAG_BREAK);
		}
	}
