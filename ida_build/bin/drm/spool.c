#pragma ident "$Id: spool.c,v 1.3 2003/12/10 06:30:32 dechavez Exp $"
/*======================================================================
 *
 *  Take an IDADRM or related request from stdin and spool it to disk.
 *
 *  This command must to be set-uid to the user which handles
 *  DRM requests (usually "nrts" in production mode, and "dec" in
 *  development mode) and will be executed by the mailer with the
 *  following syntax:
 *
 *  drm_spool drm_home type
 *
 *  where "drm_home" is the value of the DRM_HOME environment variable
 *  of the effective user and type is one of `idadrm', `spyder', or
 *  `autodrm'.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include "drm.h"
#include "util.h"

#define VERSION "drm_spool Version 2.10.2"

void drm_exit(status)
int status;
{
    util_log(1, "exit %d", status);
    exit(status);
}

#define MYNAME "drm_spool"

int main(int argc, char **argv)
{
FILE *fp;
int i, status, lineno = 0;
char *home = NULL;
char *log  = NULL;
char *type = NULL;
int debug  = 1;
pid_t drmd_pid;
struct drm_finfo finfo;
struct drm_names *name;
char buffer[DRM_BUFLEN],  fname[MAXPATHLEN+1];
char seqno[MAXPATHLEN+1], seqflag[MAXPATHLEN+1];
time_t now;
struct stat sbuf;
struct tm *current;
int first_today = 0;

    now       = time(NULL);
    current   = localtime(&now);
    finfo.yr  = current->tm_year % 100;
    finfo.day = ++current->tm_yday;

    chdir("/var/tmp");

/*  Parse command line  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "type=", strlen("type=")) == 0) {
            type = util_lcase(argv[i] + strlen("type="));
        } else {
            exit(1);
        }
    }

    util_logopen(log, 1, 9, debug, NULL, MYNAME);
    util_log(2, "%s", VERSION);

    if (home == NULL) home = getenv(DRM_HOME);
    if (home == NULL) home = DRM_DEFHOME;
    if (chdir(home) != 0) {
        util_log(1, "fatal error: can't chdir home: %s: %s",
            home, syserrmsg(errno)
        );
        exit(2);
    }

/*  Determine message type  */

    if (type == NULL) {
        util_log(1, "fatal error: message type not specified");
        exit(3);
    }

    if (strcmp(type, DRM_SPYDER_STRING) == 0) {
        finfo.type = DRM_SPYDER;
        util_log(1, "incoming `%s' request", DRM_SPYDER_STRING);
    } else if (strcmp(type, DRM_IDADRM_STRING) == 0) {
        finfo.type = DRM_IDADRM;
        util_log(1, "incoming `%s' request", DRM_IDADRM_STRING);
    } else if (strcmp(type, DRM_AUTODRM_STRING) == 0) {
        finfo.type = DRM_AUTODRM;
        util_log(1, "incoming `%s' request", DRM_AUTODRM_STRING);
    } else {
        util_log(1, "fatal error: unrecognized message type");
        exit(4);
    }

    sprintf(seqno, ".%s.seqno", type);
    sprintf(seqflag, ".%s.%02d%03d", type, finfo.yr, finfo.day);
    finfo.set = 0;

/*  Get message ident (sequence number)  */

    if (stat(seqflag, &sbuf) == 0) {
        first_today = 0;
    } else if (errno == ENOENT) {
        first_today = 1;
        if ((fp = fopen(seqflag, "w")) == NULL) {
            util_log(1, "fatal error: fopen(1): %s/%s: %s",
                home, seqflag, syserrmsg(errno)
            );
            exit(5);
        }
        fclose(fp);
    } else {
        util_log(1, "fatal error: can't stat `%s/%s': %s",
            home, seqflag, syserrmsg(errno)
        );
        exit(6);
    }

    if ((fp = fopen(seqno, "r+")) == NULL) {
        util_log(1, "fatal error: fopen(2): %s/%s: %s",
            home, seqno, syserrmsg(errno)
        );
        exit(7);
    }

    if (util_wlockw(fileno(fp), 0, 0, 0) != 0) {
        util_log(1, "fatal error: util_wlockw: %s/%s: %s",
            home, seqno, syserrmsg(errno)
        );
        exit(8);
    }

    if (setvbuf(fp, (char *) NULL, _IONBF, 0) != 0) {  /* unbuffered I/O */
        util_log(1, "fatal error: setvbuf: %s/%s: %s",
            home, seqno, syserrmsg(errno)
        );
        exit(9);
    }

    fscanf(fp, "%ld", &finfo.seqno);

    if (first_today) finfo.seqno = 1;

    rewind(fp);
    fprintf(fp, "%ld\n", finfo.seqno + 1);
    util_unlock(fileno(fp), 0, 0, 0);
    fclose(fp);

