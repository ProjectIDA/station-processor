#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

_SUB	LONG	ST_GetJulian(STDTIME intime)
{

	LONG jul1;

	jul1 = _julday(intime.year,1,intime.day);

	return(jul1);

}
