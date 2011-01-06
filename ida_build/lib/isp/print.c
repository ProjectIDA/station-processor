#pragma ident "$Id: print.c,v 1.5 2007/10/31 17:05:25 dechavez Exp $"
/*======================================================================
 *
 *  Print various things.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include "isp.h"

#define ONE_MIN   60
#define ONE_HOUR  60 * ONE_MIN
#define ONE_DAY   24 * ONE_HOUR

char isp_alrm_code(struct isp_alarm *alarm, int mask)
{
int set;
time_t now, interval;

    now = time(NULL);
    set = alarm->code & mask;
    if (!set) return '0';

    switch (mask) {
      case ISP_ALARM_DAS:
        interval = now - alarm->das; break;
      case ISP_ALARM_OPER:
        interval = now - alarm->oper; break;
      case ISP_ALARM_IOERR:
        interval = now - alarm->ioerr; break;
      case ISP_ALARM_OFFLINE:
        interval = now - alarm->offline; break;
      case ISP_ALARM_RQFULL:
        interval = now - alarm->rqfull; break;
      case ISP_ALARM_OQFULL:
        interval = now - alarm->oqfull; break;
      case ISP_ALARM_BQFULL:
        interval = now - alarm->bqfull; break;
      case ISP_ALARM_AUX:
        interval = now - alarm->aux; break;
      case ISP_ALARM_DASDBG:
        interval = now - alarm->dasdbg; break;
      case ISP_ALARM_DASCLK:
        interval = now - alarm->dasclk; break;
      case ISP_ALARM_AUXCLK:
        interval = now - alarm->auxclk; break;
      case ISP_ALARM_NRTS:
        interval = now - alarm->nrts; break;
      case ISP_ALARM_ADC:
        interval = now - alarm->adc; break;
      default:
        return '?';
    }

    if (interval <      ONE_MIN ) return '1';
    if (interval <  2 * ONE_MIN ) return '2';
    if (interval <  5 * ONE_MIN ) return '3';
    if (interval < 15 * ONE_MIN ) return '4';
    if (interval < 30 * ONE_MIN ) return '5';
    if (interval <      ONE_HOUR) return '6';
    if (interval <  4 * ONE_HOUR) return '7';
    if (interval <  8 * ONE_HOUR) return '8';
    if (interval < 12 * ONE_HOUR) return '9';
    if (interval < 18 * ONE_HOUR) return 'A';
    if (interval <      ONE_DAY ) return 'B';
    if (interval <  2 * ONE_DAY ) return 'C';
    if (interval <  3 * ONE_DAY ) return 'D';
    if (interval <  4 * ONE_DAY ) return 'E';
                                  return 'F';
}

void isp_prtstatus(FILE *fp, struct isp_status *status)
{
int rel, ver, sub;
IDA_IDENT_REC  *idrec;
IDA_CONFIG_REC *cnf;
long uptime, latency;
float percent;
char m, c, c1, c2, c3;
static char *tape = "tape";
static char *Tape = "Tape";
static char *disk = "disk";
static char *Disk = "Disk";
static char *huh  = "????";
char *media, *Media;

    if (status->output.type == ISP_OUTPUT_DISK) {
        media = disk;
        Media = Disk;
    } else if (status->output.type == ISP_OUTPUT_TAPE) {
        media = tape;
        Media = Tape;
    } else {
        media = huh;
        Media = huh;
    }

    idrec = &status->iddata;
    cnf   = &status->config;

    uptime  = status->tstamp - status->start;

/* Line 1 */

    fprintf(fp,
        "Alarms DAS OPR IOE OFF OVR AUX DBG DCL ACL          ISP Clock: "
    );
    fprintf(fp, "%s\n", util_lttostr(status->tstamp, 0));

/* Line 2 */

    fprintf(fp, "0x%04x", status->alarm.code);
    fprintf(fp, "  %c",   isp_alrm_code(&status->alarm, ISP_ALARM_DAS));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_OPER));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_IOERR));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_OFFLINE));
    c1 = isp_alrm_code(&status->alarm, ISP_ALARM_OQFULL);
    c2 = isp_alrm_code(&status->alarm, ISP_ALARM_BQFULL);
    c3 = isp_alrm_code(&status->alarm, ISP_ALARM_RQFULL);
    c = c1;
    c = (c1 > c2) ? c1 : c2;
    if (c3 > c) c = c3;
    fprintf(fp, "   %c",  c);
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_AUX));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_DASDBG));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_DASCLK));
    fprintf(fp, "   %c",  isp_alrm_code(&status->alarm, ISP_ALARM_AUXCLK));

    fprintf(fp, "                  UPTIME:      %s\n",
        util_lttostr(uptime, 8)
    );

