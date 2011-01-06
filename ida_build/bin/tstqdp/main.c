#pragma ident "$Id: main.c,v 1.15 2007/05/11 16:23:05 dechavez Exp $"
/*======================================================================
 *
 *  Test Harness for the Q330 QDP library.
 *
 *====================================================================*/
#include "qdp.h"

typedef struct {
    QDP_PAR q330;
    LOGIO *lp;
    BOOL func;
    QDP_LCQ lcq;
    UINT32 count;
    QDP_DP_TOKEN tokens;
    char *path;
    int format;
    UINT32 flags;
} ALLPAR;

#define IDA10 0
#define MSEED 1

static void PrintHlpHeader(QDP_HLP *hlp)
{
    printf("%016llX ", hlp->meta->combo.fix.sys_num);
    printf("chn='%3s' ", hlp->chn);
    printf("loc='%2s' ", hlp->loc);
    printf("tofs=%s ", utilDttostr(hlp->tofs, 0, NULL));
    printf("qual=%d%% ", hlp->qual);
    printf("sint=%.3lf ", (REAL64) hlp->nsint / NANOSEC_PER_SEC);
    printf("nbyte=%d\n", hlp->nbyte);
}

static void ida10(void *arg, QDP_HLP *hlp)
{
#define IDA10_FIXEDRECLEN 1024
FILE *fp;
int i;
REAL64 tstamp;
INT32 *data;
static UINT32 seqno = 0;
UINT8 packet[IDA10_FIXEDRECLEN];

    fp = (FILE *) arg;

    if (!qdpHlpToIDA10(packet, hlp, seqno++)) {
        fprintf(stderr, "ida10: qdpHlpToIDA10: %s\n", strerror(errno));
        exit(1);
    }

    if (fwrite(packet, IDA10_FIXEDRECLEN, 1, fp) != 1) {
        fprintf(stderr, "ida10: fwrite: %s\n", strerror(errno));
        exit(1);
    }

    PrintHlpHeader(hlp);
}

static void mseed(void *arg, QDP_HLP *hlp)
{
FILE *fp;

    fp = (FILE *) arg;
    PrintHlpHeader(hlp);
}

static void PrintLC(QDP_LCQ *lcq, QDP_LC *lc)
{
int i;
REAL64 tstamp;

    if (lc->state != QDP_LC_STATE_FULL) return;

    tstamp = ((REAL64) qdpRootTimeTag(&lcq->mn232) / (REAL64) NANOSEC_PER_SEC) + QDP_EPOCH_TO_1970_EPOCH;
    printf("src=(0x%02x, %d), ", lc->src[0], lc->src[1]);
    printf("tstamp=%s ", utilDttostr(tstamp, 0, NULL));
    printf("clock=%d%% ", lcq->qual);
    printf("delay=%ld ", lc->delay);
    printf("chn='%s' ", lc->chn);
    printf("loc='%s' ", lc->loc);
    printf("nsamp=%d\n", lc->nsamp);
    for (i = 0; i < lc->nsamp; i++) {
        printf("%10ld ", lc->data[i]);
        if (i > 0 && (i+1) % 10 == 0) printf("\n");
    }
    if (i < 10) printf("\n");

    lc->state = QDP_LC_STATE_EMPTY;
}

