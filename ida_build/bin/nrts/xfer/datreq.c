#pragma ident "$Id: datreq.c,v 1.4 2006/02/10 02:04:00 dechavez Exp $"
/*======================================================================
 *
 *  Service a request for data packets.
 *
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "nrts.h"
#include "nrts_xfer.h"
#include "ida.h"
#include "util.h"

static int sd;
static int timeout;

struct nrts_sys sys;
struct old_xfer_req req;
static char buffer[MAX_BUFSIZ];
static char client_id[1024];
int total = 0;
int (*xfer)() = NULL;
IDA *ida;

int do_datreq(char *client, char *home, int ifd, int ofd, int to, char *dbdir)
{
u_short sval;
int slen = sizeof(u_short);
int i, j, done, rev;
char *syscode, *mapname;
int sindex, cindex, pending;
time_t prev_time, lapse;
struct nrts_files *file;
struct nrts_sta *sta;
struct nrts_chn *chn;
struct old_xfer_sta *rsta;
struct old_xfer_chn *rchn;
struct nrts_mmap map;
struct nrts_ida *info;
DBIO *db;
static char *fid = "do_datreq";

    init_rate();

    sd = ofd;
    timeout = to;

/* Get data request parameters  */

    if (nrts_recvreq(ifd, &req, to) != 0) {
        util_log(1, "client not responding: %s", syserrmsg(errno));
        die(1);
    }
    timeout = to = (int) req.timeout;
    req.client = client;

    sprintf(client_id, "pid%ld@%s", req.pid, req.client);
    util_log(1, "data request from %s", client_id);
    nrts_prtreq(3, &req);

/*  Load the database */

    if ((db = dbioOpen(dbspec, NULL)) == NULL) {
        util_log(1, "%s: can't load database `%s': %s", fid, dbdir, syserrmsg(errno)
        ack(errno);
        die(1);
    }
    util_log(2, "`%s' database loaded successfully", dbdir);

/* Open the disk buffer files */

    file = nrts_files(&home, req.syscode);

    if ((req.hfd = open(file->hdr, O_RDONLY)) < 0) {
        util_log(1, "%s: %s: %s", fid, file->hdr, syserrmsg(errno));
        util_log(1, "%s: can't open hdr disk buffer", fid);
        ack(errno);
        die(0);
    }

    if ((req.dfd = open(file->dat, O_RDONLY)) < 0) {
        util_log(1, "%s: %s: %s", fid, file->dat, syserrmsg(errno));
        util_log(1, "%s: can't open dat disk buffer", fid);
        ack(errno);
        die(0);
    }

/*  Get static copy of system info and xfer function  */

    util_log(3, "getting static system information");
    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) != 0) {
        util_log(1, "can't get system map: %s", syserrmsg(errno));
        ack(errno);
        die(1);
    }

    sys = *((struct nrts_sys *) map.ptr);

    if (sys.type == NRTS_IDA) {
        info = (struct nrts_ida *) sys.info;
        rev = info->rev;
        mapname = info->map;
        xfer = (int (*)()) nrts_idanetwrite;
        req.decode = (struct nrts_header *(*)())
                     nrts_inidecode(home, req.syscode, &sys, 0, ida, 0, req.hfd);
        if (req.decode == NULL) {
            util_log(1, "unable to determine decode function");
            ack(errno);
            die(1);
        }
    } else if (sys.type == NRTS_IDA10) {
        rev = 10;
        mapname = NULL;
        xfer = (int (*)()) nrts_idanetwrite;
        req.decode = (struct nrts_header *(*)())
                     nrts_inidecode(home, req.syscode, &sys, 0, ida, 0, req.hfd);
        if (req.decode == NULL) {
            util_log(1, "unable to determine decode function");
            ack(errno);
            die(1);
        }
    } else {
        util_log(1, "%s: unsuported system type '%d'",
            fid, sys.type
        );
        ack(EINVAL);
        die(0);
    }

    if ((ida = idaCreateHandle(sys->sta[0].name, rev, mapname, db, NULL, 0)) == NULL) {
        util_log(1, "idaCreateHandle failed");
        ack(errno);
        die(1);
    }

/* Get station and channel disk loop indices and check request validity  */

    for (pending = 0, i = 0; i < req.nsta; i++) {
        rsta = req.sta + i;
        rsta->sysndx = nrts_standx(&sys, rsta->name);
        if (rsta->sysndx < 0) {
            util_log(1, "no sys info for station %s(!?)", rsta->name);
            ack(EINVAL);
            die(0);
        }
        sta = sys.sta + rsta->sysndx;
        for (j = 0; j < rsta->nchn; j++) {
            rchn = rsta->chn + j;
            rchn->sysndx = nrts_chnndx(sta, req.sta[i].chn[j].name);
            if (rchn->sysndx < 0) {
                util_log(1, "no channel %s for station %s",
                    rchn->name, rsta->name
                );
                ack(EINVAL);
                die(0);
            }
            chn = sta->chn + rchn->sysndx;
            if (
                (
                    (rchn->begtim >  (double)  0) &&
                    (rchn->endtim >  (double)  0) &&
                    (rchn->begtim > rchn->endtim)
                ) || (
                    (rchn->endtim == NRTS_OLDEST) &&
                    (rchn->begtim != NRTS_OLDEST)
                ) || (
                    (rchn->begtim == NRTS_YNGEST) &&
                    (rchn->endtim != NRTS_YNGEST)
                )
            ) {
                util_log(1, "illegal request, beg > end for (%d, %d)",
                    i, j
                );
                ack(EINVAL);
                die(0);
            }
            if (
                (rchn->begtim < (double) 0) &&
                (rchn->begtim != NRTS_OLDEST) &&
                (rchn->begtim != NRTS_YNGEST)
            ) {
                util_log(1, "illegal request, beg < 0 (%d, %d)", i, j);
                ack(EINVAL);
                die(0);
            }
            if (
                (rchn->endtim < (double) 0) &&
                (rchn->endtim != NRTS_OLDEST) &&
                (rchn->endtim != NRTS_YNGEST)
            ) {
                util_log(1, "illegal request, end < 0 (%d, %d)", i, j);
                ack(EINVAL);
                die(0);
            }
            rchn->count = 0;
            getindex(&req, chn, rchn, buffer);
            if (!rchn->done) ++pending;
            rchn->nrec  = 0;
        }
    }

