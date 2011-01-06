#pragma ident "$Id: sclk.h,v 1.1.1.1 2000/02/08 20:20:22 dec Exp $"

/*
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 */

#ifndef sclk_h_defined
#define sclk_h_defined

/* Constants */

#define SCLK_BUFLEN  64
#define SCLK_TO       5
#define SCLK_MAXTRY   5

/* Structure templates */

struct sclk_info {
    int fd;
    int to;
    int n;
    long dropped;
    char buf[SCLK_BUFLEN];
};

typedef struct sclk_info SCLK;

struct sclk_tstamp {
    time_t time;   /* Epoch time   */
    char   qual;   /* Quality code */
};

struct sclk_param {
    int not_yet_defined;
};

/* Function prototypes */

SCLK *sclk_open(char *port, int speed, struct sclk_param *param);
void  sclk_close(SCLK *cp);
int   sclk_gettime(SCLK *cp, struct sclk_tstamp *tstamp);

#endif

/* Revision History
 *
 * $Log: sclk.h,v $
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
