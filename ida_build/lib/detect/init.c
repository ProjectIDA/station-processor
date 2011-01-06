#pragma ident "$Id: init.c,v 1.1 2007/09/26 16:41:36 dechavez Exp $"
/*======================================================================
 * 
 * Initialize a detector
 *
 *====================================================================*/
#include "detect.h"

BOOL detectInit(DETECTOR *engine)
{
BOOL result;

    if (engine == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    switch (engine->type) {
      case DETECTOR_TYPE_STALTA:
        result = detectInitStaLta(&engine->stalta);
        break;

      default:
        errno = EINVAL;
        result = FALSE;
    }

    return result;
}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.1  2007/09/26 16:41:36  dechavez
 * created
 *
 */
