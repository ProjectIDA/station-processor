// Sta.cpp: implementation of the CSta class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "Sta.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSta::CSta()
	{
	Sta="";
	}

CSta::CSta(CString StaName)
	{
	Sta=StaName;
	}

void CSta::AddChan(CString ChanName, double dSpS, BOOL bSelected)
	{
	for(int i=0; i<ChanInfo.GetSize(); ++i)
		{
		if(ChanName.Compare(ChanInfo[i]->Chan)==0)
			{
			return;
			}
		}
	CChanInfo *ci=new CChanInfo(ChanName,dSpS,bSelected);
	ChanInfo.Add(ci);
	}


CSta::~CSta()
	{
	while(ChanInfo.GetSize()>0)
		{
		CChanInfo *ci=ChanInfo[0];
		ChanInfo.RemoveAt(0);
		delete ci;
		}

	}
