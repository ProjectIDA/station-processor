#pragma ident "$Id: isi.c,v 1.12 2007/03/26 21:33:26 dechavez Exp $"
/*======================================================================
 *
 *  Write packets to ISI disk loop
 *
 *====================================================================*/
#include "ispd.h"
#include "isi/dl.h"
#include "nrts/dl.h"
#include "sanio.h" /* for the time conversion factor */

#define MY_MOD_ID ISPD_MOD_ISI

extern ISP_PARAMS *Params;
extern struct ispd_heap  *Heap;
extern struct ispd_queue *Q;
extern char *Syscode;
extern struct nrts_sys *Nrts;

static BOOL flag = FALSE;

#define SUBFORM_OFF 23  /* IDA 9.x subformat */
#define RT593_OFF   55  /* system time delay factor */
#define UNUSED_OFF  58  /* creation time stamp */
#define BEGSYS_OFF  10  /* begin system time */
#define ENDSYS_OFF  42  /* end system time */

static void PatchRT593(UINT8 *raw, int correction)
{
UINT32 value;

    utilUnpackUINT32(&raw[BEGSYS_OFF], &value);
    utilPackUINT32(&raw[BEGSYS_OFF], value - correction);

    utilUnpackUINT32(&raw[ENDSYS_OFF], &value);
    utilPackUINT32(&raw[ENDSYS_OFF], value - correction);

    raw[SUBFORM_OFF] = 1;
    raw[RT593_OFF] = (char) correction;
    raw[UNUSED_OFF] = raw[UNUSED_OFF+1] = 0;
}

static void PatchRev09Header(ISI_DL *dl, UINT8 *raw, UINT32 seqno, UINT32 tstamp)
{
#define CHAN_OFF 36
#define Is24Bit(raw) (raw[CHAN_OFF] >= 22 && raw[CHAN_OFF] <= 24)
#define REV9_TAG_OFFSET    2  /* offest to previously unused field for tag */
#define REV9_SEQNO_OFFSET  30 /* offset to IDA sequence number in raw packet header */
#define REV9_TSTAMP_OFFSET 60 /* offset to reception timestamp in raw packet header */
int i;
INT8 *ptr;
UINT16 chan;
static char *ida9_0 = IDA_REV9_TAG_0;
static char *ida9_X = IDA_REV9_TAG_X;
char *tag;

/* all packets get the sequence number */

    utilPackUINT32(&raw[REV9_SEQNO_OFFSET], seqno);

/* data packets get more */

    if (ida_rtype(raw, 9) != IDA_DATA) {
        tag = ida9_0;
    } else {
        /* shift the data over to leave room for creation time stamp */
        for (ptr = &raw[1020], i = 0; i < 240; i++, ptr -= 4) memcpy(ptr, ptr - 4, 4);
        /* insert creation time stamp */
        utilPackUINT32(&raw[REV9_TSTAMP_OFFSET], tstamp);
        /* special handling for RT593 systems */
        if (Params->rt593.present) {
            utilUnpackUINT16(&raw[CHAN_OFF], &chan);
            if (chan >= 22 && chan <= 24) {
                PatchRT593(raw, Params->rt593.correct);
                tag = ida9_X;
            } else {
                tag = ida9_0;
            }
        } else {
            tag = ida9_0;
        }
    }

/* set the appropriate IDA9 tag */

    memcpy(&raw[REV9_TAG_OFFSET], tag, strlen(tag));
}

static void PatchRev10Header(ISI_DL *dl, UINT8 *raw, UINT32 seqno, UINT32 tstamp)
{
#define REV10_SEQNO_OFFSET 28 /* offset to IDA sequence number in raw packet header */
UINT8 *ptr;
static char *reserved = "* IDA 10.2 *";

    if (raw[3] != 1) return; /* no room to patch 10.0 and 10.2 and up already patched */

    seqno = (UINT32) dl->sys->seqno.counter;
    tstamp = time(NULL) - SAN_EPOCH_TO_1970_EPOCH;
    ptr = &raw[REV10_SEQNO_OFFSET];
    ptr += utilPackUINT32(ptr, seqno);
    ptr += utilPackUINT32(ptr, tstamp);
    utilPackBytes(ptr, reserved, strlen(reserved));

    raw[3] = 2; /* 10.2 */
}

static void PatchHeader(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
UINT32 seqno, tstamp;
static char *fid = "IsiThread:PatchHeader";

    seqno = (UINT32) dl->sys->seqno.counter;
    tstamp = time(NULL) - SAN_EPOCH_TO_1970_EPOCH;

    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA9:
        PatchRev09Header(dl, raw->payload, seqno, tstamp);
        break;
      case ISI_TYPE_IDA10:
        PatchRev10Header(dl, raw->payload, seqno, tstamp);
        break;
      default:
        util_log(1, "%s: UNEXPECTED DESC TYPE VALUE = %d", fid, raw->hdr.desc.type);
        ispd_die(MY_MOD_ID + 1);
    }
}

