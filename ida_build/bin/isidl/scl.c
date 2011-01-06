#pragma ident "$Id: scl.c,v 1.1 2008/01/25 22:03:31 dechavez Exp $"
/*======================================================================
 *
 *  Build Stream Control List
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_SCN

static BOOL LoadNRTS(LNKLST *head, NRTS_DL *nrts)
{
int i, j;
ISI_STREAM_NAME name;

    if (nrts == NULL) return TRUE;
    for (i = 0; i < nrts->sys->nsta; i++) {
        for (j = 0; j < nrts->sys->sta[i].nchn; j++) {
            isiStaChnToStreamName(nrts->sys->sta[i].name, nrts->sys->sta[i].chn[j].name, &name);
            if (!listAppend(head, &name, sizeof(ISI_STREAM_NAME))) return FALSE;
        }
    }

    return TRUE;
}

static BOOL LoadSclFromNrts(ISIDL_PAR *par)
{
int i;
NRTS_DL *nrts;
static char *fid = "BuildStreamControlList:LoadSclFromNrts";

    par->scl.source = SCL_FROM_NRTS;

    for (i = 0; i < par->nsite; i++) {
        if (!LoadNRTS(par->scl.list, par->dl[i]->nrts)) {
            fprintf(stderr, "%s: LoadNRTS: %s", fid, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL LoadSclFromDataBase(ISIDL_PAR *par)
{
static char *fid = "BuildStreamControlList:LoadSclFromDataBase";

    par->scl.source = SCL_FROM_DB;

    fprintf(stderr, "LoadSclFromDataBase not implemented\n");
    return FALSE;
}

static BOOL LoadSclFromFile(ISIDL_PAR *par)
{
static char *fid = "BuildStreamControlList:LoadSclFromFile";

    par->scl.source = SCL_FROM_FILE;

    fprintf(stderr, "LoadSclFromFile not implemented\n");
    return FALSE;
}

BOOL BuildStreamControlList(ISIDL_PAR *par)
{
static char *fid = "BuildStreamControlList";

    if (par->scl.spec == NULL) {
        par->scl.source = SCL_UNSPECIFIED;
        return TRUE;
    }

    if ((par->scl.list = listCreate()) == NULL) {
        fprintf(stderr, "%s: listCreate: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (strcasecmp(par->scl.spec, "nrts") == 0) return LoadSclFromNrts(par);
    if (strcasecmp(par->scl.spec, "db") == 0) return LoadSclFromDataBase(par);
    return LoadSclFromFile(par);
}

/* Revision History
 *
 * $Log: scl.c,v $
 * Revision 1.1  2008/01/25 22:03:31  dechavez
 * created
 *
 */
