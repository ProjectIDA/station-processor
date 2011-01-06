#pragma ident "$Id: detector.cpp,v 1.3 2008/02/01 18:23:44 akimov Exp $"
// Detector.cpp: implementation of the CDetector class.
//
//////////////////////////////////////////////////////////////////////
#include "detector.h"
#include <math.h>


CDetector::CDetector(int nLTA, int nSTA, double dThreshold)
	{
	nQueueSize = nSTA+nLTA;
    nSta=nSTA;
    nLta=nLTA;
    nQty=0;
	dL2S=dThreshold;
	bInit = false;
	dBuff = NULL;
	dSum_sta=dSum2_sta=dSum_lta=dSum2_lta=0.;
	m_snr = 0.;
	}
CDetector::CDetector()
	{
	nQueueSize = 0;
    nSta=0;
    nLta=0;
    nQty=0;
	dL2S=0;
	bInit = false;
	dBuff = NULL;
	dSum_sta=dSum2_sta=dSum_lta=dSum2_lta=0.;
	m_snr = 0.;
	}


bool CDetector::Init(double dSpS)
{
	if(bInit) return true;
	dBuff = new double[nQueueSize]; 
	if(dBuff==NULL) return false;

	for(int i=0; i<nLta+nSta; ++i)
	{
		dBuff[i] = 0.;
	}
	
	bInit = true;
	nFlagMaxTime=(int)(5.*dSpS);
	nFlagTime = 0;
	m_tsta = nSta/dSpS;
	
	return true;
}

bool CDetector::Init(double dSpS, int nLTA, int nSTA, double dThreshold)
{
	if(bInit) return true;

	nQueueSize = nSTA+nLTA;
    nSta=nSTA;
    nLta=nLTA;
    nQty=0;
	dL2S=dThreshold;
	bInit = false;
	dBuff = NULL;
	dSum_sta=dSum2_sta=dSum_lta=dSum2_lta=0.;
	m_snr = 0.;


	dBuff = new double[nQueueSize]; 
	if(dBuff==NULL) return false;

	for(int i=0; i<nLta+nSta; ++i)
	{
		dBuff[i] = 0.;
	}
	
	bInit = true;
	nFlagMaxTime=(int)(5.*dSpS);
	nFlagTime = 0;
	m_tsta = nSta/dSpS;
	
//	printf("\n<sta=%d, lta=%d, sps=%f, %f>",nSta, nLta, dSpS, dThreshold);

	return true;
}



CDetector::~CDetector()
{
	if(dBuff!=NULL) delete [] dBuff;
}

BOOL CDetector::Detect(double d)
{
	if(!bInit) return FALSE;
    double dSta;
    double dLta;

	double d1 = dBuff[0];
	double d2 = dBuff[nLta];

	for (int i=0; i<nQueueSize-1; ++i) dBuff[i]=dBuff[i+1];
	dBuff[nQueueSize-1]=d;

	dSum_sta=dSum_sta+(d-d2)/(double)nSta;
	dSum2_sta=dSum2_sta+(d*d-d2*d2)/(double)nSta;
	m_dsta = dSta = dSum2_sta-dSum_sta*dSum_sta;

	dSum_lta=dSum_lta+(d2-d1)/(double)nLta;
	dSum2_lta=dSum2_lta+(d2*d2-d1*d1)/(double)nLta;
	dLta = dSum2_lta-dSum_lta*dSum_lta;


	if(dLta>0.)
		m_snr = dSta/dLta;
	else
		m_snr = 99999999.;

//	printf("\n<sta=%f, lta=%f, %f, %f>",dSta, dLta, m_snr,d);

	if(nQty>3*nQueueSize)
	{
		if( dSta>dLta*dL2S )
		{
			nFlagTime=nFlagMaxTime;
			return TRUE;
		}
		else
		{
			if(nFlagTime==0) return FALSE;
			--nFlagTime;
			return TRUE;
		}
	}
	else
	{
		++nQty;
	}

	return FALSE;
}


//	dSum=dSum+(d-dFirst)/(double)nQueueSize;
//	dSum2=dSum2+(d*d-dFirst*dFirst)/(double)nQueueSize;

double CDetector::GetSTA()
	{
	return m_dsta;
	}
double CDetector::GetTsta()
	{
	return m_tsta;
	}
float CDetector::GetSNR()
	{
	return m_snr;
	}
	

/* Revision History
 *
 * $Log: detector.cpp,v $
 * Revision 1.3  2008/02/01 18:23:44  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.2  2008/01/27 17:03:18  akimov
 * from aap.zip 01/27/2008
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
