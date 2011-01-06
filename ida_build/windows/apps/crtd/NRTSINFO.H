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
	virtual ~CNRTSINFO();

	void AddStaChan(CString CSta, CString CChan, double dSpS);
	void LoadConfigFromIni();
	void SaveConfigToIni();
	int LoadConfigFromNRTSServer();
	void CNRTSINFO::Clear();


public:
	int nSta;
	CArray<CNRTSSTAINFO *, CNRTSSTAINFO *> StaInfo; // information array of all station

};

#endif // !defined(AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
