// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "DataView.h"
#include <direct.h>

#include "MainFrm.h"
#include "Splash.h"
#include "OpenFileDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDataViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_INITMENUPOPUP()
	ON_WM_MENUSELECT()
	ON_COMMAND_RANGE(IDC_GAIN1, IDC_GAIN1+20, OnChangeGain)
	ON_COMMAND_RANGE(IDC_FILTER1, IDC_FILTER1+20, OnSetFilter)
	ON_COMMAND(IDC_UPDATEVIEW, OnUpdateView)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(IDC_REPAINTPREVIEW, OnRepaintPreview)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDC_START, OnStart)
	ON_COMMAND(IDC_SELECTCHAN, OnSelectChanSta)
	ON_COMMAND(IDC_FITTOWINDOW, OnFitToWindow)
	ON_COMMAND(IDC_EXPANDHOR, OnExpandHorizontal)
	ON_COMMAND(IDC_CONTRACTHORZ, OnContractHorizontal)
	ON_COMMAND(IDC_CONTRACTVERT, OnContractVertical)
	ON_COMMAND(IDC_EXPANDVERTICAL, OnExpandVertical)
	ON_COMMAND(IDC_SHIFTLEFT, OnShiftLeft)
	ON_COMMAND(IDC_SHIFTRIGHT, OnShiftRight)
	ON_COMMAND(IDC_DOWN, OnDown)
	ON_COMMAND(IDC_UP, OnUp)
	ON_WM_KEYUP()
	ON_COMMAND(IDC_SELECTTRACES, OnSelecttraces)
	ON_COMMAND(IDC_ORIGINAL, OnOriginal)
	ON_COMMAND(IDC_SELECTALL, OnSelectall)
	ON_COMMAND(IDC_DELETEALL, OnDeleteall)
	ON_COMMAND(IDC_AUTOAMP, OnAutoamp)
	ON_COMMAND(IDC_AMPFIXED, OnAmpfixed)
	ON_COMMAND(IDC_CLIPONOFF, OnCliponoff)
	ON_COMMAND(IDC_AUTOA, OnAutoa)
	ON_COMMAND(IDC_AUTO0, OnAuto0)
	ON_COMMAND(IDC_AUTO1, OnAuto1)
	ON_COMMAND(IDC_INVERT, OnInvert)
	ON_COMMAND(IDC_SETGAIN_1, OnSetgain1)
	ON_COMMAND(IDC_SETTIME, OnSettime)
	ON_COMMAND(IDC_EXIT, OnExit)
	ON_COMMAND(IDC_XFIT, OnXfit)
	ON_COMMAND(IDC_DELETETRACES, OnDeletetraces)
	ON_COMMAND(IDC_FILTERNONE, OnFilternone)
	ON_COMMAND(IDC_PRINT, OnPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_AMPLTIME
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():m_Preview(&m_Display)
{
	// CG: The following block was inserted by 'Status Bar' component.
	{
		m_bMenuSelect = FALSE;
	}

	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
	{

	}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	m_wndToolBar.ShowWindow(SW_HIDE);

	HBRUSH hBrush=(HBRUSH)GetStockObject(LTGRAY_BRUSH);
	HCURSOR hCur=(HCURSOR)::LoadCursor(NULL,IDC_ARROW);
	HICON hIco=NULL;
	LPCTSTR lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);

	RECT rcM = { 0,0,100,100 };
	GetClientRect(&rcM);
	rcM.bottom-=10;
	if(!m_Display.Create(lpClass,"", WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,rcM,this,901))	return -1;
	lpClass=AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW,hCur,hBrush,hIco);
	if(!m_Preview.Create(lpClass,"",WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,rcM,this,902)) return -1;

	RECT rc={0,0,200,200};
//	tooltip.Create(this,TTS_ALWAYSTIP);
//	tooltip.AddTool(&m_Preview,LPSTR_TEXTCALLBACK , &rc, IDC_PREVIEWWINDOW);
//	tooltip.AddTool(&m_Preview,_T("Preview"));
//	tooltip.Activate(TRUE);
	EnableToolTips(TRUE);
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers






