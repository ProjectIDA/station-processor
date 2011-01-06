#pragma ident "$Id: patch.c,v 1.6 2007/01/25 20:28:24 dechavez Exp $"
/*======================================================================
 *
 *  Patch headers.  The station name is forced to match the ISP's
 *  idea of the name, and the time stamps are forced to follow the
 *  original DAS/ARS conventions.
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
#include "ispd.h"

extern ISP_PARAMS *Params;

#define YEAR_OFF    6
#define BEGEXT_OFF 14
#define ENDEXT_OFF 46
#define SNAME_OFF  26

void patchMK7(char *packet)
{
short year;
long  lval;
 
    if (Params->ida->rev.value < 5) return;
 
/* Remove the DAS encoded GPS year and save time and year as per 
 * MK6/7 conventions.
 */
    memcpy(&lval, packet + BEGEXT_OFF, 4); LSWAB(&lval, 1);
    year = 1997 + (lval >> 25);
    lval &= 0x01ffffff;
    LSWAB(&lval, 1); memcpy(packet + BEGEXT_OFF,  &lval, 4);
    SSWAB(&year, 1); memcpy(packet + YEAR_OFF,    &year, 2);
 
/* Data packets also have an end time stamp... fix that too */

    if (packet[0] == 'D') {
        memcpy(&lval, packet + ENDEXT_OFF, 4); LSWAB(&lval, 1);
        lval &= 0x01ffffff;
        LSWAB(&lval, 1); memcpy(packet + ENDEXT_OFF,  &lval, 4);
    }

/* Insert our notion of the station name */

    memcpy(packet + SNAME_OFF, Params->sta, 8);

    return;
}

/* Revision History
 *
 * $Log: patch.c,v $
 * Revision 1.6  2007/01/25 20:28:24  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.5  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.4  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.3  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.2  2000/10/19 22:26:02  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
