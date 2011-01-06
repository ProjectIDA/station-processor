#pragma ident "$Id: tokens.c,v 1.4 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 *
 * Decode DP tokens
 *
 *====================================================================*/
#include "qdp.h"

#define IsVariableLength(ident) ((ident & 0x80) != 0 ? TRUE : FALSE)
#define SizeOfLengthField(ident) (IsVariableLength(ident) ? (((ident & 0x40) == 0) ? 1 : 2) : 0)

/* Initialize an instance of a QDP_TYPE_DT_DATA structure.  We use a known
 * string in the signature field of this structure to determine if the linked
 * list field has been initialized or not.  This is to avoid memory leaks or
 * unpredictable behavior accessing the list.
 */

static void ClearScalerFields(QDP_DP_TOKEN *token)
{
    token->ver.valid = FALSE;
    token->site.valid = FALSE;
    token->dss.valid = FALSE;
    token->clock.valid = FALSE;
    token->logid.valid = FALSE;
    token->cnfid.valid = FALSE;
    token->srvr.net.valid = FALSE;
    token->srvr.web.valid = FALSE;
    token->srvr.data.valid = FALSE;
}

static void InitListFields(QDP_DP_TOKEN *token)
{
    listInit(&token->lcq);
    listInit(&token->iir);
    listInit(&token->fir);
    listInit(&token->cds);
    listInit(&token->mhd);
    listInit(&token->tds);
}

static void ClearListFields(QDP_DP_TOKEN *token)
{
    listClear(&token->lcq);
    listClear(&token->iir);
    listClear(&token->fir);
    listClear(&token->cds);
    listClear(&token->mhd);
    listClear(&token->tds);
}

static void InitTokens(QDP_DP_TOKEN *token)
{
#define QDP_STRUCT_SIG "QDP_STRUCT_INIT"

    if (strcmp(token->signature, QDP_STRUCT_SIG) == 0) {
        ClearListFields(token);
    } else {
        InitListFields(token);
        sprintf(token->signature, "%s", QDP_STRUCT_SIG);
        token->signature[QDP_DP_TOKEN_SIG_LEN] = 0;
    }

    ClearScalerFields(token);
}

static int UnpackPascalString(UINT8 *start, char *dest)
{
UINT8 *ptr, nchar;

    ptr = start;
    nchar = *ptr++;
    ptr += utilUnpackBytes(ptr, dest, nchar);
    dest[nchar] = 0;

    return (int) (ptr - start);
}

static int UnpackLengthField(UINT8 *start, int *dest, int key)
{
UINT8 *ptr, clen;
UINT16 slen;

    ptr = start;
    if (SizeOfLengthField(key) == 1) {
        clen = *ptr++;
        *dest = (int) clen;
    } else {
        ptr += utilUnpackUINT16(ptr, &slen);
        *dest = (int) slen;
    }

    return (int) (ptr - start);
}

