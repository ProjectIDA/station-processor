#pragma ident "$Id: AUZEvent.h,v 1.2 2008/02/01 18:23:44 akimov Exp $"
// AUZEvent.h: interface for the AUZEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUZEVENT_H__39EDF5F9_119C_4F27_A6EF_D9E29D387EDC__INCLUDED_)
#define AFX_AUZEVENT_H__39EDF5F9_119C_4F27_A6EF_D9E29D387EDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "platform.h"
#include <string>
using namespace std;

class AUZEvent  
	{
public:
	AUZEvent(const char *pSta, const char *pChan, const char *pLC, double ttt, float fSNR, float calib, float calper);
	virtual ~AUZEvent();
	bool Init(double dSpS, double dSec);
	void SaveInfo();
	BOOL CalcAmplPer();
	bool AddCount(double d, double t);
	void PutEventInfoToMailQueue();


	string sta,chan,lc;
	double *dbuff;
	double t, tlast, sint;
	long nCount, nMaxCount;

	float ampl, Tampl;
	float fcalib, fcalper;
	bool bdone;
	float SNR;
	};

#endif // !defined(AFX_AUZEVENT_H__39EDF5F9_119C_4F27_A6EF_D9E29D387EDC__INCLUDED_)

/* Revision History
 *
 * $Log: AUZEvent.h,v $
 * Revision 1.2  2008/02/01 18:23:44  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
