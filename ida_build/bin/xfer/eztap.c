#pragma ident "$Id: eztap.c,v 1.5 2006/08/15 01:33:45 dechavez Exp $"
/*======================================================================
 *
 *  Example nrts_edes client, to demonstrate the use of the routines
 *  for simplest access to remote data.
 *
 *  The desired stations and channels are specified in a string of
 *  the form: sta:chn,...[+sta:chn,...]...
 *
 *  Examples will probably make it clearer that trying to explain:
 *
 *  example 1: pfo:bhz,bhn,bhe
 *  example 2: pfo:bhz,bhn,bhe+aru:bhz,bhn,bhe
 *
 *  With this program, set retry to 0 (the default) if you want the
 *  program to give up in case of I/O timeouts with the server.  If
 *  retry is non-zero, then in case of timeout the program will close
 *  the existing connection and attempt to reconnect.  Any errors other
 *  than timeout will cause the client to break the connection and the
 *  program will quit.
 *
 *  Examples of the use of this program:
 *
 *  eztap if=idahub.ucsd.edu sc=pfo:bhz,bhn,bhe+tau:bhz,bhn,bhe | more
 *
 *  will start a continuous feed of PFO and TAU broad-band data, starting
 *  with the youngest data currently on-line and continuing with new
 *  packets as they are acquired from the station(s).  This program will
 *  run indefinitely.
 *
 *  eztab if=idahub.ucsd.edu sc=pfo:bhz \
 *        beg=1997:071-23:55 end=1997:072-00:05 | more
 *
 *  will bring over a 10 minute segment of BHZ data centered around
 *  midnight.  This program will exit when the data are complete, or
 *  when the server has advised us that there is no chance of the data
 *  ever being complete.
 *
 *  The data are returned in an xfer_packet structure, defined in
 *  xfer.h  In this example we print a few of the fields, to verify
 *  that data are being acquired.  Below is a list of everything that
 *  is returned in the packet:
 *
 *  Type     Name     Descriptions
 *  char *   sname    station name
 *  float    lat      station latitutude, decimal degrees
 *  float    lon      station longitude, decimal degrees
 *  float    elev     station elevation, meters ASL
 *  char *   cname    channel name
 *  char *   instype  GSE2.0 instype
 *  float    depth    sensor depth of burial, meters 
 *  float    sint     nominal sample interval in seconds
 *  float    calib    CSS 3.0 calib
 *  float    calper   CSS 3.0 calper
 *  float    vang     vertical orientation of sensor
 *  float    hang     horizontal orientation of sensor
 *  double   beg      time of first sample in packet
 *  double   end      time of last  sample in packet
 *  int      tear     if set, there was a time tear
 *  long     nsamp    number of samples
 *  long *   data     the data, as properly ordered longs
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "util.h"
#include "xfer.h"

void help(myname)
char *myname;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s if=host sc=sta:chn,... [options]\n",
        myname
    );
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"port=int    => port number\n");
    fprintf(stderr,"beg=string  => beg time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"end=string  => end time (yyyy:ddd-hh:mm:ss)\n");
    fprintf(stderr,"+/-retry    => toggle timeout retry behavior\n");
    fprintf(stderr,"-l          => print packet latency\n");
    fprintf(stderr,"\n");
    exit(1);
}

int main(int argc, char **argv)
{
int i, status;
XFER *xp;
char *tmp;
char *host = NULL;
char *sc   = NULL;
int keepup = 1;
int retry  = 0;
int port   = -1;
long latency;
double beg = XFER_YNGEST;
double end = XFER_YNGEST;
struct xfer_packet packet;
BOOL debug = FALSE, PrintLatency = FALSE;
 
/*  Get command line arguments  */

    for (i = 1; i < argc; i++) {
        if (strncasecmp(argv[i], "if=", strlen("if=")) == 0) {
            host = argv[i] + strlen("if=");
        } else if (strncasecmp(argv[i], "server=",strlen("server=")) == 0) {
            host = argv[i] + strlen("server=");
        } else if (strncasecmp(argv[i], "port=", strlen("port=")) == 0) {
            port = atoi(argv[i] + strlen("port="));
        } else if (strncasecmp(argv[i], "sc=", strlen("sc=")) == 0) {
            sc   = strdup(argv[i] + strlen("sc="));
        } else if (strcasecmp(argv[i], "+retry") == 0) {
            retry = XFER_MINIMAL_GAP;
        } else if (strcasecmp(argv[i], "-retry") == 0) {
            retry = 0;
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
        } else if (strcasecmp(argv[i], "-debug") == 0) {
            debug = TRUE;
        } else if (strcasecmp(argv[i], "-l") == 0) {
            PrintLatency = TRUE;
        } else {
            help(argv[0]);
        }
    }

/* Must specify host and sta/chan string */

    if (host == (char *) NULL || sc == (char *) NULL) help(argv[0]);

    if (debug) util_logopen("-", 1, 9, 3, NULL, argv[0]);

/* Open connection to server */

    if (port > 0) {
        xp = Xfer_Open2(host, port, sc, beg, end, keepup, retry, 0, 0);
    } else {
        xp = Xfer_Open(host, sc, beg, end, keepup, retry);
    }
    if (xp == (XFER *) NULL) {
        fprintf(stderr, "%s: can't connect with server: %s\n",
            argv[0], Xfer_ErrStr()
        );
        exit(1);
    }

/* Loop until we are done.  For demonstration purposes we will
 * print a few of the packet header contents. 
 */

    while ((status = Xfer_Read(xp, &packet)) == XFER_OK) {
    /* print selected header contents... see xfer.h for full selection */
        printf("%4s:%-5s %s ",
            packet.sname,
            packet.cname,
            util_dttostr(packet.beg, 0)
        );
        printf("%s %6.2f %4ld %9ld %9ld",
            util_dttostr(packet.end, 0),
            packet.sint,
            packet.nsamp,
            packet.data[0],
            packet.data[packet.nsamp-1]
        );
        if (PrintLatency) {
            printf(" %s", util_lttostr(latency, 8));
            latency = time(NULL) - (long) packet.end;
        }
        printf("\n");
    }

/* Close the connection */

    Xfer_Close(xp);

/* See if we got everything we wanted */

    if (status != XFER_FINISHED) {
        fprintf(stderr, "%s: Xfer_Read failed: %s\n",
            argv[0], Xfer_ErrStr()
        );
        exit(1);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: eztap.c,v $
 * Revision 1.5  2006/08/15 01:33:45  dechavez
 * added -l option for latency reports
 *
 * Revision 1.4  2004/09/28 22:57:51  dechavez
 * prettier printouts
 *
 * Revision 1.3  2003/10/16 18:20:38  dechavez
 * added port option
 *
 * Revision 1.2  2002/02/23 00:08:31  dec
 * added debug command line arguments and support
 *
 * Revision 1.1.1.1  2000/02/08 20:20:20  dec
 * import existing IDA/NRTS sources
 *
 */
