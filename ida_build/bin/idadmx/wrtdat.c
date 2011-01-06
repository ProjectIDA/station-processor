#pragma ident "$Id: wrtdat.c,v 1.3 2006/02/09 20:14:39 dechavez Exp $"
/*======================================================================
 *
 *  Write data in current uncompressed buffer to disk file(s).
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;

void wrtdat(IDA_DREC *drec, struct counter *count)
{
IDA_DHDR *dhead;
char *path, *data;
long nbytes;
struct wflist *wflist;
BufferedStream *pOBF;

    dhead  = &drec->head;

/*  Fill output buffer  */

    data = loadbuf(dhead, drec->data, count);
    nbytes = dhead->wrdsiz * dhead->nsamp;

/*  Determine output path name and open the file  */

    path = pathname((int) dhead->dl_stream);
    pOBF = utilOpenBufferedStream(path);

    if(pOBF==NULL) {
        sprintf(Buffer,"wrtdat: %s: %s", path, syserrmsg(errno));
        logmsg(Buffer);
        die(ABORT);
    }

/*  Get the wfdisc record for this output file  */

    if ((wflist = get_wfdisc(path, pOBF, dhead)) == NULL) {
        return;
    }

/*  Write it  */

    if (utilWriteBufferedStream(pOBF, data, nbytes) != nbytes) {
        sprintf(Buffer,"wrtdat: fwrite: %s", syserrmsg(errno));
        logmsg(Buffer);
        sprintf(Buffer,"    file name = %s", path);
        sprintf(Buffer+strlen(Buffer),", record = %d\n", count->rec);
        logmsg(Buffer);
        die(ABORT);
    }

/*  Update wfdisc record, etc., and close the output file  */

    wflist->wfdisc.nsamp += (nbytes / (long) dhead->wrdsiz);

}

/* Revision History
 *
 * $Log: wrtdat.c,v $
 * Revision 1.3  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.2  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
