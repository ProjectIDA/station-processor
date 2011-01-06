// DataView.h : main header file for the DATAVIEW application
//

#if !defined(AFX_DATAVIEW_H__AA568B84_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_DATAVIEW_H__AA568B84_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "PrintDlg.h"
#include "WaveformsDisplay.h"


/////////////////////////////////////////////////////////////////////////////
// CDataViewApp:
// See DataView.cpp for the implementation of this class
//

class CDataViewApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CDataViewApp();
	void LoadGainMenu();
	void LoadFilterMenu();
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDataViewApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAVIEW_H__AA568B84_08E2_11D3_B68A_941D97000000__INCLUDED_)
