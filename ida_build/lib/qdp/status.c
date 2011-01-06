#pragma ident "$Id: status.c,v 1.5 2009/02/03 23:05:10 dechavez Exp $"
/*======================================================================
 * 
 * Deal with C1_STAT packets
 *
 *====================================================================*/
#include "qdp.h"
#include "util.h"

/* Print contents of a decoded C1_STAT packet */

static void BinUINT32(FILE *fp, UINT16 value)
{
    utilPrintBinUINT8(fp, (UINT8) (value >> 24));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value >> 16);
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) (value >> 8));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value);
    fprintf(fp, "\n");
}

static void BinUINT16(FILE *fp, UINT16 value)
{
    utilPrintBinUINT8(fp, (UINT8) (value >> 8));
    fprintf(fp, " ");
    utilPrintBinUINT8(fp, (UINT8) value);
    fprintf(fp, "\n");
}

static void BinUINT8(FILE *fp, UINT8 value)
{
    utilPrintBinUINT8(fp, value);
    fprintf(fp, "\n");
}

static void PrintGBL(FILE *fp, QDP_STAT_GBL *gbl)
{
char tmpbuf[80];

    fprintf(fp, "Global Status\n");
    fprintf(fp, "-------------\n");
    fprintf(fp, "     Acquisition Control = 0x%04x\n", gbl->aqctrl);
    fprintf(fp, "           Clock Quality = "); BinUINT16(fp, gbl->clock_qual);
    fprintf(fp, "      Minutes Since Loss = %hd\n", gbl->clock_loss);
    fprintf(fp, "          Seconds Offset = %hu.%06hu\n", gbl->sec_offset, gbl->usec_offset);
    fprintf(fp, "             Total Hours = %4.2f\n", (float) (gbl->total_time / SEC_PER_HOUR));
    fprintf(fp, "          Power On Hours = %4.2f\n", (float) (gbl->power_time / SEC_PER_HOUR));
    fprintf(fp, "    Time of Last Re-Sync = %s\n", utilLttostr(gbl->last_resync + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "Total Number of Re-Syncs = %d\n", (int) gbl->resyncs);
    fprintf(fp, "              GPS Status = "); BinUINT16(fp, gbl->gps_stat);
    fprintf(fp, "       Calibrator Status = "); BinUINT16(fp, gbl->cal_stat);
    fprintf(fp, "  Sensor Control Bitmmap = "); BinUINT16(fp, gbl->sensor_map);
    fprintf(fp, "           Status Inputs = "); BinUINT16(fp, gbl->stat_inp);
    fprintf(fp, "    Miscellaneous Inputs = "); BinUINT16(fp, gbl->misc_inp);
    fprintf(fp, "    Data sequence number = %hu\n", gbl->data_seq);
    fprintf(fp, " Current sequence number = %lu\n", gbl->cur_sequence);
    fprintf(fp, "             Current VCO = %hd", gbl->cur_vco & 0x7ffff);
    if (gbl->cur_vco & 0x8000) fprintf(fp, " (under PLL control)"); fprintf(fp, "\n");

    fprintf(fp, "\n");
}

static void PrintGPS(FILE *fp, QDP_STAT_GPS *gps)
{
char tmpbuf[80];

    fprintf(fp, "GPS Status\n");
    fprintf(fp, "----------\n");
    if (gps->gpson) {
        fprintf(fp, "          ON ");
    } else {
        fprintf(fp, "         OFF ");
    }
    fprintf(fp, "Time = %d min\n", (int) gps->gpstime);
    fprintf(fp, "       Satellites = %d used, %d in view\n", (int) gps->sat_used, (int) gps->sat_view);
    fprintf(fp, "             Time = %s\n", gps->time);
    fprintf(fp, "             Date = %s\n", gps->date);
    fprintf(fp, "              Fix = %s\n", gps->fix);
    fprintf(fp, "        Elevation = %s\n", gps->height);
    fprintf(fp, "         Latitude = %s\n", gps->lat);
    fprintf(fp, "        Longitude = %s\n", gps->lon);
    fprintf(fp, "Time of Last 1PPS = %s\n", utilLttostr(gps->last_good + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "  Checksum Errors = %d\n", (int) gps->check_err);

    fprintf(fp, "\n");
}

static void PrintPWR(FILE *fp, QDP_STAT_PWR *pwr)
{
    fprintf(fp, "Power Supply Status\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "     Charging Phase = "); BinUINT16(fp, pwr->phase);
    fprintf(fp, "Battery Temperature = %hu deg C\n", pwr->battemp);
    fprintf(fp, "   Battery Capacity = %hu%%\n", pwr->capacity);
    fprintf(fp, " Depth of Discharge = %hu%%\n", pwr->depth);
    fprintf(fp, "    Battery Voltage = %.3f\n", (float) pwr->batvolt / 1000.0);
    fprintf(fp, "      Input Voltage = %.3f\n", (float) pwr->inpvolt / 1000.0);
    fprintf(fp, "    Battery Current = %hu ma\n", pwr->batcur);
    fprintf(fp, "Absorption Setpoint = %hu\n", pwr->absorption);
    fprintf(fp, "     Float Setpoint = %hu\n", pwr->floatsp);
    fprintf(fp, "      Alerts Bitmap = "); BinUINT8(fp, pwr->alerts);
    fprintf(fp, "       Loads Bitmap = "); BinUINT8(fp, pwr->loads_off);

    fprintf(fp, "\n");
}

static void PrintBOOM(FILE *fp, QDP_STAT_BOOM *boom)
{
int i;

    fprintf(fp, "Miscellaneous Analog Status\n");
    fprintf(fp, "---------------------------\n");
    fprintf(fp, "           Boom Positions ="); for (i = 0; i < QDP_NCHAN; i++) fprintf(fp, " %4hd", boom->pos[i]);
    fprintf(fp, "\n");
    fprintf(fp, "   Analog Positive Supply = %.2fV\n", (float) boom->amb_pos / 100.0);
    if (boom->amb_neg) fprintf(fp, "   Analog Negative Supply = %.1fV\n", (float) boom->amb_neg / 100.0);
    fprintf(fp, "            Input Voltage = %.2fV\n", (float) boom->supply * 150.0 / 1000.0);
    fprintf(fp, "       System Temperature = %huC\n", boom->sys_temp);
    fprintf(fp, "             Main Current = %huma\n", boom->main_cur);
    fprintf(fp, "      GPS Antenna Current = %huma\n", boom->ant_cur);
    fprintf(fp, "Seismometer 1 Temperature = ");
    if (boom->seis1_temp != 666) fprintf(fp, "%hu deg C\n", boom->seis1_temp); else fprintf(fp, "n/a\n");
    fprintf(fp, "Seismometer 2 Temperature = ");
    if (boom->seis2_temp != 666) fprintf(fp, "%hu deg C\n", boom->seis2_temp); else fprintf(fp, "n/a\n");
    fprintf(fp, " Slave Processor Timeouts = %lu\n", boom->cal_timeouts);

    fprintf(fp, "\n");
}

static void PrintTHR(FILE *fp, QDP_STAT_THR *stat)
{
UINT16 i;

    fprintf(fp, "Thread Status\n");
    fprintf(fp, "-------------\n");

    fprintf(fp, "\n");
}

static void PrintPLL(FILE *fp, QDP_STAT_PLL *pll)
{
    fprintf(fp, "PLL Status\n");
    fprintf(fp, "----------\n");
    fprintf(fp, "     Initial VCO = %.2f\n", pll->vco.initial);
    fprintf(fp, "         RMS VCO = %.2f\n", pll->vco.rms);
    fprintf(fp, "        Best VCO = %.2f\n", pll->vco.best);
    fprintf(fp, "      Time error = %.3f\n", pll->time_error);
    fprintf(fp, "Ticks Track Lock = %lu\n", pll->ticks_track_lock);
    fprintf(fp, "              Km = %hu\n", pll->ticks_track_lock);
    fprintf(fp, "           State = %s (0x%04x)\n", qdpPLLStateString(pll->state), pll->state);

    fprintf(fp, "\n");
}

static void PrintGSV(FILE *fp, QDP_STAT_GSV *gsv)
{
UINT16 i;

    fprintf(fp, "GPS Satellites\n");
    fprintf(fp, "--------------\n");
    fprintf(fp, "    Number Elevation Azimuth SNR\n");
    for (i = 0; i < gsv->nentry; i++) fprintf(fp, "%2d: %6hu %9hu %7hu %3hu\n",
        i+1,
        gsv->satl[i].num,
        gsv->satl[i].elevation,
        gsv->satl[i].azimuth,
        gsv->satl[i].snr
    );

    fprintf(fp, "\n");
}

static char *DotDecimal(UINT32 ip, char *buf)
{
    buf[0] = 0;
    sprintf(buf + strlen(buf), "%03d", (ip & 0xFF000000) >> 24);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x00FF0000) >> 16);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x0000FF00) >>  8);
    sprintf(buf + strlen(buf), ".");
    sprintf(buf + strlen(buf), "%03d", (ip & 0x000000FF) >>  0);

    return buf;
}

