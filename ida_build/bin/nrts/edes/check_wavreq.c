#pragma ident "$Id: check_wavreq.c,v 1.5 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Check a request against the supported systems and verify that
 *  it is valid and that at least one requested channel is present.
 *  Close disk loop files for any systems that have no data requested.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "util.h"
#include "nrts.h"
#include "xfer.h"
#include "edes.h"

#define SYS  (edes->sys + i)
#define STA  (SYS->sta + j)
#define CHN  (STA->chn + k)

#define CSTA (edes->cnf[i].sta + j)
#define CCHN (CSTA->chn + k)

#define ISTA (edes->inf[i].sta + j)
#define ICHN (ISTA->chn + k)

#define RSTA (request->sta + jj)
#define RCHN (RSTA->chn + kk)

int check_wavreq(struct edes_req *request, struct edes_params *edes)
{
int chncounter;
int i, j, k, jj, kk, smatch, cmatch, begOK, endOK;
static char *fid = "check_wavreq";

    for (i = 0; i < edes->nsys; i++) {
        edes->inf[i].count = 0;
        for (j = 0; j < SYS->nsta; j++) {
            for (k = 0; k < STA->nchn; k++) {
                ICHN->status = EDES_IGNORE;
            }
        }
    }

/* Set ignore flags for all supported but unrequested station/channels */
/* Also, reject request if the time windows don't make sense           */

    edes->keepup = request->keepup;

    chncounter = 0;
    for (jj = 0; jj < request->nsta; jj++) {
        for (smatch = 0, i = 0; !smatch && i < edes->nsys; i++) {
            for (j = 0; !smatch && j < SYS->nsta; j++) {
                if (strcasecmp(STA->name, RSTA->name) == 0) {
                    ++smatch;
                    for (kk = 0; kk < RSTA->nchn; kk++) {
                        for (cmatch = 0, k = 0; !cmatch && k < STA->nchn; k++) {
                            if (strcasecmp(CHN->name, RCHN->name) == 0) {
                                ++cmatch;
                                ICHN->beg = RCHN->beg;
                                ICHN->end = RCHN->end;

                            /* Look for beg/end of diskloop flags */

                                begOK = endOK = 0;
                                if (RCHN->beg == XFER_OLDEST) {
                                    begOK = 1;
                                }
                                if (RCHN->beg == XFER_YNGEST) {
                                    begOK = 1;
                                }
                                if (RCHN->end == XFER_OLDEST) {
                                    endOK = 1;
                                }
                                if (RCHN->end == XFER_YNGEST) {
                                    endOK = 1;
                                }

                            /* Make sure beg/end are consistent */

                                if (!begOK && RCHN->beg < (double) 0) {
                                    util_log(1, "%s: reject negative beg", fid);
                                } else if (!endOK && RCHN->end < (double) 0) {
                                    util_log(1, "%s: reject negative end", fid);
                                } else if (RCHN->end != XFER_YNGEST && RCHN->end < RCHN->beg) {
                                    util_log(1, "%s: reject end %.3lf < beg %.3lf", fid, RCHN->end, RCHN->beg);
                                } else {
                                    ICHN->count  = 0;
                                    ICHN->status = EDES_WAITING;
                                    ++edes->inf[i].count;
                                    ++chncounter;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (chncounter == 0) {
        xfer_errno = XFER_ENOSUCH;
        return -6;
    }

    for (i = 0; i < edes->nsys; i++) {
        if (edes->inf[i].count == 0) {
            close(edes->inf[i].hfd);
            close(edes->inf[i].dfd);
            close(edes->map[i].fd);
        }
    }

    return 0;
}

/* Revision History
 *
 * $Log: check_wavreq.c,v $
 * Revision 1.5  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.4  2002/04/02 01:27:46  nobody
 * ansi function call
 *
 * Revision 1.3  2000/03/16 06:54:48  nobody
 * Corrected errors in checking requests which caused it to reject
 * beg=value end=XFER_YNGEST
 *
 * Revision 1.2  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
