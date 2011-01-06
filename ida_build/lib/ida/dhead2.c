#pragma ident "$Id: dhead2.c,v 1.2 2007/06/01 19:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 2 data record header.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define REV2_SRATE 200 /* digitizer sample rate          */
#define NBYTES     960 /* number of bytes in data area   */
#define MULT         5 /* milliseconds per clock tic     */

/* Filter limits */

#define MIN_FILT 0
#define MAX_FILT 7
#define NUM_FILT 8

/*  Filter decimation factors  */

static int decimation[NUM_FILT] = {
/* rev 2 DAS filter 0 has decimation by */   -1,
/* rev 2 DAS filter 1 has decimation by */   10,
/* rev 2 DAS filter 2 has decimation by */   -1,
/* rev 2 DAS filter 3 has decimation by */   -1,
/* rev 2 DAS filter 4 has decimation by */   -1,
/* rev 2 DAS filter 5 has decimation by */   -1,
/* rev 2 DAS filter 6 has decimation by */   40,
/* rev 2 DAS filter 7 has decimation by */ 2000
};

/*  Mode codes  */

static int mode_map[NUM_FILT] = {
/* rev 2 filter 0 = */ BAD_MODE,
/* rev 2 filter 1 = */ CONT,
/* rev 2 filter 2 = */ BAD_MODE,
/* rev 2 filter 3 = */ BAD_MODE,
/* rev 2 filter 4 = */ BAD_MODE,
/* rev 2 filter 5 = */ BAD_MODE,
/* rev 2 filter 6 = */ CONT,
/* rev 2 filter 7 = */ CONT
};

/* Channel limits */

#define MIN_CHAN 0
#define MAX_CHAN 5

/*  Offsets to header info  */

#define TQUAL_OFF   20  /* time quality code                      */
#define FORMAT_OFF  38  /* format                                 */
#define CHANNEL_OFF 40  /* channel                                */
#define FILTER_OFF  41  /* filter                                 */
#define LASTW_OFF   42  /* last word                              */
#define TIME_OFF    50  /* time tag (there is only one in rev 2)  */
#define NSAMP_OFF   60  /* number of samples                      */

int dhead_rev2(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
char  ctemp;
short stemp;
long  ltemp;
unsigned long ultemp;
static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

    dest->extra = default_extra;

/*  Format code  */

    memcpy(&ctemp, src + FORMAT_OFF, 1);
    dest->dl_format = (short) ctemp;

    switch (dest->dl_format) {
      case 'I': dest->form = S_UNCOMP;  break;
      case 'C': dest->form = IGPP_COMP; break;
      default:  return -101;
    }

/*  Channel code  */

    memcpy(&ctemp, src + CHANNEL_OFF, 1);
    dest->dl_channel = (short) ctemp;

    if (dest->dl_channel < MIN_CHAN || dest->dl_channel > MAX_CHAN) return -102;

/*  Filter/mode codes  */

    memcpy(&ctemp, src + FILTER_OFF, 1);
    dest->dl_filter = (short) ctemp;

    if (dest->dl_filter < MIN_FILT || dest->dl_filter > MAX_FILT) return -103;

    dest->mode = mode_map[dest->dl_filter];

/*  Following are fixed for all rev 2 data  */

    dest->nbytes = NBYTES;
    dest->srate  = REV2_SRATE;

/*  Number of samples  */

    memcpy(&stemp, src + NSAMP_OFF, 2); SSWAB(&stemp, 1);
    dest->nsamp = stemp;

/*  Digitizer code  */

    dest->atod = IDA_DAS;

/*  Uncompressed word size  */

    dest->wrdsiz = (dest->dl_filter == 7) ? 4 : 2;

/*  Uncompressed last word  */

    memcpy(&ltemp, src + LASTW_OFF, 4); LSWAB(&ltemp, 1);
    dest->last_word = ltemp;

/*  Sample interval  */

    dest->decim  = decimation[dest->dl_filter];
    dest->sint  = (float) dest->decim / (float) dest->srate;

/*  Time tag of first datum  */

    dest->beg.year = IDA_UNKNOWN;
    
    dest->beg.mult = MULT;

    memcpy(&dest->beg.code, src + TQUAL_OFF, 1);
    if (dest->beg.code == 0) dest->beg.code = '-'; /* missed qual code */
    
    memcpy(&ultemp, src + TIME_OFF, 4); LSWAB(&ultemp, 1);
    dest->beg.sys = (ultemp * 10) / MULT;
    dest->beg.ext = (ultemp / 100);
    dest->beg.hz  = (dest->beg.ext * 1000) / MULT;
    dest->beg.ext += 86400;

/*  Fake a "proper" time tag for last datum in buffer  */

    dest->end = dest->beg;
    dest->end.sys += (unsigned long) ((dest->nsamp - 1) * dest->decim);

/*  Fill in the rest of the time structure  */

    ida_filltime(ida, &dest->beg);
    ida_filltime(ida, &dest->end);

/*  filltime() flags hz > sys as an error, but it is OK in this rev  */

    dest->beg.error &= ~TTAG_BAD_OFFSET;
    dest->end.error &= ~TTAG_BAD_OFFSET;

/*  Fake a stream number from the channel map */

    dest->dl_stream = idaFakeStreamNumber(ida, dest->dl_channel, dest->dl_filter);

    return 0;
}

/* Revision History
 *
 * $Log: dhead2.c,v $
 * Revision 1.2  2007/06/01 19:05:28  dechavez
 * use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