static char *MacString(UINT8 *mac, char *buf)
{
int i;

    buf[0] = 0;
    for (i = 0; i < 6; i++) sprintf(buf + strlen(buf), "%02x:", mac[i]);
    buf[strlen(buf)-1] = 0; /* truncate trailing colon */

    return buf;
}

static void PrintARP(FILE *fp, QDP_STAT_ARP *arp)
{
UINT16 i;
char buf1[32], buf2[32];

    fprintf(fp, "ARP Table\n");
    fprintf(fp, "---------\n");
    fprintf(fp, "      IP Address       MAC Address    Timeout\n");
    for (i = 0; i < arp->nentry; i++) fprintf(fp, "%2d: %s %s %7hu\n",
        i+1,
        DotDecimal(arp->addr[i].ip, buf1),
        MacString(arp->addr[i].mac, buf2),
        arp->addr[i].timeout
    );

    fprintf(fp, "\n");
}

static void PrintDPORT(FILE *fp,  QDP_STAT_DPORT *dport)
{
char tmpbuf[80];

    fprintf(fp, "Data Port %d Status\n", dport->log_num + 1);
    fprintf(fp, "------------------\n");
    fprintf(fp, "  Data Packets Sent = %lu\n", dport->sent);
    fprintf(fp, "    Packets Re-Sent = %lu\n", dport->resends);
    fprintf(fp, "  Fill Packets Sent = %lu\n", dport->fill);
    fprintf(fp, "    Sequence Errors = %lu\n", dport->seq);
    fprintf(fp, "            Backlog = %lu bytes\n", dport->pack_used);
    fprintf(fp, "            Timeout = %hu msec\n", dport->retran * 100);
    fprintf(fp, "          Interface = %s\n", qdpPhysicalPortString(dport->phy_num));
    fprintf(fp, "Time Since Last Ack = %hu sec\n", dport->last_ack);
    fprintf(fp, "              Flags = "); BinUINT16(fp, dport->flags);

    fprintf(fp, "\n");
}

