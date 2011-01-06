#pragma ident "$Id: indx.c,v 1.3 2004/06/24 17:39:01 dechavez Exp $"
/*======================================================================
 *
 * nrts_chnndx - given a channel name, return it's index in the
 *               provided station structure.
 *
 * nrts_standx - given a station name, return it's index in the
 *               provided system structure.
 *
 * Both functions return -1 if the given name is not found.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

int nrts_chnndx(struct nrts_sta *sta, char *name)
{
int i;
static char *fid = "nrts_chnndx";

    if (name == NULL) return -1;

    for (i = 0; i < sta->nchn; i++) {
        if (strcmp(name, sta->chn[i].name) == 0) return i;
    }

    return -1;
}

int nrts_standx(struct nrts_sys *sys, char *name)
{
int i;
static char *fid = "nrts_standx";

    if (name == NULL) return -1;

    for (i = 0; i < sys->nsta; i++) {
        util_log(9, "%s: strcmp(%s, %s)", fid, name, sys->sta[i].name);
        if (strcmp(name, sys->sta[i].name) == 0) return i;
    }

    return -1;
}

/* Revision History
 *
 * $Log: indx.c,v $
 * Revision 1.3  2004/06/24 17:39:01  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.2  2001/10/05 16:58:01  dec
 * removed forced case changes
 *
 * Revision 1.1.1.1  2000/02/08 20:20:30  dec
 * import existing IDA/NRTS sources
 *
 */
