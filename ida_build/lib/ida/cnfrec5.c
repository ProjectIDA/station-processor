#pragma ident "$Id: cnfrec5.c,v 1.1 2006/02/08 22:57:42 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 5 configuration record.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define MAXSTREAMS  25  /* maximum number of streams, this rev  */

#define YEAR_OFF     6  /* year                            */
#define MULT_OFF     8  /* system and 1-hz time multiplier */
#define TAGSYS_OFF  10  /* record stamp system time        */
#define TAGEXT_OFF  14  /* record stamp external time      */
#define TAGHZ_OFF   18  /* record stamp 1-Hz time          */
#define TAGQUAL_OFF 22  /* record stamp Omega quality code */
#define UNIT_OFF    24  /* unit id                              */
#define SRATE_OFF   34  /* digitizer sample rate                */
#define NSTREAM_OFF 38  /* number of active streams             */
#define TABLE_OFF   42  /* start of configuration table         */
#define CHAN_OFF     0  /* channel, relative to TABLE_OFF       */
#define FILT_OFF     2  /* filter, relative to TABLE_OFF        */
#define MODE_OFF     4  /* mode, relative to TABLE_OFF          */
#define OUTPUT_OFF   6  /* output bitmap, relative to TABLE_OFF */
#define GAIN_OFF     8  /* gain, relative to TABLE_OFF          */
#define TBL_SIZE    10  /* size of one table entry              */
#define DETCHN_OFF 292  /* detector channel bitmap              */
#define KEYCHN_OFF 294  /* event detector key channel           */
#define STA_OFF    296  /* short-term average                   */
#define LTA_OFF    298  /* long-term average                    */
#define THRESH_OFF 300  /* threshold offset                     */
#define MAXLEN_OFF 302  /* maximum trigger length               */
#define VOTERS_OFF 306  /* minimum number of voters required    */
#define MEMORY_OFF 308  /* number of pre-event buffers          */
#define JUMPER_OFF 310  /* backplane jumpers                    */

#define DAS_MASK 0x01
#define MDM_MASK 0x02
#define D2A_MASK 0x04

int cnfrec_rev5(IDA *ida, IDA_CONFIG_REC *dest, UINT8 *src)
{
int i, j;
char  *ptr;
short stemp;
long  ltemp;

    if (MAXSTREAMS > IDA_MAXSTREAMS) return -101;

/*  Digitizer info  */

    memcpy(&stemp, src + UNIT_OFF, 2); SSWAB(&stemp, 1);
    if (stemp == 0) {
        dest->atod = IDA_ARS;
    } else {
        dest->atod = IDA_DAS;
    }

    memcpy(&ltemp, src + SRATE_OFF, 4); LSWAB(&ltemp, 1);
    dest->srate = ltemp;

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

/*  Configuration table  */

    memcpy(&ltemp, src + NSTREAM_OFF, 4); LSWAB(&ltemp, 1);
    dest->nstream = ltemp;
    if (MAXSTREAMS    > IDA_MAXSTREAMS) return -101;
    if (dest->nstream >      MAXSTREAMS) return -101;

    for (ptr=src+TABLE_OFF, i=j=0; i < MAXSTREAMS; i++, ptr+=TBL_SIZE) {
        dest->table[j].dl_stream  = i;
        memcpy(&stemp, ptr + CHAN_OFF, 2); SSWAB(&stemp, 1);
        dest->table[j].dl_channel = stemp;
        memcpy(&stemp, ptr + FILT_OFF, 2); SSWAB(&stemp, 1);
        dest->table[j].dl_filter = stemp;
        memcpy(&stemp, ptr + GAIN_OFF, 2); SSWAB(&stemp, 1);
        dest->table[j].dl_gain = stemp;
        memcpy(&stemp, ptr + MODE_OFF, 2); SSWAB(&stemp, 1);
        dest->table[j].mode = stemp ? TRIG : CONT;
        memcpy(&stemp, ptr + OUTPUT_OFF, 2); SSWAB(&stemp, 1);
        dest->table[j].output = 0x00;
        if (stemp & DAS_MASK) dest->table[j].output |= IDA_TAPE;
        if (stemp & MDM_MASK) dest->table[j].output |= IDA_MODEM;
        if (stemp & D2A_MASK) dest->table[j].output |= IDA_DTOA;
        ++j;
    }
        
/*  Event detector parameters  */

    memcpy(&stemp, src + DETCHN_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.chan = stemp;

    memcpy(&stemp, src + KEYCHN_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.key = stemp;

    memcpy(&stemp, src + STA_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.sta = stemp;

    memcpy(&stemp, src + LTA_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.lta = stemp;

    memcpy(&stemp, src + THRESH_OFF, 2); SSWAB(&stemp, 1);
    ltemp = ((long) stemp * 10000L) / 32768L;
    dest->detect.thresh = (short) ltemp;

    memcpy(&ltemp, src + MAXLEN_OFF, 4); LSWAB(&ltemp, 1);
    dest->detect.maxlen = ltemp;

    memcpy(&stemp, src + VOTERS_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.voters = stemp;

    memcpy(&stemp, src + MEMORY_OFF, 2); SSWAB(&stemp, 1);
    dest->detect.memory = stemp;

/*  Backplane jumpers  */

    memcpy(&stemp, src + JUMPER_OFF, 2); SSWAB(&stemp, 1);
    dest->jumpers = stemp;

    return 0;
}

/* Revision History
 *
 * $Log: cnfrec5.c,v $
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
