#pragma ident "$Id: auzglobals.cpp,v 1.2 2008/01/27 16:59:17 akimov Exp $"
#include "auzglobals.h"
#include "isi.h"


string auzglobals::server   = "127.0.0.1";
string auzglobals::pemail;
int auzglobals::port = ISI_DEFAULT_PORT;
int auzglobals::timeout = 60;
bool auzglobals::bContinue=true;
vector <CDetect *> auzglobals::det_streams;
bool auzglobals::bd = false;
bool auzglobals::debug = false;


auzglobals::auzglobals()
	{

	}

auzglobals::~auzglobals()
	{
	Clean();
	}

void auzglobals::Clean()
	{
	while(det_streams.size()>0)
		{
		CDetect *p = det_streams[0];
		det_streams.erase(det_streams.begin());
		delete p;
		}
	}

/* Revision History
 *
 * $Log: auzglobals.cpp,v $
 * Revision 1.2  2008/01/27 16:59:17  akimov
 * added global variables to support -bd and -debug options
 *
 * Revision 1.1  2008/01/10 16:37:41  dechavez
 * initial developmental snapshot of Andrei Akimov sources
 *
 */
