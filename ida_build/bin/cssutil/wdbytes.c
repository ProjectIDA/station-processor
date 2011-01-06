#pragma ident "$Id: wdbytes.c,v 1.2 2003/12/10 06:15:37 dechavez Exp $"
/*======================================================================
 *
 *  Determine the number of bytes of data pointed to by a wfdisc file.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"
#include "util.h"

long total = 0;

static int getsize(fp, fname)
FILE *fp;
char *fname;
{
int i, nrecs;
struct wfdisc *wfdisc;
long  bytes = 0;
long kbytes = 0;
long mbytes = 0;

    if ((nrecs = rwfdisc(fp, &wfdisc)) <= 0) {
        fprintf(stderr,"wdbytes: bad or empty wfdisc file\n");
        exit(2);
    }

    for (i = 0; i < nrecs; i++) {
        switch (wfdisc[i].datatype[1]) {
            case '2':  bytes += wfdisc[i].nsamp * 2; break;
            case '4':  bytes += wfdisc[i].nsamp * 4; break;
            default: 
                fprintf(stderr,"wdbytes: unrecognized datatype ");
                fprintf(stderr,"'%s'\n", wfdisc[i].datatype);
                exit(1);
        }
    }
    kbytes =  bytes / 1024;
    mbytes = kbytes / 1024;
    total += bytes;

    printf("%10ld %7ld %4ld", bytes, kbytes, mbytes);
    if (fname != NULL) printf(" %s", fname);
    printf("\n");
}

int main(int argc, char **argv)
{
int i;
long bytes, kbytes, mbytes;
FILE *fp;

    if (argc == 1) {
        getsize(stdin, NULL);
    } else if (argc == 2) {
        if ((fp = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr,"wdbytes: ");
            perror(argv[1]);
        } else {
            getsize(fp, argv[1]);
            fclose(fp);
        }
    } else {
        for (i = 1; i < argc; i++) {
            if ((fp = fopen(argv[i], "r")) == NULL) {
                fprintf(stderr,"wdbytes: ");
                perror(argv[i]);
            } else {
                getsize(fp, argv[i]);
                fclose(fp);
                bytes  = total;
                kbytes = total / BYTES_PER_KBYTE;
                mbytes = total / BYTES_PER_MBYTE;
            }
        }
        printf("%10ld %7ld %4ld total\n", bytes, kbytes, mbytes);
    }

    exit(0);

}

/* Revision History
 *
 * $Log: wdbytes.c,v $
 * Revision 1.2  2003/12/10 06:15:37  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
