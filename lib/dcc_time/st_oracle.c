#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB STDTIME ST_GetOracleTime(char *instr)
{
	int i,y,d,h,m,s;

	STDTIME intime;

	if (strlen(instr)<1) return(ST_End());

	intime = ST_Zero();

	i = sscanf(instr,"%d %d %d %d %d",
	       &y,&d,&h,&m,&s);

	if (i!=5) return(ST_End());

	intime.year = y;
	intime.day = d;
	intime.hour = h;
	intime.minute = m;
	intime.second = s;

	if (intime.year<2) intime = ST_Zero();

	return(intime);
}

static char _ttime[50];	

_SUB char *ST_PutOracleTime(STDTIME intime)
{

	if (intime.year<2) {
		intime = ST_Zero();
		intime.year = 1;
		intime.day = 1;
	}

	sprintf(_ttime,"%d %d %d %d %d",
		intime.year,
		intime.day,
		intime.hour,
		intime.minute,
		intime.second);

	return(_ttime);
}



