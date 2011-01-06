#pragma ident "$Id: doarceof.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*=====================================================================
 *
 *  Record is a DCC applied end of field tape mark.
 *
 *===================================================================*/
#include "idalst.h"

void doarceof(char *buffer, struct counters *count, int print)
{
static long tagno = ARCEOF_TAG;

/*  Flush time code quality tracking  */

    cktqual(NULL, count, print);

/*  Flag the presence of the tape marker  */

    if (!(print & P_DCCREC)) return;

    tag(tagno++);
    printf("End-of-field-tape mark read at record %7ld.\n", count->rec);
}      

/* Revision History
 *
 * $Log: doarceof.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
