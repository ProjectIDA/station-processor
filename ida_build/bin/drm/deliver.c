#pragma ident "$Id: deliver.c,v 1.3 2004/04/26 20:50:54 dechavez Exp $"
/*======================================================================
 *
 *  Deliver data.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include "service.h"

static int merged = 0;

void set_merged_flag()
{
    merged = 1;
}

int deliver(cnf, datreq)
struct drm_cnf *cnf;
struct drm_datreq *datreq;
{
int status;
struct drm_names *name;
static char dest[MAXPATHLEN+1];
static char *fid = "deliver";

    switch (datreq->remote.agent) {
      case DRM_RCP: 
        util_log(1, "delivering data via rcp");
        status = drm_rcp(datreq);
        break;
      case DRM_FTP: 
        util_log(1, "delivering data via ftp");
        status = drm_ftp(datreq, merged);
        break;
      case DRM_EMAIL:
        util_log(1, "delivering data via email");
        status = drm_email(cnf, datreq);
        break;
      case DRM_WILLCALL:
        status = DRM_DATAREADY;
        break;
      default:
        util_log(1, "%s: fatal error: unknown delivery agent `%d'",
            fid, datreq->remote.agent
        );
        drm_exit(DRM_BAD);
    }

    name = drm_names();

    if (chdir(name->dir.home) != 0) {
        util_log(1, "%s: fatal error: can't chdir: %s: %s", 
            fid, name->dir.home, syserrmsg(errno)
        );
        drm_exit(DRM_BAD);
    }

    if (!merged && status != DRM_DELIVERED) {
        util_log(1, "moving data to anonymous ftp pickup directory");
        sprintf(dest, "%s/%s", name->dir.pickup, name->request);
        if (util_move(name->dir.work, dest) != 0) {
            util_log(1, "%s: fatal error: can't move %s -> %s: %s",
                fid, name->dir.work, dest, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
    } else {
        util_log(1, "remove working directory %s", name->dir.work);
        if (rmdir(name->dir.work) != 0) {
            util_log(1, "%s: fatal error: can't rmdir: %s: %s", 
                fid, name->dir.work, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
    }

    return status;
}

/* Revision History
 *
 * $Log: deliver.c,v $
 * Revision 1.3  2004/04/26 20:50:54  dechavez
 * include name of working directory being removed in log message
 *
 * Revision 1.2  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
