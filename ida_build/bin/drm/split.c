#pragma ident "$Id: split.c,v 1.3 2007/11/05 20:19:16 dechavez Exp $"
/*======================================================================
 *
 *  Split a single IDADRM data request into one or more sub-requests,
 *  depending on network configuration.  Channels which are available
 *  on only a subset of nodes are grouped into individual requests.
 *
 *  If none of the requested channels are acquired by any NRTS in the
 *  network then the original request is returned, unaltered.
 *
 *  Call with copy flag set to 0 if you just want the total number of
 *  requests which would result.  Set it to 1 if you want the requests
 *  to be stored in the output buffer.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"
#include "service.h"
#include "fnmatch.h"

#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD 0x10
#endif

int split_req(input, output, buffer)
struct drm_datreq *input;
struct drm_datreq **output;
char *buffer;
{
FILE *fp;
int i, j, ii, jj, n, nchn, nchan, nmatch, reqndx;
char *chan[DRM_MAXCHN];
struct drm_cnf *cnf;
struct drm_info *info;
struct drm_sta *entry = NULL;
static struct drm_datreq request[DRM_MAXHOP];
static char *fid = "split_req";

/* By default will return original request if no matches found */

    *output = request;
    request[0] = *input;

/* Get working copy of desired channels */

    nchan = input->nchn;
    for (i = 0; i < input->nchn; i++) chan[i] = input->chn[i];

/* Get the configured system list for this station */

    cnf   = drm_cnf();
    for (entry = NULL, i = 0; i < cnf->nsta && entry == NULL; i++) {
        if (strcmp(cnf->sta[i].name, input->sname) == 0) {
            entry = cnf->sta + i;
        }
    }

    if (entry == NULL) return 1;

/* Match requested channels against the per-system configured channels */

    reqndx = -1;
    for (i = 0; i < entry->nhop; i++) {
        info = getinfo(entry->node[i], buffer);
        for (j = 0; j < info->nsta; j++) {
            if (strcasecmp(input->sname, info->sta[j].name) == 0) {

            /* Working now with NRTS system that acquires this station */
            /* Find all channels which are acquired at this node       */

                n = nmatch = 0;
                for (ii = 0; ii < info->sta[j].nchn; ii++) {
                    for (jj = 0; jj < nchan; jj++) {
                        if (chan[jj] != NULL) {
                            if (
                                fnmatch(
                                    info->sta[j].chn[ii].name, chan[jj],
                                    FNM_CASEFOLD
                                ) == 0
                            ) {
                                if (++nmatch == 1) {
                                    if (++reqndx == DRM_MAXHOP) {
                                        util_log(1, "%s: increase DRM_MAXHOP",
                                            fid
                                        );
                                        drm_exit(DRM_BAD);
                                    }
                                    request[reqndx] = *input;
                                    n = request[reqndx].nchn = 0;
                                }
                                request[reqndx].chn[n] = chan[jj];
                                ++n; ++request[reqndx].nchn;
                                chan[jj] = NULL;
                            }
                        }
                    }
                }
            }
        }
    }
    ++reqndx;

    return reqndx ? reqndx : 1;
}

/* Revision History
 *
 * $Log: split.c,v $
 * Revision 1.3  2007/11/05 20:19:16  dechavez
 * added FNM_CASEFOLD if needed
 *
 * Revision 1.2  2003/11/21 20:20:26  dechavez
 * include local "fnmatch.h"
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
