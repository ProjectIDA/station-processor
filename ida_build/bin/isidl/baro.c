#pragma ident "$Id: baro.c,v 1.8 2009/05/14 16:42:34 dechavez Exp $"
/*======================================================================
 *
 * Service a Paroscientific barometer.
 *
 *====================================================================*/
#include "isidl.h"
#include "paro.h"

#define MY_MOD_ID ISIDL_MOD_BARO

static BOOL debug = FALSE;

void ToggleBarometerDebugState()
{
    debug = !debug;
}

BOOL BarometerDebugEnabled()
{
    return debug;
}

static void SaveSample(BAROMETER *bp, INT32 value)
{
    MUTEX_LOCK(&bp->mutex);
        bp->sample = value;
    MUTEX_UNLOCK(&bp->mutex);
}

INT32 GetSample(BAROMETER *bp)
{
INT32 value;

    MUTEX_LOCK(&bp->mutex);
        value = bp->sample;
        bp->sample = MISSED_BAROMETER_SAMPLE;
    MUTEX_UNLOCK(&bp->mutex);

    return value;
}

static BOOL ParseBarometerArgs(BAROMETER *bp, char *argstr, int source)
{
static char *fid = "ParseBarometerArgs";
LNKLST *TokenList;

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    switch (TokenList->count) {
      case 3:
        bp->format.type = ISI_TYPE_IDA10;
        if ((bp->format.ida10.chname = strdup((char *) TokenList->array[2])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            return FALSE;
        }
        bp->maxsamp = IDA_MAXDLEN / sizeof(UINT32);
        break;

      case 4:
        bp->format.type = ISI_TYPE_MSEED;
        if ((bp->format.mseed.chn = strdup((char *) TokenList->array[2])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            return FALSE;
        }
        if ((bp->format.mseed.loc = strdup((char *) TokenList->array[3])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            return FALSE;
        }
        bp->maxsamp = 0; // NEED TO DEFINE
        break;

      case 5:
        bp->format.type = ISI_TYPE_IDA9;
        bp->format.ida9.strm = atoi((char *) TokenList->array[2]);
        bp->format.ida9.chan = atoi((char *) TokenList->array[3]);
        bp->format.ida9.filt = atoi((char *) TokenList->array[4]);
        bp->maxsamp = IDA_MAXDLEN / sizeof(UINT32);
        break;

#ifdef INCLUDE_Q330
      case 6:
        bp->format.type = ISI_TYPE_QDPLUS;
        bp->format.qdplus.seqno = 0;
        bp->format.qdplus.serialno = (UINT64) strtoll((char *) TokenList->array[2], NULL, 16);
        if ((bp->format.qdplus.chn = strdup((char *) TokenList->array[3])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            return FALSE;
        }
        if ((bp->format.qdplus.loc = strdup((char *) TokenList->array[4])) == NULL) {
            fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
            return FALSE;
        }
        bp->maxsamp = atoi((char *) TokenList->array[5]);
        if (bp->maxsamp < 1) {
            fprintf(stderr, "%s: illegal QDP baromater nsamp %d (too small)", fid, bp->maxsamp);
            return FALSE;
        } else if (bp->maxsamp > QDPLUS_DT_USER_AUX_MAX_PAYLOAD / sizeof(UINT32)) {
            fprintf(stderr, "%s: illegal QDP baromater nsamp %d (too larg)", fid, bp->maxsamp);
            return FALSE;
        }
        break;
#endif /* INCLUDE_Q330 */

      default:
        fprintf(stderr, "%s: unexpected token count '%d'\n", fid, TokenList->count);
        return FALSE;
    }

    if ((bp->port = strdup((char *) TokenList->array[0])) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
        return FALSE;
    }
    bp->baud = atoi(TokenList->array[1]);

    return TRUE;
}

static BOOL InitDevice(BAROMETER *bp)
{
#define LOOP_INTERVAL (10 * MSEC_PER_SEC)
#define LOG_INTERVAL  (3600 * MSEC_PER_SEC)
#define LOG_TEST (LOG_INTERVAL / LOOP_INTERVAL)
int count;
BOOL ready = FALSE;

/* connect to barometer and verify constants */

    count = bp->ValidSensorSeen ? LOG_TEST - 1 : -1;

    while (!ready) {
        if (bp->pp == NULL) bp->pp = paroOpen(bp->port, bp->baud, bp->lp, FALSE);
        if (bp->pp != NULL && paroInit(bp->pp, &bp->param)) ready = TRUE;
        if (!ready) {
            if (++count % LOG_TEST == 0) LogMsg(LOG_INFO, "barometer@%s not responding: %s", bp->port, strerror(errno));
            utilDelayMsec(LOOP_INTERVAL);
        }
    }

    LogMsg(LOG_INFO, "barometer@%s OK (S/N %s)\n", bp->port, bp->param.sn);
    bp->ValidSensorSeen = TRUE;

    paroCommand(bp->pp, PARO_P4);
    return TRUE;
}

static THREAD_FUNC ReadThread(void *argptr)
{
char *reply;
BOOL BarometerOK;
BAROMETER *bp;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "reading barometer data from %s", bp->port);

/* Loop forever, reading and saving readings */

    BarometerOK = FALSE;

    while (1) {

        while (!BarometerOK) BarometerOK = InitDevice(bp);

        if ((reply = paroRead(bp->pp)) == NULL) {
            BarometerOK = FALSE;
        } else {
            SaveSample(bp, (INT32) atoi(reply));
        }
    }
}

static BOOL InitBarometer(ISIDL_PAR *par, BAROMETER *bp, char *argstr)
{

    if (!ParseBarometerArgs(bp, argstr, par->source)) return FALSE;

    MUTEX_INIT(&bp->mutex);

    bp->pp   = NULL;

    bp->ValidSampleSeen = FALSE;
    bp->sample = MISSED_BAROMETER_SAMPLE;
    bp->nsamp = 0;

    bp->lp = GetLogHandle();

    bp->param.pr = PARO_DEFAULT_PR;
    bp->param.pi = PARO_DEFAULT_PI;
    bp->param.un = PARO_DEFAULT_UN;
    bp->param.uf = PARO_DEFAULT_UF;
    bp->param.pa = PARO_DEFAULT_PA;
    bp->param.pm = PARO_DEFAULT_PM;

    return TRUE;
}

BOOL AddBarometer(ISIDL_PAR *par, char *argstr)
{
BAROMETER new;
static char *fid = "AddBarometer";

    if (!InitBarometer(par, &new, argstr)) return FALSE;
    if (!listAppend(&par->baro, &new, sizeof(BAROMETER))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

void StartBarometerReader(BAROMETER *bp)
{
THREAD tid;
static char *fid = "StartBarometerThread";

/* Launch the barometer reader */

    if (!THREAD_CREATE(&tid, ReadThread, (void *) bp)) {
        LogMsg(LOG_INFO, "%s: THREAD_CREATE: ReadThread: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 6);
    }

/* Launch the packet builder thread */

    switch (bp->format.type) {
      case ISI_TYPE_IDA9:
        if (!THREAD_CREATE(&tid, Ida9BarometerThread, (void *) bp)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: Ida9BarometerThread: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 8);
        }
        break;

      case ISI_TYPE_IDA10:
        if (!THREAD_CREATE(&tid, Ida10BarometerThread, (void *) bp)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: Ida10BarometerThread: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 9);
        }
        break;

      case ISI_TYPE_MSEED:
        if (!THREAD_CREATE(&tid, MseedBarometerThread, (void *) bp)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: MseedBarometerThread: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 10);
        }
        break;

#ifdef INCLUDE_Q330
      case ISI_TYPE_QDPLUS:
        if (!THREAD_CREATE(&tid, QdplusBarometerThread, (void *) bp)) {
            LogMsg(LOG_INFO, "%s: THREAD_CREATE: Ida9BarometerThread: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 8);
        }
        break;
#endif /* INCLUDE_Q330 */

      default:
        LogMsg(LOG_INFO, "%s: unsupported packet format code '%d'", fid, bp->format.type);
        Exit(MY_MOD_ID + 11);
    }
}

/* Revision History
 *
 * $Log: baro.c,v $
 * Revision 1.8  2009/05/14 16:42:34  dechavez
 * Changed paroOpen() to conform with libparo 1.2.1
 *
 * Revision 1.7  2007/02/08 22:56:49  dechavez
 * LOG_ERR, LOG_WARN to LOG_INFO
 *
 * Revision 1.6  2006/06/30 18:18:34  dechavez
 * replaced message queues for processing locally acquired data with static buffers
 *
 * Revision 1.5  2006/06/19 19:12:18  dechavez
 * Conditional Q330 support, use ISI_TYPE_x constants for output types
 *
 * Revision 1.4  2006/06/07 22:41:48  dechavez
 * include format specific maxsamp
 *
 * Revision 1.3  2006/06/02 21:06:54  dechavez
 * support for QDPLUS format
 *
 * Revision 1.2  2006/04/03 21:38:06  dechavez
 * include paroOpen as part of device init in order to permit starting system
 * without a barometer physically connected (ie, expect it to get added later
 * and want to immediately start acquiring data without interrupting other inputs)
 *
 * Revision 1.1  2006/03/30 22:04:48  dechavez
 * initial release
 *
 */
