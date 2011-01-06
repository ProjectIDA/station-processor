#pragma ident "$Id: process.c,v 1.2 2007/09/25 20:52:29 dechavez Exp $"
/*======================================================================
 * 
 * Run a block of data through the engine
 *
 *====================================================================*/
#include "detect.h"

int detectProcessINT32(DETECTOR *engine, INT32 *data, UINT32 nsamp)
{
    switch (engine->type) {
      case DETECTOR_TYPE_STALTA:
        engine->stalta.debug.enabled = engine->debug;
        engine->state = detectStaLtaINT32(&engine->stalta, data, nsamp);
        break;
    }

    return engine->state;
}

/* Revision History
 *
 * $Log: process.c,v $
 * Revision 1.2  2007/09/25 20:52:29  dechavez
 * initial production release
 *
 */
