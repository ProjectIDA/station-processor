#pragma ident "$Id: process.c,v 1.15 2009/02/04 17:26:35 dechavez Exp $"
/*======================================================================
 *
 * Process packets
 *
 * Note, we ignore all CNP blockettes because there is no way to
 * determine the sample interval.  Yes.  This is true. One needs to
 * resort to meta-data which are not embedded in the Q330, and so is
 * beyond the scope of this library. 
 *
 *====================================================================*/
#include "qdp.h"

/* Update LCQ status state flags */

static void UpdateLcqStatusFlags(QDP_LCQ *lcq)
{
    lcq->status.empty = FALSE;
    lcq->status.seqno = lcq->seqno;
}

/* Change the state of the LCQ state flag */

static void SetMetaState(QDP_LCQ *lcq, UINT8 newstate)
{
static int debug = 0;

    if (lcq->meta.state == newstate) return;

    if (debug) {
        printf("LCQ state change: ");
        utilPrintBinUINT8(stdout, lcq->meta.state);
    }

    lcq->meta.state = newstate;

    if (debug) {
        printf(" -> ");
        utilPrintBinUINT8(stdout, lcq->meta.state);
        printf("\n");
        fflush(stdout);
    }
}

/* Test for LCQ token match */

static BOOL TokenMatch(UINT8 *a, UINT8 *b)
{
int i;

    for (i = 0; i < QDP_LCQSRC_LEN; i++) if (a[i] != b[i]) return FALSE;
    return TRUE;
}

/* Locate a specific logical channel in a list */

static QDP_LC *LocateLC(QDP_LCQ *lcq, UINT8 *src)
{
int i;
BOOL match;
QDP_LC *lc;
LNKLST_NODE *crnt;
static char *fid = "LocateLC";

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        if (TokenMatch(src, lc->src)) return lc;
        crnt = listNextNode(crnt);
    }
    return NULL;
}

/* Locate the DP tokens for this logical channel, if any */

static QDP_TOKEN_LCQ *LocateTokens(QDP_LCQ *lcq, UINT8 *src)
{
int i, j;
QDP_TOKEN_LCQ *token;

    for (i = 0; i < lcq->meta.token.lcq.count; i++) {
        token = (QDP_TOKEN_LCQ *) lcq->meta.token.lcq.array[i];
        if (TokenMatch(src, token->src)) return token;
    }
    return NULL;
}

/* Initialize a new QDP_LC structure */

static BOOL InitLC(QDP_LC *lc, QDP_LCQ *lcq, UINT8 *src, int chan, int filt, UINT64 nsint)
{
int i;
static char *fid = "GetLC:CreateLC:InitLC";

    for (i = 0; i < QDP_LCQSRC_LEN; i++) lc->src[i] = src[i];
    lc->nsint = nsint;

    if ((lc->token = LocateTokens(lcq, src)) != NULL) {
        strcpy(lc->chn, lc->token->chn);
        strcpy(lc->loc, lc->token->loc);
        lc->maxsamp = lc->token->rate > 0 ? lc->token->rate : 1;
    } else if (lcq->par.rules.reqtoken) {
        lcq->event |= QDP_LCQ_EVENT_NO_TOKENS;
        return FALSE;
    } else {
        sprintf(lc->chn, "%02x", src[0]);
        sprintf(lc->loc, "%02x", src[1]);
        lc->maxsamp = QDP_MAX_SPS;
    }

    lc->delay = qdpLookupFilterDelay(&lcq->meta, src);
    lc->state = QDP_LC_STATE_EMPTY;
    if ((lc->data = (INT32 *) malloc(sizeof(INT32) * lc->maxsamp)) == NULL) {
        qdpLcqError(lcq, "%s: malloc: %s", fid, strerror(errno));
        lcq->event |= QDP_LCQ_EVENT_FATAL;
        return FALSE;
    }

    if (lcq->par.rules.valid) {
        if (!qdpInitHLP(&lcq->par.rules, lc, &lcq->meta)) {
            qdpLcqError(lcq, "%s: qdpInitHLP: %s", fid, strerror(errno));
            lcq->event |= QDP_LCQ_EVENT_FATAL;
            return FALSE;
        }
    } else {
        lc->hlp = NULL;
    }

    return TRUE;

}

