#pragma ident "$Id: email.c,v 1.2 2004/01/23 22:23:13 dechavez Exp $"
/*======================================================================
 *
 *  Deliver data via email.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "service.h"

BOOL AsciiOutputFormat(int format)
{
    switch (format) {
      case DRM_CM6:
      case DRM_SACASCII:
      case DRM_NATIVE:
        return TRUE;
      default:
        return FALSE;
    }
}

int drm_email(struct drm_cnf *cnf, struct drm_datreq *datreq)
{
int count, status, forceftp;
DIR *dirp;
char *message;
struct dirent *dp;
struct stat sbuf;
struct drm_names *name;
char subj[512];
static char *fid = "drm_email";

    name = drm_names();

/*  Only ascii data formats can be delivered via email  */

    if (!AsciiOutputFormat(datreq->format)) {
        util_log(1, "%s: fatal error: cannot email binary data", fid);
        return DRM_FAIL;
    }

/* Set the subject line */

    if (datreq->type == DRM_AUTODRM) {
        sprintf(subj, "re: AutoDRM request `%s' ", datreq->ident);
    } else {
        sprintf(subj, "re: IDADRM request `%s' ", datreq->ident);
    }
    sprintf(subj+strlen(subj), "(%d/%d)", datreq->finfo.set, datreq->nset);

/* Make sure file is not too large */

    if (stat(name->path.message, &sbuf) != 0) {
        util_log(1, "%s: error: can't stat %s: %s'",
            fid, name->path.message, syserrmsg(errno)
        );
        return DRM_FAIL;
    }

    if (S_ISREG(sbuf.st_mode)) {
        if (sbuf.st_size > cnf->maxemail) {
            util_log(1, "file size (%ld) is too large for email", sbuf.st_size);
            util_log(1, "default to anonymous ftp pickup");
            message = name->msg.forceftp;
            forceftp = 1;
        } else {
            message = name->path.message;
            forceftp = 0;
        }
        status = util_email(datreq->email, subj, message);
        if (status != 0) {
            util_log(1, "%s: util_email failed, status %d", fid,status);
            return DRM_FAIL;
        }
        if (!forceftp && unlink(name->path.message) != 0) {
            util_log(1, "%s: warning: can't unlink %s: %s",
                fid, name->path.message, syserrmsg(errno)
            );
        }
    } else {
        util_log(1, "%s: error: `./%s' is not a regular file!",
            fid, name->path.message
        );
        return DRM_FAIL;
    }

    return forceftp ? DRM_DATAREADY : DRM_DELIVERED;
}

/* Revision History
 *
 * $Log: email.c,v $
 * Revision 1.2  2004/01/23 22:23:13  dechavez
 * added AsciiOutputFormat() and use it to test for output type
 * instead of explicit compares
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
