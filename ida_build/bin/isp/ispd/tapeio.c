#pragma ident "$Id: tapeio.c,v 1.8 2007/01/05 02:35:49 dechavez Exp $"
/*======================================================================
 *
 *  Module to handle tape I/O
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

#ifdef INCLUDE_TAPE
#include "mtio.h"


#define MY_MOD_ID ISPD_MOD_TAPEIO

#define MAXBUFLEN 65536
static char tmpbuf[MAXBUFLEN];

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;

static TAPE *null = (TAPE *) NULL;
static TAPE *tp   = (TAPE *) NULL;
static u_long ioerr = 0;
static int eject_flag = 0;

static int crnt_state, prev_state = MTIO_OFFLINE;
static int pending = 0;
static MUTEX mp = MUTEX_INITIALIZER;
static SEMAPHORE sp;

void set_eject_flag(int value)
{
static char *fid = "set_eject_flag";

    MUTEX_LOCK(&mp);
        eject_flag = value;
    MUTEX_UNLOCK(&mp);
}

static void lock_device()
{
static char *fid = "lock_device";

    while (1) {
        MUTEX_LOCK(&mp);
            if (crnt_state == MTIO_ONLINE) {
                pending = 0;
                return; /* we've got the mutex */
            }
            pending = 1;
        MUTEX_UNLOCK(&mp);
        SEM_WAIT(&sp); /* block until back on-line */
    }
}

#define release_device() MUTEX_UNLOCK(&mp);

int eject_tape(int sync)
{
static char *fid = "eject_tape";

    if (sync) MUTEX_LOCK(&mp);
        if (mtio_state(tp) == MTIO_ONLINE) {
            util_log(1, "ejecting tape");
            if (mtio_eject(tp) != 0) {
                util_log(1, "%s: %s: %s",
                    fid, Params->odev, syserrmsg(errno)
                );
            }
            mtio_close(tp); tp = (TAPE *) tp;
            tp = (TAPE *) NULL;
            prev_state = crnt_state;
            crnt_state = MTIO_OFFLINE;
            set_alarm(ISP_ALARM_OFFLINE);
        }
    if (sync) MUTEX_UNLOCK(&mp);

    return 0;
}

/* Check newly inserted tapes for data.  If data are present, then
 * wait for operator instructions on how to proceed.
 */

static void check_tape(int boot_flag)
{
size_t status, buflen;
int action;
static char *fid = "check_tape";

    MUTEX_LOCK(&Status->lock);
        Status->output.state = ISP_OUTPUT_CHECK;
    MUTEX_UNLOCK(&Status->lock);

/* Check for data by trying to read from the tape */

    if (mtio_rew(tp) < 0) {
        util_log(1, "%s: mtio_rew: %s", fid, syserrmsg(errno));
        eject_tape(0);
        return;
    }

    status = mtio_read(tp, tmpbuf, MAXBUFLEN);

    if (mtio_rew(tp) < 0) {
        util_log(1, "%s: mtio_rew: %s", fid, syserrmsg(errno));
        eject_tape(0);
        return;
    }

    if (status > 0) {
        util_log(1, "tape contains data", fid);
        if (boot_flag) {
            action = ISP_MEDIA_APPEND;
        } else {
            action = media_alert(0);
        }
        switch (action) {
          case ISP_MEDIA_EJECT:
            eject_tape(0);
            break;
          case ISP_MEDIA_OVERWRITE:
            util_log(1, "OVERWRITTING TAPE");
            MUTEX_LOCK(&Status->lock);
                Status->output.state = ISP_OUTPUT_ONLINE;
            MUTEX_UNLOCK(&Status->lock);
            break;
          default:
            util_log(1, "skipping to end of data");
            MUTEX_LOCK(&Status->lock);
                Status->output.state = ISP_OUTPUT_SKIP;
            MUTEX_UNLOCK(&Status->lock);
            mtio_eom(tp);
            MUTEX_LOCK(&Status->lock);
                Status->output.state = ISP_OUTPUT_ONLINE;
            MUTEX_UNLOCK(&Status->lock);
            break;
        }
    }

    return;
}

/* Thread to watch for tape inserts/removals */

static THREAD_FUNC TapeWatchThread(void *dummy)
{
int boot_flag;
static struct mtget info;
static char *fid = "TapeWatchThread";

    boot_flag = 1;
    util_log(2, "TAPE_WATCH thread started, tid = %d", THREAD_SELF());

    set_alarm(ISP_ALARM_OFFLINE);

    while (1) {
        
        MUTEX_LOCK(&mp);

        /* if we were offline, see if a tape is now present */

            if (prev_state == MTIO_OFFLINE) {
                if ((tp = mtio_open(Params->odev, "w")) != null) {
                    if (mtio_status(tp, &info) != -1) {
                        crnt_state = MTIO_ONLINE;
                        clear_alarm(ISP_ALARM_OFFLINE);
                        if (ioerr) {
                            clear_alarm(ISP_ALARM_IOERR);
                            ioerr = 0;
                        }
                        check_tape(boot_flag);
                    } else {
                        mtio_close(tp); tp = (TAPE *) NULL;
                    }
                }

                if (tp == null && shutting_down()) {
                    util_log(1, "shutting down w/o final flush (no tape)");
                    complete_shutdown();
                }

        /* if we were on-line, make sure we still are that way */
        /* THIS CODE DOES NOT WORK AND I DON'T KNOW HOW TO MAKE IT WORK! */

            } else if (mtio_nop(tp) < 0 || mtio_status(tp, &info) < 0) {
                mtio_close(tp); tp = (TAPE *) tp;
                set_alarm(ISP_ALARM_OFFLINE);
                crnt_state = MTIO_OFFLINE;
            }

            if (crnt_state != prev_state) {
                MUTEX_LOCK(&Status->lock);
                    if (crnt_state == MTIO_ONLINE) {
                        Status->output.state = ISP_OUTPUT_ONLINE;
                        Status->output.err   = 0;
                        Status->output.file  = info.mt_fileno + 1;
                        Status->output.nrec  = 0;
                    } else {
                        Status->output.state = ISP_OUTPUT_OFFLINE;
                    }
                MUTEX_UNLOCK(&Status->lock);
            }

            prev_state = crnt_state;
            boot_flag = 0;

            if (pending && (crnt_state == MTIO_ONLINE)) {
                pending = 0;
                SEM_POST(&sp);
            }

        MUTEX_UNLOCK(&mp);

        sleep(10);
    }
}

