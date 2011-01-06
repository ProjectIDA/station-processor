#if !defined(AFX_SUBEDIT_H__B8BC7D28_3F6C_4D2D_853C_353E12F3126F__INCLUDED_)
#define AFX_SUBEDIT_H__B8BC7D28_3F6C_4D2D_853C_353E12F3126F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubEdit window

class CSubEdit : public CEdit
{
// Construction
public:
	CSubEdit();
	int m_x;
	int m_y, m_cy;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSubEdit)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBEDIT_H__B8BC7D28_3F6C_4D2D_853C_353E12F3126F__INCLUDED_)
