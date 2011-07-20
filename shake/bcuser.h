/****************************************************************
 * File - bcuser.h 
 *
 * User header file for accessing the bc635/7PCI-V2/PCIe module
 * This file may not be distributed -- it may only be used for 
 * development or evaluation purposes.
 * For details refer to SW_License.txt.
 *
 * Copyright (c) Symmetricom, 2000-2009
 *
 ***************************************************************/

#ifndef _BCUSER_H_
#define _BCUSER_H_

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "bcdrvr.h"


// DATA STUCTURES
typedef struct _TimeData {
	BYTE mode;			// Selected Reference mode
	BYTE  tmformat;		// Time format (i.e. bin or bcd)
	DWORD year;		    // Current year data
	BYTE  gpstmfmt;		// GPS or UTC time format (0=UTC, 1=GPS)
	BYTE  leapsec;		// Current leap second count
	char  leapevt;		// Current leap second event type (1=ins,0=no,-1=del)
	DWORD leapevttm;	// Scheduled time for leap second event
	short locoff;		// Local time offset
	BYTE  lhlfhr;		// Local time offset half hour flag
	long  propdelay;	// Propgation delay
	BYTE  dlight;		// IEEE Daylight Savings Flag 
	BYTE  localt;		// Local Time Flag Enable/Disable
} TimeData;

typedef struct _TimeCodeData {
	BYTE  format;		// Time Code Decode Type 
	BYTE  modulation;	// Time Code Decode Modulation
	BYTE  gencode;		// Time Code Generator Type
	short genoffset;	// Time Code Generator Offset
	BYTE  ghfhr;		// Time Code Gen Offset half Hour flag 0/1
} TimeCodeData;

// The data structure used in bcGetTimeCodeDataEx to return time code
// subtype for V2 hardware.
typedef struct _TimeCodeDataEx {
	BYTE  inputFormat;	// Time Code Decode Type 
	BYTE  inputSubType;	// Input Time Code SubType, supported in V2 hardware
	BYTE  modulation;	// Time Code Decode Modulation
	BYTE  outputFormat;	// Time Code Generator Type
	BYTE  outputSubType;	// Output Time Code SubType, supported in V2 hardware
	short genoffset;	// Time Code Generator Offset
	BYTE  ghfhr;		// Time Code Gen Offset half Hour flag 0/1
} TimeCodeDataEx;

typedef struct _OtherData {
	BYTE hbtmode;		// Heartbeat/Periodic mode (0=async,1=sync)
	USHORT hbtcnt1;		// Heartbeat/Periodic n1
	USHORT hbtcnt2;		// Heartbeat/Periodic n2
	BYTE freq;			// Current freq output (1,5,10MHz)
	BYTE evtctl;		// Enable/Disable events
	BYTE evtsense;		// Event trigger on edge (1=falling,0=rising)
	BYTE evtlock;		// Enable/Disable event capture lockout
	BYTE evtsrc;		// Event trigger source (0=evt,1=hbt)
} OtherData;

typedef struct _OtherDataEx {
	BYTE hbtmode;		// Heartbeat/Periodic mode (0=async,1=sync)
	USHORT hbtcnt1;		// Heartbeat/Periodic n1
	USHORT hbtcnt2;		// Heartbeat/Periodic n2
	BYTE freq;			// Current freq output (1,5,10MHz)
	BYTE evtctl;		// Enable/Disable events
	BYTE evtsense;		// Event trigger on edge (1=falling,0=rising)
	BYTE evtlock;		// Enable/Disable event capture lockout
	BYTE evtsrc;		// Event trigger source (0=evt,1=hbt)
	BYTE evt2ctl;		// Enable/Disable event2
	BYTE evt2sense;		// Event2 trigger on edge (1=falling,0=rising)
	BYTE evt2lock;		// Enable/Disable event2 capture lockout
	BYTE evt3ctl;		// Enable/Disable event3
	BYTE evt3sense;		// Event3 trigger on edge (1=falling,0=rising)
	BYTE evt3lock;		// Enable/Disable event3 capture lockout
} OtherDataEx;

