#pragma ident "$Id: ida10.c,v 1.4 2007/02/08 22:55:58 dechavez Exp $"
/*======================================================================
 *
 * Fill IDA10 packets with barometer data and write to disk loop
 *
 * Not yet implemented... stubs only.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_IDA10

void CompleteIda10Header(ISI_RAW_PACKET *raw, ISI_DL *dl)
{
    if (!ida10InsertSeqno32(raw->payload, (UINT32) dl->sys->seqno.counter)) {
        LogMsg(LOG_INFO, "ida10InsertSeqno32 failed!");
        Exit(MY_MOD_ID + 1);
    }
}

THREAD_FUNC Ida10BarometerThread(void *argptr)
{
BAROMETER *bp;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "IDA10 barometer support not implemented");
    Exit(MY_MOD_ID + 2);
}

/* Revision History
 *
 * $Log: ida10.c,v $
 * Revision 1.4  2007/02/08 22:55:58  dechavez
 * LOG_ERR to LOG_INFO
 *
 * Revision 1.3  2006/12/22 02:52:55  dechavez
 * CompleteIDA10Header calls ida10InsertSeqno32()
 *
 * Revision 1.2  2006/06/02 21:05:27  dechavez
 * changed input to CompleteIda9Header() to be ISI_RAW_PACKET instead of its payload
 *
 * Revision 1.1  2006/03/30 22:01:09  dechavez
 * stub release
 *
 */
