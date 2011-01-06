#pragma ident "$Id: baro.c,v 1.1 2009/03/24 21:11:40 dechavez Exp $"
/*======================================================================
 *
 * Service a single barometer.
 *
 *====================================================================*/
#include "parodl.h"

#define MY_MOD_ID PARODL_MOD_BARO

static BOOL debug = FALSE;

void ToggleBarometerDebugState()
{
    debug = !debug;
    if (debug) {
        LogMsg(LOG_INFO, "barometer debug output enabled");
    } else {
        LogMsg(LOG_INFO, "barometer debug output suspended");
    }
}

BOOL BarometerDebugEnabled()
{
    return debug;
}

static void StoreSample(BAROMETER *bp, INT32 value)
{
static char *fid = "StoreSample";

    MUTEX_LOCK(&bp->mutex);
        bp->sample = value;
    MUTEX_UNLOCK(&bp->mutex);
    if (BarometerDebugEnabled()) LogMsg(LOG_INFO, "read %ld from %s sensor", value, bp->cname);
    if (!bp->ValidSampleSeen) {
        LogMsg(LOG_INFO, "initial %s sample received", bp->cname);
        bp->ValidSampleSeen = TRUE;
    }
}

INT32 GetSample(BAROMETER *bp)
{
INT32 value;
static char *fid = "GetSample";

    MUTEX_LOCK(&bp->mutex);
        if (bp->ValidSampleSeen) {
            value = bp->sample;
            bp->sample = PARODL_MISSED_SAMPLE;
        } else {
            value = PARODL_NO_SENSOR;
        }
    MUTEX_UNLOCK(&bp->mutex);

    return value;
}

static BOOL ParseBarometerArgs(BAROMETER *bp, char *argstr)
{
LNKLST *TokenList;
static char *fid = "ParseBarometerArgs";

    if ((TokenList = utilStringTokenList(argstr, ":,", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (!listSetArrayView(TokenList)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (TokenList->count != 3 && TokenList->count != 4) {
        fprintf(stderr, "%s: unexpected token count '%d'\n", fid, TokenList->count);
        return FALSE;
    }

    if ((bp->port = strdup((char *) TokenList->array[0])) == NULL) {
        fprintf(stderr, "%s: strdup: %s\n", fid, strerror(errno));
        return FALSE;
    }

    bp->baud = atoi(TokenList->array[1]);

    strlcpy(bp->cname, (char *) TokenList->array[2], IDA10_CNAMLEN+1);

    if (TokenList->count == 4) {
        bp->MsecSint = atoi(TokenList->array[3]);
    } else {
        bp->MsecSint = DEFAULT_SINT;
    }

    return TRUE;
}

static BOOL InitHandle(BAROMETER *bp, char *argstr, char *sname, BOOL debug)
{
static UINT32 fixedlen = IDA_BUFSIZ;

/* lp and debug fields get set in main */

    MUTEX_INIT(&bp->mutex);

    if (!ParseBarometerArgs(bp, argstr)) return FALSE;

    bp->pp   = NULL;

    bp->sample = PARODL_MISSED_SAMPLE;
    bp->ValidSampleSeen = FALSE;
    bp->ValidSensorSeen = FALSE;
    bp->nsamp = 0;
    bp->maxsamp = IDA10_DEFDATALEN / sizeof(INT32);

    if (!isiInitRawPacket(&bp->pkt, NULL, fixedlen)) return FALSE;
    bp->pkt.hdr.len.used = fixedlen;
    bp->pkt.hdr.len.native = fixedlen;
    bp->pkt.hdr.desc.comp = ISI_COMP_NONE;
    bp->pkt.hdr.desc.type = ISI_TYPE_IDA10;
    bp->pkt.hdr.desc.order = ISI_ORDER_UNDEF;
    bp->pkt.hdr.desc.size = sizeof(UINT8);
    strlcpy(bp->pkt.hdr.site, sname, ISI_SITELEN+1);

    return TRUE;
}

BOOL AddBarometer(LNKLST *list, char *argstr, char *sname)
{
BAROMETER new;
static char *fid = "AddBarometer";

    if (!InitHandle(&new, argstr, sname, debug)) return FALSE;
    if (!listAppend(list, &new, sizeof(BAROMETER))) {
        fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
        return FALSE;
    }

    return TRUE;
}

static BOOL ConnectToDevice(BAROMETER *bp)
{
#define LOOP_INTERVAL (10 * MSEC_PER_SEC)
#define LOG_INTERVAL  (3600 * MSEC_PER_SEC)
#define LOG_TEST (LOG_INTERVAL / LOOP_INTERVAL)
int count;
static char *fid = "ConnectToDevice";

/* connect to barometer and verify constants */

    count = bp->ValidSensorSeen ? LOG_TEST - 1 : -1;

    while (bp->pp == NULL) {
        if (BarometerDebugEnabled()) LogMsg(LOG_INFO, "%s: chan=%s port=%s, baud=%d", fid, bp->cname, bp->port, bp->baud);
        if ((bp->pp = paroOpen(bp->port, bp->baud, bp->lp, bp->debug)) == NULL) {
            if (++count % LOG_TEST == 0) LogMsg(LOG_INFO, "chan '%s' barometer@%s not responding: %s", bp->cname, bp->port, strerror(errno));
            utilDelayMsec(LOOP_INTERVAL);
        }
    }

    LogMsg(LOG_INFO, "chan '%s' barometer@%s OK (S/N %s)\n", bp->cname, bp->port, bp->pp->param.sn);
    bp->ValidSensorSeen = TRUE;

    paroGetcnf(bp->pp);
    paroCommand(bp->pp, PARO_P4);
    return TRUE;
}

THREAD_FUNC ReadThread(void *argptr)
{
INT32 value;
BOOL BarometerOK;
BAROMETER *bp;

    bp = (BAROMETER *) argptr;

    LogMsg(LOG_INFO, "reading %s data from %s", bp->cname, bp->port);

/* Loop forever, reading and saving readings */

    BarometerOK = FALSE;

    while (1) {

        while (!BarometerOK) BarometerOK = ConnectToDevice(bp);

        if (!paroReadINT32(bp->pp, &value)) {
            LogMsg(LOG_INFO, "paroReadINT32 failed: %s", strerror(errno));
            BarometerOK = FALSE;
        } else {
            StoreSample(bp, value);
        }
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: baro.c,v $
 * Revision 1.1  2009/03/24 21:11:40  dechavez
 * initial release
 *
 */
