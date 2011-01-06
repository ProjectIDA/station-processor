#pragma ident "$Id: main.c,v 1.18 2009/02/26 22:47:21 dechavez Exp $"
/*======================================================================
 *
 *  List QDPLUS packets
 *
 *====================================================================*/
#include "zlib.h"
#include "isi.h"
#include "qdplus.h"
#include "ida10.h"

#define PRINT_NOTHING    0x0000
#define PRINT_CMNHDR     0x0001
#define PRINT_ACTION     0x0002
#define PRINT_TOKENS     0x0004
#define PRINT_COMBO      0x0008
#define PRINT_STATUS     0x0010
#define PRINT_USER       0x0020
#define PRINT_IGNORED    0x0040
#define PRINT_LCQ_EVENTS 0x0080
#define PRINT_LCQ        0x0100
#define PRINT_LCQLC     (0x0200 | PRINT_LCQ)
#define PRINT_LCQDATA   (0x0400 | PRINT_LCQLC)
#define PRINT_HEX        0x0800
#define PRINT_EVERYTHING 0xffff
#define DEFAULT_PRINT    (PRINT_CMNHDR | PRINT_LCQ)

typedef struct {
    char *name;
    UINT16 mask;
    char *description;
} PRINT_MAP;

static PRINT_MAP map[] =
{
    {    "all", PRINT_EVERYTHING, "print everything"},
    {    "hdr", PRINT_CMNHDR,     "print QDP common header"},
    {    "act", PRINT_ACTION,     "print LCQ action flag"},
    {    "tok", PRINT_TOKENS,     "print tokens"},
    {    "cmb", PRINT_COMBO,      "print combo (FIX, GLOB, LOG) packets"},
    {   "stat", PRINT_STATUS,     "print status packets"},
    {   "user", PRINT_USER,       "print user packets (not implemented)"},
    {    "ign", PRINT_IGNORED,    "print ignored packets"},
    {    "evt", PRINT_LCQ_EVENTS, "print LCQ events"},
    {    "lcq", PRINT_LCQ,        "print logical channel header"},
    {    "chn", PRINT_LCQLC,      "print logical channel headers"},
    {    "dat", PRINT_LCQDATA,    "print data samples"},
    {    "hex", PRINT_HEX,        "hex dump of each QDP packet"},
    {NULL, 0}
};

static UINT16 print = PRINT_NOTHING;
static UINT16 printlcq = 0;

static void PrintHelp()
{
int i;

    fprintf(stderr, "The 'print' argument is a comma delimited list of any of the following\n");
    for (i = 0; map[i].name != NULL; i++) fprintf(stderr, "%4s - %s\n", map[i].name, map[i].description);
    exit(1);
}

static void DecodePrintArg(char *string)
{
BOOL done;
int i, n, ntoken;
#define MAXTOKEN 32
char *token[MAXTOKEN];
char *arg;

    if ((arg = strdup(string)) == NULL) {
        perror("strdup");
        exit(1);
    }

    if ((ntoken = utilParse(arg, token, ",", MAXTOKEN, 0)) < 0) {
        fprintf(stderr, "error parsing '%s'\n", string);
        exit(1);
    }

    for (n = 0; n < ntoken; n++) {
        if (strcasecmp(token[n], "help") == 0) PrintHelp();
        for (done = FALSE, i = 0; !done && map[i].name != NULL; i++) {
            if (strcasecmp(token[n], map[i].name) == 0) {
                print |= map[i].mask;
                done = TRUE;
            }
        }
        if (!done) {
            fprintf(stderr, "unrecgonized print flag '%s'\n", token[n]);
            PrintHelp();
        }
    }

    if ((print & PRINT_LCQDATA) == PRINT_LCQDATA) {
        printlcq = QDP_PRINT_LCQDATA;
    } else if ((print & PRINT_LCQLC) == PRINT_LCQLC) {
        printlcq = QDP_PRINT_LCQLC;
    } else {
        printlcq = 0;
    }
}

static void PrintUser(QDPLUS_PKT *pkt)
{
    printf("DT_USER packet processing goes here\n");
}

