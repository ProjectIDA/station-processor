#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

#define _tdays(x) (_tleap(x)?366:365)

_SUB	STDTIME	ST_AddToTime(STDTIME intime,LONG dd,LONG dh,
		LONG dm,LONG ds,LONG dms)
{

	STDTIME outtime;
	LONG dy;

	dy = intime.year;
	dd = intime.day + dd - 1;	/* Let jul date start at 0 for now */
	dh = intime.hour + dh;
	dm = intime.minute + dm;
	ds = intime.second + ds;
	dms = intime.msec + dms;	

/*-------------Normalize the date-------------*/

	timenorm(&dy,&dd,&dh,&dm,&ds,&dms);

	outtime.year = dy;
	outtime.day = dd + 1;
	outtime.hour = dh;
	outtime.minute = dm;
	outtime.second = ds;
	outtime.msec = dms;

	return(outtime);		/* Returning a structure! */

}
