#pragma ident "$Id: ida5.c,v 1.1 2006/07/10 21:08:41 dechavez Exp $"
/*======================================================================
 *
 * Patch IDA5 packet headers
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_IDA5

#define REV5_SNAME_OFFSET  26 /* offset to station name in raw packet header */
#define REV5_SNAMELEN 4

void CompleteIda5Header(ISI_RAW_PACKET *raw, ISI_DL *dl)
{
int i;
UINT8 *ptr;

/* insert the correct, upper case, station name */

    ptr = &raw->payload[REV5_SNAME_OFFSET];
    memcpy(ptr, dl->sys->site, REV5_SNAMELEN);
    for (i = 0; i < REV5_SNAMELEN; i++) ptr[i] = toupper(ptr[i]);
}

/* Revision History
 *
 * $Log: ida5.c,v $
 * Revision 1.1  2006/07/10 21:08:41  dechavez
 * initial release
 *
 */