static void PrintSPORT(FILE *fp, QDP_STAT_SPORT *sport, int index)
{
    fprintf(fp, "Serial Port %d Status\n", index + 1);
    fprintf(fp, "--------------------\n");
    fprintf(fp, "Receive Checksum Errors = %lu\n", sport->check);
    fprintf(fp, "             I/O Errors = %lu\n", sport->ioerrs);
    fprintf(fp, "              Interface = %s\n", qdpPhysicalPortString(sport->phy_num));
    fprintf(fp, "Dest. Unreachable Count = %lu\n", sport->unreach);
    fprintf(fp, "      Quench ICMP Count = %lu\n", sport->quench);
    fprintf(fp, "Echo Request ICMP Count = %lu\n", sport->echo);
    fprintf(fp, "         Redirect Count = %lu\n", sport->redirect);
    fprintf(fp, "         Overrun Errors = %lu\n", sport->over);
    fprintf(fp, "         Framing Errors = %lu\n", sport->frame);

    fprintf(fp, "\n");
}

static void PrintETH(FILE *fp, QDP_STAT_ETH *eth)
{
    fprintf(fp, "Ethernet Status\n");
    fprintf(fp, "---------------\n");
    fprintf(fp, "Receive Checksum Errors = %lu\n", eth->check);
    fprintf(fp, "             I/O Errors = %lu\n", eth->ioerrs);
    fprintf(fp, "              Interface = %s\n", qdpPhysicalPortString(eth->phy_num));
    fprintf(fp, "Dest. Unreachable Count = %lu\n", eth->unreach);
    fprintf(fp, "      Quench ICMP Count = %lu\n", eth->quench);
    fprintf(fp, "Echo Request ICMP Count = %lu\n", eth->echo);
    fprintf(fp, "         Redirect Count = %lu\n", eth->redirect);
    fprintf(fp, "      Total Runt Frames = %lu\n", eth->runt);
    fprintf(fp, "             CRC errors = %lu\n", eth->crc_err);
    fprintf(fp, "       Broadcast Frames = %lu\n", eth->bcast);
    fprintf(fp, "         Unicast Frames = %lu\n", eth->ucast);
    fprintf(fp, "            Good Frames = %lu\n", eth->good);
    fprintf(fp, "          Jabber Errors = %lu\n", eth->jabber);
    fprintf(fp, "          Out of Window = %lu\n", eth->outwin);
    fprintf(fp, "            Tranmsit OK = %lu\n", eth->txok);
    fprintf(fp, "         Packets Missed = %lu\n", eth->miss);
    fprintf(fp, "             Collisions = %lu\n", eth->collide);
    fprintf(fp, "            Link Status = "); BinUINT16(fp, eth->linkstat);

    fprintf(fp, "\n");
}

