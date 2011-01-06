#pragma ident "$Id: drmd.c,v 1.9 2004/01/29 19:14:12 dechavez Exp $"
/*======================================================================
 *
 *  Scan the IDADRM queue and fork/exec up to <maxproc> drm_service
 *  processes to service the request(s) therein.
 *
 *====================================================================*/
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "util.h"
#include "drm.h"

#define VERSION "drmd Version 2.13.1"

#define DEF_MAXPROC   5
#define DEF_INTERVAL 30
#define DRM_PROC     "drm_service"

static int loglevel = 1;
static int nproc    = 0;
static int maxproc  = -1;
static char *home   = NULL;
static char buffer[DRM_BUFLEN];
static struct drm_cnf *cnf = NULL;
static struct drm_names *name = NULL;

static struct pinfo {
    pid_t pid;
    char  request[DRM_MAXNAMLEN+1];
} *pinfo;

static pid_t setpid(char *myname)
{
FILE *fp;
pid_t pid;

    if ((fp = fopen(name->path.pid, "r")) != NULL) {
        fscanf(fp, "%ld", &pid);
        if (pid > 0 && kill(pid, 0) == 0) {
            util_log(1, "another %s already running, pid %ld", myname, pid);
        } else {
            pid = 0;
        }
        fclose(fp);
    } else {
        pid = 0;
    }

    if (pid) return pid;
 
    if ((fp = fopen(name->path.pid, "w")) == NULL) {
        util_log(1, "FATAL ERROR: fopen: %s: %s", name->path.pid, syserrmsg(errno));
        return (pid_t) -1;
    }

    fprintf(fp, "%ld\n", pid = getpid());
    fclose(fp);

    return pid;
}

static int die(int status)
{
char *address;

    if (status != 0) {
        address = (cnf != NULL && cnf->admin != NULL) ? cnf->admin : "root";
        util_email(address, "drmd failed... see syslog", NULL);
    }
    util_log(1, "exit %d", status);
    exit(status);
}

void drm_exit(int status)
{
    die(status == DRM_OK ? 0 : 1);
}

static void reap(int sig)
{
int i, pid, ok;
int status;

    while ((pid = wait3(&status, WNOHANG, (struct rusage *) 0)) > 0) {
        for (ok = 0, i = 0; !ok && i < maxproc; i++) {
            if (pinfo[i].pid == pid) {
                --nproc;
                pinfo[i].pid = -1;
                ok = 1;
            }
        }
    }
    signal(sig, reap);
}

static void describe_self()
{
    util_log(1, "%s", VERSION);
    util_log(1, "DRM_HOME = `%s'", home);
    util_log(1, "maximum concurrent processes = %d", maxproc);
    util_log(1, "%d processes currently active", nproc);
}

static void catch(int sig)
{
int pid;
int status;
static char *fid = "catch";

    switch (sig) {
      case SIGHUP:
        loglevel = 1;
        break;
      case SIGINT:
        break;
      case SIGBUS:
      case SIGSEGV:
        abort();
        break;
      case SIGQUIT:
        util_log(1, "SIGQUIT");
        die(0);
      case SIGTERM:
        util_log(1, "SIGTERM");
        die(0);
      default:
        util_log(1, "going down on signal %d", sig);
        die(1);
    }

    signal(sig, catch);
}

static void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"home=name   => set DRM_HOME\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"maxproc=val => set max number of concurrent processes\n");
    fprintf(stderr,"-once       => quit when queue is empty\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"default: home=%s maxproc=<cnf val or %d> log=syslogd\n",
        DRM_DEFHOME, DEF_MAXPROC
    );
    exit(1);
}

/*======================================================================
 *
 *  Lock files in the queue directory.  
 *
 *  Locking is done by setting to mode to DRM_LOCKED.
 *  To avoid race conditions, we first get an exclusive lock on a
 *  "lock lock" file, which will result that only one process can be
 *  locking a file at any given moment.  Returns 1 if the file was
 *  locked, 0 if somebody else 
 *
 *====================================================================*/

