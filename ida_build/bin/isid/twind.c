#pragma ident "$Id: twind.c,v 1.13 2008/10/10 22:47:24 dechavez Exp $"
/*======================================================================
 *
 *  Service a time window based data request where the packets come from
 *  the NRTS disk loop.
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_TWIND

#ifdef DEBUG

static VOID PrintTwindReq(FILE *fp, ISID_TWIND_REQUEST *twreq)
{
    fprintf(fp, "%s.%s.%s", twreq->name.sta, twreq->name.chn, twreq->name.loc);
    isiPrintReqTime(fp, twreq->beg);
    isiPrintReqTime(fp, twreq->end);
}

#endif /* DEBUG */

/* Identify available streams and init status fields */

static VOID InitStreamStatus(CLIENT *client, ISID_DATA_REQUEST *datreq)
{
UINT32 i;
ISID_TWIND_REQUEST *twreq;
static char *fid = "ProcessTwindRequest:InitStreamStatus";

/* Initialize status of each stream */

    for (i = 0; i < datreq->nreq; i++) {
        twreq = &datreq->req.twind[i];
        twreq->dl = isidlLocateNrtsDiskloop(client->master, &twreq->name, &twreq->nrts);
        if (twreq->dl != NULL) {
            twreq->count = 0;
            isiStreamNameString(&twreq->name, twreq->ident);
            if (twreq->end == ISI_KEEPUP) {
                twreq->continuous = TRUE;
                twreq->end = ISI_NEWEST;
            } else {
                twreq->continuous = FALSE;
            }
            client->finished = FALSE; /* at least one stream is available for transfer */
            twreq->status = ISID_WAITING_FOR_DATA;
        } else {
            twreq->status = ISID_DONE;
        }
    }
}

/* Check disk loop for index of next packet to send */

static VOID CheckDiskLoop(CLIENT *client, ISID_TWIND_REQUEST *twreq)
{
struct nrts_chn chn;
long newrec, oldest, yngest;
static char *fid = "ProcessTwindRequest:ProcessOneStream:CheckDiskLoop";

    if (client->finished) return;

    client->result = IACP_ALERT_NONE;

/* Get snapshot of current system status */

    chn = *twreq->nrts.chn;
    if (chn.status != NRTS_IDLE) return;

/* Update indices if we have already sent data for this channel */

    if (twreq->count > 0) {

    /* compare current state with what was there the last time through */

        if (chn.count < (unsigned long) twreq->count) {
            LogMsg(LOG_INFO, "%s: %s: UNEXPECTED ERROR 1: %ld < %ld", client->ident, fid, chn.count, twreq->count);
            client->result = IACP_ALERT_SERVER_FAULT;
            twreq->status = ISID_ABORT_REQUEST;
            client->finished = TRUE;
            return;
        }

        newrec = chn.count - twreq->count; /* number of records acquired since last pass */

        if (newrec > chn.hdr.nrec) {
            LogMsg(LOG_INFO, "%s: %s: DISKLOOP OVERRUN: %ld > %ld", client->ident, fid, newrec, chn.hdr.nrec);
            client->result = IACP_ALERT_SERVER_FAULT;
            twreq->status = ISID_ABORT_REQUEST;
            client->finished = TRUE;
            return;
        } else if (newrec < 0) {
            LogMsg(LOG_INFO, "%s: %s: UNEXPECTED ERROR 2: %ld < 0", client->ident, fid, newrec, chn.hdr.nrec);
            client->result = IACP_ALERT_SERVER_FAULT;
            twreq->status = ISID_ABORT_REQUEST;
            client->finished = TRUE;
            return;
        } else if (newrec == 0) {
            twreq->status = twreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE; /* nothing new acquired */
            return;
        } else {
            twreq->begndx = (twreq->endndx + 1) % chn.hdr.nrec; /* a few new records acquired */
            twreq->endndx = (twreq->begndx + newrec - 1) % chn.hdr.nrec;
            twreq->status = ISID_DATA_READY;
        }

/* Otherwise search the disk loop if there are any data in it */

    } else if (chn.count > 0) {

        oldest = chn.hdr.oldest;
        yngest = chn.hdr.yngest;

    /* Get first index */

        if (twreq->beg == ISI_OLDEST) {
            twreq->begndx = oldest;
        } else if (twreq->beg == ISI_NEWEST) {
            twreq->begndx = yngest;
        } else {
            twreq->begndx = nrtsSearchDiskloop(twreq->dl, &twreq->nrts, twreq->beg, oldest, -1, client->recv.buf, client->recv.buflen);
        }

        if (twreq->begndx < 0) {
            twreq->status = (twreq->continuous > 0) ? ISID_WAITING_FOR_DATA : ISID_DONE;
            return;
        }

    /* Get the last index */

        if (twreq->end == ISI_OLDEST) {
            twreq->endndx = oldest;
        } else if (twreq->end == ISI_NEWEST) {
            twreq->endndx = yngest;
        } else {
            twreq->endndx = nrtsSearchDiskloop(twreq->dl, &twreq->nrts, twreq->end, -1, yngest, client->recv.buf, client->recv.buflen);
        }

        if (twreq->endndx < 0) {
            twreq->status = ISID_DONE;
            return;
        }

/* or ignore this channel if the disk loop is empty */

    } else {
        twreq->status = twreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
        return;
    }

/* Initialize next index to read to the begining */

    twreq->nxtndx = twreq->begndx;

/* Save the current dl counter and mark the channel ready for xfer */

    twreq->count  = chn.count;
    twreq->status = ISID_DATA_READY;

    return;
}

