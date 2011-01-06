#pragma ident "$Id: seqno.c,v 1.25 2009/03/20 21:04:18 dechavez Exp $"
/*======================================================================
 *
 *  Service a seqno based data request where the packets come from
 *  the ISI raw disk loop
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isid.h"

#define MY_MOD_ID ISID_MOD_SEQNO

static BOOL OkToSend(ISID_DATA_REQUEST *datreq, ISI_RAW_PACKET *raw, ISI_DL *dl)
{
IDA *ida;
LNKLST_NODE *crnt;
ISI_STREAM_NAME name;
static char *fid = "OkToSend";

    if (datreq->slist->count == 0) return TRUE; /* no list present */

    ida = dl->nrts == NULL ? NULL : dl->nrts->ida;
    if (isiRawPacketStreamName(&name, raw, ida) == NULL) return TRUE;
    strlcpy(name.sta, dl->sys->site, ISI_STALEN+1);

    crnt = listFirstNode(datreq->slist);
    while (crnt != NULL) {
        if (isiStreamNameCompare(&name, (ISI_STREAM_NAME *) crnt->payload) == 0) return TRUE;
        crnt = listNextNode(crnt);
    }

    return FALSE;
}

/* Identify available streams and init status fields */

static VOID CheckRequest(CLIENT *client, ISID_DATA_REQUEST *datreq)
{
UINT32 i;
ISID_SEQNO_REQUEST *sreq;
char tmp1[128], tmp2[128];
static char *fid = "CheckRequest";

/* Verify disk loop for requested site exists */

    for (i = 0; i < datreq->nreq; i++) {
        sreq = &datreq->req.seqno[i];
        if ((sreq->dl = LocateDiskLoopBySite(client->master, sreq->site)) == NULL) {
            LogMsg(LOG_DEBUG, "%s: %s disk loop not found", fid, sreq->site);
            sreq->status = ISID_DONE;
        } else {
            LogMsg(LOG_DEBUG, "%s: site=%s beg=%s end=%s", fid, sreq->site, isiSeqnoString(&sreq->beg, tmp1), isiSeqnoString(&sreq->end, tmp2));
            if (sreq->beg.signature == ISI_CURRENT_SEQNO_SIG) sreq->beg.signature = sreq->dl->sys->seqno.signature;
            if (sreq->end.signature == ISI_CURRENT_SEQNO_SIG) sreq->end.signature = sreq->dl->sys->seqno.signature;
            sreq->count = 0;
            sreq->status = ISID_WAITING_FOR_DATA;
            LogMsg(LOG_DEBUG, "%s: %s disk loop present", fid, sreq->site);
            if (isiIsKeepupSeqno(&sreq->end)) {
                sreq->continuous = TRUE;
                sreq->end = ISI_NEWEST_SEQNO;
            } else {
                sreq->continuous = FALSE;
            }
            client->finished = FALSE; /* at least one stream is available for transfer */
        }
    }
}

/* check for new activity by comparing counters */

static UINT32 CompareCounters(CLIENT *client, ISI_DL *snapshot, ISID_SEQNO_REQUEST *sreq)
{
UINT64 now, then;
UINT32 newrec;

    now = snapshot->sys->count;
    then = sreq->count;

    if (now == then) return 0;

    if (now  < then) {
        LogMsg(LOG_ERR, "%s: DISKLOOP REVERTED: now=%llu then=%llu %lu", client->ident, now, then);
        client->result = IACP_ALERT_SERVER_FAULT;
        client->finished = TRUE;
        return 0;
    }

    if ((newrec = (UINT32) (now - then)) > snapshot->sys->numpkt) {
        LogMsg(LOG_ERR, "%s: DISKLOOP OVERRUN: now=%llu then=%llu numpkt=%lu", client->ident, now, then, snapshot->sys->numpkt);
        client->result = IACP_ALERT_SERVER_FAULT;
        client->finished = TRUE;
        return 0;
    }

    return newrec;
}

/* Search disk loop for specific packets */

static int SearchForIndicies(ISI_DL *snapshot, ISID_SEQNO_REQUEST *sreq)
{
    sreq->begndx = isidlSearchDiskLoopForSeqno(snapshot, &sreq->beg, snapshot->sys->index.oldest, ISI_UNDEFINED_INDEX);

    if (sreq->begndx == ISI_CORRUPT_INDEX) {
        return ISID_CORRUPT_DISK_LOOP;
    } else if (sreq->begndx == ISI_UNDEFINED_INDEX) {
        return sreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
    }

    sreq->endndx = isidlSearchDiskLoopForSeqno(snapshot, &sreq->end, ISI_UNDEFINED_INDEX, snapshot->sys->index.yngest);

    if (sreq->endndx == ISI_CORRUPT_INDEX) {
        return ISID_CORRUPT_DISK_LOOP;
    } else {
        return (sreq->endndx == ISI_UNDEFINED_INDEX) ? ISID_DONE : ISID_DATA_READY;
    }
}

