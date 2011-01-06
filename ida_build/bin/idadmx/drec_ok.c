#pragma ident "$Id: drec_ok.c,v 1.2 2006/02/09 20:14:38 dechavez Exp $"
/*======================================================================
 *
 *  Make sure data record is recognized, supported, and error free.
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;

int drec_ok(IDA_DHDR *dhead, struct counter *count)
{

    if (dhead->dl_stream < 0 || dhead->dl_stream >= IDA_MAXSTREAMS) {
        sprintf(Buffer,"drec_ok: ");
        sprintf(Buffer+strlen(Buffer),"Illegal data logger stream ");
        sprintf(Buffer+strlen(Buffer),"(%d), ", dhead->dl_stream);
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    } else if (
        dhead->form != S_UNCOMP    && 
        dhead->form != L_UNCOMP    && 
        dhead->form != FP32_UNCOMP &&
        dhead->form != IGPP_COMP
    ) {
        sprintf(Buffer,"drec_ok: ");
        sprintf(Buffer+strlen(Buffer),"Unsupported compression, ");
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    } else if (dhead->beg.error & TTAG_BAD_OFFSET) {
        sprintf(Buffer,"drec_ok: ");
        sprintf(Buffer+strlen(Buffer),"Illegal time tag, ");
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    } else if (!ida_sampok(dhead)) {
        sprintf(Buffer,"drec_ok: ");
        sprintf(Buffer+strlen(Buffer),"Illegal nsamp ");
        sprintf(Buffer+strlen(Buffer),"record %d ignored.\n",count->rec);
        logmsg(Buffer);
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: drec_ok.c,v $
 * Revision 1.2  2006/02/09 20:14:38  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
