#pragma ident "$Id: ldump.c,v 1.1.1.1 2000/02/08 20:20:41 dec Exp $"
/*======================================================================
 *
 *  log buffer dumps
 *
 *====================================================================*/
#include <ctype.h>
#include "util.h"

#define printchar(c) ((isascii(c) && isprint(c)) ? c : '.')

int util_hexlog(int level, unsigned char *ptr, long count, long offset, char off_base, char *buf)
{
int whole_lines;
int num_last;
int line, byte, base;
static char hex_format[] = "%8x:";
static char heX_format[] = "%8X:";
static char dec_format[] = "%10d:";
static char oct_format[] = "%12o:";
char *format;
int bytes_per_line = 16;

    switch (off_base) {
        case 'x':           format = hex_format; break;
        case 'X':           format = heX_format; break;
        case 'd': case 'D': format = dec_format; break;
        case 'o': case 'O': format = oct_format; break;
        default:  return -1;
    }

    num_last    = count % bytes_per_line;
    whole_lines = (count - num_last) / bytes_per_line;
    for (line = 0; line < whole_lines; line++) {
        buf[0] = 0;
        sprintf(buf+strlen(buf), format, offset);
        base = line*bytes_per_line;
        for (byte = 0; byte < bytes_per_line; byte++) {
            sprintf(buf+strlen(buf)," %02x",ptr[byte+base]);
        }
        sprintf(buf+strlen(buf), " | ");
        for (byte = 0; byte < bytes_per_line; byte++) {
            sprintf(buf+strlen(buf),"%c",printchar(ptr[byte+base]));
        }
        offset += bytes_per_line;
        util_log(level, buf);
    }
    if (num_last != 0) {
        buf[0] = 0;
        printf(format, offset);
        base = line*bytes_per_line;
        for (byte = 0; byte < num_last; byte++) {
            sprintf(buf+strlen(buf)," %02x",ptr[byte+base]);
        }
        for (byte = num_last; byte < bytes_per_line; byte++) {
            sprintf(buf+strlen(buf),"   ");
        }
        sprintf(buf+strlen(buf)," | ");
        for (byte = 0; byte < num_last; byte++) {
            sprintf(buf+strlen(buf),"%c",printchar(ptr[byte+base]));
        }
        util_log(level, buf);
    }
    return 0;
}

/* Revision History
 *
 * $Log: ldump.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:41  dec
 * import existing IDA/NRTS sources
 *
 */
