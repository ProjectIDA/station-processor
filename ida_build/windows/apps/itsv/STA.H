// Sta.h: interface for the CSta class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STA_H__AA568B9F_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_STA_H__AA568B9F_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "ChanInfo.h"

class CSta  
{
public:
	CSta();
	CSta(CString StaName);
	void AddChan(CString ChanName, double dSpS, BOOL bSelected);

	virtual ~CSta();
	CString Sta;
	CArray<CChanInfo *, CChanInfo *> ChanInfo;

};

#endif // !defined(AFX_STA_H__AA568B9F_08E2_11D3_B68A_941D97000000__INCLUDED_)
