#pragma ident "$Id: main.c,v 1.7 2007/01/11 22:02:25 dechavez Exp $"
/*======================================================================
 *
 *  List ISI disk loop headers
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"

extern char *VersionIdentString;
#define DEFAULT_LOG "-"
#define DEFAULT_HOME   ISI_DEFAULT_BASE_DIR

static void help(char *myname)
{
    printf("%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ options ] [ site site ... ]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "-v      => verbose (include NRTS report)\n");
    fprintf(stderr, "db=spec => database specifier\n");
    fprintf(stderr, "\n");
    exit(1);
}

int main (int argc, char **argv)
{
int i;
char *dbspec = NULL;
LNKLST sitelist, *list = NULL;
ISI_GLOB glob;
BOOL verbose = FALSE;
ISI_DL_MASTER *master;

    listInit(&sitelist);
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else {
            list = &sitelist;
            if (!listAppend(list, argv[i], strlen(argv[i])+1)) {
                perror("listAppend");
                exit(1);
            }
        }
    }
    if (list != NULL && !listSetArrayView(list)) {
        perror("listSetArrayView");
        exit(1);
    }

    if (!isidlSetGlobalParameters(dbspec, argv[0], &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if ((master = isidlOpenDiskLoopSet(list, &glob, NULL, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "unable to open disk loop(s)\n");
        exit(1);
    }

    for (i = 0; i < master->ndl; i++) {
        if (i) printf("\n");
        isidlPrintDL(stdout, master->dl[i]);
        if (verbose) nrtsPrintDL(stdout, master->dl[i]->nrts);
    }
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.7  2007/01/11 22:02:25  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.6  2006/03/13 23:12:32  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.5  2006/02/14 17:05:02  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.4  2006/02/10 01:08:37  dechavez
 * removed unused includes
 *
 * Revision 1.3  2005/07/26 17:15:37  dechavez
 * Only print NRTS report when -v option is given
 *
 * Revision 1.2  2005/07/26 00:51:14  dechavez
 * Added ISI_GLOB and NRTS disk loop support
 *
 * Revision 1.1  2005/06/30 01:36:35  dechavez
 * initial release
 *
 */
