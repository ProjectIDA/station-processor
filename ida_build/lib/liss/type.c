#pragma ident "$Id: type.c,v 1.1 2005/09/30 18:08:34 dechavez Exp $"
/*======================================================================
 *
 *  Determine blockette type, and offset to next blockette.
 *
 *====================================================================*/
#include "util.h"
#include "liss.h"

INT16 lissTypeAndOffset(UINT8 *src, INT32 *next_ptr, BOOL swap)
{
INT16 type, next;

    memcpy(&type, src, 2);
    if (swap) utilSwapINT16(&type, 1);

    memcpy(&next, src + 2, 2);
    if (swap) utilSwapINT16(&next, 1);

    *next_ptr = next;

    return type;
}

/* Revision History
 *
 * $Log: type.c,v $
 * Revision 1.1  2005/09/30 18:08:34  dechavez
 * initial release
 *
 */
