#pragma ident "$Id: dosrec.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is DAS status record
 *
 *====================================================================*/
#include "idalst.h"

void dosrec(char *buffer, struct counters *count, int print)
{
int status;
IDA_DAS_STATS srec;

/*  Load the structure  */

    if ((status = ida_srec(ida, &srec, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_DASSTAT, status, print);
        ++count->corrupt;
    }

/*  Print the contents  */

    if (print & P_SREC) psrec(&srec, count, print);

}

/* Revision History
 *
 * $Log: dosrec.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
