#pragma ident "$Id: srec8.c,v 1.3 2006/02/08 23:20:40 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 8 das stat record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define NAKS_OFF       2
#define ACKS_OFF       6
#define DROPPED_OFF   10
#define TIMEOUTS_OFF  14
#define TRIGGERS_OFF  18
#define EVENT_OFF     22
#define CALIB_OFF     23
#define SEQNO_OFF     26

int srec_rev8(IDA *unused, IDA_DAS_STATS *dest, UINT8 *src)
{
long ltemp;

/*  number of naks received  */

    memcpy(&ltemp, src + NAKS_OFF, 4); LSWAB(&ltemp, 1);
    dest->naks = ltemp;

/*  number of acks received  */

    memcpy(&ltemp, src + ACKS_OFF, 4); LSWAB(&ltemp, 1);
    dest->acks = ltemp;

/*  number of dropped packets  */

    memcpy(&ltemp, src + DROPPED_OFF, 4); LSWAB(&ltemp, 1);
    dest->dropped = ltemp;

/*  number of xmit timeouts  */

    memcpy(&ltemp, src + TIMEOUTS_OFF, 4); LSWAB(&ltemp, 1);
    dest->timeouts = ltemp;

/*  number of detector triggers  */

    memcpy(&ltemp, src + TRIGGERS_OFF, 4); LSWAB(&ltemp, 1);
    dest->triggers = ltemp;

/*  event in progress flag */

    dest->event = (int) src[EVENT_OFF];

    dest->calib = (int) src[CALIB_OFF];

/* packet sequence number */

    memcpy(&ltemp, src + SEQNO_OFF, 4); LSWAB(&ltemp, 1);
    dest->seqno = ltemp;

    return 0;
}

/* Revision History
 *
 * $Log: srec8.c,v $
 * Revision 1.3  2006/02/08 23:20:40  dechavez
 * ansi-style declarations
 *
 * Revision 1.2  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
