#pragma ident "$Id: AVERAGE.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// Average.cpp: implementation of the CAverage class.
//
//////////////////////////////////////////////////////////////////////

#include "Average.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAverage::CAverage(int nQueue)
	{
	nQueueSize=nQueue;
	dBuff=new double[nQueue];
	for(int i=0; i< nQueueSize; ++i) dBuff[i]=0.0;
	dSum=0.0;
	dSum2=0.0;
	}

double CAverage::Push(double d)
	{
	double dFirst=dBuff[0];
	for(int i=0; i< nQueueSize-1 ; ++i) dBuff[i]=dBuff[i+1];
	dBuff[nQueueSize-1]=d;
	dSum=dSum+(d-dFirst)/(double)nQueueSize;
	dSum2=dSum2+(d*d-dFirst*dFirst)/(double)nQueueSize;
	return (dSum2-dSum*dSum);
	}

CAverage::~CAverage()
	{
	delete[]dBuff;
	}

/* Revision History
 *
 * $Log: AVERAGE.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
