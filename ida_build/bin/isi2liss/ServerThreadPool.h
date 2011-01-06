#pragma ident "$Id: ServerThreadPool.h,v 1.2 2008/01/27 17:10:38 akimov Exp $"

//////////////////////////////////////////////////////////////////////
// ServerThreadPool.h: interface for the ServerThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SERVERTHREADPOOL_H_)
#define _SERVERTHREADPOOL_H_

#include "ServerThread.h"
#include "platform.h"
#include <vector>


using namespace std;

class ServerThreadPool  
{
public:
	ServerThreadPool();
	~ServerThreadPool();
static	MUTEX ThreadPool_mutex;
static	vector <ServerThread *> ThreadPool;
static void RemoveThread(ServerThread *p);
static void AddThread(ServerThread *p);
static void BroadcastData(const char *p, long n);
static	void Init();
private:
static bool bInit;

};

#endif // !defined(_SERVERTHREADPOOL_H_)

/* Revision History
 *
 * $Log: ServerThreadPool.h,v $
 * Revision 1.2  2008/01/27 17:10:38  akimov
 * Added initialization procedure for global mutex.
 *
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
