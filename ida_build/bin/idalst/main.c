#pragma ident "$Id: main.c,v 1.5 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Program to list and/or check the contents of output from IDA MK3
 *  and later data loggers.
 *
 *====================================================================*/
#define MAIN
#include <stdio.h>
#include "idalst.h"

int main(int argc, char **argv)
{
char buffer[IDA_BUFSIZ];
struct counters count;
int print, check, sort, group, read_status, type;
int verbose;
struct globals global;
FILE *isplog = (FILE *) NULL;
IDA_DHDR first[IDA_MAXSTREAMS], last[IDA_MAXSTREAMS];

    Global    = &global;

    init(
        argc, argv,
        &count, &print, &check, &sort, &group, &verbose, buffer
    );

    global.count = &count;
    global.first = first;
    global.last  = last;
    global.print = print;
    global.check = check;
    
    while ((read_status = getdat(buffer, &count)) == READ_OK) {
        if (check & C_SAME) {
            if (ckdupadj(buffer, &count, print)) continue;
        }
        switch ((type = ida_rtype(buffer, ida->rev.value))) {
            case IDA_DATA: 
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_DATA        \r");
                }
                ++count.data;
                dodata(buffer,first,last,&count,print,check,sort,group);
                break;
            case IDA_CALIB:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_CALIB       \r");
                }
                ++count.calib;
                if (ida->rev.value > 4) docalib(buffer, &count, print, check);
                break;
            case IDA_LOG:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_LOG         \r");
                }
                ++count.log;
                dolog(buffer, &count, print, check);
                break;
            case IDA_CONFIG:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_CONFIG      \r");
                }
                ++count.config;
                doconfig(buffer, &count, print, check);
                break;
            case IDA_EVENT:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_EVENT       \r");
                }
                ++count.event;
                doevent(buffer, &count, print, check);
                break;
            case IDA_IDENT:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_IDENT       \r");
                }
                ++count.ident;
                doident(buffer, &count, print, check);
                break;
            case IDA_POSTHDR:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_POSTHDR     \r");
                }
                ++count.posthdr;
                break;
            case IDA_LABEL:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_LABEL       \r");
                }
                ++count.dcc;
                dolabel(buffer, &count, print);
                break;
            case IDA_RESERVED:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_RESERVED    \r");
                }
                ++count.resvd;
                break;
            case IDA_ARCEOF:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_ARCEOF      \r");
                }
                ++count.dcc;
                doarceof(buffer, &count, print);
                break;
            case IDA_DMPREC:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_DMPREC      \r");
                }
                ++count.dcc;
                dodmp(buffer, &count, print);
                break;
            case IDA_DASSTAT:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_DASSTAT     \r");
                }
                ++count.srec;
                dosrec(buffer, &count, print);
                break;
            case IDA_ISPLOG:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_ISPLOG      \r");
                }
                if (++count.isplog == 1) {
                    isplog = fopen(global.isplog, "w");
                }
                if (isplog != (FILE *) NULL) {
                    fprintf(isplog, "%s", buffer + 2);
                    fflush(isplog);
                }
                break;
            case IDA_UNKNOWN:
                if (verbose) {
                    fprintf(stderr,"Record %ld ", count.rec);
                    fprintf(stderr,"is of type IDA_UNKNOWN     \r");
                }
                ++count.notdef;
                donotdef(buffer, &count, print);
                break;
            default:   fprintf(stderr,"idalst: bad getdat retval\n");
                fprintf(stderr,"Record %ld ", count.rec);
                fprintf(stderr,"is of unknown type 0x%04x\n", type);
                ++count.notdef;
                donotdef(buffer, &count, print);
                break;
        }
        fflush(stdout);
    }
    if (verbose) fprintf(stderr,"\n");
    summary(read_status, &count, first, last, print, check);

    if (isplog != (FILE *) NULL) fclose(isplog);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.5  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.4  2003/12/10 06:31:20  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2002/03/28 02:50:35  dec
 * treat unrecognized packets as IDA_UNKNOWN
 *
 * Revision 1.2  2000/02/18 00:51:32  dec
 * #define and print VERSION
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
