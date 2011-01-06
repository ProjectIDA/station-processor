#pragma ident "$Id: wdc.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Wfdisc "compiler".
 *
 *====================================================================*/
#include <stdio.h>
#include "util.h"
#include "cssio.h"

int main(int argc, char **argv)
{
int nrec = 0;
FILE *ifp, *ofp;
struct wfdisc wfdisc;
struct list {
    struct wfdisc wfdisc;
    struct list *next;
}head, *crnt, *new;

    head.next = NULL;
    crnt = &head;

    if (argc == 1) {
        ifp = stdin;
        ofp = stdout;
    } else if (argc == 2) {
        if ((ifp = fopen(argv[1], "rb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        ofp = stdout;
    } else if (argc == 3) {
        if ((ifp = fopen(argv[1], "rb")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
        if ((ofp = fopen(argv[2], "w")) == NULL) {
            perror(argv[1]);
            exit(1);
        }
    } else {
        fprintf(stderr,"usage: sacb2a binary_name ascii_name\n");
        exit(1);
    }

    if (SETMODE(fileno(ofp), O_BINARY) == -1) {
        perror("wdc: setmode");
        exit(1);
    }

    while (rwfdrec(ifp, &wfdisc) == 0) {
        if ((new = (struct list *) malloc(sizeof(struct list))) == NULL) {
            perror("wdc: malloc");
            exit(1);
        }
        new->wfdisc = wfdisc;
        new->next   = NULL;
        crnt->next   = new;
        crnt = new;
        ++nrec;
    }

    if (fwrite(&nrec, sizeof(int), 1, ofp) != 1) {
        perror("wdc: fwrite");
        exit(1);
    }

    crnt = head.next;
    while (crnt != NULL) {
        if (fwrite(&crnt->wfdisc, sizeof(struct wfdisc), 1, ofp) != 1) {
            perror("wdc: fwrite");
            exit(1);
        }
        crnt = crnt->next;
    }
}

/* Revision History
 *
 * $Log: wdc.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
