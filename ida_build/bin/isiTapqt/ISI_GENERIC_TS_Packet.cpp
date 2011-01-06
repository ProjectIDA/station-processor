#include "isi.h"
#include "util.h"
#include "ReadFileCache.h"

const static int nSizeOfHeader = sizeof(ISI_GENERIC_TSHDR);

int ISI_GENERIC_TS_WrtPacket(int nFileHandle,  ISI_GENERIC_TS *packet)
	{
	int nDataSize = packet->hdr.nbytes;

	if(write(nFileHandle,&packet->hdr, nSizeOfHeader)!=nSizeOfHeader)	return -1;
	if(write(nFileHandle,packet->data, nDataSize)!=nDataSize)	return -1;
	return nSizeOfHeader+nDataSize;
	}

int ISI_GENERIC_TS_RdPacket(CReadFileCache &FC, ISI_GENERIC_TS *packet)
	{
	long lNumberOfBytesRead;

	if(!FC.ReadFile(&packet->hdr, nSizeOfHeader, &lNumberOfBytesRead))	 return -1;
	if(lNumberOfBytesRead==0) return -1000;

	int nDataSize = packet->hdr.nbytes;
	packet->data = malloc( nDataSize );
	if(packet->data == NULL) return -2000;

	packet->precious=TRUE;
    packet->nalloc = nDataSize;
 


	if(!FC.ReadFile(packet->data, nDataSize, &lNumberOfBytesRead ))	 return -1;
	if(lNumberOfBytesRead==0) return -1000;
	return nDataSize+nSizeOfHeader;
	}
