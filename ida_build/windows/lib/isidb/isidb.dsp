# Microsoft Developer Studio Project File - Name="isidb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=isidb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "isidb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "isidb.mak" CFG="isidb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "isidb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "isidb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "isidb - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_MYSQL" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_MYSQL" /YX /FD /GZ /c
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

# Name "isidb - Win32 Release"
# Name "isidb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\isidb\calib.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\chan.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\chnlocmap.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\coords.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\datlen.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\debug.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\inst.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\revs.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\root.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\sint.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\site.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\sitechan.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\srate.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\stamap.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\system.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\tqual.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\trecs.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\version.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isidb\wrdsiz.c

!IF  "$(CFG)" == "isidb - Win32 Release"

# ADD CPP /I "..\..\..\include\win32\mysql"

!ELSEIF  "$(CFG)" == "isidb - Win32 Debug"

# ADD CPP /MDd /I "..\..\..\include\win32\mysql"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
