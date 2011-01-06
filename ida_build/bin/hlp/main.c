#pragma ident "$Id: main.c,v 1.5 2007/05/03 21:03:40 dechavez Exp $"
/*======================================================================
 *
 *  Test harness for developing library code to generate high-level
 *  packets from a QDP packet stream.
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "qdplus.h"
#include "ida10.h"
#include "isi/dl.h"

extern char *VersionIdentString;

static BOOL verbose = FALSE;
static BOOL StateFlagSet = FALSE;

typedef struct {
    ISI_RAW_PACKET raw;
    ISI_DL *dl;
} LOCALPKT;

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */

        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGHUP:
            printf("SIGHUP\n");
            StateFlagSet = TRUE;
            break;

          default:
            break;
        }
    }
}

VOID StartSignalHandler(VOID)
{
THREAD tid;
sigset_t set;

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        perror("THREAD_CREATE");
        exit(1);
    }
}

static ISI_DL *OpenDiskLoop(char *dbspec, char *myname, char *site, LOGIO *lp)
{
ISI_DL *dl;
static ISI_GLOB glob;
static char *fid = "OpenDiskLoop";

    if (!isidlSetGlobalParameters(dbspec, myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters failed: %s\n", fid, strerror(errno));
        exit(1);
    }

    if ((dl = isidlOpenDiskLoop(&glob, site, lp, ISI_RDWR)) == NULL) {
        fprintf(stderr, "%s: isidlOpenDiskLoop failed: %s\n", fid, strerror(errno));
        exit(1);
    }

    return dl;
}

static void isidl(void *arg, QDP_HLP *hlp)
{
LOCALPKT *local;
static char *fid = "isidl";

    local = (LOCALPKT *) arg;
    local->raw.hdr.status = ISI_RAW_STATUS_OK;

    if (!qdpHlpToIDA10(local->raw.payload, hlp, local->dl->sys->seqno.counter)) {
        fprintf(stderr, "%s: qdpHlpToIDA10 failed: %s\n", fid, strerror(errno));
        exit(1);
    }

    if (!isidlWriteToDiskLoop(local->dl, &local->raw, ISI_OPTION_GENERATE_SEQNO)) {
        fprintf(stderr, "%s: isidlWriteToDiskLoop failed: %s\n", fid, strerror(errno));
        exit(1);
    }
}

static void ida10(void *arg, QDP_HLP *hlp)
{
FILE *fp;
static UINT32 seqno = 0;
IDA10_TS ts;
UINT8 packet[IDA10_FIXEDRECLEN], tmp[1024];
static char *fid = "ida10";

    fp = (FILE *) arg;

    if (!qdpHlpToIDA10(packet, hlp, seqno++)) {
        fprintf(stderr, "%s: qdpHlpToIDA10 failed: %s\n", fid, strerror(errno));
        exit(1);
    }

    if (!ida10UnpackTS(packet, &ts)) {
        fprintf(stderr, "%s: ida10UnpackTS: %s\n", fid, strerror(errno));
        exit(1);
    }
    if (verbose) printf("%s\n", ida10TStoString(&ts, tmp));

    if (fwrite(packet, IDA10_FIXEDRECLEN, 1, fp) != 1) {
        perror("fwrite");
        exit(1);
    }
}

static void mseed(void *arg, QDP_HLP *hlp)
{
    fprintf(stderr, "sorry, miniSEED support not implemented\n");
}

static void process(QDPLUS *handle, QDPLUS_PKT *pkt)
{
QDP_LCQ *lcq;

    if ((lcq = qdplusProcessPacket(handle, pkt)) == NULL) {
        fprintf(stderr, "qdplusProcessPacket failed\n");
        exit(1);
    }

    if (lcq->event & QDP_LCQ_EVENT_FATAL) {
        fprintf(stderr, "QDP_LCQ_EVENT_FATAL: %s\n", strerror(errno));
        exit(1);
    }

    if (StateFlagSet) {
        if (!qdplusSaveState(handle)) {
            fprintf(stderr, "qdplusSaveState: %s\n", strerror(errno));
            exit(1);
        }
        qdplusPrintState(stdout, handle);
        StateFlagSet = FALSE;
    }
}

static void help(char *myname)
{
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ options ] { ida10=path | mseed=path } SiteName\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "-v         => verbose\n");
    fprintf(stderr, "-lcase     => use lower case channel names\n");
    fprintf(stderr, "-strict    => require LCQ token entries for all streams\n");
    fprintf(stderr, "skip=val   => skip the specified number of packets\n");
    fprintf(stderr, "keep=sn    => only use packets from digitizer with given serial number\n");
    fprintf(stderr, "count=val  => only process specified number of packets\n");
    fprintf(stderr, "meta=spec  => name of metadata directory\n");
    fprintf(stderr, "state=spec => name of hlp state file\n");
    fprintf(stderr, "\n");
    exit(1);
}

#define OUTPUT_UNDEFINED 0
#define OUTPUT_IDA10     1
#define OUTPUT_MSEED     2
#define OUTPUT_ISIDL     3

