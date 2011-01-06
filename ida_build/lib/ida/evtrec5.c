#pragma ident "$Id: evtrec5.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 5 event record.
 *
 *  Note: Unlike the documentation, here we treat a single event
 *        entry as a pair of elements (start and end).  So, the
 *        TBL_SIZE is 36, instead of 18 as documented.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define NEVENT   27  /* number of events/record is fixed in rev 5 */
#define TBL_OFF  34  /* start of event table                      */
#define TBL_SIZE 36  /* size of one entry                         */

#define YEAR_OFF     6  /* year                            */
#define MULT_OFF     8  /* system and 1-hz time multiplier */
#define TAGSYS_OFF  10  /* record stamp system time        */
#define TAGEXT_OFF  14  /* record stamp external time      */
#define TAGHZ_OFF   18  /* record stamp 1-Hz time          */
#define TAGQUAL_OFF 22  /* record stamp Omega quality code */
#define BASE_OFF    34  /* start of event info             */
#define STA_OFF      0  /* sta, relative to BASE_OFF       */
#define LTA_OFF      2  /* lta, relative to BASE_OFF       */
#define BEGSYS_OFF   4  /* system time at trigger on       */
#define BEGEXT_OFF   8  /* external time at trigger on     */
#define BEGHZ_OFF   12  /* 1-Hz time at trigger on         */
#define BEGTQ_OFF   16  /* time quality at trigger on      */
#define ZERO1_OFF   18  /* should be 0 at this sta offset  */
#define ZERO2_OFF   20  /* should be 0 at this lta offset  */
#define ENDSYS_OFF  22  /* system time at trigger off      */
#define ENDEXT_OFF  26  /* external time at trigger off    */
#define ENDHZ_OFF   30  /* 1-Hz time at trigger off        */
#define ENDTQ_OFF   34  /* time quality at trigger off     */

int evtrec_rev5(IDA *ida, IDA_EVENT_REC *dest, UINT8 *src)
{
int i;
char *ptr;
short stemp;
long  ltemp;

    if ((dest->nevent = NEVENT) > IDA_MAXEVENTS) return -101;

/*  Record time stamp  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.year = stemp;
    memcpy(&stemp, src + MULT_OFF, 2); SSWAB(&stemp, 1);
    dest->ttag.mult = stemp;
    memcpy(&ltemp, src + TAGSYS_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.sys = ltemp;
    memcpy(&ltemp, src + TAGEXT_OFF, 4); LSWAB(&ltemp, 1);
    dest->ttag.ext = ltemp;
    memcpy(&ltemp, src + TAGHZ_OFF,  4); LSWAB(&ltemp, 1);
    dest->ttag.hz  = ltemp;
    memcpy(&dest->ttag.code, src + TAGQUAL_OFF, 1);

    ida_filltime(ida, &dest->ttag);

/*  Trigger table  */

    for (ptr = src + TBL_OFF, i = 0; i < NEVENT; i++, ptr += TBL_SIZE) {

        memcpy(&stemp, ptr + STA_OFF, 2); SSWAB(&stemp, 1);
        dest->event[i].sta = (long) stemp;
        memcpy(&stemp, ptr + LTA_OFF, 2); SSWAB(&stemp, 1);
        dest->event[i].lta = (long) stemp;

        memcpy(&ltemp, ptr + BEGSYS_OFF, 4); LSWAB(&ltemp, 1);
        dest->event[i].beg.sys = ltemp;
        memcpy(&ltemp, ptr + BEGEXT_OFF, 4); LSWAB(&ltemp, 1);
        dest->event[i].beg.ext = ltemp;
        memcpy(&ltemp, ptr + BEGHZ_OFF,  4); LSWAB(&ltemp, 1);
        dest->event[i].beg.hz  = ltemp;
        memcpy(&dest->event[i].beg.code, ptr + BEGTQ_OFF, 1);

        memcpy(&ltemp, ptr + ENDSYS_OFF, 4); LSWAB(&ltemp, 1);
        dest->event[i].end.sys = ltemp;
        memcpy(&ltemp, ptr + ENDEXT_OFF, 4); LSWAB(&ltemp, 1);
        dest->event[i].end.ext = ltemp;
        memcpy(&ltemp, ptr + ENDHZ_OFF,  4); LSWAB(&ltemp, 1);
        dest->event[i].end.hz  = ltemp;
        memcpy(&dest->event[i].end.code, ptr + ENDTQ_OFF, 1);

        dest->event[i].beg.year = dest->ttag.year;
        dest->event[i].beg.mult = dest->ttag.mult;
        ida_filltime(ida, &dest->event[i].beg);

        dest->event[i].end.year = dest->ttag.year;
        dest->event[i].end.mult = dest->ttag.mult;
        ida_filltime(ida, &dest->event[i].end);

        dest->event[i].duration = dest->event[i].end.tru - dest->event[i].beg.tru;
    }

    return 0;
}

/* Revision History
 *
 * $Log: evtrec5.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
