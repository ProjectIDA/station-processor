/* @(#)seed.h	1.5 02/02/97 */
/*
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *
 */

#ifndef seed_h_included
#define seed_h_included

/* Selected name lengths (all less trailing NULL) */

#define SEED_SNAMLEN 5  /* station name        */
#define SEED_CNAMLEN 3  /* channel code        */
#define SEED_LNAMLEN 2  /* location identifier */
#define SEED_NNAMLEN 2  /* network code        */

/* Selected encoding formats */

#define SEED_INT_16 1  /* 16 bit integers                      */
#define SEED_INT_24 2  /* 24 bit integers                      */
#define SEED_INT_32 3  /* 32 bit integers                      */
#define SEED_IEEE_F 4  /* IEEE floading point                  */
#define SEED_IEEE_D 5  /* IEEE double precision floating point */
#define SEED_STEIM1 10 /* Steim (1) compression                */
#define SEED_STEIM2 11 /* Steim (2) compression                */

#define SEED_LTL_ENDIAN 0
#define SEED_BIG_ENDIAN 1

/* Selected flag masks */

#define SEED_FLAG_START 0x08  /* B00001000 = start of time series */
#define SEED_FLAG_STOP  0x10  /* B00010000 = end   of time series */

/* Misc. constants */

#define SEED_MINRECEXP  8
#define SEED_MAXRECEXP 16

/* Structure templates */

struct seed_fsdh {
    long    seqno;                 /* sequence number                  */
    char    staid[SEED_SNAMLEN+1]; /* station identifier code          */
    char    locid[SEED_LNAMLEN+1]; /* location identifier              */
    char    chnid[SEED_CNAMLEN+1]; /* channel identifier               */
    char    netid[SEED_NNAMLEN+1]; /* network code                     */
    double  start;                 /* record start time                */
    short   nsamp;                 /* number of samples                */
    short   srfact;                /* sample rate factor               */
    short   srmult;                /* sample rate multiplier           */
    char    active;                /* activity flags                   */
    char    ioclck;                /* I/O and clock flags              */
    char    qual;                  /* data quality flags               */
    char    more;                  /* number of blockettes that follow */
    long    tcorr;                 /* time correction                  */
    short   bod;                   /* offset to begining of data       */
    short   first;                 /* first blockette                  */
};

struct seed_b1000 {
    short next;     /* next blockette's byte number */
    char  format;   /* encoding format              */
    char  order;    /* word order                   */
    char  length;   /* record length                */
};

/* Function prototypes */

char *seed_dtconv(
    char *,
    double
);

void seed_fsdh(
    struct seed_fsdh *,
    char *
);

long seed_mini(
    long *,
    long,
    struct seed_fsdh *,
    char *,
    int,
    long *
);

void seed_load1000(
    char *,
    struct seed_b1000 *,
    unsigned long
);

void seed_loadfsdh(
    char *,
    struct seed_fsdh *,
    unsigned long
);

double seed_makesint(
    short,
    short
);

void seed_sintsplit(
    double,
    short *,
    short *
);

double seed_ttodt(
    char *
);

#endif
