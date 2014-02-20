#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB 	STDTIME ST_TimeMin(STDTIME intime,STDTIME insect)
{

	if (ST_TimeComp(intime,insect)<=0) return(intime);
	else return(insect);

}

_SUB 	STDTIME ST_TimeMax(STDTIME intime,STDTIME insect)
{

	if (ST_TimeComp(intime,insect)>=0) return(intime);
	else return(insect);

}