static int drm_lock(char *file)
{
FILE *lockfp, *fp;
int lockfd, locked;
struct stat statbuf;
static char path[MAXPATHLEN+1];
static char *fid = "drm_lock";

    if (file == NULL) {
        util_log(1, "%s: fatal error: null input", fid);
        util_email(cnf->admin, "drmd aborted(drm_lock:1)... see syslog", NULL);
        exit(1);
    }

/*  Grab the lock lock so we can be sure to avoid race conditions */

    if ((lockfp = fopen(name->path.locklock, "w")) == NULL) {
        util_log(1, "%s: fatal error: fopen `%s': %s", fid, name->path.locklock, syserrmsg(errno));
        util_email(cnf->admin, "drmd aborted(drm_lock:2)... see syslog", NULL);
        exit(1);
    }

    lockfd = fileno(lockfp);

    if (util_wlockw(lockfd, 0, SEEK_SET, 0) != 0) {
        util_log(1, "%s: fatal error: failed to get lock lock: %s", fid, syserrmsg(errno));
        util_email(cnf->admin, "drmd aborted(drm_lock:3)... see syslog", NULL);
        exit(1);
    }

    fprintf(lockfp, "%d\n", getpid());

/*  See if file is already locked  */

    sprintf(path, "%s/%s", name->dir.queue, file);
    if (stat(path, &statbuf) != 0) {
        if (errno == ENOENT) {
            util_unlock(lockfd, 0, SEEK_SET, 0);
            fclose(lockfp);
            return -0;
        }
        util_log(1, "%s: fatal error: can't stat `%s': %s", fid, path, syserrmsg(errno));
        util_email(cnf->admin, "drmd aborted(drm_lock:4)... see syslog", NULL);
        exit(1);
    }

    if (statbuf.st_mode == (0100000 + DRM_LOCKED)) {
        util_unlock(lockfd, 0, SEEK_SET, 0);
        fclose(lockfp);
        return 0;
    }

/*  Lock it  */

    if (chmod(path, DRM_LOCKED) != 0) {
        util_log(1, "%s: fatal error: chmod `%s': %s", fid, path, syserrmsg(errno));
        util_email(cnf->admin, "drmd aborted(drm_lock:5)... see syslog", NULL);
        exit(1);
    }

    util_unlock(lockfd, 0, SEEK_SET, 0);
    fclose(lockfp);
    return 1;
}

/*======================================================================
 *
 *  Get list of all files in the queue, sorted by age (oldest first).
 *  Since other processes can be actively working on these files while
 *  this search is taking place we won't worry about missing files
 *  when it comes time to stat() them.
 *
 *  Returns the number of entries or -1 on error.
 * 
 *====================================================================*/

static char *list[DRM_MAXQUEUE];

static int _drm_queue_compare(char **a, char **b)
{
struct stat astat, bstat;

    if (stat(*a, &astat) == 0 && stat(*b, &bstat) == 0) {
        return astat.st_mtime - bstat.st_mtime;
    } else {
        return 0;
    }
}

/*======================================================================
 *
 *  Set/clear patience.
 *
 *  Note, the flag file create below must be the same as done for
 *  flag.patience in names.c.
 *
 *====================================================================*/

