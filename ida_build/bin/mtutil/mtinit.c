#pragma ident "$Id: mtinit.c,v 1.2 2005/06/02 17:37:17 dechavez Exp $"
#include <stdio.h>
#include "mtio.h"

main(argc, argv)
int argc;
char *argv[];
{
TAPE *tp;

    if (argc != 2) {
        fprintf(stderr, "usage: %s tape_device\n", argv[0]);
        exit(1);
    }

    if (!utilQuery("Are you sure? ")) exit(0);

    if ((tp = mtio_open(argv[1], "w")) == (TAPE *) NULL) {
        fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
        perror("mtio_open");
        exit(1);
    }

    if (mtio_rew(tp) != 0) {
        fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
        perror("mtio_rew");
        exit(1);
    }

    if (mtio_weof(tp, 10) != 0) {
        fprintf(stderr, "%s: %s: ", argv[0], argv[1]);
        perror("mtio_weof");
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: mtinit.c,v $
 * Revision 1.2  2005/06/02 17:37:17  dechavez
 * replace util_query with utilQuery
 *
 * Revision 1.1  2000/05/01 23:01:23  dec
 * import existing sources
 *
 */
