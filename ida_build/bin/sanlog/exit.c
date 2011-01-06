#pragma ident "$Id: exit.c,v 1.1 2001/09/18 20:54:30 dec Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2001 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "sanlog.h"

extern SANLOG_LIST *List;

static void LoggedExit(int status)
{
    util_log(1, "exit %d", status);
    exit(status);
}

void GracefulExit(int status)
{
int i;
static MUTEX mutex = MUTEX_INITIALIZER;
static char *fid  = "GracefulExit";

    MUTEX_LOCK(&mutex); /* prevent multiple attempts */

    for (i = 0; i < List->count; i++) {
        MUTEX_LOCK(&List->san[i].mutex);
        if (List->san[i].fp != NULL) {
            util_log(1, "close %s", List->san[i].path);
            fclose(List->san[i].fp);
        }
    }

    util_log(1, "exit %d", status);
    exit(status);
}

/* Revision History
 *
 * $Log: exit.c,v $
 * Revision 1.1  2001/09/18 20:54:30  dec
 * created
 *
 */
