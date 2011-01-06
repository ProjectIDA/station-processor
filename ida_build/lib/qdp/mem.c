#pragma ident "$Id: mem.c,v 1.8 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * C1_MEM support.  Application should call qdpSaveMem each time it
 * receives a C1_MEM packet.  qdpSaveMem will return TRUE once all
 * expected segments have been received.  The application should deal
 * with the full mem block before it calls qdpSaveMem again, as the
 * next time it is called the previous contents will be destroyed.
 *
 *====================================================================*/
#include "qdp.h"

void qdpInitMemBlk(QDP_MEMBLK *blk)
{
    if (blk == NULL) return;
    blk->full = FALSE;
    blk->type = QDP_MEM_TYPE_INVALID;
    memset(blk->pending, 0, sizeof(blk->pending));
    blk->nbyte = 0;
}

BOOL qdpSaveMem(QDP_MEMBLK *blk, QDP_TYPE_C1_MEM *mem)
{
int i, offset, segindex;
static char *fid = "qdpSaveMem";

    if (blk == NULL || mem == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (mem->seg >= QDP_MAX_MEM_SEG || mem->nseg >= QDP_MAX_MEM_SEG) {
        errno = EFAULT;
        return FALSE;
    }

    if (mem->nbyte > QDP_MAX_C1_MEM_CFGDATA) {
        errno = EFAULT;
        return FALSE;
    }

/* Assume 438 bytes of payload in each config sub-packet, except the last one */

    if (mem->seg < mem->nseg && mem->nbyte != QDP_MAX_C1_MEM_CFGDATA) {
        errno = EFAULT;
        return FALSE;
    }

/* Previously full block gets discarded */

    if (blk->full) {
        qdpInitMemBlk(blk);
    }

/* Can't switch types until done with current block */

    if (blk->nbyte != 0 && blk->type != mem->type) {
        qdpInitMemBlk(blk);
    }

/* Fresh blocks get told how many segments to expect */

    if (blk->nbyte == 0) {
        blk->type = mem->type;
        blk->remain = mem->nseg;
        for (i = 0; i < blk->remain; i++) blk->pending[i] = 1;
    }

/* If not already seen, save segment */

    segindex = mem->seg - 1;
    if (blk->pending[segindex] != 0) {
        offset = segindex * QDP_MAX_C1_MEM_CFGDATA;
        memcpy(&blk->data[offset], mem->contents, mem->nbyte);
        blk->nbyte += mem->nbyte;
        blk->pending[segindex] = 0;
        if (--blk->remain == 0) blk->full = TRUE;
    }

    return TRUE;
}

void qdpGetMemBlk(QDP *qp, QDP_MEMBLK *mem)
{
    memcpy(mem, &qp->meta.raw.token, sizeof(QDP_MEMBLK));
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
 * $Log: mem.c,v $
 * Revision 1.8  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