static void drm_patience(char *request, long age)
{
FILE *fp;
char flag[MAXPATHLEN];
char *string;
struct stat sbuf;
struct drm_finfo *finfo;
static char *fid = "drm_patience";

    if (cnf->patience <= 0 || age <= cnf->patience) return;

    if (request == NULL) {
        util_log(1, "%s: illegal input", fid);
        die(DRM_BAD);
    } else if ((finfo = drm_fparse(request, buffer)) == NULL) {
        util_log(1, "%s: fatal error: can't parse `%s'", fid, request);
        die(DRM_BAD);
    }

    if (finfo->type == DRM_IDADRM) {
        string = DRM_IDADRM_STRING;
    } else if (finfo->type == DRM_SPYDER) {
        string = DRM_SPYDER_STRING;
    } else if (finfo->type == DRM_AUTODRM) {
        string = DRM_AUTODRM_STRING;
    } else {
        util_log(1, "%s: fatal error: unknown type `%d'", fid, finfo->type);
        die(DRM_BAD);
    }

    sprintf(flag, "%s/archive/%s/%02d%03d/%05ld/patience.%d",
        name->dir.home, string, finfo->yr, finfo->day, finfo->seqno, finfo->set
    );
    if (stat(flag, &sbuf) == 0) return;

    if ((fp = fopen(flag, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, flag, syserrmsg(errno));
        die(DRM_BAD);
    }
    fclose(fp);

    util_log(1, "%s exceeds patience threshold (%ld)", request, cnf->patience);
    sprintf(flag, "IDADRM request %s needs attention", request);
    util_email(cnf->admin, flag, NULL);
    util_log(1, "%s notified", cnf->admin);
}

static int drm_queue(char ***listptr)
{
static int first = 1;
DIR *dirp;
int i;
struct dirent *dp;
struct stat sbuf;
static int count = -1;
char qfile[MAXPATHLEN+1];
int (*compare_fnc)();
static char *fid = "drm_queue";

    compare_fnc = _drm_queue_compare;
    if (first) {
        for (i = 0; i < DRM_MAXQUEUE; i++) {
            if ((list[i] = (char *) malloc(DRM_MAXNAMLEN+1)) == NULL) {
                util_log(1, "%s: fatal error: malloc: %s",
                    fid, syserrmsg(errno)
                );
                util_email(cnf->admin, "drmd aborted(drm_queue:2)", NULL);
                exit(1);
            }
        }
        count = 0;
        first = 0;
    } else if (count < 0) {
        util_log(1, "%s: fatal error: queue not initialized", fid);
          util_email(cnf->admin, "drmd aborted(drm_queue:3)", NULL);
           exit(1);
    }

    util_log(4, "listing %s queue", name->dir.queue);

    if ((dirp = opendir(name->dir.queue)) == NULL) {
        util_log(1, "%s: fatal error: can't opendir(`%s'): %s",
            fid, name->dir.queue, syserrmsg(errno)
        );
          util_email(cnf->admin, "drmd aborted(drm_queue:7)", NULL);
           exit(1);
    }

    count = 0;
    for (dp = readdir(dirp); dp!=NULL && count<DRM_MAXQUEUE; dp=readdir(dirp)) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            if (strlen(dp->d_name) > DRM_MAXNAMLEN) {
                util_log(1, "%s: fatal error: file name `%s' is too long",
                    fid, dp->d_name
                );
                  util_email(cnf->admin, "drmd aborted(drm_queue:8)", NULL);
                   exit(1);
            }
            sprintf(qfile, "%s/%s", name->dir.queue, dp->d_name);
            if (stat(qfile, &sbuf) != 0 && errno != ENOENT) {
                util_log(1, "%s: fatal error: can't stat `%s: %s'",
                    fid, qfile, syserrmsg(errno)
                );
                  util_email(cnf->admin, "drmd aborted(drm_queue:9)", NULL);
                   exit(1);
            }
            if (S_ISREG(sbuf.st_mode) && drm_fparse(dp->d_name,buffer)!=NULL) {
                if (sbuf.st_mode != (0100000 + DRM_LOCKED)) {
                    strcpy(list[count], dp->d_name);
                    ++count;
                } else {
                    drm_patience(dp->d_name, time(NULL) - sbuf.st_mtime);
                }
            }
        }
    }

    if (count == DRM_MAXQUEUE) {
        util_log(2, "warning: queue full, ignoring some files (for now)");
    }

    closedir(dirp);
    qsort(list, count, sizeof(char *), compare_fnc);
    *listptr = list;

    util_log(4, "%s queue contains %d entries", name->dir.queue, count);
    return count;
}

void drm_ack_init(int dummy)
{
    return;
}

