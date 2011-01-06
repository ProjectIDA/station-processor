#pragma ident "$Id: revcode.c,v 1.5 2007/01/11 22:02:32 dechavez Exp $"
/*======================================================================
 *
 *  Print description of all or selected locally defined IDA rev codes.
 *
 *====================================================================*/
#include "ida.h"
#include "isi/db.h"

int main(int argc, char **argv)
{
int i, val;
DBIO *db;
LNKLST *list;
IDA_REV *rev, result;
char *spec = NULL;

    if (argc > 1) {
        for (i = 0; i < argc; i++) {
            if (strncmp(argv[i], "db=", strlen("db=")) == 0) spec = argv[i] + strlen("db=");
        }
    }

    if ((db = dbioOpen(spec, NULL)) == NULL) {
        fprintf(stderr, "%s: dbioOpen: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    printf("database = %s\n", db->dbid);

    if ((spec == NULL && argc > 1) || (spec != NULL && argc > 2)) {
        for (i = 1; i < argc; i++) {
            if (strncmp(argv[i], "db=", strlen("db=")) != 0) {
                val = atoi(argv[i]);
                if (isidbLookupRev(db, val, &result)) {
                    printf("Rev %d = %s\n", result.value, result.description);
                } else {
                    printf("Rev %s is not defined.\n", argv[i]);
                }
            }
        }
    } else {
        if ((list = isidbRevList(db)) == NULL) {
            fprintf(stderr, "%s: can't load revision codes from database\n", argv[0]);
            exit(0);
        }
        for (i = 0; i < list->count; i++) {
            rev = (IDA_REV *) list->array[i];
            printf("Rev %d = %s\n", rev->value, rev->description);
        }
    }
}

/* Revision History
 *
 * $Log: revcode.c,v $
 * Revision 1.5  2007/01/11 22:02:32  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.4  2006/02/14 17:05:03  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.3  2006/02/10 01:47:18  dechavez
 * dbio support
 *
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
