// MWindow.h: interface for the CMWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MWINDOW_H__AA568B9A_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_MWINDOW_H__AA568B9A_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WfdiscRecord.h"



class CMWindow  
{
public:
	CMWindow();
	CMWindow(CString sSta, CString sChan, double dSpS);
	void AddWfdRecord(wfdisc wfd);
	virtual ~CMWindow();
	void AutoScale(double t1=-1, double t2=-1);

public:
	int x1,x2,y1,y2;
	int xw1,xw2,yw1,yw2;
	CString StaName;
	CString ChanName;
	double dSpS;
	long amin,amax;
	long amin_1, amax_1;
	CArray<CWfdiscRecord *, CWfdiscRecord *> wfdrec;

};

#endif // !defined(AFX_MWINDOW_H__AA568B9A_08E2_11D3_B68A_941D97000000__INCLUDED_)
