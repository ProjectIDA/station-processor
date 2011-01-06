#pragma ident "$Id: detect.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// Detect.cpp: implementation of the CDetect class.
//
//////////////////////////////////////////////////////////////////////

#include "detect.h"
#include "detector.h"
#include <math.h>



//extern vector <CDetect *> DetectorsList;


//void WriteStringToLogFile(LPCSTR lpMsg, bCreateOnly);



void AddCountToEvents(CDetect *pCDetect, double d, double tc)
	{
	for(int m=0; m<pCDetect->AUZEventList.size(); ++m)
		{
		AUZEvent *p  =	pCDetect->AUZEventList[m];
		p->AddCount( d, tc);
		}

	}

void RemoveSavedEvents(CDetect *pCDetect)
	{
	int m=0; 

	while(m<pCDetect->AUZEventList.size())
		{
		AUZEvent *p  =	pCDetect->AUZEventList[m];
		if(p->bdone)
			{
			pCDetect->AUZEventList.erase(pCDetect->AUZEventList.begin()+m);
			delete p;
			}
		else
			{
			m++;
			}
		}

	}

BOOL CDetect::bStop=FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDetect::CDetect()
{
	bEventFlag=FALSE;
	dLastEventTime=0.;
	bDbConnected=FALSE;
	nCountInit=0;
}

CDetect::~CDetect()
{
}

int CDetect::Init(AUZINFO &auzinfo) //CString StationName, CString ChanName, CString csIniFilePath)
{

	dLTA=auzinfo.flta;
	dSTA=auzinfo.fsta;
	dThreshold=auzinfo.threshold;
	fr1=auzinfo.fr1;
	fr2=auzinfo.fr2;
	fcalib=auzinfo.calib;
	fcalper=auzinfo.calper;
	filter_order=auzinfo.order;

	sta      = auzinfo.sta;
	chan     = auzinfo.chan;
	lcode      = auzinfo.loc;

	dStreamSpS = auzinfo.sps;

	nInit=(int)(2*dStreamSpS*max(60., dLTA));
	Detector.Init(dStreamSpS, (int)(dLTA*dStreamSpS), (int)(dSTA*dStreamSpS), dThreshold);
	iifilter.iir_design(filter_order, "BP", fr1, fr2, 1.f/dStreamSpS);
	iifilter_amp.iir_design(2, "BP", 0.6f, 1.9f, 1.f/dStreamSpS);

	printf("\n%5s %5s %5s %f %f %f %f %f %f %d", sta.c_str(), chan.c_str(), lcode.c_str(), dStreamSpS, dLTA, dSTA, dThreshold, fr1, fr2, filter_order); 

	bStop=FALSE;


//	CTime tm=CTime::GetCurrentTime();

//	APR.Init(c.m_hdbc, sta.c_str(), chan.c_str(), lcode.c_str());

	return 0;
}

int CDetect::Stop()
{
	bStop=TRUE;
//	DWORD dwRes=WaitForSingleObject(pThread->m_hThread, 60000);
	return 0;
}

double CDetect::GetEventTime()
	{
	return dLastEventTime;
	}

double CDetect::GetLastCountTime()
	{
	return dLastCountTime;
	}

BOOL CDetect::IsStop()
	{
	return bStop;
	}

bool CDetect::SingleLoop(long *buff, int nCounts, double dTbeg)
	{
	double d,d_a;
	int   n;
	double dTend;

	for (n=0; n<nCounts; ++n)
		{
		d_a=iifilter_amp.apply_iir(buff[n]);
		d=iifilter.apply_iir(buff[n]);
		double tc = dTbeg+(double)n/StreamSpS();

		RemoveSavedEvents(this);
		AddCountToEvents(this, d_a, tc);


		if (nCountInit<=nInit) {++nCountInit; Detector.Detect(d); continue;}
		if (Detector.Detect(d))
			{
			if(!IsEvent())
				{
				double dTT=dTbeg+(double)n/StreamSpS() - Detector.GetTsta()/2.;
				SetLastEventTime(dTT);
				SetEventFlag();
//					pCDetect->SaveEventInfoToDB(dTT);
					
					AUZEvent *p = new AUZEvent(sta.c_str(), chan.c_str(), lcode.c_str(), dTT, Detector.GetSNR(), fcalib, fcalper);
//AUZEvent::AUZEvent(const char *pSta, const char *pChan, const char *pLC, const double ttt, float fcb, float fcp)

				
					AUZEventList.push_back(p);
					p->Init(StreamSpS(), 5);
					p->AddCount( d_a, tc);

					}
				}
			else
				{
				ResetEventFlag();
				}
			}
	if(nCounts>0)
		{
		dTend = dTbeg+(double)nCounts/StreamSpS();
		SetLastCountTime(dTend);
		}
	return true;

	}




void CDetect::SetLastCountTime(double dTime)
	{
	dLastCountTime=dTime;
	}
void CDetect::SetLastEventTime(double dTime)
	{
	dLastEventTime=dTime;
	}
void CDetect::SetEventFlag()
	{
	bEventFlag=TRUE;
	}
void CDetect::ResetEventFlag()
	{
	bEventFlag=FALSE;
	}
BOOL CDetect::IsEvent()
	{
	return bEventFlag;
	}

/* Revision History
 *
 * $Log: detect.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
