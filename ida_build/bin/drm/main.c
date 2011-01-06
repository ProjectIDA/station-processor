#pragma ident "$Id: main.c,v 1.3 2003/12/10 06:30:31 dechavez Exp $"
/*======================================================================
 *
 *  Process a single IDADRM request.
 *
 *====================================================================*/
#include <errno.h>
#include "service.h"

int main(int argc, char **argv)
{
FILE *fp;
char *request;
int first, type, status;
void *ptr;
char buffer[DRM_BUFLEN];
struct drm_rawreq *rawreq;
struct drm_cnf *cnf;
struct drm_names *name;

    init(argc, argv, &request, &first, buffer);
    cnf = drm_cnf();
    name = drm_names();

    util_log(first ? 1 : 2, "%s processing started", request);

    type = drm_rdfile(request, &ptr, cnf, buffer);

    if (type == DRM_RAW) {
        rawreq = (struct drm_rawreq *) ptr;
        if (rawreq->aux.help)  drm_ack(rawreq->email, DRM_HELP);
        if (rawreq->aux.slist) drm_ack(rawreq->email, DRM_SLIST);
        if (rawreq->aux.calib) drm_ack(rawreq->email, DRM_CALIB);
        status = prepsets((struct drm_rawreq *) ptr, buffer);

    } else if (type == DRM_DATASET) {
        status = service((struct drm_datreq *) ptr, cnf, first, buffer);

    } else if (type == DRM_BAD) {
        rawreq = (struct drm_rawreq *) ptr;
        drm_ack(rawreq->email, DRM_BADREQ);
        status = DRM_OK;

    } else if (type == DRM_SPAM) {
        status = DRM_OK;

    } else if (type != DRM_OK) {
        util_log(1, "abort: unknown file type `%d'", type);
        drm_exit(DRM_BAD);
    }

    if (status != DRM_TRYLATER && status != DRM_DEFER) {
        util_log(1, "%s processing completed", request);
        if ((fp = fopen(name->flag.done, "w")) == NULL) {
            util_log(1, "WARNING: can't create flag `%s': %s",
                name->flag.done, syserrmsg(errno)
            );
        } else {
            fclose(fp);
        }
    }
    drm_exit(status);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.2  2003/04/24 00:10:37  dechavez
 * Anti-spam support introduced
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