static void PrintIgnored(QDP_PKT *qdp)
{
int i;
UINT64 sn, auth = 0;
QDP_TYPE_C1_SRVCH c1_srvch;
QDP_TYPE_C1_SRVRSP c1_srvrsp;
QDP_TYPE_C1_RQMEM c1_rqmem;
QDPLUS_AUXPKT userpkt;
char tmpbuf[32];
UINT32 seqno;

    switch (qdp->hdr.cmd) {
      case QDP_C1_RQSRV:
        utilUnpackUINT64(qdp->payload, &sn);
        printf("sn = %016llX\n", sn);
        break;

      case QDP_C1_SRVCH:
        qdpDecode_C1_SRVCH(qdp->payload, &c1_srvch);
        printf("challenge = %016llX\n", c1_srvch.challenge);
        printf("       ip = %08lX = %s\n", c1_srvch.dp.ip, c1_srvch.dp.dotdecimal);
        printf("     port = %d\n", c1_srvch.dp.port);
        printf("      reg = %d\n", c1_srvch.dp.registration);
        break;

      case QDP_C1_SRVRSP:
        qdpDecode_C1_SRVRSP(qdp->payload, &c1_srvrsp);
        printf(" serialno = %016llX\n", c1_srvrsp.serialno);
        printf("challenge = %016llX\n", c1_srvrsp.challenge);
        printf("       ip = %08lX = %s\n", c1_srvrsp.dp.ip, c1_srvrsp.dp.dotdecimal);
        printf("     port = %d\n", c1_srvrsp.dp.port);
        printf("      reg = %d\n", c1_srvrsp.dp.registration);
        printf("   random = %016llX\n", c1_srvrsp.random);
        printf("      md5 = %008lX%008lX%008lX%008lX\n", c1_srvrsp.md5[0], c1_srvrsp.md5[1], c1_srvrsp.md5[2], c1_srvrsp.md5[3]);
        qdpMD5(&c1_srvrsp, auth);
        printf("my digest = %008lX%008lX%008lX%008lX\n", c1_srvrsp.md5[0], c1_srvrsp.md5[1], c1_srvrsp.md5[2], c1_srvrsp.md5[3]);
        break;

      case QDP_C1_RQMEM:
        qdpDecode_C1_RQMEM(qdp->payload, &c1_rqmem);
        printf("offset = %hu\n", c1_rqmem.offset);
        printf(" nbyte = %hu\n", c1_rqmem.nbyte);
        printf("  type = %s (0x%04X)\n", qdpMemTypeString(c1_rqmem.type), c1_rqmem.type);
        printf("passwd = 0x%08X%08X%08X%08x\n", c1_rqmem.passwd[0], c1_rqmem.passwd[1], c1_rqmem.passwd[2], c1_rqmem.passwd[3]);
        break;

      case QDP_DT_USER:
        if (qdp->payload[0] == QDPLUS_DT_USER_AUX) {
            qdplusDecodeUserAux(qdp->payload, &userpkt);
            printf("type = %s, ", isiDatumTypeString(userpkt.type));
            printf("chn = '%s', ", userpkt.chn);
            printf("loc = '%s', ", userpkt.loc);
            printf("nsamp = %hu, ", userpkt.nsamp);
            printf("sint = %.3f, ", (float) userpkt.sint / MSEC_PER_SEC);
            printf("tofs = %s, ", utilLttostr(userpkt.tofs.ext, 0, tmpbuf));
            printf("tols = %s\n", utilLttostr(userpkt.tols.ext, 0, tmpbuf));
        } else {
            printf("user type = %d\n", qdp->payload[0]);
            utilPrintHexDump(stdout, qdp->payload, qdp->hdr.dlen);
        }
        break;

      default:
        utilPrintHexDump(stdout, qdp->payload, qdp->hdr.dlen);
    }
}

