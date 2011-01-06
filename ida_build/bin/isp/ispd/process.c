#pragma ident "$Id: process.c,v 1.13 2007/01/05 00:47:54 dechavez Exp $"
/*======================================================================
 *
 * QC all new packets (DAS and aux) and forward them To the mass
 * store thread.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_PROCESS

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;

static THREAD_FUNC ProcessThread(void *dummy)
{
static struct stat statbuf;
static u_long dropped;
static int status, type, error, quit, tqual, want;
static long tstamp;
static union runion record;
static UINT8 *packet;
static OLD_MSGQ_MSG *msg;
static char *fid = "ProcessThread";

    util_log(2, "Packet processing thread started, tid = %d", THREAD_SELF());

    packet = (UINT8 *) NULL;
    dropped = 0;

    while(1) {

    /* Get next DAS record off the raw message queue */

        msg = msgq_get(&Q->das_process, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received!", fid);
            ispd_die(MY_MOD_ID + 1);
        }
        packet = (UINT8 *) msg->data;

    /* Decode packet to detect gross errors */

        if (error = decode((char *) packet, &type, &record, &tstamp, &tqual)) {
            if (Params->digitizer == ISP_DAS) {
                util_log(1, "corrupt `%c' DAS packet dropped: status %d",
                    packet[0], error
                );
            } else {
                util_log(1, "corrupt `%c%c' SAN packet dropped: status %d",
                    packet[0], packet[1], error
                );
            }
            MUTEX_LOCK(&Status->lock);
                ++Status->count.badr;
            MUTEX_UNLOCK(&Status->lock);
            msgq_put(&Heap->packets, msg);
            continue;
        }

    /* A (probably) good record at this point... update counters */

        MUTEX_LOCK(&Status->lock);
            if (Params->digitizer == ISP_DAS) {
                switch (type) {
                  case IDA_DATA:
                    if (packet[0] == 'D') {
                        ++Status->count.drec;
                        Status->clock = tstamp;
                        Status->clock_qual = tqual;
                    } else {
                        ++Status->count.arec;
                    }
                    break;
                  case IDA_IDENT:
                    ++Status->count.irec;
                    break;
                  case IDA_CONFIG:
                    ++Status->count.krec;
                    break;
                  case IDA_CALIB:
                    ++Status->count.crec;
                    break;
                  case IDA_LOG:
                    ++Status->count.lrec;
                    break;
                  case IDA_EVENT:
                    ++Status->count.erec;
                    break;
                  case IDA_ISPLOG:
                    ++Status->count.urec;
                    break;
                  case IDA_DASSTAT:
                    ++Status->count.srec;
                    break;
                  default:
                    ++Status->count.orec;
                    type = IDA_UNKNOWN;
                }
            } else {
                switch (type) {
                  case IDA_DATA:
                    ++Status->san.data.count.p1;
                    Status->clock = tstamp;
                    Status->clock_qual = tqual;
                    break;
                  case IDA_LOG:
                    ++Status->san.data.count.p2;
                    break;
                  case IDA_ISPLOG:
                    ++Status->count.urec;
                    break;
                  default:
                    ++Status->san.data.count.p3;
                    type = IDA_UNKNOWN;
                }
            }
            ++Status->count.nrec;
        MUTEX_UNLOCK(&Status->lock);

    /* Drop all unrecognized records */

        if (type == IDA_UNKNOWN) {
            msgq_put(&Heap->packets, msg);
            continue;
        }

    /* Process DAS status records */

        if (type == IDA_DASSTAT && Params->digitizer == ISP_DAS) {
            set_event(record.s.event);
            if (calib_status() != ISP_CALIB_PENDING_AT_ISP) {
                set_calib(record.s.calib);
            }
            clear_sreqsent();
            MUTEX_LOCK(&Status->lock);
                Status->last.dasstat = time(NULL);
                Status->das.acks     = record.s.acks;
                Status->das.naks     = record.s.naks;
                Status->das.dropped  = record.s.dropped;
                Status->das.timeouts = record.s.timeouts;
                Status->das.triggers = record.s.triggers;
                Status->flag.event   = record.s.event;
                Status->flag.calib   = record.s.calib;
            MUTEX_UNLOCK(&Status->lock);
            util_log(2, "DAS STATUS: %4ld %4ld %4ld %4ld %4ld",
                record.s.naks, record.s.acks, record.s.dropped,
                record.s.timeouts, record.s.triggers
            );
            packet[IDA_BUFSIZ-1] = Params->savedasstats ? 1 : 0; /* IDA10 OK */
        }

    /* Process DAS ident records */

        if (type == IDA_IDENT && Params->digitizer == ISP_DAS) {
            util_log(1, "IDDATA from DAS received");
            save_iddata((char *) packet);
            MUTEX_LOCK(&Status->lock);
                if (record.i.boot_flag) {
                    ++Status->das.reboots;
                    Status->das.start = time(NULL);
                }
                Status->have_iddata = 1;
                Status->iddata = record.i;
                strcpy(Status->iddata.sname, Params->sta);
            MUTEX_UNLOCK(&Status->lock);
        }

    /* Process DAS configuration records */

        if (type == IDA_CONFIG && Params->digitizer == ISP_DAS) {
            util_log(1, "DAS configuration record received");
            save_config((char *) packet);
            MUTEX_LOCK(&Status->lock);
                SaveDasConfig(&record.k);
                Status->have_config = 1;
                Status->config = record.k;
            MUTEX_UNLOCK(&Status->lock);
            if (!initializing()) ConfigureDas(0);
        }

    /* Process DAS log records */

        if (type == IDA_LOG && Params->digitizer == ISP_DAS) {
            util_log(2, "DAS log received");
            SaveDasLog(&record.l);
        } else if (type == IDA_LOG && Params->digitizer == ISP_SAN) {
            util_log(2, "SAN log received");
        }

    /* Process DAS calibration records */

        if (type == IDA_CALIB && Params->digitizer == ISP_DAS) {
            util_log(2, "DAS calibration record received");
        } else if (type == IDA_CALIB && Params->digitizer == ISP_SAN) {
            util_log(2, "SAN calibration record received");
        }

    /* Process DAS event records */

        if (type == IDA_EVENT) {
            util_log(2, "DAS event record received");
        }

    /* Process ISP log records */

        if (type == IDA_ISPLOG) {
            util_log(2, "ISP log record received");
        }

    /* Release packet to massio or heap, depending of if we want it */

        if (Params->digitizer == ISP_SAN) {
            if (type == IDA_LOG && !SaveSanLog(&record.lm)) {
                want = 0;
            } else {
                want = 1;
            }
        } else if (type != IDA_DATA && type != IDA_DASSTAT) {
            want = 1;
        } else {
            want = packet[IDA_BUFSIZ-1]; /* IDA10 OK */
        }

        if (want) {
            msgq_put(&Q->massio, msg);
        } else {
            msgq_put(&Heap->packets, msg);
        }
    }
}

void InitProcess()
{
THREAD tid;
static char *fid = "InitProcess";

    if (!THREAD_CREATE(&tid, ProcessThread, NULL)) {
        util_log(1, "%s: failed to create ProcessThread", fid);
        ispd_die(MY_MOD_ID + 2);
    }
}

/* Revision History
 *
 * $Log: process.c,v $
 * Revision 1.13  2007/01/05 00:47:54  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.12  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.11  2005/08/26 20:15:07  dechavez
 * remove tee support (now handled by ISI disk loop I/O)
 *
 * Revision 1.10  2005/04/04 20:20:49  dechavez
 * moved tee code out to its own file
 *
 * Revision 1.9  2004/01/29 19:03:41  dechavez
 * use static variables where appropriate
 *
 * Revision 1.8  2002/09/12 16:07:55  dechavez
 * casts to quiet certain compilers
 *
 * Revision 1.7  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.6  2001/12/20 18:27:35  dec
 * tee all packets, and fflush when done
 *
 * Revision 1.5  2001/10/24 23:13:51  dec
 * Tee all packet types (not just data) and call ZipTeeFiles when closing
 * each tee file.
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
