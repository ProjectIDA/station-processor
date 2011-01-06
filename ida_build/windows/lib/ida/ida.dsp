# Microsoft Developer Studio Project File - Name="ida" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ida - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ida.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ida.mak" CFG="ida - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ida - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ida - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ida - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ida - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ida - Win32 Release"
# Name "ida - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\ida\cfs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\chnlocmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\cnfrec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\cnfrec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\crec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\crec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dcmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhead9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\dhlen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\drec9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\evtrec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\evtrec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\ext1hzoff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\filltime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\format.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\handle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\idrec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\idrec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\logrec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\logrec5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\rtype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\sampok.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\srec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\srec8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\timerr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\timstr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\ida\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\lib\ida\protos.h
# End Source File
# End Group
# End Target
# End Project
