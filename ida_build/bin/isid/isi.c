#pragma ident "$Id: isi.c,v 1.3 2005/07/26 00:43:21 dechavez Exp $"
/*======================================================================
 *
 *  ISI disk loop utilities
 *
 *====================================================================*/
#include "isid.h"

static int CompareDL(const void *aptr, const void *bptr)
{
ISI_DL *a, *b;

    a = *((ISI_DL **)aptr);
    b = *((ISI_DL **)bptr);

    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return  1;

    return strcmp(a->sys->site, b->sys->site);
}

ISI_DL *LocateDiskLoopBySite(ISI_DL_MASTER *master, char *target)
{
int i;

    for (i = 0; i < master->ndl; i++) {
        if (strcmp(master->dl[i]->sys->site, target) == 0) return master->dl[i];
    }

    return NULL;
}

/* Revision History
 *
 * $Log: isi.c,v $
 * Revision 1.3  2005/07/26 00:43:21  dechavez
 * 1.5.0(B3) use ISI_GLOB, ISI_DL_MASTER instead of NRTS
 *
 * Revision 1.2  2005/06/30 01:43:05  dechavez
 * first working seqno (ISI_DL) version, no compression
 *
 * Revision 1.1  2005/06/24 22:01:57  dechavez
 * initial release
 *
 */
