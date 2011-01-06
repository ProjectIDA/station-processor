#pragma ident "$Id: meta.c,v 1.1 2006/12/12 22:45:13 dechavez Exp $"
/*======================================================================
 *
 * Manage Q330 meta-data
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "isi/dl.h"

/* metadata directory for a particular instrument */

static char *MetaDirPath(ISI_DL *dl, char *path, UINT64 serialno, BOOL create)
{
    sprintf(path, "%s%c%016llX", dl->path.meta, PATH_DELIMITER, serialno);
    if (create) util_mkpath(path, 0755);
    return path;
}

/* Write a chunk of metadata to disk */

void isidlWriteQDPMetaData(ISI_DL *dl, UINT64 serialno, QDP_META *meta)
{
FILE  *fp;
ISI_SEQNO seqno;
char path[MAXPATHLEN+1];
static char *fid = "isidlWriteQDPMetaData";

    if (dl == NULL || meta == NULL) return;

/* Don't try anything if we don't have a meta directory */

    if (!utilDirectoryExists(dl->path.meta)) return;

/* Build output path name $meta/serialno/currentsequencenumber */

    MetaDirPath(dl, path, serialno, TRUE);
    seqno = isidlCrntSeqno(dl);
    sprintf(path+strlen(path), "%c%08lx%016llx", PATH_DELIMITER, seqno.signature, seqno.counter);

/* Write meta-data to the file */

    if ((fp = fopen(path, "wb")) == NULL) {
       logioMsg(dl->lp, LOG_ERR, "%s: can't open %s: %s", fid, path, strerror(errno));
        return;
    }

    if (qdpWriteMeta(fp, meta) < 0) {
        logioMsg(dl->lp, LOG_ERR, "%s: qdpWriteMeta: %s: %s", fid, path, strerror(errno));
    }
    fclose(fp);
}

/* Revision History
 *
 * $Log: meta.c,v $
 * Revision 1.1  2006/12/12 22:45:13  dechavez
 * initial release
 *
 */
