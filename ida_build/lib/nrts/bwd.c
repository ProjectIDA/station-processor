#pragma ident "$Id: bwd.c,v 1.27 2007/09/06 18:34:52 dechavez Exp $"
/*======================================================================
 *
 *  MT-safe way of managing the NRTS bwd segment (gap) list
 *
 *====================================================================*/
#include "nrts/dl.h"
#include "cssio.h"

typedef struct {
    LNKLST *ndx; /* list of indices where gaps are to be found */
    LNKLST *bwd; /* bwd list derived from above */
} GAPLNKLST;
    
static void DeriveWfdiscFromPkt(NRTS_DL *dl, NRTS_PKT *pkt, WFDISC *wfdisc)
{
int yr, da, unused;

    *wfdisc = wfdisc_null;
    sprintf(wfdisc->dir, "%s/%s", dl->home, pkt->isi.hdr.name.sta);
    strlcpy(wfdisc->dfile, NRTS_DATNAME, CSS_DFILE_LEN+1);
    strlcpy(wfdisc->sta, pkt->isi.hdr.name.sta, CSS_STA_LEN+1);
    strlcpy(wfdisc->chan, pkt->isi.hdr.name.chnloc, CSS_CHAN_LEN+1);
    strlcpy(wfdisc->datatype, isiDatatypeString(&pkt->isi.hdr.desc), CSS_DATATYPE_LEN+1);
    wfdisc->nsamp = pkt->isi.hdr.nsamp;
    wfdisc->time = pkt->isi.hdr.tofs.value;
    wfdisc->endtime = pkt->isi.hdr.tols.value;
    wfdisc->smprate = (float) 1.0 / (float) isiSrateToSint(&pkt->isi.hdr.srate);
    wfdisc->foff = pkt->foff.dat;
    utilTsplit(wfdisc->time, &yr, &da, &unused, &unused, &unused, &unused);
    wfdisc->jdate = yr * 1000 + da;
}

static BOOL CreateWfdisc(NRTS_DL *dl, LNKLST *list, NRTS_PKT *pkt, WFDISC *new, NRTS_CHN *chn)
{
static BOOL first;
static char *fid = "CreateWfdisc";

    logioMsg(dl->lp, dl->debug->bwd, "create new %s wfdisc record", pkt->ident);

/* If this is the first wfdisc ever for this stream then we need to 
 * update the begin time for the sys monitor.
 */

    if (listFirstNode(list) == NULL) chn->beg = new->time;

/* Append this wfdisc to the list */

    if (!listAppend(list, new, sizeof(WFDISC))) {
        logioMsg(dl->lp, LOG_ERR, "%s: listAppend failed", fid);
        return FALSE;
    }

/* Update stuff for the sys monitor */

    chn->end = new->endtime;
    ++chn->nseg;
    logioMsg(dl->lp, dl->debug->bwd, "increment %s nseg to %d\n", pkt->ident, chn->nseg);

    if (dl->flags & NRTS_DL_FLAGS_USE_BWD_DB) nrtsUpdateBwdDb(dl->db, new, NRTS_CREATE);

    return TRUE;
}

