#pragma ident "$Id: status.c,v 1.2 2005/06/02 17:37:17 dechavez Exp $"
/*======================================================================
 *
 *  Status query.
 *
 *====================================================================*/
#include <stdio.h>
#include "mtio.h"
#include "util.h"

main(argc, argv)
int argc;
char *argv[];
{
int i, want, got;
TAPE *tp;
char *device;
struct mtget info;

    if (argc == 2) {
        device = argv[1];
    } else {
        fprintf(stderr,"usage: %s device\n", argv[0]);
        exit(1);
    }

    if ((tp = mtio_open(device, "w")) == (TAPE *) NULL) {
        fprintf(stderr,"%s: mtio_open: ", argv[0]);
        perror(device);
        exit(1);
    }

	do {
    	if (mtio_status(tp, &info) == -1) {
        	fprintf(stderr,"%s: mtio_status: ", argv[0]);
        	perror(device);
        	exit(1);
    	}
    	printf("type   = %d\n", info.mt_type);
    	printf("dsreg  = %d\n", info.mt_dsreg);
    	printf("erreg  = %d\n", info.mt_erreg);
    	printf("resid  = %d\n", info.mt_resid);
    	printf("fileno = %d\n", info.mt_fileno);
    	printf("blkno  = %d\n", info.mt_blkno);
    	printf("flags  = %d\n", info.mt_flags);
    	printf("bf     = %d\n", info.mt_bf);
	} while (utilQuery("Repeat? "));

    exit(0);

}

/* Revision History
 *
 * $Log: status.c,v $
 * Revision 1.2  2005/06/02 17:37:17  dechavez
 * replace util_query with utilQuery
 *
 * Revision 1.1  2000/05/01 23:01:23  dec
 * import existing sources
 *
 */
