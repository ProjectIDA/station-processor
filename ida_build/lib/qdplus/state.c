#pragma ident "$Id: state.c,v 1.4 2008/12/17 02:23:21 dechavez Exp $"
/*======================================================================
 *
 * QDPLUS state managment
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "qdplus.h"

/* Open state file */

static FILE *OpenStateFile(QDPLUS *handle, char *perm, int *status)
{
FILE *fp;

    if (handle == NULL || perm == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (handle->par.path.state == NULL) {
        errno = EINVAL;
        return NULL;
    }

    fp = fopen(handle->par.path.state, perm);
    if (fp == NULL && status != NULL) *status = QDPLUS_STATE_IOERR;

    return fp;
}

/* Print state contents */

static void PrintState(FILE *fp, LNKLST *head)
{
LNKLST_NODE *crnt;
QDPLUS_STATE *state;

    if (fp == NULL || state == NULL) return;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        state = (QDPLUS_STATE *) crnt->payload;
        fprintf(fp, "%3s", state->chn);
        fprintf(fp, " %2s", state->loc);
        fprintf(fp, " %08lx%016llx", state->seqno.signature, state->seqno.counter);
        fprintf(fp, " %ld", state->offset);
        fprintf(fp, "\n");
        crnt = listNextNode(crnt);
    }
}

void qdplusPrintState(FILE *fp, QDPLUS *handle)
{
LNKLST_NODE *crnt;
QDPLUS_DIGITIZER *digitizer;

    if (fp == NULL || handle == NULL) return;

    if (handle->par.path.state != NULL) {
        fprintf(fp, "%s\n", handle->par.path.state);
    } else {
        fprintf(fp, "qdplusPrintState called with NULL par.path.state!\n");
    }

    if ((crnt = listFirstNode(handle->list.digitizer)) == NULL) {
        fprintf(fp, "empty file\n");
        return;
    }

    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        fprintf(fp, "S/N %016llX\n", digitizer->serialno);
        PrintState(fp, digitizer->state);
        crnt = listNextNode(crnt);
    }

    fflush(fp);
}

/* Save current state */

static BOOL SaveState(FILE *fp, QDPLUS_DIGITIZER *digitizer)
{
QDP_LC *lc;
LNKLST_NODE *crnt;

/* write out digitizer serial number */

    if (fwrite(&digitizer->serialno, sizeof(UINT64), 1, fp) != 1) return FALSE;

/* write out each active HLP */
    
    crnt = listFirstNode(digitizer->lcq.lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        if (lc->hlp != NULL && lc->hlp->nsamp != 0) {
            if (!isiWriteSeqno(fp, &lc->hlp->seqno)) return FALSE;
            if (fwrite(lc->hlp->chn, QDP_CNAME_LEN+1, 1, fp) != 1) return FALSE;
            if (fwrite(lc->hlp->loc, QDP_LNAME_LEN+1, 1, fp) != 1) return FALSE;
            if (fwrite(&lc->hlp->offset, sizeof(INT32), 1, fp) != 1) return FALSE;
        }
        crnt = listNextNode(crnt);
    }

/* flag end of list with undefined sequence number */

    if (!isiWriteSeqno(fp, &ISI_UNDEFINED_SEQNO)) return FALSE;

    return TRUE;
}

