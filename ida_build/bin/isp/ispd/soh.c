#pragma ident "$Id: soh.c,v 1.5 2002/03/15 22:51:38 dec Exp $"
/*======================================================================
 *
 * Thread to periodically send status requests to the DAS
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997-2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "ispd.h"
#include "sanio.h"

#define MY_MOD_ID ISPD_MOD_SOH

extern ISP_PARAMS *Params;

/* DAS status requests */

static THREAD_FUNC DasSohThread(void *dummy)
{
static int count = 0;

    clear_sreqsent();

    while (1) {
        if (!sreqsent() || ++count % 10 == 0) das_statreq(3);
        sleep(Params->statint);
    }
}

/* SAN soh handled here */

static THREAD_FUNC SanSohThread(void *dummy)
{
static int count = 0;

    while (1) {
        if (have_dascnf()) {
            if (++count != 1) {
                sleep(5);
            } else {
                SanCmnd(SAN_SNDSOH);
            }
            if (SanCmndConn() == TRUE) {
                SanCmnd(SAN_SNDSOH_A);
                if (count % 6 == 0) SanCmnd(SAN_SNDSOH_B);
            }
        } else {
            count = 0;
            sleep(1);
        }
    }
}

static void *san_nop(void *dummy)
{
static int count = 0;

    while (1) {
        SanCmnd(SAN_NOP);
        sleep(Params->san.timeout/2);
    }
}

/* Start appropriate thread */

void InitSoh()
{
THREAD tid;
static char *fid = "InitSoh";

    if (Params->digitizer == ISP_DAS) {
        if (!THREAD_CREATE(&tid, DasSohThread, NULL)) {
            util_log(1, "%s: failed to create DasSohThread", fid);
            ispd_die(MY_MOD_ID + 1);
        }
    } else {
        if (!THREAD_CREATE(&tid, SanSohThread, NULL)) {
            util_log(1, "%s: failed to create SanSohThread", fid);
            ispd_die(MY_MOD_ID + 2);
        }
    }
}

/* Revision History
 *
 * $Log: soh.c,v $
 * Revision 1.5  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
