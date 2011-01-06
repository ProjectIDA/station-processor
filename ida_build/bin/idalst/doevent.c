#pragma ident "$Id: doevent.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is an event record...
 *
 *====================================================================*/
#include "idalst.h"

void doevent(char *buffer, struct counters *count, int print, int check)
{
int status;
IDA_EVENT_REC evtrec;

/*  Load the structure  */

    if ((status = ida_evtrec(ida, &evtrec, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_EVENT, status, print);
        ++count->corrupt;
        return;
    }

/*  Print the record contents  */

    if (print & P_EVENT) pevent(&evtrec, count, print);

}

/* Revision History
 *
 * $Log: doevent.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
