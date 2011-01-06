#pragma ident "$Id: main.c,v 1.3 2007/01/11 22:02:29 dechavez Exp $"
/*======================================================================
 *
 *  Find the index of an ISI disk loop record for a given seqno #
 *
 *====================================================================*/
#include "isi.h"
#include "isi/dl.h"
#include "util.h"

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ -v ] site { sig count | complete (hex only) }\n", myname);
    exit(1);
}

UINT32 utilStringToUINT32(char *string)
{
int base;

    if (string == NULL) {
        errno == EINVAL;
        return 0;
    }

    if (strncasecmp(string, "0x", strlen("0x")) == 0) {
        base = 16;
    } else {
        base = 10;
    }

    return (UINT32) strtol(string, NULL, base);
}

UINT64 utilStringToUINT64(char *string)
{
int base;

    if (string == NULL) {
        errno == EINVAL;
        return 0;
    }

    if (strncasecmp(string, "0x", strlen("0x")) == 0) {
        base = 16;
    } else {
        base = 10;
    }

    return (UINT32) strtoll(string, NULL, base);
}

int main (int argc, char **argv)
{
char *site = NULL;
ISI_SEQNO target;
ISI_GLOB glob;
ISI_DL *dl;
UINT32 i, index;
BOOL verbose = FALSE;
char buf[1024];

    if (argc == 1) help(argv[0]);
    if (strcmp(argv[1], "-v") == 0) {
        verbose = TRUE;
        index = 2;
    }

    switch (argc - index) {
      case 3:
        site = argv[index++];
        target.signature = utilStringToUINT32(argv[index++]);
        target.counter = utilStringToUINT64(argv[index++]);
        break;
      case 2:
        site = argv[index++];
        isiStringToSeqno(argv[index++], &target);
        break;
      default:
        fprintf(stderr, "%s: wrong number of arguments\n", argv[0]);
        help(argv[0]);
    }

    if (!isidlSetGlobalParameters(NULL, argv[0], &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if ((dl = isidlOpenDiskLoop(&glob, site, NULL, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "isidlOpenDiskLoop failed for site=%s\n", site);
        exit(1);
    }

    index = isidlSearchDiskLoopForSeqno(dl, &target, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
    if (index == ISI_UNDEFINED_INDEX) {
        fprintf(stderr, "no match found for site=%s, seqno=%s\n", site, isiSeqnoString(&target, buf));
        exit(1);
    }

    
    if (verbose) {
        printf("site=%s, seqno=%s, index=%lu\n", site, isiSeqnoString(&target, buf), index);
    } else {
        printf("%lu\n", index);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.3  2007/01/11 22:02:29  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.2  2006/03/13 23:20:56  dechavez
 * added support for specifying full 96bit seqno (hex only)
 *
 * Revision 1.1  2005/10/18 18:59:33  dechavez
 * initial release
 *
 */
