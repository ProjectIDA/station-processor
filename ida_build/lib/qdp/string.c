#pragma ident "$Id: string.c,v 1.13 2009/02/03 23:01:04 dechavez Exp $"
/*======================================================================
 *
 * Stirng conversions
 *
 *====================================================================*/
#include "qdp.h"

typedef struct {
    char *text;
    int code;
} TEXT_MAP;

static TEXT_MAP CmdMap[] = {
    {"C1_CACK",    QDP_C1_CACK}, 
    {"C1_SRVCH",   QDP_C1_SRVCH},
    {"C1_CERR",    QDP_C1_CERR}, 
    {"C1_MYSN",    QDP_C1_MYSN},
    {"C1_PHY",     QDP_C1_PHY}, 
    {"C1_LOG",     QDP_C1_LOG},
    {"C1_GLOB",    QDP_C1_GLOB}, 
    {"C1_FIX",     QDP_C1_FIX},
    {"C1_MAN",     QDP_C1_MAN}, 
    {"C1_STAT",    QDP_C1_STAT},
    {"C1_RT",      QDP_C1_RT}, 
    {"C1_THN",     QDP_C1_THN},
    {"C1_GID",     QDP_C1_GID}, 
    {"C1_RCNP",    QDP_C1_RCNP},
    {"C1_SPP",     QDP_C1_SPP}, 
    {"C1_SC",      QDP_C1_SC},
    {"C1_FLGS",    QDP_C1_FLGS}, 
    {"C1_DCP",     QDP_C1_DCP},
    {"C1_DEV",     QDP_C1_DEV}, 
    {"C1_MEM",     QDP_C1_MEM},
    {"C1_MOD",     QDP_C1_MOD}, 
    {"C1_FREE",    QDP_C1_FREE},
    {"C2_PHY",     QDP_C2_PHY}, 
    {"C2_GPS",     QDP_C2_GPS},
    {"C2_WIN",     QDP_C2_WIN}, 
    {"C2_AMASS",   QDP_C2_AMASS},
    {"C2_POC",     QDP_C2_POC}, 
    {"C2_BACK",    QDP_C2_BACK},
    {"C2_VACK",    QDP_C2_VACK}, 
    {"C2_BCMD",    QDP_C2_BCMD},
    {"C2_REGRESP", QDP_C2_REGRESP}, 
    {"C2_QV",      QDP_C2_QV},
    {"C2_MD5",     QDP_C2_MD5}, 
    {"C1_TERR",    QDP_C1_TERR},
    {"C1_RQSRV",   QDP_C1_RQSRV}, 
    {"C1_SRVRSP",  QDP_C1_SRVRSP},
    {"C1_DSRV",    QDP_C1_DSRV}, 
    {"C1_SAUTH",   QDP_C1_SAUTH},
    {"C1_POLLSN",  QDP_C1_POLLSN}, 
    {"C1_SPHY",    QDP_C1_SPHY},
    {"C1_RQPHY",   QDP_C1_RQPHY}, 
    {"C1_SLOG",    QDP_C1_SLOG},
    {"C1_RQLOG",   QDP_C1_RQLOG}, 
    {"C1_CTRL",    QDP_C1_CTRL},
    {"C1_SGLOB",   QDP_C1_SGLOB}, 
    {"C1_RQGLOB",  QDP_C1_RQGLOB},
    {"C1_RQFIX",   QDP_C1_RQFIX}, 
    {"C1_SMAN",    QDP_C1_SMAN},
    {"C1_RQMAN",   QDP_C1_RQMAN}, 
    {"C1_RQSTAT",  QDP_C1_RQSTAT},
    {"C1_WSTAT",   QDP_C1_WSTAT}, 
    {"C1_VCO",     QDP_C1_VCO},
    {"C1_PULSE",   QDP_C1_PULSE}, 
    {"C1_QCAL",    QDP_C1_QCAL},
    {"C1_STOP",    QDP_C1_STOP}, 
    {"C1_RQRT",    QDP_C1_RQRT},
    {"C1_MRT",     QDP_C1_MRT}, 
    {"C1_RQTHN",   QDP_C1_RQTHN},
    {"C1_RQGID",   QDP_C1_RQGID}, 
    {"C1_SCNP",    QDP_C1_SCNP},
    {"C1_SRTC",    QDP_C1_SRTC}, 
    {"C1_SOUT",    QDP_C1_SOUT},
    {"C1_SSPP",    QDP_C1_SSPP}, 
    {"C1_RQSPP",   QDP_C1_RQSPP},
    {"C1_SSC",     QDP_C1_SSC}, 
    {"C1_RQSC",    QDP_C1_RQSC},
    {"C1_UMSG",    QDP_C1_UMSG}, 
    {"C1_WEB",     QDP_C1_WEB},
    {"C1_RQFLGS",  QDP_C1_RQFLGS}, 
    {"C1_RQDCP",   QDP_C1_RQDCP},
    {"C1_RQDEV",   QDP_C1_RQDEV}, 
    {"C1_SDEV",    QDP_C1_SDEV},
    {"C1_PING",    QDP_C1_PING}, 
    {"C1_SMEM",    QDP_C1_SMEM},
    {"C1_RQMEM",   QDP_C1_RQMEM}, 
    {"C1_ERASE",   QDP_C1_ERASE},
    {"C1_RQMOD",   QDP_C1_RQMOD}, 
    {"C1_RQFREE",  QDP_C1_RQFREE},
    {"C2_SPHY",    QDP_C2_SPHY}, 
    {"C2_RQPHY",   QDP_C2_RQPHY},
    {"C2_SGPS",    QDP_C2_SGPS}, 
    {"C2_RQGPS",   QDP_C2_RQGPS},
    {"C2_SWIN",    QDP_C2_SWIN}, 
    {"C2_RQWIN",   QDP_C2_RQWIN},
    {"C2_SAMASS",  QDP_C2_SAMASS}, 
    {"C2_RQAMASS", QDP_C2_RQAMASS},
    {"C2_SBPWR",   QDP_C2_SBPWR}, 
    {"C2_BRDY",    QDP_C2_BRDY},
    {"C2_BOFF",    QDP_C2_BOFF}, 
    {"C2_BRESP",   QDP_C2_BRESP},
    {"C2_REGCHK",  QDP_C2_REGCHK}, 
    {"C2_INST",    QDP_C2_INST},
    {"C2_RQQV",    QDP_C2_RQQV}, 
    {"C2_RQMD5",   QDP_C2_RQMD5},
    {"C2_TERC",    QDP_C2_TERC}, 
    {"DT_DATA",    QDP_DT_DATA},
    {"DT_FILL",    QDP_DT_FILL}, 
    {"DT_USER",    QDP_DT_USER}, 
    {"DT_DACK",    QDP_DT_DACK},
    {"DT_OPEN",    QDP_DT_OPEN},
    {NULL, 0}
};

