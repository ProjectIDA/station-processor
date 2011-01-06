#pragma ident "$Id: auzinfo.h,v 1.1 2008/01/10 16:37:41 dechavez Exp $"
#ifndef _AUZINFO_H_
#define _AUZINFO_H_
#include <string>
using namespace std;
typedef struct 
	{
	string sta, chan, loc;
	float flta, fsta;
	float fr1, fr2,threshold;
	int order;
	float calib, calper, sps;
	}
AUZINFO;
#endif /*   _AUZINFO_H_    */

/* Revision History
 *
 * $Log: auzinfo.h,v $
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
