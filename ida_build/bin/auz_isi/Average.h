#pragma ident "$Id: Average.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// Average.h: interface for the CAverage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVERAGE_H__F2BC8764_5428_11D2_84D3_000000000000__INCLUDED_)
#define AFX_AVERAGE_H__F2BC8764_5428_11D2_84D3_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CAverage  
{
public:
	CAverage(int);
	double Push(double d);
	double *dBuff;
	int nQueueSize;
	double dSum,dSum2;
	virtual ~CAverage();

};

#endif // !defined(AFX_AVERAGE_H__F2BC8764_5428_11D2_84D3_000000000000__INCLUDED_)

/* Revision History
 *
 * $Log: Average.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
