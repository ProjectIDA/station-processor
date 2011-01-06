#include "stdafx.h"
#include "Aspiscsi.h"
/*#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
*/
//#include "wnaspi32.h"
//#include "scsidefs.h"

//static bool bInitSuccess=false;

//static	HANDLE ASPICompletionEvent=NULL;
//static	DWORD ASPIEventStatus=NULL;
//static	BYTE NumAdapters;

//static HINSTANCE hlibAPI = 0;
LPSENDASPI32COMMAND lpfnSendASPI32Command;
LPGETASPI32SUPPORTINFO lpfnGetASPI32SupportInfo;
 
//bool GetDeviceInfo(	int target_id, int adapter_id, int lun_id, unsigned char *, int nBuffSize);

CAspiscsi::CAspiscsi()	// standard constructor
{
	m_bInitSuccess = false;
	m_ASPICompletionEvent = NULL;
	m_ASPIEventStatus = NULL;
	m_hlibAPI = NULL;
	m_NumAdapters = 0;
	m_pDevList = NULL;
	m_ReadError = 0;
}

CAspiscsi::~CAspiscsi()	// standard disconstructor
{
	PDEVLIST pDVL;
	pDVL = m_pDevList;
	if (m_pDevList != NULL)
		while (pDVL != NULL)
		{
			pDVL = (PDEVLIST)m_pDevList->pNext;
			delete m_pDevList;
			m_pDevList = pDVL;
		}
	FreeLibrary(m_hlibAPI);
	CloseHandle(m_ASPICompletionEvent);
}


bool CAspiscsi::InitASPI()
{

	UINT fuError;
	//int status;
	DWORD ASPIStatus;

	fuError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	/*HANDLE hnd = CreateFile("\\\\.\\Tape0",
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);
	if (hnd == 0)
	{
		status = GetLastError();
		return FALSE;
	}
	
	status = GetTapeStatus(hnd);
	
*/
	
	
	
//	CloseHandle(hnd);
	m_hlibAPI = LoadLibrary("wnaspi32.dll");
	SetErrorMode(fuError);

	if(m_hlibAPI==NULL) return FALSE;

	m_lpfnSendASPI32Command=(LPSENDASPI32COMMAND)GetProcAddress (m_hlibAPI,"SendASPI32Command");
	if(m_lpfnSendASPI32Command==NULL) 
	{
		int status = GetLastError();
	//	return FALSE;
	}
	m_lpfnGetASPI32SupportInfo=(LPGETASPI32SUPPORTINFO)GetProcAddress (m_hlibAPI,"GetASPI32SupportInfo");
	if(m_lpfnGetASPI32SupportInfo==NULL)
	{
		int status = GetLastError();
		return FALSE;
	}

	ASPIStatus = GetASPI32SupportInfo();
	switch ( HIBYTE(LOWORD(ASPIStatus)) )
	{
	case SS_COMP:
   /*
    * ASPI for Win32 is properly initialized
    */
		m_NumAdapters = LOBYTE(LOWORD(ASPIStatus));
		break;

	default:
//		MessageBox(NULL,"ASPI for Win32 is not initialized!!",NULL,MB_ICONSTOP);
		return FALSE;
	}
   
	if(m_ASPICompletionEvent==NULL)
	{
		m_ASPICompletionEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	}

	SRB_HAInquiry MySRBi;

	MySRBi.SRB_Cmd     = SC_HA_INQUIRY;
	MySRBi.SRB_HaId    = 0;
	MySRBi.SRB_Flags    = 0;
	MySRBi.SRB_Hdr_Rsvd   = 0;
	ASPIStatus     = SendASPI32Command ( (LPSRB) &MySRBi );

	m_bInitSuccess=true;
	return TRUE;
}
PDEVLIST CAspiscsi::ScanSCSI(HWND hwnd)
{          
	static WORD  SRBIsPending=FALSE;
	static BYTE  target_id=0,adapter_id=0,lun_id=0;
	static char  InquiryBuffer[100];
	static  SRB_GDEVBlock ExecSRB;
	PDEVLIST pDVL = NULL, pTail = NULL;
	DWORD ASPIStatus;
	char string_buf[4096], info_buf[100]={0};
	int nAdapters=0; //number of tape devices found
   
	for(adapter_id=0; adapter_id < m_NumAdapters; ++adapter_id)
	{
		for(target_id=0; target_id<8; ++target_id)
		{
			for(lun_id=0; lun_id<8; ++lun_id)
			{
				char info[100]={0};
		
				memset((void*)&ExecSRB,0,sizeof(SRB_GDEVBlock));

				ExecSRB.SRB_Cmd    = SC_GET_DEV_TYPE;
				ExecSRB.SRB_HaId   = adapter_id;
				ExecSRB.SRB_Flags   = 0;
				ExecSRB.SRB_Hdr_Rsvd  = 0;
				ExecSRB.SRB_Target   = target_id;
				ExecSRB.SRB_Lun   = lun_id;

				ASPIStatus     = SendASPI32Command ( (LPSRB) &ExecSRB );
				if (ExecSRB.SRB_Status==SS_COMP)
				{
					if (ExecSRB.SRB_DeviceType!=DTYPE_SEQD) 
						continue;
					wsprintf ( string_buf,
					"Host Adapter #%d - SCSI ID #%d:  ",
					(BYTE) ExecSRB.SRB_HaId,
					(BYTE) ExecSRB.SRB_Target);
					GetDeviceInfo(target_id, adapter_id, lun_id, (unsigned char *)info_buf, sizeof(info_buf));
 					lstrcat(string_buf,info_buf);
					pDVL=new DEVLIST();

					pDVL->adapter=adapter_id;
					pDVL->lun=lun_id;
	 				pDVL->target=target_id;
					pDVL->pNext=NULL;
					pDVL->device = nAdapters;
					if(m_pDevList == NULL)
					{
						m_pDevList = pDVL;
						pTail=pDVL;
					}
					else
					{
						pTail->pNext = pDVL;
						pTail=pDVL;
					}
					nAdapters++;
				}
			}
		}
	}
	m_NumAdapters=nAdapters; //Update Number of scsi adapters to number of tape devices
	return m_pDevList;
}

