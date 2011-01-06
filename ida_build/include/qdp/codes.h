#pragma ident "$Id: codes.h,v 1.10 2009/02/23 21:48:09 dechavez Exp $"
/*======================================================================
 *
 * Q330 command, response, error codes, bit-maps, etc.
 *
 *====================================================================*/
#ifndef qdp_codes_h_included
#define qdp_codes_h_included

/* Basic Responses, Q330 -> DP */

#define QDP_C1_CACK         0xa0   /* Command Acknowledge */
#define QDP_C1_SRVCH        0xa1   /* Server Challenge */
#define QDP_C1_CERR         0xa2   /* Command Error */
#define QDP_C1_MYSN         0xa3   /* My Serial Number */
#define QDP_C1_PHY          0xa4   /* Physical Interfaces */
#define QDP_C1_LOG          0xa5   /* Logical (data) port */
#define QDP_C1_GLOB         0xa6   /* Global Programming */
#define QDP_C1_FIX          0xa7   /* Fixed Values after Reboot */
#define QDP_C1_MAN          0xa8   /* Manufacturer's Area */
#define QDP_C1_STAT         0xa9   /* Status */
#define QDP_C1_RT           0xaa   /* Routing Table */
#define QDP_C1_THN          0xab   /* Thread Names */
#define QDP_C1_GID          0xac   /* GPS ID Strings */
#define QDP_C1_RCNP         0xad   /* CNP Reply Message */
#define QDP_C1_SPP          0xae   /* Slave Processor Parameters */
#define QDP_C1_SC           0xaf   /* Sensor Control Mapping */
#define QDP_C1_UNUSED_B0    0xb0   /* unused */
#define QDP_C1_FLGS         0xb1   /* Combination Packet */
#define QDP_C1_COMBO QDP_C1_FLGS   /* preferred name */
#define QDP_C1_DCP          0xb2   /* Digitizer Calibration Packet */
#define QDP_C1_DEV          0xb3   /* CNP Device Info */

/* Memory Responses, Q330 -> DP */

#define QDP_C1_MEM          0xb8   /* Memory Contents */
#define QDP_C1_MOD          0xb9   /* Memory Module Map */
#define QDP_C1_FREE         0xba   /* Free Flash Memory

/* Advanced Responses, Q330 -> DP */

#define QDP_C2_PHY          0xc0   /* Physical Interface */
#define QDP_C2_GPS          0xc1   /* GPS Parameters */
#define QDP_C2_WIN          0xc2   /* Recording Windows */
#define QDP_C2_UNUSED_C3    0xc3   /* unused */
#define QDP_C2_AMASS        0xc4   /* Automatic Mass Re-centering */
#define QDP_C2_POC          0xc5   /* Point of Contact */
#define QDP_C2_BACK         0xc6   /* Baler Acknowledge */
#define QDP_C2_VACK         0xc7   /* Vacuum Acknowledge (VACC -> DP) */
#define QDP_C2_BCMD         0xc8   /* Baler Command (VACC -> DP) */
#define QDP_C2_REGRESP      0xc9   /* Registration Response */
#define QDP_C2_QV           0xca   /* QuickView Response */
#define QDP_C2_MD5          0xcb   /* MD5 Result */
#define QDP_C1_TERR         0x61   /* Tertiary Responses */

/* Tertiary Responses, Q330 -> DP */

#define QDP_C3_ANNC         0      /* Announce Structure */

/* Basic Commands, DP -> Q330 */