static TEXT_MAP ErrMap[] = {
    {"no error",                      QDP_CERR_NOERR},
    {"permission denied",             QDP_CERR_PERM},
    {"too many servers",              QDP_CERR_TMSERV},
    {"not registered",                QDP_CERR_NOTR},
    {"invalid registration response", QDP_CERR_INVREG},
    {"parameter error",               QDP_CERR_PAR},
    {"invalid EEPROM structure",      QDP_CERR_SNV},
    {"not on config port",            QDP_CERR_CTRL},
    {"not on special functions port", QDP_CERR_SPEC},
    {"memory operation in progress",  QDP_CERR_MEM},
    {"calibration in progress",       QDP_CERR_CIP},
    {"data not (yet) available",      QDP_CERR_DNA},
    {"not on console",                QDP_CERR_DB9},
    {"flash write/erase error",       QDP_CERR_FLASH},
    {NULL, 0}
};

static TEXT_MAP CtrlMap[] = {
    {"save",          QDP_CTRL_SAVE},
    {"reboot",        QDP_CTRL_REBOOT},
    {"resync",        QDP_CTRL_RESYNC},
    {"GPS On",        QDP_CTRL_GPS_ON},
    {"GPS Off",       QDP_CTRL_GPS_OFF},
    {"GPS Coldstart", QDP_CTRL_GPS_COLDSTART},
    {NULL, 0}
};

