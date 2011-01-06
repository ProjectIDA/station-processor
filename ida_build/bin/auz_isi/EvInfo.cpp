#pragma ident "$Id: EvInfo.cpp,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
// EvInfo.cpp: implementation of the CEvInfo class.
//
//////////////////////////////////////////////////////////////////////


#include "platform.h"
#include "EvInfo.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEvInfo::CEvInfo()
	{
	lastEventTime=0;
	lastCountTime=0;
	bFlag=false;
	}

CEvInfo::~CEvInfo()
	{

	}

/* Revision History
 *
 * $Log: EvInfo.cpp,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
