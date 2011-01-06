#pragma ident "$Id: meta.c,v 1.9 2007/05/17 22:25:05 dechavez Exp $"
/*======================================================================
 *
 *  Support for QDP metadata
 *
 *  Assumes that metadata are stored in directories whose names are the
 *  the UINT64 hex serial numbers.  The metadata files are named by
 *  the ISI_SEQNO sequence number of the disk loop at the time they
 *  were written (as part of the QDP handshake.
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "qdplus.h"

/* Destroy all these lists cleanly */

void qdplusDestroyMetaData(QDPLUS_META_DATA *meta)
{
LNKLST_NODE *crnt;
QDPLUS_META_DATUM *datum;

    if (meta == NULL) return;

    crnt = listFirstNode(meta->list);
    while (crnt != NULL) {
        datum = (QDPLUS_META_DATUM *) crnt->payload;
        qdpDestroyTokens(&datum->data.token);
        crnt = listNextNode(crnt);
    }
    listDestroy(meta->list);
}

void qdplusDestroyMetaDataList(LNKLST *list)
{
LNKLST_NODE *crnt;
QDPLUS_META_DATA *meta;

    if (list == NULL) return;
    crnt = listFirstNode(list);
    while (crnt != NULL) {
        meta = (QDPLUS_META_DATA *) crnt->payload;
        qdplusDestroyMetaData(meta);
        crnt = listNextNode(crnt);
    }
    listDestroy(list);
}

/* Insert the QDPLUS_META_DATUM structures in increasing sequence number order */

static BOOL OrderedInsert(LNKLST *head, QDPLUS_META_DATUM *new)
{
LNKLST_NODE *crnt;
QDPLUS_META_DATUM *old;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        old = (QDPLUS_META_DATUM *) crnt->payload;
        if (isiSeqnoLE(&new->range.beg, &old->range.beg)) {
            if (listInsertBefore(crnt, new, sizeof(QDPLUS_META_DATUM)) == NULL) {
                return FALSE;
            } else {
                return TRUE;
            }
        }
        crnt = listNextNode(crnt);
    }

    return listAppend(head, new, sizeof(QDPLUS_META_DATUM));
}

/* Set the range endpoint for each element in a QDPLUS_META_DATUM list */

static void SetRangeEndpoints(LNKLST *list)
{
int i;
QDPLUS_META_DATUM *crnt, *next;

    for (i = 0; i < list->count - 1; i++) {
        crnt = (QDPLUS_META_DATUM *) list->array[i];
        next = (QDPLUS_META_DATUM *) list->array[i+1];
        crnt->range.end = next->range.beg;
    }
}

/* Read the contents of one file into a QDPLUS_META_DATUM */

static BOOL LoadMetaDatum(char *path, char *fname, QDPLUS_META_DATUM *datum)
{
char tmpname[ISI_SEQNO_STRING_LEN+1], *string;

    strcpy(datum->path, path);
    if (qdpReadMetaFile(datum->path, &datum->data) <= 0) return FALSE;
    qdpDestroyTokens(&datum->data.token); /* We'll reload into list later */
    if (strlen(fname) == ISI_SEQNO_STRING_LEN + 1) {
        strlcpy(tmpname, fname, ISI_SEQNO_STRING_LEN + 1);
        string = tmpname;
    } else {
        string = fname;
    }
    if (!isiStringToSeqno(string, &datum->range.beg)) return FALSE;
    datum->range.end = ISI_NEVER_SEQNO; /* assume for now */
    
    return TRUE;
}

/* Read all the metadata files for a particular instrument */

static BOOL ValidFile(char *path, char *fname)
{
    if (fname[0] == '.') return FALSE;
    if (strlen(fname) != ISI_SEQNO_STRING_LEN) {
        if (strlen(fname) == ISI_SEQNO_STRING_LEN + 1 && fname[ISI_SEQNO_STRING_LEN] != '.') return FALSE;
    }
    if (!utilFileExists(path)) return FALSE;
    return TRUE;
}

