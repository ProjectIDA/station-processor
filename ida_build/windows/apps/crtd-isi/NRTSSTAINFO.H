// NRTSSTAINFO.h: interface for the CNRTSSTAINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "NRTSCHANINFO.h"

class CNRTSSTAINFO  
{
public:
	CNRTSSTAINFO();
	CNRTSSTAINFO(CString StaName);
	void AddChan(CString ChanName, CString csLCODE, double dSpS, BOOL);
	virtual ~CNRTSSTAINFO();

public:
	CString Sta;
	CArray<CNRTSCHANINFO *, CNRTSCHANINFO *> ChanInfo; // information array of all station

};

#endif // !defined(AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
