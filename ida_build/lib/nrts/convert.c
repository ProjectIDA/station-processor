#pragma ident "$Id: convert.c,v 1.11 2008/02/08 16:04:04 akimov Exp $"
/*======================================================================
 *
 *  Reblock non-IDA packets into IDA10
 *
 *====================================================================*/
#include "nrts/dl.h"

#define FIXED_NSAMP (IDA10_DEFDATALEN / sizeof(UINT32))
#define GENTAG_OFFSET 10

typedef struct {
    LISS_FSDH fsdh;                  /* input MiniSEED header */
    REAL64 tons;                     /* time of next sample */
    REAL64 sint;                     /* sample interval in seconds */
    UINT8 packet[IDA10_FIXEDRECLEN]; /* holds the IDA10.5 packet under construction */
    int nsamp;                       /* number of data samples currently in ida10 packet */
} CONVERSION_WORKSPACE;

/* Fill in static parts of the IDA10.5 header */

static void InitIda10Header(UINT8 *start, LISS_FSDH *fsdh)
{
UINT8 *ptr, descriptor;
IDA10_GENTAG dummy;
char chnloc[IDA10_CNAMLEN+1];

    ptr = start;

    /* 10.5 common header */

    ptr += utilPackBytes(ptr, (UINT8 *) "TS", 2);
    *ptr++ = 10; /* format 10 */
    *ptr++ = 5;  /* subformat 5 */
    ptr += utilPackBytes(ptr, fsdh->staid, IDA105_SNAME_LEN);
    ptr += utilPackBytes(ptr, fsdh->netid, IDA105_NNAME_LEN);
    ptr += ida10PackGenericTtag(ptr, &dummy); /* this will get overwritten each time */
    ptr += IDA105_RESERVED_BYTES;
    ptr += utilPackUINT16(ptr, IDA10_FIXED_NBYTES);

    /* end of common header */

    /* stream name, a concatenation of channel and location codes */

    sprintf(chnloc, "%s%s", fsdh->chnid, fsdh->locid);
    utilTrimString(chnloc);
    util_lcase(chnloc);

    ptr += utilPackBytes(ptr, chnloc, IDA10_CNAMLEN);

    /* data format/status descriptor */

    descriptor = 0;
    descriptor |= IDA10_COMP_NONE;
    descriptor |= IDA10_A2D_24;
    descriptor |= IDA10_MASK_INT32;
    *ptr++ = descriptor;

    /* conversion gain */

    *ptr++ = 1;

    /* number of samples */

    ptr += utilPackUINT16(ptr, FIXED_NSAMP);

    /* sample rate */

    ptr += utilPackINT16(ptr, fsdh->srfact);
    ptr += utilPackINT16(ptr, fsdh->srmult);

}

/* Start a building a new packet (static parts of IDA10 header are already done) */

static void StartNewPacket(CONVERSION_WORKSPACE *new, LISS_FSDH *fsdh, REAL64 tons)
{
IDA10_GENTAG ttag;

    new->fsdh = *fsdh;
    new->nsamp = 0;
    new->sint = lissSint(fsdh);
    new->tons = new->fsdh.start = tons;

    ttag.tstamp = (INT64) ((tons - (REAL64) SAN_EPOCH_TO_1970_EPOCH) * (REAL64) NANOSEC_PER_SEC);
    ttag.status.receiver = ttag.status.generic = 0;

    if (fsdh->ioclck & LISS_IOC_CLOCK_LOCKED) ttag.status.generic |= IDA10_GENTAG_LOCKED;
    if (fsdh->qual & LISS_DQ_TIME_SUSPECT) ttag.status.generic |= IDA10_GENTAG_SUSPICIOUS;

    ida10PackGenericTtag(&new->packet[GENTAG_OFFSET], &ttag);
}

/* Make sure sample interval is unchanged between packets */

static BOOL VerifySampleInterval(LISS_FSDH *crnt, LISS_FSDH *next)
{
    if (crnt->srfact != next->srfact) return FALSE;
    if (crnt->srmult != next->srmult) return FALSE;
    
    return TRUE;
}

/* Locate conversion workspace for this stream, if it exists */

static BOOL StreamMatch(LISS_FSDH *a, LISS_FSDH *b)
{
    if (strcmp(a->staid, b->staid) != 0) return FALSE;
    if (strcmp(a->chnid, b->chnid) != 0) return FALSE;
    if (strcmp(a->locid, b->locid) != 0) return FALSE;

    return TRUE;
}

