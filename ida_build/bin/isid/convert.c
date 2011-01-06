#pragma ident "$Id: convert.c,v 1.16 2009/01/06 20:49:12 dechavez Exp $"
/*======================================================================
 *
 *  Convert a NRTS_PKT into an IACP_FRAME
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_CONVERT

#define UNSUPPORTED_COMPRESSION 0xffffffff

static void ApplyFFU(NRTS_PKT *pkt, INT32 *src)
{
int i;
long *ldata;

    if (pkt->work.buf.ida.ffu_shift > 0) {
        ldata = (long *) (src);
        for (i = 0; i < pkt->isi.hdr.nsamp ; i++) ldata[i] >>= pkt->work.buf.ida.ffu_shift; 
     }
}

static UINT32 IdaCompressData(UINT8 *dest, NRTS_PKT *pkt)
{
UINT32 newlen;
INT32 *src, *tmpbuf = NULL;
static char *fid = "IdaCompressData";

/* We can only handle uncompressed INT32's here */

    if (pkt->isi.hdr.desc.comp != ISI_COMP_NONE) return UNSUPPORTED_COMPRESSION;
    if (pkt->isi.hdr.desc.type != ISI_TYPE_INT32)  return UNSUPPORTED_COMPRESSION;

/* Make sure the data to be compressed are in this host's byte order */

    if (pkt->orig.order != NATIVE_BYTE_ORDER) {
        if ((tmpbuf = (INT32 *) malloc(pkt->isi.hdr.nsamp * sizeof(INT32))) == NULL) {
            LogMsg(LOG_WARN, "%s: malloc: %s", fid, strerror(errno));
            return UNSUPPORTED_COMPRESSION;
        }
        memcpy(tmpbuf, pkt->isi.data, pkt->isi.hdr.nsamp * sizeof(INT32));
        utilSwapUINT32((UINT32 *) tmpbuf, pkt->isi.hdr.nsamp);
        src = tmpbuf;
    } else {
        src = (INT32 *) pkt->isi.data;
    }

/* Silently apply Fels' shifts to IDA5 and 6 data */

    if (pkt->orig.type == ISI_TYPE_IDA5 || pkt->orig.type == ISI_TYPE_IDA6) ApplyFFU(pkt, src);

/* Compress */

    newlen = utilIdaCompressINT32(dest, src, pkt->isi.hdr.nsamp);
    if (tmpbuf != NULL) free(tmpbuf);

    return newlen;
}

static BOOL BuildNativeUncompressed(NRTS_PKT *pkt, IACP_FRAME *frame, UINT8 *buf)
{
UINT8 *ptr;
static char *fid = "BuildNativeUncompressed";

    if (pkt == NULL || frame == NULL || buf == NULL) {
        LogMsg(LOG_ERR, "%s: NULL inputs!", fid);
        errno = EINVAL;
        return FALSE;
    }

/* Copy over raw packet */

    ptr = buf;
    ptr += utilPackBytes(ptr, pkt->orig.data, pkt->orig.len);
    frame->payload.len = (UINT32) (ptr - frame->payload.data);

/* Update the generic header */

    pkt->isi.hdr.nbytes    = pkt->orig.len;
    pkt->isi.hdr.desc.type = pkt->orig.type;
    isiPackGenericTSHDR(frame->payload.data, &pkt->isi.hdr);

    return TRUE;
}

static BOOL BuildNative(UINT32 compress, NRTS_PKT *pkt, IACP_FRAME *frame, UINT8 *buf)
{
UINT8 *ptr;
UINT32 cmplen;
static char *fid = "BuildNative";

/* We will only IDA compress, regardless what they ask */

    switch (compress) {
      case ISI_COMP_NONE:
        return BuildNativeUncompressed(pkt, frame, buf);

      default:
        compress = ISI_COMP_IDA;
    }

/* Load in generic header */

    ptr = frame->payload.data = buf;
    ptr += isiPackGenericTSHDR(ptr, &pkt->isi.hdr);

/* Copy over the raw packet header */

    ptr += utilPackBytes(ptr, pkt->orig.hdr.data, pkt->orig.hdr.len);

/* Compress the data */

    cmplen = IdaCompressData(ptr, pkt);
    if (cmplen >= pkt->orig.dat.len || cmplen == UNSUPPORTED_COMPRESSION) {
        return BuildNativeUncompressed(pkt, frame, buf);
    }
    ptr += cmplen;
    
    frame->payload.len  = (UINT32) (ptr - frame->payload.data);

/* Update the generic header */

    pkt->isi.hdr.nbytes    = cmplen + pkt->orig.hdr.len;
    pkt->isi.hdr.desc.comp = compress;
    pkt->isi.hdr.desc.type = pkt->orig.type;
    isiPackGenericTSHDR(frame->payload.data, &pkt->isi.hdr);

    return TRUE;
}

