#pragma ident "$Id: AUZEvent.cpp,v 1.3 2008/02/01 18:23:44 akimov Exp $"
// AUZEvent.cpp: implementation of the AUZEvent class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "AUZEvent.h"
#include "mail.h"
#include "util.h"
#include "auzglobals.h"


//#include "log.h"



void DFT(short dir, long m, double *x1, double *y1);
void RemoveLinearTrend(double  *y, float dt, long qty, double *a, double *b);
void RemoveMean(double *y, long nQty);


AUZEvent::AUZEvent(const char *pSta, const char *pChan, const char *pLC, double ttt, float fSNR, float calib, float calper)
	{
	sta  = pSta;
	chan = pChan;
	lc   = pLC;
	dbuff=NULL;
	t = ttt;
	bdone = false;
	nMaxCount = 0;
	nCount   = 0;
	tlast = 0;
	ampl=-1; Tampl=-1;
	fcalib  = calib;
	fcalper = calper;
	SNR = fSNR;
	}

bool AUZEvent::Init(double dSpS, double dSec)
	{
	if(dbuff!=NULL) return false;
	if(dSpS<=0 || dSpS>10000.)
		{
		SaveInfo();
		return false;
		}
	if(dSec<=0 || dSec>600.  )
		{
		SaveInfo();
		return false;
		}

	sint = 1./dSpS;

	nMaxCount = (int)(dSpS*dSec);
	dbuff = new double[nMaxCount];
	if(dbuff==NULL) return false;
	return true;
	}

AUZEvent::~AUZEvent()
	{
	if(dbuff!=NULL) delete [] dbuff;
	}

bool AUZEvent::AddCount(double d, double t)
	{
	if(bdone) return false;
	if(nCount==0)
		{
		nCount = 1;
		dbuff[0] = d;
		tlast    = t;
		}
	else
		{
		if( fabs(t-tlast)< sint*1.5 )
			{
			dbuff[nCount++] = d;
			tlast    = t;
			if(nCount == nMaxCount) SaveInfo();
			}
		else
			{
			SaveInfo();
			}
		}
	return true;
	}
void AUZEvent::SaveInfo()
	{
	CalcAmplPer();
	PutEventInfoToMailQueue();
	bdone = true;
	}

BOOL AUZEvent::CalcAmplPer()
	{
	int i,j, n1, n2, nMax=0, nMin=0, nMax0=0, nMin0=0 ;
	double ty=0.0;
	double sM=0, aS;

	double *aa, *bb;
	double a,b;

	double  dAmin, dAmax ;

	aa=new double[nCount];
	bb=new double[nCount];
	for(i=0; i<nCount; ++i)
		{
		aa[i]=dbuff[i];
		bb[i]=0.;
		}

	RemoveLinearTrend(aa, sint, nCount, &a, &b);

	RemoveMean(aa, nCount);
	DFT(1, nCount, aa, bb);

	for(i=2; i<nCount/2; ++i)
		{
		aS = sqrt(aa[i]*aa[i]+bb[i]*bb[i]);
		if(sM<aS)
			{
			sM=aS;
			ty=(nCount*sint)/i;
			}
		}

	delete[] aa;
	delete[] bb;

	int n=(int)(ty/sint+0.5);

	ampl = 0;

	for(i=0; i<nCount; ++i)
		{
		n1=i;
		n2=(int)(n1+n*2./3.);

		dAmin=dAmax=dbuff[n1];

		for(j=n1+1; j<=n2; ++j)
			{
			if(j>=nCount) break;
			if(dbuff[j]<dAmin) 
				{
				dAmin = dbuff[j];
				}
			if(dbuff[j]>dAmax) 
				{
				dAmax = dbuff[j];
				}
			}

		if(ampl<(dAmax-dAmin))
			{
			ampl=dAmax-dAmin;
			nMin=n1;
			nMax=n2;
			}

		}

	Tampl = ty;

	ampl = ampl/2.;

	double acount = ampl;

	if(fcalib<=0)
		{
		ampl= -1;
		}
	else
		{
		ampl=(Tampl)*ampl*fcalib/(fcalper);
		}

	ampl= -1;

/****************************************************/
	char lpMsg[128]="";
	time_t d=(time_t)t;
	double logat = 0;
	if( (ampl>0) && (Tampl>0) )
		{ logat = log10(ampl/Tampl);
		}
//	CTime ttt(d); sprintf(lpMsg, "Sta: %s Event time: %s amp(counts):%.6f amp(mkm):%.6f T(s):%.6f calib=%.6f", sta.c_str(), ttt.Format("%H:%M:%S %d.%m.%Y"), acount, ampl, Tampl, calib);
//	WriteStringToLogFile(lpMsg);


	return TRUE;
	}

void AUZEvent::PutEventInfoToMailQueue()
	{

	AddArrivalToMailQueue(sta.c_str(), t, 1.,  SNR, ampl, Tampl);

	int yr,mnth,da,day,hh,mm,sc,ms;

	utilTsplit(t, &yr, &da, &hh, &mm, &sc, &ms);
	utilJdtomd(yr, da, &mnth, &day);

	char cMsg[1024];
	sprintf(cMsg, "%4s %3s %2s %4d/%02d/%02d %02d:%02d:%02d.%03d %.5f %.1f", sta.c_str(), chan.c_str(), lc.c_str(), yr, mnth, day, hh, mm, sc, ms, ampl, Tampl); 
	if(auzglobals::debug) util_log(1, cMsg);
	if(!auzglobals::bd) printf("\n%s", cMsg);


	}

/* Revision History
 *
 * $Log: AUZEvent.cpp,v $
 * Revision 1.3  2008/02/01 18:23:44  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.2  2008/01/27 16:58:11  akimov
 * removed some unused (commented) stuff
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
