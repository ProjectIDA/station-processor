#pragma ident "$Id: remap_req.c,v 1.2 2004/09/28 23:32:36 dechavez Exp $"
/*======================================================================
 *
 *  Remap incoming request into a protocol independent form
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "xfer.h"
#include "edes.h"

extern struct edes_params *EdesParams;

#define NSTA     (EdesParams->nsys)
#define STA(i)   (EdesParams->sys[(i)].sta[0])
#define NCHN(i)  (STA(i).nchn)
#define CHN(i,j) (STA(i).chn[(j)])

static int StationIndex(char *name)
{
int i;

    for (i = 0; i < NSTA; i++) {
        if (strcasecmp(STA(i).name, name) == 0) return i;
    }

    return -1;
}

static void load_xfer01_chn(int loglevel, int sindex, struct xfer01_stareq *in, struct edes_stareq *out)
{
int j;
static char *fid = "load_xfer01_chn";

    if (in->nchn == 1 && strcmp(in->chn[0].name, "*") == 0) {
        out->nchn = NCHN(sindex);
        util_log(loglevel, "%s: expanding channel wildcard, nchn=%d", fid, out->nchn);
        for (j = 0; j < NCHN(sindex); j++) {
            strcpy(out->chn[j].name, CHN(sindex,j).name);
            out->chn[j].beg = in->chn[0].beg;
            out->chn[j].end = in->chn[0].end;
        }
    } else {
        util_log(loglevel, "%s: remapping %d (in->nchn) channel requests", fid, in->nchn);
        out->nchn = in->nchn;
        for (j = 0; j < in->nchn; j++) {
            strcpy(out->chn[j].name, in->chn[j].name);
            out->chn[j].beg = in->chn[j].beg;
            out->chn[j].end = in->chn[0].end;
            util_log(loglevel, "%s: name=%s beg=%.3lf end=%.3lf",
                fid, out->chn[j].name, out->chn[j].beg, out->chn[j].end
            );
        }
    }
}

static void load_xfer01_sta(int loglevel, struct xfer01_wavreq *in, struct edes_req *out)
{
int i, sindex;
static char *fid = "load_xfer01_sta";

    if (in->nsta == 1 && strcmp(in->sta[0].name, "*") == 0) {
        out->nsta = NSTA;
        util_log(loglevel, "%s: expanding station wildcard, nsta=%d", fid, out->nsta);
        for (i = 0; i < NSTA; i++) {
            strcpy(out->sta[i].name, STA(i).name);
            load_xfer01_chn(loglevel, i, &in->sta[0], &out->sta[i]);
        }
    } else {
        util_log(loglevel, "%s: remapping %d (in->nsta) station requests", fid, in->nsta);
        for (out->nsta = 0, i = 0; i < in->nsta; i++) {
            if ((sindex = StationIndex(in->sta[i].name)) != -1) {
                strcpy(out->sta[out->nsta].name, in->sta[i].name);
                util_log(loglevel, "%s: name=%s", fid, out->sta[out->nsta].name);
                load_xfer01_chn(loglevel, sindex, &in->sta[i], &out->sta[out->nsta]);
                ++out->nsta;
            } else {
                util_log(loglevel, "%s: reject unsupported station %s", fid, in->sta[i].name);
            }
        }
    }
}

static void LogEdesChnReq(int loglevel, struct edes_chnreq *chn)
{
    util_log(loglevel, "        chn name=%s, beg=%.3lf, end=%.3lf", chn->name, chn->beg, chn->end);
}

static void LogEdesStaReq(int loglevel, struct edes_stareq *sta)
{
int i;

    util_log(loglevel, "        sta name=%s, nchn=%d", sta->name, sta->nchn);
    for (i = 0; i < sta->nchn; i++) LogEdesChnReq(loglevel, &sta->chn[i]);
}

static void LogEdesReq(int loglevel, struct edes_req *out)
{
int i;

    util_log(loglevel, "type=%d, timeout=%d, ident=%d, cnf_form=%d", 
        out->type, out->timeout, out->ident, out->cnf_form
    );
    if (out->type != XFER_WAVREQ) return;
    util_log(loglevel, "    wav_form=%d, keepup=%d, comp=%d, nsta=%d",
        out->wav_form, out->keepup, out->comp, out->nsta
    );
    for (i = 0; i < out->nsta; i++) LogEdesStaReq(loglevel, &out->sta[i]);
}

static void load_xfer01_wavreq(int loglevel, struct xfer01_wavreq *in, struct edes_req *out)
{
int i;
static char *fid = "load_xfer01_wavreq";

    out->wav_form = in->format;
    out->keepup   = in->keepup;
    out->comp     = in->comp;
    load_xfer01_sta(loglevel, in, out);
}

int remap_req(struct xfer_req *xfer, struct edes_req *er)
{
static int loglevel = 2;

    switch (xfer->protocol) {
      case 0x01:
        er->type     = xfer->type;
        er->timeout  = xfer->timeout;
        er->ident    = xfer->preamble.ver01.client_id;
        er->cnf_form = xfer->preamble.ver01.format;
        if (xfer->type == XFER_WAVREQ) load_xfer01_wavreq(loglevel, &xfer->request.wav.ver01, er);
        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return -1;
    }
    LogEdesReq(loglevel, er);

    return 0;
}

/* Revision History
 *
 * $Log: remap_req.c,v $
 * Revision 1.2  2004/09/28 23:32:36  dechavez
 * fixed bug parsing request string
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