/* Send positive ack to client */

    util_log(2, "request accepted, return postive ack");
    if (ack(0) != 0) die(0);

/*  Send over the data  */

    util_log(2, "begin data transfer");
    while (1) {
        done = 0;
        while (!done) {
            util_log(3, "pending = %d", pending);
            if (pending) {
                xfer_data();
                pending = 0;
            }
            if (req.keep_up > 0) {
                prev_time = time(NULL);
                do {
                    sleep(5);
                    sys = *((struct nrts_sys *) map.ptr);
                    util_log(3, "determining indices for newly acquired data");
                    for (i = 0; i < req.nsta; i++) {
                        rsta = req.sta + i;
                        sta  = sys.sta + rsta->sysndx;
                        for (j = 0; j < rsta->nchn; j++) {
                            rchn = rsta->chn + j;
                            chn  = sta->chn + rchn->sysndx;
                            getindex(&req, chn, rchn, buffer);
                            if (!rchn->done) ++pending;
                        }
                    }
                    lapse = time(NULL) - prev_time;
                } while (pending == 0 && lapse < req.keep_up);
            }
            if (pending == 0) done = 1;
        }
    
    /*  Tell client no more data are coming  */
    
        if (req.keep_up) util_log(2, "advising client no data are pending");
        if ((*xfer)(ofd, &sys, NULL, req.compress, to, ida) != NRTS_OK) {
            util_log(1, "%s not responding: %s",
                client_id, syserrmsg(errno)
            );
            util_log(2, "EOD notification failed");
            die(1);
        }
    
        if (total != 0) {
            do_rate(NULL, NULL, 0, client_id);
            total = 0;
        }

        if (req.keep_up > 0) {
            if (util_read(ifd, (char *) &sval, slen, timeout) != slen) {
                util_log(1, "%s not responding: %s",
                    client_id, syserrmsg(errno)
                );
                die(1);
            }
            if ((req.keep_up = ntohs(sval)) > 0) {
                util_log(2, "client wishes to continue connection");
            } else {
                util_log(1, "breaking connection at client request");
                die(0);
            }
        } else {
            die(0);
        }
    }
}

int ack(val)
int val;
{
int slen = sizeof(u_short);
u_short sval;
static char *fid = "ack";

    if (val != 0) util_log(1, "ack %d (%s)", val, syserrmsg(val));
    sval = htons((short) val);
    if (util_write(sd, (char *) &sval, slen, timeout) != slen) {
        util_log(1, "%s not responding: %s",
            client_id, syserrmsg(errno)
        );
        return -1;
    }

    return 0;
}

void xfer_data(void)
{
int i, j, status, count, dummy;
char *record;
struct nrts_sta *sta;
struct nrts_chn *chn;
struct old_xfer_sta *rsta;
struct old_xfer_chn *rchn;
struct timeval begtm, endtm;
struct nrts_header *hd;
static struct timezone tz = {0, 0};
static char *fid = "xfer_data";

    do {
        for (count = 0, i = 0; i < req.nsta; i++) {
            rsta = req.sta  + i;
            sta  = sys.sta + rsta->sysndx;
            for (j = 0; j < rsta->nchn; j++) {
                rchn = rsta->chn + j;
                chn  = sta->chn + rchn->sysndx;
                if (!rchn->done) {
                    record = getrec(&sys, chn, &req, rchn, buffer);
                    if (record != NULL) {
                        gettimeofday(&begtm, &tz);
                        status =
                        (*xfer)(sd, &sys, record, req.compress, timeout, ida);
                        if (status != NRTS_OK) {
                            util_log(1, "%s not responding: %s",
                                client_id, syserrmsg(errno)
                            );
                            util_log(2, "data transfer failed");
                            die(1);
                        }
                        gettimeofday(&begtm, &tz);
                        do_rate(&begtm, &endtm, sys.raw.len, client_id);
                        ++count;
                        if (++total == 1024) {
                            do_rate(NULL, NULL, 0, client_id);
                            total = 0;
                        }
                        if (req.cooked) {
                            hd = (*req.decode)(record, chn->hdr.len, &dummy);
                            if (hd == NULL) {
                                util_log(1, "%s: can't decode header", fid);
                                die(1);
                            }
                            if (++rchn->nrec == 1) hd->tear = 0.0;
                            if (nrts_sendhead(sd, hd, timeout) != NRTS_OK) {
                                util_log(1, "%s not responding: %s",
                                    client_id, syserrmsg(errno)
                                );
                                util_log(2, "data transfer failed");
                                die(1);
                            }
                        }
                    }
                }
            }
        }
    } while (count != 0);
}

/* Revision History
 *
 * $Log: datreq.c,v $
 * Revision 1.4  2006/02/10 02:04:00  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2002/04/30 16:39:18  dec
 * get file names with nrts_files before opening disk loop files (how the
 * hell this worked before is beyond me...)
 *
 * Revision 1.2  2002/04/26 23:15:47  dec
 * pass hdr file descriptor to nrts_inidecode
 *
 * Revision 1.1.1.1  2000/02/08 20:20:16  dec
 * import existing IDA/NRTS sources
 *
 */
