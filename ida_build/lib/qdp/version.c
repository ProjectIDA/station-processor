#pragma ident "$Id: version.c,v 1.33 2009/03/17 18:23:59 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "qdp.h"

static VERSION version = {1, 5, 2};
char *libqdpBuildIdent = "libqdp release 1.5.2, compiled " __DATE__ " " __TIME__;

/* qdp library release notes

1.5.2   03/17/2009
        fsa.c: THREAD_DETACH all threads
        version.c: added libqdpBuildIdent

1.5.1   02/23/2009
        encode.c: added qdpEncode_C1_SPP(), qdpEncode_C2_GPS()
        print.c: print all fields int qdpPrint_C2_GPS() and added new funcs
            qdpPrint_CMNHDR(), qdpPrintLCQ(), qdpPrintPkt()

1.5.0   02/03/2009
        decode.c: added qdpDecode_C2_GPS(), qdpDecode_C1_DCP(), qdpDecode_C1_MAN(),
            qdpDecode_C1_SPP(), qdpDecode_C1_GID()
        pkts.c: added qdpRequestCheckout(), qdp_C1_RQSTAT(), qdp_C2_RQGPS(),
            qdp_C1_RQMAN, qdp_C1_RQSPP(), qdp_C1_RQDCP(), qdp_C1_RQGID(),
            qdp_C1_RQFIX(), qdp_C1_RQPHY(), qdp_C1_RQSC(), qdp_C1_RQGLOB()
        print.c: added qdpPrintCheckoutPackets(), qdpPrint_C2_GPS(), qdpPrint_C1_MAN(),
            qdpPrint_C1_SPP(), qdpPrint_C1_DCPMAN(), qdpPrint_C1_DCP(), qdpPrint_C1_GID()
            and added underlines to section headers
        process.c: log ProcessBlockette() error returns
        status.c: fixed up QDP_BOOM report
        string.c: added qdpAuxtypeString(), qdpClocktypeString(), qdpCalibratorString()

1.4.0   01/23/2009
        decode.c: fixed typo with qpdDecode_C1_FIX() name, added
            qdpDecode_C1_SC(), qdpDecode_C2_AMASS()
        encode.c: added qdpEncode_C1_PULSE(), qdpEncode_C1_SC(), qdpEncode_C1_GLOB()
        print.c: added qdpPrint_C1_SC(), qdpPrint_C2_AMASS_SENSOR(), qdpPrint_C2_AMASS()
        string.c: added qdpSensorControlString(), fixed C1_GLOB typo

1.3.2   01/06/2009
        encode.c: added qdpEncode_C1_PHY()

1.3.1   12/09/2008
        reorder.c: made "out of window" messages debug comments instead of warnings

1.3.0   10/02/2008.3.0   10/02/2008
        cmds.c: added qdpNoParCmd, qdpRqstat
        decode.c: added qdpDecode_C1_PHY
        encode.c: added qdpEncode_NoParCmd
        init.c: used QDP_STATUS_DATA_PORT_x instead of QDP_LOGICAL_PORT_x_STATUS
        print.c: added qdpPrint_C1_PHY
        process.c: removed unused dlen parameter from call to qdpDecode_C1_STAT
        status.c: removed unused dlen parameter from qdpDecode_C1_STAT,
            moved GSV report to under GPS in qdpPrint_C1_STAT.

1.2.0   03/05/2008
        process.c: added support for CNP316 (aux packets)

1.1.1   01/07/2008 *** First version tested on little-endian platform ***
        data.c: little-endian bug fixes
        ida10.c: little-endian bug fixes
        print.c: adding missing argument to detect[] string in qdpPrintTokenLcq
        steim.c: #ifdef'd out some dead code

1.1.0   12/20/2007
        qdp/limits.h: created
        qdp/status.h: created
        qdp.h: moved various limits to new qdp/limits.h, added support for
               C1_STAT, defined physical ports
        proocess.c: added support for C1_STAT
        status.c: created
        string.c: added qdpPLLStateString(), qdpPhysicalPortString()

1.0.5   12/14/2007
        qdp/lcq.h: changed blockette datum types from unsigned to signed

1.0.4   10/31/2007
        encode.c: added qdpEncode_C1_QCAL()
        init.c: replaced sprintf with snprintf
        md5.c: replaced sprintf with snprintf
        stats.c: added buffer length to utilTimeString() calls

1.0.3   09/06/2007
        init.c: include port number in udpioInit failure messages

1.0.2   06/26/2007
        steim.c: fixed dnib 01 (4 8-bit differences) bug

1.0.1   06/14/2007
        hlp.c: flush partial packets
        ida10.c: generate fixed-length 1024 byte packets

1.0.0   05/18/2007
        Initial production release
  
 */

char *qdpVersionString()
{
static char string[] = "qdp library version 100.100.100 and slop";

    snprintf(string, strlen(string), "qdp library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *qdpVersion()
{
    return &version;
}
