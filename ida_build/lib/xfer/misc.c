#pragma ident "$Id: misc.c,v 1.4 2005/05/25 22:41:56 dechavez Exp $"
/*======================================================================
 *
 * Misc. helper functions
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include "xfer.h"
#include "util.h"

/*======================================================================
 *
 * Parse a chn,chn,... string, and return number of channels added.
 *
 *====================================================================*/

int Xfer_ParseCC(req, i, cc, beg, end)
struct xfer_req *req;
int i;
char *cc;
double beg;
double end;
{
int j, k, ntoken;
static int maxtoken = XFER_MAXCHN;
char *token[XFER_MAXCHN];
struct xfer01_wavreq *wavreq;
static char *fid = "Xfer_ParseCC";

    if (req->protocol != 0x01) {
        xfer_errno = XFER_EPROTOCOL;
        return -100;
    }

    wavreq = (struct xfer01_wavreq *) &req->request.wav.ver01;

/* Parse string into individual channel names */

    ntoken = util_sparse(cc, token, ",", maxtoken);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, cc);
        xfer_errno = XFER_EFAULT;
        return -200;
    }

/* Make sure we don't duplicate channel names for this station */

    for (j = 0; j < wavreq->sta[i].nchn; i++) {
        for (k = 0; k < ntoken; k++) {
            if (strcasecmp((const char *) wavreq->sta[i].chn[j].name, token[k]) == 0) {
                token[k] = (char *) NULL;
            }
        }
    }

/* Add in the new channel names */

    for (k = 0; k < ntoken; k++) {
        if (token[k] != (char *) NULL) {
            if (wavreq->sta[i].nchn == XFER_MAXCHN) {
                util_log(1, "%s: increase XFER_MAXCHN (%d) to at least %d",
                    fid, XFER_MAXCHN, wavreq->sta[i].nchn + 1
                );
                xfer_errno = XFER_ELIMIT;
                return -300;
            }
            strcpy(wavreq->sta[i].chn[wavreq->sta[i].nchn].name, token[k]);
            wavreq->sta[i].chn[wavreq->sta[i].nchn].beg = beg;
            wavreq->sta[i].chn[wavreq->sta[i].nchn].end = end;
            ++wavreq->sta[i].nchn;
        }
    }

    return ntoken;
}

/*======================================================================
 *
 * Parse a sta:chn,chn,... string, and return number of channels added
 *
 *====================================================================*/

int Xfer_ParseSC(req, sc, beg, end)
struct xfer_req *req;
char *sc;
double beg;
double end;
{
int i, ntoken;
struct xfer01_wavreq *wavreq;
static int maxtoken = 2;
char *token[2];
static char *fid = "Xfer_ParseSC";

    if (req->protocol != 0x01) {
        xfer_errno = XFER_EPROTOCOL;
        return -10;
    }

    wavreq = (struct xfer01_wavreq *) &req->request.wav.ver01;

    ntoken = util_sparse(sc, token, ":", 2);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, sc);
        xfer_errno = XFER_EFAULT;
        return -20;
    } else if (ntoken != 2) {
        util_log(1, "%s: illegal `%s'", fid, sc);
        xfer_errno = XFER_EINVAL;
        return 0;
    }

/* See if we already have an entry for this station */

    for (i = 0; i < wavreq->nsta; i++) {
        if (strcasecmp((const char *) wavreq->sta[i].name, token[0]) == 0) {
            return Xfer_ParseCC(req, i, token[1], beg, end);
        }
    }

/* Create an entry for this station */

    if (wavreq->nsta == XFER_MAXSTA) {
        util_log(1, "%s: increase XFER_MAXSTA (%d) to at least %d",
            fid, XFER_MAXSTA, wavreq->nsta + 1
        );
        xfer_errno = XFER_ELIMIT;
        return -40;
    }

    strcpy(wavreq->sta[wavreq->nsta].name, token[0]);
    wavreq->sta[wavreq->nsta].nchn = 0;
    ++wavreq->nsta;

    return Xfer_ParseCC(req, wavreq->nsta - 1, token[1], beg, end);
}

/*======================================================================
 *
 * Parse a sta:chn,chn,...+sta:chn,chn,... string and return number of
 * stations added.
 *
 *====================================================================*/

int Xfer_ParseSCSC(req, scsc, beg, end)
struct xfer_req *req;
char *scsc;
double beg;
double end;
{
int i, ntoken, nsta, retval;
struct xfer01_wavreq *wavreq;
static int maxtoken = XFER_MAXSTA;
char *token[XFER_MAXSTA];
static char *fid = "Xfer_ParseSCSC";

    if (req->protocol != 0x01) {
        xfer_errno = XFER_EPROTOCOL;
        return -1;
    }

    wavreq = (struct xfer01_wavreq *) &req->request.wav.ver01;

    ntoken = util_sparse(scsc, token, "+", maxtoken);
    if (ntoken < 1) {
        util_log(1, "%s: util_sparse(%s, ...) failed", fid, scsc);
        xfer_errno = XFER_EFAULT;
        return -20;
    }

    for (nsta = 0, i = 0; i < ntoken; i++) {
        if ((retval = Xfer_ParseSC(req, token[i], beg, end)) < 0) {
            return retval;
        } else if (retval > 0) {
            ++nsta;
        }
    }

    return nsta;
}

