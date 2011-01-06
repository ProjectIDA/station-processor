#pragma ident "$Id: i10dmx.h,v 1.12 2007/09/25 21:03:30 dechavez Exp $"
/*======================================================================
 *
 *  Include file of constants, type definitions, function declarations
 *
 *====================================================================*/
#ifndef idadmx_h_defined
#define idadmx_h_defined

extern char *VersionIdentString;

#include "cssio.h"
#include "util.h"
#include "ida10.h"
#include "mio.h"
#include "q330.h"

#define STANDARD   0
#define SUPPRESSED 1

/*  Exit codes  */

#define NORMAL     0
#define INIT_ERROR 1
#define ABORT      2
#define ON_SIGNAL  3

/*  Structure templates  */

struct wflist {
    char   name[MAXPATHLEN+2]; /* .w file this wfdisc rec refers to */
    struct wfdisc28 wfdisc;    /* the wfdisc record                 */
    struct wflist *next;       /* pointer to next element           */
};

struct counter {
    UINT32 rec;       /* number of records which have been read  */
    UINT32 stream[IDA10_MAXSTREAMS]; /* per stream count        */
};

/*  Function declarations  */

void InitDetector(char *cfgpath, Q330_DETECTOR *engine);
void RunDetector(IDA10_TS *src, struct counter *count);
void exitcode(void);
void die(int status);
void help(void);
void init(int  argc, char **argv, struct counter *count, int *verbose, FILE **fp);
void init_msglog(char *input,  struct counter *countptr);
void close_msglog(void);
void logmsg(char *string);
void nameinit(char *outdir, char *sta);
char *pathname(char *defsname, char *channel);
char *logname(char *defsname);
char *ttagname(void);
char *crnt_dname(void);
char *crnt_fname(void);
char *basedir(void);
char *sname(char *defsname);
int rdrec(FILE *fp, IDA10_TS *ts, struct counter *count, int verbose);
int tsrec_ok(IDA10_TS *ts, struct counter *count);
void init_wfdisc(int format, struct counter *countptr, char *dbspec);
void wrt_wfdisc(void);
struct wflist *get_wfdisc(char *path, BufferedStream *fp, IDA10_TSHDR *hdr);
void wrtdat(IDA10_TS *ts, struct counter *count);
void init_ttag(void);
void store_ttag(IDA10_TS *ts);
void unpackII(UINT8 *buf, long recno);
void unpackLM(UINT8 *buf, long recno);

#endif

/* Revision History
 *
 * $Log: i10dmx.h,v $
 * Revision 1.12  2007/09/25 21:03:30  dechavez
 * added detector support
 *
 * Revision 1.11  2006/11/14 00:04:39  dechavez
 * updated prototypes
 *
 * Revision 1.10  2006/02/09 20:12:38  dechavez
 * libisidb database support
 *
 * Revision 1.9  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.8  2002/05/15 18:22:34  dec
 * use VersionIdentString variable instead of #defined VERSION, update
 * prototypes
 *
 * Revision 1.7  2002/03/15 23:04:21  dec
 * 1.3.0
 *
 * Revision 1.6  2002/01/25 19:25:16  dec
 * change counter fields to UINT32's
 * update version ident to 1.2.2
 *
 * Revision 1.5  2001/12/20 22:24:50  dec
 * 1.2.1
 *
 * Revision 1.4  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.3  2000/11/06 23:17:43  dec
 * Release 1.1.1
 *
 * Revision 1.2  2000/02/18 06:39:15  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
