#pragma ident "$Id: check.c,v 1.3 2003/12/10 06:30:31 dechavez Exp $"
/*======================================================================
 *
 *  Make sure drmd is alive.
 *
 *  Examine the files in the hold directory and see if they are ready
 *  to be moved back into the queue.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "drm.h"

#define VERSION "drm_check Version 2.10.2"
#define myname "drm_check"

char buffer[DRM_BUFLEN];
char *checkflag = DRM_CHECKFLAG;

struct drm_cnf *cnf = NULL;
struct drm_names *name;

void set_return_address(address) /* dummy, called by drm_rddatreq */
char *address;
{
    return;
}

static int die(status)
int status;
{
char *address;

    if (status != 0) {
        if (fopen(checkflag, "w") == NULL) {
            util_log(1, "%s: %s", checkflag, syserrmsg(errno));
        } else {
            util_log(1, "create %s", checkflag);
        }
        if (status == 100) {
            status = 0;
        } else {
            address = (cnf==NULL || cnf->admin==NULL) ? "root" : cnf->admin;
            util_email(address, "drm_check failed", NULL);
        }
    }
    util_log(2, "exit %d", status);
    exit(status);
}

void drm_exit(status)
int status;
{
    die(status == DRM_OK ? 0 : 1);
}

static void help()
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"home=name   => set DRM_HOME\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    exit(1);
}

void test(request)
char *request;
{
pid_t drmd_pid;
double now;
FILE *fp;
struct drm_datreq datreq;

    util_log(2, "testing %s", request);
    if ((fp = fopen(request, "r")) == NULL) {
        util_log(1, "fopen: %s: %s", request, syserrmsg(errno));
        die(1);
    }

    drm_rddatreq(fp, &datreq);

    now = (double) time((time_t *) NULL);
    util_log(2, "request end  = %s", util_dttostr(datreq.end, 0));
    util_log(2, "current time = %s", util_dttostr(now, 0));
    if (datreq.end > now) {
        util_log(2, "data still not ready");
        return;
    }

    util_log(2, "data ready for processing");
    sprintf(buffer, "%s/%s", name->dir.queue, request);
    if (rename(request, buffer) == 0) {
        if ((fp = fopen(name->path.pid, "r")) != NULL) {
            fscanf(fp, "%d", &drmd_pid);
            if (kill(drmd_pid, 0) == 0) {
                if ((kill(drmd_pid, SIGINT)) == 0) {
                    util_log(2, "drmd[%d] signaled", drmd_pid);
                } else {
                    util_log(1, "kill(%d, SIGINT) failed: %s", 
                        drmd_pid, syserrmsg(errno)
                    );
                }
            }
            fclose(fp);
        }
        util_log(1, "%s re-queued", request);
    } else {
        util_log(2, "rename(%s, %s): %s", request, buffer, syserrmsg(errno));
        die(1);
    }
    return;
}
        
int main(int argc, char **argv)
{
FILE *fp;
int i, count, deleted;
DIR *dirp, *dirp2;
pid_t pid;
time_t age;
struct dirent *dp, *dp2;
struct stat sbuf;
char *home = NULL;
char *log  = NULL;
int debug  = 1;
char ftpdir[MAXPATHLEN+1];
char pathname[MAXPATHLEN+1];
char tstname[MAXPATHLEN+1];
static char *drm_home;
time_t now;
struct tm *current;
int yr, day;

    now     = time(NULL);
    current = localtime(&now);
    yr      = current->tm_year;
    day     = current->tm_yday;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else {
            help();
        }
    }

    util_logopen(log, 1, 9, debug, NULL, myname);
    util_log(2, "%s", VERSION);

    if (home == NULL) {
        home = getenv(DRM_HOME);
        if (home == NULL) home = DRM_DEFHOME;
    }

    drm_init(home, buffer, NULL);

    name = drm_names();
    cnf  = drm_cnf();

    if (stat(checkflag, &sbuf) == 0) {
        util_log(2, "%s exists... quit", checkflag);
        die(0);
    }

