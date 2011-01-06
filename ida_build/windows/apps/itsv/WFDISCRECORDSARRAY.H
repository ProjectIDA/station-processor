// WfdiscRecordsArray.h: interface for the CWfdiscRecordsArray class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WFDISCRECORDSARRAY_H__F06107F1_0A20_11D3_B68C_F8DEF5000000__INCLUDED_)
#define AFX_WFDISCRECORDSARRAY_H__F06107F1_0A20_11D3_B68C_F8DEF5000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "WfdiscRecord.h"

class CWfdiscRecordsArray  
{
public:
	CWfdiscRecordsArray();
	virtual ~CWfdiscRecordsArray();
	void Add(wfdisc wfd);
	int GetSize();
	void Clear();

	CWfdiscRecord * operator [] (int);

	CArray<CWfdiscRecord *, CWfdiscRecord *> wfdrec;

};

#endif // !defined(AFX_WFDISCRECORDSARRAY_H__F06107F1_0A20_11D3_B68C_F8DEF5000000__INCLUDED_)
