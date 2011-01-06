#pragma ident "$Id: binprint.c,v 1.3 2008/10/02 22:55:55 dechavez Exp $"
/*======================================================================
 *
 * print binary
 *
 *====================================================================*/
#include "util.h"

void utilPrintBinUINT8(FILE *fp, UINT8 value)
{
int i, j;

    for (i=0,j=7; i < 8; i++,j--) {
        fprintf(fp, "%d", (value & (1 << j)) == 0 ? 0 : 1);
    }
}

void utilPrintBinUINT16(FILE *fp, UINT16 value)
{
int i;
UINT8 *ptr;

#ifdef LTL_ENDIAN_HOST
    for (ptr = (UINT8 *) &value, i = 1; i >= 0; i--) {
        utilPrintBinUINT8(fp, ptr[i]);
        if (i > 0) fprintf(fp, " ");
    }
#else
    for (ptr = (UINT8 *) &value, i = 0; i < 2; i++) {
        utilPrintBinUINT8(fp, ptr[i]);
        if (i < 1) fprintf(fp, " ");
    }
#endif
}

void utilPrintBinUINT32(FILE *fp, UINT32 value)
{
int i;
UINT8 *ptr;

#ifdef LTL_ENDIAN_HOST
    for (ptr = (UINT8 *) &value, i = 3; i >= 0; i--) {
        utilPrintBinUINT8(fp, ptr[i]);
        if (i > 0) fprintf(fp, " ");
    }
#else
    for (ptr = (UINT8 *) &value, i = 0; i < 4; i++) {
        utilPrintBinUINT8(fp, ptr[i]);
        if (i < 3) fprintf(fp, " ");
    }
#endif
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: binprint.c,v $
 * Revision 1.3  2008/10/02 22:55:55  dechavez
 * added utilPrintBinUINT16
 *
 * Revision 1.2  2008/01/07 20:33:06  dechavez
 * little-endian fix for utilPrintBinUINT32()
 *
 * Revision 1.1  2006/12/06 23:51:24  dechavez
 * initial release
 *
 */
