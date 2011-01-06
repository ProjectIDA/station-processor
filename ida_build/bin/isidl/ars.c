#pragma ident "$Id: ars.c,v 1.7 2008/03/05 23:26:36 dechavez Exp $"
/*======================================================================
 *
 *  Serial data from a Reftek ARS
 *
 *====================================================================*/
#include "isidl.h"
#include "sanio.h" /* for the time conversion factor */

#define MY_MOD_ID ISIDL_MOD_ARS

#define DEFAULT_BAUD     19200
#define DEFAULT_PARITY   TTYIO_PARITY_NONE
#define DEFAULT_SBITS    2
#define DEFAULT_FLOW     TTYIO_FLOW_SOFT

#define DEFAULT_PKTREV   8
#define MIN_ARS_PKTREV   5
#define MAX_ARS_PKTREV   8

/* various constants for revs 5-9 */

#define REV5678_PREAMBLE_LEN 2
#define REV5678_HLEN 60
#define REV5678_DLEN 964 /* last for bytes are padding */

/* Print timeout messages in a throttled manner */

static void LogTimeoutMessage(char *fid)
{
static time_t LastReportTime = 0;
static time_t ReportInterval = 60;
time_t now;

    now = time(NULL);
    if ((now - LastReportTime) >= ReportInterval) {
        LogMsg(LOG_INFO, "%s: timeout reading ARS", fid);
        LastReportTime = now;
    }
}

/* Check to see if the candidate preamble matches known IDA packets */

static BOOL Verify5678(char *preamble) 
{
    if (preamble[0] != ~preamble[1]) return FALSE;
    switch (preamble[0]) {
      case 'A':
      case 'C':
      case 'D':
      case 'E':
      case 'H':
      case 'I':
      case 'K':
      case 'L':
        return TRUE;
    }

    return FALSE;
}

/* Sync to the start of an IDA rev 5/6/7/8 packet */

static int Sync5678(TTYIO *tp, UINT8 *dest, int pktrev)
{
int i;
UINT32 dumped;
static int first = TRUE;
static char *fid = "Sync5678";

    for (i = 0; i < REV5678_PREAMBLE_LEN; i++) {
        while (ttyioRead(tp, &dest[i], 1) != 1) {
            if (errno != ETIMEDOUT) {
                LogMsg(LOG_INFO, "%s: ttyioRead: %s", fid, strerror(errno));
                Exit(MY_MOD_ID + 1);
            }
            LogTimeoutMessage(fid);
        }
    }

    dumped = 0;
    while (!Verify5678(dest)) {
        dest[0] = dest[1];
        if (++dumped % 10000 == 0) {
            LogMsg(LOG_INFO, "%s: attempting to sync, %lu bytes dumped so far", fid, dumped);
            LogMsg(LOG_INFO, "%s: VERIFY MODEM PORT SPEED IS %ld", fid, tp->attr.at_ibaud);
        }
        while (ttyioRead(tp, &dest[1], 1) != 1) {
            if (errno != ETIMEDOUT) {
                LogMsg(LOG_INFO, "%s: ttyioRead: %s", fid, strerror(errno));
                Exit(MY_MOD_ID + 2);
            }
            LogTimeoutMessage(fid);
        }
    }

    if (first || dumped != 0) {
        LogMsg(LOG_INFO, "synced to IDA %d ARS stream, %lu bytes dumped", pktrev, dumped);
        first = FALSE;
    }

    return REV5678_PREAMBLE_LEN;
}

/* Read one ARS packet (converting rev 8 packets into rev 9) */

static BOOL ReadOnePacket(ARS *ars)
{
UINT8 *ptr;
static int hlen = REV5678_HLEN - REV5678_PREAMBLE_LEN;
static int dlen = REV5678_DLEN;

/* sync to start of packet */

    ptr = ars->local.raw.payload;
    ptr += Sync5678(ars->tp, ptr, ars->pktrev);

/* read in header */

    if (ttyioRead(ars->tp, ptr, hlen) != hlen) return FALSE;
    ptr += hlen;

/* IDA 8 packets leave room for the creation time stamp */

    if (ars->pktrev == 8 && ida_rtype(ars->local.raw.payload, ars->pktrev) == IDA_DATA) ptr += sizeof(UINT32);

/* read in data */

    if (ttyioRead(ars->tp, ptr, dlen) != dlen) return FALSE;

    return TRUE;
}

/* Initialize ARS input */

BOOL ParseARSArgs(ARS *ars, char *argstr)
{
LNKLST *TokenList;
static char *fid = "ParseARSArgs";

    ars->port = NULL;
    ars->baud = DEFAULT_BAUD;
    ars->parity = DEFAULT_PARITY;
    ars->flow = DEFAULT_FLOW;
    ars->sbits = DEFAULT_SBITS;
    ars->pktrev = DEFAULT_PKTREV;

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count >= 1) { // dev
        if ((ars->port = strdup((char *) TokenList->array[0])) == NULL) {
            perror("strdup");
            return FALSE;
        }
    }

    if (TokenList->count >= 2) { // dev:baud
        if ((ars->baud = atoi((char *) TokenList->array[1])) <= 0) {
            fprintf(stderr, "illegal baud string '%s'\n", (char *) TokenList->array[1]);
            return FALSE;
        }
    }

    if (TokenList->count >= 3) { // dev:baud:pktrev
        ars->pktrev = atoi((char *) TokenList->array[2]);
        if (ars->pktrev < MIN_ARS_PKTREV || ars->pktrev > MAX_ARS_PKTREV) {
            fprintf(stderr, "illegal ars pktrev '%s'\n", (char *) TokenList->array[2]);
            return FALSE;
        }
    }

    if (TokenList->count >= 4) { // dev:baud:pktrev:flow
        if (strcasecmp((char *) TokenList->array[2], "hard")) {
            ars->flow = TTYIO_FLOW_HARD;
        } else if (strcasecmp((char *) TokenList->array[3], "soft")) {
            ars->flow = TTYIO_FLOW_SOFT;
        } else if (strcasecmp((char *) TokenList->array[3], "none")) {
            ars->flow = TTYIO_FLOW_NONE;
        } else {
            fprintf(stderr, "illegal flow string '%s'\n", (char *) TokenList->array[3]);
            return FALSE;
        }
    }

    return TRUE;
}

