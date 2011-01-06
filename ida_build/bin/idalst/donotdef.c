#pragma ident "$Id: donotdef.c,v 1.5 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Current record is not recognized...
 *
 *====================================================================*/
#include "idalst.h"

#define BYTES_TO_DUMP  64
#define BYTES_PER_LINE 16
#define PRINTCHAR(c) ((isascii(c) && isprint(c)) ? c : '.')

void donotdef(char *buffer, struct counters *count, int print)
{
static long tagno = UREC_TAG;
int offset, whole_lines, num_last, line, byte, base;

    if (tagno == UREC_TAG) {
        tag(tagno++); printf("\n"); tag(tagno++);
        printf( "UNRECOGNIZED RECORD SUMMARY\n");
    }

    tag(tagno++);
    printf( "Record # %d:\n", count->rec);

    num_last    = BYTES_TO_DUMP % BYTES_PER_LINE;
    whole_lines = (BYTES_TO_DUMP - num_last) / BYTES_PER_LINE;
    for (line = 0, offset = 0; line < whole_lines; line++) {
        tag(tagno++);
        printf( "%8x:", offset);
        base = line*BYTES_PER_LINE;
        for (byte = 0; byte < BYTES_PER_LINE; byte++) {
            printf( " %02x", (UINT8) buffer[byte+base]);
        }
        printf( " | ");
        for (byte = 0; byte < BYTES_PER_LINE; byte++) {
            printf( "%c",PRINTCHAR(buffer[byte+base]));
        }
        offset += BYTES_PER_LINE;
        printf( "\n");
    }
    if (num_last != 0) {
        tag(tagno++);
        printf( "%8x:", offset);
        base = line*BYTES_PER_LINE;
        for (byte = 0; byte < num_last; byte++) {
            printf( " %02x",buffer[byte+base]);
        }
        for (byte = num_last; byte < BYTES_PER_LINE; byte++) {
            printf( "   ");
        }
        printf( " | ");
        for (byte = 0; byte < num_last; byte++) {
            printf( "%c",PRINTCHAR(buffer[byte+base]));
        }
        printf( "\n");
    }
}

/* Revision History
 *
 * $Log: donotdef.c,v $
 * Revision 1.5  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.4  2003/12/10 06:31:20  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2003/06/11 20:26:20  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.2  2002/03/28 02:51:21  dec
 * treat buffer as unsigned char to avoid sign extension when dumping
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
