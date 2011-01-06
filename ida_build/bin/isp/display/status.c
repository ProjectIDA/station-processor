#pragma ident "$Id: status.c,v 1.4 2001/10/24 23:20:55 dec Exp $"
/*======================================================================
 *
 * Query the server for regular state of health updates.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "isp_console.h"

static int interval = 1;
static MUTEX mutex = MUTEX_INITIALIZER;
static int MediaType = ISP_OUTPUT_NONE;

static int StatusInterval()
{
int retval;

    MUTEX_LOCK(&mutex);
        retval = interval;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void SetStatusInterval(int newinterval)
{
    if (newinterval < 1) return;
    MUTEX_LOCK(&mutex);
        interval = newinterval;
    MUTEX_UNLOCK(&mutex);
}

void *StatusRequest(void *dummy)
{
    while (1) {
        RemoteCommand(ISP_STATUS);
        sleep(StatusInterval());
    }
}

int OutputMediaType()
{
int retval;

    MUTEX_LOCK(&mutex);
        retval = MediaType;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

void SetOutputMediaType(int type)
{
    MUTEX_LOCK(&mutex);
        MediaType = type;
    MUTEX_UNLOCK(&mutex);
}

/* Revision History
 *
 * $Log: status.c,v $
 * Revision 1.4  2001/10/24 23:20:55  dec
 * added CDROM support (only partial support for DAS systems)
 *
 * Revision 1.3  2001/05/20 17:44:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