#define QDP_C1_RQSRV        0x10   /* Request Server Registration */
#define QDP_C1_SRVRSP       0x11   /* Server Response */
#define QDP_C1_DSRV         0x12   /* Delete Server */
#define QDP_C1_SAUTH        0x13   /* Set Authorization Codes */
#define QDP_C1_POLLSN       0x14   /* Poll for Serial Number */
#define QDP_C1_SPHY         0x15   /* Set Physical Interfaces */
#define QDP_C1_RQPHY        0x16   /* Request Physical Interfaces */
#define QDP_C1_SLOG         0x17   /* Set Logical (data) Port */
#define QDP_C1_RQLOG        0x18   /* Request Logical (data) Port */
#define QDP_C1_CTRL         0x19   /* Control 330 Operation */
#define QDP_C1_SGLOB        0x1a   /* Set Global Programming */
#define QDP_C1_RQGLOB       0x1b   /* Request Global Programming */
#define QDP_C1_RQFIX        0x1c   /* Request Fixed Values after Reboot */
#define QDP_C1_SMAN         0x1d   /* Set Manufacturer's Area */
#define QDP_C1_RQMAN        0x1e   /* Request Manufacturer's Area */
#define QDP_C1_RQSTAT       0x1f   /* Request Status */
#define QDP_C1_WSTAT        0x20   /* Write to Status Ports */
#define QDP_C1_VCO          0x21   /* Set VCO */
#define QDP_C1_PULSE        0x22   /* Pulse sensor control line(s) */
#define QDP_C1_QCAL         0x23   /* Start QCAL330 Calibration */
#define QDP_C1_STOP         0x24   /* Stop Calibration */
#define QDP_C1_RQRT         0x25   /* Request Routing Table */
#define QDP_C1_MRT          0x26   /* Modify Routing Table */
#define QDP_C1_RQTHN        0x27   /* Request Thread Names */
#define QDP_C1_RQGID        0x28   /* Request GPS ID Strings */
#define QDP_C1_SCNP         0x29   /* Send CNP Message */
#define QDP_C1_SRTC         0x2a   /* Set Real Time Clock */
#define QDP_C1_SOUT         0x2b   /* Set Output Bits */
#define QDP_C1_SSPP         0x2c   /* Set Slave Processor Parameters */
#define QDP_C1_RQSPP        0x2d   /* Request Slave Processor Parameters */
#define QDP_C1_SSC          0x2e   /* Set Sensor Control Mapping */
#define QDP_C1_RQSC         0x2f   /* Request Sensor Control Mapping */
#define QDP_C1_UMSG         0x30   /* Send User Message */
#define QDP_C1_UNUSED_31    0x31   /* unused */
#define QDP_C1_UNUSED_32    0x32   /* unused */
#define QDP_C1_WEB          0x33   /* Set Web Server Link */
#define QDP_C1_RQFLGS       0x34   /* Request Combination Packet */
#define QDP_C1_RQDCP        0x35   /* Request Digitizer Calibration packet */
#define QDP_C1_RQDEV        0x36   /* Request CNP Device Info */
#define QDP_C1_SDEV         0x37   /* Set Device Options */
#define QDP_C1_PING         0x38   /* PING Q330 */

/* Memory Commands, DP -> Q330 */

#define QDP_C1_SMEM         0x40   /* Set Memory Contents */
#define QDP_C1_RQMEM        0x41   /* Request Memory Contents */
#define QDP_C1_ERASE        0x42   /* Erase Flash Sectors */
#define QDP_C1_RQMOD        0x43   /* Request Memory Module Map */
#define QDP_C1_RQFREE       0x44   /* Request Free Flash Memory */

/* Advanced Commands, DP -> Q330 */

#define QDP_C2_SPHY         0x50   /* Set Physical Interface */
#define QDP_C2_RQPHY        0x51   /* Request Physical Interface */
#define QDP_C2_SGPS         0x52   /* Set GPS Parameters */
#define QDP_C2_RQGPS        0x53   /* Request GPS Parameters */
#define QDP_C2_SWIN         0x54   /* Set Recording Windows */
#define QDP_C2_RQWIN        0x55   /* Request Recording Windows */
#define QDP_C2_UNUSED_56    0x56   /* unused */
#define QDP_C2_SAMASS       0x57   /* Set Automatic Mass Re-centering */
#define QDP_C2_RQAMASS      0x58   /* Request Automatic Mass Re-centering */
#define QDP_C2_SBPWR        0x59   /* Set Baler Power and Dialer Control */
#define QDP_C2_BRDY         0x5a   /* Baler Ready */
#define QDP_C2_BOFF         0x5b   /* Baler Off */
#define QDP_C2_BRESP        0x5c   /* Baler Response */
#define QDP_C2_REGCHK       0x5d   /* Registration Check */
#define QDP_C2_INST         0x5e   /* Installer Command */
#define QDP_C2_RQQV         0x5f   /* QuickView Request */
#define QDP_C2_RQMD5        0x60   /* Request MD5 Result */
#define QDP_C2_TERC         0x69   /* Tertiary Commands */

/* Tertiary Commands, DP -> Q330 */

#define QDP_C3_RQANNC       1      /* Request Announce Structure */
#define QDP_C3_SANNC        2      /* Set Announce Structure */

/* Data comm */

#define QDP_DT_DATA         0x00   /* Data Record */
#define QDP_DT_FILL         0x06   /* Fill Record */
#define QDP_DT_USER         0x07   /* User defined packet */
#define QDP_DT_DACK         0x0a   /* Data Acknowledge */
#define QDP_DT_OPEN         0x0b   /* Open Data Port */

/* QDP_C1_CERR Codes */

#define QDP_CERR_NOERR     -1   /* Not a C1_CERR packet */
#define QDP_CERR_PERM       0   /* No Permission */
#define QDP_CERR_TMSERV     1   /* Too many servers */
#define QDP_CERR_NOTR       2   /* You are not registered */
#define QDP_CERR_INVREG     3   /* Invalid Registration Request */
#define QDP_CERR_PAR        4   /* Parameter Error */
#define QDP_CERR_SNV        5   /* Structure not valid */
#define QDP_CERR_CTRL       6   /* Control Port Only */
#define QDP_CERR_SPEC       7   /* Special Port Only */
#define QDP_CERR_MEM        8   /* Memory operation already in progress */
#define QDP_CERR_CIP        9   /* Calibration in Progress */
#define QDP_CERR_DNA       10   /* Data not Available */
#define QDP_CERR_DB9       11   /* Console Port Only */
#define QDP_CERR_FLASH     12   /* Flash Write or Erase Error */