/* Line 3 */

    fprintf(fp, "\n");

/* Line 4 */

    fprintf(fp, "DAS clock: ");
    if (status->clock > 0) {
        fprintf(fp, "%s", util_lttostr(status->clock, 0));
        fprintf(fp, " Quality: ");
        if (status->clock_qual == 1) {
            fprintf(fp, "Locked");
        } else if (status->clock_qual > 6) {
            fprintf(fp, "Unknown");
        } else if (status->clock < 0) {
            fprintf(fp, "ERROR");
        } else {
            fprintf(fp, "Unlocked");
        }
    }
    fprintf(fp, "\n");

/* Line 5 */

    fprintf(fp, "Output Buffer: ");
    percent = (float) (status->buffer.nrec * 100) /
              (float)  status->buffer.capacity;
    fprintf(fp, "%3d%%%c (%7ld) ", 
        (int) percent, m, status->buffer.nrec
    );

    if (status->output.state == ISP_OUTPUT_ONLINE) {
        if (media == disk) {
            fprintf(fp, "%s: %7ld recs            ",
                Media, status->output.nrec
            );
            percent = (float) (status->output.nrec * 100) /
                      (float) status->output.capacity;
            fprintf(fp, "  Capacity: %3d%%", (int) percent);
        } else if (media == tape) {
            if (status->output.file == 1) {
                fprintf(fp, "%s: %7ld recs                     ",
                    Media, status->output.nrec
                );
            } else {
                fprintf(fp, "%s (file %d): %7ld recs           ",
                    Media, status->output.file, status->output.nrec
                );
            }
        }
    } else if (status->output.state == ISP_OUTPUT_SKIP) {
        fprintf(fp, "%s: %s skipping to end of data",
            Media, status->output.device
        );
    } else if (status->output.state == ISP_OUTPUT_OPER) {
        fprintf(fp, "%s: %s WAITING FOR OPERATOR INPUT",
            Media, status->output.device
        );
    } else if (status->output.state == ISP_OUTPUT_OFFLINE) {
        fprintf(fp, "%s: NO MEDIA PRESENT AT %s",
            Media, status->output.device
        );
    } else {
        fprintf(fp, "%s: unknown %s status", Media, status->output.device);
    }
    fprintf(fp, "\n");

/* Line 6 */

    fprintf(fp, "\n");

/* Line 7 */

    fprintf(fp, "UPLINK STATISTICS ");
    if (status->cleared.comm > 0) {
        latency = status->tstamp - status->cleared.comm;
        fprintf(fp, "(%s)", util_lttostr(latency, 8));
    }
    fprintf(fp, "\n");
    fprintf(fp, "BadPkt NoSync  Short    NAK DupPkt   ");
    fprintf(fp, "Last Read    Last Packet   ");
    if (status->last.commerr > 0) fprintf(fp, "Last Error");
    fprintf(fp, "\n");
    fprintf(fp, "%6ld ", status->count.badr);
    fprintf(fp, "%6ld ", status->count.sync);
    fprintf(fp, "%6ld ", status->count.shrt);
    fprintf(fp, "%6ld ", status->count.naks);
    fprintf(fp, "%6ld ", status->count.dups);
    if (status->last.read > 0) {
        latency = status->tstamp - status->last.read;
        fprintf(fp, " %s ", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "              ");
    }
    if (status->last.pkt > 0) {
        latency = status->tstamp - status->last.pkt;
        fprintf(fp, " %s", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "              ");
    }
    if (status->last.commerr > 0) {
        latency = status->tstamp - status->last.commerr;
        fprintf(fp, " %s", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "              ");
    }
    fprintf(fp, "\n");

/* Line 8 */

    fprintf(fp, "\n");

