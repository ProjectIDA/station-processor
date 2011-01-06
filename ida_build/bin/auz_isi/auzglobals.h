#pragma ident "$Id: auzglobals.h,v 1.3 2008/02/01 18:23:44 akimov Exp $"
// auzglobals.h: interface for the auzglobals class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUZGLOBALS_H__F6B0C4BD_D25F_4B8C_85A1_AA70DAA1C8A2__INCLUDED_)
#define AFX_AUZGLOBALS_H__F6B0C4BD_D25F_4B8C_85A1_AA70DAA1C8A2__INCLUDED_

#include "detect.h"
#include <string>
#include <vector>

using namespace std;

class auzglobals  
{
public:
	auzglobals();
	virtual ~auzglobals();
	void Clean();
	static string server;
	static string pemail;
	static int port;
	static int timeout;
	static bool bContinue;
	static vector <CDetect *> det_streams;
	static bool bd;
	static bool debug;


};

#endif // !defined(AFX_AUZGLOBALS_H__F6B0C4BD_D25F_4B8C_85A1_AA70DAA1C8A2__INCLUDED_)

/* Revision History
 *
 * $Log: auzglobals.h,v $
 * Revision 1.3  2008/02/01 18:23:44  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.2  2008/01/27 17:01:36  akimov
 * added bd and debug
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
