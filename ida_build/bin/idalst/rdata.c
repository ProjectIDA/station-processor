#pragma ident "$Id: rdata.c,v 1.2 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Read the next record.
 *
 *====================================================================*/
#include "idalst.h"

#define BUFLEN (long) IDA_BUFSIZ

int rdata(MIO *fp, char *buffer, struct counters *count)
{

    if (count->skip > 0) {
        while (count->rec < count->skip) {
            if (fread(buffer, sizeof(char), BUFLEN, fp) != BUFLEN) {
                return feof(fp) ? READ_EOF : READ_ERR;
            } else {
                ++count->rec;
            }
        }
    }
            
    while (1) {
        if (fread(buffer, sizeof(char), BUFLEN, fp) != BUFLEN) {
            return feof(fp) ? READ_EOF : READ_ERR;
        } else {
            if (count->maxrec == 0 || count->rec < count->maxrec) {
                ++count->rec;
                return READ_OK;
            } else {
                return READ_DONE;
            }
        }
    } /* Not reached, ignore possible compiler warning message */
}

/* Revision History
 *
 * $Log: rdata.c,v $
 * Revision 1.2  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