/*  Make sure drmd is running  */

    util_log(2, "checking to see that drmd is running");

    if (stat(name->path.pid, &sbuf) != 0) {
        util_email(cnf->admin, "drmd problem (no pid file)", NULL);
        die(100);
    }

    if ((fp = fopen(name->path.pid, "r")) == NULL) {
        util_log(1, "fatal error: fopen: %s: %s",
            name->path.pid, syserrmsg(errno)
        );
        die(1);
    }
    if (util_rlockw(fileno(fp), 0, 0, 0) != 0) {
        util_log(1, "fatal error: util_rlockw: %s: %s",
            name->path.pid, syserrmsg(errno)
        );
        die(1);
    }
 
    fscanf(fp, "%ld", &pid);
    util_log(2, "drmd pid is %ld", pid);

    if (kill(pid, 0) != 0) {
        util_log(2, "drmd[%ld] is missing!", pid);
        util_email(cnf->admin, "drmd is not running", NULL);
        die(100);
    }
    util_log(2, "drmd[%ld] is present", pid);
    fclose(fp);

/*  Check requests in the holding area  */

    util_log(2, "checking requests in the holding area");

    if (chdir(name->dir.hold) != 0) {
        util_log(1, "chdir: %s: %s", name->dir.hold, syserrmsg(errno));
        die(1);
    }
    if ((dirp = opendir(".")) == NULL) {
        util_log(1, "opendir: %s: %s", name->dir.hold, syserrmsg(errno));
        die(1);
    }

    count = 0;
    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (stat(dp->d_name, &sbuf) != 0) {
                util_log(1, "can't stat `%s/%s: %s'",
                    name->dir.hold, dp->d_name, syserrmsg(errno)
                );
                die(1);
            }
            if (S_ISREG(sbuf.st_mode)) {
                ++count;
                test(dp->d_name);
            }
        }
    }
    closedir(dirp);

    if (count == 0) util_log(2, "no files present in holding area");

/*  Remove expired data from anonymous ftp area  */

    util_log(2, "checking age of data in anonymous ftp");

    sprintf(ftpdir, "%s%s/pickup", cnf->ftphome, cnf->anonftp);

    if ((dirp = opendir(ftpdir)) == NULL) {
        util_log(1, "opendir: %s: %s", ftpdir, syserrmsg(errno));
        die(1);
    }

    for (dp = readdir(dirp); dp != NULL; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            sprintf(tstname, "%s/%s", ftpdir, dp->d_name);
            if (stat(tstname, &sbuf) != 0) {
                util_log(1, "can't stat `%s: %s'", tstname, syserrmsg(errno));
                die(1);
            }
            if (S_ISDIR(sbuf.st_mode)) {
                deleted = 0;
                if ((dirp2 = opendir(tstname)) == NULL) {
                    util_log(1, "opendir: %s: %s", tstname, syserrmsg(errno));
                    die(1);
                }
                for (dp2 = readdir(dirp2); dp2 != NULL; dp2=readdir(dirp2)){
                    sprintf(pathname, "%s/%s", tstname, dp2->d_name);
                    if (stat(pathname, &sbuf) == 0 && S_ISREG(sbuf.st_mode)) {
                        age = now - sbuf.st_mtime;
                        if (age > cnf->holdanon) {
                            util_log(2, "delete expired file `%s'", pathname);
                            if ((unlink(pathname)) != 0) {
                                util_log(1, "unlink: %s: %s",
                                    pathname, syserrmsg(errno)
                                );
                                die(1);
                            }
                            ++deleted;
                        }
                    }
                }
                closedir(dirp2);
                if (deleted) {
                    sprintf(tstname, "%s/%s", ftpdir, dp->d_name);
                    if (rmdir(tstname) == 0) {
                        util_log(1, "delete empty directory `%s'", tstname);
                    }
                }
            } else if (S_ISREG(sbuf.st_mode)) {
                age = now - sbuf.st_mtime;
                if (age > cnf->holdanon) {
                    util_log(1, "delete expired file `%s'", tstname);
                    if ((unlink(tstname)) != 0) {
                        util_log(1, "unlink: %s: %s",
                            pathname, syserrmsg(errno)
                        );
                        die(1);
                    }
                }
            }
        }
    }
    closedir(dirp);

/* Remove yesterday's sequence number flags */

    if (chdir(home) != 0) {
        util_log(1, "chdir: %s: %s", home, syserrmsg(errno));
        die(1);
    }

    sprintf(buffer, ".autodrm.%02d%03d", yr, day); unlink(buffer);
    sprintf(buffer, ".spyder.%02d%03d",  yr, day); unlink(buffer);
    sprintf(buffer, ".idadrm.%02d%03d",  yr, day); unlink(buffer);

/* All done */

    exit(0);
}

void drm_ack_init(dummy)
int dummy;
{
    return;
}

/* Revision History
 *
 * $Log: check.c,v $
 * Revision 1.3  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.2  2000/02/18 06:36:26  dec
 * Added ReleaseNotes and consistent version numbering
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
