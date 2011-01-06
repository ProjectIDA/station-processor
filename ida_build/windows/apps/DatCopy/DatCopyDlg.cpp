// DatCopyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DatCopy.h"
#include "DatCopyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
UINT CopyThread(LPVOID Context);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatCopyDlg dialog

CDatCopyDlg::CDatCopyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDatCopyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDatCopyDlg)
	m_Status0 = _T("");
	m_Status1 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_DevList = NULL;
	m_Cancel = FALSE;
	m_Thread = NULL;
	m_nTimer = 0;
}

void CDatCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatCopyDlg)
	DDX_Text(pDX, IDC_STATUS0, m_Status0);
	DDX_Text(pDX, IDC_STATUS1, m_Status1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDatCopyDlg, CDialog)
	//{{AFX_MSG_MAP(CDatCopyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_DAC0, OnSelchangeDac0)
	ON_CBN_SELCHANGE(IDC_DAC1, OnSelchangeDac1)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatCopyDlg message handlers

BOOL CDatCopyDlg::OnInitDialog()
{
	PDEVLIST dv=NULL, pNext;
	CComboBox	*pDacCombo0;
	CComboBox   *pDacCombo1;
	CDialog::OnInitDialog();
	int x=0;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//Initialize aspi
	dv = NULL;
	pDacCombo0 = (CComboBox *)GetDlgItem(IDC_DAC0);
	pDacCombo1 = (CComboBox *)GetDlgItem(IDC_DAC1);
	if (!InitASPI())
	{
		AfxMessageBox("Could not initialize ASPI");
	}
	else
	{
		//scan for all SCSI devices.
		//dv will contain a list
		dv = ScanSCSI(m_hWnd);
	}	
	if(dv!=NULL)
		while(true)
		{
			char cBuff[120];
			pNext = (PDEVLIST)dv->pNext; //get the next device
			//sprintf(m_cBuff,"/dev/%d:%d:%d ", dv->adapter, dv->target, dv->lun); 
			//Create the device name used for opening the device
			m_DevName[x++].Format("%d:%d:%d ", dv->adapter, dv->target, dv->lun);
			sprintf(cBuff,"TAPE%d", dv->device); 
			pDacCombo0->AddString(cBuff);
			pDacCombo1->AddString(cBuff);
			if (x == MAX_DEVICES)
				break; //we only support number MAX_DEVICES
//			delete dv;
			dv=pNext; 
			if(pNext==NULL) break;
		}
	switch(GetNumAdapters())
	{
	case 0:
		pDacCombo0->AddString("No available tape device");
		pDacCombo0->SetCurSel(0);
		pDacCombo0->EnableWindow(FALSE);
		pDacCombo1->AddString("No available tape device");
		pDacCombo1->SetCurSel(0);
		pDacCombo1->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		(CWnd *)GetDlgItem(IDCANCEL)->SetFocus();
		break;
	case 1:
		pDacCombo0->SetCurSel(0);
		pDacCombo1->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		break;
	default:
		pDacCombo0->SetCurSel(0);
		pDacCombo1->SetCurSel(1);
		break;
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDatCopyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDatCopyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDatCopyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDatCopyDlg::OnSelchangeDac0() 
{
	// TODO: Add your control notification handler code here
	if ( ((CComboBox *)GetDlgItem(IDC_DAC1))->GetCurSel() == 
		 ((CComboBox *)GetDlgItem(IDC_DAC0))->GetCurSel() )
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	else
	{
		//items are not the same make sure we have at least 2 devices
		if (GetNumAdapters() > 1)
			GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	
}

void CDatCopyDlg::OnSelchangeDac1() 
{
	// TODO: Add your control notification handler code here
	if ( ((CComboBox *)GetDlgItem(IDC_DAC1))->GetCurSel() == 
		 ((CComboBox *)GetDlgItem(IDC_DAC0))->GetCurSel() )
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	else
	{
		//items are not the same make sure we have at least 2 devices
		if (GetNumAdapters() > 1)
			GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CDatCopyDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_Cancel = FALSE;
	m_nTimer = 0;
	GetDlgItem(IDC_STATUS0)->SetWindowText(" ");
	GetDlgItem(IDC_STATUS1)->SetWindowText(" ");
	m_Dev[0] = ((CComboBox *)GetDlgItem(IDC_DAC0))->GetCurSel();
	m_Dev[1] = ((CComboBox *)GetDlgItem(IDC_DAC1))->GetCurSel();
	GetDlgItem(IDOK)->EnableWindow(FALSE); //CRAY OUT THE OK BUTTONE
	m_CopyCompleted = FALSE;
	m_Thread = AfxBeginThread(CopyThread,this); //START THE COPY
}
/* ******************************************************************************************

	Function: GetDevice()
	returns the selected device. 0 = read from, 1 = write to
	return -1 if the input parameter is returned
*********************************************************************************************/
int CDatCopyDlg::GetDevice(int Dev) //Retrieves Selected Device
{
	if ( (Dev < 0) || (Dev > 1))
		return -1;
	else
		return (m_Dev[Dev]);
}

/*******************************************************************************************************************
 
   Function: OnCancel
   This function handles the Cancel Button click action. If the CopyThread is active, the Cancel Flag is set and 
   a 50 ms periodic timer is started. The CopyThread checks this flag to determine if it should terminate. When the
   thread terminates, the program is terminated here with a call to CDialog::OnCancel
   
********************************************************************************************************************/
void CDatCopyDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (m_Thread) //thread is still active
	{
		m_Cancel = TRUE;
		if (m_nTimer == 0) //Has timer already started
			m_nTimer = SetTimer(TIMER_CANCEL,50,NULL); //Start the timer and wait until thread expires

		//Wait until Thread is gone
	}
	else
		CDialog::OnCancel();

}

/*******************************************************************************************************************
 
   Function: OnStop
   This function handles the Stop Button click action. If the CopyThread is active, the Cancel Flag is set and 
   a 50 ms periodic timer is started. The CopyThread checks this flag to determine if it should terminate.
   
********************************************************************************************************************/
void CDatCopyDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	if (m_Thread) //thread is still active
	{
		m_Cancel = TRUE;
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
		if (m_nTimer == 0) //Has timer already started
			m_nTimer = SetTimer(TIMER_STOP,50,NULL); //Start the timer and wait until thread expires
	}
	else
		GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
}


/*******************************************************************************************************************
 
   Function: OnTimer
   This function is activated when the Stop or Quit button is pressed. It waits until the CopyThread is not active, 
   When this happens, the timer is killed and the Dialog box updated. If the CopyThread is active.

********************************************************************************************************************/

void CDatCopyDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_Thread == NULL) //THREAD HAS BEEN CANCELLED
	{
		if (!m_CopyCompleted)
		{
			GetDlgItem(IDC_STATUS0)->SetWindowText("Copy cancelled");
			GetDlgItem(IDC_STATUS1)->SetWindowText("Copy cancelled");
		}
		GetDlgItem(IDOK)->EnableWindow(TRUE); //CRAY OUT THE OK BUTTONE
		GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		KillTimer(m_nTimer);
		CDialog::OnTimer(nIDEvent);
		if (nIDEvent == TIMER_CANCEL)
			CDialog::OnCancel();
				
	}
	else
		CDialog::OnTimer(nIDEvent);
}

