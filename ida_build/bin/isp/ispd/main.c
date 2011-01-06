#pragma ident "$Id: main.c,v 1.6 2002/03/15 22:51:37 dec Exp $"
/*======================================================================
 *
 *  ISP daemon, a Solaris based implementation of the IDA MK7 ARS.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "ispd.h"

/* Global variables */

char *Home                  = (char *) NULL;
char *Syscode               = (char *) NULL;
struct nrts_sys *Nrts       = (struct nrts_sys *) NULL;
struct ispd_queue   *Q      = (struct ispd_queue *) NULL;
struct ispd_heap    *Heap   = (struct ispd_heap *) NULL;
struct isp_dascnf   *DasCnf = (struct isp_dascnf  *) NULL;
struct isp_status   *Status = (struct isp_status  *) NULL;
struct isp_reftime  *Reftime;
ISP_SERVER *Server = (ISP_SERVER *) NULL;
ISP_PARAMS *Params = (ISP_PARAMS *) NULL;

int main(int argc, char **argv, char **envp)
{
struct sockaddr_in cli_addr;
int client, len = sizeof(cli_addr);
struct isp_status status;
ISP_SERVER server;
ISP_PARAMS params;
struct isp_dascnf dascnf;
struct ispd_heap heap;
struct ispd_queue q;
struct isp_reftime reftime = {-1, -1, -1, MUTEX_INITIALIZER};
SANIO_CONFIG sancnf;
static char *fid = "main";

    SaveCommandLine(argc, argv, envp);

    Params  = &params;
    Status  = &status;
    DasCnf  = &dascnf;
    Server  = &server;
    Reftime = &reftime;
    Heap    = &heap;
    Q       = &q;

/* Start up data acquistition */

    init(argc, argv);

/* Spend eternity waiting for and servicing client requests */

    while (1) {
        client = accept(Server->sd, (struct sockaddr *) &cli_addr, &len);
        if (client < 0 && errno != EINTR) {
            util_log(1, "%s: accept: %s", fid, syserrmsg(errno));
            shutdown(client, 2);
            close(client);
        } else if (client >= 0) {
            serve(client);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.6  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.5  2001/10/08 18:17:13  dec
 * added SaveCommandLine call
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/08 01:57:24  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.2  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