bool CAspiscsi::GetDeviceInfo(	int target_id, int adapter_id, int lun_id, unsigned char * lpBuff, int nBuffSize)
	{
	SRB_ExecSCSICmd ExecSRB;
	DWORD ASPIStatus;
	unsigned char buf[100];

	memset((void*)&ExecSRB,0,sizeof(SRB_ExecSCSICmd));

	ExecSRB.SRB_Cmd     = SC_EXEC_SCSI_CMD;
	ExecSRB.SRB_HaId     = adapter_id;
	ExecSRB.SRB_Flags    = SRB_DIR_IN |SRB_EVENT_NOTIFY ;
	ExecSRB.SRB_Target    = target_id;
	ExecSRB.SRB_Lun   = lun_id;
	ExecSRB.SRB_BufPointer   = buf;
	ExecSRB.SRB_BufLen    = 32;
	ExecSRB.SRB_SenseLen   = SENSE_LEN;
	ExecSRB.SRB_CDBLen    = 6;
	ExecSRB.CDBByte[0]    = 0x12; 
	ExecSRB.CDBByte[1]    = 0;
	ExecSRB.CDBByte[2]    = 0;
	ExecSRB.CDBByte[3]    = 0;
	ExecSRB.CDBByte[4]    = 32;
	ExecSRB.CDBByte[5]    = 0;
	ExecSRB.SRB_PostProc=(void (__cdecl *)(void))m_ASPICompletionEvent;

	ASPIStatus     = SendASPI32Command ( (LPSRB) &ExecSRB );
  	m_ASPIEventStatus = WaitForSingleObject(m_ASPICompletionEvent,5000);
	switch(m_ASPIEventStatus)
	{
	case WAIT_OBJECT_0:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_ABANDONED:
		ResetEvent(m_ASPICompletionEvent);
		return false;
	case WAIT_TIMEOUT:
		ResetEvent(m_ASPICompletionEvent);
		return false;
	}
	if(ExecSRB.SRB_Status==SS_COMP)	
		return true;
	return false;
}
PSCSI CAspiscsi::Open(LPCSTR dev)
{
	//if ASPI has not been initialize, try to initialize it here
	if (!m_bInitSuccess)
		if(!InitASPI()) 
			return NULL;
	PSCSI tp=new SCSI;
	tp->nQty=0;
	tp->nRead=0;
	sscanf(dev,"%d%*[:]%d%*[:]%d",&tp->adapter,&tp->target_id,&tp->lun);
//	if(Reset(tp->adapter, tp->target_id, tp->lun)<0)
//		return NULL;
//	if(Rewind(tp)<0) 
//		return NULL;
	return tp;
}
void CAspiscsi::Close(PSCSI pSCSI)
{
	delete pSCSI;
}
//
//  Read from Tap drive
int CAspiscsi::Read(SCSI *tp, unsigned char *buffer, int count)
{
	int nrd=0;
	if(tp->nQty==0)
	{
		memset(tp->buf,0,sizeof(tp->buf));
		int nb=SCSIRead(tp->adapter, tp->target_id, tp->lun, tp->buf, sizeof(tp->buf));
//		int nb=SCSIRead(tp->adapter, tp->target_id, tp->lun, buffer, count);
//		
		if(nb==0) return 0;
		tp->nQty=nb;
		tp->nRead=0;
	}
	for(int i=0; i<count; ++i)
	{
		buffer[i]=tp->buf[tp->nRead++];
		--tp->nQty;
		++nrd;
		if(tp->nQty==0) break;
	}
	return nrd;
//	return nb;
}
/*****************************************************************************

	Write to SCSI Device. Return Error code - 0 = no errors

 ******************************************************************************/
