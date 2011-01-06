#pragma ident "$Id: string.c,v 1.1 2007/09/25 20:52:29 dechavez Exp $"
/*======================================================================
 * 
 * Initialize a detector
 *
 *====================================================================*/
#include "detect.h"

char *detectTriggerString(DETECTOR *engine, char *buf)
{
static char *BadInput = "ERROR: detectTriggerString(engine == NULL)";
static char *NoMatch  = "ERROR: detectTriggerString(unsupported engine type)";
static char MtUnsafeSpace[1000];

    if (buf == NULL) buf = MtUnsafeSpace;

    if (engine == NULL) return BadInput;

    switch (engine->type) {
      case DETECTOR_TYPE_STALTA:
        return detectStaLtaTriggerString(&engine->stalta, buf);
    }

    return NoMatch;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.1  2007/09/25 20:52:29  dechavez
 * initial production release
 *
 */