THREAD_FUNC IsiThread(void *argptr)
{
ISI_RAW_PACKET raw;
ISI_DL *dl;
NRTS_DL *nrts;
OLD_MSGQ_MSG *msg;
static char *fid = "IsiThread";
char tbuf[1024];

    LogMsg(LOG_DEBUG, "ISI thread started");
    flag = TRUE;

    dl = (ISI_DL *) argptr;
    isidlLogDL(dl, LOG_INFO);
    if ((nrts = dl->nrts) != NULL) {
        nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM | NRTS_DL_FLAGS_STRICT_TIME;
        LogMsg(LOG_INFO, "%s NRTS support enabled, flags=0x%0x, tictol=%lu, bwdint=%d", dl->sys->site, nrts->flags, nrts->tol.tic, nrts->bwd.flushint);
        Nrts = nrts->sys;
        if (nrts->flags & NRTS_DL_FLAGS_STRICT_TIME) {
            LogMsg(LOG_INFO, "strict clock checks enabled, tolerance = %lu sec", nrts->tol.clock);
        } else {
            LogMsg(LOG_INFO, "strict clock checks disabled");
        }
    } else {
        LogMsg(LOG_INFO, "old NRTS support disabled");
    }

    isiInitRawHeader(&raw.hdr);
    strcpy(raw.hdr.site, dl->sys->site);
    raw.hdr.len.payload = dl->sys->maxlen;
    raw.hdr.desc.comp = ISI_COMP_NONE;
    raw.hdr.desc.order = ISI_ORDER_UNDEF;
    raw.hdr.desc.size  = sizeof(UINT8);
    switch (Params->ida->rev.value) {
      case  8:
        raw.hdr.desc.type = ISI_TYPE_IDA9; /* we will reformat into rev 9 */
        break;
      case 10:
        raw.hdr.desc.type = ISI_TYPE_IDA10;
        break;
      default:
        util_log(1, "%s: UNEXPECTED PKTREV VALUE = %d", fid, Params->ida->rev.value);
        ispd_die(MY_MOD_ID + 2);
    }

    while (1) {
        msg = msgq_get(&Q->nrts, OLD_MSGQ_WAITFOREVER);
        if (!msgq_chkmsg2(fid, msg)) {
            util_log(1, "%s: corrupt message received!", fid);
            ispd_die(MY_MOD_ID + 3);
        }
        raw.payload = (UINT8 *) msg->data;
        raw.hdr.len.native = msg->len;
        raw.hdr.status = ISI_RAW_STATUS_OK;
        PatchHeader(dl, &raw);
        raw.hdr.len.used = msg->len < IDA10_FIXEDRECLEN ? IDA10_FIXEDRECLEN : msg->len;
        if (!isidlWriteToDiskLoop(dl, &raw, ISI_OPTION_GENERATE_SEQNO)) {
            LogMsg(LOG_ERR, "%s: isidlWriteToDiskLoop: %s", fid, strerror(errno));
            ispd_die(MY_MOD_ID + 4);
        }
        msgq_put(&Heap->packets, msg);
    }
}

void InitIsi(LOGIO *lp)
{
THREAD tid;
static ISI_DL *dl;
static char *fid = "InitIsi";

    dl = isidlOpenDiskLoop(&Params->glob, Syscode, lp, ISI_RDWR);
    if (dl == NULL) {
        if (errno == EACCES) {
            LogMsg(LOG_WARN, "ERROR: ISI disk loop is locked");
            ispd_die(MY_MOD_ID + 5);
        }
    } else {
        SaveDLHandle((void *) dl);
        if (!THREAD_CREATE(&tid, IsiThread, (void *) dl)) {
            util_log(1, "%s: failed to create IsiThread", fid);
            ispd_die(MY_MOD_ID + 6);
        }
        return;
    }
}

/* Revision History
 *
 * $Log: isi.c,v $
 * Revision 1.12  2007/03/26 21:33:26  dechavez
 * Renamed NRTS_STATION_SYSTEM NRTS_STRICT_TIME constants to NRTS_DL_FLAGS_x
 *
 * Revision 1.11  2007/02/08 23:33:38  dechavez
 * ISI_GENERATE_SEQNO -> ISI_OPTION_GENERATE_SEQNO
 *
 * Revision 1.10  2007/01/25 20:28:23  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.9  2007/01/11 22:02:30  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.8  2006/12/06 22:38:04  dechavez
 * renamed all MSGQ... to OLDMSGQ...
 *
 * Revision 1.7  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.6  2005/11/04 04:40:27  dechavez
 * define global Nrts if NRTS support enabled
 *
 * Revision 1.5  2005/09/13 00:35:43  dechavez
 * ensure isi raw packets are at least 1024 bytes long
 *
 * Revision 1.4  2005/09/06 18:29:38  dechavez
 * added IDA9 tag to previously unused space at offset 2
 *
 * Revision 1.3  2005/08/26 20:13:00  dechavez
 * convert IDA 8 to 9 and IDA 10.1 to 10.2 before writing to ISI disk loop
 *
 * Revision 1.2  2005/07/26 00:58:14  dechavez
 * initial ISI dl support (2.4.6d)
 *
 * Revision 1.1  2005/07/06 15:42:54  dechavez
 * initial release
 *
 */