static void PrintBALER(FILE *fp, QDP_STAT_BALER *baler)
{
char tmpbuf[80];

    fprintf(fp, "          Last On = %s\n", utilLttostr(baler->last_on + QDP_EPOCH_TO_1970_EPOCH, 0, tmpbuf));
    fprintf(fp, "    No. of Cycles = %lu\n", baler->powerups);
    fprintf(fp, "           Status = "); BinUINT16(fp, baler->baler_status);
    fprintf(fp, "          Up Time = %hu minutes\n", baler->baler_time);
}

static void PrintAllBALERs(FILE *fp, QDP_TYPE_C1_STAT *stat)
{
    fprintf(fp, "BALER status\n");
    fprintf(fp, "------------\n");
    fprintf(fp, "status = "); BinUINT16(fp, stat->baler.special.baler_status);

    if (stat->baler.special.baler_status & 0x0001) {
        fprintf(fp, "\n");
        fprintf(fp, "    Serial 1 Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.serial[0]);
    }

    if (stat->baler.special.baler_status & 0x0002) {
        fprintf(fp, "\n");
        fprintf(fp, "    Serial 2 Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.serial[1]);
    }

    if (stat->baler.special.baler_status & 0x0004) {
        fprintf(fp, "\n");
        fprintf(fp, "    Ethernet Baler\n");
        fprintf(fp, "    --------------\n");
        PrintBALER(fp, &stat->baler.ethernet);
    }

    fprintf(fp, "\n");

}

static void PrintDYN(FILE *fp, QDP_STAT_DYN *dyn)
{
char buf[32];

    fprintf(fp, "Dynamic IP addresses\n");
    fprintf(fp, "--------------------\n");
    fprintf(fp, "Serial 1 IP address = %s\n", DotDecimal(dyn->serial[0], buf));
    fprintf(fp, "Serial 2 IP address = %s\n", DotDecimal(dyn->serial[1], buf));
    fprintf(fp, "Ethernet IP address = %s\n", DotDecimal(dyn->ethernet, buf));

    fprintf(fp, "\n");
}

static void PrintAUX(FILE *fp, QDP_STAT_AUX *aux)
{
    ; // UNSUPPORTED
}

static void PrintSS(FILE *fp, QDP_STAT_SS *ss, int index)
{
    ; // UNSUPPORTED
}

void qdpPrint_C1_STAT(FILE *fp, QDP_TYPE_C1_STAT *stat)
{
char buf[32];

    if (stat->alert.tokens) fprintf(fp, "*** DP tokens changed\n");
    if (stat->alert.port) fprintf(fp, "*** Logical port programming changed\n");
    if (stat->alert.usrmsg.ip != 0) {
        fprintf(fp, "*** User message from %s: '%s'\n", DotDecimal(stat->alert.usrmsg.ip, buf), stat->alert.usrmsg.txt);
    }
    if (stat->bitmap & QDP_SRB_GBL)   PrintGBL(fp, &stat->gbl);
    if (stat->bitmap & QDP_SRB_GPS)   PrintGPS(fp, &stat->gps);
    if (stat->bitmap & QDP_SRB_GSV)   PrintGSV(fp, &stat->gsv);
    if (stat->bitmap & QDP_SRB_PWR)   PrintPWR(fp, &stat->pwr);
    if (stat->bitmap & QDP_SRB_BOOM)  PrintBOOM(fp, &stat->boom);
    if (stat->bitmap & QDP_SRB_THR)   PrintTHR(fp, &stat->thr);
    if (stat->bitmap & QDP_SRB_PLL)   PrintPLL(fp, &stat->pll);
    if (stat->bitmap & QDP_SRB_ARP)   PrintARP(fp, &stat->arp);
    if (stat->bitmap & QDP_SRB_DP1)   PrintDPORT(fp, &stat->dport[0]);
    if (stat->bitmap & QDP_SRB_DP2)   PrintDPORT(fp, &stat->dport[1]);
    if (stat->bitmap & QDP_SRB_DP3)   PrintDPORT(fp, &stat->dport[2]);
    if (stat->bitmap & QDP_SRB_DP4)   PrintDPORT(fp, &stat->dport[3]);
    if (stat->bitmap & QDP_SRB_SP1)   PrintSPORT(fp, &stat->sport[0], 0);
    if (stat->bitmap & QDP_SRB_SP2)   PrintSPORT(fp, &stat->sport[1], 1);
    if (stat->bitmap & QDP_SRB_SP3)   PrintSPORT(fp, &stat->sport[2], 2);
    if (stat->bitmap & QDP_SRB_ETH)   PrintETH(fp, &stat->eth);
    if (stat->bitmap & QDP_SRB_BALER) PrintAllBALERs(fp, stat);
    if (stat->bitmap & QDP_SRB_DYN)   PrintDYN(fp, &stat->dyn);
    if (stat->bitmap & QDP_SRB_AUX)   PrintAUX(fp, &stat->aux);
    if (stat->bitmap & QDP_SRB_SS)    PrintSS(fp, &stat->ss[0], 0);
    if (stat->bitmap & QDP_SRB_SS)    PrintSS(fp, &stat->ss[1], 0);
}

/* Initialize a QDP_TYPE_C1_STAT structure */

BOOL qdpClear_C1_STAT(QDP_TYPE_C1_STAT *dest)
{
    if (dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    dest->gsv.nentry = 0; if (dest->gsv.satl != NULL) free(dest->gsv.satl);
    dest->arp.nentry = 0; if (dest->arp.addr != NULL) free(dest->arp.addr);
    dest->thr.nentry = 0; if (dest->thr.task != NULL) free(dest->thr.task);

    dest->bitmap = 0;
    dest->alert.usrmsg.ip = 0;
    dest->alert.usrmsg.txt[0] = 0;
    dest->alert.port = FALSE;
    dest->alert.tokens = FALSE;

    return TRUE;
}

BOOL qdpInit_C1_STAT(QDP_TYPE_C1_STAT *dest)
{
    if (dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    dest->gsv.nentry = 0;
    dest->gsv.satl = NULL;

    dest->arp.nentry = 0;
    dest->arp.addr = NULL;

    dest->thr.nentry = 0;
    dest->thr.task = NULL;

    return qdpClear_C1_STAT(dest);
}

static int DecodeUMSG(UINT8 *start, QDP_TYPE_C1_STAT *dest)
{
int nchar;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->alert.usrmsg.ip);
    nchar = *ptr++;
    ptr += utilUnpackBytes(ptr, dest->alert.usrmsg.txt, nchar);
    dest->alert.usrmsg.txt[nchar] = 0;

    return (int) (ptr - start);
}

static int DecodeGBL(UINT8 *start, QDP_STAT_GBL *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->aqctrl);
    ptr += utilUnpackUINT16(ptr, &dest->clock_qual);
    ptr += utilUnpackUINT16(ptr, &dest->clock_loss);
    ptr += utilUnpackUINT16(ptr, &dest->current_voltage);

    ptr += utilUnpackUINT32(ptr, &dest->sec_offset);
    ptr += utilUnpackUINT32(ptr, &dest->usec_offset);
    ptr += utilUnpackUINT32(ptr, &dest->total_time);
    ptr += utilUnpackUINT32(ptr, &dest->power_time);
    ptr += utilUnpackUINT32(ptr, &dest->last_resync);
    ptr += utilUnpackUINT32(ptr, &dest->resyncs);

    ptr += utilUnpackUINT16(ptr, &dest->gps_stat);
    ptr += utilUnpackUINT16(ptr, &dest->cal_stat);
    ptr += utilUnpackUINT16(ptr, &dest->sensor_map);
    ptr += utilUnpackUINT16(ptr, &dest->cur_vco);
    ptr += utilUnpackUINT16(ptr, &dest->data_seq);
    ptr += utilUnpackUINT16(ptr, &dest->pll_flag);
    ptr += utilUnpackUINT16(ptr, &dest->stat_inp);
    ptr += utilUnpackUINT16(ptr, &dest->misc_inp);

    ptr += utilUnpackUINT32(ptr, &dest->cur_sequence);

    return (int) (ptr - start);
}

static int DecodeGPS(UINT8 *start, QDP_STAT_GPS *dest)
{
int nchar;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->gpstime);
    ptr += utilUnpackUINT16(ptr, &dest->gpson);
    ptr += utilUnpackUINT16(ptr, &dest->sat_used);
    ptr += utilUnpackUINT16(ptr, &dest->sat_view);

    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->time, 9); dest->time[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->date, 11); dest->date[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->fix, 5); dest->fix[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->height, 11); dest->height[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->lat, 13); dest->lat[nchar] = 0;
    nchar = *ptr++; ptr += utilUnpackBytes(ptr, dest->lon, 13); dest->lon[nchar] = 0;

    ptr += utilUnpackUINT32(ptr, &dest->last_good);
    ptr += utilUnpackUINT32(ptr, &dest->check_err);

    return (int) (ptr - start);
}

