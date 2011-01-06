#if !defined(AFX_WAVEFORMSDISPLAY_H__AA568B96_08E2_11D3_B68A_941D97000000__INCLUDED_)
#define AFX_WAVEFORMSDISPLAY_H__AA568B96_08E2_11D3_B68A_941D97000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaveformsDisplay.h : header file
//
#include "MWindow.h"
#include "StaInfo.h"
#include "WaveformsDisplay.h"
#include "WfdiscRecordsArray.h"
#include "InfoWindow.h"
#include "Screen.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveformsDisplay window
const int WindowInfoXsize=100;

class CWaveformsDisplay : public CWnd 
{
friend class CWaveformsPrint;
// Construction
public:
	CWaveformsDisplay();
//********************************************
public:
	void ClearWaveformInfo();
	void DrawFocused(int, CDC *pDC);
	void LoadWfdiscFileInfo(CString FileName);
	void RunSelectDlg();
	void FillWaveformInfo();
	void FindMinMaxTimeInWaveformInfo();
	int CalculateMWindowsSize();
	void DrawWaveforms();
	void DrawTimePanel(CDC *pDC);
	void DrawWindow(int nWin, CDC *pDC, int TimeBarYsize, int x1, int x2, int y1, int y2,
   int xw1, int xw2, int yw1, int yw2, int MaxY, int MaxX, bool bPrint=FALSE);
	void GetTextRectangle(CDC *pdc, LPSTR lpText, RECT &rc);

	void RedrawPreview();
	void RepaintPreview();
	void OnFitToWindow();
	void OnExpandHorizontal();
	void OnContractHorizontal();
	void OnExpandVertical();
	void OnContractVertical();
	void OnShiftToLeft();
	void OnShiftToRight();
	void OnShiftToDown();
	void OnShiftToUp();
	int GetWindowFromPoint(int x, int y);
	void ContractX(int x);
	void ContractY(int y);
	void OnFitToWindow(int);
	void Original();
	void SelectTraces();
	void SelectAll();
	void DeleteAll();
	void SetFitMaxMin(int nWf);
	void SetAmpFlagFixed();
	void SetAmpFlagAuto();
	void SetAmpFlagInvert();
	void SetAmpFlagAuto0();
	void SetAmpFlagAuto1();
	void SetAmpFlagAutoA();
	void SetStartTime();
	long GetDeltaInterval();
	void Fit();
	void DeleteTraces();
	void DrawShiftedScreenBmp(int delta);


	void ExpandY(int y);
	void ExpandX(int x);
	void RedrawWindow(int nWindow);
	BOOL ClipOnOff();
	int GetLeftPanelNumberFromPoint(int x, int y);
	void AutoA();
	void Auto0();
	void ApplyAuto();
	void ApplyAuto0();
	void ApplyAuto1();
	void ApplyAutoA();
	void ApplyScale();
	void GetMinMaxForTrace(int nWf, long &amax, long &amin);
	void FlipInvertFlag();
	void ChangeGain(double nGain, int flag);
	void DrawTimeMark(int x, double tt, CDC *pDC);
	void DrawLeftPannel();
	void DrawLeftPannelWindow(int nScreen, CDC *pDC);
	void DrawTimePannel(double t1, double t2);
	void MakeShiftedScreenBmp();
	void SetFilterNumber(int nFilt);
	void DrawTimeMarks(CDC *pDC, double t1, double t2, int xw1, int xw2, int yw1, int yw2, int MaxY);
	void Print();


//********************************************
protected:
	void GetTextRectangle(LPSTR lpText, RECT &rc);

public:
	CBitmap Bitmap; // off screen bitmap
	CBitmap leftPannel;
	CBitmap timePanel;
	CBitmap offscrbmp;
	CArray<CMWindow *, CMWindow *> WaveformInfo; // array of all waveforms 
	CArray<int , int > tmmarks; // array time - marks 

	CCriticalSection ScreenBitmapCriticalSection;

	CWfdiscRecordsArray wfdarray;

	int MaxX, MaxY;
	int xcursor,ycursor;
	double dGain;
	double t1,t2;
	double tB1,tB2;
	int nActiveScreen;
	CStaInfo AllStaChan;
	bool bClip;
	bool bPanelSelectionActive;
	bool bInvert;
	int oldPanelAct;
	int DisplayMode;
//****************** BOX *****************
private:
	int TimeBarYsize;
	int nAmpFlag;
	int mouse_x;
	int mouse_y;

	int mouse_x1;
	int mouse_y1;

	BOOL bBox;
	int nBoxWindow;
	bool bBoxExpandX;
	bool bBoxContractX;
	bool bBoxExpandY;
	bool bBoxContractY;
	bool bMiddleButtonDrag;
	int mouse_x_old,mouse_y_old;
	enum DisplayModes { mFx, mAuto, mAuto0, mAuto1, mAutoA}; 
	int nLastMarkRightPos;
	CString WfdFileName;
public:
	CInfoWindow infwnd;
	CInfoWindow amplinfwnd;
	CScreen *pScreen;
public:
	int nWindowFromPoint(int x, int y);
	void ProcessCursorAction(int x, int y, int nFlags);
	void RunSetTimeDlg();
	void ApplyGain();
	int mbuttonx;
	int mbuttony;
	int xX1, xX2;
	int nFilterType;


//****************************************

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveformsDisplay)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWaveformsDisplay();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWaveformsDisplay)
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVEFORMSDISPLAY_H__AA568B96_08E2_11D3_B68A_941D97000000__INCLUDED_)
