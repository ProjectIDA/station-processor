#pragma ident "$Id: glob.c,v 1.8 2007/10/05 19:35:46 dechavez Exp $"
/*======================================================================
 *
 * Read the global initialization file.  This is expected to be called
 * early in the application while still in the foreground.  If anything
 * goes wrong, then the routine will print a diagnostic stderr message
 * and exit.
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"

#define DELIMITERS " =\t"
#define MAX_TOKEN  32
#define MAXLINELEN 256
#define COMMENT    '#'
#define QUOTE      '"'

#define DEFAULT_ROOT "/usr/nrts"
#define DEFAULT_TRECS 3000 /* typical number of IDA records in one hour */
#define DEFAULT_DEBUG_PKT  LOG_DEBUG
#define DEFAULT_DEBUG_TTAG LOG_DEBUG
#define DEFAULT_DEBUG_BWD  LOG_DEBUG
#define DEFAULT_DEBUG_DL   LOG_DEBUG
#define DEFAULT_DEBUG_LOCK LOG_DEBUG

void idaffDefaultGlob(IDAFF_GLOB *glob)
{
char *home;
static char *NRTS_HOME = "NRTS_HOME";
static char *default_home = DEFAULT_ROOT;

    if ((home = getenv(NRTS_HOME)) == NULL) home = default_home;

    strlcpy(glob->root, home, MAXPATHLEN);
    glob->trecs = DEFAULT_TRECS;
    glob->debug.pkt  = DEFAULT_DEBUG_PKT;
    glob->debug.ttag = DEFAULT_DEBUG_TTAG;
    glob->debug.bwd  = DEFAULT_DEBUG_BWD;
    glob->debug.dl   = DEFAULT_DEBUG_DL;
    glob->debug.lock = DEFAULT_DEBUG_LOCK;
}

static int BadTokenCount(char *fid, char *item, int lineno)
{
    if (item == NULL) {
        util_log(1, "%s: wrong number of args for token '%s', line %d", fid, item, lineno);
    } else {
        util_log(1, "%s: wrong number of args at line %d", fid, lineno);
    }

    return 1;
}

static int Interpret(char *fid, IDAFF_GLOB *glob, int ntoken, char **token, int lineno)
{
int debug;

    if (ntoken < 1) return BadTokenCount(fid, NULL, lineno);

    if (strcasecmp(token[0], "home") == 0 || strcasecmp(token[0], "root") == 0) {
        if (ntoken != 2) return BadTokenCount(fid, token[0], lineno);
        strlcpy(glob->root, token[1], MAXPATHLEN);
    } else if (strcasecmp(token[0], "dbid") == 0) {
        ; /* dbid may exist in old files, not relevant in this context */
    } else if (strcasecmp(token[0], "trecs") == 0) {
        if (ntoken != 2) return BadTokenCount(fid, token[0], lineno);
        if (atoi(token[1]) < 0) {
            util_log(1, "%s: bad trecs value '%s'", fid, token[1]);
            return 1;
        }
        glob->trecs = (UINT32) atoi(token[1]);
    } else if (strcasecmp(token[0], "debug") == 0) {
        if (ntoken != 3) return BadTokenCount(fid, token[0], lineno);
        if ((debug = logioSeverityStringToInt(token[2])) < 1) {
            util_log(1, "%s: logioSeverityString: %s unknown", fid, token[2]);
            return 1;
        }
        if (strcasecmp(token[1], "bwd") == 0) {
            glob->debug.bwd = debug;
        } else if (strcasecmp(token[1], "ttag") == 0) {
            glob->debug.ttag = debug;
        } else if (strcasecmp(token[1], "pkt") == 0) {
            glob->debug.pkt = debug;
        } else if (strcasecmp(token[1], "dl") == 0) {
            glob->debug.dl = debug;
        } else if (strcasecmp(token[1], "lock" ) == 0) {
            glob->debug.lock = debug;
        } else {
            util_log(1, "%s: unknown debug module code '%s'", fid, token[1]);
            return 1;
        }
    } else {
         util_log(1, "%s: unrecognized token '%s' ignored", fid, token[0]);
    }

    return 0;
}

BOOL idaffReadGlobalInitFile(char *path, IDAFF_GLOB *glob)
{
FILE *fp = NULL;
int status, lineno, ntoken, errors = 0;
char *token[MAX_TOKEN];
char line[MAXLINELEN];
static char *fid = "idaffReadGlobalInitFile";

    if (glob == NULL) {
        errno = EINVAL;
        perror(fid);
        return FALSE;
    }

/* Initialize with the hard-coded default */

    idaffDefaultGlob(glob);

    if (path == NULL) return TRUE;

/* User must want to load a config file */

    if ((fp = fopen(path, "r")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, strerror(errno));
        return FALSE;
    }
    
    errors = 0;
    while ((status = utilGetLine(fp, line, MAXLINELEN, COMMENT, &lineno)) == 0) {
        ntoken = utilParse(line, token, DELIMITERS, MAX_TOKEN, QUOTE);
        errors += Interpret(fid, glob, ntoken, token, lineno);
    }
    fclose(fp);
    
    if (status != 1) {
        util_log(1, "%s: utilGetLine: %s", fid, strerror(errno));
        return FALSE;
    }

    return errors ? FALSE : TRUE;
}

void idaffLookupGlob(IDAFF *ff, IDAFF_GLOB *glob)
{
    if (ff == NULL || glob == NULL) return;
    *glob = ff->glob;
}

/* Revision History
 *
 * $Log: glob.c,v $
 * Revision 1.8  2007/10/05 19:35:46  dechavez
 * added support for NRTS_HOME environment variable
 *
 * Revision 1.7  2007/01/08 15:48:03  dechavez
 * strlcpy() instead of strncpy()
 *
 * Revision 1.6  2006/11/10 06:54:24  dechavez
 * added ttag to ISI_DEBUG
 *
 * Revision 1.5  2006/07/07 17:28:42  dechavez
 * removed duplicate code in Interpret()
 *
 * Revision 1.4  2006/06/26 22:19:00  dechavez
 * change trecs to UINT32
 *
 * Revision 1.3  2006/03/15 20:46:31  dechavez
 * fix empty return when path is NULL
 *
 * Revision 1.2  2006/03/14 20:36:20  dechavez
 * fixed parse error for home dir, detect/ignore dbid
 *
 * Revision 1.1  2006/03/13 22:16:28  dechavez
 * initial release (brought over from libisidl)
 *
 */
