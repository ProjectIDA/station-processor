#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

static STDTIME setuptime;
static BOOL dateset,timeset;

timeload(int hour,int minute,int second,int msec)
{

	timeset=TRUE;
	
	setuptime.hour=hour;
	setuptime.minute=minute;
	setuptime.second=second;
	setuptime.msec=msec;
}

_setyear(void)
{

	int y,f;

	y = setuptime.year;
	if (y>1900) return;		/* Its probably ok */

	f=0;
	if (y>99) f=TM_CENT;
	else if (y>9) f=TM_DECADE;

	setuptime.year = _calyear(y,0,f);

}

juldateset(int year,int day)
{

	dateset = TRUE;

	setuptime.year=year;
	_setyear();

	setuptime.day=day;

}

extern	UBYTE _dmsize[];

nordate(int month,int day,int year)
{
	int ct,i;

	dateset = TRUE;
	setuptime.year = year;
	_setyear();

	_dmsize[1] = _tleap(setuptime.year)?29:28;

	ct=0;
	for (i=0; i<month-1; i++) ct+=_dmsize[i];
	ct += day;

	setuptime.day=ct;
}

STDTIME ST_CleanDate(STDTIME indate, WORD epoch, ULONG timflgs);

_SUB 	STDTIME	ST_ParseTime(UBYTE *inbuff)
{

	int getbuf,i;
	UBYTE	trsbuf[120];

	for (i=0; i<strlen(inbuff); i++) {
		if (isupper(inbuff[i])) trsbuf[i]=tolower(inbuff[i]);
		else trsbuf[i]=inbuff[i];
	}

	trsbuf[i++] = '\0';

	dateset=timeset=FALSE;

	parstart(trsbuf,strlen(trsbuf));	

	getbuf=timparse();

	if (getbuf!=0) {
		setuptime.year=0;
		return(setuptime);
	}

	if (!timeset) {
		setuptime.hour=0;
		setuptime.minute=0;
		setuptime.second=0;
		setuptime.msec=0;
	}

	if (!dateset) {
		setuptime.year=0;
		return(setuptime);
	}

	return(ST_CleanDate(setuptime,0,TM_MILEN));
				/* Returning a structure! */

}


