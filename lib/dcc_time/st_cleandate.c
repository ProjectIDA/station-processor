#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

#define _tdays(x) (_tleap(x)?366:365)

_SUB	STDTIME	ST_CleanDate(STDTIME indate,WORD epoch,ULONG timflgs)
{

	LONG dy,dd,dh,dm,ds,dms;
	STDTIME outtime;

	dy = indate.year;
	dd = indate.day;
	dh = indate.hour;
	dm = indate.minute;
	ds = indate.second;
	dms = indate.msec;

	dy = _calyear(dy,epoch,timflgs);
	dd--;		/* Let jul date start at 0 for now */

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
