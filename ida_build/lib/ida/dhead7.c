#pragma ident "$Id: dhead7.c,v 1.3 2007/06/01 19:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 7 data record header.
 *
 *  There seems to be a problem with either the way we decode the format
 *  bitmap, or with the way it is written.  As decoded here, the
 *  24-bit streams come out as 16:16 fixed point integers when in fact
 *  they are unshifted 32 bit integers.  Rather than muck with the DAS
 *  code, we just force the 24-bit streams to map out the way we know
 *  they should.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define NBYTES 960  /* number of bytes in data area */

#define MIN_FILT  0
#define MAX_FILT 20
#define NUM_FILT 21
#define NUM_FSET  3 /* number of filter sets in system (1 ARS + 2 DAS) */

static int decimation[NUM_FSET][NUM_FILT] = {
{
    /* rev 7 ARS filter  0 has decimation by */  1,
    /* rev 7 ARS filter  1 has decimation by */ -1,
    /* rev 7 ARS filter  2 has decimation by */ 10,
    /* rev 7 ARS filter  3 has decimation by */ -1,
    /* rev 7 ARS filter  4 has decimation by */ -1,
    /* rev 7 ARS filter  5 has decimation by */ -1,
    /* rev 7 ARS filter  6 has decimation by */ -1,
    /* rev 7 ARS filter  7 has decimation by */ -1,
    /* rev 7 ARS filter  8 has decimation by */ -1,
    /* rev 7 ARS filter  9 has decimation by */ -1,
    /* rev 7 ARS filter 10 has decimation by */  2,
    /* rev 7 ARS filter 11 has decimation by */ -1,
    /* rev 7 ARS filter 12 has decimation by */ -1,
    /* rev 7 ARS filter 13 has decimation by */ -1,
    /* rev 7 ARS filter 14 has decimation by */ -1,
    /* rev 7 ARS filter 15 has decimation by */ -1,
    /* rev 7 ARS filter 16 has decimation by */ -1,
    /* rev 7 ARS filter 17 has decimation by */ -1,
    /* rev 7 ARS filter 18 has decimation by */ -1,
    /* rev 7 ARS filter 19 has decimation by */ -1,
    /* rev 7 ARS filter 20 has decimation by */ -1
},
{
    /* rev 7 DAS 16-bit filter  0 has decimation by */  1,
    /* rev 7 DAS 16-bit filter  1 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  2 has decimation by */ 2000,
    /* rev 7 DAS 16-bit filter  3 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  4 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  5 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  6 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  7 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  8 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter  9 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 10 has decimation by */ 2,
    /* rev 7 DAS 16-bit filter 11 has decimation by */ 10,
    /* rev 7 DAS 16-bit filter 12 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 13 has decimation by */ 200,
    /* rev 7 DAS 16-bit filter 14 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 15 has decimation by */ 2000,
    /* rev 7 DAS 16-bit filter 16 has decimation by */ 5,
    /* rev 7 DAS 16-bit filter 17 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 18 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 19 has decimation by */ -1,
    /* rev 7 DAS 16-bit filter 20 has decimation by */ -1
},
{
    /* rev 7 DAS 24-bit filter  0 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  1 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  2 has decimation by */ 1000,
    /* rev 7 DAS 24-bit filter  3 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  4 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  5 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  6 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  7 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  8 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter  9 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 10 has decimation by */ 1,
    /* rev 7 DAS 24-bit filter 11 has decimation by */ 5,
    /* rev 7 DAS 24-bit filter 12 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 13 has decimation by */ 100,
    /* rev 7 DAS 24-bit filter 14 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 15 has decimation by */ 1000,
    /* rev 7 DAS 24-bit filter 16 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 17 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 18 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 19 has decimation by */ -1,
    /* rev 7 DAS 24-bit filter 20 has decimation by */ 1
}
};

#define MIN_CHAN 0
#define MAX_CHAN 24
#define NUM_CHAN 25

static int srate_map[NUM_CHAN] = {
/* rev 7 channel  0 sample rate = */  -1, /* not available */
/* rev 7 channel  1 sample rate = */  -1, /* not available */
/* rev 7 channel  2 sample rate = */  -1, /* not available */
/* rev 7 channel  3 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  4 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  5 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  6 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  7 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  8 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel  9 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 10 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 11 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 12 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 13 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 14 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 15 sample rate = */ 200, /* DAS,  16-bit  */
/* rev 7 channel 16 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 17 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 18 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 19 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 20 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 21 sample rate = */   1, /* ARS,  16-bit  */
/* rev 7 channel 22 sample rate = */ 100, /* DAS,  24-bit  */
/* rev 7 channel 23 sample rate = */ 100, /* DAS,  24-bit  */
/* rev 7 channel 24 sample rate = */ 100  /* DAS,  24-bit  */
};

