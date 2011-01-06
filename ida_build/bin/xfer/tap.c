#pragma ident "$Id: tap.c,v 1.6 2003/12/10 06:27:09 dechavez Exp $"
/*======================================================================
 *
 *  General purpose nrts_edes client, allowing one to exercise all
 *  features of the protocol in all its ugly glory.  Most users will
 *  likely prefer the "ez" interface, given by eztap.c.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include "util.h"
#include "xfer.h"

#define VERSION "Version 2.2.1"

void help(myname)
char *myname;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s if=[sta@]host [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"cnf=gen     => config only (gen(eric) format only\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"The following valid only if cnf option not used:\n");
    fprintf(stderr,"beg=string  => beg time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"end=string  => end time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"chan=list   => list of channels to request\n");
    fprintf(stderr,"sc=list     => list of stations/channels to request\n");
    fprintf(stderr,"format=code => format code (gen|ida|xfer|seed)\n");
    fprintf(stderr,"to=secs     => set read timeout\n");
    fprintf(stderr,"tto=secs    => set 'tiny-time-out' for connect()\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"port=val    => TCP/IP port number\n");
    fprintf(stderr,"tcpbuf=val  => set TCP/IP buffer length to val\n");
    fprintf(stderr,"+/-retry    => toggle timeout retry behavior\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"The old style syntax of specifying the station ");
    fprintf(stderr,"name as part of the\n");
    fprintf(stderr,"if= argument is supported.  In this case, you ");
    fprintf(stderr,"must specify the desired\n");
    fprintf(stderr,"channel list using the chan= argument.  If no ");
    fprintf(stderr,"chan= argument is given\n");
    fprintf(stderr,"then chan=bhz,bhn,bhe is assumed.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"The more general style is to specify if=host, and ");
    fprintf(stderr,"then use sc= to\n");
    fprintf(stderr,"list the desired stations and names.  For a single ");
    fprintf(stderr,"station request\n");
    fprintf(stderr,"this would be sc=sta:chn,... (eg, sc=pfo:bhz,bhn,");
    fprintf(stderr,"bhe) while for a\n");
    fprintf(stderr,"multiple station request this would be sc=sta:chn");
    fprintf(stderr,"...+sc=sta:chn,...\n");
    fprintf(stderr,"(eg, sc=abkt:bhz,bhn,bhe+tau:vhz,vhn,vhe).\n");
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
char *tmp       = NULL;
char *input     = NULL;
char *log       = NULL;
char *service   = NULL;
char *sta       = NULL;
char *host      = NULL;
char *cnfformat = NULL;
char *wavformat = NULL;
char *chan      = NULL;
char *sc        = NULL;
int xfer        = 1;
int nrts        = 0;
int debug       = 0;
int port        = -1;
int keepup      = 1;
int retry       = 0;
int tto         = 0;
int sd, i, status, wrstatus, cnffmt, wavfmt;
unsigned long nsend, nrecv, sendbase, recvbase, nrec;
struct timeval start, stop;
double beg = XFER_YNGEST;
double end = XFER_YNGEST;
double dstart, dstop, elapsed, rate;
struct xfer_packet xfer_packet;
struct xfer_cnf cnf;
struct xfer_req req;
struct xfer_wav wav;
#define STACHN_LEN 1023
static char stachn[STACHN_LEN+1];
static char def_wavformat[] = "xfer";
static char def_chnlst[]    = "*";
 
    wavformat = def_wavformat;

/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i]+strlen("debug="));
        } else if (strncasecmp(argv[i], "to=", strlen("to=")) == 0) {
            req.timeout = atoi(argv[i] + strlen("to="));
        } else if (strncasecmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncasecmp(argv[i], "tto=", strlen("tto=")) == 0) {
            tto = atoi(argv[i] + strlen("tto="));
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "tcpbuf=", strlen("tcpbuf=")) == 0) {
            req.sndbuf = req.rcvbuf = atoi(argv[i] + strlen("tcpbuf="));
        } else if (strncasecmp(argv[i], "format=", strlen("format=")) == 0){
            wavformat = argv[i] + strlen("format=");
        } else if (strcasecmp(argv[i], "+retry") == 0) {
            retry = 1;
        } else if (strcasecmp(argv[i], "-retry") == 0) {
            retry = 0;
        } else if (strncasecmp(argv[i], "chan=", strlen("chan=")) == 0) {
            chan = strdup(argv[i] + strlen("chan="));
            sc   = (char *) NULL;
        } else if (strncasecmp(argv[i], "sc=", strlen("sc=")) == 0) {
            sc   = strdup(argv[i] + strlen("sc="));
            chan = (char *) NULL;
        } else if (strncasecmp(argv[i], "if=", strlen("if=")) == 0) {
            input = argv[i] + strlen("if=");
        } else if (strncasecmp(argv[i], "beg=", strlen("beg=")) == 0) {
            tmp = util_lcase(argv[i] + strlen("beg="));
            if (strcasecmp(tmp, "beg") == 0) {
                beg = XFER_OLDEST;
            } else if (strcasecmp(tmp, "end") == 0) {
                beg = XFER_YNGEST;
            } else {
                beg = util_attodt(argv[i] + strlen("beg="));
            }
        } else if (strncasecmp(argv[i], "end=", strlen("end=")) == 0) {
            tmp = util_lcase(argv[i] + strlen("end="));
            if (strcasecmp(tmp, "beg") == 0) {
                end = XFER_OLDEST;
            } else if (strcasecmp(tmp, "end") == 0) {
                end = XFER_YNGEST;
            } else {
                end = util_attodt(argv[i] + strlen("end="));
            }
            keepup = 0;
        } else if (strncasecmp(argv[i], "config=", strlen("config=")) == 0) {
            cnfformat = argv[i] + strlen("config=");
        } else if (strncasecmp(argv[i], "cnf=", strlen("cnf=")) == 0) {
            cnfformat = argv[i] + strlen("cnf=");
        } else if (strcasecmp(argv[i], "config") == 0) {
            cnfformat = "gen";
        } else {
            help(argv[0]);
        }
    }

/* If the remote port was not given, then use the system defined service */
 
    service = (port < 0) ? XFER_SERVICE : (char *) NULL;

