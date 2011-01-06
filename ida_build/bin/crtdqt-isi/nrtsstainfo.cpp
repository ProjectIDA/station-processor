// NRTSSTAINFO.cpp: implementation of the CNRTSSTAINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "nrtsinfo.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSSTAINFO::CNRTSSTAINFO()
	{
	}
CNRTSSTAINFO::CNRTSSTAINFO(QString StaName)
	{
	Sta=StaName;
	}
void CNRTSSTAINFO::AddChan(QString ChanName, QString csLCODE, double dSpS, BOOL bDraw)
	{
	CNRTSCHANINFO *ci=new CNRTSCHANINFO(ChanName, csLCODE, dSpS, bDraw);
	ChanInfo.append(ci);
	}

CNRTSSTAINFO::~CNRTSSTAINFO()
	{
	Clear();
	}
void CNRTSSTAINFO::Clear()
	{
	while(1)
		{
		CNRTSCHANINFO *ci=ChanInfo.first();
		if(ci==NULL) return;
		ChanInfo.removeFirst();
		delete ci;
		}
	}