static void PrintIgnored(QDP_PKT *pkt)
{
int i;
UINT64 sn, auth = 0;
QDP_TYPE_C1_SRVCH c1_srvch;
QDP_TYPE_C1_SRVRSP c1_srvrsp;
QDP_TYPE_C1_RQMEM c1_rqmem;
char tmpbuf[32];
UINT32 seqno;

    switch (pkt->hdr.cmd) {
      case QDP_C1_RQSRV:
        utilUnpackUINT64(pkt->payload, &sn);
        printf("sn = %016llX\n", sn);
        break;

      case QDP_C1_SRVCH:
        qdpDecode_C1_SRVCH(pkt->payload, &c1_srvch);
        printf("challenge = %016llX\n", c1_srvch.challenge);
        printf("       ip = %08lX = %s\n", c1_srvch.dp.ip, c1_srvch.dp.dotdecimal);
        printf("     port = %d\n", c1_srvch.dp.port);
        printf("      reg = %d\n", c1_srvch.dp.registration);
        break;

      case QDP_C1_SRVRSP:
        qdpDecode_C1_SRVRSP(pkt->payload, &c1_srvrsp);
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
        qdpDecode_C1_RQMEM(pkt->payload, &c1_rqmem);
        printf("offset = %hu\n", c1_rqmem.offset);
        printf(" nbyte = %hu\n", c1_rqmem.nbyte);
        printf("  type = %s (0x%04X)\n", qdpMemTypeString(c1_rqmem.type), c1_rqmem.type);
        printf("passwd = 0x%08X%08X%08X%08x\n", c1_rqmem.passwd[0], c1_rqmem.passwd[1], c1_rqmem.passwd[2], c1_rqmem.passwd[3]);
        break;

      default:
        utilPrintHexDump(stdout, pkt->payload, pkt->hdr.dlen);
    }
}

static void WalkData(QDP_LCQ *lcq)
{
QDP_LC *lc;
LNKLST_NODE *crnt;

    crnt = listFirstNode(lcq->lc);
    while (crnt != NULL) {
        lc = (QDP_LC *) crnt->payload;
        PrintLC(lcq, lc);
        crnt = listNextNode(crnt);
    }
}

static void ReportResults(QDP_LCQ *lcq)
{
    if (lcq->event != 0) {
        printf("LCQ events:");
        if (lcq->event & QDP_LCQ_EVENT_NO_META) printf(" NO_META");
        if (lcq->event & QDP_LCQ_EVENT_UNSUPPORTED) printf(" UNSUPPORTED");
        if (lcq->event & QDP_LCQ_EVENT_DECOMP_ERR) printf(" DECOMP_ERR");
        if (lcq->event & QDP_LCQ_EVENT_FATAL) {
            printf(" FATAL\n");
            exit(1);
        }
        printf("\n");
    }

    switch (lcq->action) {

      case QDP_LCQ_SAVE_MEMBLK:
        printf("LCQ action = QDP_LCQ_SAVE_MEMBLK\n");
        break;

      case QDP_LCQ_LOAD_TOKEN:
        printf("LCQ action = QDP_LCQ_LOAD_TOKEN\n");
        qdpPrintTokens(stdout, &lcq->meta.token);
        break;

      case QDP_LCQ_LOAD_COMBO:
        printf("LCQ action = QDP_LCQ_LOAD_COMBO\n");
        qdpPrint_C1_COMBO(stdout, &lcq->meta.combo); break;
        break;

      case QDP_LCQ_LOAD_DT_DATA:
        printf("LCQ action = QDP_LCQ_LOAD_DT_DATA\n");
        WalkData(lcq);
        break;

      case QDP_LCQ_IGNORE:
        printf("LCQ action = QDP_LCQ_IGNORE\n");
        PrintIgnored(lcq->pkt);
        break;

      default:
        printf("ERROR: Unknown LCQ action '%d'\n", lcq->action);
        exit(1);
    }
}

static void ProcessPacket(void *arg, QDP_PKT *pkt)
{
UINT64 serialno;
BOOL first = TRUE;
UINT16 prev;
ALLPAR *par;

    par = (ALLPAR *) arg;
    ++par->count;

    logioMsg(par->lp, LOG_INFO, "%s(%hu, %hu) from %016llX received by application",
        qdpCmdString(pkt->hdr.cmd), pkt->hdr.seqno, pkt->hdr.ack, par->q330.serialno
    );
    if (pkt->hdr.cmd == QDP_DT_DATA) {
        if (!first) {
            if (pkt->hdr.seqno != prev + 1) logioMsg(par->lp, LOG_INFO, " *** SEQNO BREAK ***");
        } else {
            first = FALSE;
        }
        prev = pkt->hdr.seqno;
    }
    qdpProcessPacket(&par->lcq, pkt);
    if (par->path == NULL) ReportResults(&par->lcq);
}

