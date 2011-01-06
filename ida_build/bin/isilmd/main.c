#pragma ident "$Id: main.c,v 1.3 2007/01/11 22:02:27 dechavez Exp $"
/*======================================================================
 *
 *  List ISI disk loop metadata
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"
#include "qdplus.h"

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
    fprintf(stderr, "-v      => verbose (print all metadata details)\n");
    fprintf(stderr, "db=spec => database specifier\n");
    fprintf(stderr, "\n");
    exit(1);
}

static void process(ISI_DL *dl, BOOL verbose)
{
char *base;
LNKLST *result;

    base = dl->path.meta;
    if ((result = qdplusReadMetaData(base)) == NULL) {
        fprintf(stderr, "qdplusReadMetaData failed for '%s': %s\n", base, strerror(errno));
        exit(1);
    }

    printf("%d instruments found for site '%s'\n", result->count, dl->sys->site);
    qdplusPrintMetaDataList(stdout, result, verbose);
    qdplusDestroyMetaDataList(result);
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
        if (master->dl[i]->flags & ISI_DL_FLAGS_HAVE_META) {
            process(master->dl[i], verbose);
        } else {
            printf("No '%s' metadata\n", master->dl[i]->sys->site);
        }
    }
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2007/01/11 22:02:27  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.2  2006/12/13 22:02:09  dechavez
 * use qdplusPrintMetaDataList() instead of explicity walking through list
 *
 * Revision 1.1  2006/12/12 23:09:32  dechavez
 * initial release
 *
 */
