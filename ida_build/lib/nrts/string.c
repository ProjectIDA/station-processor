#pragma ident "$Id: string.c,v 1.1 2009/02/03 22:25:52 dechavez Exp $"
/*======================================================================
 *
 *  NRTS related string functions
 *
 *====================================================================*/
#include "nrts.h"

char *nrtsPktString(NRTS_PKT *pkt, char *buf)
{
static char mt_unsafe[128];
static char *fid = "nrtsPktString";

    if (buf == NULL) buf = mt_unsafe;

    buf[0] = 0;
    switch (pkt->orig.type) {
      case ISI_TYPE_IDA5:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_IDA5 not supported", fid); break;
      case ISI_TYPE_IDA6:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_IDA6 not supported", fid); break;
      case ISI_TYPE_IDA7:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_IDA7 not supported", fid); break;
      case ISI_TYPE_IDA8:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_IDA8 not supported", fid); break;
      case ISI_TYPE_IDA9:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_IDA9 not supported", fid); break;
      case ISI_TYPE_QDPLUS:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_QDPLUS not supported", fid); break;
      case ISI_TYPE_MSEED:
        sprintf(buf+strlen(buf), "%s: ISI_TYPE_MSEED not supported", fid); break;
      case ISI_TYPE_IDA10:
        ida10TSHDRtoString(&pkt->work.buf.ida10.hdr, buf); break;
      default:
        sprintf(buf+strlen(buf), "%s: type '%d neither recognized nor supported", fid, pkt->orig.type); break;
    }

    return buf;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.1  2009/02/03 22:25:52  dechavez
 * initial release
 *
 */
