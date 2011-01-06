#pragma ident "$Id: selrec.c,v 1.3 2006/02/10 01:46:10 dechavez Exp $"
/*======================================================================
 *
 *  Cull the selected records.
 *
 *====================================================================*/
#include "ida/limits.h"
#include "util.h"

int longcmp();

int main(int argc, char **argv)
{
int i, nrec;
long *list, count;
char buffer[IDA_BUFSIZ];

    nrec = argc - 1;
    if (nrec < 1) exit(1);

    if ((list = (long *) malloc (nrec*sizeof(long))) == NULL) {
        perror("malloc");
        exit(1);
    }

    if (SETMODE(fileno(stdin), O_BINARY) == -1) {
        perror("setmode");
        exit(1);
    }

    if (SETMODE(fileno(stdout), O_BINARY) == -1) {
        perror("setmode");
        exit(1);
    }

    for (i = 1; i < argc; i++) list[i-1] = atol(argv[i]);

    qsort((void *) list, (size_t) nrec, sizeof(long), longcmp);

    count = 0;
    while (count < list[nrec-1]) {
        if (fread(buffer, IDA_BUFSIZ, (size_t) 1, stdin) != 1) {
            perror("fread");
            exit(1);
        }
        ++count;
        if (
            bsearch((void *) &count, (void *) list, (size_t) nrec, 
                     sizeof(long), longcmp) != NULL
        ) {
            if (fwrite(buffer, IDA_BUFSIZ, (size_t) 1, stdout) != 1) {
                perror("stdout");
                exit(1);
            }
        }
    }
}

int longcmp(a, b)
long *a, *b;
{
    return *a - *b;
}

/* Revision History
 *
 * $Log: selrec.c,v $
 * Revision 1.3  2006/02/10 01:46:10  dechavez
 * removed uneeded includes
 *
 * Revision 1.2  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
