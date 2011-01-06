#pragma ident "$Id: string.c,v 1.15 2009/02/03 22:50:17 dechavez Exp $"
/*======================================================================
 *
 *  String conversions
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "ida10.h"

typedef struct {
    int code;
    char *text;
} MESSAGE_MAP;

char *ida10ErrorString(int code)
{
static MESSAGE_MAP map[] = {
    {IDA10_OK,       "OK"},
    {IDA10_EINVAL,   "EINVAL"},
    {IDA10_ESRATE,   "ESRATE"},
    {IDA10_EMSGSIZE, "EMSGSIZE"},
    {IDA10_IOERR,    "IOERR"},
    {IDA10_DATAERR,  "DATAERR"},
    {IDA10_EOF,      "EOF"},
    {-1,  NULL}
};
int i;
static char defaultMessage[] = "unknown error";

    for (i = 0; map[i].text != NULL; i++) {
        if (map[i].code == code) return map[i].text;
    }

    return defaultMessage;
}

char *ida10DataTypeString(IDA10_TSHDR *hdr)
{
#ifdef BIG_ENDIAN_HOST
static char *int1String  = "s1";
static char *int2String  = "s2";
static char *int4String  = "s4";
static char *int8String  = "s8";
static char *real4String = "t4";
static char *real8String = "t8";
#else
static char *int1String  = "i1";
static char *int2String  = "i2";
static char *int4String  = "i4";
static char *int8String  = "i8";
static char *real4String = "f4";
static char *real8String = "f8";
#endif /* !BIG_ENDIAN_HOST */
static char *unknown = "??";

    switch (hdr->datatype) {

      case IDA10_DATA_INT8:   return int1String;
      case IDA10_DATA_INT16:  return int2String;
      case IDA10_DATA_INT32:  return int4String;
      case IDA10_DATA_REAL32: return real4String;
      case IDA10_DATA_REAL64: return real8String;
      default:
        return unknown;
    }
}
        
char *ida10CMNHDRtoString(IDA10_CMNHDR *hdr, char *msgbuf)
{
char tbuf[64];
static char mt_unsafe_buffer[1024];

    if (msgbuf == NULL) msgbuf = mt_unsafe_buffer;

    switch (hdr->type) {
      case IDA10_TYPE_TS: sprintf(msgbuf, "TS"); break;
      case IDA10_TYPE_CA: sprintf(msgbuf, "CA"); break;
      case IDA10_TYPE_CF: sprintf(msgbuf, "CF"); break;
      case IDA10_TYPE_LM: sprintf(msgbuf, "LM"); break;
      case IDA10_TYPE_ISPLOG: sprintf(msgbuf, "IL"); break;
      default: sprintf(msgbuf, "??"); break;
    }

    sprintf(msgbuf + strlen(msgbuf), " %d.%d", hdr->format, hdr->subformat);

    if (hdr->boxid == IDA10_64BIT_BOXID) {
        sprintf(msgbuf + strlen(msgbuf), " %015llX", hdr->serialno);
    } else {
        sprintf(msgbuf + strlen(msgbuf), " %04X", hdr->boxid);
    }
    sprintf(msgbuf + strlen(msgbuf), " %s", utilDttostr(hdr->beg, 0, tbuf));
    sprintf(msgbuf + strlen(msgbuf), " %4d", hdr->nbytes);

    if (hdr->extra.valid) {
        sprintf(msgbuf + strlen(msgbuf), " 0x%08x", hdr->extra.seqno);
        if (hdr->extra.tstamp != 0) sprintf(msgbuf + strlen(msgbuf), " %s", utilLttostr(hdr->extra.tstamp, 1000, tbuf));
    }

    return msgbuf;
}

static void PrintStatusFlag(char *string, IDA10_CLOCK_STATUS *status)
{
    if (!status->init) {
        sprintf(string, "U");
    } else if (status->suspect) {
        sprintf(string, "?");
    } else if (!status->avail) {
        sprintf(string, "*");
    } else if (!status->locked) {
        sprintf(string, "-");
    } else {
        sprintf(string, " ");
    }
}

char *ida10TSHDRtoString(IDA10_TSHDR *hdr, char *msgbuf)
{
char tbuf[64];
static char mt_unsafe_buffer[1024];

    if (msgbuf == NULL) msgbuf = mt_unsafe_buffer;

    sprintf(msgbuf, "%s", hdr->sname);
    sprintf(msgbuf + strlen(msgbuf), " %-6s", hdr->cname);
    sprintf(msgbuf + strlen(msgbuf), " %s", utilDttostr(hdr->tofs, 0, tbuf));
    PrintStatusFlag(msgbuf + strlen(msgbuf), &hdr->cmn.ttag.beg.status);
    sprintf(msgbuf + strlen(msgbuf), " %s", utilDttostr(hdr->tols, 0, tbuf));
    PrintStatusFlag(msgbuf + strlen(msgbuf), &hdr->cmn.ttag.beg.status);
    sprintf(msgbuf + strlen(msgbuf), " %6.3lf", hdr->sint);
    sprintf(msgbuf + strlen(msgbuf), " %3d", hdr->gain);
    sprintf(msgbuf + strlen(msgbuf), " %3d", hdr->nsamp);
    sprintf(msgbuf + strlen(msgbuf), " %4d", hdr->unused);
    sprintf(msgbuf + strlen(msgbuf), "%c", hdr->unused ? '*' : ' ');
    if (hdr->cmn.extra.valid) {
        sprintf(msgbuf + strlen(msgbuf), " 0x%08x", hdr->cmn.extra.seqno);
        sprintf(msgbuf + strlen(msgbuf), " %s", utilLttostr(hdr->cmn.extra.tstamp, 1000, tbuf));
    }

    return msgbuf;
}

