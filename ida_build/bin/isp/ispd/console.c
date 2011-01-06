#pragma ident "$Id: console.c,v 1.20 2006/03/15 19:25:34 dechavez Exp $"
/*======================================================================
 *
 *  Server thread for dialogs with the ISP console program.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "ispd.h"
#include "sanio.h"

#define MY_MOD_ID ISPD_MOD_CONSOLE

/* Global variables */

extern struct isp_reftime *Reftime;
extern struct isp_dascnf  *DasCnf;
extern struct isp_status  *Status;
extern struct ispd_heap   *Heap;
extern struct ispd_queue  *Q;

extern ISP_PARAMS *Params;
extern ISP_SERVER *Server;

/* Static variables */

MUTEX initialized_mutex = MUTEX_INITIALIZER;
int initialized_flag = 0;

MUTEX operator_mutex = MUTEX_INITIALIZER;
int operator = -1;

MUTEX boot_mutex = MUTEX_INITIALIZER;
int abort_boot = 0;

MUTEX alert_mutex = MUTEX_INITIALIZER;
int alert_flag = 0;
int alert_decision = -1;

static SEMAPHORE alert_sp;

static struct client_info {
    MUTEX mutex;
    int sd;
    int to;
    int notify;
    int cnflag;
    char buffer[ISP_BUFLEN];
} client[ISP_MAXCLIENTS];

static int initialized()
{
int retval;

    MUTEX_LOCK(&initialized_mutex);
        retval = initialized_flag;
    MUTEX_UNLOCK(&initialized_mutex);

    return retval;
}

static int alert_set()
{
int status;

    MUTEX_LOCK(&alert_mutex);
        status = alert_flag;
    MUTEX_UNLOCK(&alert_mutex);

    return status;
}

static THREAD_FUNC MediaOptThread(void *optr)
{
int op;
static char *fid = "MediaOptThread";

    if (alert_set()) {
        util_log(1, "WARNING: %s: alert flag is set, thread exits", fid, op);
        THREAD_EXIT(0);
    }

    op = *((int *) optr);

    if (op == ISP_MEDIA_CHANGE) {
        if (OutputMediaType() == ISP_OUTPUT_TAPE) {
            util_log(1, "commencing tape change");
            set_eject_flag(1);
            flush_buffer(ISPD_FLUSH_MEDIA);
        }
        flush_idents();
        clear_commstats(1);
        clear_packetcounters(1);
        clear_auxstats(1);
        MUTEX_LOCK(&Status->lock);
            Status->last.change = time(NULL);
        MUTEX_UNLOCK(&Status->lock);
    } else if (op == ISP_MEDIA_FLUSH) {
        flush_buffer(ISPD_FLUSH_FORCE);
    } else {
        util_log(1, "%s: unknown media operation (%d) ignored", fid, op);
    }

    THREAD_EXIT(0);
}

BOOL SendBootMgrMsg(int op)
{
    util_log(1, "Contacting boot manager");
    if (isp_bootmgr(op) != 0) {
        util_log(1, "Unable to contact boot manager: %s", syserrmsg(errno));
        return FALSE;
    }
    return TRUE;
}

