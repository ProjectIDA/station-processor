# Microsoft Developer Studio Project File - Name="isitapqt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=isitapqt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "isitapqt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "isitapqt.mak" CFG="isitapqt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "isitapqt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "isitapqt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" delayimp.lib "..\..\lib\iacp\Release\iacp.lib" "..\..\lib\isi\Release\isi.lib" "..\..\lib\util\Release\util.lib" "..\..\lib\logio\Release\logio.lib" "..\..\lib\cssio\Release\cssio.lib" "..\..\lib\win32\Release\win32.lib" /nologo /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /nodefaultlib:"MSVCRTD.lib" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "c:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\isiTapqt" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "UNICODE" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" "..\..\lib\iacp\Debug\iacp.lib" "..\..\lib\isi\Debug\isi.lib" "..\..\lib\util\Debug\util.lib" "..\..\lib\logio\Debug\logio.lib" "..\..\lib\cssio\Debug\cssio.lib" "..\..\lib\win32\Debug\win32.lib" /nologo /subsystem:windows /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "isitapqt - Win32 Release"
# Name "isitapqt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\configDlg.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\DateTimeEdit.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\GettingDataThread.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\ISI_GENERIC_TS_Packet.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isitap_globals.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isitap_time.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isitapdlg.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isiTapResults.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\main.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\nrtschaninfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\nrtsinfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\nrtsstainfo.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\ReadFileCache.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\thread.cpp

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\WFDISC.CPP

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# ADD CPP /MT

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\configDlg.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\configDlg.h

"moc_configDlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  "..\..\..\bin\isiTapqt\configDlg.h" -o moc_configDlg.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\configDlg.h

"moc_configDlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\configDlg.h -o moc_configDlg.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\DateTimeEdit.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\GettingDataThread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isitapdlg.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\isitapdlg.h

"moc_isitapdlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isitapdlg.h -o moc_isitapdlg.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\isitapdlg.h

"moc_isitapdlg.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isitapdlg.h -o moc_isitapdlg.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\isiTapResults.h

!IF  "$(CFG)" == "isitapqt - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\isiTapResults.h

"moc_isiTapResults.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isiTapResults.h -o moc_isiTapResults.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "isitapqt - Win32 Debug"

# Begin Custom Build
InputPath=..\..\..\bin\isiTapqt\isiTapResults.h

"moc_isiTapResults.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\isiTapqt\isiTapResults.h -o moc_isiTapResults.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isiTapqt\thread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\moc_configDlg.cpp
# End Source File
# Begin Source File

SOURCE=moc_isitapdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_isiTapResults.cpp
# End Source File
# End Group
# End Target
# End Project
