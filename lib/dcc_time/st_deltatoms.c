#include "../../include/dcc_std.h"
#include "../../include/dcc_time.h"

DELTA_T ST_MinusDelta(DELTA_T indelta);

_SUB	LONG ST_DeltaToMS(DELTA_T indelta)
{

	LONG totalms;
	BOOL minf = FALSE;

	if (indelta.nday>20) return(TIM_LIM);
	if (indelta.nday<-20) return(-TIM_LIM);

	if (indelta.nday<0) {
		minf = TRUE;
		indelta = ST_MinusDelta(indelta);
	}

	totalms = ((LONG) indelta.nday) * 86400L * 1000L;
	totalms += ((LONG) indelta.nhour) * 3600L * 1000L;
	totalms += ((LONG) indelta.nmin) * 60L * 1000L;
	totalms += ((LONG) indelta.nsec) * 1000L;
	totalms += ((LONG) indelta.nmsecs);

	if (minf) totalms = -totalms;

	if (totalms>TIM_LIM) totalms = TIM_LIM;
	if (totalms<-TIM_LIM) totalms = - TIM_LIM;

	return(totalms);

}
