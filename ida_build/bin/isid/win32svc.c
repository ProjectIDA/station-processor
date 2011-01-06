#pragma ident "$Id: win32svc.c,v 1.2 2003/10/16 21:08:14 dechavez Exp $"
/*======================================================================
 * 
 * Run as a WIN32 Service
 *
 *====================================================================*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "isid.h"

static char *SERVICE_NAME = "ISIService";
static char *myname = "isid";

static HANDLE                termEvent           = NULL;
static SERVICE_STATUS_HANDLE serviceStatusHandle = 0;
static THREAD                threadHandle        = NULL;
extern VOID GracefulExit(DWORD error);

/* General purpose SCM reports */

static BOOL NotifySCM(
    DWORD dwCurrentState,
    DWORD dwWin32ExitCode, 
    DWORD dwServiceSpecificExitCode,
    DWORD dwCheckPoint,
    DWORD dwWaitHint
) {
SERVICE_STATUS status;

    status.dwServiceType  = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = dwCurrentState;

    if (dwCurrentState == SERVICE_START_PENDING) {
        status.dwControlsAccepted = 0;
    } else {
        status.dwControlsAccepted  = SERVICE_ACCEPT_STOP;
        status.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
        status.dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    }

    if (dwServiceSpecificExitCode == 0) {
        status.dwWin32ExitCode = dwWin32ExitCode;
    } else {
        status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    }
    status.dwServiceSpecificExitCode = dwServiceSpecificExitCode;

    status.dwCheckPoint = dwCheckPoint;
    status.dwWaitHint   = dwWaitHint;

    if (!SetServiceStatus(serviceStatusHandle, &status)) {
        GracefulExit(MY_MOD_ID + 1);
        return FALSE;
    }

    return TRUE;
}

/* Graceful exits */

static VOID GracefulExit(DWORD error)
{

/* Bring down the application threads */

    IacpdDie(error); /* this doesn't exit() for WIN32_SERVICEs */

/* Close termination event if it exists */

    if (termEvent != NULL) CloseHandle(termEvent);

/* Notify SCM that we are going down */

    if (serviceStatusHandle) {

        NotifySCM(SERVICE_STOPPED, error, 0, 0, 0);
    }

    if (threadHandle != NULL) {
        THREAD_EXIT(&threadHandle);
    }
}

/* Deal with events received from the SCM */

static VOID Handler(DWORD controlCode) 
{
DWORD currentState = 0;

    switch (controlCode) {

      case SERVICE_CONTROL_STOP:
      case SERVICE_CONTROL_SHUTDOWN:
        currentState = SERVICE_STOP_PENDING;
        NotifySCM(
            SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000
        );
        SetEvent(termEvent);
        return;

      case SERVICE_CONTROL_INTERROGATE:
        NotifySCM(currentState, NO_ERROR, 0, 0, 0);
        return;

/* We'll never really pause, but let's pretend we do for SCM's sake */

      case SERVICE_CONTROL_PAUSE:
        NotifySCM(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
        Sleep(250);
        NotifySCM(SERVICE_PAUSED, NO_ERROR, 0, 0, 0);
        return;

/* Since we never pause, we'll never continue... pretend again for SCM */

      case SERVICE_CONTROL_CONTINUE:
        NotifySCM(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
        Sleep(250);
        NotifySCM(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
        return;
    }
}

/* Start the service */

static VOID ServiceMain(DWORD argc, LPTSTR *argv) 
{
MainThreadParams thread_params;

/*  Register with SCM */

    serviceStatusHandle = RegisterServiceCtrlHandler(
        SERVICE_NAME, (LPHANDLER_FUNCTION) Handler
    );

    if (!serviceStatusHandle) {
        GracefulExit(MY_MOD_ID + 2);
        return;
    }

/* Notify SCM of progress */

    if (!NotifySCM(SERVICE_START_PENDING, NO_ERROR, 0, 1, 1000)) {
        GracefulExit(MY_MOD_ID + 3); 
        return;
    }

/* Create the termination event object */

    if (!(termEvent = CreateEvent(0, TRUE, FALSE, 0))) {
        GracefulExit(MY_MOD_ID + 4);
        return;
    }

/* Notify SCM of progress */

    if (!NotifySCM(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000)) {
        GracefulExit(MY_MOD_ID + 5);
        return;
    }

/* Start the service thread */

    thread_params.argc   = (int) argc;
    thread_params.argv   = (char **) argv;
    thread_params.myname = myname;

    if (!THREAD_CREATE(&threadHandle, MainThread, &thread_params)) {
        GracefulExit(MY_MOD_ID + 6);
    }

/* Notify SCM of progress */

    if (!NotifySCM(SERVICE_RUNNING, NO_ERROR, 0, 0, 0)) {
        GracefulExit(MY_MOD_ID + 7);
        return;
    }

/* Wait for stop signal, and then terminate */

    WaitForSingleObject (termEvent, INFINITE);

    GracefulExit(0);
}

VOID main(VOID)
{
SERVICE_TABLE_ENTRY serviceTable[] = { 
    { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
    { NULL, NULL }
};

/* This really needs to have error checking and a message
 * written to the event log if it fails.  Since I don't know
 * how to write to the event log, I don't even bother to check
 * the status as there is no other way to report failures.
 * Such events will be evident right away.
 */

    StartServiceCtrlDispatcher(serviceTable);
}

/* Revision History
 *
 * $Log: win32svc.c,v $
 * Revision 1.2  2003/10/16 21:08:14  dechavez
 * corrected program name
 *
 * Revision 1.1  2003/10/16 18:07:25  dechavez
 * initial release
 *
 */
