#pragma ident "$Id: doident.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is an ident record...
 *
 *====================================================================*/
#include "idalst.h"

void doident(char *buffer, struct counters *count, int print, int check)
{
int status;
IDA_IDENT_REC idrec;

/*  Load the structure  */

    if ((status = ida_idrec(ida, &idrec, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_IDENT, status, print);
        ++count->corrupt;
    }

/*  Check for illegal time tags  */

    if (check & C_BADTAG && idrec.ttag.error) {
        ++count->badtag;
        if (print & P_BADTAG)pbadtag(&idrec.ttag,count,IDA_IDENT,print);
    }

/*  Print the contents  */

    if (print & P_IDENT) pident(&idrec, count, print);

}

/* Revision History
 *
 * $Log: doident.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