typedef struct _EventsData {
	BYTE evtsrc;		// Event trigger source (0=evt,1=hbt)
	BYTE evtctl;		// Enable/Disable events
	BYTE evtsense;		// Event trigger on edge (1=falling,0=rising)
	BYTE evtlock;		// Enable/Disable event capture lockout
	BYTE evt2ctl;		// Enable/Disable event2
	BYTE evt2sense;		// Event2 trigger on edge (1=falling,0=rising)
	BYTE evt2lock;		// Enable/Disable event2 capture lockout
	BYTE evt3ctl;		// Enable/Disable event3
	BYTE evt3sense;		// Event3 trigger on edge (1=falling,0=rising)
	BYTE evt3lock;		// Enable/Disable event3 capture lockout
} EventsData;

typedef struct _OscData {
	BYTE  BattStat;		// Real Time Clock Battery status
	BYTE  ClkSrc;		// Selected Clock input
    BYTE  DisCtl;		// Disciplining Enable/Disable
    WORD  DisGain;		// Disciplining Gain
    WORD  DacVal;		// DAC Value
    BYTE  JamCtl;		// Jamsynch Enable/Disable
	long  AdjClk;		// Advance/Retard Clock Value
} OscData;

typedef struct _ModelData
{
	BYTE model1;		// Model 'B'
	BYTE model2;		// Model 'C'
	BYTE model3;		// Model '6'
	BYTE model4;		// Model '3'
	BYTE model5;		// Model '5' or '7'
	BYTE model6;		// Model 'P'
	BYTE model7;		// Model 'C'
	BYTE model8;		// Model 'I'
} ModelData;

typedef struct _VerData {
	BYTE byt1;			// Version 'D' 
	BYTE byt2;			// Version 'T' 
	BYTE byt3;			// Version '6' 
	BYTE byt4;			// Version '0' 
	BYTE byt5;			// Version '0' 
	BYTE byt6;			// Version '0' 
	BYTE byt7;			// Version 'A,B,C...' 
	BYTE byt8;			// Version '.' 
	BYTE byt9;			// Version '1,2,3...' 
	BYTE byt10;			// Version '1,2,3...' 
	BYTE byt11;			// Version '1,2,3...' 
} VerData;

typedef struct _GpsPkt {
	BYTE id;			// GPS Packet ID Byte
	BYTE len;			// GPS Packet Data Length
	BYTE *data;			// GPS Packet Data Area Pointer
} GpsPkt;


// PCI REGISTER OFFSETS
enum { PCI_TIMEREQ_OFFSET	= 0x0 };
enum { PCI_EVENTREQ_OFFSET	= 0x4 };
enum { PCI_UNLOCK_OFFSET	= 0x8 };
enum { PCI_CONTROL_OFFSET	= 0x10 };
enum { PCI_ACK_OFFSET		= 0x14 };
enum { PCI_MASK_OFFSET		= 0x18 };
enum { PCI_INTSTAT_OFFSET	= 0x1c };
enum { PCI_MINSTRB_OFFSET	= 0x20 };
enum { PCI_MAJSTRB_OFFSET	= 0x24 };
enum { PCI_TIME0_OFFSET		= 0x30 };
enum { PCI_TIME1_OFFSET		= 0x34 };
enum { PCI_EVENT0_OFFSET	= 0x38 };
enum { PCI_EVENT1_OFFSET	= 0x3c };

// EVENT2 AND EVENT3
enum { PCI_EVENT2_0_OFFSET	= 0x28 };
enum { PCI_EVENT2_1_OFFSET	= 0x2c };
enum { PCI_EVENT3_0_OFFSET	= 0x48 };
enum { PCI_EVENT3_1_OFFSET	= 0x4c };


// MODE SELECTS 
enum { MODE_IRIG			= 0x00 };
enum { MODE_FREE			= 0x01 };
enum { MODE_1PPS			= 0x02 };
enum { MODE_RTC				= 0x03 };
enum { MODE_GPS				= 0x06 };


// TIME FORMAT SELECTS 
enum { FORMAT_DECIMAL		= 0x00 };
enum { FORMAT_BINARY		= 0x01 };

// TIME CODE TYPE 
enum { TCODE_IRIG_A			= 0x41 };
enum { TCODE_IRIG_B			= 0x42 };
enum { TCODE_IEEE			= 0x49 };
enum { TCODE_NASA			= 0x4E };