static int UnpackTokenVersion(UINT8 *start, QDP_TOKEN_VERSION *dest)
{
UINT8 *ptr;

    ptr = start;
    dest->version = *ptr++;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenSite(UINT8 *start, QDP_TOKEN_SITE *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, dest->nname, QDP_NNAME_LEN);
    dest->nname[QDP_NNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->sname, QDP_SNAME_LEN);
    dest->sname[QDP_SNAME_LEN] = 0;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenServer(UINT8 *start, QDP_TOKEN_SERVER *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &dest->port);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenDss(UINT8 *start, QDP_TOKEN_DSS *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, dest->passwd.hi, 7);
    dest->passwd.hi[7] = 0;
    ptr += utilUnpackBytes(ptr, dest->passwd.mi, 7);
    dest->passwd.mi[7] = 0;
    ptr += utilUnpackBytes(ptr, dest->passwd.lo, 7);
    dest->passwd.lo[7] = 0;
    ptr += utilUnpackUINT32(ptr, &dest->timeout);
    ptr += utilUnpackUINT32(ptr, &dest->maxbps);
    dest->verbosity = *ptr++;
    dest->maxcpu = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->port);
    ptr += utilUnpackUINT16(ptr, &dest->maxmem);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenClock(UINT8 *start, QDP_TOKEN_CLOCK *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackINT32(ptr, &dest->offset);
    ptr += utilUnpackUINT16(ptr, &dest->maxlim);
    dest->pll.locked = *ptr++;
    dest->pll.track = *ptr++;
    dest->pll.hold = *ptr++;
    dest->pll.off = *ptr++;
    dest->pll.spare = *ptr++;
    dest->maxhbl = *ptr++;
    dest->minhbl = *ptr++;
    dest->nbl = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->clkqflt);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenLogid(UINT8 *start, QDP_TOKEN_LOGID *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, dest->mesg.loc, QDP_LNAME_LEN);
    dest->mesg.loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->mesg.chn, QDP_CNAME_LEN);
    dest->mesg.chn[QDP_CNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->time.loc, QDP_LNAME_LEN);
    dest->time.loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->time.chn, QDP_CNAME_LEN);
    dest->time.chn[QDP_CNAME_LEN] = 0;

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackTokenCnfid(UINT8 *start, QDP_TOKEN_CNFID *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, dest->loc, QDP_LNAME_LEN);
    dest->loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->chn, QDP_CNAME_LEN);
    dest->chn[QDP_CNAME_LEN] = 0;
    dest->flags = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->interval);

    dest->valid = TRUE;
    return (int) (ptr - start);
}

static int UnpackLcqAvepar(UINT8 *start, QDP_LCQ_AVEPAR *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->len);
    dest->filt = *ptr++;

    return (int) (ptr - start);
}

static int UnpackLcqDecim(UINT8 *start, QDP_LCQ_DECIM *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->src = *ptr++;
    dest->fir = *ptr++;

    return (int) (ptr - start);
}

static int UnpackLcqDetect(UINT8 *start, QDP_LCQ_DETECT *dest)
{
UINT8 *ptr;

    ptr = start;

    dest->base = *ptr++;
    dest->use = *ptr++;
    dest->options = *ptr++;
    dest->set = TRUE;

    return (int) (ptr - start);
}

static int UnpackTokenLcq(UINT8 *start, QDP_TOKEN_LCQ *dest, int key)
{
UINT8 i, *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    ptr += utilUnpackBytes(ptr, dest->loc, QDP_LNAME_LEN);
    dest->loc[QDP_LNAME_LEN] = 0;
    ptr += utilUnpackBytes(ptr, dest->chn, QDP_CNAME_LEN);
    dest->chn[QDP_CNAME_LEN] = 0;
    dest->ref = *ptr++;
    dest->src[0] = *ptr++;
    dest->src[1] = *ptr++;
    ptr += utilUnpackUINT32(ptr, &dest->options);
    ptr += utilUnpackINT16(ptr, &dest->rate);

/* optional fields */

    if (dest->options & QDP_LCQ_HAVE_PRE_EVENT_BUFFERS) ptr += utilUnpackUINT16(ptr, &dest->pebuf);
    if (dest->options & QDP_LCQ_HAVE_GAP_THRESHOLD)     ptr += utilUnpackREAL32(ptr, &dest->gapthresh);
    if (dest->options & QDP_LCQ_HAVE_CALIB_DELAY)       ptr += utilUnpackUINT16(ptr, &dest->caldly);
    if (dest->options & QDP_LCQ_HAVE_FRAME_COUNT)       dest->comfr = *ptr++;
    if (dest->options & QDP_LCQ_HAVE_FIR_MULTIPLIER)    ptr += utilUnpackREAL32(ptr, &dest->firfix);
    if (dest->options & QDP_LCQ_HAVE_AVEPAR)            ptr += UnpackLcqAvepar(ptr, &dest->ave);
    if (dest->options & QDP_LCQ_HAVE_CNTRL_DETECTOR)    dest->cntrl = *ptr++;
    if (dest->options & QDP_LCQ_HAVE_DECIM_ENTRY)       ptr += UnpackLcqDecim(ptr, &dest->decim);
    for (i = 0; i < QDP_LCQ_NUM_DETECT; i++) dest->detect[i].set = FALSE;
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_1)        ptr += UnpackLcqDetect(ptr, &dest->detect[0]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_2)        ptr += UnpackLcqDetect(ptr, &dest->detect[1]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_3)        ptr += UnpackLcqDetect(ptr, &dest->detect[2]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_4)        ptr += UnpackLcqDetect(ptr, &dest->detect[3]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_5)        ptr += UnpackLcqDetect(ptr, &dest->detect[4]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_6)        ptr += UnpackLcqDetect(ptr, &dest->detect[5]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_7)        ptr += UnpackLcqDetect(ptr, &dest->detect[6]);
    if (dest->options & QDP_LCQ_HAVE_DETECTOR_8)        ptr += UnpackLcqDetect(ptr, &dest->detect[7]);

