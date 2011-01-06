#pragma ident "$Id: string.c,v 1.2 2006/05/17 23:23:10 dechavez Exp $"
/*======================================================================
 *
 *  String helper functions
 *
 *====================================================================*/
#include "ttyio.h"

typedef struct {
    int value;
    char *name;
} VALUE_MAP;

static VALUE_MAP FlowMap[] = {
    { TTYIO_FLOW_NONE, "none" },
    { TTYIO_FLOW_HARD, "hard" },
    { TTYIO_FLOW_SOFT, "soft" },
    { TTYIO_FLOW_BAD,   NULL  }
};

static VALUE_MAP ParityMap[] = {
    { TTYIO_PARITY_NONE,  "none" },
    { TTYIO_PARITY_ODD,   "odd" },
    { TTYIO_PARITY_EVEN,  "even" },
    { TTYIO_PARITY_MARK,  "mark" },
    { TTYIO_PARITY_SPACE, "space" },
    { TTYIO_PARITY_BAD,    NULL  }
};

static int LookupByName(char *target, VALUE_MAP *map, int notfound)
{
VALUE_MAP *ptr;

    for (ptr = map; ptr->name != NULL; ++ptr) {
        if (strcasecmp(ptr->name, target) == 0) return ptr->value;
    }

    return notfound;
}

static char *LookupByValue(int target, VALUE_MAP *map, char *notfound)
{
VALUE_MAP *ptr;

    for (ptr = map; ptr->name != NULL; ++ptr) {
        if (ptr->value == target) return ptr->name;
    }

    return notfound;
}

int ttyioStringToFlow(char *target)
{
static int notfound = TTYIO_FLOW_BAD;

    return LookupByName(target, FlowMap, notfound);
}

int ttyioStringToParity(char *target)
{
static int notfound = TTYIO_PARITY_BAD;

    return LookupByName(target, ParityMap, notfound);
}

char *ttyioFlowToString(int target)
{
static char *notfound = "<bad value>";

    return LookupByValue(target, FlowMap, notfound);
}

char *ttyioParityToString(int target)
{
static char *notfound = "<bad value>";

    return LookupByValue(target, ParityMap, notfound);
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
 * $Log: string.c,v $
 * Revision 1.2  2006/05/17 23:23:10  dechavez
 * added copyright notice
 *
 * Revision 1.1  2005/06/10 15:27:14  dechavez
 * initial release
 *
 */
