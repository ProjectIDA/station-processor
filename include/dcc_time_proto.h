/* Processing file st_addtodelta.c */

	DELTA_T ST_AddToDelta(DELTA_T intime,LONG dd,LONG dh,
		LONG dm,LONG ds,LONG dms)
;

/* Found 1 functions */

/* Processing file st_addtotime.c */

	STDTIME	ST_AddToTime(STDTIME intime,LONG dd,LONG dh,
		LONG dm,LONG ds,LONG dms)
;

/* Found 1 functions */

/* Processing file st_cleandate.c */

	STDTIME	ST_CleanDate(STDTIME indate,WORD epoch,ULONG timflgs)
;

/* Found 1 functions */

/* Processing file st_deltacomp.c */

	int ST_DeltaComp(DELTA_T intime,DELTA_T insect)
;

/* Found 1 functions */

/* Processing file st_deltaprint.c */

	UBYTE *ST_DeltaPrint(DELTA_T delta,BOOL printplusflag,BOOL fixms)
;

/* Found 1 functions */

/* Processing file st_deltatoms.c */

	LONG ST_DeltaToMS(DELTA_T indelta)
;

/* Found 1 functions */

/* Processing file st_difftimes.c */

	DELTA_T	ST_DiffTimes(STDTIME intime,STDTIME insect)
;

/* Found 1 functions */

/* Processing file st_flttotime.c */

	STDTIME	ST_FLTToTime(FLTTIME intime)
;

/* Found 1 functions */

/* Processing file st_formatdate.c */


/* Found 0 functions */

/* Processing file st_formdelta.c */

	DELTA_T ST_FormDelta(LONG dd,LONG dh,LONG dm,LONG ds,LONG dms)
;

/* Found 1 functions */

/* Processing file st_getcurrent.c */

	STDTIME ST_GetCurrentTime()
;

/* Found 1 functions */

/* Processing file st_getjulian.c */

	LONG	ST_GetJulian(STDTIME intime)
;

/* Found 1 functions */

/* Processing file st_getlocal.c */

	STDTIME ST_GetLocalTime()
;

/* Found 1 functions */

/* Processing file st_julian.c */

 LONG _julday(LONG year,LONG mon,LONG day)
;
 LONG ST_Julian(LONG year, LONG mon, LONG day)
;
 VOID ST_CnvJulToCal(LONG injul,WORD *outyr,WORD *outmon,
	WORD *outday,WORD *outjday)
;
 STDTIME ST_CnvJulToSTD(JULIAN injul)
;

/* Found 4 functions */

/* Processing file st_limits.c */

 STDTIME ST_Zero()
;
 DELTA_T ST_ZeroDelta()
;
 STDTIME ST_End()
;

/* Found 3 functions */

/* Processing file st_localzone.c */


/* Found 0 functions */

/* Processing file st_minmax.c */

 	STDTIME ST_TimeMin(STDTIME intime,STDTIME insect)
;
 	STDTIME ST_TimeMax(STDTIME intime,STDTIME insect)
;

/* Found 2 functions */

/* Processing file st_minusdelta.c */

	DELTA_T	ST_MinusDelta(DELTA_T indelta)
;

/* Found 1 functions */

/* Processing file st_oracle.c */

 STDTIME ST_GetOracleTime(char *instr)
;
 char *ST_PutOracleTime(STDTIME intime)
;

/* Found 2 functions */

/* Processing file st_parsetime.c */

 	STDTIME	ST_ParseTime(UBYTE *inbuff)
;

/* Found 1 functions */

/* Processing file st_printcal.c */

	char *ST_PrintCalDate(STDTIME odate,BOOL longfmt)
;
	char *ST_PrintFullDate(STDTIME odate)
;

/* Found 2 functions */

/* Processing file st_printdate.c */

	UBYTE *ST_PrintDate(STDTIME odate,BOOL fixfmt)
;

/* Found 1 functions */

/* Processing file st_printdec.c */

	UBYTE *ST_PrintDECDate(STDTIME odate,BOOL printtime)
;

/* Found 1 functions */

/* Processing file st_setupdate.c */

	BOOL _tleap(WORD year)		/* Gregorian leap rules */
;
	LONG _calyear(LONG dy,LONG epoch,ULONG timflgs)
;
	STDTIME	ST_SetupDate(LONG dy,LONG dd,LONG dh,LONG dm,
		LONG ds,LONG dms,WORD epoch,ULONG timflgs)
;

/* Found 3 functions */

/* Processing file st_spanprint.c */

	UBYTE *ST_SpanPrint(STDTIME ftime,STDTIME etime,BOOL fixfmt)
;

/* Found 1 functions */

/* Processing file st_timecomp.c */

	int ST_TimeComp(STDTIME intime,STDTIME insect)
;

/* Found 1 functions */

/* Processing file st_timeminusd.c */

	STDTIME	ST_TimeMinusDelta(STDTIME intime,DELTA_T indelta)
;

/* Found 1 functions */

/* Processing file st_timenorm.c */

	VOID timenorm(LONG *dy,LONG *dd,LONG *dh,LONG *dm,LONG *ds,LONG *dms)
;
	VOID timenormd(LONG *dd,LONG *dh,LONG *dm,LONG *ds,LONG *dms)
;

/* Found 2 functions */

/* Processing file st_timepar.c */


/* Found 0 functions */

/* Processing file st_timeplusd.c */

	STDTIME	ST_TimePlusDelta(STDTIME intime,DELTA_T indelta)
;

/* Found 1 functions */

/* Processing file st_timespan.c */

 int ST_TimeSpan(STDTIME starta,STDTIME enda,
		     STDTIME startb,STDTIME endb,
		     STDTIME *retstart,STDTIME *retend)
;

/* Found 1 functions */

/* Processing file st_timetoflt.c */

	FLTTIME	ST_TimeToFLT(STDTIME intime)
;
	long double ST_TimeToLD(STDTIME intime)
;
	double ST_TimeToMs(STDTIME intime)
;

/* Found 3 functions */

/* Processing file st_unixtimes.c */

 long ST_GetUnix(STDTIME instd)
;
 long ST_GetUnixTest(STDTIME instd)
;
 double ST_GetDblUnix(STDTIME instd)
;
 STDTIME ST_CnvUnixtoSTD(long intim)
;
 STDTIME ST_CnvUnixDbltoSTD(double intim)
;

/* Found 5 functions */

