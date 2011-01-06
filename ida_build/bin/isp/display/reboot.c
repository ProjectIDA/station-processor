#pragma ident "$Id: reboot.c,v 1.4 2001/05/20 17:44:44 dec Exp $"
/*======================================================================
 *
 * Reboot something
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

int RebootDigitizer(char **token, int ntoken, WINDOW *sohwin, int certain)
{
int i;

    if (ntoken == 2) {
        if (strcasecmp(token[1], "isp") == 0) {
            if (Confirm() && Reconfirm()) {
                if (Connected()) {
                    return RemoteCommand(ISP_HOST_REBOOT);
                } else {
                    isp_bootmgr(6);
                    return ISP_OK;
                }
            } else {
                return ISP_OK;
            }
        } else if (
            strcasecmp(token[1], "das") == 0 ||
            strcasecmp(token[1], "san") == 0
        ) {
            if (certain || Confirm()) {
                return RemoteCommand(ISP_DAS_REBOOT);
            } else {
                return ISP_OK;
            }
        }
    }

    return ISP_OK;
}

/* Revision History
 *
 * $Log: reboot.c,v $
 * Revision 1.4  2001/05/20 17:44:44  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/02 20:30:44  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/10/19 22:24:53  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
