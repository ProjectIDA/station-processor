#pragma ident "$Id: rdfile.c,v 1.2 2003/04/24 00:10:37 dechavez Exp $"
/*======================================================================
 *
 *  Read a file.  The name is assumed to identify the type of file.
 *  The calling process must have already locked this file.
 *
 *  The return value is one of:

 *  DRM_RAW
 *  File is raw, straight from the mailer.  The output points to a
 *  drm_rawreq structure.
 *  
 *  DRM_DATASET
 *  File holds request parameters for one data set.  The ouput points
 *  to a drm_datreq structure;
 *  
 *  DRM_OK
 *  The file name is of a recognized type, but the file is absent.  This
 *  is not a problem since another process could have deleted it.  The
 *  output contents are undefined.
 *  
 *  DRM_BAD
 *  The file type cannot be determined from the name.  This should never
 *  happen!  The output contents are undefined.
 *
 *====================================================================*/
#include <errno.h>
#include <sys/param.h>
#include "drm.h"

int drm_rdfile(request, ptrptr, cnf, buffer)
char *request;
void ** ptrptr;
struct drm_cnf *cnf;
char *buffer;
{
FILE *fp;
int status, spam, errors = 0;
char path[MAXPATHLEN+1];
struct drm_finfo *finfo;
struct drm_names *name;
static struct drm_rawreq rawreq;
static struct drm_datreq datreq;
static char *fid = "drm_rdfile";

    if ((finfo = drm_fparse(request, buffer)) == NULL) return DRM_BAD;

    if (finfo->set < 0) {
        util_log(1, "%s: fatal error: unknown file `%s'", fid, request);
        drm_exit(DRM_BAD);
    }

    name = drm_names();

    if ((fp = fopen(name->path.queue, "r")) == NULL) {
        if (errno == ENOENT) {
            return DRM_OK;
        } else {
            util_log(1, "%s: fatal error: fopen: %s: %s",
                fid, name->path.queue, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
    }

/*  Data set files were prepared by me... just read and pass back  */

    if (finfo->set > 0) {
        *ptrptr = (void *) &datreq;
        drm_rddatreq(fp, &datreq);
        fclose(fp);
        return DRM_DATASET;
    }

/*  Initialize message file documenting possible request errors  */

    if (util_cat(name->canned.badreq, name->msg.badreq, NULL) != 0) {
        util_log(1, "%s: fatal error: util_cat(%s, %s, NULL): %s",
            fid, name->canned.badreq, name->msg.badreq, syserrmsg(errno)
        );
        drm_exit(DRM_BAD);
    }

/*  Invoke request format specific reader  */

    rawreq.finfo = *finfo;
    *ptrptr = (void *) &rawreq;
    if (finfo->type == DRM_AUTODRM) {
        if ((errors = drm_rdautodrm(fp, finfo, &rawreq, cnf, buffer, &spam)) != 0) {
            if (spam) {
                return DRM_SPAM;
            } else if (errors > 0) {
                util_log(1, "request contains errors");
            } else {
                util_log(1, "%s: drm_rdautodrm failed, status %d", fid, errors);
            }
        }
    } else {
        util_log(1, "%s: fatal error: unknown request file `%s'", fid, request);
        drm_exit(DRM_BAD);
    }

    fclose(fp);

/*  If file was read without errors, then pass it back  */

    if (errors == 0) return DRM_RAW;

/*  Otherwise, set things so we can bounce it  */

    rawreq.finfo = *finfo;
    if (rawreq.email == NULL) rawreq.email = cnf->admin;
    *ptrptr = (void *) &rawreq;

    return DRM_BAD;
}

/* Revision History
 *
 * $Log: rdfile.c,v $
 * Revision 1.2  2003/04/24 00:10:37  dechavez
 * Anti-spam support introduced
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
