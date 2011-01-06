#pragma ident "$Id: watchdog.c,v 1.3 2009/01/26 21:13:00 dechavez Exp $"
/*======================================================================
 *
 * Watchdog logging.
 *
 *====================================================================*/
#include "logio.h"
#include "util.h"

/* Start watchdog logging.  This is meant to be called from a signal
 * handler.  The first time it is called, it starts logging one line
 * on top of itself (ie, the file never grows).  If it gets called a
 * second time then newlines are added.
 */

void logioStartWatchdog(LOGIO_WATCHDOG *watch)
{
    if (watch == NULL) return;
    if (!watch->active) {
        watch->active = TRUE;
        watch->newline = FALSE;
        logioUpdateWatchdog(watch, "single-line logging started");
    } else if (!watch->newline) {
        watch->newline = TRUE;
        logioUpdateWatchdog(watch, "multi-line logging started");
    }
}

void logioStopWatchdog(LOGIO_WATCHDOG *watch)
{
    if (watch == NULL) return;
    logioUpdateWatchdog(watch, "logging suspended");
    watch->active = watch->newline = FALSE;
}

void *logioUpdateWatchdog(LOGIO_WATCHDOG *watch, char *message)
{
FILE *fp;
char tsbuf[LOGIO_MAX_TSLEN+1];
char path[MAXPATHLEN+1];

    if (watch == NULL) return;
    if (!watch->active) return;

    if (!utilDirectoryExists(watch->dir)) util_mkpath(watch->dir, 0755);
    if (!utilDirectoryExists(watch->dir)) return;

    sprintf(path, "%s/%lu", watch->dir, THREAD_SELF());
    if ((fp = fopen(path, watch->newline ? "a" : "w")) == NULL) return NULL;

    fprintf(fp, "%s", logioTstamp(tsbuf));
    if (message != NULL) fprintf(fp, " %s", message);
    fprintf(fp, "\n");
    fclose(fp);

    return;
}

LOGIO_WATCHDOG *logioInitWatchdog(char *dir, char *name)
{
LOGIO_WATCHDOG *watch;

    if (dir == NULL || name == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if ((watch = (LOGIO_WATCHDOG *) malloc(sizeof(LOGIO_WATCHDOG))) == NULL) return NULL;

    sprintf(watch->dir, "%s/%s.%lu", dir, name, getpid());

    return watch;
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
 * $Log: watchdog.c,v $
 * Revision 1.3  2009/01/26 21:13:00  dechavez
 * only create directory upon first use
 *
 * Revision 1.2  2009/01/26 21:09:15  dechavez
 * added logioStartWatchdog(), logioStopWatchdog(), and changed logioUpdateWatchdog()
 * to use LOGIO_WATCHDOG handle.  Changed logioInitWatchdogDir() to logioInitWatchdog()
 * which returns the handle.
 *
 * Revision 1.1  2009/01/05 17:36:43  dechavez
 * initial release
 *
 */
