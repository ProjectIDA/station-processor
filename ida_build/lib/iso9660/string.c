#pragma ident "$Id: string.c,v 1.2 2008/04/30 19:52:40 dechavez Exp $"
/*======================================================================
 *
 * Stirng conversions
 *
 *====================================================================*/
#include "iso9660.h"

typedef struct {
    char *text;
    int code;
} TEXT_MAP;

static TEXT_MAP ErrorMap[] = {
    {"no error",              ISO9660_OK}, 
    {"image full",            ISO9660_IMAGE_FULL},
    {"directory full",        ISO9660_DIRECTORY_FULL}, 
    {"read error",            ISO9660_IMAGE_READ_ERROR},
    {"write error",           ISO9660_IMAGE_WRITE_ERROR}, 
    {"open error",            ISO9660_IMAGE_OPEN_ERROR},
    {"bad signature",         ISO9660_IMAGE_BAD_SIGNATURE}, 
    {"corrupted",             ISO9660_IMAGE_CORRUPTED},
    {"input file open error", ISO9660_INPUT_FILE_OPEN_ERROR}, 
    {"input file read error", ISO9660_INPUT_FILE_READ_ERROR},
    {"no memory",             ISO9660_MEMORY_ALLOC_ERROR}, 
    {NULL, 0}
};

static char *LocateString(int code, TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

char *iso9660ErrorString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ErrorMap, DefaultMessage);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
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
 * $Log: string.c,v $
 * Revision 1.2  2008/04/30 19:52:40  dechavez
 * added missing end quote on pragma ident
 *
 * Revision 1.1  2008/03/10 20:52:08  dechavez
 * initial release
 *
 */
