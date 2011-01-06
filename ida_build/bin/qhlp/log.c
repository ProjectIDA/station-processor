#pragma ident "$Id: log.c,v 1.2 2007/09/14 19:52:23 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_LOG

static LOGIO lp;

void DbgMsg(char *file, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];
FILE *fp;

    if (file == NULL) {
        fp = stdout;
    } else {
        if ((fp = fopen(file, "a")) == NULL) {
            fprintf(stderr, "DbgMsg: fopen: %s: %s\n", file, strerror(errno));
            return;
        }
    }

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    fprintf(fp, "%s", msgbuf);
    fclose(fp);
}

void LogMsg(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_INFO, msgbuf);
}

void LogDebug(char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, LOG_DEBUG, msgbuf);
}

LOGIO *GetLogHandle()
{
    return &lp;
}

void LogMsgLevel(int level)
{
    logioSetThreshold(&lp, level);
}

LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug)
{
char *PREFIX;

    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    if ((PREFIX = strdup(prefix)) == NULL) return NULL;
    util_ucase(PREFIX);
    logioSetPrefix(&lp, PREFIX);
    free(PREFIX);

    if (debug) logioSetThreshold(&lp, LOG_DEBUG);
    LogMsg("QDP to HLP reformatter - %s (%s %s)", VersionIdentString, __DATE__, __TIME__);

    return &lp;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: log.c,v $
 * Revision 1.2  2007/09/14 19:52:23  dechavez
 * set prefix to be upper case site name
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
