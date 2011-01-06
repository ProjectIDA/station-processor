#pragma ident "$Id: main.c,v 1.3 2008/10/10 22:47:24 dechavez Exp $"
/*======================================================================
 * 
 * isid - IDA Station Interface Daemon
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_MAIN

static IACP *server;

VOID DisableNewConnections(VOID)
{
    iacpSetDisabled(server, TRUE);
}

/* Main program loop */

static THREAD_FUNC MainThread(MainThreadParams *cmdline)
{
PARAM *par;
IACP *client, *server;
static char buf[128];

/* Load the run time parameters */

    par = LoadPar(cmdline->myname, cmdline->argc, cmdline->argv);

/* Initialize everything */

    server = Init(cmdline->myname, par);

/* Ready to begin */

    LogMsg(LOG_INFO, "listening for ISI connections at port %d\n", par->port);

    while (1) {
        BlockOnShutdown();
        if ((client = iacpAccept(server)) != (IACP *) NULL) {
            if (!ShutdownInProgress()) {
                LogMsg(LOG_DEBUG, "%s: connection established", client->peer.ident);
                ServiceConnection(client);
            } else {
                iacpDisconnect(client, IACP_ALERT_SHUTDOWN);
            }
        } else {
            LogMsg(LOG_WARN, "incoming ISI connection failed: %s (ignored)", strerror(errno));
        }
    }
}

#ifdef WIN32_SERVICE
#   include "win32svc.c"
#else

/* Or just run it like a regular console app or Unix program */

#ifdef unix
int main(int argc, char **argv)
#else
VOID main(int argc, char **argv)
#endif
{
MainThreadParams cmdline;

    cmdline.argc   = argc;
    cmdline.argv   = argv;
    cmdline.myname = argv[0];

    MainThread(&cmdline);
}

#endif /* WIN32_SERVICE */

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.2  2003/11/04 00:23:26  dechavez
 * tuned log levels
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