void CMainFrame::OnSize(UINT nType, int cx, int cy) 
	{
	CFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	RECT rcM,rc1,rc,rc2;
	int sbH,tbH;

	GetClientRect(&rcM);
	m_wndStatusBar.GetWindowRect(&rc1);
	m_wndToolBar.GetWindowRect(&rc2);

	sbH=(rc1.bottom-rc1.top);
	tbH=(rc2.bottom-rc2.top);

// Set Display window position
	rc=rcM;
	rc.bottom=rcM.bottom-sbH-35;
	rc.top=tbH+1;
	m_Display.MoveWindow(&rc);

// Set Scroll bar position
	rc=rcM;
	rc.top=rcM.bottom-sbH-35;
	rc.bottom=rcM.bottom-sbH;
	m_Preview.MoveWindow(&rc);


	}
void CMainFrame::OnFileOpen() 
	{
	OpenFileDlg fdlg(TRUE,"wfdisc",NULL,OFN_EXPLORER|OFN_ENABLETEMPLATE|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"wfdisc files|*.wfdisc|All files|*.*||",this);
//	fdlg.m_ofn.lpstrInitialDir=cWfdiscDir;
	fdlg.m_ofn.lpstrTitle="Select .wfdisc file name";
	//	fdlg.m_ofn.Flags |= (/*OFN_ENABLEHOOK|*/OFN_EXPLORER|OFN_ENABLETEMPLATE);
	fdlg.m_ofn.hInstance=AfxGetInstanceHandle();
	fdlg.m_ofn.lpTemplateName=MAKEINTRESOURCE(IDD_OPENFILEDIALOG);

	UINT APIENTRY MyHook(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam); 

//	fdlg.m_ofn.lpfnHook = MyHook; 


	if(fdlg.DoModal()==IDOK) 
		{
		m_Display.LoadWfdiscFileInfo(fdlg.GetPathName());
		}
	}

void CMainFrame::OnStart()
	{
	CCommandLineInfo cmdInfo;
	theApp.ParseCommandLine(cmdInfo);

	if(cmdInfo.m_nShellCommand==CCommandLineInfo::FileNew)
		{
		OpenFileDlg fdlg(TRUE,"wfdisc",NULL,OFN_EXPLORER|OFN_ENABLETEMPLATE|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"wfdisc files|*.wfdisc|All files|*.*||",this);
	//	fdlg.m_ofn.lpstrInitialDir=cWfdiscDir;
		fdlg.m_ofn.lpstrTitle="Select .wfdisc file name";
	//	fdlg.m_ofn.Flags |= (/*OFN_ENABLEHOOK|*/OFN_EXPLORER|OFN_ENABLETEMPLATE);
		fdlg.m_ofn.hInstance=AfxGetInstanceHandle();
		fdlg.m_ofn.lpTemplateName=MAKEINTRESOURCE(IDD_OPENFILEDIALOG);

		UINT APIENTRY MyHook(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam); 

//	fdlg.m_ofn.lpfnHook = MyHook; 


		if(fdlg.DoModal()==IDOK) 
			{
			m_Display.LoadWfdiscFileInfo(fdlg.GetPathName());
			}
		return;
		}
	if(cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
		{
		CFileStatus rStatus;
		if(!CFile::GetStatus(cmdInfo.m_strFileName,rStatus))
			{
			char cBuff[MAX_PATH+128];
			sprintf(cBuff,"File '%s'not found",cmdInfo.m_strFileName);
			MessageBox(cBuff,"Error",MB_ICONSTOP);
			OnExit();
			return;
			}

		char path_buffer[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( cmdInfo.m_strFileName, drive, dir, fname, ext );
		lstrcpy(path_buffer,drive);
		lstrcat(path_buffer,dir);
		_chdir(path_buffer);
   		m_Display.LoadWfdiscFileInfo(cmdInfo.m_strFileName);
		}
	}

void CMainFrame::OnSelectChanSta() 
	{
	// TODO: Add your command handler code here

	m_Display.RunSelectDlg();
	
	}
void CMainFrame::OnUpdateView() 
	{
	m_Preview.DrawPreviewBitmap();
	}

void CMainFrame::OnFitToWindow() 
	{
	// TODO: Add your command handler code here
	m_Display.OnFitToWindow();	
	}

void CMainFrame::OnExpandHorizontal() 
	{
	m_Display.OnExpandHorizontal();
	}

void CMainFrame::OnContractHorizontal() 
	{
	m_Display.OnContractHorizontal();
	}

void CMainFrame::OnContractVertical() 
	{
	m_Display.OnContractVertical();
	}

void CMainFrame::OnExpandVertical() 
	{
	m_Display.OnExpandVertical();
	}

void CMainFrame::OnShiftLeft() 
	{
	m_Display.OnShiftToLeft();
	}

void CMainFrame::OnShiftRight() 
	{
	// TODO: Add your command handler code here

	m_Display.OnShiftToRight();
	}


void CMainFrame::OnDown() 
	{
	// TODO: Add your command handler code here
	m_Display.OnShiftToDown();
	}

void CMainFrame::OnUp() 
	{
	// TODO: Add your command handler code here
	m_Display.OnShiftToUp();
	}

void CMainFrame::OnSetTextToStatusbar1(LPSTR lpText)
	{
	CSize size;
	{
		HGDIOBJ hOldFont = NULL;
		HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
		CClientDC dc(NULL);
		if (hFont != NULL) 
			hOldFont = dc.SelectObject(hFont);
		size = dc.GetTextExtent(lpText);
		if (hOldFont != NULL) 
			dc.SelectObject(hOldFont);
	}

	// Update the pane to reflect the current time
	UINT nID, nStyle;
	int nWidth;

	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	if(nWidth<size.cx)
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, size.cx);

	m_wndStatusBar.SetPaneText(1,lpText,TRUE);
	}
void CMainFrame::OnRepaintPreview() 
	{
	m_Preview.InvalidateRect(NULL, FALSE);
	}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR		*pnmhdr=(LPNMHDR )lParam;
	LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam;

	switch (pnmhdr->code)
		{
		case TTN_NEEDTEXT:
			if (lpttt->uFlags & TTF_IDISHWND)
				{
				char szBuff[64];
				UINT nID =pnmhdr->idFrom;
    			nID = ::GetDlgCtrlID((HWND)nID);
				if(nID==IDC_PREVIEWWINDOW)
					{
					*szBuff=0;
					LoadString(AfxGetInstanceHandle(),nID,szBuff,sizeof(szBuff));
					lstrcpy(lpttt->szText,szBuff);
					return TRUE;
					}
				}
			return FALSE;
			break;
		}	
	
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar==VK_SHIFT)
		{
		if(m_Display.xcursor!=-1)
			{
			m_Display.xcursor=-1;
			m_Display.infwnd.ShowWindow(SW_HIDE);
			m_Display.amplinfwnd.ShowWindow(SW_HIDE);
			m_Display.InvalidateRect(NULL,FALSE);
			}
		}
	
	CFrameWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnSelecttraces() 
	{
	// TODO: Add your command handler code here
	m_Display.SelectTraces();
	}

