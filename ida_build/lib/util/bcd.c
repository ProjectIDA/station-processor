#pragma ident "$Id: bcd.c,v 1.2 2003/12/04 23:29:06 dechavez Exp $"
/*======================================================================
 *
 * Convert from BCD's to ints and back.  bcd2int code was taken from
 * Bruce Crawford (UNR Seismology).
 *
 *====================================================================*/
#include <stdlib.h>
#include "util.h"

int util_bcd2int( unsigned char *input, int numDigits, int nibble)
{
int i, decimal, value;

    value = 0;

    for (value = 0, i = 0; i < numDigits; i++) {
        if (nibble) {
            decimal = *input & 0xF;
        } else {
            decimal = ((int) (*input & 0xF0)) >> 4;
        }

        value = value * 10 + decimal;

        if (nibble) ++input;
        nibble = !nibble;
    }

    return value;
}

int util_int2bcd(unsigned char *output, int len, int value, int numDigits)
{
char format[10], buf[64];
int i, j, hiorder, nibble;

    sprintf(format, "%%0%dd", numDigits);
    sprintf(buf, format, value);
    for (hiorder = 1, i = 0, j = 0; i < strlen(buf); i++) {
        nibble = (int) buf[i] - 0x30;
        if (hiorder) {
            if (j == len) return -1; /* no more room! */
            output[j] = nibble << 4;
        } else {
            output[j] |= nibble;
            ++j;
        }
        hiorder = !hiorder;
    }

    return numDigits;
}

#ifdef DEBUG_TEST

#define MAXLEN 5

main(int argc, char **argv)
{
int i, j, value, len, retval, nd, numDigits;
unsigned char bcdbuf[MAXLEN];

    if ((argc-1) % 2) {
        fprintf(stderr, "usage: %s value nd [value nd ...]\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i+=2) {
        value = atoi(argv[i]);
        nd    = atoi(argv[i+1]);
        if ((numDigits = util_int2bcd(bcdbuf, MAXLEN, value, nd)) < 0) {
            printf("util_int2bcd FAILED on `%d'\n", value);
        } else {
            printf("%d =>", value, numDigits/2);
            for (j = 0; j < numDigits/2; j++) printf(" %02x", bcdbuf[j]);
            if (numDigits % 2) printf(" %x", (bcdbuf[j] >> 4) & 0x0F);
            printf(" => %d\n", util_bcd2int(bcdbuf, numDigits, 0));
        }
    }
}
#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: bcd.c,v $
 * Revision 1.2  2003/12/04 23:29:06  dechavez
 * removed tabs
 *
 * Revision 1.1.1.1  2000/02/08 20:20:40  dec
 * import existing IDA/NRTS sources
 *
 */
