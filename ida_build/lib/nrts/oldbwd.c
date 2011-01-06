#pragma ident "$Id: oldbwd.c,v 1.1 2005/07/25 23:40:25 dechavez Exp $"
/*======================================================================
 *
 *  Manage the NRTS binary wfdiscs.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"
#include "cssio.h"

static int flag  = 1;
static int bwdfd = -1;
static struct nrts_sys *sys = NULL;

static struct wdlist {
    struct wfdisc wfdisc;
    struct wdlist *next;
} list_head[NRTS_MAXSTA][NRTS_MAXCHN];

void nrts_inibwdlist(int ofd, struct nrts_sys *sysptr)
{
int i, j;

    flag = 2;

    if (bwdfd ==   -1) bwdfd = ofd;
    if (sys   == NULL) sys   = sysptr;

    if (sys->type == NRTS_ASL) return;

    for (i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            list_head[i][j].next  = (struct wdlist *) NULL;
            sys->sta[i].chn[j].nseg = 0;
            sys->sta[i].chn[j].beg  = (double) -1;
            sys->sta[i].chn[j].end  = (double) -1;
        }
    }
}

#ifdef __STDC__
int nrts_inibwd(
    struct nrts_files *file,
    struct nrts_sys *sysptr,
    char *syscode,
    char *home
)
#else
int nrts_inibwd(file, sysptr, syscode, home)
struct nrts_files *file;
struct nrts_sys *sysptr;
char *syscode;
char *home;
#endif
{
int i, mode;
int have_data = 0;
size_t nrec;
struct wfdisc *array;
struct nrts_sta *sta;
struct nrts_header header;
static char *fid = "nrts_inibwd";

    flag = 2;

    if (sys == NULL) sys = sysptr;

/*  If the wfdiscs file is not stable we have a problem  */

    if (sys->bwd != NRTS_STABLE) {
        util_log(1, "wfdiscs are not current... run nrts_bwd");
        return -1;
    }

/*  Open the output file  */

#ifdef O_SYNC
    mode = O_RDWR | O_SYNC;
#else
    mode = O_RDWR;
#endif /* O_SYNC */

    if ((bwdfd = open(file->bwd, mode|O_BINARY)) < 0) {
        util_log(1, "%s: open: %s: %s", fid, file->bwd, syserrmsg(errno));
        return -2;
    }

/* Get the current wfdiscs from disk */

    util_log(1, "loading stable wfdiscs");
    if ((int) (nrec = nrts_syswd(home, syscode, &array, 0, 0, NULL)) < 0) {
        util_log(1, "%s: nrts_syswd: %s", fid, syserrmsg(errno));
        return -3;
    }
    util_log(1, "%d wfdisc records read", nrec);

/* Load the lists with the current wfdiscs */

    nrts_inibwdlist(bwdfd, sys);
    for (i = 0; i < (int) nrec; i++) {
        if ((header.standx = nrts_standx(sys, array[i].sta)) < 0) {
            util_log(1, "unexpected station code '%s'", array[i].sta);
            return -4;
        }
        sta = sys->sta + header.standx;
        if ((header.chnndx = nrts_chnndx(sta, array[i].chan)) < 0) {
            util_log(1, "unexpected `%s:%s' station:channel pair",
                array[i].sta, array[i].chan
            );
            return -5;
        }
        header.wfdisc = array[i];
        if (nrts_bwd(&header, NRTS_CREATE) != 0) {
            util_log(1, "%s: nrts_bwd: %s", fid, syserrmsg(errno));
            return -6;
        }
    }
    if (nrec) free(array);

    return 0;
}

int _unlock_bwd(status)
int status;
{
static char *fid = "_unlock_bwd";

    if (util_unlock(bwdfd, 0, SEEK_SET, 0) == -1) {
        util_log(1, "%s: util_unlock: %s", fid, syserrmsg(errno));
        util_log(1, "%s: prior status was `%d'", status);
        util_log(1, "%s: new (error) status is `%d'", (status = -1));
    }
    util_log(2, "DEBUG: UN-locked bwd file (in %s)", fid);

    return status;
}