void CMainFrame::OnOriginal() 
	{
	// TODO: Add your command handler code here
	m_Display.Original();
	}

void CMainFrame::OnSelectall() 
	{
	// TODO: Add your command handler code here
	m_Display.SelectAll();
	}

void CMainFrame::OnDeleteall() 
	{
	// TODO: Add your command handler code here
	m_Display.DeleteAll();
	}

void CMainFrame::OnAutoamp() 
	{
	// TODO: Add your command handler code here
	CMenu *menu=GetMenu();
	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bRes1=MaiFrameMenu.ModifyMenu(2,MF_BYPOSITION|MF_POPUP,(UINT)sub1->m_hMenu,"Ampl:A");
	sub1->CheckMenuItem(IDC_AUTOAMP, MF_CHECKED);
	sub1->CheckMenuItem(IDC_AUTO0, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO1, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AMPFIXED, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOA, MF_UNCHECKED);
	DrawMenuBar();
	m_Display.SetAmpFlagAuto();
	}

void CMainFrame::OnAmpfixed() 
	{
	// TODO: Add your command handler code here

	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bRes1=MaiFrameMenu.ModifyMenu(2,MF_BYPOSITION|MF_STRING,(UINT)sub1->m_hMenu,"Ampl:Fx");
	sub1->CheckMenuItem(IDC_AUTOAMP, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO0, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO1, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AMPFIXED, MF_CHECKED);
	sub1->CheckMenuItem(IDC_AUTOA, MF_UNCHECKED);
	DrawMenuBar();
	m_Display.SetAmpFlagFixed();

	}

