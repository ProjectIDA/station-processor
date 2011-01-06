#pragma ident "$Id: atoi.c,v 1.1 2005/05/26 23:16:24 dechavez Exp $"
#include "oldttyio.h"

int ttyio_atoi(char *string)
{
    if (strcasecmp(string, "TTYIO_FHARD") == 0) {
        return TTYIO_FHARD;
    } else if (strcasecmp(string, "TTYIO_FSOFT") == 0) {
        return TTYIO_FSOFT;
    } else if (strcasecmp(string, "TTYIO_FNONE") == 0) {
        return TTYIO_FNONE;
    } if (strcasecmp(string, "hard") == 0) {
        return TTYIO_FHARD;
    } else if (strcasecmp(string, "soft") == 0) {
        return TTYIO_FSOFT;
    } else if (strcasecmp(string, "none") == 0) {
        return TTYIO_FNONE;
    } else if (strcasecmp(string, "read") == 0) {
        return TTYIO_READ;
    } else if (strcasecmp(string, "write") == 0) {
        return TTYIO_WRITE;
    } else {
        return -1;
    }
}

/* Revision History
 *
 * $Log: atoi.c,v $
 * Revision 1.1  2005/05/26 23:16:24  dechavez
 * moved over from original ttyio
 *
 */