int main(int argc, char **argv)
{
int i;
FILE *fp;
UINT32 skip = 0, in = 0, kept = 0, count = 0xffffffff;;
UINT64 keep;
QDPLUS_PKT pkt;
QDPLUS *handle;
char *ospec = NULL, *log = NULL;
int output = OUTPUT_UNDEFINED;
ISI_SEQNO beg = ISI_OLDEST_SEQNO;
ISI_SEQNO end = ISI_NEWEST_SEQNO;
BOOL debug = FALSE;
QDPLUS_PAR par = QDPLUS_DEFAULT_PAR;
UINT32 flags = QDP_DEFAULT_HLP_RULE_FLAG;
LOCALPKT local;
LOGIO logio, *lp = NULL;
static char *DefaultState = "state";
static char *DefaultMeta = "meta";
static char *DefaultSite = "site";

    StartSignalHandler();

    if (utilDirectoryExists(DefaultMeta)) par.path.meta = DefaultMeta;
    par.path.state = DefaultState;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "meta=", strlen("meta=")) == 0) {
            par.path.meta = argv[i] + strlen("meta=");
        } else if (strncmp(argv[i], "state=", strlen("state=")) == 0) {
            par.path.state = argv[i] + strlen("state=");
        } else if (strncmp(argv[i], "keep=", strlen("keep=")) == 0) {
            keep = (UINT64) strtoll((char *) argv[i]+strlen("keep="), NULL, 16);
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = (UINT32) atol(argv[i]+strlen("count="));
        } else if (strncmp(argv[i], "skip=", strlen("skip=")) == 0) {
            skip = (UINT32) atol(argv[i]+strlen("skip="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "isidl=", strlen("isidl=")) == 0) {
            if (output != OUTPUT_UNDEFINED) {
                fprintf(stderr, "%s: unexpected multiple outpus (arg '%s')\n", argv[0], argv[i]);
                help(argv[0]);
            }
            par.site = argv[i] + strlen("isidl=");
            output = OUTPUT_ISIDL;
        } else if (strncmp(argv[i], "ida10=", strlen("ida10=")) == 0) {
            if (output != OUTPUT_UNDEFINED) {
                fprintf(stderr, "%s: unexpected multiple outpus (arg '%s')\n", argv[0], argv[i]);
                help(argv[0]);
            }
            ospec = argv[i] + strlen("ida10=");
            output = OUTPUT_IDA10;
        } else if (strncmp(argv[i], "mseed=", strlen("mseed=")) == 0) {
            if (output != OUTPUT_UNDEFINED) {
                fprintf(stderr, "%s: unexpected multiple outpus (arg '%s')\n", argv[0], argv[i]);
                help(argv[0]);
            }
            ospec = argv[i] + strlen("mseed=");
            output = OUTPUT_MSEED;
        } else if (strcmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-lcase") == 0) {
            flags |= QDP_HLP_RULE_FLAG_LCASE;
        } else if (strcmp(argv[i], "-strict") == 0) {
            flags |= QDP_HLP_RULE_FLAG_STRICT;
        } else if (par.site != NULL) {
            fprintf(stderr, "%s: unexpected argument '%s'\n", argv[0], argv[i]);
            help(argv[0]);
        } else {
            par.site = argv[i];
        }
    }

    if (log != NULL) {
        logioInit(&logio, log, NULL, argv[0]);
        if (debug) logioSetThreshold(&logio, LOG_DEBUG);
        lp = &logio;
    } else {
        lp = NULL;
    }

    if (par.site == NULL) par.site = DefaultSite;

    if (output == OUTPUT_UNDEFINED) {
        fprintf(stderr, "Missing output specifier\n");
        help(argv[0]);
    }

    if (output == OUTPUT_ISIDL) {
        local.dl = OpenDiskLoop(NULL, argv[0], par.site, lp);
        if (!isiInitRawPacket(&local.raw, NULL, local.dl->sys->maxlen)) {
            fprintf(stderr, "isiInitRawPacket: %s", strerror(errno));
            exit(1);
        }
        strcpy(local.raw.hdr.site, local.dl->sys->site);
        local.raw.hdr.len.used = local.dl->sys->maxlen;
        local.raw.hdr.len.native = local.dl->sys->maxlen;
        local.raw.hdr.desc.comp = ISI_COMP_NONE;
        local.raw.hdr.desc.type = ISI_TYPE_IDA10;
        local.raw.hdr.desc.order = ISI_TYPE_UNDEF;
        local.raw.hdr.desc.size = sizeof(UINT8);
    } else {
        if ((fp = fopen(ospec, "wb")) == NULL) {
            fprintf(stderr, "%s: fopen: ", argv[0]);
            perror(ospec);
            exit(1);
        }
    }

    if (output == OUTPUT_ISIDL) {
        if (!qdpInitHLPRules(&par.lcq.rules, IDA10_DEFDATALEN, QDP_HLP_FORMAT_NOCOMP32, isidl, (void *) &local, flags)) {
            perror("qdpInitHLPRules");
            exit(1);
        }
    } else if (output == OUTPUT_IDA10) {
        if (!qdpInitHLPRules(&par.lcq.rules, IDA10_DEFDATALEN, QDP_HLP_FORMAT_NOCOMP32, ida10, (void *) fp, flags)) {
            perror("qdpInitHLPRules");
            exit(1);
        }
    } else if (output == OUTPUT_MSEED) {
        if (!qdpInitHLPRules(&par.lcq.rules, IDA10_DEFDATALEN, QDP_HLP_FORMAT_NOCOMP32, mseed, &fp, flags)) {
            perror("qdpInitHLPRules");
            exit(1);
        }
    } else {
        fprintf(stderr, "unknown output format %d!\n", output);
        exit(1);
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
            process(handle, &pkt);
        }
    }
    printf("%lu packets read\n", in);
    printf("%lu packets processed\n", kept);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.5  2007/05/03 21:03:40  dechavez
 * Added -lcase option, and SIGHUP handler to save state data
 *
 * Revision 1.4  2007/02/20 02:21:40  dechavez
 * added isidl option
 *
 * Revision 1.3  2007/01/04 18:09:02  dechavez
 * Default to strict token requirements with -nostrict override option
 *
 * Revision 1.2  2006/12/22 02:31:45  dechavez
 * IDA10 packet support added, as well as stub for miniSEED
 *
 * Revision 1.1  2006/12/15 00:20:42  dechavez
 * initial release
 *
 */
