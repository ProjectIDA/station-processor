#pragma ident "$Id: detector.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// Detector.h: interface for the CDetector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DETECTOR_H__E6D528F3_7C72_11D3_B615_682B67000000__INCLUDED_)
#define AFX_DETECTOR_H__E6D528F3_7C72_11D3_B615_682B67000000__INCLUDED_

#include "platform.h"


class CDetector  
{
public:
	CDetector(int nLTA, int nSTA, double dThreshold);
	CDetector();

	BOOL Detect(double d);
	bool Init(double);
	bool Init(double dSpS, int nLTA, int nSTA, double dThreshold);

	virtual ~CDetector();
	double GetSTA();
	double GetTsta();
	float GetSNR();

private:
	double nSpS;
	long nLta,nSta,nQueueSize;
	long nQty;
	double dL2S,tlta,m_tsta;
	double *dBuff;
	double dSum_sta,dSum2_sta,dSum_lta,dSum2_lta;
	bool bInit;
	int nFlagMaxTime, nFlagTime;
	double m_dsta;
	float m_snr;
};

#endif // !defined(AFX_DETECTOR_H__E6D528F3_7C72_11D3_B615_682B67000000__INCLUDED_)

/* Revision History
 *
 * $Log: detector.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