/* derived fields */

    dest->frate = dest->rate < 0 ? (REAL64) -1.0 / (REAL64) dest->rate : (REAL64) dest->rate;
    dest->sint = (REAL64) 1.0 / dest->frate;

    return dest->len; // (int) (ptr - start);
}

static int UnpackIIRDatum(UINT8 *start, QDP_IIR_DATA *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackREAL32(ptr, &dest->ratio);
    dest->npole = *ptr++;
    dest->type = dest->npole & 0x80 ? QDP_IIR_HIPASS : QDP_IIR_LOPASS;
    dest->npole &= 0x7f;

    return (int) (ptr - start);
}

static int UnpackTokenIIR(UINT8 *start, QDP_TOKEN_IIR *dest, int key)
{
UINT8 i, *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);
    dest->nsection = *ptr++;
    ptr += utilUnpackREAL32(ptr, &dest->gain);
    ptr += utilUnpackREAL32(ptr, &dest->refreq);
    for (i = 0; i < dest->nsection; i++) ptr += UnpackIIRDatum(ptr, &dest->data[i]);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenFIR(UINT8 *start, QDP_TOKEN_FIR *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenCDS(UINT8 *start, QDP_TOKEN_CDS *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    dest->options = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);
    dest->nentry = dest->len - (int) (ptr - start);
    ptr += utilUnpackBytes(ptr, dest->equation, dest->nentry);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenMHD(UINT8 *start, QDP_TOKEN_MHD *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    dest->detf = *ptr++;
    dest->iw = *ptr++;
    dest->nht = *ptr++;
    ptr += utilUnpackUINT32(ptr, &dest->fhi);
    ptr += utilUnpackUINT32(ptr, &dest->flo);
    ptr += utilUnpackUINT16(ptr, &dest->wa);
    ptr += utilUnpackUINT16(ptr, &dest->spare);
    ptr += utilUnpackUINT16(ptr, &dest->tc);
    dest->x1 = *ptr++;
    dest->x2 = *ptr++;
    dest->x3 = *ptr++;
    dest->xx = *ptr++;
    dest->av = *ptr++;
    ptr += UnpackPascalString(ptr, dest->name);

    return dest->len; // (int) (ptr - start);
}

static int UnpackTokenTDS(UINT8 *start, QDP_TOKEN_TDS *dest, int key)
{
UINT8 *ptr;

    ptr = start;

    ptr += UnpackLengthField(ptr, &dest->len, key);
    dest->id = *ptr++;
    dest->detf = *ptr++;
    dest->iw = *ptr++;
    dest->nht = *ptr++;
    ptr += utilUnpackUINT32(ptr, &dest->fhi);
    ptr += utilUnpackUINT32(ptr, &dest->flo);
    ptr += utilUnpackUINT16(ptr, &dest->wa);
    ptr += utilUnpackUINT16(ptr, &dest->spare);
    ptr += UnpackPascalString(ptr, dest->name);

    return dest->len; // (int) (ptr - start);
}

static int SkipToken(UINT8 *start, int key)
{
int len;
UINT8 *ptr;

    ptr = start;
    ptr += UnpackLengthField(ptr, &len, key);
//    printf("qdpUnpackTokens: SkipToken: ignore %d byte type '%d' token\n", len, key);
    return len;
}

BOOL qdpUnpackTokens(QDP_MEMBLK *blk, QDP_DP_TOKEN *token)
{
UINT8 *ptr, key;
QDP_TOKEN_LCQ lcq;
QDP_TOKEN_IIR iir;
QDP_TOKEN_FIR fir;
QDP_TOKEN_CDS cds;
QDP_TOKEN_MHD mhd;
QDP_TOKEN_TDS tds;

    if (blk == NULL || token == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    InitTokens(token);

    ptr = blk->data;
    while (ptr < blk->data + blk->nbyte) {
        key = *ptr++;
        switch (key) {

          case 0: break;
          case 1: ptr += UnpackTokenVersion(ptr, &token->ver);       break;
          case 2: ptr += UnpackTokenSite(ptr,    &token->site);      break;
          case 3: ptr += UnpackTokenServer(ptr,  &token->srvr.net);  break;
          case 4: ptr += UnpackTokenDss(ptr,     &token->dss);       break;
          case 5: ptr += UnpackTokenServer(ptr,  &token->srvr.web);  break;
          case 6: ptr += UnpackTokenClock(ptr,   &token->clock);     break;
          case 7: ptr += UnpackTokenLogid(ptr,   &token->logid);     break;
          case 8: ptr += UnpackTokenCnfid(ptr,   &token->cnfid);     break;
          case 9: ptr += UnpackTokenServer(ptr,  &token->srvr.data); break;

          case 128:
            ptr += UnpackTokenLcq(ptr, &lcq, key);
            if (!listAppend(&token->lcq, &lcq, sizeof(QDP_TOKEN_LCQ))) return FALSE;
            break;

          case 129:
            ptr += UnpackTokenIIR(ptr, &iir, key);
            if (!listAppend(&token->iir, &iir, sizeof(QDP_TOKEN_IIR))) return FALSE;
            break;

          case 130:
            ptr += UnpackTokenFIR(ptr, &fir, key);
            if (!listAppend(&token->fir, &fir, sizeof(QDP_TOKEN_FIR))) return FALSE;
            break;

          case 131:
            ptr += UnpackTokenCDS(ptr, &cds, key);
            if (!listAppend(&token->cds, &cds, sizeof(QDP_TOKEN_CDS))) return FALSE;
            break;

          case 132:
            ptr += UnpackTokenMHD(ptr, &mhd, key);
            if (!listAppend(&token->mhd, &mhd, sizeof(QDP_TOKEN_MHD))) return FALSE;
            break;

          case 133:
            ptr += UnpackTokenTDS(ptr, &tds, key);
            if (!listAppend(&token->tds, &tds, sizeof(QDP_TOKEN_TDS))) return FALSE;
            break;

          default:
            if (SizeOfLengthField(key) != 0) {
                ptr += SkipToken(ptr, key);
            } else {
                errno = EINVAL;
                return FALSE;
            }
        }

        ++token->count;
    }

    if (!listSetArrayView(&token->lcq)) return FALSE;
    if (!listSetArrayView(&token->iir)) return FALSE;
    if (!listSetArrayView(&token->fir)) return FALSE;
    if (!listSetArrayView(&token->cds)) return FALSE;
    if (!listSetArrayView(&token->mhd)) return FALSE;
    if (!listSetArrayView(&token->tds)) return FALSE;

    return TRUE;
}

static void DestroyListFields(QDP_DP_TOKEN *token)
{
    listDestroy(&token->lcq);
    listDestroy(&token->iir);
    listDestroy(&token->fir);
    listDestroy(&token->cds);
    listDestroy(&token->mhd);
    listDestroy(&token->tds);
}

void qdpDestroyTokens(QDP_DP_TOKEN *token)
{
    if (token == NULL) return;
    ClearScalerFields(token);
    DestroyListFields(token);
    memset(token->signature, 0, QDP_DP_TOKEN_SIG_LEN);
}

/*-----------------------------------------------------------------------+
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
 * $Log: tokens.c,v $
 * Revision 1.4  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
