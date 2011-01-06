#pragma ident "$Id: systems.c,v 1.1 2006/06/02 21:13:24 dechavez Exp $"
#include <sys/param.h>
#include <signal.h>
#include <time.h>
#include "nrts_mon.h"

extern char **Systems;
extern int nsys;

char *LoadSystems(char *home, char *defsys)
{
int i, unused;
LNKLST list;
FILE *fp;
char *name;
char buffer[80];
char path[MAXPATHLEN+1];
NRTS_FILES *file;
char *retval = NULL;

    listInit(&list);

    sprintf(path, "%s/etc/Systems", home);
    if ((fp = fopen(path, "r")) == NULL) {
        perror(path);
        exit(1);
    }

    while (utilGetLine(fp, buffer, MAXPATHLEN, '#', &unused) == 0) {
        file = nrts_files(&home, buffer);
        if (utilFileExists(file->sys)) {
            if (!listAppend(&list, buffer, strlen(buffer)+1)) {
                listDestroy(&list);
                fclose(fp);
                perror("listAppend");
                exit(1);
            }
        }
    }
    fclose(fp);

    if (!listSetArrayView(&list)) {
        perror("listSetArrayView");
        exit(1);
    }

    if (list.count < 1) {
        fprintf(stderr, "ERROR: no valid NRTS disk loops listed in %s\n", path);
        exit(1);
    }
    nsys = list.count;

    if ((Systems = (char **) malloc(nsys * sizeof(char *))) == NULL) {
        perror("malloc");
        exit(1);
    }

    for (i = 0; i < nsys; i++) {
        if ((Systems[i] = strdup((char *) list.array[i])) == NULL) {
            perror("strdup");
            exit(1);
        }
        if (defsys != NULL && strcmp(Systems[i], defsys) == 0) retval = Systems[i];
    }

    listDestroy(&list);

    return retval == NULL ? Systems[0] : retval;

}

/* Revision History
 *
 * $Log: systems.c,v $
 * Revision 1.1  2006/06/02 21:13:24  dechavez
 * initial release
 *
 * Revision 1.4  2004/01/29 18:54:04  dechavez
 * initializations to calm purify builds
 *
 * Revision 1.3  2001/05/20 18:43:52  dec
 * remove uneeded includes
 *
 * Revision 1.2  2000/09/19 23:17:35  nobody
 * Increase number of stations and adjust display accordingly
 *
 * Revision 1.1.1.1  2000/02/08 20:20:12  dec
 * import existing IDA/NRTS sources
 *
 */