static THREAD_FUNC BootMgrThread(void *optr)
{
int op;
int status, abort;
char *operation;
static char *reboot   = "reboot";
static char *shutdown = "shutdown";
static char *fid = "BootMgrThread";

    if (!alert_set()) flush_buffer(ISPD_FLUSH_FORCE);

    op = *((int *) optr);
    if (op == 0) {
        operation = shutdown;
    } else if (op == 6) {
        operation = reboot;
    } else {
        util_log(1, "%s: unknown boot operation (%d) ignored", fid, op);
        THREAD_EXIT(0);
    }

/* I suppose it could be stuck in a loop with clever arguments, but 
 * this works too.
 */

    util_log(1, "System %s in 60 seconds", operation);
    sleep(30);
    MUTEX_LOCK(&boot_mutex);
        abort = abort_boot;
    MUTEX_UNLOCK(&boot_mutex);
    if (abort) {
        util_log(1, "System %s request aborted", operation);
        THREAD_EXIT(0);
    }
    util_log(1, "System %s in 30 seconds", operation);
    sleep(15);
    MUTEX_LOCK(&boot_mutex);
        abort = abort_boot;
    MUTEX_UNLOCK(&boot_mutex);
    if (abort) {
        util_log(1, "System %s request aborted", operation);
        THREAD_EXIT(0);
    }
    util_log(1, "System %s in 15 seconds", operation);
    sleep(5);
    MUTEX_LOCK(&boot_mutex);
        abort = abort_boot;
    MUTEX_UNLOCK(&boot_mutex);
    if (abort) {
        util_log(1, "System %s request aborted", operation);
        THREAD_EXIT(0);
    }
    util_log(1, "System %s in 10 seconds", operation);
    sleep(5);
    MUTEX_LOCK(&boot_mutex);
        abort = abort_boot;
    MUTEX_UNLOCK(&boot_mutex);
    if (abort) {
        util_log(1, "System %s request aborted", operation);
        THREAD_EXIT(0);
    }
    util_log(1, "System %s in 5 seconds", operation);
    sleep(5);
    MUTEX_LOCK(&boot_mutex);
        abort = abort_boot;
    MUTEX_UNLOCK(&boot_mutex);
    if (abort) {
        util_log(1, "System %s request aborted", operation);
        THREAD_EXIT(0);
    }

    if (!SendBootMgrMsg(op)) util_log(1, "%s request failed", operation);

    THREAD_EXIT(0);
}