/* Line 9 */

    fprintf(fp, "PACKETS RECEIVED ");
    if (status->cleared.pkt > 0) {
        latency = status->tstamp - status->cleared.pkt;
        fprintf(fp, "(%s)", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "                          ");
    }
    fprintf(fp, "\n");
    fprintf(fp, "     Data  Ident Config  Calib    Log  Event  ");
    fprintf(fp, "Status  Other    Aux     Total\n");
    fprintf(fp, "%9ld ", status->count.drec);
    fprintf(fp, "%6ld ", status->count.irec);
    fprintf(fp, "%6ld ", status->count.krec);
    fprintf(fp, "%6ld ", status->count.crec);
    fprintf(fp, "%6ld ", status->count.lrec);
    fprintf(fp, "%6ld ", status->count.erec);
    fprintf(fp, "%7ld ", status->count.srec);
    fprintf(fp, "%6ld ", status->count.orec);
    fprintf(fp, "%6ld ", status->count.arec);
    fprintf(fp, "%9ld ", status->count.nrec);
    fprintf(fp, "\n");

/* Line 10 */

    fprintf(fp, "\n");
    
/* Line 11 */

    rel = idrec->atod_rev / 100;
    if (rel < 8) {
        rel = idrec->atod_rev / 10;
        ver = idrec->atod_rev - (rel * 10);
        sub = -1;
    } else {
        ver = (idrec->atod_rev - (rel * 100)) / 10;
        sub = idrec->atod_rev - (rel * 100) - (ver * 10);
    }
    fprintf(fp, "DAS STATISTICS ");
    if (status->have_iddata) {
        if (sub < 0) {
            fprintf(fp,
                "(STA `%s', Rev V%d.%d/%d, CPUID %hd, jumpers 0x%0x)",
                idrec->sname, rel, ver, idrec->dsp_rev,
                idrec->atod_id, cnf->jumpers
            );
        } else {
            fprintf(fp,
                "(STA `%s', Rev V%d.%d.%d/%d, CPUID %hd, jumpers 0x%0x)",
                idrec->sname, rel, ver, sub, idrec->dsp_rev,
                idrec->atod_id, cnf->jumpers
            );
        }
    }
    fprintf(fp, "\n");
    fprintf(fp, " Last Reboot Count     Sent   Drop   Errs ");
    fprintf(fp, "NoAcks  Trigs   Last Report\n");
    if (status->das.start > 0) {
        latency = status->tstamp - status->das.start;
        fprintf(fp, "%s ", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "             ");
    }
    fprintf(fp, "%5ld ", status->das.reboots);
    fprintf(fp, "%8ld ", status->das.acks);
    fprintf(fp, "%6ld ", status->das.dropped);
    fprintf(fp, "%6ld ", status->das.naks);
    fprintf(fp, "%6ld ", status->das.timeouts);
    fprintf(fp, "%6ld ", status->das.triggers);
    if (status->last.dasstat > 0) {
        latency = status->tstamp - status->last.dasstat;
        fprintf(fp, " %s", util_lttostr(latency, 8));
    } else {
        fprintf(fp, "             ");
    }
    fprintf(fp, "\n");

/* Line 12 */

    fprintf(fp, "\n");

/* Line 13 */

    fprintf(fp, "Trig: ");
    if (status->have_config && cnf->detect.chan <= 0) {
        fprintf(fp, "disabled   ");
    } else if (status->flag.event) {
        fprintf(fp, "ON         ");
    } else {
        fprintf(fp, "off        ");
    }

    fprintf(fp, "Calib: ");
    if (status->flag.calib == ISP_CALIB_OFF) {
        fprintf(fp, "off       ");
    } else if (status->flag.calib == ISP_CALIB_PENDING) {
        fprintf(fp, "pending   ");
    } else if (status->flag.calib == ISP_CALIB_ON) {
        fprintf(fp, "ON        ");
    } else {
        fprintf(fp, "%d?       ", status->flag.calib);
    }

    fprintf(fp, "Config: ");
    if (status->flag.config == ISP_CONFIG_UNKNOWN) {
        fprintf(fp, "unknown    ");
    } else if (status->flag.config == ISP_CONFIG_INPROGRESS) {
        fprintf(fp, "in progress");
    } else if (status->flag.config == ISP_CONFIG_VERIFIED) {
        fprintf(fp, "verified   ");
    } else {
        fprintf(fp, "%d?       ", status->flag.config);
    }

    fprintf(fp, "\n");
}