static CONVERSION_WORKSPACE *LocateBuffer(LNKLST *head, LISS_FSDH *fsdh)
{
LNKLST_NODE *crnt;
CONVERSION_WORKSPACE *work;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        work = (CONVERSION_WORKSPACE *) crnt->payload;
        if (StreamMatch(fsdh, &work->fsdh)) return work;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

/* Create a new conversion workspace */

static CONVERSION_WORKSPACE *CreateBuffer(LNKLST *head, LISS_FSDH *fsdh)
{
UINT8 *ptr;
CONVERSION_WORKSPACE new;

    InitIda10Header(new.packet, fsdh);
    StartNewPacket(&new, fsdh, fsdh->start);

    if (!listAppend(head, &new, sizeof(CONVERSION_WORKSPACE))) return NULL;

    return LocateBuffer(head, fsdh);
}

/* Get conversion workspace for this stream, creating a new one if required */

static CONVERSION_WORKSPACE *GetBuffer(LNKLST *head, LISS_FSDH *fsdh)
{
CONVERSION_WORKSPACE *work;

    if ((work = LocateBuffer(head, fsdh)) != NULL) return work;
    return CreateBuffer(head, fsdh);
}

/* Convert MiniSEED into IDA10.5 */

void nrtsMSEEDToIDA10DiskLoop(NRTS_DL *dl, UINT8 *raw)
{
int i, smperr, j;
REAL64 incerr;
LISS_PKT pkt;
INT32 *data;
CONVERSION_WORKSPACE *work;
static char *fid = "nrtsMSEEDToIDA10DiskLoop";

/* Silently ignore garbage calls */

    if (dl == NULL || raw == NULL) return;

/* Do nothing if conversion is not enabled */

    if (dl->ida10 == NULL) return;

/* Ignore non-data packets */

    lissUnpackMiniSeed(&pkt, raw, LISS_OPTION_DECODE);
    if (pkt.status != LISS_PKT_OK) return;

/* Get conversion workspace for this stream */

    if ((work = GetBuffer(dl->ida10, &pkt.fsdh)) == NULL) {
        logioMsg(dl->lp, LOG_INFO, "%s: ERROR - workspace failure: %s", fid, strerror(errno));
        logioMsg(dl->lp, LOG_INFO, "%s: ERROR - auto-conversion disabled", fid);
        listDestroy(dl->ida10);
        dl->ida10 = NULL;
        return;
    }

/* Sanity checks */

    if (!VerifySampleInterval(&work->fsdh, &pkt.fsdh)) {
        logioMsg(dl->lp, LOG_INFO, "%s: WARNING - sample interval change %s.%s.%s", fid, pkt.fsdh.staid, pkt.fsdh.chnid, pkt.fsdh.locid);
        StartNewPacket(work, &pkt.fsdh, pkt.fsdh.start);
    } else {
        incerr = pkt.fsdh.start - work->tons;
        if (fabs(incerr) > work->sint) {
            smperr = (int) (incerr / work->sint);
            logioMsg(dl->lp, LOG_INFO, "%s: WARNING - %d sample %s.%s.%s. time tag increment error", fid, smperr, pkt.fsdh.staid, pkt.fsdh.chnid, pkt.fsdh.locid);
            StartNewPacket(work, &pkt.fsdh, pkt.fsdh.start);
        } else {
            work->tons = pkt.fsdh.start; /* sync to current packet to avoid accumulating one sample drift errors */
        }
    }

/* Build IDA10 packet, flush to disk loop when full */

    data = (INT32 *) &work->packet[IDA10_FIXEDHDRLEN];
    for (i = 0; i < pkt.fsdh.nsamp; i++) {
        data[work->nsamp++] = pkt.data[i];
        work->tons += work->sint;
        if (work->nsamp == FIXED_NSAMP) {
            for(j=0; j<FIXED_NSAMP; ++j) utilPackINT32((UINT8 *) &data[j], data[j]);
            if (!nrtsWriteToDiskLoop(dl, work->packet)) {
                logioMsg(dl->lp, LOG_INFO, "%s: nrtsWriteToDiskLoop failure, auto-conversion disabled", fid);
                listDestroy(dl->ida10);
                dl->ida10 = NULL;
                return;
            }
            StartNewPacket(work, &pkt.fsdh, work->tons);
        }
    }
}

/* Revision History
 *
 * $Log: convert.c,v $
 * Revision 1.11  2008/02/08 16:04:04  akimov
 * cast sample data byte swapper destination argument to remove compiler warnings
 *
 * Revision 1.10  2008/02/08 15:57:30  akimov
 * ensure sample data are in network byte order
 *
 * Revision 1.7  2008/02/03 21:21:45  dechavez
 * ignore lissUnpackMiniSeed return value and instead check
 * packet decode status to detect non-data packets to ignore
 *
 * Revision 1.6  2008/01/25 21:45:30  dechavez
 * sync time of next sample to current packet to avoid accumulating one sample drift errors
 *
 * Revision 1.5  2008/01/09 03:36:47  dechavez
 * added missing return value to StreamMatch()
 *
 * Revision 1.4  2008/01/07 21:40:42  dechavez
 * created
 *
 */
