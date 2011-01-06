#pragma ident "$Id: ida.c,v 1.26 2008/10/10 21:40:54 dechavez Exp $"
/*====================================================================
 *
 *  IDA rev 5 through 9 packet decoder
 *
 *====================================================================*/
#include "nrts/dl.h"

#ifdef LOCAL_DEBUG_ENABLED
static char *hdrstring(IDA_DHDR *crnt)
{
static char buf[1024];

    sprintf(buf, " %2d",  crnt->wrdsiz);
    sprintf(buf+strlen(buf), " %4d",  crnt->nsamp);
    sprintf(buf+strlen(buf), " %7.3f",crnt->sint);
    sprintf(buf+strlen(buf), " %s",   sys_timstr(crnt->beg.sys, crnt->beg.mult));
    sprintf(buf+strlen(buf), "  %d",  crnt->beg.qual);
    sprintf(buf+strlen(buf), " %s",   tru_timstr(&crnt->beg));
    sprintf(buf+strlen(buf), " %s",   sys_timstr(crnt->end.sys, crnt->end.mult));
    sprintf(buf+strlen(buf), "  %d",  crnt->end.qual);
    sprintf(buf+strlen(buf), " %s",   tru_timstr(&crnt->end));
    if (crnt->extra.valid) sprintf(buf+strlen(buf), " 0x%08x", crnt->extra.seqno);
    return buf;
}
#endif /* LOCAL_DEBUG_ENABLED */

#define IDA5678_HEADER_LENGTH 60
#define IDA9_HEADER_LENGTH   64
#define StationSystem(dl) (dl->flags & NRTS_DL_FLAGS_STATION_SYSTEM)
#define WantRealTimeTest(dl) (dl->flags & NRTS_DL_FLAGS_STRICT_TIME)
#define SIGN(sign) (sign < 0 ? '-' : '+')
#define FIRST(dl) ((dl->sys->nread == 0) ? TRUE : FALSE)

/* offsets to various fields in the raw packets */

#define YEAR_OFF     6
#define BEGEXT_OFF  14
#define BEGHZ_OFF   18
#define BEGQUAL_OFF 22
#define ENDSYS_OFF  42
#define ENDEXT_OFF  46
#define ENDHZ_OFF   50
#define ENDQUAL_OFF 54

#define LockedClock(tag) ((tag)->qual == 1 || (tag)->code == NRTS_AUTOINC)

static BOOL IsDuplicate(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, char *ident)
{
char tmp1[64];

    if (prev != NULL && prev->beg.sys == crnt->beg.sys && prev->end.sys == crnt->end.sys) {
        utilDttostr(crnt->beg.tru, 0, tmp1);
        logioMsg(dl->lp, dl->debug->pkt, "duplicate %s %s start and end times", ident, tmp1);
        return TRUE;
    }
    return FALSE;
}

static BOOL IsCompleteOverlap(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, char *ident)
{
char tmp1[64], tmp2[64];

    if (prev != NULL && (crnt->end.tru <= prev->end.tru)) {
        utilDttostr(crnt->end.tru, 0, tmp1);
        utilDttostr(prev->end.tru, 0, tmp2);
        logioMsg(dl->lp, dl->debug->pkt, "%s tols %s completely overlaps NRTS tols %s", ident, tmp1, tmp2);
        return TRUE;
    }

    return FALSE;
}

static BOOL IsPartialOverlap(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, char *ident)
{
char tmp1[64], tmp2[64];

    if (prev != NULL && (crnt->beg.tru <= prev->end.tru) && (crnt->end.tru > prev->end.tru)) {
        utilDttostr(crnt->beg.tru, 0, tmp1);
        utilDttostr(prev->end.tru, 0, tmp2);
        logioMsg(dl->lp, dl->debug->pkt, "%s tofs %s partially overlaps NRTS tols %s", ident, tmp1, tmp2);
        return TRUE;
    }

    return FALSE;
}

/* Check for time tears between two successive packets of the same stream */