#define BUFFER_SIZE 32768

//Display I/O Error from SCSI Drive
void DisplayError(int Status,CWnd* pCWnd); //prototype
/**********************************************************************************************

  This thread will copy from one tape to another base on the selection in the dialog box.
  It will be assumed that the calling thread has performed all the necessary error checking

  **********************************************************************************************/

UINT CopyThread(LPVOID Context)
{

	int bc,Status;
	int Response = IDYES   ;
	PSCSI tp0,tp1;
	PUCHAR pBuffer;
	CAspiscsi *pAspi;
	CDatCopyDlg *pDlg = (CDatCopyDlg *)Context;
	pBuffer = (PUCHAR)new char[BUFFER_SIZE];
	pAspi = (CAspiscsi *)new CAspiscsi();
	LPCSTR dev0 = pDlg->DeviceName(pDlg->GetDevice(0));
	tp0 = pAspi->Open(dev0); //Open dev0: Copy frlm
	LPCSTR dev1 = pDlg->DeviceName(pDlg->GetDevice(1));
	tp1 = pAspi->Open(dev1); //Open dev1: copy to
	//Lets make sure we didn't have any problems opening the devices
	if ((tp0 == NULL) || (tp1 == NULL))
	{
		if (tp0 == NULL)
			pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Could not open device");
		else
			pAspi->Close(tp0);
		if (tp1 == NULL)
			pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Could not open device");
		else
			pAspi->Close(tp1);
		pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
		delete pBuffer;
		delete pAspi;
		pDlg->Done();
		return 0;
	}
	pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Rewinding");\
	pAspi->Reset(tp1);
	Status = pAspi->Rewind(tp1);
	if (Status != 0)
	{
		DisplayError(Status,pDlg->GetDlgItem(IDC_STATUS1));
		//pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Error Rewinding target tape");
		pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
		pAspi->Close(tp0);
		pAspi->Close(tp1);
		delete pBuffer;
		delete pAspi;
		pDlg->Done();
		return 0 ;
	}
	//Check to see if target tap is empty
	if (pDlg->IsCancel()) //cancel or stop button pressed
	{
		pAspi->Close(tp0);
		pAspi->Close(tp1);
		delete pBuffer;
		delete pAspi;
		pDlg->Done();
		return 0;
	}
	bc = pAspi->Read(tp1,pBuffer,10);
	pAspi->Rewind(tp1);
	if (pDlg->IsCancel()) //cancel or stop button pressed
	{
		pDlg->Done();
		pAspi->Close(tp0);
		pAspi->Close(tp1);
		delete pBuffer;
		delete pAspi;
		return 0;
	}
	if (bc != 0) //tap is not empty
	{
		Response = pDlg->MessageBox("Target tape is not empty - overwrite?","WARNING",MB_YESNO);
	}
	if (Response == IDYES)
	{

		if (!pDlg->IsCancel())
		{
			pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText(" ");
			pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Rewinding");
			Status = pAspi->Rewind(tp0);
			if (Status != 0)
			{
				DisplayError(Status,pDlg->GetDlgItem(IDC_STATUS0));
				//pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Error Rewinding source tape");
				pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
				pAspi->Close(tp0);
				pAspi->Close(tp1);
				delete pBuffer;
				delete pAspi;
				pDlg->Done();
				return 0;
			}
		}
		if (!pDlg->IsCancel())
		{
			pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Reading");
			pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText(" ");
			bc = pAspi->Read(tp0,pBuffer,BUFFER_SIZE);

/*			if (pAspi->ReadError() != 0)
			{
				DisplayError(pAspi->ReadError(),pDlg->GetDlgItem(IDC_STATUS0));
				pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
				pAspi->Close(tp0);
				pAspi->Close(tp1);
				delete pBuffer;
				delete pAspi;
				pDlg->Done();
				return 0 ;
			}
*/
			//keep reading and writing until there is no more data to read
			//or until the START OR STOP button is pressed
			while (bc != 0)
			{
				if (pDlg->IsCancel()) break;
				pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Writing");
				pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText(" ");
				Status = pAspi->Write(tp1,pBuffer,bc);
				if (pDlg->IsCancel()) break;
				if (Status != 0)
				{
					DisplayError(Status,pDlg->GetDlgItem(IDC_STATUS1));
					pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
					pAspi->Close(tp0);
					pAspi->Close(tp1);
					delete pBuffer;
					delete pAspi;
					pDlg->Done();
					return 0 ;
				}
				pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Reading");
				pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText(" ");
				bc = pAspi->Read(tp0,pBuffer,BUFFER_SIZE);
/*				if (pAspi->ReadError() != 0)
				{
					DisplayError(pAspi->ReadError(),pDlg->GetDlgItem(IDC_STATUS0));
					pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
					pAspi->Close(tp0);
					pAspi->Close(tp1);
					delete pBuffer;
					delete pAspi;
					pDlg->Done();
					return 0 ;
				}
*/
			}
			if (!pDlg->IsCancel())
			{
				pDlg->GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
				pDlg->GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
				pDlg->CopyCompleted();//indicate we have completed the tap copy
				pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText("Copy completed");
				pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Rewinding/Ejecting Tape");
				pAspi->Rewind(tp1); //rewind target tape
				pAspi->Eject(tp1);	//eject target tape
				pDlg->GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
				pDlg->GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
				pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
				pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Copy completed");
			}
		}
	}
	else
	{	//reenable the OK Buttone
		pDlg->GetDlgItem(IDOK)->EnableWindow(TRUE); //Enable the OK Buttone
		pDlg->GetDlgItem(IDC_STATUS0)->SetWindowText(" ");
		pDlg->GetDlgItem(IDC_STATUS1)->SetWindowText("Tape is not empty");

	}

	//Copy has completed. Let's eject the tapes
	
	pAspi->Close(tp0);
	pAspi->Close(tp1);
	delete pBuffer;
	delete pAspi;
	pDlg->Done();
	return(0);
}


/**********************************************************************************************

  Display Error message detected from Tape Drive

  **********************************************************************************************/

 void DisplayError(int Status,CWnd* pCWnd)
{

	switch(Status)
	{
	case E$NotReady:
		pCWnd->SetWindowText("Device not ready");
		break;
	case E$Abort:
		pCWnd->SetWindowText("Command Aborted");
		break;
	case E$UnitAttention:
		pCWnd->SetWindowText("Unit needs Attention/Device not Ready");
		break;
	case E$IOErr:
		pCWnd->SetWindowText("I/O Error");
		break;
	default:
		pCWnd->SetWindowText("Unknown Error");
		break;
	}
}


