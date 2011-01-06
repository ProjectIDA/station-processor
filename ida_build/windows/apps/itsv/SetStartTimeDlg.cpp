// SetStartTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "SetStartTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetStartTimeDlg dialog

CString &TimeToString(double t, int nTimeFormat);
double StringToTime(CString s, int nTimeFormat);


CSetStartTimeDlg::CSetStartTimeDlg(double tbeg, int duration,CWnd* pParent /*=NULL*/)
	: CDialog(CSetStartTimeDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CSetStartTimeDlg)
	m_duration = 0;
	//}}AFX_DATA_INIT
	nTimeFormat=1;
	m_duration=duration;
	dStartTime=tbeg;
	}


void CSetStartTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetStartTimeDlg)
	DDX_Text(pDX, IDC_DURATION, m_duration);
	DDX_Control(pDX, IDC_STARTTIME1, m_starttime1);
	DDX_Control(pDX, IDC_STARTTIME2, m_starttime2);
	//}}AFX_DATA_MAP
}




BEGIN_MESSAGE_MAP(CSetStartTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CSetStartTimeDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetStartTimeDlg message handlers

void CSetStartTimeDlg::OnRadio1() 
	{
	// TODO: Add your control notification handler code here
	SeTimeFormat1();
	UpdateData(FALSE);
	}
void CSetStartTimeDlg::SeTimeFormat1()
	{
	CString s;

	if(nTimeFormat==0)
		s=m_starttime1.GetText();
	else
		s=m_starttime2.GetText();
	s.Replace("_","0");
	dStartTime=StringToTime(s, nTimeFormat);
	nTimeFormat=0;
	s=TimeToString(dStartTime,nTimeFormat);
	m_starttime1.SetText(s);
	m_starttime1.ShowWindow(SW_SHOW);
	m_starttime2.ShowWindow(SW_HIDE);
	CButton *cb1=(CButton *)GetDlgItem(IDC_RADIO1);
	CButton *cb2=(CButton *)GetDlgItem(IDC_RADIO2);
	cb1->SetCheck(1);
	cb2->SetCheck(0);
	}
void CSetStartTimeDlg::SeTimeFormat2()
	{

	CString s;

	if(nTimeFormat==0)
		s=m_starttime1.GetText();
	else
		s=m_starttime2.GetText();
	
	dStartTime=StringToTime(s, nTimeFormat);

	nTimeFormat=1;

	s.Replace("_","0");

	s=TimeToString(dStartTime,nTimeFormat);
	m_starttime1.ShowWindow(SW_HIDE);
	m_starttime2.ShowWindow(SW_SHOW);
	m_starttime2.SetText(s);
	CButton *cb1=(CButton *)GetDlgItem(IDC_RADIO1);
	CButton *cb2=(CButton *)GetDlgItem(IDC_RADIO2);
	cb1->SetCheck(0);
	cb2->SetCheck(1);
	}

void CSetStartTimeDlg::OnRadio2() 
	{
	// TODO: Add your control notification handler code here
	SeTimeFormat2();
	UpdateData(FALSE);
	}

void CSetStartTimeDlg::OnOK() 
	{
	UpdateData();
	CString s;

	if(nTimeFormat==0)
		s=m_starttime1.GetText();
	else
		s=m_starttime2.GetText();
	dStartTime=StringToTime(s, nTimeFormat);
	dDuration=m_duration;
	CDialog::OnOK();
	}

BOOL CSetStartTimeDlg::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	CString s;
	s=TimeToString(dStartTime,0);
	m_starttime1.SetText(s);
	s=TimeToString(dStartTime,1);
	m_starttime2.SetText(s);

	if(nTimeFormat==0)
		{
		SeTimeFormat1();
		}
	else
		{
		SeTimeFormat2();
		}


	return TRUE;
	}
