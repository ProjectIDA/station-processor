#pragma ident "$Id: getrec.c,v 1.23 2007/01/11 18:07:47 dechavez Exp $"
/*======================================================================
 *
 *  Data input routine.
 *
 *====================================================================*/
#include "nrts.h"
#include "util.h"
#ifndef WIN32
#include "mtio.h"
#endif
#include "wrtdl.h"
#include "isi.h"

#define PROBLEM   0x00
#define FROM_TTY  0x01
#define FROM_TAPE 0x02
#define FROM_DISK 0x04
#define FROM_NET  0x08
#define FROM_ASL  0x10
#define FROM_ASL_TO_IDA 0x20
#define FROM_ISI  0x80

static FILE *filefp;
#ifndef WIN32
static TAPE *tapetp;
#endif
static int   ttyfd;
static int   timeout;
static int   tto;
static char *RemoteHost;
static int  (*net_read)();
static char *keeparg;
static double end;
static double beg;
static char *service;
static int port;
static int retry;
static char *syscode;
static char *home;
static char *pwd;
static char *dbgpath;
static IDA *ida;
static ISI_PARAM IsiParam;
static ISI_DATA_REQUEST *IsiDatreq = NULL;
static ISI *IsiHandle = NULL;

#define MAXSC ((NRTS_MAXSTA*(NRTS_SNAMLEN+1) + \
               (NRTS_MAXSTA*NRTS_MAXCHN*(NRTS_CNAMLEN+1))))
struct {
    void *info;
    struct seed_minipacket packet;
    int blksiz;
    int rev;
    int nbuf;
    int tol;
    char *buf;
    char sc[MAXSC];
    int nsta;
    struct {
        char *name;
        int nchn;
        struct {
            char *name;
            double end;
        } chn[NRTS_MAXCHN];
    } sta[NRTS_MAXSTA];
} asl2ida;

static struct nrts_sys *sys;
static struct param *param;

static struct xfer_req *req;
static struct xfer01_wavreq *wavreq;

static unsigned long send_base = 0;
static unsigned long recv_base = 0;

static void PrintIacpStats(FILE *fp, IACP_STATS *stats)
{
    fprintf(fp, " %12lu", stats->nbyte);
    fprintf(fp, " %12lu", stats->nframe);
    fprintf(fp, " %12lu", stats->maxlen);
    fprintf(fp, " %12lu", stats->minlen);
    fprintf(fp, " %12lu", stats->avelen);
    fprintf(fp, " %12lu", stats->stddev);
}

BOOL LogIsiIacpStats(time_t tstamp)
{
FILE *fp;
IACP_STATS unused, recv;
char tbuf[64];

    if (param->IsiStatPath == NULL) return FALSE;
    if ((fp = fopen(param->IsiStatPath, "a+")) == NULL) return FALSE;
    fprintf(fp, "%s", utilLttostr(tstamp, 0, tbuf));
    if (isiGetIacpStats(IsiHandle, &unused, &recv)) PrintIacpStats(fp, &recv);
    fprintf(fp, "\n");
    fclose(fp);
    return TRUE;
}