int main(int argc, char **argv)
{
FILE *fp;
int i, p, nfiles, fd, ok, did_something;
pid_t pid, child_pid, lock_pid, my_pid;
char **file;
char *log    = "syslogd";
int debug    = 1;
int runonce  = 0;
int background = 1;
static char *drm_home;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "bg=", strlen("bg=")) == 0) {
            background = atoi(argv[i] + strlen("bg="));
        } else if (strcmp(argv[i], "-once") == 0) {
            runonce = 1;
        } else if (strncmp(argv[i], "maxproc=", strlen("maxproc=")) == 0) {
            maxproc = atoi(argv[i] + strlen("maxproc="));
        } else {
            help(argv[0]);
        }
    }

    util_logopen(log, 1, 9, debug, NULL, argv[0]);

    if (home == NULL) {
        home = getenv(DRM_HOME);
        if (home == NULL) home = DRM_DEFHOME;
    }
    sprintf(buffer, "%s=%s", DRM_HOME, home);
    if ((drm_home = strdup(buffer)) == NULL) {
        perror("strdup");
        exit(1);
    }
    if (putenv(drm_home) != 0) {
        perror("putenv");
        exit(1);
    }

    drm_init(home, buffer, NULL);

    name = drm_names();
    cnf  = drm_cnf();
    if (maxproc <= 0) {
        maxproc = (cnf->maxproc > 0) ? cnf->maxproc : DEF_MAXPROC;
    }

    pinfo = (struct pinfo *) malloc(maxproc * sizeof(struct pinfo));
    if (pinfo == NULL) {
        perror("malloc");
        exit(1);
    }
    for (i = 0; i < maxproc; i++) pinfo[i].pid = -1;

    util_log(2, "installing signal handlers");

    if (signal(SIGTERM, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGQUIT, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGHUP, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGINT, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGBUS, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGSEGV, catch) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    if (background) {

        /* Make DRM_HOME the working directory */

        util_log(2, "making %s working directory", home);
        if (chdir(home) != 0) {
            util_log(1, "chdir: %s: %s", home, syserrmsg(errno));
            exit(1);
        }

        util_log(2, "going into background");
        if (util_bground(0, 0) < 0) exit(1);

    }

    util_logclose();
    util_logopen(log, 1, 9, debug, NULL, argv[0]);
    if (background) util_log(2, "now running in the background");
    my_pid = getpid();
    if ((pid = setpid(argv[0])) < 0) die(1);
    if (pid != my_pid) die(0);
    describe_self();

    util_log(2, "installing SIGCHLD handler");
    if (signal(SIGCHLD, reap) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    if ((unlink(DRM_CHECKFLAG)) != 0 && errno != ENOENT) {
        util_log(1, "warning: unlink(%s): %s",
            DRM_CHECKFLAG, syserrmsg(errno)
        );
    }
    util_log(2, "check flag removed");

    while (1) {
        did_something = 0;
        if (nproc < maxproc && (nfiles = drm_queue(&file)) > 0) {
            if (nfiles == 1) {
                util_log(loglevel, "%d inactive file in the queue", nfiles);
            } else {
                util_log(loglevel, "%d inactive files in the queue", nfiles);
            }
            for (i = 0; i < nfiles && nproc < maxproc; i++) {
                if (drm_lock(file[i])) {
                    util_log(2, "%s lock obtained", file[i]);
                    ++nproc;
                    if ((child_pid = fork()) < 0) {
                        util_log(1, "fatal error: fork: %s", syserrmsg(errno));
                        die(1);
                    } else if (child_pid > 0) {
                        for (ok = 0, p = 0; !ok && p < maxproc; p++) {
                            if (pinfo[p].pid < 0) {
                                pinfo[p].pid = child_pid;
                                strcpy(pinfo[p].request, file[i]);
                                ok = 1;
                            }
                        }
                        if (!ok) {
                            util_log(1, "fatal error: logic error 1");
                            die(1);
                        }
                        util_log(1, "%s control passed to %s[%d]", file[i], DRM_PROC, child_pid);
                        did_something = 1;
                    } else {
                        sleep(1);
                        util_logclose();
                        util_logopen(log, 1, 9, debug, NULL, argv[0]);
                        util_log(3, "execlp(`%s', `%s', `%s', 0)", DRM_PROC, DRM_PROC, file[i]);
                        execlp(DRM_PROC, DRM_PROC, file[i], (char *) 0);
                        util_log(1, "execlp: %s: %s", DRM_PROC, syserrmsg(errno));
                        die(-1);
                    }
                } else {
                    util_log(2, "%s already locked, file ignored", file[i]);
                }
            }
        } else {
            if (!did_something) {
                if (nfiles == 0) {
                    util_log(loglevel = 2, "0 inactive files in the queue");
                    if (runonce) {
                        util_log(1, "exit(0)");
                        exit(0);
                    }
                }
                util_log(2, "paused(), nfiles=%d", nfiles);
                pause();
                util_log(2, "awake");
            }
        }
    }
}

/* Revision History
 *
 * $Log: drmd.c,v $
 * Revision 1.9  2004/01/29 19:14:12  dechavez
 * 2.13.1
 *
 * Revision 1.8  2004/01/23 22:24:45  dechavez
 * reload signal handler in reap(), clean up exit tests
 *
 * Revision 1.7  2003/12/10 06:28:22  dechavez
 * 2.12.2, cosmetic changes to calm solaris cc
 *
 * Revision 1.6  2003/11/21 20:21:08  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.5  2002/04/26 17:36:21  nobody
 * Turned off all logging in signal handlers (deadlocks?)
 *
 * Revision 1.4  2002/04/26 01:06:21  nobody
 * rebuild with DRM_MAXQUEUE increased to 8192
 *
 * Revision 1.3  2001/04/22 02:38:06  dec
 * added -once option
 *
 * Revision 1.2  2000/02/18 06:36:27  dec
 * Added ReleaseNotes and consistent version numbering
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
