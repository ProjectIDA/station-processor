#pragma ident "$Id: pconfig.c,v 1.4 2008/01/16 23:54:00 dechavez Exp $"
/*======================================================================
 *
 *  Print contents of configuration record.
 *
 *====================================================================*/
#include "idalst.h"

void pconfig(IDA_CONFIG_REC *cnf, struct counters *count, int print)
{
int i;
short tst;
char *logger;
static long tagno = CONFIG_TAG;

    tag(tagno++); printf("\n"); tag(tagno++);

    printf("Record %ld is a", count->rec);
    if (cnf->atod == IDA_DAS) {
        printf(" %s", logger = "DAS");
    } else if (cnf->atod == IDA_ARS) {
        printf("n %s", logger = "ARS");
    } else {
        printf("n %s", logger = "UNKNOWN");
    }
    printf(" configuration record:\n");

    if (cnf->ttag.year != 9999) {
        tag(tagno++);
        printf("Year                  = %hd\n", cnf->ttag.year);
        tag(tagno++);
        printf("External time stamp   = ");
        printf("%s\n", ext_timstr(cnf->ttag.ext, NULL));
    } else {
        tag(tagno++);
        printf("External time stamp   = ");
        printf("%s\n", util_lttostr(cnf->ttag.ext, 0));
    }

    tag(tagno++);
    printf("Digitizer sample rate = %hd\n", cnf->srate);

    tag(tagno++);
    printf("Backplane jumpers     = 0x%0x\n", cnf->jumpers);

    tag(tagno++);
    printf("Detector channels     =");
    if (cnf->detect.chan == 0) printf(" none defined");
    for (i = 0; i < sizeof(cnf->detect.chan) * 8; i++) {
        tst = cnf->detect.chan >> i;
        if (tst & 0x01) printf(" %d", i);
        if (cnf->detect.key == i) printf("(key)");
    }
    printf("\n");

    if (print & P_DETECT && cnf->detect.chan != 0) {
        tag(tagno++);
        printf("STA window len. (sec) = %hd\n", cnf->detect.sta);
        tag(tagno++);
        printf("LTA window len. (sec) = %hd\n", cnf->detect.lta);
        tag(tagno++);
        printf("Detection threshold   = 0.%04hd\n", cnf->detect.thresh);
        tag(tagno++);
        printf("Max. event len. (sec) = %ld\n", cnf->detect.maxlen);
        tag(tagno++);
        printf("Min. no. voters       = %hd\n", cnf->detect.voters);
        tag(tagno++);
        printf("No. pre-event buffers = %hd\n", cnf->detect.memory);
    }

    tag(tagno++); printf("\n");
    tag(tagno++);
    printf("          %s ", logger);
    printf("Configuration Table (record %ld)\n", count->rec);
    tag(tagno++);
    printf("Stream  Chan  Filt  Mode  DAT  MDM  D2A  Gain  Detect\n");

    for (i = 0; i < cnf->nstream; i++) {
        if (cnf->table[i].dl_channel >= 0) {
            tag(tagno++);
            printf("  %02hd", cnf->table[i].dl_stream);
            printf("%8hd",cnf->table[i].dl_channel);
            printf("%6hd",cnf->table[i].dl_filter);
            printf("  %s",cnf->table[i].mode == CONT ? "cont" : "trig");
            printf("  %s",cnf->table[i].output&IDA_TAPE  ?"yes":" no");
            printf("  %s",cnf->table[i].output&IDA_MODEM ?"yes":" no");
            printf("  %s",cnf->table[i].output&IDA_DTOA  ?"yes":" no");
            if (cnf->table[i].dl_gain >= 0) {
                printf("%6hd ", cnf->table[i].dl_gain);
            } else {
                printf("   n/a ");
            }
            tst = cnf->detect.chan >> cnf->table[i].dl_channel;
            printf("   %s", tst & 0x01 ? "yes" : "no");
            if (cnf->table[i].dl_channel == cnf->detect.key) printf("*");
            printf("\n");
        }
    }
}

/* Revision History
 *
 * $Log: pconfig.c,v $
 * Revision 1.4  2008/01/16 23:54:00  dechavez
 * add new xxx_timstr() buffer args
 *
 * Revision 1.3  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