static BOOL LoadMetaDataDirectory(char *dpath, QDPLUS_META_DATA *meta)
{
int i;
char *fname, fpath[MAXPATHLEN+1];
LNKLST *dirlist;
QDPLUS_META_DATUM *datum;
static char *fid = "LoadMetaDataDirectory";

    if ((dirlist = utilListDirectory(dpath)) == NULL) return FALSE;

    if ((meta->list = listCreate()) == NULL) {
        listDestroy(dirlist);
        return FALSE;
    }

    for (i = 0; i < dirlist->count; i++) {
        fname = (char *) dirlist->array[i];
        sprintf(fpath, "%s%c%s", dpath, PATH_DELIMITER, fname);
        if (ValidFile(fpath, fname)) {
            if ((datum = (QDPLUS_META_DATUM *) malloc(sizeof(QDPLUS_META_DATUM))) == NULL) {
                listDestroy(dirlist);
                return FALSE;
            }
            if (!LoadMetaDatum(fpath, fname, datum)) {
                listDestroy(meta->list);
                listDestroy(dirlist);
                free(datum);
                return FALSE;
            }
            if (!OrderedInsert(meta->list, datum)) {
                listDestroy(meta->list);
                listDestroy(dirlist);
                free(datum);
                return FALSE;
            }
            free(datum);
        }
    }
    listDestroy(dirlist);

    if (!listSetArrayView(meta->list)) {
        listDestroy(meta->list);
        return FALSE;
    }

/* Decode the token data into the list */

    for (i = 0; i < meta->list->count; i++) {
        datum = (QDPLUS_META_DATUM *) meta->list->array[i];
        if (!qdpUnpackTokens(&datum->data.raw.token, &datum->data.token)) {
            listDestroy(meta->list);
            return FALSE;
        }
    }

    return TRUE;
}

/* Read all the metadata files into a linked list of QDPLUS_META_DATA structures */

static BOOL ValidSubdir(char *path, char *subdir)
{
    if (subdir[0] == '.') return FALSE;
    if (strlen(subdir) != QDPLUS_SERIALNO_STRING_LEN) return FALSE;
    if (!utilDirectoryExists(path)) return FALSE;
    return TRUE;
}

LNKLST *qdplusReadMetaData(char *base)
{
int i;
QDPLUS_META_DATA meta, *instance;
LNKLST *dirlist, *result;
char *name, path[MAXPATHLEN+1];
static char *fid = "qdplusReadMetaData";

    if ((result = listCreate()) == NULL) return NULL;

    if ((dirlist = utilListDirectory(base)) == NULL) {
        listDestroy(result);
        return NULL;
    }

    for (i = 0; i < dirlist->count; i++) {
        name = (char *) dirlist->array[i];
        sprintf(path, "%s%c%s", base, PATH_DELIMITER, name);
        if (ValidSubdir(path, name)) {
            meta.serialno = strtoll(name, NULL, 16);
            if (!LoadMetaDataDirectory(path, &meta)) {
                listDestroy(dirlist);
                listDestroy(result);
                return NULL;
            }
            if (!listAppend(result, &meta, sizeof(QDPLUS_META_DATA))) {
                listDestroy(dirlist);
                listDestroy(result);
                return NULL;
            }
        }
    }

    listDestroy(dirlist);
    if (!listSetArrayView(result)) {
        listDestroy(result);
        return NULL;
    }

    for (i = 0; i < result->count; i++) {
        instance = (QDPLUS_META_DATA *) result->array[i];
        SetRangeEndpoints(instance->list);
    }

    return result;
}

/* Print metadata */

void qdplusPrintMetaDatum(FILE *fp, QDPLUS_META_DATUM *datum)
{
char buf[ISI_SEQNO_STRING_LEN+100];

    fprintf(fp, "path=%s\n", datum->path);
    fprintf(fp, "range = %s ", isiSeqnoString(&datum->range.beg, buf));
    fprintf(fp, "to %s\n", isiSeqnoString(&datum->range.end, buf));
    qdpPrint_C1_COMBO(fp, &datum->data.combo);
    qdpPrintTokens(fp, &datum->data.token);
}

