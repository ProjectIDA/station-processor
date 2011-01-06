// WfdiscRecordsArray.cpp: implementation of the CWfdiscRecordsArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "WfdiscRecordsArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWfdiscRecordsArray::CWfdiscRecordsArray()
	{

	}

CWfdiscRecordsArray::~CWfdiscRecordsArray()
	{
	Clear();
	}
void CWfdiscRecordsArray::Clear()
	{
	while(wfdrec.GetSize()>0)
		{
		CWfdiscRecord *wfr=wfdrec[0];
		wfdrec.RemoveAt(0);
		delete wfr;
		}

	}
void CWfdiscRecordsArray::Add(wfdisc wfd)
	{
	CWfdiscRecord *wfr=new CWfdiscRecord(wfd);
	wfdrec.Add(wfr);
	}
int CWfdiscRecordsArray::GetSize()
	{
	return wfdrec.GetSize();
	}
CWfdiscRecord  *CWfdiscRecordsArray::operator [] (int i)
	{
	return wfdrec[i];
	}
