#pragma ident "$Id: reorder.c,v 1.4 2006/11/10 06:17:59 dechavez Exp $"
/*======================================================================
 *
 *  Reorder TS data
 *
 *====================================================================*/
#include "ida10.h"

VOID ida10ReorderData(IDA10_TS *ts)
{
    switch (ts->hdr.datatype) {

      case IDA10_DATA_INT8:
        break;

      case IDA10_DATA_INT16:
        utilSwapINT16(ts->data.int16, ts->hdr.nsamp);
        break;

      case IDA10_DATA_INT32:
        utilSwapINT32(ts->data.int32, ts->hdr.nsamp);
        break;

      case IDA10_DATA_REAL32:
      case IDA10_DATA_REAL64:
        break;
    }
}

/* Revision History
 *
 * $Log: reorder.c,v $
 * Revision 1.4  2006/11/10 06:17:59  dechavez
 * added REAL64 support
 *
 * Revision 1.3  2006/08/15 00:50:53  dechavez
 * removed premature 64 bit data type support
 *
 * Revision 1.2  2005/03/23 21:27:50  dechavez
 * added (untested) support for 64-bit data types
 *
 * Revision 1.1  2003/10/16 16:52:02  dechavez
 * initial release
 *
 */
