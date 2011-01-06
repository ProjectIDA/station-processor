#pragma ident "$Id: data.c,v 1.5 2008/01/07 21:23:01 dechavez Exp $"
/*======================================================================
 *
 * Decode DT_DATA packets
 *
 *====================================================================*/
#include "qdp.h"

#define IsStatusBlockette(byte) (!((byte) & QDP_DCM_STATUS))
#define BlocketteIdent(byte) ((byte) & (IsStatusBlockette(byte) ?  QDP_DCM_ST :  QDP_DCM))
#define BlocketteResid(byte) ((byte) & (IsStatusBlockette(byte) ? ~QDP_DCM_ST : ~QDP_DCM))

static int Decode_BT_38(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_38 *dest, BOOL *error)
{
UINT8 *ptr;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8[0] = *ptr++;
    dest->bit8[1] = *ptr++;
    dest->bit8[2] = *ptr++;

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_816(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_816 *dest, BOOL *error)
{
UINT8 *ptr;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->bit16);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_316(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_316 *dest, BOOL *error)
{
UINT8 *ptr;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->bit16[0]);
    ptr += utilUnpackUINT16(ptr, &dest->bit16[1]);
    ptr += utilUnpackUINT16(ptr, &dest->bit16[2]);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_32(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_32 *dest, BOOL *error)
{
UINT8 *ptr;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->bit16);
    ptr += utilUnpackUINT32(ptr, &dest->bit32);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_232(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_232 *dest, BOOL *error)
{
UINT8 *ptr;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    ptr += utilUnpackUINT16(ptr, &dest->bit16);
    ptr += utilUnpackUINT32(ptr, &dest->bit32[0]);
    ptr += utilUnpackUINT32(ptr, &dest->bit32[1]);

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_COMP(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_COMP *dest, BOOL *error)
{
UINT8 *ptr, *save;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    save = ptr += utilUnpackUINT16(ptr, &dest->bit16);
    ptr += utilUnpackINT32(ptr, &dest->prev);
    ptr += utilUnpackUINT16(ptr, &dest->doff);

    dest->map = ptr;                                  /* flags start here */
    dest->comp.data = start + dest->doff;             /* data start here */
    dest->size = dest->bit16 & QDP_DMSZ;              /* blockette size */
    dest->comp.nseq = (dest->size - dest->doff) >> 2; /* number of 32 bit sequences */
    dest->filt = dest->bit8 & QDP_DMFBIT;             /* filter code (freq bit number) */
    dest->chan = blk->resid;                          /* input channel comes from "channel" residual */
    ptr = save + ((dest->size - 1) & 0xfffc);         /* blockettes are on 4-byte boundaries */

    dest->maplen = (int) (dest->comp.data - dest->map);
    dest->comp.seq = (UINT32 *) dest->comp.data;
#ifdef LTL_ENDIAN_HOST
    utilSwapUINT32(dest->comp.seq, dest->comp.nseq);
#endif

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

static int Decode_BT_MULT(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_MULT *dest, BOOL *error)
{
UINT8 *ptr, *save;

    *error = FALSE;
    ptr = start + 1; /* skip over channel */

    dest->bit8 = *ptr++;
    save = ptr += utilUnpackUINT16(ptr, &dest->bit16);
    dest->comp.seq = (UINT32 *) ptr;

    dest->filt = dest->bit8 & QDP_DMFBIT;
    dest->chan = blk->resid;                          /* input channel comes from "channel" residual */
    dest->size = dest->bit16 & QDP_DMSZ;
    dest->segno = dest->bit8 >> 3;
    dest->lastseg = dest->bit16 & QDP_DMLS ? TRUE : FALSE;
    ptr = save + ((dest->size - 1) & 0xfffc); /* blockettes are on 4-byte boundaries */

    dest->comp.nseq = (dest->size - sizeof(UINT32)) >> 2; /* number of 32 bit sequences */

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

/* decode a special purpose blockette
 * For now, we decode enough to figure out how to skip over it.
 */

static int Decode_BT_SPEC(UINT8 *start, QDP_DT_BLOCKETTE *blk, QDP_BLOCKETTE_SPEC *dest, BOOL *error)
{
UINT8 *ptr;
UINT16 size;

    *error = FALSE;

    ptr = start; /* include channel, unlike the others */

    dest->VariableLengthData = ptr;
    switch (blk->resid) {

      case QDP_SP_CALSTART:
        ptr += 28;
        break;

      case QDP_SP_CALABORT:
        ptr += 8;
        break;

      case QDP_SP_CNPBLK:
      case QDP_SP_CFGBLK:
        utilUnpackUINT16(&dest->VariableLengthData[2], &size);
        ptr += size;
        break;

      default:
        errno = EINVAL;
        *error = TRUE;
    }

    dest->seqno = blk->seqno;
    return (int) (ptr - start);
}

/* Figure out which of the 8 possible formats a blockette takes */

static int BlocketteFormat(UINT8 ident)
{
int i;
static struct {
    UINT8 ident;
    int   format;
} BlocketteIdentMap[] = {
    {QDP_DC_ST38,    QDP_BT_38},
    {QDP_DC_ST816,   QDP_BT_816},
    {QDP_DC_ST32,    QDP_BT_32},
    {QDP_DC_ST232,   QDP_BT_232},
    {QDP_DC_MN38,    QDP_BT_38},
    {QDP_DC_MN816,   QDP_BT_816},
    {QDP_DC_MN32,    QDP_BT_32},
    {QDP_DC_MN232,   QDP_BT_232},
    {QDP_DC_AG38,    QDP_BT_38},
    {QDP_DC_AG816,   QDP_BT_816},
    {QDP_DC_AG32,    QDP_BT_32},
    {QDP_DC_AG232,   QDP_BT_232},
    {QDP_DC_CNP38,   QDP_BT_38},
    {QDP_DC_CNP816,  QDP_BT_816},
    {QDP_DC_CNP316,  QDP_BT_316},
    {QDP_DC_CNP232,  QDP_BT_232},
    {QDP_DC_D32,     QDP_BT_32},
    {QDP_DC_COMP,    QDP_BT_COMP},
    {QDP_DC_MULT,    QDP_BT_MULT},
    {QDP_DC_SPEC,    QDP_BT_SPEC},
    {0,              QDP_BT_UNKNOWN}
};

    for (i = 0; BlocketteIdentMap[i].format != QDP_BT_UNKNOWN; i++) {
        if (ident == BlocketteIdentMap[i].ident) return BlocketteIdentMap[i].format;
    }

    return QDP_BT_UNKNOWN;
}

/* Decode a single blockette and add it to the list */

static int DecodeBlockette(UINT8 *start, UINT32 seqno, LNKLST *list, BOOL *error)
{
int len;
QDP_DT_BLOCKETTE blk;

    *error = FALSE;

    blk.seqno   = seqno;
    blk.channel = start[0];
    blk.ident = BlocketteIdent(blk.channel);
    blk.resid = BlocketteResid(blk.channel);
    blk.is_status = IsStatusBlockette(blk.channel);
    if ((blk.format = BlocketteFormat(blk.ident)) == QDP_BT_UNKNOWN) {
        *error = TRUE;
        errno = EINVAL;
        return 0;
    }

    blk.root = blk.channel;
    switch (blk.format) {
      case QDP_BT_38:
        len = Decode_BT_38(start, &blk, &blk.data.bt_38, error); break;
      case QDP_BT_816:
        len = Decode_BT_816(start, &blk, &blk.data.bt_816, error); break;
      case QDP_BT_316:
        len = Decode_BT_316(start, &blk, &blk.data.bt_316, error); break;
      case QDP_BT_32:
        len = Decode_BT_32(start, &blk, &blk.data.bt_32, error); break;
      case QDP_BT_232:
        len = Decode_BT_232(start, &blk, &blk.data.bt_232, error); break;
      case QDP_BT_COMP:
        blk.root = blk.channel & 0xe7;
        len = Decode_BT_COMP(start, &blk, &blk.data.bt_comp, error); break;
      case QDP_BT_MULT:
        blk.root = blk.channel & 0xe7;
        len = Decode_BT_MULT(start, &blk, &blk.data.bt_mult, error); break;
      case QDP_BT_SPEC:
        blk.root = 0;
        len = Decode_BT_SPEC(start, &blk, &blk.data.bt_spec, error); break;
      default:
        errno = EINVAL;
        *error = TRUE;
        break;
    }
    if (*error) return 0;

    if (!listAppend(list, &blk, sizeof(QDP_DT_BLOCKETTE))) {
        *error = TRUE;
        return 0;
    }

    return len;
}

/* Initialize an instance of a QDP_TYPE_DT_DATA structure.  We use a known
 * string in the signature field of this structure to determine if the linked
 * list field has been initialized or not.  This is to avoid memory leaks or
 * unpredictable behavior accessing the list.
 */

static void InitDtData(QDP_TYPE_DT_DATA *dt_data)
{
#define QDP_DT_DATA_SIGNATURE "QDP_STRUCT_INIT"

    if (strcmp(dt_data->signature, QDP_DT_DATA_SIGNATURE) == 0) {
        listClear(&dt_data->blist);
    } else {
        listInit(&dt_data->blist);
        sprintf(dt_data->signature, "%s", QDP_DT_DATA_SIGNATURE);
        dt_data->signature[QDP_DT_DATA_SIG_LEN] = 0;
    }

    dt_data->seqno = 0;
    dt_data->used = 0;
}

/* Load in the MN232 data */

void qdpLoadMN232(QDP_BLOCKETTE_232 *blk, QDP_MN232_DATA *dest)
{
    if (blk == NULL || dest == NULL) return;

    dest->seqno = blk->seqno;
    dest->sec   = blk->bit32[0];
    dest->usec  = blk->bit32[1];
    dest->qual  = blk->bit8;
    dest->loss  = blk->bit16;
}

/* Decode a QDP DT_DATA payload into a QDP_TYPE_DT_DATA structure */

BOOL qdpDecode_DT_DATA(UINT8 *start, UINT16 dlen, QDP_TYPE_DT_DATA *dest)
{
BOOL error;
UINT8 *ptr;

    if (start == NULL || dlen == 0 || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    InitDtData(dest);

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->seqno);
    do {
        ptr += DecodeBlockette(ptr, dest->seqno, &dest->blist, &error);
        if (error) return FALSE;
    } while((UINT16) (ptr - start) < dlen);

    dest->used = (int) (ptr - start);

    if (!listSetArrayView(&dest->blist)) return FALSE;

    return TRUE;
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
 * $Log: data.c,v $
 * Revision 1.5  2008/01/07 21:23:01  dechavez
 * little-endian bug fixes
 *
 * Revision 1.4  2007/10/31 23:11:48  dechavez
 * fixed benign type error in BlocketteFormat() BlocketteIdentMap[]
 *
 * Revision 1.3  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
