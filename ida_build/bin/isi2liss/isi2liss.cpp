#pragma ident "$Id: isi2liss.cpp,v 1.4 2008/02/01 18:23:55 akimov Exp $"

#include "util.h"
#include "ServerThreadPool.h"
#include <string>
#include <vector>
#include "isi.h"
#include "util.h"
#include "ISICLIENT_PARAM.h"

using namespace std;

void RunISIClientthread(ISICLIENT_PARAM *isi_param);
void init(int argc, char* argv[],ISICLIENT_PARAM &isi_param);
void help(char *myname);

void CatchSignal(int sig)
	{
	signal(SIGPIPE, CatchSignal);
	return ;
	}
int SetSignalHandlers()
	{
   	signal(SIGPIPE, CatchSignal);
	return 1;
	}


int main(int argc, char* argv[])
{
	static char *fid = "main()";
	char  cBuff[1024];
	char cMsg[256]; 
	bool bStop = false;
	unsigned long wl;

	SOCKET ssock;
    struct sockaddr_in self;

	ISICLIENT_PARAM isi_param;

	ServerThreadPool::Init();
	init(argc, argv, isi_param);
	if(isi_param.bd) utilBackGround();
    
    SetSignalHandlers();
	util_logopen("syslogd", 1, 9, 1, argv[1], argv[0]);
   if (!utilNetworkInit()) {
        perror("utilNetworkInit");
        exit(0);
    }


	RunISIClientthread( &isi_param);

	ssock=socket (AF_INET, SOCK_STREAM, 0);

	/*---Initialize address/port structure---*/
	memset(&self,0, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(isi_param.lissport);
	self.sin_addr.s_addr = htonl(INADDR_ANY);


	int i=1;
	int nRes;
	nRes = setsockopt(ssock,SOL_SOCKET,SO_REUSEADDR ,(char *) &i, sizeof(i)); 


	/*---Assign a port number to the socket---*/
    if ( bind(ssock, (struct sockaddr*)&self, sizeof(self)) != 0 )
		{
		util_log(1, "%s: bind: %s", fid, strerror(errno));
		exit(-1);
		}


	utilSetNonBlockingSocket(ssock);

	/*---Make it a "listening socket"---*/
	if ( listen(ssock, 1) != 0 )
		{
		util_log(1, "%s: listen: %s", fid, strerror(errno)); 
		exit(0);
		return 0;
		}

	while(!bStop)
		{
		SOCKET clientfd;
		struct sockaddr_in client_addr;
		socklen_t addrlen=sizeof(client_addr);

		sleep(1);
		clientfd = accept(ssock, (struct sockaddr*)&client_addr, &addrlen);


		if(clientfd==INVALID_SOCKET) continue;
		sprintf(cBuff,"[%s:%d] - connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		util_log(1, cBuff);
		
		ServerThread *pServer = new ServerThread(clientfd);

		if(pServer->Run())
			{
			ServerThreadPool::AddThread(pServer);
			}
		else
			{
			delete pServer;
			util_log(1, "%s: THREAD_CREATE: %s", fid, strerror(errno));
			}

		}

	utilCloseSocket(ssock);


	return 0;
}

void init(int argc, char* argv[],ISICLIENT_PARAM &isi_param)
	{
	int i;

	if(argc<2)
		{
		help(argv[0]);
		}

	if (strncmp(argv[1], "-help", strlen("-help")) == 0) {
			help(argv[0]);
		}

	isi_param.site       = argv[1];
	isi_param.server     = "127.0.0.1";
	isi_param.port       = ISI_DEFAULT_PORT;
	isi_param.lissport   = 4000;
	isi_param.bd         = false;
	isi_param.timeout    = 60;

	for (i = 2; i < argc; i++) {
		if (strncmp(argv[i], "isi=", strlen("isi=")) == 0) {
			isi_param.server = argv[i] + strlen("isi=");
		}

		if (strncmp(argv[i], "port=", strlen("port=")) == 0) {
			isi_param.port = atoi(argv[i] + strlen("port="));
		}

		if (strncmp(argv[i], "to=", strlen("to=")) == 0) {
			isi_param.timeout = atoi(argv[i] + strlen("to="));
		}

		if (strncmp(argv[i], "lissport=", strlen("lissport=")) == 0) {
			isi_param.lissport = atoi(argv[i] + strlen("lissport="));
		}

		if (strncmp(argv[i], "-bd", strlen("-bd")) == 0) {
			isi_param.bd         = true;
		}

		if (strncmp(argv[i], "-help", strlen("-help")) == 0) {
			help(argv[0]);
		}


	}

	}

void help(char *myname)
	{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s  <site name> [options] \n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"site name is the *required* parameter\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"isi=string        sets the name of the server to the specified string. Default value is 127.0.0.1\n");
    fprintf(stderr,"to=secs           sets read timeout. Default value is 60\n");
    fprintf(stderr,"port=int          sets the port number. Default value is 39136\n");
    fprintf(stderr,"lissport=int      sets output liss server port. Default value is 4000\n");
    fprintf(stderr,"-bd               run in the background\n");
    fprintf(stderr,"\n"); 
	exit(0);
	}

/* Revision History
 *
 * $Log: isi2liss.cpp,v $
 * Revision 1.4  2008/02/01 18:23:55  akimov
 * Addressed compile errors noted by F. Shelly
 *
 * Revision 1.3  2008/01/27 17:08:25  akimov
 * Added initialization procedure call for class ServerThreadPool.  There is no
 * any object of ServerThreadPool type declared, so constructor for that class
 * was never get called, thus mutex in ServerThreadPool was not initialized.
 *
 * Revision 1.2  2008/01/25 21:58:43  akimov
 * set default timeout to 60 seconds
 *
 * Revision 1.1  2008/01/21 22:29:55  akimov
 * initial release
 *
 */
