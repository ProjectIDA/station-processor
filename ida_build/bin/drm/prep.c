#pragma ident "$Id: prep.c,v 1.3 2004/01/29 23:56:14 dechavez Exp $"
/*======================================================================
 *
 *  prepsets: prepare single dataset IDADRM requests
 *  prep_line: parse a line, stripping comment character
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "service.h"
#include "util.h"

int prepsets(struct drm_rawreq *rawreq, char *buffer)
{
FILE *fp;
int i, j, k, n, fd, nchn, nchan, nset, total;
char *fname, *path, **chan;
struct drm_names *name;
struct drm_datreq *split;
static char *fid = "prepsets";

/* We will be splitting requests across channel boundaries which are natural
 * for the configuration of each NRTS node.  Count how many data sets will
 * result from this split.
 */

    for (total = 0, i = 0; i < rawreq->nset; i++) {
        nchn = drm_chanlst(rawreq->set + i, &chan, buffer);
        if (nchn > 0) {
            rawreq->set[i].nchn = nchn;
            for (j = 0; j < rawreq->set[i].nchn; j++) {
                if ((rawreq->set[i].chn[j] = strdup(chan[j])) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    drm_exit(DRM_BAD);
                }
            }
            total += split_req(rawreq->set + i, &split, buffer);
        } else {
            ++total;
        }
    }
    
/* For each data request, do the split and write the request */

    name = drm_names();

    for (n = 1, i = 0; i < rawreq->nset; i++) {

    /* Split request */

        nset = split_req(rawreq->set + i, &split, buffer);

        for (j = 0; j < nset; j++) {

        /* Update finfo parameters to reflect final number of requests */

            split[j].nset = total;
            split[j].finfo.set = n++;

        /* Create locked queue file */

            if ((fname = strdup(drm_mkname(&split[j].finfo))) == NULL){
                util_log(1, "%s: fatal error: strdup: %s", 
                    fid, syserrmsg(errno)
                );
                drm_exit(DRM_BAD);
            }
            sprintf(buffer, "%s/%s", name->dir.queue, fname);
            if ((path  = strdup(buffer)) == NULL) {
                util_log(1, "%s: fatal error: strdup: %s", 
                    fid, syserrmsg(errno)
                );
                drm_exit(DRM_BAD);
            }
            if ((fd = open(path, O_CREAT, DRM_LOCKED)) < 0) {
                util_log(1, "%s: fatal error: %s: %s", 
                    fid, path, syserrmsg(errno)
                );
                drm_exit(DRM_BAD);
            }
            close(fd);

            if ((fp = fopen(path, "a")) == NULL) {
                util_log(1, "%s: fatal error: %s: %s", 
                    fid, path, syserrmsg(errno)
                );
                drm_exit(DRM_BAD);
            }

        /* Write request in IDADRM format */

            drm_wrdatreq(fp, split + j);
            fclose(fp);

        /* Unlock the file */

            if (chmod(path, DRM_UNLOCKED) != 0) {
                util_log(1, "%s: fatal error: can't chmod %s: %s",
                    fid,  path, syserrmsg(errno)
                );
                drm_exit(DRM_BAD);
            }
            util_log(1, "request %s queued", fname);
        }
    }

    return DRM_OK;
}

int prep_line(char *buffer, char **token, int maxtoken, char comment, char *delimiters)
{
int i;

/*  Remove everything after comment delimiter  */

    if (comment != (char) 0) {
        i = 0;
        while (i < strlen(buffer) && buffer[i++] != comment);
        buffer[--i] = 0;
    }

/*  Remove trailing blanks and newline  */

    i = strlen(buffer) - 1;
    while (i >= 0 && (buffer[i] == ' ' || buffer[i] == '\n')) --i;
    buffer[++i] = 0;

/*  Parse the line into individual tokens and return number of tokens */

    return utilParse(buffer, token, delimiters, maxtoken, 0);
}

/* Revision History
 *
 * $Log: prep.c,v $
 * Revision 1.3  2004/01/29 23:56:14  dechavez
 * utilParse instead of old  util_sparse
 *
 * Revision 1.2  2004/01/23 22:23:33  dechavez
 * switched to ansi declarations
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
