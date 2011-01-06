#pragma ident "$Id: rtype.c,v 1.4 2006/02/08 23:09:18 dechavez Exp $"
/*======================================================================
 *
 *  Given a raw record, determine the type and return the corresponding
 *  internal code.
 *
 *====================================================================*/
#include "ida.h"

int ida_rtype(UINT8 *buf, int rev)
{

    if (rev >= 5) {
        if (buf[0] != (UINT8) ~buf[1]) return IDA_CORRUPT;
        switch ((char) *buf) {
          case 'A': return IDA_DATA;     break;
          case 'C': return IDA_CALIB;    break;
          case 'D': return IDA_DATA;     break;
          case 'E': return IDA_EVENT;    break;
          case 'H': return IDA_IDENT;    break;
          case 'I': return IDA_ISPLOG;   break;
          case 'K': return IDA_CONFIG;   break;
          case 'L': return IDA_LOG;      break;
          case 's': return IDA_DASSTAT;  break;
        }
    } else if (rev >= 1 && rev <= 4) {
        switch (*buf) {
          case 'C': return IDA_CALIB;    break;
          case 'D': return IDA_DATA;     break;
          case 'H': return IDA_OLDHDR;   break;
          case 'P': return IDA_POSTHDR;  break;
          case 'R': return IDA_RESERVED; break;
        }
    } else if (strncmp(buf, "tapedmp", strlen("tapedmp")) == 0) {
        return IDA_DMPREC;
    } else if (strncmp(buf, "cartdmp", strlen("cartdmp")) == 0) {
        return IDA_DMPREC;
    } else if (strncmp(buf, "Soviet", strlen("Soviet")) == 0) {
        return IDA_LABEL;
    } else if (strncmp(buf,"IRIS/IDA OME",strlen("IRIS/IDA OME"))==0) {
        return IDA_LABEL;
    } else if (strncmp(buf, "*** END", strlen("*** END")) == 0) {
        return IDA_ARCEOF;
    }

/*  Unrecognized record type  */

    return IDA_UNKNOWN;
}

/* Revision History
 *
 * $Log: rtype.c,v $
 * Revision 1.4  2006/02/08 23:09:18  dechavez
 * portable data types
 *
 * Revision 1.3  2005/05/25 22:37:54  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2004/06/24 18:37:45  dechavez
 * removed unneccesary includes (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:25  dec
 * import existing IDA/NRTS sources
 *
 */
