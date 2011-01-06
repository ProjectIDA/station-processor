# Microsoft Developer Studio Project File - Name="isidl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=isidl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "isidl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "isidl.mak" CFG="isidl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "isidl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "isidl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "isidl - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include" /I "..\..\..\include\win32" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "INCLUDE_Q330" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ..\..\lib\iacp\Release\iacp.lib ..\..\lib\isi\Release\isi.lib ..\..\lib\util\Release\util.lib ..\..\lib\logio\Release\logio.lib ..\..\lib\nrts\Release\nrts.lib ..\..\lib\ida10\Release\ida10.lib ..\..\lib\ida\Release\ida.lib ..\..\lib\dbio\Release\dbio.lib ..\..\lib\cssio\Release\cssio.lib ..\..\lib\msgq\Release\msgq.lib ..\..\lib\win32\Release\win32.lib ..\..\lib\isidl\Release\isidl.lib ..\..\lib\zlib\zdll.lib ..\..\lib\ttyio\Release\ttyio.lib ../../lib/isidb/Release/isidb.lib ../../lib/paro/Release/paro.lib ../../lib/mysql/opt/libmysql.lib ../../lib/udpio/Release/udpio.lib ../../lib/qdp/Release/qdp.lib ../../lib/md5/Release/md5.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "isidl - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\include\win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "INCLUDE_Q330" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib ..\..\lib\iacp\Debug\iacp.lib ..\..\lib\isi\Debug\isi.lib ..\..\lib\util\Debug\util.lib ..\..\lib\logio\Debug\logio.lib ..\..\lib\nrts\Debug\nrts.lib ..\..\lib\ida10\Debug\ida10.lib ..\..\lib\ida\Debug\ida.lib ..\..\lib\dbio\Debug\dbio.lib ..\..\lib\cssio\Debug\cssio.lib ..\..\lib\msgq\Debug\msgq.lib ..\..\lib\win32\Debug\win32.lib ..\..\lib\isidl\Debug\isidl.lib ..\..\lib\zlib\zdll.lib ..\..\lib\ttyio\Debug\ttyio.lib ../../lib/logio/Debug/logio.lib ../../lib/win32/Debug/win32.lib ../../lib/mysql/opt/libmysql.lib ../../lib/isidb/Debug/isidb.lib ../../lib/paro/Debug/paro.lib  ../../lib/qdp/Debug/qdp.lib ../../lib/udpio/Debug/udpio.lib ../../lib/md5/Debug/md5.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /heap:0x2e8480
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "isidl - Win32 Release"
# Name "isidl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\bin\isidl\ars.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\baro.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\bground.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\clock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\exit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\help.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\ida10.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\ida5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\ida9.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\isi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\isidl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\local.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\meta.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\mseed.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\q330.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\qdplus.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\ReleaseNotes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\isidl\signals.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