/* Set the dl indices for which packets to send */

static int CheckDiskLoop(CLIENT *client, ISID_SEQNO_REQUEST *sreq)
{
ISI_DL snapshot;
ISI_DL_SYS sys;
UINT32 newrec;
static char *fid = "ServiceSeqnoRequest:SendAvailableData:CheckDiskLoop";

    client->result = IACP_ALERT_NONE;

/* Get snapshot of the disk loop header */

    if (!isidlSnapshot(sreq->dl, &snapshot, &sys)) {
        LogMsg(LOG_ERR, "%s: isiSnapshot failed: %s", fid, strerror(errno));
        client->result = IACP_ALERT_SERVER_FAULT;
        return -1;
    }

/* Update indices if we have already xferd data for this site */

    if (sreq->count > 0) {

    /* compare counter with what was there the last time through */

        newrec = CompareCounters(client, &snapshot, sreq);
        if (client->result == IACP_ALERT_SERVER_FAULT) return -2;

        if (newrec > 0) {
            sreq->begndx = (sreq->endndx + 1) % snapshot.sys->numpkt;
            sreq->endndx = (sreq->begndx + newrec - 1) % snapshot.sys->numpkt;
            sreq->status = ISID_DATA_READY;
        } else {
            sreq->status = sreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
        }

/* Otherwise search the disk loop if there are any data in it */

    } else if (snapshot.sys->count > 0) {

        sreq->status = SearchForIndicies(&snapshot, sreq);

    /* Get the last index */

/* or ignore this site if the disk loop is empty */

    } else {
        sreq->status = sreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
    }

/* Bail if we've got a corrupt disk loop */

    if (sreq->status == ISID_CORRUPT_DISK_LOOP) {
        LogMsg(LOG_ERR, "ERROR: corrupt %s disk loop detected! %s", snapshot.sys->site);
        client->result = IACP_ALERT_SERVER_FAULT;
        return -3;
    }

/* Set the transfer start index if we've got something to send */

    if (sreq->status == ISID_DATA_READY) {
        sreq->nxtndx = sreq->begndx;
        sreq->count  = snapshot.sys->count;
    }

    return 0;
}

/* Read a packet from the disk loop, package/reformat it and send to client */

#define COMPBUFLEN(raw) (raw->hdr.len.used + (raw->hdr.len.used / 10) + 12)

static VOID ReadAndForward(CLIENT *client, ISID_DATA_REQUEST *datreq, ISID_SEQNO_REQUEST *sreq)
{
BOOL SentOK;
int cbuflen;
IACP_FRAME *frame;
ISI_RAW_PACKET *raw;
UINT8 *uncompressedBuf = NULL, *compressedBuf = NULL;
static char *fid = "ReadAndForward";

    client->result = IACP_ALERT_NONE;

/* Find the packet in the disk loop */

    if ((raw = isiAllocateRawPacket(sreq->dl->sys->maxlen)) == NULL) {
        LogMsg(LOG_ERR, "%s: isiAllocateRawPacket: %s", fid, strerror(errno));
        client->result = IACP_ALERT_SERVER_FAULT;
        client->finished = TRUE;
        return;
    }
    uncompressedBuf = raw->payload;

    if (!isidlReadDiskLoop(sreq->dl, raw, sreq->nxtndx)) {
        LogMsg(LOG_ERR, "%s: isidlReadDiskLoop failed for index %d: %s", fid, sreq->nxtndx, strerror(errno));
        client->result = IACP_ALERT_SERVER_FAULT;
        client->finished = TRUE;
        return;
    }

/* If using stream list, compress and send only if on the list */

    if (OkToSend(datreq, raw, sreq->dl)) {

    /* Compress */

        if (datreq->compress != ISI_COMP_NONE) {
            cbuflen = COMPBUFLEN(raw);
            if ((compressedBuf = (UINT8 *) malloc(cbuflen)) == NULL) {
                LogMsg(LOG_ERR, "%s: malloc: %s", fid, strerror(errno));
                client->result = IACP_ALERT_SERVER_FAULT;
                client->finished = TRUE;
                return;
            }
            if (!isiCompressRawPacket(raw, compressedBuf, cbuflen)) {
                LogMsg(LOG_WARN, "%s: isiCompressRawPacket: %s", isiRawPacketString(raw, client->temp.buf), strerror(errno));
            }
        }

    /* Package it into an IACP_FRAME */

        frame = &client->send.frame;
        frame->payload.type = ISI_IACP_RAW_PKT;
        frame->payload.data = client->temp.buf;
        frame->payload.len = isiPackRawPacket(frame->payload.data,  raw);

    /* Send to client */

        SentOK = iacpSendFrame(client->iacp, &client->send.frame);
        if (!SentOK && errno == EPIPE) {
            errno = ECONNRESET;
            client->brkrcvd = TRUE;
            client->finished = TRUE;
        }
    } else {
        SentOK = TRUE; /* dropped by server same as sent OK */
    }

/* Free temporary memory */

    if (compressedBuf != NULL) free(compressedBuf);
    free(uncompressedBuf);
    raw->payload = NULL; /* since it was either compressedBuf or uncompressedBuf */
    isiDestroyRawPacket(raw);

/* Quit if we failed to send the frame */

    if (!SentOK) {
        LogMsg(LOG_INFO, "%s: %s: iacpSendFrame: %s", client->ident, sreq->site, strerror(errno));
        client->result = IACP_ALERT_IO_ERROR;
        client->finished = TRUE;
        return;
    }

/* Update indices for the next pass through */

    if (sreq->nxtndx == sreq->endndx) {
        sreq->status = sreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
    } else {
        sreq->nxtndx = (sreq->nxtndx + 1) % sreq->dl->sys->numpkt;
    }
}