static NRTS_PKT *PreviousPacket(CLIENT *client)
{
LNKLST_NODE *node;
NRTS_PKT *pkt;

    node = listFirstNode(&client->history);
    while (node != NULL) {
        pkt = (NRTS_PKT *) node->payload;
        if (isiStreamNameCompare(&pkt->isi.hdr.name, &client->pkt.isi.hdr.name) == 0) return pkt;
        node = listNextNode(node);
    }

    return NULL;
}

static void LogOtherStuff(NRTS_PKT *prev, CLIENT *client, ISID_TWIND_REQUEST *twreq)
{
char buf[512];
NRTS_PKT *crnt;

    crnt = &client->pkt; // to make the code easier to read

    LogMsg(LOG_INFO, "%s: prev hdr: %s\n", client->ident, isiGenericTsHdrString(&prev->isi.hdr, buf));
    LogMsg(LOG_INFO, "%s: crnt hdr: %s\n", client->ident, isiGenericTsHdrString(&crnt->isi.hdr, buf));
    LogMsg(LOG_INFO, "%s: begndx=%ld endndx=%ld nxtndx=%ld count=%ld", client->ident, twreq->begndx, twreq->endndx, twreq->nxtndx, twreq->count);
}

static void LogChn(CLIENT *client, NRTS_CHN *chn)
{
    LogMsg(LOG_INFO, "%s: count=%lu nrec=%ld hide=%lu oldest=%lu yngest=%lu lend=%lu", client->ident, chn->count, chn->hdr.nrec, chn->hdr.hide, chn->hdr.oldest, chn->hdr.yngest, chn->hdr.lend);
}

static void LogExtra(CLIENT *client, IDA_EXTRA *prev, IDA_EXTRA *crnt)
{
char buf[512];

    if (!prev->valid || !crnt->valid) return;

    LogMsg(LOG_INFO, "%s: prev extra: 0x%08x %s\n", client->ident, prev->seqno, utilLttostr(prev->tstamp, 1000, buf));
    LogMsg(LOG_INFO, "%s: crnt extra: 0x%08x %s\n", client->ident, crnt->seqno, utilLttostr(crnt->tstamp, 1000, buf));
}