BOOL qdplusSaveState(QDPLUS *handle)
{
FILE *fp;
LNKLST_NODE *crnt;
UINT16 dirty = QDPLUS_FLAG_DIRTY;
UINT16 clean = QDPLUS_FLAG_CLEAN;
QDPLUS_DIGITIZER *digitizer;

    if ((fp = OpenStateFile(handle, "wb", NULL)) == NULL) return FALSE;

    if (fwrite(&dirty, sizeof(UINT16), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    fflush(fp);

    crnt = listFirstNode(handle->list.digitizer);
    while (crnt != NULL) {
        if (!SaveState(fp, (QDPLUS_DIGITIZER *) crnt->payload)) {
            fclose(fp);
            return FALSE;
        }
        crnt = listNextNode(crnt);
    }

    rewind(fp);
    if (fwrite(&clean, sizeof(UINT16), 1, fp) != 1) {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

/* Read state file */

#define STATE_READ_OK       0
#define STATE_READ_FINISHED 1
#define STATE_READ_ERROR    2

static int ReadState(FILE *fp, QDPLUS_STATE *state)
{
    if (!isiReadSeqno(fp, &state->seqno)) return STATE_READ_ERROR;
    if (isiIsUndefinedSeqno(&state->seqno)) return STATE_READ_FINISHED;
    if (fread(state->chn, QDP_CNAME_LEN+1, 1, fp) != 1) return STATE_READ_ERROR;
    if (fread(state->loc, QDP_LNAME_LEN+1, 1, fp) != 1) return STATE_READ_ERROR;
    if (fread(&state->offset, sizeof(INT32), 1, fp) != 1) return STATE_READ_ERROR;

    return STATE_READ_OK;
}

int qdplusReadStateFile(QDPLUS *handle)
{
FILE *fp;
int status;
UINT16 flag;
UINT64 serialno;
LNKLST_NODE *crnt;
QDPLUS_STATE state;
QDPLUS_DIGITIZER *digitizer;

    status = QDPLUS_STATE_BUG;
    if ((fp = OpenStateFile(handle, "rb", &status)) == NULL) return status;

    if (fread(&flag, sizeof(UINT16), 1, fp) != 1) {
        status = feof(fp) ? QDPLUS_STATE_EMPTY : QDPLUS_STATE_IOERR;
        fclose(fp);
        return status;
    }

    if (flag != QDPLUS_FLAG_CLEAN) {
        fclose(fp);
        return QDPLUS_STATE_DIRTY;
    }

    while (fread(&serialno, sizeof(UINT64), 1, fp) == 1) {
        if ((digitizer = qdplusGetDigitizer(handle, serialno)) == NULL) {
            fclose(fp);
            return QDPLUS_STATE_BUG;
        }
        if (digitizer->state != NULL) listDestroy(digitizer->state);
        if ((digitizer->state = listCreate()) == NULL) {
            fclose(fp);
            return QDPLUS_STATE_NOMEM;
        }
        while ((status = ReadState(fp, &state)) == STATE_READ_OK) {
            if (!listAppend(digitizer->state, &state, sizeof(QDPLUS_STATE))) {
                fclose(fp);
                return QDPLUS_STATE_NOMEM;
            }
        }
        if (status != STATE_READ_FINISHED) {
            fclose(fp);
            return QDPLUS_STATE_IOERR;
        }
    }

    status = feof(fp) ? QDPLUS_STATE_OK : QDPLUS_STATE_IOERR;
    fclose(fp);

    return status;
}

/* Find seqno range spanned by state file */

static void StateSeqnoLimits(QDPLUS_DIGITIZER *digitizer, ISI_SEQNO *beg, ISI_SEQNO *end)
{
LNKLST_NODE *crnt;
QDPLUS_STATE *state;

    crnt = listFirstNode(digitizer->state);
    while (crnt != NULL) {
        state = (QDPLUS_STATE *) crnt->payload;
        if (isiIsUndefinedSeqno(beg)) *beg = state->seqno;
        if (isiIsUndefinedSeqno(end)) *end = state->seqno;
        if (isiSeqnoLT(&state->seqno, beg)) *beg = state->seqno;
        if (isiSeqnoGT(&state->seqno, end)) *end = state->seqno;
        crnt = listNextNode(crnt);
    }
}

BOOL qdplusStateSeqnoLimits(QDPLUS *handle, ISI_SEQNO *beg, ISI_SEQNO *end)
{
LNKLST_NODE *crnt;
QDPLUS_DIGITIZER *digitizer;

    if (handle == NULL || beg == NULL || end == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    *beg = ISI_UNDEFINED_SEQNO;
    *end = ISI_UNDEFINED_SEQNO;

    if ((crnt = listFirstNode(handle->list.digitizer)) == NULL) return TRUE;

    while (crnt != NULL) {
        digitizer = (QDPLUS_DIGITIZER *) crnt->payload;
        StateSeqnoLimits(digitizer, beg, end);
        crnt = listNextNode(crnt);
    }

    return TRUE;
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
 * $Log: state.c,v $
 * Revision 1.4  2008/12/17 02:23:21  dechavez
 * fixed empty qhlp state file bug
 *
 * Revision 1.3  2008/12/15 23:27:47  dechavez
 * use clean/dirty flag in state file
 *
 * Revision 1.2  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
