#pragma ident "$Id: debug.c,v 1.6 2007/05/18 18:00:57 dechavez Exp $"
/*======================================================================
 * 
 * QDP library debug message logging
 *
 *====================================================================*/
#include "qdp.h"

void qdpDebug(QDP *qp, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (qp == NULL || format == NULL) return;

    MUTEX_LOCK(&qp->par.mutex);
        if (qp->par.debug >= QDP_DEBUG) {
            ptr = message;
            va_start(marker, format);
            vsprintf(ptr, format, marker);
            va_end(marker);
            logioMsg(qp->lp, LOG_INFO, message);
        }
    MUTEX_UNLOCK(&qp->par.mutex);
}

void qdpInfo(QDP *qp, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (qp == NULL || format == NULL) return;

    MUTEX_LOCK(&qp->par.mutex);
        if (qp->par.debug >= QDP_INFO) {
            ptr = message;
            va_start(marker, format);
            vsprintf(ptr, format, marker);
            va_end(marker);
            logioMsg(qp->lp, LOG_INFO, message);
        }
    MUTEX_UNLOCK(&qp->par.mutex);
}

void qdpWarn(QDP *qp, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (qp == NULL || format == NULL) return;

    MUTEX_LOCK(&qp->par.mutex);
        ptr = message;
        va_start(marker, format);
        vsprintf(ptr, format, marker);
        va_end(marker);
        logioMsg(qp->lp, LOG_WARN, message);
    MUTEX_UNLOCK(&qp->par.mutex);
}

void qdpError(QDP *qp, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (qp == NULL || format == NULL) return;

    MUTEX_LOCK(&qp->par.mutex);
        ptr = message;
        va_start(marker, format);
        vsprintf(ptr, format, marker);
        va_end(marker);
        logioMsg(qp->lp, LOG_ERR, message);
    MUTEX_UNLOCK(&qp->par.mutex);
}

void qdpLcqDebug(QDP_LCQ *lcq, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (lcq == NULL || format == NULL) return;

    if (lcq->par.debug >= QDP_DEBUG) {
        ptr = message;
        va_start(marker, format);
        vsprintf(ptr, format, marker);
        va_end(marker);
        logioMsg(lcq->par.lp, LOG_INFO, message);
    }
}

void qdpLcqInfo(QDP_LCQ *lcq, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (lcq == NULL || format == NULL) return;

    if (lcq->par.debug >= QDP_INFO) {
        ptr = message;
        va_start(marker, format);
        vsprintf(ptr, format, marker);
        va_end(marker);
        logioMsg(lcq->par.lp, LOG_INFO, message);
    }
}

void qdpLcqWarn(QDP_LCQ *lcq, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (lcq == NULL || format == NULL) return;

    ptr = message;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);
    logioMsg(lcq->par.lp, LOG_WARN, message);
}

void qdpLcqError(QDP_LCQ *lcq, char *format, ...)
{
BOOL debug;
va_list marker;
char message[1024], *ptr;

    if (lcq == NULL || format == NULL) return;

    ptr = message;
    va_start(marker, format);
    vsprintf(ptr, format, marker);
    va_end(marker);
    logioMsg(lcq->par.lp, LOG_ERR, message);
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * Revision 1.6  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
