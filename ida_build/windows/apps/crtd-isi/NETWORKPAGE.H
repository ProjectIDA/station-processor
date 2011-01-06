#if !defined(AFX_NETWORKPAGE_H__F0C2AFB3_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_NETWORKPAGE_H__F0C2AFB3_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NetworkPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkPage dialog

class CNetworkPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkPage)

// Construction
public:
	CNetworkPage();
	~CNetworkPage();

// Dialog Data
	//{{AFX_DATA(CNetworkPage)
	enum { IDD = IDD_NETWORKDLG };
	CString	m_server;
	int		m_port;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkPage)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkPage)
	afx_msg void OnChangeNrtsserver();
	afx_msg void OnChangePort();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKPAGE_H__F0C2AFB3_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
