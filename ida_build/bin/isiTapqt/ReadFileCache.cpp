// ReadFileCache.cpp: implementation of the CReadFileCache class.
//
//////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "ReadFileCache.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CReadFileCache::CReadFileCache():nCacheSize(512000)
	{
	dwBytesInCache = 0;
	dwFilePos      = 0;
	CacheBuff      = 0;
	hF             = 0;
	}
bool CReadFileCache::SetFileHandle(int hF)
	{

	long dwRead=0;
	this->hF=hF;
	if(CacheBuff==0) CacheBuff = new char[nCacheSize];
	dwFilePos=0;
	dwBytesInCache=0;
	
	if(!ReadFileToCache())
		{
		return false;
		}
	return true;
	}
bool CReadFileCache::ReadFile(void *lpBuffer,  long nNumberOfBytesToRead,  long *lpNumberOfBytesRead)
	{
	long dwRead=0, dwRead1=0, dwRestOfRead;
	if(dwBytesInCache==0)
		{
		*lpNumberOfBytesRead=0; /*EOF*/
		return true;
		}
	if(dwFilePos+nNumberOfBytesToRead>dwBytesInCache)
		{
		if(dwBytesInCache-dwFilePos>0)
			{
			memcpy(lpBuffer, &CacheBuff[dwFilePos], dwBytesInCache-dwFilePos);
			}
		dwRead = dwBytesInCache-dwFilePos;
		dwRestOfRead = nNumberOfBytesToRead-dwRead;
		ReadFileToCache();
		ReadFile(((char *)lpBuffer)+dwRead,  dwRestOfRead,  &dwRead1);
		*lpNumberOfBytesRead = dwRead1+dwRead;
		}
	else
		{
		memcpy(lpBuffer, &CacheBuff[dwFilePos], nNumberOfBytesToRead);
		dwFilePos+=nNumberOfBytesToRead;
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
		}
	return true;
	}

CReadFileCache::~CReadFileCache()
	{
	if(CacheBuff!=NULL) delete[] CacheBuff;
	}
bool CReadFileCache::ReadFileToCache()
	{
	int iRead=0;
	dwFilePos=0;
	dwBytesInCache=0;

	iRead = read(this->hF, CacheBuff, nCacheSize);
	if(iRead<=0)
		{
		return FALSE;
		}
	dwBytesInCache=iRead;
	return TRUE;
	}