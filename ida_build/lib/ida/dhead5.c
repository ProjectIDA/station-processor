#pragma ident "$Id: dhead5.c,v 1.3 2007/06/01 19:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 5 data record header.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define NBYTES 960  /* number of bytes in data area */

#define ARS_SRATE   1  /* digitizer sample rate */
#define DAS_SRATE 200  /* digitizer sample rate */

#define NUM_ATOD 2 /* number of digitizers in system (1 ARS + 1 DAS) */

#define MIN_CHAN 0
#define MAX_CHAN 23

#define MIN_FILT 0
#define MAX_FILT 4
#define NUM_FILT 5

static int ffu_map[NUM_FILT] = {
/* shift NONE     filtered streams by */ 0,
/* shift BAD_FILT filtered streams by */ 0,
/* shift RDECIM   filtered streams by */ 16,
/* shift BB_20    filtered streams by */ 14,
/* shift LP       filtered streams by */ 11
};

static int decimation[NUM_ATOD][NUM_FILT] = {
{
    /* rev 5 ARS filter 0 has decimation by */  1,
    /* rev 5 ARS filter 1 has decimation by */ -1,
    /* rev 5 ARS filter 2 has decimation by */ 10,
    /* rev 5 ARS filter 3 has decimation by */ -1,
    /* rev 5 ARS filter 4 has decimation by */ -1,
},
{
    /* rev 5 DAS filter 0 has decimation by */    1,
    /* rev 5 DAS filter 1 has decimation by */   -1,
    /* rev 5 DAS filter 2 has decimation by */ 2000,
    /* rev 5 DAS filter 3 has decimation by */   10,
    /* rev 5 DAS filter 4 has decimation by */ 2000
}
};

#define YEAR_OFF     6  /* year                            */
#define MULT_OFF     8  /* system and 1-hz time multiplier */
#define BEGSYS_OFF  10  /* begin system time               */
#define BEGEXT_OFF  14  /* begin external time             */
#define BEGHZ_OFF   18  /* begin 1-Hz time                 */
#define BEGQUAL_OFF 22  /* begin Omega quality code        */
#define UNIT_OFF    24  /* unit id                         */
#define SNAME_OFF   26  /* station name                    */
#define FORMAT_OFF  34  /* format                          */
#define CHANNEL_OFF 36  /* channel                         */
#define FILTER_OFF  38  /* filter                          */
#define NSAMP_OFF   40  /* no. samples                     */
#define ENDSYS_OFF  42  /* begin system time               */
#define ENDEXT_OFF  46  /* begin external time             */
#define ENDHZ_OFF   50  /* begin 1-Hz time                 */
#define ENDQUAL_OFF 54  /* begin Omega quality code        */
#define STREAM_OFF  56  /* stream ID                       */

