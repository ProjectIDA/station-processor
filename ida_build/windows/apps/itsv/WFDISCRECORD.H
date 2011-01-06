// WfdiscRecord.h: interface for the CWfdiscRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WFDISCRECORD_H__AA568B9B_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_WFDISCRECORD_H__AA568B9B_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "wfdiscio.h"

class CWfdiscRecord  
	{
public:
	CWfdiscRecord();
	CWfdiscRecord(struct wfdisc wfd);
	long GetCount(int n);
	bool CloseDataSet();
	bool OpenDataSet();


	virtual ~CWfdiscRecord();
//*********************************
	long LoadData(int,int nMaxCounts=0);

	struct wfdisc wfd;
	long *lData;

	int nCacheFirstCount, nCacheSize;
	int nCacheMaximumSize;
	CFile cssFile;



	};

#endif // !defined(AFX_WFDISCRECORD_H__AA568B9B_08E2_11D3_B68A_941D97000000__INCLUDED_)
