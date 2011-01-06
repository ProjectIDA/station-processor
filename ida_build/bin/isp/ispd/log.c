#pragma ident "$Id: log.c,v 1.4 2006/06/02 21:32:09 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "ispd.h"

#define MY_MOD_ID ISPD_MOD_LOG

static LOGIO lp;

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
    LogMsg(LOG_INFO, "%s", isp_release);
    LogMsg(LOG_DEBUG, "%s", cssioVersionString());
    LogMsg(LOG_DEBUG, "%s", dbioVersionString());
    LogMsg(LOG_DEBUG, "%s", iacpVersionString());
    LogMsg(LOG_DEBUG, "%s", ida10VersionString());
    LogMsg(LOG_DEBUG, "%s", isiVersionString());
    LogMsg(LOG_DEBUG, "%s", isidlVersionString());
    LogMsg(LOG_DEBUG, "%s", logioVersionString());
    LogMsg(LOG_DEBUG, "%s", nrtsVersionString());
    LogMsg(LOG_DEBUG, "%s", sanioVersionString());
    LogMsg(LOG_DEBUG, "%s", utilVersionString());

    utilHookOldLog(&lp);

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.4  2006/06/02 21:32:09  dechavez
 * add msgqVersionString
 *
 * Revision 1.3  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.2  2005/07/26 18:38:11  dechavez
 * prefix logio log messages with site tag
 *
 * Revision 1.1  2005/07/06 15:42:14  dechavez
 * initial release
 *
 */
