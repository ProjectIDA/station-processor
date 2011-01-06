#pragma ident "$Id: wddel.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Delete ALL files pointed to by the wfdisc read from stdin.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"
#include "util.h"

#ifdef unix
#include <sys/param.h>
#else
#define MAXPATHLEN 64
#endif

struct group {
    int nrec;
    struct wfdisc *wfdisc;
};

static int wfdisc_cmp(const void *ptra, const void *ptrb)
{
struct wfdisc *a, *b;
int dirtst;

    a = (struct wfdisc *) ptra;
    b = (struct wfdisc *) ptrb;

    return ((dirtst = strcmp(a->dir,b->dir)) == 0) ? 
        strcmp(a->dfile,b->dfile) : dirtst;
}

static int gather(wfdisc, nrecs, grp)
struct wfdisc wfdisc[];
int nrecs;
struct group **grp;
{
int count, i, j, k, l, doit;
char name1[MAXPATHLEN], name2[MAXPATHLEN];
struct group tmp, *out;

/*  Scan thru list first time to get number of unique names  */

    count = 0; i = 0; j = 1;
    sprintf(name1, "%s/%s", wfdisc[i].dir, wfdisc[i].dfile);
    do {
        if (j < nrecs) {
            sprintf(name2, "%s/%s", wfdisc[j].dir, wfdisc[j].dfile);
            doit = (strcmp(name1, name2) != 0);
        } else {
            doit = TRUE;
        }
        if (doit) {
            ++count;
            strcpy(name1, name2);
            i = j++; 
        } else {
            ++j;
        }
    } while (j <= nrecs);

/*  Allocate memory to hold all the groups  */

    if ((out = (struct group *) malloc(count*sizeof(struct group))) == NULL) {
        perror("wddel: malloc");
        exit(8);
    }

/*  Now repeat test, assigning groups  */

    count = 0; i = 0; j = 1;
    sprintf(name1, "%s/%s", wfdisc[i].dir, wfdisc[i].dfile);
    do {
        if (j < nrecs) {
            sprintf(name2, "%s/%s", wfdisc[j].dir, wfdisc[j].dfile);
            doit = (strcmp(name1, name2) != 0);
        } else {
            doit = TRUE;
        }
        if (doit) {
            tmp.nrec = j - i;
            if ((tmp.wfdisc = (struct wfdisc *) 
                malloc(tmp.nrec*sizeof(struct wfdisc))) == NULL) {
                perror("wddel: malloc");
                exit(9);
            }
            for (k=0, l=i; k < tmp.nrec; k++, l++) {
                tmp.wfdisc[k] = wfdisc[l];
            }
            out[count++] = tmp;
            strcpy(name1, name2);
            i = j++; 
        } else {
            ++j;
        }
    } while (j <= nrecs);

/*  Return total number of groups  */

    *grp = out;
    return count;

}

static int help()
{
    fprintf(stderr,"usage: wddel [+/-v] < wfdisc\n");
    exit(1);
}

int main(int argc, char **argv)
{
char path[MAXPATHLEN];
int  i, j, nrecs, nfiles, verbose;
struct wfdisc *in_rec;
struct group  *grp;

/*  Set up defaults  */

    verbose = FALSE;

/*  Scan command line for overrides  */

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "+v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = FALSE;
        } else {
            fprintf(stderr,"wddel: unrecognized argument '%s'\n",argv[i]);
            help();
        }
    }

/*  Read in entire wfdisc, and sort by path name  */

    if ((nrecs = rwfdisc(stdin, &in_rec)) <= 0) {
        fprintf(stderr,"wddel: bad or empty wfdisc file\n");
        exit(2);
    }
    if (nrecs > 1) qsort(in_rec, nrecs, sizeof(struct wfdisc), wfdisc_cmp);

/*  Gather wfdisc records with common path name  */

    nfiles = gather(in_rec, nrecs, &grp);

/*  Delete each file  */

    for (i = 0; i < nfiles; i++) {
        sprintf(path, "%s/%s", grp[i].wfdisc[0].dir, grp[i].wfdisc[0].dfile);
        if (verbose) fprintf(stderr, "%s\n", path);
        if (unlink(path) != 0) {
            fprintf(stderr, "wddel: ");
            perror(path);
        }
    }

    exit(0);
}

/* Revision History
 *
 * $Log: wddel.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
