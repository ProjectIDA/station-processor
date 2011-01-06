#pragma ident "$Id: mediaops.c,v 1.6 2002/03/15 22:51:37 dec Exp $"
/*======================================================================
 *
 *  Generic media operations.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "ispd.h"

static int initialized = 0;
static int (*eject)();
static MUTEX mp = MUTEX_INITIALIZER;

extern ISP_PARAMS *Params;

#define MY_MOD_ID ISPD_MOD_MEDIAOPS

static int MediaType = ISP_OUTPUT_NONE;

void SetOutputMediaType()
{
    MediaType = ispGetMediaType(Params);
}

int OutputMediaType()
{
    return MediaType;
}

void eject_media()
{
int ok;

    MUTEX_LOCK(&mp);
        ok = initialized;
    MUTEX_UNLOCK(&mp);

    if (ok) (*eject)(1);
}

void InitMediaOps()
{
    switch (OutputMediaType()) {
      case ISP_OUTPUT_TAPE:
        if (tapeio_init() == 0) eject = eject_tape;
        break;
#ifdef INCLUDE_REMOVABLE_DISK_SUPPORT
      case ISP_OUTPUT_DISK:
        if (diskio_init() == 0) eject = eject_disk;
        break;
#endif /* INCLUDE_REMOVABLE_DISK_SUPPORT */
      case ISP_OUTPUT_CDROM:
        if (cdio_init() == 0) eject = eject_cdrom;
        break;
      case ISP_OUTPUT_NONE:
        return;
      default:
        util_log(1, "ABORT: cannot determine output device type");
        ispd_die(MY_MOD_ID + 1);
    }

    MUTEX_LOCK(&mp);
        initialized = 1;
    MUTEX_UNLOCK(&mp);
}

/* Revision History
 *
 * $Log: mediaops.c,v $
 * Revision 1.6  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.5  2001/10/24 23:16:18  dec
 * added CDROM support
 *
 * Revision 1.4  2001/10/08 18:18:16  dec
 * added OutputMediaType()
 *
 * Revision 1.3  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