int CAspiscsi::Write(SCSI *tp, unsigned char *buffer, int count)
{
	int Error = SCSIWrite(tp->adapter, tp->target_id, tp->lun, buffer, count);
	return Error;
}


/*************************************************************************************************************

	Function: Reset

	

  ************************************************************************************************************/
int CAspiscsi::Reset(PSCSI tp)
{
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;


	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=tp->adapter;
	SCSICmd.SRB_Flags= SRB_EVENT_NOTIFY | SRB_DIR_OUT;
	SCSICmd.SRB_Target=tp->target_id;
	SCSICmd.SRB_Lun=tp->lun;
	SCSICmd.SRB_BufLen=0;
	SCSICmd.SRB_BufPointer=0;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))m_ASPICompletionEvent;


    SCSICmd.CDBByte[0]       = 0x01; //reset
    SCSICmd.CDBByte[1]       = tp->lun;


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	m_ASPIEventStatus = WaitForSingleObject(m_ASPICompletionEvent,INFINITE);

	switch(m_ASPIEventStatus)
	{
	case WAIT_OBJECT_0:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_ABANDONED:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_TIMEOUT:
		ResetEvent(m_ASPICompletionEvent);
		break;
	}

	int i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	return i;
}
/*************************************************************************************************************

	Function: Rewind

	Rewind the Dat Drive

  ************************************************************************************************************/

int CAspiscsi::Rewind(PSCSI tp)
{
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0;
	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId = tp->adapter;
	SCSICmd.SRB_Flags = SRB_EVENT_NOTIFY | SRB_DIR_IN;
	SCSICmd.SRB_Target = tp->target_id;
	SCSICmd.SRB_Lun = tp->lun;
	SCSICmd.SRB_BufLen=0;
	SCSICmd.SRB_BufPointer=0;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))m_ASPICompletionEvent;


    SCSICmd.CDBByte[0]       = 0x01; //rewind
    SCSICmd.CDBByte[1]       = tp->lun;


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	m_ASPIEventStatus = WaitForSingleObject(m_ASPICompletionEvent,50000);

	switch(m_ASPIEventStatus)
	{
	case WAIT_OBJECT_0:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_ABANDONED:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_TIMEOUT:
		ResetEvent(m_ASPICompletionEvent);
		break;
	}
	int i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);
	return i;
}

/*************************************************************************************************************

	Function: ABORTS AN ACTIVE SRB COMMAND

	

  ************************************************************************************************************/
