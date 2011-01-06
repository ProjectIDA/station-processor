// WfdiscRecord.cpp: implementation of the CWfdiscRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataView.h"
#include "WfdiscRecord.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CopyStringWithBlankRemove(LPSTR lpTar, LPSTR lpSou);
int cssio_wrdsize(char *datatype);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CWfdiscRecord::CWfdiscRecord()
	{
	wfd=wfdisc_null;
	nCacheFirstCount=0;
	nCacheMaximumSize=100000;//counts
	nCacheSize=0;
	lData=NULL;
	}

CWfdiscRecord::CWfdiscRecord(struct wfdisc wfd)
	{
	this->wfd=wfd;
	nCacheFirstCount=0;
	nCacheMaximumSize=100000;//counts
	nCacheSize=0;
	lData=NULL;
	}

CWfdiscRecord::~CWfdiscRecord()
	{
	if(lData!=NULL) delete[]lData;
	}
long CWfdiscRecord::LoadData(int nDesCount, int nMaxCounts)
	{
	int nType,i;
	int nWordSize=cssio_wrdsize(wfd.datatype);
	int nSamples=wfd.nsamp;
	void *vData;
	long *llData; short *shData;
	int FirstCount, nTotalCounts;


	if(nDesCount>=nSamples) return 0;

	FirstCount=nDesCount;

	if(nDesCount+nCacheMaximumSize>nSamples)
		{
		nTotalCounts=nSamples-nDesCount;
		}
	else
		{
		nTotalCounts=nCacheMaximumSize;
		}
	if(nMaxCounts>0)
		if(nTotalCounts>nMaxCounts) 
			{
			nTotalCounts=nMaxCounts;
			}
	if(lData!=NULL) delete []lData;
	lData=new long[nTotalCounts*sizeof(long)];

	nCacheSize=nTotalCounts;
	nCacheFirstCount=FirstCount;


	vData=new char[nWordSize*nCacheSize];
	cssFile.Seek(wfd.foff+nCacheFirstCount*nWordSize,CFile::begin);
	cssFile.Read(vData,nWordSize*nCacheSize);


	if(lstrcmp(wfd.datatype,"i2")==0) nType=0;
	else if(lstrcmp(wfd.datatype,"i4")==0) nType=1;
	else if(lstrcmp(wfd.datatype,"s2")==0) nType=2;
	else if(lstrcmp(wfd.datatype,"s4")==0) nType=3;
	else nType=-1;

	if(nType==-1)
		{
		if(lData!=NULL) delete[] lData;
		delete[] vData;
		return -1;
		}
	
	switch(nType)
		{
		case 0:
			shData=(short *)vData;
			for(i=0; i<nCacheSize; ++i) lData[i]=shData[i];
			break;
		case 1:
			llData=(long *)vData;
			for(i=0; i<nCacheSize; ++i) lData[i]=llData[i];
			break;
		case 2:
			shData=(short *)vData;
			util_sswap(shData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=shData[i];
			break;
		case 3:
			llData=(long *)vData;
			util_lswap(llData, nCacheSize);
			for(i=0; i<nCacheSize; ++i) lData[i]=llData[i];
			break;
		}
	delete[] vData;

	return nCacheSize;
	}
long CWfdiscRecord::GetCount(int n)
	{
	if( (n>=nCacheFirstCount) && (n<nCacheFirstCount+nCacheSize) )
		{
		long count=lData[n-nCacheFirstCount];
		return count;
		}
	else
		{
		LoadData(n);
		return GetCount(n);
		}
	}
bool CWfdiscRecord::OpenDataSet()
	{
	char cFullPath[_MAX_PATH],cTmp[_MAX_PATH];
	CFileException fileException;

	CopyStringWithBlankRemove(cTmp,wfd.dir);


	if(lstrcmp(cTmp,".")==0)
		{
		*cFullPath=0;
		}
	else
		{
		lstrcpy(cFullPath,cTmp);
		lstrcat(cFullPath,"\\");
		}
	lstrcat(cFullPath,wfd.dfile);


	if(!cssFile.Open(cFullPath,CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary,&fileException))
		{
		return FALSE;
		}
	return true;
	}
bool CWfdiscRecord::CloseDataSet()
	{
	try
		{
		delete []lData;
		lData=NULL;
		cssFile.Close();
		}
	catch(CFileException fileException)
		{
		return false;
		}
	return true;
	}