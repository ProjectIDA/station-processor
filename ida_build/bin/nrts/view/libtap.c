#pragma ident "$Id: libtap.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
/*======================================================================
 *
 *  Single station interface to the nrts_edep server.  This is just to
 *  give backward compatibility to old nrts_xfer clients.  It can't be
 *  fully compatible because the nrts_edep service does not allow for
 *  sending nrts_sys structures, so the return value of nrts_tapinit()
 *  has been changed to int.  Also, the channel wildcarding available
 *  by setting info->keeparg to NULL is not supported.  You must 
 *  explicity specify the desired channels.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include "nrts.h"
#include "util.h"
#include "xfer.h"

static char *host, *service, *sta;
static int port;
static int sd = -1;

static struct xfer_req req;
static struct xfer_req *rptr = (struct xfer_req *) &req;
static struct xfer01_wavreq *wrp = &req.request.wav.ver01;

static struct xfer_cnf cnf;
static struct xfer_cnfgen1 *cnp = (struct xfer_cnfgen1 *) &cnf.type.gen1;

static int initialized = 0;

int rs;  /* req sta index  */
int rc;  /* req chn index  */
int cs;  /* cnf sta index  */
int cc;  /* cnf chn index  */
/* Mapping from cnf indices to req indicies */
static int rsi[XFER_MAXSTA];
static int rci[XFER_MAXSTA][XFER_MAXCHN];
/* Mapping from req indices to cnf indicies */
static int csi[XFER_MAXSTA];
static int cci[XFER_MAXSTA][XFER_MAXCHN];

int _establish_connection()
{
int i, j, retry;

/* Submit request */

    sd = Xfer_Connect(host, service, port, "tcp", &req, &cnf, 1);
    if (sd < 0) return -1;

/* Generate the mappings between cnf and req indices */

#define STACMP(i, j)    strcasecmp(\
    wrp->sta[i].name, cnp->sta[j].name\
)
#define CHNCMP(i,j,k,l) strcasecmp(\
    wrp->sta[i].chn[j].name, cnp->sta[k].chn[l].name\
)

    for (i = 0; i < XFER_MAXSTA; i++) {
        rsi[i] = -1;
        csi[i] = -1;
        for (j = 0; j < XFER_MAXSTA; j++) {
            rci[i][j] = -1;
            cci[i][j] = -1;
        }
    }

    for (rs = 0; rs < wrp->nsta; rs++) {
        for (cs = 0; cs < cnp->nsta; cs++) {
            if (STACMP(rs, cs) == 0) {
                rsi[cs] = rs;
                csi[rs] = cs;
                for (rc = 0; rc < wrp->sta[rs].nchn; rc++) {
                    for (cc = 0; cc < cnp->sta[cs].nchn; cc++) {
                        if (CHNCMP(rs, rc, cs, cc) == 0) {
                            rci[cs][cc] = rc;
                            cci[rs][rc] = cc;
                        }
                    }
                }
            }
        }
    }

/* Eliminate any requests for non existant stations */

    for (rs = 0; rs < wrp->nsta; rs++) {
        if (csi[rs] < 0) wrp->sta[rs].nchn = 0;
    }

    return 0;
}

void _remove_chn()
{
int i;
struct xfer01_stareq *sp;

    sp = wrp->sta + rs;
    for (i = rc; i < sp->nchn - 1; i++) sp->chn[i] = sp->chn[i+1];
    if (--sp->nchn == 0) {
        for (i = rs; i < wrp->nsta - 1; i++) wrp->sta[i] = wrp->sta[i+1];
        --wrp->nsta;
    }
}

int nrts_tapinit(info)
struct nrts_tapinfo *info;
{
int status;
char *input, *tmp, *sname;
time_t start;
#define SC_MAXLEN 256
static char sc[SC_MAXLEN+1];
static char *fid = "nrts_tapinit";

/* Make sure channels are explicity specified */

    if (info->keeparg == (char *) NULL) {
        util_log(1, "%s: NULL keeparg not permitted", fid);
        return -1;
    }

/* Get station name and server location from input string */

    if (info->input == (char *) NULL) {
        util_log(1, "%s: no input specified", fid);
        return -2;
    } else if ((input = strdup(info->input)) == (char *) NULL) {
        util_log(1, "%s: strdup(%s): %s",
            fid, info->input, syserrmsg(errno)
        );
        return -3;
    }

    if ((tmp = strchr(input, '@')) == (char *) NULL) {
        host  = "localhost";
        sname = input;
    } else {
        host  = tmp + 1;
        *tmp  = 0;
        sname = input;
    }

    if (strlen(sname) + strlen(info->keeparg) > SC_MAXLEN - 1) {
        util_log(1, "%s: station name and/or channel list too big", fid);
        return -4;
    }

    sprintf(sc, "%s:%s", sname, info->keeparg);

/* Get service name if port not explicity given */

    service = (port = info->port) < 0 ? XFER_SERVICE: (char *) NULL;

/* Fill in the request structure */

    if (service != (char *) NULL) {
        util_log(1, "taking input from %s@%s:%s", sname, host, service);
    } else {
        util_log(1, "taking input from %s@%s:%d", sname, host, port);
    }

    req.type = XFER_WAVREQ;
    status = Xfer_FillReq(
        &req, XFER_CNFGEN1, XFER_WAVGEN1, sc, info->beg,
        info->end, info->keep_up, info->timeout
    );

    if (status != 0) {
        util_log(1, "%s: Xfer_FillReq failed, status %d", fid, status);
        return -4;
    }

    initialized = 1;

    return 0;
}

int nrts_tap(output, len)
void **output;
size_t *len;
{
int status;
static struct xfer_wav wav;
static struct nrts_packet packet;
static char *fid = "nrts_tap";

    do {

    /* Connect if necessary */

        if (sd < 0 && (status = _establish_connection()) < 0) {
            util_log(1, "%s: can't communicate with server: %s",
                fid, Xfer_ErrStr()
            );
            return NRTS_ERROR;
        }

    /* Read the next packet from the server */

        status = Xfer_RecvWav(sd, &cnf, &wav);

        if (status == XFER_FINISHED) {
            close(sd);
            sd = -1;
            return NRTS_EOF;
        } else if (status == XFER_ETIMEDOUT) {
            close(sd);
            sd = -1;
        } else if (status != XFER_OK) {
            close(sd);
            sd = -1;
            return NRTS_ERROR;;
        }

    } while (status != XFER_OK);

/* Must have been a good read... convert packet to output format */

    if (Xfer_CvtNRTS(&cnf, &wav, &packet) != 0) {
        util_log(1, "%s: Xfer_CvtNRTS failure!", fid);
        close(sd);
        sd = -1;
        return NRTS_ERROR;;
    }

/* Update request structure in case we have to reconnect after timeout */

    cs = wav.type.gen1.standx;
    cc = wav.type.gen1.chnndx;
    rs = rsi[cs];
    rc = rci[cs][cc];

    wrp->sta[rs].chn[rc].beg = packet.end.time + packet.sint;

/* If all data for this channel have arrived, purge it from the request */

    if (
        wrp->sta[rs].chn[rc].end > (double) 0 && 
        wrp->sta[rs].chn[rc].beg >= wrp->sta[rs].chn[rc].end
    ) _remove_chn();

/* Store output and return success */

    *output = (void *) &packet;
    *len    = sizeof(struct nrts_packet);

    return NRTS_OK;
}

/* Revision History
 *
 * $Log: libtap.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
