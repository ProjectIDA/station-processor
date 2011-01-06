#pragma ident "$Id: site.c,v 1.13 2007/09/06 18:26:18 dechavez Exp $"
/*======================================================================
 *
 * IDA data exchange protocols, site specific routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "xfer.h"

#ifdef IDA_SUPPORT

/* Compress an IDA data logger record */

static int idaN_compress(IDA *ida, int *comp, char *input, char *output)
{
int cbyte;
char *cptr;
IDA_DHDR dhead;
int hlen, dlen;
long nsamp;
static union {
    long  l[IDA_MAXDLEN/4];   /* uncompressed long   */
    short s[IDA_MAXDLEN/2];   /* uncompressed shorts */
} data;
static char  cdata[(IDA_MAXDLEN/4)*5];  /* compressed data */
static char  *fid = "idaN_compress";

/* Non-data and corrupt data records are sent uncompressed */

    if (
        ida_rtype(input, ida->rev.value) != IDA_DATA ||
        ida_dhead(ida, &dhead, input) != 0 ||
        (dhead.wrdsiz != 2 && dhead.wrdsiz != 4)
    ) {
        *comp = XFER_CMPNONE;
    }

    ida_dhlen(ida->rev.value, &hlen, &dlen);
    nsamp = (long) dhead.nsamp;
    if (dlen != (dhead.wrdsiz * dhead.nsamp)) {
        util_log(1, "%s: dlen %d != wrdsiz %d * nsamp %hd\n",
            fid, dlen, dhead.wrdsiz, dhead.nsamp
        );
        *comp = XFER_CMPNONE;
    }

/* Do the compression */

    switch (*comp) {

      case XFER_CMPIGPP:

        /*  Compress, first converting to host byte order if necessary */

        if (dhead.wrdsiz == 4) {
            memcpy((char *) data.l, input + hlen, dlen);
            if (dhead.order != util_order()) util_lswap(data.l, nsamp);
            cbyte = util_lcomp(cdata, data.l, nsamp);
        } else {
            memcpy((char *) data.s, input + hlen, dlen);
            if (dhead.order != util_order()) util_sswap(data.s, nsamp);
            cbyte = util_scomp(cdata, data.s, nsamp);
        }

        /* Check results and only keep it if the size reduced */

        if (cbyte > 0 && cbyte < dlen) {
            cptr = cdata;
        } else {
            *comp = XFER_CMPNONE;
        }

        break;

      case XFER_CMPNONE:
      default:
        *comp = XFER_CMPNONE;
    }

    if (*comp == XFER_CMPNONE) {
        cptr  = input + hlen;
        cbyte = dlen;
    }

/* Copy the results to the output buffer */

    memcpy(output, input, hlen);
    memcpy(output+hlen, cptr, cbyte);

    return cbyte + hlen;
}

/* Decompress an IDA data logger record */

static char *idaN_decompress(IDA *ida, int *comp, long *nbyte, char *input)
{
IDA_DHDR dhead;
int hlen, dlen, unused;
long nsamp;
static char  output[IDA_BUFSIZ];        /* header + compressed data */
static union {
    long  l[IDA_MAXDLEN/4];   /* uncompressed long   */
    short s[IDA_MAXDLEN/2];   /* uncompressed shorts */
} data;
static char  *fid = "idaN_decompress";

    if (*comp == XFER_CMPNONE) return input;

/* This had better be a (D)ata record, as all others were supposed to
 * have been sent uncompressed.
 */
    if (
        ida_rtype(input, ida->rev.value) != IDA_DATA ||
        ida_dhead(ida, &dhead, input) != 0 ||
        (dhead.wrdsiz != 2 && dhead.wrdsiz != 4)
    ) {
        util_log(1, "%s: non-data or corrupt record sent compressed?", fid);
        return NULL;
    }

    ida_dhlen(ida->rev.value, &hlen, &dlen);
    nsamp = (long) dhead.nsamp;
    if (dlen != (dhead.wrdsiz * dhead.nsamp)) {
        util_log(1, "%s: dlen %d != wrdsiz %d * nsamp %hd\n",
            fid, dlen, dhead.wrdsiz, dhead.nsamp
        );
        return NULL;
    }

/* Do the decompression, restoring original byte order if required */

    memcpy(output, input, hlen);

    switch (*comp) {

      case XFER_CMPIGPP:

        if (dhead.wrdsiz == 4) {
            util_ldcmp(data.l, input + hlen, nsamp);
            if (dhead.order != util_order()) util_lswap(data.l, nsamp);
            memcpy(output + hlen, (char *) data.l, dlen);
        } else {
            util_sdcmp(data.s, input + hlen, nsamp);
            if (dhead.order != util_order()) util_sswap(data.s, nsamp);
            memcpy(output + hlen, (char *) data.s, dlen);
        }

        break;

      default:
        return NULL;
    }

    *comp  = XFER_CMPNONE;
    *nbyte = hlen + dlen;
    if (*nbyte < IDA_BUFSIZ) {
        unused = IDA_BUFSIZ - (*nbyte);
        memset(output + hlen + dlen, 0, unused);
        *nbyte += unused;
    }
    return output;
}

/* Compress an IDA10 data logger record */

#ifdef SUNOS

static int ida10_compress(int *comp, char *input, char *output, int reclen)
{
    return -1;
}

static char *ida10_decompress(int *comp, long *nbyte, char *input)
{
    return (char *) NULL;
}

#else

