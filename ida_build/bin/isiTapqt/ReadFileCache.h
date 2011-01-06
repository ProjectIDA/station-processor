// ReadFileCache.h: interface for the CReadFileCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_READFILECACHE_H_)
#define _READFILECACHE_H_


class CReadFileCache  
{
public:
	CReadFileCache();
	virtual ~CReadFileCache();
	bool ReadFileToCache();
	bool ReadFile(void *lpBuffer,  long nNumberOfBytesToRead,  long *lpNumberOfBytesRead);
	bool SetFileHandle(int hF);

	int nCacheSize;

private:
	char *CacheBuff;
	int hF;
	long dwFilePos;
	long dwBytesInCache;
};

#endif // !defined(_READFILECACHE_H_)
