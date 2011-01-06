#pragma ident "$Id: restart.c,v 1.2 2001/10/24 23:11:51 dec Exp $"
/*======================================================================
 *
 * Restart
 *
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_RESTART

static char **MyArgv;
static char **MyEnvp;

void ForcedRestart()
{
int child, status;
static char *fid = "ForcedRestart";

    if ((child = fork()) < 0) {
        util_log(1, "%s: fork: %s", fid, strerror(errno));
        status = MY_MOD_ID + 1;
        util_log(1, "%s: exit %d", fid, status);
        exit(status);
    } else if (child > 0) {
util_log(1, "%s: child pid = %d", fid, child);
        status = MY_MOD_ID + 2;
util_log(1, "%s: sleep 60", fid);
sleep(60);
        util_log(1, "%s: exit %d", fid, status);
        exit(status);
    } else {
sleep(10);
util_log(1, "%s(C): I am child, about to execve", fid);
util_log(1, "%s(C): dump MyArgv", fid);
while (*MyArgv != NULL) {
    util_log(1, "%s(C): %s", fid, *MyArgv);
    ++MyArgv;
}
util_log(1, "%s(C): dump MyArgv completed", fid);
util_log(1, "%s(C): dump MyEnvp", fid);
while (*MyEnvp != NULL) {
    util_log(1, "%s(C): `%s'", fid, *MyEnvp);
    ++MyEnvp;
}
util_log(1, "%s(C): dump MyEnvp completed", fid);
        execve(MyArgv[0], MyArgv, MyEnvp);
        util_log(1, "%s(C): execve: %s", fid, strerror(errno));
        status = MY_MOD_ID + 3;
        util_log(1, "%s(C): exit %d", fid, status);
        exit(status);
    }
}

void SaveCommandLine(int numargs, char **arglist, char **envlist)
{
int i, j, numenv;
static char *fid = "SaveCommandLine";

    if ((MyArgv = (char **) malloc(sizeof(char **)*(numargs+1))) == NULL) {
        fprintf(stderr, "%s: malloc: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 4);
    }

    for (i = 0; i < numargs; i++) {
        if ((MyArgv[i] = strdup(arglist[i])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            exit(MY_MOD_ID + 5);
        }
    }
    MyArgv[numargs] = NULL;

    numenv = 0;
    while (envlist[numenv] != NULL) ++numenv;
    if ((MyEnvp = (char **) malloc(sizeof(char **)*(numenv+1))) == NULL) {
        fprintf(stderr, "%s: malloc: %s\n", fid, strerror(errno));
        exit(MY_MOD_ID + 6);
    }

    for (i = 0, j = 0; i < numenv; i++) {
        if (strlen(envlist[i]) > 0) {
            if ((MyEnvp[j++] = strdup(envlist[i])) == NULL) {
                fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
                exit(MY_MOD_ID + 6);
            }
        }
    }
    MyEnvp[j] = NULL;
}


/* Revision History
 *
 * $Log: restart.c,v $
 * Revision 1.2  2001/10/24 23:11:51  dec
 * corrected some initialization errors, still doesn't work and will
 * likely be abandoned
 *
 * Revision 1.1  2001/10/08 18:16:14  dec
 * introduced, still doesn't work
 *
 */
