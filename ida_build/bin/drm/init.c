#pragma ident "$Id: init.c,v 1.13 2005/10/26 23:18:33 dechavez Exp $"
/*======================================================================
 *
 *  Initialization routine.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "service.h"

#define VERSION "drm_service Version 2.15.2"

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s [options] filename\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"home=name   => set DRM_HOME\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"filename    => $DRM_HOME/queue/`filename' request file\n");
    fprintf(stderr,"\n");
    exit(1);
}

void init(int argc, char **argv, char **fname, int *first, char *buffer)
{
int i;
char *drm_home  = NULL;
char *log       = NULL;
int debug       = 1;
struct drm_names *name;
static char *lastarg = NULL;
struct stat sbuf;
static char *fid = "init";

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "home=", strlen("home=")) == 0) {
            drm_home = argv[i] + strlen("home=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else {
            if (lastarg != NULL) help(argv[0]);
            if ((lastarg = strdup(argv[i])) == NULL) {
                fprintf(stderr, "%s: init: ", argv[0]);
                perror("strdup");
                exit(1);
            }
        }
    }

    if ((*fname = lastarg) == NULL) help(argv[0]);

/*  Start default logging  */

    util_logopen(log, 1, 9, debug, NULL, argv[0]);
    util_log(2, "%s", VERSION);

/*  Set environment  */

    if (drm_home == NULL) {
        drm_home = getenv(DRM_HOME);
        if (drm_home == NULL) {
            drm_home = DRM_DEFHOME;
            sprintf(buffer, "%s=%s", DRM_HOME, drm_home);
            if ((drm_home = strdup(buffer)) == NULL) {
                util_log(1, "fatal error: strdup: %s", syserrmsg(errno));
                exit(1);
            }
            if (putenv(drm_home) != 0) {
                util_log(1, "fatal error: putenv: %s", syserrmsg(errno));
                exit(1);
            }
        }
    }

/*  drm library initialization  */

    drm_init(drm_home, buffer, *fname);
    name = drm_names();
    *first = (stat(name->path.log, &sbuf) == 0) ? 0 : 1;

/*  Set up exit handlers  */

    util_log(2, "setting up signal/exit handlers");
    if (exitcode() != 0) {
        util_log(1, "%s: exitcode failed", fid);
        exit(1);
    }

/*  Switch to work directory logging  */

    if (log == NULL) {
        util_log(1, "switch logging to %s", name->path.log);
        util_logclose();
        util_logopen(name->path.log, 1, 9, debug, NULL, argv[0]);
        util_log(*first ? 1 : 2, "%s", VERSION);
    }
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.13  2005/10/26 23:18:33  dechavez
 * updated version to 2.15.2
 *
 * Revision 1.12  2004/05/05 16:51:46  dechavez
 * updated spam detection triggers
 *
 * Revision 1.11  2004/04/26 20:48:36  dechavez
 * 2.15.0
 *
 * Revision 1.10  2004/01/29 23:55:09  dechavez
 * 2.14.2
 *
 * Revision 1.9  2004/01/29 19:14:48  dechavez
 * 2.14.1
 *
 * Revision 1.8  2004/01/23 22:23:48  dechavez
 * update version string to 2.14.0
 *
 * Revision 1.7  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.6  2003/04/24 00:10:37  dechavez
 * Anti-spam support introduced
 *
 * Revision 1.5  2002/04/25 21:07:24  dec
 * 2.12.0
 *
 * Revision 1.4  2002/02/11 17:37:28  dec
 * 2.11.1
 *
 * Revision 1.3  2000/10/17 23:41:49  dec
 * drm_service 2.10.2
 *
 * Revision 1.2  2000/02/18 06:36:27  dec
 * Added ReleaseNotes and consistent version numbering
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
