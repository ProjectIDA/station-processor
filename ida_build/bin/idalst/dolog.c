#pragma ident "$Id: dolog.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is a log record...
 *
 *====================================================================*/
#include "idalst.h"

void dolog(char *buffer, struct counters *count, int print, int check)
{
int status;
IDA_LOG_REC lrec;

/*  Load the structure  */

    if ((status = ida_logrec(ida, &lrec, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_LOG, status, print);
        ++count->corrupt;
        return;
    }

/*  Print the contents  */

    if (print & P_LOG) plog(&lrec, count, print);

}

/* Revision History
 *
 * $Log: dolog.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
