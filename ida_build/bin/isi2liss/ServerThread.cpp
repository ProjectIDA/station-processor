#pragma ident "$Id: ServerThread.cpp,v 1.2 2008/01/27 17:09:11 akimov Exp $"

#include "ServerThread.h"
#include "ServerThreadPool.h"

//
// Heartbeat sending function
// prevents client from disconnection by timeout
//
//
static bool sendHeartbeat(SOCKET sc)
	{
	char buff[512] ;
	static char *fid = "sendHeartbeat";

	memset(buff,'0',6);
	memset(&buff[6],' ',506);

	if(util_write(sc, buff, 512, 60)<0)
		{
		util_log(1, "%s: util_write: %s", fid, syserrmsg(errno)); 
		return false;
		}
	return true;
	}


//
// Thread to write data from data_queue to opened socket 
// 
// argptr - pointer to associated ServerThread
//
THREAD_FUNC serverthread(void *argptr)
	{
	time_t nTO = time(NULL);
	static char *fid = "serverthread";
	ServerThread *p = (ServerThread *)argptr;
	int sc = p->GetSocket();

	while(true)
		{

		if(time(NULL)-nTO>40)
			{
			if(!sendHeartbeat(sc)) break;
			nTO = time(NULL);
			}
		MUTEX_LOCK(&p->data_queue_mutex);

		if(p->data_queue.size()>0)
			{
			nTO = time(NULL);
			RawDataBlock *rdb = p->data_queue[0];
			MUTEX_UNLOCK(&p->data_queue_mutex);
			if(util_write(sc, rdb->pdata,  rdb->nSize, 60)<0)
				{
				util_log(1, "%s: util_write: %s", fid, syserrmsg(errno)); 
				break;
				}
			MUTEX_LOCK(&p->data_queue_mutex);
			p->data_queue.erase(p->data_queue.begin());
			delete rdb;
			MUTEX_UNLOCK(&p->data_queue_mutex);
			continue;
			}
		else
			{
			MUTEX_UNLOCK(&p->data_queue_mutex);
			sleep(2);
			}

		}

	utilCloseSocket(sc);
	p->SetActiveFlag(false);
	ServerThreadPool::RemoveThread(p);
	}
//
// ServerThread Constructor
// 
// int soc - output server socket
//
ServerThread::ServerThread(int soc)
	{
	bThreadActive = true;
	m_hsock = soc;
	MUTEX_INIT(&data_queue_mutex);
	}

ServerThread::~ServerThread()
	{
	MUTEX_DESTROY(&data_queue_mutex);
	CleanDataQueue();
	}
//
//
// Run output thread 
//
//
bool ServerThread::Run()
	{
	THREAD tid;
	static char *fid = "ServerThread::Run()"; 
	bThreadActive = true;

    if (!THREAD_CREATE(&tid, serverthread, (void *) this)) {
		util_log(1, "%s: THREAD_CREATE: %s", fid, syserrmsg(errno)); 
        exit(1);
    } 


	return true;
	}

//
// Set intern flag bThreadActive state 
// bThreadActive==false means thread is not running
//
//
void ServerThread::SetActiveFlag(bool b)
	{
	bThreadActive = b;
	}
//
//
// Cleans internal data queue
//
//
void ServerThread::CleanDataQueue()
	{
	MUTEX_LOCK(&data_queue_mutex);

	while(data_queue.size())
		{
		RawDataBlock *p = data_queue[0];
		delete p;
		data_queue.erase(data_queue.begin());
		}
	MUTEX_UNLOCK(&data_queue_mutex);
	}

int ServerThread::GetSocket()
	{
	return m_hsock;
	}

//
// Puts data block to internal queue
//
// const char *p - pointer to data block
// n - block size
//
//
bool ServerThread::PutDataBlockToQueue(const char *p, long n)
	{
	if(!bThreadActive) return false;
	RawDataBlock *rdb = new RawDataBlock;
	if(rdb==NULL) return false;

	rdb->PutDataBlock(p,n);
	MUTEX_LOCK(&data_queue_mutex);
	data_queue.push_back(rdb);
	MUTEX_UNLOCK(&data_queue_mutex);
	return true;
	}

/* Revision History
 *
 * $Log: ServerThread.cpp,v $
 * Revision 1.2  2008/01/27 17:09:11  akimov
 * Moved mutex lock after heartbeat call, to fix bug with missing mutex unlock
 * if sending heartbeat fails.
 *
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
