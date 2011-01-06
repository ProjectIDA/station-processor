// DisplayParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crtd.h"
#include "DisplayParamDlg.h"
#include "crtdglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayParamDlg property page

IMPLEMENT_DYNCREATE(CDisplayParamDlg, CPropertyPage)

CDisplayParamDlg::CDisplayParamDlg() : CPropertyPage(CDisplayParamDlg::IDD)
	{
	//{{AFX_DATA_INIT(CDisplayParamDlg)
	m_bufftime = 0;
	m_disptime = 0;
	m_refreshtime = 0;
	m_delay = 0;
	//}}AFX_DATA_INIT
	m_bufftime=nBufferedTime;
	m_disptime=nDisplayedTime;
	m_refreshtime=nRefreshTime;
	m_delay=nDelay;

	}

CDisplayParamDlg::~CDisplayParamDlg()
{
}

void CDisplayParamDlg::DoDataExchange(CDataExchange* pDX)
	{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayParamDlg)
	DDX_Text(pDX, IDC_BUFFEREDTIME, m_bufftime);
	DDV_MinMaxInt(pDX, m_bufftime, 1, 86400);
	DDX_Text(pDX, IDC_DISPLAYTIME, m_disptime);
	DDV_MinMaxInt(pDX, m_disptime, 1, 86400);
	DDX_Text(pDX, IDC_REFRESHTIME, m_refreshtime);
	DDV_MinMaxInt(pDX, m_refreshtime, 1, 60);
	DDX_Text(pDX, IDC_DELAY, m_delay);
	DDV_MinMaxInt(pDX, m_delay, 0, 3600);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CDisplayParamDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CDisplayParamDlg)
	ON_EN_CHANGE(IDC_BUFFEREDTIME, OnChangeBufferedtime)
	ON_EN_CHANGE(IDC_DISPLAYTIME, OnChangeDisplaytime)
	ON_EN_CHANGE(IDC_REFRESHTIME, OnChangeRefreshtime)
	ON_EN_CHANGE(IDC_DELAY, OnChangeDelay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayParamDlg message handlers

BOOL CDisplayParamDlg::OnApply() 
	{
	// TODO: Add your specialized code here and/or call the base class

	char cBuff[64];
	nBufferedTime=m_bufftime;
	nDisplayedTime=m_disptime;
	nRefreshTime=m_refreshtime;
	nDelay=m_delay;
	
	sprintf(cBuff,"%d",nDisplayedTime);
	WritePrivateProfileString("-CONFIG-","DISPLAYEDTIME",cBuff, pIniFile);

	sprintf(cBuff,"%d",nBufferedTime);
	WritePrivateProfileString("-CONFIG-","BUFFEREDTIME",cBuff, pIniFile);

	sprintf(cBuff,"%d",nRefreshTime);
	WritePrivateProfileString("-CONFIG-","REFRESHTIME",cBuff, pIniFile);

	sprintf(cBuff,"%d",nDelay);
	WritePrivateProfileString("-CONFIG-","DELAY",cBuff, pIniFile);

	return CPropertyPage::OnApply();
	}

void CDisplayParamDlg::OnChangeBufferedtime() 
	{
	SetModified(TRUE);	
	}

void CDisplayParamDlg::OnChangeDisplaytime() 
	{
	SetModified(TRUE);	
	}

void CDisplayParamDlg::OnChangeRefreshtime() 
	{
	SetModified(TRUE);	
	}

void CDisplayParamDlg::OnChangeDelay() 
	{
	SetModified(TRUE);	
	}
