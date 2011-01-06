#pragma ident "$Id: ttag.c,v 1.6 2006/11/13 23:55:40 dechavez Exp $"
/*======================================================================
 *
 *  Save time tag stuff
 *
 *====================================================================*/
#include "i10dmx.h"

static IDA10_TTAG_HISTORY history;
static FILE *fp = NULL;

void init_ttag()
{
char *path;

    path = ttagname();
    if ((fp = fopen(path, "w")) == NULL) {
        fprintf(stderr, "fopen: ");
        perror(path);
        exit(1);
    }
   ida10InitTtagHistory(&history);
}

void store_ttag(IDA10_TS *ts)
{
   ida10PrintTSttag(fp, ts, &history);
}

/* Revision History
 *
 * $Log: ttag.c,v $
 * Revision 1.6  2006/11/13 23:55:40  dechavez
 * Moved all print and history functions off th ida10 library, tested 10.3
 *
 * Revision 1.5  2006/08/15 01:22:12  dechavez
 * untested IDA10.3 (OFIS) support
 *
 * Revision 1.4  2004/12/10 18:17:45  dechavez
 * use ida10ClockStatusString() to print decoded status
 *
 * Revision 1.3  2002/05/15 20:19:00  dec
 * Changed numeric decomposition of ttag status to interpreted yes/no
 * strings for easier reading, compute and print offset change regardless
 * of validity of time stamp
 *
 * Revision 1.2  2002/05/15 18:21:22  dec
 * Print record number, stream name, offset difference and comments
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
