#pragma ident "$Id: glob.c,v 1.15 2009/02/03 22:53:37 dechavez Exp $"
/*======================================================================
 *
 * Initialize the global parameters.
 *
 *====================================================================*/
#include "isi/dl.h"
#include "isi/db.h"

#define DEFAULT_DEBUG_PKT  LOG_DEBUG
#define DEFAULT_DEBUG_TTAG LOG_DEBUG
#define DEFAULT_DEBUG_BWD  LOG_DEBUG
#define DEFAULT_DEBUG_DL   LOG_DEBUG
#define DEFAULT_DEBUG_LOCK LOG_DEBUG
#define DEFAULT_HOME ISI_DL_DEFAULT_HOME_DIR
#define DEFAULT_DBID ISI_DL_DEFAULT_DBID
#define DEFAULT_TRECS 3000 /* typical number of IDA records in one hour */
#define DEFAULT_FLAGS 0x0000

#define DEFAULT_DEBUG { \
    DEFAULT_DEBUG_PKT, \
    DEFAULT_DEBUG_TTAG, \
    DEFAULT_DEBUG_BWD, \
    DEFAULT_DEBUG_DL, \
    DEFAULT_DEBUG_LOCK \
}

ISI_GLOB *isidlInitDefaultGlob(DBIO *db, ISI_GLOB *glob)
{
static ISI_DEBUG default_debug = DEFAULT_DEBUG;
char *home;
static char *NRTS_HOME = "NRTS_HOME";
static char *default_home = DEFAULT_HOME;

    if (glob == NULL) return NULL;

    if ((home = getenv(NRTS_HOME)) == NULL) home = default_home;

    glob->db = db;
    glob->dbid = (glob->db == NULL) ? NULL : glob->db->dbid;
    strlcpy(glob->root, home, MAXPATHLEN);
    glob->trecs = DEFAULT_TRECS;
    glob->debug = default_debug;;
    glob->flags = DEFAULT_FLAGS;

    return glob;
}

BOOL isidlSetGlobalParameters(char *dbspec, char *app, ISI_GLOB *glob)
{
DBIO *db;
static char *fid = "isidlSetGlobalParameters";

    if (glob == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if ((db = dbioOpen(dbspec, NULL)) == NULL) return FALSE;

    isidlInitDefaultGlob(db, glob);
    isidbLookupRoot(db, app, glob->root);
    isidbLookupDebug(db, app, &glob->debug);
    isidbLookupTrecs(db, &glob->trecs);

    return TRUE;
}

void isidlLoadDebugFlags(FILE *fp, ISI_DEBUG *debug)
{
int i, ntoken, lineno;
#define BUFLEN 1024
char buf[BUFLEN];
#define MAXTOK 5
char *token[MAXTOK];
char *delimiters = " ,;:\t";

    while (utilGetLine(fp, buf, BUFLEN, '#', &lineno) == 0) {
        ntoken = utilParse(buf, token, delimiters, MAXTOK, 0);
        for (i = 0; i < ntoken; i++) {
            if (strcasecmp(token[i], "pkt") == 0)  debug->pkt  = LOG_INFO;
            if (strcasecmp(token[i], "ttag") == 0) debug->ttag = LOG_INFO;
            if (strcasecmp(token[i], "bwd") == 0)  debug->bwd  = LOG_INFO;
            if (strcasecmp(token[i], "dl") == 0)   debug->dl   = LOG_INFO;
            if (strcasecmp(token[i], "lock") == 0) debug->lock = LOG_INFO;
        }
    }
}

/* Revision History
 *
 * $Log: glob.c,v $
 * Revision 1.15  2009/02/03 22:53:37  dechavez
 * added isidlLoadDebugFlags()
 *
 * Revision 1.14  2007/11/05 22:30:41  dechavez
 * Backed out the "fix" in Revision 1.13.  isidlInitDefaultGlob() saves the
 * db pointer and it is a bad thing to have freed it.
 *
 * Revision 1.13  2007/10/31 17:02:37  dechavez
 * removed memory leak in isidlSetGlobalParameters()
 *
 * Revision 1.12  2007/10/05 19:38:51  dechavez
 * added support for NRTS_HOME environment variable
 *
 * Revision 1.11  2007/02/20 02:19:57  dechavez
 * cosmetic changes only
 *
 * Revision 1.10  2007/01/23 02:55:24  dechavez
 * added flags field
 *
 * Revision 1.9  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.8  2006/11/10 06:42:44  dechavez
 * added ttag to ISI_DEBUG
 *
 * Revision 1.7  2006/03/14 01:31:21  dechavez
 * forgot to return TRUE on success
 *
 * Revision 1.6  2006/03/13 22:30:36  dechavez
 * major rework to use libisidb lookups instead of flat file
 *
 * Revision 1.5  2006/02/09 00:24:40  dechavez
 * pass LOGIO to dbioOpen
 *
 * Revision 1.4  2005/09/30 21:57:16  dechavez
 * initialize db isiInitDefaultGlob()
 *
 * Revision 1.3  2005/09/10 21:41:54  dechavez
 * added support for lock debugger
 *
 * Revision 1.2  2005/08/26 18:15:52  dechavez
 * added tee (trecs) support
 *
 * Revision 1.1  2005/07/26 00:25:20  dechavez
 * initial release
 *
 */
