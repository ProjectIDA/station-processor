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
	}
CNRTSSTAINFO::CNRTSSTAINFO(CString StaName)
	{
	Sta=StaName;
	}
void CNRTSSTAINFO::AddChan(CString ChanName, CString csLCODE, double dSpS, BOOL bDraw)
	{
	CNRTSCHANINFO *ci=new CNRTSCHANINFO(ChanName, csLCODE, dSpS, bDraw);
	ChanInfo.Add(ci);
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
