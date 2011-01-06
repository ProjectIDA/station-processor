#pragma ident "$Id: main.c,v 1.8 2007/09/25 21:03:30 dechavez Exp $"
/*======================================================================
 *
 *  Program to demultiplex and decompress IDA 10 format data into CSS 3.0
 *  format disk files.
 *
 *====================================================================*/
#include "i10dmx.h"

/*  Global variables  */

char *Buffer;
int saveTtags = 0;

int main(int argc, char **argv)
{
FILE *fp;
int verbose;
struct counter count;
IDA10_TS ts;
char buffer[2 * IDA10_MAXRECLEN];

    fprintf(stderr,"%s %s\n", argv[0], VersionIdentString);

    Buffer = buffer;
    init(argc, argv, &count, &verbose, &fp);
    while (rdrec(fp, &ts, &count, verbose)) {
        wrtdat(&ts, &count);
        RunDetector(&ts, &count);
    }
    die(NORMAL);

}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.8  2007/09/25 21:03:30  dechavez
 * added detector support
 *
 * Revision 1.7  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.6  2002/05/15 18:20:33  dec
 * use VersionIdentString variable instead of #defined VERSION
 *
 * Revision 1.5  2002/03/15 23:04:09  dec
 * FILE input only, explicit reclen requirement/assumption removed
 *
 * Revision 1.4  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.3  2000/02/18 06:39:15  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 01:07:37  dec
 * added ReleaseNotes, christened 1.1.0
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
