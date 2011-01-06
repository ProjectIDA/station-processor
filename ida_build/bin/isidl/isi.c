#pragma ident "$Id: isi.c,v 1.25 2008/03/05 23:25:27 dechavez Exp $"
/*======================================================================
 *
 *  Remote ISI raw disk loop input
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isidl.h"
#define MY_MOD_ID ISIDL_MOD_ISI

#ifdef unix
#define DEFAULT_DEBUG_PATH__PREFIX "/var/tmp/"
#else
#define DEFAULT_DEBUG_PATH__PREFIX "\\"
#endif

#define BUILD_SCL_FROM_NRTS(par) (strcasecmp(par->scl, "nrts") == 0)

static char *FileName = __FILE__;

static ISI_PARAM isiparam;
static ISI_SEQNO begseqno, endseqno, *beg, *end;
static char *server;
static int port;
static ISI *isi = NULL;

static struct {
    char *path;
    FILE *fp;
    BOOL on;
    BOOL enabled;
} debug = {NULL, NULL, FALSE, FALSE};

static UINT32 FindLargestPacketLength(ISIDL_PAR *par)
{
int i;
UINT32 result;
static char *fid = "FindLargestPacketLength";

    BlockShutdown(fid);
        result = par->dl[0]->sys->maxlen;
        for (i = 1; i < par->nsite; i++) if (par->dl[i]->sys->maxlen > result) result = par->dl[i]->sys->maxlen;
    UnblockShutdown(fid);
    return result;
}

static int LocateDiskLoopBySite(ISIDL_PAR *par, char *target)
{
int i, result;
static char *fid = "LocateDiskLoopBySite";

    BlockShutdown(fid);
        for (result = -1, i = 0; result == -1 && i < par->nsite; i++) {
            if (strcmp(par->dl[i]->sys->site, target) == 0) result = i;
        }
    UnblockShutdown(fid);
    return result;
}

void ToggleISIDebugState()
{
static char *fid = "ToggleISIDebugState";

    if (!debug.enabled) return;

    if (!debug.on) {
        if ((debug.fp = fopen(debug.path, "a+")) == NULL) {
            LogMsg(LOG_INFO, "%s: fopen: %s: %s", fid, debug.path, strerror(errno));
            return;
        }
        if (isi != NULL && isi->iacp != NULL) isi->iacp->dbgfp = debug.fp;
        LogMsg(LOG_INFO, "IACP DUMPS ON: %s", debug.path);
        debug.on = TRUE;
    } else {
        fclose(debug.fp);
        debug.fp = NULL;
        if (isi != NULL && isi->iacp != NULL) isi->iacp->dbgfp = NULL;
        LogMsg(LOG_INFO, "IACP DUMPS OFF: close %s", debug.path);
        debug.on = FALSE;;
    }
}

static void InitISI(ISIDL_PAR *par)
{
int ntoken;
#define MAXTOKEN 3
char *token[MAXTOKEN];
char path[MAXPATHLEN+1];
static char *fid = "InitISI";

    isiInitDefaultPar(&isiparam);
    if (par->sndbuf != 0) isiparam.attr.at_sndbuf = par->sndbuf;
    if (par->rcvbuf != 0) isiparam.attr.at_rcvbuf = par->rcvbuf;

    if (par->dbgpath != NULL && (debug.path = strdup(par->dbgpath)) == NULL) {
        perror("strdup");
        exit(MY_MOD_ID + 1);
    } else {
        sprintf(path, "%sisidl.%d", DEFAULT_DEBUG_PATH__PREFIX, getpid());
        if ((debug.path = strdup(path)) == NULL) {
            perror("strdup");
            exit(MY_MOD_ID + 1);
        }
    }

    if (par->begstr != NULL) {
        if (!isiStringToSeqno(par->begstr, &begseqno)) {
            fprintf(stderr, "illegal beg string '%s'\n", par->begstr);
            exit(MY_MOD_ID + 2);
        }
        beg = &begseqno;
    } else {
        beg = NULL;
    }

    if (par->endstr != NULL) {
        if (!isiStringToSeqno(par->endstr, &endseqno)) {
            fprintf(stderr, "illegal end string '%s'\n", par->endstr);
            exit(MY_MOD_ID + 3);
        }
    } else {
        endseqno = ISI_DEFAULT_END_SEQNO;
        end = &endseqno;
    }

    ntoken = utilParse(par->input, token, ":", MAXTOKEN, 0);
    if (ntoken < 1) {
        fprintf(stderr, "error parsing ISI string '%s'\n", par->input);
        exit(MY_MOD_ID + 4);
    }

    if (ntoken >= 1) { // host
        if ((server = strdup(token[0])) == NULL) {
            perror("strdup");
            exit(MY_MOD_ID + 5);
        }
    }

    if (ntoken >= 2) { // host:port
        if ((port = atoi(token[1])) <= 0) {
            fprintf(stderr, "illegal port string '%s'\n", token[1]);
            exit(MY_MOD_ID + 6);
        }
        isiSetServerPort(&isiparam, port);
    }

    isiSetTimeout(&isiparam, par->timeout);
    isiSetLog(&isiparam, par->lp);

    debug.enabled = TRUE;
}

/* Generate a data request structure */