// ADDITIONAL TIME CODE TYPE FOR V2 HARDWARE
enum { TCODE_2137           = 0x32 };
enum { TCODE_IRIG_E         = 0x45 };
enum { TCODE_IRIG_G         = 0x47 };
enum { TCODE_XR3            = 0x58 };
enum { TCODE_IRIG_e         = 0x65 };

// IRIG TIME CODE SUB TYPE.
// The sub type is used when calling
//
//   bcSetTcInEx (BC_PCI_HANDLE hBC_PCI, BYTE TcIn, BYTE SubType)
//   bcSetGenCodeEx (BC_PCI_HANDLE hBC_PCI, BYTE GenTc, BYTE SubType)
//
// For time code with no sub type, please input TCODE_IRIG_SUBTYPE_NONE
// as 'SubType'. See 'bcSetTcInEx()' comment.

// This is for time code with no sub type.
enum { TCODE_IRIG_SUBTYPE_NONE 	= 0x0 };

// Sub type 'Y' can be used with IRIG A, B, E, e
//   AY - IRIG A with year
//   BY - IRIG B with year
//   EY - IRIG E 1000 with year
//   eY - IRIG E 100 with year
// This is an input time code sub type - used as SubType in bcSetTcInEx().
enum { TCODE_IRIG_SUBTYPE_Y     = 0x59 };

// Sub type 'T' is used with IRIG B
//   BT - IRIG B Legacy TrueTime
enum { TCODE_IRIG_SUBTYPE_T     = 0x54 };

// Output time code sub type - used as SubType in bcSetGenCodeEx().
// See 'bcSetGenCodeEx()' comment.
enum { TCODE_IRIG_SUBTYPE_0     = 0x30 };
enum { TCODE_IRIG_SUBTYPE_1     = 0x31 };
enum { TCODE_IRIG_SUBTYPE_2     = 0x32 };
enum { TCODE_IRIG_SUBTYPE_3     = 0x33 };
enum { TCODE_IRIG_SUBTYPE_4     = 0x34 };
enum { TCODE_IRIG_SUBTYPE_5     = 0x35 };
enum { TCODE_IRIG_SUBTYPE_6     = 0x36 };
enum { TCODE_IRIG_SUBTYPE_7     = 0x37 };


// TIME CODE MODULATION 
enum { TCODE_MOD_AM			= 0x4D };
enum { TCODE_MOD_DC			= 0x44 };


// CLOCK SOURCE SELECTS 
enum { CLK_INT				= 0x49 };
enum { CLK_EXT				= 0x45 };


// PERIODIC OUTPUT 
enum { PERIODIC_SYNC		= 0x01 };
enum { PERIODIC_NOSYNC		= 0x00 };


// JAM SYNC CONTROL
enum { JAM_SYNC_ENA			= 0x01 };
enum { JAM_SYNC_DIS			= 0x00 };


// GPS TIME FORMAT
enum { GPS_TIME_FMT			= 0x01 };
enum { UTC_TIME_FMT			= 0x00 };


// GPS OPERATIONAL MODE
enum { GPS_STATIC			= 0x01 };
enum { GPS_NONE_STATIC		= 0x00 };


// LOCAL OFFSET FLAG 
enum { LOCAL_OFF_ENABLE		= 0x01 };
enum { LOCAL_OFF_DISABLE	= 0x00 };


// YEAR INCREMENT FLAG
enum { YEAR_AUTO_ENA		= 0x01 };
enum { YEAR_AUTO_DIS		= 0x00 };


// MISCELLANEOUS
enum { CMD_WARMSTART		= 0x01 };

// CONTROL REGISTER
enum { LOCKOUT_DISABLE		= 0x00 };
enum { LOCKOUT_ENABLE		= 0x01 };
enum { EVENT_INPUT			= 0x00 };
enum { PROGRAM_PERIODIC		= 0x01 };
enum { EVENT_FALLING_EDGE	= 0x00 };
enum { EVENT_RISING_EDGE	= 0x01 };
enum { EVENT_DISABLE		= 0x00 };
enum { EVENT_ENABLE			= 0x01 };
enum { OUT_FREQ_1MHZ		= 0x01 };
enum { OUT_FREQ_5MHZ		= 0x05 };
enum { OUT_FREQ_10MHZ		= 0x0A };

