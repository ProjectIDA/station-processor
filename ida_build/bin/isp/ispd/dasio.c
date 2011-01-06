#pragma ident "$Id: dasio.c,v 1.8 2006/12/06 22:38:04 dechavez Exp $"
/*======================================================================
 *
 *  Here we handle all I/O with the DAS.
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

#define MY_MOD_ID ISPD_MOD_DASIO

/* Check for a pending DAS command and send it if there is one */

#define DASWRITE(m) \
m = msgq_get(&Q->das_write, OLD_MSGQ_NOBLOCK);\
if (m != (OLD_MSGQ_MSG *) NULL) das_write(m)

extern struct isp_dascnf *DasCnf;
extern struct isp_status *Status;
extern ISP_PARAMS *Params;
extern struct ispd_queue *Q;
extern struct ispd_heap  *Heap;

static int das = -1;
static char ack_str[2]     = {1, DAS_ACK};
static char nak_str[2]     = {1, DAS_NAK};
static char reboot_str[3]  = {2, DAS_RESET_CMD, DAS_CONFIRM_CMD};

#define ACK(das)    write(das, ack_str, 2)
#define NAK(das)    write(das, nak_str, 2)
#define REBOOT(das) write(das, reboot_str, 3)

/* Process one DAS command */

static void *das_write(OLD_MSGQ_MSG *msg)
{
char *ptr;
int ll, type, opt;
long len, rem;
static char *fid = "das_write";

/* Send the message to the DAS */

    ll  = msg->data[0];
    ptr = msg->data + 1;
    if (ptr[0] > 0) {
        rem = ptr[0] + 1;
        while (rem > 0) {
            len = rem > Params->mtu ? Params->mtu : rem;
            if (write(das, ptr, len) != len) {
                util_log(1, "%s: FATAL ERROR: write: %s",
                    fid, syserrmsg(errno)
                );
                ispd_die(MY_MOD_ID + 1);
            }
            ptr += len;
            rem -= len;
            if (rem && Params->delay) usleep(Params->delay);
        }
    }

    type = msg->data[2];
    opt  = msg->data[3];
    msgq_put(&Heap->commands, msg);

    switch (type) {
      case DAS_DEFAULT_TRIGCONFIG_CMD:
        if (ll>0) util_log(ll, "DAS default trigger command sent");
        break;
      case DAS_RECONFIGURE_DETECTOR_CMD:
        if (ll>0) util_log(ll, "DAS detector config command sent");
        break;
      case DAS_START_CALIBRATE_CMD:
        if (ll>0) util_log(ll, "DAS user-defined calib command sent");
        das_statreq(0);
        break;
      case DAS_CALIB1:
        if (ll>0) util_log(ll, "DAS preset calib #1 command sent");
        das_statreq(0);
        break;
      case DAS_CALIB2:
        if (ll>0) util_log(ll, "DAS preset calib #2 command sent");
        das_statreq(0);
        break;
      case DAS_CALIB3:
        if (ll>0) util_log(ll, "DAS preset calib #3 command sent");
        das_statreq(0);
        break;
      case DAS_CALIB4:
        if (ll>0) util_log(ll, "DAS preset calib #4 command sent");
        das_statreq(0);
        break;
      case DAS_CALIB5:
        if (ll>0) util_log(ll, "DAS preset calib #5 command sent");
        das_statreq(0);
        break;
      case DAS_DEFAULT_CONFIG_CMD:
        if (ll>0) util_log(ll, "DAS default config request sent");
        break;
      case DAS_END_CALIBRATE_CMD:
        if (ll>0) util_log(ll, "DAS calib abort command sent");
        das_statreq(0);
        break;
      case DAS_FLUSHQ_CMD:
        if (ll>0) util_log(ll, "DAS flush queue request sent");
        break;
      case DAS_SEND_IDDATA_CMD:
        if (ll>0) util_log(ll, "IDDATA request sent");
        break;
      case DAS_LOGLEVEL_CMD:
        if (ll>0) util_log(ll, "DAS log level %d command sent", opt);
        break;
      case DAS_RECONFIGURE_CMD:
        if (ll>0) util_log(ll, "DAS reconfigure command sent");
        break;
      case DAS_SEND_STATS_CMD:
        if (ll>0) util_log(ll, "DAS status request sent");
        break;
      case DAS_SENDLOG_CMD:
        if (ll>0) util_log(ll, "DAS flush log request sent");
        break;
      case DAS_OFFLINE_CMD:
        if (ll>0) util_log(ll, "DAS offline command sent");
        break;
      case DAS_SETYEAR_CMD:
        if (ll>0) util_log(ll, "DAS set year command sent");
        break;
      case DAS_RESET_CMD:
        util_log(1, "DAS reset");
        msgq_clear(&Q->das_write, &Heap->commands);
        clear_dasrebootflag();
        break;
      case DAS_ACK:
        if (ll>0) util_log(ll, "ACK sent");
        break;
      case DAS_NAK:
        if (ll>0) util_log(ll, "NAK sent");
        break;
      default:
        util_log(1, "UNKNOWN command (0x%0x) sent!", ptr[1]);
        break;
    }

    if (reboot_pending()) {
        REBOOT(das);
        util_log(1, "DAS reset");
        msgq_clear(&Q->das_write, &Heap->commands);
        clear_dasrebootflag();
    }
}