void server_init()
{
int i, yes=1, ilen=sizeof(int);
struct sockaddr_in serv_addr;
static char *fid = "server_init";

    SEM_INIT(&alert_sp, 0, 1);

/* Allocate space for all possible clients */

    Server->maxclients = ISP_MAXCLIENTS;

    for (i = 0; i < Server->maxclients; i++) {
        MUTEX_INIT(&client[i].mutex);
        client[i].to     = Server->to;
        client[i].notify = 1;
        client[i].cnflag = 1;
        client[i].sd     = -1;
    }

    operator = -1;

/* Setup as a server */

    Server->sd = socket(AF_INET, SOCK_STREAM, 0);
    if (Server->sd < 0) {
        util_log(1, "%s: socket: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 2);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(Server->port);
    setsockopt(Server->sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, ilen);

    if (bind(
        Server->sd,
        (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)
    ) != 0) {
        util_log(1, "%s: bind: %s", fid, syserrmsg(errno));
        ispd_die(MY_MOD_ID + 3);
    }

    listen(Server->sd, 5);

    util_log(1, "ISP console service installed at port %d", Server->port);

    MUTEX_LOCK(&initialized_mutex);
        initialized_flag = 1;
    MUTEX_UNLOCK(&initialized_mutex);

    return;
}

int media_alert(int local)
{
int decision;
static char *fid = "media_alert";

/* If called from massio thread, block until resolved here */

    if (!local) {

    /* Set the alert flag */

        MUTEX_LOCK(&alert_mutex);
            alert_flag = 1;
        MUTEX_UNLOCK(&alert_mutex);

    /* Wait for somebody to decide what to do */

        MUTEX_LOCK(&Status->lock);
            Status->output.state = ISP_OUTPUT_OPER;
        MUTEX_UNLOCK(&Status->lock);
        set_alarm(ISP_ALARM_OPER);
        SEM_WAIT(&alert_sp);
        clear_alarm(ISP_ALARM_OPER);

        MUTEX_LOCK(&alert_mutex);
            alert_flag = 0;
        MUTEX_UNLOCK(&alert_mutex);

    /* And tell our caller what it was */

        MUTEX_LOCK(&alert_mutex);
            decision = alert_decision;
        MUTEX_UNLOCK(&alert_mutex);

        return decision;
    }

/* Must be here because of a media alert message from the operator. */

    MUTEX_LOCK(&alert_mutex);
        if (!alert_flag) {
            util_log(1, "Unexpected media alert response ignored");
        } else {
            switch (alert_decision) {
              case ISP_MEDIA_EJECT:
              case ISP_MEDIA_OVERWRITE:
              case ISP_MEDIA_APPEND:
                SEM_POST(&alert_sp);
                break;
              default:
                util_log(1, "UNKNOWN media alert response 0x%0x ignored",
                    alert_decision
                );
            }
        }
    MUTEX_UNLOCK(&alert_mutex);
}

static void disconnect(int id, char *fid, THREAD tid, int status, char *peer)
{

    if (status == 0) {
        util_log(1, "%s client disconnect", peer);
    } else {
        util_log(1, "%s client disconnect: %s", peer, syserrmsg(errno));
    }

    MUTEX_LOCK(&client[id].mutex);
        shutdown(client[id].sd, 2);
        close(client[id].sd);
        client[id].sd = -1;
        client[id].notify = 0;
        client[id].cnflag = 0;
        MUTEX_LOCK(&operator_mutex);
            if (operator == id) operator = -1;
        MUTEX_UNLOCK(&operator_mutex);
    MUTEX_UNLOCK(&client[id].mutex);

    THREAD_EXIT(0);
}

static int set_id(int sd)
{
int i, id;
static MUTEX mutex = MUTEX_INITIALIZER;

    MUTEX_LOCK(&mutex);
        for (i = 0, id = -1; id < 0 && i < Server->maxclients; i++) {
            if (client[i].sd < 0) {
                id = i;
                client[id].sd = sd;
                client[id].notify = 1;
                client[id].cnflag = 1;
                MUTEX_LOCK(&operator_mutex);
                    if (operator < 0) operator = id;
                MUTEX_UNLOCK(&operator_mutex);
            }
        }
    MUTEX_UNLOCK(&mutex);

    return id;
}

void set_cnflag()
{
int i;
static MUTEX mutex = MUTEX_INITIALIZER;

    if (!initialized()) return;

    MUTEX_LOCK(&mutex);
        for (i = 0;  i < Server->maxclients; i++) {
            MUTEX_LOCK(&client[i].mutex);
                if (client[i].sd > 0) {
                    client[i].cnflag = 1;
                }
            MUTEX_UNLOCK(&client[i].mutex);
        }
    MUTEX_UNLOCK(&mutex);
}

static int operator_id()
{
int id;

    MUTEX_LOCK(&operator_mutex);
        id = operator;
    MUTEX_UNLOCK(&operator_mutex);

    return id;
}

static void set_operator(int newid)
{

    MUTEX_LOCK(&operator_mutex);
        if (operator >= 0) {
            MUTEX_LOCK(&client[operator].mutex);
                client[operator].notify = 1;
            MUTEX_UNLOCK(&client[operator].mutex);
        }
        operator = newid;
        MUTEX_LOCK(&client[operator].mutex);
            client[operator].notify = 1;
        MUTEX_UNLOCK(&client[operator].mutex);
    MUTEX_UNLOCK(&operator_mutex);
}

static int send_state(int sd, char *buffer, int state, int to)
{
UINT8 *ptr;
static char *fid = "send_state";

    ptr = (UINT8 *) buffer + 4;
    utilPackINT16(ptr, (UINT16) state);
    return isp_send(sd, ISP_STATE, buffer, 2, to);
}

static int SendStatus(int sd, char *buffer, int to, int myid)
{
struct isp_status soh;

    MUTEX_LOCK(&Status->lock);
        soh = *Status;
    MUTEX_UNLOCK(&Status->lock);
    soh.tstamp = time(NULL);
    soh.flag.operator = (myid == operator_id());
    soh.flag.event              = event_in_progress();
    soh.flag.calib              = calib_status();
    soh.flag.config             = config_status();
    soh.heap.barometer.nfree    = msgq_count(&Heap->barometer);
    soh.heap.barometer.lowat    = msgq_lowat(&Heap->barometer);
    soh.heap.commands.nfree     = msgq_count(&Heap->commands);
    soh.heap.commands.lowat     = msgq_lowat(&Heap->commands);
    soh.heap.packets.nfree      = msgq_count(&Heap->packets);
    soh.heap.packets.lowat      = msgq_lowat(&Heap->packets);
    soh.heap.obuf.nfree         = msgq_count(&Heap->obuf);
    soh.heap.obuf.lowat         = msgq_lowat(&Heap->obuf);
    soh.queue.baro.npend        = msgq_count(&Q->baro);
    soh.queue.baro.hiwat        = msgq_hiwat(&Q->baro);
    soh.queue.das_read.npend    = msgq_count(&Q->das_read);
    soh.queue.das_read.hiwat    = msgq_hiwat(&Q->das_read);
    soh.queue.das_write.npend   = msgq_count(&Q->das_write);
    soh.queue.das_write.hiwat   = msgq_hiwat(&Q->das_write);
    soh.queue.das_process.npend = msgq_count(&Q->das_process);
    soh.queue.das_process.hiwat = msgq_hiwat(&Q->das_process);
    soh.queue.massio.npend      = msgq_count(&Q->massio);
    soh.queue.massio.hiwat      = msgq_hiwat(&Q->massio);
    soh.queue.nrts.npend        = msgq_count(&Q->nrts);
    soh.queue.nrts.hiwat        = msgq_hiwat(&Q->nrts);
    soh.queue.obuf.npend        = msgq_count(&Q->obuf);
    soh.queue.obuf.hiwat        = msgq_hiwat(&Q->obuf);
    return isp_send_status(sd, buffer, &soh, to);
}

static THREAD_FUNC ServerThread(void *argptr)
{
int myid, sd, command, state, alert, op;
int *to, *notify, *cnflag;
THREAD tid;
char *buffer, peer[MAXPATHLEN+1];
void *data;
short stmp;
struct isp_dascal cal;
struct isp_dascnf cnf;
static char *fid = "ServerThread";

    myid   = *((int *) argptr);
    util_log(2, "%s: starting, with myid = %d\n", fid, myid);
    if (myid < 0 || myid > Server->maxclients) {
        util_log(1, "BUG somewhere, illegal myid!");
        ispd_die(MY_MOD_ID + 4);
    }
    sd     = client[myid].sd;
    to     = &client[myid].to;
    notify = &client[myid].notify;
    cnflag = &client[myid].cnflag;
    buffer = client[myid].buffer;
    if (util_peer(sd, buffer, ISP_BUFLEN) != NULL) {
        strcpy(peer, util_peer(sd, buffer, ISP_BUFLEN));
    } else {
        util_log(1, "can't grok peer, dumping new connection");
        sprintf(peer, "undecipherable peer");
        disconnect(myid, fid, THREAD_SELF(), 1, peer);
    }

    if (myid == operator_id()) {
        util_log(1, "OPERATOR connect from %s", peer);
    } else {
        util_log(1, "observer connect from %s", peer);
    }

/* Pass state, run, configuration and channel map to client right away */

    state = (myid == operator_id()) ? ISP_OPERATOR : ISP_OBSERVER;
    if (send_state(sd, buffer, state, *to) != ISP_OK) {
        disconnect(myid, fid, THREAD_SELF(), 1, peer);
    }

    if (isp_send_params(sd, buffer, Params, *to) != ISP_OK) {
        disconnect(myid, fid, THREAD_SELF(), 3, peer);
    }

    if (Params->digitizer == ISP_DAS) {
        if (ispSendChnLocMap(sd, buffer, Params->ida, *to) != ISP_OK) {
            util_log(1, "%s: ispSendChnLocMap: %s", fid, strerror(errno));
            ispd_die(MY_MOD_ID + 5);
        }
    }

    *cnflag = 1;

    while (1) {

        state = (myid == operator_id()) ? ISP_OPERATOR : ISP_OBSERVER;

        if (*notify) {
            if (send_state(sd, buffer, state, *to) != ISP_OK) {
                disconnect(myid, fid, THREAD_SELF(), 1, peer);
            }
            *notify = 0;
        }

        if (*cnflag) {
            MUTEX_LOCK(&DasCnf->mutex);
                cnf = *DasCnf;
            MUTEX_UNLOCK(&DasCnf->mutex);
            cnf.flag = ISP_DASCNF_SET;
            if (isp_send_dascnf(sd, buffer, &cnf, *to) != ISP_OK) {
                disconnect(myid, fid, THREAD_SELF(), 3, peer);
            }
            *cnflag = 0;
        }

        if ((command = isp_recv(sd, buffer, -1, &data)) == ISP_ERROR) {
            disconnect(myid, fid, THREAD_SELF(), 4, peer);
        }
        state = (myid == operator_id()) ? ISP_OPERATOR : ISP_OBSERVER;

    /* Process commands common to everybody */

        switch (command) {

          case ISP_ABORT_BOOTOP:
            MUTEX_LOCK(&boot_mutex);
                abort_boot = 1;    
            MUTEX_UNLOCK(&boot_mutex);
            continue;

          case ISP_NOP:
            continue;

          case ISP_STATUS:
            if (SendStatus(sd, buffer, *to, myid) != ISP_OK) {
                disconnect(myid, fid, THREAD_SELF(), 2, peer);
            }
            continue;

          case ISP_PARAM:
            if (isp_send_params(sd, buffer, Params, *to) != ISP_OK) {
                disconnect(myid, fid, THREAD_SELF(), 3, peer);
            }
            continue;

          case ISP_CHNMAP:
            if (Params->digitizer == ISP_DAS) {
                if (ispSendChnLocMap(sd, buffer, Params->ida, *to) != ISP_OK) {
                    disconnect(myid, fid, THREAD_SELF(), 3, peer);
                }
            }
            continue;

          case ISP_DISCONNECT:
            disconnect(myid, fid, THREAD_SELF(), 0, peer);
            continue;

          case ISP_UPDATE_CDR_STATS:
            MUTEX_LOCK(&Status->lock);
                Status->output.file = -1;
                Status->buffer.nrec = -1;
                Status->output.nrec = -1;
            MUTEX_UNLOCK(&Status->lock);
            if (SendStatus(sd, buffer, *to, myid) != ISP_OK) {
                disconnect(myid, fid, THREAD_SELF(), 2, peer);
            } else {
                sleep(2);
                UpdateCdStats(TRUE);
            }
            continue;

          case ISP_STATE:
            memcpy(&stmp, data, 2);
            if (ntohs(stmp) == ISP_OPERATOR) set_operator(myid);
            continue;

          case ISP_TIMEOUT:
            memcpy(&stmp, data, 2);
            *to = ntohs(stmp);
            util_log(2, "%s(%d): socket I/O timeout set to %d sec",
                fid, THREAD_SELF(), *to
            );
            continue;

          case ISP_DASCNF:
            if (Params->digitizer == ISP_DAS) {
                if (isp_decode_dascnf(&cnf, (char *) data) == 0) {
                    if (state == ISP_OPERATOR && cnf.flag != 0) {
                        if (isp_setcnf(&cnf) == 0) ConfigureDas(0);
                    } else if (cnf.flag == ISP_DASCNF_REQ) {
                        *cnflag = 1;
                    }
                }
            }
            continue;
        }

    /* Operator only commands after this point */

        if (state != ISP_OPERATOR) continue;

        switch (command) {

          case ISP_CLR_EVERYTHING:
            clear_commstats(1);
            clear_packetcounters(1);
            clear_auxstats(1);
            break;

          case ISP_CLR_COMMSTAT:
            clear_commstats(1);
            break;

          case ISP_CLR_PACKSTAT:
            clear_packetcounters(1);
            break;

          case ISP_CLR_AUXSTAT:
            clear_auxstats(1);
            break;

          case ISP_LOG_INCR:
            util_incrloglevel();
            break;

          case ISP_LOG_RESET:
            util_rsetloglevel();
            break;

          case ISP_DAS_REBOOT:
            if (Params->digitizer == ISP_DAS) {
                das_reboot();
            } else {
                SanCmnd(SAN_REBOOT);
            }
            break;

          case ISP_DAS_FLUSHLOG:
            if (Params->digitizer == ISP_DAS) {
                das_flushlog(1);
            }
            break;

          case ISP_DAS_IDDATA:
            if (Params->digitizer == ISP_DAS) {
                das_iddata(1);
            }
            break;

          case ISP_DAS_LOG_INFO:
            if (Params->digitizer == ISP_DAS) {
                das_loginfo(1);
            }
            break;

          case ISP_DAS_LOG_DEBUG:
            if (Params->digitizer == ISP_DAS) {
                das_logdebug(1);
            }
            break;

          case ISP_DAS_ABORT_CAL:
            if (Params->digitizer == ISP_DAS) {
                das_abortcal(1);
            }
            break;

          case ISP_DASCAL:
            if (Params->digitizer == ISP_DAS) {
                isp_decode_dascal(&cal, data);
                das_calibrate(1, &cal);
            }
            break;

          case ISP_DAS_SOH:
            if (Params->digitizer == ISP_DAS) {
                das_statreq(1);
            } else {
                SanCmnd(SAN_SNDSOH);
            }
            break;

          case ISP_DAS_ACK:
            if (Params->digitizer == ISP_DAS) {
                das_ack(1);
            }
            break;

          case ISP_DAS_NAK:
            if (Params->digitizer == ISP_DAS) {
                das_nak(1);
            }
            break;

          case ISP_NRTS_START:
            break;

          case ISP_NRTS_STOP:
            break;

          case ISP_MEDIA_FLUSH:
            op = ISP_MEDIA_FLUSH;
            if (OutputMediaType() != ISP_OUTPUT_NONE) {
                if (!THREAD_CREATE(&tid, MediaOptThread, &op)) {
                    util_log(1, "%s: failed to create MediaOptThread", fid);
                    util_log(1, "tape flush request canceled");
                }
            }
            break;

          case ISP_MEDIA_EJECT:
            if (OutputMediaType() != ISP_OUTPUT_NONE) {
                if (alert_set()) {
                    MUTEX_LOCK(&alert_mutex);
                        alert_decision = ISP_MEDIA_EJECT;
                    MUTEX_UNLOCK(&alert_mutex);
                    media_alert(1);
                } else {
                    eject_media();
                }
            }
            break;

          case ISP_MEDIA_OVERWRITE:
            if (OutputMediaType() != ISP_OUTPUT_NONE) {
                if (alert_set()) {
                    MUTEX_LOCK(&alert_mutex);
                        alert_decision = ISP_MEDIA_OVERWRITE;
                    MUTEX_UNLOCK(&alert_mutex);
                    media_alert(1);
                }
            }
            break;

          case ISP_MEDIA_APPEND:
            if (OutputMediaType() != ISP_OUTPUT_NONE) {
                if (alert_set()) {
                    MUTEX_LOCK(&alert_mutex);
                        alert_decision = ISP_MEDIA_APPEND;
                    MUTEX_UNLOCK(&alert_mutex);
                    media_alert(1);
                }
            }
            break;

          case ISP_MEDIA_CHANGE:
            if (OutputMediaType() != ISP_OUTPUT_NONE) {
                op = ISP_MEDIA_CHANGE;
                if (!THREAD_CREATE(&tid, MediaOptThread, &op)) {
                    util_log(1, "%s: failed to create MediaOptThread", fid);
                    util_log(1, "tape change request canceled");
                }
            }
            break;

          case ISP_DEBUG_BARO:
            baroDebugToggle();
            break;

          case ISP_DEBUG_DPM:
            dpmDebugToggle();
            break;

          case ISP_DEBUG_CLOCK:
            clockDebugToggle();
            break;

          case ISP_HOST_REBOOT:
            MUTEX_LOCK(&boot_mutex);
                abort_boot = 0;
            MUTEX_UNLOCK(&boot_mutex);
            util_log(1, "ISP reboot requested by operator");
            op = 6;
            if (!THREAD_CREATE(&tid, BootMgrThread, &op)) {
                util_log(1, "%s: failed to create BootMgrThread", fid);
                util_log(1, "ISP reboot request canceled");
            }
            break;

          case ISP_HOST_SHUTDOWN:
            MUTEX_LOCK(&boot_mutex);
                abort_boot = 0;
            MUTEX_UNLOCK(&boot_mutex);
            util_log(1, "ISP shutdown requested by operator");
            op = 0;
            if (!THREAD_CREATE(&tid, BootMgrThread, &op)) {
                util_log(1, "%s: failed to create BootMgrThread", fid);
                util_log(1, "ISP shutdown request canceled");
            }
            break;

          default:
            util_log(1, "%s(%d): unknown command 0x%02x ignored", 
                fid, THREAD_SELF(), command
            );
        }
    }
}

void serve(int sd)
{
THREAD tid;
static int myid;
static char *fid = "serve";

/* See if we have room for this connection */

    if ((myid = set_id(sd)) < 0) {
        util_log(1, "client limit exceeded: connection refused");
        shutdown(sd, 2);
        close(sd);
        return;
    }

/* Make sure client will see current CD stats */

    UpdateCdStats(FALSE);

/* Start a thread to handle all dialog with the client */

    if (!THREAD_CREATE(&tid, ServerThread, &myid)) {
        util_log(1, "%s: failed to create ServerThread", fid);
        ispd_die(MY_MOD_ID + 7);
    }

    return;
}

/* Revision History
 *
 * $Log: console.c,v $
 * Revision 1.20  2006/03/15 19:25:34  dechavez
 * fixed test of ispSendChnLocMap() return value
 *
 * Revision 1.19  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.18  2005/11/04 04:02:58  dechavez
 * check for util_peer success before strcpy'ing it into ServerThread's peer buffer
 *
 * Revision 1.17  2005/10/11 18:28:31  dechavez
 * UpdateCdStats anytime a client connects.  This will ensure that the clients
 * see current stats without having to explicity request an update
 *
 * Revision 1.16  2005/08/26 20:17:30  dechavez
 * removed NRTS start/stop options
 *
 * Revision 1.15  2004/12/21 00:33:58  dechavez
 * clear tabs
 *
 * Revision 1.14  2003/12/10 06:25:47  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.13  2003/05/23 19:52:19  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.12  2002/09/09 22:01:46  dec
 * added ISP_DEBUG_DPM support, added peer name to disconnect messages,
 * added SendBootMgrMsg()
 *
 * Revision 1.11  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.10  2002/02/22 23:56:35  dec
 * added ISP_UPDATE_CDR_STATS response
 *
 * Revision 1.9  2001/10/26 23:41:58  dec
 * suppress tape related ops when handling an ISP_MEDIA_CHANGE command for
 * systems with output device other than tape
 *
 * Revision 1.8  2001/10/24 23:19:52  dec
 * corrected initialization error
 *
 * Revision 1.7  2001/10/08 18:15:11  dec
 * sane response to media I/O ops when no media output configured
 *
 * Revision 1.6  2001/05/20 17:43:45  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.5  2001/04/19 22:45:35  dec
 * support for ISP_DEBUG_BARO and ISP_DEBUG_CLOCK commands
 *
 * Revision 1.4  2001/01/12 23:32:52  dec
 * set SO_REUSEADDR socket option
 *
 * Revision 1.3  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:06  dec
 * import existing IDA/NRTS sources
 *
 */