static BOOL OverrunDetected(NRTS_PKT *prev, CLIENT *client, ISID_TWIND_REQUEST *twreq, NRTS_CHN *chn)
{
char buf[512];
NRTS_PKT *crnt;

    crnt = &client->pkt; // to make the code easier to read

    if (!prev->extra.valid || !crnt->extra.valid) return FALSE;

    if ((crnt->extra.seqno < prev->extra.seqno) && (crnt->extra.tstamp < prev->extra.tstamp)) {
        LogMsg(LOG_INFO, "%s: *** OVERRUN DETECTED ***, index=%ld", client->ident, twreq->nxtndx);
        LogExtra(client, &prev->extra, &crnt->extra);
        LogChn(client, chn);
        return TRUE;
    }

    return FALSE;
}

static BOOL SintChanged(NRTS_PKT *prev, CLIENT *client, ISID_TWIND_REQUEST *twreq, NRTS_CHN *chn)
{
REAL64 sint, sint2;
NRTS_PKT *crnt;

    crnt = &client->pkt; // to make the code easier to read

    sint  = isiSrateToSint(&prev->isi.hdr.srate);
    sint2 = isiSrateToSint(&crnt->isi.hdr.srate);

    if (sint != sint2) {
        LogMsg(LOG_INFO, "%s: *** %s SAMPLE INTERVAL CHANGE *** from %.3lf to %.3lf\n", client->ident, crnt->ident, sint, sint2);
        LogOtherStuff(prev, client, twreq);
        LogExtra(client, &prev->extra, &crnt->extra);
        LogChn(client, chn);
        return TRUE;
    }

    return FALSE;
}

static BOOL ReliableIda10ExternalTime(IDA10_CLOCK_STATUS *status)
{
    if (status->derived) return TRUE;

    if (status->suspect) return FALSE;
    if (!status->init) return FALSE;
    if (!status->avail) return FALSE;

    return TRUE;
}

static void Ida10ClockTests(NRTS_PKT *pkt, CLIENT *client)
{
UINT64 err;
int status, sign;
REAL64 errsec, errsmp;
IDA10_TSHDR *prev, *crnt;
BOOL HaveTimeTear;
static char *fid = "Ida10ClockTests";

    prev = &pkt->work.buf.ida10.hdr;
    crnt = &client->pkt.work.buf.ida10.hdr;
    status = ida10TtagIncrErr(prev, crnt, &sign, &err);
    if (status != 0) {
        LogMsg(LOG_INFO, "%s: ida10TtagIncrErr failed with status %d", fid, status);
        return;
    } else {
        errsec = (REAL64) err / (REAL64) NANOSEC_PER_SEC;
        errsmp = errsec / (REAL64) crnt->sint;
        HaveTimeTear = (int) errsmp == 0 ? FALSE : TRUE;
    }
//LogMsg(LOG_INFO, "%s: prev = %s\n", fid, ida10TSHDRtoString(prev, NULL));
//LogMsg(LOG_INFO, "%s: crnt = %s\n", fid, ida10TSHDRtoString(crnt, NULL));
//LogMsg(LOG_INFO, "%s: ReliableIda10ExternalTime(prev) = %s, errsmp = %d\n", fid, ReliableIda10ExternalTime(&prev->cmn.ttag.end.status)?"TRUE":"FALSE", (int) errsmp);
    if (ReliableIda10ExternalTime(&prev->cmn.ttag.end.status) && !HaveTimeTear) {
        crnt->tofs = prev->tols + prev->sint;
        crnt->cmn.ttag.beg.status.derived = TRUE;
        crnt->tols = crnt->tofs + ((crnt->nsamp - 1) * crnt->sint);
        crnt->cmn.ttag.end.status.derived = TRUE;
        client->pkt.isi.hdr.tofs.value = crnt->tofs;
        client->pkt.isi.hdr.tofs.status |= ISI_TSTAMP_STATUS_DERIVED;
        client->pkt.isi.hdr.tols.value = crnt->tols;
        client->pkt.isi.hdr.tols.status |= ISI_TSTAMP_STATUS_DERIVED;
//LogMsg(LOG_INFO, "%s: *** derived %s time stamps", client->ident, pkt->ident);
    }
}

