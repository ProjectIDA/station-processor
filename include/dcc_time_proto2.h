/* Filename:  dcc_time_proto2.h
 * Purpose:   This file contains time function declarations that
 *            have been upgraded to be accurate to the tenth of a millisecond.
 *            STDTIME and DELTA_T structures are not used, and have
 *            been replaced with STDTIME2 and DELTA_T2 structures.
 */
#include "dcc_time.h"

STDTIME2 ST_AddToTime2(STDTIME2 intime, LONG inday, LONG inhour,	
                       LONG inminute, LONG insecond, LONG intenthmsec);

STDTIME2 ST_CleanDate2(STDTIME2 indate, WORD epoch, ULONG timflgs);

VOID ST_CnvJulToCal2(LONG injul, WORD *outyr, WORD *outmon, WORD *outday, WORD *outjday);
STDTIME2 ST_CnvJulToSTD2(JULIAN injul);

UBYTE *ST_DeltaPrint2(DELTA_T2 delta, BOOL printplusflag, BOOL fixms);

LONG ST_DeltaToMS2(DELTA_T2 indelta);

DELTA_T2 ST_DiffTimes2(STDTIME2 intime, STDTIME2 insect);

STDTIME2 ST_GetCurrentTime2(void);

LONG ST_GetJulian2(STDTIME2 intime);

LONG ST_Julian2(LONG year, LONG mon, LONG day);
 
DELTA_T2 ST_MinusDelta2(DELTA_T2 indelta);

UBYTE *ST_PrintDate2(STDTIME2 odate, BOOL fixfmt);

STDTIME2 ST_ParseTime2(UBYTE *);

int ST_TimeComp2(STDTIME2 time_arg1, STDTIME2 time_arg2);

STDTIME2 ST_TimeMax2(STDTIME2 time_arg1, STDTIME2 time_arg2);
STDTIME2 ST_TimeMin2(STDTIME2 time_arg1, STDTIME2 time_arg2);

VOID timenorm2(LONG *dy, LONG *dd, LONG *dh, LONG *dm, LONG *ds, LONG *dms);

VOID timenormd2(LONG *dd, LONG *dh, LONG *dm, LONG *ds, LONG *dms);

