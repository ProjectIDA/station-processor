#pragma ident "$Id: log.c,v 1.7 2008/01/17 19:37:27 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_LOG

static LOGIO *lp = (LOGIO *) NULL;

VOID LogMsgLevel(int level)
{
    logioSetThreshold(lp, level);
}

VOID LogMsg(int level, char *format, ...)
{
va_list marker;
char *ptr, msgbuf[LOGIO_MAX_MSG_LEN];

    ptr = msgbuf;
    va_start(marker, format);
    vsnprintf(ptr, LOGIO_MAX_MSG_LEN, format, marker);
    va_end(marker);

    logioMsg(lp, level, msgbuf);
}

BOOL InitLogging(char *myname, PARAM *par)
{
    if (!logioInit(&par->lp, par->log, NULL, myname)) {
        return FALSE;
    } else {
        lp = &par->lp;
    }

    if (par->debug) logioSetThreshold(lp, LOG_DEBUG);
    logioMsg(lp, LOG_INFO, "IDA System Interface Daemon - %s", VersionIdentString);
    logioMsg(lp, LOG_INFO, "Build %s %s", __DATE__, __TIME__);

    return TRUE;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.7  2008/01/17 19:37:27  dechavez
 * reformatted Build line
 *
 * Revision 1.6  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.5  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.4  2006/02/09 20:20:49  dechavez
 * updated library list
 *
 * Revision 1.3  2005/09/30 22:36:21  dechavez
 * changed format of build date/time message
 *
 * Revision 1.2  2005/09/10 21:55:23  dechavez
 * include build date and time in version string
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