int nrts_wrtbwd()
{
int nrec = 0;
size_t len;
int i, j;
int yr, da, hr, mn, sc, ms;
struct wdlist *crnt;
static char *fid = "nrts_wrtbwd";

    if (bwdfd < 0 || sys == NULL) {
        util_log(1, "%s: FATAL ERROR: called prior to init!", fid);
        nrts_die(1);
    }

/*  Lock the bwd file, if possible.  If not, then we are done here.  */

    if (util_wlock(bwdfd, 0, SEEK_SET, 0) == -1) {
        if (errno == EACCES) {
            util_log(1, "bwd file is locked... wfdiscs not updated");
            return 0;
        } else {
            util_log(1, "%s: util_wlock: %s", fid, syserrmsg(errno));
            return -1;
        }
    }
    util_log(2, "DEBUG: locked bwd file");

    if (ftruncate(bwdfd, 0) != 0) {
        util_log(1, "%s: ftruncate: %s", fid, syserrmsg(errno));
        return _unlock_bwd(-2);
    }

    if (lseek(bwdfd, 0, SEEK_SET) != 0) {
        util_log(1, "%s: lseek: %s", fid, syserrmsg(errno));
        return _unlock_bwd(-3);
    }

/*  Write out the information  */

    len  = sizeof(struct wfdisc);
    for (nrec = 0, i = 0; i < sys->nsta; i++) {
        for (j = 0; j < sys->sta[i].nchn; j++) {
            crnt = list_head[i][j].next;
            while (crnt != NULL) {
                util_tsplit(crnt->wfdisc.time,&yr,&da,&hr,&mn,&sc,&ms);
                crnt->wfdisc.jdate = (yr * 1000) + da;
                if (write(bwdfd, &crnt->wfdisc, len) != (int) len) {
                    util_log(1, "%s: write: %s", fid, syserrmsg(errno));
                    return _unlock_bwd(-4);
                }
                ++nrec;
                crnt = crnt->next;
            }
        }
    }

/* Unlock the bwd file */

    if (util_unlock(bwdfd, 0, SEEK_SET, 0) == -1) {
        util_log(1, "%s: util_unlock: %s", fid, syserrmsg(errno));
        return -5;
    }
    util_log(2, "DEBUG: UN-locked bwd file (in %s)", fid);

    sys->bwd = NRTS_STABLE;

    return nrec;
}