static void process(QDPLUS *handle, QDPLUS_PKT *pkt, UINT32 in)
{
QDP_LCQ *lcq;
static int first = TRUE;

    if (!qdpVerifyCRC(&pkt->qdp)) {
        printf("CRC error, packet no. %lu dropped\n", in);
        printf("%016llX CRC ERROR: %hu bytes of %s(%hu, %hu)\n", pkt->serialno, pkt->qdp.hdr.dlen, qdpCmdString(pkt->qdp.hdr.cmd), pkt->qdp.hdr.seqno, pkt->qdp.hdr.ack);
        return;
    }

    if ((lcq = qdplusProcessPacket(handle, pkt)) == NULL) {
        printf("qdplusProcessPacket failed\n");
        exit(1);
    }

    if (print & PRINT_HEX) {
        printf("%016llX %08lx%016llx, raw len=%d, payload len=%d\n", pkt->serialno, pkt->seqno.signature, pkt->seqno.counter, pkt->qdp.len, pkt->qdp.hdr.dlen);
        utilPrintHexDump(stdout, pkt->qdp.raw, pkt->qdp.len);
    }
    if (print & PRINT_CMNHDR) qdplusPrintPkt(stdout, pkt, QDP_PRINT_HDR);

    //if (!first) printf("\n"); first = FALSE;

    if ((print & PRINT_LCQ_EVENTS) && lcq->event != 0) {
        printf("LCQ events:");
        if (lcq->event & QDP_LCQ_EVENT_NO_META) printf(" NO_META");
        if (lcq->event & QDP_LCQ_EVENT_UNSUPPORTED) printf(" UNSUPPORTED");
        if (lcq->event & QDP_LCQ_EVENT_UNDOCUMENTED) printf(" UNDOCUMENTED");
        if (lcq->event & QDP_LCQ_EVENT_NO_TOKENS) printf(" NO_TOKENS");
        if (lcq->event & QDP_LCQ_EVENT_DECOMP_ERR) printf(" DECOMP_ERR");
        if (lcq->event & QDP_LCQ_EVENT_SINT_CHANGE) printf(" SINT_CHANGE");
        if (lcq->event & QDP_LCQ_EVENT_FATAL) {
            printf(" FATAL\n");
            exit(1);
        }
        printf("\n");
    }

    switch (lcq->action) {

      case QDP_LCQ_SAVE_MEMBLK:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_SAVE_MEMBLK\n");
        break;

      case QDP_LCQ_LOAD_TOKEN:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_TOKEN\n");
        if (print & PRINT_TOKENS) qdpPrintTokens(stdout, &lcq->meta.token);
        break;

      case QDP_LCQ_LOAD_COMBO:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_COMBO\n");
        if (print & PRINT_COMBO) qdpPrint_C1_COMBO(stdout, &lcq->meta.combo); break;
        break;

      case QDP_LCQ_LOAD_DT_DATA:
           if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_DT_DATA\n");
           if (print & PRINT_LCQ) qdpPrintLCQ(stdout, lcq, printlcq);
        break;

      case QDP_LCQ_LOAD_C1_STAT:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_LOAD_C1_STAT\n"); fflush(stdout);
        if (print & PRINT_STATUS) qdpPrint_C1_STAT(stdout, &lcq->c1_stat); fflush(stdout);
        break;

      case QDP_LCQ_IS_DT_USER:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_IS_DT_USER\n");
        if (print & PRINT_USER) PrintUser(pkt);
        break;

      case QDP_LCQ_IGNORE:
        if (print & PRINT_ACTION) printf("LCQ action = QDP_LCQ_IGNORE\n");
        if (print & PRINT_IGNORED) PrintIgnored(lcq->pkt);
        break;

      default:
        if (print & PRINT_ACTION) printf("ERROR: Unknown LCQ action '%d'\n", lcq->action);
        exit(1);
    }
}

void Ida10Callback(void *arg, QDP_HLP *hlp)
{
FILE *fp;
static UINT32 seqno = 0;
UINT8 buf[IDA10_FIXEDRECLEN];

    fp = (FILE *) arg;

    if (!qdpHlpToIDA10(buf, hlp, ++seqno)) {
        fprintf(stderr, "qdpHlpToIDA10: %s\n", strerror(errno));
        exit(1);
    }

    if (fwrite(buf, sizeof(char), IDA10_FIXEDRECLEN, fp) != IDA10_FIXEDRECLEN) {
        fprintf(stderr, "fwrite: %s\n", strerror(errno));
        exit(1);
    }

}