/* Create a new QDP_LC list entry */

static QDP_LC *CreateLC(QDP_LCQ *lcq, UINT8 *src, int chan, int filt, UINT64 nsint)
{
QDP_LC new, *lc;
static char *fid = "GetLC:CreateLC";

    if (!InitLC(&new, lcq, src, chan, filt, nsint)) return NULL;

    if (!listAppend(lcq->lc, &new, sizeof(QDP_LC))) {
        qdpLcqError(lcq, "%s: listAppend: %s", fid, strerror(errno));
        return NULL;
    }

    if ((lc = LocateLC(lcq, src)) == NULL) {
        qdpLcqError(lcq, "%s: PROGAM LOGIC ERROR: can't find my OWN lc!", fid);
        return NULL;
    }
    qdpLcqInfo(lcq, "src=(0x%02x, %d) chn=%s loc=%s delay=%.6lf sint=%.3lf", lc->src[0], lc->src[1], lc->chn, lc->loc, (REAL64) lc->delay / 1000000.0, (REAL64) lc->nsint / NANOSEC_PER_SEC);
    return lc;
}

/* Get the LC for this data source, creating one if necessary */

static QDP_LC *GetLC(QDP_LCQ *lcq, UINT8 *src, int chan, int filt, UINT64 nsint)
{
QDP_LC *lc;
static char *fid = "GetLC";

    if ((lc = LocateLC(lcq, src)) != NULL) {
        return lc;
    } else {
        if (nsint == 0) nsint = lcq->meta.combo.fix.freq[filt].nsint;
        return CreateLC(lcq, src, chan, filt, nsint);
    }
}

/* Process ST38 blockette */

static void ProcessST38(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessST38";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process ST816 blockette */

static void ProcessST816(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessST816";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process ST32 blockette */

static void ProcessST32(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessST32";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process ST232 blockette */

static void ProcessST232(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessST232";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process MN38 blockette */

static void ProcessMN38(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
int i, npar;
UINT64 nsint;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessMN38";

    src[0] = blk->channel;
    switch (blk->resid) {
      case 0: npar = 2; nsint = NANOSEC_PER_SEC * 10; break;
      case 1: npar = 3; nsint = NANOSEC_PER_SEC * 10; break;
      case 3: npar = 1; nsint = NANOSEC_PER_SEC * 10; break;
      case 2: /* ignored because sample interval is ambiguous */
        lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
        return;
      default:
        lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
        return;
    }

    for (i = 0; i < npar; i++) {
        src[1] = i;
        if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
            lc->data[0] = blk->data.bt_38.bit8[i];
            lc->nsamp = 1;
            lc->state = QDP_LC_STATE_FULL;
        }
    }
    return;
}

/* Process MN816 blockette */

static void ProcessMN816(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT64 nsint;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessMN816";

    src[0] = blk->channel;
    src[1] = 0;

    switch (blk->resid) {
      case 0:
        nsint = NANOSEC_PER_SEC * 10;
        if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
            lc->data[0] = blk->data.bt_816.bit16;
            lc->nsamp = 1;
            lc->state = QDP_LC_STATE_FULL;
        }
        break;
      case 1:
        lcq->status.pktbufpct = blk->data.bt_816.bit16;
        UpdateLcqStatusFlags(lcq);
        nsint = NANOSEC_PER_SEC * 10;
        if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
            lc->data[0] = blk->data.bt_816.bit16;
            lc->nsamp = 1;
            lc->state = QDP_LC_STATE_FULL;
        }
        break;
      default:
        lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    }
    return;
}

/* Process MN32 blockette */

static void ProcessMN32(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT64 nsint;
REAL64 from, to;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessMN32";

    src[0] = blk->channel;
    src[1] = 0;

    nsint = blk->data.bt_32.bit8 * NANOSEC_PER_SEC;
    if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
        lc->delay = blk->data.bt_32.bit16 * (USEC_PER_MSEC / 5);
        lc->data[0] = blk->data.bt_32.bit32;
        lc->nsamp = 1;
        lc->state = QDP_LC_STATE_FULL;
        if (lc->nsint != nsint) {
            from = utilNsToS(lc->nsint);
            to = utilNsToS(nsint);
            qdpLcqWarn(lcq, "WARNING: MN32 '%s:%s' sample interval changed from %.3lf to %.3lf\n", lc->chn, lc->loc, from, to);
            lcq->event |= QDP_LCQ_EVENT_SINT_CHANGE;
        }
    }

    return;
}

