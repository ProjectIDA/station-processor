// PrintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "PrintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg

IMPLEMENT_DYNAMIC(CPrintDlg, CPrintDialog)

CPrintDlg::CPrintDlg(BOOL bPrintSetupOnly, DWORD dwFlags, CWnd* pParentWnd) :
	CPrintDialog(bPrintSetupOnly, dwFlags, pParentWnd)
	{
	}


BEGIN_MESSAGE_MAP(CPrintDlg, CPrintDialog)
	//{{AFX_MSG_MAP(CPrintDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPrintDlg::OnOK() 
	{
	char cBuff[256];
	GetDlgItemText(IDC_HEADERTEXT,cBuff,sizeof(cBuff));
	m_headertext=cBuff;
	CPrintDialog::OnOK();
	// TODO: Add your control notification handler code here
	}
CString CPrintDlg::GetHeaderText()
	{
	return m_headertext;
	}

BOOL CPrintDlg::OnInitDialog() 
	{
	CWnd *pWnd=GetDlgItem(9999);
	CPrintDialog::OnInitDialog();
	return TRUE;
	}

int CPrintDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	if (CPrintDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
	}