/* Pull full buffers of the queue and dump them to tape */

static THREAD_FUNC TapeWriteThread(void *dummy)
{
int status;
size_t blen, remain, want, put, nrec;
char *ptr;
OLD_MSGQ_MSG *obuf;
static char *fid = "TapeWriteThread";

    blen = Params->bfact * IDA_BUFSIZ; /* IDA10 OK */
    MUTEX_LOCK(&mp);
        ioerr = 0;
    MUTEX_UNLOCK(&mp);

    while (1) {

        obuf = msgq_get(&Q->obuf, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, obuf)) {
            util_log(1, "%s: corrupt message received", fid);
            ispd_die(MY_MOD_ID + 1);
        }
        nrec = *((size_t *) obuf->data);
        util_log(1, "dumping %ld records to %s", nrec, Params->odev);

        ptr = obuf->data + sizeof(size_t);
        remain = nrec * IDA_BUFSIZ; /* IDA10 OK */

        while (remain > 0) {
            want = remain > blen ? blen : remain;
            nrec = want / IDA_BUFSIZ; /* IDA10 OK */
            do {
                lock_device();
                    put = mtio_write(tp, ptr, want);
                    if (put == want) {
                        MUTEX_LOCK(&Status->lock);
                            Status->output.nrec += nrec;
                            status = Status->output.state;
                        MUTEX_UNLOCK(&Status->lock);
                        ptr += put;
                        if (ioerr) {
                            clear_alarm(ISP_ALARM_IOERR);
                            ioerr = 0;
                        }
                    } else {
                        if (put != 0) {
                            if (++ioerr == 1) {
                                set_alarm(ISP_ALARM_IOERR);
                                util_log(1, "%s: %s",
                                    Params->odev, syserrmsg(errno)
                                );
                            }
                            MUTEX_LOCK(&Status->lock);
                                ++Status->output.err;
                            MUTEX_UNLOCK(&Status->lock);
                            eject_tape(0);
                        }
                    }
                release_device();
                if (put != want) {
                    if (shutting_down()) {
                        complete_shutdown();
                        THREAD_EXIT(0);
                    } else {
                        sleep(5);
                    }
                }
            } while (put != want);
            remain -= put;
        }
        util_log(1, "tape dump completed OK");

        if (shutting_down()) {
            complete_shutdown();
            THREAD_EXIT(0);
        }

        MUTEX_LOCK(&mp);
            if (eject_flag) eject_tape(0);
            eject_flag = 0;
        MUTEX_UNLOCK(&mp);

        msgq_put(&Heap->obuf, obuf);
    }
}

void check_eject_flag()
{
    MUTEX_LOCK(&mp);
        if (eject_flag) eject_tape(0);
        eject_flag = 0;
    MUTEX_UNLOCK(&mp);
}

int tapeio_init()
{
int i;
THREAD tid;
static char *fid = "tapeio_init";

    SEM_INIT(&sp, 0, 1);

    MUTEX_LOCK(&Status->lock);
        strncpy(Status->output.device, Params->odev, ISP_DEVNAMELEN - 1);
        Status->output.device[ISP_DEVNAMELEN-1] = 0;
        Status->output.type  = ISP_OUTPUT_TAPE;
        Status->output.state = ISP_OUTPUT_UNKNOWN;
    MUTEX_UNLOCK(&Status->lock);

/* Start tape watch thread */

    if (!THREAD_CREATE(&tid, TapeWatchThread, NULL)) {
        util_log(1, "%s: failed to start TapeWatchThread", fid);
        ispd_die(MY_MOD_ID + 3);
    }

/* Start tape writer thread */

    if (!THREAD_CREATE(&tid, TapeWriteThread, NULL)) {
        util_log(1, "%s: failed to start TapeWriteThread", fid);
        ispd_die(MY_MOD_ID + 4);
    }
 
    return 0;
}

#else

int tapeio_init()
{
    return;
}

void check_eject_flag()
{
    return;
}

int eject_tape(int sync)
{
    return 0;
}

void set_eject_flag(int value)
{
    return;
}

#endif /* INCLUDE_TAPE */

/* Revision History
 *
 * $Log: tapeio.c,v $
 * Revision 1.8  2007/01/05 02:35:49  dechavez
 * removed digital power monitor and tape support
 *
 * Revision 1.7  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.6  2002/11/05 17:43:45  dechavez
 * removed IDA 8 only restriction for tape I/O (assumes SAN rev 1.11.3 or
 * greater)
 *
 * Revision 1.5  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2001/02/23 19:08:54  dec
 * added check_eject_flag()
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
