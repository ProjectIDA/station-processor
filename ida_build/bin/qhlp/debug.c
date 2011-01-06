#pragma ident "$Id: debug.c,v 1.1 2009/05/14 18:07:43 dechavez Exp $"
/*======================================================================
 *
 *  Manage debug state
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_DEBUG

static void ToggleWatchdogLogging()
{
    if (!Watch->newline) {
        logioStartWatchdog(Watch);
        if (!Watch->newline) {
            LogMsg("watchdog logging started in single line mode");
        } else {
            LogMsg("watchdog logging bumped to multi-line mode");
        }   
    } else {
        logioStopWatchdog(Watch);
        LogMsg("watchdog logging suspended");
    }
}

static void SetGlobDebugFlags(FILE *fp, QHLP_PAR *par)
{
ISI_DEBUG debug = {LOG_DEBUG, LOG_DEBUG, LOG_DEBUG, LOG_DEBUG, LOG_DEBUG};

    isidlLoadDebugFlags(fp, &debug);
    par->input.dl->glob->debug  = debug;
    par->output.dl->glob->debug = debug;

    LogMsg("debug flags reset:  pkt=%s ttag=%s bwd=%s dl=%s lock=%s",
        debug.pkt  == LOG_INFO ? "ON" : "off",
        debug.ttag == LOG_INFO ? "ON" : "off",
        debug.bwd  == LOG_INFO ? "ON" : "off",
        debug.dl   == LOG_INFO ? "ON" : "off",
        debug.lock == LOG_INFO ? "ON" : "off"
    );
}

void UpdateDebugState(QHLP_PAR *par)
{
FILE *fp;

    if (Watch != NULL) ToggleWatchdogLogging();

    fp = fopen(par->dbginfo, "r");
    SetGlobDebugFlags(fp, par);
    if (fp != NULL) fclose(fp);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: debug.c,v $
 * Revision 1.1  2009/05/14 18:07:43  dechavez
 * initial release
 *
 */