static int ida10_compress(int *comp, char *input, char *output, int datlen)
{
int cbyte, len;
char *cptr;
IDA10_TS ts;
IDA10_CMNHDR cmn;
static char  cdata[(IDA10_MAXDATALEN/4)*5];  /* compressed data */
static char  *fid = "ida10_compress";

/* Non-data records are sent uncompressed, corrupt records are ignored */

    if (!ida10UnpackTS((UINT8 *)input, &ts)) {
        if (ida10UnpackCmnHdr((UINT8 *) input, &cmn)) {
            len = ida10PacketLength(&cmn);
            memcpy(output, input, len);
            *comp = XFER_CMPNONE;
            return len;
        } else {
            util_log(1, "%s: corrupt record received!", fid);
            return -1;
        }
    }

/* Do the compression */

    switch (*comp) {

      case XFER_CMPIGPP:

        if (ts.hdr.datatype == IDA10_DATA_INT32) {
            cbyte = util_lcomp(cdata, (long *) ts.data.int32, ts.hdr.nsamp);

            /* Check results and only keep it if the size reduced */

            if (cbyte > 0 && cbyte < datlen) {
                cptr = cdata;
            } else {
                cptr  = input  + IDA10_TSHEADLEN;
                cbyte = datlen;
                *comp = XFER_CMPNONE;
            }
        } else {
            cptr  = input  + IDA10_TSHEADLEN;
            cbyte = datlen;
            *comp = XFER_CMPNONE;
        }

        break;

      case XFER_CMPNONE:
      default:
        cptr  = input  + IDA10_TSHEADLEN;
        cbyte = datlen;
        *comp = XFER_CMPNONE;
    }

/* Copy the results to the output buffer */

    memcpy(output, input, IDA10_TSHEADLEN);
    memcpy(output+IDA10_TSHEADLEN, cptr, cbyte);

    return IDA10_TSHEADLEN + cbyte;
}

/* Decompress an IDA10 data logger record */

static char *ida10_decompress(int *comp, long *nbyte, char *input)
{
IDA10_TSHDR tshdr;
long i, *lptr;
static char output[IDA10_MAXRECLEN];  /* header + uncompressed data */
static char  *fid = "ida10_decompress";

    if (*comp == XFER_CMPNONE) return input;

/* This had better be a TS record, as all others were supposed to
 * have been sent uncompressed.
 */
    if (ida10UnpackTSHdr((UINT8 *)input, &tshdr) == 0) {
        util_log(1, "%s: non-data or corrupt record sent compressed?", fid);
        return NULL;
    }

/* Sanity checks on header contents */

    if (tshdr.nsamp > IDA10_MAXDATALEN/4) {
        util_log(1, "%s: tshdr.nsamp %d > %d!",
            fid, tshdr.nsamp, IDA10_MAXDATALEN/4
        );
        return NULL;
    }

/* Header is copied over intact */

    memcpy(output, input, IDA10_TSHEADLEN);

/* Decompress the data portion, restore to NBO */

    switch (*comp) {

      case XFER_CMPIGPP:

        lptr = (long *) (output + IDA10_TSHEADLEN);
        util_ldcmp(lptr, input + IDA10_TSHEADLEN, tshdr.nsamp);
        for (i = 0; i < tshdr.nsamp; i++) lptr[i] = htonl(lptr[i]);

        break;

      default:
        util_log(1, "%s: unsupported compression type (%d)", fid,*comp);
        return NULL;
    }

    *nbyte = IDA10_TSHEADLEN + (tshdr.nsamp * sizeof(INT32));
    return output;
}

#endif

int ida_compress(IDA *ida, int *comp, UINT8 *input, UINT8 *output, int datlen)
{
    if (ida->rev.value != 10) {
        return idaN_compress(ida, comp, input, output);
    } else {
        return ida10_compress(comp, input, output, datlen);
    }
}

UINT8 *ida_decompress(IDA *ida, int *comp, long *nbyte, UINT8 *input)
{
    if (ida->rev.value != 10) {
        return idaN_decompress(ida, comp, nbyte, input);
    } else {
        return ida10_decompress(comp, nbyte, input);
    }
}

#endif /* IDA_SUPPORT */

/* Revision History
 *
 * $Log: site.c,v $
 * Revision 1.13  2007/09/06 18:26:18  dechavez
 * changed ida_compress(), ida_decompress() byte args from char to UINT8
 *
 * Revision 1.12  2006/11/10 06:29:40  dechavez
 * IDA10_DATA_INT32 instead of format specific IDA10_TYPE_INT32
 *
 * Revision 1.11  2006/02/09 20:01:48  dechavez
 * IDA handle support
 *
 * Revision 1.10  2005/05/25 22:41:56  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.9  2005/03/23 21:33:30  dechavez
 * use new "datatype" field in TS header instead of decoding bit mask on the fly
 *
 * Revision 1.8  2003/12/10 06:05:03  dechavez
 * various casts, includes, and the like to calm solaris cc
 *
 * Revision 1.7  2003/10/16 17:53:20  dechavez
 * restricted ida10 compression to int32 types only
 *
 * Revision 1.6  2003/03/14 00:42:50  dechavez
 * fixed bug when reverting to decompressed ida10 data when compression failed
 * to reduce size
 *
 * Revision 1.5  2003/03/13 23:13:24  dechavez
 * make ida10_compress fail when given a corrupt record
 *
 * Revision 1.4  2002/03/15 22:43:00  dec
 * changed IDA10_HEADLEN to more specific IDA10_TSHEADLEN
 *
 * Revision 1.3  2001/09/09 01:13:02  dec
 * allow for variable length IDA10 data buffer
 *
 * Revision 1.2  2000/02/08 23:27:03  dec
 * ifdef'd dummy SunOS version (lacks long long)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:43  dec
 * import existing IDA/NRTS sources
 *
 */
