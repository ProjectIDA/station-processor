#pragma ident "$Id: main.c,v 1.7 2006/02/09 20:14:39 dechavez Exp $"
/*======================================================================
 *
 *  Program to demultiplex and decompress IDA format data into CSS 3.0
 *  format disk files.
 *
 *====================================================================*/
#include "idadmx.h"
#include "util.h"

/*  Global variables  */

char *Buffer;
IDA *ida;

int main(int argc, char **argv)
{
MIO *fp;
FILE *hp = NULL;
int verbose, echo, shift, fixyear_flag;
unsigned long swab;
struct counter count;
IDA_DREC drec;
static char buffer[IDA_BUFSIZ*2];

    fprintf(stderr,"%s %s\n", argv[0], VersionIdentString);

    Buffer = buffer;
    init(argc, argv, &count, &verbose, &echo, &fp, &shift, &hp, &swab, &fixyear_flag);
    utilInitOutputStreamBuffers();
    while (rdrec(fp, hp, &drec, &count)) wrtdat(&drec, &count);
    utilDestroyOutputStreamBuffers();
    die(NORMAL);

}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.7  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.6  2005/10/11 22:49:03  dechavez
 * switch to new VersionIdentString global
 *
 * Revision 1.5  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.4  2003/10/16 18:16:58  dechavez
 * ansi function declaration
 *
 * Revision 1.3  2000/02/18 06:39:18  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.2  2000/02/18 00:55:29  dec
 * Added release notes, changed version to 2.10.1
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