static BOOL TimeTearTest(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, char *ident)
{
float errsec, errsmp;
int sign, status;
unsigned long errtic;

    if (prev == NULL) return FALSE;

    if ((status = ida_timerr(crnt, prev, &sign, &errtic)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "*** UNEXPECTED ida_timerr status %d ***", status);
        return FALSE;
    }
    if (errtic <= dl->tol.tic) return FALSE;

    errsec  = (float) ((errtic * crnt->beg.mult) / 1000.0);
    errsmp  = errsec / crnt->sint;
    logioMsg(dl->lp, LOG_INFO, "%c%.3f sec %s time tear (%.2f samp)", SIGN(sign), errsec, ident, errsmp);

    return TRUE;
}

/* Patch original data to correct for bogus external time strings */

static BOOL PatchTimeStamp(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, UINT8 *raw)
{
int error;
UINT8 *ptr;
static char *fid = "PatchTimeStamp";

/* Force beg, end, ext and hz times to match that from the previous record.
 * We use the end time of the previous record to avoid introducing apparent
 * backward time jumps.
 */
    ptr = raw + YEAR_OFF;
    utilPackINT16(ptr, (INT16) prev->end.year);

    ptr = raw + BEGEXT_OFF;
    utilPackINT32(ptr, (INT32) prev->end.ext);

    ptr = raw + BEGHZ_OFF;
    utilPackINT32(ptr, (INT32) prev->end.hz);

    raw[BEGQUAL_OFF] = NRTS_AUTOINC;

    ptr = raw + ENDEXT_OFF;
    utilPackINT32(ptr, (INT32) prev->end.ext);

    ptr = raw + ENDHZ_OFF;
    utilPackINT32(ptr, (INT32) prev->end.hz);

    raw[ENDQUAL_OFF] = NRTS_AUTOINC;

/* Reload the header to reflect the modified buffer contents */

    if ((error = ida_dhead(dl->ida, crnt, raw)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ida_dhead error %d", fid, error);
        return FALSE;
    }

    return TRUE;
}

/* Look for common problems getting reliable external time */

static BOOL SuspectExternalTime(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, UINT8 *raw, NRTS_PKT *pkt)
{
BOOL suspicious = FALSE, TimeStampIsTrustworthy = FALSE, TimeStampIsSuspect = TRUE;
char tmp1[64], tmp2[64];
struct {long beg; long end;} diff;
struct {double prev; double crnt;} offset;

/* If we already have a (previously validated) packet, then we will
 * compare the new packet to it.  If the offsets match then we know
 * this packet is OK.
 */

    if (prev != NULL) {

    /* If offsets match then we can be certain this packet is OK */

        offset.prev = ida_ext1hzoff(&prev->beg);
        offset.crnt = ida_ext1hzoff(&crnt->beg);
        diff.beg = (long) ((offset.crnt - offset.prev) * 1000.0); /* diff in msec */

        offset.prev = ida_ext1hzoff(&prev->end);
        offset.crnt = ida_ext1hzoff(&crnt->end);
        diff.end = (long) ((offset.crnt - offset.prev) * 1000.0); /* diff in msec */

        if (diff.beg == 0 && diff.end == 0) return TimeStampIsTrustworthy;

    /* otherwise, make sure the clock didn't jump backwards */

        if (crnt->beg.ext < prev->beg.ext) {
            utilLttostr(crnt->beg.ext, 0, tmp1);
            utilLttostr(crnt->end.ext, 0, tmp2);
            logioMsg(dl->lp, dl->debug->pkt, "%s ext time jumped backwards from %s to %s", pkt->ident, tmp1, tmp2);
            suspicious = TRUE;
        }
    }

    /* and believe the quality codes */

    if (crnt->beg.error && crnt->beg.error != TTAG_LEAP_YEAR_PATCH) {
        logioMsg(dl->lp, dl->debug->pkt, "%s beg stamp error flag: 0x%02x", pkt->ident,  crnt->beg.error);
        suspicious = TRUE;
    } else if (crnt->end.error && crnt->end.error != TTAG_LEAP_YEAR_PATCH) {
        logioMsg(dl->lp, dl->debug->pkt, "%s end stamp error flag: 0x%02x", pkt->ident,  crnt->end.error);
        suspicious = TRUE;
    } else if (!LockedClock(&crnt->beg)) {
        logioMsg(dl->lp, dl->debug->pkt, "%s beg ttag unlocked '%c' (%d)", pkt->ident,  crnt->beg.code, crnt->beg.qual);
        suspicious = TRUE;
    } else if (!LockedClock(&crnt->end)) {
        logioMsg(dl->lp, dl->debug->pkt, "%s end ttag unlocked '%c' (%d)", pkt->ident,  crnt->end.code, crnt->end.qual);
        suspicious = TRUE;
    }

    if (!suspicious) return TimeStampIsTrustworthy;

/* Packet is suspect at this point.
 * If we have a time tear or if this is a virgin disk loop then we are stuck with it.
 */

    if (prev == NULL || pkt->tear) return TimeStampIsSuspect;

/* Otherwise we can use the exernal and 1Hz time from the previous packet to
 * patch this one so that it will unpack OK
 */

    if (!PatchTimeStamp(dl, crnt, prev, raw)) {
        logioMsg(dl->lp, LOG_ERR, "!!!ERROR!!! PatchTimeStamp corrupted data !!!ERROR!!!");
        return TimeStampIsSuspect;
    }

    return TimeStampIsTrustworthy;
}

/* Test to see if the data times are close to "real" time */

static BOOL RealTimeTest(NRTS_DL *dl, IDA_DHDR *crnt, char *ident)
{
long now, diff;
char tmp1[64], tmp2[64];

    now = (long) time(NULL);
    diff = now > (long) crnt->end.tru ? now - (long) crnt->end.tru : (long) crnt->end.tru - now;
    if (diff > (long) dl->tol.clock) {
        utilLttostr(now, 0, tmp1);
        utilDttostr(crnt->end.tru, 0, tmp2);
        logioMsg(dl->lp, LOG_INFO, "SUSPECT TIME STAMP: host=%s %s=%s diff=%ld sec", tmp1, ident, tmp2, diff);
        return FALSE;
    }

    return TRUE;
}

static BOOL HaveGrossErrors(NRTS_DL *dl, IDA_DHDR *crnt, char *ident)
{
int status, sign;
unsigned long ticerr;

    if (!ida_sampok(crnt)) {
       logioMsg(dl->lp, dl->debug->pkt, "%s illegal number of samples = %d", ident, crnt->nsamp);
       return TRUE;
    }

    if (crnt->wrdsiz != 2 && crnt->wrdsiz != 4) {
        logioMsg(dl->lp, dl->debug->pkt, "%s unexpected wrdsiz = %d", ident, crnt->wrdsiz);
        return TRUE;
    }

    if ((crnt->beg.error & TTAG_BAD_OFFSET) || (crnt->end.error & TTAG_BAD_OFFSET)) {
        logioMsg(dl->lp, dl->debug->pkt, "%s bad offset (hz time > sys time)", ident);
        return TRUE;
    }

    if ((crnt->beg.error & TTAG_BAD_SYSTIM) || (crnt->end.error & TTAG_BAD_SYSTIM)) {
        logioMsg(dl->lp, dl->debug->pkt, "%s bad systim (beg >= end)", ident);
        return TRUE;
    }

    if (crnt->sint <= 0.0) {
        logioMsg(dl->lp, dl->debug->pkt, "%s bad sample interval %.3f", ident, crnt->sint);
        return TRUE;
    }

    if (crnt->beg.sys >= crnt->end.sys) {
        logioMsg(dl->lp, dl->debug->pkt, "%s beg.sys >= end.sys", ident);
        return TRUE;
    }

    if (crnt->beg.hz > crnt->end.hz) { /* equal is OK */
        logioMsg(dl->lp, dl->debug->pkt, "%s beg.hz > end.hz", ident);
        return TRUE;
    }

    if ((status = ida_timerr(crnt, NULL, &sign, &ticerr)) != 0) {
        logioMsg(dl->lp, dl->debug->pkt, "%s bad systim (ida_timerr statuts %d)", ident, status);
        return TRUE;
    }

    if (ticerr != 0) {
        logioMsg(dl->lp, dl->debug->pkt, "%s intra-record %c%d tic increment error", ident, SIGN(sign), ticerr);
        return TRUE;
    }

    return FALSE;
}

#define REV9_TAG_OFFSET 2 /* offset to IDA9 tag */
#define SUBFORM_OFF    23 /* IDA 9.x subformat, flags field */

static void SetLeapYearBugFlag(UINT8 *raw)
{
static char *Ida9 = IDA_REV9_TAG_X;
static char *IDA9 = IDA_REV9_TAG_0;

    if (memcmp(&raw[REV9_TAG_OFFSET], Ida9, strlen(Ida9)) == 0) { /* alread Ida9 */
        raw[SUBFORM_OFF] |= IDA9_FLAG_LEAPYEAR_BUG;
    } else if (memcmp(&raw[REV9_TAG_OFFSET], IDA9, strlen(IDA9)) == 0) { /* IDA9 */
        memcpy(&raw[REV9_TAG_OFFSET], Ida9, strlen(Ida9));
        raw[SUBFORM_OFF] = IDA9_FLAG_LEAPYEAR_BUG;
    }
}

static BOOL CheckYear(NRTS_DL *dl, IDA_DHDR *crnt, UINT8 *raw, NRTS_PKT *pkt)
{
UINT32 HostTime;
int error, ExpectedYear;
struct { int yr; int jd; int mo; int da; int hr; int mn; int sc; int ms; } host, data;
static char *fid = "CheckYear";

/* Get packet and host times, broken down into components */

    utilTsplit(crnt->end.tru, &data.yr, &data.jd, &data.hr, &data.mn, &data.sc, &data.ms);
    utilJdtomd(data.yr, data.jd, &data.mo, &data.da);

    HostTime = crnt->extra.tstamp +  SAN_EPOCH_TO_1970_EPOCH;
    utilTsplit((double) HostTime, &host.yr, &host.jd, &host.hr, &host.mn, &host.sc, &host.ms);
    utilJdtomd(host.yr, host.jd, &host.mo, &host.da);

/* Leap year check, if enabled */

    if (dl->flags & NRTS_DL_FLAGS_LEAP_YEAR_CHK) {
        if (data.jd == host.jd - 1) {
            SetLeapYearBugFlag(raw);
            if ((error = ida_dhead(dl->ida, crnt, raw)) != 0) {
                logioMsg(dl->lp, LOG_ERR, "%s: ida_dhead error %d", fid, error);
                return FALSE;
            }
            utilTsplit(crnt->end.tru, &data.yr, &data.jd, &data.hr, &data.mn, &data.sc, &data.ms);
            utilJdtomd(data.yr, data.jd, &data.mo, &data.da);
        }
    }

/* Predict what we expect the year portion of the packet time stamp to be */

/* Case 1: host is in new year, packet is in last year */

    if (host.mo == 1 && data.mo == 12) {
        ExpectedYear = host.yr - 1;

/* Case 2: host is in old year, packet is in new year */

    } else if (host.mo == 12 && data.mo == 1) {
        ExpectedYear = host.yr + 1;

/* Default: both host and packet are in current year */

    } else {
        ExpectedYear = host.yr;
    }

/* If they match, then there is nothing to do */

    if (ExpectedYear == data.yr) return TRUE;

/* Otherwise, patch the year in the packet header and reload to make it stick */

    utilPackINT16(raw + YEAR_OFF, (INT16) ExpectedYear);
    logioMsg(dl->lp, dl->debug->pkt, "%s year patched (was %d, now %d)", pkt->ident, data.yr, ExpectedYear);
    if ((error = ida_dhead(dl->ida, crnt, raw)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ida_dhead error %d", fid, error);
        return FALSE;
    }

    return TRUE;
}

static int StationChecks(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, UINT8 *raw, NRTS_PKT *pkt)
{

/* Reject obviously corrupt packets immediately */

    if (HaveGrossErrors(dl, crnt, pkt->ident)) return NRTS_REJECT;

/* Reject duplicates */

    if (IsDuplicate(dl, crnt, prev, pkt->ident)) return NRTS_DUPLICATE;

/* Attempt to set the proper year */

    if (!CheckYear(dl, crnt, raw, pkt)) return NRTS_REJECT;

/* If this is the start of an acquistion session, then refuse to start taking
 * data until we get one with a clearly reasonable time stamp.
 */

    if (FIRST(dl) && WantRealTimeTest(dl) && !RealTimeTest(dl, crnt, pkt->ident)) return NRTS_REJECT;

/* Note time discontinuities */

    pkt->tear = (prev == NULL) ? TRUE : TimeTearTest(dl, crnt, prev, pkt->ident);

/* Look at the external time, patch if possible */

    if (SuspectExternalTime(dl, crnt, prev, raw, pkt)) return NRTS_REJECT;
    pkt->isi.hdr.tofs.value =  crnt->beg.tru;
    pkt->isi.hdr.tols.value =  crnt->end.tru;

/* Note overlaps */

    if (IsCompleteOverlap(dl, crnt, prev, pkt->ident)) return NRTS_COMPLETE_OVERLAP;
    if (IsPartialOverlap(dl, crnt, prev, pkt->ident)) return NRTS_PARTIAL_OVERLAP;

/* Must be OK */

    return NRTS_ACCEPT;
}

/* Check data coming from a remote NRTS.  The packets are guaranteed to be OK since
 * they can't get into the remote NRTS without passing the station checks.  That
 * means all we have to test for is time discontinuities.
 */

static int HubChecks(NRTS_DL *dl, IDA_DHDR *crnt, IDA_DHDR *prev, NRTS_PKT *pkt)
{

/* Reject duplicates and overlaps */

    if (IsDuplicate(dl, crnt, prev, pkt->ident)) return NRTS_DUPLICATE;
    if (IsCompleteOverlap(dl, crnt, prev, pkt->ident)) return NRTS_COMPLETE_OVERLAP;
    if (IsPartialOverlap(dl, crnt, prev, pkt->ident)) return NRTS_PARTIAL_OVERLAP;

/* Note time discontinuities */

    pkt->tear = (prev == NULL) ? TRUE : TimeTearTest(dl, crnt, prev, pkt->ident);

    return NRTS_ACCEPT;
}

static int CheckPacket(NRTS_DL *dl, UINT8 *raw, NRTS_PKT *pkt)
{
int i;
NRTS_STA *sta;
NRTS_CHN *chn;
IDA_DHDR *crnt, *prev;
static char *fid = "CheckPacket";

/* Nothing to do if the disk loop is not configured to store this stream */

    sta = &dl->sys->sta[0];
    for (pkt->chndx = -1, i = 0; pkt->chndx < 0 && i < sta->nchn; i++) {
        if (strcmp(sta->chn[i].name, pkt->isi.hdr.name.chnloc) == 0) pkt->chndx = i;
    }
    if (pkt->chndx < 0) {
//        logioMsg(dl->lp, dl->debug->pkt, "%s: no chndx found for %s", fid, pkt->ident);
        return NRTS_IGNORE;
    }

    chn = &sta->chn[i];

/* Pointers to this and previous header for this particular stream */

    crnt = &pkt->work.buf.ida;
    if (dl->save[pkt->chndx] != NULL) {
        prev = (IDA_DHDR *) dl->save[pkt->chndx]; /* previous header for this stream */
    } else {
        prev = NULL; /* no previous packets for this stream have been seen */
    }

/* Checks are different between stations and hubs, as the latter
 * don't have to allow for abnormal packets.  The station system
 * will take care of elimininating those.
 */

    if (StationSystem(dl)) {
        return StationChecks(dl, crnt, prev, raw, pkt);
    } else {
        return HubChecks(dl, crnt, prev, pkt);
    }
}

BOOL nrtsIdaDecoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out)
{
int error;
NRTS_DL *dl;
IDA_DHDR *crnt;
char tmp[256];
static char *fid = "nrtsIdaDecoder";

    if (arg == NULL || in == NULL || out == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Pull the disk loop handle from the arg */

    dl = (NRTS_DL *) arg;

/* Set up pointers to raw packet */

    crnt = &out->work.buf.ida;
    out->work.len = sizeof(IDA_DHDR);

    out->orig.order = NRTS_BIG_ENDIAN_ORDER;
    switch (dl->ida->rev.value) {
      case 5:
        out->orig.type = ISI_TYPE_IDA5;
        out->isi.data =  in + IDA5678_HEADER_LENGTH;
        break;
      case 6:
        out->orig.type = ISI_TYPE_IDA6;
        out->isi.data =  in + IDA5678_HEADER_LENGTH;
        break;
      case 7:
        out->orig.type = ISI_TYPE_IDA7;
        out->isi.data =  in + IDA5678_HEADER_LENGTH;
        break;
      case 8:
        out->orig.type = ISI_TYPE_IDA8;
        out->isi.data =  in + IDA5678_HEADER_LENGTH;
        break;
      case 9:
        out->orig.type = ISI_TYPE_IDA9;
        out->isi.data =  in + IDA9_HEADER_LENGTH;
        break;
      default:
        logioMsg(dl->lp, LOG_ERR, "%s: format rev %d is unsupported", fid, dl->ida->rev.value);
        return FALSE;
    }

/* We are only interested in waveform data */

    if (ida_rtype((char *) in, dl->ida->rev.value) != IDA_DATA) {
        out->status = NRTS_NOTDATA;
        return TRUE;
    }

/* Decode the header */

    if ((error = ida_dhead(dl->ida, crnt, in)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ida_dhead error %d", fid, error);
        return FALSE;
    }

/* Convert to generic time series format */

    isiInitGenericTSHDR(&out->isi.hdr);

    if (stream != NULL) {
        isiStaChnToStreamName(stream->sta->name, stream->chn->name, &out->isi.hdr.name);
    } else {
        idaBuildStreamName(dl->ida, crnt, &out->isi.hdr.name);
    }
    isiStreamNameString(&out->isi.hdr.name, out->ident);
    isiSintToSrate((REAL64) crnt->sint, &out->isi.hdr.srate);

    out->isi.hdr.desc.order = ISI_ORDER_BIGENDIAN;
    if (crnt->form == IGPP_COMP) {
        out->isi.hdr.desc.comp = ISI_COMP_IDA;
    } else {
        out->isi.hdr.desc.comp = ISI_COMP_NONE;
    }
    if (crnt->wrdsiz == sizeof(INT32)) {
        out->isi.hdr.desc.type = ISI_TYPE_INT32;
        out->isi.hdr.desc.size = sizeof(INT32);
    } else if (crnt->wrdsiz == sizeof(INT16)) {
        out->isi.hdr.desc.type = ISI_TYPE_INT16;
        out->isi.hdr.desc.size = sizeof(INT32);
    } else {
        logioMsg(dl->lp, LOG_ERR, "%s: unsupported sample size (%d)", fid, crnt->wrdsiz);
        return FALSE;
    }

    out->isi.hdr.tofs.value  = crnt->beg.tru;
    out->isi.hdr.tofs.status = crnt->beg.qual;

    out->isi.hdr.tols.value  = crnt->end.tru;
    out->isi.hdr.tols.status = crnt->end.qual;
    out->isi.hdr.nsamp = (UINT32) crnt->nsamp;

    out->isi.hdr.nbytes = crnt->nbytes;

#ifdef DEBUG
    logioMsg(dl->lp, dl->debug->pkt, "%s: %s", fid, isiGenericTsHdrString(&out->isi.hdr, tmp));
#endif /* DEBUG */

    out->extra = crnt->extra;

/* Read only systems take this packet as is, writers are more cautious */

    out->status = (dl->perm == ISI_RDONLY) ? NRTS_ACCEPT : CheckPacket(dl, in, out);

#ifdef DEBUG
    switch (out->status) {
      case NRTS_ACCEPT:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_ACCEPT", fid); break;
      case NRTS_REJECT:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_REJECT", fid); break;
      case NRTS_DUPLICATE:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_DUPLICATE", fid); break;
      case NRTS_OVERLAP:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_OVERLAP", fid); break;
      case NRTS_NOTDATA:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_NOTDATA", fid); break;
      case NRTS_IGNORE:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = NRTS_IGNORE", fid); break;
      default:
        logioMsg(dl->lp, dl->debug->pkt, "%s: status = %d", fid, out->status); break;
    }
#endif /* DEBUG */

    return TRUE;
}

/* Revision History
 *
 * $Log: ida.c,v $
 * Revision 1.26  2008/10/10 21:40:54  dechavez
 * don't log those "no chndx" messages anymore (scares the natives)
 *
 * Revision 1.25  2008/03/05 22:53:23  dechavez
 * added support for NRTS_DL_FLAGS_LEAP_YEAR_CHK, setting the IDA9_FLAG_LEAPYEAR_BUG bit if leap year bug detected
 *
 * Revision 1.24  2007/09/14 19:28:43  dechavez
 * distinguish between complete and partial overlaps
 *
 * Revision 1.23  2007/06/01 19:20:55  dechavez
 * added IDA_EXTRA to NRTS_PKT, saved same
 *
 * Revision 1.22  2007/03/26 21:32:28  dechavez
 * Renamed NRTS_STATION_SYSTEM NRTS_STRICT_TIME constants to NRTS_DL_FLAGS_x
 *
 * Revision 1.21  2007/02/08 23:00:04  dechavez
 * ifdef'd out packet header string/status debug messages
 *
 * Revision 1.20  2006/07/07 17:24:12  dechavez
 * Mark packets from channels missing from NRTS disk loop as NRTS_IGNORE
 * instead of NRTS_REJECT, thus allowing downstream systems the choice of
 * including them in their NRTS loops.
 * Changed debug logging of packet status to print correct values instead
 * of NRTS_REJECT for everthing that wasn't a NRTS_ACCEPT.
 *
 * Revision 1.19  2006/05/17 23:24:51  dechavez
 * added support for ida rev 5
 *
 * Revision 1.18  2006/04/20 19:17:40  dechavez
 * fixed typo in CheckYear (aap)
 *
 * Revision 1.17  2006/02/09 19:39:35  dechavez
 * make hdrstring() conditionally visible
 *
 * Revision 1.16  2006/02/09 19:37:16  dechavez
 * IDA handle support, rework CheckYear for another try
 *
 * Revision 1.15  2005/12/20 16:42:54  dechavez
 * added CheckForYearTransition() test
 *
 * Revision 1.14  2005/12/09 21:22:33  dechavez
 * patch suspect time stamps with fixed ext/hz time pair (and so eliminate bogus
 * "backward external time jump" errors that previous beg/end pairs were causing),
 * return NRTS_NOTDATA for non-data packets (instead of "failing")
 *
 * Revision 1.13  2005/11/03 23:57:45  dechavez
 * explict casts to supress compiler warnings, removed unreferenced variables
 *
 * Revision 1.12  2005/10/18 18:56:35  dechavez
 * SuspectExternalTime() changed to flag all unlocked clocks as suspect
 *
 * Revision 1.11  2005/10/06 22:00:28  dechavez
 * fixed branches in CheckYear that had returns without value
 *
 * Revision 1.10  2005/09/06 20:05:00  dechavez
 * fixed SuspectExternalTime logic
 *
 * Revision 1.9  2005/08/26 21:06:25  dechavez
 * move rev test to _after_ ida ptr is defined (duh)
 *
 * Revision 1.8  2005/08/26 18:24:15  dechavez
 * added ida rev 9 support
 *
 * Revision 1.7  2005/07/27 18:30:45  dechavez
 * set severity of time tear and suspect time messages to LOG_INFO
 *
 * Revision 1.6  2005/07/26 00:00:35  dechavez
 * added strict checks for NRTS_DL output
 *
 */
