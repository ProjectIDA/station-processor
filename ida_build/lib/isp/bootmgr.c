#pragma ident "$Id: bootmgr.c,v 1.1.1.1 2000/02/08 20:20:27 dec Exp $"
/*======================================================================
 *
 *  Interact with bootmgr service (reboot/shutdown).
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "isp.h"

int isp_bootmgr(int level)
{
int sd;
short stmp;
static char *service = ISP_BOOTMGR;
int port = ISP_BOOTMGR_PORT;
static char *fid = "isp_reboot";

    if (service == (char *) NULL && port <= 0) {
        util_log(1, "%s: service and port both undefined", fid);
        errno = EINVAL;
        return -1;
    }

    if (level != 0 && level != 6) {
        util_log(1, "%s: illegal init level (%d)", fid, level);
        errno = EINVAL;
        return -2;
    }

    sd = util_connect("localhost", service, port, "tcp", 0, 0);
    if (sd < 1) {
        util_log(1, "%s: util_connect: %s", fid, syserrmsg(errno));
        return -3;
    }

    stmp = htons(level);
    if (write(sd, &stmp, 2) != 2) {
        util_log(1, "%s: write: %s", fid, syserrmsg(errno));
        return -4;
    }

    return 0;
}

/* Revision History
 *
 * $Log: bootmgr.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
