#pragma ident "$Id: RawDataBlock.cpp,v 1.1 2008/01/21 22:29:55 akimov Exp $"

#include "RawDataBlock.h"


RawDataBlock::RawDataBlock()
	{
	pdata = NULL;
	nSize = 0;
	}

RawDataBlock::~RawDataBlock()
	{
	if(pdata!=NULL)
		{
		delete []pdata;
		nSize = 0;
		}

	}

bool RawDataBlock::PutDataBlock(const char *p, long n)
	{
	if(pdata!=NULL)
		{
		delete []pdata;
		nSize = 0;
		}
	pdata = new char[n];
	if(pdata==NULL) return false;
	memcpy(pdata, p, n);
	nSize = n;
	return true;
	}

/* Revision History
 *
 * $Log: RawDataBlock.cpp,v $
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
