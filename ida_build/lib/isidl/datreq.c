#pragma ident "$Id: datreq.c,v 1.6 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 *  Expand ISI stream and seqno requests
 *
 *====================================================================*/
#include "isi/dl.h"
#include "isi/db.h"

/* Expand a possibly wildcarded stream request into a list */

INT32 isidlExpandTwindRequest(ISI_DL_MASTER *master, ISI_TWIND_REQUEST *req, LNKLST *out)
{
UINT32 i;
LNKLST config;
int count;
ISI_STREAM_CNF *cnf;
ISI_TWIND_REQUEST new;

    count = 0;

    if (!listInit(&config)) return -1;
    if (!isidlBuildStreamCnfList(master, &config)) return -2;
    if (!listSetArrayView(&config)) return -3;

    for (i = 0; i < config.count; i++) {
        cnf = (ISI_STREAM_CNF *) config.array[i];
        if (isiStreamNameCompareWild(&req->name, &cnf->name) == 0) {
            new = *req;
            new.name = cnf->name;
            if (!listAppend(out, &new, sizeof(ISI_TWIND_REQUEST))) return -4;
            ++count;
        }
    }
    listDestroy(&config);

    return count;
}

/* Expand a possibly wildcarded seqno request into a list */

INT32 isidlExpandSeqnoRequest(ISI_DL_MASTER *master, ISI_SEQNO_REQUEST *req, LNKLST *out)
{
UINT32 i;
LNKLST *system;
int count;
char *site;
ISI_SEQNO_REQUEST new;

    count = 0;

    if ((system = isidbSystemList(master->glob->db)) == NULL) return -1;

    for (i = 0; i < system->count; i++) {
        site = (char *) system->array[i];
        site[ISI_SITELEN] = 0;
        if (isiSiteNameCompareWild(req->site, site) == 0) {
            new = *req;
            strlcpy(new.site, site, ISI_SITELEN+1);
            if (!listAppend(out, &new, sizeof(ISI_SEQNO_REQUEST))) return -4;
            ++count;
        }
    }

    return count;
}

/* Revision History
 *
 * $Log: datreq.c,v $
 * Revision 1.6  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.5  2007/01/11 17:51:55  dechavez
 * renamed all the "stream" requests to the more accurate "twind" (time window)
 *
 * Revision 1.4  2007/01/08 16:02:31  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.3  2006/02/14 17:05:26  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.2  2006/02/09 00:31:24  dechavez
 * add isi/db.h
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
