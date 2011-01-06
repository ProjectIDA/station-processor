#pragma ident "$Id: viewlog.c,v 1.4 2008/03/05 23:21:26 dechavez Exp $"
/*======================================================================
 *
 * View selected log file(s)
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "isp_console.h"

#ifndef MORE
#   ifdef SOLARIS
#      define MORE "/opt/local/bin/less"
#   else
#      define MORE "/usr/bin/less"
#   endif
#endif

#ifndef LS
#   ifdef SOLARIS
#      define LS "/usr/bin/ls"
#   else
#      define LS "/bin/ls"
#   endif
#endif

void ViewLog(int type, char *default_file)
{
FILE *fp;
static char path[MAXPATHLEN+1], command[MAXPATHLEN+32], namebuf[16];
char *logdir, *logfile;

    logfile = default_file;
    if (type == DASLOG) {
        logdir = "daslog";
        if (logfile == (char *) NULL) {
            sprintf(namebuf, "%07ld", util_today());
            logfile = namebuf;
        }
    } else {
        logdir = "isplog";
        if (logfile == (char *) NULL) {
            strcpy(namebuf, "active");
            logfile = namebuf;
        }
    }

    while (1) {
        sprintf(path, "%s/%s", logdir, logfile);
        if ((fp = fopen(path, "r")) == (FILE *) NULL) {
            printf("Can't view %s: %s\n", path, syserrmsg(errno));
            fflush(stdout);
        } else {
            fclose(fp);
            sprintf(command, "%s %s", MORE, path);
            system(command);
        }
        if (!utilQuery("View another log? ")) return;
        printf("Available logs:\n");
        fflush(stdout);
        sprintf(command, "%s %s", LS, logdir);
        system(command);
        namebuf[0] = 0;
        while (strlen(namebuf) == 0) {
            printf("which log? "); fflush(stdout);
            fgets(namebuf, 16, stdin);
            namebuf[strlen(namebuf)-1] = 0; /* strip off new line */
        }
        logfile = namebuf;
    }
}

/* Revision History
 *
 * $Log: viewlog.c,v $
 * Revision 1.4  2008/03/05 23:21:26  dechavez
 * Use correct paths for less and ls in isp/display in non-Solaris builds
 *
 * Revision 1.3  2003/06/11 20:55:53  dechavez
 * use new form of utilQuery and utilPause
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
