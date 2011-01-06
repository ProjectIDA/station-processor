#pragma ident "$Id: report.c,v 1.6 2005/05/06 01:13:23 dechavez Exp $"
/*======================================================================
 *
 *  Status reports (client MUTEX is locked!)
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_REPORT

static time_t StartTime;

static VOID PrintCmndDetails(FILE *fp, CLIENT *client)
{
    if (client->last.msg.len > 0) {
        utilPrintHexDump(fp, client->last.msg.data, client->last.msg.len);
    }
}

VOID InitReportGenerator(PARAM *par)
{
static char *fid = "InitReportGenerator";

    StartTime = time(NULL);
}

/* Print a status report */

VOID ClientReport(FILE *fp, CLIENT *client, time_t now)
{
static char buf[256];
struct {
    IACP_STATS stats;
    time_t tstamp;
} send, recv;
time_t start;

    fprintf(fp, "\n");

    start = iacpGetStartTime(client->iacp);

    iacpGetRecvStats(client->iacp, &recv.stats);
    recv.tstamp = iacpGetRecvTstamp(client->iacp);

    iacpGetSendStats(client->iacp, &send.stats);
    send.tstamp = iacpGetSendTstamp(client->iacp);

    fprintf(fp, "Client index %lu\n", client->index);
    fprintf(fp, "%s connected since %s ", client->ident, utilLttostr(start, 0, buf));
    fprintf(fp, "(%s)\n", utilLttostr(now - start, 8, buf));

    fprintf(fp, "Last input:   %s (%lu bytes, %lu frames) (last type=%lu, len=%lu)\n",
        utilLttostr(now - recv.tstamp, 8, buf), recv.stats.nbyte, recv.stats.nframe,
        client->last.msg.type, client->last.msg.len
    );

    fprintf(fp, "Last output:  %s (%lu bytes, %lu frames)\n",
         utilLttostr(now - send.tstamp, 8, buf), send.stats.nbyte, send.stats.nframe
    );

    PrintCmndDetails(fp, client);
}

VOID PrintStatusReport(FILE *fp)
{
time_t now;
char buf[256];
static char *fid = "PrintStatusReport";

    fprintf(fp, "\n");
    fprintf(fp, "**** ISID STATUS REPORT ****\n");

    now = time(NULL);
    fprintf(fp, "\n");
    fprintf(fp, "ISID %s\n", VersionIdentString);
    fprintf(fp, "pid = %d\n", getpid());
    fprintf(fp, "uptime: %s\n", utilLttostr(now - StartTime, 8, buf));

/* frontend report */

    if (PrintActiveClientReport(fp, now) == 0) {
        fprintf(fp, "\n");
        fprintf(fp, "No clients currently connected.\n");
    }

    fprintf(fp, "\n");
    fprintf(fp, "**** ISID STATUS REPORT COMPLETE ****\n\n");
}

/* Revision History
 *
 * $Log: report.c,v $
 * Revision 1.6  2005/05/06 01:13:23  dechavez
 * cleared tabs
 *
 * Revision 1.5  2005/05/06 01:12:26  dechavez
 * support for IACP_STATS
 *
 * Revision 1.4  2003/12/22 21:09:10  dechavez
 * include version number in report
 *
 * Revision 1.3  2003/11/04 00:20:05  dechavez
 * Include index in ClientReport
 *
 * Revision 1.2  2003/10/16 21:08:14  dechavez
 * corrected program name
 *
 * Revision 1.1  2003/10/16 18:07:24  dechavez
 * initial release
 *
 */