static TEXT_MAP PortMap[] = {
    {"Configuration Port",     QDP_CFG_PORT},
    {"Special Functions Port", QDP_SFN_PORT},
    {"Logical Port 1",         QDP_LOGICAL_PORT_1},
    {"Logical Port 2",         QDP_LOGICAL_PORT_2},
    {"Logical Port 3",         QDP_LOGICAL_PORT_3},
    {"Logical Port 4",         QDP_LOGICAL_PORT_4},
    {NULL, 0}
};

static TEXT_MAP DebugMap[] = {
    {"terse",  QDP_TERSE},
    {"normal", QDP_INFO},
    {"debug",  QDP_DEBUG},
    {NULL, 0}
};

static TEXT_MAP MemTypeMap[] = {
    {"Flash Memory",                         QDP_MEM_TYPE_FLASH},
    {"Configuration Memory for Data Port 1", QDP_MEM_TYPE_CONFIG_DP1},
    {"Configuration Memory for Data Port 2", QDP_MEM_TYPE_CONFIG_DP2},
    {"Configuration Memory for Data Port 3", QDP_MEM_TYPE_CONFIG_DP3},
    {"Configuration Memory for Data Port 4", QDP_MEM_TYPE_CONFIG_DP4},
    {"Web Page Memory",                      QDP_MEM_TYPE_WEBPAGE_MEM},
    {"Slave Processor EEPROM",               QDP_MEM_TYPE_SLAVE_EEPROM},
    {"Slave Processor PIC Internal Memory",  QDP_MEM_TYPE_SLAVE_PIC},
    {"Clock Chip RAM",                       QDP_MEM_TYPE_CLOCK_RAM},
    {"Calibrator PIC Internal Memory",       QDP_MEM_TYPE_CALIB_PIC},
    {"QAPCHP EEPROM",                        QDP_MEM_TYPE_QAPCHP_EEPROM},
    {"Packet Buffer Memory",                 QDP_MEM_TYPE_PKTBUF_MEM},
    {"DSP Program Memory",                   QDP_MEM_TYPE_DSP_PROG_MEM},
    {"DSP Data Memory",                      QDP_MEM_TYPE_DSP_DATA_MEM},
    {"*** INVALID ***",                      QDP_MEM_TYPE_INVALID},
    {NULL, 0}
};

static TEXT_MAP BlocketteIdentMap[] = {
    {"QDP_DC_ST38  ",    QDP_DC_ST38},
    {"QDP_DC_ST816 ",   QDP_DC_ST816},
    {"QDP_DC_ST32  ",    QDP_DC_ST32},
    {"QDP_DC_ST232 ",   QDP_DC_ST232},
    {"QDP_DC_MN38  ",    QDP_DC_MN38},
    {"QDP_DC_MN816 ",   QDP_DC_MN816},
    {"QDP_DC_MN32  ",    QDP_DC_MN32},
    {"QDP_DC_MN232 ",   QDP_DC_MN232},
    {"QDP_DC_AG38  ",    QDP_DC_AG38},
    {"QDP_DC_AG816 ",   QDP_DC_AG816},
    {"QDP_DC_AG32  ",    QDP_DC_AG32},
    {"QDP_DC_AG232 ",   QDP_DC_AG232},
    {"QDP_DC_CNP38 ",   QDP_DC_CNP38},
    {"QDP_DC_CNP816",  QDP_DC_CNP816},
    {"QDP_DC_CNP316",  QDP_DC_CNP316},
    {"QDP_DC_CNP232",  QDP_DC_CNP232},
    {"QDP_DC_D32   ",     QDP_DC_D32},
    {"QDP_DC_COMP  ",    QDP_DC_COMP},
    {"QDP_DC_MULT  ",    QDP_DC_MULT},
    {"QDP_DC_SPEC  ",    QDP_DC_SPEC},
    {NULL, 0}
};

static TEXT_MAP PLLStateMap[] = {
    {"off",      QDP_PLL_OFF},
    {"hold",     QDP_PLL_HOLD},
    {"tracking", QDP_PLL_TRACK},
    {"locked",   QDP_PLL_LOCK},
    {NULL, 0}
};

static TEXT_MAP PhysicalPortMap[] = {
    {"Serial 1", QDP_PP_SER1},
    {"Serial 2", QDP_PP_SER2},
    {"Serial 3", QDP_PP_SER3},
    {"Ethernet", QDP_PP_ETH},
    {NULL, 0}
};

