#pragma ident "$Id: intercept.c,v 1.1 2008/03/05 23:25:55 dechavez Exp $"
/*======================================================================
 *
 *  "Intercept" processing.  This is called from the ISI data source
 *  module after getting the raw packet from the remote source and
 *  before writing the raw packet to the disk loop.  Its purpose is to
 *  replicate any required processing normally done on directly 
 *  connected systems back at the source.  
 *
 *  Specifically, it is used to generate the QDP metadata directories
 *  that exist on station systems which are directly connected to the
 *  Q330 digitizers.  There, the meta data get saved to disk via libqdp
 *  callback as part of the Q330 handshake.  Here, we have to watch for
 *  the metadata to come passing by and save them explicitly.
 *
 *  We implement this here in a more general fashion only to allow for
 *  some unanticipated situation where will will want to do a similar
 *  customizations for other data types.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_INTERCEPT

static void InterceptQDPLUS(ISI_RAW_PACKET *raw, ISI_DL *dl, INTERCEPT_BUFFER *ib)
{
QDPLUS_PKT pkt;
QDP_TYPE_C1_MEM c1_mem;

    qdplusUnpackWrappedQDP(raw->payload, &pkt);
    switch (pkt.qdp.hdr.cmd) {
      case QDP_C1_MEM:
        qdpDecode_C1_MEM(pkt.qdp.payload, &c1_mem);
        ib->lcq.meta.state &= ~QDP_META_STATE_HAVE_TOKEN;
        qdpSaveMem(&ib->lcq.meta.raw.token, &c1_mem);
        if (ib->lcq.meta.raw.token.full) {
            qdpUnpackTokens(&ib->lcq.meta.raw.token, &ib->lcq.meta.token);
            ib->lcq.meta.state |= QDP_META_STATE_HAVE_TOKEN;
        }
        break;

      case QDP_C1_COMBO:
        memcpy(&ib->lcq.meta.raw.combo, pkt.qdp.payload, pkt.qdp.hdr.dlen);
        qdpDecode_C1_COMBO(ib->lcq.meta.raw.combo, &ib->lcq.meta.combo);
        ib->lcq.meta.state |= QDP_META_STATE_HAVE_COMBO;
        break;

      default:
        return;
    }

    if (ib->lcq.meta.state == QDP_META_STATE_COMPLETE) {
        isidlWriteQDPMetaData(dl, pkt.serialno, &ib->lcq.meta);
        ib->lcq.meta.state = QDP_META_STATE_HAVE_MN232;
    }
}

void InterceptProcessor(ISI_RAW_PACKET *raw, ISI_DL *dl, INTERCEPT_BUFFER *ib)
{
    switch (raw->hdr.desc.type) {
      case ISI_TYPE_QDPLUS:
        InterceptQDPLUS(raw, dl, ib);
        break;

      default:
        return;
    }
}

BOOL InitInterceptBuffer(INTERCEPT_BUFFER *ib)
{
#ifdef INCLUDE_Q330
    if (!qdpInitLCQ(&ib->lcq, NULL)) return FALSE;
    ib->lcq.meta.state = QDP_META_STATE_HAVE_MN232;
#endif /* INCLUDE_Q330 */
    return TRUE;
}

/* Revision History
 *
 * $Log: intercept.c,v $
 * Revision 1.1  2008/03/05 23:25:55  dechavez
 * initial release
 *
 */
