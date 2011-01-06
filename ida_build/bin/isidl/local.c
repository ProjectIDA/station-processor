#pragma ident "$Id: local.c,v 1.6 2007/05/03 20:27:16 dechavez Exp $"
/*======================================================================
 *
 *  Process locally acquired data.  Update headers as appropriate
 *  and write to disk loop.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_LOCAL

void ProcessLocalData(LOCALPKT *local)
{
BOOL ok;
static char *fid = "ProcessLocalData";

    local->raw.hdr.status = ISI_RAW_STATUS_OK;

    MUTEX_LOCK(&local->mutex);

    /* If appropriate, add sequence number and time stamps to the packet headers */

        switch (local->raw.hdr.desc.type) {
          case ISI_TYPE_IDA9:
            CompleteIda9Header(&local->raw, local->dl, local->rt593);
            break;
          case ISI_TYPE_IDA5:
          case ISI_TYPE_IDA6:
            CompleteIda5Header(&local->raw, local->dl);
            break;
          case ISI_TYPE_IDA10:
            CompleteIda10Header(&local->raw, local->dl);
            break;
#ifdef INCLUDE_Q330
          case ISI_TYPE_QDPLUS:
            CompleteQdplusHeader(&local->raw, local->dl);
            break;
#endif /* INCLUDE_Q330 */
        }

    /* Write to the disk loop */

        BlockShutdown(fid);
            ok = isidlWriteToDiskLoop(local->dl, &local->raw, ISI_OPTION_GENERATE_SEQNO);
        UnblockShutdown(fid);
    MUTEX_UNLOCK(&local->mutex);

    if (!ok) {
        LogMsg(LOG_INFO, "%s: isidlWriteToDiskLoop: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }
}

static BOOL InitLocalPacket(LOCALPKT *local, int len, int comp, int type, ISI_DL *dl)
{
static char *fid = "InitLocalPacket";

    local->dl = dl;

    MUTEX_INIT(&local->mutex);

    if (!isiInitRawPacket(&local->raw, NULL, len)) return FALSE;

    local->raw.hdr.len.used   = len;
    local->raw.hdr.len.native = len;
    local->raw.hdr.desc.comp  = comp;
    local->raw.hdr.desc.type  = type;
    local->raw.hdr.desc.order = ISI_ORDER_UNDEF;
    local->raw.hdr.desc.size  = sizeof(UINT8);
    strlcpy(local->raw.hdr.site, local->dl->sys->site, ISI_SITELEN+1);

    local->rt593 = FALSE;

    return TRUE;
}

BOOL InitLocalProcessor(ISIDL_PAR *par)
{
BAROMETER *bp;
Q330 *q330;
int i, len, comp, type;
BOOL rt593 = FALSE;
static char *fid = "InitLocalProcessor";

/* Not much to do here for LISS systems */

    if (par->source == SOURCE_LISS) {
        if (par->baro.count > 0) {
            fprintf(stderr, "barometer not supported for LISS feeds\n");
            return FALSE;
        }
        return TRUE;
    }

/* Check for RT593 support */

    if (par->flags & ISIDL_FLAG_PATCH_RT593) rt593 = TRUE;

/* Make sure all barometers have the same output format */

    if (par->baro.count > 0) {
        bp = (BAROMETER *) par->baro.array[0];
        type = bp->format.type;

        for (i = 1; i < par->baro.count; i++) {
            bp = (BAROMETER *) par->baro.array[i];
            if (bp->format.type != type) {
                fprintf(stderr, "%s: inconsistent multiple barometer specs\n");
                return FALSE;
            }
        }
    }

/* Make sure barometer format is consistent with primary format */

    switch (par->source) {
      case SOURCE_ARS:
        switch (par->ars.pktrev) {
          case 5: type = ISI_TYPE_IDA5; break;
          case 6: type = ISI_TYPE_IDA6; break;
          case 8: type = ISI_TYPE_IDA9; break; // Since we will convert IDA 8 to IDA 9
          default:
            fprintf(stderr, "%s: unsupported ARS pktrev '%d'\n", fid, par->ars.pktrev);
            return FALSE;
        }
        for (i = 0; i < par->baro.count; i++) {
            if (type != ISI_TYPE_IDA9) {
                fprintf(stderr, "%s: no barometer support for ARS pktrev '%d' \n", fid, par->ars.pktrev);
                return FALSE;
            }
            bp = (BAROMETER *) par->baro.array[i];
            if (bp->format.type != type) {
                fprintf(stderr, "%s: barometer spec not consistent with IDA9\n", fid);
                return FALSE;
            }
        }
        len  = IDA_BUFSIZ;
        comp = ISI_COMP_NONE;
        break;

      case SOURCE_UNDEFINED:
        for (i = 0; i < par->baro.count; i++) {
            bp = (BAROMETER *) par->baro.array[i];
            if (bp->format.type != ISI_TYPE_IDA9) {
                fprintf(stderr, "%s: barometer spec not consistent with IDA9\n", fid);
                return FALSE;
            }
        }
        len  = IDA_BUFSIZ;
        comp = ISI_COMP_NONE;
        type = ISI_TYPE_IDA9;
        break;

#ifdef INCLUDE_Q330
      case SOURCE_Q330:
        for (i = 0; i < par->baro.count; i++) {
            bp = (BAROMETER *) par->baro.array[i];
            if (bp->format.type != ISI_TYPE_QDPLUS) {
                fprintf(stderr, "%s: barometer spec not consistent with QDP\n", fid);
                return FALSE;
            }
        }
        len  = QDPLUS_PKTLEN;
        comp = ISI_COMP_NONE;
        type = ISI_TYPE_QDPLUS;
        break;
#endif /* INCLUDE_Q330 */

      default:
        fprintf(stderr, "%s: ERROR: unsupported source type (%d)\n", fid, par->source);
        return FALSE;
    }

/* initialize the raw packets */

    if (par->source == SOURCE_ARS) {
        if (!InitLocalPacket(&par->ars.local, len, comp, type, par->dl[0])) {
            fprintf(stderr, "%s: InitLocalPacket: %s", fid, strerror(errno));
            return FALSE;
        }
        if (type == ISI_TYPE_IDA9) par->ars.local.rt593 = rt593;
    }

    for (i = 0; i < par->baro.count; i++) {
        bp = (BAROMETER *) par->baro.array[i];
        if (!InitLocalPacket(&bp->local, len, comp, type, par->dl[0])) {
            fprintf(stderr, "%s: InitLocalPacket: %s", fid, strerror(errno));
            return FALSE;
        }
    }

    for (i = 0; i < par->q330.count; i++) {
        q330 = (Q330 *) par->q330.array[i];
        if (!InitLocalPacket(&q330->local, len, comp, type, par->dl[0])) {
            fprintf(stderr, "%s: InitLocalPacket: %s", fid, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: local.c,v $
 * Revision 1.6  2007/05/03 20:27:16  dechavez
 * added liss support
 *
 * Revision 1.5  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.4  2007/02/08 22:53:51  dechavez
 * LOG_ERR to LOG_INFO, use LOCALPKT handle
 *
 * Revision 1.3  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.2  2006/07/10 21:09:48  dechavez
 * complete headers for IDA5 and IDA6 packets
 *
 * Revision 1.1  2006/06/30 18:20:47  dechavez
 * initial release
 *
 */
