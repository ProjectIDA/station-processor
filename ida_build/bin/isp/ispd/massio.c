#pragma ident "$Id: massio.c,v 1.15 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Take care of output to the mass storage device.  Packets are 
 *  copied from the upstream message queue to an internal buffer.
 *  When the internal buffer is full, or when requested, it is released
 *  to the thread that actually does the writing.
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
#include <sys/types.h>
#include <sys/stat.h>
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_MASSIO

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;

static int done, initialized = 0;
static size_t nrec, maxrec  = 0;
static size_t *nrec_ptr;
static int  flush_flag = 0;
static MUTEX mp = MUTEX_INITIALIZER;

static struct {
    int flag;
    MUTEX mp;
} idents = {0, MUTEX_INITIALIZER};

static struct {
    int iddata;
    int config;
    MUTEX mp;
} have = {0, 0, MUTEX_INITIALIZER};

static struct {
    char iddata[IDA_BUFSIZ]; /* IDA10 OK */
    char config[IDA_BUFSIZ]; /* IDA10 OK */
} saved;

static struct {
    int flag;
    MUTEX mp;
} need = {0, MUTEX_INITIALIZER};

/* Save iddata packet */

void save_iddata(char *buf)
{
    MUTEX_LOCK(&have.mp);
        memcpy(saved.iddata, buf, IDA_BUFSIZ); /* IDA10 OK */
        have.iddata = 1;
    MUTEX_UNLOCK(&have.mp);
}

/* Save config packet */

void save_config(char *buf)
{
    MUTEX_LOCK(&have.mp);
        memcpy(saved.config, buf, IDA_BUFSIZ); /* IDA10 OK */
        have.config = 1;
    MUTEX_UNLOCK(&have.mp);
}

/* Flush current buffer */

void flush_buffer(int force)
{
    if (OutputMediaType() != ISP_OUTPUT_TAPE) {
        FlushIsoImage();
        return;
    }

    MUTEX_LOCK(&mp);
        MUTEX_LOCK(&Status->lock);
            if (initialized && Status->buffer.nrec > 0) flush_flag = 1;
        MUTEX_UNLOCK(&Status->lock);
    MUTEX_UNLOCK(&mp);

    if (flush_flag) {
        util_log(1, "massio buffer flush requested");
    } else {
        util_log(1, "massio buffer flush ignored (nrec = 0)");
        check_eject_flag();
        if (shutting_down()) complete_shutdown();
    }
}

void flush_idents()
{
    MUTEX_LOCK(&need.mp);
        need.flag = 1;
    MUTEX_UNLOCK(&need.mp);
}

/* Copy data from input to output buffer */

static void copy_data(char *obuf, char *ibuf)
{
    MUTEX_LOCK(&mp);
        MUTEX_LOCK(&Status->lock);
            nrec = Status->buffer.nrec;
        MUTEX_UNLOCK(&Status->lock);
        memcpy(
            obuf + sizeof(size_t) + (nrec * IDA_BUFSIZ), /* IDA10 OK */
            ibuf,
            IDA_BUFSIZ /* IDA10 OK */
        );
        MUTEX_LOCK(&Status->lock);
            *nrec_ptr = nrec = ++Status->buffer.nrec;
        MUTEX_UNLOCK(&Status->lock);
        done = (nrec == maxrec || flush_flag != 0);
        if (done) flush_flag = 0;
    MUTEX_UNLOCK(&mp);
}

/* Try to grab one buffer from the heap */

static int get_obuf(OLD_MSGQ_MSG **out)
{
static int flag = 0;
int retval;
OLD_MSGQ_MSG *obuf;

    MUTEX_LOCK(&mp);

        obuf = msgq_get(&Heap->obuf, OLD_MSGQ_NOBLOCK);

        if (obuf != (OLD_MSGQ_MSG *) NULL) {
            nrec_ptr = (size_t *) obuf->data;
            MUTEX_LOCK(&Status->lock);
                *nrec_ptr = Status->buffer.nrec = 0;
            MUTEX_UNLOCK(&Status->lock);
            *out = obuf;
            if (flag) {
                util_log(1, "NOTICE: massio heap OK");
                flag = 0;
            }
            retval = 1;

        } else {
            MUTEX_LOCK(&Status->lock);
                Status->buffer.nrec = -12345;
            MUTEX_UNLOCK(&Status->lock);
            if (++flag == 1) {
                util_log(1, "NOTICE: massio heap exhausted!");
            }
            retval = 0;
        }

    MUTEX_UNLOCK(&mp);

/* flush copies of identifiers, if requested */

    if (retval == 1) {
        MUTEX_LOCK(&need.mp);
            if (need.flag) {
                if (Params->digitizer == ISP_DAS) {
                    MUTEX_LOCK(&have.mp);
                        if (have.iddata) copy_data(obuf->data,saved.iddata);
                        if (have.config) copy_data(obuf->data,saved.config);
                    MUTEX_UNLOCK(&have.mp);
                } else if (Params->digitizer == ISP_SAN) {
                    FlushSanCF();
                }
                need.flag = 0;
            }
        MUTEX_UNLOCK(&need.mp);
    }

    return retval;
}

