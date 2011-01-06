#pragma ident "$Id: type.c,v 1.2 2007/01/11 17:48:03 dechavez Exp $"
/*======================================================================
 *
 *  Determine blockette type, and offset to next blockette.
 *
 *  Requires 2-byte shorts.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <assert.h>
#include "seed.h"
#include "util.h"

int seed_type(char *src, long *next_ptr, int swap)
{
UINT8 *ptr;
short type, next;

    assert(sizeof(short) == 2);

    memcpy(&type, src, 2);
    if (swap) util_sswap(&type, 1);

    memcpy(&next, src + 2, 2);
    if (swap) util_sswap(&next, 1);

    *next_ptr = next;

    return type;
}

/* Revision History
 *
 * $Log: type.c,v $
 * Revision 1.2  2007/01/11 17:48:03  dechavez
 * added platform.h and stdtypes.h stuff
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
