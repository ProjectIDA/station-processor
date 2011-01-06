#pragma ident "$Id: signals.c,v 1.8 2008/02/08 16:05:43 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_SIGNALS

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
        Exit(-(MY_MOD_ID + (INT32) fdwCtrlType));
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

    LogMsg(LOG_DEBUG, "signal handler installed");

    if (!status) {
        LogMsg(LOG_INFO, "%s: SetConsoleCtrlHandler: %lu",
            fid, GetLastError( )
        );
        Exit(MY_MOD_ID);
    }
}

#else 

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    LogMsg(LOG_DEBUG, "signal handler installed");

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg(LOG_INFO, "SIGTERM");
            Exit(-(MY_MOD_ID + sig));
            break;

          case SIGQUIT:
            LogMsg(LOG_INFO, "SIGQUIT");
            Exit(-(MY_MOD_ID + sig));
            break;

          case SIGHUP:
            LogMsg(LOG_INFO, "SIGHUP");
            ToggleISIDebugState();
            ToggleBarometerDebugState();
            break;

          case SIGINT:
            LogMsg(LOG_INFO, "SIGINT");
            Exit(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            LogMsg(LOG_INFO, "SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg(LOG_INFO, "SIGUSR2");
            LogMsgLevel(LOG_INFO);
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
        Exit(-(MY_MOD_ID + sig));
        break;

      case SIGQUIT:
        LogMsg(LOG_INFO, "SIGQUIT");
        Exit(-(MY_MOD_ID + sig));
        break;

      case SIGINT:
        LogMsg(LOG_INFO, "SIGINT");
        Exit(-(MY_MOD_ID + sig));
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
        Exit(-(MY_MOD_ID + sig));
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
    signal(SIGPIPE,  SignalHandler);

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

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: %s", fid, strerror(errno));
        Exit(MY_MOD_ID);
    }
#endif // SIGWAIT_THREAD_DOES_NOT_WORK not defined
}

#endif /* ifdef WIN32 */

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.8  2008/02/08 16:05:43  dechavez
 * added SIGPIPE handler for Slate builds
 *
 * Revision 1.7  2008/02/02 07:13:08  dechavez
 * Fixed incorrect name of exit handler for ARM_SLATE builds
 *
 * Revision 1.6  2008/01/14 16:57:54  dechavez
 * SIGWAIT_THREAD_DOES_NOT_WORK support (from F. Shelly, ASL)
 *
 * Revision 1.5  2007/02/08 22:52:18  dechavez
 * LOG_ERR to LOG_INFO
 *
 * Revision 1.4  2006/03/30 22:02:18  dechavez
 * added ToggleBarometerDebugState() to SIGHUP handler
 *
 * Revision 1.3  2005/09/30 22:29:56  dechavez
 * ToggleISIDebugState() on SIGHUP
 *
 * Revision 1.2  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