/* Process MN232 blockette */

static void ProcessMN232(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT64 nsint;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessMN232";

    switch (blk->resid) {
      case 0:

    /* Save the data for the root time tag */

        qdpLoadMN232(&blk->data.bt_232, &lcq->mn232);
        lcq->qual = qdpClockQuality(&lcq->meta.token.clock, &lcq->mn232);
        SetMetaState(lcq, lcq->meta.state | QDP_META_STATE_HAVE_MN232);

    /* Extract the phase and quality samples */

        if (lcq->meta.state == QDP_META_STATE_COMPLETE) {

            nsint = NANOSEC_PER_SEC;
            src[0] = blk->channel;

            src[1] = 1;
            if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
                lc->data[0] = (INT32) lcq->mn232.usec;
                if (lc->data[0] >= (USEC_PER_SEC / 2)) lc->data[0] -= USEC_PER_SEC;
                lc->nsamp = 1;
                lc->state = QDP_LC_STATE_FULL;
            }

            src[1] = 2;
            if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
                lc->data[0] = lcq->qual;
                lc->nsamp = 1;
                lc->state = QDP_LC_STATE_FULL;
            }

            src[1] = 3;
            if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
                lc->data[0] = (INT32) lcq->mn232.loss;
                lc->nsamp = 1;
                lc->state = QDP_LC_STATE_FULL;
            }

        } else {
            lcq->event |= QDP_LCQ_EVENT_NO_META;
        }

        break;
      default:
        lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
        break;
    }

    return;
}

/* Process AG38 blockette */

static void ProcessAG38(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
#define QDP_AG38_NPAR 3
int i;
QDP_LC *lc;
UINT64 nsint[QDP_AG38_NPAR] = {NANOSEC_PER_SEC * 10, NANOSEC_PER_SEC * 10, NANOSEC_PER_SEC * 100};
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessAG38";

    src[0] = blk->channel;
    for (i = 0; i < QDP_AG38_NPAR; i++) {
        src[1] = i;
        if ((lc = GetLC(lcq, src, -1, -1, nsint[blk->resid])) != NULL) {
            lc->nsamp = 1;
            lc->data[0] = blk->data.bt_38.bit8[i];
            lc->state = QDP_LC_STATE_FULL;
        }
    }

    return;
}

/* Process AG816 blockette */

static void ProcessAG816(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT64 nsint;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessAG816";

    src[0] = blk->channel;

    switch (blk->resid) {
      case 0:
        lcq->status.calib.bitmap = blk->data.bt_816.bit16;
        lcq->status.calib.abort = blk->data.bt_816.bit8;
        UpdateLcqStatusFlags(lcq);
        break;
      case 1:
      case 2:
        src[1] = 0;
        nsint = NANOSEC_PER_SEC * 10;
        if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
            lc->data[0] = blk->data.bt_816.bit16;
            lc->nsamp = 1;
            lc->state = QDP_LC_STATE_FULL;
        }
        break;
      default:
        lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    }

    return;
}

/* Process AG32 blockette */

static void ProcessAG32(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessAG32";

    lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    return;
}

/* Process AG232 blockette */

static void ProcessAG232(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessAG232";

    lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    return;
}

/* Process CNP38 blockette */

