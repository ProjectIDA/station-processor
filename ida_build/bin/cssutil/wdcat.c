#pragma ident "$Id: wdcat.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Merges all data pointed to by a wfdisc file.  Distinct records for
 *  a particular channel are collapsed into a single record as long as
 *  the sample rate is exactly identical and there is zero gap or
 *  overlap between sequential records for a particular stream.
 *
 *  The data which are pointed to by this new wfdisc file are written
 *  to a .w file whose pathname is given by the required dir and dfile
 *  command line arguments.
 *
 *====================================================================*/
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/param.h>
#include "cssio.h"
#include "util.h"

#define MAXWRDLEN 64

char path[MAXPATHLEN+1];
            
static int wdcompare(const void *aptr, const void *bptr)
{
int val;
struct wfdisc *a, *b;

    a = (struct wfdisc *) aptr;
    b = (struct wfdisc *) bptr;

    if ((val = strcmp(a->sta, b->sta)) != 0) {
        return val;
    } else if ((val = strcmp(a->chan, b->chan)) != 0) {
        return val;
    } else {
        return a->time - b->time;
    }
}

static int copy(wfp, out, in)
FILE *wfp;
struct wfdisc *out;
struct wfdisc *in;
{
FILE *fp;
int wrdsize, status;
long i;
char *buffer[MAXWRDLEN];

    assert((wrdsize = cssio_wrdsize(in->datatype)) <= MAXWRDLEN);

    sprintf(path, "%s/%s", in->dir, in->dfile);
    if ((fp = fopen(path, "rb")) == NULL) {
        fprintf(stderr,"wdcat: fopen: ");
        perror(path);
        exit(4);
    }

    if (fseek(fp, in->foff, 0) != 0) {
        fprintf(stderr,"wdcat: fseek: ");
        perror(path);
        exit(4);
    }

    clearerr(fp);
    for (i = 0; i < in->nsamp; i++) {
        status = fread(buffer, sizeof(char), wrdsize, fp);
        if (status != wrdsize) {
            if (feof(fp)) {
                fprintf(stderr, "wdcat: unexpected %s EOF\n", path);
                fprintf(stderr, "wdcat: %ld of %ld samples written\n",
                    i, in->nsamp
                );
                exit(5);
            } else {
                fprintf(stderr, "wdcat: fread: expected %d, got %d\n",
                    wrdsize, status
                );
                perror(path);
            }

            exit(4);
        }
        if (fwrite(buffer, sizeof(char), wrdsize, wfp) != wrdsize) {
            fprintf(stderr, "wdcat: fwrite: ");
            perror(path);
            exit(4);
        }
    }

    fclose(fp);

    out->nsamp  += in->nsamp;
    out->endtime = in->endtime;
}

static int continuous(prev, crnt)
struct wfdisc *prev;
struct wfdisc *crnt;
{
double expected, error;

    if (
        (strcmp(prev->sta,  crnt->sta)  != 0) ||
        (strcmp(prev->chan, crnt->chan) != 0) ||
        (prev->smprate != crnt->smprate)
    ) return 0;

    expected = prev->time + ((double) prev->nsamp / (double) prev->smprate);
    error    = fabs(crnt->time - expected);    /* error in seconds */
    error    = (double) prev->smprate * error; /* error in samples */

    return ((int) error == 0) ? 1 : 0;
}

static int wrtnewrec(ofp, newrec)
FILE *ofp;
struct wfdisc *newrec;
{
    if (wwfdisc(ofp, newrec) != 0) {
        perror("wdcat: wwfdisc");
        exit(4);
    }
}

static int help()
{
    fprintf(stderr,"usage: wdcat dir=name dfile=name ");
    fprintf(stderr,"input_wfdisc output_wfdisc]\n");
    exit(1);
}

int main(int argc, char **argv)
{
long foff;
int i, nrec;
FILE *ifp = stdin;
FILE *ofp = stdout;
FILE *wfp = NULL;
char *iname = NULL;
char *oname = NULL;
char *dir   = NULL;
char *dfile = NULL;
struct wfdisc *wfdisc, *prev, out;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "dir=", strlen("dir=")) == 0) {
            dir = argv[i] + strlen("dir=");
        } else if (strncmp(argv[i], "dfile=", strlen("dfile=")) == 0) {
            dfile = argv[i] + strlen("dfile=");
        } else if (iname == NULL) {
            iname = argv[i];
            if ((ifp = fopen(iname, "rb")) == NULL) {
                perror(iname);
                exit(1);
            }
        } else if (oname == NULL) {
            oname = argv[i];
            if ((ofp = fopen(oname, "wb")) == NULL) {
                perror(oname);
                exit(1);
            }
        } else {
            help();
        }
    }

    if (dir == NULL || dfile == NULL) {
        fprintf(stderr,"wdcat: must specify dir and dfile\n");
        exit(1);
    }

/*  Read entire input wfdisc into memory and sort by station/channel/time  */

    if ((nrec = rwfdisc(ifp, &wfdisc)) <= 0) {
        fprintf(stderr,"wdcat: bad or empty wfdisc file\n");
        exit(2);
    }

    for (i = 0; i < nrec; i++) {

    /*  Make sure that the channels and datatypes are OK  */

        if (strcmp(wfdisc[i].chan, wfdisc_null.chan) == 0) {
            fprintf(stderr,"wdcat: wfdisc contains undefined chan(s)\n");
            exit(3);
        } else if (cssio_wrdsize(wfdisc[i].datatype) < 0) {
            fprintf(stderr,"wdcat: wfdisc contains illegal datatype(s)\n");
            exit(3);
        }

    /*  Make sure we can read the data files  */

        sprintf(path, "%s/%s", wfdisc[i].dir, wfdisc[i].dfile);
        if ((wfp = fopen(path, "rb")) == NULL) {
            fprintf(stderr,"wdcat: fopen: ");
            perror(path);
            exit(3);
        }
        fclose(wfp);
        
    }

    qsort((char *) wfdisc, nrec, sizeof(struct wfdisc), wdcompare);

/*  Open new .w file  */

    if (util_mkpath(dir, 0755) != 0) {
        perror("wdcat: util_mkpath");
        exit(4);
    }

    sprintf(path, "%s/%s", dir, dfile);
    if ((wfp = fopen(path, "wb")) == NULL) {
        fprintf(stderr,"wdcat: ");
        perror(path);
        exit(4);
    }

/*  Copy data to new .w file, creating new wfdisc records as appropriate  */

    out = wfdisc_null;
    for (prev = &out, foff = 0, i = 0; i < nrec; i++) {

        if (!continuous(prev, &wfdisc[i])) {
            if (i > 0) wrtnewrec(ofp, &out);
            out = wfdisc[i];
            strcpy(out.dir,   dir);
            strcpy(out.dfile, dfile);
            out.foff  = ftell(wfp);
            out.nsamp = 0;
        }
        copy(wfp, &out, &wfdisc[i]);
        prev = &wfdisc[i];
    }
    wrtnewrec(ofp, &out);

    exit(0);
}

/* Revision History
 *
 * $Log: wdcat.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
