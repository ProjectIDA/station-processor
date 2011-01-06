#pragma ident "$Id: die.c,v 1.9 2007/01/11 22:02:30 dechavez Exp $"
/*======================================================================
 *
 *  Graceful exits.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <unistd.h>
#include "ispd.h"
#include "isi/dl.h"

static SEMAPHORE sp;
static ISI_DL *dl = NULL;

static void die(int status)
{
    util_log(1, "exit %d", status);
    exit(status);
}

void SaveDLHandle(void *dlptr)
{
    dl = (ISI_DL *) dlptr;
}

void complete_shutdown()
{
    SEM_POST(&sp);
}

void ispd_die(int status)
{
BOOL first = FALSE;
static int count  = 0;
static MUTEX mp = MUTEX_INITIALIZER;
static char *fid  = "ispd_die";

    MUTEX_LOCK(&mp);
        if (++count == 1) {
            first = TRUE;
            SEM_INIT(&sp, 0, 1);
        } else if (status == 0) {
            util_log(1, "multiple termination signals... force exit");
            die(0);
        }
    MUTEX_UNLOCK(&mp);

    if (first) {
        if (dl != NULL) isidlCloseDiskLoop(dl);
        set_shutdown(1);
        if (OutputMediaType() == ISP_OUTPUT_TAPE) {
            flush_buffer(ISPD_FLUSH_FORCE);
            SEM_WAIT(&sp);
        }
        /* shutdown_inject(); */
        die(status);
    } else {
        pause();
    }
}

/* Revision History
 *
 * $Log: die.c,v $
 * Revision 1.9  2007/01/11 22:02:30  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.8  2005/08/26 20:17:07  dechavez
 * removed tee subsystem
 *
 * Revision 1.7  2005/07/26 00:58:14  dechavez
 * initial ISI dl support (2.4.6d)
 *
 * Revision 1.6  2005/04/04 20:21:38  dechavez
 * use new tee.c function names
 *
 * Revision 1.5  2001/10/24 23:19:18  dec
 * Only flush buffer (and block for completion) for tape output
 *
 * Revision 1.4  2001/05/21 15:05:48  dec
 * remove debugging print statements
 *
 * Revision 1.3  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2001/02/23 22:53:16  dec
 * don't sleep during shutdown
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
