#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	DELTA_T ST_AddToDelta(DELTA_T intime,LONG dd,LONG dh,
		LONG dm,LONG ds,LONG dms)
{

	DELTA_T outtime;

	dd += intime.nday;
	dh += intime.nhour;
	dm += intime.nmin;
	ds += intime.nsec;
	dms += intime.nmsecs;

/*-------------Normalize the date-------------*/

	timenormd(&dd,&dh,&dm,&ds,&dms);

	outtime.nday = dd;
	outtime.nhour = dh;
	outtime.nmin = dm;
	outtime.nsec = ds;
	outtime.nmsecs = dms;

	return(outtime);		/* Returning a structure! */

}

