#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	DELTA_T ST_FormDelta(LONG dd,LONG dh,LONG dm,LONG ds,LONG dms)
{

	DELTA_T outtime;

/*-------------Normalize the date-------------*/

	timenormd(&dd,&dh,&dm,&ds,&dms);

	outtime.nday = dd;
	outtime.nhour = dh;
	outtime.nmin = dm;
	outtime.nsec = ds;
	outtime.nmsecs = dms;

	return(outtime);		/* Returning a structure! */

}
