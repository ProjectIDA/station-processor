# Microsoft Developer Studio Project File - Name="crtdqt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=crtdqt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "crtdqt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "crtdqt.mak" CFG="crtdqt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crtdqt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "crtdqt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "crtdqt - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "../../../include" /I "$(QTDIR)\include" /I "C:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\crtdqt-isi" /I "C:\Qt\3.3.3\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /D "QT_CLEAN_NAMESPACE" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "qtmain.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" delayimp.lib ..\..\lib\iacp\Release\iacp.lib ..\..\lib\isi\Release\isi.lib ..\..\lib\util\Release\util.lib ..\..\lib\logio\Release\logio.lib ..\..\lib\cssio\Release\cssio.lib /nologo /subsystem:windows /machine:IX86 /nodefaultlib:"msvcrt.lib" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "crtdqt - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "$(QTDIR)\include" /I "C:\USER\AKIMOV\02102005\IDA_NRTS_SRC\windows\apps\crtdqt-isi" /I "C:\Qt\3.3.4\mkspecs\win32-msvc" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "QT_THREAD_SUPPORT" /D "QT_CLEAN_NAMESPACE" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "opengl32.lib" "glu32.lib" "delayimp.lib" "..\..\lib\iacp\Debug\iacp.lib" "..\..\lib\isi\Debug\isi.lib" "..\..\lib\util\Debug\util.lib" "..\..\lib\logio\Debug\logio.lib" "qt-mt334.lib" "qtmain.lib" "..\..\lib\cssio\Debug\cssio.lib" /nologo /subsystem:windows /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib

!ENDIF 

# Begin Target

# Name "crtdqt - Win32 Release"
# Name "crtdqt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\CRTDGLOB.CPP"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\crtdqtui.ui.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\DataBlock.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\DrawWaveformThread.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\GettingDataThread.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\IsiInterruptThread.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\main.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\MainFrame.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\MainFrame.h"

!IF  "$(CFG)" == "crtdqt - Win32 Release"

# Begin Custom Build
InputPath="..\..\..\bin\crtdqt-isi\MainFrame.h"

"moc_crtdqtui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\crtdqt-isi\MainFrame.h -o moc_crtdqtui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "crtdqt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath="..\..\..\bin\crtdqt-isi\MainFrame.h"

"moc_crtdqtui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\crtdqt-isi\MainFrame.h -o moc_crtdqtui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\mwindow.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\NRTSCHANINFO.CPP"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\NRTSINFO.CPP"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\NRTSSTAINFO.CPP"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\Preview.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\qmake_image_collection.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\QmyButton.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\QmyToolBar.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\RTDisplay.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\set_all_parameters.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\set_all_parameters.h"

!IF  "$(CFG)" == "crtdqt - Win32 Release"

# Begin Custom Build
InputPath="..\..\..\bin\crtdqt-isi\set_all_parameters.h"

"moc_set_all_parameters.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\crtdqt-isi\set_all_parameters.h -o moc_set_all_parameters.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "crtdqt - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath="..\..\..\bin\crtdqt-isi\set_all_parameters.h"

"moc_set_all_parameters.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc  ..\..\..\bin\crtdqt-isi\set_all_parameters.h -o moc_set_all_parameters.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\StopAcquisitionThread.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\thread.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\DrawWaveformThread.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\GettingDataThread.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\QmyButton.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\QmyToolBar.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\bin\crtdqt-isi\StopAcquisitionThread.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=moc_crtdqtui.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_set_all_parameters.cpp
# End Source File
# End Group
# End Target
# End Project
