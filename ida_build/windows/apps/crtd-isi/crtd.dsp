# Microsoft Developer Studio Project File - Name="crtd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=crtd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "crtd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "crtd.mak" CFG="crtd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crtd - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "crtd - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "crtd - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\lib\iacp\Release\iacp.lib ..\..\lib\isi\Release\isi.lib ..\..\lib\util\Release\util.lib ..\..\lib\logio\Release\logio.lib ..\..\lib\cssio\Release\cssio.lib  ..\..\lib\win32\Release\win32.lib /subsystem:windows /machine:I386 /out:"Release/crtd-isi.exe"
# SUBTRACT LINK32 /nologo

!ELSEIF  "$(CFG)" == "crtd - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\lib\iacp\Debug\iacp.lib ..\..\lib\isi\Debug\isi.lib ..\..\lib\util\Debug\util.lib ..\..\lib\logio\Debug\logio.lib ..\..\lib\cssio\Debug\cssio.lib  ..\..\lib\win32\Debug\win32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/crtd-isi.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "crtd - Win32 Release"
# Name "crtd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\crtd.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\crtd.rc
# End Source File
# Begin Source File

SOURCE=.\crtdglob.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\DataBlock.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\DisplayParamDlg.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\DrawBitmap.cpp

!IF  "$(CFG)" == "crtd - Win32 Release"

!ELSEIF  "$(CFG)" == "crtd - Win32 Debug"

# ADD CPP /Zp8

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrawListView.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\MainFrame.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\MWindow.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\MWindowX.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkPage.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\NRTSCHANINFO.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\NRTSINFO.cpp

!IF  "$(CFG)" == "crtd - Win32 Release"

# ADD CPP /Zp8

!ELSEIF  "$(CFG)" == "crtd - Win32 Debug"

# ADD CPP /Zp8
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NRTSSTAINFO.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\Preview.cpp

!IF  "$(CFG)" == "crtd - Win32 Release"

# ADD CPP /Zp8

!ELSEIF  "$(CFG)" == "crtd - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RTDisplay.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\SelectChan.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp

!IF  "$(CFG)" == "crtd - Win32 Release"

# ADD CPP /Zp8

!ELSEIF  "$(CFG)" == "crtd - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Zp8 /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=.\TIMEFUNC.CPP
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\crtd.h
# End Source File
# Begin Source File

SOURCE=.\crtdDoc.h
# End Source File
# Begin Source File

SOURCE=.\crtdglob.h
# End Source File
# Begin Source File

SOURCE=.\crtdView.h
# End Source File
# Begin Source File

SOURCE=.\DataBlock.h
# End Source File
# Begin Source File

SOURCE=.\DBPickView.h
# End Source File
# Begin Source File

SOURCE=.\DisplayParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\DrawListView.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MWindow.h
# End Source File
# Begin Source File

SOURCE=.\MWindowX.h
# End Source File
# Begin Source File

SOURCE=.\NetworkPage.h
# End Source File
# Begin Source File

SOURCE=.\NRTSCHANINFO.h
# End Source File
# Begin Source File

SOURCE=.\NRTSINFO.h
# End Source File
# Begin Source File

SOURCE=.\NRTSSTAINFO.h
# End Source File
# Begin Source File

SOURCE=.\Preview.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectChan.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TimeFunc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Ckunck.bmp
# End Source File
# Begin Source File

SOURCE=.\res\crtd.ico
# End Source File
# Begin Source File

SOURCE=.\res\crtd.rc2
# End Source File
# Begin Source File

SOURCE=.\res\crtdDoc.ico
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbarr.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section crtd : {72ADFD78-2C39-11D0-9903-00A0C91BC942}
# 	1:10:IDB_SPLASH:107
# 	2:21:SplashScreenInsertKey:4.0
# End Section
