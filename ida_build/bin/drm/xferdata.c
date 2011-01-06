#pragma ident "$Id: xferdata.c,v 1.1.1.1 2000/02/08 20:19:59 dec Exp $"
/*======================================================================
 *
 *  Service a data request.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "service.h"
#include "xfer.h"

int xferdata(path, server, datreq, stainfo, avail)
char *path;
char *server;
struct drm_datreq *datreq;
struct drm_sta *stainfo;
int avail;
{
int i, j, status, packetcount;
double request_begin, next_samp[DRM_MAXCHN];
FILE *fp;
XFER *xp;
long beg_time, end_time, elapsed_time, urate, nbyte, overhead, nrec;
float rate;
unsigned long send_base, recv_base, nsend, nrecv;
struct xfer_packet  packet;
struct remap map;
static char *fid = "xferdata";

/* Open file to hold raw data */

    if ((fp = fopen(path, "a+b")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

/* Load station/channel name remappings (if they exist) */

    load_stachnmap(&map);

/* Adjust request start if data already exist in this file */

    for (i = 0; i < datreq->nchn; i++) next_samp[i] = datreq->beg;

    datreq->nrec = 0;
    rewind(fp);
    while (Xfer_RdPacket(fp, &packet) == 0) {
        for (i = 0; i < datreq->nchn; i++) {
            if (strcasecmp(datreq->chn[i], packet.cname) == 0) {
                next_samp[i] = packet.end + (double) packet.sint;
                i = datreq->nchn;
            }
        }
        ++datreq->nrec;
    }

    if (datreq->nrec) {
        request_begin = next_samp[0];
        for (i = 1; i < datreq->nchn; i++) {
            if (next_samp[i] < request_begin) request_begin = next_samp[i];
        }
        util_log(1, "%ld records present from previous attempt(s)",
            datreq->nrec
        );
        util_log(1, "xfer request start time changed to %s",
            util_dttostr(request_begin, 0)
        );
    } else {
        request_begin = datreq->beg;
    }

    if (avail == DRM_DONE) return DRM_OK; /* we just needed the count */

/* Submit request to data server */

    recv_base = xfer_nrecv;
    send_base = xfer_nsend;

    beg_time  = time(NULL);
    util_log(1, "watchdog timer set to %ld seconds", stainfo->maxconn);

    xp = Xfer_Open2(
        server, 0, datreq->sc, request_begin, datreq->end,
        0, 0, stainfo->to, stainfo->tto
    );

    if (xp == (XFER *) NULL) {
        util_log(1, "request failed: %s", Xfer_ErrStr());
        Xfer_Close(xp);
        return DRM_TRYLATER;
    }

    overhead = time(NULL) - beg_time;

/* Read data packets from server */

    nbyte = nrec = 0;
    util_log(1, "begin data transfer from %s", server);

    packetcount = 0;
    while ((status = Xfer_Read(xp, &packet)) == XFER_OK) {
        if (++packetcount == 1) util_log(1, "initial packet received");

        if ((time(NULL) - beg_time) > stainfo->maxconn) {
            util_log(1, "watchdog timer expired, break connection");
            Xfer_Close(xp);
            drm_exit(DRM_WATCHDOG);
        }

    /* Remap station/channel names if mapping exists */

        if (map.nentry > 0) remap_stachn(&map, &packet);

    /* Do some checks on what we got, and write it out if OK */

        if (nrec == 0 && packet.beg > datreq->end) {
            util_log(1, "%s:%s packet begin time of %s",
                packet.sname, packet.cname,
                util_dttostr(packet.beg, 0)
            );
            util_log(1, "is later than requested end time of %s (gap?)",
                util_dttostr(datreq->end, 0)
            );
            util_log(1, "packet dropped");
        } else if (Xfer_WrtPacket(fp, &packet) == 0) {
            nrec += 1;
        } else {
            util_log(1, "%s: fatal error: Xfer_WrtPacket: %s: %s", 
                fid, path, syserrmsg(errno)
            );
            Xfer_Close(xp);
            return DRM_BAD;
        }
    }
    end_time = time(NULL);
    nsend = xfer_nsend - send_base;
    nrecv = xfer_nrecv - recv_base;

    datreq->nrec += nrec;

/* Close the connection */

    Xfer_Close(xp);
    fclose(fp);

/* Report on transfer rates seen */

    elapsed_time = end_time - beg_time;
    if (elapsed_time > 0) {
        rate  = (float) nrec / (float) elapsed_time;
        urate = (long) ((double) nbyte / elapsed_time);
        util_log(1, "%ld records received at %.2f records/sec", nrec, rate);
        util_log(1, "%ld (%ld + %ld) RAW bytes in %ld seconds = %ld bytes/sec",
            nsend + nrecv, nsend, nrecv, elapsed_time,
            (nsend + nrecv)/elapsed_time
        );
    } else {
        util_log(1, "%ld records received", nrec);
    }

/* Determine status */

    if (status != XFER_FINISHED) {
        util_log(1, "data transfer failed: %s", Xfer_ErrStr());
        return DRM_TRYLATER;
    }
    return DRM_OK;
}

/* Revision History
 *
 * $Log: xferdata.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
