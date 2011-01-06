# Microsoft Developer Studio Project File - Name="util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "util.mak" CFG="util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "util - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "util - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "util - Win32 Release"

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

!ELSEIF  "$(CFG)" == "util - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
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

# Name "util - Win32 Release"
# Name "util - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\util\atolk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\basename.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\bcd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\bground.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\binprint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\BufferedStream.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\case.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\cat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\cfgpair.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\etoh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\getline.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\gsecmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\hexdump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\htoe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\isint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\ldump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\lenprt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\lock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\mkfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\mkpath.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\mmap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\myip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\pack.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\peer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\pow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\query.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\rwtimeo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\setid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\signal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\steim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\string.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\swap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\syserr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\timefunc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\util\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\util.h
# End Source File
# End Group
# End Target
# End Project
