#pragma ident "$Id: signals.c,v 1.8 2009/03/20 21:03:34 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_SIGNALS

static SEMAPHORE semaphore;

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
        GracefulExit(-(MY_MOD_ID + (INT32) fdwCtrlType));
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

    LogMsg(LOG_INFO, "signal handler installed");
    SEM_POST(&semaphore);

    if (!status) {
        LogMsg(LOG_ERR, "%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );
        GracefulExit(MY_MOD_ID);
    }
}

#else 

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    sigfillset(&set); /* catch all signals defined by the system */

    LogMsg(LOG_INFO, "signal handler installed");
    SEM_POST(&semaphore);

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg(LOG_INFO, "SIGTERM");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGQUIT:
            LogMsg(LOG_INFO, "SIGQUIT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGINT:
            LogMsg(LOG_INFO, "SIGINT");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            LogMsg(LOG_INFO, "SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg(LOG_INFO, "SIGUSR2");
            LogMsgLevel(LOG_INFO);
            break;

          case SIGPIPE:
            break;

          case SIGSEGV:
            LogMsg(LOG_INFO, "SIGSEGV");
            GracefulExit(-(MY_MOD_ID + sig));
            break;

          default:
            LogMsg(LOG_INFO, "signal %d ignored", sig);
            break;
        }
    }
}

#ifdef SIGWAIT_THREAD_DOES_NOT_WORK

void SignalHandler(int sig)
{
    /* process signals */

    switch (sig) {
      case SIGTERM:
        LogMsg(LOG_INFO, "SIGTERM");
        GracefulExit(-(MY_MOD_ID + sig));
        break;

      case SIGQUIT:
        LogMsg(LOG_INFO, "SIGQUIT");
        GracefulExit(-(MY_MOD_ID + sig));
        break;

      case SIGINT:
        LogMsg(LOG_INFO, "SIGINT");
        GracefulExit(-(MY_MOD_ID + sig));
        break;

      case SIGUSR1:
        LogMsg(LOG_INFO, "SIGUSR1");
        LogMsgLevel(LOG_DEBUG);
        break;

      case SIGUSR2:
        LogMsg(LOG_INFO, "SIGUSR2");
        LogMsgLevel(LOG_INFO);
        break;

      case SIGPIPE:
        break;

      case SIGSEGV:
        LogMsg(LOG_INFO, "SIGSEGV");
        GracefulExit(-(MY_MOD_ID + sig));
        break;

      default:
        LogMsg(LOG_INFO, "signal %d ignored", sig);
        break;
    } // switch on signal type

    // If we get here then signal was not fatal so return
    return;
} // SignalHandler()

#endif // SIGWAIT_THREAD_DOES_NOT_WORK

VOID StartSignalHandler(VOID)
{
int status;
THREAD tid;
sigset_t set;
static char *fid = "StartSignalHandler";

#ifdef SIGWAIT_THREAD_DOES_NOT_WORK

/*
 * Under the Monta Vista linux port to the Slate, sigwait in a pthread
 * does not receive signals sent to the controlling process.  So we use
 * signal intercept calls instead.
 */
    // SIGCHLD, SIGUSR1, and SIGALRM
    signal(SIGCHLD,  SignalHandler);
    signal(SIGUSR1,  SignalHandler);
    signal(SIGUSR2,  SignalHandler);
    signal(SIGALRM,  SignalHandler);

    signal(SIGTERM,  SignalHandler);
    signal(SIGQUIT,  SignalHandler);
    signal(SIGINT,  SignalHandler);
    signal(SIGSEGV,  SignalHandler);

    // We need to capture SIGPIPE even though we don't exit
    signal(SIGPIPE, SignalHandler);

#else // SIGWAIT_THREAD_DOES_NOT_WORK

/* Block all signals */
/* We keep the pthread specific function in place because there is no
 * easy way to emulate this under Windows.  This should not be a problem 
 * because this whole section is ifdef'd out under Windows and is known to
 * work under Solaris/Linux (where the THREAD macros are pthread based).
 */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    SEM_INIT(&semaphore, 0, 0);
    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        LogMsg(LOG_ERR, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        GracefulExit(MY_MOD_ID);
    }
    THREAD_DETACH(tid);
    SEM_WAIT(&semaphore);

#endif // SIGWAIT_THREAD_DOES_NOT_WORK not defined
}

#endif /* ifdef WIN32 */

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.8  2009/03/20 21:03:34  dechavez
 * THREAD_DETACH
 *
 * Revision 1.7  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.6  2008/02/07 22:11:13  dechavez
 * added SIGPIPE hander for Slate builds
 *
 * Revision 1.5  2008/02/01 23:17:09  dechavez
 * SIGWAIT_THREAD_DOES_NOT_WORK support (from F. Shelly, ASL)
 *
 * Revision 1.4  2007/01/11 17:55:52  dechavez
 * catch and exit on SEGV
 *
 * Revision 1.3  2004/06/30 20:44:13  dechavez
 * removed support for CTRL_LOG_OFF_EVENT (WIN32 builds)
 *
 * Revision 1.2  2004/04/26 21:18:11  dechavez
 * renamed IacpdDie to GracefulExit
 *
 * Revision 1.1  2003/10/16 18:07:25  dechavez
 * initial release
 *
 */
