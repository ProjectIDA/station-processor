#pragma ident "$Id: nrts_alpha.h,v 1.15 2008/09/11 16:42:20 dechavez Exp $"
#ifndef _nrts_alpha_h
#define _nrts_alpha_h

#include "nrts.h"
#include "util.h"
#include "alpha.h"

extern char *VersionIdentString;

#define RECASE_NONE  0
#define RECASE_UPPER 1
#define RECASE_LOWER 2

#ifdef USE_LYNKS_LIBDSS
#define USE_LYNKS_SPEXX
#define USE_LIBDSS
#endif

#ifdef USE_LUNA_LIBDSS
#define USE_CHRYSALIS_LUNA2
#define USE_LIBDSS
#endif

struct notify {
    char *address;
    char *subject;
    char *fname;
    time_t time;
    int flag;
};

#define DEF_ADDRESS "alphamgr@idahub.ucsd.edu"
#define DEF_WARN    300
#define SUBJECT     "%s Alpha data problem"
#define MESSAGE     "msgs/alpha"

#define ALPHA_NET     "IDA"
#define ALPHA_DEFCHAN "bhz,bhn,bhe"
#define TWENTY_MIN    1200000
#define DEF_NAP       30

#define changed(a,b) \
((a)->sint!=(b)->sint || (a)->wrdsiz!=(b)->wrdsiz || (a)->nsamp!=(b)->nsamp)

#define UNKNOWN 0x01
#define PRESENT 0x02
#define ABSENT  0x03

/*  The following should be constant for all data records serviced */

struct static_info {
    char site_name[NRTS_SNAMLEN+1]; /* station code            */
    float sint;                     /* nominal sample interval */
    float samprate;                 /* nominal sample rate     */
    int nsamp;                      /* number of samples       */
    int wrdsiz;                     /* bytes per sample        */
    int len;                        /* bytes per record        */
    char itype[3];                  /* input data type code    */
    char otype[3];                  /* output data type code   */
    double increment;               /* record length in secs   */
};

/* Function prototypes */

#ifdef __STDC__

void die(int);

int do_search(
    int,
    double,
    long,
    long,
    struct nrts_packet **
);

int exitcode(
    void
);

int getrec(
    int,
    double,
    struct nrts_packet **
);

void help(
    char *
);

struct nrts_packet *index_read(
    int,
    long
);

AlphaConn *init(
    int,
    char **,
    int *,
    int *,
    struct static_info *,
    int *,
    double *,
    struct notify *
);

int read_init(
    char *,
    char *,
    struct nrts_sys *,
    struct nrts_files *,
    int,
    IDA *,
    char *,
    int
);

#else

int catch_signal();
void die();
int do_search();
int exitcode();
int getrec();
void help();
struct nrts_packet *index_read();
AlphaConn *init();
int read_init();


#endif /* __STDC__ */

#endif /* _nrts_alpha_h */

/* Revision History
 *
 * $Log: nrts_alpha.h,v $
 * Revision 1.15  2008/09/11 16:42:20  dechavez
 * use VersionIdentString from ReleaseNotes.c instead of VERSION constant
 *
 * Revision 1.14  2006/02/10 02:43:29  dechavez
 * removed spurious ; from version string
 *
 * Revision 1.13  2006/02/10 02:09:41  dechavez
 * 3.0.0
 *
 * Revision 1.12  2006/02/10 02:04:50  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.11  2005/09/13 00:43:56  dechavez
 * 2.12.1
 *
 * Revision 1.10  2004/04/26 21:03:25  dechavez
 * MySQL support added (via dbspec instead of dbdir)
 *
 * Revision 1.9  2003/11/21 20:24:15  dechavez
 * updated prototypes
 *
 * Revision 1.8  2003/06/11 21:24:06  dechavez
 * fixed include order
 *
 * Revision 1.7  2003/03/13 23:19:20  dechavez
 * 2.11.0 (5 char mapped to 3 char)
 *
 * Revision 1.6  2002/02/11 17:32:36  dec
 * 2.10.5
 *
 * Revision 1.5  2001/04/23 15:27:52  dec
 * 2.10.4
 *
 * Revision 1.4  2001/01/31 22:57:04  dec
 * 2.10.3
 *
 * Revision 1.3  2001/01/11 17:39:52  dec
 * Add sname= option to allow the user to force the output station name.
 *
 * Revision 1.2  2000/03/09 19:58:53  dec
 * Added support for Luna2, Makefile left for unauthenticated builds
 *
 * Revision 1.1.1.1  2000/02/08 20:20:10  dec
 * import existing IDA/NRTS sources
 *
 */
