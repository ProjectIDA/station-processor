#pragma ident "$Id: docalib.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is a Calibration record...
 *
 *====================================================================*/
#include "idalst.h"

void docalib(char *buffer, struct counters *count, int print, int check)
{
int status;
IDA_CALIB_REC crec;

/*  Load the structure  */

    if ((status = ida_crec(ida, &crec, buffer)) != 0) {
        if (print & P_CORRUPT) pcorrupt(count, IDA_CALIB, status, print);
        ++count->corrupt;
        return;
    }

/*  Check for illegal time tags  */

    if (check & C_BADTAG && crec.ttag.error) {
        ++count->badtag;
        if (print & P_BADTAG)pbadtag(&crec.ttag,count,IDA_CALIB,print);
    }

/*  Print the calibration record header  */

    if (print & P_CALIB) pcalib(&crec, count, print);

}

/* Revision History
 *
 * $Log: docalib.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
