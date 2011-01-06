#pragma ident "$Id: decode.c,v 1.11 2007/01/25 20:28:23 dechavez Exp $"
/*======================================================================
 *
 *  Decode a packet to see if it is (probably) OK.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"

extern ISP_PARAMS *Params;
extern struct isp_status *Status;

static int decodeMK8(UINT8 *packet, int *type, union runion *record, long *tstamp, int *tqual)
{
static int prev_tqual = -12345;

    switch (ida10Type(packet)) {
      case IDA10_TYPE_TS:
        *type = IDA_DATA;
        if (ida10UnpackTS(packet, &record->ts)) {
            *tstamp = (long) record->ts.hdr.cmn.end;
            *tqual  = ida10ClockQuality(&record->ts.hdr.cmn.ttag.beg);
            return 0;
        } else {
            return -1;
        }
      case IDA10_TYPE_LM:
        *type = IDA_LOG;
        if (ida10UnpackLM(packet, &record->lm)) {
            *tstamp = (long) record->lm.cmn.end;
            *tqual  = ida10ClockQuality(&record->lm.cmn.ttag.beg);
            return 0;
        } else {
            return -1;
        }
      case IDA10_TYPE_ISPLOG:
        *type = IDA_ISPLOG;
        return 0;
      default:
        *type = IDA_UNKNOWN;
        util_log(1, "Unknown packet from SAN (%02x %02x %02x %02x %02x %02x %02x %02x)",
            packet[0], packet[1], packet[2], packet[3],
            packet[4], packet[5], packet[6], packet[7]
        );
        return 0;
    }
}

static int decodeMK7(UINT8 *packet, int *type, union runion *record, long *tstamp, int *tqual)
{
int i, status;
static int prev_tqual = -12345;

/* Figure out packet type */

    *type = ida_rtype(packet, Params->ida->rev.value);

/* Patch headers as required */

    if (*type != IDA_DASSTAT && *type != IDA_ISPLOG  && packet[0] != 'A') patchMK7(packet);

/* Apply type specific decoder */

    switch(*type) {

      case IDA_DATA:
        status = ida_dhead(Params->ida, &record->d, packet);
        if (status == 0) {
            *tstamp = (time_t) record->d.end.tru;
            *tqual  = (int)    record->d.end.qual;
            if (packet[0] != 'A' && *tqual != prev_tqual) {
                if (*tqual == 1) {
                    clear_alarm(ISP_ALARM_DASCLK);
                } else if (prev_tqual == 1) {
                    set_alarm(ISP_ALARM_DASCLK);
                }
            }
            prev_tqual = *tqual;
        }
        if (record->d.dl_stream >= 0 && record->d.dl_stream < DAS_MAXSTREAM) {
            MUTEX_LOCK(&Status->lock);    
                Status->last.stream[record->d.dl_stream] = time(NULL);
            MUTEX_UNLOCK(&Status->lock);    
        }
        return status;

      case IDA_IDENT:
        return ida_idrec(Params->ida, &record->i, packet);

      case IDA_CONFIG:
        return ida_cnfrec(Params->ida, &record->k, packet);

      case IDA_CALIB:
        return ida_crec(Params->ida, &record->c, packet);

      case IDA_LOG:
        return ida_logrec(Params->ida, &record->l, packet);

      case IDA_EVENT:
        return ida_evtrec(Params->ida, &record->e, packet);

      case IDA_DASSTAT:
        return ida_srec(Params->ida, &record->s, packet);

      case IDA_ISPLOG:
        return 0;

      default:
        util_log(1, "Unknown packet from DAS (0x%08x)", *((long *) packet));
        if (packet[0] != ~packet[1]) {
            util_log(1, "WARNING! corrupt packet sent to MK7 decoder!");
            return -1;
        }
        return 0;
    }
}

int decode(char *packet, int *type, union runion *record, long *tstamp, int *tqual)
{
    if (Params->digitizer == ISP_SAN) {
        return decodeMK8((UINT8 *) packet, type, record, tstamp, tqual);
    } else {
        return decodeMK7((UINT8 *) packet, type, record, tstamp, tqual);
    }
}

/* Revision History
 *
 * $Log: decode.c,v $
 * Revision 1.11  2007/01/25 20:28:23  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.10  2006/11/13 20:02:08  dechavez
 * use ida10ClockQuality() instead of hardecoded tqual values
 *
 * Revision 1.9  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.8  2004/06/21 20:13:14  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.7  2002/09/09 21:59:45  dec
 * dump first 8 bytes of unknown SAN records
 *
 * Revision 1.6  2002/03/15 22:51:37  dec
 * support for variable length IDA10.x records added
 *
 * Revision 1.5  2001/05/20 17:43:46  dec
 * Release 2.2 (switch to platform.h MUTEX, SEMAPHORE, THREAD macros)
 *
 * Revision 1.4  2000/11/02 20:25:21  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.3  2000/10/19 22:26:02  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
