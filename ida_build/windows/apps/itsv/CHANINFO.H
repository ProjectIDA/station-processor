// ChanInfo.h: interface for the CChanInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANINFO_H__AA568B9E_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_CHANINFO_H__AA568B9E_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CChanInfo  
{
public:
	CChanInfo();
	CChanInfo(CString ChanName, double dSpS, BOOL bSelected);
	virtual ~CChanInfo();
	CString Chan;
	double dSpS;
	BOOL bSelected;

};

#endif // !defined(AFX_CHANINFO_H__AA568B9E_08E2_11D3_B68A_941D97000000__INCLUDED_)
