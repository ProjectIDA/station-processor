#pragma ident "$Id: syscode.c,v 1.4 2007/01/07 17:51:59 dechavez Exp $"
/*======================================================================
 *
 *  Get the ident for the system which contains the given station, or
 *  NULL if no such system is to be found.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"

#define DELIMITERS " ,\t"
#define MAXTEXT 128
#define COMMENT '#'
#define MAXTOKEN 16
#define GETLINE util_getline(fp, buffer, MAXTEXT, '#', &lineno)

char *nrts_syscode(home, staname)
char *home;
char *staname;
{
char Systems[MAXPATHLEN+1];
FILE *fp, *tp;
int i, tok, ntoken, lineno;
static char buffer[MAXTEXT+1];
static char *token[MAXTOKEN];
struct nrts_files *file;
struct nrts_sys *sys;
struct nrts_mmap map;
static char *fid = "nrts_syscode";

    if (home == NULL || staname == NULL) {
        errno = EINVAL;
        return NULL;
    }

/*  First see if the system name is the same as the station code */

    strlcpy(buffer, staname, MAXTEXT+1);
    file = nrts_files(&home, buffer);
    if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) == 0) {
        sys = (struct nrts_sys *) map.ptr;
        if (nrts_standx(sys, staname) >= 0) return buffer;
    }

/*  If we didn't get it that way, look for it in $home/etc/Systems  */

    sprintf(Systems, "%s/etc/Systems", home);
    if ((fp = fopen(Systems, "r")) != NULL) {
        while (GETLINE == 0) {
            if ((ntoken = util_sparse(buffer,token,DELIMITERS,MAXTOKEN)) < 0) {
                util_log(1, "%s: can't parse line %d, file %d",
                    fid, lineno, Systems
                );
            } else {
                for (tok = 0; tok < ntoken; tok++) {
                    file = nrts_files(&home, token[tok]);
                    if ((tp = fopen(file->sys, "r")) != NULL) {
                        fclose(tp);
                        if (nrts_mmap(file->sys, "r", NRTS_SYSTEM, &map) == 0) {
                            sys = (struct nrts_sys *) map.ptr;
                            for (i = 0; i < sys->nsta; i++) {
                                if (strcmp(sys->sta[i].name, staname) == 0) {
                                    strlcpy(buffer, token[tok], MAXTEXT+1);
                                    return buffer;
                                }
                            }
                            #ifdef _WIN32
                            /*CloseHandle(map.fd);*/
                            nrts_unmap(&map);
                            #else
                            close(map.fd);
                            #endif
                        }
                    }
                }
            }
        }
        fclose(fp);
    }

    return NULL;
}

/* Revision History
 *
 * $Log: syscode.c,v $
 * Revision 1.4  2007/01/07 17:51:59  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.3  2005/05/25 22:39:50  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 17:36:13  dechavez
 * removed unnecessary includes, WIN32 port (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:31  dec
 * import existing IDA/NRTS sources
 *
 */
