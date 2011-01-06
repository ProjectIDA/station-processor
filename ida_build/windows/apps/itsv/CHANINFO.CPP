// ChanInfo.cpp: implementation of the CChanInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "ChanInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChanInfo::CChanInfo()
	{
	Chan="";
	dSpS=1.;
	bSelected=true;
	}
CChanInfo::CChanInfo(CString ChanName, double dSpS, BOOL bSelected=true)
	{
	Chan=ChanName;
	this->dSpS=dSpS;
	this->bSelected=bSelected;
	}

CChanInfo::~CChanInfo()
	{

	}
