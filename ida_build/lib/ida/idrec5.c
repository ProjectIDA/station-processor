#pragma ident "$Id: idrec5.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 5 identification record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define YEAR_OFF     6  /* year                            */
#define MULT_OFF     8  /* system and 1-hz time multiplier */
#define TAGSYS_OFF  10  /* record stamp system time        */
#define TAGEXT_OFF  14  /* record stamp external time      */
#define TAGHZ_OFF   18  /* record stamp 1-Hz time          */
#define TAGQUAL_OFF 22  /* record stamp Omega quality code */
#define UNIT_OFF    24  /* unit id                         */
#define SNAME_OFF   26  /* station name                    */
#define INSERT_OFF  34  /* last tape insertion ext time    */
#define EJECT_OFF   38  /* last tape ejection  ext time    */
#define ARSID_OFF   42  /* ARS identification number       */
#define ARSVER_OFF  44  /* ARS firmware revision           */
#define DASID_OFF   46  /* DAS identification number       */
#define DASVER_OFF  48  /* DAS firmware revision           */
#define DSPVER_OFF  50  /* DSP firmware revision           */
#define SEQ_OFF     52  /* tape sequence number            */

int idrec_rev5(IDA *ida, IDA_IDENT_REC *dest, UINT8 *src)
{
short stemp;
long  ltemp;

/*  Digitizer info  */

    memcpy(&stemp, src + UNIT_OFF, 2); SSWAB(&stemp, 1);
    if (stemp == 0) {
        dest->atod = IDA_ARS;
        memcpy(&stemp, src + ARSID_OFF,  2); SSWAB(&stemp, 1);
        dest->atod_id = stemp;
        memcpy(&stemp, src + ARSVER_OFF, 2); SSWAB(&stemp, 1);
        dest->atod_rev = stemp;
    } else {
        dest->atod = IDA_DAS;
        memcpy(&stemp, src + DASID_OFF,  2); SSWAB(&stemp, 1);
        dest->atod_id = stemp;
        memcpy(&stemp, src + DASVER_OFF, 2); SSWAB(&stemp, 1);
        dest->atod_rev = stemp;
        memcpy(&stemp, src + DSPVER_OFF, 2); SSWAB(&stemp, 1);
        dest->dsp_rev = stemp;
    }

/*  Station name  */

    memcpy(dest->sname, src + SNAME_OFF, IDA_SNAMLEN);

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

/*  Last tape insert time  */

    dest->beg.year  = dest->ttag.year;
    dest->beg.mult  = dest->ttag.mult;
    dest->beg.sys   = 0;
    dest->beg.hz    = 0;
    dest->beg.code  = 0;
    dest->beg.error = 0;
    memcpy(&ltemp, src + INSERT_OFF, 4); LSWAB(&ltemp, 1);
    dest->beg.ext = ltemp;

/*  Last tape eject time  */

    dest->end = dest->beg;
    memcpy(&ltemp, src + EJECT_OFF, 4); LSWAB(&ltemp, 1);
    dest->end.ext = ltemp;

/*  Tape sequence number  */

    memcpy(&stemp, src + SEQ_OFF, 2); SSWAB(&stemp, 1);
    dest->boot_flag = ((dest->tape_no = stemp) == -1) ? 1 : 0;

    return 0;
}

/* Revision History
 *
 * $Log: idrec5.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
