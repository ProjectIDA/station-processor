// DatCopy.h : main header file for the DATCOPY application
//

#if !defined(AFX_DATCOPY_H__1B774976_14A8_4C84_A08A_4CC5776B8735__INCLUDED_)
#define AFX_DATCOPY_H__1B774976_14A8_4C84_A08A_4CC5776B8735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDatCopyApp:
// See DatCopy.cpp for the implementation of this class
//

class CDatCopyApp : public CWinApp
{
public:
	CDatCopyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatCopyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDatCopyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATCOPY_H__1B774976_14A8_4C84_A08A_4CC5776B8735__INCLUDED_)
