#pragma ident "$Id: fsf.c,v 1.1 2000/05/01 23:01:23 dec Exp $"
/*======================================================================
 *
 *  Forward space files.  It is the operators responsibility to be
 *  sure to specify the no-rewind device name in order to insure that
 *  the tape doesn't space to the desired location and then rewind.
 *
 *====================================================================*/
#include <stdio.h>
#include "mtio.h"

main(argc, argv)
int argc;
char *argv[];
{
int i, want, got;
TAPE *tp;
char *device;
struct mtget info;

    if (argc == 3) {
        device = argv[1];
        want   = atoi(argv[2]);
    } else {
        fprintf(stderr,"usage: %s device count\n", argv[0]);
        exit(1);
    }

    if ((tp = mtio_open(device, "r")) == (TAPE *) NULL) {
        fprintf(stderr,"%s: mtio_open: ", argv[0]);
        perror(device);
        exit(1);
    }

    if (mtio_status(tp, &info) == -1) {
        fprintf(stderr,"%s: mtio_status: ", argv[0]);
        perror(device);
        exit(1);
    }

	printf("before skip:\n");
    printf("type   = %d\n", info.mt_type);
    printf("dsreg  = %d\n", info.mt_dsreg);
    printf("erreg  = %d\n", info.mt_erreg);
    printf("resid  = %d\n", info.mt_resid);
    printf("fileno = %d\n", info.mt_fileno);
    printf("blkno  = %d\n", info.mt_blkno);
    printf("flags  = %d\n", info.mt_flags);
    printf("bf     = %d\n", info.mt_bf);

    if ((got = mtio_fsf(tp, want)) < 0) {
        fprintf(stderr,"%s: mtio_fsf: ", argv[0]);
    } else {
        printf("%s: %d files skipped\n", device, got);
    }

    if (mtio_status(tp, &info) == -1) {
        fprintf(stderr,"%s: mtio_status: ", argv[0]);
        perror(device);
        exit(1);
    }

	printf("after skip:\n");
    printf("type   = %d\n", info.mt_type);
    printf("dsreg  = %d\n", info.mt_dsreg);
    printf("erreg  = %d\n", info.mt_erreg);
    printf("resid  = %d\n", info.mt_resid);
    printf("fileno = %d\n", info.mt_fileno);
    printf("blkno  = %d\n", info.mt_blkno);
    printf("flags  = %d\n", info.mt_flags);
    printf("bf     = %d\n", info.mt_bf);
}

/* Revision History
 *
 * $Log: fsf.c,v $
 * Revision 1.1  2000/05/01 23:01:23  dec
 * import existing sources
 *
 */