static BOOL ReduceWfdisc(NRTS_DL *dl, LNKLST *list, NRTS_PKT *pkt, WFDISC *new, NRTS_CHN *chn)
{
int errors;
LNKLST_NODE *crnt;
WFDISC *first;
static char *fid = "ReduceWfdisc";

/* It's a bug if we don't already have a wfdisc for this stream */

    if ((crnt = listFirstNode(list)) == NULL) {
        logioMsg(dl->lp, LOG_WARN, "%s: UNEXPECTED EMPTY LNKLST", fid);
        return FALSE;
    }
    first = (WFDISC *) crnt->payload;

/* Sanity checks (sample rate, station and channel names must never change) */

    errors = 0;

    if (first->smprate != new->smprate) {
        logioMsg(dl->lp, LOG_ERR, "%s: ERROR: variable sample rate (%.3f != %.3f)!", fid, first->smprate, new->smprate);
        ++errors;
    }

    if (strcmp(first->sta, new->sta) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ERROR: variable station name (%.3f != %.3f)!", fid, first->sta, new->sta);
        ++errors;
    }

    if (strcmp(first->chan, new->chan) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ERROR: variable channel name (%s != %s)!", fid, first->chan, new->chan);
        ++errors;
    }

    if (errors) {
        logioMsg(dl->lp, LOG_ERR, "%s: FAILED SANITY CHECKS", fid);
        return FALSE;
    }

/* First reduce the number of samples the first record points to */

    first->nsamp -= new->nsamp;
    if (first->nsamp < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ERROR: negative %s:%s nsamp after reduction (new nsamp=%d results nsamp=%d", fid, new->sta, new->chan, new->nsamp, first->nsamp);
        return FALSE;
    }

/* Delete the record if the data it points to have been completely overwritten */

    if (first->nsamp == 0) {
        if ((crnt = listRemoveNode(list, crnt)) == NULL) {
            logioMsg(dl->lp, LOG_ERR, "%s: listRemoveNode failed", fid);
            return FALSE;
        }
        --chn->nseg;
        logioMsg(dl->lp, dl->debug->bwd, "decrement %s nseg to %d\n", pkt->ident, chn->nseg);
        first = (WFDISC *) crnt->payload;
    } else {

/* Otherwise finish updating the first wfdisc record */

        first->foff += new->nsamp * pkt->isi.hdr.desc.size;
        first->time += (new->nsamp / first->smprate);
    }

    chn->beg = first->time;

    if (dl->flags & NRTS_DL_FLAGS_USE_BWD_DB) nrtsUpdateBwdDb(dl->db, first, NRTS_REDUCE);

    return TRUE;
}

static BOOL ExtendWfdisc(NRTS_DL *dl, LNKLST *list, NRTS_PKT *pkt, WFDISC *new, NRTS_CHN *chn)
{
WFDISC *last;
LNKLST_NODE *crnt;
static char *fid = "ExtendWfdisc";

/* If we don't have any wfdiscs for this stream, then create one */

    if ((crnt = listFirstNode(list)) == NULL) {
        logioMsg(dl->lp, dl->debug->bwd, "%s: empty list, create new wfdisc", fid);
        return CreateWfdisc(dl, list, pkt, new, chn);
    }

/* Find the last wfdisc in the list */

    while (listNextNode(crnt) != NULL) crnt = listNextNode(crnt);
    last = (WFDISC *) crnt->payload;

/* Update that wfdisc to include the new data */

    last->nsamp += new->nsamp;
    last->endtime = new->endtime;

/* Update the sys info as well, so that it will show up in the monitor */

    chn->end = last->endtime;

    if (dl->flags & NRTS_DL_FLAGS_USE_BWD_DB) nrtsUpdateBwdDb(dl->db, last, NRTS_EXTEND);

    return TRUE;
}

static BOOL CheckContinuity(NRTS_DL *dl, LNKLST *list, NRTS_PKT *pkt, WFDISC *new, NRTS_CHN *chn)
{
WFDISC *prev;
LNKLST_NODE *last;
double gap;
static char *fid = "CheckContinuity";

/* Nothing to check if the list for this stream is empty */

    if ((last = listLastNode(list)) == NULL) return CreateWfdisc(dl, list, pkt, new, chn);

/* Get the wfdisc record that points to the last sample */

    prev = (WFDISC *) last->payload;

/* Need to generate a new record if the sample rate has changed */

    if (prev->smprate != new->smprate) {
        logioMsg(dl->lp, LOG_INFO, "%s sample rate changed", pkt->ident);
        return CreateWfdisc(dl, list, pkt, new, chn);
    }

/* Need to generate a new record if there is any kind of overlap of if there
 * is a  gap of more than one sample
 */

//  gap = (new->time - prev->endtime) * prev->smprate;
    gap = (new->time - (prev->time+prev->nsamp/prev->smprate)) * prev->smprate;
    if (((int) fabs(gap)) > 0) {
        logioMsg(dl->lp, dl->debug->bwd, "%.5f sample %s time tear", gap, pkt->ident);
        return CreateWfdisc(dl, list, pkt, new, chn);
    }

/* Otherwise we just extend the existing wfdisc record */

    return ExtendWfdisc(dl, list, pkt, new, chn);

}

/* Update wfdisc list to include data in the supplied packet */

BOOL nrtsUpdateBwd(NRTS_DL *dl, NRTS_PKT *pkt, int action)
{
LNKLST *list;
BOOL result, TimeToFlush;
WFDISC wfdisc;
NRTS_CHN *chn;
static char *fid = "nrtsUpdateBwd";

    if (dl == NULL || pkt == NULL) {
        if (dl != NULL) logioMsg(dl->lp, LOG_ERR, "%s: NULL inputs not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

/* Ignore this packet if it isn't in the disk loop */

    if (pkt->chndx < 0) return TRUE;

/* Get the NRTS_CHN for this stream so we can update the sys stuff */

    chn = &dl->sys->sta[0].chn[pkt->chndx];

/* Convert to wfdisc format */

    DeriveWfdiscFromPkt(dl, pkt, &wfdisc);

/* Find the list associated with this stream */

    list = &dl->bwd.list[pkt->chndx];

/* Take requested action */

    MUTEX_LOCK(&dl->bwd.mutex);
        dl->sys->bwd = NRTS_BUSY1;
        switch (action) {
          case NRTS_EXTEND:
#ifdef DEBUG
            logioMsg(dl->lp, dl->debug->bwd, "%s: action = NRTS_EXTEND", fid);
#endif /* DEBUG */
            result = ExtendWfdisc(dl, list, pkt, &wfdisc, chn);
            break;
          case NRTS_REDUCE:
#ifdef DEBUG
            logioMsg(dl->lp, dl->debug->bwd, "%s: action = NRTS_REDUCE", fid);
#endif /* DEBUG */
            result = ReduceWfdisc(dl, list, pkt, &wfdisc, chn);
            break;
          case NRTS_CREATE:
            logioMsg(dl->lp, dl->debug->bwd, "%s: action = NRTS_CREATE", fid);
            result = CreateWfdisc(dl, list, pkt, &wfdisc, chn);
            break;
          case NRTS_TESTWD:
            logioMsg(dl->lp, dl->debug->bwd, "%s: action = NRTS_TESTWD", fid);
            result = CheckContinuity(dl, list, pkt, &wfdisc, chn);
            break;
          case NRTS_NOP:
            logioMsg(dl->lp, dl->debug->bwd, "%s: action = NRTS_NOP", fid);
            result = TRUE;
            break;
          default:
            logioMsg(dl->lp, LOG_ERR, "%s: UNEXPECTED ACTION FLAG '%d'", fid, action);
            result = FALSE;
        }
        TimeToFlush = (time(NULL) - dl->bwd.tolf) > dl->bwd.flushint ? TRUE : FALSE;
    MUTEX_UNLOCK(&dl->bwd.mutex);

    if (TimeToFlush && !nrtsFlushBwd(dl)) {
        logioMsg(dl->lp, LOG_ERR, "%s: WARNING: nrtsFlushBwd() failed", fid);
    }

    return result;
}

/* Flush all wfdiscs to disk */

BOOL nrtsFlushBwd(NRTS_DL *dl)
{
int i;
FILE *fp;
LNKLST_NODE *crnt;
static char *fid = "nrtsFlushBwd";

    if (dl == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (dl->flags & NRTS_DL_FLAGS_NO_BWD_FF) return TRUE; /* not using bwd file */

    if ((fp = fopen(dl->fname.bwd, "wb")) == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: FlushBwdList: fopen: %s: %s", fid, dl->fname.bwd, strerror(errno));
        return FALSE;
    }

    MUTEX_LOCK(&dl->bwd.mutex);
        for (i = 0; i < dl->sys->sta[0].nchn; i++) {
            crnt = listFirstNode(&dl->bwd.list[i]);
            while (crnt != NULL) {
                if (fwrite(crnt->payload, sizeof(WFDISC), 1, fp) != 1) {
                    logioMsg(dl->lp, LOG_ERR, "%s: FlushBwdList: fwrite: %s: %s", fid, dl->fname.bwd, strerror(errno));
                    MUTEX_UNLOCK(&dl->bwd.mutex);
                    fclose(fp);
                    return FALSE;
                }
                crnt = listNextNode(crnt);
            }
        }
        dl->bwd.tolf = time(NULL);
        if (dl->sys != NULL) dl->sys->bwd = NRTS_STABLE;
    MUTEX_UNLOCK(&dl->bwd.mutex);
    fclose(fp);

    logioMsg(dl->lp, dl->debug->bwd, "%s: flush OK", fid);
    return TRUE;

}

static BOOL SanityCheck(NRTS_DL *dl, NRTS_PKT *beg, NRTS_PKT *end, char *ident)
{
static char *fid = "SanityCheck";

    if (beg->chndx != end->chndx) {
        logioMsg(dl->lp, LOG_ERR, "%s: ident=%s, chndx mismatch!", fid, ident);
        return FALSE;
    }

    if (memcmp(&beg->isi.hdr.desc, &end->isi.hdr.desc, sizeof(ISI_DATA_DESC)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ident=%s, desc mismatch!", fid, ident);
        return FALSE;
    }

    if (memcmp(&beg->isi.hdr.srate, &end->isi.hdr.srate, sizeof(ISI_SRATE)) != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: ident=%s, srate mismatch!", fid, ident);
        return FALSE;
    }

    if (beg->foff.dat >= end->foff.dat) {
        logioMsg(dl->lp, LOG_ERR, "%s: ident=%s, inconsistent foffs!", fid, ident);
        return FALSE;
    }

    return TRUE;
}

static int GapTest(NRTS_DL *dl, NRTS_PKT *beg, NRTS_PKT *end, char *ident)
{
long gap;
int wrdsiz;
WFDISC left, rite;
struct { double time; long samp; long byte; } span;

    DeriveWfdiscFromPkt(dl, beg, &left);
    DeriveWfdiscFromPkt(dl, end, &rite);

    wrdsiz = beg->isi.hdr.desc.size;

    span.time = rite.time - left.time;
    span.samp = (long) (span.time * left.smprate);
    span.byte = span.samp * wrdsiz;

    gap = span.byte - (rite.foff - left.foff);

    if (gap == wrdsiz || gap == -wrdsiz) gap = 0;

    return gap;
}

static BOOL RecursiveGapSearch(NRTS_DL *dl, INT32 begndx, INT32 endndx, NRTS_STREAM *stream, GAPLNKLST *list, char *ident, NRTS_PKT *beg, NRTS_PKT *end)
{
int status, gap, midpoint;
char tmpbuf[256];
static char *fid = "RecursiveGapSearch";


    if (begndx == endndx) return TRUE;

    ++dl->bwd.level;
    logioMsg(dl->lp, dl->debug->bwd, "%s[%d]: ident=%s, begndx=%d, endndx=%d", fid, dl->bwd.level, ident, begndx, endndx);
    
    if ((status = nrtsRead(dl, stream, begndx, dl->buf, dl->buflen, beg, NULL)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: failed nrtsRead ident=%s, begndx=%d, status=%d: %s", fid, ident, begndx, status, strerror(errno));
        return FALSE;
    }
    
    if ((status = nrtsRead(dl, stream, endndx, dl->buf, dl->buflen, end, NULL)) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: failed nrtsRead ident=%s, endndx=%d, status=%d: %s", fid, ident, endndx, status, strerror(errno));
        return FALSE;
    }

    logioMsg(dl->lp, dl->debug->bwd, "%s: beg= %s", fid, isiGenericTsHdrString(&beg->isi.hdr, tmpbuf));
    logioMsg(dl->lp, dl->debug->bwd, "%s: end= %s", fid, isiGenericTsHdrString(&end->isi.hdr, tmpbuf));

    if (!SanityCheck(dl, beg, end, ident)) {
        logioMsg(dl->lp, LOG_ERR, "%s: failed SanityCheck ident=%s, begndx=%d, endndx=%d", fid, ident, begndx, endndx);
        return FALSE;
    }

    gap = GapTest(dl, beg, end, ident);
    logioMsg(dl->lp, dl->debug->bwd, "%s[%d]: ident=%s, begndx=%d, endndx=%d, gap=%d", fid, dl->bwd.level, ident, begndx, endndx, gap);
    if (gap != 0) {
        if (endndx == begndx + 1) {
            logioMsg(dl->lp, dl->debug->bwd, "%s[%d]: GAP DETECTED between %d and %d: BRANCH COMPLETE", fid, dl->bwd.level, begndx, endndx);
            if (!listAppend(list->ndx, &begndx, sizeof(int))) {
                logioMsg(dl->lp, LOG_ERR, "%s: listAppend failed", fid);
                return FALSE;
            }
            --dl->bwd.level;
            return TRUE;
        } else {
            midpoint = begndx + (endndx - begndx) / 2;
            if (!RecursiveGapSearch(dl, begndx, midpoint, stream, list, ident, beg, end)) {
                logioMsg(dl->lp, LOG_ERR, "%s[%d]: RecursiveGapSearch failed (%d, %d, %s)", begndx, endndx, ident);
                return FALSE;
            }
            if (!RecursiveGapSearch(dl, midpoint, endndx, stream, list, ident, beg, end)) {
                logioMsg(dl->lp, LOG_ERR, "%s[%d]: RecursiveGapSearch failed (%d, %d, %s)", begndx, endndx, ident);
                return FALSE;
            }
        }
    }

    logioMsg(dl->lp, dl->debug->bwd, "%s[%d]: ident=%s, begndx=%d, endndx=%d: BRANCH COMPLETE", fid, dl->bwd.level, ident, begndx, endndx);
    --dl->bwd.level;
    return TRUE;
}

static BOOL GapSearch(NRTS_DL *dl, int begndx, int endndx, NRTS_STREAM *stream, GAPLNKLST *list, char *ident)
{
NRTS_PKT beg, end;

    beg.indx = end.indx = -1;

    return RecursiveGapSearch(dl, begndx, endndx, stream, list, ident, &beg, &end);
}

static BOOL PopulateBwdList(NRTS_DL *dl, int begndx, int endndx, NRTS_STREAM *stream, GAPLNKLST *list, char *ident)
{
struct {
    NRTS_PKT pkt;
    int ndx;
    WFDISC wd;
} beg, end;
int status;
BOOL done;
LNKLST_NODE *crnt;
char tmpbuf[512];
#define ValueOf(ptr) ((int) (*(int *) ptr->payload))
static char *fid = "PopulateBwdList";

    crnt = listFirstNode(list->ndx);

    beg.ndx = begndx;
    if (crnt != NULL) {
        end.ndx = ValueOf(crnt);
        crnt = listNextNode(crnt);
    } else {
        end.ndx = endndx;
    }

    beg.pkt.indx = end.pkt.indx = -1;

    done = FALSE;
    do {
        logioMsg(dl->lp, dl->debug->bwd, "%s: generate record to span %d through %d", fid, beg.ndx, end.ndx);

        if ((status = nrtsRead(dl, stream, beg.ndx, dl->buf, dl->buflen, &beg.pkt, NULL)) < 0) {
            logioMsg(dl->lp, LOG_ERR, "%s: failed nrtsRead ident=%s, beg.ndx=%d, status=%d: %s", fid, ident, beg.ndx, status, strerror(errno));
            return FALSE;
        }
        if ((status = nrtsRead(dl, stream, end.ndx, dl->buf, dl->buflen, &end.pkt, NULL)) < 0) {
            logioMsg(dl->lp, LOG_ERR, "%s: failed nrtsRead ident=%s, end.ndx=%d, status=%d: %s", fid, ident, end.ndx, status, strerror(errno));
            return FALSE;
        }

        logioMsg(dl->lp, dl->debug->bwd, "%s: beg=%s", fid, isiGenericTsHdrString(&beg.pkt.isi.hdr, tmpbuf));
        logioMsg(dl->lp, dl->debug->bwd, "%s: end=%s", fid, isiGenericTsHdrString(&end.pkt.isi.hdr, tmpbuf));

        DeriveWfdiscFromPkt(dl, &beg.pkt, &beg.wd);
        DeriveWfdiscFromPkt(dl, &end.pkt, &end.wd);

        beg.wd.endtime = end.wd.endtime;
        beg.wd.nsamp = end.wd.nsamp + (end.wd.foff - beg.wd.foff) / beg.pkt.isi.hdr.desc.size;

        if (!listAppend(list->bwd, &beg.wd, sizeof(WFDISC))) {
            logioMsg(dl->lp, LOG_ERR, "%s: failed listAppend ident=%s: %s", fid, ident, strerror(errno));
            return FALSE;
        }

        if (end.ndx != endndx) {
            beg.ndx = end.ndx + 1;
            if (crnt != NULL) {
                end.ndx = ValueOf(crnt);
                crnt = listNextNode(crnt);
            } else {
                end.ndx = endndx;
           }
        } else {
            done = TRUE;
        }
    } while (!done);    

    listClear(list->ndx);
    return TRUE;
}

static BOOL BuildSegList(NRTS_DL *dl, NRTS_STREAM *stream, GAPLNKLST *list, char *ident)
{
int begndx, endndx;
static char *fid = "BuildSegList";

    logioMsg(dl->lp, dl->debug->bwd, "%s: yngest=%d, oldest=%d", fid, stream->chn->hdr.yngest, stream->chn->hdr.oldest);

/* nothing to do if there are no data for this stream */

    if (stream->chn->hdr.yngest < 0) return TRUE;

/* need to make two passes if we have wraparound */

    dl->bwd.level = 0;
    if (stream->chn->hdr.yngest < stream->chn->hdr.oldest) {
        logioMsg(dl->lp, dl->debug->bwd, "%s: need to make two passes", fid);

        /* do first half (oldest packet to end of file) */

        begndx = stream->chn->hdr.oldest;
        endndx = stream->chn->hdr.nrec - 1;
        logioMsg(dl->lp, dl->debug->bwd, "%s: 1st half (oldest to EOF)", fid);
        if (!GapSearch(dl, begndx, endndx, stream, list, ident)) {
            logioMsg(dl->lp, LOG_ERR, "%s: GapSearch failed (%d, %d, %s)", fid, begndx, endndx, ident);
            return FALSE;
        }
        if (!PopulateBwdList(dl, begndx, endndx, stream, list, ident)) {
            logioMsg(dl->lp, LOG_ERR, "%s: GapSearch failed (%d, %d, %s)", fid, begndx, endndx, ident);
            return FALSE;
        }

        /* do second half (begining of file to most youngest packet) */

        logioMsg(dl->lp, dl->debug->bwd, "%s: 2nd half (BOF to yngest)", fid);
        begndx = 0;
        endndx = stream->chn->hdr.yngest;

/* otherwise, search oldest to youngest in one pass */

    } else {
        logioMsg(dl->lp, dl->debug->bwd, "%s: searching from oldest to yngest in one pass", fid);
        begndx = stream->chn->hdr.oldest;
        endndx = stream->chn->hdr.yngest;
    }

    if (dl->bwd.level != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: GapSearch BUG, level=%d", fid, dl->bwd.level);
        return FALSE;
    }

    if (!GapSearch(dl, begndx, endndx, stream, list, ident)) {
        logioMsg(dl->lp, LOG_ERR, "%s: GapSearch failed (%d, %d, %s)", fid, begndx, endndx, ident);
        return FALSE;
    }
    if (!PopulateBwdList(dl, begndx, endndx, stream, list, ident)) {
        logioMsg(dl->lp, LOG_ERR, "%s: GapSearch failed (%d, %d, %s)", fid, begndx, endndx, ident);
        return FALSE;
    }

    if (dl->bwd.level != 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: GapSearch BUG, level=%d", fid, dl->bwd.level);
        return FALSE;
    }

    stream->chn->nseg = list->bwd->count;

    return TRUE;
}

static void InitBwd(NRTS_BWD *bwd)
{
    MUTEX_INIT(&bwd->mutex);
    bwd->tolf = 0;
    bwd->flushint = NRTS_DEFAULT_BWD_FLUSHINT;
}

BOOL nrtsBuildBwdFromDisk(NRTS_DL *dl)
{
int i;
NRTS_STREAM stream;
GAPLNKLST list;
LNKLST ndxlist;
char ident[NRTS_SNAMLEN + 1 + NRTS_CNAMLEN + 1];
static char *fid = "nrtsBuildBwdFromDisk";

    if (dl == NULL) {
        logioMsg(dl->lp, LOG_ERR, "%s: NULL input not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    InitBwd(&dl->bwd);

/* Seach the disk loop for gaps and create list */

    stream.sys = dl->sys;
    stream.sta = &dl->sys->sta[0];
    list.ndx = &ndxlist;
    listInit(list.ndx);

    for (i = 0; i < stream.sta->nchn; i++) {
        stream.chn = &stream.sta->chn[i];
        list.bwd = &dl->bwd.list[i];
        listInit(list.bwd);
        sprintf(ident, "%s:%s", stream.sta->name, stream.chn->name);
        logioMsg(dl->lp, dl->debug->bwd, "%s: **** BEGIN %s ****", fid, ident);
        if (!BuildSegList(dl, &stream, &list, ident)) {
            logioMsg(dl->lp, LOG_ERR, "%s: BuildSegList failed for %s", fid, ident);
            return FALSE;
        }
        logioMsg(dl->lp, dl->debug->bwd, "%s: **** FINISHED %s ****", fid, ident);
    }

    listDestroy(list.ndx);

/* Flush gap list to disk */

    if (!nrtsFlushBwd(dl)) {
        logioMsg(dl->lp, LOG_ERR, "%s: nrtsFlushBwd failed", fid);
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: bwd.c,v $
 * Revision 1.27  2007/09/06 18:34:52  dechavez
 * added station and channel name to ReduceWfdisc() sanity checks, added station/channel name in nsamp error messsage
 *
 * Revision 1.26  2007/06/01 19:51:05  dechavez
 * fixed spurious (zero sample) gaps
 *
 * Revision 1.25  2007/06/01 19:13:54  dechavez
 * fixed chn nseg on initialization from disk, added new (NULL) chn args to nrtsRead
 *
 * Revision 1.24  2007/04/18 22:56:32  dechavez
 * support NRTS_DL_FLAGS_NO_BWD_FF option, use aap formula for computing gap size
 *
 * Revision 1.23  2007/04/02 03:12:00  dechavez
 * corrected nrtsUpdateBwdDb() args for NRTS_EXTEND and NRTS_REDUCE cases (aap)
 *
 * Revision 1.22  2007/03/26 21:42:32  dechavez
 * call nrtsUpdateBwdDb() if option set in dl flags
 *
 * Revision 1.21  2007/02/01 01:54:44  dechavez
 * NRTS_TESTWD support
 *
 * Revision 1.20  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.19  2006/08/15 01:16:34  dechavez
 * fixed error introduced in 4.1.5
 *
 * Revision 1.18  2006/07/20 20:27:57  dechavez
 * additional uninitialized NRTS_PKT.indx bug fix (GapSearch)
 *
 * Revision 1.17  2006/07/19 22:18:47  dechavez
 * additional uninitialized NRTS_PKT.indx before nrtsRead bug fixes
 *
 * Revision 1.16  2006/07/18 20:14:32  dechavez
 * fixed uninitialized beg/end pkt indicies bug in PopulateBwdList (aap)
 *
 * Revision 1.15  2006/06/19 18:11:19  dechavez
 * redesigned recursive gap search to minimize stack requirements and use new smart
 * nrtsRead for performance improvments (aap)
 *
 * Revision 1.14  2006/02/14 17:05:29  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.13  2006/02/09 19:49:06  dechavez
 * ISI_STREAM_NAME oldchn -> chnlock, set bwd status while locked
 *
 * Revision 1.12  2005/11/03 23:59:56  dechavez
 * explict casts to supress compiler warnings, removed unreferenced variables
 *
 * Revision 1.11  2005/10/06 22:01:46  dechavez
 * added missing default return from SanityCheck
 *
 * Revision 1.10  2005/09/30 18:11:17  dechavez
 * creating wfdisc record message demoted to LOG_DEBUG
 *
 * Revision 1.9  2005/07/27 18:31:56  dechavez
 * set severity of create wfdisc record message to LOG_INFO
 *
 * Revision 1.8  2005/07/25 23:42:13  dechavez
 * initial release of MT-safe routines
 *
 */
