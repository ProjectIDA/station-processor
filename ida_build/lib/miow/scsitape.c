#include "platform.h"
#include "win32/wnaspi32.h"
#include "win32/scsitape.h"
#include "win32/devlist.h"
#include "win32/scsidefs.h"


int SCSIRead(int adapter, int target_id, int lun, BYTE *buf, int len);
int SCSIWrite(int adapter, int target_id, int lun, BYTE *buf, int len);
int SCSIRewind(int adapter, int target_id, int lun);
static int ScsiIoError(int aspi_status, int adapter_status, int target_status, BYTE *sense);
int SCSIReset(int adapter, int target_id, int lun);


static BOOL bInitSuccess=FALSE;

static HANDLE ASPICompletionEvent=NULL;
static DWORD ASPIEventStatus=0;

static HINSTANCE hlibAPI = 0;
LPSENDASPI32COMMAND lpfnSendASPI32Command;
LPGETASPI32SUPPORTINFO lpfnGetASPI32SupportInfo;


BOOL InitASPI()
	{

	UINT fuError;

	fuError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	hlibAPI = LoadLibrary("wnaspi32.dll");
	SetErrorMode(fuError);

	lpfnSendASPI32Command=(LPSENDASPI32COMMAND)GetProcAddress (hlibAPI,"SendASPI32Command");
	lpfnGetASPI32SupportInfo=(LPGETASPI32SUPPORTINFO)GetProcAddress (hlibAPI,"GetASPI32SupportInfo");

	if(ASPICompletionEvent==NULL)
		{
		ASPICompletionEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		}


	{
	DWORD ASPIStatus;
	BYTE NumAdapters;
	ASPIStatus = GetASPI32SupportInfo();
	switch ( HIBYTE(LOWORD(ASPIStatus)) )
		{
		case SS_COMP:
   /*
    * ASPI for Win32 is properly initialized
    */
		NumAdapters = LOBYTE(LOWORD(ASPIStatus));
		break;
		default:
		fprintf(stderr,"ASPI for Win32 is not initialized!!");
		bInitSuccess=FALSE;
		return FALSE;
		}
   
	}		
	bInitSuccess=TRUE;
	return TRUE;
	}
void CloseASPI()
	{
	if(bInitSuccess)
		{
		FreeLibrary(hlibAPI);
		CloseHandle(ASPICompletionEvent);
		ASPICompletionEvent=NULL;
		ASPIEventStatus=0;
		hlibAPI=NULL;
		}
    }

int SCSIRead(int adapter, int target_id, int lun, BYTE *buf, int len)
	{
	_rdwr *rdwr;
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0, i;


	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=adapter;
	SCSICmd.SRB_Flags=SRB_DIR_IN | SRB_EVENT_NOTIFY;
	SCSICmd.SRB_Target=target_id;
	SCSICmd.SRB_Lun=lun;
	SCSICmd.SRB_BufLen=len;
	SCSICmd.SRB_BufPointer=buf;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))ASPICompletionEvent;
	rdwr=(_rdwr *)&SCSICmd.CDBByte;
	rdwr->cmd = 0x08;
	rdwr->fixed = 0;
	rdwr->sili = 0;
	rdwr->lun = lun;
	rdwr->len_0 = (char)(len & 0xff);
	rdwr->len_1 = (char)((len>>8) & 0xff);
	rdwr->len_2 = (char)((len>>16) & 0xff);


/*    SCSICmd.CDBByte[0]       = 0x08;
    SCSICmd.CDBByte[1]       = 0x00;
    SCSICmd.CDBByte[2]       = (len & 0x00ff0000) >> 16;
    SCSICmd.CDBByte[3]       = (len & 0x0000ff00) >>  8;
    SCSICmd.CDBByte[4]       = (len & 0x000000ff);*/



	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,50000);


	switch(ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_TIMEOUT:
			ResetEvent(ASPICompletionEvent);
			break;
		}
    i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	

    if ( i )  
		{  /* If any error ... */
		unsigned bytes_read;

		if ( i == E$BlankCheck )
			{  /* EEOM  (early end of medium) */
			if ( (SCSICmd.SenseArea[2] & 0x40) && (SCSICmd.SenseArea[0] &0x80) ) 
				{
          /* compute the number of bytes read */
				bytes_read =  (WORD)(unsigned char)SCSICmd.SenseArea[6] + (((unsigned char)SCSICmd.SenseArea[5])<<8);
				return ( int ) len-bytes_read;
				} 
				else 
					return 0;
			}
		else 
			if ( (i==E$Medium) && (SCSICmd.SenseArea[2] & 0x40) && (SCSICmd.SenseArea[0] & 0x80) ) 
				{
				bytes_read =  (WORD)(unsigned char)SCSICmd.SenseArea[6] + (((unsigned char)SCSICmd.SenseArea[5])<<8);
				return ( int ) len-bytes_read;
				}
			else  
				{
				bytes_read =  (WORD)(unsigned char)SCSICmd.SenseArea[6] + (((unsigned char)SCSICmd.SenseArea[5])<<8);
				return len-bytes_read;
				}
		}
    return len;
  }



int SCSIWrite(int adapter, int target_id, int lun, BYTE *buf, int len)
	{
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0;
	int i;


	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=adapter;
	SCSICmd.SRB_Flags=SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	SCSICmd.SRB_Target=target_id;
	SCSICmd.SRB_Lun=lun;
	SCSICmd.SRB_BufLen=len;
	SCSICmd.SRB_BufPointer=buf;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))ASPICompletionEvent;