static void ProcessCNP38(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessCNP38";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process CNP816 blockette */

static void ProcessCNP816(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessCNP816";

    lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    return;
}

/* Process CNP316 blockette */

static void ProcessCNP316(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
int i, npar, nib;
UINT64 nsint;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessCNP316";

/* only deal with aux channels for now (resid 1, 2, 3) */

    switch (blk->resid) {
      case 1: npar = 3; break;
      case 2: npar = 3; break;
      case 3: npar = 2; break;
      default: 
        lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
        return;
    }

    src[0] = blk->channel;
    nsint = NANOSEC_PER_SEC; /* this is a GUESS */

    for (i = 0; i < npar; i++) {
        nib = (blk->data.bt_316.bit8 >> (i*2)) & 0x03;
        if (nib != 0) {
            src[1] = i;
            if ((lc = GetLC(lcq, src, -1, -1, nsint)) != NULL) {
                lc->data[0] = blk->data.bt_316.bit16[i];
                lc->nsamp = 1;
                lc->state = QDP_LC_STATE_FULL;
            }
        }
    }

    return;
}

/* Process CNP232 blockette */

static void ProcessCNP232(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessCNP232";

    lcq->event |= QDP_LCQ_EVENT_UNDOCUMENTED;
    return;
}

/* Process D32 blockette */

static void ProcessD32(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessD32";

    src[0] = blk->root;
    src[1] = blk->data.bt_32.bit8 - 1;

    if ((lc = GetLC(lcq, src, blk->resid, blk->data.bt_32.bit8 - 1, 0)) == NULL) return;
    lc->data[0] = blk->data.bt_32.bit32;
    lc->nsamp = 1;
    lc->state = QDP_LC_STATE_FULL;
    return;
}

/* Process COMP blockette */

static void ProcessCOMP(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
int result;
QDP_LC *lc;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessCOMP";

    if (lcq->meta.state != QDP_META_STATE_COMPLETE) return;

    src[0] = blk->root;
    src[1] = blk->data.bt_comp.filt;

    if ((lc = GetLC(lcq, src, blk->resid, blk->data.bt_comp.filt, 0)) == NULL) return;
    result = qdpDecompressCOMP(&blk->data.bt_comp, lc->data, lc->maxsamp, &lc->nsamp);
    if (result == QDP_OK) {
        lc->state = QDP_LC_STATE_FULL;
    } else {
        lcq->event |= QDP_LCQ_EVENT_DECOMP_ERR;
    }
}

/* Process MULT blockette */

static void ProcessMULT(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessMULT";

    if (lcq->meta.state != QDP_META_STATE_COMPLETE) return;

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Process SPEC blockette */

static void ProcessSPEC(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
static char *fid = "ProcessSPEC";

    lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    return;
}

/* Copy blockette data into one (or more) logical channels */

static BOOL ProcessBlockette(QDP_LCQ *lcq, QDP_DT_BLOCKETTE *blk)
{
QDP_LC *lc;
UINT8 src[QDP_LCQSRC_LEN];
static char *fid = "ProcessBlockette";

    src[0] = blk->root;

    switch(blk->ident) {
      case QDP_DC_ST38:   ProcessST38(lcq, blk); break;
      case QDP_DC_ST816:  ProcessST816(lcq, blk); break;
      case QDP_DC_ST32:   ProcessST32(lcq, blk); break;
      case QDP_DC_ST232:  ProcessST232(lcq, blk); break;
      case QDP_DC_MN38:   ProcessMN38(lcq, blk); break;
      case QDP_DC_MN816:  ProcessMN816(lcq, blk); break;
      case QDP_DC_MN32:   ProcessMN32(lcq, blk); break;
      case QDP_DC_MN232:  ProcessMN232(lcq, blk); break;
      case QDP_DC_AG38:   ProcessAG38(lcq, blk); break;
      case QDP_DC_AG816:  ProcessAG816(lcq, blk); break;
      case QDP_DC_AG32:   ProcessAG32(lcq, blk); break;
      case QDP_DC_AG232:  ProcessAG232(lcq, blk); break;
      case QDP_DC_CNP38:  ProcessCNP38(lcq, blk); break;
      case QDP_DC_CNP816: ProcessCNP816(lcq, blk); break;
      case QDP_DC_CNP316: ProcessCNP316(lcq, blk); break;
      case QDP_DC_CNP232: ProcessCNP232(lcq, blk); break;
      case QDP_DC_D32:    ProcessD32(lcq, blk); break;
      case QDP_DC_COMP:   ProcessCOMP(lcq, blk); break;
      case QDP_DC_MULT:   ProcessMULT(lcq, blk); break;
      case QDP_DC_SPEC:   ProcessSPEC(lcq, blk); break;
      default:
        qdpLcqWarn(lcq, "WARNING: unrecognized DT_DATA blockette (0x%02x) ignored", blk->channel);
        lcq->event |= QDP_LCQ_EVENT_UNSUPPORTED;
    }

    return (lcq->event & QDP_LCQ_EVENT_FATAL) ? FALSE : TRUE;
}

/* Flush any live logical channels which don't match this seqno */

static UINT32 VerifySeqno(QDP_LCQ *lcq, UINT32 seqno)
{
QDP_LC *lc;
LNKLST_NODE *crnt;
BOOL TimeTear = FALSE;
static char *fid = "qdpProcessPacket:VerifySeqno";

    if (lcq->seqno == seqno) return seqno; /* continue processing current second */

    if (seqno != lcq->seqno + 1) TimeTear = TRUE;

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        if (TimeTear) qdpFlushHLP(lcq, lc);
        if (lc->state != QDP_LC_STATE_EMPTY) {
            qdpLcqWarn(lcq, "WARNING: flushing partial or full %s%s!", lc->chn, lc->loc);
            lc->state = QDP_LC_STATE_EMPTY;
            // PUT CODE HERE TO DEAL WITH DT_MULT BUFFERING
        }
        crnt = listNextNode(crnt);
    }
    SetMetaState(lcq, lcq->meta.state & ~QDP_META_STATE_HAVE_MN232);

    return seqno;
}

/* "empty" any full logical channels */

static void EmptyFullLC(QDP_LCQ *lcq)
{
QDP_LC *lc;
LNKLST_NODE *crnt;

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        if (lc->state == QDP_LC_STATE_FULL) lc->state = QDP_LC_STATE_EMPTY;
        crnt = listNextNode(crnt);
    }
}

