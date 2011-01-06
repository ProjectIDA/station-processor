; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDisplayParamDlg
LastTemplate=CFrameWnd
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "crtd.h"
LastPage=0

ClassCount=10
Class1=CCrtdApp
Class2=CAboutDlg

ResourceCount=15
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_DISPLAYPARAMDLG
Class3=MainFrame
Class4=CRTDisplay
Resource4=IDD_SELECT
Resource5=IDR_MAINFRAME (English (U.S.))
Class5=CSelectChan
Class6=CNetworkPage
Resource6=IDD_NETWORKDLG
Class7=CDisplayParamDlg
Resource7=IDR_TOOLBARRUN (English (U.S.))
Class8=CPreview
Resource8=IDD_NETWORKDLG (English (U.S.))
Resource9=IDD_SELECT (Russian)
Resource10=IDD_NETWORKDLG (Russian)
Resource11=IDD_DISPLAYPARAMDLG (Russian)
Resource12=IDR_TOOLBARRUN
Resource13=IDD_SELECT (English (U.S.))
Resource14=IDD_ABOUTBOX (English (U.S.))
Class9=CSplashWnd
Class10=CDBPickView
Resource15=IDD_DISPLAYPARAMDLG (English (U.S.))

[CLS:CCrtdApp]
Type=0
HeaderFile=crtd.h
ImplementationFile=crtd.cpp
Filter=N
LastObject=CCrtdApp

[CLS:CAboutDlg]
Type=0
HeaderFile=crtd.cpp
ImplementationFile=crtd.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_APP_EXIT
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_APP_ABOUT
CommandCount=12

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_RUN
Command2=ID_CONFIG
Command3=ID_APP_ABOUT
CommandCount=3

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_APP_EXIT
Command6=ID_EDIT_UNDO
Command7=ID_EDIT_CUT
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_APP_ABOUT
CommandCount=12

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:MainFrame]
Type=0
HeaderFile=MainFrame.h
ImplementationFile=MainFrame.cpp
BaseClass=CFrameWnd
Filter=T
LastObject=IDC_RUNDBPICK
VirtualFilter=fWC

[CLS:CRTDisplay]
Type=0
HeaderFile=RTDisplay.h
ImplementationFile=RTDisplay.cpp
BaseClass=CWnd
Filter=W
LastObject=CRTDisplay
VirtualFilter=WC

[DLG:IDD_SELECT]
Type=1
Class=CSelectChan
ControlCount=3
Control1=IDC_LOADLIST,button,1342242816
Control2=IDC_CHAN,SysListView32,1350632449
Control3=IDC_STA,SysListView32,1350632449

[DLG:IDD_NETWORKDLG]
Type=1
Class=CNetworkPage
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_NRTSSERVER,edit,1350631552
Control4=IDC_PORT,edit,1350631552

[CLS:CSelectChan]
Type=0
HeaderFile=SelectChan.h
ImplementationFile=SelectChan.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CSelectChan

[CLS:CNetworkPage]
Type=0
HeaderFile=NetworkPage.h
ImplementationFile=NetworkPage.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=IDC_PORT

[DLG:IDD_DISPLAYPARAMDLG]
Type=1
Class=CDisplayParamDlg
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DISPLAYTIME,edit,1350631552
Control5=IDC_BUFFEREDTIME,edit,1350631552
Control6=IDC_REFRESHTIME,edit,1350631552

[CLS:CDisplayParamDlg]
Type=0
HeaderFile=DisplayParamDlg.h
ImplementationFile=DisplayParamDlg.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=IDC_DISPLAYTIME
VirtualFilter=idWC

[CLS:CPreview]
Type=0
HeaderFile=Preview.h
ImplementationFile=Preview.cpp
BaseClass=CWnd
Filter=W
LastObject=CPreview
VirtualFilter=WC

[TB:IDR_TOOLBARRUN (English (U.S.))]
Type=1
Class=?
Command1=IDC_STOP
Command2=IDC_AUTOSCALE
Command3=IDC_EXPANDVERT
Command4=IDC_CONTRACTVERT
Command5=IDC_SHIFTDOWN
Command6=IDC_SHIFTUP
CommandCount=6

[DLG:IDD_SELECT (Russian)]
Type=1
ControlCount=3
Control1=IDC_LOADLIST,button,1342242816
Control2=IDC_CHAN,SysListView32,1350632449
Control3=IDC_STA,SysListView32,1350632449
Class=CSelectChan

[DLG:IDD_NETWORKDLG (Russian)]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_NRTSSERVER,edit,1350631552
Control4=IDC_PORT,edit,1350631552
Class=CNetworkPage

[DLG:IDD_DISPLAYPARAMDLG (Russian)]
Type=1
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DISPLAYTIME,edit,1350631552
Control5=IDC_BUFFEREDTIME,edit,1350631552
Control6=IDC_REFRESHTIME,edit,1350631552
Class=CDisplayParamDlg

[TB:IDR_MAINFRAME]
Type=1
Command1=ID_RUN
Command2=ID_CONFIG
Command3=ID_APP_ABOUT
CommandCount=3

[TB:IDR_TOOLBARRUN]
Type=1
Command1=IDC_STOP
Command2=IDC_AUTOSCALE
Command3=IDC_EXPANDVERT
Command4=IDC_CONTRACTVERT
Command5=IDC_SHIFTDOWN
Command6=IDC_SHIFTUP
CommandCount=6

[DLG:IDD_NETWORKDLG (English (U.S.))]
Type=1
Class=CNetworkPage
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_NRTSSERVER,edit,1350631552
Control4=IDC_PORT,edit,1350631552

[DLG:IDD_DISPLAYPARAMDLG (English (U.S.))]
Type=1
Class=CDisplayParamDlg
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DISPLAYTIME,edit,1350631552
Control5=IDC_BUFFEREDTIME,edit,1350631552
Control6=IDC_REFRESHTIME,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_DELAY,edit,1350631552

[DLG:IDD_SELECT (English (U.S.))]
Type=1
Class=CSelectChan
ControlCount=3
Control1=IDC_LOADLIST,button,1342242816
Control2=IDC_CHAN,SysListView32,1350632449
Control3=IDC_STA,SysListView32,1350632449

[CLS:CSplashWnd]
Type=0
HeaderFile=Splash.h
ImplementationFile=Splash.cpp
BaseClass=CWnd

[CLS:CDBPickView]
Type=0
HeaderFile=DBPickView.h
ImplementationFile=DBPickView.cpp
BaseClass=CFrameWnd
Filter=T
VirtualFilter=fWC
LastObject=CDBPickView


