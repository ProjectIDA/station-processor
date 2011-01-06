; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPrintDlg
LastTemplate=CPrintDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "DataView.h"
LastPage=0

ClassCount=13
Class1=CDataViewApp
Class2=CDataViewDoc
Class3=CDataViewView
Class4=CMainFrame
Class7=CWaveformPreview

ResourceCount=13
Resource1=IDD_OPENFILEDIALOG
Class5=CAboutDlg
Class6=CSplashWnd
Class8=CWaveformsDisplay
Resource2=IDD_SETTIMEDLG (English (U.S.))
Class9=CSelectDlg
Class10=CInfoWindow
Resource3=IDD_SELECT
Class11=OpenFileDlg
Resource4=IDD_ABOUTBOX
Resource5=IDD_OPENFILEDIALOG (English (U.S.))
Resource6=IDR_MAINFRAME (English (U.S.))
Resource7=IDD_ABOUTBOX (English (U.S.))
Resource8=IDR_MENUAUTOAMPL (English (U.S.))
Resource9=IDR_MENUFIXEDAMPL (English (U.S.))
Resource10=IDD_SETTIMEDLG
Class12=CSetStartTimeDlg
Resource11=IDR_MAINFRAME
Class13=CPrintDlg
Resource12=IDD_SELECT (English (U.S.))
Resource13=1538 (English (U.S.))

[CLS:CDataViewApp]
Type=0
HeaderFile=DataView.h
ImplementationFile=DataView.cpp
Filter=N
LastObject=CDataViewApp

[CLS:CDataViewDoc]
Type=0
HeaderFile=DataViewDoc.h
ImplementationFile=DataViewDoc.cpp
Filter=N

[CLS:CDataViewView]
Type=0
HeaderFile=DataViewView.h
ImplementationFile=DataViewView.cpp
Filter=C

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=IDC_PRINT
BaseClass=CFrameWnd
VirtualFilter=fWC



[CLS:CAboutDlg]
Type=0
HeaderFile=DataView.cpp
ImplementationFile=DataView.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342373889
Control3=IDC_STATIC,static,1342308352
Control4=IDC_ESSWSTATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=IDC_SELECTCHAN
Command3=IDC_EXIT
Command4=IDC_SELECTTRACES
Command5=IDC_DELETETRACES
Command6=IDC_ORIGINAL
Command7=IDC_SELECTALL
Command8=IDC_DELETEALL
Command9=IDC_AMPFIXED
Command10=IDC_AUTOAMP
Command11=IDC_AUTO0
Command12=IDC_AUTO1
Command13=IDC_AUTOA
Command14=IDC_CLIPONOFF
Command15=IDC_INVERT
Command16=IDC_SETGAIN_1
Command17=IDC_XFIT
Command18=IDC_SETTIME
Command19=ID_APP_ABOUT
CommandCount=19

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

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=IDC_SELECTCHAN
Command3=IDC_DOWN
Command4=IDC_UP
Command5=IDC_SHIFTLEFT
Command6=IDC_SHIFTRIGHT
Command7=IDC_FITTOWINDOW
Command8=IDC_EXPANDVERTICAL
Command9=IDC_CONTRACTVERT
Command10=IDC_CONTRACTHORZ
Command11=IDC_EXPANDHOR
Command12=IDC_ALIGNVERTICAL
Command13=ID_APP_ABOUT
CommandCount=13

[CLS:CSplashWnd]
Type=0
HeaderFile=Splash.h
ImplementationFile=Splash.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CSplashWnd

[CLS:CWaveformPreview]
Type=0
HeaderFile=WaveformPreview.h
ImplementationFile=WaveformPreview.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CWaveformPreview

[CLS:CWaveformsDisplay]
Type=0
HeaderFile=WaveformsDisplay.h
ImplementationFile=WaveformsDisplay.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CWaveformsDisplay

[DLG:IDD_SELECT]
Type=1
Class=CSelectDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STA,SysListView32,1350665221
Control4=IDC_STATIC,button,1342177287
Control5=IDC_CHAN,SysListView32,1350665221
Control6=IDC_SELECT_ALL,button,1342242816
Control7=IDC_UNSELECT_ALL,button,1342242816

[CLS:CSelectDlg]
Type=0
HeaderFile=SelectDlg.h
ImplementationFile=SelectDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_UNSELECT_ALL

[CLS:CInfoWindow]
Type=0
HeaderFile=InfoWindow.h
ImplementationFile=InfoWindow.cpp
BaseClass=CWnd
Filter=W
LastObject=CInfoWindow
VirtualFilter=WC

[DLG:IDD_OPENFILEDIALOG]
Type=1
Class=?
ControlCount=1
Control1=IDC_LIST,SysListView32,1350665221

[CLS:OpenFileDlg]
Type=0
HeaderFile=OpenFileDlg.h
ImplementationFile=OpenFileDlg.cpp
BaseClass=CFileDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_APP_ABOUT

