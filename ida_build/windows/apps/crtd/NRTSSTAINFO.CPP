// NRTSSTAINFO.cpp: implementation of the CNRTSSTAINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NRTSSTAINFO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSSTAINFO::CNRTSSTAINFO()
	{
	nChan=0;
	}
CNRTSSTAINFO::CNRTSSTAINFO(CString StaName)
	{
	nChan=0;
	Sta=StaName;
	}
void CNRTSSTAINFO::AddChan(CString ChanName, double dSpS, BOOL bDraw)
	{
	CNRTSCHANINFO *ci=new CNRTSCHANINFO(ChanName,dSpS,bDraw);
	ChanInfo.Add(ci);
	++nChan;
	}

CNRTSSTAINFO::~CNRTSSTAINFO()
	{
	CNRTSCHANINFO *ci;
	while(ChanInfo.GetSize()>0)
		{
		ci=ChanInfo[0];
		ChanInfo.RemoveAt(0);
		delete ci;
		}

	}