static void DeviceSpecificClockTests(NRTS_PKT *prev, CLIENT *client, ISID_TWIND_REQUEST *twreq, NRTS_CHN *chn)
{
    switch (prev->orig.type) {
      case ISI_TYPE_IDA10:
        Ida10ClockTests(prev, client);
        break;
    }
}

static void NoteTimeTears(NRTS_PKT *prev, CLIENT *client, ISID_TWIND_REQUEST *twreq, NRTS_CHN *chn)
{
struct {
    INT32 samples;
    REAL64 seconds;
} ttear;
REAL64 sint, increment;
NRTS_PKT *crnt;
char buf[512];

    crnt = &client->pkt; // to make the code easier to read

    sint = isiSrateToSint(&crnt->isi.hdr.srate);

    increment = crnt->isi.hdr.tofs.value - prev->isi.hdr.tols.value;
    ttear.seconds = increment - sint;
    ttear.samples = (INT32) (ttear.seconds / sint);

    if (ttear.samples != 0) {
        LogMsg(LOG_INFO, "%s: %s time tear of %.3lf seconds (%ld samples)\n", client->ident, client->pkt.ident, ttear.seconds, ttear.samples);
        LogExtra(client, &prev->extra, &crnt->extra);
        LogOtherStuff(prev, client, twreq);
        LogChn(client, chn);
    }
}

static BOOL WeirdIndexingProblemCheck(CLIENT *client, ISID_TWIND_REQUEST *twreq, NRTS_CHN *chn)
{
NRTS_PKT *prev, new;

    if ((prev = PreviousPacket(client)) != NULL) {

        if (OverrunDetected(prev, client, twreq, chn)) return TRUE;
        if (SintChanged(prev, client, twreq, chn)) return TRUE;
        DeviceSpecificClockTests(prev, client, twreq, chn);
        NoteTimeTears(prev, client, twreq, chn);
        *prev = client->pkt;

    } else {
        new = client->pkt;
        listAppend(&client->history, (void *) &new, sizeof(NRTS_PKT));
    }

    return FALSE;
}

/* Read a packet from the disk loop, package/reformat it and send to client */

static VOID ReadAndForward(CLIENT *client, ISID_DATA_REQUEST *datreq, ISID_TWIND_REQUEST *twreq)
{
#define MAX_LAPPED_RETRY 20
#define LAPPED_RETRY_INTERVAL 250
BOOL lapped;
int retry;
NRTS_CHN chn;
static char *fid = "ReadAndForward";

    client->result = IACP_ALERT_NONE;

/* Read the specified packet from the disk loop */

    lapped = FALSE;
    retry  = 0;
    do {
        if (lapped) utilDelayMsec(LAPPED_RETRY_INTERVAL);
        client->pkt.indx = -1;
        if (nrtsRead(twreq->dl, &twreq->nrts, twreq->nxtndx, client->recv.buf, client->recv.buflen, &client->pkt, &chn) < 0) {
            LogMsg(LOG_INFO, "%s: nrtsRead: %s: %s", client->ident, twreq->ident, strerror(errno));
            client->result = IACP_ALERT_SERVER_FAULT;
            return;
        }
//        lapped = WeirdIndexingProblemCheck(client, twreq, &chn);
    } while (lapped && ++retry <= MAX_LAPPED_RETRY);

    if (lapped) {
        LogMsg(LOG_INFO, "%s: WARNING: still lapped after %d attempts", client->ident, retry);
    } else if (retry != 0) {
        LogMsg(LOG_INFO, "%s: NOTICE: lapped recovered after %d attempts", client->ident, retry);
    }

/* Reformat into an IACP_FRAME */

    if (!lapped) {
        if (!ConvertData(client, datreq, &client->pkt)) {
            LogMsg(LOG_INFO, "%s: unable to build %s IACP frame!", client->ident, twreq->ident);
            client->result = IACP_ALERT_SERVER_FAULT;
            return;
        }

/* Send to client */

        if (iacpSendFrame(client->iacp, &client->send.frame)) {
            ; /* maybe update some stats later */
        } else {
            if (errno = EPIPE) {
                errno = ECONNRESET;
                client->brkrcvd = TRUE;
            }
            LogMsg(LOG_INFO, "%s: %s: iacpSendFrame: %s", client->ident, twreq->ident, strerror(errno));
            client->result = IACP_ALERT_IO_ERROR;
            return;
        }
    }

/* Update indices for the next pass through */

    if (twreq->nxtndx == twreq->endndx) {
        twreq->status = twreq->continuous ? ISID_WAITING_FOR_DATA : ISID_DONE;
    } else {
        twreq->nxtndx = (twreq->nxtndx + 1) % twreq->nrts.chn->hdr.nrec;
    }
}