[DLG:IDD_SELECT (English (U.S.))]
Type=1
Class=CSelectDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STA,SysListView32,1350665221
Control4=IDC_STATIC,button,1342177287
Control5=IDC_CHAN,SysListView32,1350665221
Control6=IDC_SELECT_ALL,button,1342242816
Control7=IDC_UNSELECT_ALL,button,1342242816

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=IDC_SELECTCHAN
Command3=IDC_DOWN
Command4=IDC_UP
Command5=IDC_SHIFTLEFT
Command6=IDC_SHIFTRIGHT
Command7=IDC_FITTOWINDOW
Command8=IDC_EXPANDVERTICAL
Command9=IDC_CONTRACTVERT
Command10=IDC_CONTRACTHORZ
Command11=IDC_EXPANDHOR
Command12=IDC_ALIGNVERTICAL
Command13=ID_APP_ABOUT
CommandCount=13

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=IDC_SELECTCHAN
Command3=IDC_PRINT
Command4=IDC_EXIT
Command5=IDC_SELECTTRACES
Command6=IDC_DELETETRACES
Command7=IDC_ORIGINAL
Command8=IDC_SELECTALL
Command9=IDC_DELETEALL
Command10=IDC_AMPFIXED
Command11=IDC_AUTOAMP
Command12=IDC_AUTO0
Command13=IDC_AUTO1
Command14=IDC_AUTOA
Command15=IDC_CLIPONOFF
Command16=IDC_INVERT
Command17=IDC_SETGAIN_1
Command18=IDC_FILTERNONE
Command19=IDC_XFIT
Command20=IDC_SETTIME
Command21=ID_APP_ABOUT
CommandCount=21

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
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342373889
Control3=IDC_STATIC,static,1342308352
Control4=IDC_ESSWSTATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_OPENFILEDIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_LIST,SysListView32,1350665221

[MNU:IDR_MENUFIXEDAMPL (English (U.S.))]
Type=1
Class=?
Command1=IDC_AMPFIXED
Command2=IDC_AUTOAMP
CommandCount=2

[MNU:IDR_MENUAUTOAMPL (English (U.S.))]
Type=1
Class=?
Command1=ID_FIXED
Command2=IDC_AUTOAMP
CommandCount=2

[DLG:IDD_SETTIMEDLG (English (U.S.))]
Type=1
Class=CSetStartTimeDlg
ControlCount=9
Control1=IDC_DURATION,edit,1350631552
Control2=IDC_RADIO1,button,1342308361
Control3=IDC_RADIO2,button,1342177289
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STARTTIME1,{C932BA85-4374-101B-A56C-00AA003668DC},1342242816
Control9=IDC_STARTTIME2,{C932BA85-4374-101B-A56C-00AA003668DC},1342242816

[CLS:CSetStartTimeDlg]
Type=0
HeaderFile=SetStartTimeDlg.h
ImplementationFile=SetStartTimeDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CSetStartTimeDlg
VirtualFilter=dWC


[DLG:IDD_SETTIMEDLG]
Type=1
Class=CSetStartTimeDlg
ControlCount=9
Control1=IDC_DURATION,edit,1350631552
Control2=IDC_RADIO1,button,1342308361
Control3=IDC_RADIO2,button,1342177289
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STARTTIME1,{C932BA85-4374-101B-A56C-00AA003668DC},1342242816
Control9=IDC_STARTTIME2,{C932BA85-4374-101B-A56C-00AA003668DC},1342242816

[CLS:CPrintDlg]
Type=0
HeaderFile=PrintDlg.h
ImplementationFile=PrintDlg.cpp
BaseClass=CPrintDialog
Filter=D
LastObject=1025
VirtualFilter=dWC

[DLG:1538 (English (U.S.))]
Type=1
Class=CPrintDlg
ControlCount=33
Control1=1075,button,1342308359
Control2=1093,static,1342308352
Control3=1139,combobox,1344471299
Control4=1025,button,1342373888
Control5=1095,static,1342308480
Control6=1099,static,1342308492
Control7=1094,static,1342308480
Control8=1098,static,1342308492
Control9=1097,static,1342308480
Control10=1101,static,1342308492
Control11=1096,static,1342308480
Control12=1100,static,1342308492
Control13=1040,button,1342373891
Control14=1072,button,1073872903
Control15=1056,button,1073938441
Control16=1058,button,1073741833
Control17=1057,button,1073741833
Control18=1089,static,1073872898
Control19=1152,edit,1082335232
Control20=1090,static,1073872898
Control21=1153,edit,1082335232
Control22=1073,button,1073872903
Control23=1092,static,1073872896
Control24=1086,static,1073873411
Control25=1041,button,1073938435
Control26=IDOK,button,1342373889
Control27=IDCANCEL,button,1342242816
Control28=IDC_CHECK2,button,1073807363
Control29=IDC_CHECK1,button,1073807363
Control30=IDC_CHECK3,button,1073807363
Control31=IDC_HEADERTEXT,edit,1350631552
Control32=IDC_STATIC,static,1342308352
Control33=1154,edit,1082196096