#define bitsper(dl_channel) ((dl_channel >= 22 && dl_channel <= 24) ? 24 : 16)

#define YEAR_OFF     6  /* year                            */
#define MULT_OFF     8  /* system and 1-hz time multiplier */
#define BEGSYS_OFF  10  /* begin system time               */
#define BEGEXT_OFF  14  /* begin external time             */
#define BEGHZ_OFF   18  /* begin 1-Hz time                 */
#define BEGQUAL_OFF 22  /* begin Omega quality code        */
#define UNIT_OFF    24  /* unit id                         */
#define FORMAT_OFF  34  /* format                          */
#define CHANNEL_OFF 36  /* channel                         */
#define FILTER_OFF  38  /* filter                          */
#define NSAMP_OFF   40  /* no. samples                     */
#define ENDSYS_OFF  42  /* begin system time               */
#define ENDEXT_OFF  46  /* begin external time             */
#define ENDHZ_OFF   50  /* begin 1-Hz time                 */
#define ENDQUAL_OFF 54  /* begin Omega quality code        */
#define STREAM_OFF  56  /* stream ID                       */

int dhead_rev7(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
short stemp;
long  ltemp;
int compressed, longint, shrtint, fixedpt, decim_index;
static int firsterr = 1;
static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

    dest->rt593 = 0;
    dest->extra = default_extra;

/*  Digitizer info  */

    memcpy(&stemp, src + UNIT_OFF, 2); SSWAB(&stemp, 1);
    if (stemp == 3) {
        dest->atod  = IDA_ARS;
        decim_index = 0;
    } else if (stemp == 2) {
        dest->atod  = IDA_DAS;
        decim_index = 1;
    } else {
        return -101;
    }

/*  Year and system time multiplier  */

    memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
    dest->beg.year = dest->end.year = stemp;

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
    if (dest->dl_channel < MIN_CHAN || dest->dl_channel > MAX_CHAN) return -102;

/*  Datalogger filter code  */

    memcpy(&stemp, src + FILTER_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_filter = stemp;
    if (dest->dl_filter < MIN_FILT || dest->dl_filter > MAX_FILT) return -103;

/*  Datalogger stream code  */

    memcpy(&stemp, src + STREAM_OFF, 2); SSWAB(&stemp, 1);
    dest->dl_stream = stemp;
    if (dest->dl_stream < 0 || dest->dl_stream > IDA_MAXSTREAMS) return -104;

/* Fel's has hardcoded the mass postion shift assuming the default config! */
/* What an asshole... anyway, here is the default mass position config and */
/* it's associated shift.                                                  */

    if (dest->dl_filter == 15 && (dest->dl_channel >= 12 && dest->dl_channel <= 14)) dest->ffu_shift = 16;

/*  Number of bytes, filter decimation, and stream sample interval */

    dest->nbytes = NBYTES;
    if (bitsper(dest->dl_channel) == 24) decim_index = 2;
    dest->decim  = decimation[decim_index][dest->dl_filter];
    dest->srate  = srate_map[dest->dl_channel];
    dest->sint   = (float) dest->decim / (float) dest->srate;

/*  Number of samples  */

    memcpy(&stemp, src + NSAMP_OFF, 2); SSWAB(&stemp, 1);
    dest->nsamp = stemp;

/*  Get internal filter, mode, channel codes and sample word size */

    shrtint = (dest->dl_format & 0x0001); fixedpt = !shrtint;
    longint = (dest->dl_format & 0x0008);
    if (fixedpt && longint) return -105;
    dest->mode = (dest->dl_format & 0x0004) ? TRIG : CONT;
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
        return -105;
    }

/*  If uncompressed, set last word to zero  */

    if (!compressed) dest->last_word = 0;

/*  KLUDGE - see comments at top  */
/*  Force 24 bit streams to be uncompressed longs (bitmap is incorrect?)  */

    if (bitsper(dest->dl_channel) == 24) {
        dest->wrdsiz = 4;
        dest->form   = L_UNCOMP;
    }

    return 0;
}

/* Revision History
 *
 * $Log: dhead7.c,v $
 * Revision 1.3  2007/06/01 19:05:28  dechavez
 * use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"
 *
 * Revision 1.2  2007/01/25 20:26:11  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
