#pragma ident "$Id: rcp.c,v 1.1.1.1 2000/02/08 20:19:59 dec Exp $"
/*======================================================================
 *
 *  Use a coprocess to rcp all the files in the current directory.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "service.h"

#ifndef RCP
#define RCP "/usr/ucb/rcp"
#endif

#ifndef RSH
#define RSH "/usr/ucb/rsh"
#endif

#ifndef EVENT_SUM
#define EVENT_SUM "event_sum"
#endif

int do_rsh(ident)
char *ident;
{
pid_t pid;
int status;
struct drm_cnf *cnf;
static char *fid = "do_rsh";
    
    cnf = drm_cnf();

    util_log(1, "rsh -n -l %s %s %s %s",
        cnf->spyder_user, cnf->spyder_host, EVENT_SUM, ident
    );

    if ((pid = fork()) < 0) return -1;

    if (pid > 0) { /* parent */
        return 0;
    } else {
        execl(RSH, RSH, "-n", "-l", cnf->spyder_user, cnf->spyder_host, 
              EVENT_SUM, ident, NULL
        );
    }
}

int do_rcp(fname, address)
char *fname;
char *address;
{
pid_t pid;
int status;
static char *fid = "do_rcp";
    
    util_log(1, "rcp %s %s", fname, address);

    if ((pid = fork()) < 0) return -1;

    if (pid > 0) { /* parent */

        if (waitpid(pid, &status, 0) != pid) {
            util_log(1, "%s: waitpid: %s", fid, syserrmsg(errno));
            return -6;
        }

        if (WIFEXITED(status)) return WEXITSTATUS(status);

        return -2;

    } else {
        execl(RCP, RCP, fname, address, NULL);
    }
}

int drm_rcp(datreq)
struct drm_datreq *datreq;
{
int count, status;
DIR *dirp;
struct dirent *dp;
struct stat sbuf;
static char *fid = "drm_rcp";

    if ((dirp = opendir(".")) == NULL) {
        util_log(1, "%s: fatal error: can't opendir `.': %s",
            fid, syserrmsg(errno)
        );
        return DRM_RCPFAIL;
    }

    count = 0;
    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (stat(dp->d_name, &sbuf) != 0) {
                util_log(1, "%s: error: can't stat `./%s: %s'",
                    fid, dp->d_name, syserrmsg(errno)
                );
                closedir(dirp);
                return DRM_RCPFAIL;
            }
            if (S_ISREG(sbuf.st_mode)) {
                status = do_rcp(dp->d_name, datreq->remote.address);
                if (status != 0) {
                    util_log(1, "%s: rcp failed, status %d", fid, status);
                    closedir(dirp);
                    return DRM_RCPFAIL;
                } else {
                    if (unlink(dp->d_name) != 0) {
                        util_log(1, "%s: warning: can't unlink %s: %s",
                            fid, dp->d_name, syserrmsg(errno)
                        );
                    }
                }
                ++count;
            }
        }
    }
    closedir(dirp);

    if (count < 1) {
        util_log(1, "%s: fatal error: no files transferred!", fid);
        return DRM_RCPFAIL;
#ifdef DOEVENTSUM
    } else if (datreq->type == DRM_SPYDER) {
        do_rsh(datreq->ident);
#endif
    }

    return DRM_DELIVERED;
}

/* Revision History
 *
 * $Log: rcp.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
