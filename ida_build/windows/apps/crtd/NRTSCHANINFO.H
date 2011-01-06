// NRTSCHANINFO.h: interface for the CNRTSCHANINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CNRTSCHANINFO   
{
public:
	CNRTSCHANINFO();
	CNRTSCHANINFO(CString ChanName, double dSpS, BOOL);
	virtual ~CNRTSCHANINFO();
public:
	double dSpS;
	CString Chan;
	BOOL bSelected;
	HANDLE hF;

};

#endif // !defined(AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