static void ProcessMeta(void *arg, QDP_META *meta)
{
FILE *fp;
ALLPAR *par;
char path[MAXPATHLEN+1];

    par = (ALLPAR *) arg;

    sprintf(path, "%016llX.meta", par->q330.serialno);
    if ((fp = fopen(path, "a")) == NULL) {
        perror(path);
        exit(1);
    }

    if (qdpWriteMeta(fp, meta) < 0) {
        perror("qdpWriteMeta");
        exit(1);
    }
    fclose(fp);
}

static THREAD_FUNC DataTestThread(void *arg)
{
QDP *qp;
QDP_PKT pkt;
ALLPAR *par;
FILE *fp;
#define IDA10_DEFDATALEN 960

    par = (ALLPAR *) arg;
    par->count = 0;
    par->flags = 0;

    logioMsg(par->lp, LOG_INFO, "%016llX data thread launched", par->q330.serialno);

    qdpSetMeta(&par->q330, (void *) par, ProcessMeta);
    if (par->path != NULL) {
        if ((fp = fopen(par->path, "wb")) == NULL) {
            perror(par->path);
            exit(1);
        }

        if (par->format == IDA10) {
            if (!qdpInitHLPRules(&par->lcq.par.rules, IDA10_DEFDATALEN, QDP_HLP_FORMAT_NOCOMP32, ida10, (void *) fp, par->flags)) {
                perror("qdpInitHLPRules");
                exit(1);
            }
        } else {
            if (!qdpInitHLPRules(&par->lcq.par.rules, IDA10_DEFDATALEN, QDP_HLP_FORMAT_NOCOMP32, mseed, (void *) fp, par->flags)) {
                perror("qdpInitHLPRules");
                exit(1);
            }
        }
    }

    if (par->func) {
        qdpSetUser(&par->q330, (void *) par, ProcessPacket);
        if ((qp = qdpConnectWithPar(&par->q330, par->lp)) == NULL) {
            fprintf(stderr, "Unable to connect to Q330 %016llX\n", par->q330.serialno);
            exit(1);
        }
        while (1) pause();
    }

    if ((qp = qdpConnectWithPar(&par->q330, par->lp)) == NULL) {
        fprintf(stderr, "Unable to connect to Q330\n");
        exit(1);
    }

    while (qdpNextPkt(qp, &pkt)) ProcessPacket((void *) par, &pkt);
}

static void CtrlTest(ALLPAR *par, UINT16 command)
{
QDP *qp;

    if ((qp = qdpConnectWithPar(&par->q330, par->lp)) == NULL) {
        fprintf(stderr, "Unable to connect to Q330\n");
        exit(1);
    }

    if (qdpCtrl(qp, command, TRUE)) {
        printf("%016llX %s command delivered\n", &par->q330.serialno, qdpCtrlString(command));
        qdpDeregister(qp, TRUE);
    } else {
        printf("%016llX %s command failed\n", par->q330.serialno, qdpCtrlString(command));
    }
}

static BOOL AddQ330(LNKLST *q330, LOGIO *lp, char *argstr)
{
ALLPAR new;
QDP_LCQ_PAR par = QDP_DEFAULT_LCQ_PAR;

    par.lp = new.lp = lp;
    if (!qdpInitLCQ(&new.lcq, NULL)) return FALSE;
    qdpInitPar(&new.q330, QDP_UNDEFINED_PORT);
    if (!qdpParseArgString(&new.q330, argstr)) return FALSE;
    if (!listAppend(q330, &new.q330, sizeof(ALLPAR))) return FALSE;
    return TRUE;
}

