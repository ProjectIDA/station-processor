#pragma ident "$Id: mseed.c,v 1.1 2006/03/30 22:01:09 dechavez Exp $"
/*======================================================================
 *
 * Fill MiniSEED packets with barometer data and write to disk loop
 *
 * Not yet implemented... stubs only.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_MSEED

void CompleteMseedHeader(UINT8 *start, ISI_DL *dl)
{
    return;
}

THREAD_FUNC MseedBarometerThread(void *argptr)
{
BAROMETER *bp;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "MiniSEED barometer support not implemented");
    Exit(MY_MOD_ID + 1);
}

/* Revision History
 *
 * $Log: mseed.c,v $
 * Revision 1.1  2006/03/30 22:01:09  dechavez
 * stub release
 *
 */