BOOL InitIDA10(QDP_HLP_RULES *rules, char *file)
{
static FILE *fp;

    if ((fp = fopen(file, "wb")) == NULL) {
        fprintf(stderr, "fopen: %s\n", strerror(errno));
        exit(1);
    }

    return qdpInitHLPRules(
        rules,
        IDA10_DEFDATALEN,
        QDP_HLP_FORMAT_NOCOMP32,
        Ida10Callback,
        (void *) fp,
        QDP_HLP_RULE_FLAG_LCASE
    );
}

#define DEFAULT_TRECS 3000

void TeeCallback(void *arg, QDP_HLP *hlp)
{
int trecs = DEFAULT_TRECS;
char *prefix;
static ISI_SEQNO seqno = {ISI_UNDEF_SEQNO_SIG, 0LL};
static gzFile *gz = NULL;
char path[MAXPATHLEN+1];
UINT8 buf[IDA10_FIXEDRECLEN];
static struct {
    UINT64 crnt;
    UINT64 prev;
} suffix = {0LL, 0LL};

    if (hlp == NULL) {
        if (gz != NULL) gzclose(gz);
        return;
    }

    if (seqno.signature == ISI_UNDEF_SEQNO_SIG) seqno.signature = time(NULL);

    prefix = (char *) arg;

    if (!qdpHlpToIDA10(buf, hlp, (UINT32) seqno.counter++)) {
        fprintf(stderr, "qdpHlpToIDA10: %s\n", strerror(errno));
        exit(1);
    }

    suffix.crnt = (seqno.counter / trecs) * trecs;
    if (suffix.crnt != suffix.prev || gz == NULL) {
        if (gz != NULL) gzclose(gz);
        sprintf(path, "%s/%08lx%016llx.gz", prefix, seqno.signature, suffix.crnt);
        if ((gz = gzopen(path, "wb")) == NULL) {
            fprintf(stderr, "gzopen: %s: %s\n", path, strerror(errno));
            exit(1);
        }
    }

    if (gzwrite(gz, buf, IDA10_FIXEDRECLEN) != IDA10_FIXEDRECLEN) {
        fprintf(stderr, "gzwrite: %s\n", strerror(errno));
        exit(1);
    }

    suffix.prev = suffix.crnt;

}

BOOL InitTee(QDP_HLP_RULES *rules, char *dir)
{
    if (util_mkpath(dir, 0755) != 0) {
        fprintf(stderr, "unable to mkdir %s: %s\n", dir, strerror(errno));
        exit(1);
    }

    return qdpInitHLPRules(
        rules,
        IDA10_DEFDATALEN,
        QDP_HLP_FORMAT_NOCOMP32,
        TeeCallback,
        (void *) dir,
        QDP_HLP_RULE_FLAG_LCASE
    );
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [ print=list { ida10=file | tee=dir } keep=serialno meta=dir count=val skip=val ] < QDPLUS_data_stream\n", myname);
    fprintf(stderr, "\n");
    PrintHelp();
}

