// NRTSCHANINFO.cpp: implementation of the CNRTSCHANINFO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NRTSCHANINFO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNRTSCHANINFO::CNRTSCHANINFO()
	{
	dSpS=1.;
	bSelected=FALSE;
	}
CNRTSCHANINFO::CNRTSCHANINFO(CString ChanName, double dSpS, BOOL bSelected)
	{
	Chan=ChanName;
	this->dSpS=dSpS;
	this->bSelected=bSelected;
	}

CNRTSCHANINFO::~CNRTSCHANINFO()
	{

	}