static void help(char *myname)
{
static char *VerboseHelp = 
"The following argument is required and may be repeated for multiple Q330s:\n"
"\n"
"     q330=host:serial:auth:sport:cport:debug where\n"
"\n"
"          host = name or dot decimal IP address of Q330\n"
"        serial = serial number\n"
"          auth = authorization code\n"
"         sport = Q330 port number, relative to base port of 5330\n"
"         cport = client side ports, relative to base ports of 6330 and 7330\n"
"         debug = 0 (terse), 1 (informational), 2 (debug, very noisy)\n"
"\n"
"In addtion, you must specify ONE of the following commands.\n"
"\n"
"           gpson - turn on GPS\n"
"          gpsoff - turn off GPS\n"
"         gpscold - cold start GPS\n"
"          resync - resync\n"
"          reboot - reboot digitizer\n"
"            data - data feed test\n"
"      ida10=path - high level packet test, IDA10 packets to path\n"
"      mseed=path - high level packet test, miniSEED packets to path\n"
"\n"
"The arguments in [ square brackets ] are optional:\n"
"\n"
"         -strict - require LCQ token entries for all streams (if ida10 or mseed)\n"
"          -trash - trash link at random intervals\n"
"           -func - use user supplied packet function\n"
"      log=string - 'string' is either the name of a log file, '-' for stdout\n"
"                   or 'syslogd:facility' for logging via syslog\n"
"\n"
" Reboot example: q330 q330=192.168.1.1:0123456789abcdef:0:0:0:0 reboot\n"
"   Data example: q330 q330=192.168.1.1:0123456789abcdef:0:1:0:0 data\n"
"\n";

    fprintf(stderr,"usage: %s q330=host:serial:auth:sport:cport:debug [-trash -func] command\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", VerboseHelp);
    fprintf(stderr, "\n");
    exit(1);

}

#ifndef DEFAULT_ARGSTR
#define DEFAULT_ARGSTR NULL
#endif

#define CMND 1
#define DATA 2
#define HLP  3

