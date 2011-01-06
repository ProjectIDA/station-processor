#pragma ident "$Id: ida10.c,v 1.25 2009/02/03 22:28:07 dechavez Exp $"
/*======================================================================
 *
 *  IDA version 10 format packet support
 *
 *====================================================================*/
#include "nrts/dl.h"

#define LaxTimeOption(dl) (dl->flags & NRTS_DL_FLAGS_LAX_TIME)
#define StationSystem(dl) (dl->flags & NRTS_DL_FLAGS_STATION_SYSTEM)
#define WantRealTimeTest(dl) (dl->flags & NRTS_DL_FLAGS_STRICT_TIME)
#define SIGN(sign) (sign > 0 ? '+' : '-')
#define FIRST(dl) ((dl->sys->nread == 0) ? TRUE : FALSE)

static int RejectPacket(NRTS_DL *dl, IDA10_TSHDR *crnt, char *cause)
{
char msgbuf[1024];

    logioMsg(dl->lp, dl->debug->pkt, "%s NRTS REJECT: %s", cause, ida10TSHDRtoString(crnt, msgbuf));
    return NRTS_REJECT;
}

/* Check for time tears between two successive packets of the same stream */

static BOOL TimeTearTest(NRTS_DL *dl, IDA10_TSHDR *crnt, IDA10_TSHDR *prev, char *ident)
{
UINT64 err;
int status, sign;
REAL64 errsec, errsmp;
char msgbuf[1024];

    if ((status = ida10TtagIncrErr(prev, crnt, &sign, &err)) != 0) {
        logioMsg(dl->lp, LOG_WARN, "*** UNEXPECTED ida10TtagIncrErr error %d ***", status);
        return FALSE;
    }
    if (err == 0) return FALSE;

    errsec = (REAL64) err / (REAL64) NANOSEC_PER_SEC;
    errsmp = errsec / (REAL64) crnt->sint;
    if ((unsigned long) errsmp > dl->tol.samp) {
        logioMsg(dl->lp, dl->debug->ttag, "%c%.5lf sec %s time tear (%.2lf samp)", SIGN(sign), errsec, ident, errsmp);
        logioMsg(dl->lp, dl->debug->ttag, "    prev: %s", ida10TSHDRtoString(prev, msgbuf));
        logioMsg(dl->lp, dl->debug->ttag, "    crnt: %s", ida10TSHDRtoString(crnt, msgbuf));
        return TRUE;
    }

    return FALSE;
}

/* Test to see if the data times are close to "real" time */

static BOOL RealTimeTest(NRTS_DL *dl, IDA10_TSHDR *crnt, char *ident)
{
long now, diff;
char tmp1[64], tmp2[64];

    now = (long) time(NULL);
    diff = now > (long) crnt->tols ? now - (long) crnt->tols : (long) crnt->tols - now;
    if (diff > (long) dl->tol.clock) {
        utilLttostr(now, 0, tmp1);
        utilDttostr(crnt->tols, 0, tmp2);
        //logioMsg(dl->lp, dl->debug->ttag, "SUSPECT TIME STAMP: host=%s %s=%s diff=%ld sec", tmp1, ident, tmp2, diff);
        return FALSE;
    }

    return TRUE;
}

static BOOL IsDuplicate(NRTS_DL *dl, IDA10_TSHDR *crnt, IDA10_TSHDR *prev, char *ident)
{
char tmp1[64];

    if (memcmp(&prev->cmn.ttag.beg, &crnt->cmn.ttag.beg, sizeof(IDA10_TTAG)) == 0 &&
        memcmp(&prev->cmn.ttag.end, &crnt->cmn.ttag.end, sizeof(IDA10_TTAG)) == 0
    ) {
        utilDttostr(crnt->tofs, 0, tmp1);
        logioMsg(dl->lp, dl->debug->ttag, "duplicate %s %s start and end times", ident, tmp1);
        return TRUE;
    }

    return FALSE;
}

