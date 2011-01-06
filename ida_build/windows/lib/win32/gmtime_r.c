#pragma ident "$Id: gmtime_r.c,v 1.1 2005/10/14 16:29:19 dechavez Exp $"

#include <time.h>
 

struct tm * gmtime_r(const time_t *clock, struct tm *result)
	{ 
	struct tm *tmp;

	tmp = gmtime(clock);
	if(tmp==NULL) return NULL;

    *result = *tmp;
	return result;
	}

/* Revision History
 *
 * $Log: gmtime_r.c,v $
 * Revision 1.1  2005/10/14 16:29:19  dechavez
 * added 10-11-2005 (aap)
 *
 */