static int DecodePWR(UINT8 *start, QDP_STAT_PWR *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->phase);
    ptr += utilUnpackUINT16(ptr, &dest->battemp);
    ptr += utilUnpackUINT16(ptr, &dest->capacity);
    ptr += utilUnpackUINT16(ptr, &dest->depth);
    ptr += utilUnpackUINT16(ptr, &dest->batvolt);
    ptr += utilUnpackUINT16(ptr, &dest->inpvolt);
    ptr += utilUnpackUINT16(ptr, &dest->batcur);
    ptr += utilUnpackUINT16(ptr, &dest->absorption);
    ptr += utilUnpackUINT16(ptr, &dest->floatsp);

    dest->alerts = *ptr++;
    dest->loads_off = *ptr++;

    return (int) (ptr - start);
}

static int DecodeBOOM(UINT8 *start, QDP_STAT_BOOM *dest)
{
int i;
UINT8 *ptr;

    ptr = start;

    for (i = 0; i < QDP_NCHAN; i++) ptr += utilUnpackINT16(ptr, &dest->pos[i]);

    ptr += utilUnpackUINT16(ptr, &dest->amb_pos);
    ptr += utilUnpackUINT16(ptr, &dest->amb_neg);
    ptr += utilUnpackUINT16(ptr, &dest->supply);

    ptr += utilUnpackINT16(ptr, &dest->sys_temp);

    ptr += utilUnpackUINT16(ptr, &dest->main_cur);
    ptr += utilUnpackUINT16(ptr, &dest->ant_cur);

    ptr += utilUnpackINT16(ptr, &dest->seis1_temp);
    ptr += utilUnpackINT16(ptr, &dest->seis2_temp);

    ptr += utilUnpackUINT32(ptr, &dest->cal_timeouts);

    return (int) (ptr - start);
}

