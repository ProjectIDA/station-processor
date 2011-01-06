// NRTSCHANINFO.h: interface for the CNRTSCHANINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#include "platform.h"
#include <qstring.h> 

class CNRTSCHANINFO   
{
public:
	CNRTSCHANINFO();
	CNRTSCHANINFO(QString &ChanName, QString &csLCODE, double dSpS, BOOL);
	virtual ~CNRTSCHANINFO();
public:
	double dSpS;
	QString Chan;
	QString LCODE;
	BOOL bSelected;
//	HANDLE hF;

};

#endif // !defined(AFX_NRTSCHANINFO_H__F0C2AFB8_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
