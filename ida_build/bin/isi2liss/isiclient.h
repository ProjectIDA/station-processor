#pragma ident "$Id: isiclient.h,v 1.1 2008/01/21 22:29:55 akimov Exp $"
#ifndef _ISICLIENT_PARAM_H_
#define _ISICLIENT_PARAM_H_
#include <string>

typedef struct 
	{
	std::string server;
	std::string site;
	int port;
	int lissport;
	int timeout;
	static bool bd;
	} 
 ISICLIENT_PARAM;

void RunISIClientthread(ISICLIENT_PARAM *isi_param);

#endif /* _ISICLIENT_PARAM_H_    */

/* Revision History
 *
 * $Log: isiclient.h,v $
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
