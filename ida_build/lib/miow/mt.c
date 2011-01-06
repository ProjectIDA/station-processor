#include "platform.h"
#include "win32\scsitape.h"
#include "mio.h"
#include "winmt.h"



int mtread(SCSI *tp, unsigned char *buffer, int count)
	{
	int i;
	int nrd=0, nb=0;
	memset(buffer,0,count);
	if(tp->nQty==0)
		{
		memset(tp->buf,0,sizeof(tp->buf));
		nb=SCSIRead(tp->adapter, tp->target_id, tp->lun, tp->buf, sizeof(tp->buf));
		if( (nb!=0) && (nb!=sizeof(tp->buf)))
			{
			if(nb%1024!=0) nb=(nb/1024+1)*1024;
			}
		if(nb==0)
			{
			nb=SCSIRead(tp->adapter, tp->target_id, tp->lun, tp->buf, sizeof(tp->buf));
			if(nb==0) 
				{
				return 0;
				}
			}
		tp->nQty=nb;
		tp->nRead=0;
		}
	
	for(i=0; i<count; ++i)
		{
		buffer[i]=tp->buf[tp->nRead++];
		--tp->nQty;
		++nrd;
		if(tp->nQty==0 && (count-nrd)>0 )
			{
			return (mtread(tp, &buffer[nrd], count-nrd)+nrd);
			}
		}
	return nrd;
	}
SCSI * mtopen(char *dev,char *cpMode)
	{
	SCSI *tp;
	int i;
	if(!InitASPI()) return NULL;
	tp=malloc(sizeof(SCSI));
	tp->nQty=0;
	tp->nRead=0;
	sscanf(&dev[5],"%d%*[:]%d%*[:]%d",&tp->adapter,&tp->target_id,&tp->lun);

	i=0;
	while(SCSIReset(tp->adapter, tp->target_id, tp->lun)<0)
		{
		if(++i==5) return NULL;
		}
	i=0;
	while(SCSIRewind(tp->adapter, tp->target_id, tp->lun)<0)
		{
		if(++i==5) return NULL;
		}

	return tp;
	}
void mtclose(SCSI *pSCSI)
	{
	CloseASPI();
	free(pSCSI);
	}

/* Revision History
 *
 * $Log: mt.c,v $
 * Revision 1.2  2005/05/17 21:36:39  dechavez
 * 05-17 update
 *
 * Revision 1.1  2005/02/09 21:08:41  dechavez
 * initial (miow library) release
 *
 */