static void ConnectARS(ISIDL_PAR *par)
{
static char *fid = "ConnectARS";

    par->ars.tp = ttyioOpen(
        par->ars.port,   /* device name */
        TRUE,            /* exclusive use */
        par->ars.baud,   /* input speed */
        par->ars.baud,   /* output speed */
        par->ars.parity, /* parity */
        par->ars.flow,   /* flow control */
        par->ars.sbits,  /* stop bits */
        par->timeout,    /* timeout in msec */
        0,               /* not using a pipe filling thread */
        par->lp
    );

    if (par->ars.tp == NULL) {
        LogMsg(LOG_INFO, "%s: ttyioOpen: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 7);
    }
    ttyioSetLog(par->ars.tp, par->lp);

    switch (par->ars.tp->attr.at_flow) {
      case TTYIO_FLOW_NONE:
        LogMsg(LOG_INFO, "connected to ars@%s, %d msec timeout, no flow control", par->ars.port, par->ars.tp->attr.at_to);
        break;
      case TTYIO_FLOW_SOFT:
        LogMsg(LOG_INFO, "connected to ars@%s, %d msec timeout, software flow control", par->ars.port, par->ars.tp->attr.at_to);
        break;
      case TTYIO_FLOW_HARD:
        LogMsg(LOG_INFO, "connected to ars@%s, %d msec timeout, hardware flow control", par->ars.port, par->ars.tp->attr.at_to);
        break;
      default:
        LogMsg(LOG_INFO, "connected to ars@%s, %d msec timeout, UNKNOWN flow control????", par->ars.port, par->ars.tp->attr.at_to);
        break;
    }
}

/* Read IDA packets from serial port directly into disk loop */

static THREAD_FUNC ARSThread(void *argptr)
{
ISIDL_PAR *par;
ARS *ars;
BOOL first = TRUE;
ISI_DL *dl;
NRTS_DL *nrts;
static char *fid = "ARSThread";

    par = (ISIDL_PAR *) argptr;

    dl = par->dl[0];
    ars = &par->ars;

    if ((nrts = dl->nrts) != NULL) {
        nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM;
        nrts->tol = par->tol;
        if (par->tol.clock != 0) nrts->flags |= NRTS_DL_FLAGS_STRICT_TIME;
        LogMsg(LOG_INFO, "%s NRTS support enabled, flags=0x%0x, tictol=%lu, bwdint=%d", par->dl[0]->sys->site, nrts->flags, nrts->tol.tic, nrts->bwd.flushint);
        if (nrts->flags & NRTS_DL_FLAGS_STRICT_TIME) {
            LogMsg(LOG_INFO, "strict clock checks enabled, tolerance = %lu sec", nrts->tol.clock);
        } else {
            LogMsg(LOG_INFO, "strict clock checks disabled");
        }
        if (par->flags & ISIDL_FLAG_LEAP_YEAR_CHECK) {
            nrts->flags |= NRTS_DL_FLAGS_LEAP_YEAR_CHK;
            LogMsg(LOG_INFO, "leap year bug check enabled");
        }
    } else {
        LogMsg(LOG_INFO, "old NRTS support disabled");
    }

    if (ars->local.rt593) LogMsg(LOG_INFO, "RT593 patch will be applied");

    ConnectARS(par);

    while (1) {

    /* read one complete packet */

        while (!ReadOnePacket(ars)) {
            if (errno != ETIMEDOUT) {
                LogMsg(LOG_INFO, "%s: ttyioRead: %s", fid, strerror(errno));
                Exit(MY_MOD_ID + 10);
            }
            LogTimeoutMessage(fid);
        }

        if (first) {
            LogMsg(LOG_INFO, "initial ARS packet received");
            first = FALSE;
        }

    /* update header and write to disk loop */

        ProcessLocalData(&ars->local);
    }
}

void StartArsReader(ISIDL_PAR *par)
{
THREAD tid;
static char *fid = "StartArsReader";

    if (!THREAD_CREATE(&tid, ARSThread, (void *) par)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: ARSThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 11);
    }
}

/* Revision History
 *
 * $Log: ars.c,v $
 * Revision 1.7  2008/03/05 23:26:36  dechavez
 * added ARS leap year bug check support
 *
 * Revision 1.6  2007/03/26 21:51:32  dechavez
 * RT593 support
 *
 * Revision 1.5  2007/02/08 22:56:56  dechavez
 * LOG_ERR to LOG_INFO, use LOCALPKT handle
 *
 * Revision 1.4  2006/06/30 18:18:02  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.3  2006/06/27 00:18:47  akimov
 * misc WIN32 warning removals
 *
 * Revision 1.2  2006/06/19 19:15:49  dechavez
 * use pktrev aware ARS handle to only convert IDA8 packets to IDA9, leaving
 * the earlier versions untouched
 *
 * Revision 1.1  2006/03/30 22:08:36  dechavez
 * former tty.c, modified to use fill packets via message queue
 *
 */