/* Determine the type of request */

   req.type = (cnfformat == (char *) NULL) ? XFER_WAVREQ : XFER_CNFREQ;

/* Must specify input system */

    if (input == (char *) NULL) help(argv[0]);

    if ((tmp = strchr(input, '@')) == (char *) NULL || input[0] == '@') {
        if (req.type == XFER_WAVREQ) {
            if (sc == (char *) NULL) {
                fprintf(stderr, "if=host requires sc=sta:chn,chn,...\n");
                help(argv[0]);
            } else if (strchr(sc, ':') == (char *) NULL) {
                fprintf(stderr, "no `:' delimited channel names in sc= \n");
                help(argv[0]);
            }
        }
        host = (input[0] == '@') ? input + 1 : input;
        sta  = (char *) NULL;
    } else {
        if (chan == (char *) NULL) chan = def_chnlst;
        if (strchr(chan, '+') != (char *) NULL) {
            fprintf(stderr, "illegal chan= string (+ not allowed)\n");
            help(argv[0]);
        }
        if (strchr(chan, ':') != (char *) NULL) {
            fprintf(stderr, "illegal chan= string (: not allowed)\n");
            help(argv[0]);
        }
        host = tmp + 1;
        *tmp = 0;
        sta  = input;
        sprintf(stachn, "%s:%s", sta, chan);
        sc = stachn;
    }

/* Figure out the formats */

    if (req.type == XFER_CNFREQ) {

        if (strcasecmp(cnfformat, "gen") == 0) {
            cnffmt = XFER_CNFGEN1;
        } else if (strcasecmp(cnfformat, "xfer") == 0) {
            cnffmt = XFER_CNFGEN1;
        } else if (strcasecmp(cnfformat, "nrts") == 0) {
            cnffmt = XFER_CNFGEN1;
        } else {
            fprintf(stderr, "bad configuration format `%s'\n", cnfformat);
            help(argv[0]);
        }
        wavfmt = -1;
        
    } else {

        cnffmt= XFER_CNFGEN1;
        nrts  = 0;
        xfer  = 0;
        if (strcasecmp(wavformat, "nrts") == 0) {
            cnffmt = XFER_CNFNRTS;
            wavfmt = XFER_WAVGEN1;
            nrts = 1;
            xfer = 0;
        } else if (strcasecmp(wavformat, "xfer") == 0) {
            wavfmt = XFER_WAVGEN1;
            xfer = 1;
            nrts = 0;
        } else if (strcasecmp(wavformat, "gen") == 0) {
            wavfmt = XFER_WAVGEN1;
        } else if (strcasecmp(wavformat, "ida") == 0) {
            cnffmt = XFER_CNFNRTS;
            wavfmt = XFER_WAVIDA;
        } else if (strcasecmp(wavformat, "seed") == 0) {
            wavfmt = XFER_WAVSEED;
        } else if (strcasecmp(wavformat, "reftek") == 0) {
            wavfmt = XFER_WAVPASSCAL;
        } else if (strcasecmp(wavformat, "raw") == 0) {
            wavfmt = XFER_WAVRAW;
        } else {
            fprintf(stderr, "bad waveform format `%s'\n", wavformat);
            help(argv[0]);
        }
    }

