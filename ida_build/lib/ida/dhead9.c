#pragma ident "$Id: dhead9.c,v 1.4 2008/03/10 16:38:56 dechavez Exp $"
/*======================================================================
 *
 *  Load a rev 9 data record header.
 *
 *  Rev 9 is similar rev 8 except that it has sequence numbers and
 *  creation time stamps.
 *
 *====================================================================*/
#include "ida.h"
#include "protos.h"

#define IDA9x_OFF    2  /* format identifier */
#define SUBFORM_OFF 23  /* IDA 9.x subformat */
#define SEQNO_OFF   30  /* packet sequence number */
#define RT593_OFF   55  /* system time delay factor */
#define TSTAMP_OFF  60  /* creation time stamp    */

#define IDA9_SUBFORMAT_OFF 23

int dhead_rev9(IDA *ida, IDA_DHDR *dest, UINT8 *src)
{
int retval;
static char *ida9_X = IDA_REV9_TAG_X;

    if ((retval = dhead_rev8(ida, dest, src)) != 0) return retval;

    utilUnpackUINT32(&src[SEQNO_OFF], &dest->extra.seqno);
    utilUnpackUINT32(&src[TSTAMP_OFF], &dest->extra.tstamp);
    dest->extra.valid = TRUE;

/* Done now if this is not an 9.x header */

    if (memcmp(src + IDA9x_OFF, (void *) ida9_X, strlen(ida9_X)) != 0) return 0;

/* Do the 9.x stuff */

    dest->subformat = src[SUBFORM_OFF] & 0x0f;
    switch (dest->subformat) {
      case 0:
        break;
      case 1:
        if ((dest->rt593 = (int) src[RT593_OFF]) != 0) {
            if (dest->beg.error & TTAG_BAD_OFFSET) dest->beg.error ^= TTAG_BAD_OFFSET;
            if (dest->end.error & TTAG_BAD_OFFSET) dest->end.error ^= TTAG_BAD_OFFSET;
        }
        break;
      default:
        return -900;
    }

    dest->flags = src[SUBFORM_OFF] & 0xf0;
    if (dest->flags & IDA9_FLAG_LEAPYEAR_BUG) {
        dest->beg.tru += 86400.;
        dest->beg.error |= TTAG_LEAP_YEAR_PATCH;
        dest->end.tru += 86400.;
        dest->end.error |= TTAG_LEAP_YEAR_PATCH;
    }

    return 0;
}

/* Revision History
 *
 * $Log: dhead9.c,v $
 * Revision 1.4  2008/03/10 16:38:56  dechavez
 * leave ext time alone when applying TTAG_LEAP_YEAR_PATCH
 *
 * Revision 1.3  2008/03/05 22:51:11  dechavez
 * added support for TTAG_LEAP_YEAR_PATCH flag
 *
 * Revision 1.2  2007/01/25 20:26:11  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.1  2006/02/08 22:57:42  dechavez
 * File derived from db*.c version of same.  Introduction of IDA handle.
 *
 */
