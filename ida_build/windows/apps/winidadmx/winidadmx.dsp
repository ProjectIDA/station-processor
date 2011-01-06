# Microsoft Developer Studio Project File - Name="winidadmx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winidadmx - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winidadmx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winidadmx.mak" CFG="winidadmx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winidadmx - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winidadmx - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winidadmx - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ../../lib/zlib/zdll.lib ../../lib/ida/Release/ida.lib ../../lib/util/Release/util.lib ../../lib/win32/Release/win32.lib ../../lib/logio/Release/logio.lib ../../lib/ida10/Release/ida10.lib wsock32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "winidadmx - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../../lib/zlib/zdll.lib ../../lib/ida/Debug/ida.lib ../../lib/util/Debug/util.lib ../../lib/win32/Debug/win32.lib ../../lib/logio/Debug/logio.lib ../../lib/ida10/Debug/ida10.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "winidadmx - Win32 Release"
# Name "winidadmx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aspiscsi.cpp

!IF  "$(CFG)" == "winidadmx - Win32 Release"

# ADD CPP /Zp8

!ELSEIF  "$(CFG)" == "winidadmx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BrowseSubdirDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BTNST.CPP

!IF  "$(CFG)" == "winidadmx - Win32 Release"

# ADD CPP /Zp8

!ELSEIF  "$(CFG)" == "winidadmx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DialogWB.cpp
# End Source File
# Begin Source File

SOURCE=.\DragList.cpp
# End Source File
# Begin Source File

SOURCE=.\DropFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\EmptyDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\scanwfdline.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectFilesOnCDDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadeButtonST.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SubEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewLstDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewResults.cpp
# End Source File
# Begin Source File

SOURCE=.\winidadmx.cpp
# End Source File
# Begin Source File

SOURCE=.\winidadmx.rc
# End Source File
# Begin Source File

SOURCE=.\winidadmxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\xDib.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BrowseSubdirDlg.h
# End Source File
# Begin Source File

SOURCE=.\devlist.h
# End Source File
# Begin Source File

SOURCE=.\DialogWB.h
# End Source File
# Begin Source File

SOURCE=.\DragList.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ida.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectFilesOnCDDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ViewLstDlg.h
# End Source File
# Begin Source File

SOURCE=.\ViewResults.h
# End Source File
# Begin Source File

SOURCE=.\winidadmx.h
# End Source File
# Begin Source File

SOURCE=.\winidadmxDlg.h
# End Source File
# Begin Source File

SOURCE=.\WorkingDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Clsdfold.ico
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Note11.ico
# End Source File
# Begin Source File

SOURCE=.\res\treeimg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\winidadmx.ico
# End Source File
# Begin Source File

SOURCE=.\res\winidadmx.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
