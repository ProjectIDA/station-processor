#pragma ident "$Id: avail.c,v 1.3 2004/01/29 19:14:40 dechavez Exp $"
/*======================================================================
 *
 *  Determines if and where requested data are available.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "drm.h"
#include "service.h"
#include "util.h"
#include "xfer.h"

#define MAXTOKENS   64

static int i, j, k, ntoken;
static double granularity = -1.0;
static char *token[MAXTOKENS];
static struct drm_names *name;

/* chantest -  test a specific NRTS system for desired channels
 * 
 * return values:
 * 
 * 0 if none of the requested channels are present
 * 1 if any  of the requested channels are present
 *
 * We also determine the minimum granularity of data packets for all the
 * requested channels.  Note that by saving the *minimum* granularity,
 * this can result in a request being considered satisfied before all
 * data in fact have been delivered (eg. if requesting bh? and vh? you
 * can get bh before vh data have been written to the disk loop).  This
 * is preferred to waiting for the extra time to get the vh data before
 * delivering the bh.  The user could alway make two requests (one for
 * bh and one for vh) if this presented a problem.  The correct thing to
 * do would probably be to split the request along sample rate boundaries...
 *
 */

static int chantest(char *node, struct drm_datreq *datreq, char *buffer)
{
double reclen;
struct drm_info *info;
static char *fid = "chantest";

    if ((info = getinfo(node, buffer)) == (struct drm_info *) NULL) {
        util_log(1, "NULL getinfo!  Configuration file out of date?");
        drm_exit(DRM_BAD);
    }

    for (i = 0; i < info->nsta; i++) {
        if (strcmp(info->sta[i].name, datreq->sname) == 0) {
            strcpy(buffer, datreq->chnlst);
            ntoken = util_sparse(buffer, token, " :,;.|/", MAXTOKENS);
            for (j = 0; j < info->sta[i].nchn; j++) {
                for (k = 0; k < ntoken; k++) {
                    if (strcmp(info->sta[i].chn[j].name, token[k]) == 0) {
                        reclen = (double) info->sta[i].chn[j].reclen;
                        if (granularity < 0.0 || granularity > reclen) {
                            granularity = reclen;
                        }
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

/* datatest -  test a specific NRTS system for desired data
 * 
 * return values:
 * 
 * DRM_DATAREADY if *all* data for *any* of the desired channels are ready 
 * 
 * DRM_NODATA    if no data for all channels are ready (could be that the
 *               data have been overwritten or the requested channels are
 *               not configured into the NRTS
 * 
 * DRM_NOTYET    if any data for *any* requested channel are present but not
 *               complete (window end time is younger than disk loop end)
 *
 */

static int datatest(struct xfer_cnfnrts *remote, struct drm_datreq *datreq, char *buffer)
{
int toolate = 0;
int nready  = 0;
int notyet  = 0;
int ntoken  = 0;
struct xfer_nrtschncnf *diskloop;
static char *fid = "datatest";

    for (i = 0; i < remote->nsta; i++) {
        if (strcmp(remote->sta[i].name, datreq->sname) == 0) {
            strcpy(buffer, datreq->chnlst);
            ntoken = util_sparse(buffer, token, " :,;.|/", MAXTOKENS);
            for (j = 0; j < remote->sta[i].nchn; j++) {
                diskloop = remote->sta[i].chn + j;
                for (k = 0; k < ntoken; k++) {
                    if (strcmp(diskloop->name, token[k]) == 0) {
                        if (
                            (datreq->end <= diskloop->end) &&
                            (datreq->end >= diskloop->beg)
                        ) {
                            ++nready;
                        } else if (datreq->end < diskloop->beg) {
                            ++toolate;
                        } else if (datreq->beg > diskloop->end) {
                            ++notyet;
                        } else if (datreq->end > diskloop->end) {
                            ++notyet;
                        } else if (
                            (datreq->end >  diskloop->end) &&
                            (datreq->beg <= diskloop->end)
                        ) {
                            ++notyet;
                        } else {
                            util_log(1, "%s: unexpected situation", fid);
                            util_log(1, "%s:%s parameters follow",
                                datreq->sname, token[k]
                            );
                            util_log(1, "request  beg = %s",
                                util_dttostr(datreq->beg, 0)
                            );
                            util_log(1, "request  end = %s",
                                util_dttostr(datreq->end, 0)
                            );
                            util_log(1, "diskloop beg = %s",
                                util_dttostr(diskloop->beg, 0)
                            );
                            util_log(1, "diskloop end = %s",
                                util_dttostr(diskloop->end, 0)
                            );
                            drm_exit(DRM_BAD);
                        } 
                    }
                }
            }
            if (nready != 0) return DRM_DATAREADY;
            if (notyet != 0) return DRM_NOTYET;
            if (toolate == ntoken) return DRM_NODATA;
            util_log(1, "%s: program logic error", fid);
            drm_exit(DRM_BAD);
        }
    }

    return DRM_NODATA;
}

int drm_avail(struct drm_datreq *datreq, char **where, char *buffer, struct drm_cnf *cnf, double *wait, struct drm_sta *entry)
{
FILE *fp;
int i, status, have_chan, have_data;
double now, yngest, oldest;
struct xfer_cnfnrts *remote;
struct stat sbuf;
static char *fid = "drm_avail";

    cnf  = drm_cnf();
    name = drm_names();

/*  Make sure we don't have this station already flagged */

    if (entry->ignore) {
        util_log(1, "station `%s' ignore flag set", entry->name);
        return DRM_IGNORE;
    }
    if (entry->defer) {
        util_log(1, "station `%s' defer flag set", entry->name);
        return DRM_DEFER;
    }

/*  Make sure station supports requested channels  */

    have_chan = 0;
    for (i = 0; i < entry->nhop; i++) {
        if (chantest(entry->node[i], datreq, buffer)) have_chan = 1;
    }
    if (!have_chan) {
        util_log(1, "requested channels are not acquired by NRTS");
        return DRM_NOCHAN;
    }

/*  Only accept data which are within the accepted window.  This
 *  assumes this computer's system clock is more or less correct.
 *  We will wait until the clock time is twice the granularity past
 *  the desired end time, in case the system clock is slightly off.
 */

    now = (double) time((time_t *) NULL);
    oldest = now - (cnf->maxpast);
    yngest = now + (cnf->maxhold);
    *wait  = (datreq->end - now) + 2*granularity;

    if (datreq->end < oldest) {
        util_log(1, "requested window too old (maxpast = %d)",
            cnf->maxpast / 86400
        );
        return DRM_NODATA;
    }

    if (datreq->end > yngest) {
        util_log(1, "requested window too far in the future (maxhold = %d)",
            cnf->maxhold / 86400
        );
        return DRM_EARLY;
    }

    if (*wait > 0.0) {
        util_log(1, "data have yet to be acquired (wait = %.3lf)", wait);
        return DRM_NOTYET;
    }

/* See if we've got some data for this request already read */

    name = drm_names();
    if ((fp = fopen(name->path.raw, "r")) != (FILE *) NULL) {
        fclose(fp);
        have_data = 1;
    } else {
        have_data = 0;
    }

/*  Determine which system (if any) has the data  */

    for (i = 0; i < entry->nhop; i++) {
        if (chantest(entry->node[i], datreq, buffer)) {
            remote = get_config(entry->node[i],entry->to,entry->tto,&status);
            if (remote != NULL) {
                *where = entry->node[i];
                status = datatest(remote, datreq, buffer);
                if (status == DRM_DATAREADY) return status;
                if (i < entry->nhop - 1) {
                    if (status == DRM_NODATA) {
                        util_log(1, "requested data not present on this node");
                    } else if (status == DRM_NOTYET) {
                        util_log(1, "data not yet complete on this node");
                    } else {
                        util_log(1, "%s: unexepected status %d", fid, status);
                        drm_exit(DRM_BAD);
                    }
                }
            } else {
                status = DRM_DONTKNOW;
                if (i < entry->nhop - 1) {
                    util_log(1, "unable to contact node (phone busy?)");
                }
            }
        }
    }

    if (status == DRM_NOCHAN) {
        if (have_data) return DRM_DONE;
        util_log(1, "requested channels are unavailable");
    } else if (status == DRM_NOTYET) {
        if (have_data) return DRM_DONE;
        util_log(1, "Data not available anywhere... ARS/NRTS problems?");
        sprintf(buffer, "%s/%s", name->dir.flags, datreq->sname);
        if (stat(buffer, &sbuf) == 0) {
            util_log(1, "flag `%s' exists, no warning sent", buffer);
        } else {
            if ((fp = fopen(buffer, "w")) == NULL) {
                util_log(1, "%s: fopen: %s: %s", fid, buffer, syserrmsg(errno));
                util_log(1, "abort: can't set warning flag");
                drm_exit(DRM_BAD);
            }
            fclose(fp);
            util_log(1, "created `%s' flag", buffer);
            util_email(cnf->admin, "possible ARS/NRTS problem", name->path.log);
            util_log(1, "warning message sent to operator");
        }
        status = DRM_NODATA;
    }

    return status;
}

/* Revision History
 *
 * $Log: avail.c,v $
 * Revision 1.3  2004/01/29 19:14:40  dechavez
 * print wait interval for DRM_NOTYET returns
 *
 * Revision 1.2  2003/11/21 20:21:35  dechavez
 * ansi function declaration
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
