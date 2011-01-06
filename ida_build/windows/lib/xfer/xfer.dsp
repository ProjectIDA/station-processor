# Microsoft Developer Studio Project File - Name="xfer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=xfer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xfer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xfer.mak" CFG="xfer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xfer - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "xfer - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xfer - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "xfer - Win32 Debug"

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
# ADD CPP /nologo /Zp8 /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "xfer - Win32 Release"
# Name "xfer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\xfer\client.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\common_r.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\convert.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\ezio.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\globals.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\log.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\misc.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\packet.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\print.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\rdwr.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\server.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\site.c
# ADD CPP /I "../../../include"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\xfer\tocss.c
# ADD CPP /I "../../../include"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
