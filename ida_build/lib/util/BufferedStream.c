#pragma ident "$Id: BufferedStream.c,v 1.5 2006/06/12 22:49:41 akimov Exp $"
/*======================================================================
 *
 * Portable buffered I/O routines.
 * Written by Andrei Akimov
 *
 *====================================================================*/
#include "util.h"

static LNKLST list;

int utilFlushBuffer(BufferedStream  *p);
/***************************************************/
/* Looking for existing stream control block       */
/* create new one if not found                     */
/*                                                 */
/* Input:                                          */
/*     pFileName  - stream(file) name              */  
/***************************************************/
BufferedStream  *utilOpenBufferedStream(char *pFileName)
	{
	int nLen;
	BufferedStream  *p;
	BufferedStream  obf;

	UINT32 count = 0;
	LNKLST_NODE *crnt;

    crnt = listFirstNode(&list);
    while (crnt != NULL) 
		{
		p = (BufferedStream  *)crnt->payload;
		if(strcmp(p->cFileName, pFileName)==0)
			{
			return p;
			}
        crnt = listNextNode(crnt);
		}
	
	nLen = strlen(pFileName);
	if(nLen<sizeof(obf.cFileName)-1)
		STRCPY(obf.cFileName, pFileName);
	else
		{
		strncpy(obf.cFileName, pFileName, sizeof(obf.cFileName)-1);
		obf.cFileName[sizeof(obf.cFileName)-1]=0;
		}

	obf.nBytes=0;
	obf.nTotalBytes=0;

	if(listAppend(&list, &obf, sizeof(obf)))
		{
		crnt = listLastNode(&list);
		p = (BufferedStream  *)crnt->payload;
		return p;
		}
	return NULL;
	}
/*****************************************************/
/* Inserts a specified number of bytes from a buffer */ 
/*  into the stream                                  */
/* return -1 on error or number of written bytes     */
/*                                                   */
/* Input:                                            */
/*     pFileName  - stream(file) name                */  
/*  BufferedStream  *p - pointer to the stream       */
/*                       control block               */
/*                                                   */
/*  char *pBuff   - poiter to an array to be written */
/*  int n         - number of characters to be       */
/*                  written                          */
/*****************************************************/
int utilWriteBufferedStream(BufferedStream  *p, char *pBuff, int n)
	{
	int i;
	if(p==NULL) return -1;
	for(i=0; i<n; ++i)
		{
		if(p->nBytes==UTIL_OUTPUTSTREAM_BUFFSIZE) 
			{
			if(utilFlushBuffer(p)!=1) return -1;
			}
		p->cBuff[p->nBytes++] = pBuff[i];
		}
	p->nTotalBytes += n;


	return n;
	}

int utilFlushBuffer(BufferedStream  *p)
	{
	static const int nerr=-1;
	int nf;
	int nbytes;
	if(p->nBytes > 0)
		{
		nf = open(p->cFileName, O_WRONLY|O_BINARY|O_CREAT|O_APPEND, 0666);

		if(nf==nerr) return nerr;
		nbytes = write(nf, p->cBuff, p->nBytes);
		close(nf);
		if(nbytes!=p->nBytes)
			{
			return nerr;
			}
		p->nBytes = 0;
		}
	return 1;
	}

int utilFlushAllBufferedStream()
	{
	UINT32 count = 0;
	LNKLST_NODE *crnt;
	BufferedStream  *p;

    crnt = listFirstNode(&list);
    while (crnt != NULL) 
		{
		p = (BufferedStream  *)crnt->payload;
		utilFlushBuffer(p);
        crnt = listNextNode(crnt);
		}
	return 0;
	}

void utilInitOutputStreamBuffers()
	{
	listInit(&list);
	}

void utilDestroyOutputStreamBuffers(void)
	{
	utilFlushAllBufferedStream();
	listDestroy(&list);
	}

#ifdef DEBUG_TEST

int main(int argc, char* argv[])
	{
	int i;
	BufferedStream  *p;
	char one[1024];
	time_t t1, t2;
	FILE *f;

	for(i=0; i<1024; ++i)
		{
		one[i] = i ; 
		}

	utilInitOutputStreamBuffers();


	t1 = time(NULL);
	for(i=0; i<100000; ++i)
		{
		p = utilOpenBufferedStream("c:\\test1");
		if(p!=NULL)
			{
			utilWriteBufferedStream(p, one, 1024);
			}
		}

	utilFlushAllBufferedStream();
	utilDestroyOutputStreamBuffers();
	t2 = time(NULL);

	printf("\nWriteBufferedStream time = %d", t2-t1);

	t1 = time(NULL);
	for(i=0; i<100000; ++i)
		{
		f = fopen("c:\\test2","a+b");
		fwrite( one, sizeof(char), 1024, f);
		fclose(f);
		}
	t2 = time(NULL);

	printf("\n fopen-fwrite-fclose time = %d", t2-t1);
	return 0;
	}
#endif

// do nothing change added to test akimov cvs
// multi-line to see what happens to newlines

/* Revision History
 *
 * $Log: BufferedStream.c,v $
 * Revision 1.5  2006/06/12 22:49:41  akimov
 * test of wincvs commit
 *
 * Revision 1.4  2006/02/14 17:05:30  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.3  2005/05/13 19:15:10  dechavez
 * 05-11 update
 *
 * Revision 1.2  2005/02/10 18:47:15  dechavez
 * changed lstrcpy to STRCPY macro for unix portablity
 *
 * Revision 1.1  2005/02/07 19:09:20  dechavez
 * initial release
 *
 */
