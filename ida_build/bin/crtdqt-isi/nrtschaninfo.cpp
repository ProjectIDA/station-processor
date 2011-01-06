// NRTSCHANINFO.cpp: implementation of the CNRTSCHANINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "nrtschaninfo.h"
#include <qstring.h> 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSCHANINFO::CNRTSCHANINFO()
	{
	dSpS=1.;
	bSelected=FALSE;
	LCODE="??";
	}
CNRTSCHANINFO::CNRTSCHANINFO(QString &ChanName, QString &csLCODE, double dSpS, BOOL bSelected)
	{
	LCODE = csLCODE;
	Chan=ChanName;
	this->dSpS=dSpS;
	this->bSelected=bSelected;
	}

CNRTSCHANINFO::~CNRTSCHANINFO()
	{

	}
