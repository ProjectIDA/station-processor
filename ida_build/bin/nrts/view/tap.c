#pragma ident "$Id: tap.c,v 1.2 2002/04/30 16:24:39 dec Exp $"
/*======================================================================
 *
 *  Code intended to provide easy access to NRTS data via nrts_xfer
 *  service.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <string.h>
#include "nrts.h"
#include "util.h"

#define NRTS_TIMEOUT 4

static char *rawbuf[16];

static struct nrts_sys sys[16];
static struct old_xfer_req req[16];

static int  (*net_read)();
static int port[16];
static char *service[16];
static char *syscode[16];
static char *rmthost[16];
static int connected[16];

static int want[16][NRTS_MAXSTA][NRTS_MAXCHN];
static int nsta[16];
static int nchn[16][NRTS_MAXSTA];
static double beg[16], end[16];

static unsigned long data_order, host_order;

int Nrts_tapinit(info, n, cnf)
struct nrts_tapinfo *info;
struct nrts_sys **cnf;
int n;
{
int i, j, k, count, ntoken, status, sid;
char *ptr, *token[16][NRTS_MAXCHN];
char tmpbuf[512];
struct nrts_ida *ida;
static char *fid = "Nrts_tapinit";

     service[n] = (port[n] = info->port) < 0 ? NRTS_XFERSVC: (char *) NULL;

/* Determine location of remote system */

    if ((syscode[n] = strdup(info->input)) == NULL) {
        util_log(2, "%s: strdup: %s", fid, syserrmsg(errno));
        return NRTS_ERROR;
    }

    if ((ptr = strchr(syscode[n], '@')) == NULL) {
        if ((rmthost[n] = (char *) malloc(MAXPATHLEN)) == NULL) {
            util_log(2, "%s: malloc: %s", fid, syserrmsg(errno));
            return NRTS_ERROR;
        }
        if (gethostname(rmthost[n], MAXPATHLEN-1) != 0) {
            util_log(2, "%s: gethostname: %s", fid, syserrmsg(errno));
            return NRTS_ERROR;
        }
        rmthost[n][MAXPATHLEN-1] = 0;
    } else {
        *ptr = 0;
        rmthost[n] = ptr + 1;
    }
        
    util_log(1, "taking input from %s@%s", syscode[n], rmthost[n]);

/*  Set network I/O parameters  */

    beg[n]          = info->beg;
    end[n]          = info->end;
    req[n].timeout  = info->timeout;
    req[n].keep_up  = info->keep_up;
    req[n].compress = info->compress;
    req[n].cooked   = info->cooked ? util_order() : 0;

    if (req[n].timeout < 0) req[n].timeout = NRTS_DEFTCPTO;
    if (req[n].timeout < req[n].keep_up/2) req[n].timeout = 2 * req[n].keep_up;

    util_log(1, "socket i/o timeout set to %d secs", req[n].timeout);
    if (req[n].keep_up > 0) {
        util_log(1, "permanent connection requested (keep up = %d sec)",
            req[n].keep_up
        );
    } else if (req[n].keep_up == 0) {
        util_log(1, "connection will break after xfer is complete");
    } else {
        util_log(1, "configuration only (no data request to follow)");
    }

/* Get remote NRTS configuration */

    util_log(2, "requesting remote system configuration");
    status = NRTS_OK;
    while (connected[n] == 0) {
        if (
            ((sid = nrts_netconn(rmthost[n], service[n], port[n]))  > 0) &&
            ((status = nrts_recvsys(sid, syscode[n], &sys[n], req[n].timeout)) == NRTS_OK)
        ) {
            connected[n] = 1;
        } else if (status < 0) {
            util_log(1, "%s@%s server error %d: %s",
                syscode[n], rmthost[n], status, syserrmsg(errno)
            );
            return NRTS_ERROR;
        } else {
            util_log(1, "%s@%s not responding: %s",
                syscode[n], rmthost[n], syserrmsg(errno)
            );
            return NRTS_TIMEOUT;
        }
    }
    util_log(2, "remote configuration received OK");
    shutdown(sid, 2);
    close(sid);
    connected[n] = 0;