static VOID ProcessOneStream(CLIENT *client, ISID_DATA_REQUEST *datreq, ISID_TWIND_REQUEST *twreq)
{
static char *fid = "ProcessOneStream";

/* Nothing to do here if data for this particular stream is complete */

    if (twreq->status == ISID_DONE) return;

/* if presently waiting for data, see if anything new has arrived */

    if (twreq->status == ISID_WAITING_FOR_DATA) CheckDiskLoop(client, twreq);
    if (client->result != IACP_ALERT_NONE) {
        client->finished = TRUE;
        return;
    }

/* if this stream has a packet ready to send, then do so */

    if (twreq->status == ISID_DATA_READY) {
        ReadAndForward(client, datreq, twreq);
        if (client->result != IACP_ALERT_NONE) {
            client->finished = TRUE;
            return;
        }
    }

/* update the flags for the next pass */

    if (twreq->status == ISID_DATA_READY) client->dataready = TRUE;
}

/* Send all currently available data */

static VOID SendAvailableData(CLIENT *client, ISID_DATA_REQUEST *datreq)
{
UINT32 i;
static char *fid = "ServiceTwindRequest:SendAvailableData";

    if (client->finished) return;

/* Send everything (if anything) that is available to send right now */

    do {
        client->dataready = FALSE;
        for (i = 0; !client->finished && i < datreq->nreq; i++) ProcessOneStream(client, datreq, &datreq->req.twind[i]);
    } while (!client->finished && client->dataready);

    if (client->finished) return;

/* We are not done if at least one stream is still waiting on data to send */

    for (client->finished=TRUE, i = 0; client->finished && i < datreq->nreq; i++) {
        if (datreq->req.twind[i].status == ISID_WAITING_FOR_DATA) client->finished = FALSE;
    }

    if (client->finished && client->result == IACP_ALERT_NONE) {
        LogMsg(LOG_INFO, "%s: request complete", client->ident);
        client->result = IACP_ALERT_REQUEST_COMPLETE;
    }
}

static BOOL ExpandRequest(ISI_DL_MASTER *master, LNKLST *input, ISI_DATA_REQUEST *output)
{
INT32 count;
LNKLST expanded;
LNKLST_NODE *crnt;
static char *fid = "ProcessTwindRequest:ExpandRequest";

    if (input->count == 0) return TRUE;

    if (!listInit(&expanded)) {
        LogMsg(LOG_INFO, "%s: listInit: %s", fid, strerror(errno));
        return FALSE;
    }

    crnt = listFirstNode(input);
    while (crnt != NULL) {
        count = isidlExpandTwindRequest(master, (ISI_TWIND_REQUEST *) crnt->payload, &expanded);
        if (count < 0) {
            LogMsg(LOG_INFO, "%s: isidlExpandTwindRequest: error %d: %s", fid, count, strerror(errno));
            return FALSE;
        }
        crnt = listNextNode(crnt);
    }

    if (!isiCopyTwindListToDataRequest(&expanded, output)) {
        LogMsg(LOG_INFO, "%s: isiCopyTwindListToDataRequest: %s", fid, strerror(errno));
        return FALSE;
    }
    listDestroy(&expanded);

    return TRUE;
}

