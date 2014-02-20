#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	DELTA_T	ST_DiffTimes(STDTIME intime,STDTIME insect)
{

	DELTA_T outtime;
	LONG jul1,jul2;
	LONG dd,dh,dm,ds,dms;

	jul1 = _julday(intime.year,1,intime.day);
	jul2 = _julday(insect.year,1,insect.day);

	dd = jul1 - jul2;
	dh = intime.hour - insect.hour;
	dm = intime.minute - insect.minute;
	ds = intime.second - insect.second;
	dms = intime.msec - insect.msec;

/*-------------Normalize the date-------------*/

	timenormd(&dd,&dh,&dm,&ds,&dms);

	outtime.nday = dd;
	outtime.nhour = dh;
	outtime.nmin = dm;
	outtime.nsec = ds;
	outtime.nmsecs = dms;

	return(outtime);		/* Returning a structure! */

}
