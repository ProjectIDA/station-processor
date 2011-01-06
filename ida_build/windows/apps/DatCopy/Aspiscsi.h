#ifndef __aspiscsi__
#define __aspiscsi__ 1
#include "win32/wnaspi32.h"
#include "win32/scsidefs.h"
#include "win32/scsitape.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//extern LPSENDASPI32COMMAND lpfnSendASPI32Command;
typedef struct 
{
	int adapter;
	int lun;
	int target;
	PVOID pNext;
	int device;
	char descr[100];
}DEVLIST, *PDEVLIST;

typedef struct scsi_handle {
	int adapter;
	int target_id;
	int lun;
	unsigned char buf[32768];
	int nRead;
	int nQty;
} SCSI, *PSCSI;



/////////////////////////////////////////////////////////////////////////////
// CDatCopyDlg dialog
class CAspiscsi
{
// Construction
public:
	CAspiscsi();	// standard constructor
	~CAspiscsi();	// standard distructor
	bool InitASPI();
	bool GetDeviceInfo(	int target_id, int adapter_id, int lun_id, PUCHAR lpBuff, int nBuffSize);
	PDEVLIST ScanSCSI(HWND hwnd);
	void AddAdapter(int n=1)
		{m_NumAdapters += n;};
	BYTE GetNumAdapters()
		{return m_NumAdapters; };
	PSCSI Open(LPCSTR dev);
	void Close(PSCSI pSCSI);
	int Read(PSCSI , PUCHAR buffer, int count);
	int Write(PSCSI , PUCHAR buffer, int count);
	int Reset(PSCSI tp);
	int Rewind(PSCSI tp);
	int ScsiIoError
		(
			int aspi_status, 
			int adapter_status,
			int target_status,
			BYTE *sense
		);
	int SCSIRead(int adapter, int target_id, int lun, PUCHAR buf, int len);
	int SCSIWrite(int adapter, int target_id, int lun, PUCHAR buf, int len);
	int Eject(PSCSI);
	int Abort(PSCSI);
	int ReadError(){
		return m_ReadError;};
private:
	bool m_bInitSuccess;
	SRB_ExecSCSICmd m_SCSICmd;
	HANDLE m_ASPICompletionEvent;
	DWORD m_ASPIEventStatus;
	BYTE m_NumAdapters;
	PDEVLIST m_pDevList;
	HINSTANCE m_hlibAPI;
	LPSENDASPI32COMMAND m_lpfnSendASPI32Command;
	LPGETASPI32SUPPORTINFO m_lpfnGetASPI32SupportInfo;
	int m_ReadError;
};

#endif