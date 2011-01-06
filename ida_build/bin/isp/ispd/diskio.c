#pragma ident "$Id: diskio.c,v 1.4 2006/02/10 02:24:11 dechavez Exp $"
/*======================================================================
 *
 *  Module to handle removable disk I/O.  I've been lazy about keeping
 *  the structure of this compatible with that in tapeio, since I 
 *  don't think it will ever get used.
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
#include "rdio.h"

#define MY_MOD_ID ISPD_MOD_DISKIO

extern ISP_PARAMS *Params;
extern struct isp_status *Status;

static RDISK *null = (RDISK *) NULL;
static RDISK *rd   = (RDISK *) NULL;

static int previous_state = RDIO_OFFLINE;
static MUTEX diskio_lock = MUTEX_INITIALIZER;

static void check_disk()
{
int factor;
int state;

    state = rdio_state(rd);

    MUTEX_LOCK(&Status->lock);
        strncpy(Status->output.device, Params->odev, ISP_DEVNAMELEN - 1);
        Status->output.device[ISP_DEVNAMELEN-1] = 0;
        Status->output.type = ISP_OUTPUT_DISK;
        if (state == RDIO_ONLINE) {
            Status->output.state = ISP_OUTPUT_ONLINE;
            Status->output.err   = 0;
            if (rdio_bsize(rd) < IDA_BUFSIZ) { /* IDA10 OK */
                factor = IDA_BUFSIZ / rdio_bsize(rd); /* IDA10 OK */
                Status->output.nrec     = rdio_nrec(rd) / factor;
                Status->output.capacity = rdio_capacity(rd) / factor;
            } else {
                factor = rdio_bsize(rd) / IDA_BUFSIZ; /* IDA10 OK */
                Status->output.nrec     = rdio_nrec(rd) * factor;
                Status->output.capacity = rdio_capacity(rd) * factor;
            }
        } else {
            Status->output.state = ISP_OUTPUT_OFFLINE;
        }
    MUTEX_UNLOCK(&Status->lock);

    if (previous_state != state) {
        if (state == RDIO_ONLINE) {
            clear_alarm(ISP_ALARM_OFFLINE);
        } else {
            set_alarm(ISP_ALARM_OFFLINE);
        }
        previous_state = state;
    }
}

int eject_disk(int sync)
{
static char *fid = "eject_disk";

    if (sync) MUTEX_LOCK(&diskio_lock);

        if (rdio_state(rd) == RDIO_ONLINE) {
            util_log(1, "ejecting disk");
            if (rdio_eject(rd) != 0) {
                util_log(1, "%s: %s: %s",
                    fid, Params->odev, syserrmsg(errno)
                );
            }
        }

        check_disk();

    if (sync) MUTEX_UNLOCK(&diskio_lock);

    return 0;
}

/* Thread to watch for disk inserts/removals */

static THREAD_FUNC DiskWatchThread(void *dummy)
{
int oldstate, newstate;
static char *fid = "DiskWatchThread";

    util_log(2, "DISK_WATCH thread started, tid = %d", THREAD_SELF());

    MUTEX_LOCK(&diskio_lock);
        oldstate = rdio_state(rd);
    MUTEX_UNLOCK(&diskio_lock);

    while (1) {
        
        MUTEX_LOCK(&diskio_lock);
            newstate = rdio_state(rd);
            if (newstate != oldstate) {
                check_disk();
                oldstate = newstate;
            }
        MUTEX_UNLOCK(&diskio_lock);
    
        sleep(5);
    }
}

int disk_write(char *buf, int bfact, size_t buflen, int reason)
{
u_long ioerr;
int status;
size_t blen, remain, want, put, nrec;
char *ptr;
static char *fid = "disk_write";

    set_alarm(ISP_ALARM_OFFLINE);

    if (buflen % IDA_BUFSIZ != 0) { /* IDA10 OK */
        util_log(1, "%s: incompatible buflen %ld!", fid, buflen);
        ispd_die(MY_MOD_ID + 3);
    }

    ioerr = 0;
    blen = bfact * IDA_BUFSIZ; /* IDA10 OK */

    if (reason != ISPD_FLUSH_FULL) util_log(1, "flushing buffered data");

    ptr = buf;
    remain = buflen;
    while (remain > 0) {
        want = remain > blen ? blen : remain;
        nrec = want / IDA_BUFSIZ; /* IDA10 OK */
        do {
            MUTEX_LOCK(&diskio_lock);
            put  = rdio_write(rd, ptr, want);
            if (put == want) {
                MUTEX_LOCK(&Status->lock);
                    Status->output.nrec += nrec;
                    Status->buffer.nrec -= nrec;
                    status = Status->output.state;
                MUTEX_UNLOCK(&Status->lock);
                ptr += put;
                if (ioerr) {
                    clear_alarm(ISP_ALARM_IOERR);
                    ioerr = 0;
                }
            } else {
                if (put != 0) {
                    util_log(1, "%s: %s", Params->odev, syserrmsg(errno));
                    MUTEX_LOCK(&Status->lock);
                        ++Status->output.err;
                    MUTEX_UNLOCK(&Status->lock);
                    eject_disk(0);
                    if (++ioerr == 1) set_alarm(ISP_ALARM_IOERR);
                }
            }
            MUTEX_UNLOCK(&diskio_lock);
            if (put != want) sleep(5);
        } while (put != want);
        remain -= put;
    }

    return 0;
}

int diskio_init()
{
THREAD tid;
int i;
static char *fid = "diskio_init";

    if (Params->ida->rev.value != 8) {
        util_log(1, "%s: ERROR: disk I/O routines only support IDA rev 8", fid);
        return -1
    }

    MUTEX_LOCK(&Status->lock);
        Status->output.device[0] = 0;
        Status->output.type      = ISP_OUTPUT_DISK;
        Status->output.state     = ISP_OUTPUT_UNKNOWN;
        Status->output.nrec      = -1;
        Status->output.capacity  = -1;
        Status->output.err       = -1;
    MUTEX_UNLOCK(&Status->lock);

/* Open the output device */

    if ((rd = rdio_open(Params->odev, "a")) == null) {
        util_log(1, "%s: rdio_open: %s: %s", 
            fid, Params->odev, syserrmsg(errno)
        );
        return -1;
    }

/* If there is no media inserted at this time then we will refuse to
 * proceed.  This is because there I can't figure out how to verify
 * that this is in fact a disk.
 */

    if (rdio_status(rd) == RDIO_OFFLINE) {
        util_log(1, "%s: %s: no media inserted(?)", fid, Params->odev);
        return -2;
    }

/* Record block size must be a multiple of the disk's block size */
    if (IDA_BUFSIZ % rdio_bsize(rd)) { /* IDA10 OK */
        util_log(1, "FATAL ERROR: %s does not support IDA block size",
            Params->odev
        );
        ispd_die(MY_MOD_ID + 1);
    }

    check_disk();

/* Start disk watch thread */

    if (!THREAD_CREATE(&tid, DiskWatchThread, NULL)) {
        util_log(1, "%s: failed to create DiskWatchThread", fid);
        ispd_die(MY_MOD_ID + 2);
    }

    return 0;
}

/* Revision History
 *
 * $Log: diskio.c,v $
 * Revision 1.4  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.2  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
