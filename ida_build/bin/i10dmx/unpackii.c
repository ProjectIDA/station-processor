#pragma ident "$Id: unpackii.c,v 1.2 2005/05/25 23:54:11 dechavez Exp $"
/*======================================================================
 *
 *  Unpack II (ISP log message) records
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include "i10dmx.h"

#define ISPLOG_FNAME "isplog"

extern char *Buffer;

void unpackII(UINT8 *buf, long recno)
{
FILE *fp;
static char *fid = "unpackII";

    if ((fp = fopen(ISPLOG_FNAME, "a")) == NULL) {
        sprintf(Buffer, "%s: fopen: %s: %s",
            fid, ISPLOG_FNAME, strerror(errno)
        );
        logmsg(Buffer);
    } else {
        fprintf(fp, "%s", buf+2);
        fclose(fp);
    }
}

/* Revision History
 *
 * $Log: unpackii.c,v $
 * Revision 1.2  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.1  2000/11/06 23:17:44  dec
 * Release 1.1.1
 *
 */
