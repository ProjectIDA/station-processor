#pragma ident "$Id: idaspan.c,v 1.7 2008/03/21 02:39:43 dechavez Exp $"
/*======================================================================
 *
 *  Print time range spanned by IDA data, gzip data automatically
 *  detected and decompresssed.
 *
 *====================================================================*/
#include "util.h"
#include "ida.h"
#include "ida10.h"
#include "zlib.h"

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s rev=code [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"reclen=nbyte => set record length\n");
    fprintf(stderr,"\n");
    fprintf(stderr, "defaults: reclen=960\n");
    exit(1);
}

BOOL DecodeMKx(char *buffer, int rev, double *tofs, double *tols, IDA *ida)
{
IDA_DHDR hdr;

    if (ida_rtype(buffer, rev) != IDA_DATA) return FALSE;
    if (ida_dhead(ida, &hdr, buffer) != 0) return FALSE;

    *tofs = hdr.beg.tru;
    *tols = hdr.end.tru;
    return TRUE;
}

BOOL DecodeSAN(char *buffer, int rev, double *tofs, double *tols, VOID *unused)
{
IDA10_TSHDR hdr;

    if (ida10Type((UINT8 *) buffer) != IDA10_TYPE_TS) return FALSE;
    if (ida10UnpackTSHdr((UINT8 *) buffer, &hdr) == 0) return FALSE;

    *tofs = hdr.tofs;
    *tols = hdr.tols;
    return TRUE;
}

int main(int argc, char **argv)
{
char *buffer, *dbspec=NULL, *log=NULL, *path=NULL;
int i, debug=-1, rev=-1, reclen=IDA_BUFLEN;
BOOL first=TRUE, (*decode)();
double tofs, tols;
struct {
    double time;
    char string[64];
} oldest, yngest;
IDA ida;
gzFile gz;
FILE *fp = stdin;
static char *defdir = "/usr/nrts";

	idaInitHandle(&ida);

    if (argc < 2) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            rev = atoi(argv[i]+strlen("rev="));
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "if=", strlen("if=")) == 0) {
            path = argv[i] + strlen("if=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "reclen=", strlen("reclen=")) == 0) {
            reclen = atoi(argv[i]+strlen("reclen="));
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else {
            help(argv[0]);
        }
    }

    if (path != NULL && (fp = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "%s: fopen: %s: %s\n", argv[0], path, strerror(errno));
        exit(1);
    }
    if ((gz = gzdopen(fileno(fp), "rb")) == NULL) {
        fprintf(stderr, "%s: gzdopen: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if (rev < 0) {
        fprintf(stderr, "%s: must specify data format rev code!\n", argv[0]);
        help(argv[0]);
    }
	ida.rev.value = rev;

    if ((buffer = (char *) malloc (reclen)) == NULL) {
        fprintf(stderr, "%s: ", argv[0]);
        perror("malloc");
        exit(1);
    }

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);

/*  Select decoder function/handle */

    if (rev < 10) {
        decode = DecodeMKx;
        if ((ida.db = dbioOpen(dbspec, NULL)) == NULL) {
            fprintf(stderr, "%s: dbioOpen: %s\n", argv[0], strerror(errno));
            exit(1);
        }
    } else {
        decode = DecodeSAN;
        ida.db = NULL;
    }

/* Read/decode until done */

    strcpy(oldest.string, "<undetermined>");
    strcpy(yngest.string, "<undetermined>");

    while (gzread(gz, buffer, reclen) == reclen) {
        if ((*decode)(buffer, rev, &tofs, &tols, &ida)) {
            if (first) {
                oldest.time = tofs;
                yngest.time = tols;
                first = FALSE;
            } else {
                if (tofs < oldest.time) oldest.time = tofs;
                yngest.time = tols;
            }
        }
    }

    if (!gzeof(gz)) {
        fprintf(stderr, "%s: gzread: %s", argv[0], strerror(errno));
        exit(1);
    }

    if (!first) {
        strcpy(oldest.string, util_dttostr(oldest.time, 0));
        oldest.string[14] = 0;
        strcpy(yngest.string, util_dttostr(yngest.time, 0));
        yngest.string[14] = 0;
    }

    printf("%s %s\n", oldest.string, yngest.string);

    exit(0);
}

/* Revision History
 *
 * $Log: idaspan.c,v $
 * Revision 1.7  2008/03/21 02:39:43  dechavez
 * fixed uninitialized pointer problem (always there but not visible until ported to FreeBSD)
 *
 * Revision 1.6  2006/02/10 01:49:05  dechavez
 * dbio support
 *
 * Revision 1.5  2005/10/20 19:57:29  dechavez
 * added support for gzip'd files, updated default dbspec
 *
 * Revision 1.4  2004/06/21 20:14:09  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.3  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2002/03/11 19:17:35  dec
 * changed string for unknown time to <undetermined>
 *
 * Revision 1.1  2002/02/15 21:50:31  dec
 * created
 *
 */
