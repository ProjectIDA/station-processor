#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

/*Return 1 if 1st time > 2nd time
	-1 if 1st time < 2nd time
	 0 if 1st time = 2nd time
*/
_SUB	int ST_TimeComp(STDTIME intime,STDTIME insect)
{

	if (intime.year>insect.year) 		return(1);
	if (intime.year<insect.year) 		return(-1);
	if (intime.day>insect.day) 		return(1);
	if (intime.day<insect.day) 		return(-1);
	if (intime.hour>insect.hour) 		return(1);
	if (intime.hour<insect.hour) 		return(-1);
	if (intime.minute>insect.minute) 	return(1);
	if (intime.minute<insect.minute) 	return(-1);
	if (intime.second>insect.second) 	return(1);
	if (intime.second<insect.second) 	return(-1);
	if (intime.msec>insect.msec) 		return(1);
	if (intime.msec<insect.msec) 		return(-1);
	return(0);

}