int nrts_bwd(hd, action)
struct nrts_header *hd;
int action;
{
double expected;
float diff;
struct nrts_chn *chn;
struct wdlist *crnt, *new;
static char *fid = "nrts_bwd";

    if (sys->type == NRTS_ASL) return 0;
    if (hd->chnndx == nrts_chnndx(sys->sta, NRTS_LOGNAME)) return 0;

    chn = sys->sta[hd->standx].chn + hd->chnndx;

    switch (action) {

      case NRTS_EXTEND:

    /* Extend the range of the last wfdisc record in the list */

        if ((crnt = list_head[hd->standx][hd->chnndx].next) == NULL) {
            return -1;
        }

        while (crnt->next != NULL) crnt = crnt->next;

        crnt->wfdisc.nsamp  += hd->wfdisc.nsamp;
        chn->end = crnt->wfdisc.endtime = hd->wfdisc.endtime;

        sys->bwd = NRTS_BUSY1;
        return 0;

      case NRTS_REDUCE:

    /* Reduce the start time and nsamp of the first wfdisc record in the list */
    /* This assumes that all records have CONSTANT nsamp!                     */

        if ((crnt = list_head[hd->standx][hd->chnndx].next) == NULL) {
            return -2;
        }

        crnt->wfdisc.nsamp -= hd->wfdisc.nsamp;
        if (crnt->wfdisc.nsamp < 0) {
            util_log(1, "illegal wfdisc dropped (%d samples after reduction)",
                crnt->wfdisc.nsamp
            );
            util_log(1, "%s", wdtoa(&crnt->wfdisc));
        }
        if (crnt->wfdisc.nsamp <= 0) {
            list_head[hd->standx][hd->chnndx].next = crnt->next;
            free(crnt);
            --chn->nseg;
            if ((crnt = list_head[hd->standx][hd->chnndx].next) == NULL) {
                chn->beg = (double) -1;
                sys->bwd = NRTS_BUSY1;
                return 0;
            }
        } else {
            crnt->wfdisc.foff += (hd->wfdisc.nsamp * hd->wrdsiz);
            crnt->wfdisc.time += (hd->wfdisc.nsamp / crnt->wfdisc.smprate);
            chn->beg = crnt->wfdisc.time;
        }

        sys->bwd = NRTS_BUSY1;
        return 0;

      case NRTS_CREATE:

    /* Create a new wfdisc record and append to the end of the list */

        crnt = &list_head[hd->standx][hd->chnndx];
        if (crnt->next == NULL) {
            chn->beg = hd->wfdisc.time;
        } else {
            while (crnt->next != NULL) crnt = crnt->next;
        }

        if ((new = (struct wdlist *) malloc(sizeof(struct wdlist))) == NULL) {
            util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
            return -4;
        }
        new->wfdisc = hd->wfdisc;
        new->next   = NULL;
        crnt->next  = new;
        chn->end    = new->wfdisc.endtime;

        ++chn->nseg;
        sys->bwd = NRTS_BUSY1;
        return 0;

      case NRTS_TESTWD:

        if ((crnt = list_head[hd->standx][hd->chnndx].next) == NULL) {
            util_log(2, "no pre-existing wfdisc records for %s:%s",
                hd->wfdisc.sta, hd->wfdisc.chan
            );
            util_log(2, "creating new %s:%s wfdisc record",
                hd->wfdisc.sta, hd->wfdisc.chan
            );
            return nrts_bwd(hd, NRTS_CREATE);
        }

        while (crnt->next != NULL) crnt = crnt->next;

        util_log(2, "testing %s:%s wfdisc for continuity",
            hd->wfdisc.sta, hd->wfdisc.chan
        );

        if (crnt->wfdisc.smprate != hd->wfdisc.smprate) {
            util_log(1, "%s:%s sample rate change (%.2f to %.2f",
                hd->wfdisc.sta, hd->wfdisc.chan,
                crnt->wfdisc.smprate, hd->wfdisc.smprate
            );
            util_log(1, "creating new %s:%s wfdisc record",
                hd->wfdisc.sta, hd->wfdisc.chan
            );
            return nrts_bwd(hd, NRTS_CREATE);
        }

        expected = crnt->wfdisc.endtime + (1.0/crnt->wfdisc.smprate);
        diff     = (float) ((long) (1000.0*(hd->wfdisc.time - expected))/1000);

        if (diff == 0.0) {
            util_log(2, "extending existing %s:%s wfdisc record",
                hd->wfdisc.sta, hd->wfdisc.chan
            );
            return nrts_bwd(hd, NRTS_EXTEND);
        } else {
            util_log(2, "prev end     = %s", 
                util_dttostr(crnt->wfdisc.endtime, 0)
            );
            util_log(2, "crnt beg     = %s",
                util_dttostr(hd->wfdisc.time, 0)
            );
            util_log(2, "expected beg = %s",
                util_dttostr(expected, 0)
            );
            util_log(1, "%.3f second tear, creating new %s:%s wfdisc record",
                diff, hd->wfdisc.sta, hd->wfdisc.chan
            );
            return nrts_bwd(hd, NRTS_CREATE);
        }

      case NRTS_NOP:

        return 0;

      default:

        util_log(1, "%s: unexpected action flag `%d'", fid, action);
        return -5;
    }
}

void nrts_closebwd()
{
    if (bwdfd != -1) close(bwdfd);
}

/* Revision History
 *
 * $Log: oldbwd.c,v $
 * Revision 1.1  2005/07/25 23:40:25  dechavez
 * created from previous bwd.c (MT-unsafe bwd stuff)
 *
 */
