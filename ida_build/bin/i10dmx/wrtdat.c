#pragma ident "$Id: wrtdat.c,v 1.4 2005/05/25 23:54:11 dechavez Exp $"
/*======================================================================
 *
 *  Write data in current uncompressed buffer to disk file(s).
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "i10dmx.h"

extern char *Buffer;

void wrtdat(IDA10_TS *ts, struct counter *count)
{
IDA10_TSHDR *hdr;
char *path;
struct wflist *wflist;
BufferedStream *pOBF;


    hdr = &ts->hdr;

/*  Determine output path name and open the file  */

    path = pathname(hdr->sname, hdr->cname);
    pOBF = utilOpenBufferedStream(path);

    if(pOBF==NULL) {
        sprintf(Buffer,"wrtdat: %s: %s", path, syserrmsg(errno));
        logmsg(Buffer);
        die(ABORT);
    }

/*  Get the wfdisc record for this output file  */

    if ((wflist = get_wfdisc(path, pOBF, hdr)) == NULL) return;

/*  Write it  */

    if (utilWriteBufferedStream(pOBF, ts->data.int8, hdr->nbytes) != hdr->nbytes) {
        sprintf(Buffer,"wrtdat: fwrite: %s", syserrmsg(errno));
        logmsg(Buffer);
        sprintf(Buffer,"    file name = %s", path);
        sprintf(Buffer+strlen(Buffer),", record = %d\n", count->rec);
        logmsg(Buffer);
        die(ABORT);
    }

/*  Update wfdisc record, etc., and close the output file  */

    wflist->wfdisc.nsamp += hdr->nsamp;

}

/* Revision History
 *
 * $Log: wrtdat.c,v $
 * Revision 1.4  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.3  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.2  2003/10/16 18:13:38  dechavez
 * Added (untested) support for types other than INT32
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