static int DecodeTHR(UINT8 *start, QDP_STAT_THR *dest)
{
UINT8 *ptr;
UINT16 tmp16, i;
UINT32 tmp32;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += 2; /* skip over length */
    ptr += utilUnpackUINT32(ptr, &tmp32);
    ptr += utilUnpackUINT16(ptr, &tmp16);
    dest->stime = (UINT64) ((tmp32 << 16) | tmp16);
    ptr += 2; /* skip over spare */

    if ((dest->task = (QDP_STAT_TASK *) malloc(dest->nentry * sizeof(QDP_STAT_TASK))) == NULL) {
        ptr += dest->nentry * 16;
        dest->nentry = 0;
    } else {
        for (i = 0; i < dest->nentry; i++) {
            ptr += utilUnpackUINT32(ptr, &tmp32);
            ptr += utilUnpackUINT16(ptr, &tmp16);
            dest->task[i].runtime = (UINT64) ((tmp32 << 16) | tmp16);
            dest->task[i].priority = *ptr++;
            dest->task[i].counter = *ptr++;
            ptr += utilUnpackUINT32(ptr, &tmp32);
            ptr += utilUnpackUINT16(ptr, &tmp16);
            dest->task[i].since = (UINT64) ((tmp32 << 16) | tmp16);
            ptr += utilUnpackUINT16(ptr, &dest->task[i].flags);
        }
    }

    return (int) (ptr - start);
}

static int DecodePLL(UINT8 *start, QDP_STAT_PLL *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackREAL32(ptr, &dest->vco.initial);
    ptr += utilUnpackREAL32(ptr, &dest->time_error);
    ptr += utilUnpackREAL32(ptr, &dest->vco.rms);
    ptr += utilUnpackREAL32(ptr, &dest->vco.best);
    ptr += 4; /* skip over "spare" */
    ptr += utilUnpackUINT32(ptr, &dest->ticks_track_lock);

    ptr += utilUnpackUINT16(ptr, &dest->km);
    ptr += utilUnpackUINT16(ptr, &dest->state);

    return (int) (ptr - start);
}

static int DecodeGSV(UINT8 *start, QDP_STAT_GSV *dest)
{
UINT16 i;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += 2; /* skip over length */
    if ((dest->satl = (QDP_STAT_SATL *) malloc(dest->nentry * sizeof(QDP_STAT_SATL))) == NULL) {
        ptr += dest->nentry * 8;
        dest->nentry = 0;
    } else {
        for (i = 0; i < dest->nentry; i++) {
            ptr += utilUnpackUINT16(ptr, &dest->satl[i].num);
            ptr += utilUnpackUINT16(ptr, &dest->satl[i].elevation);
            ptr += utilUnpackUINT16(ptr, &dest->satl[i].azimuth);
            ptr += utilUnpackUINT16(ptr, &dest->satl[i].snr);
        }
    }

    return (int) (ptr - start);
}

