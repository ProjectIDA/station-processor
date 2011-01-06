#pragma ident "$Id: doconfig.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is a configuration record...
 *
 *====================================================================*/
#include "idalst.h"

void doconfig(char *buffer, struct counters *count, int print, int check)
{
int status;
IDA_CONFIG_REC cnf;

/*  Load the structure  */

    if ((status = ida_cnfrec(ida, &cnf, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_CONFIG, status, print);
        ++count->corrupt;
        return;
    }

/*  Check for illegal time tags  */

    if (check & C_BADTAG && cnf.ttag.error) {
        ++count->badtag;
        if (print & P_BADTAG)pbadtag(&cnf.ttag,count,IDA_CONFIG,print);
    }

/*  Print the record contents  */

    if (print & P_CONFIG) pconfig(&cnf, count, print);

}

/* Revision History
 *
 * $Log: doconfig.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
