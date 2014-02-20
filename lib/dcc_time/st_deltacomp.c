#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	int ST_DeltaComp(DELTA_T intime,DELTA_T insect)
{

	if (intime.nday>insect.nday) 		return(1);
	if (intime.nday<insect.nday) 		return(-1);
	if (intime.nhour>insect.nhour) 		return(1);
	if (intime.nhour<insect.nhour) 		return(-1);
	if (intime.nmin>insect.nmin) 		return(1);
	if (intime.nmin<insect.nmin) 		return(-1);
	if (intime.nsec>insect.nsec) 		return(1);
	if (intime.nsec<insect.nsec) 		return(-1);
	if (intime.nmsecs>insect.nmsecs)	return(1);
	if (intime.nmsecs<insect.nmsecs)	return(-1);
	return(0);

}