static VOID ProcessOneSite(CLIENT *client, ISID_DATA_REQUEST *datreq, ISID_SEQNO_REQUEST *sreq)
{

/* Nothing to do here if data for this particular stream is complete */

    if (sreq->status == ISID_DONE) return;

/* if presently waiting for data, see if anything new has arrived */

    if (sreq->status == ISID_WAITING_FOR_DATA) CheckDiskLoop(client, sreq);
    if (client->result != IACP_ALERT_NONE) {
        client->finished = TRUE;
        return;
    }

/* if this site has a packet ready to send, then do so */

    if (sreq->status == ISID_DATA_READY) ReadAndForward(client, datreq, sreq);
    if (client->result != IACP_ALERT_NONE) {
        client->finished = TRUE;
        return;
    }

/* update the flags for the next pass */

    if (sreq->status == ISID_DATA_READY) client->dataready = TRUE;
}

/* Send all currently available data */

static VOID SendAvailableData(CLIENT *client, ISID_DATA_REQUEST *datreq)
{
UINT32 i;
ISID_SEQNO_REQUEST *sreq;
static char *fid = "SendData";

    if (client->finished) return;

/* Send everything (if anything) that is available to send right now */

    do {
        client->dataready = FALSE;
        for (i = 0; !client->finished && i < (int) datreq->nreq; i++) ProcessOneSite(client, datreq, &datreq->req.seqno[i]);
    } while (!client->finished && client->dataready);

    if (client->finished) return;

/* We are not done if at least one site is still waiting on data to send */

    for (client->finished=TRUE, i = 0; client->finished && i < datreq->nreq; i++) {
        if (datreq->req.seqno[i].status == ISID_WAITING_FOR_DATA) client->finished = FALSE;
    }

    if (client->finished && client->result == IACP_ALERT_NONE) {
        LogMsg(LOG_INFO, "%s: data request complete", client->ident);
        client->result = IACP_ALERT_REQUEST_COMPLETE;
    }
}

static BOOL ExpandRequest(ISI_DL_MASTER *master, LNKLST *input, ISI_DATA_REQUEST *output)
{
INT32 count;
LNKLST expanded;
LNKLST_NODE *crnt;
static char *fid = "ProcessSeqnoRequest:ExpandRequest";

    if (input->count == 0) return TRUE;

    if (!listInit(&expanded)) {
        LogMsg(LOG_ERR, "%s: listInit: %s", fid, strerror(errno));
        return FALSE;
    }

    crnt = listFirstNode(input);
    while (crnt != NULL) {
        count = isidlExpandSeqnoRequest(master, (ISI_SEQNO_REQUEST *) crnt->payload, &expanded);
        if (count < 0) {
            LogMsg(LOG_ERR, "%s: isidlExpandSeqnoRequest: error %d: %s", fid, count, strerror(errno));
            return FALSE;
        }
        crnt = listNextNode(crnt);
    }

    if (!isiCopySeqnoListToDataRequest(&expanded, output)) {
        LogMsg(LOG_ERR, "%s: isiCopySeqnoListToDataRequest: %s", fid, strerror(errno));
        return FALSE;
    }
    listDestroy(&expanded);

    return TRUE;
}

/* Seqno based data request thread */