char *ida10TStoString(IDA10_TS *ts, char *msgbuf)
{
int i;
INT32 lmin, lmax;
REAL32 rmin, rmax;
REAL64 dmin, dmax;
static char mt_unsafe_buffer[1024];

    if (msgbuf == NULL) msgbuf = mt_unsafe_buffer;

    ida10TSHDRtoString(&ts->hdr, msgbuf);
    switch (ts->hdr.datatype) {

      case IDA10_DATA_INT8:
        lmin = lmax = ts->data.int8[0];
        for (i = 1; i < ts->hdr.nsamp; i++) {
            if (ts->data.int8[i] < lmin) lmin = ts->data.int8[i];
            if (ts->data.int8[i] > lmax) lmax = ts->data.int8[i];
        }
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmin);
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmax);
        break;

      case IDA10_DATA_INT16:
        lmin = lmax = ts->data.int16[0];
        for (i = 1; i < ts->hdr.nsamp; i++) {
            if (ts->data.int16[i] < lmin) lmin = ts->data.int16[i];
            if (ts->data.int16[i] > lmax) lmax = ts->data.int16[i];
        }
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmin);
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmax);
        break;

      case IDA10_DATA_INT32:
        lmin = lmax = ts->data.int32[0];
        for (i = 1; i < ts->hdr.nsamp; i++) {
            if (ts->data.int32[i] < lmin) lmin = ts->data.int32[i];
            if (ts->data.int32[i] > lmax) lmax = ts->data.int32[i];
        }
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmin);
        sprintf(msgbuf + strlen(msgbuf), " %12d", lmax);
        break;

      case IDA10_DATA_REAL32:
        rmin = rmax = ts->data.real32[0];
        for (i = 1; i < ts->hdr.nsamp; i++) {
            if (ts->data.real32[i] < rmin) rmin = ts->data.real32[i];
            if (ts->data.real32[i] > rmax) rmax = ts->data.real32[i];
        }
        sprintf(msgbuf + strlen(msgbuf), " %12.5e", rmin);
        sprintf(msgbuf + strlen(msgbuf), " %12.5e", rmax);
        break;

      case IDA10_DATA_REAL64:
        dmin = dmax = ts->data.real64[0];
        for (i = 1; i < ts->hdr.nsamp; i++) {
            if (ts->data.real64[i] < dmin) dmin = ts->data.real64[i];
            if (ts->data.real64[i] > dmax) dmax = ts->data.real64[i];
        }
        sprintf(msgbuf + strlen(msgbuf), " %12.5e", dmin);
        sprintf(msgbuf + strlen(msgbuf), " %12.5e", dmax);
        break;

      default:
        sprintf(msgbuf + strlen(msgbuf), " ????????????");
        sprintf(msgbuf + strlen(msgbuf), " ????????????");
    }

    return msgbuf;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.15  2009/02/03 22:50:17  dechavez
 * added alert characters for suspect time PrintStatusFlag() and for unused bytes in ida10TSHDRtoString()
 *
 * Revision 1.14  2007/04/18 23:04:06  dechavez
 * use static buffer if output not supplied in calls to ida10CMNHDRtoString(),
 * ida10TSHDRtoString(), ida10TStoString()
 *
 * Revision 1.13  2006/12/22 02:46:59  dechavez
 * initial release
 *
 * Revision 1.12  2006/11/10 06:17:15  dechavez
 * added REAL64 support, generic clock status
 *
 * Revision 1.11  2006/08/15 00:53:09  dechavez
 * removed premature 64 bit data type support, added OFIS ttag support
 *
 * Revision 1.10  2005/10/06 19:37:11  dechavez
 * include format.subformat in ida10CMNHDRtoString()
 *
 * Revision 1.9  2005/08/26 18:41:44  dechavez
 * added 10.2 support
 *
 * Revision 1.8  2005/03/23 21:27:13  dechavez
 * added (untested) support for 64-bit data types
 *
 * Revision 1.7  2004/12/10 17:44:28  dechavez
 * added ida10TtagToString(), ida10ClockStatusString()
 *
 * Revision 1.6  2004/06/24 18:18:40  dechavez
 * removed unnecessary includes (aap)
 *
 * Revision 1.5  2003/10/16 16:50:59  dechavez
 * added ida10DataTypeString(), and type specific max/min in ida10TStoString()
 *
 * Revision 1.4  2002/11/06 00:52:34  dechavez
 * added unused bytes to TS string
 *
 * Revision 1.3  2002/11/05 17:46:30  dechavez
 * removed some tabs
 *
 * Revision 1.2  2002/04/29 17:28:22  dec
 * added ida10TSHDRtoString() and ida10TStoString()
 *
 * Revision 1.1  2002/03/15 22:41:20  dec
 * created
 *
 */
