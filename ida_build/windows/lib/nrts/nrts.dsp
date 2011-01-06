# Microsoft Developer Studio Project File - Name="nrts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=nrts - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nrts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nrts.mak" CFG="nrts - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nrts - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "nrts - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nrts - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\win32" /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "INCLUDE_NRTS_BWD_DB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "nrts - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\win32" /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "INCLUDE_NRTS_BWD_DB" /YX /FD /c
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

# Name "nrts - Win32 Release"
# Name "nrts - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\nrts\asldecode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\bwd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\bwdb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\chksys.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\decode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\die.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\dl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\files.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\fixwd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\getmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\ida.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\ida10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\ida10decode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\idadecode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\indx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\liss.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\mmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\oldbwd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\prt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\rcnf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\read.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\search.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\stawd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\syscode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\sysident.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\systems.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\syswd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\time.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\version.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nrts\wrtdl.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\cssio.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\iacp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ida.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ida10.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\nrts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\util.h
# End Source File
# End Group
# End Target
# End Project
