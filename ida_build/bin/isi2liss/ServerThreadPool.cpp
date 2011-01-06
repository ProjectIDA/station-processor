#pragma ident "$Id: ServerThreadPool.cpp,v 1.2 2008/01/27 17:09:47 akimov Exp $"

//////////////////////////////////////////////////////////////////////
// ServerThreadPool.cpp: implementation of the ServerThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerThreadPool.h"


MUTEX ServerThreadPool::ThreadPool_mutex = MUTEX_INITIALIZER;
bool ServerThreadPool::bInit = false;
vector <ServerThread *> ServerThreadPool::ThreadPool;

ServerThreadPool::ServerThreadPool()
	{
	MUTEX_INIT(&ThreadPool_mutex);
	}

ServerThreadPool::~ServerThreadPool()
	{

	}
void ServerThreadPool::Init()
	{
	if(!bInit)MUTEX_INIT(&ThreadPool_mutex);
	bInit = true;
	}

void ServerThreadPool::RemoveThread(ServerThread *p)
	{
	MUTEX_LOCK(&ThreadPool_mutex);
	int i,nSz = ThreadPool.size();
	for(i=0; i<nSz; ++i)
		{
		if(ThreadPool[i]==p)
			{
			ThreadPool.erase(ThreadPool.begin()+i);
			delete p;
			break;
			}
		}

	MUTEX_UNLOCK(&ThreadPool_mutex);
	}
void ServerThreadPool::AddThread(ServerThread *p)
	{
	MUTEX_LOCK(&ThreadPool_mutex);
	ThreadPool.push_back(p);
	MUTEX_UNLOCK(&ThreadPool_mutex);
	}

void ServerThreadPool::BroadcastData(const char *p, long n)
	{
	MUTEX_LOCK(&ThreadPool_mutex);
	int i,nSz = ThreadPool.size();
	for(i=0; i<nSz; ++i)
		{
		ThreadPool[i]->PutDataBlockToQueue(p, n);
		}
	MUTEX_UNLOCK(&ThreadPool_mutex);
	}

/* Revision History
 *
 * $Log: ServerThreadPool.cpp,v $
 * Revision 1.2  2008/01/27 17:09:47  akimov
 * Added initialization procedure for global mutex.
 *
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
