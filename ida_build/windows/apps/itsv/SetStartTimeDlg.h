//{{AFX_INCLUDES()
#include "msmask.h"
//}}AFX_INCLUDES
#if !defined(AFX_SETSTARTTIMEDLG_H__0A665165_41A6_11D4_8D76_00B0D0233922__INCLUDED_)
#define AFX_SETSTARTTIMEDLG_H__0A665165_41A6_11D4_8D76_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetStartTimeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetStartTimeDlg dialog

class CSetStartTimeDlg : public CDialog
{
// Construction
public:
	CSetStartTimeDlg(double tbeg, int duration, CWnd* pParent = NULL);   // standard constructor
	int nTimeFormat;
	double dStartTime, dDuration;
	void SeTimeFormat1();
	void SeTimeFormat2();
// Dialog Data
	//{{AFX_DATA(CSetStartTimeDlg)
	enum { IDD = IDD_SETTIMEDLG };
	int		m_duration;
	CMSMask	m_starttime1;
	CMSMask	m_starttime2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetStartTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetStartTimeDlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETSTARTTIMEDLG_H__0A665165_41A6_11D4_8D76_00B0D0233922__INCLUDED_)
