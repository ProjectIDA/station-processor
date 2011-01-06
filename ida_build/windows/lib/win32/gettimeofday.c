#pragma ident "$Id: gettimeofday.c,v 1.1 2005/09/30 16:29:27 dechavez Exp $"

#include "platform.h"

int gettimeofday(struct timeval *tp, void *tzp)
	{
	if (tp != NULL)
		{
		struct _timeb timebuf;
		ftime (&timebuf);
		if (tp != NULL)
			{
			tp->tv_sec = timebuf.time;
			tp->tv_usec = (long)(timebuf.millitm) * (1000000/1000);
			}
/*		if (tzp != NULL)
			{
			tzp->tz_minuteswest = timebuf.timezone;
			tzp->tz_dsttime = 0;      
			}*/
		}
	return 0;
	}    

/* Revision History
 *
 * $Log: gettimeofday.c,v $
 * Revision 1.1  2005/09/30 16:29:27  dechavez
 * initial release (09-30-2005-aap)
 *
 */
