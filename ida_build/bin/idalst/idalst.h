#pragma ident "$Id: idalst.h,v 1.10 2006/03/24 18:38:44 dechavez Exp $"
/*======================================================================
 *
 *  Miscellaneous constants
 *
 *====================================================================*/
#ifndef idalst_h_defined
#define idalst_h_defined

#include "platform.h"
#include "util.h"
#include "tags.h"
#include "mio.h"
#include "ida.h"

/*  Constants  */

#define BIG_TIMERR 2  /*  a "big" time error, in samples  */

#define STANDARD   0
#define SUPPRESSED 1

/*  Misc. return codes  */

#define READ_OK    0
#define READ_DONE  1
#define READ_EOF   2
#define READ_ERR   3
#define USER_ABORT 4

/*  Sort options  */

#define BY_RECORD 1
#define BY_STREAM 2

/*  Group options  */

#define BY_SIZE   3
#define BY_SIGN   4

/*  Print flag masks  */

#define P_IDENT    0x000001  /* print ident record contents            */
#define P_DATA     0x000002  /* print data record contents             */
#define P_CALIB    0x000004  /* print calibration record contents      */
#define P_LOG      0x000008  /* print log record contents              */
#define P_CONFIG   0x000010  /* print configuration record contents    */
#define P_EVENT    0x000020  /* print event record contents            */
#define P_DETECT   0x000040  /* print event detector parameters        */
#define P_TTRIP    0x000080  /* print time triples                     */
#define P_DCCREC   0x000100  /* print DCC applied record summaries     */
#define P_STRMAP   0x000200  /* print local mapping of stream codes    */
#define P_BADTAG   0x000400  /* print illegal time tag contents        */
#define P_TAG      0x000800  /* tag output for sorting                 */
#define P_CORRUPT  0x001000  /* print corrupt record status summary    */
#define P_ALLQUAL  0x002000  /* print all time quality transitions     */
#define P_DUPDAT   0x004000  /* print duplicate data record headers    */
#define P_DUNMAP   0x008000  /* print unmapped record stream info      */
#define P_SREC     0x010000  /* print das status records               */
#define P_SEQNO    0x020000  /* print packet sequence numbers          */

/*  Check flag masks  */

#define C_TINC   0x0001   /* check for time increment errors        */
#define C_TOFF   0x0002   /* check for time offset errors           */
#define C_LASTW  0x0004   /* do last word checks                    */
#define C_SAME   0x0008   /* look for duplicate records             */
#define C_NSAMP  0x0010   /* check for nsamp validity               */
#define C_TQUAL  0x0020   /* track clock qual changes               */
#define C_BADTAG 0x0040   /* check time tag for internal errors     */
#define C_TTC    0x0080   /* check for true time gaps               */

/*  Counter  */

struct counters {
    long rec;     /* current record number                           */
    long ars;     /* number of ARS records read                      */
    long das;     /* number of DAS records read                      */
    long calib;   /* number of calibration records read              */
    long data;    /* number of data records read                     */
    long srec;    /* number of das status records                    */
    long dunmap;  /* number of unmappable data records               */
    long ident;   /* number of identification records read           */
    long log;     /* number of log records read                      */
    long isplog;  /* number of ISP log records read                  */
    long config;  /* number of configuration records read            */
    long event;   /* number of event records read                    */
    long oldhdr;  /* number of old-style header records read         */
    long posthdr; /* number of post-header records read              */
    long resvd;   /* number of reserved records read                 */
    long notdef;  /* number of unrecognized records read             */
    long dcc;     /* number of DCC applied records                   */
    long trgrec;  /* number of trigger mode data records read        */
    long ierr;    /* number of detected time stamp increment errors  */
    long oerr;    /* number of detected time stamp offest errors     */
    long exterr;  /* number of detected external time jumps          */
    long perr;    /* number of above which exceed +2 samples         */
    long nerr;    /* number of above which exceed -2 samples         */
    long badtag;  /* number of incorrect time tags                   */
    long badd;    /* number of corrupt data records read             */
    long corrupt; /* number of corrupt records read (all types)      */
    long dupadj;  /* number of adjacent duplicate records            */
    long dupdat;  /* number of duplicate data records                */
    long iltq;    /* number of "illogical" time quality transitions  */
    long lastw;   /* number of last word errors                      */
    long stream[IDA_MAXSTREAMS]; /* individual stream record count  */
    long totpts[IDA_MAXSTREAMS]; /* total samples per each stream   */
    long reboots; /* number of possible das reboots                  */
};

/*  Default startup options  */

struct defaults {
    int print;
    int check;
    int sort;
    int group;
    int verbose;
    int loglevel;
    long bs;
    char *input;
};

/* Global stuff */

struct globals {
    struct counters *count;
    IDA_DHDR *first, *last;
    int print;
    int check;
    char isplog[MAXPATHLEN+1];
};

