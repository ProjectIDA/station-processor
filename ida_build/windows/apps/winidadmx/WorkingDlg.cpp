// WorkingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "winidadmx.h"
#include "WorkingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorkingDlg dialog


CWorkingDlg::CWorkingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorkingDlg::IDD, pParent)
	{
	BOOL b=Create(CWorkingDlg::IDD,pParent);

	//{{AFX_DATA_INIT(CWorkingDlg)
	//}}AFX_DATA_INIT
	}


void CWorkingDlg::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void CWorkingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkingDlg)
	DDX_Control(pDX, IDC_PICT, m_pic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWorkingDlg, CDialog)
	//{{AFX_MSG_MAP(CWorkingDlg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PICT, OnPict)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CWorkingDlg, CDialog)
	//{{AFX_DISPATCH_MAP(CWorkingDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWorkingDlg message handlers

BOOL CWorkingDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	hB1.LoadBitmap(IDB_BITMAP1);
	hB2.LoadBitmap(IDB_BITMAP2);
	nbpm=0;
	SetTimer(2,500,NULL);
	return TRUE;
	}

void CWorkingDlg::OnTimer(UINT nIDEvent) 
	{

	if(nbpm==0)
		{
		m_pic.SetBitmap(hB1);
		nbpm=1;
		}
	else
		{
		m_pic.SetBitmap(hB2);
		nbpm=0;
		}


	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
	}
void CWorkingDlg::Stop() 
	{
	DestroyWindow();
	}
void CWorkingDlg::OnDestroy() 
	{
	CDialog::OnDestroy();
	
	KillTimer(2);	
	}

void CWorkingDlg::OnPict() 
	{
	// TODO: Add your control notification handler code here
	
	}


void CWorkingDlg::OnCancel() 
	{
	// TODO: Add extra cleanup here
	
	}