#define NTAPES 8
static int dev_type(char *input)
{
int i;
struct stat statbuf;
static char *tape[NTAPES] = {
    "/dev/rwd",  "/dev/rst",  "/dev/rmt",
    "/dev/nrwd", "/dev/nrst", "/dev/nrmt",
    "/dev/rmt/", "/dev/nrmt/"
};
static char *fid = "dev_type";

/* Remote NRTS source flagged with an @ at the begining of the host name */

    if (input[0] == '@') {
        if ((RemoteHost = strdup(input + 1)) == NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        if (port > 0) {
            util_log(1, "taking input from %s:%d", RemoteHost, port);
        } else {
            util_log(1, "taking input from %s", RemoteHost);
        }
        if (init_request(sys, keeparg) != 0) {
            util_log(1, "%s: can't init request", fid);
            return PROBLEM;
        }
        return FROM_NET;
    }

/* ISI source denoted by isi@host */

    if (strncasecmp(input, "isi@", strlen("isi@")) == 0) {
        if (!isiInitDefaultPar(&IsiParam)) {
            util_log(1, "%s: isiInitDefaultPar: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        IsiParam.attr.at_dbgpath = dbgpath;
        if ((RemoteHost = strdup(input + strlen("isi@"))) == (char *) NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        if (port > 0) isiSetServerPort(&IsiParam, port);
        util_log(1, "taking ISI input from %s:%d", RemoteHost, isiGetServerPort(&IsiParam));
        if ((beg = ConvertToIsiBegTime(beg)) == ISI_UNDEFINED_TIMESTAMP) {
            util_log(1, "%s: ConvertToIsiBegTime(%.3lf) failed", fid, beg);
            return PROBLEM;
        }
        if ((end = ConvertToIsiEndTime(end, (BOOL) wavreq->keepup)) == ISI_UNDEFINED_TIMESTAMP) {
            util_log(1, "%s: ConvertToIsiEndTime(%.3lf, %s) failed", fid, end, utilBoolToString((BOOL) wavreq->keepup));
            return PROBLEM;
        }
        if ((IsiDatreq = BuildIsiDataRequest(sys, beg, end, &IsiParam)) == NULL) {
            util_log(1, "%s: BuildIsiDataRequest failed", fid);
            return PROBLEM;
        }
        if (!isiStartLogging(&IsiParam, NULL, LogIsiMessage, NULL)) {
            util_log(1, "%s: isiSetLogging: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        return FROM_ISI;
    }

/* ASL network dataloggers denoted by asl@host */

    if (strncasecmp(input, "asl@", strlen("asl@")) == 0) {
        if ((RemoteHost = strdup(input + strlen("asl@"))) == (char *) NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        return FROM_ASL;
    }

    if (strncasecmp(input, "asl2ida@", strlen("asl2ida@")) == 0) {
        if ((RemoteHost = strdup(input + strlen("asl2ida@"))) == (char *) NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            return PROBLEM;
        }
        return FROM_ASL_TO_IDA;
    }

/* Otherwise it must be a local device or file */

    if (stat(input, &statbuf) != 0) {
        util_log(1, "%s: can't stat %s: %s", fid, input, syserrmsg(errno));
        return PROBLEM;
    }

    if (S_ISREG(statbuf.st_mode)) {
        return FROM_DISK;
    } else if (S_ISCHR(statbuf.st_mode)) {
        for (i = 0; i < NTAPES; i++) {
            if (strncmp(tape[i], input, strlen(tape[i])) == 0) return FROM_TAPE;
        }
        return FROM_TTY;
    } 

    util_log(1, "%s: can't identify type of input '%s'", fid, input);
    return PROBLEM;

}
#ifndef WIN32
static int tty_getrec(char *buffer, int want)
{
int status;
static int xon       = 0;
static int timed_out = 0;
static char *fid = "tty_getrec";

    do {
        status = tty_read(ttyfd, buffer, want, timeout);
        if (status != NRTS_OK) {
            if (errno == ETIMEDOUT) {
                timed_out = 1;
                if (++xon == 1) util_log(3, "serial port timed out (%d sec)",
                    timeout
                );
                if (++xon == 1) util_log(3, "sending XON to serial port");
                if (tty_reset(ttyfd) != 0) {
                    util_log(1, "%s: ttyreset failed: %s (ignored)",
                        fid, syserrmsg(errno)
                    );
                }
            } else {
                util_log(1, "tty_read failed: %s (ignored)", syserrmsg(errno));
            }
        }
    } while (status != NRTS_OK);

    if (timed_out) util_log(3, "serial port acquistion resumed");

    timed_out = xon = 0;

    return NRTS_OK;
}
#endif
#ifdef WIN32
static int mt_getrec(char *buffer, int want)
{
static int eof = 0;
static char *fid = "mt_getrec";
return NRTS_ERROR;
}
#else
static int mt_getrec(char *buffer, int want)
{
int retval;
static int eof = 0;
static char *fid = "mt_getrec";

    while (1) {
        retval = (int) mtio_read(tapetp, buffer, want);
        if (retval = want) {
            eof = 0;
            return NRTS_OK;
        } else if (retval == 0) {
            if (++eof == 2) return NRTS_EOF;
        } else {
            return NRTS_ERROR;
        }
    }
}
#endif
static int fp_getrec(char *buffer, int want)
{
int retval;
static char *fid = "fp_getrec";

    retval = fread(buffer, sizeof(char), want, filefp);
    if (retval == want) {
        return NRTS_OK;
    } else {
        return feof(filefp) ? NRTS_EOF : NRTS_ERROR;
    }
}

static int isi_getrec(char *buffer, int want)
{
int status;
ISI_GENERIC_TS *ts;
static char *fid = "isi_getrec";

    if ((IsiHandle == NULL)) {
        IsiHandle = isiInitiateDataRequest(RemoteHost, &IsiParam, IsiDatreq);
        if (IsiHandle == NULL) {
            util_log(1, "%s: abort: request failed", fid);
            return NRTS_ERROR;
        }
        util_log(1, "connected to %s", IsiHandle->iacp->peer.ident);
    }

    while ((ts = isiReadGenericTS(IsiHandle, &status)) != NULL) {
        if (ts->hdr.nbytes != (UINT32) want) {
            util_log(1, "%s: unexpected nbytes (%lu != %d), packet ignored", fid, ts->hdr.nbytes, want);
        } else {
            memcpy(buffer, ts->data, ts->hdr.nbytes);
            return NRTS_OK;
        }
    }

    return (status == ISI_DONE) ? NRTS_EOF : NRTS_ERROR;
}

#define NUM_SLEEPINTS 8
#define MAX_REFUSED   15

static int net_getrec(char *buffer, int want)
{
int status, nbyte, lost_conn = 0;
int ready = 0;
char *data;
static unsigned long nsend = 0;
static unsigned long nrecv = 0;
static time_t beg_time = 0;
static time_t end_time = 0;
static long total_time = 0;
static long total_byte = 0;
static long total_rec  = 0;
static long overhead   = 0;
static u_long count = 0;
static struct xfer_cnf cnf;
static struct xfer_wav wav;
static int numtry = 0;
static int sd = -1;
static int contact_level;
static char *fid = "net_getrec";
static char buf[1024];

    contact_level = ready = 0;
    do {

    /* establish connection with server */

        signal(SIGHUP,  SIG_IGN);
        while (sd <= 0) {
            util_log(2, "generating request parameters");
            if (request(sys, beg, end, wavreq) != 0) {
                util_log(1, "%s: abort: request failed", fid);
                return NRTS_ERROR;
            }
            beg_time = time(NULL);
            sd = Xfer_Connect2(RemoteHost,service,port,"tcp",req,&cnf,retry,tto);
            if (sd <= 0) {
                if (
                    retry && (
                        (xfer_errno == XFER_ETIMEDOUT) ||
                        (xfer_errno == XFER_ECONNECT)  ||
                        (xfer_errno == XFER_EHOSTNAME)
                    )
                ) {
                    sleep(60);
                } else {
                    util_log(1, "%s: %s", RemoteHost, Xfer_ErrStr());
                    return NRTS_ERROR;
                }
            } else {
                contact_level = count = 0;
                util_log(2, "request accepted");
                overhead += time(NULL) - beg_time;
            }
        }

    /* read a packet from the server */
 
        if ((status = Xfer_RecvWav(sd, &cnf, &wav)) != XFER_OK) {
            close(sd);
            sd = -1;
        } else if (++count == 1) {
            util_log(1, "initial packet received");
        }
        ++total_rec;

    /* quit on EOD from server */

        if (status == XFER_FINISHED) {
            end_time = time(NULL);
            total_time += (end_time - beg_time);
            nsend = xfer_nsend - send_base;
            nrecv = xfer_nrecv - recv_base;
            util_log(2, "EOD received from %s@%s", service, RemoteHost);
            if (total_time > 0) {
                util_log(2, "%ld (%ld + %ld) RAW bytes in %ld seconds = %ld bytes/sec",
                    nsend + nrecv, nsend, nrecv, total_time,
                    (nsend + nrecv)/total_time
                );
                util_log(2, "%ld records received at %.2f records/sec",
                    total_rec, (float) total_rec / (float) total_time
                );
                util_log(2, "%ld data bytes received in %ld seconds = %ld bytes/sec",
                    total_byte, total_time, total_byte/total_time
                );
            }
            if (total_time-overhead > 0) {
                util_log(2, "rate less %ld secs of overhead = %ld bytes/sec",
                    overhead, total_byte/(total_time-overhead)
                );
            }

            return NRTS_EOF;

    /* re-establish the connection if it has gone away */

        } else if (status != XFER_OK) {
            end_time = time(NULL);
            total_time += end_time - beg_time;
            end_time = beg_time = 0;
            util_log(1, "%s not responding: %s", RemoteHost, Xfer_ErrStr());
            if (wavreq->keepup == 0) die(1);
            lost_conn = 1;

    /* Copy data to output buffer */

        } else {

            switch (wav.format) {
              case XFER_WAVIDA:
                nbyte = wav.type.ida.nbyte;
                data  = wav.type.ida.data;
#ifdef DEBUG
util_log(1, "%s: nbyte = %d, rev = %d, comp = %d", fid, wav.type.ida.nbyte, wav.type.ida.rev, wav.type.ida.comp);
util_log(1, "%s: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", fid, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
buf[0] = 0;
for (i = 0; i < 80; i++) sprintf(buf+strlen(buf),"%c", data[i] >= 0x20 && data[i] <= 0x7e ? data[i] : '.');
util_log(1, buf);
#endif
                break;
              case XFER_WAVRAW:
                nbyte = wav.type.raw.nbyte;
                data  = wav.type.raw.data;
                break;
#ifdef PASSCAL_SUPPORT
              case XFER_WAVPASSCAL:
                nbyte = wav.type.pas.nbyte;
                data  = wav.type.pas.data;
                break;
#endif /* PASSCALL_SUPPORT */
#ifdef SEED_SUPPORT
              case XFER_WAVSEED:
                nbyte = wav.type.seed.nbyte;
                data  = wav.type.seed.data;
                break;
#endif /* SEED_SUPPORT */
              default:
                util_log(1, "Unsupported format (%d)!", wav.format);
                die(1);
            }

            total_byte += nbyte;
            if (nbyte < want) {
                util_log(1, "Unexpected short (%d != %d) packet dropped",
                    nbyte, want
                );
                status = XFER_ERROR;
            } else if (nbyte > want) {
                util_log(1, "Unexpected long (%d != %d) packet dropped",
                    nbyte, want
                );
                status = XFER_ERROR;
            } else {
                memcpy(buffer, data, nbyte);
                ready = 1;
            }
        }
    } while (!ready);

    if (lost_conn) util_log(1, "%s OK", RemoteHost);

    return NRTS_OK;
}

static int asl_getrec(char *buffer, int want)
{
int lost_conn = 0;
static char **rec;
static int nrec = 0;
static int index = -1;
static u_long count = 0;
static int sd = -1;
static char *fid = "asl_getrec";


    /* establish connection with server */

    while (1) {

        signal(SIGHUP,  SIG_IGN);

        while (sd <= 0) {
            sd = util_connect(RemoteHost, NULL, port, "tcp", 0, 0);
            if (sd <= 0) {
                if (errno != ETIMEDOUT || !retry) {
                    util_log(1, "%s: %s", RemoteHost, syserrmsg(errno));
                    return NRTS_ERROR;
                }
            } else {
                lost_conn = count = 0;
            }
        }

        if (util_read(sd, buffer, want, timeout) == want) {
            if (++count == 1) util_log(1, "initial packet received");
            return NRTS_OK;
        }

        util_log(1, "%s not responding: %s", RemoteHost, syserrmsg(errno));
        shutdown(sd, 2);
        close(sd);
        sd = -1;
        lost_conn = 1;
        sleep(1);
    }
}

static int asl2ida_getrec(char *buffer, int want)
{
int i, j, status, lost_conn = 0;
long errcount = 0;
static char **rec;
static int nrec = 0;
static int index = -1;
static u_long count = 0;
static int sd = -1;
static char *fid = "asl2ida_getrec";

/* Return any pending data */

    if (nrec) {
        memcpy(buffer, rec[index], IDA_BUFSIZ);
        ++index;
        --nrec;
        return NRTS_OK;
    }

/* Otherwise get some data from the server */

    errcount = 0;
    while (1) {

    /* establish connection with server */

        signal(SIGHUP,  SIG_IGN);
        while (sd <= 0) {
            sd = util_connect(RemoteHost, NULL, port, "tcp", 0, 0);
            if (sd <= 0) {
                if (retry == 0) {
                    util_log(1, "%s: %s", RemoteHost, syserrmsg(errno));
                    util_log(1, "giving up on %s");
                    return NRTS_ERROR;
                } else {
                    sleep(60);
                }
            } else {
                lost_conn = count = 0;
            }
        }

        status = seed_readmini(sd,&asl2ida.packet,asl2ida.buf,asl2ida.blksiz,timeout);
        if (status == 0) {
            if (++count == 1) util_log(1, "initial packet received");
            for (i = 0; i < asl2ida.nsta; i++) {
                if (strcasecmp(asl2ida.packet.sname, asl2ida.sta[i].name) == 0) {
                    for (j = 0; j < asl2ida.sta[i].nchn; j++) {
                        if (
                            strcasecmp(
                                asl2ida.packet.cname, asl2ida.sta[i].chn[j].name
                            ) == 0
                        ) {
                            if (asl2ida.packet.beg > asl2ida.sta[i].chn[j].end) {
                                nrec = seed_minitoida(
                                    &asl2ida.packet, asl2ida.info, &rec
                                );
                                if (nrec >= 0 && errcount > 0) {
                                    util_log(1, "data feed restored");
                                    errcount = 0;
                                }
                                if (nrec < 0) {
                                    util_log(1, "%s: seed_minitoida: status %d",
                                        fid, nrec
                                    );
                                    shutdown(sd, 2);
                                    close(sd);
                                    sd = -1;
                                    lost_conn = 1;
                                } else if (nrec > 0) {
                                    asl2ida.sta[i].chn[j].end = asl2ida.packet.beg +
                                    (asl2ida.packet.sint*(double)asl2ida.packet.nsamp-1);
                                    memcpy(buffer, rec[0], IDA_BUFSIZ);
                                    --nrec;
                                    index = 1;
                                    return NRTS_OK;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            if (++errcount == 1) {
                util_log(1, "lost data feed: %s", syserrmsg(errno));
            }
            shutdown(sd, 2);
            close(sd);
            sd = -1;
            lost_conn = 1;
            sleep(errcount > 1 ? 30 : 1);
        }
    }
}

int init_getrec(char *homeptr, char *syscodeptr, char *iname, struct param *paramptr, int ctsrts, int leap_fix, int *flags)
{
int i, j, hlen, dlen;
char *copy;
int status;
char *input;
struct nrts_ida *info;
struct nrts_sta *sta;
struct nrts_chn *chn;
static int (*read_func)();
#ifndef WIN32
long speed;
char *token;
#endif /* !WIN32 */
static char *fid = "init_getrec";

    param   = paramptr;
    sys     = param->sys;
    req     = &param->req;
    wavreq  = param->wavreq;
    end     = param->end;
    beg     = param->beg;
    retry   = param->retry;
    tto     = param->tto;
    ida     = param->ida;
    dbgpath = param->dbgpath;
    if ((timeout = param->timeout) < 0) timeout = NRTS_DEFTTYTO;
    service = (port = param->port) < 0 ? XFER_SERVICE : NULL;
    keeparg = param->keeparg;
    home    = strdup(homeptr);
    syscode = strdup(syscodeptr);

    if (iname == NULL) {
        filefp = stdin;
        util_log(1, "taking input from stdin");
        param->read_func = fp_getrec;
        status = 0;
        *flags |= NRTS_FROMMEDIA;
    } else {
        if ((copy = strdup(iname)) == NULL) return -1;
        if ((input = strtok(copy, ",")) == NULL) return -1;
        switch (dev_type(input)) {
#ifndef WIN32
          case FROM_TTY:
            if ((token = strtok(NULL, ",")) == NULL) return -1;
            speed = atol(token);
            util_log(1, "taking input from tty port '%s', speed=%ld",
                     input, speed);
            param->read_func = tty_getrec;
            status = 
            ((ttyfd = tty_init(input, speed, timeout, sys, ctsrts, leap_fix, param->tapfile, ida)) < 0)?-1:0;
            *flags |= NRTS_FROMTTY;
            InitTeeDir(home, syscode);
            break;
#endif
#ifndef WIN32
          case FROM_TAPE:
            util_log(1, "taking input from tape '%s'", input);
            param->read_func = mt_getrec;
            status = ((tapetp = mtio_open(input, "rb")) == (TAPE *) NULL) ? -1 : 0;
            *flags |= NRTS_FROMMEDIA;
            break;
#endif
          case FROM_DISK:
            util_log(1, "taking input from file '%s'", input);
            param->read_func = fp_getrec;
            status = ((filefp = fopen(input, "rb")) == NULL) ? -1 : 0;
            *flags |= NRTS_FROMMEDIA;
            break;
          case FROM_NET:
            status = 0;
            param->read_func = net_getrec;
            util_log(1, "socket i/o timeout set to %d secs", req->timeout);
            if (wavreq->keepup) {
                util_log(1, "permanent connection requested");
            } else {
                util_log(1, "connection will break after xfer is complete");
            }
            switch (wavreq->comp) {
              case XFER_CMPNONE:
                util_log(1, "data will be transferred without compression");
                break;
              case XFER_CMPIGPP:
                util_log(1, "first difference compression requested");
                break;
              case XFER_CMPSTM1:
                util_log(1, "Steim 1 compression requested");
                break;
              default:
                util_log(1, "unknown compression (%d)", wavreq->comp);
                status = -1;
            }
            *flags |= NRTS_FROMNRTS;
            break;
          case FROM_ISI:
            status = 0;
            param->read_func = isi_getrec;
            isiSetTimeout(&IsiParam, req->timeout * 1000);
            util_log(1, "socket i/o timeout set to %d secs", req->timeout);
            if (wavreq->keepup) {
                util_log(1, "permanent connection requested");
            } else {
                util_log(1, "connection will break after xfer is complete");
            }
            switch (isiGetDatreqCompress(IsiDatreq)) {
              case ISI_COMP_NONE:
                util_log(1, "data will be transferred without compression");
                break;
              case ISI_COMP_IDA:
                util_log(1, "IDA first difference compression requested");
                break;
              case ISI_COMP_STEIM1:
                util_log(1, "Steim 1 compression requested");
                break;
              case ISI_COMP_STEIM2:
                util_log(1, "Steim 2 compression requested");
                break;
              default:
                util_log(1, "unknown compression (%lu)", isiGetDatreqCompress(IsiDatreq));
                status = -1;
            }
            *flags |= NRTS_FROMNRTS;
            break;
          case FROM_ASL:
            param->read_func = asl_getrec;
            util_log(1, "input is an ASL server at %s.%d", RemoteHost, port);
            util_log(1, "socket i/o timeout set to %d secs", timeout);
            *flags |= NRTS_FROMNRTS;
            if (sys->type != NRTS_ASL) {
                util_log(1, "%s: ASL input requires ASL output", fid);
                return -1;
            }
            retry  = 1; /* always retry LISS connections */
            status = 0;
            break;
          case FROM_ASL_TO_IDA:
            param->read_func = asl2ida_getrec;
            util_log(1, "input is an ASL server at %s.%d", RemoteHost, port);
            util_log(1, "socket i/o timeout set to %d secs", timeout);
            *flags |= NRTS_FROMNRTS;
            if (sys->type != NRTS_IDA) {
                util_log(1, "%s: ASLTOIDA input requires IDA output", fid);
                return -1;
            }
            info = (struct nrts_ida *) sys->info;
            asl2ida.rev = info->rev;
            asl2ida.blksiz = param->blksiz;
            asl2ida.buf = malloc(asl2ida.blksiz);
            util_log(1, "blocksize set to %d bytes", asl2ida.blksiz);
            asl2ida.packet.datlen = 2 * asl2ida.blksiz;
            ida_dhlen(asl2ida.rev, &hlen, &dlen);
            asl2ida.nbuf = ((2*asl2ida.blksiz) / (dlen/sizeof(long))) + 2;
            asl2ida.packet.data = (INT32 *) malloc(asl2ida.packet.datlen * sizeof(INT32));
            asl2ida.tol = param->tolerance;
            if ( asl2ida.buf == (char *) NULL || asl2ida.packet.data == (INT32 *) NULL) {
                util_log(1, "%s: malloc: %s", fid, syserrmsg(errno));
                return -1;
            }
            asl2ida.sc[0] = 0;
            asl2ida.nsta = sys->nsta;
            for (i = 0; i < sys->nsta; i++) {
                sta = sys->sta + i;
                asl2ida.sta[i].name = sta->name;
                asl2ida.sta[i].nchn = sta->nchn;
                if (i) sprintf(asl2ida.sc+strlen(asl2ida.sc), "+");
                sprintf(asl2ida.sc+strlen(asl2ida.sc), "%s:", sta->name);
                for (j = 0; j < sta->nchn; j++) {
                    chn = sta->chn + j;
                    asl2ida.sta[i].chn[j].name = chn->name;
                    asl2ida.sta[i].chn[j].end  = (double) -1;
                    if (j) sprintf(asl2ida.sc+strlen(asl2ida.sc), ",");
                    sprintf(asl2ida.sc+strlen(asl2ida.sc), "%s", chn->name);
                }
            }
            asl2ida.info = seed_toidainit(asl2ida.sc, ida, asl2ida.nbuf, asl2ida.tol);
            if (asl2ida.info == (void *) NULL) {
                util_log(1, "%s: seed_toidainit failed", fid);
                status = -1;
            } else {
                status = 0;
            }
            retry  = 1; /* always retry LISS connections */
            break;
          default:
            status = -1;
        }
    }

    if ((*flags & NRTS_FROMTTY) && (*flags & NRTS_STRICT)) {
        util_log(1, "strict external time checks will be enforced");
    } else if (*flags & NRTS_FORCETTST) {
        util_log(1, "FORCED strict external time checks");
    } else {
        util_log(1, "external time is assumed to be reliable");
    }

    return status;
}

/* Revision History
 *
 * $Log: getrec.c,v $
 * Revision 1.23  2007/01/11 18:07:47  dechavez
 * stdtypes.h adjustments for some asl calls
 *
 * Revision 1.22  2006/06/02 21:12:13  dechavez
 * removed UDP support
 *
 * Revision 1.21  2006/02/10 02:07:35  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.20  2005/09/15 14:18:16  dechavez
 * Fixed getrec problem for nrts feeds (not sure how it got commented out in the first place!)
 *
 * Revision 1.19  2005/06/10 15:42:18  dechavez
 * Rename isiSetLogging() to isiStartLogging()
 *
 * Revision 1.18  2005/05/25 23:59:40  dechavez
 * reconcile Windows/Unix builds
 *
 * Revision 1.17  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.16  2005/05/23 20:56:51  dechavez
 * WIN32 mods (AAP 05-23 update)
 *
 * Revision 1.15  2005/05/06 01:09:29  dechavez
 * added optional logging of ISI:IACP recv stats
 *
 * Revision 1.14  2005/01/28 02:09:45  dechavez
 * Added dbgpath debug support for ISI feeds
 *
 * Revision 1.13  2004/09/28 23:26:14  dechavez
 * turned down log volume
 *
 * Revision 1.12  2004/02/05 22:33:49  dechavez
 * tee packets at fixed length
 *
 * Revision 1.11  2004/02/05 21:08:57  dechavez
 * added support for tee files
 *
 * Revision 1.10  2003/12/22 22:14:18  dechavez
 * removed obsolete signal and flush wfdisc stuff
 *
 * Revision 1.9  2003/12/04 23:37:52  dechavez
 * added missing fid's
 *
 * Revision 1.8  2003/11/25 20:41:55  dechavez
 * added ISI support
 * ANSI function declarations, made local use only functions static
 *
 * Revision 1.7  2003/11/21 20:23:24  dechavez
 * removed Sigfunc casts
 *
 * Revision 1.6  2003/05/23 19:50:16  dechavez
 * added support for UDP input
 *
 * Revision 1.5  2001/09/12 22:38:58  dec
 * fixed < 1024 byte error
 *
 * Revision 1.4  2000/10/07 22:04:23  dec
 * 2.10.7
 *
 * Revision 1.3  2000/09/21 22:07:31  nobody
 * 2.10.6
 *
 * Revision 1.2  2000/02/18 00:17:58  dec
 * print state of NRTS_FORCETTST flag
 *
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