static int checksum(char *buf)
{
int i;
u_long ltemp, actual, expected, status;

#define CKSUM_OFF 30

    memcpy(&ltemp, buf + CKSUM_OFF, 4);
    expected = ntohl(ltemp);

    for (actual = 0, i = 0; i < CKSUM_OFF; i++) actual += (u_long) buf[i];
    for (i = CKSUM_OFF + 4; i < 1022; i++)      actual += (u_long) buf[i];

    if (actual != expected) {
        util_log(1, "checksum error: 0x%x != 0x%x", actual, expected);
        status = 1;
    } else {
        status = 0;
    }

    return status;
}

static u_long saveseqno(char *buf, u_long prev)
{
static int first = 1;
u_long seqno, ltemp, gap;

#define SEQNO_OFF 26

    memcpy(&ltemp, buf + SEQNO_OFF, 4);
    seqno = ntohl(ltemp);

    if (first) {
        first = 0;
    } else {
        if (seqno == prev + 1 || seqno == prev) return seqno;
        if (seqno <  prev) {
            gap = (long) (prev - seqno);
            util_log(1, "seqno wrap: 0x%x -> 0x%x = -%ld", prev,seqno,gap);
        } else if (seqno > prev) {
            gap = (long) (seqno - prev);
            util_log(1, "seqno jump: 0x%x -> 0x%x =  %ld", prev,seqno,gap);
        }
    }

    return seqno;
}

/* Thread to handle all reads from the DAS.  It does nothing beyond
 * reading full and unique packets and feeding them to das_process().
 */

