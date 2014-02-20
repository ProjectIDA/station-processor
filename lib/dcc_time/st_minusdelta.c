#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	DELTA_T	ST_MinusDelta(DELTA_T indelta)
{

	DELTA_T outtime;
	LONG dd,dh,dm,ds,dms;

	dd = - indelta.nday;
	dh = - indelta.nhour;
	dm = - indelta.nmin;
	ds = - indelta.nsec;
	dms = - indelta.nmsecs;	

/*-------------Normalize the date-------------*/

	timenormd(&dd,&dh,&dm,&ds,&dms);

	outtime.nday = dd;
	outtime.nhour = dh;
	outtime.nmin = dm;
	outtime.nsec = ds;
	outtime.nmsecs = dms;

	return(outtime);		/* Returning a structure! */

}