/*  Complete the request name  */

    sprintf(fname, "%s-%02d%03d_%05ld.%d",
        type, finfo.yr, finfo.day, finfo.seqno, finfo.set
    );

/*  Get all names  */

    drm_init(home, buffer, fname);
    name = drm_names();

/*  Create incoming file  */

    if ((fp = fopen(name->path.incoming, "w")) == NULL) {
        util_log(1, "fatal error: fopen(3): %s: %s:",
            name->path.incoming, syserrmsg(errno)
        );
        exit(10);
    }

    if (setvbuf(fp, (char *) NULL, _IONBF, 0) != 0) {
        util_log(1, "fatal error: setvbuf: %s/%s: %s",
            home, seqno, syserrmsg(errno)
        );
        exit(11);
    }

    if (chmod(name->path.incoming, DRM_UNLOCKED) != 0) {
        util_log(1, "fatal error: chmod: %s: %s:",
            name->path.incoming, syserrmsg(errno)
        );
        exit(12);
    }
    
/*  Copy the message to the incoming file  */

    while ((status=util_getline(stdin,buffer,DRM_BUFLEN,(char) 0,&lineno))==0) {
        if (fprintf(fp, "%s\n", buffer) < 0) {
            util_log(1, "fatal error: fprintf: %s: %s",
                name->path.incoming, syserrmsg(errno)
            );
            exit(13);
        }
        if (strncmp(buffer, "From:", strlen("From:")) == 0) util_log(1, buffer);
    }

    if (status != 1) {
        util_log(1, "fatal error: getline: %s", syserrmsg(errno));
        exit(14);
    }

    fclose(fp);

/*  Create archive directory and set the timestamp there  */

    if (util_mkpath(name->dir.archive, 0755) != 0) {    
        util_log(1, "fatal error: util_mkpath: %s: %s",
            name->dir.work, syserrmsg(errno)
        );
        exit(15);
    }

    if ((fp = fopen(name->path.timestamp, "w")) == NULL) {
        util_log(1, "fatal error: fopen(4): %s: %s",
            name->path.timestamp, syserrmsg(errno)
        );
        exit(16);
    }

    if (setvbuf(fp, (char *) NULL, _IONBF, 0) != 0) {
        util_log(1, "fatal error: setvbuf: %s: %s",
            seqno, syserrmsg(errno)
        );
        exit(17);
    }

    fclose(fp);

/*  Load the request into the queue  */

    if (rename(name->path.incoming, name->path.queue) != 0) {
        util_log(1, "fatal error: rename: %s, %s: %s",
            name->path.incoming, name->path.queue, syserrmsg(errno)
        );
        exit(18);
    }

    util_log(1, "request %s queued", fname);

/*  Send drmd a wake up signal  */

    if ((fp = fopen(name->path.pid, "r")) != NULL) {
        fscanf(fp, "%d", &drmd_pid);
        if (kill(drmd_pid, 0) == 0) {
            if ((kill(drmd_pid, SIGINT)) == 0) {
                util_log(1, "drmd[%d] signaled", drmd_pid);
            } else {
                util_log(1, "kill(%d, SIGINT) failed: %s", 
                    drmd_pid, syserrmsg(errno)
                );
            }
        } else {
            util_log(1, "WARNING: no drmd to signal");
        }
        fclose(fp);
    } else {
        util_log(1, "WARNING: no drmd to signal");
    }
    
    exit(0);
}

void drm_ack_init(dummy)
int dummy;
{
    return;
}

/* Revision History
 *
 * $Log: spool.c,v $
 * Revision 1.3  2003/12/10 06:30:32  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.2  2000/02/18 06:36:27  dec
 * Added ReleaseNotes and consistent version numbering
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