static BOOL IsCompleteOverlap(NRTS_DL *dl, IDA10_TSHDR *crnt, IDA10_TSHDR *prev, char *ident)
{
char tmp1[64], tmp2[64];

    if (crnt->tols <= prev->tols) {
        utilDttostr(crnt->tols, 0, tmp1);
        utilDttostr(prev->tols, 0, tmp2);
        logioMsg(dl->lp, dl->debug->pkt, "%s tols %s completely overlaps NRTS tols %s", ident, tmp1, tmp2);
        return TRUE;
    }

    return FALSE;
}

static BOOL IsPartialOverlap(NRTS_DL *dl, IDA10_TSHDR *crnt, IDA10_TSHDR *prev, char *ident)
{
char tmp1[64], tmp2[64];

    if (crnt->tofs <= prev->tols && crnt->tols > prev->tols) {
        utilDttostr(crnt->tofs, 0, tmp1);
        utilDttostr(prev->tols, 0, tmp2);
        logioMsg(dl->lp, dl->debug->pkt, "%s tofs %s partially overlaps NRTS tols %s", ident, tmp1, tmp2);
        return TRUE;
    }

    return FALSE;
}

static int CheckPacket(NRTS_DL *dl, NRTS_PKT *pkt)
{
int i;
NRTS_STA *sta;
NRTS_CHN *chn;
IDA10_TSHDR *crnt, *prev;
static char *fid = "CheckPacket";

/* Nothing to do if the disk loop is not configured to store this stream */

    sta = &dl->sys->sta[0];
    for (pkt->chndx = -1, i = 0; pkt->chndx < 0 && i < sta->nchn; i++) {
        if (strcmp(sta->chn[i].name, pkt->isi.hdr.name.chnloc) == 0) pkt->chndx = i;
    }
    if (pkt->chndx < 0) return NRTS_IGNORE;

    chn = &sta->chn[i];

/* Pointers to this and previous header for this particular stream */

    crnt = &pkt->work.buf.ida10.hdr;
    if (dl->save[pkt->chndx] != NULL) {
        prev = &((IDA10_TS *) dl->save[pkt->chndx])->hdr; /* previous header for this stream */
    } else {
        prev = NULL; /* no previous packets for this stream have been seen */
    }

/* Station systems need to be a bit more strict than down stream systems */

    if (StationSystem(dl)) {

    /* Reject obviously corrupt packets immediately */

        if (crnt->unused != 0) return RejectPacket(dl, crnt, "PacketLength");

    /* If this is the start of an acquistion session, then refuse to start taking
     * data until we get one with a clearly reasonable time stamp.
     */

        if (FIRST(dl) && WantRealTimeTest(dl) && !RealTimeTest(dl, crnt, pkt->ident)) return RejectPacket(dl, crnt, "RealTimeTest");
    }

/* Note time discontinuities */

    if (prev != NULL) {
        pkt->tear = TimeTearTest(dl, crnt, prev, pkt->ident);
    } else {
        pkt->tear = TRUE;
    }

/* Check external time quality only if there is a time tear */

    if (pkt->tear) {
        if (!crnt->cmn.ttag.beg.status.init) return RejectPacket(dl, crnt, "TimeStringNotInit");
        if (!LaxTimeOption(dl)) {
            if (!crnt->cmn.ttag.beg.status.avail)   return RejectPacket(dl, crnt, "TimeStringUnavail");
            if ( crnt->cmn.ttag.beg.status.suspect) return RejectPacket(dl, crnt, "TimeStringSuspect");
        }
    }

/* Check for gaps or overlaps only if there is a time tear */

    if (pkt->tear && prev != NULL) {
        if (IsDuplicate(dl, crnt, prev, pkt->ident)) return NRTS_DUPLICATE;
        if (IsCompleteOverlap(dl, crnt, prev, pkt->ident)) return NRTS_COMPLETE_OVERLAP;
        if (IsPartialOverlap(dl, crnt, prev, pkt->ident)) return NRTS_PARTIAL_OVERLAP;
    }

/* Must be OK */

    return NRTS_ACCEPT;
}