/* If keep_up was negative then all we are interested in is the remote
 * system configuration, so we are done.
 */

    if (req[n].keep_up < 0) *cnf = &sys[n];

/*  Otherwise, allocate space for raw record reads  */

    if ((rawbuf[n] = (char *) malloc(sys[n].raw.len*sizeof(char))) == NULL) {
        util_log(2, "%s: malloc: %s", fid, syserrmsg(errno));
        return NRTS_ERROR;
    }
    util_log(2, "allocated %d bytes of raw data space", sys[n].raw.len);

/* Determine system specific read function and data/host byte orders */

    if (sys[n].type == NRTS_IDA || sys[n].type == NRTS_IDA10) {
        net_read = nrts_idanetread;
    /*
    data_order = BIG_ENDIAN_ORDER;
        host_order = byte_order();
         if (req[n].compress != NRTS_NOP && host_order != data_order) {
           util_log(1, "Requested compression denied (wrong host byte order)");
          req[n].compress = NRTS_NOP;
        }
    */
    } else {
        util_log(1, "%s: unsupported system type %d", fid, sys[n].type);
        return NRTS_ERROR;
    }

    if (req[n].compress == NRTS_NOP) {
        util_log(1, "data will be transferred without compression");
    } else if (req[n].compress == NRTS_FDCMP) {
        util_log(1, "first difference compression requested");
    } else {
        util_log(1, "unknown compression code %d given", req[n].compress);
        return NRTS_ERROR;
    }

/* Clear out channel start/end times for future record keeping */

    for (i = 0; i < sys[n].nsta; i++) {
        for (j = 0; j < sys[n].sta[i].nchn; j++) {
            sys[n].sta[i].chn[j].beg = (double) -1;
            sys[n].sta[i].chn[j].end = (double) -1;
        }
    }

/* Prepare initial request message */
/* For now, we assume 1 station per system, so keeparg is just chan names */

    nsta[n] = 0;
    strcpy(req[n].syscode, syscode[n]);

    if (OLD_XFER_MAXSTA < sys[n].nsta) {
        util_log(1, "%s: too many stations, increase OLD_XFER_MAXSTA", fid);
        return NRTS_ERROR;
    }

    for (i = 0; i < NRTS_MAXSTA; i++) {
        nchn[n][i] = 0;
        for (j = 0; j < NRTS_MAXCHN; j++) {
            want[n][i][j] = 0;
        }
    }

    if (info->keeparg == NULL) {
        nsta[n] = sys[n].nsta;
        for (i = 0; i < sys[n].nsta; i++) {
            nchn[n][i] = sys[n].sta[i].nchn;
            if (OLD_XFER_MAXCHN < sys[n].sta[i].nchn) {
                util_log(1, "%s: nchan too big, increase OLD_XFER_MAXCHN", fid);
                return NRTS_ERROR;
            }
            for (j = 0; j < sys[n].sta[i].nchn; j++) {
                want[n][i][j] = 1;
            }
        }
        util_log(1, "requesting all configured channels");

    } else {
                
        ntoken = util_sparse(info->keeparg, token[n], ",:/", NRTS_MAXCHN);
        if (ntoken < 1) {
            util_log(1, "%s: illegal keeparg '%s'", fid, info->keeparg);
            return NRTS_ERROR;
        }

        for (i = 0; i < sys[n].nsta; i++) {
            if (OLD_XFER_MAXCHN < sys[n].sta[i].nchn) {
                util_log(1, "%s: nchan too big, increase OLD_XFER_MAXCHN", fid);
                return NRTS_ERROR;
            }
            tmpbuf[0] = 0;
            for (count = 0, j = 0; j < sys[n].sta[i].nchn; j++) {
                for (k = 0; k < ntoken; k++) {
                    if (strcasecmp(token[n][k], sys[n].sta[i].chn[j].name) == 0) {
                        want[n][i][j] = 1;
                        ++count;
                        sprintf(tmpbuf+strlen(tmpbuf), " %s", token[n][k]);
                    }
                }
            }
            nchn[n][i] = count;
            if (count) {
                ++nsta[n];
                util_log(1, "request %s chans %s", sys[n].sta[i].name, tmpbuf);
            }
        }

    }

    *cnf = &sys[n];
    return NRTS_OK;
}

