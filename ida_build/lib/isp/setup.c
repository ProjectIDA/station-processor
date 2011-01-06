#pragma ident "$Id: setup.c,v 1.1.1.1 2000/02/08 20:20:27 dec Exp $"
/*======================================================================
 *
 *  Intended to help maintain a consistent environment between the
 *  the various ISP related processes.  This insures that the the
 *  NRTS_HOME environment variable is set and that the working
 *  directory is $NRTS_HOME/SYSTEM, where SYSTEM is the user supplied
 *  station name.  If a user supplied name is not provided and the
 *  ISP_SYSTEM environment variable is set, then that is taken to be
 *  the station name.
 *
 *  The working directory name is returned.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include <unistd.h>
#include "isp.h"
#include "nrts.h"
#include "util.h"

char *isp_setup(home_ptr, system_ptr)
char **home_ptr;
char **system_ptr;
{
static char buffer[MAXPATHLEN+16];
static char *defhome = NRTS_DEFHOME;
char *home, *system, *envstr;
static char *fid = "isp_setup";

/* System name must be given or found in the environment */

    if (*system_ptr != (char *) NULL) {
        system = *system_ptr;
    } else if ((system = getenv(ISP_SYSTEM)) == (char *) NULL) {
        fprintf(stderr, "must specify system name or define ISP_SYSTEM\n");
        return (char *) NULL;
    } else if ((*system_ptr = strdup(system)) == (char *) NULL) {
        fprintf(stderr, "%s: ", fid);
        perror("strdup");
        return (char *) NULL;
    }

/* IF NRTS_HOME is not given, use the default */

    if (*home_ptr != (char *) NULL) {
        home = *home_ptr;
    } else {
        if ((home = getenv(NRTS_HOME)) == (char *) NULL) {
            home = defhome;
            sprintf(buffer, "NRTS_HOME=%s", home);
            if ((envstr = strdup(buffer)) == (char *) NULL) {
                fprintf(stderr, "%s: ", fid);
                perror("strdup");
                return (char *) NULL;
            }
            if (putenv(envstr) != 0) {
                fprintf(stderr, "%s: ", fid);
                perror("putenv");
                return (char *) NULL;
            }
        }
        if ((*home_ptr = strdup(home)) == (char *) NULL) {
            fprintf(stderr, "%s: ", fid);
            perror("strdup");
            return (char *) NULL;
        }
    }

/* Our working directory will be NRTS_HOME/system/isp */

    sprintf(buffer, "%s/%s/isp", home, system);
    if (chdir(buffer) != 0) {
        fprintf(stderr, "chdir: ");
        perror(buffer);
        return (char *) NULL;
    }

    if (getwd(buffer) == 0) {
        fprintf(stderr, "%s: ", fid);
        perror("getwd");
        return (char *) NULL;
    }

    return buffer;
}

/* Revision History
 *
 * $Log: setup.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
