#pragma ident "$Id: wdread.c,v 1.2 2003/12/10 06:15:38 dechavez Exp $"
/*======================================================================
 *
 *  Scan the wfdisc at stdin for records which correspond to the 
 *  desired directory and file names and echo them to stdout.
 *
 *====================================================================*/
#include <stdio.h>
#include "cssio.h"
#define REQUIRED_ARGC 3
#define DIR argv[1]
#define FIL argv[2]
#define MAX_CHAR 1024

int main(int argc, char **argv)
{
struct wfdisc  wd;
char *dname, *fname, input[MAX_CHAR];
int count = 0;

    if (argc != REQUIRED_ARGC) {
        fprintf(stderr,"usage: wdread dirname file_name <>\n");
        exit(1);
    }

    dname = DIR;
    fname = FIL;

    while (rwfdrec(stdin, &wd) == 0) {
        if (strcmp(wd.dir,dname) == 0 && strcmp(wd.dfile,fname) == 0) {
            printf("%s", input);
            ++count;
        }
    }

   exit(count);
}

/* Revision History
 *
 * $Log: wdread.c,v $
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