int _update_Tap_request(n)
int n;
{
int i, j, k, l;
struct nrts_sta *sta;
struct nrts_chn *chn;
static char *fid = "_update_Tap_request";

    req[n].nsta = nsta[n];
    for (k = 0, i = 0; i < sys[n].nsta; i++) {
        sta = sys[n].sta + i;
        if (nchn[n][i] > 0) {
            strcpy(req[n].sta[k].name, sta->name);
            for (l = 0, j = 0; j < sta->nchn; j++) {
                if (want[n][i][j]) {
                    chn = sta->chn + j;
                    strcpy(req[n].sta[k].chn[l].name, chn->name);
                    if (chn->beg <= (double) 0) {
                        req[n].sta[k].chn[l].begtim = beg[n];
                    } else {
                        req[n].sta[k].chn[l].begtim = chn->end +
                                                    (double) chn->sint;
                    }
                    req[n].sta[k].chn[l].endtim = end[n];
                    ++l;
                }
            }
            if (l != nchn[n][i]) {
                util_log(1, "%s: logic error: %d != %d", fid, l, nchn[n][i]);
                return -1;
            }
            req[n].sta[k].nchn = l;
            k++;
        }
    }
    return 0;
}

static int sd[16];

int Nrts_tap_connect(n)
int n;
{

      while (connected[n] == 0) {
          util_log(2, "generating request parameters");
          if (
              (_update_Tap_request(n) == 0) &&
              ((sd[n] = nrts_netconn(rmthost[n], service[n], port[n]))  > 0) &&
              (nrts_sendreq(sd[n], &req[n], req[n].timeout) == 0)
          ) {
              connected[n] = 1;
          } else {
              util_log(1, "%s@%s not responding: %s",
                  syscode[n], rmthost[n], syserrmsg(errno)
              );
          }
      }
      return sd[n];

}

