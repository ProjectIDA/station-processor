#pragma ident "$Id: wfdisc.c,v 1.5 2007/06/12 23:03:56 dechavez Exp $"
/*======================================================================
 *
 *  Snapshot of the current (NRTS) wfdiscs.  Since a CSS 3.0 wfdisc can
 *  only point to 99,999,999 samples, we break records which point to
 *  more than that into smaller records.
 *
 *====================================================================*/
#include "isi/dl.h"

#define MAX_NSAMP 99999999 /* %8ld */

static BOOL SplitLargeRecord(LNKLST *list, WFDISC *crnt, int len, BOOL *failed)
{
WFDISC new;
double sint;

    if (crnt->nsamp <= MAX_NSAMP) return FALSE;

    new = *crnt;
    sint = (double) 1.0 / (double) new.smprate;

    new.nsamp = MAX_NSAMP;
    new.endtime = new.time + ((double) (MAX_NSAMP - 1) * sint);
    if (!listAppend(list, &new, len)) {
        *failed = TRUE;
        return FALSE;
    }

    crnt->nsamp -= MAX_NSAMP;
    crnt->time = new.endtime + sint;
    crnt->jdate = atol((const char *)util_dttostr(crnt->time, 4));
    crnt->foff += MAX_NSAMP * cssio_wrdsize(crnt->datatype);

    return TRUE;
}

static BOOL EnforceSampleMaximum(LNKLST *list, int len)
{
WFDISC *wfdisc;
LNKLST_NODE *crnt;
BOOL failed = FALSE;

    crnt = listFirstNode(list);
    while (crnt != NULL) {
        wfdisc = (WFDISC *) crnt->payload;
        while (SplitLargeRecord(list, wfdisc, len, &failed));
        if (failed) return FALSE;
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

BOOL isidlGetWfdiscList(ISI_DL_MASTER *master, LNKLST *list, UINT32 maxdur)
{
int fd;
char *bwd;
WFDISC wfdisc;
int i, len = sizeof(WFDISC);
static char *fid = "isidlGetWfdiscList";

    if (master == NULL || list == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    for (i = 0; i < master->ndl; i++) {
        if (master->dl[i]->nrts != NULL) {
            bwd = master->dl[i]->nrts->fname.bwd;
            if ((fd = open(bwd, O_RDONLY|O_BINARY)) < 0) {
                logioMsg(master->lp, LOG_ERR, "%s: open: %s: %s", fid, bwd);
                return FALSE;
            }
            while (read(fd, &wfdisc, len) == len) {
                if (!listAppend(list, &wfdisc, len)) {
                    logioMsg(master->lp, LOG_ERR, "%s: listAppend: %s", fid, strerror(errno));
                    return FALSE;
                }
            }
            close(fd);
        }
    }

    if (!EnforceSampleMaximum(list, len)) {
        logioMsg(master->lp, LOG_ERR, "%s: EnforceSampleMaximum: %s", fid, strerror(errno));
        return FALSE;
    }

    if (!listSetArrayView(list)) {
        logioMsg(master->lp, LOG_ERR, "%s: listSetArrayView: %s", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: wfdisc.c,v $
 * Revision 1.5  2007/06/12 23:03:56  dechavez
 * ensure that no record points to more than 99,999,999 samples
 *
 * Revision 1.4  2007/02/01 02:00:48  dechavez
 * allow for missing NRTS disk loop
 *
 * Revision 1.3  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.2  2006/02/14 17:05:26  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