static UINT16 BuildIsiTstampStatus(IDA10_CLOCK_STATUS *status)
{
UINT16 result;

    result = 0;
    if (status->locked) result |= ISI_TSTAMP_STATUS_LOCKED;
    if (status->suspect) result |= ISI_TSTAMP_STATUS_SUSPECT;
    if (!status->avail) result |= ISI_TSTAMP_STATUS_AUTOINC;
    if (!status->init) result |= ISI_TSTAMP_STATUS_NOINIT;

    return result;
}

BOOL nrtsIda10Decoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out)
{
NRTS_DL *dl;
IDA10_TS *ts;
static char *fid = "nrtsIda10Decoder";

    if (arg == NULL || in == NULL || out == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    dl = (NRTS_DL *) arg;

    out->chndx = -1;
    if (ida10Type((UINT8 *)in) != IDA10_TYPE_TS) {
        logioMsg(dl->lp, dl->debug->pkt, "%s: ignore non-TS packet", fid);
        out->status = NRTS_NOTDATA;
        return TRUE;
    }

    ts = &out->work.buf.ida10;
    out->work.len = sizeof(IDA10_TSHDR);

    if (!ida10UnpackTS(in, ts)) {
        logioMsg(dl->lp, LOG_ERR, "%s: ida10UnpackTS failed: %s", fid, strerror(errno));
        out->status = NRTS_REJECT;
        return FALSE;
    }

    out->orig.type = ISI_TYPE_IDA10;
    out->orig.order = NATIVE_BYTE_ORDER; /* since that is what ida10UnpackTS does */

    isiInitGenericTSHDR(&out->isi.hdr);
    if (stream != NULL) {
        isiStaChnToStreamName(stream->sta->name, stream->chn->name, &out->isi.hdr.name);
    } else {
        strlcpy(out->isi.hdr.name.sta, dl->sys->sta[0].name, ISI_STALEN+1);
        strlcpy(out->isi.hdr.name.chnloc, ts->hdr.cname, ISI_CHNLOCLEN+1);
    }
    isiStreamNameString(&out->isi.hdr.name, out->ident);
    isiSintToSrate((REAL64) ts->hdr.sint, &out->isi.hdr.srate);

    out->isi.hdr.desc.order = NATIVE_BYTE_ORDER;
    switch (ts->hdr.dl.format & IDA10_MASK_COMP) {
      case IDA10_COMP_NONE:
        out->isi.hdr.desc.comp = ISI_COMP_NONE;
        break;
      case IDA10_COMP_IGPP:
        out->isi.hdr.desc.comp = ISI_COMP_IDA;
        break;
      case IDA10_COMP_STEIM1:
        out->isi.hdr.desc.comp = ISI_COMP_STEIM1;
        break;
      case IDA10_COMP_STEIM2:
        out->isi.hdr.desc.comp = ISI_COMP_STEIM2;
        break;
      default:
        out->isi.hdr.desc.comp = ISI_COMP_UNDEF;
        logioMsg(dl->lp, LOG_ERR, "%s: unknown IDA10 comp code: %d", fid, ts->hdr.dl.format & IDA10_MASK_COMP);
        errno = UNSUPPORTED;
        return FALSE;
    }

    switch (ts->hdr.datatype) {
      case IDA10_DATA_INT32:
        out->isi.hdr.desc.type = ISI_TYPE_INT32;
        out->isi.hdr.desc.size = sizeof(INT32);
        break;
      case IDA10_DATA_INT16:
        out->isi.hdr.desc.type = ISI_TYPE_INT16;
        out->isi.hdr.desc.size = sizeof(INT16);
        break;
      case IDA10_DATA_INT8:
        out->isi.hdr.desc.type = ISI_TYPE_INT8;
        out->isi.hdr.desc.size = sizeof(INT8);
        break;
      case IDA10_DATA_REAL32:
        out->isi.hdr.desc.type = ISI_TYPE_REAL32;
        out->isi.hdr.desc.size = sizeof(REAL32);
        break;
      case IDA10_DATA_REAL64:
        out->isi.hdr.desc.type = ISI_TYPE_REAL64;
        out->isi.hdr.desc.size = sizeof(REAL64);
        break;
      default:
        out->isi.hdr.desc.type = ISI_TYPE_UNDEF;
        logioMsg(dl->lp, LOG_ERR, "%s: unknown IDA10 datatype: %d", fid, ts->hdr.datatype);
        errno = UNSUPPORTED;
        out->status = NRTS_REJECT;
        return FALSE;
    }

    out->isi.hdr.tofs.value = ts->hdr.tofs;
    out->isi.hdr.tofs.status  = BuildIsiTstampStatus(&ts->hdr.cmn.ttag.beg.status);

    out->isi.hdr.tols.value = ts->hdr.tols;
    out->isi.hdr.tols.status  = BuildIsiTstampStatus(&ts->hdr.cmn.ttag.beg.status);

    out->isi.hdr.nsamp = (UINT32) ts->hdr.nsamp;

    out->isi.hdr.nbytes = ts->hdr.nbytes;
    out->isi.data = ts->data.int8;

    out->extra = ts->hdr.cmn.extra;

 /* Read only systems take this packet as is, writers are more cautious */

    out->status = (dl->perm == ISI_RDONLY) ? NRTS_ACCEPT : CheckPacket(dl, out);

    return TRUE;
}

/* Revision History
 *
 * $Log: ida10.c,v $
 * Revision 1.25  2009/02/03 22:28:07  dechavez
 * improved logging of rejected packets, cleaned up logic flow a bit
 *
 * Revision 1.24  2008/10/10 21:40:54  dechavez
 * don't log those "no chndx" messages anymore (scares the natives)
 *
 * Revision 1.23  2007/09/14 19:29:56  dechavez
 * distinguish between complete and partial overlaps, improved debug messages
 *
 * Revision 1.22  2007/06/23 15:52:26  dechavez
 * ignore avail and suspicious bits when NRTS_DL_FLAGS_LAX_TIME set
 *
 * Revision 1.21  2007/06/21 22:45:17  dechavez
 * added status->avail check to list of conditions that define an unreliable time stamp
 *
 * Revision 1.20  2007/06/01 19:20:55  dechavez
 * added IDA_EXTRA to NRTS_PKT, saved same
 *
 * Revision 1.19  2007/03/26 21:32:28  dechavez
 * Renamed NRTS_STATION_SYSTEM NRTS_STRICT_TIME constants to NRTS_DL_FLAGS_x
 *
 * Revision 1.18  2007/02/01 01:52:03  dechavez
 * only check for gaps and overlaps in the case of system time tears (eliminates gaps due to erratic GPS)
 *
 * Revision 1.17  2007/01/23 02:43:57  dechavez
 * 4.3.2
 *
 * Revision 1.16  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.15  2006/11/10 06:38:33  dechavez
 * generic time stamp/status, support for REAL64
 *
 * Revision 1.14  2006/08/15 01:16:47  dechavez
 * removed premature 64 bit data type support
 *
 * Revision 1.13  2006/02/09 19:44:04  dechavez
 * ISI_STREAM_NAME oldchn -> chnlock
 *
 * Revision 1.12  2005/12/09 21:22:49  dechavez
 * return NRTS_NOTDATA for non-data packets (instead of "failing")
 *
 * Revision 1.11  2005/11/03 23:56:13  dechavez
 * explicit casts to suppress compiler warnings
 *
 * Revision 1.10  2005/10/10 23:45:17  dechavez
 * fixed error tagging byte order for little-endian systems
 *
 * Revision 1.9  2005/10/06 21:58:56  dechavez
 * replaced uninitialized errtol in TimeTearTest() with tol.samp from handle
 *
 * Revision 1.8  2005/09/13 01:17:06  dechavez
 * added missing errsmp to arg list for warning message in TimeTearTest()
 *
 * Revision 1.7  2005/07/27 18:30:45  dechavez
 * set severity of time tear and suspect time messages to LOG_INFO
 *
 * Revision 1.6  2005/07/26 17:11:48  dechavez
 * removed leftover debug code
 *
 * Revision 1.5  2005/07/26 00:00:35  dechavez
 * added strict checks for NRTS_DL output
 *
 */