void CMainFrame::OnCliponoff() 
	{
	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bCleap=m_Display.ClipOnOff();
	BOOL bRes1;

	if(bCleap)
		{
		bRes1=sub1->ModifyMenu(IDC_CLIPONOFF,MF_BYCOMMAND|MF_STRING,IDC_CLIPONOFF,"Clip Off");
		}
	else
		{
		bRes1=sub1->ModifyMenu(IDC_CLIPONOFF,MF_BYCOMMAND|MF_STRING,IDC_CLIPONOFF,"Clip On");
		}

	}

void CMainFrame::OnAutoa() 
	{
	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bRes1=MaiFrameMenu.ModifyMenu(2,MF_BYPOSITION|MF_STRING,(UINT)sub1->m_hMenu,"Ampl:AA");
	sub1->CheckMenuItem(IDC_AMPFIXED, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOAMP, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO0, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO1, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOA, MF_CHECKED);
	DrawMenuBar();
	m_Display.SetAmpFlagAutoA();
	}

void CMainFrame::OnAuto0() 
	{
	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bRes1=MaiFrameMenu.ModifyMenu(2,MF_BYPOSITION|MF_STRING,(UINT)sub1->m_hMenu,"Ampl:A0");
	sub1->CheckMenuItem(IDC_AMPFIXED, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOAMP, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO0, MF_CHECKED);
	sub1->CheckMenuItem(IDC_AUTO1, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOA, MF_UNCHECKED);
	DrawMenuBar();
	m_Display.SetAmpFlagAuto0();
	}

void CMainFrame::OnAuto1() 
	{
	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	BOOL bRes1=MaiFrameMenu.ModifyMenu(2,MF_BYPOSITION|MF_STRING,(UINT)sub1->m_hMenu,"Ampl:A1");
	sub1->CheckMenuItem(IDC_AMPFIXED, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTOAMP, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO0, MF_UNCHECKED);
	sub1->CheckMenuItem(IDC_AUTO1, MF_CHECKED);
	sub1->CheckMenuItem(IDC_AUTOA, MF_UNCHECKED);

	DrawMenuBar();
	m_Display.SetAmpFlagAuto1();
	}

void CMainFrame::OnInvert() 
	{
	// TODO: Add your command handler code here
	m_Display.FlipInvertFlag();
	}

void CMainFrame::OnSetgain1() 
	{
	// TODO: Add your command handler code here
	m_Display.ChangeGain(1,0);
	}
void CMainFrame::OnSetFilter(UINT nID )
	{
	int nFilter=nID-IDC_FILTER1;
	m_Display.SetFilterNumber(nFilter);
	}
void CMainFrame::OnChangeGain(UINT nID )
	{

	CMenu *sub1=MaiFrameMenu.GetSubMenu(2);
	CString cstr;
	sub1->GetMenuString( nID, cstr, MF_BYCOMMAND);
	LPCSTR lpStr=(LPCSTR)cstr;
	int nn=0,nnn=0;

	if( cstr[4]=='X' || cstr[4]=='x' )
		{
		double ddg=0;
//		ddg=atof(&lpStr[2]);
		sscanf(&lpStr[5],"%d%*[., ]%d",&nn,&nnn);
		if(nnn!=0)
			{
			ddg=nn+(double)nnn/pow(10,ceil(log10(nnn)));
			}
		else
			{
			ddg=nn;
			}
		m_Display.ChangeGain(ddg,1);
		}
	else
		if( cstr[4]=='/')
		{
		double ddg;
		sscanf(&lpStr[5],"%d%*[., ]%d",&nn,&nnn);
		if(nnn!=0)
			{
			ddg=nn+(double)nnn/pow(10,ceil(log10(nnn)));
			}
		else
			{
			ddg=nn;
			}
		ddg=1/ddg;
		m_Display.ChangeGain(ddg,1);
		}

	}

void CMainFrame::OnSettime() 
	{
	m_Display.RunSetTimeDlg();
	}

void CMainFrame::OnExit() 
	{
	DestroyWindow();
	}

void CMainFrame::OnXfit() 
	{
	m_Display.Fit();
	}

void CMainFrame::OnDeletetraces() 
	{
	m_Display.DeleteTraces(); 
	}

void CMainFrame::OnFilternone() 
	{
	// TODO: Add your command handler code here
	m_Display.SetFilterNumber(-1);
	}

void CMainFrame::OnPrint() 
	{
	// TODO: Add your command handler code here
	m_Display.Print();
	}
