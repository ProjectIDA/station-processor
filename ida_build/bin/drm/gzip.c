#pragma ident "$Id: gzip.c,v 1.1 2004/06/04 23:07:48 dechavez Exp $"
/*======================================================================
 *
 *  gzip the file
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "util.h"
#include "zlib.h"

int GzipRawFile(char *input, char *output)
{
FILE *ifp;
gzFile gz;
int ibytes;
#define BUFFER_LENGTH 1024
char buffer[BUFFER_LENGTH];
static char *fid = "GzipRawFile";
    
    if (input == NULL || output == NULL) {
        util_log(1, "%s: NULL input(s)", fid);
        return -1;
    }

    if ((ifp = fopen(input, "rb")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, input, strerror(errno));
        return -2;
    }

    if ((gz = gzopen(output, "wb")) == NULL) {
        util_log(1, "%s: gzopen: %s: %s", fid, output, strerror(errno));
        return -3;
    }

    while ((ibytes = fread(buffer, 1, BUFFER_LENGTH, ifp)) > 0) {
        if (gzwrite(gz, buffer, ibytes) != ibytes) {
            util_log(1, "%s: gzwrite: %s: %s", fid, output, strerror(errno));
            return -4;
        }
    }

    if (!feof(ifp)) {
        util_log(1, "%s: fread: %s: %s", fid, input, strerror(errno));
        return -5;
    }

    fclose(ifp);
    gzclose(gz);

    return 0;
}

/* Revision History
 *
 * $Log: gzip.c,v $
 * Revision 1.1  2004/06/04 23:07:48  dechavez
 * initial release
 *
 */
