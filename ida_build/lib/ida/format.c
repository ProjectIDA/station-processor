#pragma ident "$Id: format.c,v 1.2 2007/01/25 20:26:11 dechavez Exp $"
/*======================================================================
 *
 *  Given a raw record, determine the packet format
 *
 *====================================================================*/
#include "ida.h"
#include "ida10.h"

int idaPacketFormat(UINT8 *buf)
{
#define IDA_REV9_TAG_OFFSET 2
static char *ida9_0 = IDA_REV9_TAG_0;
static char *ida9_X = IDA_REV9_TAG_X;

/* rev 10 test */

    switch (ida10Type(buf)) {
      case IDA10_TYPE_TS:
      case IDA10_TYPE_CA:
      case IDA10_TYPE_CF:
      case IDA10_TYPE_LM:
        return 10;
    }

/* rev 9 test */

    if (memcmp(buf + IDA_REV9_TAG_OFFSET, (void *) ida9_0, strlen(ida9_0)) == 0) return 9;
    if (memcmp(buf + IDA_REV9_TAG_OFFSET, (void *) ida9_X, strlen(ida9_X)) == 0) return 9;

/* rev 8 test (WARNING: will also be true for rev 5-7, but they are not deployed anymore) */

    switch (ida_rtype(buf, 8)) {
      case IDA_DATA:
      case IDA_CALIB:
      case IDA_EVENT:
      case IDA_IDENT:
      case IDA_LOG:
        return 8;
    }

    return 0;
}

/* Revision History
 *
 * $Log: format.c,v $
 * Revision 1.2  2007/01/25 20:26:11  dechavez
 * IDA9.x (aka RT593) support
 *
 * Revision 1.1  2005/09/06 18:26:01  dechavez
 * initial release
 *
 */