/* QDP_C1_MEM memory type codes */

#define QDP_MEM_TYPE_FLASH         0x00
#define QDP_MEM_TYPE_CONFIG_DP1    0x01
#define QDP_MEM_TYPE_CONFIG_DP2    0x02
#define QDP_MEM_TYPE_CONFIG_DP3    0x03
#define QDP_MEM_TYPE_CONFIG_DP4    0x04
#define QDP_MEM_TYPE_WEBPAGE_MEM   0x05
#define QDP_MEM_TYPE_SLAVE_EEPROM  0x10
#define QDP_MEM_TYPE_SLAVE_PIC     0x11
#define QDP_MEM_TYPE_CLOCK_RAM     0x12
#define QDP_MEM_TYPE_CALIB_PIC     0x13
#define QDP_MEM_TYPE_QAPCHP_EEPROM 0x14
#define QDP_MEM_TYPE_PKTBUF_MEM    0x15
#define QDP_MEM_TYPE_DSP_PROG_MEM  0x16
#define QDP_MEM_TYPE_DSP_DATA_MEM  0x17
#define QDP_MEM_TYPE_INVALID       0xFF

/* QDP_C1_CTRL flags bitmap */

#define QDP_CTRL_SAVE          0x01 /* save current programming to EEPROM */
#define QDP_CTRL_REBOOT        0x02 /* Reboot */
#define QDP_CTRL_RESYNC        0x04 /* Re-Sync */
#define QDP_CTRL_GPS_ON        0x08 /* Turn GPS On */
#define QDP_CTRL_GPS_OFF       0x10 /* Turn GPS Off */
#define QDP_CTRL_GPS_COLDSTART 0x20 /* Cold-start GPS */

/* Sensor control mappings */

#define QDP_SC_IDLE             0 /* not calibrating or re-centering */
#define QDP_SC_SENSOR_A_CALIB   1 /* sensor A calibrating */
#define QDP_SC_SENSOR_A_CENTER  2 /* sensor A centering */
#define QDP_SC_SENSOR_A_CAP     3 /* sensor A capacitive coupling */
#define QDP_SC_SENSOR_B_CALIB   4 /* sensor B calibrating */
#define QDP_SC_SENSOR_B_CENTER  5 /* sensor B centering */
#define QDP_SC_SENSOR_B_CAP     6 /* sensor B capacitive coupling */
#define QDP_SC_SENSOR_A_LOCK    7 /* sensor A lock */
#define QDP_SC_SENSOR_A_UNLOCK  8 /* sensor A unlock */
#define QDP_SC_SENSOR_A_AUX1    9 /* sensor A aux1 */
#define QDP_SC_SENSOR_A_AUX2   10 /* sensor A aux2 */
#define QDP_SC_SENSOR_B_LOCK   11 /* sensor B lock */
#define QDP_SC_SENSOR_B_UNLOCK 12 /* sensor B unlock */
#define QDP_SC_SENSOR_B_AUX1   13 /* sensor B aux1 */
#define QDP_SC_SENSOR_B_AUX2   14 /* sensor B aux2 */
#define QDP_SC_ACTIVE_HIGH_BIT 0x0100
#define QDP_SC_MASK            0x00ff

/* Clock types */

#define QDP_CLOCK_NONE 0 /* no clock */
#define QDP_CLOCK_M12  1 /* Motorola M12 */

/* Auxiliary AD types */

#define QDP_AUXAD_DEFAULT 32

/* Calibrator types */

#define QDP_CALIBRATOR_QCAL330 33

/* Manufacturer's Area flag bits */

#define QDP_MAN_CLEAR_COUNTERS 0x01
#define QDP_MAN_CHN13_26BIT    0x08
#define QDP_MAN_CHN46_26BIT    0x10
#define QDP_MAN_ENABLE_NTP     0x20

/* Bitmap to control various printer output */

#define QDP_PRINT_HDR     0x01
#define QDP_PRINT_PAYLOAD 0x02
#define QDP_PRINT_RAW     0x04

#define QDP_PRINT_LCQ      0x01
#define QDP_PRINT_LCQLC   (0x02 | QDP_PRINT_LCQ)
#define QDP_PRINT_LCQDATA (0x04 | QDP_PRINT_LCQLC)

#endif /* qdp_codes_h_included */

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
 * $Log: codes.h,v $
 * Revision 1.10  2009/02/23 21:48:09  dechavez
 * added QDP_PRINT_* bitmaps
 *
 * Revision 1.9  2009/02/04 05:40:59  dechavez
 * fixed QDP_C2_RQGPS typo, added clock type, aux ad types, calibrator types
 * and manufacturer's area constants
 *
 * Revision 1.8  2009/01/23 23:53:10  dechavez
 * fixed QDP_C1_GLOB typo
 *
 * Revision 1.7  2007/10/31 17:18:11  dechavez
 * fixed GPS on/off/coldstart codes
 *
 * Revision 1.6  2007/05/17 22:27:44  dechavez
 * initial production release
 *
 */
