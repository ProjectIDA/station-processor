// DatCopyDlg.h : header file
//

#if !defined(AFX_DATCOPYDLG_H__FE5182A7_585C_41EA_A96D_14296DFDE56F__INCLUDED_)
#define AFX_DATCOPYDLG_H__FE5182A7_585C_41EA_A96D_14296DFDE56F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Aspiscsi.h"
/////////////////////////////////////////////////////////////////////////////
// CDatCopyDlg dialog

//Maximum Supported Devices
#define MAX_DEVICES 10

class CDatCopyDlg : public CDialog, CAspiscsi
{
// Construction
public:
	CDatCopyDlg(CWnd* pParent = NULL);	// standard constructor
	//PSCSI m_tp0,m_tp1;//Opened device handles
	int GetDevice(int Dev); //Retrieves Selected Device
	void Eject(PSCSI tp);
	LPCTSTR DeviceName(int Dev){
		return(m_DevName[Dev]);};
	int IsCancel(){
		return m_Cancel;};
	void Done(){
		m_Thread = NULL;};
	void CopyCompleted(){
		m_CopyCompleted = TRUE;};
		// Dialog Data
	//{{AFX_DATA(CDatCopyDlg)
	enum { IDD = IDD_DATCOPY_DIALOG };
	CString	m_Status0;
	CString	m_Status1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatCopyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	HICON m_hIcon;
	CWinThread*	m_Thread;
	UINT m_nTimer;
	enum{TIMER_STOP=1,TIMER_CANCEL};
	// Generated message map functions
	//{{AFX_MSG(CDatCopyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSelchangeDac0();
	afx_msg void OnSelchangeDac1();
	virtual void OnCancel();
	afx_msg void OnStop();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	PDEVLIST m_DevList;
	CString m_DevName[MAX_DEVICES];
	int m_Dev[2];	//Device Selected. item 0 = read from, item 1 = write to
	int m_Cancel;
	bool m_CopyCompleted;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATCOPYDLG_H__FE5182A7_585C_41EA_A96D_14296DFDE56F__INCLUDED_)