/* Thread to buffer QC'd packets for later dumping to mass storage */

static THREAD_FUNC MassioThread(void *dummy)
{
int ready;
u_long newly_lost = 0;
OLD_MSGQ_MSG *ibuf, *obuf;
static char *fid = "MassioThread";

    util_log(2, "MASSIO output thread started, tid = %d", THREAD_SELF());

/* Take upstream packets and copy them to the output buffer */

    ready = get_obuf(&obuf);

    while (1) {

        ibuf = (OLD_MSGQ_MSG *) NULL;
        while (ibuf == (OLD_MSGQ_MSG *) NULL) {
            ibuf = msgq_get(&Q->massio, OLD_MSGQ_NOBLOCK);
            if (ibuf == (OLD_MSGQ_MSG *) NULL) {
                if (shutting_down()) complete_shutdown();
                sleep(1);
            }
        };

        if (ready || (ready = get_obuf(&obuf))) {

        /* Copy packet to output buffer */

            copy_data(obuf->data, ibuf->data);

        /* Forward packet to NRTS thread */

            msgq_put(&Q->nrts, ibuf);

        /* Note any lost packets */

            if (newly_lost) {
                util_log(1, "NOTICE: %ld packets were lost to massio", 
                    newly_lost
                );
                newly_lost = 0;
            }

        } else {

        /* Forward packet to NRTS thread */

            msgq_put(&Q->nrts, ibuf);

        /* Count lost packets */

            MUTEX_LOCK(&Status->lock);
                ++Status->output.lost;
            MUTEX_UNLOCK(&Status->lock);
            ++newly_lost;
            done = 0;
        }

        if (done) {
            msgq_put(&Q->obuf, obuf);
            MUTEX_LOCK(&Status->lock);
                Status->buffer.nrec = 0;
            MUTEX_UNLOCK(&Status->lock);
            if (shutting_down()) THREAD_EXIT(0);
            ready = get_obuf(&obuf);
        }
    }
}

/* Thread to bypass massio buffering when no output device given */

static THREAD_FUNC BypassMassioThread(void *dummy)
{
OLD_MSGQ_MSG *ibuf, *obuf;
static char *fid = "BypassMassioThread";

    util_log(2, "BYPASS MASSIO output thread started, tid = %d", THREAD_SELF());

    while (1) {
        ibuf = msgq_get(&Q->massio, OLD_MSGQ_WAITFOREVER);
        msgq_put(&Q->nrts, ibuf);
    }
}

void InitMassio()
{
THREAD tid;
static char *fid = "InitMassio";

    maxrec = Params->bfact * Params->obuf;

    MUTEX_LOCK(&Status->lock);
        Status->buffer.capacity = maxrec;
        Status->output.lost     = 0;
    MUTEX_UNLOCK(&Status->lock);

    if (OutputMediaType() != ISP_OUTPUT_TAPE) {
        if (!THREAD_CREATE(&tid, BypassMassioThread, NULL)) {
            util_log(1, "%s: failed to start BypassMassioThread", fid);
            ispd_die(MY_MOD_ID + 1);
        }
    } else {
        if (Params->ida->rev.value != 8) {
            util_log(1, "%s: ERROR: tape output supported for rev 8 data only", fid);
            ispd_die(MY_MOD_ID + 2);
        }
        if (!THREAD_CREATE(&tid, MassioThread, NULL)) {
            util_log(1, "%s: failed to start MassioThread", fid);
            ispd_die(MY_MOD_ID + 3);
        }
    }

    MUTEX_LOCK(&mp);
        initialized = 1;
    MUTEX_UNLOCK(&mp);
}

/* Revision History
 *
 * $Log: massio.c,v $
 * Revision 1.15  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.14  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.13  2005/10/11 18:24:23  dechavez
 * FlushIsoImage on flush_buffer for all but tape systems, removed redundant
 * UpdateCdStats from init
 *
 * Revision 1.12  2005/09/30 22:08:10  dechavez
 * attempt UpdateCdStats() for all cases except tape output
 *
 * Revision 1.11  2005/08/26 20:16:18  dechavez
 * Add a call to UpdateCdStats in InitMassio so that fresh isi displays show
 * current (and now decoupled from ISP) ISO stats without having to explicity
 * request an update manually
 *
 * Revision 1.10  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.9  2001/10/24 23:17:36  dec
 * added CDROM support and BypassMassioThread to use if not using tape
 *
 * Revision 1.8  2001/10/08 18:17:38  dec
 * bypass massio if no output selected
 *
 * Revision 1.7  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.6  2001/02/23 22:52:50  dec
 * input queue made non-blocking in order to permit graceful shutdown of
 * process when no output data are pending
 *
 * Revision 1.5  2001/02/23 19:10:50  dec
 * revert to Revision 1.3, and then call check_event_flag() when
 * ignoring 0 record buffer flush requests
 *
 * Revision 1.4  2001/02/23 17:19:34  dec
 * allow 0 record tape flushes (prevents inadvertent delayed tape ejects if
 * change tape command issued when no output data are ready)
 *
 * Revision 1.3  2000/11/08 01:57:24  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.2  2000/09/20 00:51:17  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
