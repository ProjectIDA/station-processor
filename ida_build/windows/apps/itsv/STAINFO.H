// StaInfo.h: interface for the CStaInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STAINFO_H__AA568B9D_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_STAINFO_H__AA568B9D_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Sta.h"
#include "wfdiscio.h"
class CStaInfo  
{
public:
	CStaInfo();
	virtual ~CStaInfo();
	void Clear();
	void AddInfo(wfdisc &wfdisc);
	CStaInfo &operator =(CStaInfo &sinf);

	CArray<CSta *, CSta *> AllSta;

};

#endif // !defined(AFX_STAINFO_H__AA568B9D_08E2_11D3_B68A_941D97000000__INCLUDED_)