int CAspiscsi::Abort(PSCSI tp)
{
	DWORD ASPIStatus,ASPIAbortStatus;
	SRB_ExecSCSICmd SCSICmd;
	HANDLE hASPIAbortEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

return 0;
	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_ABORT_SRB;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=tp->adapter;
	SCSICmd.SRB_Flags= SRB_EVENT_NOTIFY | SRB_DIR_OUT;
	SCSICmd.SRB_Target=tp->target_id;
	SCSICmd.SRB_Lun=tp->lun;
	SCSICmd.SRB_BufLen=0;
	SCSICmd.SRB_BufPointer=0;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))hASPIAbortEvent;


    SCSICmd.CDBByte[0]       = 0x01; //reset
    SCSICmd.CDBByte[1]       = tp->lun;


	ASPIStatus		= SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIAbortStatus = WaitForSingleObject(hASPIAbortEvent,INFINITE);

	int i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	return i;
}
/*************************************************************************************************************

	Function: SCSIRead

	Called by Read to read data from the Dat drive

  ************************************************************************************************************/
int CAspiscsi::SCSIRead(int adapter, int target_id, int lun, PUCHAR buf, int len)
{
	_rdwr *rdwr;
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0;
	bool bRetry= true;

	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=adapter;
	SCSICmd.SRB_Flags=SRB_DIR_IN | SRB_EVENT_NOTIFY;
	SCSICmd.SRB_Target=target_id;
	SCSICmd.SRB_Lun=lun;
	SCSICmd.SRB_BufLen=len;
	SCSICmd.SRB_BufPointer=buf;
	SCSICmd.SRB_SenseLen=SENSE_LEN;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))m_ASPICompletionEvent;
	rdwr=(_rdwr *)&SCSICmd.CDBByte;
	rdwr->cmd = 0x08;
	rdwr->fixed = 0;
	rdwr->sili = 0;
	rdwr->lun = lun;
	rdwr->len_0 = (char)(len & 0xff);
	rdwr->len_1 = (char)((len>>8) & 0xff);
	rdwr->len_2 = (char)((len>>16) & 0xff);


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	m_ASPIEventStatus = WaitForSingleObject(m_ASPICompletionEvent,INFINITE);


	switch(m_ASPIEventStatus)
	{
	case WAIT_OBJECT_0:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_ABANDONED:
		ResetEvent(m_ASPICompletionEvent);
		break;
	case WAIT_TIMEOUT:
		ResetEvent(m_ASPICompletionEvent);
		break;
	}
    
    int i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);
	m_ReadError = i;
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

/*************************************************************************************************************

	Function: SCSIWrite

	Called by Write to send data to the Dat drive

  ************************************************************************************************************/

int CAspiscsi::SCSIWrite(int adapter, int target_id, int lun, BYTE *buf, int len)
{
	DWORD ASPIStatus;
	SRB_ExecSCSICmd SCSICmd;
	int fixed=0;


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
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))m_ASPICompletionEvent;
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
  	m_ASPIEventStatus = WaitForSingleObject(m_ASPICompletionEvent,20000);


	switch(m_ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(m_ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(m_ASPICompletionEvent);
			break;
		case WAIT_TIMEOUT:
			ResetEvent(m_ASPICompletionEvent);
			break;
		}

    int i = ScsiIoError(SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,
                    SCSICmd.SRB_TargStat, SCSICmd.SenseArea);

	//m_WriteError = i;
	
//	char cBuff[128];
//	sprintf(cBuff,"\n %X %X %X",SCSICmd.SRB_Status, SCSICmd.SRB_HaStat,  SCSICmd.SRB_TargStat);
	if(SCSICmd.SRB_Status!=1 || SCSICmd.SRB_HaStat!=0 || SCSICmd.SRB_TargStat!=0) return -1;

	return i;
}

/********************************************************************************************************

  Function: Eject()

  Eject the Tape cartridge;

  *******************************************************************************************************/