// INTERRUPTS
enum { INTERRUPT_EVENT		= 0x01 };
enum { INTERRUPT_PERIODIC	= 0x02 };
enum { INTERRUPT_STROBE		= 0x04 };
enum { INTERRUPT_1PPS		= 0x08 };
enum { INTERRUPT_GPS		= 0x10 };
enum { INTERRUPT_EVENT2		= 0x20 };
enum { INTERRUPT_EVENT3		= 0x40 };


// Choices for bcSetPeriodicDDSSelect()
enum { SELECT_PERIODIC_OUT      = 0x0 };
enum { SELECT_DDS_OUT           = 0x1 };

// Choices for bcSetDDSDivider()
enum { DDS_DIVIDE_BY_1E0        = 0x0 };
enum { DDS_DIVIDE_BY_1E1        = 0x1 };
enum { DDS_DIVIDE_BY_1E2        = 0x2 };
enum { DDS_DIVIDE_BY_1E3        = 0x3 };
enum { DDS_DIVIDE_BY_1E4        = 0x4 };
enum { DDS_DIVIDE_BY_1E5        = 0x5 };
enum { DDS_DIVIDE_BY_1E6        = 0x6 };
enum { DDS_DIVIDE_BY_1E7        = 0x7 };
enum { DDS_DIVIDE_BY_PREG       = 0xF };

// Choices for bcSetDDSDividerSource()
enum { DDS_DIVIDER_SRC_DDS      = 0x0 };
enum { DDS_DIVIDER_SRC_MULT     = 0x1 };
enum { DDS_DIVIDER_SRC_100MHZ   = 0x2 };

// Choices for bcSetDDSSyncMode()
enum { DDS_SYNC_MODE_FRAC       = 0x0 };
enum { DDS_SYNC_MODE_CONT       = 0x1 };

// Choices for bcSetDDSMultiplier()
enum { DDS_MULTIPLY_BY_1        = 0x1 };
enum { DDS_MULTIPLY_BY_2        = 0x2 };
enum { DDS_MULTIPLY_BY_3        = 0x3 };
enum { DDS_MULTIPLY_BY_4        = 0x4 };
enum { DDS_MULTIPLY_BY_6        = 0x6 };
enum { DDS_MULTIPLY_BY_8        = 0x8 };
enum { DDS_MULTIPLY_BY_10       = 0xA };
enum { DDS_MULTIPLY_BY_16       = 0x10 };


//
// User SDK Functions
//

// Functions to start/stop the device
BC_PCI_HANDLE bcStartPci(void);
void  bcStopPci(BC_PCI_HANDLE hBC_PCI);

// Functions to read/write device registers
BOOL  bcReadReg  (BC_PCI_HANDLE hBC_PCI, DWORD dwOffset, PDWORD ulpData);
BOOL  bcWriteReg (BC_PCI_HANDLE hBC_PCI, DWORD dwOffset, DWORD data);

// Functions to read/write dual port RAM registers
BOOL  bcReadDPReg  (BC_PCI_HANDLE hBC_PCI, DWORD dwOffset, PBYTE bpData);
BOOL  bcWriteDPReg (BC_PCI_HANDLE hBC_PCI, DWORD dwOffset, BYTE data);

// Functions to read binary/decimal time
BOOL  bcReadBinTime (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PBYTE bpstat);
BOOL  bcReadDecTime (BC_PCI_HANDLE hBC_PCI, struct tm *ptm, PDWORD ulpMin, PBYTE bpstat);

// Functions to set binary/decimal time
BOOL  bcSetBinTime (BC_PCI_HANDLE hBC_PCI, DWORD newtime);
BOOL  bcSetDecTime (BC_PCI_HANDLE hBC_PCI, struct tm);

// Functions to request/set the year
BOOL  bcReqYear (BC_PCI_HANDLE hBC_PCI, PDWORD year);
BOOL  bcSetYear (BC_PCI_HANDLE hBC_PCI, DWORD year);

// Function to read event time
BOOL  bcReadEventTime (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PBYTE bpstat);

// Function to set the strobe time
BOOL  bcSetStrobeTime (BC_PCI_HANDLE hBC_PCI, DWORD dMaj, DWORD dMin);

