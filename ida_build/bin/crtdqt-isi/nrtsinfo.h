// NRTSINFO.h: interface for the CNRTSINFO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_


#include <qstring.h> 
#include <qptrlist.h>  


#include "nrtsstainfo.h"

class CNRTSINFO  
{
public:
	CNRTSINFO();
	CNRTSINFO(CNRTSINFO &Info);
	virtual ~CNRTSINFO();

	void AddStaChan(QString &CSta, QString &CChan, QString &csLCODE,double dSpS, BOOL bSel);
//	void LoadConfigFromIni();
//	void SaveConfigToIni();
	int LoadConfigFromServer();
	
	void Clear();
	void CopyFrom(CNRTSINFO &Info);
	CNRTSINFO & operator =(CNRTSINFO &Info);
	int GetSelectedChanNumber();
	void SaveConfigToFile();
	void LoadConfigFromFile();
	void SaveConfig();
	void LoadConfig();



public:
//	int nSta;
	QPtrList<CNRTSSTAINFO > StaInfo; // information array of all station

};

#endif // !defined(AFX_NRTSINFO_H__F0C2AFB7_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