static BOOL BuildGenericUncompressed(NRTS_PKT *pkt, IACP_FRAME *frame, UINT8 *buf)
{
UINT8 *ptr;
static char *fid = "BuildGenericUncompressed";

    if (pkt == NULL || frame == NULL || buf == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    ptr = buf;
    ptr += utilPackBytes(ptr, pkt->isi.data, pkt->isi.hdr.nbytes);
    frame->payload.len = (UINT32) (ptr - frame->payload.data);

    return TRUE;

}

static BOOL BuildGeneric(UINT32 compress, NRTS_PKT *pkt, IACP_FRAME *frame, UINT8 *ptr)
{
UINT32 cmplen;
static char *fid = "BuildGeneric";

/* We will only IDA compress, regardless what they ask */

    switch (compress) {
      case ISI_COMP_NONE:
        return BuildGenericUncompressed(pkt, frame, ptr);

      default:
        compress = ISI_COMP_IDA;
    }

/* (attempt to) compress the data */

    cmplen = IdaCompressData(ptr, pkt);
    if (cmplen >= pkt->orig.dat.len || cmplen == UNSUPPORTED_COMPRESSION) {
        return BuildGenericUncompressed(pkt, frame, ptr);
    }
    ptr += cmplen;
    frame->payload.len = (UINT32) (ptr - frame->payload.data);

/* Update the generic header */

    pkt->isi.hdr.desc.comp = compress;
    pkt->isi.hdr.nbytes    = cmplen;
    isiPackGenericTSHDR(frame->payload.data, &pkt->isi.hdr);

    return TRUE;
}

BOOL ConvertData(CLIENT *client, ISID_DATA_REQUEST *req, NRTS_PKT *pkt)
{
UINT8 *ptr;
IACP_FRAME *frame;
static char *fid = "ConvertData";

    if (client == NULL || req == NULL || pkt == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* All converters start with the data frame already having the
 * generic header in place.
 */

    frame = &client->send.frame;
    frame->payload.type = ISI_IACP_GENERIC_TS;
    ptr = frame->payload.data = client->temp.buf;
    ptr += isiPackGenericTSHDR(ptr, &pkt->isi.hdr);

    if (req->format == ISI_FORMAT_GENERIC) {
        return BuildGeneric(req->compress, pkt, frame, ptr);
    } else {
        return BuildNative(req->compress, pkt, frame, ptr);
    }
}

/* Revision History
 *
 * $Log: convert.c,v $
 * Revision 1.16  2009/01/06 20:49:12  dechavez
 * Fixed unitialized pointer problem in BuildNative()
 *
 * Revision 1.15  2008/08/05 20:07:07  dechavez
 * support generic packets of non-INT32 data types (rather than have compression
 * fail, just fall through to uncompressed)
 *
 * Revision 1.14  2007/10/16 20:19:39  dechavez
 * Fixed bug where all data would be sent with IDA compression, even if the
 * client explicity requested uncompressed, cleaned up logic flow in the process.
 *
 * Revision 1.13  2006/07/10 21:11:01  dechavez
 * shift IDA6 as well as IDA5
 *
 * Revision 1.12  2006/06/19 18:16:19  dechavez
 * Silently apply Fels' shifts to all IDA rev 5 packets before serving them.
 *
 * Revision 1.11  2006/02/10 02:20:49  dechavez
 * oldchn -> chnloc
 *
 * Revision 1.10  2005/10/10 23:33:22  dechavez
 * cleaned up confusion with byte order macros (indirectly leading to fixing
 * of ida10 compression bug on little-endian servers
 *
 * Revision 1.9  2005/07/06 15:53:15  dechavez
 * use only IDA compression, no matter what was requested
 *
 * Revision 1.8  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.7  2005/02/09 17:02:59  dechavez
 * Fixed x86 compression error (again!)
 *
 * Revision 1.6  2004/09/29 18:23:43  dechavez
 * further stabs at trying to get the byte order thing correct for x86 builds
 *
 * Revision 1.5  2004/06/21 19:58:13  dechavez
 * Fixed uninitialzed pointer problem in BuildGenericUncompressed()
 *
 * Revision 1.4  2003/12/17 16:55:40  dechavez
 * fixed bug determining original packet type in IdaCompressData
 *
 * Revision 1.3  2003/12/10 06:18:19  dechavez
 * restored byte-swap native data on little endian servers
 *
 * Revision 1.2  2003/12/04 23:19:45  dechavez
 * Don't byte-swap ida data on little endian servers, include byte count
 * in failed compression log messages
 *
 * Revision 1.1  2003/10/16 18:07:23  dechavez
 * initial release
 *
 */
