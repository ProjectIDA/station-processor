#pragma ident "$Id: main.c,v 1.2 2006/08/15 01:20:54 dechavez Exp $"
#include "sanio.h"
#include <errno.h>
#include <ctype.h>
#include "ida10.h"

#define MAX_STREAMS 64
struct {
    IDA10_TSHDR hdr;
    int count;
} save[MAX_STREAMS];

BOOL SoftPLL = FALSE;

static long IncrError(IDA10_TS *ts)
{
int i, status, sign;
UINT32 err;

    for (i = 0; i < MAX_STREAMS; i++) {
        if (save[i].count == 0) {
            save[i].hdr = ts->hdr;
            ++save[i].count;
            return 0;
        } else if (strcmp(save[i].hdr.cname, ts->hdr.cname) == 0) {
            status = ida10TtagIncrErr(&save[i].hdr, &ts->hdr, &sign, &err);
            if (status != 0) {
                fprintf(stderr, "\nida10TtagIncrErr: error %d\n", status);
                exit(1);
            }
            ++save[i].count;
            save[i].hdr = ts->hdr;
            return (long) sign * (long) err;
        }
    }

    fprintf(stderr, "\nIncrease MAX_STREAMS (%d)\n", MAX_STREAMS);
    exit(1);
}

static BOOL ProbablyValidTimeStamp(IDA10_TTAG *ttag)
{
    if (ttag->status & IDA10_TT_STAT_RTT_FAILURE) return FALSE;
    if (ttag->status & IDA10_TT_STAT_EXTTIME_NOINIT) return FALSE;
    if (ttag->status & IDA10_TT_STAT_SUSPICIOUS_GPS) return FALSE;
    if (!(ttag->status & IDA10_TT_STAT_EXTTIME_QUALITY)) return FALSE;
    if (!(ttag->status & IDA10_TT_STAT_EXTTIME_AVAIL)) return FALSE;
    if (!(ttag->status & IDA10_TT_STAT_EXTPPS_AVAIL)) return FALSE;

    if (SoftPLL) return TRUE;

    if (!(ttag->status & IDA10_TT_STAT_PLL_ENABLE)) return FALSE;

    return TRUE;
}

static void ProcessTS(UINT8 *buf, long recno)
{
long error, timerr = 0;
IDA10_TS ts;
char tmpbuf[1024];
struct {
    INT64 offset;
    INT64 systim;
} Save;
INT64 systim, offset, OffsetDiff;
REAL64 EpochTime;
IDA10_TTAG *ttag;
char OffsetChar;
static int count = 25;
static BOOL HaveOffset = FALSE;
static char *SanHeader = 
"#                                                                          "
"                                     10-15 9  67   5   4   3   2   1   0\n"
"# Recno Stream ---- Ext Time --- --- Sys Time ---  ---- Offset ----  ----- "
"Epoch Time ----   PLL    Phase Status=VNDR ? TYPE SYN VLD LKD ETA PLL PPS";
static char *OfisHeader = 
"#                                                                          "
"                                     10-15 9  67   5   4   3   2   1   0\n"
"# Recno Stream ---- Ext Time --- --- Sys Time ---  ---- Offset ----  ----- "
"Epoch Time ----   ----- PPS Time ----- Status=VNDR ? TYPE SYN VLD LKD ETA PLL PPS";

    if (!ida10UnpackTS(buf, &ts)) {
        printf(" %7ld - *ERROR*\n", recno);
        return;
    }
    ttag = &ts.hdr.cmn.ttag.beg;

    if (count++ % 25 == 0) printf("%s\n", ttag->type == IDA10_TIMER_OFIS ? OfisHeader : SanHeader);
    printf(" %6s", ts.hdr.cname);
    printf("%7ld", recno);

    OffsetDiff = 0;
    OffsetChar = ' ';
    systim = ida10SystemTime(ttag);
    if (HaveOffset && systim < Save.systim) HaveOffset = FALSE; /* erase offset at power cycle */
    if (ProbablyValidTimeStamp(ttag)) {
        offset = ida10Offset(ttag);
        if (!HaveOffset) {
            Save.systim = systim;
            Save.offset = offset;
            HaveOffset = TRUE;
            OffsetChar = '*';
        } else if ((OffsetDiff = offset - Save.offset) != 0) {
            Save.offset = offset;
            OffsetChar = '*';
        }
    } else {
        OffsetChar = '-';
    }

    if ((INT32) ttag->external > 0) {
        printf(" %s", utilLttostr(ttag->external, 1000, tmpbuf));
    } else if ((INT32) ttag->external < 0) {
        printf(" %17ld", (INT32) ttag->external);
    } else {
        printf("                  ");
    }

    printf(" %s",  utilLttostr(ttag->system.sec, 8, tmpbuf));
    printf(".%03hd", ttag->system.msc);
    printf("%c", IncrError(&ts) ? '*' : ' ');

    if (HaveOffset) {
        printf(" %s%c",  utilDttostr((REAL64) Save.offset / 1000.0, 8, tmpbuf), OffsetChar);
    } else {
        printf("                  ");
    }

    if (HaveOffset) {
        EpochTime = (REAL64) (systim + Save.offset) / 1000.0;
        printf(" %s", utilDttostr(EpochTime, 1000, tmpbuf));
    } else {
        printf("                      ");
    }

    if (ttag->type == IDA10_TIMER_OFIS) {
        printf(" %s",  utilLttostr(ttag->ofis.pps.sec, 8, tmpbuf));
        printf(".%03hd", ttag->ofis.pps.msc);
    } else {
        printf(" %5hu", ttag->san.pll);
        printf(" %8lld", ida10PhaseToUsec(ttag->san.phase));
    }
    printf(" %s", ida10ClockStatusString(ttag, tmpbuf));
    printf("\n");
}

static void help(char *myname)
{
    fprintf(stderr, "usage: %s [-softpll]\n", myname);
    exit(0);
}

int main(int argc, char **argv)
{
int i, status, RecordType;
UINT8 buf[IDA10_MAXRECLEN];
long recno;

    for (i = 0; i < MAX_STREAMS; i++) save[i].count = 0;

    for (i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "--help") == 0) {
            help(argv[0]);
        } else if (strcasecmp(argv[i], "-softpll") == 0) {
            SoftPLL = TRUE;
        }
    }

    recno = 0;
    while ((status = ida10ReadRecord(stdin, buf, IDA10_MAXRECLEN, &RecordType)) != IDA10_EOF) {
        ++recno;
        if (status != IDA10_OK) {
            printf("error %d: %s\n", status, ida10ErrorString(status));
        } else if (RecordType == IDA10_TYPE_TS) {
            ProcessTS(buf, recno);
        }
    }
    printf("EOF encountered after reading %ld records\n", recno);
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2006/08/15 01:20:54  dechavez
 * 10.3 (OFIS) support
 *
 * Revision 1.1  2004/12/10 18:32:02  dechavez
 * initial release
 *
 */
