#pragma ident "$Id: systems.c,v 1.7 2007/09/06 18:31:59 dechavez Exp $"
/*======================================================================
 *
 *  Get list of supported system names
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

#define DELIMITERS "     ,"
#define MAXTEXT 128
#define COMMENT '#'
#define MAXTOKEN 16
#define GETLINE util_getline(fp, buffer, MAXTEXT, '#', &lineno)

int nrts_systems(home, sysname, maxsys)
char *home;
char **sysname;
int maxsys;
{
FILE *fp;
char buffer[MAXTEXT];
char fname[MAXPATHLEN+1];
char *token[MAXTOKEN];
int i, nsys, lineno, status, ntoken;
static char *fid = "nrts_systems";

    sprintf(fname, "%s/%s", home, NRTS_SYSTEMS);
    if ((fp = fopen(fname, "r")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, fname, syserrmsg(errno));
        return -1;
    }

    nsys = 0;
    while ((status = GETLINE) == 0) {
        ntoken = util_sparse(buffer, token, DELIMITERS, MAXTOKEN);
        for (i = 0; i < ntoken; i++) {
            if (nsys == maxsys) {
                util_log(1, "%s: %s: too many entries", fid, fname);
                return -2;
            }
            sprintf(fname, "%s/%s/sys", home, token[i]);
            if (utilFileExists(fname)) {
                if ((sysname[nsys++] = strdup(token[i])) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -3;
                }
            }
        }
    }

    return nsys;
}

/* Revision History
 *
 * $Log: systems.c,v $
 * Revision 1.7  2007/09/06 18:31:59  dechavez
 * changed nrts_systems() to ignore etc/Systems entries which are not for NRTS disk loops
 *
 * Revision 1.6  2007/08/28 18:40:33  dechavez
 * ignore systems which lack sys files (ie, non-NRTS disk loops)
 *
 * Revision 1.5  2005/07/26 00:08:39  dechavez
 * removed obsolete nrtsGetSystemList()
 *
 * Revision 1.4  2004/06/24 17:34:20  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.3  2004/06/11 17:03:05  dechavez
 * Use (new) home dir field of handle to locate Systems file in nrtsGetSystemList
 *
 * Revision 1.2  2003/10/16 17:00:31  dechavez
 * added nrtsGetSystemList()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:31  dec
 * import existing IDA/NRTS sources
 *
 */