/* Time window based data request */

void ProcessTwindRequest(CLIENT *client)
{
ISID_DATA_REQUEST datreq;
static char *fid = "ProcessTwindRequest";

/* Make sure general request parameters are OK */

    switch (client->datreq.format) {
      case ISI_FORMAT_GENERIC:
      case ISI_FORMAT_NATIVE:
        break;
      default:
        LogMsg(LOG_INFO, "%s: unknown format code %lu", client->ident, client->datreq.format);
        client->result = IACP_ALERT_UNSUPPORTED;
        return;
    }

/* Expand any wildcards in the request list */

    if (!ExpandRequest(client->master, &client->incoming.list, &client->datreq)) {
        LogMsg(LOG_INFO, "%s: %s: ExpandRequest failed", client->ident, fid);
        client->result = IACP_ALERT_SERVER_FAULT;
        return;
    }

/* Copy expanded request into server side format */
    
    if (!CopyIntoServerSideFormat(&datreq, &client->datreq)) {
        LogMsg(LOG_INFO, "%s: %s: CopyIntoServerSideFormat: %s", client->ident, fid, strerror(errno));
        client->result = IACP_ALERT_SERVER_FAULT;
        return;
    }
    InitStreamStatus(client, &datreq);
    LogMsg(LOG_INFO, "%s: ISI time-based data request (%d streams)", client->ident, datreq.nreq);

/* Send a copy of the expanded request back to the client */

    if (!isiIacpSendDataRequest(client->iacp, &client->datreq)) {
        LogMsg(LOG_INFO, "%s: isiIacpSendDataRequest failed: %s", client->ident, strerror(errno));
        client->result = IACP_ALERT_IO_ERROR;
        client->finished = TRUE;
    }

/* Send the data */

    while (!client->finished) {
        MaintainHeartbeat(client);
        SendAvailableData(client, &datreq);
        PauseForNewData(client);
    }

    ClearDataRequest(&datreq);
}

/* Revision History
 *
 * $Log: twind.c,v $
 * Revision 1.13  2008/10/10 22:47:24  dechavez
 * initial support for legacy NRTS service
 *
 * Revision 1.12  2008/03/05 23:11:39  dechavez
 * commented out call to WeirdIndexingProblemCheck() (memory leak)
 *
 * Revision 1.11  2008/01/25 22:11:15  dechavez
 * removed options from client
 *
 * Revision 1.10  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.9  2007/06/21 22:51:16  dechavez
 * Increment time stamps in generic packets by sample interval, if there are
 * no time tears between IDA10 packets
 *
 * Revision 1.8  2007/06/14 22:00:16  dechavez
 * Cleaned up WeirdIndexingProblemCheck() and associated code.  It _should_
 * no longer be needed now that the nrtsRead race condition has been found
 * and fixed, however it doesn't hurt to keep it in.
 *
 * Revision 1.7  2007/05/17 22:22:31  dechavez
 * 3.1.0 (beta 1)
 *
 * Revision 1.6  2007/03/26 19:48:56  dechavez
 * immediately cease trying to send data on first instance of iacpSendFrame
 * failure (was "lingering" until the heartbeat thread would bring things down)
 *
 * Revision 1.5  2007/02/08 19:20:42  dechavez
 * corrected infinite loop bug introduced in previous fix (aap)
 *
 * Revision 1.4  2007/02/06 19:22:15  dechavez
 * Fixed hanging ServiceThreads on abnormal disconnect (ie, timeout)
 *
 * Revision 1.3  2007/02/06 00:15:48  dechavez
 * fixed bug causing first channel in a request to be ignored, and bug
 * causing duplicate packets instead of new channel packets when dl indices
 * matched between streams
 *
 * Revision 1.2  2007/01/11 22:02:18  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.1  2007/01/11 17:57:40  dechavez
 * initial release as twind.c (used to be stream.c)
 *
 */
