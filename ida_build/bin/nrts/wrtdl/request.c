#pragma ident "$Id: request.c,v 1.5 2003/11/25 20:40:44 dechavez Exp $"
/*======================================================================
 *
 *  Generate xfer requests.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "nrts.h"
#include "util.h"
#include "wrtdl.h"

static int want[NRTS_MAXSTA][NRTS_MAXCHN];
static int nsta;
static int nchn[NRTS_MAXSTA];

int init_request(struct nrts_sys *sys, char *keeparg)
{
int i, j, k, count, ntoken;
char *token[NRTS_MAXCHN];
char buffer[512];
static char *fid = "init_request";

    nsta = 0;

    if (XFER_MAXSTA < sys->nsta) {
        util_log(1, "%s: too many stations, increase XFER_MAXSTA", fid);
        return -1;
    }

    for (i = 0; i < NRTS_MAXSTA; i++) {
        nchn[i] = 0;
        for (j = 0; j < NRTS_MAXCHN; j++) {
            want[i][j] = 0;
        }
    }

/* If keeparg is not given, then request all locally acquired streams */

    if (keeparg == NULL) {
        nsta = sys->nsta;
        for (i = 0; i < sys->nsta; i++) {
            nchn[i] = sys->sta[i].nchn;
            if (XFER_MAXCHN < sys->sta[i].nchn) {
                util_log(1, "%s: too many channels, increase XFER_MAXCHN",
                    fid
                );
                return -1;
            }
            for (j = 0; j < sys->sta[i].nchn; j++) {
                want[i][j] = 1;
            }
        }
        util_log(1, "requesting all configured channels");
        return 0;
    }
                
/* If keeparg is given, then use it as a channel list (assume all stas) */

    util_lcase(keeparg);
    if ((ntoken = util_sparse(keeparg, token, ",:/", NRTS_MAXCHN)) < 1) {
        util_log(1, "%s: illegal keeparg '%s'", fid, keeparg);
        return -1;
    }

    for (i = 0; i < sys->nsta; i++) {
        if (XFER_MAXCHN < sys->sta[i].nchn) {
            util_log(1, "%s: too many channels, increase XFER_MAXCHN", fid);
            return -1;
        }
        buffer[0] = 0;
        for (count = 0, j = 0; j < sys->sta[i].nchn; j++) {
            for (k = 0; k < ntoken; k++) {
                if (strcasecmp(token[k], sys->sta[i].chn[j].name) == 0) {
                    want[i][j] = 1;
                    ++count;
                    sprintf(buffer+strlen(buffer), " %s", token[k]);
                }
            }
        }
        nchn[i] = count;
        if (count) {
            ++nsta;
            util_log(1, "request %s chans %s", sys->sta[i].name, buffer);
        }
    }

    return 0;
}

int request(struct nrts_sys *sys, double beg, double end, struct xfer01_wavreq *req)
{
int i, j, k, l;
struct nrts_sta *sta;
struct nrts_chn *chn;
static char *fid = "request";

    req->nsta = nsta;
    for (k = 0, i = 0; i < sys->nsta; i++) {
        sta = sys->sta + i;
        if (nchn[i] > 0) {
            strcpy(req->sta[k].name, sta->name);
            for (l = 0, j = 0; j < sta->nchn; j++) {
                if (want[i][j]) {
                    chn = sta->chn + j;
                    strcpy(req->sta[k].chn[l].name, chn->name);

       /* Request start time depends on status of disk loop and on
        * value of `beg' argument.
        *
        * If this is a virgin disk loop then the request start time is
        * whatever the `beg' argument specifies.
        *
        * If there is already data in this disk loop, then the request 
        * start time is the greater of the end time (plus 1 sample interval)
        * or the value of the `beg' argument.  That way we can force a gap
        * in the disk loop (say the current end time is so far in the past
        * that we have no interest in data around that time), but not put
        * any duplicate data in the loop in the process (say we specify a
        * beg time which is earlier that the current end time... we won't
        * want to obey that request and instead force end + 1 sample).
        */

                    if (chn->beg <= (double) 0) {
                        req->sta[k].chn[l].beg = beg;
                        if (beg == XFER_OLDEST) {
                            util_log(2, "virgin disk loop, set %s:%s beg = %s",
                                sta->name, chn->name, "XFER_OLDEST"
                            );
                        } else if (beg == XFER_YNGEST) {
                            util_log(2, "virgin disk loop, set %s:%s beg = %s",
                                sta->name, chn->name, "XFER_YNGEST"
                            );
                        } else {
                            util_log(2, "virgin disk loop, set %s:%s beg = %s",
                                sta->name, chn->name, util_dttostr(beg, 0)
                            );
                        }

                    } else if (chn->end < beg) {
                        req->sta[k].chn[l].beg = beg;
                        util_log(2, "new request, force %s:%s beg = %s",
                            sta->name, chn->name,
                            util_dttostr(req->sta[k].chn[l].beg, 0)
                        );

                    } else {
                        req->sta[k].chn[l].beg = chn->end +
                                                    (double) chn->sint;
                        util_log(2, "new request, set %s:%s beg = %s",
                            sta->name, chn->name,
                            util_dttostr(req->sta[k].chn[l].beg,0)
                        );
                    }

                    req->sta[k].chn[l].end = end;
                    ++l;
                }
            }
            if (l != nchn[i]) {
                util_log(1, "%s: logic error: %d != %d", fid, l, nchn[i]);
                return -1;
            }
            req->sta[k].nchn = l;
            k++;
        }
    }
    return 0;
}

/* Revision History
 *
 * $Log: request.c,v $
 * Revision 1.5  2003/11/25 20:40:44  dechavez
 * ANSI function declarations
 *
 * Revision 1.4  2002/05/16 17:27:53  nobody
 * case insensitive channel name comparison
 *
 * Revision 1.3  2002/04/02 01:29:16  dec
 * switch to ansi style
 *
 * Revision 1.2  2000/10/12 17:24:09  dec
 * 2.10.8
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
