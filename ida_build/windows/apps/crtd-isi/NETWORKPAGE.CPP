// NetworkPage.cpp : implementation file
//

#include "stdafx.h"
#include "crtd.h"
#include "NetworkPage.h"
#include "crtdglob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkPage property page

IMPLEMENT_DYNCREATE(CNetworkPage, CPropertyPage)

CNetworkPage::CNetworkPage() : CPropertyPage(CNetworkPage::IDD)
{
	//{{AFX_DATA_INIT(CNetworkPage)
	m_server = _T("");
	m_port = 0;
	//}}AFX_DATA_INIT
	m_server=server;
	m_port=nNrtsPort;
}

CNetworkPage::~CNetworkPage()
{
}

void CNetworkPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkPage)
	DDX_Text(pDX, IDC_NRTSSERVER, m_server);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDV_MinMaxInt(pDX, m_port, 10000, 60000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkPage)
	ON_EN_CHANGE(IDC_NRTSSERVER, OnChangeNrtsserver)
	ON_EN_CHANGE(IDC_PORT, OnChangePort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkPage message handlers

BOOL CNetworkPage::OnApply() 
	{
	// TODO: Add your specialized code here and/or call the base class
	char cBuff[32];
	server=m_server;
	nNrtsPort=m_port;

	WritePrivateProfileString("-CONFIG-","SERVER",server, pIniFile);
	sprintf(cBuff,"%d",nNrtsPort);
	WritePrivateProfileString("-CONFIG-","PORT",cBuff, pIniFile);

	return CPropertyPage::OnApply();
	}

void CNetworkPage::OnChangeNrtsserver() 
	{
	SetModified(TRUE);	
	}

void CNetworkPage::OnChangePort() 
	{
	SetModified(TRUE);	
	}
