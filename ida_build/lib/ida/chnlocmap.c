#pragma ident "$Id: chnlocmap.c,v 1.8 2009/02/04 23:51:12 dechavez Exp $"
/*======================================================================
 * 
 *  IDA MK7 channel map support
 *
 *====================================================================*/
#include "ida.h"
#include "isi/db.h"

BOOL idaSetChnLocMap(IDA *ida, char *name, UINT32 when)
{
char buf[IDA_MNAMLEN+1];
static char *fid = "idaSetChnLocMap";

    if (ida == NULL) {
        errno = EINVAL;
        return FALSE;
    }

/* Clear anything that might have been there */

    ida->mapname[0] = 0;
    listDestroy(ida->chnlocmap);
    ida->chnlocmap = NULL;

/* Only revs 1-9 use a channel map */

    if (ida->rev.value > 9) {
        sprintf(ida->mapname, "n/a");
        return TRUE;
    }

/* If no map name is given, consult the database for the default map */

    if (name == NULL) {
        if (ida->site != NULL) {
            name = isidbLookupStamap(ida->db, ida->site, when, buf, IDA_MNAMLEN);
            if (name == NULL) {
                logioMsg(ida->lp, LOG_ERR, "ERROR: can't determine channel map for station '%s'", ida->site);
                return FALSE;
            }
            strlcpy(ida->mapname, name, IDA_MNAMLEN+1);
        } else {
            sprintf(ida->mapname, "Std.%02d", ida->rev.value);
            name = ida->mapname;
        }
    } else {
        strlcpy(ida->mapname, name, IDA_MNAMLEN+1);
    }

    if (name == NULL) {
        logioMsg(ida->lp, LOG_ERR, "ERROR: can't determine channel map");
        return FALSE;
    }

/* Read in the named map from the database */

    if ((ida->chnlocmap = isidbChnLocMapList(ida->db, name)) == NULL) {
        logioMsg(ida->lp, LOG_ERR, "ERROR: can't load '%s' channel map, db=%s", name, ida->db->dbid);
        return FALSE;
    }

    return TRUE;
}

/* Look for a match in the channel map */

static BOOL FoundMatch(IDA_CHNLOCMAP *map, int ccode, int fcode, int tcode)
{
    if (map->ccode != ccode) return FALSE;
    if (map->fcode != fcode) return FALSE;
    if (map->tcode != tcode) return FALSE;
    return TRUE;
}

static IDA_CHNLOCMAP *FindMapEntry(IDA *ida, int ccode, int fcode, int tcode)
{
IDA_CHNLOCMAP *map;
LNKLST_NODE *crnt;

    /* no channel map is OK */

    if (ida->chnlocmap == NULL) return NULL;

    /* look for an exact match */

    crnt = listFirstNode(ida->chnlocmap);
    while (crnt != NULL) {
        map = (IDA_CHNLOCMAP *) crnt->payload;
        if (FoundMatch(map, ccode, fcode, tcode)) return map;
        crnt = listNextNode(crnt);
    }

    /* Nothing found.  If this was a triggered stream try again as continuous */

    if (tcode == 0) return NULL;

    crnt = listFirstNode(ida->chnlocmap);
    while (crnt != NULL) {
        map = (IDA_CHNLOCMAP *) crnt->payload;
        if (FoundMatch(map, ccode, fcode, 0)) return map;
        crnt = listNextNode(crnt);
    }

    /* No luck there either */

    return NULL;
}

char *idaChnlocName(IDA *ida, int ccode, int fcode, int tcode, char *buf, int buflen)
{
#define MIN_BUF_LEN (ISI_CHNLEN+ISI_LOCLEN+1)
IDA_CHNLOCMAP *map;
static char *nomatch = "?????";
static char result[MIN_BUF_LEN];

    if (ida == NULL || buf == NULL) return NULL;

    if ((map = FindMapEntry(ida, ccode, fcode, tcode)) == NULL) {
        strlcpy(result, nomatch, MIN_BUF_LEN);
    } else {
        sprintf(result, "%s%s", map->chn, map->loc);
    }

    strlcpy(buf, result, buflen);

    return buf;
}

void idaBuildStreamName(IDA *ida, IDA_DHDR *hdr, ISI_STREAM_NAME *name)
{
IDA_CHNLOCMAP *map;

    if (ida == NULL || hdr == NULL || name == NULL) return;

    if (ida->site != NULL) {
        strlcpy(name->sta, ida->site, ISI_STALEN+1);
    } else {
        name->sta[0] = 0;
    }

/* Look in the channel map for the names, if any */

    if ((map = FindMapEntry(ida, hdr->dl_channel, hdr->dl_filter, hdr->mode)) != NULL) {
        strlcpy(name->chn, map->chn, ISI_CHNLEN+1);
        strlcpy(name->loc, map->loc, ISI_LOCLEN+1);
        sprintf(name->chnloc, "%s%s", name->chn, name->loc);
        return;
    }

/* No mapping found so make up a default name */

    if (hdr->dl_channel >=  -9 && hdr->dl_channel <= 99) {
        sprintf(name->chn, "c%02d", hdr->dl_channel);
    } else {
        sprintf(name->chn, "c??");
    }

    if (hdr->dl_filter >= -9 && hdr->dl_filter <= 99) {
        sprintf(name->loc, "%02d", hdr->dl_filter);
    } else {
        sprintf(name->loc, "??");
    }

    sprintf(name->chnloc, "%s%s", name->chn, name->loc);
}

/* Revision History
 *
 * $Log: chnlocmap.c,v $
 * Revision 1.8  2009/02/04 23:51:12  dechavez
 * don't assume array view was set
 *
 * Revision 1.7  2008/01/16 23:41:10  dechavez
 * removed conditional debug code
 *
 * Revision 1.6  2008/01/16 23:26:13  dechavez
 * added conditional debug code, fill in missing mapname in idaSetChnLocMap()
 *
 * Revision 1.5  2007/10/30 22:07:45  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.4  2007/01/11 21:59:21  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.3  2007/01/08 15:51:18  dechavez
 * strlcpy() instead of strncpy()
 *
 * Revision 1.2  2006/06/26 22:42:34  dechavez
 * fixed signed/unsigned comparisons
 *
 * Revision 1.1  2006/02/08 22:58:02  dechavez
 * initial release
 *
 */