#ifndef MAIN
extern struct globals *Global;
extern IDA *ida;
#else
struct globals *Global;
IDA *ida;
#endif

/*  Function prototypes  */

/* chktrut.c */
void chktrut(IDA_DHDR *dhead);
void printttglist(void);

/* ckdupadj.c */
int ckdupadj(char *buffer, struct counters *count, int print);

/* ckdupdat.c */
int ckdupdat(IDA_DHDR *prev_head, long prev_rec, IDA_DHDR *crnt_head, long crnt_rec, int print, struct counters *count);

/* ckextjmp.c */
void ckextjmp(IDA_DHDR *dhead, struct counters *count, int print);

/* cklastw.c */
void cklastw(IDA_DREC *drec, struct counters *count, int print);

/* cktinc.c */
long cktinc(IDA_DHDR *crnt_hdr, IDA_DHDR *prev_hdr, long crnt_rec, long prev_rec, int sort, int group, int print);

/* cktoff.c */
int cktoff(IDA_DHDR *dhead, long record, int print);

/* cktqual.c */
void cktqual(IDA_DHDR *dhead, struct counters *count, int print);

/* doarceof.c */
void doarceof(char *buffer, struct counters *count, int print);

/* docalib.c */
void docalib(char *buffer, struct counters *count, int print, int check);

/* doconfig.c */
void doconfig(char *buffer, struct counters *count, int print, int check);

/* dodata.c */
void dodata(char *buffer, IDA_DHDR *first, IDA_DHDR *last, struct counters *count, int print, int check, int sort, int group);

/* dodmp.c */
void dodmp(char *buffer, struct counters *count, int print);

/* doevent.c */
void doevent(char *buffer, struct counters *count, int print, int check);

/* doident.c */
void doident(char *buffer, struct counters *count, int print, int check);

/* dolabel.c */
void dolabel(char *buffer, struct counters *count, int print);

/* dolog.c */
void dolog(char *buffer, struct counters *count, int print, int check);

/* donotdef.c */
void donotdef(char *buffer, struct counters *count, int print);

/* dosrec.c */
void dosrec(char *buffer, struct counters *count, int print);

/* exitcode.c */
void catch_signal(int sig);
void exitcode(void);

/* getdat.c */
int inigetdat(long bs, char *input, int data_flag);
int getdat(char *output, struct counters *count);

/* help.c */
void help(struct defaults *def, FILE *fp);

/* init.c */
void init(int  argc, char **argv, struct counters *count, int *print, int *check, int *sort, int *group, int *verbose, char *buffer);

/* pbadtag.c */
void pbadtag(IDA_TIME_TAG *ttag, struct counters *count, int type, int print);

/* pcalib.c */
void pcalib(IDA_CALIB_REC *crec, struct counters *count, int print);

/* pconfig.c */
void pconfig(IDA_CONFIG_REC *cnf, struct counters *count, int print);

/* pcorrupt.c */
void pcorrupt(struct counters *count, int type, int status, int print);

/* pdata.c */
void pdata (IDA_DREC *drec, struct counters *count, int print);

/* pdunmap.c */
void pdunmap(IDA_DHDR *head, struct counters *count, int print);

/* pevent.c */
void pevent(IDA_EVENT_REC *evtrec, struct counters *count, int print);

/* pident.c */
void pident(IDA_IDENT_REC *idrec, struct counters *count, int print);

/* pinput.c */
void pinput (char *input, long bs, int data_flag, char *map, struct counters *count, int print, int check, int sort, int group, int loglevel);

/* plog.c */
void init_plog(int level);
void plog(IDA_LOG_REC  *lrec, struct counters *count, int print);

/* poldcnf.c */
void poldcnf(IDA_OLD_HEADER *hrec, int print, long *start_tag);

/* psrec.c */
void psrec(IDA_DAS_STATS *srec, struct counters *count, int print);

/* pttrip.c */
void pttrip(IDA_DHDR *dhead, struct counters  *count, int print);

/* rdata.c */
int rdata(MIO *fp, char *buffer, struct counters *count);

/* summary.c */
void summary(int read_status, struct counters *count, IDA_DHDR *first, IDA_DHDR *last, int print, int check);

#endif

/* Revision History
 *
 * $Log: idalst.h,v $
 * Revision 1.10  2006/03/24 18:38:44  dechavez
 * true time tag test support added (akimov)
 *
 * Revision 1.9  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.8  2005/09/30 22:28:19  dechavez
 * seqno support
 *
 * Revision 1.7  2005/08/26 20:21:56  dechavez
 * rev 2.11.0
 *
 * Revision 1.6  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.5  2003/12/10 06:31:20  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.4  2003/06/11 20:26:20  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.3  2002/03/28 02:51:21  dec
 * treat buffer as unsigned char to avoid sign extension when dumping
 *
 * Revision 1.2  2000/02/18 00:51:32  dec
 * #define and print VERSION
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