// Functions to request/set the time format
BOOL  bcReqTimeFormat (BC_PCI_HANDLE hBC_PCI, PBYTE timeformat);
BOOL  bcSetTimeFormat (BC_PCI_HANDLE hBC_PCI, BYTE tmfmt);

// Functions to configure the hardware
void  bcSetMode (BC_PCI_HANDLE hBC_PCI, BYTE mode);
BOOL  bcSetLocOff (BC_PCI_HANDLE hBC_PCI, USHORT offset, BYTE half);
BOOL  bcSetGenOff (BC_PCI_HANDLE hBC_PCI, USHORT offset, BYTE half);
BOOL  bcSetPropDelay (BC_PCI_HANDLE hBC_PCI, long value);
BOOL  bcSetHbt (BC_PCI_HANDLE hBC_PCI, BYTE mode, USHORT n1, USHORT n2);
BOOL  bcSetTcIn (BC_PCI_HANDLE hBC_PCI, BYTE TcIn);
BOOL  bcSetTcInMod (BC_PCI_HANDLE hBC_PCI, BYTE TcInMod);
BOOL  bcSetGenCode (BC_PCI_HANDLE hBC_PCI, BYTE GenTc);
BOOL  bcSetLeapEvent (BC_PCI_HANDLE hBC_PCI, char flag, DWORD leapevt);
BOOL  bcSetClkSrc (BC_PCI_HANDLE hBC_PCI, BYTE clk);
BOOL  bcSetDac (BC_PCI_HANDLE hBC_PCI, USHORT dac);
BOOL  bcSetGain (BC_PCI_HANDLE hBC_PCI, short gain);
BOOL  bcSetJam (BC_PCI_HANDLE hBC_PCI, BYTE jam);
BOOL  bcSetGpsTmFmt (BC_PCI_HANDLE hBC_PCI, BYTE gpsfmt);
BOOL  bcSetGpsOperMode (BC_PCI_HANDLE hBC_PCI, BYTE gpsmode);
BOOL  bcSetLocalOffsetFlag (BC_PCI_HANDLE hBC_PCI, BYTE flagoff);
BOOL  bcSetYearAutoIncFlag (BC_PCI_HANDLE hBC_PCI, BYTE yrinc);
BOOL  bcAdjustClock (BC_PCI_HANDLE hBC_PCI, long cval);
void  bcCommand (BC_PCI_HANDLE hBC_PCI, BYTE cmd);
BOOL  bcForceJam (BC_PCI_HANDLE hBC_PCI);
BOOL  bcSyncRtc (BC_PCI_HANDLE hBC_PCI);
BOOL  bcDisRtcBatt (BC_PCI_HANDLE hBC_PCI);

// Functions to request the card's configuration
BOOL  bcReqOscData (BC_PCI_HANDLE hBC_PCI, OscData *);
BOOL  bcReqTimeCodeData (BC_PCI_HANDLE hBC_PCI, TimeCodeData *);
BOOL  bcReqTimeData (BC_PCI_HANDLE hBC_PCI, TimeData *);
BOOL  bcReqOtherData (BC_PCI_HANDLE hBC_PCI, OtherData *);
BOOL  bcReqVerData (BC_PCI_HANDLE hBC_PCI, VerData *);
BOOL  bcReqModel (BC_PCI_HANDLE hBC_PCI, ModelData *);
BOOL  bcReqSerialNum (BC_PCI_HANDLE hBC_PCI, PDWORD serial);
BOOL  bcReqHardwarFab (BC_PCI_HANDLE hBC_PCI, PWORD fab);
BOOL  bcReqAssembly (BC_PCI_HANDLE hBC_PCI, PWORD num);

// Function to request the card's Revision ID
// This is the 8 bit Revision ID in the PCI configuration space
// For version 1 PCI card, the Revision ID is 0x12
// For version 2 PCI card, the Revision ID is 0x20
BOOL  bcReqRevisionID (BC_PCI_HANDLE hBC_PCI, PWORD id);

// Function to request the time code data extended with V2 hardware support.
// The V2 hardware supports time code sub type. Calling the original function
// bcReqTimeCodeData (with TimeCodeData *) will not return the sub type info.
BOOL  bcReqTimeCodeDataEx (BC_PCI_HANDLE hBC_PCI, TimeCodeDataEx *pDat);

