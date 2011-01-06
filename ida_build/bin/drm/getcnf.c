#pragma ident "$Id: getcnf.c,v 1.2 2002/04/25 23:39:55 dec Exp $"
/*======================================================================
 *
 *  Get remote configuration, using edes service.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "service.h"
#include "xfer.h"

struct xfer_cnfnrts *get_config(server, to, tto, status)
char *server;
int to;
int tto;
int *status;
{
int sd;
struct xfer_req req;
static char *service = XFER_SERVICE;
static struct xfer_cnf cnf;

    req.protocol = 1;
    req.type     = XFER_CNFREQ;
    req.timeout  = to;
    req.preamble.ver01.format    = XFER_CNFNRTS;
    req.preamble.ver01.client_id = getpid();

    sd = Xfer_Connect2(server, service, -1, "tcp", &req, &cnf, 0, tto);
    if (sd < 0) {
        util_log(1, "%s: %s", server, Xfer_ErrStr());
        return (struct xfer_cnfnrts *) NULL;
    }

    close(sd);

    util_log(1, "configuration received OK");
    return &cnf.type.nrts;
}

/* Revision History
 *
 * $Log: getcnf.c,v $
 * Revision 1.2  2002/04/25 23:39:55  dec
 * remove log messge
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
