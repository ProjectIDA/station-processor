#pragma ident "$Id: service.c,v 1.2 2004/01/23 22:21:31 dechavez Exp $"
/*======================================================================
 *
 *  Service a data request.
 *
 *====================================================================*/
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "service.h"
#include "xfer.h"

int service(datreq, cnf, first, buffer)
struct drm_datreq *datreq;
struct drm_cnf *cnf;
int first;
char *buffer;
{
FILE *fp;
int i, status;
long count = 0;
size_t len;
char *server;
struct drm_names *name;
double wait;
struct stat sbuf;
static char *idstring[DRM_NTYPE] = {
    DRM_IDADRM_STRING, DRM_SPYDER_STRING, DRM_AUTODRM_STRING
};
struct drm_sta *stainfo;
static char *fid = "service";

    name = drm_names();
    set_datreq(datreq);

    if (datreq->type == DRM_SPYDER) {
        util_log(first ? 1 : 2, "SPYDER request %s (%d/%d)",
            datreq->ident, datreq->finfo.set, datreq->nset
        );
    } else if (datreq->type == DRM_AUTODRM) {
        util_log(first ? 1 : 2, "AutoDRM request from %s (%d/%d)",
            datreq->email, datreq->finfo.set, datreq->nset
        );
    } else {
        util_log(first ? 1 : 2, "IDADRM request from %s (%d/%d)",
            datreq->email, datreq->finfo.set, datreq->nset
        );
    }
    util_log(first ? 1 : 2, "START  %s", util_dttostr(datreq->beg, 0));
    util_log(first ? 1 : 2, "STOP   %s", util_dttostr(datreq->end, 0));
    if (datreq->allchan) {
        util_log(first ? 1 : 2, "WAVEF  %s: all channels", datreq->sname);
    } else {
        util_log(first ? 1 : 2, "WAVEF  %s: %s", datreq->sname, datreq->chnlst);
    }

    if (datreq->sname == NULL) {
        util_log(1, "%s: fatal error: NULL sname!", fid);
        drm_exit(DRM_BAD);
    }

    if (datreq->sc == NULL) {
        if (datreq->type == DRM_SPYDER) {
            util_log(1, "no configured channels requested");
            return DRM_OK;
        } else {
            util_log(1, "%s: fatal error: NULL chnlst!", fid);
            drm_exit(DRM_BAD);
        }
    }

/*  Get server list and connection parameters for this station  */

    stainfo = (struct drm_sta *) NULL; 
    for (i = 0; i < cnf->nsta && stainfo == (struct drm_sta *) NULL; i++) {
        if (strcmp(cnf->sta[i].name, datreq->sname) == 0) {
            stainfo = cnf->sta + i;
        }
    }

    if (stainfo == (struct drm_sta *) NULL) {
        sprintf(buffer, "station `%s' not in DRM configuration table",
            datreq->sname
        );
        util_log(1, "%s", buffer);
        if (datreq->type != DRM_SPYDER) {
            drm_ack(datreq->email, DRM_NODATA);
            util_log(1, "send warning to %s", cnf->admin);
            if (util_email(cnf->admin, buffer, name->path.log) != 0) {
                util_log(1, "%s: fatal error: util_email failed", fid);
                return DRM_BAD;
            }
        }
        return DRM_OK;
    }

    stainfo->maxconn = stainfo->dialup ? cnf->maxconn2 : cnf->maxconn1;

/*  Create working directory  */

    if (util_mkpath(name->dir.work, 0755) != 0) {
        util_log(1, "%s: fatal error: util_mkpath: %s: %s",
            fid, name->dir.work, syserrmsg(errno)
        );
        drm_exit(DRM_BAD);
    }

/*  Create canned responses appropriate for the interface  */

    if (datreq->type == DRM_AUTODRM) {
        if ((status = canned_autodrm(cnf, datreq)) != 0) {
            util_log(1, "%s: fatal error: canned_autodrm() failed, status `%d'",
                fid, status
            );
            return DRM_BAD;
        }
    } else if (datreq->type == DRM_IDADRM) {
        if ((status = canned_idadrm(cnf, datreq)) != 0) {
            util_log(1, "%s: fatal error: canned_idadrm() failed, status `%d'",
                fid, status
            );
            return DRM_BAD;
        }
    }

/* Reject anybody on the abusers list */

    if (abuser(datreq->email)) {
        drm_ack(datreq->email, DRM_NOPERM);
        if (rmdir(name->dir.work) != 0) {
            util_log(1, "%s: warning: can't rmdir: %s: %s", 
                fid, name->dir.work, syserrmsg(errno)
            );
        }
        return DRM_OK;
    }

/* Dialup requestors must be on the authorized users list */

    if (stainfo->dialup && !authorized(datreq->email)) {
        drm_ack(datreq->email, DRM_NOPERM);
        if (rmdir(name->dir.work) != 0) {
            util_log(1, "%s: warning: can't rmdir: %s: %s", 
                fid, name->dir.work, syserrmsg(errno)
            );
        }
        return DRM_OK;
    }

/*  See if data are available anywhere  */

    util_log(first ? 1 : 2, "checking data availability");
    status = drm_avail(datreq, &server, buffer, cnf, &wait, stainfo);
    util_log(2, "drm_avail returns status %d", status);

    if (
        status == DRM_NOTCONFIGURED ||
        status == DRM_NODATA        ||
        status == DRM_NOCHAN        ||
        status == DRM_IGNORE        ||
        status == DRM_EARLY
    ) {
        util_log(2, "remove working directory");
        if (rmdir(name->dir.work) != 0) {
            util_log(1, "%s: warning: can't rmdir: %s: %s", 
                fid, name->dir.work, syserrmsg(errno)
            );
        }
    }
    
    if (status == DRM_NOCHAN) {
        if (datreq->type != DRM_SPYDER) drm_ack(datreq->email, DRM_NOCHAN);
        return DRM_OK;
    }
    
    if (status == DRM_NODATA) {
        if (datreq->type != DRM_SPYDER) drm_ack(datreq->email, DRM_NODATA);
        return DRM_OK;
    }
    
    if (status == DRM_IGNORE) {
        if (datreq->type != DRM_SPYDER) drm_ack(datreq->email, DRM_NODATA);
        return DRM_IGNORE;
    }

    if (status == DRM_NOTYET) {
        if (datreq->type != DRM_SPYDER && wait > cnf->waitnotify) {
            drm_ack(datreq->email, DRM_NOTYET);
        }
        return DRM_TRYLATER;
    }

    if (status == DRM_DONTKNOW || status == DRM_DEFER) {
        if (datreq->type != DRM_SPYDER) {
            if (stat(name->flag.queued, &sbuf) == 0) {
                util_log(2, "%s exists... no message sent", name->flag.queued);
            } else {
                if ((fp = fopen(name->flag.queued, "w")) == NULL) {
                    util_log(1, "%s: fopen: %s: %s",
                        fid, name->flag.queued, syserrmsg(errno)
                    );
                    drm_exit(DRM_BAD);
                }
                drm_ack(datreq->email, DRM_TRYLATER);
            }
        }
        return status == DRM_DONTKNOW ? DRM_TRYLATER : DRM_DEFER;
    }

    if (status == DRM_EARLY) {
        if (datreq->type != DRM_SPYDER) drm_ack(datreq->email, DRM_EARLY);
        return DRM_OK;
    }

    if (status != DRM_DATAREADY) {
        util_log(1, "%s: fatal error: drm_avail returns %d", fid, status);
        return DRM_BAD;
    }

/* At this point data are (or have been) available from `server' */

    if (datreq->format != DRM_NATIVE) {
        status = xferdata(name->path.raw, server, datreq, stainfo, status);
        if (status != DRM_OK) return status;
        util_log(2, "xferdata returns OK, datreq->nrec = %d", datreq->nrec);
    } else {
        status = isidata(name->path.raw, server, datreq, stainfo, status);
        if (status != DRM_OK) return status;
    }

    if (datreq->nrec == 0) {
        util_log(1, "no requested data received");
        if (datreq->type != DRM_SPYDER) drm_ack(datreq->email, DRM_NODATA);
        util_log(1, "removing working directory");
        if (unlink(name->path.raw) != 0) {
            util_log(1, "%s: fatal error: unlink: %s: %s",
                fid, name->path.raw, syserrmsg(errno)
            );
            return DRM_BAD;
        }
        if (rmdir(name->dir.work) != 0) {
            util_log(1, "%s: fatal error: can't rmdir: %s: %s", 
                fid, name->dir.work, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
        return DRM_OK;
    }

/*  Convert to desired output format  */

    if (chdir(name->dir.work) != 0) {
        util_log(1, "%s: fatal error: chdir: %s: %s", 
            fid, name->dir.work, syserrmsg(errno)
        );
        return DRM_BAD;
    }

    convert(name->path.raw, datreq);

/*  Deliver data  */

    status = deliver(cnf, datreq);

    if (datreq->remote.agent == DRM_EMAIL) {
        if (status == DRM_DELIVERED || status == DRM_DATAREADY) return DRM_OK;
    } else if (datreq->type != DRM_SPYDER) {
        drm_ack(datreq->email, status);
    } else if (status != DRM_DELIVERED) {
        sprintf(buffer, "SPYDER delivery failed");
        util_log(1, "send warning to %s", cnf->admin);
        if (util_email(cnf->admin, buffer, name->path.log) != 0) {
            util_log(1, "%s: fatal error: util_email failed", fid);
            drm_exit(DRM_BAD);
        }
    }

    if (
        status != DRM_DELIVERED &&
        status != DRM_REMOTEFTP &&
        status != DRM_DATAREADY
    ) drm_exit(status);

    return DRM_OK;
}

/* Revision History
 *
 * $Log: service.c,v $
 * Revision 1.2  2004/01/23 22:21:31  dechavez
 * run DRM_NATIVE format requests through isidata()
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
