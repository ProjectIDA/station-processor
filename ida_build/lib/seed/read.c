#pragma ident "$Id: read.c,v 1.6 2007/01/11 17:48:03 dechavez Exp $"
/*======================================================================
 *
 *  Read a mini seed record, and decode it a bit.  Initially, we only
 *  support uncompressed 16 and 32 bit data plus Steim 1 and Steim 2
 *  compressed formats only.
 *
 *  Reject everything except for data packets with recognized contents.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <math.h>
#include "seed.h"
#include "util.h"

#define TEST_LOCID(locid)                                      \
(                                                              \
   strcmp((locid), "00") == 0 || strcmp((locid), "  ") == 0 || \
   strcmp((locid), " 0") == 0 || strcmp((locid), "0 ") == 0 || \
   strcmp((locid),  "0") == 0 || strcmp((locid),  " ") == 0 || \
   strcmp((locid),   "") == 0                                  \
)

static int logReturn(char *fid, int retval)
{
    util_log(3, "%s: return %d", fid, retval);
    return retval;
}

int seed_readmini(
    int fd,
    struct seed_minipacket *output,
    char *buffer,
    size_t blksiz,
    int to
) {
char *ptr;
int nread, status;
long i, keep;
short *sdata;
BOOL done;
#ifdef DEBUG_TEST
FILE *fp;
#endif
static char *fid = "seed_readmini";

/* Get a data packet, discarding "empty" reads from LISS and LOG packets */

    do {

    /* get a packet */

        if (to > 0) {
            done = FALSE;
            while (!done) {
                util_log(3, "%s: call util_read for %d bytes", fid, blksiz);
                nread = util_read(fd, buffer, blksiz, to);
                util_log(3, "%s: util_read returns %d bytes", fid, nread);
                if (nread < 0) {
                    util_log(1, "%s: util_read: error %d: %s",
                        fid, nread, strerror(errno)
                    );
                    return logReturn(fid, -1);
                } else if (nread != (int) blksiz) {
                    util_log(1, "%s: read(1): expect %d, got %d",
                        fid, blksiz, nread
                    );
                    return logReturn(fid, -2);
                }
                if (buffer[6] == 'D') {
                    util_log(3, "%s: call seed_minihdr", fid);
                    status = seed_minihdr(output, buffer);
                    util_log(3, "%s: seed_minihdr returns %d", fid, status);
                    if (status == 0) {
                        done = TRUE;
                    } else {
                        util_log(3, "%s: packet ignored", fid);
                    }
                } else {
                    util_log(3, "%s: buffer[6] != 'D', packet ignored", fid);
                }
            }
        } else {
            do {
                if ((nread = read(fd, buffer, blksiz)) != (int) blksiz) {
                    util_log(1, "%s: read: expect %d, got %d",
                        fid, blksiz, nread
                    );
                    if (nread == 0) {
                        return logReturn(fid, -3);
                    } else {
                        return logReturn(fid, -4);
                    }
                }
            } while (buffer[6] != 'D' || seed_minihdr(output, buffer) != 0);
        }
        util_log(3, "%s: packet read/decoded OK", fid);

    /* see if we want it */

        keep = 1;
        if (keep && strcasecmp(output->cname, "ACE") == 0) keep = 0;
        if (keep && strcasecmp(output->cname, "LOG") == 0) keep = 0;
        if (keep && output->fsdh.nsamp < 1               ) keep = 0;
        if (keep && output->b1000.format == 0            ) keep = 0;
        if (keep && TEST_LOCID(output->fsdh.locid) == 0  ) keep = 0;

        if (keep == 0) {
            util_log(2, "%s: dump: cname=%s nsamp=%d format=%d locid='%s'",
                fid, output->cname, output->fsdh.nsamp,
                output->b1000.format, output->fsdh.locid
            );
        }

    } while (keep == 0);

/* Decode the data */

    if (output->datlen < (size_t) output->fsdh.nsamp) {
        util_log(1, "%s: buffer space problem: %d < %d",
            fid, output->datlen, output->fsdh.nsamp
        );
        return logReturn(fid, -5);
    }

    ptr = buffer + output->fsdh.bod;

    switch (output->b1000.format) {

      case SEED_STEIM1:
        util_log(3, "%s: calling util_dsteim1", fid);
        status = util_dsteim1((long *) output->data, output->datlen, ptr, output->srclen,
            output->order, output->nsamp
        );
        util_log(3, "%s: util_dsteim1 returns %d", fid, status);
        if (status != 0) {
            util_log(1, "%s: util_dsteim1: status %d", fid, status);
            return logReturn(fid, -6);
        }
        break;

      case SEED_STEIM2:
        util_log(3, "%s: calling util_dsteim2", fid);
        status = util_dsteim2((long *) output->data, output->datlen, ptr, output->srclen,
            output->order, output->nsamp
        );
        util_log(3, "%s: util_dsteim2 returns %d", fid, status);
        if (status != 0) {
            util_log(1, "%s: util_dsteim2: status %d", fid, status);
            return logReturn(fid, -7);
        }
        break;

      case SEED_INT_32:
        util_log(3, "%s: data consists of %d 32 bit integers", fid, output->nsamp);
        memcpy(output->data, (long *) ptr, output->nsamp);
        if (output->fsdh.swap) util_lswap(output->data, output->nsamp);
        break;

      case SEED_INT_16:
        util_log(3, "%s: data consists of %d 16 bit integers", fid, output->nsamp);
        sdata = (short *) ptr;
        for (i = 0; i < output->nsamp; i++) {
            output->data[i] = (long) sdata[i];
        }
        if (output->fsdh.swap) util_lswap(output->data, output->nsamp);
        break;

      default:
        util_log(1, "%s: unsupported format `%d'",
            fid, output->b1000.format
        );
#ifdef DEBUG_TEST
        if ((fp = fopen("bad.data", "w")) != (FILE *) NULL) {
            fwrite(buffer, blksiz, sizeof(char), fp);
            fclose(fp);
        }
        fprintf(stderr, "guilty packet written to `bad.data'\n");
        fprintf(stderr, "%s/%s/%s %s %.3lf %4ld %8ld %8ld\n",
            output->sname, output->cname, output->nname,
            util_dttostr(output->beg, 0), output->sint, output->nsamp,
            output->data[0], output->data[output->nsamp-1]
        );
#endif
        return logReturn(fid, -8);
    }

    return logReturn(fid, 0);
}

