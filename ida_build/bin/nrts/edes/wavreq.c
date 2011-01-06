#pragma ident "$Id: wavreq.c,v 1.10 2006/08/15 01:27:44 dechavez Exp $"
/*======================================================================
 *
 *  Service a request for waveform records.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "nrts.h"
#include "xfer.h"
#include "util.h"
#include "edes.h"

#define SYS  (edes->sys + i)
#define SSTA  (SYS->sta + j)
#define SCHN (SSTA->chn + k)

#define INF  (edes->inf + i)
#define ISTA  (INF->sta + j)
#define ICHN (ISTA->chn + k)

#define CNF  (edes->cnf + i)
#define CSTA  (CNF->sta + j)
#define CCHN (CSTA->chn + k)

#define FORM (request->wav_form)
#define COMP (request->comp)

extern char *Peer;
static int wait_flag = 0;

static unsigned int HeartbeatInterval;
static struct xfer_cnf *cnf;
static struct edes_req *request;
static struct edes_params *edes;
static char *buffer;
static char *peer;
static char WhoWhatBuf[4096];
static char *WhoWhat = NULL;

void logWhoWhat()
{
int i;
static char msgbuf[1024];

    sprintf(msgbuf, "%s:", Peer);
    if (WhoWhat == NULL) {
        for (i = 0; i < request->nsta; i++) {
            sprintf(msgbuf+strlen(msgbuf), " %s", request->sta[i].name);
        }
        util_log(1, msgbuf);
    } else {
        util_log(1, "%s: %s", Peer, WhoWhat);
    }
}

char *SetWhoWhat(struct xfer_req *req)
{
    xferWavreqString(req, WhoWhatBuf);
    WhoWhat = WhoWhatBuf;

    return WhoWhat;
}

void set_wait_flag(int flag)
{
    wait_flag = flag;
    if (!wait_flag) util_log(1, "nice behavior is %s", wait_flag ? "ON" : "OFF");
}

void catch_alarm(int sig)
{
    util_log(3, "woke up");
    return;
}

int send_data(void)
{
static time_t delay, now, last_hbeat = 0;
int i, j, k, nsent, done, have_data;
static char *fid = "send_data";

/* Loop over all channels looking for data to send */

    nsent = 0;
    do {

        /* start off assuming we don't have, and won't have, anything */

        done = 1;
        have_data = 0;

        /* now loop through all channels to see if this is true */

        for (i = 0; i < edes->nsys; i++) {
            for (j = 0; j < SYS->nsta; j++) {
                for (k = 0; k < SSTA->nchn; k++) {

                    /* if waiting for data, see if any are ready */

                    if (ICHN->status == EDES_WAITING) {
                        set_indices(edes,i,j,k,buffer);
                    }

                    /* send anything we have to send */

                    if (ICHN->status == EDES_READY) {
                        transfer(cnf,FORM,COMP,edes,i,j,k,buffer,peer);
                        ++nsent;
                    }

                    /* if we have more to do, set appropriate flag(s) */

                    if (ICHN->status == EDES_READY) {
                        have_data = 1;
                        done = 0;
                    } else if (ICHN->status == EDES_WAITING) {
                        done = 0;
                    }
                }
            }
        }
    } while (have_data);

/* Quit if we have sent all requested data */

    if (done) {
        util_log(2, "%s: waveform request processing complete", fid);
        return EDES_DONE;
    }

/* If we didn't send anything, send a heartbeat */

    if (nsent == 0) {
        now = time(NULL);
        delay = now - last_hbeat;
        if (delay > HeartbeatInterval) {
            util_log(2, "send HEARTBEAT (delay = %ld)", now - last_hbeat);
            Xfer_Ack(edes->osd, XFER_HEARTBEAT);
            last_hbeat = now;
        }
    }

    return EDES_WAITING;
}

void wavreq(struct xfer_cnf *cnf_ptr, struct edes_req *request_ptr, struct edes_params *edes_ptr, char *buffer_ptr, char *peer_ptr)
{
#define POLL_INTERVAL 1 /* sec */
int i, j, k, paklen, status;
int PollInterval = POLL_INTERVAL;
static char *fid = "wavreq";

    util_log(2, "%s: begin waveform request processing", fid);

    cnf     = cnf_ptr;
    request = request_ptr;
    edes    = edes_ptr;
    buffer  = buffer_ptr;
    peer    = peer_ptr;

/* heartbeat interval will be 1/2 the timeout */

    HeartbeatInterval = (time_t) request->timeout / 2;

/* and start the processing */

    if (wait_flag) {
        do {
            /* signal(SIGALRM, xfer_CatchSignal); */
            status = send_data();
            if (status != EDES_DONE) {
                signal(SIGALRM, catch_alarm);
                util_log(3, "sleep(%d)", PollInterval);
                sleep(PollInterval);
            }
        } while (status != EDES_DONE);
        /* signal(SIGALRM, xfer_CatchSignal); */
    } else {
        while (send_data() != EDES_DONE);
    }

/* Wait for client to notice we are done and disconnect */

    while (Xfer_Ack(edes->osd, XFER_FINISHED)) sleep(HeartbeatInterval);

}

/* Revision History
 *
 * $Log: wavreq.c,v $
 * Revision 1.10  2006/08/15 01:27:44  dechavez
 * replaced "pause for one packet" with "pause for 1 second" to improve latency
 *
 * Revision 1.9  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.8  2004/04/26 21:11:14  dechavez
 * misc debugging statements added, plus MySQL support (via dbspec instead of dbdir)
 *
 * Revision 1.7  2003/05/05 23:22:03  dechavez
 * Block in Xfer_Ack loop when request completed
 *
 * Revision 1.6  2002/04/26 00:24:51  nobody
 * include peer name in new log who what
 *
 * Revision 1.5  2002/04/25 21:59:26  dec
 * use SetWhoWhat so that LogWhoWhat can print full request dumps
 *
 * Revision 1.4  2002/04/02 01:24:52  nobody
 * don't bother with logWhoWhat since complete sta:chan is now logged
 *
 * Revision 1.3  2002/02/23 00:02:24  dec
 * no longer install (obsolete) xfer_CatchSignal handler
 *
 * Revision 1.2  2000/03/16 06:16:23  dec
 * Removed reliance on frozen configuration file in favor of new
 * lookup table (sint).  Added SIGHUP handler to print peer coordinates
 * and list of requested stations.
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