int dhead_rev5(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
short stemp;
long  ltemp;
int compressed, longint, shrtint, fixedpt, decim_index;
static int firsterr = 1;
static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

    dest->extra = default_extra;

/*  Digitizer info  */

    memcpy(&stemp, src + UNIT_OFF, 2); SSWAB(&stemp, 1);
    if (stemp == 0) {
        dest->atod  = IDA_ARS;
        dest->srate = ARS_SRATE;
        decim_index = 0;
    } else {
        dest->atod  = IDA_DAS;
        dest->srate = DAS_SRATE;
        decim_index = 1;
    }

/*  Year and system time multiplier  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->beg.year = stemp;

    memcpy(&stemp, src + MULT_OFF, 2); SSWAB(&stemp, 1);
    dest->beg.mult = dest->end.mult = (unsigned long) stemp;

/*  Time tag of first datum  */

    memcpy(&ltemp, src + BEGSYS_OFF, 4); LSWAB(&ltemp, 1);
    dest->beg.sys = (unsigned long) ltemp;
    memcpy(&ltemp, src + BEGEXT_OFF, 4); LSWAB(&ltemp, 1);
    dest->beg.ext = (unsigned long) ltemp;
    memcpy(&ltemp, src + BEGHZ_OFF,  4); LSWAB(&ltemp, 1);
    dest->beg.hz  = (unsigned long) ltemp;
    memcpy(&dest->beg.code, src + BEGQUAL_OFF, 1);

/*  Time tag of last datum  */

    memcpy(&ltemp, src + ENDSYS_OFF, 4); LSWAB(&ltemp, 1);
    dest->end.sys = (unsigned long) ltemp;
    memcpy(&ltemp, src + ENDEXT_OFF, 4); LSWAB(&ltemp, 1);
    dest->end.ext = (unsigned long) ltemp;
    memcpy(&ltemp, src + ENDHZ_OFF,  4); LSWAB(&ltemp, 1);
    dest->end.hz  = (unsigned long) ltemp;
    memcpy(&dest->end.code, src + ENDQUAL_OFF, 1);

/*  Year of last datum, checking for possible year change in record  */

    dest->end.year = dest->beg.year;
    if (dest->end.ext < dest->beg.ext) ++dest->end.year;

/*  Fill in the rest of the time structure  */

    ida_filltime(ida, &dest->beg);
    ida_filltime(ida, &dest->end);

    if (dest->beg.sys >= dest->end.sys) {
        dest->beg.error |= TTAG_BAD_SYSTIM;
        dest->end.error |= TTAG_BAD_SYSTIM;
    }

/*  Data logger format bitmap  */

    memcpy(&stemp, src + FORMAT_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_format = stemp;

/*  Datalogger channel code  */

    memcpy(&stemp, src + CHANNEL_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_channel = stemp;
    if (dest->dl_channel < MIN_CHAN || dest->dl_channel > MAX_CHAN) return -101;

/*  Datalogger filter code and Fels' fuck up shift for this filter  */

    memcpy(&stemp, src + FILTER_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_filter = stemp;
    if (dest->dl_filter < MIN_FILT || dest->dl_filter > MAX_FILT) return -102;
    dest->ffu_shift = ffu_map[dest->dl_filter];

/*  Datalogger stream code  */

    memcpy(&stemp, src + STREAM_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_stream = stemp;
    if (dest->dl_stream < 0 || dest->dl_stream > IDA_MAXSTREAMS) return -103;

/*  Number of bytes, filter decimation, and stream sample interval */

    dest->nbytes = NBYTES;
    dest->decim  = decimation[decim_index][dest->dl_filter];
    dest->sint   = (float) dest->decim / (float) dest->srate;

/*  Number of samples  */

    memcpy(&stemp, src + NSAMP_OFF, 2); SSWAB(&stemp, 1);
    dest->nsamp = stemp;

/*  Get internal filter, mode, channel codes and sample word size */

    shrtint = (dest->dl_format & 0x0001); fixedpt = !shrtint;
    longint = (dest->dl_format & 0x0008);
    if (fixedpt && longint) return -104;
    dest->mode   = (dest->dl_format & 0x0004) ? TRIG : CONT;
    compressed = (dest->dl_format & 0x0002);
    if (longint) {
        dest->wrdsiz = 4;
        dest->form   = (compressed) ? IGPP_COMP : L_UNCOMP;
    } else if (shrtint) {
        dest->wrdsiz = 2;
        dest->form   = (compressed) ? IGPP_COMP : S_UNCOMP;
    } else if (fixedpt) {
        dest->wrdsiz = 4;
        dest->form   = (compressed) ? IGPP_COMP : FP32_UNCOMP;
    } else {
        dest->wrdsiz = 0;
        dest->form   = BAD_FORM;
    }

/*  If uncompressed, set last word to zero  */

    if (!compressed) dest->last_word = 0;

    dest->extra.valid = FALSE;

    return 0;
}

/* Revision History
 *
 * $Log: dhead5.c,v $
 * Revision 1.3  2007/06/01 19:05:28  dechavez
 * use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"
 *
 * Revision 1.2  2006/07/10 21:11:58  dechavez
 * define (unused) station name offset
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