#ifdef DEBUG_TEST

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void help(char *myname, int status)
{
    fprintf(stderr, "usage: %s [bs=blksiz] < stdin\n", myname);
    fprintf(stderr, "       %s [bs=blksiz host=hostname ", myname);
    fprintf(stderr, "port=portnumber to=timeout]\n");
    fprintf(stderr, "       %s [bs=blksiz file=filename\n", myname);
    exit(status);
}

#define FROM_STDIN  0
#define FROM_SOCKET 1
#define FROM_FILE   2

main(int argc, char **argv)
{
FILE *fp;
int i, fd, count, status, source = FROM_STDIN;
char *buffer = (char *) NULL;
char *host   = (char *) NULL;
char *file   = (char *) NULL;
char *log    = (char *) NULL;
int debug    = 0;
int port = -1;
int to   = 60;
size_t blksiz = 0;
struct seed_minipacket packet;

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "bs=", strlen("bs=")) == 0) {
            blksiz = atoi(argv[i] + strlen("bs="));
        } else if (strncasecmp(argv[i], "host=", strlen("host=")) == 0) {
            host = argv[i] + strlen("host=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            to = atoi(argv[i] + strlen("to="));
        } else if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncasecmp(argv[i], "file=", strlen("file=")) == 0) {
            file = argv[i] + strlen("host=");
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else {
            help(argv[0], 0);
        }
    }
    if (host != (char *) NULL && file != (char *) NULL) {
        fprintf(stderr, "%s: cannot specify both host and file!", argv[0]);
        help(argv[0], 1);
    }

    if (host != (char *) NULL) {
        if (!blksiz) blksiz = 512;
        if (port < 0) {
            fprintf(stderr, "%s: host requires port\n", argv[0]);
            help(argv[0], 2);
        }
        source = FROM_SOCKET;
    } else {
        to = 0;
        if (!blksiz) blksiz = 4096;
        if (file != (char *) NULL) source = FROM_FILE;
    }

    if (debug) util_logopen(log, 1, 9, debug, NULL, argv[0]);

    fprintf(stderr, "Blocksize = %d\n", blksiz);
    buffer = malloc(blksiz);
    packet.datlen = 2 * blksiz;
    packet.data = (long *) malloc(packet.datlen * sizeof(long));
    if (buffer == (char *) NULL || packet.data == (long *) NULL) {
        perror("malloc");
        exit(1);
    }

    switch (source) {
      case FROM_FILE:
        fprintf(stderr, "Reading from `%s'\n", file);
        if ((fp = fopen(file, "r")) == (FILE *) NULL) {
            perror(file);
            exit(1);
        }
        fd = fileno(fp);
        break;
      case FROM_SOCKET:
        fprintf(stderr, "Reading from host %s, port %d\n", host, port);
        if ((fd = util_connect(host, NULL, port, "tcp", 0, 0)) < 0) {
            perror("util_connect");
            exit(1);
        }
        break;
      default:
        fprintf(stderr, "Reading from standard input\n");
        fd = fileno(stdin);
    }

    count = 0;
    while ((status = seed_readmini(fd, &packet, buffer, blksiz, to)) == 0) {
        fprintf(stderr,"Block %3d: ", ++count);
        fprintf(stderr, "%s/%s/%s %s %.3lf %4ld %8ld %8ld\n",
            packet.sname, packet.cname, packet.nname,
            util_dttostr(packet.beg, 0), packet.sint, packet.nsamp,
            packet.data[0], packet.data[packet.nsamp-1]
        );
        fwrite(buffer, blksiz, 1, stdout); fflush(stdout);
    }
    fprintf(stderr, "seed_readmini status = %d\n", status);
    exit(0);
}
#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.6  2007/01/11 17:48:03  dechavez
 * added platform.h and stdtypes.h stuff
 *
 * Revision 1.5  2005/05/25 22:40:22  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.4  2000/10/06 19:55:49  dec
 * added debugging trace statements
 *
 * Revision 1.3  2000/09/21 22:22:06  nobody
 * return on util_read errors
 *
 * Revision 1.2  2000/09/21 22:05:43  nobody
 * ignore decode errors (most likely these are log records anyway)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