void ProcessSeqnoRequest(CLIENT *client)
{
ISID_DATA_REQUEST datreq;
static char *fid = "ProcessSeqnoRequest";

    InitDataRequest(&datreq);

/* Expand any wildcards in the request list */

    if (!ExpandRequest(client->master, &client->incoming.list, &client->datreq)) {
        LogMsg(LOG_ERR, "%s: %s: ExpandRequest failed", client->ident, fid);
        client->result = IACP_ALERT_SERVER_FAULT;
    }

/* Copy expanded request into server side format */
    
    if (!CopyIntoServerSideFormat(&datreq, &client->datreq)) {
        LogMsg(LOG_ERR, "%s: %s: CopyIntoServerSideFormat: %s", client->ident, fid, strerror(errno));
        client->result = IACP_ALERT_SERVER_FAULT;
        ClearDataRequest(&datreq);
        return;
    }
    if (datreq.slist->count == 0) {
        LogMsg(LOG_INFO, "%s: ISI seqno data request (%d sites, all available streams)", client->ident, datreq.nreq);
    } else {
        LogMsg(LOG_INFO, "%s: ISI seqno data request (%d sites, %lu streams)", client->ident, datreq.nreq, datreq.slist->count);
    }

/* Send a copy of the expanded request back to the client */

    if (!isiIacpSendDataRequest(client->iacp, &client->datreq)) {
        LogMsg(LOG_WARN, "%s: isiIacpSendDataRequest failed: %s", client->ident, strerror(errno));
        client->result = IACP_ALERT_IO_ERROR;
        client->finished = TRUE;
        ClearDataRequest(&datreq);
        return;
    }

/* Process the data request */

    CheckRequest(client, &datreq);

    while (!client->finished) {
        MaintainHeartbeat(client);
        SendAvailableData(client, &datreq);
        PauseForNewData(client);
    }

    ClearDataRequest(&datreq);
}

/* Revision History
 *
 * $Log: seqno.c,v $
 * Revision 1.25  2009/03/20 21:04:18  dechavez
 * removed DestroySnapshot (mutex no longer involved)
 *
 * Revision 1.24  2009/02/23 22:02:12  dechavez
 * Use DestroySnapshot to deal avoid isidlSnapshot memory leak
 *
 * Revision 1.23  2009/02/04 17:54:14  dechavez
 * Added additional debug messages
 *
 * Revision 1.22  2009/01/26 21:03:23  dechavez
 * renamed snapshot ISI_DL instances to "snapshot", for clarity
 *
 * Revision 1.21  2009/01/06 20:49:49  dechavez
 * Fixed missing return value problem in CompareCounters().
 *
 * Revision 1.20  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.19  2008/02/07 19:54:18  dechavez
 * initialize local datreq structure in ProcessSeqnoRequest()
 *
 * Revision 1.18  2008/01/25 22:12:24  dechavez
 * removed options from client, now use new data request slist field for optional stream control
 *
 * Revision 1.17  2008/01/17 19:39:46  dechavez
 * removed dead code
 *
 * Revision 1.16  2008/01/16 23:39:23  dechavez
 * fixed ISID_OPTION_FOLLOW_NRTS bug when serving compressed output data
 *
 * Revision 1.15  2008/01/15 23:30:36  dechavez
 * ISID_OPTION_FOLLOW_NRTS support added
 *
 * Revision 1.14  2007/03/26 19:48:56  dechavez
 * immediately cease trying to send data on first instance of iacpSendFrame
 * failure (was "lingering" until the heartbeat thread would bring things down)
 *
 * Revision 1.13  2007/02/08 19:19:48  dechavez
 * Fixed hanging ServiceThreads on abnormal disconnect (ie, timeout)
 *
 * Revision 1.12  2007/01/11 22:02:18  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.11  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.10  2006/09/29 19:58:02  dechavez
 * Check for heartbeat flag in main loop.  Eliminated memory leak when
 * thread exiting after failed sends
 *
 * Revision 1.9  2006/02/14 17:04:58  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.8  2006/02/09 20:23:56  dechavez
 * support for new pollint client parameter
 *
 * Revision 1.7  2005/09/30 22:40:21  dechavez
 * Check for client termination request at top of SendData do loop instead of
 * outside it (so we don't linger until all data have been sent).  Fixed core
 * dump bug (%s instead of %d) in log message.
 *
 * Revision 1.6  2005/09/14 23:44:43  dechavez
 * fixed problem of freeing same memory twice when cleaning up in ReadAndForward()
 *
 * Revision 1.5  2005/09/10 21:54:29  dechavez
 * use isiReadDiskLoop to load dynamically allocated/destroyed raw packet
 *
 * Revision 1.4  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.3  2005/07/06 15:51:43  dechavez
 * now with working compression support
 *
 * Revision 1.2  2005/06/30 01:40:29  dechavez
 * first working version, no compression
 *
 * Revision 1.1  2005/06/24 22:01:57  dechavez
 * initial release
 *
 */
