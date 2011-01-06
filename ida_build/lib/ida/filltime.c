#pragma ident "$Id: filltime.c,v 1.3 2006/06/26 23:49:47 dechavez Exp $"
/*======================================================================
 *
 *  Fill in the tru and qual fields in an IDA time_tag structure,
 *  assuming that the remaining fields are correct.
 *
 *====================================================================*/
#include "ida.h"

void ida_settqual(IDA *ida, IDA_TIME_TAG *ttag)
{
UINT32 i;
IDA_TQUAL *map;

    if (ttag == NULL) return;
    if (ida == NULL || ida->tqual == NULL) {
        ttag->qual = IDA_UNDEFINED_TQUAL;
        return;
    }

    for (ttag->qual = IDA_UNDEFINED_TQUAL, i = 0; ttag->qual == IDA_UNDEFINED_TQUAL && i < ida->tqual->count; i++) {
        map = (IDA_TQUAL *) ida->tqual->array[i];
        if (ttag->code == map->factory) ttag->qual = map->internal;
    }
}

void ida_filltime(IDA *ida, IDA_TIME_TAG *ttag)
{
double dext, doff;

    if (ttag == NULL) return;

/*  Get absolute time stamp  */

    dext  = (double) ttag->ext;
    if (ttag->sys >= ttag->hz) {
        doff  = (double) (ttag->sys - ttag->hz);
    } else {
        doff  = (double) - 1 * (double) (ttag->hz - ttag->sys);
    }
    doff *= (double) ttag->mult;
    doff /= (double) 1000;
    if (ttag->year == IDA_EMBEDDED_YEAR) {
        ttag->tru = dext + doff;
    } else {
        ttag->tru = dext + doff + (double) year_secs(ttag->year);
    }

    ida_settqual(ida, ttag);

/*  Look for internal errors  */

    ttag->error = 0x0000;

    if (ttag->qual < 0) {
        ttag->error |= TTAG_BAD_CLKRD;
    } else {
        if (ttag->hz > ttag->sys) ttag->error |= TTAG_BAD_OFFSET;
        if (ttag->ext < TTAG_MIN_EXTTIM) ttag->error |= TTAG_LOW_EXTTIM | TTAG_BAD_EXTTIM;
        if (ttag->year != IDA_EMBEDDED_YEAR && ttag->ext > TTAG_MAX_EXTTIM) ttag->error |= TTAG_BIG_EXTTIM | TTAG_BAD_EXTTIM;
    }
}

/* Revision History
 *
 * $Log: filltime.c,v $
 * Revision 1.3  2006/06/26 23:49:47  dechavez
 * fixed signed/unsigned compare
 *
 * Revision 1.2  2006/03/13 22:19:30  dechavez
 * removed requirement that IDA handle be non-null in ida_filltime().  Allows
 * use of the function for completing tru time field without requiring the
 * database stuff from the handle for time quality field
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
