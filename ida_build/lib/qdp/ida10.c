#pragma ident "$Id: ida10.c,v 1.4 2008/01/07 21:22:38 dechavez Exp $"
/*======================================================================
 *
 *  QDP_HLP to IDA10.4
 *
 *====================================================================*/
#include "qdp.h"
#include "ida10.h"

#define IDA10_EPOCH_TO_1970_EPOCH 915148800
#define IDA10_1024_FIXED_LENGTH_NBYTES 974

BOOL qdpHlpToIDA10(UINT8 *start, QDP_HLP *hlp, UINT32 seqno)
{
UINT8 *ptr, descriptor;
INT16 fact, mult;
char chnloc[QDP_CNAME_LEN+QDP_LNAME_LEN+1];

    if (start == NULL || hlp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    ptr = start;

/* 10.4 TS header */

    *ptr++ = 'T';
    *ptr++ = 'S';
    *ptr++ = 10;
    *ptr++ = 4;

    /* serial number */

    ptr += utilPackUINT64(ptr, hlp->meta->combo.fix.sys_num);

    /* time stamp */

    ptr += utilPackUINT32(ptr, hlp->mn232.seqno);
    ptr += utilPackINT32(ptr, hlp->mn232.sec);
    ptr += utilPackINT32(ptr, hlp->mn232.usec);
    ptr += utilPackINT32(ptr, hlp->offset * hlp->nsint);
    ptr += utilPackINT32(ptr, hlp->delay);
    ptr += utilPackUINT16(ptr, hlp->mn232.loss);
    *ptr++ = hlp->mn232.qual;
    *ptr++ = qdpClockQuality(&hlp->meta->token.clock, &hlp->mn232);
    
    /* application supplied sequence number */

    ptr += utilPackUINT32(ptr, seqno);

    /* host time stamp */

    ptr += utilPackUINT32(ptr, time(NULL) - IDA10_EPOCH_TO_1970_EPOCH);

    /* reserved */

    *ptr++ = '1';
    *ptr++ = '0';
    *ptr++ = '.';
    *ptr++ = '4';

    /* number of bytes to follow (we are forcing 1024 byte packet length) */

    ptr += utilPackUINT16(ptr, IDA10_1024_FIXED_LENGTH_NBYTES);

    /* end of common header */

    /* stream name, a concatenation of chan and loc */

    sprintf(chnloc, "%s%2s", hlp->chn, hlp->loc);
    utilTrimString(chnloc);
    ptr += utilPackBytes(ptr, chnloc, 6);

    /* data format/status descriptor */

    switch (hlp->format) {
      case QDP_HLP_FORMAT_NOCOMP32:
        descriptor = 0;
        descriptor |= IDA10_COMP_NONE;
        descriptor |= IDA10_A2D_24;
        descriptor |= IDA10_MASK_INT32;
        /* trigger and calibration bits should be set */
        break;
      default:
        errno = ENOTSUP;
        return FALSE;
    }
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    ptr += utilPackUINT16(ptr, hlp->nsamp);

    /* sample rate */

    ida10SintToFactMult((REAL64) hlp->nsint / NANOSEC_PER_SEC, &fact, &mult);
    ptr += utilPackINT16(ptr, fact);
    ptr += utilPackINT16(ptr, mult);

    /* the data, already in network byte order */

    switch (hlp->format) {
      case QDP_HLP_FORMAT_NOCOMP32:
        utilPackBytes(ptr, hlp->data, hlp->nbyte);
        break;
      default:
        errno = ENOTSUP;
        return FALSE;
    }

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: ida10.c,v $
 * Revision 1.4  2008/01/07 21:22:38  dechavez
 * little-endian bug fixes
 *
 * Revision 1.3  2007/06/14 21:55:41  dechavez
 * generate fixed-length 1024 byte packets
 *
 * Revision 1.2  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
