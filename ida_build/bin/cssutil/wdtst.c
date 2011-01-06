#pragma ident "$Id: wdtst.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Test to see if the foff, nsamp, and datatype fields are consistent
 *  with the physical length of the data file.
 *
 *  Can read both 2.8 and 3.0 wfdiscs.
 *
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"
#include "cssio.h"

int compare(aptr, bptr)
#ifdef __STDC__
const void *aptr, *bptr;
#else
void *aptr, *bptr;
#endif
{
int val;
struct wfdisc *a, *b;

    a = (struct wfdisc *) aptr;
    b = (struct wfdisc *) bptr;

    if (strcmp(a->dir, b->dir) != 0)     return 0;
    if (strcmp(a->dfile, b->dfile) != 0) return 0;
    if (a->foff == b->foff)              return 0;
    return (a->foff > b->foff) ? 1 : -1;
}

int examine(wfdisc, sbuf, pos)
struct wfdisc *wfdisc;
struct stat *sbuf;
long pos;
{
off_t len, eof, end;

    eof = sbuf->st_size;
    len = (off_t) wfdisc->nsamp * (off_t) cssio_wrdsize(wfdisc->datatype);
    end = (off_t) wfdisc->foff + len;

    if (end > eof) {
        printf("record %ld points to %ld bytes past physical eof!\n",
            pos, (long) end - eof
        );
        return 1;
    }

    return 0;
}

void process(fp, fname, myname)
FILE *fp;
char *fname;
char *myname;
{
long i, nrec, total, errors;
char path[MAXPATHLEN+1];
double exp_endtime, diff, diff_samp;
struct wfdisc *wfdisc;
struct stat sbuf;

    printf("\nreading wfdisc file `%s'\n", fname);
    if ((nrec = rwfdisc(fp, &wfdisc)) < 0) {
        fprintf(stderr, "%s: rwfdisc failed\n", myname);
        exit(1);
    }
    printf("%ld records read\n", nrec);
    if (nrec == 0) exit(0);

    qsort((char *) wfdisc, nrec, sizeof(struct wfdisc), compare);

/* Check for internal consistency */

    for (total = 0, i = 0; i < nrec; i++) {
        errors = 0;
        if (wfdisc[i].foff < 0) {
            if (++errors == 1) printf("\n");
            printf("illegal foff = %ld\n", wfdisc[i].foff);
        }

        if (cssio_wrdsize(wfdisc[i].datatype) <= 0) {
            if (++errors == 1) printf("\n");
            printf("illegal datatype = %s\n", wfdisc[i].datatype);
        }

        if (wfdisc[i].nsamp <= 0) {
            if (++errors == 1) printf("\n");
            printf("illegal nsamp = %s\n", wfdisc[i].nsamp);
        }

        exp_endtime = wfdisc[i].time +
            ((double) wfdisc[i].nsamp / (double) wfdisc[i].smprate);
        if (wfdisc[i].endtime > exp_endtime) {
            diff = wfdisc[i].endtime - exp_endtime;
        } else {
            diff = exp_endtime - wfdisc[i].endtime;
        }
        diff_samp = diff * (double) wfdisc[i].smprate;
        if (diff_samp > 2.0) {
            if (++errors == 1) printf("\n");
            printf("actual - expected endtime = %.3lf sec (%.3lf samp)\n",
                diff, diff_samp
            );
        }

        sprintf(path, "%s/%s", wfdisc[i].dir, wfdisc[i].dfile);
        if (stat(path, &sbuf) != 0) {
            if (++errors == 1) printf("\n");
            printf("can't stat `%s': %s\n", path, syserrmsg(errno));
        }

        if (errors) {
            printf("record number %d rejected\n", i+1);
            ++total;
        } else {
            total += examine(wfdisc + i, sbuf, i+1);
        }
    }
    printf("%ld corrupt records detected\n", total);
}

void help(myname)
char *myname;
{
    fprintf(stderr,"usage: %s [wfdisc_file_name]\n", myname);
    exit(1);
}

int main(int argc, char **argv)
{
long i;
FILE *ifp;

    if (argc == 1) {
        process(stdin, "stdin");
    } else {
        for (i = 1; i < argc; i++) {
            if (strncmp(argv[i], "-h", strlen("-h")) == 0) help(argv[0]);
            if ((ifp = fopen(argv[i], "r")) == NULL) {
                fprintf(stderr, "%s: ", argv[0]);
                perror(argv[i]);
                exit(1);
            }
            process(ifp, argv[i], argv[0]);
            fclose(ifp);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wdtst.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
