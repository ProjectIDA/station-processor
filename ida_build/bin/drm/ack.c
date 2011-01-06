#pragma ident "$Id: ack.c,v 1.1.1.1 2000/02/08 20:19:58 dec Exp $"
/*======================================================================
 *
 *  Email acknowledgements
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "drm.h"

static char *type = NULL;

void drm_ack_init(code)
int code;
{
static char *fid = "drm_ack_init";

    switch (code) {
      case DRM_AUTODRM: type = "AutoDRM"; break;
      case DRM_SPYDER:  type = "SPYDER";  break;
      case DRM_IDADRM:  type = "IDADRM";  break;
      default:
        util_log(1, "%s: illegal type code `%d'", fid, code);
        drm_exit(DRM_BAD);
   }

    return;
}

void drm_ack(address, code)
char *address;
int code;
{
char *logstr;
char *message = NULL;
struct drm_cnf   *cnf;
struct drm_names *name;
static char subject[128];
static char *fid = "drm_ack";

    if (type == NULL) {
        util_log(1, "%s: never initialized!", fid);
        drm_exit(DRM_BAD);
    }

    name = drm_names();
    cnf  = drm_cnf();

    switch (code) {
      case DRM_BADREQ:
        sprintf(subject, "errors in your %s request", type);
        message = name->msg.badreq;
        logstr  = "sending bad request message to %s";
        break;
      case DRM_DELIVERED:
        sprintf(subject, "%s request satisfied", type);
        message = name->msg.delivered;
        logstr  = "sending successful delivery message to %s";
        break;
      case DRM_FAIL:
        sprintf(subject, "%s request failed", type);
        message = name->msg.fail;
        logstr  = "sending request failed message to %s";
        util_email(cnf->admin, subject, name->path.log);
        break;
      case DRM_NODATA:
        sprintf(subject, "%s data unavailable", type);
        message = name->msg.nodata;
        logstr  = "sending no data message to %s";
        break;
      case DRM_DATAREADY:
        sprintf(subject, "%s data set ready", type);
        message = name->msg.dataready;
        logstr  = "sending data set ready message to %s";
        break;
      case DRM_RCPFAIL:
        sprintf(subject, "%s rcp failed", type);
        message = name->msg.rcpfail;
        logstr  = "sending rcp failed message to %s";
        util_email(cnf->admin, subject, name->path.log);
        break;
      case DRM_FTPFAIL:
        sprintf(subject, "%s ftp failed", type);
        message = name->msg.ftpfail;
        logstr  = "sending ftp failed message to %s";
        util_email(cnf->admin, subject, name->path.log);
        break;
      case DRM_NOCHAN:
        sprintf(subject, "%s data unavailable", type);
        message = name->msg.nochan;
        logstr  = "sending no channel message to %s";
        break;
      case DRM_NOPERM:
        sprintf(subject, "%s permission denied", type);
        message = name->msg.noperm;
        logstr  = "sending permission denied message to %s";
        break;
      case DRM_TRYLATER:
        sprintf(subject, "%s request queued", type);
        message = name->msg.queued;
        logstr  = "sending request queued message to %s";
        break;
      case DRM_REMOTEFTP:
        sprintf(subject, "%s ftp failed", type);
        message = name->msg.remoteftp;
        logstr  = "sending remote ftp failed message to %s";
        util_email(cnf->admin, subject, name->path.log);
        break;
      case DRM_EARLY:
        sprintf(subject, "rejected %s request", type);
        message = name->msg.early;
        logstr  = "sending request rejection notice to %s";
        break;
      case DRM_NOTYET:
        sprintf(subject, "%s request on hold", type);
        message = name->msg.wait;
        logstr  = "sending pending data message to %s";
        break;
      case DRM_HELP:
        sprintf(subject, "%s User's Guide", type);
        message = name->canned.help;
        logstr  = "sending User's Guide to %s";
        break;
      case DRM_SLIST:
        sprintf(subject, "%s Station List", type);
        message = name->canned.slist;
        logstr  = "sending station list to %s";
        break;
      case DRM_CALIB:
        sprintf(subject, "%s response information", type);
        message = name->canned.calib;
        logstr  = "sending response information to %s";
        break;
      case DRM_DIVERTED:
        sprintf(subject, "diverted %s request", type);
        message = name->msg.badreq;
        logstr  = "diverting message to %s";
        break;
      default:
        util_log(1, "%s: program error 1, unknown code `%d'", fid, code);
        drm_exit(DRM_BAD);
    }

    util_log(1, logstr, address);

    if (address == NULL) {
        util_log(1, "%s: fatal error: null email address", fid);
        drm_exit(DRM_BAD);
    } else if (util_email(address, subject, message) != 0) {
        util_log(1, "%s: fatal error: util_email(%s, %s, %s): %s",
            fid, address, subject, message, syserrmsg(errno)
        );
        drm_exit(DRM_BAD);
    }

    set_delivered_flag();

    return;
}

/* Revision History
 *
 * $Log: ack.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
