#pragma ident "$Id: read.c,v 1.8 2009/05/14 18:12:44 dechavez Exp $"
/*======================================================================
 *
 *  Read QDPLUS packets from the source disk loop
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_READ

#define WAITING_FOR_NEW_DATA 0
#define DATA_AVAILABLE       1

#define SAVE_STATE_INTERVAL  30 /* seconds */

static struct {
    UINT32 nxtndx;
    UINT32 endndx;
    UINT64 count;
    int status;
} history;

void SaveState(QDPLUS *handle)
{
static BOOL suspended = FALSE;
static char *fid = "SaveState";

    logioUpdateWatchdog(Watch, fid);

    if (suspended) return;

    if (!qdplusSaveState(handle)) {
        LogMsg("%s: qdplusSaveState failed: %s", fid, strerror(errno));
        LogMsg("%s: suspended", fid);
        suspended = TRUE;
    }
}

/* Wait until new data are available */

static int PauseForNewData(QDPLUS *handle, ISI_DL *input)
{
ISI_DL snapshot;
ISI_DL_SYS sys;
BOOL HaveNotSavedState;
static char *fid = "PauseForNewData";

    logioUpdateWatchdog(Watch, fid);

/* Check for new data once per second */

    HaveNotSavedState = TRUE;

    while (history.status != DATA_AVAILABLE) {
        QuitOnShutdown(MY_MOD_ID + 0);
        if (!isidlSnapshot(input, &snapshot, &sys)) {
            LogMsg("%s: isidlSnapshot failed: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 1);
        }
        if (snapshot.sys->count > history.count) {
            if (history.nxtndx == ISI_UNDEFINED_INDEX) { /* ie, was a virgin dl */
                history.nxtndx = snapshot.sys->index.oldest;
            } else {
                history.nxtndx = (history.nxtndx + 1) % snapshot.sys->numpkt;
            }
            history.endndx = snapshot.sys->index.yngest;
            history.status = DATA_AVAILABLE;
        } else {
            if (HaveNotSavedState) {
                SaveState(handle);
                HaveNotSavedState = FALSE;
            }
            utilDelayMsec(1000);
        }
    }
}

/* Read the next packet from the disk loop */

void ReadNextPacket(QHLP_PAR *qhlp, QDPLUS_PKT *pkt)
{
static UINT64 count = 0;
static char *fid = "ReadNextPacket";

    logioUpdateWatchdog(Watch, fid);

    if (history.status == WAITING_FOR_NEW_DATA) PauseForNewData(qhlp->qdplus, qhlp->input.dl);

    logioUpdateWatchdog(Watch, "ReadNextPacket:isidlReadDiskLoop");
    if (!isidlReadDiskLoop(qhlp->input.dl, &qhlp->input.raw, history.nxtndx)) {
        LogMsg("%s: isidlReadDiskLoop (index %lu) failed: %s", fid, history.nxtndx, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }

    if (history.nxtndx == history.endndx) {
        history.status = WAITING_FOR_NEW_DATA;
        history.count = qhlp->input.dl->sys->count;
    } else {
        history.nxtndx = (history.nxtndx + 1) % qhlp->input.dl->sys->numpkt;
    }

    logioUpdateWatchdog(Watch, "ReadNextPacket:qdplusUnpackWrappedQDP");
    qdplusUnpackWrappedQDP(qhlp->input.raw.payload, pkt);
}

/* Sanity check on indicies */

static BOOL VerifyIndicies(ISI_DL *dl)
{
static char *fid = "RecoverState:VerifyIndicies";

    if (isiIsValidIndex(history.nxtndx) && isiIsValidIndex(history.endndx)) {
        history.status = DATA_AVAILABLE;
    } else if (history.nxtndx == ISI_UNDEFINED_INDEX) {
        history.status = WAITING_FOR_NEW_DATA;
        history.count = dl->sys->count;
    } else {
        LogMsg("%s: FATAL ERROR: corrupt disk loop: nxtndx = 0x%08x, endndx = 0x%08x", fid, history.nxtndx, history.endndx);
        return FALSE;
    }

    return TRUE;
}

/* Pre-load the LCQ to recreate the earlier state */

void RecoverState(QHLP_PAR *qhlp)
{
ISI_DL snapshot;
ISI_DL_SYS sys;
QDPLUS_PKT pkt;
struct { ISI_SEQNO seqno; UINT32 index; } beg, end;
static char *fid = "RecoverState";

/* Get the current state of the disk loop */

    if (!isidlSnapshot(qhlp->input.dl, &snapshot, &sys)) {
        LogMsg("%s: isidlSnapshot failed: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 3);
    }

/* If there isn't any preserved state, then configure to dump the entire QDP disk loop */

    if (qhlp->qdplus->state != QDPLUS_STATE_OK) {
        LogMsg("no valid HLP state data found, default to oldest QDP data available");
        history.nxtndx = snapshot.sys->index.oldest;
        history.endndx = snapshot.sys->index.yngest;
        if (!VerifyIndicies(&snapshot)) Exit(MY_MOD_ID + 4);
        qdplusSetLcqStateFlag(qhlp->qdplus, QDP_LCQ_STATE_PRODUCTION);
        LogMsg("going into production mode with nxtndx=0x%08x, endndx=0x%08x\n", history.nxtndx, history.endndx);
        return;
    }

/* We have state... find the range of packets to preload */

    if (!qdplusStateSeqnoLimits(qhlp->qdplus, &beg.seqno, &end.seqno)) {
        LogMsg("%s: qdplusStateSeqnoLimits: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 5);
    }

    if (isiIsUndefinedSeqno(&beg.seqno) || isiIsUndefinedSeqno(&end.seqno)) {
        LogMsg("empty HLP state data found, default to oldest QDP data available");
        history.nxtndx = snapshot.sys->index.oldest;
        history.endndx = snapshot.sys->index.yngest;
        if (!VerifyIndicies(&snapshot)) Exit(MY_MOD_ID + 6);
        qdplusSetLcqStateFlag(qhlp->qdplus, QDP_LCQ_STATE_PRODUCTION);
        LogMsg("going into production mode with nxtndx=0x%08x, endndx=0x%08x\n", history.nxtndx, history.endndx);
        return;
    }

    if (!isiIsAbsoluteSeqno(&beg.seqno) || !isiIsAbsoluteSeqno(&end.seqno)) {
        LogMsg("%s: unexpected non-absolute sequence numbers", fid);
        Exit(MY_MOD_ID + 7);
    }

    beg.index = isidlSearchDiskLoopForSeqno(&snapshot, &beg.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
    LogMsg("beg.seqno = %s, beg.index = 0x%08x\n", isiSeqnoString(&beg.seqno, NULL), beg.index);
    end.index = isidlSearchDiskLoopForSeqno(&snapshot, &end.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
    LogMsg("end.seqno = %s, end.index = 0x%08x\n", isiSeqnoString(&end.seqno, NULL), end.index);

/* If we can't find the desired packets (disk loop overwritten?) then start with current data */

    if (!isiIsValidIndex(beg.index) || !isiIsValidIndex(end.index)) {
        LogMsg("unable to locate dl indices for HLP state data, default to start with current QDP data");
        history.nxtndx = snapshot.sys->index.yngest;
        history.endndx = snapshot.sys->index.yngest;
        if (!VerifyIndicies(&snapshot)) Exit(MY_MOD_ID + 8);
        qdplusSetLcqStateFlag(qhlp->qdplus, QDP_LCQ_STATE_PRODUCTION);
        LogMsg("going into production mode with nxtndx=0x%08x, endndx=0x%08x\n", history.nxtndx, history.endndx);
        return;
    }

/* We know where to find the data to backfill, set up reader accordingly */

    history.nxtndx = beg.index;
    history.endndx = end.index;
    if (!VerifyIndicies(&snapshot)) Exit(MY_MOD_ID + 9);

/* Backfill the LCQ */

    qdplusSetLcqStateFlag(qhlp->qdplus, QDP_LCQ_STATE_INITIALIZE);
    LogMsg("recovering state with data from indicies 0x%08x through 0x%08x", history.nxtndx, history.endndx);

    do {
        QuitOnShutdown(MY_MOD_ID + 9);
        ReadNextPacket(qhlp, &pkt);
        if (qdplusProcessPacket(qhlp->qdplus, &pkt) == NULL) {
            LogMsg("%s: qdplusProcessPacket failed: %s", fid, strerror(errno));
            Exit(MY_MOD_ID + 10);
        }
    } while (history.status == DATA_AVAILABLE);

    qdplusSetLcqStateFlag(qhlp->qdplus, QDP_LCQ_STATE_PRODUCTION);
    LogMsg("state recovery complete");
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: read.c,v $
 * Revision 1.8  2009/05/14 18:12:44  dechavez
 * added debug verbosity toggling via /usr/nrts/etc/debug
 *
 * Revision 1.7  2009/01/26 21:17:52  dechavez
 * switched to liblogio 2.4.1 calls, renamed instances of ISI_DL used with
 * isidlSnapshot for improved clarity
 *
 * Revision 1.6  2009/01/05 17:39:55  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.5  2008/12/15 23:29:36  dechavez
 * support corrupt or empty state files
 *
 * Revision 1.4  2007/09/14 19:51:31  dechavez
 * look in the proper disk loop for the save state flag
 *
 * Revision 1.3  2007/05/15 20:43:02  dechavez
 * use QuitOnShutdown() instead of ShutdownInProgress() test
 *
 * Revision 1.2  2007/05/11 16:20:53  dechavez
 * Check for shutdowns in pause for data loop
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
