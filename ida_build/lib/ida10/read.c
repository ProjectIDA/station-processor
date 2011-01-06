#pragma ident "$Id: read.c,v 1.9 2008/04/02 20:35:31 dechavez Exp $"
/*======================================================================
 *
 *  Read a variable length record of any format rev
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

static int ReadIspLogRec(FILE *fp, UINT8 *buf, int buflen)
{
int ReadLen;

    if (buflen < IDA10_ISPLOGLEN) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }
    ReadLen = IDA10_ISPLOGLEN - IDA10_PREAMBLE_LEN;
    if (fread(buf, 1, ReadLen, fp) != (size_t) ReadLen) return IDA10_IOERR;
    return IDA10_OK;
}

int ida10ReadRecord(FILE *fp, UINT8 *buffer, int buflen, int *pType)
{
UINT8 *ptr;
IDA10_CMNHDR cmn;
int ReadLen, CmnHdrLen;

    if (buffer == NULL || pType == NULL) {
        errno = EINVAL;
        return IDA10_EINVAL;
    }

    if (buflen < IDA10_PREAMBLE_LEN) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

/* Read the preamble and determine the type */

    ptr = buffer;
    ReadLen = IDA10_PREAMBLE_LEN;
    if (fread(ptr, 1, ReadLen, fp) != (size_t) ReadLen) {
        return feof(fp) ? IDA10_EOF : IDA10_IOERR;
    }

    if ((*pType = ida10Type(ptr)) == IDA10_TYPE_UNKNOWN) {
        return IDA10_DATAERR;
    }
    ptr += ReadLen;

/* Special handling for the ISP logs */

    if (*pType == IDA10_TYPE_ISPLOG) return ReadIspLogRec(fp, ptr, buflen);

/* Remaining records must follow IDA10 format rules */

    switch (ida10SubFormatCode(buffer)) {
      case IDA10_SUBFORMAT_0:
        CmnHdrLen = IDA10_SUBFORMAT_0_HDRLEN;
        break;
      case IDA10_SUBFORMAT_1:
        CmnHdrLen = IDA10_SUBFORMAT_1_HDRLEN;
        break;
      case IDA10_SUBFORMAT_2:
        CmnHdrLen = IDA10_SUBFORMAT_2_HDRLEN;
        break;
      case IDA10_SUBFORMAT_3:
        CmnHdrLen = IDA10_SUBFORMAT_3_HDRLEN;
        break;
      case IDA10_SUBFORMAT_4:
        CmnHdrLen = IDA10_SUBFORMAT_4_HDRLEN;
        break;
      case IDA10_SUBFORMAT_5:
        CmnHdrLen = IDA10_SUBFORMAT_5_HDRLEN;
        break;
      default:
        return IDA10_DATAERR;
    }

/* Read and decode common header */

    if (buflen < CmnHdrLen) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }

    ReadLen = CmnHdrLen - IDA10_PREAMBLE_LEN;

    if (fread(ptr, 1, ReadLen, fp) != (size_t) ReadLen) return IDA10_IOERR;
    ptr += ReadLen;

    if (ida10UnpackCmnHdr(buffer, &cmn) == 0) return IDA10_DATAERR;

/* Read the rest of the record */

    if (buflen < ida10PacketLength(&cmn)) {
        errno = EMSGSIZE;
        return IDA10_EMSGSIZE;
    }
    ReadLen = cmn.nbytes;
    if (fread(ptr, 1, ReadLen, fp) != (size_t) ReadLen) return IDA10_IOERR;

    return IDA10_OK;
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.9  2008/04/02 20:35:31  dechavez
 * added ida10.5 support
 *
 * Revision 1.8  2006/12/08 17:26:43  dechavez
 * 10.4 support
 *
 * Revision 1.7  2006/11/10 06:19:47  dechavez
 * removed some debug printfs
 *
 * Revision 1.6  2006/08/19 02:18:34  dechavez
 * removed trailing CVS junk from remarks
 *
 * Revision 1.5  2006/08/19 02:17:46  dechavez
 * further 10.3 support
 *
 * Revision 1.4  2005/08/26 18:41:44  dechavez
 * added 10.2 support
 *
 * Revision 1.3  2005/05/25 22:38:06  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 18:18:40  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.1  2002/03/15 22:41:20  dec
 * created
 *
 */