/* Fill out the request */

    if (Xfer_FillReq(&req, cnffmt, wavfmt, sc, beg, end, keepup, 0) != 0) {
        fprintf(stderr, "Xfer_FillReq failed!\n");
        exit(1);
    }

/*  Start logging */

    if (debug) util_logopen(log, 1, 9, debug, NULL, argv[0]);
    util_log(1, "%s", VERSION);

    if (req.type == XFER_WAVREQ) {
        util_log(1, "waveform request, format=%s", wavformat);
    } else {
        util_log(1, "configuration request, format=%s", cnfformat);
    }

    if (service != (char *) NULL) {
        util_log(1, "data source = %s.%s", host, service);
    } else {
        util_log(1, "data source = %s.%d", host, port);
    }

/* Initiate connection with server and submit request */

    util_log(2, "Xfer_Connect()'ing");
    sendbase = xfer_nsend;
    recvbase = xfer_nrecv;
    sd = Xfer_Connect2(host, service, port, "tcp", &req, &cnf, retry, tto);
    if (sd < 0) {
        fprintf(stderr, "%s: %s: %s\n",
            argv[0], host, Xfer_ErrStr()
        );
        exit(1);
    }
    nsend = xfer_nsend - sendbase;
    nrecv = xfer_nrecv - recvbase;
    util_log(2, "%ld request bytes sent, %ld conf bytes received",
        nsend, nrecv
    );

/* If this is a configuration only request then print it out and quit */

    if (req.type == XFER_CNFREQ) {
        if (Xfer_WriteCnf(stdout, &cnf) != 0) {
            fprintf(stderr, "%s: ", argv[0]);
            perror("Xfer_WriteCnf");
            Xfer_Exit(sd, xfer_errno);
        }
        Xfer_Exit(sd, 0);
    }

/*  Otherwise read/write data packets */

    nrec = 0;
    sendbase = xfer_nsend;
    recvbase = xfer_nrecv;
    util_log(2, "begin Xfer_RecvWav() loop");
    gettimeofday(&start, NULL);
    while ((status = Xfer_RecvWav(sd, &cnf, &wav)) == XFER_OK) {
        ++nrec;
        if (xfer) {
            if (xfer_Convert(&cnf, &wav, &xfer_packet) != 0) {
                fprintf(stderr, "%s: Xfer_CvtNRTS failure!", argv[0]);
                Xfer_Exit(sd, 1);
            }
            if ((wrstatus = Xfer_WrtPacket(stdout, &xfer_packet)) != 0) {
                fprintf(stderr, "%s: Xfer_WrtPacket: status %d: %s\n",
                    argv[0], wrstatus, syserrmsg(errno)
                );
                Xfer_Exit(sd, xfer_errno);
            }
        } else {
            if (Xfer_WriteWav(stdout, &wav, 1) != XFER_OK) {
                fprintf(stderr, "%s: Xfer_WriteWav: %s\n",
                    argv[0], syserrmsg(errno)
                );
                Xfer_Exit(sd, xfer_errno);
            }
        }
    }
    gettimeofday(&stop, NULL);
    dstart = (double) start.tv_sec + ((double) start.tv_usec / 1000000.0);
    dstop  = (double) stop.tv_sec  + ((double) stop.tv_usec  / 1000000.0);
    elapsed = dstop - dstart;
    nrecv = xfer_nrecv - recvbase;
    rate  = ((double) nrecv / elapsed) / 1024.0;
    util_log(2, "%ld bytes received in %.3lf secs: %.2lf Kbyte/sec",
        nrecv, elapsed, rate
    );
    util_log(2, "%ld records received: %.2lf rec/sec",
        nrec, (double) nrec / elapsed
    );

    if (status == XFER_FINISHED) {
        util_log(1, "data transfer completed OK");
        exit(0);
    } else {
        util_log(1, "data transfer failed: %s", Xfer_ErrStr());
        exit(1);
    }
}

/* Revision History
 *
 * $Log: tap.c,v $
 * Revision 1.6  2003/12/10 06:27:09  dechavez
 * update version to 2.2.1 to reflect relink with solaris cc calmed libraries
 *
 * Revision 1.5  2003/10/16 18:22:56  dechavez
 * version 2.2.0, removed NRTS format support
 *
 * Revision 1.4  2003/06/11 21:51:23  dechavez
 * commented #endif label
 *
 * Revision 1.3  2002/02/23 00:07:52  dec
 * explicitly state cnf format for ida and nrts feeds, added manually
 * updated version string (2.1.1)
 *
 * Revision 1.2  2001/05/20 17:41:47  dec
 * replace ftime() with more portable gettimeofday()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
