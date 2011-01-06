#pragma ident "$Id: print.c,v 1.8 2007/10/31 17:03:43 dechavez Exp $"
/*======================================================================
 *
 * Print/log stuff
 *
 *====================================================================*/
#include "isi/dl.h"

#define BUFLEN 256

static INT64 SetTimeStamps(ISI_DL *dl, INT64 *uptime, INT64 *update)
{
INT64 now;

    now = utilTimeStamp();

    if (dl->sys->state != ISI_INACTIVE) {
        *uptime = now - dl->sys->tstamp.start;
    } else {
        *uptime = UTIL_UNDEFINED_TIMESTAMP;
    }

    if (dl->sys->tstamp.write != UTIL_UNDEFINED_TIMESTAMP) {
        *update = now - dl->sys->tstamp.write;
    } else {
        *update = UTIL_UNDEFINED_TIMESTAMP;
    }

    return now;
}

void isidlLogDL(ISI_DL *dl, int level)
{
ISI_DL_SYS sys;
ISI_DL snapshot;
INT64 uptime, update;
char buf[BUFLEN];

    if (!isidlSnapshot(dl, &snapshot, &sys)) return;

    SetTimeStamps(&snapshot, &uptime, &update);

    logioMsg(dl->lp, level, "  site = %s", sys.site);
    logioMsg(dl->lp, level, " seqno = %s", isiSeqnoString(&sys.seqno, buf));
    logioMsg(dl->lp, level, "maxlen = %lu", sys.maxlen);
    logioMsg(dl->lp, level, "parent = %d", sys.parent);
    logioMsg(dl->lp, level, " state = %s", isidlStateString(sys.state));
    logioMsg(dl->lp, level, " count = %llu", sys.count);
    logioMsg(dl->lp, level, " index = %s", isidlIndexString(&sys.index, buf));
    logioMsg(dl->lp, level, "uptime = %s", utilTimeString(uptime, 108, buf, BUFLEN));
    logioMsg(dl->lp, level, "update = %s", utilTimeString(update, 108, buf, BUFLEN));
    if (dl->flags & ISI_DL_FLAGS_HAVE_META) {
        if (dl->flags & ISI_DL_FLAGS_HAVE_QDP_STATE) {
            logioMsg(dl->lp, level, "  meta = %s (state file present)", dl->path.meta);
        } else {
            logioMsg(dl->lp, level, "  meta = %s", dl->path.meta);
        }
    } else {
        logioMsg(dl->lp, level, "  meta = UNAVAIL\n");
    }
    logioMsg(dl->lp, level, "  nrts = %s\n", dl->nrts != NULL ? "avail" : "UNAVAIL");
}

void isidlPrintDL(FILE *fp, ISI_DL *dl)
{
ISI_DL_SYS sys;
ISI_DL snapshot;
INT64 now, uptime, update;
ISI_RAW_PACKET raw;
char buf[BUFLEN];

    if (!isidlSnapshot(dl, &snapshot, &sys)) return;
    isiInitRawPacket(&raw, NULL, sys.maxlen);

    now = SetTimeStamps(&snapshot, &uptime, &update);

    fprintf(fp, "  site = %s\n", sys.site);
    fprintf(fp, "   now = %s\n", utilTimeString(now, 100, buf, BUFLEN));
    fprintf(fp, " start = %s\n", utilTimeString(sys.tstamp.start, 100, buf, BUFLEN));
    fprintf(fp, "uptime = %s\n", utilTimeString(uptime, 108, buf, BUFLEN));
    fprintf(fp, "update = %s\n", utilTimeString(update, 108, buf, BUFLEN));
    fprintf(fp, "  base = %s\n", dl->base);
    fprintf(fp, " seqno = %s\n", isiSeqnoString(&sys.seqno, buf));
    fprintf(fp, "oldest = ");
    if (!isidlReadDiskLoop(dl, &raw, sys.index.oldest)) {
        fprintf(fp, "UNREADABLE!\n");
    } else {
        fprintf(fp, "%s\n", isiSeqnoString(&raw.hdr.seqno, buf));
    }
    if (dl->tee.disabled) {
        fprintf(fp, "   tee = disabled\n");
    } else {
        fprintf(fp, "   tee = %08lx%016llx\n", sys.seqno.signature, (sys.seqno.counter / dl->glob->trecs) * dl->glob->trecs);
    }
    fprintf(fp, "numpkt = %lu\n", sys.numpkt);
    fprintf(fp, "maxlen = %lu\n", sys.maxlen);
    fprintf(fp, "parent = %d\n", sys.parent);
    fprintf(fp, " count = %llu\n", sys.count);
    fprintf(fp, " index = %s\n", isidlIndexString(&sys.index, buf));
    fprintf(fp, "backup = %s\n", isidlIndexString(&sys.backup, buf));
    fprintf(fp, " state = %s\n", isidlStateString(sys.state));
    if (dl->flags & ISI_DL_FLAGS_HAVE_META) {
        if (dl->flags & ISI_DL_FLAGS_HAVE_QDP_STATE) {
            fprintf(fp, "  meta = %s (state file present)\n", dl->path.meta);
        } else {
            fprintf(fp, "  meta = %s\n", dl->path.meta);
        }
    } else {
        fprintf(fp, "  meta = UNAVAIL\n");
    }
    fprintf(fp, "  nrts = %s\n", dl->nrts != NULL ? "avail" : "UNAVAIL");

}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.8  2007/10/31 17:03:43  dechavez
 * added buffer length to utilTimeString() calls
 *
 * Revision 1.7  2007/04/18 23:12:35  dechavez
 * note state file, if present
 *
 * Revision 1.6  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.5  2006/12/12 22:45:57  dechavez
 * print availability of metadata
 *
 * Revision 1.4  2006/12/08 17:31:10  dechavez
 * include sequence number of oldest packet in isiPrintDL()
 *
 * Revision 1.3  2006/03/13 22:34:07  dechavez
 * include tee file name in isiPrintDL() output
 *
 * Revision 1.2  2005/09/10 21:40:56  dechavez
 * ISI_DL_SYS instead of ISI_DL_HDR
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