static int DecodeARP(UINT8 *start, QDP_STAT_ARP *dest)
{
UINT16 i, j;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT16(ptr, &dest->nentry);
    ptr += 2; /* skip over length */
    if ((dest->addr = (QDP_STAT_ARPADDR *) malloc(dest->nentry * sizeof(QDP_STAT_ARPADDR))) == NULL) {
        ptr += dest->nentry * 12;
        dest->nentry = 0;
    } else {
        for (i = 0; i < dest->nentry; i++) {
            ptr += utilUnpackUINT32(ptr, &dest->addr[i].ip);
            for (j = 0; j < 6; j++) dest->addr[i].mac[j] = *ptr++;
            ptr += utilUnpackUINT16(ptr, &dest->addr[i].timeout);
        }
    }

    return (int) (ptr - start);
}

static int DecodeDPORT(UINT8 *start, QDP_STAT_DPORT *dest)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->sent);
    ptr += utilUnpackUINT32(ptr, &dest->resends);
    ptr += utilUnpackUINT32(ptr, &dest->fill);
    ptr += utilUnpackUINT32(ptr, &dest->seq);
    ptr += utilUnpackUINT32(ptr, &dest->pack_used);
    ptr += utilUnpackUINT32(ptr, &dest->last_ack);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += utilUnpackUINT16(ptr, &dest->log_num);
    ptr += utilUnpackUINT16(ptr, &dest->retran);
    ptr += utilUnpackUINT16(ptr, &dest->flags);

    return (int) (ptr - start);
}

static int DecodeSPORT(UINT8 *start, QDP_STAT_SPORT *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->check);
    ptr += utilUnpackUINT32(ptr, &dest->ioerrs);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += 2; /* skip over spare */

    ptr += utilUnpackUINT32(ptr, &dest->unreach);
    ptr += utilUnpackUINT32(ptr, &dest->quench);
    ptr += utilUnpackUINT32(ptr, &dest->echo);
    ptr += utilUnpackUINT32(ptr, &dest->redirect);
    ptr += utilUnpackUINT32(ptr, &dest->over);
    ptr += utilUnpackUINT32(ptr, &dest->frame);

    return (int) (ptr - start);
}

static int DecodeETH(UINT8 *start, QDP_STAT_ETH *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->check);
    ptr += utilUnpackUINT32(ptr, &dest->ioerrs);

    ptr += utilUnpackUINT16(ptr, &dest->phy_num);
    ptr += 2; /* skip over spare */

    ptr += utilUnpackUINT32(ptr, &dest->unreach);
    ptr += utilUnpackUINT32(ptr, &dest->quench);
    ptr += utilUnpackUINT32(ptr, &dest->echo);
    ptr += utilUnpackUINT32(ptr, &dest->redirect);
    ptr += utilUnpackUINT32(ptr, &dest->runt);
    ptr += utilUnpackUINT32(ptr, &dest->crc_err);
    ptr += utilUnpackUINT32(ptr, &dest->bcast);
    ptr += utilUnpackUINT32(ptr, &dest->ucast);
    ptr += utilUnpackUINT32(ptr, &dest->good);
    ptr += utilUnpackUINT32(ptr, &dest->jabber);
    ptr += utilUnpackUINT32(ptr, &dest->outwin);
    ptr += utilUnpackUINT32(ptr, &dest->txok);
    ptr += utilUnpackUINT32(ptr, &dest->miss);
    ptr += utilUnpackUINT32(ptr, &dest->collide);

    ptr += utilUnpackUINT16(ptr, &dest->linkstat);
    ptr += 2; /* skip over spare */
    ptr += 4; /* skip over spare */

    return (int) (ptr - start);
}

static int DecodeBALER(UINT8 *start, QDP_STAT_BALER *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->last_on);
    ptr += utilUnpackUINT32(ptr, &dest->powerups);

    ptr += utilUnpackUINT16(ptr, &dest->baler_status);
    ptr += utilUnpackUINT16(ptr, &dest->baler_time);

    return (int) (ptr - start);
}

static int DecodeDYN(UINT8 *start, QDP_STAT_DYN *dest)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackUINT32(ptr, &dest->serial[0]);
    ptr += utilUnpackUINT32(ptr, &dest->serial[1]);
    ptr += 4; /* skip over spare */
    ptr += utilUnpackUINT32(ptr, &dest->ethernet);

    return (int) (ptr - start);
}