static TEXT_MAP SensorControlMap[] = {
    {"Idle",                 QDP_SC_IDLE},
    {"Sensor A Calibrate",   QDP_SC_SENSOR_A_CALIB},
    {"Sensor A Centering",   QDP_SC_SENSOR_A_CENTER},
    {"Sensor A Capacitive",  QDP_SC_SENSOR_A_CAP},
    {"Sensor B Calibrate",   QDP_SC_SENSOR_B_CALIB},
    {"Sensor B Centering",   QDP_SC_SENSOR_B_CENTER},
    {"Sensor B Capacitive",  QDP_SC_SENSOR_B_CAP},
    {"Sensor A Unlock",      QDP_SC_SENSOR_A_LOCK},
    {"Sensor A Lock",        QDP_SC_SENSOR_A_UNLOCK},
    {"Sensor A Auxiliary 1", QDP_SC_SENSOR_A_AUX1},
    {"Sensor A Auxiliary 2", QDP_SC_SENSOR_A_AUX2},
    {"Sensor B Unlock",      QDP_SC_SENSOR_B_UNLOCK},
    {"Sensor B Lock",        QDP_SC_SENSOR_B_LOCK},
    {"Sensor B Auxiliary 1", QDP_SC_SENSOR_B_AUX1},
    {"Sensor B Auxiliary 2", QDP_SC_SENSOR_B_AUX2},
    {NULL, 0}
};

static TEXT_MAP AuxtypeMap[] = {
    {"AuxAD", QDP_AUXAD_DEFAULT},
    {NULL, 0}
};

static TEXT_MAP ClocktypeMap[] = {
    {"No clock", QDP_CLOCK_NONE},
    {"Motorola M12", QDP_CLOCK_M12},
    {NULL, 0}
};

static TEXT_MAP CalibratorMap[] = {
    {"QCAL330", QDP_CALIBRATOR_QCAL330},
    {NULL, 0}
};

static char *LocateString(int code, TEXT_MAP *map, char *DefaultMessage)
{
int i;

    for (i = 0; map[i].text != NULL; i++) if (map[i].code == code) return map[i].text;
    return DefaultMessage;
}

char *qdpCmdString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CmdMap, DefaultMessage);
}

char *qdpErrString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ErrMap, DefaultMessage);
}

char *qdpCtrlString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CtrlMap, DefaultMessage);
}

char *qdpPortString(int code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, PortMap, DefaultMessage);
}

char *qdpDebugString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, DebugMap, DefaultMessage);
}

char *qdpMemTypeString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, MemTypeMap, DefaultMessage);
}

char *qdpBlocketteIdentString(UINT8 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, BlocketteIdentMap, DefaultMessage);
}

char *qdpPLLStateString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) code, PLLStateMap, DefaultMessage);
}

char *qdpPhysicalPortString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) code, PhysicalPortMap, DefaultMessage);
}

char *qdpSensorControlString(UINT32 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString((UINT8) (code & QDP_SC_MASK), SensorControlMap, DefaultMessage);
}

char *qdpAuxtypeString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, AuxtypeMap, DefaultMessage);
}

char *qdpClocktypeString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, ClocktypeMap, DefaultMessage);
}

char *qdpCalibratorString(UINT16 code)
{
static char *DefaultMessage = "<unknown>";

    return LocateString(code, CalibratorMap, DefaultMessage);
}

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
 * $Log: string.c,v $
 * Revision 1.13  2009/02/03 23:01:04  dechavez
 * dded qdpAuxtypeString(), qdpClocktypeString(), qdpCalibratorString()
 *
 * Revision 1.12  2009/01/24 00:11:08  dechavez
 * added qdpSensorControlString(), fixed C1_GLOB typo
 *
 * Revision 1.11  2007/12/21 18:54:26  dechavez
 * Fixed PhysicalPortMap entries
 *
 * Revision 1.10  2007/12/20 22:49:14  dechavez
 * added qdpPLLStateString(), qdpPhysicalPortString()
 *
 * Revision 1.9  2007/05/18 18:00:57  dechavez
 * initial production release
 *
 */