void qdplusPrintMetaData(FILE *fp, QDPLUS_META_DATA *meta)
{
int i;
QDPLUS_META_DATUM *datum;

    if (fp == NULL || meta == NULL) return;

    fprintf(fp, "serialno = %016llX\n", meta->serialno);
    fprintf(fp, "%d metadata sets:\n", meta->list->count);

    for (i = 0; i < meta->list->count; i++) {
        datum = (QDPLUS_META_DATUM *) meta->list->array[i];
        qdplusPrintMetaDatum(fp, datum);
    }
}

void qdplusPrintMetaDataList(FILE *fp, LNKLST *list, BOOL verbose)
{
int i, j;
QDPLUS_META_DATA *meta;
QDPLUS_META_DATUM *datum;

    if (verbose) {
        for (i = 0; i < list->count; i++) {
            if (i) fprintf(fp, "\n");
            meta = (QDPLUS_META_DATA *) list->array[i];
            qdplusPrintMetaData(fp, meta);
        }
    } else {
        for (i = 0; i < list->count; i++) {
            meta = (QDPLUS_META_DATA *) list->array[i];
            fprintf(fp, "serial number %016llX has %d sets\n", meta->serialno, meta->list->count);
            for (j = 0; j < meta->list->count; j++) {
                datum = (QDPLUS_META_DATUM *) meta->list->array[j];
                fprintf(fp, "%2d: ", j+1);
                fprintf(fp, "%s = ", datum->path);
                fprintf(fp, "%s to ", isiSeqnoString(&datum->range.beg, NULL));
                fprintf(fp, "%s\n", isiSeqnoString(&datum->range.end, NULL));
            }
        }
    }
}

/* Initialize LCQ metadata */

static QDPLUS_META_DATA *LocateMetaData(LNKLST *head, UINT64 serialno)
{
LNKLST_NODE *crnt;
QDPLUS_META_DATA *meta;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        meta = (QDPLUS_META_DATA *) crnt->payload;
        if (meta->serialno == serialno) return meta;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static QDPLUS_META_DATUM *LocateMetaDatum(LNKLST *head, UINT64 serialno, ISI_SEQNO *seqno)
{
LNKLST_NODE *crnt;
QDPLUS_META_DATA *meta;
QDPLUS_META_DATUM *datum;

    if ((meta = LocateMetaData(head, serialno)) == NULL) return NULL;

    crnt = listFirstNode(meta->list);
    while (crnt != NULL) {
        datum = (QDPLUS_META_DATUM *) crnt->payload;
        if (isiSeqnoGE(seqno, &datum->range.beg) && isiSeqnoLT(seqno, &datum->range.end)) return datum;
        crnt = listNextNode(crnt);
    }
    
    return NULL;
}

BOOL qdplusInitializeLCQMetaData(QDP_LCQ *lcq, LNKLST *head, UINT64 serialno, ISI_SEQNO *seqno)
{
QDPLUS_META_DATUM *datum;

    if ((datum = LocateMetaDatum(head, serialno, seqno)) != NULL) {
        memcpy(lcq->meta.raw.combo, datum->data.raw.combo, QDP_MAX_PAYLOAD);
        qdpDecode_C1_COMBO(lcq->meta.raw.combo, &lcq->meta.combo);
        lcq->meta.state |= QDP_META_STATE_HAVE_COMBO;

        lcq->meta.raw.token = datum->data.raw.token;
        if (!qdpUnpackTokens(&datum->data.raw.token, &lcq->meta.token)) return FALSE;
        lcq->meta.state |= QDP_META_STATE_HAVE_TOKEN;
    }
    
    return TRUE;
}

/* Revision History
 *
 * $Log: meta.c,v $
 * Revision 1.9  2007/05/17 22:25:05  dechavez
 * initial production release
 *
 */
