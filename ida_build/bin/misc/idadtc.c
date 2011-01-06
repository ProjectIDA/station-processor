#pragma ident "$Id: idadtc.c,v 1.2 2003/12/10 06:31:22 dechavez Exp $"
/*======================================================================
 *
 *  Dump an IDA data disk to tape.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include "util.h"
#include "mtio.h"

#ifdef SOLARIS
#define IDEV "/dev/rdsk/c0t0d0s2"
#define ODEV "/dev/rmt/0lb"
#else
#define IDEV "/dev/rsd3c"
#define ODEV "/dev/rst2"
#endif

#define IBS  512
#define OBS  1024
#define REWIND  1
#define VERBOSE 2

char *myname;

void help(badarg)
char *badarg;
{
    fprintf(stderr, "\n");
    if (strncmp(badarg, "-h", strlen("-h")) != 0) {
        fprintf(stderr, "%s: unrecognized argument `%s'\n", myname, badarg);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "usage: %s [options]\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "if=device   => input device        (default = %s)\n",
        IDEV
    );
    fprintf(stderr, "of=device   => output device       (default = %s)\n",
        ODEV
    );
    fprintf(stderr, "ibs=val     => input block size    (default = %d)\n",
        IBS
    );
    fprintf(stderr, "obs=val     => output block size   (default = %d)\n",
        OBS
    );
    fprintf(stderr, "verbose=val => log level (0-3)     (default = %d)\n",
        VERBOSE
    );
    fprintf(stderr, "rewind=val  => initial rewind flag (default = %d)\n",
        REWIND
    );
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{
TAPE *ofd;
int ifd;
long i, csp, rno = 0;
long sec, nsec, nread, hr, mn, sc;
long ibs = IBS;
long obs = OBS;
long nout;
int rewind  = REWIND;
int verbose = VERBOSE;
long orec, num, total;
char *ptr, buffer[OBS];
static char *idev = IDEV;
static char *odev = ODEV;
time_t beg, end, elapsed;
char *version = "idadtc (05/14/97 version)";

    assert(IBS < OBS);
    myname = argv[0];

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            idev = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "of=", strlen("of=")) == 0) {
            odev = argv[i] + strlen("of=");
        } else if (strncmp(argv[i], "ibs=", strlen("ibs=")) == 0) {
            ibs = util_atolk(argv[i]+strlen("ibs="));
        } else if (strncmp(argv[i], "obs=", strlen("obs=")) == 0) {
            obs = util_atolk(argv[i]+strlen("obs="));
        } else if (strncmp(argv[i], "verbose=", strlen("verbose=")) == 0) {
            verbose = util_atolk(argv[i]+strlen("verbose="));
        } else if (strncmp(argv[i], "rewind=", strlen("rewind=")) == 0) {
            rewind = util_atolk(argv[i]+strlen("rewind="));
        } else {
            help(argv[i]);
        }
    }

    if (ibs < IBS) {
        fprintf(stderr, "%s: ibs must not be less than %ld\n", myname, IBS);
        exit(1);
    }

    if (obs < ibs) {
        fprintf(stderr, "%s: obs must not be less than ibs\n", myname);
        exit(1);
    }

    if (obs < IBS) {
        fprintf(stderr, "%s: obs must not be less than %ld\n", myname, IBS);
        exit(1);
    }

    if (verbose) {
        printf("%s\n", version);
        printf("input:  %s\n", idev);
        printf("output: %s\n", odev);
        printf("ibs:    %ld bytes\n", ibs);
        printf("obs:    %ld bytes\n", obs);
        fflush(stdout);
    }

    if ((ifd = open(idev, O_RDONLY)) < 0) {
        fprintf(stderr, "%s: open: ", myname);
        perror(idev);
        exit(1);
    }

    if ((ofd = mtio_open(odev, "w")) == (TAPE *) NULL) {
        fprintf(stderr, "%s: mtio_open: ", myname);
        perror(odev);
        exit(2);
    }

    if (rewind) {
        if (verbose) printf("rewinding %s...", odev); fflush(stdout);
        for (i = 0; i < 2; i++) {
            if (mtio_rew(ofd) != 0) {
                fprintf(stderr, "\n%s: mtio_rew: ", myname);
                perror(odev);
                exit(3);
            }
        }
        printf(" done\n");
        fflush(stdout);
    }

    for (i = 0; i < 3; i++) {
        if ((nread = read(ifd, buffer, IBS)) != IBS) {
            fprintf(stderr, "%s: read: ", myname);
            perror(idev);
            exit(4);
        }
    }

    memcpy(&csp, buffer + 4, 4);
    if (util_order() != BIG_ENDIAN_ORDER) util_lswap(&csp, 1);
    nsec = csp - 4;

    if ((nread = read(ifd, buffer, ibs)) != ibs) {
        fprintf(stderr, "%s: read: ", myname);
        perror(idev);
        exit(5);
    }

    if (verbose) {
        printf("Reading %ld sectors (%ld data logger records)...\n",
            nsec, nsec*ibs / 1024
        );
        printf("%ld output records expected\n", (nout = nsec*ibs / obs));
        fflush(stdout);
        time(&beg);
    }

    ptr = buffer;
    num = 0;
    orec = 0;
    total = 0;
    for (sec = 1; sec <= nsec; sec++) {
        if (verbose > 2) {
            printf("reading %ld-byte record no. %ld\r", ibs, sec);
            fflush(stdout);
        }
        if ((nread = read(ifd, ptr, ibs)) != ibs) {
            fprintf(stderr, "%s: read: ", myname);
            perror(idev);
            exit(6);
        }
        if ((num += nread) == obs) {
            if (verbose > 1 && obs > 1024) {
                if (verbose > 2) printf("\n");
                printf("writing %ld-byte record no. %ld", obs, orec+1);
                if (verbose > 2) {
                    printf(" (%ld more to go)          ", nout - orec - 1);
                    printf("\n");
                } else {
                    printf("\r");
                }
                fflush(stdout);
            }
            if (mtio_write(ofd, buffer, obs) != obs) {
                fprintf(stderr, "%s: mtio_write: ", myname);
                perror(odev);
                exit(7);
            }
            ptr = buffer;
            num = 0;
            ++orec;
        } else {
            ptr += nread;
        }
        total += nread;
    }

    if (num != 0) {
        if (mtio_write(ofd, buffer, num) != num) {
            fprintf(stderr, "%s: mtio_write: ", myname);
            perror(odev);
            exit(8);
        }
        if (verbose) {
            printf("%ld %ld-byte + 1 %ld-byte records written to tape\n",
                orec, obs, num
            );
            fflush(stdout);
        }
    } else {
        if (verbose) {
            printf("%ld %ld-byte records written to tape\n", orec, obs);
            fflush(stdout);
        }
    }

    if (verbose) {
        time(&end);
        sc = end - beg;
        hr  = (sc - (sc % 3600)) / 3600;
        sc -= hr * 3600;
        mn = (sc - (sc % 60)) / 60;
        sc -= mn * 60;
        printf("Total: %ld bytes copied\n", total);
        printf("Began: %s", ctime(&beg));
        printf("Ended: %s", ctime(&end));
        printf("Elapsed time: %2.2d:%2.2d:%2.2d\n", hr, mn, sc);
        fflush(stdout);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: idadtc.c,v $
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