/*	rdwr=(_rdwr *)&SCSICmd.CDBByte;
	rdwr->cmd = 0x08;
	rdwr->fixed = 0;
	rdwr->sili = 0;
	rdwr->lun = lun;
	rdwr->len_0 = (char)(length & 0xff);
	rdwr->len_1 = (char)((length>>8) & 0xff);
	rdwr->len_2 = (char)((length>>16) & 0xff);*/


    SCSICmd.CDBByte[0]       = 0x0a;
    SCSICmd.CDBByte[1]       = lun;
    SCSICmd.CDBByte[2]       = (len & 0x00ff0000) >> 16;
    SCSICmd.CDBByte[3]       = (len & 0x0000ff00) >>  8;
    SCSICmd.CDBByte[4]       = (len & 0x000000ff);



	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,20000);


	switch(ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_TIMEOUT:
			ResetEvent(ASPICompletionEvent);
			break;
		}

    i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);


	
//	char cBuff[128];
//	sprintf(cBuff,"\n %X %X %X",SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,  SCSICmd.SRB_TargStat);
	if(SCSICmd.SRB_Status!=1 || SCSICmd.SRB_HaStat!=0 || SCSICmd.SRB_TargStat!=0) return -1;

	return i;
	}

int SCSIRewind(int adapter, int target_id, int lun)
	{
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0;
	int i;


	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=adapter;
	SCSICmd.SRB_Flags= SRB_EVENT_NOTIFY | SRB_DIR_IN;
	SCSICmd.SRB_Target=target_id;
	SCSICmd.SRB_Lun=lun;
	SCSICmd.SRB_BufLen=0;
	SCSICmd.SRB_BufPointer=0;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))ASPICompletionEvent;


    SCSICmd.CDBByte[0]       = 0x01;
    SCSICmd.CDBByte[1]       = lun;


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,50000);

	switch(ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_TIMEOUT:
			ResetEvent(ASPICompletionEvent);
			break;
		}



	i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	return i;

	}

int SCSIReset(int adapter, int target_id, int lun)
	{
	int i;
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;


	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=adapter;
	SCSICmd.SRB_Flags= SRB_EVENT_NOTIFY | SRB_DIR_OUT;
	SCSICmd.SRB_Target=target_id;
	SCSICmd.SRB_Lun=lun;
	SCSICmd.SRB_BufLen=0;
	SCSICmd.SRB_BufPointer=0;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))ASPICompletionEvent;


    SCSICmd.CDBByte[0]       = 0x01;
    SCSICmd.CDBByte[1]       = lun;


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,50000);

	switch(ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_TIMEOUT:
			ResetEvent(ASPICompletionEvent);
			break;
		}



	i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	return i;

	}



static int ScsiIoError(int aspi_status, int adapter_status, int target_status, BYTE *sense)
{
  int i=0;

  switch ( aspi_status )  {   /* check the aspi status */
  case 0x82:
    i = E$NoDevice;
    break;
  case 0x80:
    i = E$AspiInval;
    break;
  case 0x81:
    i = E$NoAdapter;
    break;
  case 0x02:
    i = E$Abort;
    break;
  case 0x04:
    /*
     * Ok, this means scsi-io-error, so we see if
     * we can find more details about the error.
     */
    i = E$IOErr;
    if ( adapter_status )  {
      switch ( adapter_status )  {
      case 0x11:
	i = E$SelTimeout;
	break;
      case 0x12:
	i = E$DataOverrun;
	break;
      case 0x13:
	i = E$BusFree;
	break;
      case 0x14:
	i = E$BusFail;
	break;
      }
    } else switch ( target_status )  {
        /*
         * If the adapter didn't show any errors
         * we going to check the target to see if
         * the target was the source of the error.
         */
    case 0x08:                         
      i = E$TargetBusy;
      break;
    case 0x18:
      i = E$Reservation;
      break;
    case 0x02:
        /* 
         * Allright, the target has send
         * sense data to the sense-data allocation
         * area at the end of the srb.
	 */
      if ( sense != ( BYTE *) 0 ) switch ( sense[2] & 0x0f )  {
      case 0x00:  /* This means "no sense", but we check for End of Medium */
        if ( sense[2] & 0x40 ) i = E$EndOfMedium;
        break;
      case 0x01:
        i = E$NoErr;/* this would be pretty stupid to report, but anyway */
	break;
      case 0x02:
	i = E$NotReady;
	break;
      case 0x03:
	i = E$Medium;
	break;
      case 0x04:
	i = E$Hardware;
	break;
      case 0x05:
	i = E$IllegalReq;
	break;
      case 0x06:
        i = E$UnitAttention;
	break;
      case 0x07:
        i = E$DataProtect;
	break;
      case 0x08:
	i = E$BlankCheck;
	break;
      case 0x0b:
	i = E$TargetAbort;
	break;
      case 0x0d:
	i = E$VolOverflow;
	break;
      }
      break;
    }
    break;
  }
  return(i);
}

/* Revision History
 *
 * $Log: scsitape.c,v $
 * Revision 1.3  2006/06/26 22:40:42  dechavez
 * used 0 instead of NULL in assigning values to ASPIEventStatus
 *
 * Revision 1.2  2005/05/17 21:36:39  dechavez
 * 05-17 update
 *
 * Revision 1.1  2005/02/09 21:08:41  dechavez
 * initial (miow library) release
 *
 */