int main (int argc, char **argv)
{
int format;
LOGIO lp;
FILE *fp;
int i, action;
char *argstr = DEFAULT_ARGSTR;
char *cmd = NULL, *log = NULL, *path = NULL;
UINT16 command;
BOOL trash  = FALSE;
BOOL func   = FALSE;
ALLPAR *par;
LNKLST q330;
THREAD tid;

    listInit(&q330);
    logioInit(&lp, log, NULL, argv[0]);

    if (argstr != NULL && !AddQ330(&q330, &lp, argstr)) {
        fprintf(stderr, "%s: can't add '%s'\n", argv[0], argstr);
        help(argv[0]);
    }

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "q330=", strlen("q330=")) == 0) {
            argstr = argv[i] + strlen("q330=");
            if (!AddQ330(&q330, &lp, argstr)) {
                fprintf(stderr, "%s: can't add '%s'\n", argv[0], argstr);
                help(argv[0]);
            }
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");;
        } else if (strcmp(argv[i], "-strict") == 0) {
            par->flags |= QDP_HLP_RULE_FLAG_STRICT;
        } else if (strcmp(argv[i], "-lcase") == 0) {
            par->flags |= QDP_HLP_RULE_FLAG_LCASE;
        } else if (strcmp(argv[i], "-trash") == 0) {
            trash = TRUE;
        } else if (strcmp(argv[i], "-func") == 0) {
            func = TRUE;
        } else if (cmd == NULL) {
            cmd = argv[i];
        } else {
            fprintf(stderr, "%s: unrecognized argument: '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        }
    }

    if (!listSetArrayView(&q330)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if (q330.count < 1 || cmd == NULL) help(argv[0]);

    action = CMND;
    if (strcmp(cmd, "gpson") == 0) {
        command = QDP_CTRL_GPS_ON;
    } else if (strcmp(cmd, "gpsoff") == 0) {
        command = QDP_CTRL_GPS_OFF;
    } else if (strcmp(cmd, "gpscold") == 0) {
        command = QDP_CTRL_GPS_COLDSTART;
    } else if (strcmp(cmd, "resync") == 0) {
        command = QDP_CTRL_RESYNC;
    } else if (strcmp(cmd, "reboot") == 0) {
        command = QDP_CTRL_REBOOT;
    } else if (strcmp(cmd, "data") == 0) {
        action = DATA;
    } else if (strncmp(cmd, "ida10=", strlen("ida10=")) == 0) {
        path = cmd + strlen("ida10=");
        action = DATA;
        format = IDA10;
    } else if (strncmp(cmd, "mseed=", strlen("mseed=")) == 0) {
        path = cmd + strlen("mseed=");
        action = DATA;
        format = MSEED;
    } else {
        fprintf(stderr, "%s: command '%s' is unrecognized\n", argv[0], cmd);
        exit(1);
    }

    for (i = 0; i < q330.count; i++) {
        par = (ALLPAR *) q330.array[i];
        par->lp = &lp;
        if (trash) qdpSetTrash(&par->q330, TRUE, TRUE);
        if (action == DATA || action == HLP) {
            if (i) utilDelayMsec(5000);
            if (par->q330.port.link < QDP_LOGICAL_PORT_1 || par->q330.port.link > QDP_LOGICAL_PORT_4) {
                fprintf(stderr, "%s: '%s' command only valid on logical data ports\n", argv[0], cmd);
                exit(1);
            }
            par->func = func;
            par->path = path;
            par->format = format;
            if (!THREAD_CREATE(&tid, DataTestThread, (void *) par)) {
                fprintf(stderr, "%s: THREAD_CREATE: %s", argv[0], strerror(errno));
                exit(1);
            }
        } else {
            if (par->q330.port.link != QDP_CFG_PORT && par->q330.port.link != QDP_SFN_PORT) {
                fprintf(stderr, "%s: '%s' command only valid on CNF or SFN ports\n", argv[0], cmd);
                exit(1);
            }
            CtrlTest(par, command);
        }
    }

    if (action == DATA || action == HLP) while(1) pause();

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.15  2007/05/11 16:23:05  dechavez
 * support new HLP flags, compiled root time tag instead of macro
 *
 * Revision 1.14  2007/01/04 18:23:07  dechavez
 * no more BAD_SRC
 *
 * Revision 1.13  2007/01/04 18:06:04  dechavez
 * Default to strict token requirements with -nostrict override option
 *
 * Revision 1.12  2006/12/22 02:36:55  dechavez
 * Added option to produce IDA10 packets, and stub for miniSEED
 *
 * Revision 1.11  2006/12/15 00:24:28  dechavez
 * print time stamp, delay, and clock quality
 *
 * Revision 1.10  2006/12/12 23:07:20  dechavez
 * Use new meta callback function to save metadata
 *
 * Revision 1.9  2006/12/06 22:54:47  dechavez
 * Use logical channel queue and dump contents of one second packets
 *
 * Revision 1.8  2006/11/13 18:54:57  dechavez
 * fixed example invocation
 *
 * Revision 1.7  2006/06/27 00:23:26  dechavez
 * Switch to qdpParseArgString() arg format, and add support for multiple Q330s
 *
 * Revision 1.6  2006/06/07 22:27:23  dechavez
 * added wait option (TRUE) to command functions
 *
 * Revision 1.5  2006/05/26 21:30:35  dechavez
 * added -func option for testing user supplied packet function
 *
 * Revision 1.4  2006/05/20 01:47:07  dechavez
 * allow for compile time defaults
 *
 * Revision 1.3  2006/05/18 21:59:00  dechavez
 * initial release
 *
 */