int Nrts_tap(packet, len, n)
struct nrts_packet *packet;
size_t *len;
int n;
{
int status;
u_short sval;
int slen = sizeof(u_short);
int lost_conn = 0;
struct nrts_header hd;
/*
static struct nrts_packet packet;
*/
static char *fid = "Nrts_tap";

/*
    do {
*/

    /* establish connection with server */
/*  
        while (connected[n] == 0) {
            util_log(2, "generating request parameters");
            if (
                (_update_Tap_request(n) == 0) &&
                ((sd[n] = nrts_netconn(rmthost[n], NRTS_XFERSVC))  > 0) &&
                (nrts_sendreq(sd[n], &req[n], req[n].timeout) == 0)
            ) {
                connected[n] = 1;
            } else {
                util_log(1, "%s@%s not responding: %s",
                    syscode[n], rmthost[n], syserrmsg(errno)
                );
            }
        }
*/
    /* read a raw record using system specific network read function */
        
  if(sys[n].type != NRTS_IDA && sys[n].type != NRTS_IDA10) {
     util_log(1, "%s: unsupported system type %d", fid, sys[n].type);
     return NRTS_ERROR;
  } 
        status = (*net_read)(sd[n], &(sys[n]), rawbuf[n], req[n].timeout);
    /* if requesting cooked data, follow-up with header read */
        if (req[n].cooked && status == NRTS_OK) {
            if ((status = nrts_recvhead(sd[n], &hd, req[n].timeout)) == NRTS_OK) {
                strcpy(packet->sname, hd.wfdisc.sta);
                strcpy(packet->cname, hd.wfdisc.chan);
        strcpy(packet->instype, hd.wfdisc.instype);
                packet->beg.time = hd.wfdisc.time;
                packet->beg.code = hd.beg.code;
                packet->beg.qual = hd.beg.qual;
                packet->end.time = hd.wfdisc.endtime;
                packet->end.code = hd.end.code;
                packet->end.qual = hd.end.qual;
                packet->tear     = hd.tear;
                packet->sint     = 1.0/hd.wfdisc.smprate;
        packet->calib    = hd.wfdisc.calib;
        packet->calper   = hd.wfdisc.calper;
        packet->vang     = hd.vang;
        packet->hang     = hd.hang;
                packet->nsamp    = hd.wfdisc.nsamp;
                packet->wrdsiz   = hd.wrdsiz;
                packet->order    = hd.order;
                packet->type     = sys[n].type;
                packet->hlen     = hd.hlen;
                packet->dlen     = hd.dlen;
                packet->header   = rawbuf[n];
                packet->data     = rawbuf[n] + packet->hlen;

                if (sys[n].sta[hd.standx].chn[hd.chnndx].beg < (double) 0) {
                    sys[n].sta[hd.standx].chn[hd.chnndx].beg = hd.wfdisc.time;
                }
                sys[n].sta[hd.standx].chn[hd.chnndx].end = hd.wfdisc.endtime;


            /* byte swap data into requested order if neccesary */

                if (
                    (req[n].cooked == BIG_ENDIAN_ORDER && packet->order == LTL_ENDIAN_ORDER) ||
                    (req[n].cooked == LTL_ENDIAN_ORDER && packet->order == BIG_ENDIAN_ORDER)
                ) {
                    if (packet->wrdsiz == 4) {
                        util_lswap((long *) packet->data, (long) packet->nsamp);
                        packet->order = req[n].cooked;
                    } else if (packet->wrdsiz == 2) {
                        util_sswap((short *) packet->data, (long) packet->nsamp);
                        packet->order = req[n].cooked;
                    }
                }
            }
        }

    /* quit on EOD from server unless keep up flag is set */

        if (status == NRTS_EOF) {
            if (req[n].keep_up) {
                util_log(1, "%s@%s reports no data available, will wait",
                    syscode[n], rmthost[n]
                );
                sval = htons(req[n].keep_up);
                if (util_write(sd[n], (char *) &sval, slen, req[n].timeout) != slen) {
                    if (errno != ETIMEDOUT) {
                        util_log(1, "continue ack: %s", syserrmsg(errno));
                        util_log(1, "abandoning connection");
                        /*
                        shutdown(sd[n], 2);
                        close(sd[n]);
                        */
                        return NRTS_ERROR;
                    }
                }
                status = NRTS_ERROR;
                return 3;
            } else {
                util_log(1, "EOD received from %s@%s", syscode[n], rmthost[n]);
                return NRTS_EOF;
            }

    /* re-establish the connection if it has gone away */

        } else if (status != NRTS_OK) {
            lost_conn = 1;
            util_log(1, "%s@%s not responding: %s",
                syscode[n], rmthost[n], syserrmsg(errno)
            );
            /*
            shutdown(sd[n], 2);
            close(sd[n]);
            */
            connected[n] = 0;
            status = NRTS_ERROR;
            return 4;
        }

/*
    } while (status != NRTS_OK);
*/

  if(status == NRTS_OK) {
    if (lost_conn) util_log(1, "%s@%s OK", syscode[n], rmthost[n]);
    *len    = sizeof(struct nrts_packet);
/*
    if (req[n].cooked) {
        *output = (void *) &packet;
        *len    = sizeof(struct nrts_packet);
    } else {
        *output = (void *) rawbuf[n];
        *len    = sys[n].raw.len;
    }
*/   
    return NRTS_OK;
  } else {
    return -1;
  }
}

int Nrts_closetap(n)
int n;
{

    shutdown(sd[n], 2);
    close(sd[n]);
    connected[n] = 0;

}

/* Revision History
 *
 * $Log: tap.c,v $
 * Revision 1.2  2002/04/30 16:24:39  dec
 * remove string lcases
 *
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
