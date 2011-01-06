#pragma ident "$Id: cfs.c,v 1.2 2006/02/14 17:05:21 dechavez Exp $"
/*======================================================================
 * 
 * Fake a unique stream number for packets that don't come with one
 *
 *====================================================================*/
#include "ida.h"

int idaFakeStreamNumber(IDA *ida, short chan, short filt)
{
IDA_CFS *cfs, new;
LNKLST_NODE *crnt;

    new.stream = 0;

/* First, search the list for a match */

    crnt = listFirstNode(ida->cfs);
    while (crnt != NULL) {
        cfs = (IDA_CFS *) crnt->payload;
        if (cfs->chan == chan && cfs->filt == filt) return cfs->stream;
        new.stream = cfs->stream + 1;
        crnt = listNextNode(crnt);
    }

/* Nothing found, generate a new entry */

    new.chan = chan;
    new.filt = filt;
    if (!listAppend(ida->cfs, &new, sizeof(IDA_CFS))) return -1;

    return new.stream;
}

/* Revision History
 *
 * $Log: cfs.c,v $
 * Revision 1.2  2006/02/14 17:05:21  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2006/02/08 22:58:02  dechavez
 * initial release
 *
 */
