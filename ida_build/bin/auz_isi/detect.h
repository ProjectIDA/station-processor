#pragma ident "$Id: detect.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// Detect.h: interface for the CDetect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETECT_H__C5EF7A31_7BD7_11D3_8BF6_008048C20040__INCLUDED_)
#define AFX_DETECT_H__C5EF7A31_7BD7_11D3_8BF6_008048C20040__INCLUDED_


#include "IIRfilter.h"
#include "AUZEvent.h"
#include "detector.h"
#include <string>
#include <vector>
#include "auzinfo.h"

using namespace std;

class CDetect  
{
public:
	CDetect();
	virtual ~CDetect();
	int Init(AUZINFO &auzinfo);
	int Stop();
	double GetEventTime();
	double GetLastCountTime();
	static BOOL IsStop();
	double StreamSpS(){return dStreamSpS;};
	double dLTA;
	double dSTA;
	double dThreshold;
	double dStreamSpS;
	void SaveEventInfoToDB(double dEventTime);
	void SetLastCountTime(double dTime);
	void SetLastEventTime(double dTime);
	void SetEventFlag();
	void ResetEventFlag();
	bool SingleLoop(long *buff, int nCounts, double dTbeg);


	BOOL IsEvent();
	float fr1, fr2;
	int filter_order;
	int nThreadNumber;
	BOOL bSound;
	int nSleepTime;
	BOOL bDbConnected;
	vector <AUZEvent *> AUZEventList;

	float fcalib, fcalper;
	CDetector Detector;
	CIIRfilter iifilter;
	CIIRfilter iifilter_amp;


public:
	string sta;
	string chan;
	string lcode;
private:
	static BOOL bStop;
	int LastError;
	BOOL bEventFlag;
	double dLastCountTime;
	double dLastEventTime;
	int nInit;
	long nCountInit;
	};

#endif // !defined(AFX_DETECT_H__C5EF7A31_7BD7_11D3_8BF6_008048C20040__INCLUDED_)

/* Revision History
 *
 * $Log: detect.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
