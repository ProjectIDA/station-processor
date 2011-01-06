// Wfdisc.cpp: implementation of the CWfdisc class.
//
//////////////////////////////////////////////////////////////////////

#include "wfdisc.h"
#include "cssio.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWfdisc::CWfdisc()
	{
	wfd=wfdisc_null;
	hF = -1;
	}
CWfdisc::~CWfdisc()
	{
	CloseFileHandle();
	}
void CWfdisc::CloseFileHandle()
	{
	if(hF!=NULL)
		{
		close(hF);
		hF=NULL;
		}

	}
int CWfdisc::WriteWfdisk(const char *pName)
	{
	char cWfdStr[WFDISC_SIZE+4096];
	sprintf(cWfdStr,WFDISC_WCS,WFDISC_WVL(&wfd));
	FILE *pFile;
	if(wfd.nsamp==0) return 0;

	pFile = fopen(pName, "a");
	if(pFile == NULL)
		{
		return -1;
		}
	fwrite(cWfdStr,WFDISC_SIZE,1,pFile);
	fclose(pFile);
	if(hF!=NULL)
		{
		long dwPos = tell(hF);
		wfd.foff = dwPos;
		}

	return 0;
	}
int CWfdisc::WriteData(ISI_GENERIC_TS *packet, const char * datatype )
	{
	long NumberOfBytesWritten;
	int nsamp = packet->hdr.nsamp;
	long native_order = util_order();
	char path[MAXPATHLEN+1];

    sprintf(path, "%s%c%s", qsDir.latin1(),PATH_DELIMITER,wfd.dfile);


	if(hF<=0)
		{
		hF = open(path, O_WRONLY| O_BINARY| O_CREAT, 0666);
		}

	if( hF==-1) return -1;

	void *vBuff;
	short *shB;
	long *lB;
	int nType,i,countsize;
	long *data = (long *)packet->data;


	countsize=cssio_wrdsize((char *)datatype);

	if(strcmp(datatype,"i2")==0)      nType=0;
	else if(strcmp(datatype,"i4")==0) nType=1;
	else if(strcmp(datatype,"s2")==0) nType=2;
	else if(strcmp(datatype,"s4")==0) nType=3;
	else nType=1;


	switch(nType)
		{
		case 0:
			vBuff=shB=new short[nsamp];
			for(i=0; i<nsamp; ++i) shB[i]=(short)data[i];
			if (native_order == BIG_ENDIAN_ORDER) util_sswap(shB, nsamp);
			break;
		case 1:
			vBuff=lB=new long[nsamp];
			for(i=0; i<nsamp; ++i) lB[i]=data[i];
			if (native_order == BIG_ENDIAN_ORDER) util_lswap(lB, nsamp);
			break;
		case 2:
			vBuff=shB=new short[nsamp];
			for(i=0; i<nsamp; ++i) shB[i]=(short)data[i];
			if (native_order == LTL_ENDIAN_ORDER) util_sswap(shB, nsamp);
			break;
		case 3:
			vBuff=lB=new long[nsamp];
			for(i=0; i<nsamp; ++i) lB[i]=data[i];
			if (native_order == LTL_ENDIAN_ORDER) util_lswap(lB, nsamp);
			break;
		}
	
	NumberOfBytesWritten = write(hF, vBuff, countsize*nsamp);

	if(NumberOfBytesWritten<countsize*nsamp)
		{
		delete[] vBuff;
		return -1;
		}
	if(wfd.time==0) wfd.time = packet->hdr.tofs.value;
	wfd.nsamp  += nsamp;
	wfd.endtime = wfd.time+(wfd.nsamp-1)/wfd.smprate;

	delete[] vBuff;
	return NumberOfBytesWritten;
	}
void CWfdisc::SetWorkingDir(const QString &s)
	{
	qsDir = s;
	}