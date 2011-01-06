#pragma ident "$Id: RawDataBlock.h,v 1.1 2008/01/21 22:29:55 akimov Exp $"

//////////////////////////////////////////////////////////////////////
// RawDataBlock.h: interface for the RawDataBlock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_RAWDATABLOCK_H_INCLUDED_)
#define _RAWDATABLOCK_H_INCLUDED_
#include "platform.h"

class RawDataBlock  
{
public:
	RawDataBlock();
	virtual ~RawDataBlock();
	bool PutDataBlock(const char *p, long n);
	char *pdata;
	long nSize;
};

#endif // !defined(_RAWDATABLOCK_H_INCLUDED_)

/* Revision History
 *
 * $Log: RawDataBlock.h,v $
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
