// NRTSSTAINFO.cpp: implementation of the CNRTSSTAINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "nrtsinfo.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSSTAINFO::CNRTSSTAINFO()
	{
	bSelected=false;
	}
CNRTSSTAINFO::CNRTSSTAINFO(QString StaName)
	{
	Sta=StaName;
	bSelected=false;
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
CNRTSCHANINFO* CNRTSSTAINFO::LookupChannel(const QString &chan, const QString &lcode)
	{
	QString			sChan;
	CNRTSCHANINFO	*ci;


	for(ci=ChanInfo.first(); ci; ci=ChanInfo.next())
		{
		sChan=ci->Chan;
		if(chan.compare(sChan)==0 && lcode.compare(ci->LCODE)==0) return ci;
		}
	return NULL;

	}