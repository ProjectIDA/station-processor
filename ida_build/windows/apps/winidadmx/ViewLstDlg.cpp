// ViewLstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "winidadmx.h"
#include "ViewLstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewLstDlg dialog


CViewLstDlg::CViewLstDlg(LPCSTR csText, CWnd* pParent /*=NULL*/)
	: CDialog(CViewLstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewLstDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	text=csText;
}


void CViewLstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewLstDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewLstDlg, CDialog)
	//{{AFX_MSG_MAP(CViewLstDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewLstDlg message handlers

BOOL CViewLstDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_RESULTS, text);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