void isp_dascnf_text(
struct isp_dascnf *cnf,
int index,
char **chan,
char **filt,
char **name,
char **mode
){
static char fbuf[5];
static char cbuf[5];
static char nbuf[NRTS_CNAMLEN+1];
static char mbuf[5];
static char *cont = "cont";
static char *trig = "trig";
static char *na   = " n/a";

    if (cnf->stream[index].active) {
        sprintf(cbuf, "%4d", cnf->stream[index].channel);
        *chan = cbuf;
        sprintf(fbuf, "%4d", cnf->stream[index].filter);
        *filt = fbuf;
        *mode = cnf->stream[index].mode ? trig : cont;
        strlcpy(nbuf, cnf->stream[index].name, NRTS_CNAMLEN+1);
        *name = nbuf;
    } else {
        *chan = na;
        *filt = na;
        *mode = na;
        *name = na;
    }
}

static void PrintStChanFiltModeName(
    FILE *fp, int st, struct isp_dascnf *cnf, int col
){
char *chan, *filt, *mode, *name;

    if (col == 2) fprintf(fp, "     ");
    isp_dascnf_text(cnf, st, &chan, &filt, &name, &mode);
    fprintf(fp, "%2d %s %s %s %-5s", st, chan, filt, mode, name);
}

void isp_prtdascnf(FILE *fp, struct isp_dascnf *cnf)
{
short bitmap;
int i, j, bit, tst, DetectorEnabled;
char *chan, *filt, *mode, *name, tmp[30];

    if (cnf->flag != ISP_DASCNF_SET) return;

    DetectorEnabled = cnf->detect.bitmap;

/* Line 1 */

    fprintf(fp,
        "St Chan Filt Mode Name      St Chan Filt Mode Name    Detector\n"
    );

    i = 0;
    j = 12;

/* Line 2 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) {
        fprintf(fp, "   STA     = %d", cnf->detect.sta);
    } else {
        fprintf(fp, "   <disabled>");
    }
    fprintf(fp, "\n");

/* Line 3 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) fprintf(fp, "   LTA     = %d", cnf->detect.lta);
    fprintf(fp, "\n");

/* Line 4 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) fprintf(fp, "   Thresh  = %.2f", 10000.0 / (float) cnf->detect.thresh);
    fprintf(fp, "\n");

/* Line 5 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) fprintf(fp, "   Voters  = %d", cnf->detect.voters);
    fprintf(fp, "\n");

/* Line 6 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) fprintf(fp, "   MaxTrig = %d", cnf->detect.maxtrig);
    fprintf(fp, "\n");

/* Line 7 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) fprintf(fp, "   PreBuf  = %d", cnf->detect.preevent);
    fprintf(fp, "\n");

/* Line 8 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    if (DetectorEnabled) {
        tmp[0] = 0;
        bitmap = cnf->detect.bitmap;
        for (bit = 0; bit < sizeof(cnf->detect.bitmap) * 8; bit++) {
            tst = bitmap >> bit;
            if (tst & 0x01) sprintf(tmp+strlen(tmp), " %d", bit);
            if (cnf->detect.key == bit) sprintf(tmp+strlen(tmp), "(key)");
        }
        fprintf(fp, "   Chans   =%s", tmp);
    }
    fprintf(fp, "\n");

/* Line 9 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

/* Line 10 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

/* Line 11 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

/* Line 12 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

/* Line 13 */

    PrintStChanFiltModeName(fp, i++, cnf, 1);
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

/* Line 14 */

    fprintf(fp, "                       ");
    PrintStChanFiltModeName(fp, j++, cnf, 2);

    fprintf(fp, "\n");

    fflush(fp);
}

/* Revision History
 *
 * $Log: print.c,v $
 * Revision 1.5  2007/10/31 17:05:25  dechavez
 * replaced string memcpy with strlcpy
 *
 * Revision 1.4  2006/02/09 20:05:14  dechavez
 * support for libida 4.0.0 and libisidb 1.0.0
 *
 * Revision 1.3  2002/05/14 20:45:49  dec
 * support 5 char channel names
 *
 * Revision 1.2  2000/11/08 19:30:33  dec
 * added adc alarm  to isp_alrm_code
 *
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
