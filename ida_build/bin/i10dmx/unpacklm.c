#pragma ident "$Id: unpacklm.c,v 1.1.1.1 2000/02/08 20:20:01 dec Exp $"
/*======================================================================
 *
 *  Unpack LM (Log Message) records.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "i10dmx.h"

extern char *Buffer;

void unpackLM(UINT8 *buf, long recno)
{
FILE *fp;
IDA10_LM lm;
static char *fid = "unpackLM";

    if (!ida10UnpackLM(buf, &lm)) {
        sprintf(Buffer, "%s: can't decode LM record no. %ld\n",
            fid, recno
        );
        logmsg(Buffer);
        return;
    }

    if ((fp = fopen(logname(lm.sname), "a")) == NULL) {
        sprintf(Buffer, "%s: fopen: %s: %s",
            fid, logname, strerror(errno)
        );
        logmsg(Buffer);
    } else {
        fprintf(fp, "%s", lm.text);
        fclose(fp);
    }
}

/* Revision History
 *
 * $Log: unpacklm.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
