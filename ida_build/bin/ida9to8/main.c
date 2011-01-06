#pragma ident "$Id: main.c,v 1.2 2007/01/19 17:49:29 dechavez Exp $"
/*======================================================================
 *
 *  Convert IDA9 packets into IDA8, gzip data automatically detected
 *  decompresssed.
 *
 *====================================================================*/
#include "platform.h"
#include "ida.h"
#include "zlib.h"

extern char *VersionIdentString;

static void ida9to8(UINT8 *ida9, UINT8 *ida8)
{
static char *IDA_REV8_TAG = "ida8";
UINT8 *ip, *op;

    ip = ida9;
    op = ida8;
    memcpy(op, ip, 2); op += 2; ip += 2;
    memcpy(op, IDA_REV8_TAG, 4); op += 4; ip += 4;
    memcpy(op, ip, 54); op += 54; ip += 58;
    memcpy(op, ip, 960);
}

int main(int argc, char **argv)
{
gzFile gz;
FILE *fp = stdin;
UINT8 ida9[IDA_BUFLEN], ida8[IDA_BUFLEN];

    if (argc != 1) {
        fprintf(stderr, "%s %s\n\n", argv[0], VersionIdentString);
        fprintf(stderr, "usage: %s < IDA9_data_stream > IDA8_data_stream\n\n", argv[0]);
        fprintf(stderr, "NOTE: the stdin IDA9 data stream may be uncompressed or gzipped\n");
        exit(0);
    }
    if ((gz = gzdopen(fileno(fp), "rb")) == NULL) {
        fprintf(stderr, "%s: gzdopen: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    while (gzread(gz, ida9, IDA_BUFLEN) == IDA_BUFLEN) {
        ida9to8(ida9, ida8);
        fwrite(ida8, 1, IDA_BUFLEN, stdout);
        fflush(stdout);
    }

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2007/01/19 17:49:29  dechavez
 * fflush stdout
 *
 * Revision 1.1  2007/01/19 17:48:22  dechavez
 * initial release
 *
 */
