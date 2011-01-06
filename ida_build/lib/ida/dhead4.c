#pragma ident "$Id: dhead4.c,v 1.2 2007/06/01 19:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 4 data record header.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define NBYTES        960  /* number of bytes in data area */
#define MULT            5  /* milliseconds per clock tic   */
#define TICRATE 1000/MULT  /* no. clock tics per second    */

/*  Mapping from header clock quality code back to Omega code  */

static char qual_map[7] = {0, ' ', '.', '*', '#', '?', '!'};

/*  Offsets to header info  */

#define MODE_OFF     1  /* mode                                    */
#define FORMAT_OFF   2  /* format                                  */
#define CHANNEL_OFF  3  /* channel                                 */
#define FILTER_OFF   4  /* filter                                  */
#define STREAM_OFF   5  /* stream ID                               */
#define EXTSC_OFF    6  /* external time, seconds field            */
#define EXTMS_OFF   10  /* external time, milliseconds field       */
#define HTZSC_OFF   12  /* 1-Hz time, seconds field                */
#define HTZMS_OFF   16  /* 1-Hz time, milliseconds field           */
#define SYSSC_OFF   18  /* datum time tag, seconds field           */
#define SYSMS_OFF   22  /* datum time tag, milliseconds field      */
#define TQUAL_OFF   25  /* time quality code                       */
#define LASTW_OFF   34  /* last word                               */
#define NSAMP_OFF   42  /* number of samples                       */
#define DECIM_OFF   44  /* filter decimation factor                */
#define SINT_OFF    46  /* digitizer sample interval, milliseconds */

#define MAX_DAS_STREAM 11 /* streams 1-11 are DAS, all others ARS */

int dhead_rev4(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
char  ctemp;
short stemp;
long  ltemp;
static IDA_EXTRA default_extra = IDA_DEFAULT_EXTRA;

    dest->extra = default_extra;

/*  Mode code  */

    memcpy(&ctemp, src + MODE_OFF, 1);
    
    switch (ctemp) {
      case 'C': dest->mode = CONT; break;
      case 'T': dest->mode = TRIG; break;
      default:  dest->mode = BAD_MODE;
    }

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

/*  Filter code  */

    memcpy(&ctemp, src + FILTER_OFF, 1);
    dest->dl_filter = (short) ctemp;

/*  Stream code  */

    memcpy(&ctemp, src + STREAM_OFF, 1);
    dest->dl_stream = (short) ctemp;

/*  Filter decimation factor  */

    memcpy(&stemp, src + DECIM_OFF, 2); SSWAB(&stemp, 1);
    dest->decim = stemp;

/*  Digitizer sample rate  */

    memcpy(&stemp, src + SINT_OFF, 2); SSWAB(&stemp, 1);
    dest->srate = (int) (1000.0 / (float) stemp);

/*  Number of bytes and stream sample interval */

    dest->nbytes = NBYTES;
    dest->sint   = (float) dest->decim / (float) dest->srate;

/*  Number of samples  */

    memcpy(&stemp, src + NSAMP_OFF, 2); SSWAB(&stemp, 1);
    dest->nsamp = stemp;

/*  Digitizer code  */

    dest->atod = (dest->dl_stream <= 18) ? IDA_DAS : IDA_ARS;

/*  Uncompressed word size  */

    dest->wrdsiz = (dest->dl_stream <= 11) ? 4 : 2;

/*  Uncompressed last word  */

    memcpy(&ltemp, src + LASTW_OFF, 4); LSWAB(&ltemp, 1);
    dest->last_word = ltemp;

/*  Time tag of first datum  */

    dest->beg.year = IDA_UNKNOWN;
    dest->beg.mult = MULT;

    memcpy(&ctemp, src + TQUAL_OFF, 1);
    stemp = (short) ctemp;
    if (stemp < 1 || stemp > 6) stemp = 0;
    dest->beg.code = qual_map[stemp];

    memcpy(&ltemp, src + EXTSC_OFF, 4); LSWAB(&ltemp, 1);
    dest->beg.ext = (unsigned long) ltemp;

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

    return 0;
}

/* Revision History
 *
 * $Log: dhead4.c,v $
 * Revision 1.2  2007/06/01 19:05:28  dechavez
 * use IDA_DEFAULT_EXTRA to initialize IDA_DHDR "extra"
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
