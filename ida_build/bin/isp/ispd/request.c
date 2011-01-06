#pragma ident "$Id: request.c,v 1.6 2005/11/04 04:39:55 dechavez Exp $"
/*======================================================================
 *
 *  Generate SAN data requests.
 *
 *  It works like this.  All configured streams are requested.  If a
 *  particular stream is saved in a local NRTS disk loop, then the next
 *  unarchived frame is requested.  If it is not saved in the disk loop
 *  (and that would only be if somebody screwed up the configuration,
 *  in my opinion), then the most current datum is requested.
 *
 *  In all cases, the "log" stream is always dumped from the oldest
 *  available.  Special code will check to see if a particular log
 *  record has already been seen and take the appropriate action if so.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "ispd.h"
#include "xfer.h"

extern ISP_PARAMS *Params;
extern struct nrts_sys *Nrts;

#ifdef USE_SIMPLE_REQUEST_LOGIC
BOOL GenerateRequest(struct xfer_req *req)
{
int status;
struct xfer01_wavreq *wavreq;
char scsc[] = "SAN:*";
static char *fid = "GenerateRequest";

    status = Xfer_FillReq(
        req,
        XFER_CNFNRTS,
        XFER_WAVIDA,
        scsc,
        XFER_YNGEST,
        XFER_YNGEST,
        1,
        Params->san.timeout
    );

    if (status != 0) {
        util_log(1, "%s: Xfer_FillReq failed, status %d", fid, status);
        return FALSE;
    }

    return TRUE;
}
#endif /* USE_SIMPLE_REQUEST_LOGIC */

static void SetBegEnd(char *chn, double *beg, double *end)
{
int i;
struct nrts_sta *nrts;

/* Always request full log dumps */

    if (strcasecmp(chn, "log") == 0) {
        *beg = XFER_OLDEST;
        *end = XFER_YNGEST;
        return;
    }

/* Check local NRTS */

    if (Nrts != (struct nrts_sys *) NULL) {
        nrts = &Nrts->sta[0];
        for (i = 0; i < nrts->nchn; i++) {
        /* Match channel name */
            if (strcmp(nrts->chn[i].name, chn) == 0) {
            /* backfill empty disk loops as much as possible */
                if (nrts->chn[i].beg < (double) 0) {
                    *beg = XFER_OLDEST;
                    *end = XFER_YNGEST;
                } else {
            /* otherwise pick up where we left off */
                    *beg = nrts->chn[i].end + (double) nrts->chn[i].sint;
                    *end = XFER_YNGEST;
                }
                return;
            }
        }
    }

/* Must not be in the disk loop... request most recent */

    *beg = XFER_YNGEST;
    *end = XFER_YNGEST;
}

BOOL GenerateRequest(struct xfer_req *req)
{
int i;
struct xfer01_wavreq *wav;
struct xfer01_stareq *sta;
char scsc[] = "SAN:*";
SANIO_CONFIG cnf;
static char *fid = "GenerateRequest";

    if (!GetSanCnf(&cnf)) {
        util_log(1, "FATAL ERROR: %s: missing SAN config!", fid);
        return FALSE;
    }

    if (cnf.nstream > XFER_MAXCHN) {
        util_log(1, "FATAL ERROR: %s: cnf.nstream > XFER_MAXCHN", fid);
        return FALSE;
    }

    req->protocol = 1;
    req->type     = XFER_WAVREQ;
    req->timeout  = Params->san.timeout;
    req->sndbuf   = XFER_SO_SNDBUF;
    req->rcvbuf   = XFER_SO_RCVBUF;
    req->preamble.ver01.client_id = getpid();
    req->preamble.ver01.format    = XFER_CNFNRTS;

    wav = &req->request.wav.ver01;
    wav->format = XFER_WAVIDA;
    wav->keepup = 1;
    wav->comp   = XFER_CMPIGPP;
    wav->nsta   = 1;
    sta = &wav->sta[0];
    strcpy(sta->name, "SAN"); /* SAN ignores station name */
    sta->nchn   = cnf.nstream;
    for (i = 0; i < sta->nchn; i++) {
        if (strlen(cnf.stream[i].name) > XFER01_CNAMLEN) {
            util_log(1, "%s: %s too long, increase XFER01_CNAMLEN!",
                fid, cnf.stream[i].name
            );
            return FALSE;
        }
        strcpy(sta->chn[i].name, cnf.stream[i].name);
        SetBegEnd(sta->chn[i].name, &sta->chn[i].beg, &sta->chn[i].end);
    }

    return TRUE;
}

/* Revision History
 *
 * $Log: request.c,v $
 * Revision 1.6  2005/11/04 04:39:55  dechavez
 * toned down log messages, added some comment clues
 *
 * Revision 1.5  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2000/11/08 01:57:24  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.3  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/10/19 22:26:02  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1  2000/10/11 20:36:33  dec
 * initial version
 *
 */