int CAspiscsi::Eject(PSCSI tp)
{
	//how do I do this
	DWORD ASPIStatus;
	HANDLE hASPIEjectEvent;
	SRB_ExecSCSICmd SCSICmd;
	memset((void*)&SCSICmd,0,sizeof(SCSICmd));
	hASPIEjectEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	SCSICmd.SRB_Cmd=SC_EXEC_SCSI_CMD;
	SCSICmd.SRB_Status=0;
	SCSICmd.SRB_HaId=tp->adapter;
	SCSICmd.SRB_Flags=SRB_DIR_OUT | SRB_EVENT_NOTIFY;
	SCSICmd.SRB_Target=tp->target_id;
	SCSICmd.SRB_Lun=tp->lun;
	//SCSICmd.SRB_BufLen=len;
	//SCSICmd.SRB_BufPointer=buf;
	SCSICmd.SRB_SenseLen=SENSE_LEN+2;
	SCSICmd.SRB_CDBLen=6;
	SCSICmd.SRB_PostProc=(void (__cdecl *)(void))hASPIEjectEvent;


    SCSICmd.CDBByte[0]       = SCSI_LOAD_UN;
    SCSICmd.CDBByte[1]       = tp->lun;


	ASPIStatus     = SendASPI32Command ( (LPSRB) &SCSICmd );
  	ASPIStatus = WaitForSingleObject(hASPIEjectEvent,20000);
	CloseHandle(hASPIEjectEvent);
	return (ASPIStatus);
}
/********************************************************************************************************

  Function: ScsiIoError

	Input:  aspi_status = SRB_Status,
			adapter_status = SRB_HaStat,
			target_status = SRB_TargStat,
			*sense = Char *SenceArea
*********************************************************************************************************/
int CAspiscsi::ScsiIoError(
	int aspi_status, 
	int adapter_status,
	int target_status,
	BYTE *sense)
{
	int i=0;
	switch ( aspi_status ) 
	{   /* check the aspi status */
	case 0x82:
		i = E$NoDevice;		// The device is not installed -5
		break;
	case 0x80:
		i = E$AspiInval;	//Invalid ASPI request -6
		break;
	case 0x81:
		i = E$NoAdapter;	//Invalid Host Adapter Number -7
		break;
	case 0x02:
		i = E$Abort;		//ASPI request aborted by Host -8
		break;
	case 0x04:
	/*
	 * Ok, this means scsi-io-error, so we see if
	 * we can find more details about the error.
	 */
		i = E$IOErr;	//default to An IO Error of unknown type -25
		if ( adapter_status ) 
		{
			switch ( adapter_status )  
			{
			case 0x11:
				i = E$SelTimeout;	// Selection Timeout -9
				break;
			case 0x12:
				i = E$DataOverrun;	// Data over-run/under-run - 10
				break;
			case 0x13:
				i = E$BusFree;		// Aspi Module is busy (this should never occur -11
				break;
			case 0x14:
				i = E$BusFail;		// Target bus phase sequence failure -12
				break;
			}
		} 
		else 
			switch ( target_status ) //SRB.Targstat 
			{
			/*
			 * If the adapter didn't show any errors
			 * we going to check the target to see if
			 * the target was the source of the error.
			 */
			case 0x08:                         
				i = E$TargetBusy;	// The specified Target/LUN is busy -13	
				break;
			case 0x18:
				i = E$Reservation;	// Reservation conflict -14
				break;
			case 0x02: //??
				/* 
				 * Allright, the target has send
				 * sense data to the sense-data allocation
				 * area at the end of the srb.
				*/
				if ( sense != ( BYTE *) 0 ) 
					switch ( sense[2] & 0x0f )  
					{
					case 0x00:  /* This means "no sense", but we check for End of Medium */
						if ( sense[2] & 0x40 )
						{
							i = E$EndOfMedium;	//end of medium -16
							TRACE("%d\n",i);
						}
						break;
					case 0x01:
						i = E$NoErr;/* this would be pretty stupid to report, but anyway */
						break;
					case 0x02:
						i = E$NotReady;
						TRACE("Device Not Ready\n");	//-15
						break;
					case 0x03:
						i = E$Medium;			//Medium Error -16
						break;
					case 0x04:
						i = E$Hardware;			//Non recoverable hardware error -17
						break;
					case 0x05:
						i = E$IllegalReq;		//Illegal Request -18
						break;
					case 0x06:
						i = E$UnitAttention;	//Unit Attention  - 19
						break;
					case 0x07:
						i = E$DataProtect;		// The block is protected -20
						break;
					case 0x08:
						i = E$BlankCheck;		//Encounterd non blank data -21
						break;
					case 0x0b:
						i = E$TargetAbort;		// The Target Aborted the command -22
						break;
					case 0x0d:
						i = E$VolOverflow;		// Volume overflow -23
						break;
					}
				break;
			}
		break;
	}
	return(i);
}

