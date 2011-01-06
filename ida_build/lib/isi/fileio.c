#pragma ident "$Id: fileio.c,v 1.1 2009/05/14 16:32:26 dechavez Exp $"
/*======================================================================
 *
 * Read/Write things
 *
 *====================================================================*/
#include "isi.h"

BOOL isiFileReadStreamName(FILE *fp, ISI_STREAM_NAME *name)
{
char sta[ISI_STALEN+1], chn[ISI_CHNLEN+1], loc[ISI_LOCLEN+1];

    if (fp == NULL || name == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (fread(sta, 1, ISI_STALEN, fp) != ISI_STALEN) return FALSE;
    sta[ISI_STALEN] = 0;
    if (fread(chn, 1, ISI_CHNLEN, fp) != ISI_CHNLEN) return FALSE;
    chn[ISI_CHNLEN] = 0;
    if (fread(loc, 1, ISI_LOCLEN, fp) != ISI_LOCLEN) return FALSE;
    loc[ISI_LOCLEN] = 0;
    isiStaChnLocToStreamName(sta, chn, loc, name);

    return TRUE;
}

BOOL isiFileReadSrate(FILE *fp, ISI_SRATE *srate)
{
    if (fp == NULL || srate == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!utilReadUINT16(fp, &srate->factor)) return FALSE;
    if (!utilReadUINT16(fp, &srate->multiplier)) return FALSE;

    return TRUE;
}

BOOL isiFileReadTstamp(FILE *fp, ISI_TSTAMP *tstamp)
{
    if (fp == NULL || tstamp == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!utilReadREAL64(fp, &tstamp->value)) return FALSE;
    if (!utilReadUINT16(fp, &tstamp->status)) return FALSE;

    return TRUE;
}

BOOL isiFileReadCdstatus(FILE *fp, ISI_CD_STATUS *status)
{
    if (fp == NULL || status == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (fread(&status->data,     1, 1, fp) != 1) return FALSE;
    if (fread(&status->security, 1, 1, fp) != 1) return FALSE;
    if (fread(&status->misc,     1, 1, fp) != 1) return FALSE;
    if (fread(&status->voltage,  1, 1, fp) != 1) return FALSE;
    if (!utilReadREAL64(fp, &status->sync)) return FALSE;
    if (!utilReadUINT32(fp, &status->diff)) return FALSE;

    return TRUE;
}

BOOL isiFileReadDatumDescription(FILE *fp, ISI_DATA_DESC *desc)
{
    if (fp == NULL || desc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (fread(&desc->comp,  1, 1, fp) != 1) return FALSE;
    if (fread(&desc->type,  1, 1, fp) != 1) return FALSE;
    if (fread(&desc->order, 1, 1, fp) != 1) return FALSE;
    if (fread(&desc->size,  1, 1, fp) != 1) return FALSE;

    return TRUE;
}

BOOL isiFileReadGenericTSHDR(FILE *fp, ISI_GENERIC_TSHDR *hdr)
{
    if (fp == NULL || hdr == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!isiFileReadStreamName(fp, &hdr->name)) return FALSE;
    if (!isiFileReadSrate(fp, &hdr->srate)) return FALSE;
    if (!isiFileReadTstamp(fp, &hdr->tofs)) return FALSE;
    if (!isiFileReadTstamp(fp, &hdr->tols)) return FALSE;
    if (!isiFileReadCdstatus(fp, &hdr->status)) return FALSE;
    if (!utilReadUINT32(fp, &hdr->nsamp)) return FALSE;
    if (!utilReadUINT32(fp, &hdr->nbytes)) return FALSE;
    if (!isiFileReadDatumDescription(fp, &hdr->desc)) return FALSE;

    return TRUE;
}

BOOL isiFileReadGenericTS(FILE *fp, ISI_GENERIC_TS *ts)
{
    if (fp == NULL || ts == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!isiFileReadGenericTSHDR(fp, &ts->hdr)) return FALSE;
    if (ts->hdr.nbytes > ts->nalloc) {
        if (ts->precious) {
            errno = ENOMEM;
            return FALSE;
        }
        if ((ts->data = (void *) realloc(ts->data, ts->hdr.nbytes)) == NULL) return FALSE;
        ts->nalloc = ts->hdr.nbytes;
    }

    if (fread(ts->data, 1, ts->hdr.nbytes, fp) != ts->hdr.nbytes) return FALSE;

    return TRUE;
}

/* Revision History
 *
 * $Log: fileio.c,v $
 * Revision 1.1  2009/05/14 16:32:26  dechavez
 * initial release
 *
 */
