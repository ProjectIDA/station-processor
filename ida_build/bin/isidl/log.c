#pragma ident "$Id: log.c,v 1.6 2006/03/30 22:03:26 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_LOG

static LOGIO lp;

LOGIO *GetLogHandle()
{
    return &lp;
}

VOID LogMsgLevel(int level)
{
    logioSetThreshold(&lp, level);
}

VOID LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);

    logioMsg(&lp, level, msgbuf);
}

LOGIO *InitLogging(char *myname, char *spec, char *prefix, BOOL debug)
{
    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    logioSetPrefix(&lp, prefix);

    if (debug) logioSetThreshold(&lp, LOG_DEBUG);
    LogMsg(LOG_INFO, "ISI Disk Loop Writer - %s (%s %s)", VersionIdentString, __DATE__, __TIME__);
    LogMsg(LOG_DEBUG, "%s", msgqVersionString());
    LogMsg(LOG_DEBUG, "%s", iacpVersionString());
    LogMsg(LOG_DEBUG, "%s", nrtsVersionString());
    LogMsg(LOG_DEBUG, "%s", logioVersionString());
    LogMsg(LOG_DEBUG, "%s", ttyioVersionString());
    LogMsg(LOG_DEBUG, "%s", isiVersionString());
    LogMsg(LOG_DEBUG, "%s", utilVersionString());
    /* LogMsg(LOG_INFO, "%s", Copyright); */

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.6  2006/03/30 22:03:26  dechavez
 * added GetLogHandle()
 *
 * Revision 1.5  2005/09/10 22:05:55  dechavez
 * include date and time in version string output
 *
 * Revision 1.4  2005/07/26 18:37:15  dechavez
 * prefix log messages with site or source tag
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