/* Process an arbitrary QDP packet.
 *
 * Token and other meta-data are absorbed here for use in decoding subsequent
 * DT_DATA packets.  Non-data packets are ignored.  DT_DATA and DT_USER packets
 * are decoded and their contents passed to a linked list of logical channel queues.
 */

BOOL qdpProcessPacket(QDP_LCQ *lcq, QDP_PKT *pkt)
{
int i;
QDP_DT_BLOCKETTE *blk;
QDP_TYPE_C1_MEM c1_mem;
static char *fid = "qdpProcessPacket";

    if (lcq == NULL || pkt == NULL) {
        qdpLcqError(lcq, "%s: NULL input(s) not allowed: %s", fid);
        errno = EINVAL;
        return FALSE;
    }

/* It is the responsibility of the calling application to deal with any
 * full logical channels between repeated calls to qdpProcessPacket().
 * We clear any full channels here since they have already been consumed
 * or ignored by the application.
 */
    EmptyFullLC(lcq);

    lcq->fresh = FALSE;
    lcq->pkt = pkt;
    lcq->action = QDP_LCQ_IGNORE;
    lcq->event = QDP_LCQ_EVENT_OK;

    switch(pkt->hdr.cmd) {
      case QDP_C1_MEM:
        qdpDecode_C1_MEM(pkt->payload, &c1_mem);
        SetMetaState(lcq, lcq->meta.state & ~QDP_META_STATE_HAVE_TOKEN);
        if (!qdpSaveMem(&lcq->meta.raw.token, &c1_mem)) {
            qdpLcqError(lcq, "%s: qdpSaveMem: %s", fid, strerror(errno));
            return FALSE;
        }
        if (lcq->meta.raw.token.full) {
            if (!qdpUnpackTokens(&lcq->meta.raw.token, &lcq->meta.token)) {
                qdpLcqError(lcq, "%s: qdpUnpackTokens: %s", fid, strerror(errno));
                return FALSE;
            } else {
                SetMetaState(lcq, lcq->meta.state | QDP_META_STATE_HAVE_TOKEN);
                lcq->action = QDP_LCQ_LOAD_TOKEN;
            }
        } else {
            lcq->action = QDP_LCQ_SAVE_MEMBLK;
        }
        break;

      case QDP_C1_COMBO:
        memcpy(lcq->meta.raw.combo, pkt->payload, pkt->hdr.dlen);
        qdpDecode_C1_COMBO(lcq->meta.raw.combo, &lcq->meta.combo);
        SetMetaState(lcq, lcq->meta.state | QDP_META_STATE_HAVE_COMBO);
        lcq->action = QDP_LCQ_LOAD_COMBO;
        break;

      case QDP_DT_DATA:
        qdpDecode_DT_DATA(pkt->payload, pkt->hdr.dlen, &lcq->dt_data);
        lcq->seqno = VerifySeqno(lcq, lcq->dt_data.seqno); // flush data if different seqno
        for (i = 0; i < lcq->dt_data.blist.count; i++) {
            blk = (QDP_DT_BLOCKETTE *) lcq->dt_data.blist.array[i];
            if (!ProcessBlockette(lcq, blk)) {
                qdpLcqError(lcq, "%s: ProcessBlockette: %s", fid, strerror(errno));
                return FALSE;
            }
            if (lcq->meta.state != QDP_META_STATE_COMPLETE) {
                lcq->event |= QDP_LCQ_EVENT_NO_META;
                return TRUE;
            }
        }
        if (lcq->par.rules.valid && !qdpBuildHLP(lcq)) {
            qdpLcqError(lcq, "%s: qdpBuildHLP: %s", fid, strerror(errno));
            lcq->event |= QDP_LCQ_EVENT_FATAL;
            return FALSE;
        }
        lcq->action = QDP_LCQ_LOAD_DT_DATA;
        break;

      case QDP_DT_USER:
        lcq->action = QDP_LCQ_IS_DT_USER; /* leave it to the user app */
        break;

      case QDP_C1_STAT:
        qdpDecode_C1_STAT(pkt->payload, &lcq->c1_stat);
        lcq->action = QDP_LCQ_LOAD_C1_STAT;
        break;

      default:
        break;
    }

    return TRUE;
}

