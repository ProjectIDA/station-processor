#pragma ident "$Id: tsrec_ok.c,v 1.3 2006/11/13 23:56:14 dechavez Exp $"
/*======================================================================
 *
 *  Make sure data record is recognized, supported, and error free.
 *
 *====================================================================*/
#include "i10dmx.h"

extern char *Buffer;

int tsrec_ok(IDA10_TS *ts, struct counter *count)
{

    if (ts->hdr.nsamp < 0) {
        sprintf(Buffer+strlen(Buffer),"Negative nsamp, ");
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    } else if (ts->hdr.nsamp > IDA10_MAXDATALEN/sizeof(INT32)) {
        sprintf(Buffer+strlen(Buffer),"Too large nsamp (%ld), ", ts->hdr.nsamp);
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: tsrec_ok.c,v $
 * Revision 1.3  2006/11/13 23:56:14  dechavez
 * removed unused ttag.valid test and added test for invalid nsamp
 *
 * Revision 1.2  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
