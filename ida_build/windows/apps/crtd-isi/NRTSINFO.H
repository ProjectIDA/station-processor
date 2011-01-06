// NRTSINFO.h: interface for the CNRTSINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "NRTSSTAINFO.h"

class CNRTSINFO  
{
public:
	CNRTSINFO();
	CNRTSINFO(const CNRTSINFO &Info);
	virtual ~CNRTSINFO();

	void AddStaChan(CString &CSta, CString &CChan, CString &csLCODE,double dSpS, BOOL bSel);
	void LoadConfigFromIni();
	void SaveConfigToIni();
	int LoadConfigFromServer();
	void Clear();
	void CopyFrom(const CNRTSINFO &Info);
	CNRTSINFO & operator =(const CNRTSINFO &Info);
	int GetSelectedChanNumber();


public:
//	int nSta;
	CArray<CNRTSSTAINFO *, CNRTSSTAINFO *> StaInfo; // information array of all station

};

#endif // !defined(AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
