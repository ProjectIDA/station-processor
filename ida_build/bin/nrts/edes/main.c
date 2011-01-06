#pragma ident "$Id: main.c,v 1.8 2008/09/12 01:05:00 dechavez Exp $"
/*======================================================================
 *
 *  IDA Earthquake Data Exchange Server
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <sys/param.h>
#include "edes.h"
#include "util.h"

#ifndef   MAXHOSTNAMELEN
#define   MAXHOSTNAMELEN 64
#endif /* MAXHOSTNAMELEN */

char *Peer;
struct edes_params *EdesParams;

int main(int argc, char **argv)
{
int status;
char *home = NULL;
struct xfer_req req;
struct xfer_cnf cnf;
struct edes_req request;
static char peer[MAXHOSTNAMELEN+32];
static char buffer[XFER_MAXDAT];
static char *fid = "main";

    if ((EdesParams = (struct edes_params *) malloc(sizeof(struct edes_params))) == NULL) {
        perror("malloc");
        exit(1234);
    }

    Peer = peer;

/* Initialize */

    if ((status = init(argc, argv, &home, EdesParams, buffer)) != 0) {
        util_log(1, "init failed: status %d", status);
        util_log(1, "sending reject ack to %s", Xfer_Peer(EdesParams->isd));
        Xfer_Ack(EdesParams->osd, xfer_errno);
        Xfer_Exit(EdesParams->osd, xfer_errno);
    }

/*  Verify client access rights  */

    if (!Xfer_Allowed(EdesParams->isd)) {
        util_log(1, "refused connection with %s", Xfer_Peer(EdesParams->isd));
        util_log(1, "sending reject ack to %s", Xfer_Peer(EdesParams->isd));
        Xfer_Ack(EdesParams->osd, XFER_EREFUSED);
        Xfer_Exit(EdesParams->osd, 0);
    } else {
        util_log(2, "connected to %s", Xfer_Peer(EdesParams->isd));
    }

/* Read incoming request */

    util_log(3, "read incoming request");
    if (Xfer_RecvReq(EdesParams->isd, &req) != XFER_OK) {
        util_log(1, "%s: %s", Xfer_Peer(EdesParams->isd), Xfer_ErrStr());
        Xfer_Exit(EdesParams->osd, xfer_errno);
    }
    Xfer_LogReq(3, &req);

/* Map request into protocol independent form */

    util_log(3, "remap request into protocol independent form");
    if ((status = remap_req(&req, &request)) != 0) {
        util_log(1, "request remap failed: status %d: %s", 
            status, Xfer_ErrStr()
        );
        Xfer_Ack(EdesParams->osd, XFER_EFAULT);
        Xfer_Exit(EdesParams->osd, xfer_errno);
    }
    sprintf(Peer, "%d@%s", request.ident, Xfer_Peer(EdesParams->isd));

/* Load current configuration into output structure */

    util_log(3, "load configuration into output structure");
    if ((status = load_config(&request, &cnf, EdesParams)) != 0) {
        util_log(1, "load configuration failed: status %d: %s",
            status, Xfer_ErrStr()
        );
        Xfer_Ack(EdesParams->osd, XFER_EFAULT);
        Xfer_Exit(EdesParams->osd, xfer_errno);
    }

/* Process request */

    switch (request.type) {

      case XFER_CNFREQ:
        util_log(1, "configuration request from %s", Peer);
        util_log(2, "sending configuration to %s", Peer);
        Xfer_LogCnf(3, &cnf);
        Xfer_SendCnf(EdesParams->osd, &cnf);
        break;

      case XFER_WAVREQ:
        util_log(1, "waveform request from %s: %s",
            Peer, SetWhoWhat(&req)
        );
        if ((status = check_wavreq(&request, EdesParams)) != 0) {
            if (xfer_errno == XFER_ENOSUCH) {
#define LOOP_FOREVER_ON_EMPTY_REQUESTS
#ifdef LOOP_FOREVER_ON_EMPTY_REQUESTS
                util_log(2, "no such error... sending heatbeats only to %s", Peer);
                util_log(2, "sending configuration to %s", Peer);
                Xfer_LogCnf(3, &cnf);
                Xfer_SendCnf(EdesParams->osd, &cnf);
                while (1) {
                    util_log(2, "HEARTBEAT to %s", Peer);
                    Xfer_Ack(EdesParams->osd, XFER_HEARTBEAT);
                    util_log(2, "sleep(%d)", request.timeout/2);
                    sleep(request.timeout/2);
                }
#else
                util_log(1, "%s request rejected, no such streams available", Peer);
                while (Xfer_Ack(EdesParams->osd, xfer_errno)) sleep(15);
                Xfer_Exit(EdesParams->osd, xfer_errno);
#endif
            } else {
                util_log(1, "%s request failed: status %d: %s", Peer, status, Xfer_ErrStr());
                util_log(2, "sending reject ack to %s", Peer);
                while (Xfer_Ack(EdesParams->osd, xfer_errno)) sleep(15);
                Xfer_Exit(EdesParams->osd, xfer_errno);
            }
        }
        util_log(2, "sending configuration to %s", Peer);
        Xfer_LogCnf(3, &cnf);
        Xfer_SendCnf(EdesParams->osd, &cnf);
        wavreq(&cnf, &request, EdesParams, buffer, Peer);
        break;

      default:
        util_log(1, "unsupported request code `%d'", req.type);
        xfer_errno = XFER_EREQUEST;
        while (Xfer_Ack(EdesParams->osd, xfer_errno)) sleep(15);
        Xfer_Exit(EdesParams->osd, xfer_errno);
    }

    Xfer_Exit(EdesParams->osd, 0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.8  2008/09/12 01:05:00  dechavez
 * dynamically allocate EdesParams to eliminate off the bat core dump in FreeBSD
 *
 * Revision 1.7  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.6  2004/09/28 23:33:18  dechavez
 * support empty requests (send heartbeats, letting client be the one to
 * terminate)
 *
 * Revision 1.5  2003/05/05 23:19:23  dechavez
 * respond to improper requests with slow Xfer_Ack loops
 *
 * Revision 1.4  2002/04/25 22:00:09  dec
 * use SetWhoWhat to get request string
 *
 * Revision 1.3  2002/04/02 01:26:32  nobody
 * use xferWavreqString to log requested stas/chans
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