/* Initialize a QDP_LCQ structure */

BOOL qdpInitLCQ(QDP_LCQ *lcq, QDP_LCQ_PAR *par)
{
static QDP_LCQ_PAR defpar = QDP_DEFAULT_LCQ_PAR;
static char *fid = "qdpInitLCQ";

    if (par == NULL) par = &defpar;

    if (lcq == NULL) {
        logioMsg(par->lp, LOG_ERR, "%s: NULL input(s) not allowed", fid);
        errno = EINVAL;
        return FALSE;
    }

    lcq->par = *par;
    lcq->fresh = TRUE;
    memset(&lcq->status, 0, sizeof(QDP_LCQ_STATUS)); lcq->status.empty = TRUE;

    qdpInitMemBlk(&lcq->meta.raw.token);
    if ((lcq->lc = listCreate()) == NULL) {
        logioMsg(par->lp, LOG_ERR, "%s: listCreate: %s", fid, strerror(errno));
        return FALSE;
    }

    if (!qdpInit_C1_STAT(&lcq->c1_stat)) {
        logioMsg(par->lp, LOG_ERR, "%s: qdpInit_C1_STAT: %s", fid, strerror(errno));
        return FALSE;
    }

    SetMetaState(lcq, 0);

/* We set the state flag to production mode immediately.  If the app
 * requires high level packet state recovery then it can set the state
 * to QDP_LCQ_STATE_INITIALIZE, invoke the application specific state 
 * recovery, and then reset to QDP_LCQ_STATE_PRODUCTION.
 */

    lcq->state.flag = QDP_LCQ_STATE_PRODUCTION;

    return TRUE;
}

/* Destroy the dynamically allocated contents of a QDP_LCQ structure */

static void DestroyLCContents(QDP_LC *lc)
{
    qdpDestroyHLP(lc->hlp);
    free(lc->data);
}

void qdpDestroyLCQ(QDP_LCQ *lcq)
{
QDP_LC *lc;
LNKLST_NODE *crnt;

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        DestroyLCContents(lc);
        crnt = listNextNode(crnt);
    }

    listDestroy(lcq->lc);
    lcq->lc = NULL;
    qdpDestroyTokens(&lcq->meta.token);
}

/*-----------------------------------------------------------------------r
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: process.c,v $
 * Revision 1.15  2009/02/04 17:26:35  dechavez
 * log ProcessBlockette() error returns
 *
 * Revision 1.14  2008/10/02 22:46:35  dechavez
 * removed unused dlen parameter from call to qdpDecode_C1_STAT
 *
 * Revision 1.13  2008/03/05 22:56:44  dechavez
 * added support for CNP316 (aux packets)
 *
 * Revision 1.12  2007/12/20 22:49:55  dechavez
 * dded support for C1_STAT
 *
 * Revision 1.11  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
