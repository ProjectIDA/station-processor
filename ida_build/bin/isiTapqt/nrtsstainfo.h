// NRTSSTAINFO.h: interface for the CNRTSSTAINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#include "platform.h"
#include "nrtsstainfo.h"
#include <qptrlist.h> 
#include "nrtschaninfo.h"

class CNRTSSTAINFO  
{
public:
	CNRTSSTAINFO();
	CNRTSSTAINFO(QString StaName);
	void AddChan(QString ChanName, QString csLCODE, double dSpS, BOOL);
	void Clear();
	virtual ~CNRTSSTAINFO();
	CNRTSCHANINFO *LookupChannel(const QString &chan, const QString &lcode);

public:
	QString Sta;
	QPtrList<CNRTSCHANINFO> ChanInfo; // information array of all station
	bool bSelected;
};

#endif // !defined(AFX_NRTSSTAINFO_H__F0C2AFB9_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
