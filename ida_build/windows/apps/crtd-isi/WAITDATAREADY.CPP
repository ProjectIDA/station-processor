#include <winsock.h>
#include <time.h>

extern BOOL bContinue;

BOOL WaitDataReady(SOCKET sock, int nTimeOut)
	{
	int nres;
	time_t t1,t2,tbuff;
	u_long arp;

	t1=time(&tbuff);

	while(1)
		{
		arp=0;
		if( (nres=ioctlsocket(sock,FIONREAD, &arp))!=0) return FALSE;
		if(arp>0) 
			{
			return TRUE;
			}
		t2=time(&tbuff);
		if(t2-t1>nTimeOut||!bContinue)
			{
			return FALSE;
			}
		Sleep(200);
		}
	return FALSE;
	}