// Functions to request/send GPS packets (Bc637PCI model only)
BOOL  bcGPSReq (BC_PCI_HANDLE hBC_PCI, GpsPkt *);
BOOL  bcGPSSnd (BC_PCI_HANDLE hBC_PCI, GpsPkt *);
BOOL  bcGPSMan (BC_PCI_HANDLE hBC_PCI, GpsPkt *, GpsPkt *);

// Interrupts functions
BOOL  bcStartInt(BC_PCI_HANDLE hBC_PCI, BC_PCI_INT_HANDLER pCallback);
void  bcStopInt (BC_PCI_HANDLE hBC_PCI);
BOOL  bcSetInt(BC_PCI_HANDLE hBC_PCI, BYTE IntVal);
BOOL  bcReqInt(BC_PCI_HANDLE hBC_PCI, PBYTE Ints);
void  bcIntHandlerRoutine(BC_PCI_HANDLE hBC_PCI, DWORD dwSource);

// Task delay function
void  bcDelay(DWORD ustime);

// ====================================================================
// DDS functions. These functions are only available for V2 hardware.
// On V1 hardware, it always return FALSE.
// ====================================================================
BOOL  bcSetPeriodicDDSSelect (BC_PCI_HANDLE hBC_PCI, BYTE bSel);
BOOL  bcSetPeriodicDDSEnable (BC_PCI_HANDLE hBC_PCI, BOOL bEnable);
BOOL  bcSetDDSDivider (BC_PCI_HANDLE hBC_PCI, BYTE bDiv);
BOOL  bcSetDDSDividerSource (BC_PCI_HANDLE hBC_PCI, BYTE bSrc);
BOOL  bcSetDDSSyncMode (BC_PCI_HANDLE hBC_PCI, BYTE bMode);
BOOL  bcSetDDSMultiplier (BC_PCI_HANDLE hBC_PCI, BYTE bMult);
// dwPeriod is an integer in the range of [0, 0xFFFFFF]
BOOL  bcSetDDSPeriodValue (BC_PCI_HANDLE hBC_PCI, DWORD dwPeriod);
BOOL  bcSetDDSTuningWord (BC_PCI_HANDLE hBC_PCI, DWORD dwTuneWord);

// The bcSetDDSFrequency automatically sets the periodic/DDS output to
// be DDS output and sets the DDS sync mode to be DDS_SYNC_MODE_FRAC.
// If you want to come back to periodic output, please call function
// bcSetPeriodicDDSSelect to set it to periodic output. You can also
// change sync mode with bcSetDDSSyncMode.
BOOL  bcSetDDSFrequency (BC_PCI_HANDLE hBC_PCI, double fFreq);

// This function is the extended version of the 'bcSetTcIn()'. It is
// equivalent to bcSetTcIn() when the 'TcIn' is 
//   TCODE_IRIG_A, or TCODE_IRIG_B, or TCODE_IEEE, or TCODE_NASA
// and the 'SubType' is TCODE_IRIG_SUBTYPE_NONE.
// For example, 'bcSetTcIn (bBC_PCI, TCODE_IRIG_B)' is equivalent
// to 'bcSetTcInEx (bBC_PCI, TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_NONE)'.
//
// However, bcSetTcInEx supports the following new codes.
//
//   TCODE_2137, TCODE_IRIG_SUBTYPE_NONE    (2137)
//   TCODE_XR3, TCODE_IRIG_SUBTYPE_NONE     (XR3)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_Y     (AY - IRIG A with year)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_Y     (BY - IRIG B with year)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_T     (BT - IRIG B Legacy TrueTime)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_NONE  (E  - IRIG E 1000Hz no year)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_Y     (EY - IRIG E 1000Hz with year)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_NONE  (e  - IRIG E 100Hz no year)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_Y     (eY - IRIG E 100Hz with year)
//   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_NONE  (G  - IRIG G no year)
//   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_Y     (GY - IRIG G with year)
//
BOOL  bcSetTcInEx (BC_PCI_HANDLE hBC_PCI, BYTE TcIn, BYTE SubType);

