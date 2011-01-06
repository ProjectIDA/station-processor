#pragma ident "$Id: log.c,v 1.1 2006/02/10 03:04:46 dechavez Exp $"
/*======================================================================
 *
 *  Logging facility
 *
 *====================================================================*/
#include "logio.h"
#include "wrtdl.h"

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

LOGIO *InitLogging(char *myname, char *spec, BOOL debug)
{
    if (!logioInit(&lp, spec, NULL, myname)) return NULL;
    if (debug) logioSetThreshold(&lp, LOG_DEBUG);

    return &lp;
}

/* Revision History
 *
 * $Log: log.c,v $
 * Revision 1.1  2006/02/10 03:04:46  dechavez
 * initial release
 *
 */
