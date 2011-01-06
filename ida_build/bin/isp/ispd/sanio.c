#pragma ident "$Id: sanio.c,v 1.12 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Here we handle all data I/O with the SAN.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"
#include "sanio.h"
#include "xfer.h"

#define MY_MOD_ID ISPD_MOD_SANIO

#define THREAD_EXIT_NOTIFICATION -1

extern ISP_PARAMS *Params;
extern struct isp_status *Status;
extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;
static SANIO_CONFIG SanCnf;
static BOOL HaveSanCnfFlag = FALSE;

static SAN_HANDLE *san; /* command & control */
static MUTEX mutex;
THREAD WriteTid;

static void SetWriteTid(THREAD tid)
{
    MUTEX_LOCK(&mutex);
        WriteTid = tid;
    MUTEX_UNLOCK(&mutex);
}

static THREAD CrntWriteTid()
{
THREAD retval;

    MUTEX_LOCK(&mutex);
        retval = WriteTid;
    MUTEX_UNLOCK(&mutex);
    return retval;
}

BOOL GetSanCnf(SANIO_CONFIG *dest)
{
BOOL retval;

    MUTEX_LOCK(&mutex);
        if ((retval = HaveSanCnfFlag) == TRUE) *dest = SanCnf;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

char *SanStreamName(int index)
{
static char name[16];

    MUTEX_LOCK(&mutex);
        if (HaveSanCnfFlag) {
            strcpy(name, SanCnf.stream[index].name);
        } else {
            sprintf(name, "%03d", index);
        }
    MUTEX_UNLOCK(&mutex);

    return name;
}

static BOOL HaveSanCnf()
{
BOOL retval;

    MUTEX_LOCK(&mutex);
        retval = HaveSanCnfFlag;
    MUTEX_UNLOCK(&mutex);

    return retval;
}

static void SaveSanCnf(SANIO_CONFIG *src)
{
    MUTEX_LOCK(&mutex);
        SanCnf = *src;
        HaveSanCnfFlag = TRUE;
    MUTEX_UNLOCK(&mutex);
    set_have_dascnf();
}

static void ClearSanCnf()
{
    MUTEX_LOCK(&mutex);
        HaveSanCnfFlag = FALSE;
    MUTEX_UNLOCK(&mutex);
    clear_have_dascnf();
}

/* Thread to send commands to SAN */

static void *sanCmndWrite(void *dummy)
{
OLD_MSGQ_MSG *msg;
BOOL ok;
THREAD mytid;
int cmnd;
static UINT8 buf[SANIO_BUFLEN];
static char *fid = "sanCmndWrite";

    mytid = THREAD_SELF();
    util_log(2, "%s thread started, tid = %d", fid, mytid);

    SetWriteTid(mytid);

    while (!HaveSanCnf()) {
        sanioRequestConfig(san, buf);
        sleep(10);
    }

/* Main loop */

    while (1) {

        if (CrntWriteTid() != mytid) {
            util_log(2, "%s: tid exits (no longer current)", fid);
            THREAD_EXIT(0);
        }

        msg = msgq_get(&Q->das_write, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received!", fid);
            ispd_die(MY_MOD_ID + 1);
        }

        if (CrntWriteTid() != mytid) {
            msgq_put(&Heap->commands, msg);
            util_log(2, "%s: tid exits (no longer current)", fid);
            THREAD_EXIT(0);
        }

        cmnd = *((int *) msg->data);

        switch (cmnd) {
          case SAN_NOP:
            util_log(3, "%s(%d): SAN_NOP", fid, mytid);
            ok = sanioSendNop(san, buf);
            break;
          case SAN_SNDCONFIG:
            util_log(3, "%s(%d): SAN_SNDCONFIG", fid, mytid);
            ok = sanioRequestConfig(san, buf);
            break;
          case SAN_SNDSOH:
            util_log(3, "%s(%d): SAN_SNDSOH", fid, mytid);
            ok = sanioRequestSoh(san, buf);
            break;
          case SAN_SNDSOH_A:
            util_log(3, "%s(%d): SAN_SNDSOH_A", fid, mytid);
            ok = sanioRequestSohA(san, buf);
            break;
          case SAN_SNDSOH_B:
            util_log(3, "%s(%d): SAN_SNDSOH_B", fid, mytid);
            ok = sanioRequestSohB(san, buf);
            break;
          case SAN_SNDSOH_C:
            util_log(3, "%s(%d): SAN_SNDSOH_C", fid, mytid);
            ok = sanioRequestSohC(san, buf);
            break;
          case SAN_SNDSOH_D:
            util_log(3, "%s(%d): SAN_SNDSOH_D", fid, mytid);
            ok = sanioRequestSohD(san, buf);
            break;
          case SAN_REBOOT:
            util_log(1, "SAN reboot", fid, mytid);
            ok = sanioReboot(san, TRUE, buf);
            break;
          case THREAD_EXIT_NOTIFICATION:
            util_log(3, "%s(%d): THREAD_EXIT_NOTIFICATION", fid, mytid);
            util_log(1, "%s: THREAD_EXIT() by command", fid);
            THREAD_EXIT(0);
          default:
            util_log(1, "%s(%d): unknown %d ignored", fid, mytid, cmnd);
            ok = TRUE;
        }

        msgq_put(&Heap->commands, msg);
    }
}

static void startWriteThread()
{
THREAD tid;
static char *fid = "startWriteThread";

    if (!THREAD_CREATE(&tid, sanCmndWrite, NULL)) {
        util_log(1, "%s: failed to start sanCmndWrite", fid);
        ispd_die(MY_MOD_ID + 2);
    }
}

static void reconnect(int cause, UINT8 *buf)
{
BOOL clear;
static char *fid = "reconnect";

    set_alarm(ISP_ALARM_DAS);
    MUTEX_LOCK(&Status->lock);
        Status->san.cmnd.connected = 0;
        Status->san.boottime = -1;
    MUTEX_UNLOCK(&Status->lock);

    util_log(1, "reconnect with SAN C&C");
    SanCmnd(THREAD_EXIT_NOTIFICATION);
    sanioDisconnect(san, cause, buf);
    ClearSanCnf();
    if (!sanioConnect(san)) {
        util_log(1, "%s: sanioConnect failed!", fid);
        ispd_die(MY_MOD_ID + 3);
    }

    MUTEX_LOCK(&Status->lock);
        Status->san.cmnd.connected = time(NULL);
        ++Status->san.cmnd.count.reconn;
        clear = (Status->san.data.connected == 0) ? FALSE : TRUE;
    MUTEX_UNLOCK(&Status->lock);
    if (clear) clear_alarm(ISP_ALARM_DAS);

    startWriteThread();
}
    
    

/* Thread to handle C&C returns from the SAN */

static void *sanCmndRead(void *dummy)
{
static int type;
static UINT8 *data;
UINT16 new_state;
SANIO_SOH soh;
SANIO_CONFIG cnf;
UINT8 buf[SANIO_BUFLEN];
BOOL clear, HaveSoh = FALSE, NewCF = FALSE;
static char *fid = "sanCmndRead";

    util_log(2, "%s thread started, tid = %d", fid, THREAD_SELF());

/* Connect to server */

    if (!sanioConnect(san)) {
        util_log(1, "%s: sanioConnect failed!", fid);
        ispd_die(MY_MOD_ID + 4);
    }
    MUTEX_LOCK(&Status->lock);
        Status->san.cmnd.connected = time(NULL);
        clear = (Status->san.data.connected == 0) ? FALSE : TRUE;
    MUTEX_UNLOCK(&Status->lock);
    if (clear) clear_alarm(ISP_ALARM_DAS);

/* Fire up writer */

    startWriteThread();

/* Main loop */

    while (1) {

        switch (type = (int) sanioRecv(san, &data)) {

          case SAN_NOP:
            util_log(3, "%s: SAN_NOP", fid);
            break;

          case SAN_LOGMSG:
            util_log(3, "%s: SAN_LOGMSG", fid);
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p3;
            MUTEX_UNLOCK(&Status->lock);
            break;

          case SAN_NEWKEY:
            util_log(3, "%s: SAN_NEWKEY", fid);
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p3;
            MUTEX_UNLOCK(&Status->lock);
            break;

          case SAN_SOH:
            util_log(3, "%s: SAN_SOH", fid);
            sanioUnpackSOH(data, &soh);
            UpdateSanSoh(san, &soh);
            HaveSoh = TRUE;
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p1;
            MUTEX_UNLOCK(&Status->lock);
            break;

          case SAN_CONFIG:
            util_log(3, "%s: SAN_CONFIG", fid);
            sanioUnpackConfig(data, &cnf);
            SaveSanCnf(&cnf);
            NewCF = TRUE;
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p2;
            MUTEX_UNLOCK(&Status->lock);
            break;

          case SAN_STATE:
            util_log(3, "%s: SAN_STATE", fid);
            utilUnpackUINT16(data, &new_state);
            if (new_state != san->state) {
                if ((san->state = new_state) == SAN_OBSERVER) {
                    util_log(1, "SAN C&C state changed to observer");
                } else {
                    util_log(1, "SAN C&C state changed to OPERATOR");
                }
            }
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p3;
            MUTEX_UNLOCK(&Status->lock);
            break;

          case SAN_TIMEDOUT:
            util_log(3, "%s: SAN_TIMEDOUT", fid);
            util_log(1, "SAN C&C read timeout");
            reconnect(type, buf);
            break;

          case SAN_ERROR:
            util_log(3, "%s: SAN_ERROR", fid);
            util_log(1, "SAN C&C read error");
            reconnect(type, buf);
            break;

          case SAN_DISCONNECT:
            util_log(3, "%s: SAN_DISCONNECT", fid);
            util_log(1, "SAN C&C server disconnect");
            reconnect(type, buf);
            break;

          default:
            util_log(1, "%s: unknown message type %d ignored", fid, type);
            MUTEX_LOCK(&Status->lock);
                ++Status->san.cmnd.count.p3;
            MUTEX_UNLOCK(&Status->lock);
        }

        if (type != SAN_TIMEDOUT && type != SAN_ERROR) {
            MUTEX_LOCK(&Status->lock);
                Status->san.cmnd.lastRead = time(NULL);
            MUTEX_UNLOCK(&Status->lock);
        }

        if (NewCF && HaveSoh) {
            BuildSanCF(&cnf, time(NULL), soh.boxID);
            FlushSanCF();
            NewCF = FALSE;
        }
    }
}

static void *sanData(void *dummy)
{
OLD_MSGQ_MSG *msg;
int status;
BOOL clear;
static int sd = -1;
static int count = 0;
static struct xfer_req req;
static struct xfer_cnf cnf;
static struct xfer_wav wav;
static char *fid = "sanData";

    util_log(2, "SAN data feed thread started, tid = %d", THREAD_SELF());

/* Main loop */

    while (1) {

   /* Establish connection with server */

        while (sd <= 0) {
            while (!HaveSanCnf()) sleep(5);
            util_log(2, "generating request parameters");
            if (!GenerateRequest(&req)) {
                util_log(1, "%s: GenerateRequest() failed", fid);
                ispd_die(MY_MOD_ID + 5);
            }
            sd = Xfer_Connect2(
                Params->san.addr,
                NULL,
                Params->san.port.data,
                "tcp",
                &req,
                &cnf,
                -1, /* always retry */
                0 /* no tto, to force MT-safe util_connect() */
            );
            if (sd <= 0) {
                if (
                    (xfer_errno == XFER_ETIMEDOUT) ||
                    (xfer_errno == XFER_ECONNECT)  ||
                    (xfer_errno == XFER_EHOSTNAME)
                ) {
                    ;
                } else {
                    util_log(1, "%s: %s", Params->san.addr, Xfer_ErrStr());
                }
                util_log(1, "%s: sleep(60)", fid);
                sleep(60);
            } else {
                util_log(2, "request accepted");
                MUTEX_LOCK(&Status->lock);
                    if (count) ++Status->san.data.count.reconn;
                    Status->san.data.connected = time(NULL);
                    clear = (Status->san.cmnd.connected == 0) ? FALSE : TRUE;
                MUTEX_UNLOCK(&Status->lock);
                if (clear) clear_alarm(ISP_ALARM_DAS);
            }
        }

    /* Read a packet from the server */

        if ((status = Xfer_RecvWav(sd, &cnf, &wav)) != XFER_OK) {
            MUTEX_LOCK(&Status->lock);
                Status->san.data.connected = 0;
            MUTEX_UNLOCK(&Status->lock);
            shutdown(sd, 2);
            close(sd);
            sd = -1;
            set_alarm(ISP_ALARM_DAS);
        } else if (++count == 1) {
            util_log(1, "initial packet received");
        }
        if (status == XFER_OK) {
            MUTEX_LOCK(&Status->lock);
                Status->san.data.lastRead = time(NULL);
            MUTEX_UNLOCK(&Status->lock);
        }

    /* Sanity checks */

        if (status == XFER_OK && wav.format != XFER_WAVIDA) {
            util_log(1, "%s: protocol error! data not in IDA format!", fid);
            ispd_die(MY_MOD_ID + 6);
        }

        if (status == XFER_FINISHED) {
            util_log(1, "%s: unexpected XFER_FINISH!", fid);
            ispd_die(MY_MOD_ID + 7);
        }

    /* Copy packet into downstream message queue */

        msg = msgq_get(&Heap->packets, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received!", fid);
            ispd_die(MY_MOD_ID + 8);
        }
        if ((msg->len = wav.type.ida.nbyte) > IDA_MAXRECLEN) {
            util_log(1, "%s: limit error: %d > %d: increase IDA_MAXRECLEN",
                fid, wav.type.ida.nbyte, IDA_MAXRECLEN
            );
            ispd_die(MY_MOD_ID + 9);
        }
        memcpy(msg->data, wav.type.ida.data, msg->len);
        msgq_put(&Q->das_process, msg);
    }
}

void InitSanIO()
{
THREAD tid;
static char *fid = "InitSanIO";

    MUTEX_INIT(&mutex);

    set_alarm(ISP_ALARM_DAS);

/* Start data acquistion thread */

    if (!THREAD_CREATE(&tid, sanData, NULL)) {
        util_log(1, "%s: failed to start sanData", fid);
        ispd_die(MY_MOD_ID + 10);
    }

/* Start command and control threads */

    san = sanioInit(
        Params->san.addr,
        Params->san.port.cmnd,
        Params->san.timeout,
        TRUE
    );
    if (san == NULL) {
        util_log(1, "%s: sanioInit failed", fid);
        ispd_die(MY_MOD_ID + 11);
    }

    if (!THREAD_CREATE(&tid, sanCmndRead, NULL)) {
        util_log(1, "%s: failed to start sanCmndRead", fid);
        ispd_die(MY_MOD_ID + 12);
    }
}

/* Revision History
 *
 * $Log: sanio.c,v $
 * Revision 1.12  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.11  2004/01/29 19:03:41  dechavez
 * use static variables where appropriate
 *
 * Revision 1.10  2003/12/10 06:25:48  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.9  2003/05/23 19:52:19  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.8  2002/03/15 22:51:38  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.7  2001/10/24 23:10:43  dec
 * set Xfer_Connect2 tto argument to 0 in order to invoke the MT-safe
 * connect() wrapper
 *
 * Revision 1.6  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.5  2000/11/15 19:57:40  dec
 * include heartbeats when determining last read time
 *
 * Revision 1.4  2000/11/08 19:05:33  dec
 * fix reconnect and no CF bug
 *
 * Revision 1.3  2000/11/08 01:57:24  dec
 * Release 2.1 (Support SAN CF records).
 *
 * Revision 1.2  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/11 20:37:45  dec
 * initial version
 *
 */
