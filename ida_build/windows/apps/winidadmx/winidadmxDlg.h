// winidadmxDlg.h : header file
//

#if !defined(AFX_WINIDADMXDLG_H__E8A1F729_5837_11D4_8D84_00B0D0233922__INCLUDED_)
#define AFX_WINIDADMXDLG_H__E8A1F729_5837_11D4_8D84_00B0D0233922__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WorkingDlg.h"
#include "Btnst.h"
#include "Label.h"
#include "DialogWB.h"
#include "DragList.h"

/////////////////////////////////////////////////////////////////////////////
// CWinidadmxDlg dialog

class CWinidadmxDlg : public CDialogWB
{
// Construction
public:
	CWinidadmxDlg(CWnd* pParent = NULL);	// standard constructor
	~CWinidadmxDlg();

	void LoadParam();
	void SaveParam();
	void LoadTapesList();
	void ViewResults(LPSTR str);
	void LoadMapFileNames();
	void SelectMap(LPCSTR lpSta, int nRelease);
	void SelectMap();



	CString ExecString;
	CWorkingDlg *WorkingDlg;
	static void ParserForOutputFileName();
	bool CheckDir(LPCSTR lpDir);
	void LoadCDDeviceNames();
	int ProcessInputFromList();
	int MakeTemporaryFile(const CStringList & FileList);
	int UnpackFileToTemp(HANDLE hF, const CString &s);
	void LoadStaAndFormat();
	void SelectRelease(int nRelease);

	void SetListHeader();
	void LoadCDFileNames();
	void SelUnselFiles(BOOL bSel);
	void LoadInfoFromDisk();
	void ClearCDInfo();
	int MakeFilesList(CStringList &FilesList);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult); 
	void FillSizeInfo();
	void LoadCDFileTime(int nIndex);
	void ClearCDFileArray();
	CString GetDefaultMapFile(LPCSTR lpSta);
	BOOL bSelectMapInCombobox(LPCSTR lpName);
	BOOL SelectMapFromAssignments(LPCSTR lpSta);
	BOOL CheckDirectory(LPCSTR lpDir);
	BOOL ProcessInputFromGZfile(LPCSTR str);
	void Process1();
	void LoadDriveNames();
	BOOL RestoreOriginalDirectory();


	void AddToListView(CListCtrl *pList, WIN32_FIND_DATA* fd , int iImage);
	void AddToArray(WIN32_FIND_DATA* fd, int iImage);
	void AddArrayToListView();
	static int SortFileArray(const void *p1, const void *p2);
	void SetDataDirFromCD();
	void EnterDirectory();



	void UpdateListView();
	void SetupImages();

	CString cTempFileForDataFromCD;
	bool m_showfiletime;
	CString m_currentDirectory;
	CString m_dataDirectory;
	CString m_dataDisk;
	CImageList m_stateImage;


// Dialog Data
	//{{AFX_DATA(CWinidadmxDlg)
	enum { IDD = IDD_WINIDADMX_DIALOG };

	CComboBox	m_inputcombo;
	CString	m_inputidstring;
	CString	m_outputdatadirectory;
	CString	m_startdate;
	CString	m_enddate;
	CString	m_discardlist;
	CString	m_records;
	CString	m_inputformat;
	CString	m_outputdirectory;
	CString	m_workingdirectory;
	BOOL	m_writerawheader;
	BOOL	m_swapbyte;
	int		m_unpackall;
	int		m_inputfrom;
	int		m_record1;
	int		m_record2;
	CString	m_stacode;
	CString	m_dbdir;
	BOOL	m_timetagsummary;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinidadmxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CDragList *m_pDragList;
	char m_CheckWorkingDirectoryFlag;
	// Generated message map functions
	//{{AFX_MSG(CWinidadmxDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnBrowseoutputdatadir();
	afx_msg void OnBrowsechanmap();
	afx_msg void OnDoubleclickedBrowse();
	virtual void OnCancel();
	afx_msg void OnInputfromtape();
	afx_msg void OnIdcinputfromdisk();
	afx_msg void OnExitIDADMX();
	afx_msg void OnExitIDALST();
	afx_msg void OnUnpackall();
	afx_msg void OnUnpackrange();
	afx_msg void OnAbout();
	afx_msg void OnSelchangeInputformat();
	afx_msg void OnInputfromCD();
	afx_msg BOOL OnDeviceChange( UINT, DWORD );
	afx_msg void OnSelall();
	afx_msg void OnUnselall();
	afx_msg void OnBrowseSubdir();
	afx_msg void OnSelchangeDriveCombo();
	afx_msg void OnSelchangeAddresscombo();
	afx_msg void OnEnteraddress();
	afx_msg void OnDblclklist(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void CheckWorkingDirectory();
	afx_msg void OnIdalstButton();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINIDADMXDLG_H__E8A1F729_5837_11D4_8D84_00B0D0233922__INCLUDED_)