static THREAD_FUNC DasReadThread(void *dummy)
{
int nodata, daserr;
time_t last_read, now, start;
int i, status, type, good_read, first, synced;
long got, dumped;
OLD_MSGQ_MSG *msg, *dasmsg;
struct termios new_termios;
static long remain = IDA_BUFSIZ - 2 - 2; /* no sync, no crc */ /*IDA10 OK */
u_long prev_seqno, seqno;
char *buf;
static char *fid = "DasReadThread";

    util_log(2, "DAS reader thread started, tid = %d", THREAD_SELF());

    daserr = 0;
    first = 1;

/* Initialize the uplink serial line */

    if ((das = ttyio_init(
        Params->port,
        (TTYIO_READ & TTYIO_WRITE),
        Params->ibaud,
        Params->obaud,
        TTYIO_PNONE,
        TTYIO_FNONE,
        (struct termios *) NULL,
        &new_termios,
        1
    )) < 0) {
        util_log(1, "%s: ttyio_init error %d: %s: %s",
            fid, -das, Params->port, syserrmsg(errno)
        );
        ispd_die(MY_MOD_ID + 2);
    }

/* Initialize the global DAS configuration table */

    for (i = 0; i < ISP_NSTREAM; i++) DasCnf->stream[i].active = 0;
    DasCnf->detect.nchan = 0;
    clear_have_dascnf();

/* Main loop */

    disable_output(1);
    set_config(ISP_CONFIG_UNKNOWN);
    last_read = -1;

    while (1) {

        DASWRITE(dasmsg);

        msg = msgq_get(&Heap->packets, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received", fid);
            ispd_die(MY_MOD_ID + 3);
        }
        memset(msg->data, 0, 2); /* clear out the sync bytes */
        buf  = msg->data;

    /* Read down the stream until we find a pair of back to back
     * bytes that are the complement of each other, indicating the
     * start of an IDA data packet.  If we timeout in the read then
     * maybe the DAS is lost, so we send it a reboot.  We do the
     * reads as several reads with small timeouts so that we don't
     * hog the uplink and prevent das_write from having a chance.
     */

        synced = 0;
        dumped = 0;
        nodata = 0;
        while (!synced) {

        /* Read first byte */

            got = 0;
            while (got != 1) {
                start = time(NULL);
                while (got == 0 && time(NULL) - start < Params->nodatato) {
                    got = util_read(das, buf + 0, 1, Params->ttyto);
                    if (got < 0) {
                        util_log(1, "util_read(1): %s",
                            syserrmsg(errno)
                        );
                        ispd_die(MY_MOD_ID + 4);
                    } else if (errno == ETIMEDOUT) {
                        DASWRITE(dasmsg);
                    }
                }
                if (got == 0) {
                    if (nodata == 0) {
                        util_log(1, "no data from DAS");
                        set_alarm(ISP_ALARM_DAS);
                    }
                    das_reboot();
                    nodata = 1;
                }
            }

            if (nodata) {
                util_log(1, "DAS OK");
                clear_alarm(ISP_ALARM_DAS);
                nodata = 0;
            }

            MUTEX_LOCK(&Status->lock);
                last_read = Status->last.read = time(NULL);
            MUTEX_UNLOCK(&Status->lock);

        /* Read second byte */

            got = 0;
            while (got != 1) {
                start = time(NULL);
                while (got == 0 && time(NULL) - start < Params->nodatato) {
                    got = util_read(das, buf + 1, 1, Params->ttyto);
                    if (got < 0) {
                        util_log(1, "util_read(2): %s",
                            syserrmsg(errno)
                        );
                        ispd_die(MY_MOD_ID + 5);
                    } else if (errno == ETIMEDOUT) {
                        DASWRITE(dasmsg);
                    }
                }
                if (got == 0) {
                    if (nodata == 0) {
                        util_log(1, "no data from DAS");
                        set_alarm(ISP_ALARM_DAS);
                    }
                    das_reboot();
                    nodata = 1;
                }
            }

            if (nodata) {
                util_log(1, "DAS OK");
                clear_alarm(ISP_ALARM_DAS);
                nodata = 0;
            }

            MUTEX_LOCK(&Status->lock);
                last_read = Status->last.read = time(NULL);
            MUTEX_UNLOCK(&Status->lock);

        /* See if we have a sync pattern */

            type = ida_rtype(buf, Params->ida->rev.value);
            if (type != IDA_CORRUPT && type != IDA_UNKNOWN) {
                synced = 1;
            } else {
                buf[0] = buf[1];
                if (++dumped == 1) set_alarm(ISP_ALARM_DAS);
                if (dumped % 8192 == 0) {
                    util_log(1, "Can't sync to DAS!");
                    das_reboot();
                }
            }
        }

    /* Found the sync pattern */

        if (first) {
            util_log(2, "initial sync established, %d bytes dropped",
                dumped
            );
            first = 0;
        } else if (dumped) {
            util_log(1,"lost sync, dropped %ld bytes before recovering",
                dumped
            );
            MUTEX_LOCK(&Status->lock);
                Status->count.sync++;
                Status->last.commerr = time(NULL);
            MUTEX_UNLOCK(&Status->lock);
        }

        if (dumped) {
            clear_alarm(ISP_ALARM_DAS);
            dumped = 0;
        }

    /* The rest of the packet should be waiting */

        got = util_read(das, buf + 2, remain, Params->ttyto);
        last_read = time(NULL);

    /* If we got it, and the checksum is OK then this is likely a
     * good packet, so we ack it
     */

        if (got == remain && checksum(buf) == 0) {

            ACK(das);
            good_read = 1;

            MUTEX_LOCK(&Status->lock);
                Status->last.pkt = Status->last.read = last_read;
            MUTEX_UNLOCK(&Status->lock);

            seqno = saveseqno(buf, prev_seqno);

        } else {

    /* Otherwise, we have a problem of some sort so we send a NAK
     * immediately, and then note the reason.
     */

            if (++daserr == 1) set_alarm(ISP_ALARM_DAS);

            NAK(das);
            good_read = 0;

            MUTEX_LOCK(&Status->lock);
                Status->last.commerr = last_read;
                Status->count.naks++;
            MUTEX_UNLOCK(&Status->lock);

        /* Note any checksum errors */

            if (got == remain) {
                MUTEX_LOCK(&Status->lock);
                    Status->last.read = last_read;
                    ++Status->count.badr;
                MUTEX_UNLOCK(&Status->lock);

        /* Note any short packets */

            } else if (got > 0) {
                MUTEX_LOCK(&Status->lock);
                    Status->last.read = last_read;
                    Status->count.shrt++;
                MUTEX_UNLOCK(&Status->lock);

                util_log(1, "short packet (%d bytes) dropped", got);

                if (
                    (got >= SEQNO_OFF + sizeof(u_long)) &&
                    (buf[0] == ~buf[1])
                ) {
                    seqno = saveseqno(buf, prev_seqno);
                    util_log(1, "seqno = 0x%x, type = 0x%02x",
                        seqno, buf[0]
                    );
                }

        /* If there was an I/O error we are hosed */

            } else if (errno == ETIMEDOUT) {
                DASWRITE(dasmsg);
            } else {
                util_log(1, "%s: read(3): %s", fid, syserrmsg(errno));
                ispd_die(MY_MOD_ID + 6);
            }
        }

    /* Give up now if we didn't get a full packet of data */

        if (!good_read) {
            msgq_put(&Heap->packets, msg);
            continue;
        }

        if (daserr) {
            clear_alarm(ISP_ALARM_DAS);
            daserr = 0;
        }

    /* Drop all duplicates */

        if (prev_seqno == seqno) {
            MUTEX_LOCK(&Status->lock);
                Status->count.dups++;
            MUTEX_UNLOCK(&Status->lock);
            util_log(1, "dup packet 0x%08x dropped", seqno);
            msgq_put(&Heap->packets, msg);
            continue;
        }

    /* Set the last byte with the public data flag */

        buf[IDA_BUFSIZ-1] = output_enabled(); /* IDA10 OK */

    /* We have a good, unique packet... pass it on */

        msg->len = IDA_BUFSIZ;
        msgq_put(&Q->das_process, msg);
        prev_seqno = seqno;
    }
}

void InitDasIO()
{
THREAD tid;
static char *fid = "InitDasIO";

    if (!THREAD_CREATE(&tid, DasReadThread, NULL)) {
        util_log(1, "%s: failed to create DasReadThread", fid);
        ispd_die(MY_MOD_ID + 7);
    }
}

/* Revision History
 *
 * $Log: dasio.c,v $
 * Revision 1.8  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.7  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.6  2005/07/06 15:45:58  dechavez
 * added message length to packets (turns out it was never set and downstream
 * code never used it until isi support was added!)
 *
 * Revision 1.5  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.4  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.3  2000/10/19 22:26:01  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
