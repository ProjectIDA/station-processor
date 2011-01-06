#pragma ident "$Id: stats.c,v 1.8 2007/10/31 17:12:57 dechavez Exp $"
/*======================================================================
 * 
 * Manipulate the statistics structure
 *
 *====================================================================*/
#include "qdp.h"

#define BUFLEN 64

void InitCounter(QDP_COUNTER *counter)
{
    counter->overrun = 0;
    counter->crc = 0;
    counter->data = 0;
    counter->fill = 0;
    counter->drop = 0;
    counter->retry = 0;
}

void qdpInitStats(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_INIT(&qp->stats.mutex);

    qp->stats.tstamp = 0;
    qp->stats.nreg = 0;
    InitCounter(&qp->stats.counter.session);
    InitCounter(&qp->stats.counter.total);
}

void qdpSetStatsTstamp(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        qp->stats.tstamp = utilTimeStamp();
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpResetSessionStats(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        qp->stats.tstamp = 0;
        InitCounter(&qp->stats.counter.session);
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsNreg(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.nreg;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsOverrun(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.overrun;
        ++qp->stats.counter.total.overrun;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsCrc(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.crc;
        ++qp->stats.counter.total.crc;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsDrop(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.drop;
        ++qp->stats.counter.total.drop;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsDupData(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.dupdata;
        ++qp->stats.counter.total.dupdata;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsData(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.data;
        ++qp->stats.counter.total.data;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsFill(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.fill;
        ++qp->stats.counter.total.fill;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpIncrStatsRetry(QDP *qp)
{
    if (qp == NULL) return;

    MUTEX_LOCK(&qp->stats.mutex);
        ++qp->stats.counter.session.retry;
        ++qp->stats.counter.total.retry;
    MUTEX_UNLOCK(&qp->stats.mutex);
}

void qdpGetStats(QDP *qp, QDP_STATS *dest)
{
    if (qp == NULL || dest == NULL) return;
    MUTEX_LOCK(&qp->stats.mutex);
        *dest = qp->stats;
    MUTEX_UNLOCK(&qp->stats.mutex);
    if (dest->tstamp) {
        dest->uptime = utilTimeStamp() - dest->tstamp;
    } else {
        dest->uptime = 0;
    }
}

UINT64 qdpSessionUptime(QDP *qp)
{
UINT64 result;

    if (qp == NULL) return 0;
    MUTEX_LOCK(&qp->stats.mutex);
        result = utilTimeStamp() - qp->stats.tstamp;
    MUTEX_UNLOCK(&qp->stats.mutex);
    return result;
}

void qdpLogStats(QDP *qp)
{
QDP_STATS stats;
char tmp[BUFLEN];

    if (qp == NULL) return;

    qdpGetStats(qp, &stats);

    qdpInfo(qp, "  serialno = %s", qp->peer.ident);
    if (stats.uptime != 0) {
        qdpInfo(qp, "    uptime = %s", utilTimeString(stats.uptime, 108, tmp, BUFLEN));
    } else {
        qdpInfo(qp, "    uptime = n/a (not currently registered)");
    }
    qdpInfo(qp, "      nreg = %lu", stats.nreg);
    qdpInfo(qp, "   overrun = %lu / %lu", stats.counter.session.overrun, stats.counter.total.overrun);
    qdpInfo(qp, "crc errors = %lu / %lu", stats.counter.session.crc, stats.counter.total.crc);
    qdpInfo(qp, "      data = %lu / %lu", stats.counter.session.data, stats.counter.total.data);
    qdpInfo(qp, "  dup data = %lu / %lu", stats.counter.session.dupdata, stats.counter.total.dupdata);
    qdpInfo(qp, "      fill = %lu / %lu", stats.counter.session.fill, stats.counter.total.fill);
    qdpInfo(qp, "     retry = %lu / %lu", stats.counter.session.retry, stats.counter.total.retry);
}

void qdpPrintStats(FILE *fp, QDP *qp)
{
QDP_STATS stats;
char tmp[BUFLEN];

    if (qp == NULL) return;

    qdpGetStats(qp, &stats);

    fprintf(fp, "  serialno = %s\n", qp->peer.ident);
    if (stats.uptime != 0) {
        fprintf(fp, "    uptime = %s\n", utilTimeString(stats.uptime, 108, tmp, BUFLEN));
    } else {
        fprintf(fp, "    uptime = n/a (not currently registered)\n");
    }
    fprintf(fp, "      nreg = %lu\n", stats.nreg);
    fprintf(fp, "   overrun = %lu / %lu\n", stats.counter.session.overrun, stats.counter.total.overrun);
    fprintf(fp, "crc errors = %lu / %lu\n", stats.counter.session.crc, stats.counter.total.crc);
    fprintf(fp, "      data = %lu / %lu\n", stats.counter.session.data, stats.counter.total.data);
    fprintf(fp, "  dup data = %lu / %lu\n", stats.counter.session.dupdata, stats.counter.total.dupdata);
    fprintf(fp, "      fill = %lu / %lu\n", stats.counter.session.fill, stats.counter.total.fill);
    fprintf(fp, "     retry = %lu / %lu\n", stats.counter.session.retry, stats.counter.total.retry);
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
 * $Log: stats.c,v $
 * Revision 1.8  2007/10/31 17:12:57  dechavez
 * added buffer length to utilTimeString() calls
 *
 * Revision 1.7  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
