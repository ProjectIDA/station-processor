#pragma ident "$Id: dhead3.c,v 1.2 2007/06/01 19:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 3 data record header.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define REV3_DECIM     10  /* filter decimation factor      */
#define REV3_SRATE    200  /* digitizer sample rate         */
#define REV3_SINT    (float) 0.05  /* stream sample interval*/
#define NBYTES        960  /* number of bytes in data area  */
#define MULT            5  /* milliseconds per clock tic    */
#define TICRATE 1000/MULT  /* no. clock tics per second     */

/*  Filter limits  */

#define MIN_FILT 0
#define MAX_FILT 1

/*  Mode codes  */

static int mode_map[] = {
/* rev 3 filter 0 = */ BAD_MODE,
/* rev 3 filter 1 = */ CONT
};

/*  Mapping from header clock quality code back to Omega code  */

static char qual_map[7] = {0, ' ', '.', '*', '#', '?', '-'};

/*  Offsets to header info  */

#define FORMAT_OFF   2  /* format                                  */
#define CHANNEL_OFF  3  /* channel                                 */
#define FILTER_OFF   4  /* filter                                  */
#define TQUAL_OFF    5  /* time quality code                       */
#define EXTSC_OFF    6  /* external time, seconds field            */
#define EXTMS_OFF   10  /* external time, milliseconds field       */
#define HTZSC_OFF   12  /* 1-Hz time, seconds field                */
#define HTZMS_OFF   16  /* 1-Hz time, milliseconds field           */
#define SYSSC_OFF   18  /* datum time tag, seconds field           */
#define SYSMS_OFF   22  /* datum time tag, milliseconds field      */
#define LASTW_OFF   34  /* last word                               */
#define NSAMP_OFF   42  /* number of samples                       */
#define SINT_OFF    46  /* digitizer sample interval, milliseconds */
#define NRTS_FLAG   52  /* "NRTS" flag to indicate year is present */
#define YEAR_OFF    56  /* NRTS added year                         */

int dhead_rev3(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
char  ctemp;
short stemp;
long  ltemp;
static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

    dest->extra = default_extra;

/*  Format code  */

    memcpy(&ctemp, src + FORMAT_OFF, 1);
    dest->dl_format = (short) ctemp;

    switch (dest->dl_format) {
      case 'I': dest->form = S_UNCOMP;  break;
      case 'L': dest->form = L_UNCOMP;  break;
      case 'C': dest->form = IGPP_COMP; break;
      default:  dest->form = BAD_FORM;
    }

/*  Channel code  */

    memcpy(&ctemp, src + CHANNEL_OFF, 1);
    dest->dl_channel = (short) ctemp;

/*  Filter/mode codes  */

    memcpy(&ctemp, src + FILTER_OFF, 1);
    dest->dl_filter = (short) ctemp;

    if (dest->dl_filter >= MIN_FILT && dest->dl_filter <= MAX_FILT) {
        dest->mode  =  mode_map[dest->dl_filter];
    } else {
        dest->mode = BAD_MODE;
    }

/*  Following are fixed for all rev 3 data  */

    dest->nbytes = NBYTES;
    dest->srate  = REV3_SRATE;
    dest->decim  = REV3_DECIM;
    dest->sint   = REV3_SINT;

/*  Number of samples  */

    memcpy(&stemp, src + NSAMP_OFF, 2); SSWAB(&stemp, 1);
    dest->nsamp = stemp;

/*  Digitizer code  */

    dest->atod = IDA_DAS;

/*  Uncompressed word size  */

    dest->wrdsiz = 2;

/*  Uncompressed last word  */

    memcpy(&ltemp, src + LASTW_OFF, 4); LSWAB(&ltemp, 1);
    dest->last_word = ltemp;

/*  Time tag of first datum  */

    if (memcmp(src + NRTS_FLAG, "NRTS", 4) == 0) {
        memcpy(&stemp, src + YEAR_OFF, 2); SSWAB(&stemp, 1);
        dest->beg.year = stemp;
    } else {
        dest->beg.year = IDA_UNKNOWN;
    }
    
    dest->beg.mult = MULT;

    memcpy(&ctemp, src + TQUAL_OFF, 1);
    stemp = (short) ctemp;
    if (stemp < 1 || stemp > 6) stemp = 0;
    dest->beg.code = qual_map[stemp];

    memcpy(&ltemp, src + EXTSC_OFF, 4); LSWAB(&ltemp, 1);
    dest->beg.ext = (unsigned long) ltemp + 86400;

    memcpy(&ltemp, src + HTZSC_OFF, 4); LSWAB(&ltemp, 1);
    memcpy(&stemp, src + HTZMS_OFF, 2); SSWAB(&stemp, 1);
    dest->beg.hz = (unsigned long) (ltemp * TICRATE) + 
                   (unsigned long) (stemp / MULT);

    memcpy(&ltemp, src + SYSSC_OFF, 4); LSWAB(&ltemp, 1);
    memcpy(&stemp, src + SYSMS_OFF, 2); SSWAB(&stemp, 1);
    dest->beg.sys = (unsigned long) (ltemp * TICRATE) + 
                    (unsigned long) (stemp / MULT);

/*  Fake a "proper" time tag for last datum in buffer  */

    dest->end = dest->beg;
    dest->end.sys += (unsigned long) ((dest->nsamp - 1) * dest->decim);

/*  Fill in the rest of the time structure  */

    ida_filltime(ida, &dest->beg);
    ida_filltime(ida, &dest->end);

/*  ida_filltime() flags hz > sys as an error, but it is OK in this rev  */

    dest->beg.error &= ~TTAG_BAD_OFFSET;
    dest->end.error &= ~TTAG_BAD_OFFSET;

/*  Fake a stream number from the channel map */

    dest->dl_stream = idaFakeStreamNumber(ida, dest->dl_channel, dest->dl_filter);

    return 0;
}

/* Revision History
 *
 * $Log: dhead3.c,v $
 * Revision 1.2  2007/06/01 19:05:28  dechavez
 * use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
