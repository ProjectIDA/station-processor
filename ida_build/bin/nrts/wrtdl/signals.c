#pragma ident "$Id: signals.c,v 1.3 2005/05/23 20:56:51 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "wrtdl.h"

#ifdef WIN32

/* Windows creates a thread that runs our signal handler */

static BOOL CtrlHandler(DWORD fdwCtrlType)
{     
BOOL retval;

    switch (fdwCtrlType) { 
      case CTRL_C_EVENT:  
      case CTRL_SHUTDOWN_EVENT:          
      case CTRL_CLOSE_EVENT:              
      case CTRL_BREAK_EVENT:          
/*        GracefulExit(0);*/
        die(0); /* just a plug*/
        retval = TRUE;  
        break;
      default:              
        retval = FALSE; 
    } 
    return retval;
}

VOID StartSignalHandler(VOID)
{
BOOL status;  
static char *fid = "StartSignalHandler";

/* Register our CtrlHandler function */

    status = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  /* handler function */
        TRUE                             /* add to list */
    );

/*    LogMsg(LOG_INFO, "signal handler installed");*/

    if (!status) {
/*        LogMsg(LOG_ERR, "%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );*/
/*        GracefulExit(MY_MOD_ID);*/
        exit(0); /* just a plug*/
    }
}

#else 


static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    util_log(1, "signal handler installed");

    sigfillset(&set); /* catch all signals defined by the system */
    sigdelset(&set, SIGHUP); /* so we don't clutter up the log when
                              * old nrts_dwd's are running
                              */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);
        util_log(1, "%s", util_sigtoa(sig));

        /* process signals */

        switch (sig) {
          case SIGTERM:
          case SIGQUIT:
          case SIGINT:
            die(0);
            break;
          default:
            util_log(1, "signal %d ignored", sig);
            break;
        }
    }
}

VOID StartSignalHandler(VOID)
{
int status;
THREAD tid;
sigset_t set;
static char *fid = "StartSignalHandler";

/* Block all signals */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        util_log(1, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        exit(1);
    }
}
#endif
/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.3  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.2  2003/12/23 21:14:09  dechavez
 * ignore SIGHUPs
 *
 * Revision 1.1  2003/12/22 22:12:48  dechavez
 * created
 *
 */
