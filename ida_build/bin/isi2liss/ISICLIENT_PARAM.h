#pragma ident "$Id: ISICLIENT_PARAM.h,v 1.1 2008/02/01 18:04:31 akimov Exp $"

#ifndef _ISICLIENT_PARAM_
#define _ISICLIENT_PARAM_
#include <string>

class ISICLIENT_PARAM
	{
	public:
	ISICLIENT_PARAM();
	~ISICLIENT_PARAM();
	std::string server;
	std::string site;
	int port;
	int lissport;
	int timeout;
	static bool bd;
	} 
 ;

#endif

 /* Revision History
 *
 * $Log: ISICLIENT_PARAM.h,v $
 * Revision 1.1  2008/02/01 18:04:31  akimov
 * created
 *
 */
