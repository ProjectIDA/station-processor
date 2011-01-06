#pragma ident "$Id: rdrec.c,v 1.12 2006/11/14 00:03:01 dechavez Exp $"
/*======================================================================
 *
 *  Read the next data record in the file.  All others are ignored.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "i10dmx.h"

extern char *Buffer;
extern int needToSwap;
extern int saveTtags;

static void RemoveBlanks(char *string)
{
int i;

    for (i = 0; i < (int) strlen(string); i++) if (string[i] == ' ') string[i] = '_';
}

int rdrec(FILE *fp, IDA10_TS *ts, struct counter *count, int verbose)
{
int status, RecordType;
static UINT8 buf[IDA10_MAXRECLEN];
static char tmpbuf[256];

    while (1) {
        status = ida10ReadRecord(fp, buf, IDA10_MAXRECLEN, &RecordType);
        if (status != IDA10_OK) {
            if (status == IDA10_EOF) return FALSE;
            sprintf(Buffer, "rdrec: ida10ReadRecord error "),
            sprintf(Buffer+strlen(Buffer), "%d at record ", status);
            sprintf(Buffer+strlen(Buffer), "%d: ",count->rec);
            sprintf(Buffer+strlen(Buffer), "%s", ida10ErrorString(status));
            logmsg(Buffer);
            return TRUE;
        }
        ++count->rec;
        if (RecordType == IDA10_TYPE_LM) {
            unpackLM(buf, count->rec);
            continue;
        } else if (RecordType == IDA10_TYPE_CF) {
            sprintf(Buffer, "record %d is type CF, ignored",
                count->rec
            );
            logmsg(Buffer);
            continue;
        } else if (RecordType == IDA10_TYPE_ISPLOG) {
            unpackII(buf, count->rec);
            continue;
        } else if (RecordType != IDA10_TYPE_TS) {
            sprintf(Buffer, "rdrec: unknown record no. %ld dropped\n",
                count->rec
            );
            logmsg(Buffer);
            continue;
        }

        if (ida10UnpackTS(buf, ts)) {
            RemoveBlanks(ts->hdr.sname);
            RemoveBlanks(ts->hdr.cname);
            if (saveTtags) store_ttag(ts);
//            if (needToSwap) ida10ReorderData(ts);
            if (verbose) printf("%s\n", ida10TStoString(ts, tmpbuf));
            return TRUE;
        } else {
            sprintf(Buffer, "rdrec: can't decode TS record no. %ld\n",
                count->rec
            );
            logmsg(Buffer);
        }
    }
}

/* Revision History
 *
 * $Log: rdrec.c,v $
 * Revision 1.12  2006/11/14 00:03:01  dechavez
 * renamed PatchName to RemoveBlanks
 *
 * Revision 1.11  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.10  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.9  2004/12/10 18:19:44  dechavez
 * don't abort on read/decode errors
 *
 * Revision 1.8  2003/10/16 18:13:38  dechavez
 * Added (untested) support for types other than INT32
 *
 * Revision 1.7  2003/05/13 23:30:06  dechavez
 * replace any blanks in station and channel name with underscores
 *
 * Revision 1.6  2002/05/15 18:21:38  dec
 * pass record number to store_ttags
 *
 * Revision 1.5  2002/03/15 23:01:36  dec
 * variable length ida10ReadRecord() reads
 *
 * Revision 1.4  2002/01/25 19:25:48  dec
 * recognize, but ignore, CF records
 *
 * Revision 1.3  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.2  2000/11/06 23:17:44  dec
 * Release 1.1.1
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
