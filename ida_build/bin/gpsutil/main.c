#pragma ident "$Id: main.c,v 1.2 2005/05/27 00:20:51 dechavez Exp $"
/*======================================================================
 *
 *  Read GPS NMEA time string from a serial port and print the
 *  decoded time stamp.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Author: David Chavez (dec@essw.com)
 *         Engineering Services & Software
 *         San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "util.h"
#include "oldttyio.h"
#include "sclk.h"

main(int argc, char **argv)
{
static char *port = "/dev/gps";
int speed = 9600;
struct sclk_param param;
struct sclk_tstamp tstamp;
SCLK *cp;

    if (argc == 3) {
        port  = argv[1];
        speed = atoi(argv[2]);
    } else if (argc != 1) {
        fprintf(stderr, "usage: %s [port speed]\n", argv[0]);
        fprintf(stderr, "default: %s %d\n", port, speed);
        exit(1);
    }

    if ((cp = sclk_open(port, speed, &param)) == (SCLK *) NULL) {
        perror(port);
        exit(1);
    }

    while (1) {
        if (sclk_gettime(cp, &tstamp) == 0) {
            printf("%s `%c' %d\n",
                util_lttostr(tstamp.time, 0), tstamp.qual, cp->dropped
            );
            cp->dropped = 0;
        } else {
            perror("sclk_gettime");
            exit(1);
        }
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2005/05/27 00:20:51  dechavez
 * using oldttyio
 *
 * Revision 1.1.1.1  2000/02/08 20:20:00  dec
 * import existing IDA/NRTS sources
 *
 */