int main (int argc, char **argv)
{
int i;
UINT32 skip = 0, in = 0, kept = 0, count = 0xffffffff;;
QDPLUS_PKT pkt;
UINT64 keep = 0;
QDPLUS *handle;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
char *ida10 = NULL;
char *tee = NULL;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strncasecmp(argv[i], "print=", strlen("print=")) == 0) {
            DecodePrintArg(argv[i]+strlen("print="));
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            keep = (UINT64) strtoll((char *) argv[i]+strlen("keep="), NULL, 16);
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = (UINT32) atol(argv[i]+strlen("count="));
        } else if (strncmp(argv[i], "skip=", strlen("skip=")) == 0) {
            skip = (UINT32) atol(argv[i]+strlen("skip="));
        } else if (strncmp(argv[i], "meta=", strlen("meta=")) == 0) {
            par.path.meta = argv[i] + strlen("meta=");
        } else if (strncmp(argv[i], "ida10=", strlen("ida10=")) == 0) {
            ida10 = argv[i] + strlen("ida10=");
        } else if (strncmp(argv[i], "tee=", strlen("tee=")) == 0) {
            tee = argv[i] + strlen("tee=");
        } else {
            fprintf(stderr, "%s: unrecognized argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (ida10 != NULL && tee != NULL) {
        fprintf(stderr, "%s: ida10 and tee options are mutually exclusive\n", argv[0]);
        exit(1);
    }

    if (ida10 != NULL) {
        if (!InitIDA10(&par.lcq.rules, ida10)) {
            fprintf(stderr, "%s: unable to initialize IDA10 packet converter\n", argv[0]);
            exit(1);
        }
    }

    if (tee != NULL) {
        if (!InitTee(&par.lcq.rules, tee)) {
            fprintf(stderr, "%s: unable to initialize tee file packet converter\n", argv[0]);
            exit(1);
        }
    }

    if ((handle = qdplusCreateHandle(&par)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("qdplusCreateHandle");
        exit(1);
    }

    if (skip) {
        printf("Skipping the first %lu records\n", skip);
        do { qdplusRead(stdin, &pkt); } while (++in < skip);
        if (ferror(stdin)) {
            printf("I/O error while reading packet number %lu\n", ++in);
            exit(1);
        }
    }

    if (keep) printf("Only tracking packets from instrument s/n %016llX\n", keep); 
    while (in < count && qdplusRead(stdin, &pkt)) {
        ++in;
        if (keep == 0 || pkt.serialno == keep) {
            ++kept;
            process(handle, &pkt, in);
        }
    }
    TeeCallback(NULL, NULL);

    printf("%lu packets read\n", in);
    printf("%lu packets processed\n", kept);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.18  2009/02/26 22:47:21  dechavez
 * added tee option
 *
 * Revision 1.17  2009/02/23 22:26:58  dechavez
 * replaced -v option with more elaborate print flags
 *
 * Revision 1.16  2008/10/09 20:42:43  dechavez
 * added ida10 option
 *
 * Revision 1.15  2008/03/05 23:15:05  dechavez
 * fixed printing typo
 *
 * Revision 1.14  2007/12/20 23:15:40  dechavez
 * added C1_STAT support
 *
 * Revision 1.13  2007/06/28 19:43:09  dechavez
 * improved help message
 *
 * Revision 1.12  2007/05/11 16:19:45  dechavez
 * Relink with libqdp with compiled root time tag instead of macro
 *
 * Revision 1.11  2007/01/04 18:07:26  dechavez
 * *** empty log message ***
 *
 * Revision 1.10  2006/12/22 02:43:55  dechavez
 * Changes to accomodate reworked structure fields
 *
 * Revision 1.9  2006/12/15 00:32:07  dechavez
 * use new QDPLUS_PAR, no meta.  Added time stamp to lc prints
 *
 * Revision 1.8  2006/12/13 22:01:06  dechavez
 * added metadata support
 *
 * Revision 1.7  2006/12/06 22:51:10  dechavez
 * Use QDPLUS logical channel queues, much print stuff moved off to library
 *
 * Revision 1.6  2006/11/13 19:02:03  dechavez
 * Added hex dumps of compressed DT_DATA blockettes
 *
 * Revision 1.5  2006/10/13 21:35:42  dechavez
 * prelinimary DT_DATA support
 *
 * Revision 1.4  2006/07/06 16:50:48  dechavez
 * fixed typo in argument search loop
 *
 * Revision 1.3  2006/07/06 16:49:26  dechavez
 * Added -v option (off by default) and changed output format to make it easier to grep
 *
 * Revision 1.2  2006/06/07 22:25:54  dechavez
 * decode DT_USER packets
 *
 * Revision 1.1  2006/06/02 20:54:09  dechavez
 * initial release
 *
 */
