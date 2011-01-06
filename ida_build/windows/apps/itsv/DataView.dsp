# Microsoft Developer Studio Project File - Name="DataView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DataView - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DataView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DataView.mak" CFG="DataView - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DataView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DataView - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DataView - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/ITSV.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DataView - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/ITSV.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DataView - Win32 Release"
# Name "DataView - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "filter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\filter\butwth.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\filter\gpl_filter_seg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\filter\GPL_filters.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\filter\hook.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChanInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DataView.cpp
# End Source File
# Begin Source File

SOURCE=.\DataView.rc
# End Source File
# Begin Source File

SOURCE=.\DataView_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\DataView_misc.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\DrawListView.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\msmask.cpp
# End Source File
# Begin Source File

SOURCE=.\MWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PrintDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenElem.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetStartTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Sta.cpp
# End Source File
# Begin Source File

SOURCE=.\StaInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Swap.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Time2String.cpp
# End Source File
# Begin Source File

SOURCE=.\TIMEFUNC.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\WaveformPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveformsDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveformsPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\wfd_misc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\WfdiscRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\WfdiscRecordsArray.cpp
# End Source File
# Begin Source File

SOURCE=.\WRDSIZE.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChanInfo.h
# End Source File
# Begin Source File

SOURCE=.\DataBlock.h
# End Source File
# Begin Source File

SOURCE=.\DataView.h
# End Source File
# Begin Source File

SOURCE=.\DataViewDoc.h
# End Source File
# Begin Source File

SOURCE=.\DataViewView.h
# End Source File
# Begin Source File

SOURCE=.\filter\filter.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\InfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\msmask.h
# End Source File
# Begin Source File

SOURCE=.\MWindow.h
# End Source File
# Begin Source File

SOURCE=.\OpenFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PrintDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Screen.h
# End Source File
# Begin Source File

SOURCE=.\ScreenElem.h
# End Source File
# Begin Source File

SOURCE=.\SelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetStartTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Sta.h
# End Source File
# Begin Source File

SOURCE=.\StaInfo.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WaveformPreview.h
# End Source File
# Begin Source File

SOURCE=.\WaveformsDisplay.h
# End Source File
# Begin Source File

SOURCE=.\WaveformsPrint.h
# End Source File
# Begin Source File

SOURCE=.\WfdiscRecord.h
# End Source File
# Begin Source File

SOURCE=.\WfdiscRecordsArray.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Ckunck.bmp
# End Source File
# Begin Source File

SOURCE=.\res\DataView.ico
# End Source File
# Begin Source File

SOURCE=.\res\DataView.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DataViewDoc.ico
# End Source File
# Begin Source File

SOURCE=.\RES\H_move.cur
# End Source File
# Begin Source File

SOURCE=.\RES\hand.cur
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\ReleaseNotes.txt
# End Source File
# End Target
# End Project
# Section DataView : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section DataView : {00000001-0000-0000-0000-000000000000}
# 	1:10:IDB_SPLASH:104
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section DataView : {C932BA85-4374-101B-A56C-00AA003668DC}
# 	2:21:DefaultSinkHeaderFile:msmask.h
# 	2:16:DefaultSinkClass:CMSMask
# End Section
# Section DataView : {A82AB343-BB35-11CF-8771-00A0C9039735}
# 	1:17:ID_INDICATOR_TIME:103
# 	1:17:ID_INDICATOR_DATE:102
# 	2:2:BH:
# 	2:17:ID_INDICATOR_TIME:ID_INDICATOR_TIME
# 	2:17:ID_INDICATOR_DATE:ID_INDICATOR_DATE
# End Section
# Section DataView : {4D6CC9A0-DF77-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:CMSMask
# 	2:10:HeaderFile:msmask.h
# 	2:8:ImplFile:msmask.cpp
# End Section
# Section DataView : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