static int DecodeAUX(UINT8 *start, QDP_STAT_AUX *dest)
{
UINT8 *ptr;

    ptr = start;
// UNSUPPORTED
    return (int) (ptr - start);
}

static int DecodeSS(UINT8 *start, QDP_STAT_SS *dest)
{
UINT8 *ptr;

    ptr = start;
// UNSUPPORTED
    return (int) (ptr - start);
}

/* Decode a QDP C1_STAT payload into a QDP_TYPE_C1_STAT structure */

BOOL qdpDecode_C1_STAT(UINT8 *start, QDP_TYPE_C1_STAT *dest)
{
UINT8 *ptr;

    if (start == NULL || dest == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    if (!qdpClear_C1_STAT(dest)) return FALSE;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &dest->bitmap);
    if (dest->bitmap & QDP_SRB_TOKEN) dest->alert.tokens = TRUE;
    if (dest->bitmap & QDP_SRB_LCHG)  dest->alert.port = TRUE;
    if (dest->bitmap & QDP_SRB_UMSG)  ptr += DecodeUMSG(ptr, dest);
    if (dest->bitmap & QDP_SRB_GBL)   ptr += DecodeGBL(ptr,  &dest->gbl);
    if (dest->bitmap & QDP_SRB_GPS)   ptr += DecodeGPS(ptr,  &dest->gps);
    if (dest->bitmap & QDP_SRB_PWR)   ptr += DecodePWR(ptr,  &dest->pwr);
    if (dest->bitmap & QDP_SRB_BOOM)  ptr += DecodeBOOM(ptr, &dest->boom);
    if (dest->bitmap & QDP_SRB_THR)   ptr += DecodeTHR(ptr,  &dest->thr);
    if (dest->bitmap & QDP_SRB_PLL)   ptr += DecodePLL(ptr,  &dest->pll);
    if (dest->bitmap & QDP_SRB_GSV)   ptr += DecodeGSV(ptr,  &dest->gsv);
    if (dest->bitmap & QDP_SRB_ARP)   ptr += DecodeARP(ptr,  &dest->arp);
    if (dest->bitmap & QDP_SRB_DP1)   ptr += DecodeDPORT(ptr, &dest->dport[0]);
    if (dest->bitmap & QDP_SRB_DP2)   ptr += DecodeDPORT(ptr, &dest->dport[1]);
    if (dest->bitmap & QDP_SRB_DP3)   ptr += DecodeDPORT(ptr, &dest->dport[2]);
    if (dest->bitmap & QDP_SRB_DP4)   ptr += DecodeDPORT(ptr, &dest->dport[3]);
    if (dest->bitmap & QDP_SRB_SP1)   ptr += DecodeSPORT(ptr, &dest->sport[0]);
    if (dest->bitmap & QDP_SRB_SP2)   ptr += DecodeSPORT(ptr, &dest->sport[1]);
    if (dest->bitmap & QDP_SRB_SP3)   ptr += DecodeSPORT(ptr, &dest->sport[2]);
    if (dest->bitmap & QDP_SRB_ETH)   ptr += DecodeETH(ptr, &dest->eth);
    if (dest->bitmap & QDP_SRB_BALER) {
        ptr += DecodeBALER(ptr, &dest->baler.serial[0]);
        ptr += DecodeBALER(ptr, &dest->baler.serial[1]);
        ptr += DecodeBALER(ptr, &dest->baler.special);
        ptr += DecodeBALER(ptr, &dest->baler.ethernet);
    }
    if (dest->bitmap & QDP_SRB_DYN)   ptr += DecodeDYN(ptr, &dest->dyn);
    if (dest->bitmap & QDP_SRB_AUX)   ptr += DecodeAUX(ptr, &dest->aux);
    if (dest->bitmap & QDP_SRB_SS)    ptr += DecodeSS(ptr, &dest->ss[0]);
    if (dest->bitmap & QDP_SRB_SS)    ptr += DecodeSS(ptr, &dest->ss[1]);

    return TRUE;
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: status.c,v $
 * Revision 1.5  2009/02/03 23:05:10  dechavez
 * fixed up QDP_BOOM report
 *
 * Revision 1.4  2008/10/02 22:45:44  dechavez
 * removed unused dlen parameter from qdpDecode_C1_STAT, moved GSV report to under GPS in qdpPrint_C1_STAT
 *
 * Revision 1.3  2007/12/21 18:57:02  dechavez
 * fixed off by one in serial port status header
 *
 * Revision 1.2  2007/12/20 23:06:59  dechavez
 * fixed numerous typos
 *
 * Revision 1.1  2007/12/20 22:47:59  dechavez
 * created
 *
 */
