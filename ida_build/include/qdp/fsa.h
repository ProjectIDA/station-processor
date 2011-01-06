#pragma ident "$Id: fsa.h,v 1.8 2007/05/17 22:27:44 dechavez Exp $"
/*======================================================================
 * 
 * Constants for the QDP Finite State Automaton
 * 
 * Two automatons are defined here.  Command and control sessions
 * (connections on either the configuration or special functions port)
 * use the ControlFSA state machine, while data sessions (connections
 * on one of the 4 logical data ports) use DataFSA.  The reason we
 * need two is that since DT_DATA packets are not useful without
 * the metadata, we extend the registration phase to ensure that all
 * the required configuration information has been obtained before
 * turning on the data stream.   The DataFSA allows for the events
 * needed to get to this higher level state.
 *
 *====================================================================*/
#ifndef qdp_fsa_included
#define qdp_fsa_included

#ifdef __cplusplus
extern "C" {
#endif

/* states */
#define QDP_STATE_IDLE       0 /* initial state */
#define QDP_STATE_RQSRV      1 /* registration request has been sent */
#define QDP_STATE_SRVRSP     2 /* challenge response has been sent */
#define QDP_STATE_REGISTERED 3 /* registered (C&C link ready) */
#define QDP_STATE_TOKEN      4 /* loading DP tokens */
#define QDP_STATE_READY      5 /* data link ready */
#define QDP_STATE_OFF        6 /* automaton is off (failed library) */
#define QDP_NUM_STATE        7
#define QDP_MAX_STATE        (QDP_NUM_STATE - 1)
#define QDP_STATE_NOCHANGE QDP_NUM_STATE

#ifdef INCLUDE_FSA_STRINGS
static char *StateString[QDP_NUM_STATE] = {
    "Idle",
    "Registration Request Sent",
    "Challenge Response Sent",
    "Registered",
    "Loading DP tokens",
    "Data Link Ready",
    "Off"
};
#endif /* INCLUDE_FSA_STRINGS */

/* events */
#define QDP_MIN_EVENT         0
#define QDP_EVENT_UP          0 /* library ready */
#define QDP_EVENT_CTO         1 /* command port timeout */
#define QDP_EVENT_DTO         2 /* data port timeout */
#define QDP_EVENT_HBEAT       3 /* time for status request */
#define QDP_EVENT_CONFIG      4 /* configuration data received */
#define QDP_EVENT_TOKEN_SOME  5 /* DP token packet received */
#define QDP_EVENT_TOKEN_DONE  6 /* final DP token packet received */
#define QDP_EVENT_SRVCH       7 /* server challenge received */
#define QDP_EVENT_TOOMANY     8 /* too many servers */
#define QDP_EVENT_NOTREG      9 /* not registered */
#define QDP_EVENT_CERR       10 /* general C1_ERR received */
#define QDP_EVENT_CACK       11 /* command acknowledgement received */
#define QDP_EVENT_CTRL       12 /* general control port packet received */
#define QDP_EVENT_DATA       13 /* DT_DATA packet received */
#define QDP_EVENT_FILL       14 /* DT_FILL packet received */
#define QDP_EVENT_NOSUP      15 /* unspported packet received */
#define QDP_EVENT_CMDRDY     16 /* command packet from app */
#define QDP_EVENT_CRCERR     17 /* CRC error */
#define QDP_EVENT_IOERR      18 /* I/O error */
#define QDP_NUM_EVENT        19
#define QDP_MAX_EVENT        (QDP_NUM_EVENT - 1)

#ifdef INCLUDE_FSA_STRINGS
static char *EventString[QDP_NUM_EVENT] = {
    "Automaton Started",
    "CTO",
    "DTO",
    "Heartbeat",
    "Configuration Data Received",
    "DP Token Data Received",
    "Final DP Token Data Received",
    "Server Challenge Received",
    "Too Many Servers",
    "Not Currently Registered",
    "C1_CERR Received",
    "C1_CACK Received",
    "Control port packet received",
    "DT_DATA Received",
    "DT_FILL Received",
    "Unexpected Packet Received",
    "Command Received From App",
    "CRC Error",
    "I/O Error"
};
#endif /* INCLUDE_FSA_STRINGS */

/* actions */
#define QDP_MIN_ACTION     0
#define QDP_ACTION_NONE    0 /* event processing complete */
#define QDP_ACTION_SEND    1 /* (re)send automaton command */
#define QDP_ACTION_SRVRQ   2 /* issue server request */
#define QDP_ACTION_BLDRSP  3 /* build server response */
#define QDP_ACTION_TLU     4 /* this layer up */
#define QDP_ACTION_TLD     5 /* this layer down */
#define QDP_ACTION_SLEEP   6 /* sleep */
#define QDP_ACTION_REGERR  7 /* note registration error */
#define QDP_ACTION_CERR    8 /* process C1_CERR on registered link */
#define QDP_ACTION_CACK    9 /* process C1_CACK on registered link */
#define QDP_ACTION_CTRL   10 /* process control port packet on registered link */
#define QDP_ACTION_RQCNF  11 /* request data link configuration */
#define QDP_ACTION_LDCNF  12 /* process data link configuration */
#define QDP_ACTION_RQMEM  13 /* request DP tokens */
#define QDP_ACTION_LDMEM  14 /* save DP tokens */
#define QDP_ACTION_OPEN   15 /* open data port */
#define QDP_ACTION_DTO    16 /* data port timeout processing */
#define QDP_ACTION_HBEAT  17 /* send a heartbeat */
#define QDP_ACTION_USRCMD 18 /* forward user command (if any) */
#define QDP_ACTION_DACK   19 /* acknowledge data packets, if required */
#define QDP_ACTION_DATA   20 /* process DT_DATA packet */
#define QDP_ACTION_FILL   21 /* process DT_FILL packet */
#define QDP_ACTION_DROP   22 /* drop spurious packet */
#define QDP_ACTION_FLUSH  23 /* flush reorder buffer */
#define QDP_ACTION_CRCERR 24 /* deal with CRC error */
#define QDP_ACTION_FAIL   25 /* library bug, shutdown system */
#define QDP_NUM_ACTION    26
#define QDP_MAX_ACTION    (QDP_NUM_ACTION - 1)

#ifdef INCLUDE_FSA_STRINGS
static char *ActionString[QDP_NUM_ACTION] = {
    "No Action (done)",
    "Send Automaton Command",
    "Issue Server Request",
    "Build Challenge Response",
    "This Layer Up",
    "This Layer Down",
    "Sleep",
    "Registration Error",
    "Process C1_CERR",
    "Process C1_CACK",
    "Process Control Port Packet",
    "Request Configuration Data",
    "Process Configuration Data",
    "Request DP Tokens",
    "Save DP Tokens",
    "Open Data Port",
    "Process Data Port Timeout",
    "Issue Status Heartbeat",
    "Process User Command",
    "Acknowledge Data Packets",
    "Process DT_DATA Packet",
    "Process DT_FILL Packet",
    "Drop Spurious Packet",
    "Flush Reorder Buffer",
    "Handle CRC error",
    "Abort Library"
};
#endif /* INCLUDE_FSA_STRINGS */

#define MAX_ACTION_PER_EVENT 3
typedef struct {
    struct {
        int action[MAX_ACTION_PER_EVENT]; /* actions */
        int next;                         /* new state */
    } event[QDP_NUM_EVENT];
} QDP_STATE_MACHINE;

#ifdef INCLUDE_FSA_TABLE

/* state transition table for a control (non-data) link */

static QDP_STATE_MACHINE ControlFSA[QDP_NUM_STATE] = {

    { /* ControlFSA state: idle */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_RQSRV,
        /* CTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,   
        /* Ack    */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* ControlFSA state: server request sent */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_BLDRSP, QDP_ACTION_SEND,   QDP_ACTION_NONE,   QDP_STATE_SRVRSP,
        /* TooMny */ QDP_ACTION_REGERR, QDP_ACTION_SLEEP,  QDP_ACTION_SRVRQ,  QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_REGERR, QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_REGERR, QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF,
        /* Ack    */ QDP_ACTION_TLU,    QDP_ACTION_NONE,   -1,                QDP_STATE_REGISTERED,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* ControlFSA state: server challenge response sent */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_SEND,   QDP_ACTION_NONE,   -1,                QDP_STATE_SRVRSP,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_BLDRSP, QDP_ACTION_SEND,   QDP_ACTION_NONE,   QDP_STATE_SRVRSP,
        /* TooMny */ QDP_ACTION_SLEEP,  QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_RQSRV,
        /* NotReg */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_REGERR, QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF,
        /* Ack    */ QDP_ACTION_TLU,    QDP_ACTION_NONE,   -1,                QDP_STATE_REGISTERED,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* ControlFSA state: registered */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_USRCMD, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_TLD,    QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_CERR,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_CACK,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_USRCMD, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_TLD,    QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF
    },

    { /* ControlFSA state: loading DP tokens (a command and control link should never get here)*/
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Token1 */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Token2 */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Srvch  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* TooMny */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* NotReg */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Err    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Ack    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Ctrl   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Data   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Fill   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Nosup  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Cmdrdy */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CRCerr */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* IOerr  */ -1,                -1,                -1,                QDP_STATE_OFF
    },

    { /* ControlFSA state: ready (a command and control link should never get here)*/
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* DTO    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Hbeat  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CnfRcv */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Token1 */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Token2 */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Srvch  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* TooMny */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* NotReg */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Err    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Ack    */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Ctrl   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Data   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Fill   */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Nosup  */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* Cmdrdy */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CRCerr */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* IOerr  */ -1,                -1,                -1,                QDP_STATE_OFF
    },

    { /* ControlFSA state: off */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE
    }
};

/* state transition table for a data link */

static QDP_STATE_MACHINE DataFSA[QDP_NUM_STATE] = {

    { /* DataFSA state: idle */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_RQSRV,
        /* CTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,   
        /* Ack    */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* DataFSA state: server request sent */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_BLDRSP, QDP_ACTION_SEND,   QDP_ACTION_NONE,   QDP_STATE_SRVRSP,
        /* TooMny */ QDP_ACTION_REGERR, QDP_ACTION_SLEEP,  QDP_ACTION_SRVRQ,  QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_REGERR, QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_REGERR, QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF,
        /* Ack    */ QDP_ACTION_RQCNF,  QDP_ACTION_NONE,   -1,                QDP_STATE_REGISTERED,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* DataFSA state: server challenge response sent */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_SEND,   QDP_ACTION_NONE,   -1,                QDP_STATE_SRVRSP,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_BLDRSP, QDP_ACTION_SEND,   QDP_ACTION_NONE,   QDP_STATE_SRVRSP,
        /* TooMny */ QDP_ACTION_SLEEP,  QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_RQSRV,
        /* NotReg */ QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   -1,                QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_REGERR, QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF,
        /* Ack    */ QDP_ACTION_RQCNF,  QDP_ACTION_NONE,   -1,                QDP_STATE_REGISTERED,
        /* Ctrl   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_FAIL,   QDP_ACTION_NONE,   -1,                QDP_STATE_OFF
    },

    { /* DataFSA state: registered */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_RQCNF,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_LDCNF,  QDP_ACTION_RQMEM,  QDP_ACTION_NONE,   QDP_STATE_TOKEN,
        /* Token1 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_TLD,    QDP_ACTION_SRVRQ,  QDP_ACTION_NONE,   QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_CERR,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_CACK,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_TLD,    QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF
    },

    { /* DataFSA state: loading DP tokens */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_SEND,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_LDCNF,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_LDMEM,  QDP_ACTION_RQMEM,  QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_LDMEM,  QDP_ACTION_TLU,    QDP_ACTION_OPEN,   QDP_STATE_READY,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_FLUSH,  QDP_ACTION_TLD,    QDP_ACTION_SRVRQ,  QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_CERR,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_CACK,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DATA,   QDP_ACTION_DACK,   QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_FILL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_USRCMD, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_TLD,    QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF
    },

    { /* DataFSA state: data link ready */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ -1,                -1,                -1,                QDP_STATE_OFF,
        /* CTO    */ QDP_ACTION_USRCMD, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_DTO,    QDP_ACTION_DACK,   QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_HBEAT,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_LDCNF,  QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_FLUSH,  QDP_ACTION_TLD,    QDP_ACTION_SRVRQ,  QDP_STATE_RQSRV,
        /* Err    */ QDP_ACTION_CERR,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_CACK,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_CTRL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_DATA,   QDP_ACTION_DACK,   QDP_ACTION_NONE,   QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_FILL,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_DROP,   QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_USRCMD, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_CRCERR, QDP_ACTION_NONE,   -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_TLD,    QDP_ACTION_FAIL,   QDP_ACTION_NONE,   QDP_STATE_OFF
    },

    { /* DataFSA state: off */
        /* EVENT     ACTIONS                                                  NEXT STATE        */
        /* Up     */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* DTO    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Hbeat  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CnfRcv */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Token1 */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Token2 */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Srvch  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* TooMny */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* NotReg */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Err    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Ack    */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Ctrl   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Data   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Fill   */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Nosup  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* Cmdrdy */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* CRCerr */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE,
        /* IOerr  */ QDP_ACTION_NONE,   -1,                -1,                QDP_STATE_NOCHANGE
    }
};

#endif /* INCLUDE_FSA_TABLE */

#ifdef __cplusplus
}
#endif

#endif /* qdp_fsa_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2006 Regents of the University of California            |
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
 * $Log: fsa.h,v $
 * Revision 1.8  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
