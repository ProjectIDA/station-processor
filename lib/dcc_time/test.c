#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

main()
{

	STDTIME get;

	get = ST_GetCurrentTime();

	printf("Time is %s\n",ST_PrintDate(get,FALSE));

}
