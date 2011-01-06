#pragma ident "$Id: version.c,v 1.59 2009/05/14 16:33:53 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "isi.h"

static VERSION version = {2, 7, 0};

/* isi library release notes

2.7.0  05/14/2009
       fileio.c: initial release
       string.c: added support for NULL (mt-unsafe) text buffers in isiTstampsString and isiGenericTsHdrString

2.6.1  02/03/2008
       utils.c: ensure isiRawPacketStreamName() returns NULL for non-data packets

2.6.0  01/25/2008
       datreq.c: added support for ISI_IACP_REQ_STREAM and ISI_IACP_REQ_OPTIONS
       pack.c: build chnloc field in isiUnpackStreamName()
       string.c: removed tabs
       utils.c: added isiRawPacketStreamName()

2.5.4  06/31/2007
       seqno.c: replaced taps with spaces
       string.c: replaced string memcpy with strlcpy
       utils.c: replaced string memcpy with strlcpy, sprintf with snprintf
       wfdisc.c: replaced string memcpy with strlcpy

2.5.3  04/18/2007
       seqno.c: added isiWriteSeqno() and isiReadSeqno()

2.5.2  02/09/2007
       wfdisc.c: isiIacpRecvWfdiscList() to ignore (instead of fail) wfdisc
       records that are the wrong length

2.5.1  01/22/2007
       cnf.c: changed LOG_ERR messages to LOG_INFO
       datreq.c: changed LOG_ERR messages to LOG_INFO
       ezio.c: changed LOG_ERR messages to LOG_INFO
       soh.c: changed LOG_ERR messages to LOG_INFO
       ts.c: changed LOG_ERR messages to LOG_INFO
       wfdi.c: changed LOG_ERR messages to LOG_INFO

2.5.0   01/11/2007
        datreq.c: renamed all the "stream" requests to the more accurate "twind" (time window)
        open.c: renamed all the "stream" requests to the more accurate "twind" (time window)
        pack.c: renamed all the "stream" requests to the more accurate "twind" (time window)
        string.c: renamed all the "stream" requests to the more accurate "twind" (time window)
        ts.c: renamed all the "stream" requests to the more accurate "twind" (time window)

2.4.9   01/07/2007
        datreq.c: fixed "free same pointer twice" bug
        log.c: switch to size-bounded string operations
        seqno.c: switch to size-bounded string operations
        uils.c: switch to size-bounded string operations
        version.c: switch to size-bounded string operations

2.4.8   12/13/2006
        string.c: add optional internal buffer to isiSeqnoString for
                  non-MT use, changed formatting of non-numeric strings

2.4.7   12/12/2006
        seqno.c: use new macros for abstract sequence numbers

2.4.6   11/08/2006
        string.c: REAL64 support

2.4.5   09/29/2006
        read.c: added test for EBADMSG

2.4.4   08/14/2006
        soh.c: treat nrec and nseg as signed ints in isiStreamSohString

2.4.3   07/07/2006
        string.c: fixed bug in isiSeqnoString with 3-part seqno's under Linux

2.4.2   06/26/2006
        datreq.c: removed unreferenced local variables
        ezio.c: removed unreferenced local variables, added missing return value to isiSetLog
        read.c: check for and return ISI_TIMEDOUT and ISI_CONNRESET conditions
        seqno.c: include util.h for prototypes

2.4.1   06/20/2006
        datreq.h: fixed multi-free bug in isiInitDataRequest (stimulated when called
            by isiClearDataRequest)

2.4.0   06/19/2006
        isid.h: define ISI_TYPE_MSEED
        string.c: support ISI_TYPE_IDA[567] and ISI_TYPE_MSEED
        ts.c: support ISI_TYPE_IDA[567]

2.3.5   06/16/2006
        open.c: calloc() handle

2.3.4   06/12/2006
        datreq.c: calloc() datreq in AllocSimpleXRequest(), so that 
            isiInitDataRequest() can detect and free any stream/seqno pointer
            in the req field.
        string.c: changed isiSeqnoString's undef to "undefined seqno" so
            it could be recognized as a sequence number
        ts.c: fixed unitialized new->type on reconnect in UpdateDataRequest (aap)

2.3.3   06/07/2006
        string.c: made static TypeString public isiDataTypeString()

2.3.2   06/02/2006
        string.c: added ISI_TYPE_QDPLUS support to TypeString()

2.3.1   03/13/2006
        seqno.c: added support for "tee name" splitting, fixed some counter casts
        string.c: changed format of seqno string

2.3.0   10/26/2005
        netutil.c: initial release
        pack.c: Renamed 'oldchn' field of ISI_STREAM_NAME to 'chnloc'
        string.c: Renamed 'oldchn' field of ISI_STREAM_NAME to 'chnloc'
        utils.c: moved iacp functions into newutil.c, oldchn -> chnloc

2.2.4   10/26/2005
        datreq.c: added isiSetDatreqType()

2.2.3   10/11/2005
        isi.h: changed ISI_DATA_REQUEST req field to a struct

2.2.2   10/09/2005
        datreq.c: fixed potential infinite loop in ReceiveDataRequestAcknowledgment)
        read.c: debug tracers removed

2.2.1   09/30/2005
        read.c: debug tracers added

2.2.0   09/10/2005
        raw.c: added isiAllocateRawPacket(), isiDestroyRawPacket(),
               added support for NULL buf pointer for isiInitRawPacket()

2.1.0   08/25/2005
        pack.c: fixed nasty bug in unpacking ISI_RAW_HEADER
        string.c: added ISI_TYPE_IDA9 support
        ts.c: added ISI_TYPE_IDA9 support

2.0.0   07/25/2005
        dl.c: removed (to isidl)
        pack.c: added support for ISI_TAG_RAW_STATUS
        raw.c: added support for ISI_TAG_RAW_STATUS
        search.c: removed (to isidl)
        seqno.c: fixed errors in isiSeqnoLT() and isiSeqnoGT()
        string.c: removed isiDLStateString(), isiIndexString() (to isidl) added isiDatatypeString()

1.10.2  07/06/2005
        dl.c: support for new len field in raw packet header
        pack.c: support for new len field in raw packet header
        raw.c: removed uneeded "precious" stuff, uneeded isiCopyRawPacket()
               and added isiCompressRawPacket() and isiDecompressRawPacket()
        string.c: support for new len field in raw packet header
        
1.10.1  06/29/2005
        datreq.c: checkpoint, debugged seqno requests
        dl.c: clear state when closing disk loop, support indices repair on open
              update activity time stamp when writing to disk loop
        pack.c: added isiPackRawPacketHeaderAndData()
        read.c: debugged isiReadFrame()
        seqno.c: added ISI_CURRENT_SEQNO_SIG support to isiStringToSeqno(),
                 moved isiSeqnoString() to string.c
        string.c: added isiDLStateString(), isiSeqnoString(),
                  isiStreamRequestString(), isiSeqnoRequestString()
        utils.c: fixed isiExpandSeqnoSiteSpecifier()

1.10.0  06/24/2005
        datreq.c: accomodate new design of ISI_DATA_REQUEST structure
        dl.c: mods, additions, subtractions as part of getting isidl to work 
        open.c: accomodate new design of ISI_DATA_REQUEST structure
        pack.c: support for updated (hdr/payload) packets and redesigned ISI_DATA_REQUEST structure
        raw.c: mods, additions, subtractions as part of getting isidl to work
        search.c: initial release (not yet tested)
        seqno.c: mods, additions, subtractions as part of getting isidl to work
        string.c: added isiRequestTypeString(), isiRawHeaderString(),
                  isiRawPacketString(), isiIndexString()
        ts.c: accomodate new design of ISI_DATA_REQUEST structure
        utils.c: added isiSiteNameCompareWild(), isiExpandSeqnoSiteSpecifier()

1.9.0   06/10/2005
        dl.c: added isiDLStateString(), isiLogDL(), isiAssignSeqno(),
              isiReservePacket(), isiReleasePacket(), replaced InitPkt with InitRawPkt(),
              changed the way the raw payloads are allocated from heap, added perm
              argument to isiOpenDiskLoop()

        ezio.c: renamed isiSetLogging to isiStartLogging, added isiSetLog and
                replaced log field in ISI_PARAM with lp
        raw.c: updated comments
        read.c: initial release
        seqno.c: added isiSeqnoString(), isiStringToSeqno(), isiUpdateSeqno()

1.8.1   05/31/2005
        pack.c: fixed oldchn intialization error in isiUnpackStreamName()

1.8.0   05/05/2005 (cvs rtag libisi_1_8_0 libisi)
        dl.c: checkpoint with working locks
        seqno.c: fixed bug printing seqno counters

1.7.0   05/05/2005 (cvs rtag libisi_1_7_0 libisi)
        This is a checkpoint build following introduction of data structures
        to support seqno based requests and raw digitizer packets.
        datreq.c: support for sequence number based requests added
        pack.c: added tagged field and ISI_SEQNO and ISI_RAW_PACKET support
        raw.c: created
        seqno.c: created
        string.c: renamed isiStreamString() to isiStreamNameString() and added
                  isiStringToStreamName()
        ts.c:  added isiInitGenericTSHDR(), use the new concise field names in ISI_GENERIC_TS
        utils.c: added isiGetIacpStats() and isiExpandStreamNameSpecifier()

1.6.0   01/27/2005 (cvs rtag libisi_1_6_0 libisi)
        ezio.c: added isiSetDbgpath()
        string.c: print at least 3 char chan field in isiStreamString()

1.5.2   06/24/2004 (cvs rtag libisi_1_5_2 libisi)
        ezio.c: fixed bug in isiWfdisc error handling
        utils.c: accept both NULL and blank loc in isiStaChnLocToStreamName()

1.5.1   06/21/2004 (cvs rtag libisi_1_5_1 libisi)
        isi.h: defined types for ida rev's 6 and 7, blank loc constant and changed
               isiStaChnToStreamName() macro to use blank loc instead of NULL
        datreq.c: print name components in quotes in isiPrintStreamReq() (helps debugging)
        string.c: recognize blank loc in isiStreamString()
        utils.c: use blank loc in isiStaChnLocToStreamName()
        
1.5.0   06/10/2004 (cvs rtag libisi_1_5_0 libisi)
        isi.h: added flag to ISI handle
        utils.c: added isiSetFlag() and isiGetFlag()
        ts.c: check (new) ISI handle flag and return with status ISI_BREAK if set
        datreq.c: AAP memory leak fix, allow for sta.chn strings in isiBuildStreamName
        open.c: AAP memory leak fix

1.4.8   01/20/2004 (cvs rtag libisi_1_4_8 libisi)
        datreq.c: init SendStreamRequest() buf with 0xee's (debug aid)
        ezio.c: added isiFreeSoh(), isiFreeCnf(), isiFreeWfdisc()
        ts.c: use calloc instead of malloc, tune logging messages

1.4.7   12/11/2003 (cvs rtag libisi_1_4_7 libisi)
        datreq.c: changed isiFreeDataRequest() to free streams via
            isiClearDataRequest() instead of its own explicit free()
        ts.c: removed a memory leak in Reconnect() by freeing the
            streams allocated in UpdateDataRequest(), cleaned up
            isiDecompressGenericTS() decompress switch, calling
            type specific functions.
        *** Still have a memory leak when reconnecting to the server **

1.4.6   12/09/2003 (cvs rtag libisi_1_4_6 libisi)
        datreq.c: added IACP_TYPE_NOP support to isiSendDataRequest()
        ezio.c: cosmetic cleanup
        ts.c: byte swap decompressed native digitizer packet data from host
            into original byte order, if needed
        wfdisc.c: various explicit casts to calm solaris cc

1.4.5   12/04/2003 (cvs rtag libisi_1_4_5 libisi)
        datreq.c: added IACP_TYPE_ALERT support to isiSendDataRequest()

1.4.4   11/26/2003 (cvs rtag libisi_1_4_4 libisi)
        string.c: fixed order test in isiDescString
        ts.c: fixed error in setting byte order in isiDecompressGenericTS()

1.4.3   11/26/2003 (cvs rtag libisi_1_4_3 libisi)
        ezio.c: set logging threshold to LOG_DEBUG when debugging flag is set in isiSetLogging()
        ts.c: toned down error message verbosity

1.4.2   11/25/2003 (cvs rtag libisi_1_4_2 libisi)
        datreq.c: added isiGetDatreqCompress()
        ezio.c: added isiGetServerPort()
        ts.c: added delays after each socket close, not just graceful closures

1.4.1   11/21/2003 (cvs rtag libisi_1_4_1 libisi)
        Built with ISI_SERVER defined
        datreq.c: fixed typo in testing for isiCopyStreamListToDataRequest
            malloc failure, removed some left over debugging printfs
        ts.c: removed inline declarations, fixed byte order for
            decompressed INT32's on little endian systems

1.4.0   11/19/2003 (cvs rtag libisi_1_4_0_bis libisi)
        datreq.c: added return value for EINVAL isiAppendStreamReq call
        ezio.c: added support for decompress parameter, changed isiSoh(),
            isiCnf(), and isiWfdisc() to return report structures
            Fixed log initialization error in isiSetLogging().
        open.c: added decompress, ts1, and ts2 fields to the handle
        pack.c: cast utilPackBytes/utilUnpackBytes args to calm compiler
        soh.c: include util.h to calm compiler
        string.c: include util.h to calm compiler
        ts.c: added isiCreateGenericTS(), isiDestroyGenericTS(), isiInitGenericTS(),
            isiDecompressGenericTS(), isiCopyGenericTS(), and
            changed isiReadGenericTS() to use ISI_GENERIC_TS in the handle
            for producing (possibly) decompressed and native ordered packets,
            added casts for compiler and removed void pointer arithmetic to determine
            offsets for decompressing native data
        utils.c: made isiStaChnToStreamName macro

1.3.0   11/13/2003 (cvs rtag libisi_1_3_0 libisi)
        cnf.c: remove server side (#define ISI_SERVER) items from client side builds
        ezio.c: replaced isiDataRequest() with isiInitiateDataRequest(),
            added isiSoh(), isiCnf() and isiWfdisc()
        soh.c: remove server side (#define ISI_SERVER) items from client side builds
        wfdisc.c: remove server side (#define ISI_SERVER) items from client side builds

1.2.0   11/04/2003 (cvs rtag libisi_1_2_0 libisi)
        string.c: added isiReqStatusString(), changed isiStreamString to
            use sta/chan/loc if oldchn is missing
        ts.c: delay ISI_SHUTDOWN_DELAY msecs following socket close and
            subsequent reconnect attempt, fix error in setting value of
            updated request start time

1.1.0   11/02/2003 (cvs rtag libisi_1_1_0 libisi)
        Main feature is the introduction of the ISI handle and related
        functionality
        cnf.c: added isiRequestCnf()
        datreq.c: added isiSetDatreqPolicy(), isiSetDatreqFormat(),
            isiSetDatreqCompress(), isiSendDataRequest(),
            isiClearDataRequest(), isiLocateStreamRequest(),
            isiFreeDataRequest(), isiAllocSimpleDataRequest(),
            isiAllocDefaultDataRequest()
            Data request's are now acknowledged with server expanded requests
        ezio.c: initial release
        log.c: initial release
        open.c: initial release
        pack.c: added isiUnpackGenericTS()
        soh.c: added isiRequestSoh(), detect ISI_UNDEFINED_TIMESTAMPs in
            isiStreamSohString() and print n/a's instead of time strings
        string.c: removed sample size from isiDescString(), added 
            nbytes to isiGenericTsHdrString()
        ts.c: initial release
        utils.c: added isiInitIncoming() and isiResetIncoming()
        wfdisc.c: added isiRequestWfdisc()

1.0.0   10/16/2003 (cvs rtag libisi_1_0_0 libisi)
        Initial release
  
 */

char *isiVersionString()
{
static char string[] = "isi library version 100.100.100 and slop";

    snprintf(string, strlen(string), "isi library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *isiVersion()
{
    return &version;
}
