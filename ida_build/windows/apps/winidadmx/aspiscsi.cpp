#include "stdafx.h"
#include "win32/wnaspi32.h"
#include "win32/scsidefs.h"
#include "devlist.h"
static bool bInitSuccess=false;

static	HANDLE ASPICompletionEvent=NULL;
static	DWORD ASPIEventStatus=NULL;
static	BYTE NumAdapters;

static HINSTANCE hlibAPI = 0;
LPSENDASPI32COMMAND lpfnSendASPI32Command;
LPGETASPI32SUPPORTINFO lpfnGetASPI32SupportInfo;

bool GetDeviceInfo(	int target_id, int adapter_id, int lun_id, unsigned char *, int nBuffSize);

void CloseASPI()
	{
	FreeLibrary(hlibAPI);
	CloseHandle(ASPICompletionEvent);
	}

bool InitASPI()
	{

	UINT fuError;
	DWORD ASPIStatus;

	fuError = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	hlibAPI = LoadLibrary("wnaspi32.dll");
	SetErrorMode(fuError);

	if(hlibAPI==NULL) return FALSE;

	lpfnSendASPI32Command=(LPSENDASPI32COMMAND)GetProcAddress (hlibAPI,"SendASPI32Command");
	if(lpfnSendASPI32Command==NULL) return FALSE;
	lpfnGetASPI32SupportInfo=(LPGETASPI32SUPPORTINFO)GetProcAddress (hlibAPI,"GetASPI32SupportInfo");
	if(lpfnGetASPI32SupportInfo==NULL) return FALSE;



	{
	DWORD ASPIStatus;
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
//		MessageBox(NULL,"ASPI for Win32 is not initialized!!",NULL,MB_ICONSTOP);
		return FALSE;
		}
   
	}		

	if(ASPICompletionEvent==NULL)
		{
		ASPICompletionEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		}

	SRB_HAInquiry MySRBi;

	MySRBi.SRB_Cmd     = SC_HA_INQUIRY;
	MySRBi.SRB_HaId    = 0;
	MySRBi.SRB_Flags    = 0;
	MySRBi.SRB_Hdr_Rsvd   = 0;
	ASPIStatus     = SendASPI32Command ( (LPSRB) &MySRBi );

	bInitSuccess=true;
	return TRUE;
	}
DevList *ScanSCSI(HWND hwnd)
	{          
	static WORD  SRBIsPending=FALSE;
	static BYTE  target_id=0,adapter_id=0,lun_id=0;
	static char  InquiryBuffer[100];
	static  SRB_GDEVBlock ExecSRB;
	DevList *dvList=NULL, *pTail=NULL;
	DWORD ASPIStatus;

   
	for(adapter_id=0; adapter_id<NumAdapters; ++adapter_id)
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
				if(ExecSRB.SRB_DeviceType!=DTYPE_SEQD) continue;
				char string_buf[4096], info_buf[100]={0};
				wsprintf ( string_buf,
				"Host Adapter #%d - SCSI ID #%d:  ",
				(BYTE) ExecSRB.SRB_HaId,
				(BYTE) ExecSRB.SRB_Target);
				GetDeviceInfo(target_id, adapter_id, lun_id, (unsigned char *)info_buf, sizeof(info_buf));
				lstrcat(string_buf,info_buf);
				DevList *pDVL=new DevList();

				pDVL->adapter=adapter_id;
				pDVL->lun=lun_id;
				pDVL->target=target_id;
				pDVL->pNext=NULL;

				if(dvList==NULL)
					{
					dvList=pDVL;
					pTail=pDVL;
					}
				else
					{
					pTail->pNext=pDVL;
					pTail=pDVL;
					}
				}
			else
				{
				}
  
				}
			}
		}
	return dvList;
  
	}

bool GetDeviceInfo(	int target_id, int adapter_id, int lun_id, unsigned char * lpBuff, int nBuffSize)
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
	ExecSRB.SRB_PostProc=(void (__cdecl *)(void))ASPICompletionEvent;

	ASPIStatus     = SendASPI32Command ( (LPSRB) &ExecSRB );
  	ASPIEventStatus = WaitForSingleObject(ASPICompletionEvent,5000);
	switch(ASPIEventStatus)
		{
		case WAIT_OBJECT_0:
			ResetEvent(ASPICompletionEvent);
			break;
		case WAIT_ABANDONED:
			ResetEvent(ASPICompletionEvent);
			return false;
		case WAIT_TIMEOUT:
			ResetEvent(ASPICompletionEvent);
			return false;
		}
	if(ExecSRB.SRB_Status==SS_COMP)	return true;
	return false;
	}