/*======================================================================
 *
 * Fill out a request
 *
 *====================================================================*/

int Xfer_FillReq(req, cnffmt, wavfmt, stachn, beg, end, keepup, timeout)
struct xfer_req *req;
int cnffmt;
int wavfmt;
char *stachn;
double beg;
double end;
int keepup;
int timeout;
{
int nsta;
char *copy;
static char *fid = "Xfer_FillReq";
 
/* Common to all requests */

    req->protocol = 1;
    req->type     = (wavfmt < 0) ? XFER_CNFREQ : XFER_WAVREQ;
    req->timeout  = (timeout > 0) ? timeout: XFER_DEFTO;
    req->sndbuf   = XFER_SO_SNDBUF;
    req->rcvbuf   = XFER_SO_RCVBUF;

/* Protocol specific preamble */

    req->preamble.ver01.client_id = getpid();
    req->preamble.ver01.format = (cnffmt > 0) ? cnffmt : XFER_CNFGEN1;

/* If this is a configuration request we are done */

    if (req->type == XFER_CNFREQ) return 0;

/* Must be a waveform request */

    req->type = XFER_WAVREQ;

    req->request.wav.ver01.format = (wavfmt > 0) ? wavfmt : XFER_WAVGEN1;
    req->request.wav.ver01.keepup = keepup;
    req->request.wav.ver01.comp   = XFER_CMPIGPP;
    req->request.wav.ver01.nsta   = 0;
    if (stachn == (char *) NULL) return 0;

/* Parse the station/channel specification string */

    if ((copy = strdup(stachn)) == NULL) {
        util_log(1, "%s: strdup: %s", fid, strerror(errno));
        return -1;
    }
    if ((nsta = Xfer_ParseSCSC(req, stachn, beg, end)) < 0) {
        util_log(1, "%s: Xfer_ParseSCSC: can't parse %s: status %d",
            fid, copy, nsta
        );
        free(copy);
        return -1;
    }

    free(copy);
    return 0;
}

/* print a Version 1 wavreq string */

char *xferWavreqString(struct xfer_req *xfer_req, char *msg)
{
int i, j;
struct xfer01_wavreq *req;

    if (xfer_req->protocol != 1) {
        sprintf(msg, "UNSUPPORTED PROTOCOL %s", xfer_req->protocol);
        return msg;
    }

    if (xfer_req->type != XFER_WAVREQ) {
        sprintf(msg, "NOT A WAVREQ");
        return msg;
    }

    req = &xfer_req->request.wav.ver01;
    sprintf(msg, "\"");
    for (i = 0; i < req->nsta; i++) {
        if (i) sprintf(msg+strlen(msg), "+");
        sprintf(msg+strlen(msg), "%s:", req->sta[i].name);
        for (j = 0; j < req->sta[i].nchn; j++) {
            if (j) sprintf(msg+strlen(msg), ",");
            sprintf(msg+strlen(msg), "%s", req->sta[i].chn[j].name);
        }
    }
    sprintf(msg+strlen(msg), "\"");

    return msg;
}

#ifdef DEBUG_TEST

main(argc, argv)
int argc;
char *argv[];
{
int i, j, retval;
struct xfer_req req;
struct xfer01_wavreq *wavreq;

    if (argc != 2) exit(1);

    printf("testing `%s'\n", argv[1]);

    req.protocol = 1;
    wavreq = (struct xfer01_wavreq *) &req.request.wav.ver01;
    wavreq->nsta = 0;

    retval = Xfer_ParseSCSC(&req, argv[1]);
    printf("Xfer_ParseSCSC() returns %d\n", retval);

    if (retval != 0) exit(1);

    printf("nsta = %d\n", wavreq->nsta);
    for (i = 0; i < wavreq->nsta; i++) {
        printf("%d channels of station `%s'\n", wavreq->sta[i].nchn,
                                                wavreq->sta[i].name);
        for (j = 0; j < wavreq->sta[i].nchn; j++) {
            printf("\t%d: %s\n", i, wavreq->sta[i].chn[j].name);
        }
    }

    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: misc.c,v $
 * Revision 1.4  2005/05/25 22:41:56  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2004/09/28 22:48:34  dechavez
 * support parsing of malformed strings
 *
 * Revision 1.2  2002/04/02 00:49:40  dec
 * added xferWavreqString()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:43  dec
 * import existing IDA/NRTS sources
 *
 */
