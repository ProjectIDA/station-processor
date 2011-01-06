#pragma ident "$Id: mtread.c,v 1.3 2004/06/24 18:56:28 dechavez Exp $"
/*======================================================================
 *
 *  Read from tape device supported by mtio library routines.
 *
 *====================================================================*/
#include "oldmtio.h"
#include "util.h"

#ifdef MSDOS
#define REAL_BIG unsigned char huge
#else
#define REAL_BIG char
#endif

static void help()
{
    fprintf(stderr,"usage: mtread device [bs=# count=# +/-v]\n");
    exit(1);
}

int main(int argc, char **argv)
{
long i;
long count = -1;
int verbose = 0;
int fno = 1;
int rno = 0;
int eof = 0;
long minlen, maxlen;
TAPE tp;
long bytes;
char *device;
REAL_BIG *buffer;
#ifdef MSDOS
long bufsiz = 10240;
#else
long bufsiz = 32768;
#endif

    if (argc < 2) help();

    device = argv[1];
#ifdef MSDOS
    if (strcmp(device, "0") == 0) {
        device = "/dev/rst00";
    } else if (strcmp(device, "1") == 0) {
        device = "/dev/rst01";
    } else if (strcmp(device, "2") == 0) {
        device = "/dev/rst02";
    } else if (strcmp(device, "3") == 0) {
        device = "/dev/rst03";
    } else if (strcmp(device, "4") == 0) {
        device = "/dev/rst04";
    } else if (strcmp(device, "5") == 0) {
        device = "/dev/rst05";
    } else if (strcmp(device, "6") == 0) {
        device = "/dev/rst06";
    } else if (strcmp(device, "7") == 0) {
        device = "/dev/rst07";
    }
#endif /* MSDOS */

    for (i = 2; i < argc; i++) {
        if (strncmp(argv[i], "bs=", strlen("bs=")) == 0) {
            bufsiz = util_atolk(argv[i] + strlen("bs="));
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = util_atolk(argv[i] + strlen("count="));
            if (count <= 0) {
                fprintf(stderr,"mtread: illegal count=\n", argv[i]);
                exit(1);
            }
        } else if (strcmp(argv[i], "+v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 0;
        } else {
            help();
        }
    }

    if ((buffer = (REAL_BIG *) malloc(bufsiz)) == NULL) {
        perror("mtread: malloc");
        exit(1);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("mtread: setmode");
        exit(1);
    }

    if ((tp = mtopen(device, "r")) == (TAPE) NULL) {
        fprintf(stderr,"mtread: mtopen");
        mterror(device);
        exit(1);
    }

    i = minlen = maxlen = 0;
    while (1) {
        if (i == count) {
            if (verbose) fprintf(stderr,"\n");
            mtclose(tp);
            exit(0);
        }
        bytes = mtread(tp, buffer, bufsiz); ++i;
        if (bytes < 0) {
            mterror("\nmtread: mtread");
#ifdef MSDOS
            if (bytes == SCSI_SMLREQ) {
                if (verbose) fprintf(stderr,"Program aborted.\n");
                exit(1);
            }
#endif
            fprintf(stderr,"Attempting to skip 1 record... ");
            if (mtfsr(tp, 1L) != 1) {
                mterror("failed");
                fprintf(stderr,"Attempting to skip 1 file... ");
                if (mtfsf(tp, 1) != 1) {
                    mterror("failed");
                    fprintf(stderr,"Aborting.\n");
                    exit(1);
                } else {
                    fprintf(stderr,"OK\n");
                    ++fno; rno = 0; --i;
                    minlen = 0;
                    maxlen = 0;
                }
            } else {
                fprintf(stderr,"OK\n");
                ++rno;
            }
        } else if (bytes == 0) {
            if (++eof < 2) {
                if (verbose) {
                    fprintf(stderr,"\rFile %d has ", fno);
                    fprintf(stderr,"%d records, ", rno);
                    fprintf(stderr,"min/max length = ");
                    fprintf(stderr,"%ld/%ld\n", minlen,maxlen);
                }
                ++fno; rno = 0; --i;
                minlen = 0;
                maxlen = 0;
            } else {
                if (count > 0 && verbose) {
                    fprintf(stderr,"\nEnd-of-data detected.\n");
                }
                exit(0);
            }
        } else {
            ++rno; eof = 0;
            if (rno == 1) {
                minlen = maxlen = bytes;
            } else {
                if (bytes < minlen) minlen = bytes;
                if (bytes > maxlen) maxlen = bytes;
            }
            if (verbose) {
                fprintf(stderr,"\rFile %d, ",    fno);
                fprintf(stderr,"record %d ",     rno);
                fprintf(stderr,"has %ld bytes.", bytes);
            }
            if (fwrite(buffer, sizeof(char), bytes, stdout) != bytes) {
                perror("mtread: fwrite");
                exit(1);
            }
        }
    }
}

/* Revision History
 *
 * $Log: mtread.c,v $
 * Revision 1.3  2004/06/24 18:56:28  dechavez
 * replace HUGE with REAL_BIG, since math.h is now included in platform.h and
 * it defines HUGE as MAXFLOAT (on at least one system)
 *
 * Revision 1.2  2003/12/10 06:31:23  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1  2000/05/01 22:55:38  dec
 * import existing sources
 *
 */
