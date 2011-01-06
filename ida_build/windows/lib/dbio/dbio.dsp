# Microsoft Developer Studio Project File - Name="dbio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dbio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dbio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbio.mak" CFG="dbio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbio - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dbio - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbio - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\win32\mysql" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_MYSQL" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dbio - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\..\..\include" /I "..\..\..\include\win32\mysql" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_MYSQL" /YX /FD /GZ /c
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

# Name "dbio - Win32 Release"
# Name "dbio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\dbio\close.c

!IF  "$(CFG)" == "dbio - Win32 Release"

# ADD CPP /Zp8 /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "dbio - Win32 Debug"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dbio\ff.c
# ADD CPP /I "..\..\..\include\win32\mysql"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dbio\glob.c
# ADD CPP /I "..\..\..\include\win32\mysql"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dbio\open.c

!IF  "$(CFG)" == "dbio - Win32 Release"

# ADD CPP /Zp8 /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "dbio - Win32 Debug"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dbio\util.c
# ADD CPP /I "..\..\..\include\win32\mysql"
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dbio\version.c

!IF  "$(CFG)" == "dbio - Win32 Release"

# ADD CPP /Zp8 /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "dbio - Win32 Debug"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