static ISI_SEQNO GetBegSeqnoFromDiskLoop(ISI_DL *dl)
{
ISI_SEQNO want;
static char *fid = "GetBegSeqnoFromDiskLoop";

    BlockShutdown(fid);

    /* Empty disk loops will, by default, try to backfill */

        if (dl->sys->index.yngest == ISI_UNDEFINED_INDEX) {
            want = ISI_OLDEST_SEQNO;

    /* Otherwise, ask for the next in sequence */

        } else {
            want = isidlPacketSeqno(dl, dl->sys->index.yngest);
            if (isiIsUndefinedSeqno(&want)) {
                want = ISI_NEWEST_SEQNO;
            } else {
                ++want.counter;
            }
        }

    UnblockShutdown(fid);

       return want;    
}

static ISI_DATA_REQUEST *CreateDataRequest(ISIDL_PAR *par)
{
int i, SiteIndex;
ISI_DATA_REQUEST *dreq;
static char *fid = "CreateDataRequest";

    if ((dreq = isiAllocSimpleSeqnoRequest(beg, end, par->site)) == NULL) {
        LogMsg(LOG_INFO, "%s: isiAllocSimpleSeqnoRequest: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
    if (par->scl.list != NULL && !listCopy(&dreq->slist, par->scl.list)) {
        LogMsg(LOG_INFO, "%s: listCopy: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 8);
    }

    for (i = 0; i < dreq->nreq; i++) {
        if ((SiteIndex = LocateDiskLoopBySite(par, dreq->req.seqno[i].site)) < 0) {
            LogMsg(LOG_INFO, "%s: LOGIC ERROR! no %s disk loop!: %s", fid, dreq->req.seqno[i].site);
            Exit(MY_MOD_ID + 8);
        }
        if (beg == NULL || !par->first[SiteIndex]) dreq->req.seqno[i].beg = GetBegSeqnoFromDiskLoop(par->dl[SiteIndex]);
    }

    beg = NULL;
    return dreq;
}

/* Establish fresh data connection */

static ISI *ConnectToServer(ISIDL_PAR *par)
{
ISI *isi;
ISI_DATA_REQUEST *dreq;
static char *fid = "ConnectToServer";

    dreq = CreateDataRequest(par);
    LogMsg(LOG_INFO, "contacting %s:%d", server, isiparam.port);
    isi = isiInitiateDataRequest(server, &isiparam, dreq);
    isiFreeDataRequest(dreq);

    if (isi == NULL) {
        LogMsg(LOG_INFO, "unable to connect to %s:%d", server, isiparam.port);
        return NULL;
    } else {
        isi->iacp->dbgfp = debug.fp;
        return isi;
    }
}

static BOOL VerifySequence(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
BOOL result;
char buf1[64], buf2[64];
static char *fid = "VerifyAndUpdateSeqno";

    BlockShutdown(fid);

        if (dl->sys->count == 0) {
            result = TRUE;
        } else {
            switch (isiCompareSeqno(&raw->hdr.seqno, &dl->sys->seqno)) {
              case 0:
                LogMsg(LOG_INFO, "duplicate %s packet dropped (seqno = %s)", raw->hdr.site, isiSeqnoString(&raw->hdr.seqno, buf1));
                result = FALSE;
              case -1: 
                isiSeqnoString(&raw->hdr.seqno, buf1);
                isiSeqnoString(&dl->sys->seqno, buf2);
                LogMsg(LOG_INFO, "overlapping %s packet dropped (new = %s, dl = %s)", raw->hdr.site, buf1, buf2);
                result = FALSE;
              default:
                result = TRUE;
            }
        }

    UnblockShutdown(fid);

    return result;
}

static void CloseConnection(ISI *isi, char *ident, ISIDL_PAR *par)
{
int i;

    LogMsg(LOG_INFO, "closing connection to %s: %s", ident, strerror(errno));
    for (i = 0; i < par->nsite; i++) par->first[i] = TRUE;
    isiClose(isi);
}

static int ReadRawPacket(ISI *isi, ISI_RAW_PACKET *raw, UINT8 *buf, UINT32 buflen)
{
int status;
static char *fid = "ReadRawPacket";

    while (1) {
        status = isiReadFrame(isi, TRUE); // TRUE means skip heartbeats
        if (status != ISI_OK) return status;
        if (isi->frame.payload.type != ISI_IACP_RAW_PKT) {
            LogMsg(LOG_INFO, "unexpected type %d packet ignored", isi->frame.payload.type);
        } else {
            isiUnpackRawPacket(isi->frame.payload.data, raw);
            if (raw->hdr.len.native > buflen) {
                LogMsg(LOG_INFO, "Unexpected large (%lu > %lu) packet dropped", raw->hdr.len.native, buflen);
                LogMsg(LOG_INFO, "DROP: %s", isiRawHeaderString(&raw->hdr, buf));
            } else {
                if (isiDecompressRawPacket(raw, buf, buflen)) return ISI_OK;
                LogMsg(LOG_INFO, "DROP: %s: %s", isiRawHeaderString(&raw->hdr, buf), strerror(errno));
            }
        }
    }
}

/* Read IDA packets from remote server directly into disk loop */

#define MAX_RETRY_INTERVAL 80
#define INITIAL_RETRY_INTERVAL 10

void ReadFromISI(ISIDL_PAR *par)
{
int i, SiteIndex;
BOOL ok, first = TRUE;
ISI_RAW_PACKET raw;
NRTS_DL *nrts;
UINT32 maxlen, alert;
UINT8 *buf;
int ReadResult, RetryInterval = INITIAL_RETRY_INTERVAL;
char ServerIdent[MAXPATHLEN+1];
BOOL NeedRT593Patch = FALSE;
BOOL InterceptEnabled = FALSE;
static char *fid = "ReadFromISI";

    maxlen = FindLargestPacketLength(par);

    if ((buf = (UINT8 *) malloc(maxlen)) == NULL) {
        LogMsg(LOG_INFO, "%s: malloc: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 9);
    }
    InitISI(par);

    for (i = 0; i < par->nsite; i++) {
        if ((nrts = par->dl[i]->nrts) != NULL) {
            nrts->flags &= ~NRTS_DL_FLAGS_STATION_SYSTEM & ~NRTS_DL_FLAGS_STRICT_TIME;
            nrts->tol = par->tol;
            LogMsg(LOG_INFO, "%s NRTS support enabled, flags=0x%0x, tictol=%lu, bwdint=%d", par->dl[i]->sys->site, nrts->flags, nrts->tol.tic, nrts->bwd.flushint);
        } else {
            LogMsg(LOG_INFO, "%s NRTS support disabled", par->dl[i]->sys->site);
        }
    }

    switch (par->scl.source) {
      case SCL_UNSPECIFIED:
        LogMsg(LOG_INFO, "taking ISI input from %s:%d (all streams)", server, isiparam.port);
        break;
      case SCL_FROM_NRTS:
        LogMsg(LOG_INFO, "taking ISI input from %s:%d (NRTS streams only)", server, isiparam.port);
        break;
      case SCL_FROM_DB:
        LogMsg(LOG_INFO, "taking ISI input from %s:%d (streams from database)", server, isiparam.port);
        break;
      case SCL_FROM_FILE:
        LogMsg(LOG_INFO, "taking ISI input from %s:%d (streams read from 's')", server, isiparam.port, par->scl.spec);
        break;
      default:
        LogMsg(LOG_INFO, "*** program bug *** unexpected fall through on SCL source");
        Exit(MY_MOD_ID +10);
    }

    if (par->flags & ISIDL_FLAG_PATCH_RT593) {
        NeedRT593Patch = TRUE;
        LogMsg(LOG_INFO, "RT593 patch will be applied (if necessary)");
    }
    if (par->flags & ISIDL_FLAG_INTERCEPT) {
        InterceptEnabled = TRUE;
        LogMsg(LOG_INFO, "raw packet intercept processing enabled");
    }

    while (1) {

    /* Connect to server, if necessary */

        while (isi == NULL) {
            if ((isi = ConnectToServer(par)) == NULL) {
                sleep(RetryInterval);
                RetryInterval *= 2;
                if (RetryInterval > MAX_RETRY_INTERVAL) RetryInterval = INITIAL_RETRY_INTERVAL;
            } else {
                sprintf(ServerIdent, "pid%lu@%s", isi->iacp->peer.pid, isi->iacp->peer.name);
                LogMsg(LOG_INFO, "connected to %s, I/O timeout=%.3f sec", ServerIdent, (float) isi->iacp->attr.at_timeo / 1000.0);
            }
        }

    /* Read next packet from server and update DL sequence number */


        ReadResult = ReadRawPacket(isi, &raw, buf, maxlen);
        switch (ReadResult) {
          case ISI_OK:
            if ((SiteIndex = LocateDiskLoopBySite(par, raw.hdr.site)) < 0) {
                LogMsg(LOG_INFO, "unexpected packet from site '%s' ignored", raw.hdr.site);
            } else {
                if (VerifySequence(par->dl[SiteIndex], &raw)) {
                    if (NeedRT593Patch) RT593PatchTest(&raw, par->dl[SiteIndex]);
                    if (InterceptEnabled) InterceptProcessor(&raw, par->dl[SiteIndex], &par->ib);
                    BlockShutdown(fid);
                        ok = isidlWriteToDiskLoop(par->dl[SiteIndex], &raw, 0);
                    UnblockShutdown(fid);
                    if (!ok) {
                        LogMsg(LOG_INFO, "%s: isidlWriteToDiskLoop: %s", fid, strerror(errno));
                        Exit(MY_MOD_ID + 11);
                    }
                }
                if (par->first[SiteIndex]) {
                    LogMsg(LOG_INFO, "initial %s packet received", raw.hdr.site);
                    par->first[SiteIndex] = FALSE;
                }
            }
            RetryInterval = INITIAL_RETRY_INTERVAL;
            break;
          case ISI_DONE:
            LogMsg(LOG_INFO, "EOF received from %s", ServerIdent);
            CloseConnection(isi, ServerIdent, par);
            return;
          case ISI_BREAK:
            LogMsg(LOG_INFO, "%s disconnected, cause code %d (%s)", ServerIdent, isi->alert, iacpAlertString(isi->alert));
            switch (isi->alert) {
              case IACP_ALERT_SERVER_FAULT:
              case IACP_ALERT_PROTOCOL_ERROR:
              case IACP_ALERT_ILLEGAL_DATA:
                LogMsg(LOG_INFO, "*** abandoning all further attempts to contact %s ***", server);
                Exit(MY_MOD_ID + 12);
                break;
              default:
                CloseConnection(isi, ServerIdent, par);
                isi = NULL;
                sleep(RetryInterval);
                RetryInterval *= 2;
                if (RetryInterval > MAX_RETRY_INTERVAL) RetryInterval = INITIAL_RETRY_INTERVAL;
            }
            break;
          case ISI_TIMEDOUT:
            LogMsg(LOG_INFO, "%s: connection timed out", ServerIdent, ReadResult);
            CloseConnection(isi, ServerIdent, par);
            isi = NULL;
            break;
          case ISI_CONNRESET:
            LogMsg(LOG_INFO, "%s: connection reset", ServerIdent, ReadResult);
            CloseConnection(isi, ServerIdent, par);
            isi = NULL;
            break;
          case ISI_BADMSG:
            LogMsg(LOG_INFO, "%s: corrupt packet received", ServerIdent, ReadResult);
            CloseConnection(isi, ServerIdent, par);
            isi = NULL;
            break;
          default:
            LogMsg(LOG_INFO, "%s: UNEXPECTED ReadResult = %d", fid, ReadResult);
            CloseConnection(isi, ServerIdent, par);
            isi = NULL;
        }
    }
}

/* Revision History
 *
 * $Log: isi.c,v $
 * Revision 1.25  2008/03/05 23:25:27  dechavez
 * Added intercept processing support
 *
 * Revision 1.24  2008/01/25 22:04:04  dechavez
 * added support for scl (stream control list)
 *
 * Revision 1.23  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.22  2007/02/20 02:45:05  dechavez
 * Fixed "reconnect at beginning" bug for isi feeds with specific beg seqno.
 *
 * Revision 1.21  2007/02/08 22:55:18  dechavez
 * LOG_ERR to LOG_INFO
 *
 * Revision 1.20  2007/01/11 22:02:21  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.19  2006/12/12 23:28:45  dechavez
 * remove obsoleted metadata wor
 *
 * Revision 1.18  2006/09/29 19:51:43  dechavez
 * added support for socket send/receive buffers, with 64Kbyte default
 *
 * Revision 1.17  2006/06/27 00:32:25  dechavez
 * added support for ISI_TIMEDOUT and ISI_CONNRESET error returns
 *
 * Revision 1.16  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.15  2006/06/02 21:03:32  dechavez
 * added branch for processing QDP metadata (not sure it will stay here though)
 *
 * Revision 1.14  2006/03/30 22:04:13  dechavez
 * immediately exit ToggleISIDebugState() if system is not an ISI reader
 *
 * Revision 1.13  2006/03/14 20:31:48  dechavez
 * pass fid to block/unlock exit mutex calls, for debugging use
 *
 * Revision 1.12  2006/03/13 23:09:40  dechavez
 * Bracket ISI_DL access with mutexes to prevent race condition in shutdown from closing active disk loops
 *
 * Revision 1.11  2005/12/09 21:27:17  dechavez
 * clear all options for isiWriteToDiskLoop()
 *
 * Revision 1.10  2005/10/17 21:19:53  dechavez
 * removed trace debug messages, save and report unexpected ReadRawPacket return value
 *
 * Revision 1.9  2005/09/30 22:34:02  dechavez
 * added ToggleISIDebugState(), plus lots of temporary trace statements
 *
 * Revision 1.8  2005/09/15 22:37:31  dechavez
 * Fixed core dump on server disconnect bug
 *
 * Revision 1.7  2005/09/13 00:40:52  dechavez
 * added dbgpath support, print decoded alert messages when server closes connection,
 * abandon reconnect attempts if reason was due to server errors and for all other
 * causes sleep progressively longer intervals between successive attempts
 *
 * Revision 1.6  2005/09/10 22:07:20  dechavez
 * Reworked to use explicit disk I/O for ISI disk loop operations instead of
 * mapped memory assignments.
 *
 * Revision 1.5  2005/08/26 20:10:21  dechavez
 * include site name when logging dropped overlaps
 *
 * Revision 1.4  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
