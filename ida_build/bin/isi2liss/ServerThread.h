#pragma ident "$Id: ServerThread.h,v 1.1 2008/01/21 22:29:55 akimov Exp $"

//////////////////////////////////////////////////////////////////////
// ServerThread.h: interface for the ServerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SERVERTHREAD_H_)
#define _SERVERTHREAD_H_
#include "platform.h"
#include "util.h"
#include "logio.h"
#include "RawDataBlock.h"
#include <vector>

using namespace std;

class ServerThread  
{
public:
	ServerThread(int soc);
	virtual ~ServerThread();
	bool Run();
	void CleanDataQueue();
	void SetActiveFlag(bool b);
	int GetSocket();
	bool PutDataBlockToQueue(const char *p, long n);

protected:
	vector <RawDataBlock *> data_queue;
	MUTEX data_queue_mutex;
	bool bThreadActive;
	int m_hsock;
	friend THREAD_FUNC serverthread(void *argptr);
};

#endif // !defined(_SERVERTHREAD_H_)

/* Revision History
 *
 * $Log: ServerThread.h,v $
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
