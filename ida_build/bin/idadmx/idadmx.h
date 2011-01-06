#pragma ident "$Id: idadmx.h,v 1.10 2006/02/09 20:14:39 dechavez Exp $"
/*======================================================================
 *
 *  Include file of constants, type definitions, function declarations
 *
 *====================================================================*/
#ifndef idadmx_h_defined
#define idadmx_h_defined

#include "platform.h"
#include "mio.h"
#include "cssio.h"
#include "util.h"
#include "ida.h"
#include "isi/db.h"

extern char *VersionIdentString;

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
    long rec;     /* number of records which have been read  */
    long stream[IDA_MAXSTREAMS]; /* per stream count        */
};

/* Function prototypes */

/*drec_ok.c */
int drec_ok(IDA_DHDR *dhead, struct counter *count);

/*exitcode.c */
int exitcode(void);
void die(int status);

/*fixyear.c */
void init_fixyear(int beg_yr, int end_yr, int beg_da, int end_da);
void fixyear(IDA_DHDR *dhead, int fixyear_flag);

/*help.c */
void help(void);

/*init.c */
void init(int argc, char **argv, struct counter *count, int *verbose, int *echo, MIO **fp, int *ffu_shift, FILE **hp, unsigned long *swab, int *fixyear_flag);

/*loadbuf.c */
char *loadbuf(IDA_DHDR *dhead, UINT8 *data, struct counter *count);

/*main.c */
int main(int argc, char **argv);

/*msglog.c */
void init_msglog(char *input, struct counter *countptr);
void close_msglog(void);
void logmsg(char *string);

/*names.c */
void nameinit(char *outdir, char *id, int dump_head, FILE **hp);
char *pathname(int stream);
char *crnt_dname(void);
char *crnt_fname(void);
char *basedir(void);
char *vol_id(void);

/*prthead.c */
void prthead(FILE *fp, long recno, IDA_DHDR *dhead);

/*rdrec.c */
int init_rdrec(int tmp_verbose, int tmp_echo, int tmp_shift, unsigned long tmp_bswap, int tmp_fixyear_flag);
int rdrec(MIO *fp, FILE *hp, IDA_DREC *drec, struct counter *count);

/*wfdisc.c */
void wrt_wfdisc(void);
void init_wfdisc(int format, struct counter *countptr, unsigned long swab);
struct wflist *get_wfdisc(char *path, BufferedStream *fp, IDA_DHDR *dhead);

/*wrtdat.c */
void wrtdat(IDA_DREC *drec, struct counter *count);

#endif

/* Revision History
 *
 * $Log: idadmx.h,v $
 * Revision 1.10  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.9  2005/10/11 22:50:00  dechavez
 * switch from VERSION to global VersionIdentString
 *
 * Revision 1.8  2005/08/26 20:23:40  dechavez
 * version 2.12.0
 *
 * Revision 1.7  2005/02/16 18:29:53  dechavez
 * 2.11.1
 *
 * Revision 1.6  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.5  2003/11/21 20:11:29  dechavez
 * 2.10.3
 *
 * Revision 1.4  2003/10/16 18:17:18  dechavez
 * updated prototypes
 *
 * Revision 1.3  2002/03/11 17:16:29  dec
 * 2.10.2
 *
 * Revision 1.2  2000/02/18 06:39:18  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
