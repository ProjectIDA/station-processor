#pragma ident "$Id: hlp.c,v 1.6 2007/06/14 21:56:18 dechavez Exp $"
/*======================================================================
 *
 * Build high level packet buffers
 *
 *====================================================================*/
#include "qdp.h"
#include "util.h"

/* Dispose of a newly completed high level packet.  If the system is in
 * production phase then the user supplied function is invoked, otherwise
 * the packet is simply tossed.
 */

void qdpFlushHLP(QDP_LCQ *lcq, QDP_LC *lc)
{
QDP_HLP *hlp;
QDP_HLP_RULES *rules;

    if (lcq == NULL || lc == NULL) return;
    rules = &lcq->par.rules;
    hlp = lc->hlp;
    if (!rules->valid || hlp == NULL) return;

    if (lcq->state.flag == QDP_LCQ_STATE_PRODUCTION) (rules->func)(rules->args, hlp);
    hlp->nsamp = hlp->nbyte = 0;
}

/* Start a new high level packet.  Initially the time of first sample
 * is the same as the time of last sample.  We will increment the time
 * of last sample each time we add a new sample.  That way we can
 * compare with the sample times from new QDP packets and detect
 * an time tears in the incoming data.
 */

static UINT8 *StartPacket(QDP_HLP *hlp, QDP_MN232_DATA *mn232, UINT64 tofs, UINT8 qual, int index, ISI_SEQNO seqno)
{
    hlp->mn232 = *mn232;
    hlp->qual = qual;
    hlp->offset = index;
    hlp->tols = tofs + (hlp->offset * hlp->nsint);
    hlp->tofs = (hlp->tols / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
    hlp->tols -= hlp->nsint;
    hlp->seqno = seqno;

    return &hlp->data[0];
}

/* Check for time increment errors between successive QDP logical channels */

static INT64 TimeIncrError(UINT64 prev, UINT64 crnt, UINT64 nsint)
{
INT64 error, sign = 1;
UINT64 expect;

    expect = prev + nsint;
    if (crnt == expect) {
        error = 0;
    } else if (crnt < expect) {
        error = expect - crnt;
        sign = -1;
    } else {
        error = crnt - expect;
    }

    if (error < nsint) error = 0;
    return error * sign;
}

static BOOL BuildUncompressed(QDP_LCQ *lcq, QDP_LC *lc)
{
int i;
UINT8 *ptr;
UINT64 tofs;
INT64 errsec;
REAL64 errsmp;
QDP_HLP *hlp;
QDP_HLP_RULES *rules;
static char *fid = "qdpBuildHLP:BuildHLP:Uncompressed";

    rules = &lcq->par.rules;
    hlp = lc->hlp;

/* Flush packet early if the next sample time doesn't match exactly what is expected */

    tofs = qdpDelayCorrectedTime(&lcq->mn232, hlp->delay);
    if (hlp->nsamp != 0 && (errsec = TimeIncrError(hlp->tols, tofs, hlp->nsint)) != 0) {
        errsmp = utilNsToS(errsec) / utilNsToS(hlp->nsint);
        qdpLcqWarn(lcq, "WARNING: %s: incoming %s time tear of %.5lf seconds (%.5lf samples) [sint=%.5lf]\n", fid, hlp->ident, utilNsToS(errsec), errsmp, utilNsToS(hlp->nsint));
        qdpFlushHLP(lcq, lc);
    }
    
/* Append the sample(s) to the packet */

    for (ptr = &hlp->data[hlp->nbyte], i = 0; i < lc->nsamp; i++) {
        if (hlp->nsamp == 0) ptr = StartPacket(hlp, &lcq->mn232, tofs, lcq->qual, i, lcq->state.seqno);
        ptr += utilPackINT32(ptr, lc->data[i]);
        hlp->nbyte += sizeof(INT32);
        hlp->nsamp += 1;
        hlp->tols += hlp->nsint;
        if (rules->maxbyte - hlp->nbyte < sizeof(INT32)) qdpFlushHLP(lcq, lc);
    }

    return TRUE;
}

static BOOL BuildSteim1(QDP_LCQ *lcq, QDP_LC *lc)
{
static char *fid = "qdpBuildHLP:BuildHLP:BuildSteim1";

    qdpLcqError(lcq, "%s: Steim 1 compressed HLP not implemented\n", fid);
    errno = ENOTSUP;
    return FALSE;
}

static BOOL BuildSteim2(QDP_LCQ *lcq, QDP_LC *lc)
{
static char *fid = "qdpBuildHLP:BuildHLP:BuildSteim2";

    qdpLcqError(lcq, "%s: Steim 2 compressed HLP not implemented\n", fid);
    errno = ENOTSUP;
    return FALSE;
}

static BOOL BuildHLP(QDP_LCQ *lcq, QDP_LC *lc)
{
BOOL result;
static char *fid = "qdpBuildHLP:BuildHLP";

    switch (lcq->par.rules.format) {
      case QDP_HLP_FORMAT_NOCOMP32:
        result = BuildUncompressed(lcq, lc);
        break;
      case QDP_HLP_FORMAT_STEIM1:
        result = BuildSteim1(lcq, lc);
        break;
      case QDP_HLP_FORMAT_STEIM2:
        result = BuildSteim2(lcq, lc);
        break;
      default:
        qdpLcqError(lcq, "%s: unsupported format '%d'\n", fid, lcq->par.rules.format);
        result = FALSE;
    }

    return result;
}

BOOL qdpBuildHLP(QDP_LCQ *lcq)
{
QDP_LC *lc;
LNKLST_NODE *crnt;
static char *fid = "qdpBuildHLP";

    if (!lcq->par.rules.valid) {
        qdpLcqWarn(lcq, "%s: WARNING: unexpected invalid rules!  Logic error? (sn=%016llX, addr=%08x)", fid, lcq->meta.combo.fix.sys_num, &lcq->par.rules);
        return TRUE;
    }

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        if (lc->hlp == NULL) {
            qdpLcqError(lcq, "%s: ERROR: unexpected NULL hlp!\n", fid);
            return FALSE;
        }
        if (lc->state == QDP_LC_STATE_FULL && !BuildHLP(lcq, lc)) return FALSE;
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

BOOL qdpInitHLPRules(QDP_HLP_RULES *dest, UINT32 maxbyte, UINT8 format, QDP_HLP_FUNC func, void *args, UINT32 flags)
{
    if (dest == NULL || func == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    dest->maxbyte = maxbyte;
    dest->format = format;
    dest->args = args;
    dest->func = func;
    dest->valid = TRUE;
    dest->flags = flags;

    return TRUE;
}

void *qdpDestroyHLP(QDP_HLP *hlp)
{
    if (hlp == NULL) return;
    free(hlp->data);
    free(hlp);
}

BOOL qdpInitHLP(QDP_HLP_RULES *rules, QDP_LC *lc, QDP_META *meta)
{
QDP_HLP *hlp;
static char *fid = "qdpInitHLP";

    if (rules == NULL || !rules->valid || lc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if ((hlp = (QDP_HLP *) malloc(sizeof(QDP_HLP))) == NULL) return FALSE;

    if ((hlp->data = (UINT8 *) malloc(rules->maxbyte)) == NULL) {
        free(hlp);
        return FALSE;
    };

    hlp->format = rules->format;
    hlp->delay = lc->delay;
    hlp->nbyte = 0;
    hlp->nsamp = 0;
    strcpy(hlp->chn, lc->chn);
    strcpy(hlp->loc, lc->loc);
    if (rules->flags & QDP_HLP_RULE_FLAG_LCASE) {
        util_lcase(hlp->chn);
        util_lcase(hlp->loc);
    } else if (rules->flags & QDP_HLP_RULE_FLAG_UCASE) {
        util_ucase(hlp->chn);
        util_ucase(hlp->loc);
    }
    hlp->nsint = lc->nsint;
    hlp->meta = meta;
    sprintf(hlp->ident, "%016llX:%02x:%02x", meta->combo.fix.sys_num, lc->src[0], lc->src[1]);

    lc->hlp = hlp;

    return TRUE;
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
 * $Log: hlp.c,v $
 * Revision 1.6  2007/06/14 21:56:18  dechavez
 * 1.0.1bis
 *
 * Revision 1.5  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