// This function is the extended version of the 'bcSetGenCode()'. It is
// equivalent to bcSetGenCode() when the 'GenTc' is 
//   TCODE_IRIG_B, or TCODE_IEEE
// and the 'SubType' is TCODE_IRIG_SUBTYPE_NONE.
// For example, 'bcSetGenCode (bBC_PCI, TCODE_IRIG_B)' is equivalent
// to 'bcSetGenCodeEx (bBC_PCI, TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_NONE)'.
//
// However, bcSetGenCodeEx supports the following new codes.
//
//   TCODE_2137, TCODE_IRIG_SUBTYPE_NONE    (2137)
//   TCODE_XR3, TCODE_IRIG_SUBTYPE_NONE     (XR3)
//   TCODE_NASA, TCODE_IRIG_SUBTYPE_NONE    (NASA)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_0     (A0 - IRIG A BCD,CF,SBS)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_1     (A1 - IRIG A BCD,CF)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_2     (A2 - IRIG A BCD)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_3     (A3 - IRIG A BCD,SBS)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_4     (A4 - IRIG A BCD,YEAR,CF,SBS)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_5     (A5 - IRIG A BCD,YEAR,CF)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_6     (A6 - IRIG A BCD,YEAR)
//   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_7     (A7 - IRIG A BCD,YEAR,SBS)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_0     (B0 - IRIG B BCD,CF,SBS)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_1     (B1 - IRIG B BCD,CF)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_2     (B2 - IRIG B BCD)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_3     (B3 - IRIG B BCD,SBS)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_4     (B4 - IRIG B BCD,YEAR,CF,SBS)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_5     (B5 - IRIG B BCD,YEAR,CF)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_6     (B6 - IRIG B BCD,YEAR)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_7     (B7 - IRIG B BCD,YEAR,SBS)
//   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_T     (BT - IRIG B BCD,CF,SBS - Legacy TrueTime)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_1     (E1 - IRIG E 1000Hz BCD,CF)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_2     (E2 - IRIG E 1000Hz BCD)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_5     (E5 - IRIG E 1000Hz BCD,YEAR,CF)
//   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_6     (E6 - IRIG E 1000Hz BCD,YEAR)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_1     (e1 - IRIG E 100Hz BCD,CF)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_2     (e2 - IRIG E 100Hz BCD)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_5     (e5 - IRIG E 100Hz BCD,YEAR,CF)
//   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_6     (e6 - IRIG E 100Hz BCD,YEAR)
//   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_5     (G5 - IRIG G BCD,YEAR,CF)
//
BOOL  bcSetGenCodeEx (BC_PCI_HANDLE hBC_PCI, BYTE GenTc, BYTE SubType);

// Function to read event time including the 100 nano seconds count
BOOL  bcReadEventTimeEx (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PWORD ulpNano, PBYTE bpstat);

// this string is set to an error message, if one occurs
//extern CHAR BC_PCI_ErrorString[]; maher

// ====================================================================
// New functions for event2 and event3 support.
// ====================================================================
BOOL  bcReadEvent2TimeEx (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PWORD ulpNano, PBYTE bpstat);
BOOL  bcReadEvent3TimeEx (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PWORD ulpNano, PBYTE bpstat);
BOOL  bcReqOtherDataEx (BC_PCI_HANDLE hBC_PCI, OtherDataEx *);
BOOL  bcReqEventsData (BC_PCI_HANDLE hBC_PCI, EventsData *);
BOOL  bcSetEventsData (BC_PCI_HANDLE hBC_PCI, EventsData *);

// This function enables the interrupts specified by the input mask value.
// The function bcStartInt() uses 'IntMask' 0x7F internally. We found that
// with IntMask 0x7F, interrupts were not handled correctly by the driver
// on newer, faster computers. You must pass the mask value corresponding
// to your desired interrupts to have the driver working properly.
BOOL  bcStartIntEx(BC_PCI_HANDLE hBC_PCI, BC_PCI_INT_HANDLER pCallback, DWORD IntMask);

// Returns the 100 nano seconds count field
BOOL  bcReadBinTimeEx (BC_PCI_HANDLE hBC_PCI, PDWORD ulpMaj, PDWORD ulpMin, PWORD ulpNano, PBYTE bpstat);
BOOL  bcReadDecTimeEx (BC_PCI_HANDLE hBC_PCI, struct tm *ptm, PDWORD ulpMin, PWORD ulpNano, PBYTE bpstat);

#ifdef __cplusplus
}
#endif

#endif //_BCUSER_H_
