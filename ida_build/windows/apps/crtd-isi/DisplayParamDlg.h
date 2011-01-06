#if !defined(AFX_DISPLAYPARAMDLG_H__F0C2AFB4_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
#define AFX_DISPLAYPARAMDLG_H__F0C2AFB4_FD5A_11D2_8661_4CAE18000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DisplayParamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayParamDlg dialog

class CDisplayParamDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CDisplayParamDlg)

// Construction
public:
	CDisplayParamDlg();
	~CDisplayParamDlg();

// Dialog Data
	//{{AFX_DATA(CDisplayParamDlg)
	enum { IDD = IDD_DISPLAYPARAMDLG };
	int		m_bufftime;
	int		m_disptime;
	int		m_refreshtime;
	int		m_delay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDisplayParamDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDisplayParamDlg)
	afx_msg void OnChangeBufferedtime();
	afx_msg void OnChangeDisplaytime();
	afx_msg void OnChangeRefreshtime();
	afx_msg void OnChangeDelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYPARAMDLG_H__F0C2AFB4_FD5A_11D2_8661_4CAE18000000__INCLUDED_)
