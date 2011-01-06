#pragma ident "$Id: signals.c,v 1.3 2009/05/14 18:12:44 dechavez Exp $"
/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_SIGNALS

static QHLP_PAR *par = NULL;

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

void StartSignalHandler(QHLP_PAR *arg)
{
BOOL status;  
static char *fid = "StartSignalHandler";

    par = arg;

/* Register our CtrlHandler function */

    status = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  /* handler function */
        TRUE                             /* add to list */
    );

    LogDebug("signal handler installed");

    if (!status) {
        LogMsg("%s: SetConsoleCtrlHandler: %lu",
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

    LogDebug("signal handler installed");

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            LogMsg("SIGTERM");
            SetShutdownFlag();
            break;

          case SIGQUIT:
            LogMsg("SIGQUIT");
            SetShutdownFlag();
            break;

          case SIGHUP:
            LogMsg("SIGHUP");
            UpdateDebugState(par);
            break;

          case SIGINT:
            LogMsg("SIGINT");
            SetShutdownFlag();
            break;

          case SIGUSR1:
            LogMsg("SIGUSR1");
            LogMsgLevel(LOG_DEBUG);
            break;

          case SIGUSR2:
            LogMsg("SIGUSR2");
            LogMsgLevel(LOG_INFO);
            break;

          default:
            LogMsg("signal %d ignored", sig);
            break;
        }
    }
}

void StartSignalHandler(QHLP_PAR *arg)
{
int status;
THREAD tid;
sigset_t set;
static char *fid = "StartSignalHandler";

    par = arg;

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
        LogMsg("%s: THREAD_CREATE: %s", fid, strerror(errno));
        Exit(MY_MOD_ID);
    }
}

#endif /* ifdef WIN32 */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: signals.c,v $
 * Revision 1.3  2009/05/14 18:12:44  dechavez
 * added debug verbosity toggling via /usr/nrts/etc/debug
 *
 * Revision 1.2  2009/01/26 21:11:28  dechavez
 * added SIGHUP handlers to toggle watchdog logging
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
