#pragma ident "$Id: getmap.c,v 1.2 2004/06/24 17:39:01 dechavez Exp $"
/*======================================================================
 *
 *  Get the mmap'd system file for the indicated station.  Returns the
 *  index to the desired station or a negative value if it can't be
 *  found.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

#define DELIMITERS "     ,"
#define MAXTEXT 128
#define COMMENT '#'
#define MAXTOKEN 16
#define GETLINE getline(fp, buffer, MAXTEXT, '#', &lineno)

int nrts_getmap(home, staname, map)
char *home;
char *staname;
struct nrts_mmap *map;
{
char *syscode;
struct nrts_files *file;
struct nrts_sys *sys;
static char *fid = "nrts_getmap";

    if (home == NULL || staname == NULL || map == NULL) {
        errno = EINVAL;
        return -2;
    }

    if ((syscode = nrts_syscode(home, staname)) == NULL) return -3;

    file = nrts_files(&home, syscode);
    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, map) != 0) return -4;
    sys = (struct nrts_sys *) map->ptr;

    return nrts_standx(sys, staname);
}

/* Revision History
 *
 * $Log: getmap.c,v $
 * Revision 1.2  2004/06/24 17:39:01  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
